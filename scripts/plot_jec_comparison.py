from __future__ import annotations

import argparse
import copy
import csv
import glob
import hashlib
import json
import math
import os
import re
import shlex
import shutil
import subprocess
import sys
import tempfile
import time
import warnings
from concurrent.futures import ProcessPoolExecutor
from pathlib import Path


def root_output_main(arguments):
    from array import array
    import uuid

    import ROOT

    ROOT.gROOT.SetBatch(True)
    ROOT.TH1.AddDirectory(False)
    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetEndErrorSize(3)
    ROOT.gStyle.SetPalette(ROOT.kViridis)


    def latex(text):
        text = text.replace('$', '').replace('−', '-').replace('–', '-')
        text = re.sub(r'\\mathrm\{([^{}]+)\}', r'{\1}', text)
        text = text.replace('\\left', '').replace('\\right', '').replace('\\,', ' ')
        return re.sub(r'\\([A-Za-z]+)', r'#\1', text)


    def color(value):
        return ROOT.TColor.GetColor(value)


    def histogram(name, edges, values, label='', errors=None):
        h = ROOT.TH1D(name, '', len(edges)-1, array('d', edges))
        h.SetDirectory(0)
        h.SetOption('HIST')
        h.GetXaxis().SetTitle(latex(label))
        flow = len(values)==len(edges)+1
        for i, value in enumerate(values):
            if value is None or not math.isfinite(value):
                continue
            b = i if flow else i+1
            h.SetBinContent(b, value)
            h.SetBinError(b, errors[i] if errors is not None else math.sqrt(max(0,value)))
        h.SetEntries(sum(value for value in values if value is not None and math.isfinite(value)))
        return h


    def tree(name, columns, rows):
        t = ROOT.TTree(name, name)
        buffers = []
        for i, column in enumerate(columns):
            values = [r[i] for r in rows if r[i] is not None]
            if any(isinstance(v,str) for v in values):
                buffer = ROOT.std.string()
                t.Branch(column, buffer)
                kind = 's'
            elif values and all(isinstance(v,(int,bool)) for v in values):
                buffer = array('q', [0])
                t.Branch(column, buffer, column+'/L')
                kind = 'i'
            else:
                buffer = array('d', [0])
                t.Branch(column, buffer, column+'/D')
                kind = 'f'
            buffers.append((kind, buffer))
        for row in rows:
            for value, (kind, buffer) in zip(row,buffers):
                if kind=='s':
                    buffer.assign(str(value) if value is not None else '')
                else:
                    buffer[0] = value if value is not None else math.nan if kind=='f' else 0
            t.Fill()
        t.Write()
        t.SetDirectory(0)


    def raw_objects(d):
        for role, sample in d['samples'].items():
            for name, spec in sample['histograms'].items():
                histogram('raw_'+role+'_'+name,spec['edges'],spec['counts'],spec['label']).Write()
            cuts = histogram('raw_'+role+'_cutflow', list(range(len(sample['cutflow'])+1)), sample['cutflow'])
            for i,label in enumerate(d['cutflow_labels'],1):
                cuts.GetXaxis().SetBinLabel(i,label)
            cuts.Write()
            profiles(role,sample['profiles'])
            columns = ['run','stored','base','selected']+[f'n_eta_positive_j{j}' for j in (1,2)]+[f'n_eta_negative_j{j}' for j in (1,2)]
            columns += [f'{stat}_{i}' for stat in ['count','sum','sum2'] for i in range(5)]
            rows = [[r['run'],r['stored'],r['base'],r['selected'],r['signs'][0],r['signs'][2],r['signs'][1],r['signs'][3]]+
                    [value for stat in ['count','sum','sum2'] for value in r['moments'][stat]] for r in sample['runs']]
            tree('raw_'+role+'_runs',columns,rows)
            tree('raw_'+role+'_lumisections',['run','ls','stored','base','selected','hash_sum','hash_xor'],sample['lumisections'])
            tree('raw_'+role+'_certification_accounting',['input_entries','retained_entries','rejected_entries','certification_rejected','pass_json_rejected'],
                 [[sample.get('input_entries',sample['cutflow'][0]),sample['cutflow'][0],
                   sample.get('certification_rejected',0)+sample.get('pass_json_rejected',0),
                   sample.get('certification_rejected',0),sample.get('pass_json_rejected',0)]])
            tree('raw_'+role+'_histogram_accounting',['histogram','entries','nonfinite','unavailable'],
                 [[name,sum(h['counts']),h['invalid'],h.get('unavailable',0)] for name,h in sample['histograms'].items()])
            tree('raw_'+role+'_threshold_counts',['threshold_gev','count'],list(zip(d['thresholds'],sample['threshold_counts'])))
        paired = d.get('paired')
        qcd = d.get('qcd')
        if qcd:
            for name, spec in qcd['histograms'].items():
                histogram('raw_qcd_'+name, spec['edges'], spec['sumw'], spec['label'],
                          [math.sqrt(value) for value in spec['sumw2']]).Write()
        if paired:
            migration = ROOT.TH2D('raw_matched_selection_migration','',2,-.5,1.5,2,-.5,1.5)
            migration.SetDirectory(0)
            for old in range(2):
                for new in range(2):migration.SetBinContent(new+1,old+1,paired['migration'][2*old+new])
            for axis in (migration.GetXaxis(),migration.GetYaxis()):
                axis.SetBinLabel(1,'Fail')
                axis.SetBinLabel(2,'Pass')
            migration.GetXaxis().SetTitle('New JEC')
            migration.GetYaxis().SetTitle('Old JEC')
            migration.Write()
            for name,spec in paired['shifts'].items():
                histogram('raw_matched_'+name,spec['edges'],spec['counts'],spec['label']).Write()
            for name,counts in paired['overlap'].items():
                histogram('raw_overlap_'+name,d['samples']['old']['histograms'][name]['edges'],counts).Write()
            profiles('matched',paired['profiles'])


    def profiles(role, specs):
        for name,spec in specs.items():
            for stat,values in spec['moments'].items():
                key = 'raw_'+role+'_profile_'+name+'_'+stat
                if not spec['yedges']:
                    histogram(key,spec['xedges'],values,spec['label'],[0]*len(values)).Write()
                else:
                    nx,ny = len(spec['xedges'])-1,len(spec['yedges'])-1
                    h = ROOT.TH2D(key,'',nx,array('d',spec['xedges']),ny,array('d',spec['yedges']))
                    h.SetDirectory(0)
                    for ix in range(nx+2):
                        for iy in range(ny+2):
                            h.SetBinContent(ix,iy,values[ix*(ny+2)+iy])
                    h.GetXaxis().SetTitle(latex(spec['label']))
                    h.GetYaxis().SetTitle(latex(spec['ylabel']))
                    h.Write()


    def draw_canvas(spec):
        name = 'canvas_'+spec['name']
        canvas = ROOT.TCanvas(name,'',spec['size'][0],spec['size'][1])
        keep = [canvas]
        axes = spec['axes']
        for i,ax in enumerate(axes):
            bx,by,bw,bh = ax['bounds']
            vertical = [a for a in axes if abs(a['bounds'][0]-bx)<1e-5 and abs(a['bounds'][2]-bw)<1e-5]
            above = [a['bounds'][1] for a in vertical if a['bounds'][1]>by]
            below = [a['bounds'][1]+a['bounds'][3] for a in vertical if a['bounds'][1]<by]
            py0 = (max(below)+by)/2 if below else 0
            py1 = (min(above)+by+bh)/2 if above else 1
            canvas.cd()
            pad = ROOT.TPad(name+'_pad'+str(i),'',0,py0,1,py1)
            keep.append(pad)
            pad.SetLeftMargin(bx)
            pad.SetRightMargin(1-bx-bw)
            pad.SetBottomMargin((by-py0)/(py1-py0))
            pad.SetTopMargin((py1-by-bh)/(py1-py0))
            pad.SetTicks(1,1)
            pad.SetLogx(ax['logx'])
            pad.SetLogy(ax['logy'])
            pad.Draw()
            pad.cd()
            frame = pad.DrawFrame(ax['xlim'][0],ax['ylim'][0],ax['xlim'][1],ax['ylim'][1])
            frame.SetName('frame_'+spec['name']+'_'+str(i))
            frame.SetTitle('')
            custom_x = [(x,label) for x,label in ax.get('custom_xticks',[]) if not label.startswith('$')]
            custom_y = [(y,label) for y,label in ax.get('custom_yticks',[]) if not label.startswith('$')]
            for axis,title,ticks in [(frame.GetXaxis(),ax['xlabel'],ax.get('xticks',[])),(frame.GetYaxis(),ax['ylabel'],ax.get('yticks',[]))]:
                axis.SetTitle(latex(title))
                axis.SetTitleFont(43)
                axis.SetLabelFont(43)
                axis.SetTitleSize(14 if bh<.25 else 18)
                axis.SetLabelSize(15)
                axis.SetTitleOffset(1.25)
                axis.SetNdivisions(500+max(2,min(10,len(ticks))))
            frame.GetYaxis().SetTitleOffset(1.8)
            if '\\frac' in ax['ylabel']:
                frame.GetYaxis().SetTitleSize(24)
            if not ax['show_xlabels']:
                frame.GetXaxis().SetLabelSize(0)
            if custom_x:
                frame.GetXaxis().SetLabelSize(0)
            if custom_y:
                frame.GetYaxis().SetLabelSize(0)
            keep.append(frame)
            labelled = {}
            for k,polygon in enumerate(ax['polygons']):
                if not polygon['xy']:
                    continue
                graph = ROOT.TGraph(len(polygon['xy']))
                for j,(x,y) in enumerate(polygon['xy']):graph.SetPoint(j,x,y)
                graph.SetFillColor(color(polygon['color']))
                graph.SetLineWidth(0)
                graph.Draw('F SAME')
                keep.append(graph)
                if polygon.get('label'):
                    labelled[polygon['label']] = (graph, 'f')
            for k,mesh in enumerate(ax['meshes']):
                h = ROOT.TH2D('draw_'+spec['name']+'_mesh'+str(i)+'_'+str(k),'',len(mesh['xedges'])-1,array('d',mesh['xedges']),
                              len(mesh['yedges'])-1,array('d',mesh['yedges']))
                h.SetDirectory(0)
                for iy,row in enumerate(mesh['values'],1):
                    for ix,value in enumerate(row,1):
                        if value is not None and math.isfinite(value):h.SetBinContent(ix,iy,value)
                h.Draw('COL SAME')
                h.Write()
                keep.append(h)
            for k,hist in enumerate(ax['histograms']):
                h = histogram('draw_'+spec['name']+'_hist'+str(i)+'_'+str(k),hist['edges'],hist['values'],ax['xlabel'],[0]*len(hist['values']))
                h.SetLineColor(color(hist['color']))
                h.SetLineWidth(2)
                h.Draw('HIST SAME')
                h.Write()
                keep.append(h)
                labelled[hist['label']] = (h,'l')
            for k,line in enumerate(ax['graphs']):
                if not line['points']:
                    continue
                has_errors = any(ex or ey for x,y,ex,ey in line['points'])
                graph = ROOT.TGraphAsymmErrors(len(line['points'])) if has_errors else ROOT.TGraph(len(line['points']))
                graph.SetName('draw_'+spec['name']+'_graph'+str(i)+'_'+str(k))
                for j,(x,y,ex,ey) in enumerate(line['points']):
                    graph.SetPoint(j,x,y)
                    if has_errors:
                        graph.SetPointError(j,ex,ex,ey,ey)
                graph.SetLineColor(color(line['color']))
                graph.SetMarkerColor(color(line['color']))
                marked = line['marker'] not in ('None','none','',' ')
                graph.SetMarkerStyle((24 if line['open'] else 20) if marked else 0)
                graph.SetMarkerSize(1.0)
                lined = line['line'] not in ('None','none','',' ')
                graph.SetLineStyle(2 if line['line']=='--' else 3 if line['line']==':' else 1)
                graph.SetLineWidth(1)
                option = 'LP' if marked and lined else 'P' if marked or not lined else 'L X'
                graph.Draw(option+' SAME')
                graph.Write()
                keep.append(graph)
                label = line.get('label') or ''
                if label and not label.startswith('_'):labelled[label] = (graph,'p' if marked else 'l')
            for text in ax['texts']:
                t = ROOT.TLatex(bx+text['x']*bw,(by-py0+text['y']*bh)/(py1-py0),latex(text['text']))
                t.SetNDC(True)
                t.SetTextFont(63 if text['bold'] else 53 if text['italic'] else 43)
                t.SetTextSize(text['size']*1.35)
                t.SetTextColor(color(text['color']))
                t.SetTextAlign({'left':10,'center':20,'right':30}.get(text['ha'],10)+{'bottom':1,'baseline':1,'center':2,'top':3}.get(text['va'],1))
                t.Draw()
                keep.append(t)
            for x,label in custom_x:
                fraction = (x-ax['xlim'][0])/(ax['xlim'][1]-ax['xlim'][0])
                t = ROOT.TLatex(bx+fraction*bw,(by-py0)/(py1-py0)-.04,latex(label))
                t.SetNDC(True)
                t.SetTextFont(43)
                t.SetTextSize(12)
                t.SetTextAlign(23)
                t.Draw()
                keep.append(t)
            for fraction,label in custom_y:
                t = ROOT.TLatex(bx-.02,(by-py0+fraction*bh)/(py1-py0),latex(label))
                t.SetNDC(True)
                t.SetTextFont(43)
                t.SetTextSize(12)
                t.SetTextAlign(32)
                t.Draw()
                keep.append(t)
            if ax['legend']:
                x0,y0,x1,y1 = ax['legend']['bounds']
                legend = ROOT.TLegend(bx+x0*bw,(by-py0+y0*bh)/(py1-py0),bx+x1*bw,(by-py0+y1*bh)/(py1-py0))
                legend.SetBorderSize(0)
                legend.SetFillStyle(0)
                legend.SetTextFont(43)
                legend.SetTextSize(14)
                for label in ax['legend']['labels']:
                    if label in labelled:
                        obj,option = labelled[label]
                        legend.AddEntry(obj,latex(label),option)
                legend.Draw()
                keep.append(legend)
            pad.RedrawAxis()
        canvas.Modified()
        canvas.Update()
        for obj in keep:
            ROOT.SetOwnership(obj,False)
        canvas.Write()
        canvas.Close()


    def export(payload, output):
        with open(payload) as stream:
            data = json.load(stream)
        destination = ROOT.TFile.Open(str(output),'RECREATE')
        if not destination or destination.IsZombie():raise OSError('Cannot create '+str(output))
        directory = destination.mkdir(data['directory'])
        directory.cd()
        ROOT.TObjString(json.dumps(data['summary'],separators=(',',':'))).Write('source_summary_json')
        ROOT.TObjString(json.dumps(data['report'],separators=(',',':'))).Write('report_json')
        raw_objects(data['summary'])
        for name,table_data in data['tables'].items():
            tree('table_'+name,table_data['columns'],table_data['rows'])
        for spec in data['canvases']:
            draw_canvas(spec)
        destination.Close()


    def main():
        parser = argparse.ArgumentParser()
        parser.add_argument('--root-payload')
        parser.add_argument('--root-merge',nargs='+')
        parser.add_argument('--output',required=True)
        args = parser.parse_args(arguments)
        target = Path(args.output).resolve()
        target.parent.mkdir(parents=True,exist_ok=True)
        temporary = target.with_name('.'+target.name+'.'+uuid.uuid4().hex+'.tmp')
        try:
            if args.root_merge:
                merger = ROOT.TFileMerger(False,False)
                merger.OutputFile(str(temporary),'RECREATE')
                for path in args.root_merge:
                    if not merger.AddFile(path):raise OSError('Cannot merge '+path)
                if not merger.Merge():raise OSError('ROOT merge failed')
                merger.Reset()
            else:
                export(args.root_payload,temporary)
            os.replace(temporary,target)
        finally:
            if temporary.exists():temporary.unlink()


    main()


if __name__ == "__main__" and any(flag in sys.argv for flag in ("--root-payload", "--root-merge")):
    try:
        root_output_main(sys.argv[1:])
    except Exception as exc:
        print("ROOT export error: "+str(exc), file=sys.stderr, flush=True)
        os._exit(1)
    sys.exit(0)


def progress(message, dataset=None):
    label = f" [{dataset}]" if dataset else ""
    print(f"[{time.strftime('%H:%M:%S')}]{label} {message}", flush=True)


if __name__ == "__main__":
    progress("Initializing NumPy and Matplotlib")

with warnings.catch_warnings():
    warnings.filterwarnings("ignore", category=UserWarning,
                            message=r"^The value of the smallest subnormal for <class 'numpy\.float(?:32|64)'> type is zero\.$",
                            module=r"^numpy\.(?:_core|core)\.getlimits$")
    import numpy as np

    np.finfo(np.float32)
    np.finfo(np.float64)

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.colors import TwoSlopeNorm, to_rgba
from matplotlib.container import ErrorbarContainer
from matplotlib.collections import PolyCollection
from matplotlib.lines import Line2D
from matplotlib.patches import StepPatch
from matplotlib.ticker import AutoMinorLocator, MaxNLocator


COLORS = {"old": "#000000", "new": "#cc0000", "qcd": "#497da8"}
FRACTIONAL_CHANGE_LABEL = "(New - old) / old"
ETA_ASYMMETRY_LABEL = r"$\frac{N_{(\eta)}-N_{(-\eta)}}{N_{(\eta)}+N_{(-\eta)}}$"
DEFAULT_NORMTAG = Path("/cvmfs/cms-bril.cern.ch/cms-lumi-pog/Normtags/normtag_PHYSICS.json")
DIJET_MASS_BINS = (1, 3, 6, 10, 16, 23, 31, 40, 50, 61, 74, 88, 103, 119, 137, 156, 176, 197, 220,
                   244, 270, 296, 325, 354, 386, 419, 453, 489, 526, 565, 606, 649, 693, 740, 788,
                   838, 890, 944, 1000, 1058, 1118, 1181, 1246, 1313, 1383, 1455, 1530, 1607, 1687,
                   1770, 1856, 1945, 2037, 2132, 2231, 2332, 2438, 2546, 2659, 2775, 2895, 3019,
                   3147, 3279, 3416, 3558, 3704, 3854, 4010, 4171, 4337, 4509, 4686, 4869, 5058,
                   5253, 5455, 5663, 5877, 6099, 6328, 6564, 6808, 7060, 7320, 7589, 7866, 8152,
                   8447, 8752, 9067, 9391, 9726, 10072, 10430, 10798, 11179, 11571, 11977, 12395,
                   12827, 13272, 13732, 14000)
HT_BIN_EDGES = {"ht": DIJET_MASS_BINS, "ht_50gev": tuple(range(0, 14001, 50))}
UNIFORM_PT_BINS = tuple(range(0, 7001, 100))
UNIFORM_PT_HISTOGRAMS = tuple(f"{name}_j{j}" for name in ("pt_wj", "raw_pt", "pt_positive", "pt_negative") for j in (1, 2))
CORE_SPECTRA = ("mjj_base", "mjj", "ak4_mjj", "pt_wj_j1", "pt_wj_j2", "eta_wj_j1", "eta_wj_j2",
                "phi_wj_j1", "phi_wj_j2", "delta_eta", "delta_phi", "jec_j1", "jec_j2",
                "pt_asymmetry", "ht", "ht_50gev", "nvtx", "raw_pt_j1", "raw_pt_j2")
CORE_LABELS = {"mjj_base": "Dijet mass [GeV]", "mjj": "Dijet mass [GeV]",
               "ak4_mjj": r"$m_{jj}^{AK4}$ [GeV]", "delta_eta": r"$|\Delta\eta_{jj}|$",
               "delta_phi": r"$|\Delta\phi_{jj}|$", "pt_asymmetry": r"$(p_{T1}-p_{T2})/(p_{T1}+p_{T2})$",
               "ht": r"$H_T$ [GeV]", "ht_50gev": r"$H_T$ [GeV]", "nvtx": "Vertices"}
for index, position in ((1, "Leading"), (2, "Subleading")):
    CORE_LABELS[f"pt_wj_j{index}"] = position + r" jet $p_T$ [GeV]"
    CORE_LABELS[f"eta_wj_j{index}"] = position + r" jet $\eta$"
    CORE_LABELS[f"phi_wj_j{index}"] = position + r" jet $\phi$"
    CORE_LABELS[f"jec_j{index}"] = r"AK4 JEC factor $C_{\mathrm{JEC}}$"
    CORE_LABELS[f"raw_pt_j{index}"] = r"AK4 $p_T / C_{\mathrm{JEC}}$ [GeV]"


def parser():
    p = argparse.ArgumentParser(description="Plot compact C++ JEC summaries; original ntuples are never reread.")
    p.add_argument("--inputs", nargs="+", required=True, help="Summary JSON files, directories or quoted globs.")
    p.add_argument("--mc-input", help="One reusable QCD summary from reduce_jec_comparison --mc-list; shared by both JECs and every era.")
    p.add_argument("--mc-label", default="QCD MC")
    p.add_argument("--mc-scale", type=float, default=1.0, help="Explicit common multiplicative QCD scale; default 1 uses the supplied cross sections.")
    p.add_argument("--datasets", nargs="+", help="Dataset labels to include; default: all supplied datasets.")
    p.add_argument("--output-dir", default="jec_comparison_plots")
    p.add_argument("--lumi-csv", action="append", default=[], help="run,lumi_pb or run,ls,lumi_pb CSV; also accepts brilcalc CSV.")
    p.add_argument("--lumi-json", action="append", default=[], help="Recorded run/LS luminosity JSON saved by --brilcalc.")
    p.add_argument("--brilcalc", action="store_true", help="Force a calibrated recorded-luminosity query, including when --lumi_pb supplies an era total; saves reusable recorded_luminosity.json.")
    p.add_argument("--no-auto-lumi", action="store_true", help="Disable automatic BRIL queries and recorded-luminosity cache reuse.")
    p.add_argument("--brilcalc-bin", default="brilcalc", help="Path to the brilcalc executable.")
    p.add_argument("--brilcalc-connection", default="web", help="BRIL connection passed with -c; default: web for access outside CERN.")
    p.add_argument("--brilcalc-work-dir", help="Keep BRIL input masks, CSVs and logs here, including on failure; default: OUTPUT_DIR/brilcalc.")
    p.add_argument("--normtag", help="Official luminosity normtag JSON; default: the CMS CVMFS normtag_PHYSICS.json.")
    p.add_argument("--lumi-unit", choices=["auto", "pb", "fb", "nb", "ub"], default="auto")
    p.add_argument("--lumi_pb", "--lumi-pb", nargs="+", action="append", default=[], metavar="VALUE",
                   help="Recorded luminosity in pb^-1: total, DATASET:TOTAL, or RUN:LUMI values. Era totals normalize mass spectra and do not disable BRIL run queries.")
    p.add_argument("--estimate-run-lumi", action="store_true", help="Estimate run luminosity from total exposure times each run's fraction of distinct stored LS. Assumes equal luminosity per LS.")
    p.add_argument("--no-estimate-run-lumi", action="store_true", help="Disable LS estimates (the default); retained for command compatibility.")
    p.add_argument("--export-lumisections", metavar="JSON", help="Export the stored run/LS coverage for brilcalc without rereading ntuples.")
    p.add_argument("--cert-json", metavar="JSON", help="Certification for BRIL/exported exposure. Histograms must already use this event coverage; apply masks with the reducer's --cert-json. Certified summaries provide their mask automatically.")
    p.add_argument("--export-only", action="store_true", help="Only export run/LS coverage; do not generate figures or ROOT output.")
    p.add_argument("--root-file", help="Single ROOT output for all dataset plot sets; default: OUTPUT_DIR/jec_comparison.root.")
    p.add_argument("--root-python", default=sys.executable, help="Python interpreter with PyROOT; default: the plotting interpreter.")
    p.add_argument("--normalization", choices=["yield", "shape", "xsec"], default="yield")
    p.add_argument("--old-label", default="Old JEC")
    p.add_argument("--new-label", default="New JEC")
    p.add_argument("--energy-tev", type=float, default=13.6)
    p.add_argument("--status", default="Preliminary")
    p.add_argument("--formats", nargs="+", choices=["pdf", "png", "svg"], default=["pdf", "png"])
    p.add_argument("--dpi", type=int, default=180)
    p.add_argument("--jobs", type=int, default=min(4, os.cpu_count() or 1), help="Parallel dataset plot jobs.")
    p.add_argument("--per-dataset", action="store_true", help="Also plot individual datasets when combining several.")
    p.add_argument("--min-count", type=int, default=20, help="Minimum entries per profile bin and run point.")
    p.add_argument("--ratio-min-count", type=int, default=20,
                   help="Minimum old count for displayed fractional-change points; ROOT tables retain all bins.")
    p.add_argument("--allow-partial", action="store_true", help="Allow debug summaries made with --max-entries; disables cross sections.")
    p.add_argument("--plots", nargs="+", choices=["spectra", "xsec", "run", "eta", "profiles", "maps", "paired", "selection"],
                   help="Optional plot groups; default: spectra, xsec, run, eta and selection.")
    p.add_argument("--all-spectra", action="store_true", help="Plot every saved histogram, beyond the core comparison set.")
    p.add_argument("--spectra", nargs="+", help="Plot only these saved histogram names when spectra are requested.")
    p.add_argument("--mass-range", nargs=2, type=float, metavar=("MIN", "MAX"),
                   help="Displayed mass range in TeV; default: 0 to 8 with cuts, 0 to 12 for all/all_stored. Counts and bins are unchanged.")
    p.add_argument("--ht-range", nargs=2, type=float, metavar=("MIN", "MAX"),
                   help="Displayed range for both HT spectra in TeV; default: the full 0 to 14 range. Counts and bins are unchanged.")
    p.add_argument("--require-dijet-binning", action="store_true",
                   help="Reject summaries that do not use the standard dijet mass bins; rerun the updated reducer first.")
    return p


def expand_inputs(values):
    paths = []
    for value in values:
        if Path(value).is_dir():
            found = sorted(Path(value).glob("*.json"))
        else:
            found = [Path(x) for x in sorted(glob.glob(value))]
        if not found:
            raise ValueError(f"No summaries match {value}")
        paths.extend(x.resolve() for x in found)
    if len(paths) != len(set(paths)):
        raise ValueError("Repeated summary paths")
    return paths


def sum_arrays(a, b):
    if len(a) != len(b):
        raise ValueError("Incompatible summary array sizes")
    return [x + y for x, y in zip(a, b)]


def merge_moments(a, b):
    for name in ("count", "sum", "sum2"):
        a[name] = sum_arrays(a[name], b[name])


def merge_hists(a, b):
    if a.keys() != b.keys():
        raise ValueError("Incompatible histogram sets")
    for name in a:
        if a[name]["edges"] != b[name]["edges"]:
            raise ValueError(f"Incompatible bins: {name}")
        a[name]["counts"] = sum_arrays(a[name]["counts"], b[name]["counts"])
        a[name]["invalid"] += b[name]["invalid"]
        a[name]["unavailable"] = a[name].get("unavailable", 0) + b[name].get("unavailable", 0)


def merge_profiles(a, b):
    if a.keys() != b.keys():
        raise ValueError("Incompatible profile sets")
    for name in a:
        if a[name]["xedges"] != b[name]["xedges"] or a[name]["yedges"] != b[name]["yedges"]:
            raise ValueError(f"Incompatible profile bins: {name}")
        merge_moments(a[name]["moments"], b[name]["moments"])


def merge_sample(a, b):
    for name in ("input_entries", "certification_rejected", "pass_json_rejected"):
        if name in a or name in b:
            fallback_a, fallback_b = (a["cutflow"][0], b["cutflow"][0]) if name == "input_entries" else (0, 0)
            a[name] = a.get(name, fallback_a)+b.get(name, fallback_b)
    if "valid_dijets" in a or "valid_dijets" in b:
        if "valid_dijets" not in a or "valid_dijets" not in b:
            raise ValueError("Incompatible validity accounting")
        a["valid_dijets"] += b["valid_dijets"]
    a["cutflow"] = sum_arrays(a["cutflow"], b["cutflow"])
    a["threshold_counts"] = sum_arrays(a["threshold_counts"], b["threshold_counts"])
    merge_hists(a["histograms"], b["histograms"])
    merge_profiles(a["profiles"], b["profiles"])
    runs = {x["run"]: x for x in a["runs"]}
    for r in b["runs"]:
        if r["run"] not in runs:
            runs[r["run"]] = copy.deepcopy(r)
        else:
            t = runs[r["run"]]
            for name in ("stored", "base", "selected"):
                t[name] += r[name]
            t["signs"] = sum_arrays(t["signs"], r["signs"])
            if "eta_zero" in t or "eta_zero" in r:
                if "eta_zero" not in t or "eta_zero" not in r:
                    raise ValueError("Incompatible eta-zero accounting")
                t["eta_zero"] = sum_arrays(t["eta_zero"], r["eta_zero"])
            merge_moments(t["moments"], r["moments"])
    a["runs"] = [runs[k] for k in sorted(runs)]
    lumis = {(x[0], x[1]): x for x in a["lumisections"]}
    for row in b["lumisections"]:
        k = tuple(row[:2])
        if k not in lumis:
            lumis[k] = copy.deepcopy(row)
        else:
            t = lumis[k]
            for i in (2, 3, 4):
                t[i] += row[i]
            t[5] = f"{(int(t[5], 16) + int(row[5], 16)) & ((1 << 64) - 1):016x}"
            t[6] = f"{int(t[6], 16) ^ int(row[6], 16):016x}"
    a["lumisections"] = [lumis[k] for k in sorted(lumis)]


def merge_paired(a, b):
    for name in ("matched", "only_old", "only_new", "jet_matched", "jet_unmatched", "reordered"):
        a[name] += b[name]
    a["migration"] = sum_arrays(a["migration"], b["migration"])
    a["threshold_migration"] = [sum_arrays(x, y) for x, y in zip(a["threshold_migration"], b["threshold_migration"])]
    merge_hists(a["shifts"], b["shifts"])
    merge_profiles(a["profiles"], b["profiles"])
    for name in a["overlap"]:
        a["overlap"][name] = sum_arrays(a["overlap"][name], b["overlap"][name])
    runs = {r[0]: r for r in a["runs"]}
    for r in b["runs"]:
        if r[0] in runs:
            runs[r[0]][1:] = sum_arrays(runs[r[0]][1:], r[1:])
        else:
            runs[r[0]] = copy.deepcopy(r)
    a["runs"] = [runs[k] for k in sorted(runs)]
    shifts = {r["run"]: r for r in a["run_shifts"]}
    for r in b["run_shifts"]:
        if r["run"] in shifts:
            merge_moments(shifts[r["run"]]["moments"], r["moments"])
        else:
            shifts[r["run"]] = copy.deepcopy(r)
    a["run_shifts"] = [shifts[k] for k in sorted(shifts)]


def merge_document(a, b, combine_certification=False):
    if bool(a.get("mock")) != bool(b.get("mock")):
        raise ValueError("Cannot combine mock demonstrations and analysis summaries")
    for name in ("schema_version", "tree", "thresholds", "cutflow_labels"):
        if a[name] != b[name]:
            raise ValueError(f"Cannot combine summaries with different {name}")
    if a["config"] != b["config"]:
        masks = [d["config"].get("certification_mask") for d in (a, b)]
        configurations = [{k: v for k, v in d["config"].items() if k != "certification_mask"} for d in (a, b)]
        if not combine_certification or not all(masks) or configurations[0] != configurations[1]:
            raise ValueError("Cannot combine summaries with different config")
        a["config"]["certification_mask"] = merge_certification_masks(masks)
    for role, sample in b["samples"].items():
        if role not in a["samples"]:
            a["samples"][role] = copy.deepcopy(sample)
        else:
            merge_sample(a["samples"][role], sample)
    for role, rows in b["files"].items():
        a["files"].setdefault(role, []).extend(copy.deepcopy(rows))
    if "paired" in b:
        if "paired" in a:
            merge_paired(a["paired"], b["paired"])
        else:
            a["paired"] = copy.deepcopy(b["paired"])
    a["partial"] = a["partial"] or b["partial"]
    a["processed_entries"] += b["processed_entries"]
    a["elapsed_seconds"] += b["elapsed_seconds"]
    if b.get("qcd") is not None:
        a["qcd"] = shared_qcd(a.get("qcd"), b["qcd"])


def shared_qcd(a, b):
    if a is None:
        return copy.deepcopy(b)
    def identity(reference):
        result = copy.deepcopy(reference)
        for component in result["components"]:
            component.pop("source", None)
            component["files"] = sorted(component["files"], key=lambda row: row["path"])
        result["components"].sort(key=lambda component: tuple(row["path"] for row in component["files"]))
        return result
    if identity(a) != identity(b):
        raise ValueError("Different QCD references supplied; reduce the complete MC component list once and reuse that summary for every era")
    return a


def validate_qcd(qcd, d):
    if qcd.get("partial") or qcd.get("normalization") != "cross_section_pb" or qcd.get("weight_convention") != "unit":
        raise ValueError("QCD requires a complete reduction with cross-section normalization and unit generator weights")
    config = {key: value for key, value in d["config"].items() if key not in ("certification_mask", "require_pass_json")}
    if qcd.get("config") != config or qcd.get("tree") != d["tree"]:
        raise ValueError(f"{d['dataset']}: QCD and data must use the same tree, preset and kinematic cuts")
    if not qcd.get("components"):
        raise ValueError("QCD summary contains no components")
    files = set()
    for component in qcd["components"]:
        for key in ("xsec_pb", "generated_events", "weight_pb", "filter_efficiency"):
            if not math.isfinite(component[key]) or component[key] <= 0:
                raise ValueError(f"Invalid QCD normalization: {key}")
        expected = component["xsec_pb"]*component["filter_efficiency"]/component["generated_events"]
        if not math.isclose(expected, component["weight_pb"], rel_tol=1e-10):
            raise ValueError("QCD component weight disagrees with its cross section and generated-event count")
        for row in component["files"]:
            if row["path"] in files:
                raise ValueError("QCD input ROOT file counted twice: "+row["path"])
            files.add(row["path"])
    for role in ("old", "new"):
        for name, hist in d["samples"][role]["histograms"].items():
            reference = qcd["histograms"].get(name)
            if reference is None or reference["edges"] != hist["edges"]:
                raise ValueError(f"{d['dataset']}: incompatible QCD histogram bins for {name}")
            for stat in ("sumw", "sumw2"):
                if len(reference[stat]) != len(hist["counts"]) or any(not math.isfinite(x) or x < 0 for x in reference[stat]):
                    raise ValueError(f"Invalid QCD {stat} for {name}")


def load_datasets(paths, requested, allow_partial, mc_input=None):
    datasets = {}
    seen = set()
    paired_presence = {}
    qcd = None
    if mc_input:
        with Path(mc_input).open() as stream:
            document = json.load(stream)
        if document.get("schema_version") != 1 or not document.get("qcd"):
            raise ValueError("--mc-input must be a reduction summary containing QCD")
        qcd = document["qcd"]
    for index, path in enumerate(paths, 1):
        progress(f"Loading summary {index}/{len(paths)}: {path}")
        with path.open() as stream:
            d = json.load(stream)
        if d.get("schema_version") != 1 or "samples" not in d:
            raise ValueError(f"Unsupported reduction summary: {path}")
        if not d["samples"] and d.get("qcd"):
            qcd = shared_qcd(qcd, d["qcd"])
            continue
        name = d["dataset"]
        if requested and name not in requested:
            progress("Skipped: dataset was not requested", name)
            continue
        if d.get("qcd"):
            qcd = shared_qcd(qcd, d["qcd"])
        if d["partial"] and not allow_partial:
            raise ValueError(f"{path} is a partial debug scan; use full summaries or --allow-partial")
        paired_presence.setdefault(name, set()).add("paired" in d)
        for role, files in d["files"].items():
            if role not in ("old", "new"):
                raise ValueError(f"Unsupported JEC role in {path}")
            for row in files:
                k = (role, row["path"])
                if k in seen:
                    raise ValueError(f"Input ROOT file counted twice for {role}: {row['path']}")
                seen.add(k)
        if name in datasets:
            merge_document(datasets[name], d)
        else:
            datasets[name] = d
    if not datasets:
        raise ValueError("No selected datasets")
    if requested and set(requested) - datasets.keys():
        raise ValueError(f"Missing datasets: {sorted(set(requested) - datasets.keys())}")
    for name, d in datasets.items():
        if set(d["samples"]) != {"old", "new"}:
            raise ValueError(f"Dataset {name} needs both old and new summaries")
        if len(paired_presence[name]) != 1:
            raise ValueError(f"Dataset {name} mixes paired and independent reductions; correlated errors would be incomplete")
        if qcd is not None:
            validate_qcd(qcd, d)
            d["qcd"] = qcd
        certified = certification_sections(d["config"].get("certification_mask"))
        for role, sample in d["samples"].items():
            read = sample.get("input_entries", sample["cutflow"][0])
            rejected = sample.get("certification_rejected", 0)
            json_rejected = sample.get("pass_json_rejected", 0)
            if any(not isinstance(value, int) or value < 0 for value in (read, rejected, json_rejected)) or read != sample["cutflow"][0]+rejected+json_rejected:
                raise ValueError(f"{name}/{role}: inconsistent event-filter accounting")
            if certified is None and rejected:
                raise ValueError(f"{name}/{role}: rejected certification events without a saved mask")
            if json_rejected and not d["config"].get("require_pass_json"):
                raise ValueError(f"{name}/{role}: rejected PassJSON events without an explicit filter")
            if certified is not None and any(row[2] and tuple(row[:2]) not in certified for row in sample["lumisections"]):
                raise ValueError(f"{name}/{role}: retained events outside the saved certification mask")
    return datasets


def load_luminosity(paths, unit):
    per_run, per_ls = {}, {}
    sources = []
    scale = {"pb": 1.0, "fb": 1000.0, "nb": 0.001, "ub": 0.000001}
    for path in paths:
        if Path(path).suffix.lower() == ".json":
            data = json.loads(Path(path).read_text())
            if data.get("unit") != "pb^-1":
                raise ValueError(f"{path}: recorded luminosity JSON must declare unit pb^-1")
            if unit not in ("auto", "pb"):
                raise ValueError(f"--lumi-unit {unit} conflicts with JSON unit pb^-1")
            rows = data.get("lumisections")
            if not isinstance(rows, list) or not rows:
                raise ValueError(f"{path}: expected recorded luminosity JSON with lumisections [run, LS, lumi_pb]")
            totals = {}
            for row in rows:
                if len(row) != 3:
                    raise ValueError(f"{path}: malformed luminosity row")
                run, ls, value = row
                if not isinstance(run, int) or not isinstance(ls, int) or run < 1 or ls < 1 or not isinstance(value, (int,float)) or not math.isfinite(value) or value < 0:
                    raise ValueError(f"{path}: invalid recorded luminosity row")
                if (run,ls) in per_ls:
                    raise ValueError(f"Duplicate luminosity row: {(run,ls)}")
                per_ls[(run,ls)] = float(value)
                totals[run] = totals.get(run,0)+value
            if data.get("runs") and any(run not in totals or not math.isclose(value,totals[run],rel_tol=1e-10,abs_tol=1e-10) for run,value in data["runs"]):
                raise ValueError(f"{path}: run luminosity totals disagree with LS rows")
            sources.append(data.get("provenance", {"input": str(Path(path).resolve())}))
            continue
        with open(path, encoding="utf-8-sig") as stream:
            lines, line_numbers = [], []
            for line_number, line in enumerate(stream, 1):
                line = line.strip()
                if not line:
                    continue
                if line.startswith("#"):
                    if line.lower().startswith("#run"):
                        line = line[1:]
                    else:
                        continue
                lines.append(line)
                line_numbers.append(line_number)
        if not lines:
            raise ValueError(f"Empty luminosity CSV: {path}")
        reader = csv.DictReader(lines)
        fields = [x.strip() for x in reader.fieldnames]
        reader.fieldnames = fields
        run_field = next((x for x in fields if x.lower() in ("run", "run:fill")), None)
        ls_field = next((x for x in fields if x.lower() in ("ls", "lumi", "lumisection", "luminosityblock")), None)
        lumi_field = next((x for x in fields if x.lower().startswith("recorded(")), None)
        if lumi_field is None:
            lumi_field = next((x for x in fields if x.lower() in ("lumi_pb", "lumi_fb", "lumi_nb", "lumi_ub", "recorded_lumi", "recorded")), None)
        if not run_field or not lumi_field:
            raise ValueError(f"{path}: expected run,lumi_pb, run,ls,lumi_pb, or brilcalc recorded luminosity")
        inferred = re.search(r"(?:lumi_|/)(pb|fb|nb|ub)", lumi_field.lower())
        inferred = inferred.group(1) if inferred else None
        if unit == "auto" and not inferred:
            raise ValueError(f"Cannot infer luminosity unit in {path}; supply --lumi-unit")
        if unit != "auto" and inferred and unit != inferred:
            raise ValueError(f"--lumi-unit {unit} conflicts with column {lumi_field}")
        factor = scale[inferred or unit]
        invalid = []
        for row in reader:
            line_number = line_numbers[reader.line_num-1]
            try:
                run = int(row[run_field].split(":")[0])
                value = float(row[lumi_field]) * factor
                ls = int(row[ls_field].split(":")[0]) if ls_field else None
            except (ValueError, AttributeError, TypeError) as exc:
                raise ValueError(f"Malformed luminosity row in {path}, line {line_number}: {row}") from exc
            if run < 1 or (ls is not None and ls < 1):
                raise ValueError(f"Invalid run/LS in {path}, line {line_number}: run={run}, LS={ls}")
            if not math.isfinite(value) or value < 0:
                context = ", ".join(f"{field}={row[field]!r}" for field in fields
                                    if field.lower().startswith("delivered(") or field.lower() in ("beamstatus", "source"))
                invalid.append(f"line {line_number}: run={run}, LS={ls}, {lumi_field}={row[lumi_field]!r}" +
                               (f", {context}" if context else ""))
                continue
            target, k = (per_ls, (run, ls)) if ls is not None else (per_run, run)
            if k in target:
                raise ValueError(f"Duplicate luminosity row: {k}; luminosity must be counted once")
            target[k] = value
        if invalid:
            details = "; ".join(invalid[:5])
            extra = f"; {len(invalid)-5} more invalid rows in the saved CSV" if len(invalid) > 5 else ""
            raise ValueError(f"Invalid recorded luminosity in {path}: {len(invalid)} row(s); {details}{extra}. "
                             "Recorded luminosity must be finite and nonnegative; these values were not replaced or used")
    ls_runs = {run for run, ls in per_ls}
    if ls_runs & per_run.keys():
        raise ValueError("Luminosity input mixes run totals and lumisection rows for the same run")
    for (run, ls), value in per_ls.items():
        per_run[run] = per_run.get(run, 0.0) + value
    return {"runs": per_run, "lumisections": per_ls, "sources": sources}


def add_luminosity_values(luminosity, values):
    tokens = [token for group in values for token in group]
    if not tokens:
        return luminosity
    runs, datasets, total = {}, {}, None
    for token in tokens:
        key, value = token.rsplit(":", 1) if ":" in token else (None, token)
        try:
            value = float(value)
        except ValueError as exc:
            raise ValueError(f"Invalid --lumi_pb value: {token}") from exc
        if not math.isfinite(value) or value <= 0:
            raise ValueError("--lumi_pb values must be finite and positive, in pb^-1")
        if key is None:
            if total is not None:
                raise ValueError("Supply one total luminosity, or label separate dataset/run values")
            total = value
        else:
            if not key:
                raise ValueError(f"Missing dataset or run in --lumi_pb {token}")
            target, name = (runs, int(key)) if key.isdigit() else (datasets, key)
            if name in target:
                raise ValueError(f"Duplicate --lumi_pb entry: {key}")
            target[name] = value
    if sum((bool(runs), bool(datasets), total is not None)) != 1:
        raise ValueError("Use one --lumi_pb mode: total, dataset totals, or per-run values")
    if datasets or total is not None:
        combined = copy.deepcopy(luminosity)
        combined.update({"datasets_pb": datasets, "total_pb": total})
        return combined
    combined = copy.deepcopy(luminosity)
    ls_runs = {run for run, ls in combined["lumisections"]}
    for run, value in runs.items():
        if run in combined["runs"] or run in ls_runs:
            raise ValueError(f"Luminosity counted twice for run {run}")
        combined["runs"][run] = value
    return combined


def moments(m):
    n = np.asarray(m["count"], dtype=float)
    total = np.asarray(m["sum"], dtype=float)
    total2 = np.asarray(m["sum2"], dtype=float)
    mean = np.divide(total, n, out=np.full_like(n, np.nan), where=n > 0)
    variance = np.divide(np.maximum(0, total2 - np.nan_to_num(total * mean)), n - 1,
                         out=np.full_like(n, np.nan), where=n > 1)
    sem = np.sqrt(np.divide(variance, n, out=np.full_like(n, np.nan), where=n > 1))
    return n, mean, sem


def stored_sections(d):
    return {tuple(row[:2]) for sample in d["samples"].values() for row in sample["lumisections"] if row[2]}


def certification_sections(path):
    if not path:
        return None
    if isinstance(path, dict):
        mask = path
    else:
        with open(path) as stream:
            mask = json.load(stream)
    return {(int(run), ls) for run, ranges in mask.items() for low, high in ranges for ls in range(low, high+1)}


def merge_certification_masks(masks):
    runs = {}
    for mask in masks:
        for run, ranges in mask.items():
            runs.setdefault(str(int(run)), []).extend(ranges)
    merged = {}
    for run, ranges in sorted(runs.items(), key=lambda item: int(item[0])):
        result = []
        for low, high in sorted(ranges):
            if result and low <= result[-1][1]+1:
                result[-1][1] = max(result[-1][1], high)
            else:
                result.append([low, high])
        merged[run] = result
    return merged


def configured_certification(args):
    return certification_sections(getattr(args, "certification_mask", None) or args.cert_json)


def export_sections(datasets, path, certified):
    sections = set().union(*(stored_sections(d) for d in datasets.values()))
    if certified is not None:
        sections &= certified
    result = {}
    for run, ls in sorted(sections):
        ranges = result.setdefault(str(run), [])
        if ranges and ranges[-1][1]+1 == ls:
            ranges[-1][1] = ls
        else:
            ranges.append([ls, ls])
    target = Path(path).resolve()
    target.parent.mkdir(parents=True, exist_ok=True)
    target.write_text(json.dumps(result, separators=(",", ":"))+"\n")
    progress(f"Exported {len(sections):,} distinct LS in {len(result)} runs to {target}")


def estimate_luminosity(datasets, luminosity, certified):
    if luminosity["runs"]:
        raise ValueError("--estimate-run-lumi requires dataset totals, rather than recorded per-run luminosity")
    if any(d["partial"] for d in datasets.values()):
        raise ValueError("Run luminosity estimates require complete summaries")
    totals = luminosity.get("datasets_pb", {})
    global_total = luminosity.get("total_pb")
    if not totals and global_total is None:
        raise ValueError("--estimate-run-lumi requires --lumi_pb")
    section_sets = {}
    for name, d in datasets.items():
        old = {tuple(row[:2]) for row in d["samples"]["old"]["lumisections"] if row[2]}
        new = {tuple(row[:2]) for row in d["samples"]["new"]["lumisections"] if row[2]}
        if old != new:
            raise ValueError(f"{name}: LS luminosity estimate requires identical old/new stored coverage")
        section_sets[name] = old if certified is None else old & certified
        if not section_sets[name]:
            raise ValueError(f"{name}: no stored LS available for luminosity estimation")
        excluded = old-certified if certified is not None else set()
        if excluded and any((row[3] or row[4]) and tuple(row[:2]) in excluded for sample in d["samples"].values() for row in sample["lumisections"]):
            raise ValueError(f"{name}: summaries contain uncertified LS; estimating only certified exposure would leave their events in the numerator. Use matching certified summaries or omit --cert-json for a rough full-sample diagnostic")
    groups = [(name, sections, totals.get(name)) for name, sections in section_sets.items()] if totals else [
        (", ".join(sorted(datasets)), set().union(*section_sets.values()), global_total)]
    result = {"runs": {}, "lumisections": {}, "estimated": True, "estimation": []}
    seen = set()
    for name, sections, total in groups:
        if total is None:
            raise ValueError(f"Missing --lumi_pb total for {name}")
        if seen & sections:
            raise ValueError("Dataset LS overlap in luminosity estimation")
        seen.update(sections)
        per_ls = total/len(sections)
        for run, ls in sections:
            result["runs"][run] = result["runs"].get(run, 0)+per_ls
            result["lumisections"][(run, ls)] = per_ls
        result["estimation"].append({"dataset": name, "lumi_pb": total, "n_ls": len(sections), "lumi_per_ls_pb": per_ls})
        progress(f"Estimated luminosity for {name}: {len(sections):,} LS, {per_ls:.9g} pb^-1 per LS; assumes uniform LS luminosity")
    return result


def fetch_bril_luminosity(datasets, args, certified):
    if not args.normtag or not Path(args.normtag).is_file():
        raise ValueError("--brilcalc requires --normtag pointing to the official readable normtag JSON")
    if any(d["partial"] for d in datasets.values()):
        raise ValueError("Recorded cross sections require complete summaries")
    for name,d in datasets.items():
        old = {tuple(row[:2]) for row in d["samples"]["old"]["lumisections"] if row[2]}
        new = {tuple(row[:2]) for row in d["samples"]["new"]["lumisections"] if row[2]}
        if old != new:
            raise ValueError(f"{name}: a shared BRIL query requires identical old/new stored LS coverage")
        if certified is not None and any(row[4] and tuple(row[:2]) not in certified for sample in d["samples"].values() for row in sample["lumisections"]):
            raise ValueError(f"{name}: certification excludes events still counted in these summaries. Regenerate them with reduce_jec_comparison --cert-json FILE; the plotter cannot filter already aggregated histograms")
    work_root = Path(args.brilcalc_work_dir or Path(args.output_dir)/"brilcalc").resolve()
    work_root.mkdir(parents=True, exist_ok=True)
    work = Path(tempfile.mkdtemp(prefix=time.strftime("query_%Y%m%d_%H%M%S_"), dir=str(work_root)))
    mask = work/"coverage.json"
    csv_path = work/"recorded.csv"
    log_path = work/"brilcalc.log"
    progress(f"BRIL query files will be retained in {work}")
    with tempfile.TemporaryDirectory(prefix="jec_bril_") as temporary:
        export_sections(datasets,mask,certified)
        command = [args.brilcalc_bin,"lumi","--byls","--precision","12f","-u","/pb","-b","STABLE BEAMS","-i",str(mask),
                   "--normtag",str(Path(args.normtag).resolve()),"--output-style","csv","-o",str(csv_path)]
        if args.brilcalc_connection:
            command.extend(["-c",args.brilcalc_connection])
        progress(f"Querying calibrated recorded luminosity with brilcalc; normtag={Path(args.normtag).resolve()}")
        def query(environment=None):
            with log_path.open("a") as log:
                log.write("Command: "+shlex.join(command)+"\n")
                log.flush()
                process = subprocess.Popen(command,stdout=log,stderr=subprocess.STDOUT,text=True,env=environment)
                started = time.monotonic()
                announced = started
                try:
                    while process.poll() is None:
                        now = time.monotonic()
                        if now-started > 300:
                            raise ValueError(f"brilcalc query exceeded 300 seconds; BRIL input and log retained in {work}")
                        if now-announced >= 20:
                            progress(f"brilcalc query in progress ({now-started:.0f} s)")
                            announced = now
                        time.sleep(0.2)
                except BaseException:
                    process.terminate()
                    process.wait()
                    raise
            return process.returncode
        status = query()
        compatible = Path("/cvmfs/unpacked.cern.ch/registry.hub.docker.com/cmssw/el8:x86_64/usr/lib64/libnsl.so.1")
        compatibility_library = None
        if status and "libnsl.so.1" in log_path.read_text() and "DPI-1047" in log_path.read_text() and compatible.is_file():
            (Path(temporary)/"libnsl.so.1").write_bytes(compatible.read_bytes())
            environment = os.environ.copy()
            environment["LD_LIBRARY_PATH"] = temporary+":"+environment.get("LD_LIBRARY_PATH","")
            compatibility_library = str(compatible)
            progress("Retrying BRIL with the CMS EL8 libnsl compatibility library in a temporary directory")
            status = query(environment)
        if status or not csv_path.is_file():
            raise ValueError(f"brilcalc query failed; input, CSV (if written) and log retained in {work}. "+log_path.read_text()[-3000:])
        try:
            luminosity = load_luminosity([csv_path],"pb")
        except (ValueError, OSError) as exc:
            raise ValueError(f"{exc}\nBRIL input, CSV and log retained in {work}; inspect {log_path}") from exc
        exposure = certification_sections(mask)
        if not luminosity["lumisections"] or set(luminosity["lumisections"])-exposure:
            raise ValueError(f"brilcalc did not return per-LS values matching the requested exposure; inspect {csv_path} and {log_path}")
        for d in datasets.values():
            for sample in d["samples"].values():
                for row in sample["lumisections"]:
                    if row[4] and (not math.isfinite(luminosity["runs"].get(row[0],math.nan)) or luminosity["runs"][row[0]] <= 0 or luminosity["lumisections"].get(tuple(row[:2]),0) <= 0):
                        raise ValueError(f"brilcalc returned no positive recorded luminosity for counted run/LS {row[:2]}; inspect {csv_path} and {log_path}")
        provenance = {"source": "brilcalc", "normtag": str(Path(args.normtag).resolve()),
                      "normtag_sha256": hashlib.sha256(Path(args.normtag).read_bytes()).hexdigest(),
                      "connection": args.brilcalc_connection or "configured default", "beam_status": "STABLE BEAMS",
                      "compatibility_library": compatibility_library,
                      "query_directory": str(work),
                      "requested_lumisections": json.loads(mask.read_text()),
                      "missing_lumisections": [list(key) for key in sorted(exposure-set(luminosity["lumisections"]))],
                      "query_log": log_path.read_text()}
        data = {"schema_version": 1,"unit": "pb^-1","provenance": provenance,
                "lumisections": [[run,ls,value] for (run,ls),value in sorted(luminosity["lumisections"].items())],
                "runs": [[run,value] for run,value in sorted(luminosity["runs"].items())],
                "total_lumi_pb": math.fsum(luminosity["runs"].values())}
        output = Path(args.output_dir).resolve()/"recorded_luminosity.json"
        output.parent.mkdir(parents=True,exist_ok=True)
        output.write_text(json.dumps(data,indent=2)+"\n")
        luminosity["sources"] = [provenance]
        progress(f"Recorded luminosity: {data['total_lumi_pb']:.9g} pb^-1 in {len(data['runs'])} runs; saved {output}")
        return luminosity


def configure_certification(datasets, args, automatic=False):
    if not args.cert_json:
        if getattr(args, "certification_mask", None):
            return True
        masks = [d["config"].get("certification_mask") for d in datasets.values()]
        if any(masks):
            if not all(masks):
                raise ValueError("Cannot combine summaries with and without an event certification mask")
            args.certification_mask = merge_certification_masks(masks)
            progress("Using the run/LS certification masks saved by the reducer for BRIL exposure")
            return True
    needs_certification = any(d["config"]["preset"] in ("dijet", "loose") for d in datasets.values())
    if needs_certification and not args.cert_json:
        years = {year for name in datasets for year in re.findall(r"20\d{2}", name)}
        paths = []
        for year in sorted(years):
            paths.extend(Path(__file__).resolve().parent.parent.joinpath("data", "json").glob(f"Cert_Collisions{year}_*_Golden.json"))
        if len(years) == 1 and len(paths) == 1:
            args.cert_json = str(paths[0])
            progress(f"Using certification for BRIL exposure: {args.cert_json}")
        else:
            message = "Selected-summary BRIL queries require --cert-json for the processed certification; a default could not be identified"
            if automatic:
                progress(message)
                return False
            raise ValueError(message)
    return True


def configure_bril(datasets, args, automatic=False):
    if not configure_certification(datasets, args, automatic):
        return False
    if not args.normtag:
        if DEFAULT_NORMTAG.is_file():
            args.normtag = str(DEFAULT_NORMTAG)
    if not args.normtag or not Path(args.normtag).is_file():
        message = f"Recorded cross sections need --normtag /path/to/official_physics_normtag.json; the default {DEFAULT_NORMTAG} is unavailable"
        if automatic:
            progress(message)
            return False
        raise ValueError(message)
    return True


def luminosity_cache_matches(path, datasets, args):
    if not configure_certification(datasets, args, automatic=True):
        return False
    data = json.loads(path.read_text())
    requested = Path(args.normtag) if args.normtag else DEFAULT_NORMTAG
    provenance = data.get("provenance", {})
    if requested.is_file():
        if provenance.get("normtag_sha256") != hashlib.sha256(requested.read_bytes()).hexdigest():
            progress("Recorded luminosity cache has a different normtag calibration; querying again")
            return False
    elif args.normtag or Path(provenance.get("normtag") or "").name != DEFAULT_NORMTAG.name:
        progress("Recorded luminosity cache does not match the requested physics normtag; querying again")
        return False
    mask = data.get("provenance", {}).get("requested_lumisections", {})
    exposure = {(int(run), ls) for run,ranges in mask.items() for low,high in ranges for ls in range(low,high+1)}
    expected = set().union(*(stored_sections(d) for d in datasets.values()))
    certified = configured_certification(args)
    if certified is not None:
        expected &= certified
    return bool(exposure) and exposure == expected


def ratio(a, b, overlap=None, variance_a=None, variance_b=None):
    a, b = np.asarray(a, dtype=float), np.asarray(b, dtype=float)
    r = np.divide(a, b, out=np.full_like(a, np.nan), where=b > 0)
    va = a if variance_a is None else np.asarray(variance_a, float)
    vb = b if variance_b is None else np.asarray(variance_b, float)
    covariance = 0 if overlap is None else np.asarray(overlap, dtype=float)
    v = np.divide(va, b * b, out=np.zeros_like(a), where=b > 0)
    v += np.divide(a * a * vb, b ** 4, out=np.zeros_like(a), where=b > 0)
    v -= np.divide(2 * a * covariance, b ** 3, out=np.zeros_like(a), where=b > 0)
    error = np.where((a >= 0) & (b > 0), np.sqrt(np.maximum(0, v)), np.nan)
    return r, error


def asymmetry(positive, negative):
    positive, negative = np.asarray(positive, float), np.asarray(negative, float)
    n = positive + negative
    a = np.divide(positive - negative, n, out=np.full_like(n, np.nan), where=n > 0)
    error = np.sqrt(np.divide(4 * positive * negative, n ** 3, out=np.full_like(n, np.nan), where=n > 0))
    return n, a, error


def fractional_change(new, old):
    new, old = np.asarray(new, float), np.asarray(old, float)
    valid = np.isfinite(new) & np.isfinite(old) & (old != 0)
    return np.divide(new-old, old, out=np.full_like(new, np.nan), where=valid)


def poisson_cross_section(counts, luminosity):
    counts, luminosity = np.asarray(counts, float), np.asarray(luminosity, float)
    valid = np.isfinite(luminosity) & (luminosity > 0)
    values = np.divide(counts, luminosity, out=np.full_like(counts, np.nan), where=valid)
    errors = np.divide(np.sqrt(counts), luminosity, out=np.full_like(counts, np.nan), where=valid)
    return values, errors


def coverage(d):
    old = {tuple(x[:2]): x for x in d["samples"]["old"]["lumisections"]}
    new = {tuple(x[:2]): x for x in d["samples"]["new"]["lumisections"]}
    common = old.keys() & new.keys()
    differences = [list(k) for k in sorted(common) if old[k][2] != new[k][2] or old[k][5:] != new[k][5:]]
    return {"old_lumisections": len(old), "new_lumisections": len(new),
            "old_only_lumisections": [list(k) for k in sorted(old.keys() - new.keys())],
            "new_only_lumisections": [list(k) for k in sorted(new.keys() - old.keys())],
            "different_stored_event_fingerprints": differences,
            "stored_event_fingerprints_equal": old.keys() == new.keys() and not differences}


def safe_name(name):
    return re.sub(r"[^A-Za-z0-9_.-]+", "_", name).strip("._") or "dataset"


def pretty_label(label):
    parts = re.split(r"(\$[^$]*\$)", label)
    for index in range(0, len(parts), 2):
        part = parts[index].replace("Absolute ", "").replace("Mock ", "")
        part = part.replace("wj jet", "wide jet").replace("ak4 jet", "AK4 jet")
        part = part.replace("delta eta", "@@DETA@@").replace("delta phi", "@@DPHI@@")
        part = re.sub(r"\bpT\b", lambda m: r"$p_T$", part)
        part = re.sub(r"\beta\b", lambda m: r"$\eta$", part)
        part = re.sub(r"\bphi\b", lambda m: r"$\phi$", part)
        parts[index] = part.replace("@@DETA@@", r"$\Delta\eta$").replace("@@DPHI@@", r"$\Delta\phi$")
    return "".join(parts)


def legend_period(dataset):
    eras = sorted(set(re.findall(r"(?<!\d)20\d{2}[A-Z]+(?![A-Za-z0-9])", dataset)))
    if len(eras) <= 2 and eras:
        return ", ".join(eras)
    if eras:
        return eras[0] + "–" + (eras[-1][4:] if eras[0][:4] == eras[-1][:4] else eras[-1])
    return "/".join(sorted(set(re.findall(r"(?<!\d)20\d{2}", dataset))))


def jet_label(label, name):
    match = re.search(r"(?:_j|_eta|_rawpt|_nvtx)([12])$", name)
    return pretty_label(label) + (f" (J{match.group(1)})" if match else "")


def canvas_payload(fig, name):
    fig.canvas.draw()
    from matplotlib.colors import to_hex
    result = {"name": name, "size": (fig.get_size_inches()*100).astype(int).tolist(), "axes": []}
    for ax in fig.axes:
        item = {"bounds": list(ax.get_position().bounds), "xlim": list(ax.get_xlim()), "ylim": list(ax.get_ylim()),
                "xlabel": ax.get_xlabel(), "ylabel": ax.get_ylabel(), "logx": ax.get_xscale()=="log",
                "logy": ax.get_yscale()=="log", "show_xlabels": any(t.get_visible() for t in ax.get_xticklabels()),
                "xticks": [float(v) for v in ax.get_xticks() if min(ax.get_xlim())<=v<=max(ax.get_xlim())],
                "yticks": [float(v) for v in ax.get_yticks() if min(ax.get_ylim())<=v<=max(ax.get_ylim())],
                "graphs": [], "histograms": [], "meshes": [], "polygons": [], "texts": [], "legend": None}
        item["custom_xticks"] = []
        item["custom_yticks"] = []
        for tick,label in zip(ax.get_xticks(),ax.get_xticklabels()):
            value = label.get_text().replace('−','-')
            if value and not value.startswith('$') and not re.fullmatch(r"[-+\d.eE]+",value):
                item["custom_xticks"].append([float(tick),value])
        for tick,label in zip(ax.get_yticks(),ax.get_yticklabels()):
            value = label.get_text().replace('−','-')
            if value and not value.startswith('$') and not re.fullmatch(r"[-+\d.eE]+",value):
                fraction = ax.transAxes.inverted().transform(ax.transData.transform((ax.get_xlim()[0],tick)))[1]
                item["custom_yticks"].append([float(fraction),value])
        item["ylim"] = sorted(item["ylim"])
        excluded = set()
        for container in ax.containers:
            if not isinstance(container, ErrorbarContainer):
                continue
            line, caps, bars = container.lines
            if line is not None:
                excluded.add(line)
            excluded.update(caps)
            points = {}
            for bar in bars:
                for segment in bar.get_segments():
                    if len(segment)!=2 or not np.isfinite(segment).all():
                        continue
                    x, y = np.mean(segment, axis=0)
                    entry = points.setdefault((float(x), float(y)), [float(x), float(y), 0., 0.])
                    entry[2] = max(entry[2], abs(float(segment[1,0]-segment[0,0]))/2)
                    entry[3] = max(entry[3], abs(float(segment[1,1]-segment[0,1]))/2)
            color = line.get_color() if line is not None else bars[0].get_colors()[0] if bars else "black"
            item["graphs"].append({"points": list(points.values()), "color": to_hex(color),
                                   "marker": str(line.get_marker()) if line is not None else "None",
                                   "line": str(line.get_linestyle()) if line is not None else "None",
                                   "label": container.get_label(),
                                   "open": line.get_fillstyle()=="none" if line is not None else False})
        for line in ax.lines:
            if line in excluded:
                continue
            xy = np.column_stack((line.get_xdata(orig=False), line.get_ydata(orig=False)))
            xy = ax.transData.inverted().transform(line.get_transform().transform(xy))
            points = [[float(x),float(y),0.,0.] for x,y in xy if math.isfinite(x) and math.isfinite(y)]
            item["graphs"].append({"points": points, "color": to_hex(line.get_color()), "marker": str(line.get_marker()),
                                   "line": str(line.get_linestyle()), "label": line.get_label(), "open": line.get_fillstyle()=="none"})
        for patch in ax.patches:
            if isinstance(patch, StepPatch):
                data = patch.get_data()
                item["histograms"].append({"edges": data.edges.tolist(), "values": data.values.tolist(),
                                           "color": to_hex(patch.get_edgecolor()), "label": patch.get_label()})
            else:
                xy = ax.transData.inverted().transform(patch.get_transform().transform(patch.get_path().vertices))
                item["polygons"].append({"xy": xy.tolist(), "color": to_hex(patch.get_facecolor())})
        for collection in ax.collections:
            if hasattr(collection, "_coordinates") and collection.get_array() is not None:
                coordinates = collection._coordinates
                values = np.asarray(collection.get_array()).reshape(coordinates.shape[0]-1, coordinates.shape[1]-1)
                item["meshes"].append({"xedges": coordinates[0,:,0].tolist(), "yedges": coordinates[:,0,1].tolist(),
                                      "values": values.tolist()})
            elif isinstance(collection, PolyCollection) and len(collection.get_facecolors()):
                rgba = collection.get_facecolors()[0]
                fill = to_hex(rgba[:3]*rgba[3]+1-rgba[3])
                for path in collection.get_paths():
                    xy = ax.transData.inverted().transform(collection.get_transform().transform(path.vertices))
                    if len(xy) and np.isfinite(xy).all():
                        item["polygons"].append({"xy": xy.tolist(), "color": fill, "label": collection.get_label()})
        for im in ax.images:
            values = np.asarray(im.get_array())
            low, high, bottom, top = im.get_extent()
            item["meshes"].append({"xedges": np.linspace(low,high,values.shape[1]+1).tolist(),
                                   "yedges": np.linspace(min(bottom,top),max(bottom,top),values.shape[0]+1).tolist(),
                                   "values": (values[::-1] if im.origin=="upper" else values).tolist()})
        for t in ax.texts:
            x, y = ax.transAxes.inverted().transform(t.get_transform().transform(t.get_position()))
            item["texts"].append({"x": float(x), "y": float(y), "text": t.get_text(), "size": t.get_fontsize(),
                                  "bold": t.get_fontweight()=="bold", "italic": t.get_fontstyle()=="italic",
                                  "ha": t.get_ha(), "va": t.get_va(), "color": to_hex(t.get_color())})
        legend = ax.get_legend()
        if legend:
            bounds = ax.transAxes.inverted().transform(legend.get_window_extent().get_points()).flatten().tolist()
            item["legend"] = {"bounds": bounds, "labels": [t.get_text() for t in legend.get_texts()]}
        result["axes"].append(item)
    return result


class Plotter:
    def __init__(self, d, luminosity, args, output, root_part=None):
        self.d, self.luminosity, self.args = d, luminosity, args
        self.all_stored = d["config"]["preset"] == "all_stored"
        self.output = Path(output)
        self.output.mkdir(parents=True, exist_ok=True)
        for folder in (self.output,)+(tuple(self.output/fmt for fmt in ("pdf", "png", "svg"))):
            for name in ("run_index_selected_fraction", "run_index_stored_yield", "run_index_dijet_cross_section"):
                for fmt in ("pdf", "png", "svg"):
                    obsolete = folder/f"{name}.{fmt}"
                    if obsolete.is_file():
                        obsolete.unlink()
        obsolete = self.output/"report.json"
        if obsolete.is_file():
            obsolete.unlink()
        self.mass_range = args.mass_range or [0, 12 if d["config"]["preset"] in ("all", "all_stored") else 8]
        self.ht_range = args.ht_range or [0, 14]
        period = legend_period(d["dataset"])
        self.period = period
        self.labels = {}
        for role, label in (("old", args.old_label), ("new", args.new_label)):
            if period and period not in label:
                label = re.sub(r"^20\d{2}\s+", "", label)
                label = f"{period} {label}"
            self.labels[role] = label
        self.mock = d.get("mock")
        self.figures = []
        self.tables = {}
        self.canvases = []
        self.root_part = root_part
        self.groups = set(args.plots or ["spectra", "xsec", "run", "eta", "selection"])
        old_hists, new_hists = (d["samples"][role]["histograms"] for role in ("old", "new"))
        if old_hists.keys() != new_hists.keys():
            raise ValueError(f"{d['dataset']}: old/new histogram sets differ")
        for name in old_hists:
            if old_hists[name]["edges"] != new_hists[name]["edges"]:
                raise ValueError(f"{d['dataset']}: old/new binning differs for {name}; reduce both samples with the same reducer")
        self.mass_binning = {name: "standard dijet" if tuple(old_hists[name]["edges"]) == DIJET_MASS_BINS else "saved legacy"
                             for name in ("mjj_base", "mjj", "ak4_mjj") if name in old_hists}
        self.ht_binning = {name: ("standard dijet" if name == "ht" else "uniform 50 GeV")
                           if name in old_hists and tuple(old_hists[name]["edges"]) == edges else "unavailable or legacy"
                           for name, edges in HT_BIN_EDGES.items()}
        self.pt_binning = {name: "uniform 100 GeV" if tuple(old_hists[name]["edges"]) == UNIFORM_PT_BINS else "saved legacy"
                           for name in UNIFORM_PT_HISTOGRAMS if name in old_hists}
        if any(value != "standard dijet" for value in self.mass_binning.values()):
            message = "Saved mass histograms use legacy bins. Rerun the updated C++ reducer for exact standard dijet bins; existing counts cannot be split."
            if args.require_dijet_binning:
                raise ValueError(f"{d['dataset']}: {message}")
            if "spectra" in self.groups:
                progress(message + " Plotting the saved bins without rebinning.", d["dataset"])
        self.runs = sorted({r["run"] for s in d["samples"].values() for r in s["runs"]})
        self.run_positions = {run: index for index, run in enumerate(self.runs)}
        self.run_maps = {role: {r["run"]: r for r in s["runs"]} for role, s in d["samples"].items()}
        self.lumi = np.asarray([luminosity["runs"].get(run, np.nan) for run in self.runs])
        self.has_lumi = bool(luminosity["runs"]) and not d["partial"]
        dataset_lumi = luminosity.get("datasets_pb", {})
        members = d.get("dataset_members", [d["dataset"]])
        if dataset_lumi:
            missing = set(members) - dataset_lumi.keys()
            if missing:
                raise ValueError(f"Missing --lumi_pb totals for datasets: {sorted(missing)}")
            total_lumi = sum(dataset_lumi[name] for name in members)
        elif luminosity.get("total_pb") is not None:
            total_lumi = luminosity["total_pb"]
        else:
            total_lumi = float(np.nansum(self.lumi))
        self.has_exposure = not d["partial"] and math.isfinite(total_lumi) and total_lumi > 0
        self.total_lumi_pb = total_lumi if self.has_exposure else None
        self.dijet_lumi_mode = "LS-weighted estimate" if luminosity.get("estimated") else "recorded per-run luminosity" if self.has_lumi else None
        self.paired = d.get("paired")
        self.qcd = d.get("qcd")
        if self.qcd:
            validate_qcd(self.qcd, d)
        if self.has_lumi:
            for role in ("old", "new"):
                for run, r in self.run_maps[role].items():
                    if r["selected"] and (not math.isfinite(luminosity["runs"].get(run, math.nan)) or luminosity["runs"][run] <= 0):
                        raise ValueError(f"{d['dataset']}: positive recorded luminosity missing for selected run {run}")
                if luminosity["lumisections"]:
                    for row in d["samples"][role]["lumisections"]:
                        if row[4] and luminosity["lumisections"].get(tuple(row[:2]),0) <= 0:
                            raise ValueError(f"Counted run/LS {row[:2]} has no positive recorded luminosity")
        if args.normalization == "xsec" and not self.has_exposure:
            raise ValueError("--normalization xsec requires luminosity and complete summaries")
        plt.rcParams.update({"font.family": "sans-serif", "font.sans-serif": ["DejaVu Sans"],
                             "font.size": 11, "axes.labelsize": 12, "axes.titlesize": 12,
                             "axes.linewidth": 1.1, "legend.fontsize": 10,
                             "errorbar.capsize": 3,
                             "xtick.direction": "in", "ytick.direction": "in", "xtick.top": True,
                             "ytick.right": True, "xtick.minor.visible": True, "ytick.minor.visible": True,
                             "xtick.major.size": 6, "ytick.major.size": 6,
                             "xtick.minor.size": 3, "ytick.minor.size": 3,
                             "xaxis.labellocation": "right", "yaxis.labellocation": "top",
                             "axes.grid": False, "savefig.bbox": "tight", "savefig.facecolor": "white",
                             "figure.facecolor": "white", "pdf.fonttype": 42, "ps.fonttype": 42})

    def brand(self, ax, detail=""):
        ax.text(0, 1.04, "CMS", transform=ax.transAxes, fontsize=17, weight="bold", ha="left")
        ax.text(0.14, 1.04, self.args.status,
                transform=ax.transAxes, fontsize=10, style="italic")
        exposure = f"{self.total_lumi_pb/1000:.0f} fb$^{{-1}}$ " if self.has_exposure else ""
        ax.text(1, 1.04, f"{exposure}({self.args.energy_tev:g} TeV)", transform=ax.transAxes,
                ha="right", fontsize=10)

    def axes(self, panels=True):
        if panels:
            fig, (top, bottom) = plt.subplots(2, 1, figsize=(8.2, 7.0), sharex=True,
                                             gridspec_kw={"height_ratios": [3, 1], "hspace": 0.06})
            fig.subplots_adjust(left=0.15, right=0.97, bottom=0.11, top=0.84)
            bottom.grid(axis="y", color="0.86", linewidth=0.6)
            return fig, top, bottom
        fig, ax = plt.subplots(figsize=(8.2, 5.8))
        fig.subplots_adjust(left=0.15, right=0.97, bottom=0.14, top=0.83)
        return fig, ax, None

    def save(self, fig, name):
        if any(ax.get_xlabel().startswith("Run Index") for ax in fig.axes):
            self.decorate_run_eras(fig.axes[0], fig.axes[1] if len(fig.axes) > 1 else None)
        for ax in fig.axes:
            lines = list(ax.lines)
            if ax.get_legend():
                lines.extend(ax.get_legend().findobj(Line2D))
            for line in lines:
                if line.get_marker() == "o" and to_rgba(line.get_color()) in (to_rgba(COLORS["old"]), to_rgba(COLORS["new"])):
                    line.set_markersize(6.0)
                    line.set_fillstyle("none" if to_rgba(line.get_color()) == to_rgba(COLORS["new"]) else "full")
        for fmt in dict.fromkeys(self.args.formats):
            directory = self.output/fmt
            directory.mkdir(parents=True, exist_ok=True)
            fig.savefig(directory/f"{name}.{fmt}", dpi=self.args.dpi)
            legacy = self.output/f"{name}.{fmt}"
            if legacy.is_file():
                legacy.unlink()
        self.figures.append(name)
        self.canvases.append(canvas_payload(fig, name))
        plt.close(fig)
        progress(f"Saved plot {len(self.figures)}: {name} ({', '.join(dict.fromkeys(self.args.formats))})", self.d["dataset"])

    def table(self, name, header, rows):
        self.tables[name] = {"columns": header, "rows": [list(row) for row in rows]}

    def remove_saved_plot(self, name):
        for folder in (self.output, self.output/"pdf", self.output/"png", self.output/"svg"):
            for fmt in ("pdf", "png", "svg"):
                path = folder/f"{name}.{fmt}"
                if path.is_file():
                    path.unlink()

    def run_values(self, role, name):
        return np.asarray([self.run_maps[role].get(run, {}).get(name, 0) for run in self.runs], float)

    def run_axis(self, ax, jet=None):
        ax.set_xlabel("Run Index" + (f" (J{jet})" if jet else ""))
        ax.xaxis.set_major_locator(MaxNLocator(nbins=10, integer=True))
        ax.xaxis.set_minor_locator(AutoMinorLocator())
        ax.set_xlim(-0.7, max(0.7, len(self.runs)-0.3))

    def draw_ratio(self, ax, x, new, old, overlap=None, fixed_range=None, variance_new=None, variance_old=None):
        r, err = ratio(new, old, overlap, variance_new, variance_old)
        displayed = fractional_change(new, old)
        shown = np.asarray(old, dtype=float) >= self.args.ratio_min_count
        ax.errorbar(np.asarray(x)[shown], displayed[shown], yerr=err[shown], fmt="o", color=COLORS["old"], ms=6.0, lw=1, capsize=3)
        valid = np.isfinite(err) & shown
        ax.axhline(0, color="0.3", lw=1, ls="--")
        visible = r[shown & np.isfinite(r)]
        if fixed_range:
            ax.set_ylim(*fixed_range)
        elif visible.size:
            extremes = np.concatenate((visible, r[valid]-err[valid], r[valid]+err[valid]))
            half = max(0.05, 1.15*np.max(np.abs(extremes-1)))
            ax.set_ylim(-half, half)
        ax.set_ylabel(FRACTIONAL_CHANGE_LABEL)
        return r, err

    def run_era_labels(self):
        era_map = dict(self.d.get("run_eras", []))
        return [re.sub(r"(?i)\s+mock\b", "", era_map.get(run, self.d["dataset"])) for run in self.runs]

    def decorate_run_eras(self, top, bottom=None):
        eras = self.run_era_labels()
        multiple_eras = len(set(eras)) > 1
        start = 0
        for index in range(1, len(eras)+1):
            if index < len(eras) and eras[index] == eras[start]:
                continue
            if start and multiple_eras:
                for ax in (top, bottom):
                    if ax is not None:
                        ax.axvline(start-0.5, color="0.78", lw=0.9, ls="--", zorder=0)
            if index > start:
                top.text((start+index-1)/2, 0.96, eras[start], transform=top.get_xaxis_transform(),
                         ha="center", va="top", fontsize=8, color="0.32",
                         bbox={"facecolor": "white", "edgecolor": "none", "alpha": 0.8, "pad": 1.5})
            start = index

    def run_index_plot(self):
        if not self.has_lumi:
            progress("Skipping run cross section: supply recorded per-run luminosity, or --lumi_pb with --estimate-run-lumi for an LS-weighted approximation", self.d["dataset"])
            return
        x = np.arange(len(self.runs))
        counts = {role: np.asarray([self.run_maps[role].get(run, {}).get("moments", {"count": [0]})["count"][0]
                                   for run in self.runs], float) for role in ("old", "new")}
        denominator = self.lumi.copy()
        fig, top, bottom = self.axes()
        fig.set_size_inches(max(8.2, min(14.0, 8.2 + len(x)/95)), 7.0)
        self.brand(top)
        values, errors, shown, reasons = {}, {}, {}, {}
        extents = []
        stride = max(1, math.ceil(len(x)/180))
        for role in ("old", "new"):
            values[role], errors[role] = poisson_cross_section(counts[role], denominator)
            mask = (counts[role] >= self.args.min_count) & np.isfinite(values[role])
            shown[role] = mask
            reasons[role] = ["; ".join(reason for condition, reason in
                                     ((not math.isfinite(denominator[i]) or denominator[i] <= 0, "no positive luminosity"),
                                      (n < self.args.min_count, f"fewer than {self.args.min_count} dijets")) if condition)
                             for i, n in enumerate(counts[role])]
            top.plot(x[mask], values[role][mask], "o", ms=6.0,
                     fillstyle="none" if role=="new" else "full", color=COLORS[role], label=self.labels[role])
            sampled = np.flatnonzero(mask)[::stride]
            top.errorbar(x[sampled], values[role][sampled], yerr=errors[role][sampled],
                         fmt="none", color=COLORS[role], elinewidth=0.7, alpha=0.8, capsize=3)
            extents.extend((values[role][mask]-errors[role][mask]).tolist())
            extents.extend((values[role][mask]+errors[role][mask]).tolist())
        finite = np.asarray(extents)
        finite = finite[np.isfinite(finite)]
        if finite.size:
            low, high = finite.min(), finite.max()
            if self.all_stored:
                positive = finite[finite > 0]
                top.set_yscale("log")
                top.set_ylim(max(1e-12, positive.min()*0.5), positive.max()*3)
            else:
                pad = max(0.15*(high-low), 0.03*high)
                top.set_ylim(max(0, low-pad), high+pad)
        top.set_ylabel("Dijet cross section [pb]")
        top.legend(frameon=False, loc="lower right" if self.all_stored else "upper left")
        paired_runs = {row[0]: row[3] for row in self.paired["runs"]} if self.paired and not self.all_stored else None
        overlap = np.asarray([paired_runs.get(run, 0) for run in self.runs], float) if paired_runs is not None else None
        r, err = self.draw_ratio(bottom, x, counts["new"], counts["old"], overlap)
        self.run_axis(bottom)
        self.table("run_index_cross_section", ["run_index", "run", "dataset", "normalization",
                                                    "normalization_lumi_pb", "old_dijets", "new_dijets", "old_xsec_pb", "new_xsec_pb",
                                                    "old_stat_error_pb", "new_stat_error_pb", "new_over_old", "fractional_change_stat_error", "fractional_change",
                                                    "old_drawn", "new_drawn", "old_skip_reason", "new_skip_reason"],
                   zip(x, self.runs, self.run_era_labels(), [self.dijet_lumi_mode]*len(x), denominator,
                       counts["old"], counts["new"], values["old"], values["new"], errors["old"], errors["new"], r, err, r-1,
                       shown["old"].astype(int), shown["new"].astype(int), reasons["old"], reasons["new"]))
        progress(f"Dijet cross section: N_run/L_run using {self.dijet_lumi_mode}; run exposure={np.nansum(self.lumi):g} pb^-1", self.d["dataset"])
        omitted = np.flatnonzero(~(shown["old"] | shown["new"]))
        if omitted.size:
            preview = ", ".join(f"{i} (run {self.runs[i]}, N_old={counts['old'][i]:g}, N_new={counts['new'][i]:g})" for i in omitted[:5])
            progress(f"Omitted {len(omitted)} run indices without positive luminosity or at least {self.args.min_count} dijets: {preview}" +
                     (", ..." if len(omitted) > 5 else "") + "; full reasons are saved in the ROOT run table", self.d["dataset"])
        self.save(fig, "run_index_cross_section")

    def spectra(self):
        old, new = (self.d["samples"][k]["histograms"] for k in ("old", "new"))
        flow_rows = []
        if self.args.spectra:
            unknown = sorted(set(self.args.spectra) - old.keys())
            if unknown:
                if "ht_50gev" in unknown:
                    raise ValueError("The 50 GeV HT histogram is missing; regenerate both summaries with the updated reducer")
                raise ValueError(f"Unknown spectra: {unknown}")
            names = list(dict.fromkeys(self.args.spectra))
        else:
            names = old if self.args.all_spectra else [name for name in CORE_SPECTRA if name in old]
            if "ht_50gev" not in old:
                progress("Skipping 50 GeV HT spectrum: regenerate both summaries with the updated reducer; coarse saved bins cannot be split exactly", self.d["dataset"])
        for name in names:
            a, b = old[name], new[name]
            if name in UNIFORM_PT_HISTOGRAMS and self.pt_binning[name] != "uniform 100 GeV":
                self.remove_saved_plot("spectrum_"+name)
                message = f"{name} needs 100 GeV pT bins; recompile the reducer and regenerate both summaries because the saved variable-width counts cannot be split exactly"
                if self.args.spectra:
                    raise ValueError(message)
                progress("Skipping " + message, self.d["dataset"])
                continue
            if name in HT_BIN_EDGES and tuple(a["edges"]) != HT_BIN_EDGES[name]:
                message = f"HT histogram {name} requires the updated bin edges; regenerate both summaries with the updated reducer"
                if self.args.spectra:
                    raise ValueError(message)
                progress("Skipping " + message, self.d["dataset"])
                continue
            if name == "ak4_mjj" and self.mass_binning[name] != "standard dijet":
                raise ValueError("AK4 mass requires the standard dijet bins; regenerate both summaries with the updated reducer")
            edges = np.asarray(a["edges"], float)
            mass = name in ("mjj", "mjj_base", "ak4_mjj")
            pt = (name.startswith("pt_") and name != "pt_asymmetry") or name.startswith("raw_pt")
            ht = name in HT_BIN_EDGES
            tev = mass or pt or ht
            display_edges = edges / (1000 if tev else 1)
            centers = (display_edges[1:] + display_edges[:-1]) / 2
            raw = {"old": np.asarray(a["counts"][1:-1], float), "new": np.asarray(b["counts"][1:-1], float)}
            fig, top, bottom = self.axes()
            self.brand(top, "Base selection" if name == "mjj_base" else "Selected dijets")
            plotted = {}
            for role in ("old", "new"):
                n = raw[role]
                denominator = 1.0
                if self.args.normalization == "shape":
                    denominator = max(1, sum(self.d["samples"][role]["histograms"][name]["counts"]))
                elif self.args.normalization == "xsec":
                    denominator = self.total_lumi_pb
                plotted[role] = n / denominator
                top.stairs(plotted[role], display_edges, color=COLORS[role], lw=1.8, label=self.labels[role])
                flow_rows.append([name, role, self.d["samples"][role]["histograms"][name]["counts"][0],
                                  self.d["samples"][role]["histograms"][name]["counts"][-1],
                                  self.d["samples"][role]["histograms"][name]["invalid"]])
            shown_range = self.mass_range if mass else self.ht_range if ht else None
            visible_bins = (display_edges[:-1] < shown_range[1]) & (display_edges[1:] > shown_range[0]) if shown_range else np.ones(len(centers), bool)
            positive = np.concatenate([v[visible_bins & (v > 0)] for v in plotted.values()])
            log = ht or name.startswith(("pt_", "raw_pt", "mjj", "ak4_mjj", "eta_", "phi_", "jec")) or name in ("met", "nvtx", "delta_eta", "delta_phi")
            if log:
                top.set_yscale("log")
                top.set_ylim(max(positive.min() * 0.4, 1e-12), positive.max() * 5) if positive.size else top.set_ylim(0.5, 5)
            elif positive.size:
                top.set_ylim(0, positive.max() * 1.25)
            top.legend(frameon=False, loc="upper right")
            top.set_ylabel({"yield": "Entries / bin", "shape": "Normalized entries / bin",
                            "xsec": "Cross section / bin [pb]"}[self.args.normalization])
            overlap = self.paired["overlap"][name][1:-1] if self.paired and self.args.normalization != "shape" else None
            scale = 1.0
            variance_new, variance_old = None, None
            if self.args.normalization == "shape":
                scale = sum(a["counts"]) / max(1, sum(b["counts"]))
                variance_new = scale**2*raw["new"]*(1-raw["new"]/max(1,sum(b["counts"])))
                variance_old = raw["old"]*(1-raw["old"]/max(1,sum(a["counts"])))
            r, err = self.draw_ratio(bottom, centers, raw["new"]*scale, raw["old"], overlap,
                                     fixed_range=(-0.5,0.5) if name.startswith("raw_pt") else None,
                                     variance_new=variance_new, variance_old=variance_old)
            label = CORE_LABELS.get(name, a["label"])
            bottom.set_xlabel(jet_label(label.replace("[GeV]", "[TeV]") if tev else label, name))
            nonempty = (raw["old"] + raw["new"]) > 0
            if shown_range:
                bottom.set_xlim(*shown_range)
            elif nonempty.any():
                indices = np.flatnonzero(nonempty)
                bottom.set_xlim(display_edges[max(0, indices[0]-1)], display_edges[min(len(edges)-1, indices[-1]+1)])
            unit = "TeV" if tev else "GeV" if "[GeV]" in a["label"] else ""
            self.table("spectrum_"+name, ["low", "high", "old_count", "new_count", "new_over_old", "fractional_change_stat_error", "display_low", "display_high", "display_unit", "fractional_change"],
                       zip(edges[:-1], edges[1:], raw["old"], raw["new"], r, err, display_edges[:-1], display_edges[1:], [unit]*len(raw["old"]), r-1))
            self.save(fig, "spectrum_" + name)
            if self.qcd:
                self.qcd_spectrum(name, "spectrum_"+name+"_data_qcd")
        self.table("histogram_flow_counts", ["histogram", "jec", "underflow", "overflow", "nonfinite"], flow_rows)

    def qcd_ratio(self, ax, x, values, errors, reference, uncertainty, counts, minimum, edges=None):
        reference, uncertainty = np.broadcast_arrays(np.asarray(reference, float), np.asarray(uncertainty, float))
        valid_mc = np.isfinite(reference) & (reference > 0) & np.isfinite(uncertainty)
        relative = np.divide(uncertainty, reference, out=np.full_like(reference, np.nan), where=valid_mc)
        band_edges = np.asarray(edges) if edges is not None else np.r_[x-0.5, x[-1]+0.5]
        ax.fill_between(np.repeat(band_edges, 2)[1:-1], np.repeat(1-relative, 2), np.repeat(1+relative, 2),
                        color=COLORS["qcd"], alpha=0.22, label="QCD stat.")
        ax.axhline(1, color="0.3", ls="--", lw=1)
        result = {}
        visible = []
        for role in ("old", "new"):
            ratios = np.divide(values[role], reference, out=np.full_like(reference, np.nan), where=valid_mc)
            stat = np.divide(errors[role], reference, out=np.full_like(reference, np.nan), where=valid_mc)
            total = np.hypot(stat, ratios*relative)
            drawn = valid_mc & np.isfinite(ratios) & np.isfinite(stat) & (counts[role] >= minimum)
            ax.errorbar(x[drawn], ratios[drawn], yerr=stat[drawn], fmt="o", ms=6, color=COLORS[role], label=self.labels[role])
            result[role] = (ratios, stat, total, drawn)
            visible.extend((ratios[drawn]-stat[drawn], ratios[drawn]+stat[drawn]))
        visible.extend(((1-relative)[valid_mc], (1+relative)[valid_mc]))
        finite = np.concatenate(visible)
        finite = finite[np.isfinite(finite)]
        low, high = (min(1, finite.min()), max(1, finite.max())) if finite.size else (0.5, 1.5)
        padding = max(0.1, (high-low)*0.15)
        ax.set_ylim(max(0, low-padding), high+padding)
        ax.set_ylabel("Data / QCD")
        return result

    def qcd_spectrum(self, name, output_name, differential=False):
        normalization = "xsec" if differential else self.args.normalization
        if normalization != "shape" and not self.has_exposure:
            progress("Skipping "+output_name+": QCD yield/cross-section comparison requires luminosity and complete data summaries", self.d["dataset"])
            return
        spec = self.qcd["histograms"][name]
        edges = np.asarray(spec["edges"], float)
        mass = name in ("mjj", "mjj_base", "ak4_mjj")
        pt = (name.startswith("pt_") and name != "pt_asymmetry") or name.startswith("raw_pt")
        ht = name in HT_BIN_EDGES
        tev = mass or pt or ht
        edges = edges/(1000 if tev else 1)
        centers, widths = (edges[1:]+edges[:-1])/2, np.diff(edges)
        raw = {role: np.asarray(self.d["samples"][role]["histograms"][name]["counts"], float) for role in ("old", "new")}
        mc = np.asarray(spec["sumw"], float)*self.args.mc_scale
        variance = np.asarray(spec["sumw2"], float)*self.args.mc_scale**2
        values, errors = {}, {}
        if normalization == "shape":
            def shape(values, variance):
                total = values.sum()
                if total <= 0:
                    return np.full_like(values, np.nan), np.full_like(values, np.nan)
                fraction = values/total
                error = np.sqrt(np.maximum(0, variance*(1-2*fraction)+fraction**2*variance.sum()))/total
                return fraction, error
            reference, uncertainty = shape(mc, variance)
            for role in ("old", "new"):
                values[role], errors[role] = shape(raw[role], raw[role])
        else:
            factor = self.total_lumi_pb if normalization == "yield" else 1.0
            reference, uncertainty = mc*factor, np.sqrt(variance)*factor
            denominator = 1.0 if normalization == "yield" else self.total_lumi_pb
            for role in ("old", "new"):
                values[role], errors[role] = raw[role]/denominator, np.sqrt(raw[role])/denominator
        reference, uncertainty = reference[1:-1], uncertainty[1:-1]
        for role in ("old", "new"):
            values[role], errors[role] = values[role][1:-1], errors[role][1:-1]
            raw[role] = raw[role][1:-1]
        if differential:
            reference, uncertainty = reference/widths, uncertainty/widths
            for role in ("old", "new"):
                values[role], errors[role] = values[role]/widths, errors[role]/widths
        fig, top, bottom = self.axes()
        self.brand(top)
        top.stairs(reference, edges, color=COLORS["qcd"], lw=1.8, label=self.args.mc_label)
        top.fill_between(edges, np.r_[reference-uncertainty, (reference-uncertainty)[-1]],
                         np.r_[reference+uncertainty, (reference+uncertainty)[-1]], step="post", color=COLORS["qcd"], alpha=0.22)
        for role in ("old", "new"):
            drawn = np.isfinite(values[role]) & (raw[role] > 0)
            top.errorbar(centers[drawn], values[role][drawn], yerr=errors[role][drawn], fmt="o", ms=6, color=COLORS[role], label=self.labels[role])
        shown_range = self.mass_range if mass else self.ht_range if ht else None
        visible = (edges[:-1] < shown_range[1]) & (edges[1:] > shown_range[0]) if shown_range else np.ones(len(centers), bool)
        positive = np.concatenate([v[visible & np.isfinite(v) & (v > 0)] for v in [reference, *values.values()]])
        top.set_yscale("log")
        top.set_ylim(max(1e-12, positive.min()*0.4), positive.max()*5) if positive.size else top.set_ylim(0.5, 5)
        top.set_ylabel(r"$d\sigma/dm_{jj}$ [pb/TeV]" if differential else
                       {"yield": "Entries / bin", "shape": "Normalized entries / bin", "xsec": "Cross section / bin [pb]"}[normalization])
        top.legend(frameon=False, loc="upper right")
        ratios = self.qcd_ratio(bottom, centers, values, errors, reference, uncertainty, raw, self.args.ratio_min_count, edges)
        label = CORE_LABELS.get(name, spec["label"])
        bottom.set_xlabel(jet_label(label.replace("[GeV]", "[TeV]") if tev else label, name))
        if shown_range:
            bottom.set_xlim(*shown_range)
        self.table(output_name, ["low", "high", "old_count", "new_count", "old_value", "new_value",
                                 "old_stat_error", "new_stat_error", "qcd_value", "qcd_stat_error",
                                 "old_over_qcd", "old_ratio_data_error", "old_ratio_total_error", "old_drawn",
                                 "new_over_qcd", "new_ratio_data_error", "new_ratio_total_error", "new_drawn"],
                   zip(edges[:-1], edges[1:], raw["old"], raw["new"], values["old"], values["new"], errors["old"], errors["new"],
                       reference, uncertainty, *ratios["old"], *ratios["new"]))
        self.save(fig, output_name)

    def qcd_run_plot(self):
        if not self.qcd or not self.has_lumi or not self.runs:
            return
        x = np.arange(len(self.runs))
        counts = {role: np.asarray([self.run_maps[role].get(run, {}).get("moments", {"count": [0]})["count"][0]
                                   if self.all_stored else self.run_maps[role].get(run, {}).get("selected", 0)
                                   for run in self.runs], float) for role in ("old", "new")}
        hist = self.qcd["histograms"]["mjj"]
        reference = np.full(len(x), sum(hist["sumw"])*self.args.mc_scale)
        uncertainty = np.full(len(x), math.sqrt(sum(hist["sumw2"]))*self.args.mc_scale)
        fig, top, bottom = self.axes()
        self.brand(top)
        values, errors = {}, {}
        top.plot([-0.7, max(0.7, len(x)-0.3)], [reference[0]]*2, color=COLORS["qcd"], label=self.args.mc_label)
        top.fill_between([-0.7, max(0.7, len(x)-0.3)], reference[0]-uncertainty[0], reference[0]+uncertainty[0], color=COLORS["qcd"], alpha=0.22)
        for role in ("old", "new"):
            values[role], errors[role] = poisson_cross_section(counts[role], self.lumi)
            mask = np.isfinite(values[role]) & (counts[role] >= self.args.min_count)
            top.errorbar(x[mask], values[role][mask], yerr=errors[role][mask], fmt="o", ms=6, color=COLORS[role], label=self.labels[role])
        top.set_ylabel("Dijet cross section [pb]")
        top.set_ylim(bottom=0)
        top.legend(frameon=False, loc="lower right")
        ratios = self.qcd_ratio(bottom, x, values, errors, reference, uncertainty, counts, self.args.min_count)
        self.run_axis(bottom)
        self.table("run_index_data_qcd", ["run_index", "run", "dataset", "lumi_pb", "old_count", "new_count",
                                         "old_xsec_pb", "new_xsec_pb", "old_stat_error_pb", "new_stat_error_pb", "qcd_xsec_pb", "qcd_stat_error_pb",
                                         "old_over_qcd", "old_ratio_data_error", "old_ratio_total_error", "old_drawn",
                                         "new_over_qcd", "new_ratio_data_error", "new_ratio_total_error", "new_drawn"],
                   zip(x, self.runs, self.run_era_labels(), self.lumi, counts["old"], counts["new"], values["old"], values["new"],
                       errors["old"], errors["new"], reference, uncertainty, *ratios["old"], *ratios["new"]))
        self.save(fig, "run_index_data_qcd")

    def mass_cross_section_plot(self):
        if not self.has_exposure:
            progress("Skipping mass cross section: provide --lumi_pb or --lumi-csv", self.d["dataset"])
            return
        if self.mass_binning.get("mjj") != "standard dijet":
            progress("Skipping standard-binned mass cross section: regenerate both summaries with the updated reducer; legacy bin counts cannot be split exactly", self.d["dataset"])
            return
        edges = np.asarray(DIJET_MASS_BINS, float)/1000
        progress(f"Mass cross section: {len(edges)-1} standard dijet bins; luminosity={self.total_lumi_pb:g} pb^-1", self.d["dataset"])
        widths = np.diff(edges)
        centers = (edges[1:]+edges[:-1])/2
        counts = {role: np.asarray(self.d["samples"][role]["histograms"]["mjj"]["counts"][1:-1],float) for role in ("old","new")}
        fig, top, bottom = self.axes()
        self.brand(top)
        values, errors = {}, {}
        for role in ("old","new"):
            values[role] = counts[role]/(widths*self.total_lumi_pb)
            errors[role] = np.sqrt(counts[role])/(widths*self.total_lumi_pb)
            top.stairs(values[role], edges, color=COLORS[role], lw=1.8, label=self.labels[role])
        positive = np.concatenate([v[v>0] for v in values.values()])
        top.set_yscale("log")
        top.set_ylim(positive.min()*0.4,positive.max()*5) if positive.size else top.set_ylim(0.5,5)
        top.set_ylabel(r"$d\sigma/dm_{jj}$ [pb/TeV]")
        top.legend(frameon=False)
        overlap = self.paired["overlap"]["mjj"][1:-1] if self.paired else None
        r, err = self.draw_ratio(bottom,centers,counts["new"],counts["old"],overlap)
        bottom.set_xlabel("Dijet mass [TeV]")
        bottom.set_xlim(*self.mass_range)
        self.table("dijet_mass_cross_section",["low_tev","high_tev","bin_width_tev","lumi_pb","old_count","new_count",
                                                "old_dsigma_dm_pb_per_tev","new_dsigma_dm_pb_per_tev",
                                                "old_stat_error_pb_per_tev","new_stat_error_pb_per_tev","fractional_change","fractional_change_stat_error"],
                   zip(edges[:-1],edges[1:],widths,np.full(len(widths),self.total_lumi_pb),counts["old"],counts["new"],
                       values["old"],values["new"],errors["old"],errors["new"],r-1,err))
        self.save(fig,"dijet_mass_cross_section")
        if self.qcd:
            self.qcd_spectrum("mjj", "dijet_mass_data_qcd", differential=True)

    def run_plots(self):
        self.run_index_plot()
        self.qcd_run_plot()
        x = np.arange(len(self.runs))
        for i, ylabel in enumerate(("Mean wide-dijet mass [TeV]", "Mean leading wide-jet pT [TeV]",
                                   "Mean subleading wide-jet pT [TeV]", "Mean leading AK4 JEC", "Mean subleading AK4 JEC")):
            fig, top, _ = self.axes(False)
            self.brand(top)
            rows = []
            for role in ("old", "new"):
                mean, sem, count = [], [], []
                for run in self.runs:
                    m = self.run_maps[role].get(run, {}).get("moments", {"count": [0]*5, "sum": [0]*5, "sum2": [0]*5})
                    n, mu, se = moments(m)
                    mean.append(mu[i]); sem.append(se[i]); count.append(n[i])
                scale = 1000 if i < 3 else 1
                mean, sem, count = np.asarray(mean)/scale, np.asarray(sem)/scale, np.asarray(count)
                mask = count >= max(2, self.args.min_count)
                top.errorbar(x[mask], mean[mask], yerr=sem[mask], fmt="o", ms=6.0, color=COLORS[role], label=self.labels[role])
                rows.extend(zip([role]*len(x), x, self.runs, count, mean, sem))
            self.run_axis(top, {1: 1, 2: 2, 3: 1, 4: 2}.get(i))
            top.set_ylabel(pretty_label(ylabel)); top.legend(frameon=False)
            limits = (0.7, 1.3) if i >= 3 else (0, 0.5) if self.all_stored else (0.5, 1.6)
            top.set_ylim(*limits)
            self.table(f"run_mean_{i}", ["jec", "run_index", "run", "count", "mean", "standard_error"], rows)
            self.save(fig, f"run_mean_{i}")
        if self.has_lumi:
            fig, top, _ = self.axes(False)
            self.brand(top)
            for role in ("old", "new"):
                counts = np.asarray([self.run_maps[role].get(run,{}).get("moments",{"count":[0]})["count"][0] for run in self.runs])
                valid = np.isfinite(self.lumi) & (self.lumi > 0)
                average = counts[valid].sum() / self.lumi[valid].sum()
                if average > 0:
                    mask = valid & (counts >= self.args.min_count)
                    values, errors = poisson_cross_section(counts, self.lumi)
                    top.errorbar(x[mask], values[mask]/average,
                                 yerr=errors[mask]/average,
                                 fmt="o", ms=6.0, color=COLORS[role], label=self.labels[role])
            top.axhline(1, ls="--", color="0.3")
            self.run_axis(top); top.set_ylabel("Cross section / mean"); top.legend(frameon=False)
            self.save(fig, "run_relative_cross_section")

    def eta_plots(self):
        for j in (1, 2):
            for variable in ("eta", "pt", "run"):
                if variable == "pt" and any(self.pt_binning.get(f"pt_{side}_j{j}") != "uniform 100 GeV" for side in ("positive", "negative")):
                    self.remove_saved_plot(f"eta_asymmetry_pt_j{j}")
                    progress(f"Skipping eta asymmetry vs pT (J{j}): recompile the reducer and regenerate both summaries for exact 100 GeV bins", self.d["dataset"])
                    continue
                fig, top, bottom = self.axes()
                self.brand(top, f"Wide jet {j}: positive / negative eta stability")
                curves, curve_errors, masks, rows = {}, {}, {}, []
                for role in ("old", "new"):
                    s = self.d["samples"][role]
                    if variable == "eta":
                        h = s["histograms"][f"eta_wj_j{j}"]
                        e = np.asarray(h["edges"])
                        n = np.asarray(h["counts"][1:-1], float)
                        half = len(n)//2
                        if len(n) % 2 or e[half] != 0 or not np.allclose(e, -e[::-1], rtol=0, atol=1e-10):
                            raise ValueError(f"{self.d['dataset']}: eta asymmetry requires symmetric bins meeting at zero")
                        zero_count = sum(r["eta_zero"][j-1] if "eta_zero" in r else
                                         r["selected"] - sum(r["signs"][2*(j-1):2*j])
                                         for r in self.run_maps[role].values())
                        if zero_count < 0 or zero_count > n[half]:
                            raise ValueError(f"{self.d['dataset']}: inconsistent eta-zero count for jet {j}")
                        positive = n.copy()
                        positive[half] -= zero_count
                        negative = positive[::-1]
                        x = (e[:-1]+e[1:])/2
                        xlabel = r"Jet $\eta$"
                    elif variable == "pt":
                        h = s["histograms"][f"pt_positive_j{j}"]
                        positive = h["counts"][1:-1]
                        negative = s["histograms"][f"pt_negative_j{j}"]["counts"][1:-1]
                        e = np.asarray(h["edges"])
                        x = (e[:-1]+e[1:])/2000
                        xlabel = r"Wide-jet $p_T$ [TeV]"
                    else:
                        positive = [self.run_maps[role].get(run, {}).get("signs", [0]*4)[2*(j-1)] for run in self.runs]
                        negative = [self.run_maps[role].get(run, {}).get("signs", [0]*4)[2*(j-1)+1] for run in self.runs]
                        x = np.arange(len(self.runs)); xlabel = "Run Index"
                    n, a, err = asymmetry(positive, negative)
                    mask = n >= self.args.min_count
                    top.errorbar(x[mask], a[mask], yerr=err[mask], fmt="o", ms=6.0, color=COLORS[role], label=self.labels[role])
                    curves[role], curve_errors[role], masks[role] = a, err, mask
                    rows.extend(zip([role]*len(x), x, positive, negative, a, err,
                                    ["TeV" if variable == "pt" else "run_index" if variable == "run" else ""]*len(x)))
                top.axhline(0, color="0.3", ls="--")
                top.set_ylabel(ETA_ASYMMETRY_LABEL, fontsize=18)
                top.set_ylim(-0.7, 0.7)
                top.legend(frameon=False)
                change = curves["new"] - curves["old"]
                change_error = np.hypot(curve_errors["new"], curve_errors["old"])
                mask = masks["old"] & masks["new"] & np.isfinite(change)
                bottom.plot(x[mask], change[mask], "o", color=COLORS["old"], ms=6.0)
                bottom.axhline(0, color="0.3", ls="--")
                bottom.set_ylabel("New - Old");bottom.set_xlabel(pretty_label(xlabel)+f" (J{j})")
                if mask.any():
                    low = min(0.0, float(np.min(change[mask])))
                    high = max(0.0, float(np.max(change[mask])))
                    padding = max(0.05, 0.1*(high-low))
                    bottom.set_ylim(low-padding, high+padding)
                else:
                    bottom.set_ylim(-0.1, 0.1)
                rows = [list(row)+[value,error] for row,value,error in zip(rows, np.tile(change, 2), np.tile(change_error, 2))]
                if variable == "eta":
                    bottom.set_xlim(-3, 3)
                    bottom.xaxis.set_major_locator(MaxNLocator(nbins=6))
                elif variable == "run":
                    self.run_axis(bottom, j)
                    rows = [row+[self.runs[int(row[1])]] for row in rows]
                counts = ["n_eta", "n_minus_eta"] if variable == "eta" else ["n_positive", "n_negative"]
                self.table(f"eta_asymmetry_{variable}_j{j}", ["jec", "x"]+counts+["asymmetry", "stat_error", "x_unit", "new_minus_old", "new_minus_old_stat_error"]+(["run"] if variable == "run" else []), rows)
                self.save(fig, f"eta_asymmetry_{variable}_j{j}")

    def profile_plots(self, paired=False):
        profiles = self.paired["profiles"] if paired else self.d["samples"]["old"]["profiles"]
        for name, p in profiles.items():
            if p["yedges"]:
                continue
            fig, top, _ = self.axes(False)
            self.brand(top, "Matched events passing both selections" if paired else "Selected dijets")
            mass = "mass" in p["label"].lower() and "[GeV]" in p["label"]
            xedges = np.asarray(p["xedges"]) / (1000 if mass else 1)
            x = (xedges[1:]+xedges[:-1])/2
            rows = []
            for role in (["new"] if paired else ["old", "new"]):
                m = p["moments"] if paired else self.d["samples"][role]["profiles"][name]["moments"]
                n, mu, sem = [v[1:-1] for v in moments(m)]
                valid = (n >= max(2, self.args.min_count)) & np.isfinite(mu)
                top.errorbar(x[valid], mu[valid], yerr=sem[valid], fmt="o", ms=6.0, color=COLORS[role],
                             label=f"{self.period} New - old JEC".strip() if paired else self.labels[role])
                rows.extend(zip([role]*len(x), x, n, mu, sem))
            if paired:
                top.axhline(0, ls="--", color="0.3")
            elif name.startswith("balance"):
                top.axhline(1, ls="--", color="0.3")
            top.set_xlabel(jet_label(p["label"].replace("[GeV]", "[TeV]") if mass else p["label"], name));top.set_ylabel(pretty_label(p["ylabel"]));top.legend(frameon=False)
            if "rawpt" in name:
                top.set_xscale("log");top.set_xlim(left=20)
            prefix = "matched_profile_" if paired else "profile_"
            self.table(prefix+name, ["jec", "x", "count", "mean", "standard_error"], rows)
            self.save(fig, prefix+name)

    def maps(self, paired=False):
        profiles = self.paired["profiles"] if paired else self.d["samples"]["old"]["profiles"]
        for name, p in profiles.items():
            if not p["yedges"]:
                continue
            shape = (len(p["xedges"])+1, len(p["yedges"])+1)
            roles = ["matched"] if paired else ["old", "new", "change"]
            matrices = {}
            rows = []
            for role in (["matched"] if paired else ["old", "new"]):
                m = p["moments"] if paired else self.d["samples"][role]["profiles"][name]["moments"]
                n, mu, sem = moments(m)
                n, mu = n.reshape(shape)[1:-1, 1:-1], mu.reshape(shape)[1:-1, 1:-1]
                matrix = n if name.startswith("occupancy") else mu
                matrices[role] = np.where(n >= self.args.min_count, matrix, np.nan)
                errors = sem.reshape(shape)[1:-1, 1:-1]
                for i in range(n.shape[0]):
                    for j in range(n.shape[1]):
                        rows.append([role, p["xedges"][i], p["xedges"][i+1], p["yedges"][j], p["yedges"][j+1], n[i,j], mu[i,j], errors[i,j]])
            self.table(("matched_map_" if paired else "map_")+name,
                       ["jec", "x_low", "x_high", "y_low", "y_high", "count", "mean", "standard_error"], rows)
            if not paired:
                a, b = matrices["old"], matrices["new"]
                matrices["change"] = np.divide(100*(b-a), a, out=np.full_like(a, np.nan), where=a != 0)
            for role in roles:
                fig, ax, _ = self.axes(False)
                self.brand(ax, "Matched AK4 jets" if paired else self.labels.get(role, "100 (new / old - 1)"))
                matrix = matrices[role]
                kwargs = {"cmap": "viridis"}
                if role in ("matched", "change"):
                    finite = np.abs(matrix[np.isfinite(matrix)])
                    limit = max(float(np.percentile(finite, 98)), 0.01) if finite.size else 1
                    kwargs = {"cmap": "RdBu_r", "norm": TwoSlopeNorm(vmin=-limit, vcenter=0, vmax=limit)}
                mesh = ax.pcolormesh(p["xedges"], p["yedges"], matrix.T, shading="flat", **kwargs)
                label = "Change [%]" if role in ("matched", "change") else ("Entries" if name.startswith("occupancy") else "Mean JEC factor")
                fig.colorbar(mesh, ax=ax, label=label)
                ax.set_xlabel(jet_label(p["label"], name));ax.set_ylabel(pretty_label(p["ylabel"]))
                if "rawpt" in name:
                    ax.set_yscale("log");ax.set_ylim(bottom=20)
                self.save(fig, ("matched_map_" if paired else "map_")+name+"_"+role)

    def selection(self):
        indices = list(range(len(self.d["cutflow_labels"])))
        if self.d["config"]["preset"] == "all_stored":
            indices = [0]
        elif self.d["config"]["preset"] == "all":
            indices = [0, 2]
            if self.d["config"]["mjj_min"] > 0:
                indices.append(len(self.d["cutflow_labels"])-1)
        labels = [self.d["cutflow_labels"][i] for i in indices]
        fig, top, bottom = self.axes()
        self.brand(top, "Selection yields")
        x = np.arange(len(labels))
        for role in ("old", "new"):
            counts = np.asarray(self.d["samples"][role]["cutflow"], float)[indices]
            top.plot(x, counts, "o-", color=COLORS[role], label=self.labels[role])
        top.set_yscale("log");top.set_ylabel("Events");top.legend(frameon=False)
        self.draw_ratio(bottom, x, [self.d["samples"]["new"]["cutflow"][i] for i in indices],
                        [self.d["samples"]["old"]["cutflow"][i] for i in indices])
        bottom.set_xticks(x);bottom.set_xticklabels(labels, rotation=25, ha="right")
        self.save(fig, "cutflow")
        x = np.asarray(self.d["thresholds"], float) / 1000
        fig, top, bottom = self.axes()
        self.brand(top, "Mass-threshold sensitivity; same base cuts")
        for role in ("old", "new"):
            top.plot(x, self.d["samples"][role]["threshold_counts"], "o-", color=COLORS[role], label=self.labels[role])
        top.set_yscale("log");top.set_ylabel(r"$N(m_{jj}>m_{\mathrm{cut}})$");top.legend(frameon=False)
        overlap = np.asarray(self.paired["threshold_migration"])[:, 3] if self.paired else None
        r, err = self.draw_ratio(bottom, x, self.d["samples"]["new"]["threshold_counts"], self.d["samples"]["old"]["threshold_counts"], overlap)
        bottom.set_xlabel(r"$m_{\mathrm{cut}}$ [TeV]")
        self.table("mass_thresholds", ["threshold_gev", "old_count", "new_count", "new_over_old", "fractional_change_stat_error", "threshold_tev"],
                   zip(self.d["thresholds"], self.d["samples"]["old"]["threshold_counts"], self.d["samples"]["new"]["threshold_counts"], r, err, x))
        self.save(fig, "mass_thresholds")

    def paired_plots(self):
        for name, h in self.paired["shifts"].items():
            fig, ax, _ = self.axes(False)
            self.brand(ax, "Matched events passing both selections")
            edges, n = np.asarray(h["edges"]), np.asarray(h["counts"][1:-1], float)
            ax.stairs(n, edges, color=COLORS["new"], lw=1.6)
            ax.axvline(0, color="0.3", ls="--")
            ax.set_xlabel(f"{name}: " + h["label"]);ax.set_ylabel("Matched events")
            if n.any():
                ax.set_yscale("log")
            self.table("matched_shift_"+name, ["low_percent", "high_percent", "count"], zip(edges[:-1], edges[1:], n))
            self.save(fig, "matched_shift_"+name)
        fig, ax, _ = self.axes(False)
        self.brand(ax, "Selection migration among matched stored events")
        matrix = np.asarray(self.paired["migration"]).reshape(2, 2)
        image = ax.imshow(matrix, cmap="Blues")
        for i in range(2):
            for j in range(2):
                percent = 100 * matrix[i,j] / max(1, matrix.sum())
                fraction = f"{percent:.2f}%" if percent >= 0.01 or percent == 0 else f"{percent:.2g}%"
                ax.text(j, i, f"{matrix[i,j]:,}\n{fraction}", ha="center", va="center", color="white" if matrix[i,j]>matrix.max()/2 else "black")
        ax.set_xticks([0, 1]);ax.set_yticks([0, 1])
        ax.set_xticklabels(["Fail", "Pass"]);ax.set_yticklabels(["Fail", "Pass"])
        ax.set_xlabel(self.labels["new"]);ax.set_ylabel(self.labels["old"])
        fig.colorbar(image, ax=ax, label="Events")
        self.save(fig, "matched_selection_migration")
        self.table("matched_threshold_migration", ["threshold_gev", "both_fail", "old_fail_new_pass", "old_pass_new_fail", "both_pass"],
                   ([t]+row for t, row in zip(self.d["thresholds"], self.paired["threshold_migration"])))
        self.profile_plots(True)
        if "maps" in self.groups:
            self.maps(True)
        for index, name in enumerate(("mjj", "pt_wj_j1", "pt_ak4_j1")):
            fig, ax, _ = self.axes(False)
            self.brand(ax, "Matched events passing both selections")
            rows = self.paired["run_shifts"]
            x = np.asarray([self.run_positions[r["run"]] for r in rows])
            values = [moments(r["moments"]) for r in rows]
            if values:
                n, mu, se = (np.asarray([v[k][index] for v in values]) for k in range(3))
                mask = n >= max(2, self.args.min_count)
                ax.errorbar(x[mask], mu[mask], yerr=se[mask], fmt="o", ms=6.0, color=COLORS["new"])
                self.table("matched_run_shift_"+name, ["run_index", "run", "count", "mean_shift_percent", "standard_error"], zip(x, [r["run"] for r in rows], n, mu, se))
            ax.axhline(0, color="0.3", ls="--");ax.set_ylabel(f"Mean {name} shift [%]")
            self.run_axis(ax, 1 if name.endswith("j1") else None)
            self.save(fig, "matched_run_shift_"+name)

    def report(self):
        info = {"dataset": self.d["dataset"], "config": self.d["config"], "partial": self.d["partial"],
                "coverage": coverage(self.d), "normalization": self.args.normalization,
                "run_stability": self.dijet_lumi_mode if self.has_lumi else "cross section unavailable without run luminosity",
                "luminosity_pb": self.total_lumi_pb,
                "mass_luminosity_source": "supplied era total" if self.luminosity.get("datasets_pb") or self.luminosity.get("total_pb") is not None else "sum of per-run luminosities",
                "run_integrated_luminosity_pb": float(np.nansum(self.lumi)) if self.has_lumi else None,
                "luminosity_estimated": bool(self.luminosity.get("estimated")),
                "luminosity_estimation": self.luminosity.get("estimation", []),
                "luminosity_sources": self.luminosity.get("sources", []),
                "dijet_run_normalization": self.dijet_lumi_mode,
                "dijet_run_formula": "N_dijets_run / luminosity_run_pb",
                "dijet_run_stat_uncertainty": "sqrt(N_dijets_run) / luminosity_run_pb",
                "samples": {role: {"input_entries": s.get("input_entries", s["cutflow"][0]),
                                   "certification_rejected": s.get("certification_rejected", 0),
                                   "pass_json_rejected": s.get("pass_json_rejected", 0),
                                   "stored": s["cutflow"][0], "base": s["cutflow"][-2], "selected": s["cutflow"][-1]}
                            for role, s in self.d["samples"].items()},
                "ratio_uncertainty": "Paired count ratios use measured event overlap; other ratios use an uncorrelated statistical approximation. Exact cross-JEC asymmetry covariance is not stored.",
                "eta_asymmetry_comparison": "A_new - A_old",
                "eta_asymmetry_comparison_stat_uncertainty": "sqrt(Var(A_new) + Var(A_old)); old/new covariance is not stored",
                "ratio_min_old_count": self.args.ratio_min_count,
                "mass_binning": self.mass_binning,
                "dijet_mass_cross_section_bins_gev": list(DIJET_MASS_BINS),
                "mass_display_range_tev": self.mass_range,
                "ht_display_range_tev": self.ht_range,
                "ht_binning": self.ht_binning,
                "pt_binning": self.pt_binning,
                "uniform_pt_bins_gev": list(UNIFORM_PT_BINS),
                "notes": ["Spectra show entries, normalized entries or cross sections per bin without division by bin width. Mass, pT and HT axes are in TeV; spectrum ROOT-table low/high retain source units and display_low/display_high give the plotted units.",
                          "Mass histograms use all saved bins before setting the display range: 0 to 8 TeV with cuts and 0 to 12 TeV for all/all_stored. Legacy counts are never split or interpolated.",
                          "All run axes use the shared run index; ROOT tables retain the physical run number. Stored-yield and selected-fraction figures are not produced.",
                          "One run_index_cross_section figure is produced. Its ROOT table records which run points are drawn and why others are omitted; the shared run-index mapping is never compressed.",
                          "Eta-asymmetry upper panels use [-0.7, 0.7] with statistical error bars. Their lower panels show A_new - A_old as black points without error bars and automatically include finite displayed differences with padding and a zero reference. Difference uncertainties remain in ROOT tables. A zero old asymmetry is valid; bins lacking sufficient entries in either sample are omitted.",
                          "Wide-jet pT, reconstructed raw pT and hemisphere pT histograms use uniform 100 GeV bins from 0 to 7000 GeV. Legacy variable-width bins cannot be split exactly; regenerate both summaries. Their incompatible figures are skipped and obsolete copies are removed.",
                          "Signed-eta asymmetry compares each bin with its reflected bin: [N(eta)-N(-eta)]/[N(eta)+N(-eta)]. Reflected points carry the opposite sign and are not independent measurements. The eta display range is [-3, 3].",
                          "Raw-pT spectra for J1 and J2 show saved AK4 pT divided by its saved JEC factor. This undoes the multiplicative correction; prior jet cleaning and corrected-pT rank ordering can still affect the sample.",
                          "Raw-pT lower panels show (new-old)/old with limits [-0.5, 0.5]. ROOT tables preserve both the original new/old ratio and fractional change, including values outside the display limits. The producer's corrected-pT thresholds and the selected-event cuts can change raw-pT acceptance.",
                          "HT is saved and plotted with the standard dijet edges (1 to 14000 GeV) and, separately, uniform 50 GeV bins (0 to 14000 GeV). Both display 0 to 14 TeV by default without automatic zoom. Legacy coarse bins cannot be split exactly and are skipped; regenerate both summaries.",
                          "JEC spectra show the saved dimensionless AK4 jetJecAK4PF_j1/j2 factors, not JEC uncertainties or event-level new/old correction ratios.",
                          "Mass thresholds count N(mjj > m_cut) after the preset's other requirements, scanning mass-cut acceptance rather than bin contents.",
                          "Marginal count errors are sqrt(N), normalized shape errors use binomial fractions, and profile errors are sample standard errors.",
                          "Cross sections describe the selected trigger/skim; no trigger-efficiency or luminosity-systematic correction is applied.",
                          "Luminosity must describe the full processed exposure, including certified sections with zero selected events.",
                          "Run cross sections use N_run/L_run in pb. Run Index is a categorical display coordinate; no bin-width factor enters. Mass differential cross sections divide by the mass width and use pb/TeV.",
                          "Run means and balance profiles are selection dependent; they do not measure an absolute jet response.",
                          "Mean JEC axes use [0.7, 1.3]. Mean mass/pT axes use [0, 0.5] TeV for all_stored and [0.5, 1.6] TeV for other presets. ROOT tables retain means outside the display limits.",
                          "Eta=0 is excluded from hemisphere asymmetries; leading and subleading jets are treated separately.",
                          "Stored-event fingerprints check input consistency probabilistically; they cannot recover events lost upstream."]}
        if self.paired:
            info["pairing"] = {k: self.paired[k] for k in ("matched", "only_old", "only_new", "jet_matched", "jet_unmatched", "reordered", "migration")}
        if self.qcd:
            info["qcd"] = {"label": self.args.mc_label, "scale": self.args.mc_scale,
                           "normalization": self.qcd["normalization"], "weight_convention": self.qcd["weight_convention"],
                           "selection": self.qcd["selection"],
                           "components": [{key: value for key, value in component.items() if key != "histograms"}
                                          for component in self.qcd["components"]]}
            info["notes"].extend([
                "One QCD prediction is shared by both JEC samples and all eras; combining eras never adds copies of the MC prediction. No normalization to data is applied unless shape normalization is requested; --mc-scale is common to both data comparisons.",
                "QCD uses unit generator weights with component weight xsec_pb * filter_efficiency / generated_events. Supplied cross sections that already include the generator filter use filter_efficiency=1. Saved MC histograms contain cross sections in pb and statistical variances in pb^2.",
                "Data/QCD point errors contain data statistics; the shaded band around one is the common QCD statistical uncertainty. ROOT tables also store quadrature total errors for individual ratios. The QCD uncertainty is correlated between the two data/QCD ratios and between run points; it cancels in their quotient. Existing new/old plots retain the appropriate data-only comparison.",
                "MC skips data certification and HLT requirements and uses the same kinematic selection. No trigger-efficiency, pileup, luminosity, cross-section or JEC systematic uncertainties are included.",
                "Normalized shape variances include the bin/total covariance and use all saved bins, including underflow and overflow. QCD run reference includes all selected mass bins, including underflow and overflow."])
        if self.mock:
            info["mock"] = self.mock
            info["notes"].insert(0, "Mock demonstration: the JEC differences are synthetic. Any supplied luminosity is artificial and the cross sections are illustrative.")
        if self.d["config"]["preset"] == "all":
            info["validity_counts"] = {role: {"stored": s["cutflow"][0], "valid_dijets": s["cutflow"][2],
                                              "without_valid_dijet": s["cutflow"][0]-s["cutflow"][2]}
                                       for role, s in self.d["samples"].items()}
            info["notes"].insert(0, "The all preset bypasses JSON, trigger, ID and jet-acceptance cuts, but every histogram uses valid stored dijets and the configured mass threshold. Upstream jet construction can depend on JECs; stored events without a valid dijet are counted separately.")
        if self.d["config"]["preset"] == "all_stored":
            info["validity_counts"] = {role: {"stored": s["cutflow"][0], "valid_dijets": s["valid_dijets"],
                                              "without_valid_dijet": s["cutflow"][0]-s["valid_dijets"]}
                                       for role, s in self.d["samples"].items()}
            rows = []
            for role, sample in self.d["samples"].items():
                stored = sample["cutflow"][0]
                for name, hist in sample["histograms"].items():
                    entries = sum(hist["counts"])
                    unavailable = hist.get("unavailable", 0)
                    if entries + hist["invalid"] + unavailable != stored:
                        raise ValueError(f"{self.d['dataset']}: inconsistent all-stored accounting for {role}/{name}")
                    rows.append([name, role, stored, entries, sum(hist["counts"][1:-1]),
                                 hist["counts"][0], hist["counts"][-1], hist["invalid"], unavailable])
            self.table("histogram_accounting", ["histogram", "jec", "stored", "entries", "visible_entries",
                                                "underflow", "overflow", "nonfinite", "unavailable"], rows)
            scope = "Every stored event passing the supplied run/LS mask" if self.d["config"].get("certification_mask") else "Every stored event"
            if self.d["config"].get("require_pass_json"):
                scope += " satisfying the explicitly requested saved PassJSON > 0.5 cut"
            info["notes"].insert(0, scope+" is retained by all_stored with no jet or trigger selection. Saved PassJSON is ignored unless --require-pass-json was explicitly supplied to the reducer. Event histograms use all retained stored values; jet observables use each available jet independently, and dijet observables require their own usable pair. Undefined values are not filled as zero. table_histogram_accounting includes unavailable and nonfinite counts for every histogram.")
            info["notes"].insert(1, "The all-stored dijet run plot uses recorded per-run exposure or an explicit LS-weighted estimate. Paired event-retention migration counts do not represent valid-dijet migration in this preset.")
        self.table("dataset_yields", ["jec", "stored", "base", "selected"],
                   ([role, s["cutflow"][0], s["cutflow"][-2], s["cutflow"][-1]] for role, s in self.d["samples"].items()))
        self.table("certification_accounting", ["jec", "input_entries", "retained_entries", "rejected_entries", "certification_rejected", "pass_json_rejected"],
                   ([role, s.get("input_entries", s["cutflow"][0]), s["cutflow"][0],
                     s.get("certification_rejected", 0)+s.get("pass_json_rejected", 0),
                     s.get("certification_rejected", 0), s.get("pass_json_rejected", 0)]
                    for role, s in self.d["samples"].items()))
        info["figures"] = [f"{fmt}/{name}.{fmt}" for name in self.figures for fmt in dict.fromkeys(self.args.formats)]
        info["root_file"] = str(self.args.root_file)
        info["root_directory"] = self.output.name
        info["root_tables"] = ["table_"+name for name in self.tables]
        info["notes"].append("Run dijet cross sections use recorded luminosity per run when supplied. Dataset totals produce LS-weighted estimates only with --estimate-run-lumi. N_run/L_era contribution points are no longer produced.")
        info["notes"].append("LS-weighted estimates assign L_run = L_dataset * N_distinct_LS_run/N_distinct_LS_dataset. Equal luminosity per LS is an unverified assumption; this does not constitute measured run luminosity or a validated run-stability result.")
        info["notes"].append("The dedicated standard-binned mass cross-section plot shows d sigma/d m in pb/TeV, with N/(bin_width_TeV * luminosity_pb). It is skipped for legacy mass bins because aggregate counts cannot be split exactly.")
        info["notes"].append("Eta-asymmetry lower panels display New - Old with black filled circles and no error bars. Other old/new comparison lower panels display (New-old)/old with capped vertical statistical errors. Asymmetry-difference uncertainties are saved using the uncorrelated approximation because old/new covariance is not stored.")
        info["notes"].append("Markers use ROOT size 1.0 and Matplotlib diameter 6.0 points. Upper-panel new markers remain open red circles. Histogram lines have no error-bar overlays. Metadata is stored only inside ROOT.")
        self.report_data = info

    def write_root(self):
        if self.root_part is None:
            raise ValueError("Missing temporary ROOT output destination")
        payload = {"directory": self.output.name, "summary": self.d, "report": self.report_data,
                   "tables": self.tables, "canvases": self.canvases}
        path = Path(self.root_part).with_suffix(".json")
        def numeric(value):
            if isinstance(value,np.generic):
                return value.item()
            raise TypeError(type(value).__name__)
        path.write_text(json.dumps(payload,default=numeric,separators=(",",":")))
        helper = Path(__file__).resolve()
        result = subprocess.run([self.args.root_python,str(helper),"--root-payload",str(path),"--output",str(self.root_part)],
                                capture_output=True,text=True)
        if result.returncode:
            raise ValueError("ROOT export failed: "+result.stderr[-4000:])

    def run(self):
        started = time.monotonic()
        name = self.d["dataset"]
        progress(f"Starting plots in {self.output}", name)
        counts = self.d["samples"]
        progress(f"Selected events: old={counts['old']['cutflow'][-1]:,}, new={counts['new']['cutflow'][-1]:,}", name)
        if self.d["config"]["preset"] == "all":
            progress("All preset retains valid stored dijets; stored events: "
                     + ", ".join(f"{role}={counts[role]['cutflow'][0]:,}" for role in ("old", "new")), name)
        if self.d["config"]["preset"] == "all_stored":
            progress("All-stored event filters: PassJSON " + ("enabled" if self.d["config"].get("require_pass_json") else "disabled") +
                     "; external run/LS mask " + ("enabled" if self.d["config"].get("certification_mask") else "disabled") +
                     "; no trigger, jet-ID or kinematic cuts", name)
        if self.d["config"].get("certification_mask") or self.d["config"].get("require_pass_json"):
            for role, sample in counts.items():
                progress(f"{role}: read={sample['input_entries']:,}, retained={sample['cutflow'][0]:,}, "
                         f"rejected by run/LS mask={sample.get('certification_rejected', 0):,}, "
                         f"rejected by PassJSON={sample.get('pass_json_rejected', 0):,}", name)
        progress("Run stability: " + ("cross section using " + self.dijet_lumi_mode if self.has_lumi else
                                      "cross sections need recorded per-run luminosity; retention plots disabled"), name)
        plan = [("spectra", self.spectra), ("xsec", self.mass_cross_section_plot), ("run", self.run_plots), ("eta", self.eta_plots),
                ("profiles", self.profile_plots), ("maps", self.maps), ("selection", self.selection)]
        if self.paired:
            plan.append(("paired", self.paired_plots))
        elif "paired" in self.groups:
            progress("Skipping matched-event plots: independent summaries", name)
        plan = [(group, action) for group, action in plan if group in self.groups]
        for index, (group, action) in enumerate(plan, 1):
            progress(f"Plot group {index}/{len(plan)}: {group}", name)
            action()
        progress("Writing editable ROOT histograms/canvases and metadata", name)
        self.report()
        self.write_root()
        progress(f"Completed {len(self.figures)} plots and {len(self.tables)} ROOT tables in {time.monotonic()-started:.1f} s: {self.output}", name)
        return str(self.root_part), len(self.figures)


def plot_job(job):
    d, luminosity, args, output, root_part = job
    return Plotter(d, luminosity, args, output, root_part).run()


def combine(datasets):
    names = sorted(datasets)
    if len({"paired" in d for d in datasets.values()}) != 1:
        raise ValueError("Cannot combine paired and independent datasets in one plot")
    observed = {"old": set(), "new": set()}
    for name in names:
        for role, sample in datasets[name]["samples"].items():
            keys = {tuple(row[:2]) for row in sample["lumisections"]}
            if observed[role] & keys:
                raise ValueError("Datasets overlap in run/lumisection; combine disjoint eras to avoid double-counting exposure")
            observed[role].update(keys)
    merged = copy.deepcopy(datasets[names[0]])
    for name in names[1:]:
        merge_document(merged, datasets[name], combine_certification=True)
    membership = {}
    for name in names:
        for sample in datasets[name]["samples"].values():
            for row in sample["runs"]:
                membership.setdefault(row["run"], set()).add(name)
    merged["run_eras"] = [[run, ", ".join(sorted(labels))] for run, labels in sorted(membership.items())]
    merged["run_era_members"] = [[run, sorted(labels)] for run, labels in sorted(membership.items())]
    merged["dataset_members"] = names
    merged["dataset"] = ", ".join(names)
    return merged


def main():
    started = time.monotonic()
    args = parser().parse_args()
    if args.jobs < 1 or args.min_count < 1 or args.ratio_min_count < 1 or args.dpi < 1 or not math.isfinite(args.energy_tev) or args.energy_tev <= 0:
        raise ValueError("jobs, min-count, ratio-min-count, dpi and energy must be positive")
    if not math.isfinite(args.mc_scale) or args.mc_scale <= 0:
        raise ValueError("mc-scale must be finite and positive")
    for name, limits in (("mass-range", args.mass_range), ("ht-range", args.ht_range)):
        if limits and (not all(math.isfinite(value) for value in limits) or limits[0] < 0 or limits[1] <= limits[0]):
            raise ValueError(f"{name} must be two finite increasing values in TeV with MIN >= 0")
    paths = expand_inputs(args.inputs)
    progress(f"Found {len(paths)} summary files")
    datasets = load_datasets(paths, args.datasets, args.allow_partial, args.mc_input)
    progress(f"Loaded {len(datasets)} datasets: {', '.join(sorted(datasets))}")
    if any(d["config"].get("certification_mask") for d in datasets.values()):
        configure_certification(datasets, args)
    if len({safe_name(name) for name in datasets}) != len(datasets):
        raise ValueError("Dataset names collide after conversion to directory names")
    if args.brilcalc and (args.lumi_csv or args.lumi_json or args.estimate_run_lumi):
        raise ValueError("--brilcalc may be combined with era totals in --lumi_pb, but not with recorded-luminosity files or --estimate-run-lumi")
    if args.brilcalc and args.no_auto_lumi:
        raise ValueError("Choose --brilcalc or --no-auto-lumi")
    groups = set(args.plots or ["spectra", "xsec", "run", "eta", "selection"])
    luminosity = add_luminosity_values(load_luminosity(args.lumi_csv+args.lumi_json, args.lumi_unit), args.lumi_pb)
    if args.brilcalc and luminosity["runs"]:
        raise ValueError("--brilcalc may be combined with an era total, but not with RUN:LUMI values")
    automatic_query = (not args.brilcalc and not args.no_auto_lumi and not luminosity["runs"]
                       and not args.estimate_run_lumi and not args.export_only
                       and ({"xsec", "run"} & groups or args.normalization == "xsec"))
    if automatic_query:
        cache = Path(args.output_dir)/"recorded_luminosity.json"
        if cache.is_file() and luminosity_cache_matches(cache, datasets, args):
            args.lumi_json.append(str(cache))
            luminosity = add_luminosity_values(load_luminosity(args.lumi_csv+args.lumi_json, args.lumi_unit), args.lumi_pb)
            progress(f"Reusing recorded luminosity: {cache}")
        elif any(d["partial"] for d in datasets.values()):
            progress("Partial summaries cannot produce cross sections; skipping the automatic BRIL query")
        else:
            if not shutil.which(args.brilcalc_bin):
                raise ValueError("Automatic per-run luminosity requires brilcalc on PATH or --brilcalc-bin PATH. Supply recorded luminosity with --lumi-json/--lumi-csv, or use --no-auto-lumi to explicitly plot without a BRIL query")
            progress("Per-run luminosity is missing; preparing an automatic calibrated BRIL query" +
                     ("; the supplied era total remains the mass-spectrum normalization" if args.lumi_pb else ""))
            configure_bril(datasets, args)
            args.brilcalc = True
    if args.brilcalc:
        configure_bril(datasets, args)
    if luminosity.get("total_pb") is not None and len(datasets) > 1 and args.per_dataset:
        raise ValueError("A single total cannot normalize individual datasets; use --lumi_pb DATASET:TOTAL for each dataset with --per-dataset")
    certified = configured_certification(args)
    if args.export_lumisections:
        export_sections(datasets,args.export_lumisections,certified)
    if args.export_only:
        if not args.export_lumisections:
            raise ValueError("--export-only requires --export-lumisections JSON")
        return
    if args.estimate_run_lumi and args.no_estimate_run_lumi:
        raise ValueError("Choose either --estimate-run-lumi or --no-estimate-run-lumi")
    if args.estimate_run_lumi:
        luminosity = estimate_luminosity(datasets,luminosity,certified)
    if args.brilcalc:
        luminosity = add_luminosity_values(fetch_bril_luminosity(datasets,args,certified), args.lumi_pb)
    if args.lumi_csv or args.lumi_json:
        progress(f"Loaded recorded luminosity for {len(luminosity['runs'])} runs from {len(args.lumi_csv)+len(args.lumi_json)} input files")
    if args.lumi_pb and not luminosity.get("estimated"):
        if luminosity.get("total_pb") is not None:
            progress(f"Dataset luminosity: {luminosity['total_pb']:g} pb^-1 for mass spectra; " +
                     (f"run plots use recorded luminosity for {len(luminosity['runs'])} runs" if luminosity["runs"] else "run cross sections need recorded per-run luminosity"))
        elif luminosity.get("datasets_pb"):
            progress("Dataset luminosities [pb^-1]: " + ", ".join(f"{name}={value:g}" for name, value in sorted(luminosity["datasets_pb"].items())))
        else:
            progress(f"Loaded recorded luminosity for {len(luminosity['runs'])} runs")
    output = Path(args.output_dir).resolve()
    args.root_file = str(Path(args.root_file).resolve() if args.root_file else output/"jec_comparison.root")
    preflight = subprocess.run([args.root_python,"-c","import ROOT; ROOT.gROOT.SetBatch(True)"],capture_output=True,text=True)
    if preflight.returncode:
        raise ValueError("ROOT output requires a Python interpreter with working PyROOT. Run cmsenv, or pass --root-python PATH. "+preflight.stderr[-1000:])
    jobs = []
    if len(datasets) > 1:
        jobs.append((combine(datasets), luminosity, args, output / "combined"))
        if args.per_dataset:
            jobs.extend((d, luminosity, args, output / safe_name(name)) for name, d in sorted(datasets.items()))
    else:
        name, d = next(iter(datasets.items()))
        jobs.append((d, luminosity, args, output / safe_name(name)))
    with tempfile.TemporaryDirectory(prefix="jec_root_") as temporary:
        jobs = [job+(str(Path(temporary)/f"part_{i}.root"),) for i,job in enumerate(jobs)]
        if len(jobs) > 1 and args.jobs > 1:
            progress(f"Generating {len(jobs)} plot sets with {min(args.jobs, len(jobs))} parallel jobs")
            with ProcessPoolExecutor(max_workers=min(args.jobs, len(jobs))) as executor:
                results = list(executor.map(plot_job, jobs))
        else:
            progress(f"Generating {len(jobs)} plot sets sequentially")
            results = [plot_job(job) for job in jobs]
        helper = Path(__file__).resolve()
        merged = subprocess.run([args.root_python,str(helper),"--root-merge"]+[path for path,count in results]+["--output",args.root_file],capture_output=True,text=True)
        if merged.returncode:
            raise ValueError("ROOT output merge failed: "+merged.stderr[-3000:])
        progress(f"Saved all dataset histograms, graphs, tables and canvases: {args.root_file}")
    progress(f"Finished: {sum(count for path, count in results)} plots in {time.monotonic()-started:.1f} s")


if __name__ == "__main__":
    try:
        main()
    except (ValueError, OSError, KeyError) as exc:
        print(f"Error: {exc}", file=sys.stderr)
        sys.exit(1)
