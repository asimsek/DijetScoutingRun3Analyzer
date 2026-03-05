#!/usr/bin/env python3

from __future__ import annotations

import argparse
import glob
import math
import os
import shlex
import sys
from array import array
from pathlib import Path
from typing import Iterable, Sequence


TREE_PATH_DEFAULT = "rootTupleTree/tree"
EVENT_COUNTER_PATH = "DijetFilter/EventCount/EventCounter"
DEFAULT_SELECTION = "dijet"
DEFAULT_DATA_EXTRA_CUT = None
DEFAULT_MC_EXTRA_CUT = None
DEFAULT_DATA_LABEL = "Data"
DEFAULT_MC_LABEL = "QCD"
DEFAULT_CMS_LABEL = "CMS"
DEFAULT_EXTRA_LABEL = "Preliminary"
DEFAULT_SQRT_S = "13.6 TeV"
DEFAULT_OUTPUT_FORMATS = ("pdf",)
DEFAULT_MIN_MJJ = 944.0
ANSI_RESET = "\033[0m"
ANSI_BRIGHT_YELLOW = "\033[93m"
ANSI_BRIGHT_RED = "\033[91m"

MASS_BINS = [
    1, 3, 6, 10, 16, 23, 31, 40, 50, 61, 74, 88, 103, 119, 137, 156, 176, 197, 220,
    244, 270, 296, 325, 354, 386, 419, 453, 489, 526, 565, 606, 649, 693, 740, 788,
    838, 890, 944, 1000, 1058, 1118, 1181, 1246, 1313, 1383, 1455, 1530, 1607, 1687,
    1770, 1856, 1945, 2037, 2132, 2231, 2332, 2438, 2546, 2659, 2775, 2895, 3019,
    3147, 3279, 3416, 3558, 3704, 3854, 4010, 4171, 4337, 4509, 4686, 4869, 5058,
    5253, 5455, 5663, 5877, 6099, 6328, 6564, 6808, 7060, 7320, 7589, 7866, 8152,
    8447, 8752, 9067, 9391, 9726, 10072, 10430, 10798, 11179, 11571, 11977, 12395,
    12827, 13272, 13732, 14000,
]

PLOT_SPECS = {
    "mjj": {
        "expr": "mjj",
        "title": "Dijet mass [GeV]",
        "raw_bins": 13999,
        "raw_xmin": 1.0,
        "raw_xmax": 14000.0,
        "bin_edges": MASS_BINS,
        "display_range": (944.0, 2132.0),
        "units": "GeV",
        "logy": True,
        "ratio_range": (0.4, 1.6),
    },
    "Dijet_MassAK4PF": {
        "expr": "Dijet_MassAK4PF",
        "title": "AK4 dijet mass [GeV]",
        "raw_bins": 13999,
        "raw_xmin": 1.0,
        "raw_xmax": 14000.0,
        "bin_edges": MASS_BINS,
        "display_range": (944.0, 2132.0),
        "units": "GeV",
        "logy": True,
        "ratio_range": (0.4, 1.6),
    },
    "pTWJ_j1": {
        "expr": "pTWJ_j1",
        "title": "Leading wide-jet p_{T} [GeV]",
        "bins": 120,
        "xmin": 0.0,
        "xmax": 5000.0,
        "units": "GeV",
        "logy": True,
        "rebin": 2,
        "ratio_range": (0.4, 1.6),
    },
    "pTWJ_j2": {
        "expr": "pTWJ_j2",
        "title": "Subleading wide-jet p_{T} [GeV]",
        "bins": 120,
        "xmin": 0.0,
        "xmax": 5000.0,
        "units": "GeV",
        "logy": True,
        "rebin": 2,
        "ratio_range": (0.4, 1.6),
    },
    "etaWJ_j1": {
        "expr": "etaWJ_j1",
        "title": "Leading wide-jet #eta",
        "bins": 60,
        "xmin": -3.0,
        "xmax": 3.0,
        "units": "",
        "logy": False,
        "ratio_range": (0.6, 1.4),
    },
    "etaWJ_j2": {
        "expr": "etaWJ_j2",
        "title": "Subleading wide-jet #eta",
        "bins": 60,
        "xmin": -3.0,
        "xmax": 3.0,
        "units": "",
        "logy": False,
        "ratio_range": (0.6, 1.4),
    },
    "deltaETAjj": {
        "expr": "deltaETAjj",
        "title": "#Delta#eta_{jj}",
        "bins": 60,
        "xmin": 0.0,
        "xmax": 1.1,
        "units": "",
        "logy": True,
        "ratio_range": (0.6, 1.4),
    },
    "phiWJ_j1": {
        "expr": "phiWJ_j1",
        "title": "Leading wide-jet #phi",
        "bins": 64,
        "xmin": -3.2,
        "xmax": 3.2,
        "units": "",
        "logy": False,
        "ratio_range": (0.6, 1.4),
    },
    "phiWJ_j2": {
        "expr": "phiWJ_j2",
        "title": "Subleading wide-jet #phi",
        "bins": 64,
        "xmin": -3.2,
        "xmax": 3.2,
        "units": "",
        "logy": False,
        "ratio_range": (0.6, 1.4),
    },
}

SELECTION_PRESETS = {
    "none": {
        "data": "1",
        "mc": "1",
    },
    "baseline": {
        "data": f"PassJSON == 1 && nJet > 1 && mjj > {DEFAULT_MIN_MJJ:.0f}",
        "mc": f"nJet > 1 && mjj > {DEFAULT_MIN_MJJ:.0f}",
    },
    "dijet": {
        "data": (
            "PassJSON == 1 && nJet > 1 && IdTight_j1 > 0.5 && IdTight_j2 > 0.5 "
            "&& pTWJ_j1 > 60 && pTWJ_j2 > 30 "
            "&& TMath::Abs(etaWJ_j1) < 2.5 && TMath::Abs(etaWJ_j2) < 2.5 "
            "&& TMath::Abs(deltaETAjj) < 1.1 "
            f"&& mjj > {DEFAULT_MIN_MJJ:.0f}"
        ),
        "mc": (
            "nJet > 1 && IdTight_j1 > 0.5 && IdTight_j2 > 0.5 "
            "&& pTWJ_j1 > 60 && pTWJ_j2 > 30 "
            "&& TMath::Abs(etaWJ_j1) < 2.5 && TMath::Abs(etaWJ_j2) < 2.5 "
            "&& TMath::Abs(deltaETAjj) < 1.1 "
            f"&& mjj > {DEFAULT_MIN_MJJ:.0f}"
        ),
    },
}

STYLE = {
    "canvas_width": 800,
    "canvas_height": 900,
    "data_color": 1,
    "mc_line_color": 632,
    "marker_style": 20,
    "marker_size": 1.0,
    "line_width": 3,
    "legend": (0.60, 0.78, 0.94, 0.90),
}


def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Plot one variable from reduced skim nTuples.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument("--plot", required=True, help="Plot spec name from PLOT_SPECS.")
    parser.add_argument("--data-list", help="Text file containing data ROOT files, globs, or directories.")
    parser.add_argument("--mc-list", help="Text file containing MC entries: path_or_glob xsec_pb.")
    parser.add_argument("--output-dir", default="plots", help="Output directory.")
    parser.add_argument("--output-prefix", default="", help="Optional filename prefix.")
    parser.add_argument("--lumi-pb", type=float, default=1.0, help="Integrated luminosity in pb^-1.")
    parser.add_argument("--threads", type=int, default=8, help="ROOT implicit MT thread count. Use 0 to disable.")
    return parser


def format_lumi(value: float) -> str:
    text = f"{value:.6f}".rstrip("0")
    if text.endswith("."):
        text += "0"
    return text


def colorize(label: str, color: str) -> str:
    return f"{color}{label}{ANSI_RESET}" if sys.stdout else label


def clean_lines(path: Path) -> Iterable[str]:
    with path.open() as handle:
        for raw in handle:
            line = raw.strip()
            if not line or line.startswith("#"):
                continue
            yield line


def normalize_path_token(token: str, base_dir: Path) -> str:
    token = os.path.expandvars(os.path.expanduser(token))
    if "://" in token:
        return token
    candidate = Path(token)
    if not candidate.is_absolute():
        candidate = (base_dir / candidate).resolve()
    return str(candidate)


def expand_source_token(token: str, base_dir: Path) -> list[str]:
    normalized = normalize_path_token(token, base_dir)
    if "://" in normalized:
        return [normalized]
    if any(char in normalized for char in "*?[]"):
        return sorted(glob.glob(normalized))
    path = Path(normalized)
    if path.is_dir():
        return sorted(str(p) for p in path.rglob("*_reduced_skim.root") if p.is_file())
    return [str(path)]


def derive_name(token: str) -> str:
    base = os.path.basename(token.rstrip("/"))
    for suffix in (".root", "_reduced_skim"):
        if base.endswith(suffix):
            base = base[: -len(suffix)]
    safe = []
    for char in base:
        safe.append(char if char.isalnum() or char == "_" else "_")
    return "".join(safe).strip("_") or "sample"


def deduplicate(seq: Iterable[str]) -> list[str]:
    seen = set()
    ordered = []
    for item in seq:
        if item in seen:
            continue
        ordered.append(item)
        seen.add(item)
    return ordered


def read_data_inputs(list_path: str) -> list[str]:
    path = Path(list_path).resolve()
    files = []
    for line in clean_lines(path):
        files.extend(expand_source_token(line, path.parent))
    files = deduplicate(files)
    if not files:
        raise RuntimeError(f"No data ROOT files found from list: {path}")
    return files


def read_mc_inputs(list_path: str) -> list[dict]:
    path = Path(list_path).resolve()
    samples = []
    warned_extra_columns = False
    for line in clean_lines(path):
        fields = shlex.split(line)
        if len(fields) < 2:
            raise RuntimeError(
                f"Invalid MC list entry in {path}: {line}\n"
                "Expected format: path_or_glob xsec_pb"
            )
        if len(fields) > 2 and not warned_extra_columns:
            print("[WARN] MC list has extra columns. Only the first two columns are used: path_or_glob xsec_pb")
            warned_extra_columns = True
        source_token = fields[0]
        files = deduplicate(expand_source_token(source_token, path.parent))
        if not files:
            raise RuntimeError(f"No MC ROOT files found for entry: {line}")
        samples.append(
            {
                "source": source_token,
                "name": derive_name(source_token),
                "xsec_pb": float(fields[1]),
                "files": files,
            }
        )
    if not samples:
        raise RuntimeError(f"No MC ROOT files found from list: {path}")
    return samples


def ensure_plot_spec(name: str) -> dict:
    if name not in PLOT_SPECS:
        raise RuntimeError(f"Unknown plot spec: {name}")
    return dict(PLOT_SPECS[name])


def combine_cuts(*cuts: str | None) -> str:
    active = [f"({cut})" for cut in cuts if cut and cut.strip()]
    return " && ".join(active) if active else "1"


def configure_root_style(ROOT) -> None:
    ROOT.gROOT.SetBatch(True)
    ROOT.TH1.SetDefaultSumw2(True)
    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetOptTitle(0)
    ROOT.gStyle.SetCanvasBorderMode(0)
    ROOT.gStyle.SetFrameBorderMode(0)
    ROOT.gStyle.SetCanvasColor(0)
    ROOT.gStyle.SetPadTickX(1)
    ROOT.gStyle.SetPadTickY(1)
    ROOT.gStyle.SetPadTopMargin(0.06)
    ROOT.gStyle.SetPadBottomMargin(0.13)
    ROOT.gStyle.SetPadLeftMargin(0.16)
    ROOT.gStyle.SetPadRightMargin(0.05)
    ROOT.gStyle.SetLegendBorderSize(0)
    ROOT.gStyle.SetLegendFont(42)
    ROOT.gStyle.SetLabelFont(42, "XYZ")
    ROOT.gStyle.SetTitleFont(42, "XYZ")
    ROOT.gStyle.SetTitleSize(0.055, "XYZ")
    ROOT.gStyle.SetLabelSize(0.045, "XYZ")
    ROOT.gStyle.SetTitleYOffset(1.35)
    ROOT.gStyle.SetEndErrorSize(0)


def build_chain(ROOT, tree_path: str, files: Sequence[str]):
    chain = ROOT.TChain(tree_path)
    added = 0
    for file_name in files:
        added += chain.Add(file_name)
    if added == 0:
        raise RuntimeError(f"Could not add any files to TChain for tree {tree_path}.")
    return chain


def make_hist_model(ROOT, hist_name: str, spec: dict):
    if all(key in spec for key in ("raw_bins", "raw_xmin", "raw_xmax")):
        return ROOT.RDF.TH1DModel(
            hist_name,
            "",
            int(spec["raw_bins"]),
            float(spec["raw_xmin"]),
            float(spec["raw_xmax"]),
        )
    if "bin_edges" in spec:
        edges = array("d", [float(value) for value in spec["bin_edges"]])
        return ROOT.RDF.TH1DModel(hist_name, "", len(edges) - 1, edges)
    return ROOT.RDF.TH1DModel(
        hist_name,
        "",
        int(spec["bins"]),
        float(spec["xmin"]),
        float(spec["xmax"]),
    )


def clone_hist(result_ptr, name: str):
    hist = result_ptr.GetValue().Clone(name)
    hist.SetDirectory(0)
    hist.Sumw2()
    return hist


def merge_overflow_bin(hist) -> None:
    nbins = hist.GetNbinsX()
    hist.SetBinContent(nbins, hist.GetBinContent(nbins) + hist.GetBinContent(nbins + 1))
    hist.SetBinError(nbins, math.hypot(hist.GetBinError(nbins), hist.GetBinError(nbins + 1)))
    hist.SetBinContent(nbins + 1, 0.0)
    hist.SetBinError(nbins + 1, 0.0)


def normalize_by_bin_width(hist) -> None:
    for bin_idx in range(1, hist.GetNbinsX() + 1):
        width = hist.GetXaxis().GetBinWidth(bin_idx)
        if width <= 0.0:
            continue
        hist.SetBinContent(bin_idx, hist.GetBinContent(bin_idx) / width)
        hist.SetBinError(bin_idx, hist.GetBinError(bin_idx) / width)


def apply_final_binning(hist, target_name: str, spec: dict):
    result = hist.Clone(target_name)
    result.SetDirectory(0)
    if all(key in spec for key in ("raw_bins", "raw_xmin", "raw_xmax")) and "bin_edges" in spec:
        edges = array("d", [float(value) for value in spec["bin_edges"]])
        result = result.Rebin(len(edges) - 1, target_name, edges)
        result.SetDirectory(0)
        return result
    if int(spec.get("rebin", 1)) > 1:
        result = result.Rebin(int(spec["rebin"]), target_name)
        result.SetDirectory(0)
    merge_overflow_bin(result)
    return result


def read_event_counter(ROOT, files: Sequence[str]) -> float:
    total = 0.0
    for file_name in files:
        root_file = ROOT.TFile.Open(file_name)
        if not root_file or root_file.IsZombie():
            raise RuntimeError(f"Could not open ROOT file for normalization: {file_name}")
        hist = root_file.Get(EVENT_COUNTER_PATH)
        if hist:
            total += float(hist.GetBinContent(2))
            root_file.Close()
            continue
        param = root_file.Get("metaData/expectedEvents")
        if param:
            total += float(param.GetVal())
            root_file.Close()
            continue
        root_file.Close()
        raise RuntimeError(
            f"Could not find {EVENT_COUNTER_PATH} or metaData/expectedEvents in {file_name}"
        )
    return total


def infer_y_title(spec: dict, hist) -> str:
    if spec.get("normalize_by_bin_width", False):
        units = spec.get("units", "").strip()
        return f"Events / {units}" if units else "Events / unit"
    if spec["expr"] in {"mjj", "Dijet_MassAK4PF"}:
        return "Events / GeV"
    width = hist.GetXaxis().GetBinWidth(1)
    units = spec.get("units", "").strip()
    return f"Events / {width:.2f} {units}".strip()


def set_hist_style(hist, *, color: int, marker: bool) -> None:
    hist.SetLineColor(color)
    hist.SetLineWidth(STYLE["line_width"])
    if marker:
        hist.SetMarkerColor(color)
        hist.SetMarkerStyle(STYLE["marker_style"])
        hist.SetMarkerSize(STYLE["marker_size"])
        hist.SetFillStyle(0)
    else:
        hist.SetMarkerSize(0.0)
        hist.SetFillStyle(0)


def make_ratio_hist(data_hist, mc_hist, name: str):
    ratio = data_hist.Clone(name)
    ratio.SetDirectory(0)
    for bin_idx in range(1, ratio.GetNbinsX() + 1):
        numerator = data_hist.GetBinContent(bin_idx)
        numerator_err = data_hist.GetBinError(bin_idx)
        denominator = mc_hist.GetBinContent(bin_idx)
        denominator_err = mc_hist.GetBinError(bin_idx)
        if denominator <= 0.0:
            ratio.SetBinContent(bin_idx, 0.0)
            ratio.SetBinError(bin_idx, 0.0)
            continue
        value = numerator / denominator
        error = 0.0
        if numerator > 0.0:
            error = value * math.sqrt(
                (numerator_err / numerator) ** 2 + (denominator_err / denominator) ** 2
            )
        ratio.SetBinContent(bin_idx, value)
        ratio.SetBinError(bin_idx, error)
    return ratio


def draw_cms_labels(ROOT, pad, lumi_pb: float) -> None:
    pad.cd()
    latex = ROOT.TLatex()
    latex.SetNDC(True)
    latex.SetTextAlign(13)
    latex.SetTextFont(62)
    latex.SetTextSize(0.055)
    latex.DrawLatex(0.21, 0.88, DEFAULT_CMS_LABEL)
    latex.SetTextFont(52)
    latex.SetTextSize(0.040)
    latex.DrawLatex(0.21, 0.82, DEFAULT_EXTRA_LABEL)
    latex.SetTextFont(42)
    latex.SetTextSize(0.040)
    latex.SetTextAlign(31)
    latex.DrawLatex(0.95, 0.93, f"{lumi_pb:.0f} pb^{{-1}} ({DEFAULT_SQRT_S})")


def compute_maximum(*hists) -> float:
    maximum = 0.0
    for hist in hists:
        if hist:
            maximum = max(maximum, hist.GetMaximum())
    return maximum


def compute_positive_minimum(*hists) -> float:
    minimum = None
    for hist in hists:
        if not hist:
            continue
        for bin_idx in range(1, hist.GetNbinsX() + 1):
            value = hist.GetBinContent(bin_idx)
            if value <= 0.0:
                continue
            minimum = value if minimum is None else min(minimum, value)
    return minimum if minimum is not None else 1e-3


def compute_auto_y_range(*hists, logy: bool) -> tuple[float, float]:
    ymax = compute_maximum(*hists)
    if ymax <= 0.0:
        return (1e-6, 1.0) if logy else (0.0, 1.0)

    if logy:
        ymin = max(compute_positive_minimum(*hists) * 0.5, 1e-6)
        if ymax <= ymin:
            ymax = ymin * 10.0
        log_min = math.log10(ymin)
        log_max = math.log10(ymax)
        span = max(log_max - log_min, 1.0)
        top_headroom = 0.22
        extra_span = span * top_headroom / (1.0 - top_headroom)
        return ymin, 10.0 ** (log_max + extra_span)

    top_headroom = 0.22
    return 0.0, ymax / (1.0 - top_headroom)


def draw_plot(ROOT, *, data_hist, mc_hist, ratio_hist, kfactor: float | None, spec: dict, lumi_pb: float, output_stem: Path, root_out):
    use_ratio = data_hist is not None and mc_hist is not None
    canvas = ROOT.TCanvas(
        f"c_{output_stem.stem}",
        f"c_{output_stem.stem}",
        STYLE["canvas_width"],
        STYLE["canvas_height"],
    )

    if use_ratio:
        top_pad = ROOT.TPad("top_pad", "top_pad", 0.0, 0.30, 1.0, 1.0)
        bottom_pad = ROOT.TPad("bottom_pad", "bottom_pad", 0.0, 0.0, 1.0, 0.30)
        top_pad.SetLeftMargin(0.16)
        top_pad.SetRightMargin(0.05)
        top_pad.SetTopMargin(0.08)
        top_pad.SetBottomMargin(0.02)
        bottom_pad.SetLeftMargin(0.16)
        bottom_pad.SetRightMargin(0.05)
        bottom_pad.SetTopMargin(0.02)
        bottom_pad.SetBottomMargin(0.35)
        bottom_pad.SetGridy(True)
        top_pad.Draw()
        bottom_pad.Draw()
    else:
        top_pad = ROOT.TPad("top_pad", "top_pad", 0.0, 0.0, 1.0, 1.0)
        top_pad.SetLeftMargin(0.16)
        top_pad.SetRightMargin(0.05)
        top_pad.SetTopMargin(0.08)
        top_pad.SetBottomMargin(0.13)
        top_pad.Draw()
        bottom_pad = None

    top_pad.cd()
    top_pad.SetLogy(bool(spec.get("logy", False)))

    draw_first = mc_hist if mc_hist is not None else data_hist
    if draw_first is None:
        raise RuntimeError("Nothing to draw: both data and MC histograms are missing.")

    draw_first.GetXaxis().SetTitle(spec["title"])
    draw_first.GetYaxis().SetTitle(infer_y_title(spec, draw_first))
    draw_first.GetYaxis().SetTitleOffset(1.45)
    draw_first.GetYaxis().SetLabelSize(0.045)
    draw_first.GetYaxis().SetTitleSize(0.050)
    draw_first.GetXaxis().SetLabelSize(0.0 if use_ratio else 0.045)
    draw_first.GetXaxis().SetTitleSize(0.0 if use_ratio else 0.050)

    if "display_range" in spec:
        xmin, xmax = spec["display_range"]
        draw_first.GetXaxis().SetRangeUser(float(xmin), float(xmax))

    ymin, ymax = compute_auto_y_range(data_hist, mc_hist, logy=bool(spec.get("logy", False)))
    draw_first.SetMinimum(float(ymin))
    draw_first.SetMaximum(float(ymax))

    if mc_hist is not None:
        mc_hist.Draw("hist")
        if data_hist is not None:
            data_hist.Draw("E1 same")
    elif data_hist is not None:
        data_hist.Draw("E1")

    legend = ROOT.TLegend(*STYLE["legend"])
    legend.SetTextSize(0.035)
    if mc_hist is not None:
        legend_label = f"{DEFAULT_MC_LABEL} * {kfactor:.2f}" if kfactor is not None else DEFAULT_MC_LABEL
        legend.AddEntry(mc_hist, legend_label, "l")
    if data_hist is not None:
        legend.AddEntry(data_hist, DEFAULT_DATA_LABEL, "lep")
    legend.Draw()

    draw_cms_labels(ROOT, top_pad, lumi_pb)
    top_pad.RedrawAxis()

    if use_ratio and bottom_pad is not None and ratio_hist is not None:
        bottom_pad.cd()
        ratio_hist.GetYaxis().SetTitle("Data / MC")
        ratio_hist.GetYaxis().SetTitleSize(0.11)
        ratio_hist.GetYaxis().SetTitleOffset(0.55)
        ratio_hist.GetYaxis().SetLabelSize(0.10)
        ratio_hist.GetYaxis().SetNdivisions(505)
        ratio_hist.GetXaxis().SetTitle(spec["title"])
        ratio_hist.GetXaxis().SetTitleSize(0.12)
        ratio_hist.GetXaxis().SetTitleOffset(1.10)
        ratio_hist.GetXaxis().SetLabelSize(0.10)
        ratio_hist.GetXaxis().SetLabelOffset(0.02)
        ratio_hist.GetYaxis().SetRangeUser(*spec.get("ratio_range", (0.5, 1.5)))
        if "display_range" in spec:
            xmin, xmax = spec["display_range"]
            ratio_hist.GetXaxis().SetRangeUser(float(xmin), float(xmax))
        ratio_hist.Draw("E1")
        axis = ratio_hist.GetXaxis()
        first_bin = axis.GetFirst()
        last_bin = axis.GetLast()
        x_min = axis.GetBinLowEdge(first_bin)
        x_max = axis.GetBinUpEdge(last_bin)
        unity = ROOT.TF1(f"unity_{output_stem.stem}", "1", x_min, x_max)
        unity.SetLineStyle(2)
        unity.SetLineColor(1)
        unity.Draw("same")
        bottom_pad.RedrawAxis()

    root_out.cd()
    canvas.Write()
    for extension in DEFAULT_OUTPUT_FORMATS:
        canvas.SaveAs(str(output_stem.with_suffix(f".{extension}")))
    canvas.Close()


def main() -> int:
    args = build_arg_parser().parse_args()

    if not args.data_list and not args.mc_list:
        raise RuntimeError("At least one of --data-list or --mc-list must be provided.")

    spec = ensure_plot_spec(args.plot)
    selection = SELECTION_PRESETS[DEFAULT_SELECTION]
    data_cut = combine_cuts(selection["data"], DEFAULT_DATA_EXTRA_CUT)
    mc_cut = combine_cuts(selection["mc"], DEFAULT_MC_EXTRA_CUT)

    output_dir = Path(args.output_dir).resolve()
    output_dir.mkdir(parents=True, exist_ok=True)
    stem_name = f"{args.output_prefix}{args.plot}" if args.output_prefix else args.plot
    output_root = output_dir / f"{stem_name}.root"
    output_stem = output_dir / stem_name

    data_files = read_data_inputs(args.data_list) if args.data_list else []
    mc_samples = read_mc_inputs(args.mc_list) if args.mc_list else []

    try:
        import ROOT
    except ImportError as exc:
        raise SystemExit("PyROOT is required to run this script. Load a CMSSW/ROOT environment first.") from exc

    configure_root_style(ROOT)
    if args.threads > 0:
        ROOT.EnableImplicitMT(args.threads)

    print(f"[INFO] Variable         : {spec['expr']}")
    print(f"[INFO] Tree             : {TREE_PATH_DEFAULT}")
    print(f"[INFO] Output ROOT file : {output_root}")
    if data_files:
        print(f"[INFO] Data files       : {len(data_files):,}")
        print(f"[INFO] Data cut         : {data_cut}")
    if mc_samples:
        mc_file_count = sum(len(sample["files"]) for sample in mc_samples)
        print(f"[INFO] MC files         : {mc_file_count:,}")
        print(f"[INFO] MC cut           : {mc_cut}")
        print(f"[INFO] Lumi [pb^-1]     : {format_lumi(args.lumi_pb)}")

    root_out = ROOT.TFile(str(output_root), "RECREATE")
    if not root_out or root_out.IsZombie():
        raise RuntimeError(f"Could not create output ROOT file: {output_root}")

    chains = []
    data_hist = None
    mc_total_hist = None
    ratio_hist = None
    data_yield_raw = 0.0
    mc_yield_raw = 0.0
    mc_total_hist_raw = None
    kfactor = None

    if data_files:
        for file_name in data_files:
            print(f"{colorize('[INFO] Data file        :', ANSI_BRIGHT_YELLOW)} {os.path.basename(file_name.rstrip('/'))}")
        data_chain = build_chain(ROOT, TREE_PATH_DEFAULT, data_files)
        chains.append(data_chain)
        data_df = ROOT.RDataFrame(data_chain)
        data_result = data_df.Filter(data_cut).Histo1D(
            make_hist_model(ROOT, f"h_data_raw_{args.plot}", spec),
            spec["expr"],
        )
        data_hist_raw = clone_hist(data_result, f"h_data_raw_{args.plot}")
        data_yield_raw = float(data_hist_raw.GetEntries())
        data_hist = apply_final_binning(data_hist_raw, f"h_data_{args.plot}", spec)
        set_hist_style(data_hist, color=STYLE["data_color"], marker=True)
        root_out.cd()
        data_hist.Write()

    for index, sample in enumerate(mc_samples):
        sample_chain = build_chain(ROOT, TREE_PATH_DEFAULT, sample["files"])
        chains.append(sample_chain)
        sample_df = ROOT.RDataFrame(sample_chain)
        n_processed = read_event_counter(ROOT, sample["files"])
        if n_processed <= 0.0:
            raise RuntimeError(f"Non-positive processed event count for MC entry: {sample['source']}")
        hist_ptr = sample_df.Filter(mc_cut).Histo1D(
            make_hist_model(ROOT, f"h_mc_raw_{index}_{args.plot}", spec),
            spec["expr"],
        )
        sample_hist_raw = clone_hist(hist_ptr, f"h_mc_raw_{sample['name']}_{args.plot}")
        n_passed = float(sample_hist_raw.GetEntries())
        eff = n_passed / n_processed if n_processed > 0.0 else 0.0
        is_last_sample = index == (len(mc_samples) - 1)
        if is_last_sample:
            if eff <= 0.0:
                raise RuntimeError(
                    f"Non-positive MC efficiency for last entry: {sample['source']} "
                    f"(Nproc={n_processed:.0f}, Npassed={n_passed:.0f})"
                )
            sample_weight = args.lumi_pb * sample["xsec_pb"] / (n_processed * eff)
        else:
            sample_weight = args.lumi_pb * sample["xsec_pb"] / n_processed
        sample_hist_raw.Scale(sample_weight)
        mc_yield_raw += sample_hist_raw.Integral(0, sample_hist_raw.GetNbinsX() + 1)
        if mc_total_hist_raw is None:
            mc_total_hist_raw = sample_hist_raw.Clone(f"h_mc_total_raw_{args.plot}")
            mc_total_hist_raw.SetDirectory(0)
        else:
            mc_total_hist_raw.Add(sample_hist_raw)
        print(
            f"{colorize('[INFO] MC component    :', ANSI_BRIGHT_RED)} "
            f"{sample['name']:<30s} "
            f"files={len(sample['files']):>4d} "
            f"xsec_pb={sample['xsec_pb']:.6g} "
            f"Nproc={n_processed:.0f} "
            f"eff={eff:.6g} "
            f"wt={sample_weight:.6g}"
        )

    if mc_total_hist_raw is not None:
        mc_total_hist = apply_final_binning(mc_total_hist_raw, f"h_mc_total_{args.plot}", spec)
        if data_hist is not None:
            if mc_yield_raw <= 0.0:
                raise RuntimeError("MC yield is zero after selection; cannot scale to data.")
            kfactor = data_yield_raw / mc_yield_raw
            mc_total_hist.Scale(kfactor)
            print(f"[INFO] kFactor         : {kfactor:.6f}")
        print(f"[INFO] MC yield raw     : {mc_yield_raw:.6g}")
        set_hist_style(mc_total_hist, color=STYLE["mc_line_color"], marker=False)
        root_out.cd()
        mc_total_hist.Write()

    if data_hist is not None and mc_total_hist is not None:
        ratio_hist = make_ratio_hist(data_hist, mc_total_hist, f"h_ratio_{args.plot}")
        set_hist_style(ratio_hist, color=STYLE["data_color"], marker=True)
        root_out.cd()
        ratio_hist.Write()

    draw_plot(
        ROOT,
        data_hist=data_hist,
        mc_hist=mc_total_hist,
        ratio_hist=ratio_hist,
        kfactor=kfactor,
        spec=spec,
        lumi_pb=args.lumi_pb,
        output_stem=output_stem,
        root_out=root_out,
    )

    root_out.Close()
    print("[INFO] Finished.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

