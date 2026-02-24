# Dijet Scouting Run3 Analyzer






```bash
cmsrel CMSSW_15_0_6
cd CMSSW_15_0_6/src
cmsenv
git cms-init
git clone git@github.com:asimsek/DijetScoutingRun3Analyzer.git
cd DijetScoutingRun3Analyzer
scram b clean; scram b -j 8
```


### NanoAOD analyzer Test:

#### Data:

```bash
cd CMSSW_15_0_6/src/DijetScoutingRun3Analyzer

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


#### QCD MC:

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







