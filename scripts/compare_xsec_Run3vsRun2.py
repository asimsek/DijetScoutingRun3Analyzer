#!/usr/bin/env python3

from __future__ import annotations

import argparse
import csv
import math
from pathlib import Path


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description=("Compare Run3/Run2 ratio in data against Run3/Run2 ratio in QCD MC using differential cross section xsec = N / (bin_width * lumi)."),
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument("--run2-root",      default="../inputs/CaloScoutingHT2016ALL_DatavsQDCMC/histo_data_mjj_fromTree.root", help="Run-2 ROOT file path.")
    parser.add_argument("--run3-root",      default="../inputs/PFScouting2024H_deta1p3_04March2026_23/dataVsQCD_mjj.root",      help="Run-3 ROOT file path.")
    parser.add_argument("--run2-data-hist", default="h_dat_rebin",         help="Run-2 data histogram name.")
    parser.add_argument("--run2-mc-hist",   default="h_MC_rebin",          help="Run-2 QCD MC histogram name.")
    parser.add_argument("--run3-data-hist", default="h_data_mjj",          help="Run-3 data histogram name.")
    parser.add_argument("--run3-mc-hist",   default="h_mc_total_mjj",      help="Run-3 QCD MC histogram name.")
    parser.add_argument("--lumi-run2-fb",   type=float, required=True,     help="Run-2 integrated luminosity [fb^-1].")
    parser.add_argument("--lumi-run3-fb",   type=float, required=True,     help="Run-3 integrated luminosity [fb^-1].")
    parser.add_argument("--year-run2",      default="2016",                help="Label for Run-2 year.")
    parser.add_argument("--year-run3",      default="2024",                help="Label for Run-3 year.")
    parser.add_argument("--output-dir",     default="run3_run2_reference", help="Output directory.")
    parser.add_argument("--output-prefix",  default="run3_run2_reference", help="Output file prefix.")
    parser.add_argument("--x-min-tev",      type=float, default=None,      help="Optional x-axis minimum [TeV].")
    parser.add_argument("--x-max-tev",      type=float, default=None,      help="Optional x-axis maximum [TeV].")
    parser.add_argument("--zoom-x-min-gev", type=float, default=1181.0,    help="Zoomed plot x-axis minimum [GeV].")
    parser.add_argument("--zoom-x-max-gev", type=float, default=4000.0,    help="Zoomed plot x-axis maximum [GeV].")
    parser.add_argument("--top-y-min",      type=float, default=0.0,       help="Top-panel y-axis minimum.")
    parser.add_argument("--top-y-max",      type=float, default=3000.0,    help="Top-panel y-axis maximum.")
    parser.add_argument("--bottom-y-min",   type=float, default=0.0,       help="Bottom-panel y-axis minimum.")
    parser.add_argument("--bottom-y-max",   type=float, default=2.0,       help="Bottom-panel y-axis maximum.")
    parser.add_argument("--norm-top-y-min", type=float, default=0.0,       help="Normalized top-only plot y-axis minimum.")
    parser.add_argument("--norm-top-y-max", type=float, default=100.0,     help="Normalized top-only plot y-axis maximum.")
    parser.add_argument("--debug", action="store_true", help="Print per-bin mass range, N, bin width, lumi, xsec and all derived ratios.")
    return parser


def configure_style(ROOT) -> None:
    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetOptTitle(0)
    ROOT.gStyle.SetCanvasBorderMode(0)
    ROOT.gStyle.SetFrameBorderMode(0)
    ROOT.gStyle.SetPadTickX(1)
    ROOT.gStyle.SetPadTickY(1)
    ROOT.gStyle.SetLegendBorderSize(0)
    ROOT.gStyle.SetLegendFont(42)
    ROOT.gStyle.SetLabelFont(42, "XYZ")
    ROOT.gStyle.SetTitleFont(42, "XYZ")
    ROOT.gStyle.SetTitleSize(0.055, "XYZ")
    ROOT.gStyle.SetLabelSize(0.045, "XYZ")
    ROOT.gStyle.SetEndErrorSize(0)


def load_hist(ROOT, root_path: Path, hist_name: str):
    root_file = ROOT.TFile.Open(str(root_path))
    if not root_file or root_file.IsZombie():
        raise RuntimeError(f"Could not open ROOT file: {root_path}")
    hist = root_file.Get(hist_name)
    if hist is None:
        root_file.Close()
        raise RuntimeError(f"Histogram '{hist_name}' not found in {root_path}")
    out = hist.Clone(f"{hist_name}_clone")
    out.SetDirectory(0)
    root_file.Close()
    return out


def garwood_interval(ROOT, n_count: int, cl: float = 0.682689492137):
    alpha = 1.0 - cl
    if n_count < 0:
        n_count = 0
    if n_count == 0:
        low = 0.0
    else:
        low = 0.5 * ROOT.Math.chisquared_quantile(alpha / 2.0, 2.0 * n_count)
    up = 0.5 * ROOT.Math.chisquared_quantile_c(alpha / 2.0, 2.0 * (n_count + 1))
    return low, up


def hist_to_xsec_data(ROOT, hist, lumi_fb: float):
    out = {}
    for i_bin in range(1, hist.GetNbinsX() + 1):
        low = float(hist.GetXaxis().GetBinLowEdge(i_bin))
        up = float(hist.GetXaxis().GetBinUpEdge(i_bin))
        width = up - low
        if width <= 0.0:
            continue
        n_evt = float(hist.GetBinContent(i_bin))
        n_int = int(round(n_evt))
        n_low, n_up = garwood_interval(ROOT, n_int)
        xsec = n_evt / (width * lumi_fb) if lumi_fb > 0.0 else 0.0
        err_low = (n_evt - n_low) / (width * lumi_fb) if lumi_fb > 0.0 else 0.0
        err_high = (n_up - n_evt) / (width * lumi_fb) if lumi_fb > 0.0 else 0.0
        key = (round(low, 6), round(up, 6))
        out[key] = {
            "low": low,
            "up": up,
            "width": width,
            "center_tev": 0.5 * (low + up) / 1000.0,
            "half_width_tev": 0.5 * width / 1000.0,
            "n_evt": n_evt,
            "xsec": xsec,
            "err_low": max(err_low, 0.0),
            "err_high": max(err_high, 0.0),
        }
    return out


def hist_to_xsec_mc(hist, lumi_fb: float):
    out = {}
    for i_bin in range(1, hist.GetNbinsX() + 1):
        low = float(hist.GetXaxis().GetBinLowEdge(i_bin))
        up = float(hist.GetXaxis().GetBinUpEdge(i_bin))
        width = up - low
        if width <= 0.0:
            continue
        n_evt = float(hist.GetBinContent(i_bin))
        n_err = float(hist.GetBinError(i_bin))
        xsec = n_evt / (width * lumi_fb) if lumi_fb > 0.0 else 0.0
        xsec_err = n_err / (width * lumi_fb) if lumi_fb > 0.0 else 0.0
        key = (round(low, 6), round(up, 6))
        out[key] = {
            "low": low,
            "up": up,
            "width": width,
            "center_tev": 0.5 * (low + up) / 1000.0,
            "half_width_tev": 0.5 * width / 1000.0,
            "n_evt": n_evt,
            "n_err": n_err,
            "xsec": xsec,
            "xsec_err": max(xsec_err, 0.0),
        }
    return out


def build_data_ratio(run3_data: dict, run2_data: dict):
    out = {}
    for key in sorted(set(run3_data) & set(run2_data)):
        b3 = run3_data[key]
        b2 = run2_data[key]
        if b3["xsec"] <= 0.0 or b2["xsec"] <= 0.0:
            continue
        ratio = b3["xsec"] / b2["xsec"]
        rel3_low = b3["err_low"] / b3["xsec"]
        rel3_high = b3["err_high"] / b3["xsec"]
        rel2_low = b2["err_low"] / b2["xsec"]
        rel2_high = b2["err_high"] / b2["xsec"]
        err_low = ratio * math.sqrt(rel3_low * rel3_low + rel2_high * rel2_high)
        err_high = ratio * math.sqrt(rel3_high * rel3_high + rel2_low * rel2_low)
        out[key] = {
            "low": b3["low"],
            "up": b3["up"],
            "center_tev": b3["center_tev"],
            "half_width_tev": b3["half_width_tev"],
            "ratio": ratio,
            "err_low": err_low,
            "err_high": err_high,
        }
    return out


def build_mc_ratio(run3_mc: dict, run2_mc: dict):
    out = {}
    for key in sorted(set(run3_mc) & set(run2_mc)):
        b3 = run3_mc[key]
        b2 = run2_mc[key]
        if b3["xsec"] <= 0.0 or b2["xsec"] <= 0.0:
            continue
        ratio = b3["xsec"] / b2["xsec"]
        rel3 = b3["xsec_err"] / b3["xsec"] if b3["xsec"] > 0.0 else 0.0
        rel2 = b2["xsec_err"] / b2["xsec"] if b2["xsec"] > 0.0 else 0.0
        err = ratio * math.sqrt(rel3 * rel3 + rel2 * rel2)
        out[key] = {
            "low": b3["low"],
            "up": b3["up"],
            "center_tev": b3["center_tev"],
            "half_width_tev": b3["half_width_tev"],
            "ratio": ratio,
            "err": err,
        }
    return out


def build_double_ratio(data_ratio: dict, mc_ratio: dict):
    out = {}
    for key in sorted(set(data_ratio) & set(mc_ratio)):
        d = data_ratio[key]
        m = mc_ratio[key]
        if d["ratio"] <= 0.0 or m["ratio"] <= 0.0:
            continue
        value = d["ratio"] / m["ratio"]
        rel_d_low = d["err_low"] / d["ratio"]
        rel_d_high = d["err_high"] / d["ratio"]
        rel_m = m["err"] / m["ratio"] if m["ratio"] > 0.0 else 0.0
        err_low = value * math.sqrt(rel_d_low * rel_d_low + rel_m * rel_m)
        err_high = value * math.sqrt(rel_d_high * rel_d_high + rel_m * rel_m)
        out[key] = {
            "low": d["low"],
            "up": d["up"],
            "center_tev": d["center_tev"],
            "half_width_tev": d["half_width_tev"],
            "value": value,
            "err_low": err_low,
            "err_high": err_high,
        }
    return out


def dict_to_points(x: dict):
    return [x[key] for key in sorted(x)]


def make_graph_data_ratio(ROOT, points: list[dict]):
    g = ROOT.TGraphAsymmErrors(len(points))
    g.SetName("g_data_ratio_run3_over_run2")
    for i, p in enumerate(points):
        g.SetPoint(i, p["center_tev"], p["ratio"])
        g.SetPointError(i, p["half_width_tev"], p["half_width_tev"], p["err_low"], p["err_high"])
    g.SetMarkerStyle(20)
    g.SetMarkerSize(1.0)
    g.SetMarkerColor(ROOT.kBlack)
    g.SetLineColor(ROOT.kBlack)
    return g


def make_graph_mc_ratio(ROOT, points: list[dict]):
    g = ROOT.TGraphErrors(len(points))
    g.SetName("g_mc_ratio_run3_over_run2")
    for i, p in enumerate(points):
        g.SetPoint(i, p["center_tev"], p["ratio"])
        g.SetPointError(i, p["half_width_tev"], p["err"])
    g.SetLineColor(ROOT.kRed + 1)
    g.SetLineWidth(3)
    g.SetMarkerSize(0)
    return g


def make_graph_double_ratio(ROOT, points: list[dict]):
    g = ROOT.TGraphAsymmErrors(len(points))
    g.SetName("g_double_ratio_data_over_mc")
    for i, p in enumerate(points):
        g.SetPoint(i, p["center_tev"], p["value"])
        g.SetPointError(i, p["half_width_tev"], p["half_width_tev"], p["err_low"], p["err_high"])
    g.SetMarkerStyle(20)
    g.SetMarkerSize(0.9)
    g.SetMarkerColor(ROOT.kBlue + 2)
    g.SetLineColor(ROOT.kBlue + 2)
    return g


def calc_chi2(data_points: list[dict], mc_points: list[dict]):
    mc_by_key = {(round(p["low"], 6), round(p["up"], 6)): p for p in mc_points}
    chi2 = 0.0
    ndof = 0
    for d in data_points:
        key = (round(d["low"], 6), round(d["up"], 6))
        m = mc_by_key.get(key)
        if m is None:
            continue
        sigma_d = 0.5 * (d["err_low"] + d["err_high"])
        sigma_m = m["err"]
        sigma = math.sqrt(sigma_d * sigma_d + sigma_m * sigma_m)
        if sigma <= 0.0:
            continue
        chi2 += ((d["ratio"] - m["ratio"]) / sigma) ** 2
        ndof += 1
    return chi2, ndof


def auto_range(values: list[float], min_floor: float | None = None):
    if not values:
        return (0.0, 2.0)
    y_min = min(values)
    y_max = max(values)
    if y_max <= y_min:
        y_min = y_min - 0.2
        y_max = y_max + 0.2
    span = y_max - y_min
    out_min = y_min - 0.15 * span
    out_max = y_max + 0.25 * span
    if min_floor is not None:
        out_min = max(min_floor, out_min)
    return (out_min, out_max)


def select_points_in_xrange(points: list[dict], x_min: float, x_max: float):
    selected = []
    for p in points:
        low = p["center_tev"] - p["half_width_tev"]
        up = p["center_tev"] + p["half_width_tev"]
        if up < x_min or low > x_max:
            continue
        selected.append(p)
    return selected


def compute_zoom_top_range(data_points: list[dict], mc_points: list[dict], x_min: float, x_max: float):
    values = []
    for p in select_points_in_xrange(data_points, x_min, x_max):
        values.append(max(0.0, p["ratio"] - p["err_low"]))
        values.append(p["ratio"] + p["err_high"])
    for p in select_points_in_xrange(mc_points, x_min, x_max):
        values.append(max(0.0, p["ratio"] - p["err"]))
        values.append(p["ratio"] + p["err"])
    if not values:
        return (0.0, 2.0)
    return auto_range(values, min_floor=0.0)


def normalize_data_mc_to_first_common_bin(data_points: list[dict], mc_points: list[dict]):
    mc_by_key = {(round(p["low"], 6), round(p["up"], 6)): p for p in mc_points}
    first_data = None
    first_mc = None
    for d in data_points:
        key = (round(d["low"], 6), round(d["up"], 6))
        m = mc_by_key.get(key)
        if m is None:
            continue
        if d["ratio"] <= 0.0 or m["ratio"] <= 0.0:
            continue
        first_data = d
        first_mc = m
        break
    if first_data is None or first_mc is None:
        raise RuntimeError("No valid common first bin found for unity normalization.")

    data_scale = 1.0 / first_data["ratio"]
    mc_scale = 1.0 / first_mc["ratio"]

    norm_data = []
    for p in data_points:
        q = dict(p)
        q["ratio"] = q["ratio"] * data_scale
        q["err_low"] = q["err_low"] * data_scale
        q["err_high"] = q["err_high"] * data_scale
        norm_data.append(q)

    norm_mc = []
    for p in mc_points:
        q = dict(p)
        q["ratio"] = q["ratio"] * mc_scale
        q["err"] = q["err"] * mc_scale
        norm_mc.append(q)

    return norm_data, norm_mc, data_scale, mc_scale, first_data, first_mc


def format_lumi_label(lumi_fb: float) -> str:
    return str(int(lumi_fb))


def draw_labels(ROOT, year_run3: str, year_run2: str, lumi_run3_fb: float, lumi_run2_fb: float):
    latex = ROOT.TLatex()
    latex.SetNDC(True)
    latex.SetTextAlign(13)
    latex.SetTextFont(62)
    latex.SetTextSize(0.055)
    latex.DrawLatex(0.22, 0.86, "CMS")
    latex.SetTextFont(52)
    latex.SetTextSize(0.040)
    latex.DrawLatex(0.22, 0.80, "Preliminary")
    latex.SetTextFont(42)
    latex.SetTextSize(0.034)
    latex.SetTextAlign(31)
    latex.DrawLatex(
        0.95,
        0.95,
        (
            f"Run3 ({year_run3}): {format_lumi_label(lumi_run3_fb)} fb^{{-1}}, "
            f"Run2 ({year_run2}): {format_lumi_label(lumi_run2_fb)} fb^{{-1}}"
        ),
    )


def add_x_padding(x_min: float, x_max: float) -> tuple[float, float]:
    if x_max <= x_min:
        raise RuntimeError("Invalid x-range: x_max must be greater than x_min.")
    x_span = x_max - x_min
    return x_min - 0.015 * x_span, x_max + 0.020 * x_span


def build_reference_canvas(
    ROOT,
    *,
    suffix: str,
    x_min: float,
    x_max: float,
    y_top_min: float,
    y_top_max: float,
    y_bot_min: float,
    y_bot_max: float,
    g_data_ratio,
    g_mc_ratio,
    g_double_ratio,
    year_run3: str,
    year_run2: str,
    lumi_run3_fb: float,
    lumi_run2_fb: float,
):
    canvas = ROOT.TCanvas(f"c_run3_run2_reference_{suffix}", "", 900, 900)
    g_data_ratio_draw = g_data_ratio.Clone(f"{g_data_ratio.GetName()}_{suffix}")
    g_mc_ratio_draw = g_mc_ratio.Clone(f"{g_mc_ratio.GetName()}_{suffix}")
    g_double_ratio_draw = g_double_ratio.Clone(f"{g_double_ratio.GetName()}_{suffix}")

    pad_top = ROOT.TPad(f"pad_top_{suffix}", f"pad_top_{suffix}", 0.0, 0.34, 1.0, 1.0)
    pad_top.SetTopMargin(0.08)
    pad_top.SetBottomMargin(0.03)
    pad_top.SetLeftMargin(0.18)
    pad_top.SetRightMargin(0.05)
    pad_top.Draw()
    pad_top.cd()

    h_top = ROOT.TH1F(f"h_top_frame_{suffix}", "", 200, x_min, x_max)
    h_top.GetXaxis().SetLabelOffset(999)
    h_top.GetYaxis().SetTitle("xsec Run3/Run2")
    h_top.GetYaxis().SetTitleSize(0.06)
    h_top.GetYaxis().SetLabelSize(0.050)
    h_top.GetYaxis().SetTitleOffset(1.20)
    h_top.GetYaxis().SetRangeUser(y_top_min, y_top_max)
    h_top.Draw("AXIS")

    g_mc_ratio_draw.SetFillColorAlpha(ROOT.kRed + 1, 0.20)
    g_mc_ratio_draw.Draw("E3 SAME")
    g_mc_ratio_draw.Draw("L SAME")
    g_data_ratio_draw.Draw("P SAME")

    legend_top = ROOT.TLegend(0.52, 0.74, 0.89, 0.90)
    legend_top.SetFillStyle(0)
    legend_top.SetBorderSize(0)
    legend_top.SetTextSize(0.036)
    legend_top.AddEntry(g_data_ratio_draw, "Data [Run3/Run2]", "pe")
    legend_top.AddEntry(g_mc_ratio_draw, "QCD [Run3/Run2]", "lf")
    legend_top.Draw()

    draw_labels(ROOT, year_run3, year_run2, lumi_run3_fb, lumi_run2_fb)
    ROOT.gPad.RedrawAxis()

    canvas.cd()
    pad_bottom = ROOT.TPad(f"pad_bottom_{suffix}", f"pad_bottom_{suffix}", 0.0, 0.0, 1.0, 0.34)
    pad_bottom.SetTopMargin(0.03)
    pad_bottom.SetBottomMargin(0.36)
    pad_bottom.SetLeftMargin(0.18)
    pad_bottom.SetRightMargin(0.05)
    pad_bottom.Draw()
    pad_bottom.cd()

    h_bottom = ROOT.TH1F(f"h_bottom_frame_{suffix}", "", 200, x_min, x_max)
    h_bottom.GetXaxis().SetTitle("Dijet Mass [TeV]")
    h_bottom.GetYaxis().SetTitle("#frac{(Run3/Run2)_{Data}}{(Run3/Run2)_{QCD MC}}")
    h_bottom.GetYaxis().SetRangeUser(y_bot_min, y_bot_max)
    h_bottom.GetYaxis().SetNdivisions(405, True)
    h_bottom.GetXaxis().SetTitleSize(0.12)
    h_bottom.GetXaxis().SetLabelSize(0.11)
    h_bottom.GetYaxis().SetTitleSize(0.095)
    h_bottom.GetYaxis().SetLabelSize(0.095)
    h_bottom.GetYaxis().SetTitleOffset(0.65)
    h_bottom.Draw("AXIS")

    g_double_ratio_draw.Draw("P SAME")
    line_unity = ROOT.TLine(x_min, 1.0, x_max, 1.0)
    line_unity.SetLineStyle(2)
    line_unity.SetLineColor(ROOT.kBlack)
    line_unity.Draw("SAME")

    ROOT.gPad.RedrawAxis()
    canvas._keepalive = [
        g_data_ratio_draw,
        g_mc_ratio_draw,
        g_double_ratio_draw,
        pad_top,
        h_top,
        legend_top,
        pad_bottom,
        h_bottom,
        line_unity,
    ]
    return canvas


def write_debug_table(
    csv_path: Path,
    run2_data: dict,
    run3_data: dict,
    run2_mc: dict,
    run3_mc: dict,
    data_ratio: dict,
    mc_ratio: dict,
    double_ratio: dict,
) -> None:
    keys = sorted(
        set(run2_data) | set(run3_data) | set(run2_mc) | set(run3_mc) | set(data_ratio) | set(mc_ratio) | set(double_ratio)
    )
    with csv_path.open("w", newline="") as handle:
        writer = csv.writer(handle)
        writer.writerow(
            [
                "mass_low_GeV",
                "mass_up_GeV",
                "run2_data_n",
                "run2_data_xsec_fb_per_GeV",
                "run3_data_n",
                "run3_data_xsec_fb_per_GeV",
                "run2_mc_n",
                "run2_mc_xsec_fb_per_GeV",
                "run3_mc_n",
                "run3_mc_xsec_fb_per_GeV",
                "ratio_data_run3_over_run2",
                "ratio_mc_run3_over_run2",
                "double_ratio_data_over_mc",
            ]
        )
        for key in keys:
            r2d = run2_data.get(key, {})
            r3d = run3_data.get(key, {})
            r2m = run2_mc.get(key, {})
            r3m = run3_mc.get(key, {})
            dr = data_ratio.get(key, {})
            mr = mc_ratio.get(key, {})
            dbr = double_ratio.get(key, {})
            low = r3d.get("low", r2d.get("low", r3m.get("low", r2m.get("low", 0.0))))
            up = r3d.get("up", r2d.get("up", r3m.get("up", r2m.get("up", 0.0))))
            writer.writerow(
                [
                    low,
                    up,
                    r2d.get("n_evt", ""),
                    r2d.get("xsec", ""),
                    r3d.get("n_evt", ""),
                    r3d.get("xsec", ""),
                    r2m.get("n_evt", ""),
                    r2m.get("xsec", ""),
                    r3m.get("n_evt", ""),
                    r3m.get("xsec", ""),
                    dr.get("ratio", ""),
                    mr.get("ratio", ""),
                    dbr.get("value", ""),
                ]
            )


def debug_print_xsec(label: str, bins: dict, lumi_fb: float) -> None:
    print(f"[DEBUG] {label}")
    print("[DEBUG]   mass_low  mass_up   nEvent        bin_width   lumi_fb   xsec[fb/GeV]")
    for key in sorted(bins):
        b = bins[key]
        if b["n_evt"] <= 0.0:
            continue
        print(
            "[DEBUG]   "
            f"{b['low']:7.1f} {b['up']:7.1f} "
            f"{b['n_evt']:12.6g} "
            f"{b['width']:10.3f} "
            f"{lumi_fb:8.3f} "
            f"{b['xsec']:13.6g}"
        )


def debug_print_ratio(label: str, bins: dict) -> None:
    print(f"[DEBUG] {label}")
    print("[DEBUG]   mass_low  mass_up   ratio")
    for key in sorted(bins):
        b = bins[key]
        value = b["ratio"] if "ratio" in b else b["value"]
        print(f"[DEBUG]   {b['low']:7.1f} {b['up']:7.1f} {value:10.6f}")


def main() -> int:
    args = build_parser().parse_args()
    try:
        import ROOT
    except ImportError as exc:
        raise SystemExit("PyROOT is required. Load ROOT/CMSSW first.") from exc

    configure_style(ROOT)

    run2_root = Path(args.run2_root).resolve()
    run3_root = Path(args.run3_root).resolve()
    out_dir = Path(args.output_dir).resolve()
    out_dir.mkdir(parents=True, exist_ok=True)

    h_run2_data = load_hist(ROOT, run2_root, args.run2_data_hist)
    h_run2_mc = load_hist(ROOT, run2_root, args.run2_mc_hist)
    h_run3_data = load_hist(ROOT, run3_root, args.run3_data_hist)
    h_run3_mc = load_hist(ROOT, run3_root, args.run3_mc_hist)

    run2_data = hist_to_xsec_data(ROOT, h_run2_data, args.lumi_run2_fb)
    run3_data = hist_to_xsec_data(ROOT, h_run3_data, args.lumi_run3_fb)
    run2_mc = hist_to_xsec_mc(h_run2_mc, args.lumi_run2_fb)
    run3_mc = hist_to_xsec_mc(h_run3_mc, args.lumi_run3_fb)

    data_ratio = build_data_ratio(run3_data, run2_data)
    mc_ratio = build_mc_ratio(run3_mc, run2_mc)
    double_ratio = build_double_ratio(data_ratio, mc_ratio)

    data_points = dict_to_points(data_ratio)
    mc_points = dict_to_points(mc_ratio)
    double_points = dict_to_points(double_ratio)

    if not data_points:
        raise RuntimeError("No common non-zero bins for data Run3/Run2 ratio.")
    if not mc_points:
        raise RuntimeError("No common non-zero bins for MC Run3/Run2 ratio.")
    if not double_points:
        raise RuntimeError("No common bins for data_ratio / mc_ratio.")

    if args.debug:
        debug_print_xsec("Run2 data", run2_data, args.lumi_run2_fb)
        debug_print_xsec("Run3 data", run3_data, args.lumi_run3_fb)
        debug_print_xsec("Run2 MC", run2_mc, args.lumi_run2_fb)
        debug_print_xsec("Run3 MC", run3_mc, args.lumi_run3_fb)
        debug_print_ratio("Data ratio (Run3/Run2)", data_ratio)
        debug_print_ratio("MC ratio (Run3/Run2)", mc_ratio)
        debug_print_ratio("Double ratio: (Data ratio)/(MC ratio)", double_ratio)

    g_data_ratio = make_graph_data_ratio(ROOT, data_points)
    g_mc_ratio = make_graph_mc_ratio(ROOT, mc_points)
    g_double_ratio = make_graph_double_ratio(ROOT, double_points)

    x_min_auto = min(p["center_tev"] - p["half_width_tev"] for p in data_points)
    x_max_auto = max(p["center_tev"] + p["half_width_tev"] for p in data_points)

    # Top-only normalized plot: normalize both series to unity at first common bin.
    norm_data_points, norm_mc_points, data_norm_scale, mc_norm_scale, first_data_bin, first_mc_bin = (
        normalize_data_mc_to_first_common_bin(data_points, mc_points)
    )
    g_data_ratio_norm = make_graph_data_ratio(ROOT, norm_data_points)
    g_data_ratio_norm.SetName("g_data_ratio_run3_over_run2_norm1")
    g_mc_ratio_norm = make_graph_mc_ratio(ROOT, norm_mc_points)
    g_mc_ratio_norm.SetName("g_mc_ratio_run3_over_run2_norm1")

    x_min = args.x_min_tev if args.x_min_tev is not None else x_min_auto
    x_max = args.x_max_tev if args.x_max_tev is not None else x_max_auto
    x_min, x_max = add_x_padding(x_min, x_max)
    zoom_x_min, zoom_x_max = add_x_padding(args.zoom_x_min_gev / 1000.0, args.zoom_x_max_gev / 1000.0)

    y_top_min = args.top_y_min
    y_top_max = args.top_y_max
    y_bot_min = args.bottom_y_min
    y_bot_max = args.bottom_y_max
    y_norm_top_min = args.norm_top_y_min
    y_norm_top_max = args.norm_top_y_max
    if y_top_max <= y_top_min:
        raise RuntimeError("Invalid top y-range: top-y-max must be greater than top-y-min.")
    if y_bot_max <= y_bot_min:
        raise RuntimeError("Invalid bottom y-range: bottom-y-max must be greater than bottom-y-min.")
    if y_norm_top_max <= y_norm_top_min:
        raise RuntimeError("Invalid normalized top y-range: norm-top-y-max must be greater than norm-top-y-min.")

    zoom_y_top_min, zoom_y_top_max = compute_zoom_top_range(data_points, mc_points, zoom_x_min, zoom_x_max)

    canvas = build_reference_canvas(
        ROOT,
        suffix="full",
        x_min=x_min,
        x_max=x_max,
        y_top_min=y_top_min,
        y_top_max=y_top_max,
        y_bot_min=y_bot_min,
        y_bot_max=y_bot_max,
        g_data_ratio=g_data_ratio,
        g_mc_ratio=g_mc_ratio,
        g_double_ratio=g_double_ratio,
        year_run3=args.year_run3,
        year_run2=args.year_run2,
        lumi_run3_fb=args.lumi_run3_fb,
        lumi_run2_fb=args.lumi_run2_fb,
    )

    canvas_zoom = build_reference_canvas(
        ROOT,
        suffix="zoom",
        x_min=zoom_x_min,
        x_max=zoom_x_max,
        y_top_min=zoom_y_top_min,
        y_top_max=zoom_y_top_max,
        y_bot_min=y_bot_min,
        y_bot_max=y_bot_max,
        g_data_ratio=g_data_ratio,
        g_mc_ratio=g_mc_ratio,
        g_double_ratio=g_double_ratio,
        year_run3=args.year_run3,
        year_run2=args.year_run2,
        lumi_run3_fb=args.lumi_run3_fb,
        lumi_run2_fb=args.lumi_run2_fb,
    )

    chi2, ndof = calc_chi2(data_points, mc_points)

    # Additional top-only plot: Run3/Run2 ratios normalized to unity for shape comparison.
    canvas_norm = ROOT.TCanvas("c_run3_run2_reference_norm1_top", "", 900, 700)
    canvas_norm.cd()
    canvas_norm.SetLeftMargin(0.18)
    canvas_norm.SetBottomMargin(0.14)
    canvas_norm.SetRightMargin(0.05)
    canvas_norm.SetTopMargin(0.08)
    h_top_norm = ROOT.TH1F("h_top_norm_frame", "", 200, x_min, x_max)
    h_top_norm.GetXaxis().SetTitle("Dijet Mass [TeV]")
    h_top_norm.GetYaxis().SetTitle("Normalized xsec Run3/Run2")
    h_top_norm.GetYaxis().SetTitleSize(0.06)
    h_top_norm.GetYaxis().SetLabelSize(0.050)
    h_top_norm.GetYaxis().SetTitleOffset(1.20)
    h_top_norm.GetXaxis().SetTitleSize(0.055)
    h_top_norm.GetXaxis().SetLabelSize(0.045)
    h_top_norm.GetYaxis().SetRangeUser(y_norm_top_min, y_norm_top_max)
    h_top_norm.Draw("AXIS")

    g_mc_ratio_norm.SetFillColorAlpha(ROOT.kRed + 1, 0.20)
    g_mc_ratio_norm.Draw("E3 SAME")
    g_mc_ratio_norm.Draw("L SAME")
    g_data_ratio_norm.Draw("P SAME")

    legend_norm = ROOT.TLegend(0.52, 0.74, 0.89, 0.90)
    legend_norm.SetFillStyle(0)
    legend_norm.SetBorderSize(0)
    legend_norm.SetTextSize(0.036)
    legend_norm.AddEntry(g_data_ratio_norm, "Data [Run3/Run2]", "pe")
    legend_norm.AddEntry(g_mc_ratio_norm, "QCD [Run3/Run2]", "lf")
    legend_norm.Draw()

    draw_labels(ROOT, args.year_run3, args.year_run2, args.lumi_run3_fb, args.lumi_run2_fb)
    ROOT.gPad.RedrawAxis()

    out_pdf = out_dir / f"{args.output_prefix}.pdf"
    out_pdf_zoom = out_dir / f"{args.output_prefix}_zoom.pdf"
    out_pdf_norm = out_dir / f"{args.output_prefix}_normToUnity_top.pdf"
    out_root = out_dir / f"{args.output_prefix}.root"
    out_csv = out_dir / f"{args.output_prefix}.csv"

    out_file = ROOT.TFile(str(out_root), "RECREATE")
    out_file.cd()
    h_run2_data.Write("h_run2_data_input")
    h_run2_mc.Write("h_run2_mc_input")
    h_run3_data.Write("h_run3_data_input")
    h_run3_mc.Write("h_run3_mc_input")
    g_data_ratio.Write()
    g_mc_ratio.Write()
    g_double_ratio.Write()
    g_data_ratio_norm.Write()
    g_mc_ratio_norm.Write()
    canvas.Write()
    canvas_zoom.Write()
    canvas_norm.Write()
    out_file.Close()

    write_debug_table(out_csv, run2_data, run3_data, run2_mc, run3_mc, data_ratio, mc_ratio, double_ratio)

    canvas.SaveAs(str(out_pdf))
    canvas.Close()
    canvas_zoom.SaveAs(str(out_pdf_zoom))
    canvas_zoom.Close()
    canvas_norm.SaveAs(str(out_pdf_norm))
    canvas_norm.Close()

    print(f"[INFO] Run-2 file               : {run2_root}")
    print(f"[INFO] Run-3 file               : {run3_root}")
    print(f"[INFO] Run-2 lumi [fb^-1]       : {args.lumi_run2_fb:g}")
    print(f"[INFO] Run-3 lumi [fb^-1]       : {args.lumi_run3_fb:g}")
    print(f"[INFO] Data ratio bins          : {len(data_points)}")
    print(f"[INFO] MC ratio bins            : {len(mc_points)}")
    print(f"[INFO] Double-ratio bins        : {len(double_points)}")
    print(
        "[INFO] First-bin anchor         : "
        f"[{first_data_bin['low']:.1f}, {first_data_bin['up']:.1f}] GeV "
        f"data={first_data_bin['ratio']:.6g} mc={first_mc_bin['ratio']:.6g}"
    )
    print(f"[INFO] Data norm scale          : {data_norm_scale:.6g}")
    print(f"[INFO] MC norm scale            : {mc_norm_scale:.6g}")
    print(f"[INFO] Top y-range              : [{y_top_min:g}, {y_top_max:g}]")
    print(f"[INFO] Zoom top y-range         : [{zoom_y_top_min:g}, {zoom_y_top_max:g}]")
    print(f"[INFO] Bottom y-range           : [{y_bot_min:g}, {y_bot_max:g}]")
    print(f"[INFO] Norm top y-range         : [{y_norm_top_min:g}, {y_norm_top_max:g}]")
    print(f"[INFO] Zoom x-range [GeV]       : [{args.zoom_x_min_gev:g}, {args.zoom_x_max_gev:g}]")
    if ndof > 0:
        print(f"[INFO] Agreement chi2/N         : {chi2:.3f}/{ndof} = {chi2/ndof:.3f}")
    else:
        print("[INFO] Agreement chi2/N         : n/a")
    print(f"[INFO] Output plot              : {out_pdf}")
    print(f"[INFO] Output plot (zoom)       : {out_pdf_zoom}")
    print(f"[INFO] Output plot (norm top)   : {out_pdf_norm}")
    print(f"[INFO] Output ROOT              : {out_root}")
    print(f"[INFO] Output table             : {out_csv}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

