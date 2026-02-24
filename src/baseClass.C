#define baseClass_cxx
#include "baseClass.h"

//
// Optional: event-count histograms that indicate the number of events originally processed.
// If none exist, the code assumes the number of events equals the tree entries.
//
namespace {
  const std::string kHistoCountNstart1 = "dijets/TriggerPass";
  const std::string kHistoCountNstart2 = "dijetscouting/TriggerPass";
  const std::string kHistoCountNstart3 = "DijetFilter/EventCount/EventCounter"; // reduced skim case

  // Sentinel numeric bounds for cuts
  constexpr double kPlusInf  =  99999999999.0;
  constexpr double kMinusInf = -99999999999.0;
} // namespace

//--------------------------------------------------------------------------------------------------
// Constructors [Ctors] / Destructors [Dtors]
//--------------------------------------------------------------------------------------------------

baseClass::baseClass(std::string* inputList,
                     std::string* cutFile,
                     std::string* treeName,
                     std::string* outputFileName,
                     std::string* cutEfficFile)
  : PileupWeight_(1.0),
    fillSkim_(true),
    fillAllPreviousCuts_(true),
    fillAllOtherCuts_(true),
    fillAllSameLevelAndLowerLevelCuts_(true),
    fillAllCuts_(true),
    oldKey_("")
{
  nOptimizerCuts_ = 20;
  inputList_      = inputList;
  cutFile_        = cutFile;
  treeName_       = treeName;
  outputFileName_ = outputFileName;
  cutEfficFile_   = cutEfficFile;

  init();
}

baseClass::~baseClass()
{
  // if (!writeCutHistos())        { STDOUT("ERROR: writeCutHistos did not complete successfully."); }
  // if (!writeCutEfficFile())     { STDOUT("ERROR: writeStatFile did not complete successfully."); }
  // if (!writeUserHistos())       { STDOUT("ERROR: writeUserHistos did not complete successfully."); }
  if (!writeSkimTree())         { STDOUT("ERROR: writeSkimTree did not complete successfully."); }
  if (!writeReducedSkimTree())  { STDOUT("ERROR: writeReducedSkimTree did not complete successfully."); }

  // if (output_root_) output_root_->Close();
  if (produceSkim_)         skim_file_->Close();
  if (produceReducedSkim_)  reduced_skim_file_->Close();

  // Keep only skim/reduced outputs; remove legacy main ROOT and cut-efficiency DAT files.
  std::remove(((*outputFileName_) + ".root").c_str());
  std::remove(((*cutEfficFile_) + ".dat").c_str());
}

//--------------------------------------------------------------------------------------------------
// Setup
//--------------------------------------------------------------------------------------------------

void baseClass::init()
{
  tree_ = nullptr;

  const int readInput = readInputList();
  readCutFile();

  if (tree_ == nullptr) {
    STDOUT("baseClass::init(): ERROR: tree_ = nullptr");
    return;
  }

  Init(tree_);

  // Remove stale legacy outputs from previous runs.
  std::remove(((*outputFileName_) + ".root").c_str());
  std::remove(((*cutEfficFile_) + ".dat").c_str());

  // Output ROOT file (disabled to keep only _reduced_skim.root output)
  // output_root_ = new TFile(((*outputFileName_) + ".root").c_str(), "RECREATE");
  output_root_ = nullptr;

  // --- Skim ---
  produceSkim_  = (int(getSkimPreCutValue("produceSkim")) == 1);
  NAfterSkim_   = 0;

  if (produceSkim_) {
    skim_file_ = new TFile(((*outputFileName_) + "_skim.root").c_str(), "RECREATE");
    skim_tree_ = fChain->CloneTree(0);

    hCount_ = new TH1I("EventCounter", "Event Counter", 3, -0.5, 2.5);
    hCount_->GetXaxis()->SetBinLabel(1, "all events");
    hCount_->GetXaxis()->SetBinLabel(2, "events analyzed");
    hCount_->GetXaxis()->SetBinLabel(3, "passed");
  }

  // --- Reduced Skim ---
  produceReducedSkim_ = (int(getSkimPreCutValue("produceReducedSkim")) == 1);
  NAfterReducedSkim_  = 0;

  if (produceReducedSkim_) {
    reduced_skim_file_ = new TFile(((*outputFileName_) + "_reduced_skim.root").c_str(), "RECREATE");
    TDirectory* reduced_root_tuple_dir = reduced_skim_file_->mkdir("rootTupleTree");
    reduced_root_tuple_dir->cd();
    reduced_skim_tree_ = new TTree("tree", "Reduced Skim");
    reduced_skim_tree_->SetDirectory(reduced_root_tuple_dir);
    reduced_skim_file_->cd();

    hReducedCount_ = new TH1I("EventCounter", "Event Counter", 3, -0.5, 2.5);
    hReducedCount_->GetXaxis()->SetBinLabel(1, "all events");
    hReducedCount_->GetXaxis()->SetBinLabel(2, "analyzed events");
    hReducedCount_->GetXaxis()->SetBinLabel(3, "passed");

    for (auto it = cutName_cut_.begin(); it != cutName_cut_.end(); ++it) {
      cut& c = it->second;
      if (c.saveVariableInReducedSkim) {
        reduced_skim_tree_->Branch(c.variableName.c_str(), &c.value, (c.variableName + "/D").c_str());
      }
    }
  }

  // Optimizer histogram (created later if needed)
}

//--------------------------------------------------------------------------------------------------
// Input list
//--------------------------------------------------------------------------------------------------

int baseClass::readInputList()
{
  auto* chain = new TChain(treeName_->c_str());

  char pName[500];
  skimWasMade_           = true;
  jsonFileWasUsed_       = false;
  pileupMCFileWasUsed_   = false;
  pileupDataFileWasUsed_ = false;

  NBeforeSkim_ = 0;
  int count_line = 0;

  STDOUT("baseClass::readinputList(): inputList_ = " << *inputList_);

  std::ifstream is(inputList_->c_str());
  if (!is.good()) {
    STDOUT("baseClass::readInputList: ERROR opening inputList: " << *inputList_);
    return 0;
  }

  STDOUT("baseClass::readInputList: Starting reading list: " << *inputList_);
  while (is.getline(pName, 500, '\n')) {
    if (pName[0] == '#')   continue;
    if (pName[0] == '\n')  continue;  // skip blank lines

    ++count_line;
    STDOUT("Adding file: " << pName);
    chain->Add(pName);

    const int NBeforeSkim = getGlobalInfoNstart(pName);
    NBeforeSkim_ += NBeforeSkim;
    STDOUT("Initial number of events: NBeforeSkim, NBeforeSkim_ = " << NBeforeSkim << ", " << NBeforeSkim_);

    // Retrieve HLT path names from the first file
    if (count_line == 1) getHltMap(pName);
  }

  tree_ = chain;
  const int entries = chain->GetEntries();
  STDOUT("chain entries = " << entries);
  STDOUT("baseClass::readInputList: Finished reading list: " << *inputList_);

  is.close();
  return 1;
}

//--------------------------------------------------------------------------------------------------
// Utilities
//--------------------------------------------------------------------------------------------------

static bool is_number(const std::string& s)
{
  std::istringstream ss(s);
  double number;
  return static_cast<bool>(ss >> number);
}

//--------------------------------------------------------------------------------------------------
// Cut file parsing
//--------------------------------------------------------------------------------------------------

void baseClass::readCutFile()
{
  STDOUT("Reading cutFile_ = " << *cutFile_);

  std::ifstream is(cutFile_->c_str());
  if (!is.good()) {
    STDOUT("ERROR opening cutFile: " << *cutFile_);
    exit(1);
  }

  int id = 0;
  int optimize_count = 0;
  std::string s;

  while (std::getline(is, s)) {
    STDOUT("read line: " << s);
    if (s.empty() || s[0] == '#') continue;

    std::vector<std::string> v = split(s);
    if (v.empty()) continue;

    // ---------- JSON ----------
    if (v[0] == "JSON") {
      if (jsonFileWasUsed_) {
        STDOUT("ERROR: Please specify only one JSON file in your cut file!");
        return;
      }
      if (v.size() != 2) {
        STDOUT("ERROR: In your cutfile, JSON file line must have the syntax: \"JSON <full json file path>\"");
      }
      jsonFileName_ = v[1];
      STDOUT("Getting JSON file: " << v[1]);
      jsonParser_.parseJSONFile(v[1]);
      jsonParser_.printGoodLumis();
      jsonFileWasUsed_ = true;
      continue;
    }

    // ---------- PILEUP DATA ----------
    if (v[0] == "PILEUP_DATA_ROOT_FILE") {
      if (pileupDataFileWasUsed_) {
        STDOUT("ERROR: Please specify only one PILEUP_DATA_ROOT_FILE in your cut file!");
        return;
      }
      if (v.size() != 2) {
        STDOUT("ERROR: In your cutfile, PILEUP_DATA_ROOT_FILE line must have the syntax: \"PILEUP_DATA_ROOT_FILE <path>\"");
      }
      pileupDataFileName_ = v[1];
      STDOUT("Getting PILEUP_DATA_ROOT_FILE: " << v[1]);
      pileupReweighter_.readPileupDataFile(&v[1]);
      pileupDataFileWasUsed_ = true;
      continue;
    }

    // ---------- PILEUP MC ----------
    if (v[0] == "PILEUP_MC_TXT_FILE") {
      if (pileupMCFileWasUsed_) {
        STDOUT("ERROR: Please specify only one PILEUP_MC_TXT_FILE in your cut file!");
        return;
      }
      if (v.size() != 2) {
        STDOUT("ERROR: In your cutfile, PILEUP_MC_TXT_FILE line must have the syntax: \"PILEUP_MC_TXT_FILE <path>\"");
      }
      pileupMCFileName_ = v[1];
      STDOUT("Getting PILEUP_MC_TXT_FILE: " << v[1]);
      pileupReweighter_.readPileupMCFile(&v[1]);
      pileupMCFileWasUsed_ = true;
      continue;
    }

    // ---------- Optimizer entry ----------
    if (v.size() > 1 && v[1] == "OPT") {
      if (optimizeName_cut_.size() >= 6) {
        STDOUT("WARNING: Optimizer can accept up to 6 variables. '" << v[0] << "' not included.");
        continue;
      }
      bool found = false;
      for (int i = 0; i < (int)optimizeName_cut_.size(); ++i) {
        if (optimizeName_cut_[i].variableName == v[0]) {
          STDOUT("ERROR: variableName = " << v[0]
                 << " already optimized. Skipping.");
          found = true;
          break;
        }
      }
      if (found) continue;

      const int level_int  = std::atoi(v[5].c_str());
      const bool greater   = (v[2] != "<");
      const double minval  = std::atof(v[3].c_str());
      const double maxval  = std::atof(v[4].c_str());

      Optimize opt(optimize_count, v[0], minval, maxval, greater, level_int, nOptimizerCuts_);
      optimizeName_cut_[optimize_count] = opt;
      ++optimize_count;
      continue;
    }

    // ---------- Pre-cut (level = -1) ----------
    const int level_int = std::atoi(v[5].c_str());
    if (level_int == -1) {
      if (preCutName_cut_.find(v[0]) != preCutName_cut_.end()) {
        STDOUT("ERROR: variableName = " << v[0] << " exists already in preCutName_cut_. Returning.");
        return;
      }
      preCutInfo_ << "### Preliminary cut values: " << s << std::endl;

      preCut pc;
      pc.variableName = v[0];

      // NOTE: fixed bug — assign value1..4 properly.
      if (is_number(v[1])) pc.value1 = decodeCutValue(v[1]); else pc.string1 = v[1];
      if (is_number(v[2])) pc.value2 = decodeCutValue(v[2]); else pc.string2 = v[2];
      if (is_number(v[3])) pc.value3 = decodeCutValue(v[3]); else pc.string3 = v[3];
      if (is_number(v[4])) pc.value4 = decodeCutValue(v[4]); else pc.string4 = v[4];

      preCutName_cut_[pc.variableName] = pc;
      continue;
    }

    // ---------- Normal cut ----------
    if (cutName_cut_.find(v[0]) != cutName_cut_.end()) {
      STDOUT("ERROR: variableName = " << v[0] << " exists already in cutName_cut_. Returning.");
      return;
    }

    cut c;
    c.variableName = v[0];

    const std::string& m1 = v[1];
    const std::string& M1 = v[2];
    std::string m2        = v[3];
    std::string M2        = v[4];

    if (m1 == "-" || M1 == "-") {
      STDOUT("ERROR: minValue1 and maxValue1 must be provided. Returning.");
      return;
    }
    if ((m2 == "-" && M2 != "-") || (m2 != "-" && M2 == "-")) {
      STDOUT("ERROR: If either minValue2 or maxValue2 is '-', both must be '-'. Returning.");
      return;
    }

    if (m2 == "-") m2 = "+inf";
    if (M2 == "-") M2 = "-inf";

    c.minValue1 = decodeCutValue(m1);
    c.maxValue1 = decodeCutValue(M1);
    c.minValue2 = decodeCutValue(m2);
    c.maxValue2 = decodeCutValue(M2);
    c.level_int = level_int;
    c.level_str = v[5];

    c.histoNBins = std::atoi(v[6].c_str());
    c.histoMin   = std::atof(v[7].c_str());
    c.histoMax   = std::atof(v[8].c_str());

    c.saveVariableInReducedSkim = (v.size() == 10 && v[9] == "SAVE");

    // Internal bookkeeping
    c.id = ++id;

    const std::string h1 = std::string(skimWasMade_ ? "cutHisto_skim___________________"
                                                    : "cutHisto_noCuts_________________") + c.variableName;
    const std::string h2 = "cutHisto_allPreviousCuts________" + c.variableName;
    const std::string h3 = "cutHisto_allOthrSmAndLwrLvlCuts_" + c.variableName;
    const std::string h4 = "cutHisto_allOtherCuts___________" + c.variableName;
    const std::string h5 = "cutHisto_allCuts________________" + c.variableName;

    c.histo1 = TH1F(h1.c_str(), "", c.histoNBins, c.histoMin, c.histoMax);
    c.histo2 = TH1F(h2.c_str(), "", c.histoNBins, c.histoMin, c.histoMax);
    c.histo3 = TH1F(h3.c_str(), "", c.histoNBins, c.histoMin, c.histoMax);
    c.histo4 = TH1F(h4.c_str(), "", c.histoNBins, c.histoMin, c.histoMax);
    c.histo5 = TH1F(h5.c_str(), "", c.histoNBins, c.histoMin, c.histoMax);

    c.histo1.Sumw2(); c.histo2.Sumw2(); c.histo3.Sumw2(); c.histo4.Sumw2(); c.histo5.Sumw2();

    c.filled  = false;
    c.value   = 0.0;
    c.weight  = 1.0;
    c.passed  = false;
    c.nEvtPassedBeforeWeight                    = 0;
    c.nEvtPassed                                = 0.0;
    c.nEvtPassedErr2                            = 0.0;
    c.nEvtPassedBeforeWeight_alreadyFilled      = false;

    orderedCutNames_.push_back(c.variableName);
    cutName_cut_[c.variableName] = c;
  }

  if (pileupMCFileWasUsed_ && pileupDataFileWasUsed_) {
    pileupReweighter_.calculatePileupWeights();
    pileupReweighter_.printPileupWeights();
  } else if (pileupMCFileWasUsed_ != pileupDataFileWasUsed_) {
    STDOUT("ERROR: You must specify TWO pileup files in your cutfile:");
    if (pileupMCFileWasUsed_)   STDOUT("   PILEUP_MC_TXT_FILE provided");
    if (pileupDataFileWasUsed_) STDOUT("   PILEUP_DATA_ROOT_FILE provided");
    exit(1);
  }

  STDOUT("baseClass::readCutFile: Finished reading cutFile: " << *cutFile_);
  STDOUT("baseClass::readCutFile: #optimized variables: " << optimizeName_cut_.size());

  // Optimizer histogram
  if (!optimizeName_cut_.empty()) {
    const int nbins = (int)std::pow(nOptimizerCuts_, optimizeName_cut_.size());
    h_optimizer_ = new TH1F("optimizer", "Optimization of cut variables", nbins, 0, nbins);
  }

  // Event-passing histogram (NoCuts + optional Skim + all cuts)
  int cutsize = (int)orderedCutNames_.size() + 1;
  if (skimWasMade_) ++cutsize;

  gDirectory->cd();
  eventcuts_ = new TH1F("EventsPassingCuts", "Events Passing Cuts", cutsize, 0, cutsize);

  is.close();
}

//--------------------------------------------------------------------------------------------------
// Cut bookkeeping
//--------------------------------------------------------------------------------------------------

void baseClass::resetCuts(const std::string& mode)
{
  for (auto it = cutName_cut_.begin(); it != cutName_cut_.end(); ++it) {
    cut& c = it->second;

    c.filled  = false;
    c.value   = 0.0;
    c.weight  = 1.0;
    c.passed  = false;

    if (mode == "newEvent" || mode.empty()) {
      c.nEvtPassedBeforeWeight_alreadyFilled = false;
    } else if (mode != "sameEvent") {
      STDOUT("ERROR: unrecognized option. Allowed: 'sameEvent', 'newEvent' (default = 'newEvent').");
    }
  }
  combCutName_passed_.clear();
}

void baseClass::fillVariableWithValue(const std::string& name, const double& val, const double& w)
{
  auto it = cutName_cut_.find(name);
  if (it == cutName_cut_.end()) {
    STDOUT("ERROR: variableName = " << name << " not found in cutName_cut_. Returning");
    return;
  }
  cut& c = it->second;
  c.filled = true;
  c.value  = val;
  c.weight = w;

  // If pileup weights are in use, this is where you'd apply them:
  // if (pileupReweighter_.pileupWeightsCalculated()) c.weight *= PileupWeight_;

  fillOptimizerWithValue(name, val);
}

bool baseClass::variableIsFilled(const std::string& name)
{
  auto it = cutName_cut_.find(name);
  if (it == cutName_cut_.end()) STDOUT("ERROR: did not find variableName = " << name << " in cutName_cut_.");
  return it->second.filled;
}

double baseClass::getVariableValue(const std::string& name)
{
  auto it = cutName_cut_.find(name);
  if (it == cutName_cut_.end()) {
    STDOUT("ERROR: did not find variableName = " << name << " in cutName_cut_.");
  }
  if (!variableIsFilled(name)) {
    STDOUT("ERROR: requesting value of not filled variable " << name);
  }
  return it->second.value;
}

void baseClass::fillOptimizerWithValue(const std::string& name, const double& val)
{
  for (int i = 0; i < (int)optimizeName_cut_.size(); ++i) {
    if (optimizeName_cut_[i].variableName == name) {
      optimizeName_cut_[i].value = val;
      return;
    }
  }
}

void baseClass::evaluateCuts()
{
  combCutName_passed_.clear();

  for (const auto& cname : orderedCutNames_) {
    cut& c = cutName_cut_.find(cname)->second;

    const bool inWin1 = (c.minValue1 < c.value && c.value <= c.maxValue1);
    const bool inWin2 = (c.minValue2 < c.value && c.value <= c.maxValue2);
    const bool pass   = c.filled && (inWin1 || inWin2);

    c.passed = pass;

    combCutName_passed_[c.level_str] = (
      combCutName_passed_.find(c.level_str) == combCutName_passed_.end()
      ? pass
      : (combCutName_passed_[c.level_str] && pass)
    );

    combCutName_passed_["all"] = (
      combCutName_passed_.find("all") == combCutName_passed_.end()
      ? pass
      : (combCutName_passed_["all"] && pass)
    );
  }

  runOptimizer();

  if (!fillCutHistos())   { STDOUT("ERROR: fillCutHistos did not complete successfully."); }
  if (!updateCutEffic())  { STDOUT("ERROR: updateCutEffic did not complete successfully."); }
}

void baseClass::runOptimizer()
{
  if (optimizeName_cut_.empty()) return;

  // Require all non-optimized cuts to pass
  for (const auto& cname : orderedCutNames_) {
    bool ignore = false;
    for (const auto& kv : optimizeName_cut_) {
      const Optimize& opt = kv.second;
      if (opt.variableName == cname) { ignore = true; break; }
    }
    if (ignore) continue;
    if (!passedCut(cname)) return;
  }

  // Build acceptance mask across N^M combinations
  const int thesize = (int)optimizeName_cut_.size();
  const int nbins   = (int)std::pow(nOptimizerCuts_, thesize);
  std::vector<bool> accept(nbins, true);

  int mysize = thesize;
  for (int cc = 0; cc < thesize; ++cc) {
    --mysize;
    for (int i = 0; i < nOptimizerCuts_; ++i) {
      if (!optimizeName_cut_[cc].Compare(i)) {
        for (int j = (int)(i * std::pow(nOptimizerCuts_, mysize)); j < nbins; ++j) {
          if ((j / (int)std::pow(nOptimizerCuts_, mysize)) % nOptimizerCuts_ == i) {
            accept[j] = false;
          }
        }
      }
    }
  }

  const double lastWeight = cutName_cut_[orderedCutNames_.back()].weight;
  for (int i = 0; i < nbins; ++i) {
    if (accept[i]) h_optimizer_->Fill(i, lastWeight);
  }
}

//--------------------------------------------------------------------------------------------------
// Cut pass helpers
//--------------------------------------------------------------------------------------------------

bool baseClass::passedCut(const std::string& name)
{
  auto cIt = cutName_cut_.find(name);
  if (cIt != cutName_cut_.end()) {
    const cut& c = cIt->second;
    return (c.filled && c.passed);
  }

  auto mIt = combCutName_passed_.find(name);
  if (mIt != combCutName_passed_.end()) {
    return mIt->second;
  }

  STDOUT("ERROR: did not find variableName = " << name
         << " neither in cutName_cut_ nor combCutName_passed_. Returning false.");
  return false;
}

bool baseClass::passedAllPreviousCuts(const std::string& name)
{
  auto cc = cutName_cut_.find(name);
  if (cc == cutName_cut_.end()) {
    STDOUT("ERROR: did not find variableName = " << name << " in cutName_cut_. Returning false.");
    return false;
  }

  for (const auto& cname : orderedCutNames_) {
    cut& c = cutName_cut_.find(cname)->second;
    if (c.variableName == name) return true;
    if (!(c.filled && c.passed)) return false;
  }

  STDOUT("ERROR. It should never pass from here.");
  return false;
}

bool baseClass::passedAllOtherCuts(const std::string& name)
{
  auto cc = cutName_cut_.find(name);
  if (cc == cutName_cut_.end()) {
    STDOUT("ERROR: did not find variableName = " << name << " in cutName_cut_. Returning false.");
    return false;
  }

  for (auto it = cutName_cut_.begin(); it != cutName_cut_.end(); ++it) {
    const cut& c = it->second;
    if (c.variableName == name) continue;
    if (!(c.filled && c.passed)) return false;
  }
  return true;
}

bool baseClass::passedAllOtherSameAndLowerLevelCuts(const std::string& name)
{
  auto cc = cutName_cut_.find(name);
  if (cc == cutName_cut_.end()) {
    STDOUT("ERROR: did not find variableName = " << name << " in cutName_cut_. Returning false.");
    return false;
  }

  const int level = cc->second.level_int;
  for (auto it = cutName_cut_.begin(); it != cutName_cut_.end(); ++it) {
    const cut& c = it->second;
    if (c.variableName == name || c.level_int > level) continue;
    if (!(c.filled && c.passed)) return false;
  }
  return true;
}

//--------------------------------------------------------------------------------------------------
// Getters for pre-cuts and cuts
//--------------------------------------------------------------------------------------------------

double baseClass::getPreCutValue1(const std::string& s)
{
  auto it = preCutName_cut_.find(s);
  if (it == preCutName_cut_.end()) STDOUT("ERROR: " << s << " not in preCutName_cut_.");
  const preCut& c = it->second;
  if (c.value1 == kMinusInf) STDOUT("ERROR: value1 of preliminary cut " << s << " was not provided.");
  return c.value1;
}

double baseClass::getPreCutValue2(const std::string& s)
{
  auto it = preCutName_cut_.find(s);
  if (it == preCutName_cut_.end()) STDOUT("ERROR: " << s << " not in preCutName_cut_.");
  const preCut& c = it->second;
  if (c.value2 == kMinusInf) STDOUT("ERROR: value2 of preliminary cut " << s << " was not provided.");
  return c.value2;
}

double baseClass::getPreCutValue3(const std::string& s)
{
  auto it = preCutName_cut_.find(s);
  if (it == preCutName_cut_.end()) STDOUT("ERROR: " << s << " not in preCutName_cut_.");
  const preCut& c = it->second;
  if (c.value3 == kMinusInf) STDOUT("ERROR: value3 of preliminary cut " << s << " was not provided.");
  return c.value3;
}

double baseClass::getPreCutValue4(const std::string& s)
{
  auto it = preCutName_cut_.find(s);
  if (it == preCutName_cut_.end()) STDOUT("ERROR: " << s << " not in preCutName_cut_.");
  const preCut& c = it->second;
  if (c.value4 == kMinusInf) STDOUT("ERROR: value4 of preliminary cut " << s << " was not provided.");
  return c.value4;
}

std::string baseClass::getPreCutString1(const std::string& s)
{
  auto it = preCutName_cut_.find(s);
  if (it == preCutName_cut_.end()) STDOUT("ERROR: " << s << " not in preCutName_cut_.");
  return it->second.string1;
}

double baseClass::getCutMinValue1(const std::string& s) { return cutName_cut_.find(s)->second.minValue1; }
double baseClass::getCutMaxValue1(const std::string& s) { return cutName_cut_.find(s)->second.maxValue1; }
double baseClass::getCutMinValue2(const std::string& s) { return cutName_cut_.find(s)->second.minValue2; }
double baseClass::getCutMaxValue2(const std::string& s) { return cutName_cut_.find(s)->second.maxValue2; }

//--------------------------------------------------------------------------------------------------
// Histogram accessors
//--------------------------------------------------------------------------------------------------

const TH1F& baseClass::getHisto_noCuts_or_skim(const std::string& s)           { return cutName_cut_.find(s)->second.histo1; }
const TH1F& baseClass::getHisto_allPreviousCuts(const std::string& s)          { return cutName_cut_.find(s)->second.histo2; }
const TH1F& baseClass::getHisto_allOthrSmAndLwrLvlCuts(const std::string& s)   { return cutName_cut_.find(s)->second.histo3; }
const TH1F& baseClass::getHisto_allOtherCuts(const std::string& s)             { return cutName_cut_.find(s)->second.histo4; }
const TH1F& baseClass::getHisto_allCuts(const std::string& s)                  { return cutName_cut_.find(s)->second.histo5; }

int    baseClass::getHistoNBins(const std::string& s) { return cutName_cut_.find(s)->second.histoNBins; }
double baseClass::getHistoMin (const std::string& s)  { return cutName_cut_.find(s)->second.histoMin;  }
double baseClass::getHistoMax (const std::string& s)  { return cutName_cut_.find(s)->second.histoMax;  }

//--------------------------------------------------------------------------------------------------
// Histogram filling / writing
//--------------------------------------------------------------------------------------------------

bool baseClass::fillCutHistos()
{
  for (const auto& name : orderedCutNames_) {
    cut& c = cutName_cut_.find(name)->second;
    if (!c.filled) continue;

    if (fillSkim_) c.histo1.Fill(c.value, c.weight);

    if (fillAllPreviousCuts_ && passedAllPreviousCuts(c.variableName))
      c.histo2.Fill(c.value, c.weight);

    if (fillAllSameLevelAndLowerLevelCuts_ && passedAllOtherSameAndLowerLevelCuts(c.variableName))
      c.histo3.Fill(c.value, c.weight);

    if (fillAllOtherCuts_ && passedAllOtherCuts(c.variableName))
      c.histo4.Fill(c.value, c.weight);

    if (fillAllCuts_ && passedCut("all"))
      c.histo5.Fill(c.value, c.weight);
  }
  return true;
}

bool baseClass::writeCutHistos()
{
  output_root_->cd();
  for (const auto& name : orderedCutNames_) {
    const cut& c = cutName_cut_.find(name)->second;
    if (fillSkim_)            c.histo1.Write();
    if (fillAllPreviousCuts_) c.histo2.Write();
    if (fillAllOtherCuts_)    c.histo4.Write();
#ifdef SAVE_ALL_HISTOGRAMS
    if (fillAllSameLevelAndLowerLevelCuts_) c.histo3.Write();
    if (fillAllCuts_)                       c.histo5.Write();
#endif
  }
  return true;
}

bool baseClass::updateCutEffic()
{
  for (const auto& name : orderedCutNames_) {
    cut& c = cutName_cut_.find(name)->second;

    if (!passedAllPreviousCuts(c.variableName)) continue;

    if (passedCut(c.variableName)) {
      if (!c.nEvtPassedBeforeWeight_alreadyFilled) {
        c.nEvtPassedBeforeWeight += 1;
        c.nEvtPassedBeforeWeight_alreadyFilled = true;
      }
      c.nEvtPassed     += c.weight;
      c.nEvtPassedErr2 += (c.weight * c.weight);
    }
  }
  return true;
}

bool baseClass::writeCutEfficFile()
{
  output_root_->cd();

  // Label event counter bins
  int bincounter = 1;
  eventcuts_->GetXaxis()->SetBinLabel(bincounter++, "NoCuts");
  if (skimWasMade_) eventcuts_->GetXaxis()->SetBinLabel(bincounter++, "Skim");
  for (const auto& n : orderedCutNames_) {
    eventcuts_->GetXaxis()->SetBinLabel(bincounter++, n.c_str());
  }

  // Stats file
  const int nEntRoottuple = fChain->GetEntriesFast();
  const int nEntTot       = (skimWasMade_ ? NBeforeSkim_ : nEntRoottuple);

  const std::string cutEfficFile = (*cutEfficFile_) + ".dat";
  std::ofstream os(cutEfficFile.c_str());

  if (jsonFileWasUsed_) {
    os << "################################## JSON file used at runtime    ############################\n"
       << "### " << jsonFileName_ << "\n";
  } else {
    os << "################################## NO JSON file used at runtime ############################\n";
  }

  if (pileupMCFileWasUsed_ && pileupDataFileWasUsed_) {
    os << "################################## PILEUP files used #######################################\n"
       << "### " << pileupMCFileName_   << "\n"
       << "### " << pileupDataFileName_ << "\n";
  } else {
    os << "################################## NO PILEUP files used ####################################\n";
  }

  os << "################################## Preliminary Cut Values ##################################\n"
     << "########################### variableName                        value1          value2          value3          value4          level\n"
     << preCutInfo_.str();

  // Header
  os << std::fixed
     << "################################## Cuts ####################################################\n"
     << "#id             variableName           min1           max1           min2           max2          level"
     << "              N          Npass         EffRel      errEffRel         EffAbs      errEffAbs\n";

  // NoCuts row
  const int precision = 4;
  os.precision(precision);

  int cutIdPed = 0;
  double minForFixed = 0.1;

  int bin = 1;
  eventcuts_->SetBinContent(bin++, nEntTot);
  if (!optimizeName_cut_.empty()) h_optimizer_->SetBinContent(0, nEntTot);

  os << std::setw(3)  << cutIdPed
     << std::setw(35) << "nocut"
     << std::setprecision(4)
     << std::setw(15) << "-"
     << std::setw(15) << "-"
     << std::setw(15) << "-"
     << std::setw(15) << "-"
     << std::setw(15) << "-"
     << std::setw(15) << nEntTot
     << std::setw(15) << nEntTot
     << std::setw(15) << 1.0
     << std::setw(15) << 0.0
     << std::setw(15) << 1.0
     << std::setw(15) << 0.0
     << "\n";

  double nEvtPassedBeforeWeight_previousCut = nEntTot;
  double nEvtPassed_previousCut             = nEntTot;

  // Skim row if present
  if (skimWasMade_) {
    eventcuts_->SetBinContent(bin++, nEntRoottuple);

    const double effRel    = double(nEntRoottuple) / double(NBeforeSkim_);
    const double effRelErr = std::sqrt(effRel * (1.0 - effRel) / double(NBeforeSkim_));
    const double effAbs    = effRel;
    const double effAbsErr = effRelErr;

    os << std::fixed
       << std::setw(3)  << ++cutIdPed
       << std::setw(35) << "skim"
       << std::setw(15) << "-"
       << std::setw(15) << "-"
       << std::setw(15) << "-"
       << std::setw(15) << "-"
       << std::setw(15) << "-"
       << std::setw(15) << NBeforeSkim_
       << std::setw(15) << nEntRoottuple
       << std::setw(15) << (effRel    < minForFixed ? std::scientific : std::fixed) << effRel
       << std::setw(15) << (effRelErr < minForFixed ? std::scientific : std::fixed) << effRelErr
       << std::setw(15) << (effAbs    < minForFixed ? std::scientific : std::fixed) << effAbs
       << std::setw(15) << (effAbsErr < minForFixed ? std::scientific : std::fixed) << effAbsErr
       << std::fixed << "\n";

    nEvtPassedBeforeWeight_previousCut = nEntRoottuple;
    nEvtPassed_previousCut             = nEntRoottuple;
  }

  // Per-cut rows
  for (const auto& name : orderedCutNames_) {
    cut& c = cutName_cut_.find(name)->second;

    eventcuts_->SetBinContent(bin++, c.nEvtPassed);

    const double effRel = (nEvtPassed_previousCut > 0.0)
                          ? (double)c.nEvtPassed / nEvtPassed_previousCut : 0.0;

    const double N  = nEvtPassedBeforeWeight_previousCut;
    const double Np = c.nEvtPassedBeforeWeight;
    const double p  = (N > 0.0 ? Np / N : 0.0);
    const double q  = 1.0 - p;
    const double w  = (c.nEvtPassedBeforeWeight > 0.0 ? c.nEvtPassed / c.nEvtPassedBeforeWeight : 0.0);

    const double effRelErr = (N > 0.0) ? std::sqrt(p * q / N) * w : 0.0;

    const double effAbs    = (nEntTot > 0.0 ? (double)c.nEvtPassed / (double)nEntTot : 0.0);
    const double pAbs      = (nEntTot > 0.0 ? Np / (double)nEntTot : 0.0);
    const double qAbs      = 1.0 - pAbs;
    const double effAbsErr = (nEntTot > 0.0) ? std::sqrt(pAbs * qAbs / (double)nEntTot) * w : 0.0;

    std::stringstream ssm1, ssM1, ssm2, ssM2;
    ssm1 << std::fixed << std::setprecision(4) << c.minValue1;
    ssM1 << std::fixed << std::setprecision(4) << c.maxValue1;

    if (c.minValue2 == kMinusInf) ssm2 << "-inf"; else ssm2 << std::fixed << std::setprecision(4) << c.minValue2;
    if (c.maxValue2 == kPlusInf ) ssM2 << "+inf"; else ssM2 << std::fixed << std::setprecision(4) << c.maxValue2;

    os << std::setw(3)  << (cutIdPed + c.id)
       << std::setw(35) << c.variableName
       << std::setw(15) << ((c.minValue1 == kMinusInf) ? "-inf" : ssm1.str())
       << std::setw(15) << ((c.maxValue1 == kPlusInf ) ? "+inf" : ssM1.str())
       << std::setw(15) << ((c.minValue2 > c.maxValue2) ? "-" : ssm2.str())
       << std::setw(15) << ((c.minValue2 > c.maxValue2) ? "-" : ssM2.str())
       << std::setw(15) << c.level_int
       << std::setw(15) << nEvtPassedBeforeWeight_previousCut
       << std::setw(15) << c.nEvtPassed
       << std::setw(15) << (effRel    < minForFixed ? std::scientific : std::fixed) << effRel
       << std::setw(15) << (effRelErr < minForFixed ? std::scientific : std::fixed) << effRelErr
       << std::setw(15) << (effAbs    < minForFixed ? std::scientific : std::fixed) << effAbs
       << std::setw(15) << (effAbsErr < minForFixed ? std::scientific : std::fixed) << effAbsErr
       << std::fixed << "\n";

    nEvtPassedBeforeWeight_previousCut = c.nEvtPassedBeforeWeight;
    nEvtPassed_previousCut             = c.nEvtPassed;
  }

  // Optimizer artifacts
  if (!optimizeName_cut_.empty()) {
    gDirectory->mkdir("Optimizer");
    gDirectory->cd("Optimizer");
    h_optimizer_->Write();
    for (int i = 0; i < (int)optimizeName_cut_.size(); ++i) {
      std::stringstream x;
      x << "Cut" << i << "_" << optimizeName_cut_[i].variableName
        << (optimizeName_cut_[i].testgreater ? "_gt_" : "_lt_")
        << optimizeName_cut_[i].minvalue << "_to_" << optimizeName_cut_[i].maxvalue;
      TObjString(x.str().c_str()).Write();
    }
    gDirectory->cd("..");
  }

  eventcuts_->Write(); // write here; WriteCutHistos runs earlier
  return true;
}

//--------------------------------------------------------------------------------------------------
// Misc helpers
//--------------------------------------------------------------------------------------------------

bool baseClass::sortCuts(const cut& X, const cut& Y) { return X.id < Y.id; }

std::vector<std::string> baseClass::split(const std::string& s)
{
  std::vector<std::string> out;
  std::string::size_type i = 0;

  while (i != s.size()) {
    while (i != s.size() && std::isspace(s[i])) ++i;
    std::string::size_type j = i;
    while (j != s.size() && !std::isspace(s[j])) ++j;
    if (i != j) {
      out.push_back(s.substr(i, j - i));
      i = j;
    }
  }
  return out;
}

double baseClass::decodeCutValue(const std::string& s)
{
  if (s == "inf"  || s == "+inf") return kPlusInf;
  if (s == "-inf" || s == "-")    return kMinusInf;
  return std::atof(s.c_str());
}

int baseClass::getGlobalInfoNstart(char* pName)
{
  int NBeforeSkim = 0;
  STDOUT(pName << "  " << NBeforeSkim);

  TFile* f = TFile::Open(pName);
  TH1I* hCount1 = (TH1I*)f->Get(kHistoCountNstart1.c_str());
  TH1I* hCount2 = (TH1I*)f->Get(kHistoCountNstart2.c_str());
  TH1I* hCount3 = (TH1I*)f->Get(kHistoCountNstart3.c_str());

  if (!hCount1 && !hCount2 && !hCount3) {
    STDOUT("Skim filter histogram(s) not found. Will assume skim was not made for ALL files.");
    skimWasMade_ = false;
    f->Close();
    return NBeforeSkim;
  }

  if      (hCount1) NBeforeSkim = (int)hCount1->GetBinContent(1);
  else if (hCount2) NBeforeSkim = (int)hCount2->GetBinContent(1);
  else if (hCount3) NBeforeSkim = (int)hCount3->GetBinContent(1);

  STDOUT(pName << "  " << NBeforeSkim);
  f->Close();
  return NBeforeSkim;
}

//--------------------------------------------------------------------------------------------------
// User histogram helpers
//--------------------------------------------------------------------------------------------------

void baseClass::CreateAndFillUserTH1D(const char* name, Int_t nbinsx, Double_t xlow, Double_t xup,
                                      Double_t value, Double_t weight)
{
  auto it = userTH1Ds_.find(std::string(name));
  if (it == userTH1Ds_.end()) {
    TH1D* h = new TH1D(name, name, nbinsx, xlow, xup);
    h->Sumw2();
    userTH1Ds_[std::string(name)] = h;
    h->Fill(value, weight);
  } else {
    it->second->Fill(value, weight);
  }
}

void baseClass::CreateUserTH1D(const char* name, Int_t nbinsx, Double_t xlow, Double_t xup)
{
  auto it = userTH1Ds_.find(std::string(name));
  if (it == userTH1Ds_.end()) {
    TH1D* h = new TH1D(name, name, nbinsx, xlow, xup);
    h->Sumw2();
    userTH1Ds_[std::string(name)] = h;
  } else {
    STDOUT("ERROR: trying to define already existing histogram " << name);
  }
}

void baseClass::FillUserTH1D(const char* name, Double_t value, Double_t weight)
{
  auto it = userTH1Ds_.find(std::string(name));
  if (it == userTH1Ds_.end()) {
    STDOUT("ERROR: trying to fill histogram " << name << " that was not defined.");
  } else {
    it->second->Fill(value, weight);
  }
}

void baseClass::CreateAndFillUserTH2D(const char* name, Int_t nbinsx, Double_t xlow, Double_t xup,
                                      Int_t nbinsy, Double_t ylow, Double_t yup,
                                      Double_t vx, Double_t vy, Double_t weight)
{
  auto it = userTH2Ds_.find(std::string(name));
  if (it == userTH2Ds_.end()) {
    TH2D* h = new TH2D(name, name, nbinsx, xlow, xup, nbinsy, ylow, yup);
    h->Sumw2();
    userTH2Ds_[std::string(name)] = h;
    h->Fill(vx, vy, weight);
  } else {
    it->second->Fill(vx, vy, weight);
  }
}

void baseClass::CreateUserTH2D(const char* name, Int_t nbinsx, Double_t xlow, Double_t xup,
                               Int_t nbinsy, Double_t ylow, Double_t yup)
{
  auto it = userTH2Ds_.find(std::string(name));
  if (it == userTH2Ds_.end()) {
    TH2D* h = new TH2D(name, name, nbinsx, xlow, xup, nbinsy, ylow, yup);
    h->Sumw2();
    userTH2Ds_[std::string(name)] = h;
  } else {
    STDOUT("ERROR: trying to define already existing histogram " << name);
  }
}

void baseClass::CreateUserTH2D(const char* name, Int_t nbinsx, Double_t* x, Int_t nbinsy, Double_t* y)
{
  auto it = userTH2Ds_.find(std::string(name));
  if (it == userTH2Ds_.end()) {
    TH2D* h = new TH2D(name, name, nbinsx, x, nbinsy, y);
    h->Sumw2();
    userTH2Ds_[std::string(name)] = h;
  } else {
    STDOUT("ERROR: trying to define already existing histogram " << name);
  }
}

void baseClass::FillUserTH2D(const char* name, Double_t vx, Double_t vy, Double_t weight)
{
  auto it = userTH2Ds_.find(std::string(name));
  if (it == userTH2Ds_.end()) {
    STDOUT("ERROR: trying to fill histogram " << name << " that was not defined.");
  } else {
    it->second->Fill(vx, vy, weight);
  }
}

void baseClass::FillUserTH2DLower(const char* name, Double_t vx, Double_t vy, Double_t weight)
{
  auto it = userTH2Ds_.find(std::string(name));
  if (it == userTH2Ds_.end()) {
    STDOUT("ERROR: trying to fill histogram " << name << " that was not defined.");
    return;
  }

  TH2D* hist   = it->second;
  TAxis* xa    = hist->GetXaxis();
  TAxis* ya    = hist->GetYaxis();
  const int nx = hist->GetNbinsX();
  const int ny = hist->GetNbinsY();

  for (int ix = 1; ix <= nx; ++ix) {
    const double xmin = xa->GetBinLowEdge(ix);
    const double xcen = xa->GetBinCenter(ix);
    if (vx <= xmin) continue;

    for (int iy = 1; iy <= ny; ++iy) {
      const double ymin = ya->GetBinLowEdge(iy);
      const double ycen = ya->GetBinCenter(iy);
      if (vy <= ymin) continue;
      hist->Fill(xcen, ycen, weight);
    }
  }
}

bool baseClass::writeUserHistos()
{
  bool ret = true;
  output_root_->cd();

  for (std::map<std::string, TH1D*>::iterator it = userTH1Ds_.begin();
       it != userTH1Ds_.end(); ++it) {
    output_root_->cd();
    it->second->Write();
  }
  for (std::map<std::string, TH2D*>::iterator it = userTH2Ds_.begin();
       it != userTH2Ds_.end(); ++it) {
    output_root_->cd();
    it->second->Write();
  }
  return ret;
}


//--------------------------------------------------------------------------------------------------
// Skim writing
//--------------------------------------------------------------------------------------------------

double baseClass::getSkimPreCutValue(const std::string& s)
{
  auto it = preCutName_cut_.find(s);
  if (it == preCutName_cut_.end()) return 0.0;
  const preCut& c = it->second;
  if (c.value1 == kMinusInf) STDOUT("ERROR: value1 of preliminary cut " << s << " was not provided.");
  return c.value1;
}

void baseClass::fillSkimTree()
{
  if (!produceSkim_) return;
  skim_tree_->Fill();
  ++NAfterSkim_;
}

void baseClass::fillReducedSkimTree()
{
  if (!produceReducedSkim_) return;
  reduced_skim_tree_->Fill();
  ++NAfterReducedSkim_;
}

bool baseClass::writeSkimTree()
{
  if (!produceSkim_) return true;

  skim_file_->cd();
  TDirectory* dir1 = skim_file_->mkdir("DijetFilter");
  TDirectory* dir2 = dir1->mkdir("EventCount");
  skim_file_->cd("DijetFilter/EventCount");

  const int nEntRoottuple = fChain->GetEntriesFast();
  const int nEntTot       = (skimWasMade_ ? NBeforeSkim_ : nEntRoottuple);

  hCount_->SetBinContent(1, nEntTot);
  hCount_->SetBinContent(2, nEntRoottuple);
  hCount_->SetBinContent(3, NAfterSkim_);
  hCount_->Write();

  skim_file_->cd();
  skim_file_->mkdir("rootTupleTree");
  skim_file_->cd("rootTupleTree");
  if (fChain->GetEntries() == 0) {
    fChain->CloneTree(0)->Write("tree");
  } else {
    skim_tree_->Write("tree");
  }

  return true;
}

bool baseClass::writeReducedSkimTree()
{
  if (!produceReducedSkim_) return true;

  reduced_skim_file_->cd();
  TDirectory* reduced_root_tuple_dir = reduced_skim_file_->GetDirectory("rootTupleTree");
  if (reduced_root_tuple_dir == nullptr) {
    reduced_root_tuple_dir = reduced_skim_file_->mkdir("rootTupleTree");
  }
  reduced_root_tuple_dir->cd();
  reduced_skim_tree_->Write("tree", TObject::kOverwrite);

  reduced_skim_file_->cd();
  TDirectory* dir1 = reduced_skim_file_->mkdir("DijetFilter");
  TDirectory* dir2 = dir1->mkdir("EventCount");
  reduced_skim_file_->cd("DijetFilter/EventCount");

  const int nEntRoottuple = fChain->GetEntriesFast();
  const int nEntTot       = (skimWasMade_ ? NBeforeSkim_ : nEntRoottuple);

  hReducedCount_->SetBinContent(1, nEntTot);
  hReducedCount_->SetBinContent(2, nEntRoottuple);
  hReducedCount_->SetBinContent(3, NAfterReducedSkim_);
  hReducedCount_->Write();

  return true;
}

//--------------------------------------------------------------------------------------------------
// JSON, pileup, and trigger helpers
//--------------------------------------------------------------------------------------------------

int baseClass::passJSON(int this_run, int this_lumi, bool this_is_data)
{
  if (!this_is_data)      return 1;
  if (!jsonFileWasUsed_) {
    STDOUT("ERROR: passJSON invoked while running on data, but no JSON file was specified!");
    return 0;
  }
  return jsonParser_.isAGoodLumi(this_run, this_lumi);
}

double baseClass::getPileupWeight(int npileup, bool this_is_data)
{
  PileupWeight_ = 1.0;

  if (this_is_data)                                  return PileupWeight_;
  if (!pileupReweighter_.pileupWeightsCalculated())  return PileupWeight_;
  if (npileup == -1)                                  return PileupWeight_;

  PileupWeight_ = pileupReweighter_.getPileupWeight(npileup);
  return PileupWeight_;
}

void baseClass::getTriggers(std::string* /*HLTKey*/,
                            std::vector<std::string>* names,
                            std::vector<bool>* decisions,
                            std::vector<int>* prescales)
{
  triggerDecisionMap_.clear();
  triggerPrescaleMap_.clear();

  const int ntriggers = (int)names->size();
  for (int i = 0; i < ntriggers; ++i) {
    triggerDecisionMap_[(*names)[i]] = (*decisions)[i];
    triggerPrescaleMap_[(*names)[i]] = (*prescales)[i];
  }
}

void baseClass::printTriggers()
{
  STDOUT("Triggers include: ");
  for (auto it = triggerPrescaleMap_.begin(); it != triggerPrescaleMap_.end(); ++it) {
    STDOUT("\t" << it->second << "\t\"" << it->first << "\"");
  }
}

bool baseClass::triggerFired(const char* name)
{
  auto it = triggerDecisionMap_.find(std::string(name));
  if (it == triggerDecisionMap_.end()) return false;
  return it->second;
}

int baseClass::triggerPrescale(const char* name)
{
  auto it = triggerPrescaleMap_.find(std::string(name));
  if (it == triggerPrescaleMap_.end()) return -999;
  return it->second;
}

void baseClass::fillTriggerVariable(const char* hlt_path, const char* variable_name)
{
  const int prescale = triggerPrescale(hlt_path);
  if (triggerFired(hlt_path)) fillVariableWithValue(variable_name, prescale);
  else                        fillVariableWithValue(variable_name, -prescale);
}

void baseClass::getHltMap(char* fileName)
{
  STDOUT("getHltMap: " << fileName);
  TFile* f = TFile::Open(fileName);
  TH1F* triggerNames = (TH1F*)f->Get("dijets/TriggerNames");

  if (!triggerNames) {
    STDOUT("TriggerNames histo not found");
    f->Close();
    return;
  }

  for (int ii = 1; ii <= triggerNames->GetNbinsX(); ++ii) {
    triggerMap_[triggerNames->GetXaxis()->GetBinLabel(ii)] = ii - 1;
  }

  f->Close();
}

