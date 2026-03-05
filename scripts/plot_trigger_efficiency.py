#!/usr/bin/env python3

from __future__ import annotations

import argparse
import csv
import glob
import math
import os
import re
import sys
import time
from array import array
from pathlib import Path

TREE_PATH_DEFAULT = "rootTupleTree/tree"
DEFAULT_CMS_LABEL = "CMS"
DEFAULT_EXTRA_LABEL = "Preliminary"
DEFAULT_SQRT_S = "13.6 TeV"
ANSI_RESET = "\033[0m"
ANSI_BRIGHT_RED = "\033[91m"
TRIGGER_BRANCH_CANDIDATES = {
    "jetht": ("passHLT_PFScoutingHT", "DST_PFScouting_JetHT"),
    "singlemuon": ("passHLT_PFScouting_SingleMuon", "DST_PFScouting_SingleMuon"),
}
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
DELTA_ETA_CUT_VAL = "1.1"
ETA_CUT_VAL = "2.5"
TRIGGER_SELECTION = (
    "PassJSON == 1 && nJet > 1 && IdTight_j1 > 0.5 && IdTight_j2 > 0.5"
    " && pTWJ_j1 > 60 && pTWJ_j2 > 30"
    f" && TMath::Abs(etaWJ_j1) < {ETA_CUT_VAL} && TMath::Abs(etaWJ_j2) < {ETA_CUT_VAL}"
    f" && TMath::Abs(deltaETAjj) < {DELTA_ETA_CUT_VAL}"
)
MASS_DISPLAY_RANGE = (280.0, 1300.0)
EFFICIENCY_Y_RANGE = (0.0, 1.30)


def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Evaluate PFScouting Run-3 JetHT trigger efficiency with Monitoring data.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument("--monitoring-list", required=True, help="Reduced nTuple list for ScoutingPFMonitoring data.")
    parser.add_argument("--scouting-list", required=True, help="Reduced nTuple list for regular PFScouting data used for 1/sqrt(N).")
    parser.add_argument("--output-dir", default="trigger_efficiency", help="Output directory.")
    parser.add_argument("--output-prefix", default="pfscouting_trigger_", help="Prefix for output file names.")
    parser.add_argument("--lumi-pb", type=float, default=5490.0, help="Integrated luminosity in pb^-1.")
    parser.add_argument("--year", default="2024", help="Data-taking year label shown in the legend.")
    parser.add_argument("--threads", type=int, default=8, help="ROOT implicit MT thread count. Use 0 to disable.")
    return parser


def mass_edges_array() -> array:
    return array("d", [float(value) for value in MASS_BINS])


def clean_lines(path: Path):
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


def deduplicate(seq) -> list[str]:
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


def maybe_red(text: str, condition: bool) -> str:
    if condition and sys.stdout.isatty():
        return f"{ANSI_BRIGHT_RED}{text}{ANSI_RESET}"
    return text


def format_elapsed_time(elapsed_seconds: float) -> str:
    total_seconds = max(0, int(elapsed_seconds))
    hours, remainder = divmod(total_seconds, 3600)
    minutes, seconds = divmod(remainder, 60)
    parts = []
    if hours > 0:
        parts.append(f"{hours} hour(s)")
    if minutes > 0 or hours > 0:
        parts.append(f"{minutes} minute(s)")
    parts.append(f"{seconds} second(s)")
    return " ".join(parts)


def make_mass_model(ROOT, hist_name: str):
    edges = mass_edges_array()
    return ROOT.RDF.TH1DModel(hist_name, "", len(edges) - 1, edges)


def print_stage(current: int, total: int, message: str) -> None:
    print(f"[PROGRESS] [{current}/{total}] {message}", flush=True)


def build_chain_with_progress(ROOT, tree_path: str, files: list[str], label: str):
    chain = ROOT.TChain(tree_path)
    total = len(files)
    added = 0
    report_every = max(1, total // 10)
    for idx, file_name in enumerate(files, start=1):
        added += chain.Add(file_name)
        if total <= 20 or idx == total or idx % report_every == 0:
            print(
                f"[PROGRESS] {label} ROOT read: [{idx}/{total}] {Path(file_name).name}",
                flush=True,
            )
    if added == 0:
        raise RuntimeError(f"Could not add any files to TChain for tree {tree_path}.")
    return chain


def clone_hist(result_ptr, name: str):
    hist = result_ptr.GetValue().Clone(name)
    hist.SetDirectory(0)
    hist.Sumw2()
    return hist


def find_branch_name(chain, candidates: tuple[str, ...], label: str) -> str:
    branch_names = {branch.GetName() for branch in chain.GetListOfBranches()}
    for candidate in candidates:
        if candidate in branch_names:
            return candidate
    raise RuntimeError(f"Could not find a {label} trigger branch in {TREE_PATH_DEFAULT}. Tried: {', '.join(candidates)}")


def extract_branch_names(expression: str) -> set[str]:
    tokens = set(re.findall(r"[A-Za-z_][A-Za-z0-9_]*", expression))
    excluded = {
        "TMath",
        "Abs",
        "and",
        "or",
        "if",
        "else",
        "true",
        "false",
    }
    return {token for token in tokens if token not in excluded}


def configure_branch_status(chain, enabled_branches: set[str]) -> None:
    branch_names = {branch.GetName() for branch in chain.GetListOfBranches()}
    chain.SetBranchStatus("*", 0)
    for branch_name in sorted(enabled_branches):
        if branch_name in branch_names:
            chain.SetBranchStatus(branch_name, 1)


def build_summary_rows(efficiency, denom_hist, scouting_hist) -> list[dict]:
    rows: list[dict] = []
    xaxis = denom_hist.GetXaxis()
    for bin_idx in range(1, denom_hist.GetNbinsX() + 1):
        center = xaxis.GetBinCenter(bin_idx)
        if center < MASS_DISPLAY_RANGE[0] or center > MASS_DISPLAY_RANGE[1]:
            continue
        low_edge = int(round(xaxis.GetBinLowEdge(bin_idx)))
        denom_count = float(denom_hist.GetBinContent(bin_idx))
        if denom_count <= 0.0:
            continue
        eff = float(efficiency.GetEfficiency(bin_idx))
        err_low = float(efficiency.GetEfficiencyErrorLow(bin_idx))
        err_up = float(efficiency.GetEfficiencyErrorUp(bin_idx))
        scouting_count = float(scouting_hist.GetBinContent(bin_idx))
        inv_sqrt_n = (1.0 / math.sqrt(scouting_count)) if scouting_count > 0.0 else 0.0
        diff_stat_minus_ineff = inv_sqrt_n - (1.0 - eff)
        rows.append(
            {
                "mass_low": low_edge,
                "mass_high": int(round(xaxis.GetBinUpEdge(bin_idx))),
                "efficiency": eff,
                "eff_err_low": err_low,
                "eff_err_up": err_up,
                "inefficiency": 1.0 - eff,
                "inv_sqrt_n": inv_sqrt_n,
                "diff_stat_minus_ineff": diff_stat_minus_ineff,
                "denominator_count": denom_count,
                "scouting_count": scouting_count,
            }
        )
    return rows


def write_summary_csv(path: Path, rows: list[dict]) -> None:
    with path.open("w", newline="") as handle:
        writer = csv.DictWriter(
            handle,
            fieldnames=[
                "mass_low",
                "mass_high",
                "efficiency",
                "eff_err_low",
                "eff_err_up",
                "inefficiency",
                "inv_sqrt_n",
                "diff_stat_minus_ineff",
                "denominator_count",
                "scouting_count",
            ],
        )
        writer.writeheader()
        writer.writerows(rows)


def format_lumi_fb(lumi_pb: float) -> str:
    return str(int(lumi_pb / 1000.0))


def draw_labels(ROOT, lumi_pb: float) -> None:
    latex = ROOT.TLatex()
    latex.SetNDC(True)
    latex.SetTextAlign(13)
    latex.SetTextFont(62)
    latex.SetTextSize(0.055)
    latex.DrawLatex(0.20, 0.88, DEFAULT_CMS_LABEL)
    latex.SetTextFont(52)
    latex.SetTextSize(0.040)
    latex.DrawLatex(0.20, 0.82, DEFAULT_EXTRA_LABEL)
    latex.SetTextFont(42)
    latex.SetTextAlign(31)
    latex.SetTextSize(0.040)
    latex.DrawLatex(0.95, 0.94, f"{format_lumi_fb(lumi_pb)} fb^{{-1}} ({DEFAULT_SQRT_S})")


def draw_efficiency_plot(ROOT, *, graph, output_pdf: Path, root_out, lumi_pb: float, year: str):
    canvas = ROOT.TCanvas("c_trigger_efficiency", "", 800, 800)
    canvas.cd()
    ROOT.gPad.SetTopMargin(0.08)
    ROOT.gPad.SetRightMargin(0.05)
    ROOT.gPad.SetLeftMargin(0.16)
    ROOT.gPad.SetBottomMargin(0.13)

    graph.SetTitle("")
    graph.GetXaxis().SetTitle("Dijet mass [GeV]")
    graph.GetYaxis().SetTitle("Trigger efficiency")
    graph.GetXaxis().SetRangeUser(*MASS_DISPLAY_RANGE)
    graph.GetYaxis().SetRangeUser(*EFFICIENCY_Y_RANGE)
    graph.GetXaxis().SetTitleSize(0.05)
    graph.GetYaxis().SetTitleSize(0.05)
    graph.GetXaxis().SetLabelSize(0.045)
    graph.GetYaxis().SetLabelSize(0.045)
    graph.GetYaxis().SetTitleOffset(1.35)
    graph.SetMarkerStyle(20)
    graph.SetMarkerSize(1.0)
    graph.SetMarkerColor(ROOT.kBlack)
    graph.SetLineColor(ROOT.kBlack)
    graph.Draw("AP")

    legend = ROOT.TLegend(0.62, 0.80, 0.94, 0.90)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.SetTextSize(0.034)
    legend.AddEntry(graph, f"Data [{year}]", "pe")
    legend.Draw()

    cut_box = ROOT.TPaveText(0.59, 0.20, 0.94, 0.28, "NDC")
    cut_box.SetBorderSize(0)
    cut_box.SetFillStyle(0)
    cut_box.SetTextAlign(23)
    cut_box.SetTextSize(0.034)
    cut_box.SetTextFont(42)
    cut_box.AddText("Wide PF-jets")
    cut_box.AddText(f"|#eta| < {ETA_CUT_VAL}, |#Delta#eta| < {DELTA_ETA_CUT_VAL}")
    cut_box.Draw()

    draw_labels(ROOT, lumi_pb)
    ROOT.gPad.RedrawAxis()

    root_out.cd()
    canvas.Write()
    canvas.SaveAs(str(output_pdf))
    canvas.Close()


def main() -> int:
    start_time = time.time()
    args = build_arg_parser().parse_args()
    total_stages = 10

    print_stage(1, total_stages, "Reading Monitoring reduced-nTuple list")
    monitoring_files = read_data_inputs(args.monitoring_list)
    print_stage(2, total_stages, "Reading Scouting reduced-nTuple list")
    scouting_files = read_data_inputs(args.scouting_list)

    try:
        import ROOT
    except ImportError as exc:
        raise SystemExit("PyROOT is required to run this script. Load a CMSSW/ROOT environment first.") from exc

    configure_root_style(ROOT)
    if args.threads > 0:
        ROOT.EnableImplicitMT(args.threads)

    output_dir = Path(args.output_dir).resolve()
    output_dir.mkdir(parents=True, exist_ok=True)
    year_tag = re.sub(r"[^A-Za-z0-9_.-]+", "_", args.year.strip())
    stem = f"{args.output_prefix}run3_pfscouting_{year_tag}"
    output_root = output_dir / f"{stem}.root"
    summary_csv = output_dir / f"{stem}_summary.csv"
    eff_pdf = output_dir / f"{stem}_efficiency.pdf"

    selection = TRIGGER_SELECTION

    print_stage(3, total_stages, "Building Monitoring TChain")
    monitoring_chain = build_chain_with_progress(ROOT, TREE_PATH_DEFAULT, monitoring_files, "Monitoring")
    print_stage(4, total_stages, "Building Scouting TChain")
    scouting_chain = build_chain_with_progress(ROOT, TREE_PATH_DEFAULT, scouting_files, "Scouting")

    print_stage(5, total_stages, "Resolving trigger branches and enabling required branches")
    monitoring_jetht = find_branch_name(monitoring_chain, TRIGGER_BRANCH_CANDIDATES["jetht"], "JetHT")
    monitoring_singlemuon = find_branch_name(monitoring_chain, TRIGGER_BRANCH_CANDIDATES["singlemuon"], "SingleMuon")
    scouting_jetht = find_branch_name(scouting_chain, TRIGGER_BRANCH_CANDIDATES["jetht"], "JetHT")

    required_branches = extract_branch_names(selection)
    required_branches.add("mjj")
    configure_branch_status(monitoring_chain, required_branches | {monitoring_jetht, monitoring_singlemuon})
    configure_branch_status(scouting_chain, required_branches | {scouting_jetht})

    monitoring_df = ROOT.RDataFrame(monitoring_chain)
    scouting_df = ROOT.RDataFrame(scouting_chain)

    denom_cut = f"({selection}) && ({monitoring_singlemuon} > 0.5)"
    numer_cut = f"({denom_cut}) && ({monitoring_jetht} > 0.5)"
    scouting_cut = f"({selection}) && ({scouting_jetht} > 0.5)"

    print_stage(6, total_stages, "Booking ROOT histograms for denominator, numerator, and scouting counts")
    denom_ptr = monitoring_df.Filter(denom_cut).Histo1D(make_mass_model(ROOT, "h_denom"), "mjj")
    numer_ptr = monitoring_df.Filter(numer_cut).Histo1D(make_mass_model(ROOT, "h_numer"), "mjj")
    scouting_ptr = scouting_df.Filter(scouting_cut).Histo1D(make_mass_model(ROOT, "h_scouting"), "mjj")

    print_stage(7, total_stages, "Materializing denominator histogram")
    h_denom = clone_hist(denom_ptr, "h_monitoring_singlemuon")
    print_stage(8, total_stages, "Materializing numerator and scouting histograms")
    h_numer = clone_hist(numer_ptr, "h_monitoring_jetht_and_singlemuon")
    h_scouting = clone_hist(scouting_ptr, "h_scouting_jetht")

    if not ROOT.TEfficiency.CheckConsistency(h_numer, h_denom):
        raise RuntimeError("Numerator/denominator histograms are not consistent for TEfficiency.")

    print_stage(9, total_stages, "Building trigger efficiency, inefficiency, and statistical uncertainty objects")
    efficiency = ROOT.TEfficiency(h_numer, h_denom)
    efficiency.SetName("teff_pfscouting_jetht")
    efficiency.SetStatisticOption(ROOT.TEfficiency.kFCP)
    graph = efficiency.CreateGraph()
    graph.SetName("g_pfscouting_jetht_efficiency")

    summary_rows = build_summary_rows(efficiency, h_denom, h_scouting)
    write_summary_csv(summary_csv, summary_rows)

    print(f"[INFO] Monitoring files  : {len(monitoring_files)}")
    print(f"[INFO] Scouting files    : {len(scouting_files)}")
    print(f"[INFO] Tree             : {TREE_PATH_DEFAULT}")
    print(f"[INFO] Selection        : {selection}")
    print(f"[INFO] JetHT branch     : {monitoring_jetht}")
    print(f"[INFO] SingleMuon branch: {monitoring_singlemuon}")
    print("[INFO] Efficiency source: Monitoring data (DST_* / passHLT_* branches)")
    print("[INFO] 1/sqrt(N) source : Scouting data")
    print(f"[INFO] Lumi [fb^-1]     : {format_lumi_fb(args.lumi_pb)}")
    print(f"[INFO] Output ROOT file : {output_root}")
    print(f"[INFO] Summary CSV      : {summary_csv}")

    print_stage(10, total_stages, "Writing ROOT/CSV/PDF outputs")
    root_out = ROOT.TFile(str(output_root), "RECREATE")
    if not root_out or root_out.IsZombie():
        raise RuntimeError(f"Could not create output ROOT file: {output_root}")

    root_out.cd()
    h_denom.Write()
    h_numer.Write()
    h_scouting.Write()
    efficiency.Write()
    graph.Write()

    draw_efficiency_plot(
        ROOT,
        graph=graph,
        output_pdf=eff_pdf,
        root_out=root_out,
        lumi_pb=args.lumi_pb,
        year=args.year,
    )

    root_out.Close()

    print("[INFO] Key mass points:")
    for row in summary_rows:
        line = (
            f"[INFO]   {row['mass_low']:4d} GeV : "
            f"eff={row['efficiency']:.4f} "
            f"-{row['eff_err_low']:.4f}/+{row['eff_err_up']:.4f} "
            f"1-eff={row['inefficiency']:.4f} "
            f"1/sqrt(N)={row['inv_sqrt_n']:.4f} "
            f"diff=(1/sqrt(N)-ineff)={row['diff_stat_minus_ineff']:.4f}"
        )
        print(maybe_red(line, row["inv_sqrt_n"] > row["inefficiency"]))
    print(f"[INFO] Total elapsed time: {format_elapsed_time(time.time() - start_time)}")
    print("[INFO] Finished.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

