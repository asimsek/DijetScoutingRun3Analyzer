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
python3 make_reduced_ntuple_list.py /eos/uscms/store/group/lpcjj/Run3PFScouting/nanoAODnTuples/2024/ScoutingPFRun3/ScoutingPFRun3_Run2024H_ScoutNano_v1 -o ScoutingPFRun3_Run2024H_reduced.txt

## Data (Monitoring)
python3 make_reduced_ntuple_list.py /eos/uscms/store/group/lpcjj/Run3PFScouting/nanoAODnTuples/2024/ScoutingPFMonitor/ScoutingPFMonitor_Run2024H_PromptReco_v1 -o ScoutingPFMonitor_Run2024H_reduced.txt

## MC (QCD)
python3 make_reduced_ntuple_list.py /eos/uscms/store/group/lpcjj/Run3PFScouting/nanoAODnTuples/2024/QCDSamples -o QCDMC_2024_reduced.txt
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




## Trigger Efficiency


```bash
python3 plot_trigger_efficiency.py --monitoring-list ../lists/reducedNtuple_lists/ScoutingPFMonitor_Run2024H_reduced.txt --scouting-list ../lists/reducedNtuple_lists/ScoutingPFRun3_Run2024H_reduced.txt --lumi-pb 5490 --year "2024H"
```


## Kinematic Plots

#### Plot a Single Variable

```bash
## Run kinematics separately on your local machine (background-jobs)
mkdir -p plot_kinematics logs

nohup python3 plot_kinematics.py --plot mjj             --data-list ../lists/reducedNtuple_lists/ScoutingPFRun3_Run2024H_reduced.txt --mc-list ../lists/reducedNtuple_lists/QCDMC_2024_reduced.txt --lumi-pb 5490 --output-dir plot_kinematics --output-prefix dataVsQCD_ > logs/mjj.log 2>&1 &
nohup python3 plot_kinematics.py --plot Dijet_MassAK4PF --data-list ../lists/reducedNtuple_lists/ScoutingPFRun3_Run2024H_reduced.txt --mc-list ../lists/reducedNtuple_lists/QCDMC_2024_reduced.txt --lumi-pb 5490 --output-dir plot_kinematics --output-prefix dataVsQCD_ > logs/Dijet_MassAK4PF.log 2>&1 &
nohup python3 plot_kinematics.py --plot pTWJ_j1         --data-list ../lists/reducedNtuple_lists/ScoutingPFRun3_Run2024H_reduced.txt --mc-list ../lists/reducedNtuple_lists/QCDMC_2024_reduced.txt --lumi-pb 5490 --output-dir plot_kinematics --output-prefix dataVsQCD_ > logs/pTWJ_j1.log 2>&1 &
nohup python3 plot_kinematics.py --plot pTWJ_j2         --data-list ../lists/reducedNtuple_lists/ScoutingPFRun3_Run2024H_reduced.txt --mc-list ../lists/reducedNtuple_lists/QCDMC_2024_reduced.txt --lumi-pb 5490 --output-dir plot_kinematics --output-prefix dataVsQCD_ > logs/pTWJ_j2.log 2>&1 &
nohup python3 plot_kinematics.py --plot etaWJ_j1        --data-list ../lists/reducedNtuple_lists/ScoutingPFRun3_Run2024H_reduced.txt --mc-list ../lists/reducedNtuple_lists/QCDMC_2024_reduced.txt --lumi-pb 5490 --output-dir plot_kinematics --output-prefix dataVsQCD_ > logs/etaWJ_j1.log 2>&1 &
nohup python3 plot_kinematics.py --plot etaWJ_j2        --data-list ../lists/reducedNtuple_lists/ScoutingPFRun3_Run2024H_reduced.txt --mc-list ../lists/reducedNtuple_lists/QCDMC_2024_reduced.txt --lumi-pb 5490 --output-dir plot_kinematics --output-prefix dataVsQCD_ > logs/etaWJ_j2.log 2>&1 &
nohup python3 plot_kinematics.py --plot deltaETAjj      --data-list ../lists/reducedNtuple_lists/ScoutingPFRun3_Run2024H_reduced.txt --mc-list ../lists/reducedNtuple_lists/QCDMC_2024_reduced.txt --lumi-pb 5490 --output-dir plot_kinematics --output-prefix dataVsQCD_ > logs/deltaETAjj.log 2>&1 &
nohup python3 plot_kinematics.py --plot phiWJ_j1        --data-list ../lists/reducedNtuple_lists/ScoutingPFRun3_Run2024H_reduced.txt --mc-list ../lists/reducedNtuple_lists/QCDMC_2024_reduced.txt --lumi-pb 5490 --output-dir plot_kinematics --output-prefix dataVsQCD_ > logs/phiWJ_j1.log 2>&1 &
nohup python3 plot_kinematics.py --plot phiWJ_j2        --data-list ../lists/reducedNtuple_lists/ScoutingPFRun3_Run2024H_reduced.txt --mc-list ../lists/reducedNtuple_lists/QCDMC_2024_reduced.txt --lumi-pb 5490 --output-dir plot_kinematics --output-prefix dataVsQCD_ > logs/phiWJ_j2.log 2>&1 &

## Check the last logs of a background job:
tail -n 50 logs/mjj.log
```

*Built-in plot names are:* `mjj`, `Dijet_MassAK4PF`, `pTWJ_j1`, `pTWJ_j2`, `etaWJ_j1`, `etaWJ_j2`, `deltaETAjj`, `phiWJ_j1`, `phiWJ_j2`


> [!IMPORTANT]
> For the MC list entries use:<br>
> `mc_path xsec_pb`<br>


> [!TIP]
> Luminosity information is available on the [PdmV Twiki Page](https://twiki.cern.ch/twiki/bin/viewauth/CMS/PdmVRun3Analysis#2024_Analysis_Summary_Table).<br>




#### Submit Plot Jobs to Condor

```bash
python3 condor_submit_kinematics.py --data-list ../lists/reducedNtuple_lists/ScoutingPFRun3_Run2024H_reduced.txt --mc-list ../lists/reducedNtuple_lists/QCDMC_2024_reduced.txt --lumi-pb 5490 --cmssw-base "$CMSSW_BASE" --output-prefix dataVsQCD_
```

> [!TIP]
> Use `--no-submit` for a dryRun.
> Use `--plots mjj,pTWJ_j1,deltaETAjj` to submit only a subset of the built-in plot specs (default=`all`).


---
---





