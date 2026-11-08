import argparse
import copy
import glob
import json
import math
import os
from pathlib import Path
import re
import sys
import tempfile
import time


CATEGORIES = ("pass", "fail", "unavailable")
PLOTS = ("Dijet_MassAK4PF", "deltaETAjj", "deltaPHIjj", "etaWJ_j1", "etaWJ_j2",
         "mjj", "pTWJ_j1", "pTWJ_j2", "phiWJ_j1", "phiWJ_j2")


def atomic_json(path, value):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = None
    try:
        with tempfile.NamedTemporaryFile(mode="w", dir=path.parent, prefix="." + path.name, delete=False) as stream:
            temporary = Path(stream.name)
            json.dump(value, stream, separators=(",", ":"), allow_nan=False)
            stream.write("\n")
            stream.flush()
            os.fsync(stream.fileno())
        os.replace(temporary, path)
    finally:
        if temporary is not None:
            temporary.unlink(missing_ok=True)


def expand_inputs(patterns):
    paths = []
    for pattern in patterns or []:
        found = sorted(glob.glob(os.path.expandvars(os.path.expanduser(pattern))))
        if not found:
            raise ValueError(f"No summaries match {pattern}; supply luminosity only for included eras")
        for item in found:
            path = Path(item).resolve()
            if path in paths:
                raise ValueError(f"Duplicate input summary: {path}")
            paths.append(path)
    return paths


def file_identity(path):
    return "/store/" + path.split("/store/", 1)[1] if "/store/" in path else path


def count(value):
    return type(value) is int and 0 <= value <= 2**53


def validate_summary(doc):
    if (doc.get("format") != "kinematics_summary" or doc.get("schema_version") != 1
            or doc.get("complete") is not True or doc.get("kind") not in ("data", "mc")):
        raise ValueError("Expected a complete reduce_kinematics summary; older JEC summaries are incompatible")
    if doc.get("jec") not in ("", "old", "new") or (doc["kind"] == "mc" and doc["jec"]):
        raise ValueError("Invalid JEC label")
    if not doc.get("datasets") or len(set(doc["datasets"])) != len(doc["datasets"]):
        raise ValueError("Invalid constituent dataset labels")
    plots = doc["contract"]["plots"]
    if set(plots) != set(PLOTS):
        raise ValueError("Incomplete histogram definitions")
    for name, spec in plots.items():
        edges = spec["edges"]
        if len(edges) < 2 or any(not math.isfinite(x) for x in edges) or any(a >= b for a, b in zip(edges, edges[1:])):
            raise ValueError(f"Invalid {name} binning")
    seen, ids = set(), set()
    if not doc.get("components"):
        raise ValueError("Summary contains no components")
    for component in doc["components"]:
        if component["id"] in ids:
            raise ValueError("Repeated component ID")
        ids.add(component["id"])
        xsec = component["xsec_pb"]
        generated = component["generated_events"]
        if not math.isfinite(xsec) or not math.isfinite(generated) or xsec < 0 or generated < 0:
            raise ValueError("Invalid MC normalization metadata")
        if doc["kind"] == "mc" and (xsec <= 0 or generated <= 0):
            raise ValueError("MC cross section and generated events must be positive")
        if doc["kind"] == "data" and (xsec != 0 or generated != 0 or component["id"] != "data"):
            raise ValueError("Invalid data component")
        files = component["files"]
        if not files:
            raise ValueError("Empty input coverage")
        for row in files:
            key = file_identity(row["path"])
            if key in seen:
                raise ValueError(f"Overlapping input ROOT file: {row['path']}")
            seen.add(key)
            if not count(row["entries"]) or row["processed"] != row["entries"]:
                raise ValueError(f"Incomplete input ROOT file: {row['path']}")
            if not math.isfinite(row["generated_events"]) or row["generated_events"] < 0:
                raise ValueError("Invalid file event counter")
        if generated != sum(row["generated_events"] for row in files):
            raise ValueError("Generated-event total does not match unique input files")
        categories = component["categories"]
        if set(categories) != set(CATEGORIES):
            raise ValueError("Missing trigger categories")
        if doc["kind"] == "data" and categories["unavailable"]["events"]:
            raise ValueError("Required data trigger flag was unavailable")
        for category in categories.values():
            if not count(category["events"]) or set(category["histograms"]) != set(PLOTS):
                raise ValueError("Invalid selection counters or missing histograms")
            for name, bins in category["histograms"].items():
                if len(bins) != len(plots[name]["edges"]) - 1 or any(not count(n) for n in bins) or sum(bins) > category["events"]:
                    raise ValueError(f"Invalid histogram counts for {name}")
        if sum(group["events"] for group in categories.values()) > sum(row["entries"] for row in files):
            raise ValueError("Selected events exceed input coverage")
    return doc


def read_summary(path):
    with Path(path).open() as stream:
        return validate_summary(json.load(stream))


def merge_summaries(documents, dataset=None):
    result = None
    seen = set()
    for doc in documents:
        validate_summary(doc)
        for component in doc["components"]:
            for row in component["files"]:
                identity = file_identity(row["path"])
                if identity in seen:
                    raise ValueError(f"Overlapping summaries include {row['path']}")
                seen.add(identity)
        if result is None:
            result = copy.deepcopy(doc)
            continue
        for key in ("kind", "jec", "contract"):
            if result[key] != doc[key]:
                raise ValueError(f"Cannot combine summaries with different {key}")
        result["datasets"] = list(dict.fromkeys(result["datasets"] + doc["datasets"]))
        by_id = {c["id"]: c for c in result["components"]}
        for incoming in doc["components"]:
            target = by_id.get(incoming["id"])
            if target is None:
                target = copy.deepcopy(incoming)
                result["components"].append(target)
                by_id[target["id"]] = target
                continue
            if any(target[key] != incoming[key] for key in ("source", "xsec_pb")):
                raise ValueError("Incompatible MC component normalization")
            target["files"].extend(copy.deepcopy(incoming["files"]))
            target["generated_events"] = sum(row["generated_events"] for row in target["files"])
            for category in CATEGORIES:
                out, part = target["categories"][category], incoming["categories"][category]
                out["events"] += part["events"]
                for name in PLOTS:
                    out["histograms"][name] = [a + b for a, b in zip(out["histograms"][name], part["histograms"][name])]
    if result is None:
        raise ValueError("No summaries supplied")
    result["dataset"] = dataset or (result["datasets"][0] if len(result["datasets"]) == 1 else "+".join(result["datasets"]))
    return validate_summary(result)


def selected_counts(component, name, trigger):
    groups = CATEGORIES if trigger == "all" else (trigger,)
    return [sum(component["categories"][group]["histograms"][name][i] for group in groups)
            for i in range(len(component["categories"]["pass"]["histograms"][name]))]


def selected_events(doc, trigger):
    groups = CATEGORIES if trigger == "all" else (trigger,)
    return sum(c["categories"][group]["events"] for c in doc["components"] for group in groups)


def load_renderer():
    try:
        import ROOT
    except ImportError as error:
        raise RuntimeError("PyROOT is required for plotting; run cmsenv first. Merging does not require ROOT.") from error
    ROOT.PyConfig.IgnoreCommandLineOptions = True
    ROOT.gROOT.SetBatch(True)
    for library in ("libHist", "libGraf", "libGpad", "libTree", "libTreePlayer", "libROOTDataFrame", "libImt"):
        if ROOT.gSystem.Load(library) < 0:
            raise RuntimeError(f"Cannot load {library}")
    source = Path(__file__).resolve().with_name("reduce_kinematics.cpp")
    bridge = r'''
namespace KinematicsJSON {
std::string contract() {return kinematics_contract().dump();}
void style() {configure_root_style();}
TH1D* histogram(const std::string& plot,const std::string& name,const std::vector<double>& counts) {
    auto hist=hist_from_counts(name,kPlotSpecs.at(plot),counts);
    return static_cast<TH1D*>(hist->Clone(name.c_str()));
}
void render(const std::string& name,TH1D* data,TH1D* mc,double k,double lumi,const std::string& stem,TFile& output) {
    output.cd();
    if(data) {set_hist_style(*data,kDataColor,true);data->Write();}
    if(mc) {set_hist_style(*mc,kMcLineColor,false);mc->Write();}
    std::shared_ptr<TH1D> ratio;
    if(data && mc) {
        ratio=make_ratio_hist(*data,*mc,"h_ratio_"+name);
        set_hist_style(*ratio,kDataColor,true);ratio->Write();
    }
    const auto factor=data && mc?std::optional<double>(k):std::nullopt;
    draw_plot(data,mc,ratio.get(),factor,kPlotSpecs.at(name),lumi,fs::path(stem),output);
}
}
'''
    declaration = "#define KINEMATICS_JSON_LIBRARY\n#include " + json.dumps(str(source)) + "\n#undef KINEMATICS_JSON_LIBRARY\n" + bridge
    if not ROOT.gInterpreter.Declare(declaration):
        raise RuntimeError("Could not load the reference C++ plotting functions")
    ROOT.KinematicsJSON.style()
    return ROOT


def parse_args():
    parser = argparse.ArgumentParser(description="Merge and plot exact kinematics reducer summaries without opening nTuples.")
    parser.add_argument("--data-input", nargs="+")
    parser.add_argument("--mc-input", nargs="+")
    parser.add_argument("--merge-json", nargs="+", help="Merge disjoint summaries; does not require ROOT")
    parser.add_argument("--dataset", help="Label for a merged summary")
    parser.add_argument("--output", type=Path, help="Merged JSON filename")
    parser.add_argument("--force", action="store_true", help="Replace an existing merged JSON")
    parser.add_argument("--jec", choices=("old", "new"), help="Require this data JEC version")
    parser.add_argument("--plot", default="all")
    parser.add_argument("--lumi-pb", type=float, help="Combined luminosity of the supplied data eras in pb^-1")
    parser.add_argument("--trigger", choices=("pass", "fail", "all"), default="pass", help="Data trigger category; MC always uses all categories")
    parser.add_argument("--output-dir", default="kinematics/plots")
    parser.add_argument("--output-prefix", default="")
    args = parser.parse_args()
    if args.merge_json:
        if args.data_input or args.mc_input or not args.output or not args.dataset:
            parser.error("--merge-json requires --dataset and --output and cannot be combined with plotting inputs")
    else:
        if args.output or args.dataset or args.force:
            parser.error("--output, --dataset and --force are for --merge-json")
        if not args.data_input and not args.mc_input:
            parser.error("Provide --data-input and/or --mc-input")
        if args.lumi_pb is None or not math.isfinite(args.lumi_pb) or args.lumi_pb <= 0:
            parser.error("--lumi-pb must be finite and positive")
    if any(c in args.output_prefix for c in ("/", "\\", "\x00")):
        parser.error("--output-prefix must be a filename prefix")
    return args


def main():
    start = time.monotonic()
    args = parse_args()
    if args.merge_json:
        paths = expand_inputs(args.merge_json)
        target = args.output.expanduser().resolve()
        if target in paths:
            raise ValueError("Merged output cannot overwrite an input summary")
        if target.exists() and not args.force:
            raise ValueError("Merged output exists; use --force to replace it")
        result = merge_summaries((read_summary(path) for path in paths), args.dataset)
        if args.jec and result["jec"] != args.jec:
            raise ValueError("JEC label mismatch")
        atomic_json(target, result)
        print(f"[INFO] Saved {target}; constituent datasets: {', '.join(result['datasets'])}")
        return
    names = list(PLOTS) if args.plot == "all" else list(dict.fromkeys(n.strip() for n in args.plot.split(",") if n.strip()))
    if not names or any(name not in PLOTS for name in names):
        raise ValueError("Available plots: " + ", ".join(PLOTS))
    paths = {kind: expand_inputs(getattr(args, kind + "_input")) for kind in ("data", "mc")}
    docs = {kind: merge_summaries(read_summary(p) for p in group) if group else None for kind, group in paths.items()}
    for kind, doc in docs.items():
        if doc is not None and doc["kind"] != kind:
            raise ValueError(f"--{kind}-input contains the wrong sample kind")
    data, mc = docs["data"], docs["mc"]
    if args.jec and (data is None or data["jec"] != args.jec):
        raise ValueError("JEC label mismatch")
    ROOT = load_renderer()
    contract = json.loads(ROOT.KinematicsJSON.contract())
    if any(doc is not None and doc["contract"] != contract for doc in docs.values()):
        raise ValueError("Summary analysis differs from current plot_kinematics.cpp; use matching sources or regenerate")
    target_yield = selected_events(data, args.trigger) if data is not None else None
    directory = Path(args.output_dir).expanduser().resolve()
    directory.mkdir(parents=True, exist_ok=True)
    label = names[0] if len(names) == 1 else "all_plots" if args.plot == "all" else re.sub(r"[^A-Za-z0-9_]", "_", args.plot).strip("_")
    root_path = directory / (args.output_prefix + label + ".root")
    output = ROOT.TFile(str(root_path), "RECREATE")
    if output.IsZombie():
        raise RuntimeError(f"Cannot create {root_path}")
    metadata = {"inputs": {k: [str(p) for p in v] for k, v in paths.items()}, "contract": contract,
                "datasets": data["datasets"] if data else [], "lumi_pb": args.lumi_pb,
                "data_trigger": args.trigger, "mc_trigger": "all", "normalizations": {}}

    def histogram(plot, name, counts):
        hist = ROOT.KinematicsJSON.histogram(plot, name, ROOT.std.vector("double")(counts))
        hist.SetDirectory(0)
        ROOT.SetOwnership(hist, True)
        return hist

    try:
        for name in names:
            data_hist = histogram(name, "h_data_" + name, selected_counts(data["components"][0], name, args.trigger)) if data else None
            mc_hist, mc_yield = None, 0.0
            for index, component in enumerate(mc["components"] if mc else []):
                hist = histogram(name, f"h_mc_{index}_{name}", selected_counts(component, name, "all"))
                hist.Scale(args.lumi_pb * component["xsec_pb"] / component["generated_events"])
                mc_yield += hist.Integral(0, hist.GetNbinsX() + 1)
                if mc_hist is None:
                    mc_hist = hist.Clone("h_mc_total_" + name)
                    mc_hist.SetDirectory(0)
                else:
                    mc_hist.Add(hist)
            k = None
            if data_hist is not None and mc_hist is not None:
                if mc_yield <= 0:
                    raise ValueError(f"QCD yield is zero; cannot normalize {name}")
                k = target_yield / mc_yield
                mc_hist.Scale(k)
                print(f"[INFO] {name}: selected data={target_yield}, raw QCD={mc_yield:.12g}, k={k:.12g}", flush=True)
            metadata["normalizations"][name] = {"data_yield": target_yield, "unscaled_qcd_yield": mc_yield, "k": k}
            ROOT.KinematicsJSON.render(name, data_hist if data_hist is not None else ROOT.nullptr,
                                      mc_hist if mc_hist is not None else ROOT.nullptr, k if k is not None else 1.0,
                                      args.lumi_pb, str(directory / (args.output_prefix + name)), output)
        output.cd()
        ROOT.TObjString(json.dumps(metadata, sort_keys=True, allow_nan=False)).Write("json_provenance")
    finally:
        output.Close()
    print(f"[INFO] Saved {len(names)} PDFs and {root_path} in {time.monotonic()-start:.2f}s")


if __name__ == "__main__":
    try:
        main()
    except (ValueError, RuntimeError, OSError, KeyError, TypeError) as error:
        print(f"[ERROR] {error}", file=sys.stderr)
        sys.exit(1)
