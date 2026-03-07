from optparse import OptionParser
import ROOT as rt
from rootTools import Utils
from framework import Config
from array import *
from itertools import *
from operator import *
from WriteDataCard import initializeWorkspace,convertToTh1xHist,convertToMjjHist
import os
import random
import sys
import math

densityCorr = False

COLOR_RESET = "\033[0m"
COLOR_INFO = "\033[1;92m"
COLOR_WARN = "\033[1;93m"
COLOR_ERROR = "\033[1;91m"

def _colorize(msg, color):
    if hasattr(sys.stdout, "isatty") and sys.stdout.isatty():
        return "%s%s%s" % (color, msg, COLOR_RESET)
    return msg

def _color_tag(level, color):
    return _colorize("[%s]" % level, color)

def log_info(message):
    print("%s %s" % (_color_tag("INFO", COLOR_INFO), message))

def log_warn(message):
    print("%s %s" % (_color_tag("WARN", COLOR_WARN), message))

def log_error(message):
    print("%s %s" % (_color_tag("ERROR", COLOR_ERROR), message), file=sys.stderr)

def _iter_roo_collection(coll):
    if coll is None:
        return
    if hasattr(coll, "getSize") and hasattr(coll, "at"):
        for i in range(int(coll.getSize())):
            obj = coll.at(i)
            if obj:
                yield obj
        return
    for obj in coll:
        yield obj

def _fit_parameter_summary(fr):
    """Return list of (name, init, final, err) from RooFitResult."""
    rows = []
    if fr is None or not hasattr(fr, "floatParsFinal"):
        return rows
    finals = fr.floatParsFinal()
    if finals is None:
        return rows
    init_map = {}
    if hasattr(fr, "floatParsInit"):
        inits = fr.floatParsInit()
        for p in _iter_roo_collection(inits):
            init_map[p.GetName()] = p.getVal()
    for p in _iter_roo_collection(finals):
        name = p.GetName()
        v_init = init_map.get(name, float("nan"))
        v_final = p.getVal()
        v_err = p.getError() if hasattr(p, "getError") else float("nan")
        rows.append((name, v_init, v_final, v_err))
    return rows

def _infer_nparfit_from_pdf_params(workspace_, box, zero_tol=1e-15):
    """Infer nParFit from pN_<box> parameters actually used by <box>_bkg PDF.
    Rule: exactly-zero pN is treated as inactive and stops the contiguous chain.
    """
    if workspace_ is None:
        return None
    bkg_pdf = workspace_.pdf('%s_bkg' % box)
    if bkg_pdf is None:
        return None

    obs = rt.RooArgSet()
    th1x = workspace_.var('th1x')
    if th1x is not None:
        obs.add(th1x)
    params = bkg_pdf.getParameters(obs)
    if params is None:
        return None

    suffix = "_%s" % box
    pvals = {}
    for p in _iter_roo_collection(params):
        name = p.GetName()
        if (not name.startswith("p")) or (not name.endswith(suffix)):
            continue
        idx_str = name[1:-len(suffix)]
        if not idx_str.isdigit():
            continue
        idx = int(idx_str)
        if idx < 2:
            continue
        try:
            pval = float(p.getVal())
        except Exception:
            wv = workspace_.var(name)
            if wv is None:
                continue
            try:
                pval = float(wv.getVal())
            except Exception:
                continue
        pvals[idx] = pval

    if len(pvals) == 0:
        return None

    if 2 not in pvals:
        return 3

    last_active = 1
    idx = 2
    while idx in pvals:
        v = pvals[idx]
        if (not math.isfinite(v)) or abs(v) <= zero_tol:
            break
        last_active = idx
        idx += 1
    return max(last_active + 1, 3)

def _infer_nparfit_from_pdf_class(workspace_, box):
    if workspace_ is None:
        return None
    bkg_pdf = workspace_.pdf('%s_bkg' % box)
    if bkg_pdf is None:
        return None
    cls = bkg_pdf.ClassName()
    prefix = 'RooDijet'
    suffix_cls = 'ParamBinPdf'
    if cls.startswith(prefix) and cls.endswith(suffix_cls):
        mid = cls[len(prefix):-len(suffix_cls)]
        if mid.isdigit():
            return int(mid)
    return None

def _infer_nparfit_from_workspace_vars(workspace_, box, zero_tol=1e-15):
    if workspace_ is None:
        return None
    suffix = "_%s" % box
    available_p = set()
    all_vars = workspace_.allVars() if hasattr(workspace_, "allVars") else None
    if not all_vars:
        return None
    for v in _iter_roo_collection(all_vars):
        name = v.GetName()
        if (not name.startswith("p")) or (not name.endswith(suffix)):
            continue
        idx_str = name[1:-len(suffix)]
        if not idx_str.isdigit():
            continue
        idx = int(idx_str)
        if idx < 2:
            continue
        try:
            pval = float(v.getVal())
        except Exception:
            continue
        if (not math.isfinite(pval)) or abs(pval) <= zero_tol:
            continue
        available_p.add(idx)
    if 2 not in available_p:
        return 3
    max_p_index = 2
    while (max_p_index + 1) in available_p:
        max_p_index += 1
    return max_p_index + 1

def _is_fit_good(fr, edm_max=1e-3):
    if fr is None:
        return False
    status = fr.status() if hasattr(fr, "status") else 999
    cov_qual = fr.covQual() if hasattr(fr, "covQual") else -1
    edm = fr.edm() if hasattr(fr, "edm") else 1e9
    return (status == 0) and (cov_qual >= 2) and (edm <= edm_max)

def _fit_rank_tuple(fr):
    # Prefer converged fits, then better covariance, then lower NLL.
    good = 0 if _is_fit_good(fr) else 1
    covq = fr.covQual() if hasattr(fr, "covQual") else -1
    nll = fr.minNll() if hasattr(fr, "minNll") else 1e99
    if not math.isfinite(nll):
        nll = 1e99
    return (good, -covq, nll)

def _set_var_value_clamped(var, value):
    if var is None or var.isConstant():
        return
    v = value
    try:
        vmin = var.getMin()
        vmax = var.getMax()
        if v < vmin:
            v = vmin
        if v > vmax:
            v = vmax
    except Exception:
        pass
    var.setVal(v)

def _apply_fit_result_to_workspace(workspace, fr):
    if fr is None or workspace is None or not hasattr(fr, "floatParsFinal"):
        return
    for p in _iter_roo_collection(fr.floatParsFinal()):
        wv = workspace.var(p.GetName())
        if wv is not None and (not wv.isConstant()):
            wv.setVal(p.getVal())
            if hasattr(wv, "setError") and hasattr(p, "getError"):
                wv.setError(p.getError())

def _capture_ranges(workspace, names):
    ranges = {}
    if workspace is None:
        return ranges
    for n in names:
        v = workspace.var(n)
        if v is None:
            continue
        try:
            ranges[n] = (v.getMin(), v.getMax())
        except Exception:
            pass
    return ranges

def _restore_ranges(workspace, ranges):
    if workspace is None:
        return
    for n, r in ranges.items():
        v = workspace.var(n)
        if v is not None:
            v.setRange(r[0], r[1])

def _widen_ranges(workspace, names, factor=2.0):
    if workspace is None:
        return
    for n in names:
        v = workspace.var(n)
        if v is None or v.isConstant():
            continue
        try:
            old_min = v.getMin()
            old_max = v.getMax()
            width = max(old_max - old_min, 1e-9)
            center = v.getVal()
            half = 0.5 * factor * width
            new_min = center - half
            new_max = center + half
            # Keep physically sensible domain for dijet shape parameters
            # when currently in positive region.
            if (n.startswith('p1_') or n.startswith('p2_')) and center > 0:
                new_min = max(new_min, 1e-4)
            v.setRange(new_min, new_max)
        except Exception:
            continue

def _run_fit_ladder(pdf, data, fitRange='Full', useWeight=False, strategy=2, eps=1e-6, suppress_root_errors=False):
    old_error_level = rt.gErrorIgnoreLevel
    if suppress_root_errors and old_error_level < rt.kBreak:
        rt.gErrorIgnoreLevel = rt.kBreak
    try:
        if useWeight:
            return pdf.fitTo(
                data,
                rt.RooFit.Range(fitRange),
                rt.RooFit.Extended(True),
                rt.RooFit.SumW2Error(True),
                rt.RooFit.Save(),
                rt.RooFit.Minimizer('Minuit2', 'migrad'),
                rt.RooFit.Strategy(strategy),
                rt.RooFit.PrintLevel(-1),
                rt.RooFit.Verbose(False),
            )

        nll = pdf.createNLL(data, rt.RooFit.Range(fitRange), rt.RooFit.Extended(True), rt.RooFit.Offset(True))
        m2 = rt.RooMinimizer(nll)
        m2.setStrategy(strategy)
        m2.setPrintLevel(-1)
        m2.setEps(eps)
        m2.setMaxFunctionCalls(100000)
        m2.setMaxIterations(100000)
        # Retry ladder requested: simplex -> migrad -> hesse.
        m2.minimize('Minuit2', 'simplex')
        m2.minimize('Minuit2', 'migrad')
        hesse_status = m2.minimize('Minuit2', 'hesse')
        if hesse_status != 3:
            m2.minimize('Minuit2', 'hesse')
        return m2.save()
    finally:
        if suppress_root_errors:
            rt.gErrorIgnoreLevel = old_error_level

def _get_seedable_vars(workspace, box):
    names = [
        'p1_%s' % box,
        'p2_%s' % box,
        'Ntot_bkg_%s' % box,
    ]
    out = []
    for n in names:
        v = workspace.var(n) if workspace is not None else None
        if v is not None and (not v.isConstant()):
            out.append(n)
    return out

def _build_multistart_seeds(workspace, seed_names, n_starts=9, rng_seed=12345):
    seeds = []
    base = {}
    for n in seed_names:
        base[n] = workspace.var(n).getVal()
    seeds.append(dict(base))

    rng = random.Random(rng_seed)
    for _ in range(max(0, n_starts - 1)):
        s = {}
        for n in seed_names:
            v = workspace.var(n)
            v0 = base[n]
            vmin = v.getMin()
            vmax = v.getMax()
            if 'Ntot' in n:
                # multiplicative spread for normalization
                u = rng.uniform(-0.45, 0.45)
                val = v0 * math.exp(u)
            else:
                span = max(abs(v0) * 0.30, 0.5)
                val = v0 + rng.uniform(-span, span)
                # Avoid pathological negative seeds once we are in positive regime.
                if (n.startswith('p1_') or n.startswith('p2_')) and v0 > 0 and val <= 0:
                    val = max(0.2 * v0, 1e-3)
            if val < vmin:
                val = vmin
            if val > vmax:
                val = vmax
            s[n] = val
        seeds.append(s)
    return seeds

def binnedFit(pdf, data, fitRange='Full', useWeight=False, robust=False, workspace=None, box=None, n_starts=9, rng_seed=12345, verbose=False):

    # Default path (existing behavior style, but with simplex->migrad->hesse ladder).
    if not robust:
        return _run_fit_ladder(pdf, data, fitRange, useWeight, strategy=2, eps=1e-6)

    # Robust path: multi-start + retry ladder.
    if workspace is None or box is None:
        log_warn("robust-fit requested but workspace/box missing, falling back to standard fit.")
        return _run_fit_ladder(pdf, data, fitRange, useWeight, strategy=2, eps=1e-6)

    seed_names = _get_seedable_vars(workspace, box)
    if len(seed_names) == 0:
        log_warn("robust-fit could not find p1/p2/Ntot vars, falling back to standard fit.")
        return _run_fit_ladder(pdf, data, fitRange, useWeight, strategy=2, eps=1e-6)

    base_ranges = _capture_ranges(workspace, seed_names)
    # Anchor with one standard fit first; robust seeds are generated around this point.
    anchor_fr = None
    try:
        anchor_fr = _run_fit_ladder(
            pdf, data, fitRange, useWeight,
            strategy=2, eps=1e-6,
            suppress_root_errors=True
        )
    except Exception as ex:
        log_warn("robust anchor fit failed, continuing with config seeds: %s" % str(ex))
    if anchor_fr is not None:
        _apply_fit_result_to_workspace(workspace, anchor_fr)
        if verbose:
            log_info(
                "anchor fit: status=%s covQual=%s edm=%.3g minNll=%.6g" % (
                    anchor_fr.status(),
                    anchor_fr.covQual(),
                    anchor_fr.edm(),
                    anchor_fr.minNll(),
                )
            )

    seeds = _build_multistart_seeds(workspace, seed_names, n_starts=n_starts, rng_seed=rng_seed)

    attempts = []
    ladder_cfgs = [
        {"strategy": 1, "eps": 1e-6, "widen": False},
        {"strategy": 2, "eps": 1e-6, "widen": False},
        {"strategy": 2, "eps": 1e-7, "widen": True},
    ]

    for i_seed, seed_vals in enumerate(seeds):
        for n, v in seed_vals.items():
            _set_var_value_clamped(workspace.var(n), v)

        if verbose:
            log_info("robust start %d/%d with seed: %s" % (i_seed + 1, len(seeds), seed_vals))

        best_this_seed = None
        for i_try, cfg in enumerate(ladder_cfgs):
            _restore_ranges(workspace, base_ranges)
            if cfg["widen"]:
                _widen_ranges(workspace, seed_names, factor=1.6)

            try:
                fr_try = _run_fit_ladder(
                    pdf,
                    data,
                    fitRange=fitRange,
                    useWeight=useWeight,
                    strategy=cfg["strategy"],
                    eps=cfg["eps"],
                    suppress_root_errors=True,
                )
            except Exception as ex:
                log_warn(
                    "robust start %d try %d failed: %s" % (
                        i_seed + 1,
                        i_try + 1,
                        str(ex),
                    )
                )
                continue

            if (best_this_seed is None) or (_fit_rank_tuple(fr_try) < _fit_rank_tuple(best_this_seed)):
                best_this_seed = fr_try

            if verbose:
                log_info(
                    "  try %d: status=%s covQual=%s edm=%.3g minNll=%.6g%s" % (
                        i_try + 1,
                        fr_try.status(),
                        fr_try.covQual(),
                        fr_try.edm(),
                        fr_try.minNll(),
                        " [good]" if _is_fit_good(fr_try) else "",
                    )
                )

            if _is_fit_good(fr_try):
                break

        _restore_ranges(workspace, base_ranges)
        if best_this_seed is not None:
            attempts.append(best_this_seed)

    if len(attempts) == 0:
        log_warn("robust-fit could not complete any attempt; falling back to standard fit.")
        return _run_fit_ladder(pdf, data, fitRange, useWeight, strategy=2, eps=1e-6, suppress_root_errors=True)

    good_attempts = [fr for fr in attempts if _is_fit_good(fr)]
    if len(good_attempts) > 0:
        best_fr = min(good_attempts, key=lambda fr: fr.minNll())
    else:
        best_fr = min(attempts, key=lambda fr: fr.minNll())
        log_warn("robust-fit found no fully good covariance/status solution; using lowest minNll candidate.")

    _apply_fit_result_to_workspace(workspace, best_fr)

    log_info(
        "Robust fit selected: starts=%d, good=%d, status=%s, covQual=%s, edm=%.3g, minNll=%.6g" % (
            len(seeds),
            len(good_attempts),
            best_fr.status(),
            best_fr.covQual(),
            best_fr.edm(),
            best_fr.minNll(),
        )
    )
    return best_fr

def convertSideband(name,w,x):
    if name=="Full":
        return "Full"
    names = name.split(',')
    nBins = (len(x)-1)
    iBinX = -1
    sidebandBins = []
    for ix in range(1,len(x)):
        iBinX+=1
        w.var('mjj').setVal((x[ix]+x[ix-1])/2.)
        inSideband = 0
        for fitname in names:
            inSideband += ( w.var('mjj').inRange(fitname) )
        if inSideband: sidebandBins.append(iBinX)

    sidebandGroups = []
    for k, g in groupby(enumerate(sidebandBins), lambda i,x:i-x):
        consecutiveBins = map(itemgetter(1), g)
        sidebandGroups.append([consecutiveBins[0],consecutiveBins[-1]+1])
        
    newsidebands = ''
    nameNoComma = name.replace(',','')
        
    for iSideband, sidebandGroup in enumerate(sidebandGroups):
        if not w.var('th1x').hasRange('%s%i'%(nameNoComma,iSideband)):
            w.var('th1x').setRange("%s%i"%(nameNoComma,iSideband),sidebandGroup[0],sidebandGroup[1])
        newsidebands+='%s%i,'%(nameNoComma,iSideband)
    newsidebands = newsidebands[:-1]
    return newsidebands

def convertFunctionToHisto(background_,name_,N_massBins_,massBins_):

    background_hist_ = rt.TH1D(name_,name_,N_massBins_,massBins_)

    for bin in range (0,N_massBins_):
        xbinLow = massBins_[bin]
        xbinHigh = massBins_[bin+1]
        binWidth_current = xbinHigh - xbinLow
        value = background_.Integral(xbinLow , xbinHigh) / binWidth_current
        background_hist_.SetBinContent(bin+1,value)

    return background_hist_

def calculateChi2AndFillResiduals(data_obs_TGraph_,background_hist_,hist_fit_residual_vsMass_,workspace_,prinToScreen_=0):
    
    N_massBins_ = data_obs_TGraph_.GetN()
    MinNumEvents = 10
    # Determine nParFit robustly from actual PDF parameters first.
    npar_from_p = _infer_nparfit_from_pdf_params(workspace_, box)
    npar_from_pdf = _infer_nparfit_from_pdf_class(workspace_, box)
    npar_from_ws = _infer_nparfit_from_workspace_vars(workspace_, box)
    npar_override = None
    npar_var = workspace_.var('nPar_%s' % box)
    if npar_var is not None:
        try:
            npar_val = int(round(float(npar_var.getVal())))
            if npar_val > 0:
                npar_override = npar_val
        except Exception:
            pass

    candidates = []
    if npar_from_p is not None:
        candidates.append(npar_from_p)
    if npar_from_pdf is not None:
        candidates.append(npar_from_pdf)
    if npar_from_ws is not None:
        candidates.append(npar_from_ws)
    if npar_override is not None:
        candidates.append(npar_override)
    nParFit = min(candidates) if candidates else 4

    chi2_FullRangeAll = 0
    chi2_PlotRangeAll = 0
    chi2_PlotRangeNonZero = 0
    chi2_PlotRangeMinNumEvents = 0 

    N_FullRangeAll = 0
    N_PlotRangeAll = 0
    N_PlotRangeNonZero = 0
    N_PlotRangeMinNumEvents = 0

    for bin in range (0,N_massBins_):
        ## Values and errors
        value_data = data_obs_TGraph_.GetY()[bin]
        err_low_data = data_obs_TGraph_.GetEYlow()[bin]
        err_high_data = data_obs_TGraph_.GetEYhigh()[bin]
        xbinCenter = data_obs_TGraph_.GetX()[bin] 
        xbinLow = data_obs_TGraph_.GetX()[bin]-data_obs_TGraph_.GetEXlow()[bin] 
        xbinHigh = data_obs_TGraph_.GetX()[bin]+data_obs_TGraph_.GetEXhigh()[bin]
        binWidth_current = xbinHigh - xbinLow
        #value_fit = background_.Integral(xbinLow , xbinHigh) / binWidth_current
        value_fit = background_hist_.GetBinContent(bin+1)
        
        ## Fit residuals
        err_tot_data = 0
        if (value_fit > value_data):
            err_tot_data = err_high_data  
        else:
            err_tot_data = err_low_data
        # Guard against empty bins where low/high errors can both be zero.
        if err_tot_data <= 0:
            err_tot_data = max(err_low_data, err_high_data)
        plotRegions = plotRegion.split(',')
        checkInRegions = [xbinCenter>workspace_.var('mjj').getMin(reg) and xbinCenter<workspace_.var('mjj').getMax(reg) for reg in plotRegions]
        if any(checkInRegions) and err_tot_data > 0:
            fit_residual = (value_data - value_fit) / err_tot_data
            err_fit_residual = 1
        else:
            fit_residual = 0
            err_fit_residual = 0

        ## Fill histo with residuals

        hist_fit_residual_vsMass_.SetBinContent(bin+1,fit_residual)
        hist_fit_residual_vsMass_.SetBinError(bin+1,err_fit_residual)

        ## Chi2

        chi2_FullRangeAll += pow(fit_residual,2)
        N_FullRangeAll += 1        
        plotRegions = plotRegion.split(',')
        checkInRegions = [xbinCenter>workspace_.var('mjj').getMin(reg) and xbinCenter<workspace_.var('mjj').getMax(reg) for reg in plotRegions]
        if any(checkInRegions):
            #print ('%i: obs %.0f, exp %.2f, chi2 %.2f'%(bin, value_data* binWidth_current * lumi, value_fit* binWidth_current * lumi, pow(fit_residual,2)))
            chi2_PlotRangeAll += pow(fit_residual,2)
            N_PlotRangeAll += 1
            if (value_data > 0):
                chi2_PlotRangeNonZero += pow(fit_residual,2)
                N_PlotRangeNonZero += 1
                if(value_data * binWidth_current * lumi > MinNumEvents):
                    chi2_PlotRangeMinNumEvents += pow(fit_residual,2)
                    N_PlotRangeMinNumEvents += 1
    
    #==================
    # Calculate chi2/ndf
    #==================

    # ndf
    ndf_FullRangeAll = N_FullRangeAll - nParFit    
    ndf_PlotRangeAll = N_PlotRangeAll - nParFit    
    ndf_PlotRangeNonZero = N_PlotRangeNonZero - nParFit    
    ndf_PlotRangeMinNumEvents = N_PlotRangeMinNumEvents - nParFit    

    chi2_ndf_FullRangeAll = chi2_FullRangeAll / ndf_FullRangeAll
    chi2_ndf_PlotRangeAll = chi2_PlotRangeAll / ndf_PlotRangeAll
    chi2_ndf_PlotRangeNonZero = chi2_PlotRangeNonZero / ndf_PlotRangeNonZero
    chi2_ndf_PlotRangeMinNumEvents = chi2_PlotRangeMinNumEvents / ndf_PlotRangeMinNumEvents

    return [chi2_FullRangeAll, ndf_FullRangeAll, chi2_PlotRangeAll, ndf_PlotRangeAll, chi2_PlotRangeNonZero, ndf_PlotRangeNonZero, chi2_PlotRangeMinNumEvents, ndf_PlotRangeMinNumEvents]

def calculatePoissonDeviance(data_hist_, pred_hist_, workspace_):
    """Poisson deviance D = 2 * sum(mu - n + n*ln(n/mu)) in plot region."""
    if data_hist_ is None or pred_hist_ is None:
        return 0.0, 0

    n_bins = min(data_hist_.GetNbinsX(), pred_hist_.GetNbinsX())
    plotRegions = plotRegion.split(',')
    deviance = 0.0
    n_used = 0

    for ib in range(1, n_bins + 1):
        xcenter = data_hist_.GetXaxis().GetBinCenter(ib)
        in_plot = [xcenter > workspace_.var('mjj').getMin(reg) and xcenter < workspace_.var('mjj').getMax(reg) for reg in plotRegions]
        if not any(in_plot):
            continue

        n = float(data_hist_.GetBinContent(ib))
        mu = float(pred_hist_.GetBinContent(ib))
        if mu <= 0.0:
            mu = 1e-12

        if n > 0.0:
            deviance += 2.0 * (mu - n + n * math.log(n / mu))
        else:
            deviance += 2.0 * mu
        n_used += 1

    return deviance, n_used


if __name__ == '__main__':
    parser = OptionParser()
    parser.add_option('-c','--config',dest="config",type="string",default="config/run2.config",
                  help="Name of the config file to use")
    parser.add_option('-d','--dir',dest="outDir",default="./",type="string",
                  help="Output directory to store cards")
    parser.add_option('-l','--lumi',dest="lumi", default=1.,type="float",
                  help="integrated luminosity in pb^-1")
    parser.add_option('-b','--box',dest="box", default="CaloDijet",type="string",
                  help="box name")
    parser.add_option('--no-fit',dest="noFit",default=False,action='store_true',
                  help="Turn off fit (useful for visualizing initial parameters)")
    parser.add_option('--fit-region',dest="fitRegion",default="Full",type="string",
                  help="Fit region")
    parser.add_option('--plot-region',dest="plotRegion",default="Full",type="string",
                  help="Plot region")
    parser.add_option('-i','--input-fit-file',dest="inputFitFile", default=None,type="string",
                  help="input fit file")
    parser.add_option('-w','--weight',dest="useWeight",default=False,action='store_true',
                  help="use weight")
    parser.add_option('-s','--signal',dest="signalFileName", default=None,type="string",
                  help="input dataset file for signal pdf")
    parser.add_option('-m','--model',dest="model", default="gg",type="string",
                  help="signal model")
    parser.add_option('--mass',dest="mass", default="750",type="string",
                  help="mgluino")
    parser.add_option('--xsec',dest="xsec", default="1",type="string",
                  help="cross section in pb")
    parser.add_option('--signal-display-scale',dest="signalDisplayScale", default=None,type="float",
                  help="optional visual-only scale factor for signal overlays in ratio panel, e.g. 100")
    parser.add_option('--fit-spectrum',dest="doSpectrumFit", default=False,action='store_true',
                  help="fit spectrum")
    parser.add_option('--multi',dest="multi", default=True,action='store_true',
                  help="multiple background pdfs")
    parser.add_option('--linearX',dest="linearX", default=False,action='store_true',
                  help="linear X axis")
    parser.add_option('--year',dest="year", default="",type="string",
                  help="data-taking period label to be shown in legend, e.g. 2024H")
    parser.add_option('--verbose',dest="verbose", default=False,action='store_true',
                  help="print detailed fit diagnostics")
    parser.add_option('--robust-fit',dest="robustFit", default=False,action='store_true',
                  help="enable robust fit mode: multi-start seeds + retry ladder")
    parser.add_option('--robust-nstarts',dest="robustNStarts", default=9,type="int",
                  help="number of multi-start seeds for --robust-fit")
    parser.add_option('--robust-seed',dest="robustSeed", default=12345,type="int",
                  help="RNG seed for --robust-fit multi-start generation")

    rt.RooMsgService.instance().setGlobalKillBelow(rt.RooFit.FATAL)
    rt.RooMsgService.instance().setSilentMode(True)
    rt.Math.MinimizerOptions.SetDefaultPrintLevel(-1)
    rt.gROOT.SetBatch(True)
    rt.gErrorIgnoreLevel = rt.kWarning
    rt.gStyle.SetPaintTextFormat('+.2f')

    (options,args) = parser.parse_args()

    # Ensure output directory exists before any ROOT/text output operations.
    if options.outDir and not os.path.isdir(options.outDir):
        os.makedirs(options.outDir, exist_ok=True)
    
    cfg = Config.Config(options.config)
    
    box = options.box
    year_label = options.year
    lumi = options.lumi
    signal_display_scale = 1.0 if options.signalDisplayScale is None else float(options.signalDisplayScale)
    if signal_display_scale <= 0.0:
        log_error("--signal-display-scale must be > 0")
        raise ValueError("--signal-display-scale must be > 0")
    noFit = options.noFit
    fitRegion = options.fitRegion
    plotRegion = options.plotRegion
    histoName = cfg.getVariables(box, "histoName")

    if options.signalFileName==None:
        signalFileNames = []
        models = []
        xsecs = []
        colors = []
        styles = []
        masses = []
    else:
        signalFileNames = options.signalFileName.split(",")
        models = options.model.split("_")
        masses = options.mass.split("_")
        xsecs = options.xsec.split("_")
        colors = [rt.kBlue+1, rt.kCyan+1, rt.kViolet+1]
        styles = [2, 4, 6]

    if options.verbose:
        log_info("signal files: %s" % signalFileNames)
        log_info("signal models: %s" % models)
        log_info("signal masses: %s" % masses)
        log_info("signal xsecs: %s" % xsecs)
        if signal_display_scale != 1.0:
            log_info("signal display scale: x%.6g" % signal_display_scale)


    myTH1 = None
    for f in args:
        if f.lower().endswith('.root'):
            rootFile = rt.TFile(f)
            names = [k.GetName() for k in rootFile.GetListOfKeys()]
            if histoName in names:
                myTH1 = rootFile.Get(histoName)
    if myTH1 is None:
        log_error("Input ROOT histogram '%s' was not found." % histoName)
        raise RuntimeError("Input ROOT histogram '%s' was not found." % histoName)

    w = rt.RooWorkspace("w"+box)

    paramNames, bkgs = initializeWorkspace(w,cfg,box,multi=options.multi,printWorkspace=options.verbose)
        
    if options.inputFitFile is not None:
        inputRootFile = rt.TFile.Open(options.inputFitFile,"r")
        wIn = inputRootFile.Get("w"+box).Clone("wIn"+box)            
        if wIn.obj("fitresult_extDijetPdf_data_obs") != None:
            frIn = wIn.obj("fitresult_extDijetPdf_data_obs")
        elif wIn.obj("nll_extDijetPdf_data_obs") != None:
            frIn = wIn.obj("nll_extDijetPdf_data_obs")
        elif wIn.obj("fitresult_extDijetPdf_data_obs_with_constr") != None:
            frIn = wIn.obj("fitresult_extDijetPdf_data_obs_with_constr")
        elif wIn.obj("nll_extDijetPdf_data_obs_with_constr") != None:
            frIn = wIn.obj("nll_extDijetPdf_data_obs_with_constr")
        elif wIn.obj("simNll") != None:
            frIn = wIn.obj("simNll")
                        
        if options.verbose:
            log_info("restoring parameters from fit")
            frIn.Print("V")
        for p in frIn.floatParsFinal():
            w.var(p.GetName()).setVal(p.getVal())
            w.var(p.GetName()).setError(p.getError())

    
    x = array('d', cfg.getBinning(box)[0]) # mjj binning
    
    th1x = w.var('th1x')
    nBins = (len(x)-1)
    th1x.setBins(nBins)

    sideband = convertSideband(fitRegion,w,x)
    plotband = convertSideband(plotRegion,w,x)

    extDijetPdf = w.pdf('extDijetPdf')

    myTH1.Rebin(len(x)-1,'data_obs_rebin',x)
    myRebinnedTH1 = rt.gDirectory.Get('data_obs_rebin')
    myRebinnedTH1.SetDirectory(0)
    
    myRealTH1 = convertToTh1xHist(myRebinnedTH1)        
    
    dataHist = rt.RooDataHist("data_obs","data_obs",rt.RooArgList(th1x), rt.RooFit.Import(myRealTH1))
    if options.verbose:
        dataHist.Print('v')
    
    
    Utils.importToWS(w,dataHist)

    if noFit and options.inputFitFile is not None:
        fr = frIn
        if options.verbose:
            fr.Print('v')
        Utils.importToWS(w, fr)
    elif noFit:
        fr = rt.RooFitResult()
    else:
        rt.gStyle.SetOptStat(0)
        if options.doSpectrumFit:
            fr = binnedFit(
                extDijetPdf,
                dataHist,
                sideband,
                options.useWeight,
                robust=options.robustFit,
                workspace=w,
                box=box,
                n_starts=options.robustNStarts,
                rng_seed=options.robustSeed,
                verbose=options.verbose,
            )
            Utils.importToWS(w, fr)
            if options.verbose:
                fr.Print('v')
                fr.Print()
                fr.covarianceMatrix().Print('v')
                fr.correlationMatrix().Print('v')
            corrHist = fr.correlationHist('correlation_matrix')
            corrCanvas = rt.TCanvas('c_corr', 'c_corr', 500, 500)
            corrCanvas.SetRightMargin(0.15)
            corrHist.Draw('colztext')
            corrCanvas.Print(options.outDir + '/corrHist.pdf')
            corrCanvas.Print(options.outDir + '/corrHist.C')
        else:
            fr = rt.RooFitResult()
            

    
    # get signal histo if any
    signalHistos = []
    signalHistosOriginal = []
    signalHistosRebin = []
    signalFiles = []
    for model, mass, xsec, signalFileName in zip(models,masses,xsecs,signalFileNames):
        if options.verbose:
            log_info("signal input: %s %s %s %s" % (model, mass, xsec, signalFileName))
        signalFile = rt.TFile.Open(signalFileName)
        signalFiles.append(signalFile)
        names = [k.GetName() for k in signalFile.GetListOfKeys()]
        for name in names:
            d = signalFile.Get(name)
            if isinstance(d, rt.TH1):
                if name=='h_%s_%i'%(model,float(mass)):
                    if options.verbose:
                        log_info("using signal hist: %s" % name)
                    d.Scale(float(xsec)*lumi/d.Integral())
                    d.Rebin(len(x)-1,name+'_rebin',x)
                    d_rebin = rt.gDirectory.Get(name+'_rebin')
                    d_rebin.SetDirectory(0)
    
                    signalHistosOriginal.append(d)
                    signalHistosRebin.append(d_rebin)
    
                    d_th1x = convertToTh1xHist(d_rebin)
                    signalHistos.append(d_th1x)        

    asimov = extDijetPdf.generateBinned(rt.RooArgSet(th1x),rt.RooFit.Name('central'),rt.RooFit.Asimov())
        
    opt = [rt.RooFit.CutRange(myRange) for myRange in plotband.split(',')]
    asimov_reduce = asimov.reduce(opt[0])
    dataHist_reduce = dataHist.reduce(opt[0])
    for iOpt in range(1,len(opt)):
        asimov_reduce.add(asimov.reduce(opt[iOpt]))
        dataHist_reduce.add(dataHist.reduce(opt[iOpt]))


        
    rss = 0
    for i in range(0,len(x)-1):
        th1x.setVal(i+0.5)
        predYield = asimov.weight(rt.RooArgSet(th1x))
        dataYield = dataHist_reduce.weight(rt.RooArgSet(th1x))
        rss += float(predYield-dataYield) * float(predYield-dataYield)
        if options.verbose:
            print ("%i <= mjj < %i; prediction: %.2f; data %i"  % (x[i],x[i+1],predYield,dataYield))
    if options.verbose:
        print ("RSS = ", rss)
        
    rt.TH1D.SetDefaultSumw2()
    
    # start writing output
    rt.gStyle.SetOptStat(0)
    rt.gStyle.SetOptTitle(0)
    c = rt.TCanvas('c_fit','c_fit',700,800)
    root_plot_path = os.path.join(options.outDir, 'Plots_%s.root' % box)
    rootFile = rt.TFile.Open(root_plot_path, 'recreate')
    if (not rootFile) or rootFile.IsZombie():
        log_error("Failed to open output ROOT file: %s" % root_plot_path)
        raise OSError("Failed to open output ROOT file: %s" % root_plot_path)
        
    h_th1x = asimov.createHistogram('h_th1x',th1x)
    h_data_th1x = dataHist_reduce.createHistogram('h_data_th1x',th1x)
    
    boxLabel = "%s %s Fit" % (box,fitRegion)
    plotLabel = "%s Projection" % (plotRegion)

    background_pdf = w.pdf('%s_bkg_unbin'%box)
    background = background_pdf.asTF(rt.RooArgList(w.var('mjj')), rt.RooArgList(w.var('p0_%s'%box)))
    int_b = background.Integral(w.var('mjj').getMin(), w.var('mjj').getMax())
    p0_b = w.var('Ntot_bkg_%s'%box).getVal() / (int_b * lumi)

    # Plot cross section in pb/TeV.
    y_unit_scale = 1.0e3
    background.SetParameter(0, p0_b * y_unit_scale)

    g_data = rt.TGraphAsymmErrors(myRebinnedTH1)
    alpha = 1 - 0.6827
    for i in range(0, g_data.GetN()):
        N = g_data.GetY()[i]
        binWidth = g_data.GetEXlow()[i] + g_data.GetEXhigh()[i]
        if N <= 0.0:
            # Hide empty-bin markers and error bars (requested behavior).
            g_data.SetPoint(i, g_data.GetX()[i], 0.0)
            g_data.SetPointEXlow(i, 0.0)
            g_data.SetPointEXhigh(i, 0.0)
            g_data.SetPointEYlow(i, 0.0)
            g_data.SetPointEYhigh(i, 0.0)
        else:
            L = rt.Math.gamma_quantile(alpha/2.0, N, 1.0)
            U = rt.Math.gamma_quantile_c(alpha/2.0, N + 1, 1.0)
            g_data.SetPointEYlow(i, y_unit_scale * (N - L) / (binWidth * lumi))
            g_data.SetPointEYhigh(i, y_unit_scale * (U - N) / (binWidth * lumi))
            g_data.SetPoint(i, g_data.GetX()[i], y_unit_scale * N / (binWidth * lumi))

        plotRegions = plotRegion.split(',')
        in_plot = [g_data.GetX()[i] > w.var('mjj').getMin(reg) and g_data.GetX()[i] < w.var('mjj').getMax(reg) for reg in plotRegions]
        if not any(in_plot):
            g_data.SetPointEYlow(i, 0.0)
            g_data.SetPointEYhigh(i, 0.0)
            g_data.SetPointEXlow(i, 0.0)
            g_data.SetPointEXhigh(i, 0.0)
            g_data.SetPoint(i, g_data.GetX()[i], 0.0)

    h_th1x_counts = h_th1x.Clone("h_th1x_counts")
    h_th1x_counts.SetDirectory(0)
    h_background_counts = convertToMjjHist(h_th1x_counts, x)
    poisson_deviance, poisson_bins = calculatePoissonDeviance(myRebinnedTH1, h_background_counts, w)
    poisson_deviance_per_bin = (poisson_deviance / poisson_bins) if poisson_bins > 0 else 0.0

    h_th1x.Scale(y_unit_scale / lumi)
    h_background = convertToMjjHist(h_th1x, x)
    h_fit_residual_vs_mass = rt.TH1D("h_fit_residual_vs_mass", "h_fit_residual_vs_mass", len(x)-1, x)
    list_chi2AndNdf_background = calculateChi2AndFillResiduals(g_data, h_background, h_fit_residual_vs_mass, w, 0)

    # Build x-axis coordinates for plotting.
    x_tev = array('d', [edge / 1000.0 for edge in x])
    n_plot_bins = len(x) - 1
    # In --linearX mode we keep true mass coordinates and only disable log-x.
    x_plot_edges = x_tev
    x_axis_title = "Dijet mass [TeV]"

    h_background_plot = rt.TH1D("h_background_plot", "", n_plot_bins, x_plot_edges)
    h_residual_plot = rt.TH1D("h_residual_plot", "", n_plot_bins, x_plot_edges)
    for ib in range(1, n_plot_bins + 1):
        h_background_plot.SetBinContent(ib, h_background.GetBinContent(ib))
        h_background_plot.SetBinError(ib, h_background.GetBinError(ib))
        h_residual_plot.SetBinContent(ib, h_fit_residual_vs_mass.GetBinContent(ib))
        h_residual_plot.SetBinError(ib, h_fit_residual_vs_mass.GetBinError(ib))

    g_data_plot = rt.TGraphAsymmErrors(g_data.GetN())
    for i in range(g_data.GetN()):
        x_center = g_data.GetX()[i] / 1000.0
        ex_low = g_data.GetEXlow()[i] / 1000.0
        ex_high = g_data.GetEXhigh()[i] / 1000.0

        g_data_plot.SetPoint(i, x_center, g_data.GetY()[i])
        g_data_plot.SetPointEXlow(i, ex_low)
        g_data_plot.SetPointEXhigh(i, ex_high)
        g_data_plot.SetPointEYlow(i, g_data.GetEYlow()[i])
        g_data_plot.SetPointEYhigh(i, g_data.GetEYhigh()[i])

    g_data_plot.SetMarkerStyle(20)
    g_data_plot.SetMarkerSize(0.9)
    g_data_plot.SetMarkerColor(rt.kBlack)
    g_data_plot.SetLineColor(rt.kBlack)

    h_background_plot.SetLineColor(rt.kRed)
    h_background_plot.SetLineWidth(2)
    h_background_plot.SetFillStyle(0)

    # Build signal shape overlays (top panel and ratio panel).
    g_signals = []
    g_signal_residuals = []
    for model, mass, xsec, signalFileName, sigHist, color, style in zip(models, masses, xsecs, signalFileNames, signalHistosRebin, colors, styles):
        n_sig_bins = sigHist.GetNbinsX()
        g_signal = rt.TGraph(n_sig_bins)
        g_signal_residual = rt.TGraph(n_sig_bins)
        sig_mass_centers_gev = []
        for ib in range(1, n_sig_bins + 1):
            bw = sigHist.GetBinWidth(ib)
            xlow = sigHist.GetBinLowEdge(ib) / 1000.0
            xhigh = (sigHist.GetBinLowEdge(ib) + bw) / 1000.0
            xc = 0.5 * (xlow + xhigh)
            yv = 0.0
            if bw > 0:
                yv = y_unit_scale * sigHist.GetBinContent(ib) / (bw * lumi)
            g_signal.SetPoint(ib - 1, xc, yv)
            err_data = g_data.GetEYhigh()[ib - 1] if (ib - 1) < g_data.GetN() else 0.0
            y_res = ((yv * signal_display_scale) / err_data) if err_data > 0.0 else 0.0
            g_signal_residual.SetPoint(ib - 1, xc, y_res)
            sig_mass_centers_gev.append(sigHist.GetBinCenter(ib))

        # Match original script behavior: clip left/right tails to the
        # values at ~0.75*m and ~1.25*m to keep compact signal shapes.
        mass_gev = float(mass)
        firstX = 0.0
        firstY = 0.0
        lastX = 0.0
        lastY = 0.0
        notSet = True
        for i in range(g_signal.GetN()):
            x_plot = g_signal.GetX()[i]
            x_gev = sig_mass_centers_gev[i]
            if x_gev > mass_gev * 0.75 and notSet:
                firstX = x_plot
                firstY = g_signal.GetY()[i]
                notSet = False
        for i in range(g_signal.GetN()):
            x_plot = g_signal.GetX()[i]
            y_plot = g_signal.GetY()[i]
            x_gev = sig_mass_centers_gev[i]
            if notSet:
                break
            if x_gev <= mass_gev * 0.75:
                g_signal.SetPoint(i, firstX, firstY)
            else:
                g_signal.SetPoint(i, x_plot, y_plot)
            if x_gev > mass_gev * 1.25:
                g_signal.SetPoint(i, lastX, lastY)
            else:
                lastX = g_signal.GetX()[i]
                lastY = g_signal.GetY()[i]

        # Apply identical tail clipping for signal overlays in ratio panel.
        firstX = 0.0
        firstY = 0.0
        lastX = 0.0
        lastY = 0.0
        notSet = True
        for i in range(g_signal_residual.GetN()):
            x_plot = g_signal_residual.GetX()[i]
            x_gev = sig_mass_centers_gev[i]
            if x_gev > mass_gev * 0.75 and notSet:
                firstX = x_plot
                firstY = g_signal_residual.GetY()[i]
                notSet = False
        for i in range(g_signal_residual.GetN()):
            x_plot = g_signal_residual.GetX()[i]
            y_plot = g_signal_residual.GetY()[i]
            x_gev = sig_mass_centers_gev[i]
            if notSet:
                break
            if x_gev <= mass_gev * 0.75:
                g_signal_residual.SetPoint(i, firstX, firstY)
            else:
                g_signal_residual.SetPoint(i, x_plot, y_plot)
            if x_gev > mass_gev * 1.25:
                g_signal_residual.SetPoint(i, lastX, lastY)
            else:
                lastX = g_signal_residual.GetX()[i]
                lastY = g_signal_residual.GetY()[i]

        g_signal.SetLineColor(color)
        g_signal.SetLineStyle(style)
        g_signal.SetLineWidth(3)
        g_signal_residual.SetLineColor(color)
        g_signal_residual.SetLineStyle(style)
        g_signal_residual.SetLineWidth(3)
        g_signals.append(g_signal)
        g_signal_residuals.append(g_signal_residual)

    # Dynamic top-panel y-range with explicit headroom for text boxes.
    y_min_candidates = []
    y_max_candidates = []
    for i in range(g_data_plot.GetN()):
        y = g_data_plot.GetY()[i]
        if y > 0:
            y_min_candidates.append(max(y - g_data_plot.GetEYlow()[i], 1e-12))
            y_max_candidates.append(y + g_data_plot.GetEYhigh()[i])
    for ib in range(1, n_plot_bins + 1):
        yb = h_background_plot.GetBinContent(ib)
        if yb > 0:
            y_min_candidates.append(yb)
            y_max_candidates.append(yb)
    for g_signal in g_signals:
        for i in range(g_signal.GetN()):
            ys = g_signal.GetY()[i]
            if ys > 0:
                y_min_candidates.append(ys)
                y_max_candidates.append(ys)

    y_data_min = min(y_min_candidates) if y_min_candidates else 1e-6
    y_data_max = max(y_max_candidates) if y_max_candidates else 1.0
    y_data_min = max(y_data_min, 1e-12)
    y_data_max = max(y_data_max, y_data_min * 10.0)

    # Add 25% padding at top and bottom in log-space.
    log_span = math.log10(y_data_max) - math.log10(y_data_min)
    pad_frac = 0.25
    y_top_min = math.pow(10.0, math.log10(y_data_min) - pad_frac * log_span)
    y_top_max = math.pow(10.0, math.log10(y_data_max) + pad_frac * log_span)
    y_top_min = max(y_top_min, 1e-12)

    h_frame_top = rt.TH1D("h_frame_top", "", n_plot_bins, x_plot_edges)
    h_frame_top.SetMinimum(y_top_min)
    h_frame_top.SetMaximum(y_top_max)
    h_frame_top.GetYaxis().SetTitle("d#sigma/dm_{jj} [pb/TeV]")
    h_frame_top.GetYaxis().SetTitleSize(0.06)
    h_frame_top.GetYaxis().SetLabelSize(0.053)
    h_frame_top.GetYaxis().SetTitleOffset(1.35)
    h_frame_top.GetXaxis().SetLabelSize(0.0)
    h_frame_top.GetXaxis().SetTitleSize(0.0)
    h_frame_top.GetYaxis().SetNdivisions(510)

    c.Clear()
    c.Divide(1, 2, 0, 0, 0)

    use_logx = (not options.linearX) and (('PF' in box) or (w.var('mjj').getMax() > 2037))

    pad_1 = c.GetPad(1)
    pad_1.SetPad(0.01, 0.34, 0.995, 0.99)
    pad_1.SetLeftMargin(0.17)
    pad_1.SetRightMargin(0.04)
    pad_1.SetTopMargin(0.09)
    pad_1.SetBottomMargin(0.02)
    pad_1.SetTickx(1)
    pad_1.SetTicky(1)
    pad_1.SetLogy(1)
    if use_logx:
        pad_1.SetLogx(1)

    pad_2 = c.GetPad(2)
    pad_2.SetPad(0.01, 0.00, 0.995, 0.34)
    pad_2.SetLeftMargin(0.17)
    pad_2.SetRightMargin(0.04)
    pad_2.SetTopMargin(0.03)
    pad_2.SetBottomMargin(0.30)
    pad_2.SetTickx(1)
    pad_2.SetTicky(1)
    if use_logx:
        pad_2.SetLogx(1)

    pad_1.cd()
    h_frame_top.Draw("AXIS")
    h_background_plot.Draw("HIST SAME")
    for g_signal in g_signals:
        g_signal.Draw("L SAME")
    g_data_plot.Draw("PZ SAME")

    # Header texts
    latex = rt.TLatex()
    latex.SetNDC()
    latex.SetTextFont(42)
    latex.SetTextSize(0.055)
    latex.DrawLatex(0.69, 0.93, "%d fb^{-1} (%.1f TeV)" % (lumi/1000., w.var('sqrts').getVal()/1000.))
    latex.SetTextFont(62)
    latex.SetTextSize(0.070)
    latex.DrawLatex(0.22, 0.82, "CMS")
    latex.SetTextFont(52)
    latex.SetTextSize(0.050)
    latex.DrawLatex(0.22, 0.76, "Preliminary")

    # Legend
    n_leg_entries = 2 + len(g_signals)
    leg_y2 = 0.90
    leg_line_h = 0.055
    leg_y1 = max(0.45, leg_y2 - n_leg_entries * leg_line_h)
    legend = rt.TLegend(0.63, leg_y1, 0.95, leg_y2)
    legend.SetTextFont(42)
    legend.SetFillStyle(0)
    legend.SetBorderSize(0)
    legend.SetTextSize(0.045)
    data_leg_text = "Data [%s]" % year_label if year_label else "Data"
    legend.AddEntry(g_data_plot, data_leg_text, "pe")
    legend.AddEntry(h_background_plot, "Background fit", "l")
    for model, mass, g_signal in zip(models, masses, g_signals):
        legend.AddEntry(g_signal, "%s (%.1f TeV)" % (model, float(mass)/1000.0), "l")
    legend.Draw()

    # Stats text
    chi2_val = list_chi2AndNdf_background[4]
    ndf_val = int(list_chi2AndNdf_background[5])
    chi2_over_ndf = (chi2_val / ndf_val) if ndf_val > 0 else 0.0
    prob_percent = int(round(100.0 * rt.TMath.Prob(chi2_val, ndf_val))) if ndf_val > 0 else 0

    stats_box = rt.TPaveText(0.20, 0.04, 0.55, 0.17, "NDC")
    stats_box.SetFillStyle(0)
    stats_box.SetBorderSize(0)
    stats_box.SetTextAlign(11)
    stats_box.SetTextFont(42)
    stats_box.SetTextSize(0.040)
    stats_box.AddText("#chi^{2}/NDF = %d / %d = %.2f" % (int(round(chi2_val)), ndf_val, chi2_over_ndf))
    stats_box.AddText("Prob. = %d%%" % prob_percent)
    stats_box.Draw()

    # Selection text
    mjj_min_tev = w.var('mjj').getMin() / 1000.0
    mjj_max_tev = w.var('mjj').getMax() / 1000.0
    cuts_box = rt.TPaveText(0.65, 0.04, 0.96, 0.24, "NDC")
    cuts_box.SetFillStyle(0)
    cuts_box.SetBorderSize(0)
    cuts_box.SetTextAlign(21)
    cuts_box.SetTextFont(42)
    cuts_box.SetTextSize(0.040)
    cuts_box.AddText("Wide PF-jets" if "PF" in box else "Wide Calo-jets")
    if mjj_max_tev > 2.5:
        cuts_box.AddText("m_{jj} > %.2f TeV" % mjj_min_tev)
    else:
        cuts_box.AddText("%.2f < m_{jj} < %.2f TeV" % (mjj_min_tev, mjj_max_tev))
    cuts_box.AddText("|#eta| < 2.5, |#Delta#eta| < 1.3")
    cuts_box.Draw()

    pad_2.cd()
    h_residual_plot.SetFillColor(rt.kRed)
    h_residual_plot.SetLineColor(rt.kRed)
    h_residual_plot.SetLineWidth(1)
    h_residual_plot.SetMinimum(-3.5)
    h_residual_plot.SetMaximum(3.5)
    h_residual_plot.GetYaxis().SetTitle("#frac{Data-Prediction}{Uncertainty}")
    h_residual_plot.GetXaxis().SetTitle(x_axis_title)
    h_residual_plot.GetYaxis().SetNdivisions(505)
    h_residual_plot.GetYaxis().SetTitleSize(0.11)
    h_residual_plot.GetYaxis().SetLabelSize(0.11)
    h_residual_plot.GetYaxis().SetTitleOffset(0.58)
    h_residual_plot.GetXaxis().SetTitleSize(0.12)
    h_residual_plot.GetXaxis().SetLabelSize(0.11)
    h_residual_plot.GetXaxis().SetTitleOffset(1.15)
    h_residual_plot.Draw("HIST")

    if use_logx:
        h_residual_plot.GetXaxis().SetNoExponent(True)
        h_residual_plot.GetXaxis().SetMoreLogLabels(True)
        h_residual_plot.GetXaxis().SetNdivisions(510)

    line0 = rt.TLine(x_plot_edges[0], 0.0, x_plot_edges[-1], 0.0)
    line0.SetLineColor(rt.kBlack)
    line0.SetLineWidth(2)
    line0.Draw("SAME")
    for g_signal_residual in g_signal_residuals:
        g_signal_residual.Draw("L SAME")
   
    if not options.linearX:
        c.Print(options.outDir+"/fit_mjj_%s_%s.pdf"%(fitRegion.replace(',','_'),box))
        c.Print(options.outDir+"/fit_mjj_%s_%s.C"%(fitRegion.replace(',','_'),box))
    else:
        c.Print(options.outDir+"/fit_mjj_%s_%s_linearX.pdf"%(fitRegion.replace(',','_'),box))
        c.Print(options.outDir+"/fit_mjj_%s_%s_linearX.C"%(fitRegion.replace(',','_'),box))
        
    rootFile.cd()
    h_background.Write("h_background")
    g_data.Write("g_data")
    h_background_plot.Write("h_background_plot")
    h_residual_plot.Write("h_residual_plot")
    c.Write()
    rootFile.Close()
    

    outFileName = "DijetFitResults_%s.root"%(box)
    outFile = rt.TFile.Open(options.outDir+"/"+outFileName,'recreate')
    outFile.cd()
    w.Write()
    outFile.Close()

    fit_param_rows = _fit_parameter_summary(fr)
    if fit_param_rows:
        print("")
        print("    Floating Parameter        InitialValue            FinalValue +/- Error")
        print("  --------------------  ------------------  -------------------------------")
        for name, v_init, v_final, v_err in fit_param_rows:
            if math.isnan(v_init):
                print("  %-20s  %18s  %18.6g +/- %-10.6g" % (name, "n/a", v_final, v_err))
            else:
                print("  %-20s  %18.6g  %18.6g +/- %-10.6g" % (name, v_init, v_final, v_err))
        print("")

    if not options.verbose:
        if options.linearX:
            fit_pdf = options.outDir+"/fit_mjj_%s_%s_linearX.pdf"%(fitRegion.replace(',','_'),box)
        else:
            fit_pdf = options.outDir+"/fit_mjj_%s_%s.pdf"%(fitRegion.replace(',','_'),box)
        log_info("Fit completed: #chi^2/NDF = %d/%d = %.2f, Prob = %d%%" % (int(round(chi2_val)), ndf_val, chi2_over_ndf, prob_percent))
        #log_info("Poisson-likelihood GOF (deviance): %.1f/%d = %.2f" % (poisson_deviance, poisson_bins, poisson_deviance_per_bin))
        log_info("Output plot: %s" % fit_pdf)
        log_info("Output ROOT: %s" % (options.outDir+"/"+outFileName))

