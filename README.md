# Dijet Scouting Run3 Analyzer

> [!IMPORTANT]
> This framework is only tested for Fermilab LPC machines.<br>
> It might need some adjustments for Lxplus.

---
---

## Setup Environment:

```bash
cmsrel CMSSW_15_0_6
cd CMSSW_15_0_6/src
cmsenv
git cms-init
git clone git@github.com:asimsek/DijetScoutingRun3Analyzer.git
cd DijetScoutingRun3Analyzer
scram b clean; scram b -j 8
```

---

## Important Notes before the nTuple production:

> [!IMPORTANT]
> Do NOT forget to validate/define the JEC paths to the `data/cfg/data_jec_list.txt` for data and `data/cfg/mc_jec_list.txt` for MC.<br>
> MC should be defined as a plain year (e.g., 2024, 2025, etc.); However, data should be defined era by era since each era usually has a different JEC configuration.


> [!IMPORTANT]
> GoldenJSON files are defined in the input cfg file (e.g.; `inputFiles_PFScouting_NanoAOD/PFScouting_2024H_cfg.txt`).<br>
> Validate/Pull the latest GoldenJSON file for your dataset from official repo: [CAF-Certification](https://cms-service-dqmdc.web.cern.ch/CAF/certification/)<br>
> `Collisions24` is for 2024 data and `Collisions25` is for 2025 data.<br>
> Please also note that the GoldenJSON files are updated regularly; However, the DQM group can also update these files outside the data taking year! Validating the latest GoldenJSON file usage will be beneficial before any nTuple production activities. 

---
---

## Local Test - NanoAOD nTuple Maker:

> [!CAUTION]
> **Run the following commands inside the main directory:** `cd $CMSSW_BASE/src/DijetScoutingRun3Analyzer`<br>


### For Data:

```bash
## Create a temporary list:
cat > /tmp/nano_test_2024H.txt << 'EOF'
root://cms-xrd-global.cern.ch//store/data/Run2024H/ScoutingPFRun3/NANOAOD/ScoutNano-v1/2520000/03e073d9-ab61-450d-aa10-fa050e94a16b.root
EOF

## Framework uses `analysisClass.C` file to process data.
## Therefore, create a symbolic link (`symlink`/`soft link`) to use any of your "analysisClass" as `analysisClass.C`:
ln -sf analysisClass_mainDijetPFScoutingSelection_Run3_NanoAOD_Recluster.C src/analysisClass.C

## Generate the ROOT reader class (`rootNtupleClass.*`) so branch declarations and addresses match the NanoAOD file/tree you are about to process:
yes | ./scripts/make_rootNtupleClass.sh -f root://cms-xrd-global.cern.ch//store/data/Run2024H/ScoutingPFRun3/NANOAOD/ScoutNano-v1/2520000/03e073d9-ab61-450d-aa10-fa050e94a16b.root -t Events

## Compile your analyzer:
make clean && make -j8

## Start nTuple production:
./main /tmp/nano_test_2024H.txt config/cutFile_mainDijetPFScoutingSelection_Run3.txt Events test_NanoAOD_2024H_n0 test_NanoAOD_2024H_n0

## Optional: Validate the output:
root -l -q -e 'TFile f("test_NanoAOD_2024H_n0_reduced_skim.root"); TTree* t=(TTree*)f.Get("rootTupleTree/tree"); if(t) t->Print(); else f.ls();'
```


### For QCD MC:

```bash
## Create a temporary list:
cat > /tmp/nano_test_QCD.txt << 'EOF'
root://cms-xrd-global.cern.ch//store/mc/RunIII2024Summer24NanoAOD/QCD_Bin-PT-1000to1500_TuneCP5_13p6TeV_pythia8/NANOAODSIM/140X_mcRun3_2024_realistic_v26-v2/120000/00ba6769-d1a4-4faa-be8f-2157b2a039d7.root
EOF

## Framework uses `analysisClass.C` file to process data.
## Therefore, create a symbolic link (`symlink`/`soft link`) to use any of your "analysisClass" as `analysisClass.C`:
ln -sf analysisClass_mainDijetPFScoutingSelection_Run3_NanoAOD_Recluster.C src/analysisClass.C

## Generate the ROOT reader class (`rootNtupleClass.*`) so branch declarations and addresses match the NanoAOD file/tree you are about to process.
## IMPORTANT: Don't forget to update the root file. You may need to use `root://cms-xrd-global.cern.ch/` or `root://cmsxrootd.fnal.gov/` to access dataset root files.
yes | ./scripts/make_rootNtupleClass.sh -f root://cms-xrd-global.cern.ch//store/mc/RunIII2024Summer24NanoAOD/QCD_Bin-PT-1000to1500_TuneCP5_13p6TeV_pythia8/NANOAODSIM/140X_mcRun3_2024_realistic_v26-v2/120000/00ba6769-d1a4-4faa-be8f-2157b2a039d7.root -t Events

## Compile your analyzer:
make clean && make -j8

## Start nTuple production:
./main /tmp/nano_test_QCD.txt config/cutFile_mainDijetPFScoutingSelection_Run3.txt Events test_NanoAOD_QCD_n0 test_NanoAOD_QCD_n0

## Optional: Validate the output:
ls -lh test_NanoAOD_QCD_n0_reduced_skim.root
root -l -q 'TFile f("test_NanoAOD_QCD_n0_reduced_skim.root"); TTree* t=(TTree*)f.Get("rootTupleTree/tree"); if(t) t->Print();'
```

---


## nTuple production on HTCondor:

> [!NOTE]
> `HTCondor` is used to parallelize nTuple production by splitting large datasets into many independent jobs, which significantly reduces total processing time and improves large-scale production throughput.


#### Create & Send jobs to Condor:

> [!CAUTION]
> **Run the HTCondor scripts/commands inside the `dijetCondor` directory:** `cd $CMSSW_BASE/src/DijetScoutingRun3Analyzer/dijetCondor`<br>

##### Data (PFScouting):

```bash
python3 condor_submit_nanoAOD.py -c inputFiles_PFScouting_NanoAOD/PFScouting_2024C_cfg.txt --force-new-list --request-memory-mb 4096
python3 condor_submit_nanoAOD.py -c inputFiles_PFScouting_NanoAOD/PFScouting_2024D_cfg.txt --force-new-list --request-memory-mb 4096
python3 condor_submit_nanoAOD.py -c inputFiles_PFScouting_NanoAOD/PFScouting_2024E_cfg.txt --force-new-list --request-memory-mb 4096
python3 condor_submit_nanoAOD.py -c inputFiles_PFScouting_NanoAOD/PFScouting_2024F_cfg.txt --force-new-list --request-memory-mb 4096
python3 condor_submit_nanoAOD.py -c inputFiles_PFScouting_NanoAOD/PFScouting_2024G_cfg.txt --force-new-list --request-memory-mb 4096
python3 condor_submit_nanoAOD.py -c inputFiles_PFScouting_NanoAOD/PFScouting_2024H_cfg.txt --force-new-list --request-memory-mb 4096
python3 condor_submit_nanoAOD.py -c inputFiles_PFScouting_NanoAOD/PFScouting_2024I_cfg.txt --force-new-list --request-memory-mb 4096
```

> [!TIP]
> Use `--no-submit` for **dryRun** (create files without sending jobs to condor).


##### Data (PFMonitoring - for Trigger Efficiency):

```bash
python3 condor_submit_nanoAOD.py -c inputFiles_PFMonitoring_NanoAOD/PFMonitoring_2024C_cfg.txt --force-new-list --request-memory-mb 4096
python3 condor_submit_nanoAOD.py -c inputFiles_PFMonitoring_NanoAOD/PFMonitoring_2024D_cfg.txt --force-new-list --request-memory-mb 4096
python3 condor_submit_nanoAOD.py -c inputFiles_PFMonitoring_NanoAOD/PFMonitoring_2024E_cfg.txt --force-new-list --request-memory-mb 4096
python3 condor_submit_nanoAOD.py -c inputFiles_PFMonitoring_NanoAOD/PFMonitoring_2024F_cfg.txt --force-new-list --request-memory-mb 4096
python3 condor_submit_nanoAOD.py -c inputFiles_PFMonitoring_NanoAOD/PFMonitoring_2024G_cfg.txt --force-new-list --request-memory-mb 4096
python3 condor_submit_nanoAOD.py -c inputFiles_PFMonitoring_NanoAOD/PFMonitoring_2024H_cfg.txt --force-new-list --request-memory-mb 4096
python3 condor_submit_nanoAOD.py -c inputFiles_PFMonitoring_NanoAOD/PFMonitoring_2024I_cfg.txt --force-new-list --request-memory-mb 4096
```

> [!TIP]
> Use `--no-submit` for **dryRun** (create files without sending jobs to condor).


##### QCD MC:

```bash
python3 condor_submit_nanoAOD.py -c inputFiles_QCD_NanoAOD/QCDMC_2024_PT50to80_cfg.txt --force-new-list
python3 condor_submit_nanoAOD.py -c inputFiles_QCD_NanoAOD/QCDMC_2024_PT80to120_cfg.txt --force-new-list
python3 condor_submit_nanoAOD.py -c inputFiles_QCD_NanoAOD/QCDMC_2024_PT120to170_cfg.txt --force-new-list
python3 condor_submit_nanoAOD.py -c inputFiles_QCD_NanoAOD/QCDMC_2024_PT170to300_cfg.txt --force-new-list
python3 condor_submit_nanoAOD.py -c inputFiles_QCD_NanoAOD/QCDMC_2024_PT300to470_cfg.txt --force-new-list
python3 condor_submit_nanoAOD.py -c inputFiles_QCD_NanoAOD/QCDMC_2024_PT470to600_cfg.txt --force-new-list
python3 condor_submit_nanoAOD.py -c inputFiles_QCD_NanoAOD/QCDMC_2024_PT600to800_cfg.txt --force-new-list
python3 condor_submit_nanoAOD.py -c inputFiles_QCD_NanoAOD/QCDMC_2024_PT800to1000_cfg.txt --force-new-list
python3 condor_submit_nanoAOD.py -c inputFiles_QCD_NanoAOD/QCDMC_2024_PT1000to1500_cfg.txt --force-new-list
python3 condor_submit_nanoAOD.py -c inputFiles_QCD_NanoAOD/QCDMC_2024_PT1500to2000_cfg.txt --force-new-list
python3 condor_submit_nanoAOD.py -c inputFiles_QCD_NanoAOD/QCDMC_2024_PT2000to2500_cfg.txt --force-new-list
python3 condor_submit_nanoAOD.py -c inputFiles_QCD_NanoAOD/QCDMC_2024_PT2500to3000_cfg.txt --force-new-list
python3 condor_submit_nanoAOD.py -c inputFiles_QCD_NanoAOD/QCDMC_2024_PT3000toInf_cfg.txt --force-new-list
```

> [!WARNING]
> A dummy (or real) `GoldenJSON` file definition in the MC config files is necessary to avoid any possible issues.<br>
> Given `GoldenJSON` won't be used for MC samples.


---


#### Condor output (root) check & re-submit: 

```bash
# Data
python3 check_condor_outputs.py cjobs_ScoutingPFRun3_Run2024G_ScoutNano_v1_NANOAOD_24February2026_12 /eos/uscms/store/group/lpcjj/Run3PFScouting/nanoAODnTuples/2024/ScoutingPFRun3/ScoutingPFRun3_Run2024G_ScoutNano_v1

python3 check_condor_outputs.py cjobs_ScoutingPFRun3_Run2024H_ScoutNano_v1_NANOAOD_24February2026_12 /eos/uscms/store/group/lpcjj/Run3PFScouting/nanoAODnTuples/2024/ScoutingPFRun3/ScoutingPFRun3_Run2024H_ScoutNano_v1

# QCD MC
python3 check_condor_outputs.py cjobs_QCD_Bin-PT-80to120_TuneCP5_13p6TeV_pythia8_NANOAODSIM_25February2026_10 /eos/uscms/store/group/lpcjj/Run3PFScouting/nanoAODnTuples/2024/QCDSamples/QCD_Bin-PT-80to120_TuneCP5_13p6TeV_pythia8 --check-subdirs

python3 check_condor_outputs.py cjobs_QCD_Bin-PT-120to170_TuneCP5_13p6TeV_pythia8_NANOAODSIM_25February2026_10 /eos/uscms/store/group/lpcjj/Run3PFScouting/nanoAODnTuples/2024/QCDSamples/QCD_Bin-PT-120to170_TuneCP5_13p6TeV_pythia8 --check-subdirs
```

> [!TIP]
> Use `--check-subdirs` to include sub-folders of the given directory for the root file search.
> Use `--total-events --dataset /ScoutingPFRun3/Run2024H-ScoutNano-v1/NANOAOD` in order to check total `N_Events` (all root files) and compare it with the dataset.<br>
> Use `--resubmit --request-memory-mb 4096` to resubmit jobs for the missing files and request more memory for them.<br>
> Only target EOS folder can be given (without `cjobs_` folder) to count the event number and compare with the dataset.<br> 
> Even single root file can be given for this purpose.<br>


#### Processed Lumi Calculation:

> [!WARNING]
> The following `brilcalc` setup instruction is prepared for `Fermilab LPC`.<br>
> Please follow the official [brilcalc instructions](https://cms-service-lumi.web.cern.ch/cms-service-lumi/brilwsdoc.html) for `lxplus`.

##### Setup `brilcalc` (for `LPC`):

```bash
### Find which brilconda python exists on your node (pick one that exists)
ls -d /cvmfs/cms-bril.cern.ch/brilconda* 2>/dev/null

### Prefer a python3-based one if present (example: brilconda310)
BRILPY=/cvmfs/cms-bril.cern.ch/brilconda310/bin/python3

### Install brilcalc
$BRILPY -m pip install --user --upgrade brilws

export PATH=$HOME/.local/bin:$PATH

## Validate
which brilcalc
brilcalc --version
```

##### Produce crab-like `processed_lumi.json` file:

```bash
python3 condor_report.py -c inputFiles_PFScouting_NanoAOD/PFScouting_2024H_cfg.txt -o processed_lumis_2024H.json \
    --brilcalc --unit /fb --normtag /cvmfs/cms-bril.cern.ch/cms-lumi-pog/Normtags/normtag_PHYSICS.json \
    --no-golden \
    --brilcalc-extra -c "$CMS_PATH/SITECONF/local/JobConfig/site-local-config.xml" 

python3 condor_report.py -c inputFiles_PFScouting_NanoAOD/PFScouting_2024H_cfg.txt -o processed_lumis_2024H.json \
    --brilcalc --unit /fb --normtag /cvmfs/cms-bril.cern.ch/cms-lumi-pog/Normtags/normtag_PHYSICS.json \
     --golden-json \
     --brilcalc-extra -c "$CMS_PATH/SITECONF/local/JobConfig/site-local-config.xml"
```

> [!TIP]
> `--brilcalc` argument can be used to perform `brilcalc` on the newly generated json file.<br>
> `--input-json <fileName>.json` argument can be used to process an existing json file with `brilcalc`.<br>
> `--golden-json` can be used to filter output json according to the goldenJSON file (oposite is `--no-golden`).


```bash
brilcalc lumi -c "$CMS_PATH/SITECONF/local/JobConfig/site-local-config.xml" -u /fb \
    -i processed_lumis_2024H.json \
    --normtag /cvmfs/cms-bril.cern.ch/cms-lumi-pog/Normtags/normtag_PHYSICS.json
```


#### Merge MC Samples:

> [!NOTE]
> This script allows you to merge (`hadd`) the sample outputs.<br>
> The result is one ROOT file per sample type (e.g.; one output for `80to120`, one for `120to170`, etc.).<br>
> This is a necessary step for MC scaling with xSec values.


```bash
python3 merge_samples.py /eos/uscms/store/group/lpcjj/Run3PFScouting/nanoAODnTuples/2024/QCDSamples --sample-glob "QCD_Bin-PT-*"
```

> [!TIP]
> `--sample-glob` argument lets you select which sample folders under the base directory will be merged.


---
---

#### Condor Job Diagnosis (Fermilab LPC):

> [!NOTE]
> If you have some jobs that are in the "`Hold/Held`" state, use the following commands `CAREFULLY` to understand the problem!<br>
> And then either release the job(s) for `re-processing`, or `remove` the it/them completely!

```bash
## Learn the hold reason of the jobs:
condor_q -hold -af HoldReason

## Release the job for re-processing:
condor_release -const 'jobstatus==5' -name lpcschedd6

## WARNING!!
## This removes all the jobs with the same `.sh` file name PERMENANTLY!!:
## ONLY USE THIS IF YOU NEED TO REMOVE ALL THE SAME TYPE OF JOBS!!
## Don't forget to change the lpcschedd number and also the file name prefix (until & including `_n`).
condor_rm -name lpcschedd5 -constraint "Owner==\"$USER\" && regexp(\".*QCD_Bin-PT-80to120_TuneCP5_13p6TeV_pythia8_NANOAODSIM_n[0-9]+\\.sh$\", Cmd)"


## Shows a detailed match diagnosis for the job, explaining why it is not running
## e.g., requirement mismatch, memory, site constraints.
condor_q -better-analyze <jobID>
```

--
--



### CMS Connect Usage:

> [!IMPORTANT]
> Need a `Fermilab (LPC) computing account` in order to request and use this sytem.<br>
> If you're working on `lxplus`, skip this part!

LPC Condor resources are limited and might take very long due to the `user-priority` and `high-demand`. `CMS Connect` provides a Tier3-like environment for condor jobs and allow to use all resources available in the `CMS Global Pool`.<br>


#### Registration & Instructions:

**Register to [ci-connect](https://www.ci-connect.net/)**

> [!CAUTION]
> `ci-connect` is the new platform and `cilogon` is deprecated.
> Follow the Twiki page instruction for `SSH Key` steps but use `ci-connect` page instead of `cilogon`.
> Allow system a few hours after uploading the SSH Key to your `ci-connect` profile.

 - [CMS Connect Setup](https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookCMSConnect)
 - [CMS Connect Examples](https://twiki.cern.ch/twiki/bin/view/Main/CMSConnectExamples)


> [!WARNING]
> The following steps can only be used once the system recognizes your new `ssh-key`.

#### Login to `CMS Connect`:

```bash
ssh <username>@login.uscms.org
```

#### Upload your grid cerificate from your LPC Area:**

> [!NOTE]
> You might need to delete the old ones in `~/.globus` folder (if exist)!

```bash
copy_certificates
```

#### Create a new proxy:

```bash
voms-proxy-init -voms cms
```

> [!WARNING]
> **Important: Set a default project for your account with the following commands to be able to submit any Condor jobs:**

```bash
mkdir ~/.ciconnect
echo "cms.org.cms" > ~/.ciconnect/defaultproject
```


--
--

## Useful Links

 + [Run3 Luminosity and uncertainty recommendations](https://twiki.cern.ch/twiki/bin/view/CMS/LumiRecommendationsRun3)
 + [Golden JSON](https://cms-service-dqmdc.web.cern.ch/CAF/certification/)
 + [BrilCalc - Lumi Calculation](https://cms-service-lumi.web.cern.ch/cms-service-lumi/brilwsdoc.html)
 + [Run3 JERC](https://cms-jerc.web.cern.ch/)
 + [Run3 PdmV](https://twiki.cern.ch/twiki/bin/view/CMS/PdmVRun3Analysis)
 + [Rucio CLI Twiki](https://twiki.cern.ch/twiki/bin/view/CMS/TheRucioCLI)
 + [Rucio WebUI](https://cms-rucio-webui.cern.ch/r2d2/request)
 + [LPC EOS Guidelines](https://www.uscms.org/uscms_at_work/computing/LPC/usingEOSAtLPC.shtml)




