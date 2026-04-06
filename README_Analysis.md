# Dijet Scouting Run3 Analyzer - Analysis

> [!IMPORTANT]
> This document covers the analysis-side plotting workflow.<br>
> The nTuple production workflow remains in [`README.md`](README.md).

---
---

## Create Reduced nTuple Lists

> [!CAUTION]
> Run these commands inside the main analyzer directory:<br>
> `cd $CMSSW_BASE/src/DijetScoutingRun3Analyzer/scripts`

```bash
## Data (Scouting)
## Full 2024
python3 make_reduced_ntuple_list.py /eos/uscms/store/group/lpcjj/Run3PFScouting/nanoAODnTuples/2024/ScoutingPFRun3/ -o ScoutingPFRun3_Run2024_ALL_reduced.txt

## 2024I
python3 make_reduced_ntuple_list.py /eos/uscms/store/group/lpcjj/Run3PFScouting/nanoAODnTuples/2024/ScoutingPFRun3/ScoutingPFRun3_Run2024I_ScoutNano_v1/ -o ScoutingPFRun3_Run2024I_reduced.txt

## QCD MC (Merged Samples)
python3 make_reduced_ntuple_list.py /eos/uscms/store/group/lpcjj/Run3PFScouting/nanoAODnTuples/2024/QCDSamples/merged -o QCDMC_2024_reduced.txt
```

> [!NOTE]
> This script searches recursively for `*_reduced_skim.root` files and writes the list under:<br>
> `$CMSSW_BASE/src/DijetScoutingRun3Analyzer/lists/reducedNtuple_lists`<br>
> The output paths are saved in `root://cmseos.fnal.gov//...` format.


> [!WARNING]
> Each QCD MC entry in the list must include its corresponding `xsec` value.<br>
> e.g.; `QCD_Bin-PT-50to80_TuneCP5_13p6TeV_pythia8_NANOAODSIM_Condor_reduced_skim.root 27390.0`<br>
> MC `xsec` values can be found in the [XSDB Page](https://xsecdb-xsdb-official.app.cern.ch/xsdb/?searchQuery=DAS=QCD_Bin-PT-2500to3000_TuneCP5_13p6TeV_pythia8).<br>
> <br>
> For `2024` QCD MC reduced-ntuple lists, use the `XSDB` cross sections: `50to80=16730000.0`, `80to120=2506000.0`, `120to170=439800.0`, `170to300=113300.0`, `300to470=7581.0`, `470to600=623.3`, `600to800=178.7`, `800to1000=30.62`, `1000to1500=9.306`, `1500to2000=0.5015`, `2000to2500=0.04264`, `2500to3000=0.004454`, `3000=0.0005539`.


---

## Trigger Efficiency (From histograms saved in nTuples)

```bash
### Merge ONLY histograms in the nTuples for trigger efficiency studies; "-T" argument solely merges histograms and skips trees.
hadd -T ScoutingPFRun3_Run2024I_ScoutNano_v1_NANOAOD_ALLHisto.root /eos/uscms/store/user/lpcjj/Run3PFScouting/nanoAODnTuples/2024/ScoutingPFRun3/ScoutingPFRun3_Run2024I_ScoutNano_v1/ScoutingPFRun3_Run2024I_ScoutNano_v1_NANOAOD_n*_reduced_skim.root

### Build the script (just once)
g++ -std=c++20 plot_trigger_efficiency_hist.cpp $(root-config --cflags --libs) -o plot_trigger_efficiency_hist

### Calculate efficiency from the merged root file
./plot_trigger_efficiency_hist --input-root ScoutingPFRun3_Run2024I_ScoutNano_v1_NANOAOD_ALLHisto.root --output-dir trigger_efficiency_hist_2024I --lumi-pb 11451.783064 --year 2024I
```




## SKIP THIS ONE: Trigger Efficiency (From Trees - Legacy ONLY)

```bash
#### SKIP THIS ONE
# Build only once
c++ -std=c++17 -O3 -o plot_trigger_efficiency plot_trigger_efficiency.cpp $(root-config --cflags --libs)

#### SKIP THIS ONE
./plot_trigger_efficiency --scouting-list ../lists/reducedNtuple_lists/ScoutingPFRun3_Run2024I_reduced.txt --lumi-pb 11451.783064 --year "2024I"  --use-scouting-for-efficiency
```


---


## Kinematic Plots

#### Plot a Single Variable

```bash
## Build once
c++ -std=c++17 -O3 -o plot_kinematics plot_kinematics.cpp $(root-config --cflags --libs)

## Run kinematics separately on your local machine
mkdir -p logs

## 2024I
./plot_kinematics --plot all --data-list ../lists/reducedNtuple_lists/ScoutingPFRun3_Run2024I_reduced.txt --mc-list ../lists/reducedNtuple_lists/QCDMC_2024_reduced.txt --lumi-pb 11451.783064 --output-dir kinematics_plots_deta1p3_2024I --output-prefix dataVsQCD_

## Full 2024
./plot_kinematics --plot all --data-list ../lists/reducedNtuple_lists/ScoutingPFRun3_Run2024_ALL_reduced.txt --mc-list ../lists/reducedNtuple_lists/QCDMC_2024_reduced.txt --lumi-pb 108757.319778 --output-dir kinematics_plots_deta1p3_2024 --output-prefix dataVsQCD_
```

> [!TIP]
> *Built-in plot names are:* `mjj`, `Dijet_MassAK4PF`, `pTWJ_j1`, `pTWJ_j2`, `etaWJ_j1`, `etaWJ_j2`, `deltaETAjj`, `phiWJ_j1`, `phiWJ_j2`, `deltaPHIjj`<br>
> `--plot all`: to process all kinematics at once.


> [!IMPORTANT]
> For the MC list entries use:<br>
> `mc_path xsec_pb`<br>


**Luminosity information is available on the [PdmV Twiki Page](https://twiki.cern.ch/twiki/bin/viewauth/CMS/PdmVRun3Analysis#2024_Analysis_Summary_Table).**


---


#### Submit Plot Jobs to Condor (Needs to be fixed!)

```bash
python3 condor_submit_kinematics.py --data-list ../lists/reducedNtuple_lists/ScoutingPFRun3_Run2024_ALL_reduced.txt --mc-list ../lists/reducedNtuple_lists/QCDMC_2024_reduced.txt --lumi-pb 108757.319778 --cmssw-base "$CMSSW_BASE" --output-prefix dataVsQCD_
```

> [!TIP]
> Use `--no-submit` for a dryRun.<br>
> Use `--plots mjj,pTWJ_j1,deltaETAjj` to submit only a subset of the built-in plot specs (default=`all`).


---


### Compare Cross-section: Run3 vs Run2

```bash
## Full 2024
python3 compare_xsec_Run3vsRun2.py --run2-root ../inputs/CaloScoutingHT2016_27August2023_13/histo_data_mjj_fromTree.root --run3-root ../inputs/PFScouting2024_ALL_deta1p3_15March2026/dataVsQCD_all_plots.root --run2-data-hist h_dat_rebin --run2-mc-hist h_MC_rebin --run3-data-hist h_data_mjj --run3-mc-hist h_mc_total_mjj --lumi-run2-fb 27.225 --lumi-run3-fb 108.757319778 --year-run2 2016 --year-run3 2024 --output-dir Run3vsRun2_Comparison --output-prefix Run3_2024ALL_vsRun2 --overlay-ratio-y-min 0.2 --overlay-ratio-y-max 1.8

## 2024I
python3 compare_xsec_Run3vsRun2.py --run2-root ../inputs/CaloScoutingHT2016_27August2023_13/histo_data_mjj_fromTree.root --run3-root ../inputs/PFScouting2024I_deta1p3_23March2026/dataVsQCD_all_plots.root --run2-data-hist h_dat_rebin --run2-mc-hist h_MC_rebin --run3-data-hist h_data_mjj --run3-mc-hist h_mc_total_mjj --lumi-run2-fb 27.225 --lumi-run3-fb 11.451783064 --year-run2 2016 --year-run3 2024 --output-dir Run3vsRun2_Comparison --output-prefix Run3_2024I_vsRun2 --overlay-ratio-y-min 0.2 --overlay-ratio-y-max 1.8

---
---


### Update a list of `.config` files with the mass range from trigger efficiency calculation

```bash
python3 ../scripts/update_config_files.py --mass-start 565 --mass-end 2132 --tag PFDijet2025 \
  ../config/dijet_3param.config \
  ../config/dijet.config \
  ../config/dijet_5param.config \
  ../config/dijet_6param.config \
  ../config/dijet_ModExp3Param.config \
  ../config/dijet_ModExp4Param.config \
  ../config/dijet_ModExp.config \
  ../config/dijet_ModExp6Param.config \
  ../config/dijet_Atlas3Param.config \
  ../config/dijet_Atlas4Param.config \
  ../config/dijet_Atlas.config \
  ../config/dijet_Atlas6Param.config \
  ../config/ModDijet_3param.config \
  ../config/ModDijet_4param.config \
  ../config/ModDijet_5param.config \
  ../config/ModDijet_6param.config
```


---
---


## Fit Method:

### CMS Dijet Function Family

```bash
python3 ../python/BinnedFit.py -c ../config/dijet_3param.config -l 11451.783064 --year 2024I ../inputs/PFScouting2024I_deta1p3_23March2026/dataVsQCD_all_plots.root -b PFDijet2024 -d fits_dijet_2024I_M565_deta1p3_15March2026/dijet_3Param --fit-spectrum --staged-fit

python3 ../python/BinnedFit.py -c ../config/dijet.config -l 11451.783064 --year 2024I ../inputs/PFScouting2024I_deta1p3_23March2026/dataVsQCD_all_plots.root -b PFDijet2024 -d fits_dijet_2024I_M565_deta1p3_15March2026/dijet_4Param --fit-spectrum --staged-fit

python3 ../python/BinnedFit.py -c ../config/dijet_5param.config -l 11451.783064 --year 2024I ../inputs/PFScouting2024I_deta1p3_23March2026/dataVsQCD_all_plots.root -b PFDijet2024 -d fits_dijet_2024I_M565_deta1p3_15March2026/dijet_5Param --fit-spectrum --staged-fit

python3 ../python/BinnedFit.py -c ../config/dijet_6param.config -l 11451.783064 --year 2024I ../inputs/PFScouting2024I_deta1p3_23March2026/dataVsQCD_all_plots.root -b PFDijet2024 -d fits_dijet_2024I_M565_deta1p3_15March2026/dijet_6Param --fit-spectrum --staged-fit
```

> [!NOTE]
> Resonance shapes can be plotted on top of the distribution by adding the following arguments into the main fitting command line.<br>
> `--mass 2000_4000_6000 -m gg_qg_qq --xsec 1.31e-1_5.65e-03_5.86e-04 -s ../inputs/resonanceShapes_13p6TeV_Summer22EE_dEta1p3/ResonanceShapes_gg_13p6TeV_Summer22EE.root,../inputs/resonanceShapes_13p6TeV_Summer22EE_dEta1p3/ResonanceShapes_qg_13p6TeV_Summer22EE.root,../inputs/resonanceShapes_13p6TeV_Summer22EE_dEta1p3/ResonanceShapes_qq_13p6TeV_Summer22EE.root`


> [!TIP]
> Various `config` files (under `$CMSSW_BASE/src/DijetScoutingRun3Analyzer/config` folder) can be used to fit the data with different parametric function familites: `dijet.config`, `dijet_ModExp4Param.config`, `dijet_Atlas.config`, `ModDijet_4param.config`, etc.<br>


### Modified Exponential (ModExp) Function Family

```bash
python3 ../python/BinnedFit.py -c ../config/dijet_ModExp3Param.config -l 11451.783064 --year 2024I ../inputs/PFScouting2024I_deta1p3_23March2026/dataVsQCD_all_plots.root -b PFDijet2024 -d fits_modExp_2024I_M565_deta1p3_15March2026/modExp_3Param --fit-spectrum --staged-fit

python3 ../python/BinnedFit.py -c ../config/dijet_ModExp4Param.config -l 11451.783064 --year 2024I ../inputs/PFScouting2024I_deta1p3_23March2026/dataVsQCD_all_plots.root -b PFDijet2024 -d fits_modExp_2024I_M565_deta1p3_15March2026/modExp_4Param --fit-spectrum --staged-fit

python3 ../python/BinnedFit.py -c ../config/dijet_ModExp.config -l 11451.783064 --year 2024I ../inputs/PFScouting2024I_deta1p3_23March2026/dataVsQCD_all_plots.root -b PFDijet2024 -d fits_modExp_2024I_M565_deta1p3_15March2026/modExp_5Param --fit-spectrum --staged-fit

python3 ../python/BinnedFit.py -c ../config/dijet_ModExp6Param.config -l 11451.783064 --year 2024I ../inputs/PFScouting2024I_deta1p3_23March2026/dataVsQCD_all_plots.root -b PFDijet2024 -d fits_modExp_2024I_M565_deta1p3_15March2026/modExp_6Param --fit-spectrum --staged-fit
```

### ATLAS Function Family


```bash
python3 ../python/BinnedFit.py -c ../config/dijet_Atlas3Param.config -l 11451.783064 --year 2024I ../inputs/PFScouting2024I_deta1p3_23March2026/dataVsQCD_all_plots.root -b PFDijet2024 -d fits_ATLAS_2024I_M565_deta1p3_15March2026/ATLAS_3Param --fit-spectrum --staged-fit

python3 ../python/BinnedFit.py -c ../config/dijet_Atlas4Param.config -l 11451.783064 --year 2024I ../inputs/PFScouting2024I_deta1p3_23March2026/dataVsQCD_all_plots.root -b PFDijet2024 -d fits_ATLAS_2024I_M565_deta1p3_15March2026/ATLAS_4Param --fit-spectrum --staged-fit

python3 ../python/BinnedFit.py -c ../config/dijet_Atlas.config -l 11451.783064 --year 2024I ../inputs/PFScouting2024I_deta1p3_23March2026/dataVsQCD_all_plots.root -b PFDijet2024 -d fits_ATLAS_2024I_M565_deta1p3_15March2026/ATLAS_5Param --fit-spectrum --staged-fit

python3 ../python/BinnedFit.py -c ../config/dijet_Atlas6Param.config -l 11451.783064 --year 2024I ../inputs/PFScouting2024I_deta1p3_23March2026/dataVsQCD_all_plots.root -b PFDijet2024 -d fits_ATLAS_2024I_M565_deta1p3_15March2026/ATLAS_6Param --fit-spectrum --staged-fit
```


### Modified Dijet (ModDijet) Function Family


```bash
python3 ../python/BinnedFit.py -c ../config/ModDijet_3param.config -l 11451.783064 --year 2024I ../inputs/PFScouting2024I_deta1p3_23March2026/dataVsQCD_all_plots.root -b PFDijet2024 -d fits_modDijet_2024I_M565_deta1p3_15March2026/modDijet_3Param --fit-spectrum --staged-fit

python3 ../python/BinnedFit.py -c ../config/ModDijet_4param.config -l 11451.783064 --year 2024I ../inputs/PFScouting2024I_deta1p3_23March2026/dataVsQCD_all_plots.root -b PFDijet2024 -d fits_modDijet_2024I_M565_deta1p3_15March2026/modDijet_4Param --fit-spectrum --staged-fit

python3 ../python/BinnedFit.py -c ../config/ModDijet_5param.config -l 11451.783064 --year 2024I ../inputs/PFScouting2024I_deta1p3_23March2026/dataVsQCD_all_plots.root -b PFDijet2024 -d fits_modDijet_2024I_M565_deta1p3_15March2026/modDijet_5Param --fit-spectrum --staged-fit

python3 ../python/BinnedFit.py -c ../config/ModDijet_6param.config -l 11451.783064 --year 2024I ../inputs/PFScouting2024I_deta1p3_23March2026/dataVsQCD_all_plots.root -b PFDijet2024 -d fits_modDijet_2024I_M565_deta1p3_15March2026/modDijet_6Param --fit-spectrum --staged-fit
```


> [!TIP]
> `--staged-fit`: Performs an automated staged background fit, progressively releasing higher-order shape parameters and carrying each stage's best-fit result forward to the next stage.<br>
> This is an experimental feature!!




---
---

