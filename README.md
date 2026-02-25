# Dijet Scouting Run3 Analyzer

**This framework is only tested for Fermilab LPC machines. It might need some adjustments for Lxplus.**


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


## Local Test - NanoAOD nTuple Maker:

### For Data:

```bash
cat > /tmp/nano_test_2024H.txt << 'EOF'
root://cms-xrd-global.cern.ch//store/data/Run2024H/ScoutingPFRun3/NANOAOD/ScoutNano-v1/2520000/03e073d9-ab61-450d-aa10-fa050e94a16b.root
EOF

ln -sf analysisClass_mainDijetPFScoutingSelection_Run3_NanoAOD_Recluster.C src/analysisClass.C

yes | ./scripts/make_rootNtupleClass.sh -f root://cms-xrd-global.cern.ch//store/data/Run2024H/ScoutingPFRun3/NANOAOD/ScoutNano-v1/2520000/03e073d9-ab61-450d-aa10-fa050e94a16b.root -t Events

make clean && make -j8

./main /tmp/nano_test_2024H.txt config/cutFile_mainDijetPFScoutingSelection_Run3.txt Events test_NanoAOD_2024H_n0 test_NanoAOD_2024H_n0

ls -lh test_NanoAOD_2024H_n0_reduced_skim.root
root -l -q -e 'TFile f("test_NanoAOD_2024H_n0_reduced_skim.root"); TTree* t=(TTree*)f.Get("rootTupleTree/tree"); if(t) t->Print(); else f.ls();'
```


### For QCD MC:

```bash
cd CMSSW_15_0_6/src/DijetScoutingRun3Analyzer

cat > /tmp/nano_test_QCD.txt << 'EOF'
root://cms-xrd-global.cern.ch//store/mc/RunIII2024Summer24NanoAOD/QCD_Bin-PT-1000to1500_TuneCP5_13p6TeV_pythia8/NANOAODSIM/140X_mcRun3_2024_realistic_v26-v2/120000/00ba6769-d1a4-4faa-be8f-2157b2a039d7.root
EOF

ln -sf analysisClass_mainDijetPFScoutingSelection_Run3_NanoAOD_Recluster.C src/analysisClass.C

yes | ./scripts/make_rootNtupleClass.sh -f root://cms-xrd-global.cern.ch//store/mc/RunIII2024Summer24NanoAOD/QCD_Bin-PT-1000to1500_TuneCP5_13p6TeV_pythia8/NANOAODSIM/140X_mcRun3_2024_realistic_v26-v2/120000/00ba6769-d1a4-4faa-be8f-2157b2a039d7.root -t Events

make clean && make -j8

./main /tmp/nano_test_QCD.txt config/cutFile_mainDijetPFScoutingSelection_Run3.txt Events test_NanoAOD_QCD_n0 test_NanoAOD_QCD_n0

ls -lh test_NanoAOD_QCD_n0_reduced_skim.root
root -l -q 'TFile f("test_NanoAOD_QCD_n0_reduced_skim.root"); TTree* t=(TTree*)f.Get("rootTupleTree/tree"); if(t) t->Print();'
```



## nTuple production on HTCondor:

#### Create & Send jobs to Condor:

##### Data:

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
> A dummy (or real) GoldenJSON file definition in the MC config files is necessary to avoid any possible issues.<br>
> GoldenJSON won't be used for MC samples.


#### Condor output (root) check & Resubmit: 

```bash
# check only
python3 check_condor_outputs.py cjobs_ScoutingPFRun3_Run2024H_ScoutNano_v1_NANOAOD_24February2026_12 /eos/uscms/store/group/lpcjj/Run3PFScouting/nanoAODnTuples/2024/ScoutingPFRun3/ScoutingPFRun3_Run2024H_ScoutNano_v1

# check total N_Events (all root files)
python3 check_condor_outputs.py cjobs_ScoutingPFRun3_Run2024H_ScoutNano_v1_NANOAOD_24February2026_12 /eos/uscms/store/group/lpcjj/Run3PFScouting/nanoAODnTuples/2024/ScoutingPFRun3/ScoutingPFRun3_Run2024H_ScoutNano_v1 --total-events --dataset /ScoutingPFRun3/Run2024H-ScoutNano-v1/NANOAOD

# re-submit missing ones
python3 check_condor_outputs.py cjobs_ScoutingPFRun3_Run2024H_ScoutNano_v1_NANOAOD_24February2026_12 /eos/uscms/store/group/lpcjj/Run3PFScouting/nanoAODnTuples/2024/ScoutingPFRun3/ScoutingPFRun3_Run2024H_ScoutNano_v1 --resubmit --request-memory-mb 5096
```



