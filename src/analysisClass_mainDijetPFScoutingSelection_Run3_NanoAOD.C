#define analysisClass_cxx
#include "analysisClass.h"
#include "JECTool.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <vector>
#include <fastjet/ClusterSequence.hh>

#include <TH1F.h>
#include <TLeaf.h>
#include <TMath.h>
#include <TLorentzVector.h>
#include <TStyle.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderArray.h>
#include <TTreeReaderValue.h>
#include <TVector2.h>

using std::size_t;

namespace {

const std::string era = "2024H";
const std::string dataList = "data/cfg/data_jec_list.txt";
const std::string mcList = "data/cfg/mc_jec_list.txt";

const std::array<const char*, 30> kNanoInputBranchesData = {
    "run",
    "luminosityBlock",
    "event",
    "nScoutingPrimaryVertex",
    "ScoutingRho_fixedGridRhoFastjetAll",
    "ScoutingMET_pt",
    "ScoutingMET_phi",
    "nScoutingPFJet",
    "ScoutingPFJet_pt",
    "ScoutingPFJet_eta",
    "ScoutingPFJet_phi",
    "ScoutingPFJet_m",
    "ScoutingPFJet_jetArea",
    "ScoutingPFJet_chargedHadronEnergy",
    "ScoutingPFJet_neutralHadronEnergy",
    "ScoutingPFJet_photonEnergy",
    "ScoutingPFJet_muonEnergy",
    "ScoutingPFJet_electronEnergy",
    "ScoutingPFJet_HFHadronEnergy",
    "ScoutingPFJet_HFEMEnergy",
    "ScoutingPFJet_HOEnergy",
    "ScoutingPFJet_chargedHadronMultiplicity",
    "ScoutingPFJet_neutralHadronMultiplicity",
    "ScoutingPFJet_photonMultiplicity",
    "ScoutingPFJet_HFHadronMultiplicity",
    "ScoutingPFJet_muonMultiplicity",
    "ScoutingPFJet_electronMultiplicity",
    "ScoutingPFJet_HFEMMultiplicity",
    "DST_PFScouting_JetHT",
    "DST_PFScouting_SingleMuon"};

const std::array<const char*, 24> kNanoInputBranchesMC = {
    "run",
    "luminosityBlock",
    "event",
    "PV_npvs",
    "Rho_fixedGridRhoFastjetAll",
    "PFMET_pt",
    "PFMET_phi",
    "nJet",
    "Jet_pt",
    "Jet_eta",
    "Jet_phi",
    "Jet_mass",
    "Jet_area",
    "Jet_chHEF",
    "Jet_neHEF",
    "Jet_neEmEF",
    "Jet_chEmEF",
    "Jet_muEF",
    "Jet_chMultiplicity",
    "Jet_neMultiplicity",
    "Jet_rawFactor",
    "Jet_hfEmEF",
    "DST_PFScouting_JetHT",
    "DST_PFScouting_SingleMuon"};

const std::array<const char*, 24> kNanoInputBranchesMonitoring = {
    "run",
    "luminosityBlock",
    "event",
    "PV_npvs",
    "Rho_fixedGridRhoFastjetAll",
    "PFMET_pt",
    "PFMET_phi",
    "nJet",
    "Jet_pt",
    "Jet_eta",
    "Jet_phi",
    "Jet_mass",
    "Jet_area",
    "Jet_chHEF",
    "Jet_neHEF",
    "Jet_neEmEF",
    "Jet_chEmEF",
    "Jet_muEF",
    "Jet_chMultiplicity",
    "Jet_neMultiplicity",
    "Jet_rawFactor",
    "Jet_hfEmEF",
    "DST_PFScouting_JetHT",
    "DST_PFScouting_SingleMuon"};

const std::array<const char*, 4> kRequiredL1DecisionBranches = {
    "L1_HTT280er",
    "L1_SingleJet180",
    "L1_DoubleJet30er2p5_Mass_Min250_dEta_Max1p5",
    "L1_ETT2000"};

const std::array<const char*, 2> kVetoL1DecisionBranches = {
    "L1_HTT200er",
    "L1_HTT255er"};

const std::array<const char*, 11> kScoutingMuonInputBranches = {
    "nScoutingMuonVtx",
    "ScoutingMuonVtx_pt",
    "ScoutingMuonVtx_eta",
    "ScoutingMuonVtx_phi",
    "ScoutingMuonVtx_trk_dxy",
    "ScoutingMuonVtx_trk_dz",
    "ScoutingMuonVtx_normchi2",
    "ScoutingMuonVtx_nValidRecoMuonHits",
    "ScoutingMuonVtx_nRecoMuonMatchedStations",
    "ScoutingMuonVtx_nValidPixelHits",
    "ScoutingMuonVtx_nTrackerLayersWithMeasurement"};

const char* kTriggerJetHTBranch = "DST_PFScouting_JetHT";
const char* kTriggerSingleMuonBranch = "DST_PFScouting_SingleMuon";

constexpr size_t kNOptionalNanoBranchesData = 2;
constexpr size_t kNRequiredNanoBranchesData = kNanoInputBranchesData.size() - kNOptionalNanoBranchesData;
static_assert(kNanoInputBranchesData.size() >= kNOptionalNanoBranchesData,
              "kNanoInputBranchesData must include optional trailing branches.");

constexpr size_t kNOptionalNanoBranchesMC = 4;
constexpr size_t kNRequiredNanoBranchesMC = kNanoInputBranchesMC.size() - kNOptionalNanoBranchesMC;
static_assert(kNanoInputBranchesMC.size() >= kNOptionalNanoBranchesMC,
              "kNanoInputBranchesMC must include optional trailing branches.");

constexpr size_t kNOptionalNanoBranchesMonitoring = 4;
constexpr size_t kNRequiredNanoBranchesMonitoring =
    kNanoInputBranchesMonitoring.size() - kNOptionalNanoBranchesMonitoring;
static_assert(kNanoInputBranchesMonitoring.size() >= kNOptionalNanoBranchesMonitoring,
              "kNanoInputBranchesMonitoring must include optional trailing branches.");

bool hasBranch(TTree* tree, const char* bname) {
  return tree && bname && tree->GetBranch(bname);
}

double safeFrac(double num, double den) {
  return (den > 0.0) ? (num / den) : 0.0;
}

double deltaRDistance(double eta1, double phi1, double eta2, double phi2) {
  const double deta = eta1 - eta2;
  const double dphi = std::abs(TVector2::Phi_mpi_pi(phi1 - phi2));
  return std::sqrt(deta * deta + dphi * dphi);
}

bool passesScoutingMuonID(double pt,
                          double eta,
                          double trkDxy,
                          double trkDz,
                          double normChi2,
                          double nValidRecoMuonHits,
                          double nRecoMuonMatchedStations,
                          double nValidPixelHits,
                          double nTrackerLayersWithMeasurement) {
  if (pt <= 30.0) return false;
  if (std::abs(eta) >= 0.8) return false;
  if (std::abs(trkDxy) >= 0.2) return false;
  if (std::abs(trkDz) >= 0.5) return false;
  if (normChi2 >= 3.0) return false;
  if (nValidRecoMuonHits <= 0.0) return false;
  if (nRecoMuonMatchedStations <= 1.0) return false;
  if (nValidPixelHits <= 0.0) return false;
  if (nTrackerLayersWithMeasurement <= 5.0) return false;
  return true;
}

std::string formatWithCommas(Long64_t value) {
  const bool negative = value < 0;
  const unsigned long long absValue =
      negative ? (static_cast<unsigned long long>(-(value + 1)) + 1ULL)
               : static_cast<unsigned long long>(value);
  std::string out = std::to_string(absValue);
  for (int i = static_cast<int>(out.size()) - 3; i > 0; i -= 3) {
    out.insert(static_cast<size_t>(i), ".");
  }
  if (negative) out.insert(out.begin(), '-');
  return out;
}

fastjet::JetAlgorithm getFastJetAlgorithm(const std::string& jetAlgo) {
  if (jetAlgo == "Kt" || jetAlgo == "kt") {
    return fastjet::kt_algorithm;
  }
  if (jetAlgo == "Cambridge" || jetAlgo == "cambridge" || jetAlgo == "CambridgeAachen" ||
      jetAlgo == "cambridge_algorithm" || jetAlgo == "CA") {
    return fastjet::cambridge_algorithm;
  }
  return fastjet::antikt_algorithm;
}

}  // namespace

analysisClass::analysisClass(std::string* inputList,
                             std::string* cutFile,
                             std::string* treeName,
                             std::string* outputFileName,
                             std::string* cutEfficFile)
    : baseClass(inputList, cutFile, treeName, outputFileName, cutEfficFile) {
  std::cout << "[analysisClass] Constructor (ctor) begin\n";

  if (int(getPreCutValue1("useJECs")) == 1) {
    std::cout << "[analysisClass] Reapplying JECs via JECTool...\n";

    auto Rmc = JECTool::resolveList(mcList, era);
    auto Rdata = JECTool::resolveList(dataList, era);

    auto mcCorr = JECTool::makeCorrector(Rmc, /*forData=*/false);
    auto dataCorr = JECTool::makeCorrector(Rdata, /*forData=*/true);
    auto jecUnc = JECTool::makeUncertainty(Rmc);

    JetCorrector = mcCorr.release();
    JetCorrector_data = dataCorr.release();
    unc = jecUnc.release();

    if (!JetCorrector || !JetCorrector_data) {
      std::cerr << "[analysisClass] ERROR: JEC corrector initialization failed. Check list files.\n";
    } else if (!unc) {
      std::cerr << "[analysisClass] WARNING: no JEC uncertainty file configured. "
                   "JEC uncertainties and JEC shifts will be disabled.\n";
    }
  }

  std::cout << "[analysisClass] Constructor (ctor) end\n";
}

analysisClass::~analysisClass() { std::cout << "[analysisClass] Destructor (dtor)\n"; }

void analysisClass::Loop() {
  std::cout << "[analysisClass] Loop begin\n";
  if (fChain == nullptr) return;

  const bool hasGenWeight = hasBranch(fChain, "genWeight");
  const bool hasScoutingRecoJets = hasBranch(fChain, "ScoutingPFJet_pt");
  const bool hasStandardRecoJets = hasBranch(fChain, "Jet_pt");

  enum class InputSampleMode {
    kMC,
    kScoutingData,
    kMonitoringData
  };
  InputSampleMode inputMode = InputSampleMode::kScoutingData;

  if (hasGenWeight) {
    if (!hasStandardRecoJets) {
      std::cerr << "[analysisClass] ERROR: MC mode selected (genWeight present) but missing 'Jet_pt'.\n";
      return;
    }
    inputMode = InputSampleMode::kMC;
  } else if (hasScoutingRecoJets) {
    inputMode = InputSampleMode::kScoutingData;
  } else if (hasStandardRecoJets) {
    inputMode = InputSampleMode::kMonitoringData;
  } else {
    std::cerr << "[analysisClass] ERROR: Data mode selected but neither 'ScoutingPFJet_pt' "
              << "nor 'Jet_pt' is available.\n";
    return;
  }

  const bool useStandardMC = (inputMode == InputSampleMode::kMC);
  const bool useMonitoringData = (inputMode == InputSampleMode::kMonitoringData);
  const bool useStandardLikeJets = useStandardMC || useMonitoringData;
  const bool isDataSample = !hasGenWeight;
  const bool applyL1TriggerVeto = isDataSample && (getPreCutValue1("applyL1TriggerVeto") > 0.5);
  const bool applyMuonSelectionCleaning =
      (inputMode == InputSampleMode::kScoutingData) && (getPreCutValue1("applyMuonSelectionCleaning") > 0.5);

  std::cout << "[analysisClass] Input mode: "
            << (useStandardMC ? "MC"
                              : (useMonitoringData ? "MonitoringData" : "ScoutingData"))
            << "\n";
  std::cout << "[analysisClass] Strategy: "
            << (useStandardMC ? "MC standard branches + MC JEC list"
                              : (useMonitoringData
                                     ? "Monitoring standard branches + DATA JEC list"
                                     : "Scouting branches + DATA JEC list"))
            << "\n";

  std::vector<std::string> missingRequired;
  fChain->SetBranchStatus("*", 0);

  auto enableBranches = [&](const auto& branches, size_t nRequired) {
    for (size_t i = 0; i < branches.size(); ++i) {
      const char* bname = branches[i];
      if (hasBranch(fChain, bname)) {
        fChain->SetBranchStatus(bname, 1);
      } else if (i < nRequired) {
        missingRequired.emplace_back(bname);
      }
    }
  };

  if (useStandardMC) {
    enableBranches(kNanoInputBranchesMC, kNRequiredNanoBranchesMC);
  } else if (useMonitoringData) {
    enableBranches(kNanoInputBranchesMonitoring, kNRequiredNanoBranchesMonitoring);
  } else {
    enableBranches(kNanoInputBranchesData, kNRequiredNanoBranchesData);
  }
  if (applyL1TriggerVeto) {
    enableBranches(kRequiredL1DecisionBranches, kRequiredL1DecisionBranches.size());
    enableBranches(kVetoL1DecisionBranches, kVetoL1DecisionBranches.size());
  }
  if (applyMuonSelectionCleaning) {
    enableBranches(kScoutingMuonInputBranches, kScoutingMuonInputBranches.size());
  }

  if (hasGenWeight) fChain->SetBranchStatus("genWeight", 1);

  const bool hasTrigJetHT = hasBranch(fChain, kTriggerJetHTBranch);
  const bool hasTrigSingleMuon = hasBranch(fChain, kTriggerSingleMuonBranch);
  std::cout << "[analysisClass] INFO: sumEt = scalar sum of jet pT; metSig = MET/sqrt(sumEt).\n";

  if (!missingRequired.empty()) {
    std::cerr << "[analysisClass] ERROR: missing required NanoAOD branches:\n";
    for (const std::string& b : missingRequired) std::cerr << "  - " << b << "\n";
    return;
  }

  // fChain->SetCacheSize(128 * 1024 * 1024);

  TTreeReader reader(fChain);

  TTreeReaderValue<UInt_t> run(reader, "run");
  TTreeReaderValue<UInt_t> luminosityBlock(reader, "luminosityBlock");
  TTreeReaderValue<ULong64_t> event(reader, "event");

  const char* rhoBranch = useStandardLikeJets ? "Rho_fixedGridRhoFastjetAll" : "ScoutingRho_fixedGridRhoFastjetAll";
  const char* metPtBranch = useStandardLikeJets ? "PFMET_pt" : "ScoutingMET_pt";
  const char* metPhiBranch = useStandardLikeJets ? "PFMET_phi" : "ScoutingMET_phi";
  const char* nJetBranch = useStandardLikeJets ? "nJet" : "nScoutingPFJet";
  const char* jetPtBranch = useStandardLikeJets ? "Jet_pt" : "ScoutingPFJet_pt";
  const char* jetEtaBranch = useStandardLikeJets ? "Jet_eta" : "ScoutingPFJet_eta";
  const char* jetPhiBranch = useStandardLikeJets ? "Jet_phi" : "ScoutingPFJet_phi";
  const char* jetMassBranch = useStandardLikeJets ? "Jet_mass" : "ScoutingPFJet_m";
  const char* jetAreaBranch = useStandardLikeJets ? "Jet_area" : "ScoutingPFJet_jetArea";

  TTreeReaderValue<Float_t> rhoValReader(reader, rhoBranch);
  TTreeReaderValue<Float_t> metPtReader(reader, metPtBranch);
  TTreeReaderValue<Float_t> metPhiReader(reader, metPhiBranch);
  TTreeReaderValue<Int_t> nJetReader(reader, nJetBranch);

  TTreeReaderArray<Float_t> jetPt(reader, jetPtBranch);
  TTreeReaderArray<Float_t> jetEta(reader, jetEtaBranch);
  TTreeReaderArray<Float_t> jetPhi(reader, jetPhiBranch);
  TTreeReaderArray<Float_t> jetMass(reader, jetMassBranch);
  TTreeReaderArray<Float_t> jetArea(reader, jetAreaBranch);

  std::unique_ptr<TTreeReaderValue<Int_t>> nVtxDataReader;
  std::unique_ptr<TTreeReaderValue<UChar_t>> nVtxMCReader;

  std::unique_ptr<TTreeReaderArray<Float_t>> jetChHadEData;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetNeHadEData;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetPhoEData;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetMuEData;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetElEData;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetHFHadEData;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetHFEMEData;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetHOEData;

  std::unique_ptr<TTreeReaderArray<Int_t>> chHadMultData;
  std::unique_ptr<TTreeReaderArray<Int_t>> neHadMultData;
  std::unique_ptr<TTreeReaderArray<Int_t>> phoMultData;
  std::unique_ptr<TTreeReaderArray<Int_t>> hfHadMultData;
  std::unique_ptr<TTreeReaderArray<Int_t>> muMultData;
  std::unique_ptr<TTreeReaderArray<Int_t>> elMultData;
  std::unique_ptr<TTreeReaderArray<Int_t>> hfEmMultData;

  std::unique_ptr<TTreeReaderArray<Float_t>> jetChHEFMC;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetNeHEFMC;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetNeEmEFMC;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetChEmEFMC;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetMuEFMC;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetHFEmEFMC;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetRawFactorMC;
  std::unique_ptr<TTreeReaderArray<UChar_t>> chHadMultMC;
  std::unique_ptr<TTreeReaderArray<UChar_t>> neHadMultMC;

  if (useStandardLikeJets) {
    nVtxMCReader = std::make_unique<TTreeReaderValue<UChar_t>>(reader, "PV_npvs");
    jetChHEFMC = std::make_unique<TTreeReaderArray<Float_t>>(reader, "Jet_chHEF");
    jetNeHEFMC = std::make_unique<TTreeReaderArray<Float_t>>(reader, "Jet_neHEF");
    jetNeEmEFMC = std::make_unique<TTreeReaderArray<Float_t>>(reader, "Jet_neEmEF");
    jetChEmEFMC = std::make_unique<TTreeReaderArray<Float_t>>(reader, "Jet_chEmEF");
    jetMuEFMC = std::make_unique<TTreeReaderArray<Float_t>>(reader, "Jet_muEF");
    chHadMultMC = std::make_unique<TTreeReaderArray<UChar_t>>(reader, "Jet_chMultiplicity");
    neHadMultMC = std::make_unique<TTreeReaderArray<UChar_t>>(reader, "Jet_neMultiplicity");

    if (hasBranch(fChain, "Jet_rawFactor")) {
      jetRawFactorMC = std::make_unique<TTreeReaderArray<Float_t>>(reader, "Jet_rawFactor");
    }
    if (hasBranch(fChain, "Jet_hfEmEF")) {
      jetHFEmEFMC = std::make_unique<TTreeReaderArray<Float_t>>(reader, "Jet_hfEmEF");
    }
  } else {
    nVtxDataReader = std::make_unique<TTreeReaderValue<Int_t>>(reader, "nScoutingPrimaryVertex");

    jetChHadEData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJet_chargedHadronEnergy");
    jetNeHadEData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJet_neutralHadronEnergy");
    jetPhoEData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJet_photonEnergy");
    jetMuEData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJet_muonEnergy");
    jetElEData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJet_electronEnergy");
    jetHFHadEData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJet_HFHadronEnergy");
    jetHFEMEData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJet_HFEMEnergy");
    jetHOEData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJet_HOEnergy");

    chHadMultData = std::make_unique<TTreeReaderArray<Int_t>>(reader, "ScoutingPFJet_chargedHadronMultiplicity");
    neHadMultData = std::make_unique<TTreeReaderArray<Int_t>>(reader, "ScoutingPFJet_neutralHadronMultiplicity");
    phoMultData = std::make_unique<TTreeReaderArray<Int_t>>(reader, "ScoutingPFJet_photonMultiplicity");
    hfHadMultData = std::make_unique<TTreeReaderArray<Int_t>>(reader, "ScoutingPFJet_HFHadronMultiplicity");
    muMultData = std::make_unique<TTreeReaderArray<Int_t>>(reader, "ScoutingPFJet_muonMultiplicity");
    elMultData = std::make_unique<TTreeReaderArray<Int_t>>(reader, "ScoutingPFJet_electronMultiplicity");
    hfEmMultData = std::make_unique<TTreeReaderArray<Int_t>>(reader, "ScoutingPFJet_HFEMMultiplicity");
  }

  std::unique_ptr<TTreeReaderValue<Bool_t>> trigJetHT;
  std::unique_ptr<TTreeReaderValue<Bool_t>> trigSingleMuon;
  if (hasTrigJetHT) {
    trigJetHT = std::make_unique<TTreeReaderValue<Bool_t>>(reader, kTriggerJetHTBranch);
  }
  if (hasTrigSingleMuon) {
    trigSingleMuon = std::make_unique<TTreeReaderValue<Bool_t>>(reader, kTriggerSingleMuonBranch);
  }

  std::vector<std::unique_ptr<TTreeReaderValue<Bool_t>>> requiredL1Readers;
  std::vector<std::unique_ptr<TTreeReaderValue<Bool_t>>> vetoL1Readers;
  auto buildL1Readers = [&](const auto& branchNames, auto& readers) {
    readers.reserve(branchNames.size());
    for (const char* branchName : branchNames) {
      readers.push_back(std::make_unique<TTreeReaderValue<Bool_t>>(reader, branchName));
    }
  };
  if (applyL1TriggerVeto) {
    buildL1Readers(kRequiredL1DecisionBranches, requiredL1Readers);
    buildL1Readers(kVetoL1DecisionBranches, vetoL1Readers);
  }

  static const int nMassBins = 103;
  static const double massBoundaries[nMassBins + 1] = {
      1, 3, 6, 10, 16, 23, 31, 40, 50, 61, 74, 88, 103, 119, 137, 156, 176, 197, 220, 244, 270, 296, 325,
      354, 386, 419, 453, 489, 526, 565, 606, 649, 693, 740, 788, 838, 890, 944, 1000, 1058, 1118, 1181, 1246, 1313, 1383, 1455, 1530, 1607, 1687,
      1770, 1856, 1945, 2037, 2132, 2231, 2332, 2438, 2546, 2659, 2775, 2895, 3019, 3147, 3279, 3416, 3558, 3704, 3854, 4010, 4171, 4337, 4509,
      4686, 4869, 5058, 5253, 5455, 5663, 5877, 6099, 6328, 6564, 6808, 7060, 7320, 7589, 7866, 8152, 8447, 8752, 9067, 9391, 9726, 10072, 10430,
      10798, 11179, 11571, 11977, 12395, 12827, 13272, 13732, 14000};

  const char* HLTname[3] = {"noTrig", "PFScouting_JetHT", "PFScouting_SingleMuon"};

  TH1F* h_mjj_HLTpass[3];
  TH1F* h_mjj_noTrig_1GeVbin = new TH1F("h_mjj_noTrig_1GeVbin", "", 14000, 0, 14000);
  for (int i = 0; i < 3; ++i) {
    h_mjj_HLTpass[i] = new TH1F(Form("h_mjj_HLTpass_%s", HLTname[i]), "", nMassBins, massBoundaries);
  }

  const double jetFidRegion = getPreCutValue1("jetFidRegion");
  const int tightJetIDFlag = int(getPreCutValue1("tightJetID"));
  const double ptCut = getPreCutValue1("ptCut");
  const double pt0Cut = getPreCutValue1("pt0Cut");
  const double pt1Cut = getPreCutValue1("pt1Cut");
  const double wideJetDeltaR = getPreCutValue1("DeltaR");
  const bool useFastJet = (int(getPreCutValue1("useFastJet")) == 1);
  std::string jetAlgo = "AntiKt";
  if (getPreCutString1("jetAlgo") != "ERROR") {
    jetAlgo = getPreCutString1("jetAlgo");
  } else {
    std::cerr << "[analysisClass] WARNING: jetAlgo not set in cut file. Defaulting to AntiKt.\n";
  }
  const bool reapplyJEC = (int(getPreCutValue1("useJECs")) == 1);
  const bool useRawOnly = (int(getPreCutValue1("noJECs")) == 1);
  const bool shiftJECs = (int(getPreCutValue1("shiftJECs")) == 1);
  const double shiftSign = getPreCutValue2("shiftJECs");
  std::unique_ptr<fastjet::JetDefinition> fjJetDefinition;
  if (useFastJet) {
    fjJetDefinition = std::make_unique<fastjet::JetDefinition>(getFastJetAlgorithm(jetAlgo), wideJetDeltaR);
    std::cout << "[analysisClass] Wide-jet mode: FastJet (" << jetAlgo << ", R=" << wideJetDeltaR << ")\n";
  } else {
    std::cout << "[analysisClass] Wide-jet mode: legacy pairwise clustering\n";
  }

  bool warnedMissingJEC = false;
  bool warnedMissingJECUnc = false;
  bool warnedMismatchedNJet = false;
  long jecResidualRunCached = -1;

  std::vector<double> rawPt;
  std::vector<double> jecFacInput;
  std::vector<double> jecUncInput;
  std::vector<double> chf;
  std::vector<double> nhf;
  std::vector<double> phf;
  std::vector<double> muf;
  std::vector<double> elf;
  std::vector<double> cemf;
  std::vector<double> nemf;
  std::vector<double> jecFactor;
  std::vector<double> jecUncRel;
  std::vector<unsigned> sortedIdx;
  std::vector<int> jetID;

  rawPt.reserve(512);
  jecFacInput.reserve(512);
  jecUncInput.reserve(512);
  chf.reserve(512);
  nhf.reserve(512);
  phf.reserve(512);
  muf.reserve(512);
  elf.reserve(512);
  cemf.reserve(512);
  nemf.reserve(512);
  jecFactor.reserve(512);
  jecUncRel.reserve(512);
  sortedIdx.reserve(512);
  jetID.reserve(512);

  const Long64_t nentries = fChain->GetEntriesFast();
  const Long64_t maxEventsCfg = static_cast<Long64_t>(getPreCutValue1("maxEvents"));
  const Long64_t nentriesToProcess =
      (maxEventsCfg > 0 && maxEventsCfg < nentries) ? maxEventsCfg : nentries;
  std::cout << "[analysisClass] entries = " << formatWithCommas(nentries)
            << " || processing = " << formatWithCommas(nentriesToProcess) << "\n";

  for (Long64_t jentry = 0; jentry < nentriesToProcess && reader.Next(); ++jentry) {
    if (jentry < 10 || jentry % 100000 == 0) {
      std::cout << "[analysisClass] jentry " << formatWithCommas(jentry) << "\n";
    }

    const bool isDataEvt = isDataSample;
    const int runNoEvt = static_cast<int>(*run);
    const int lumiEvt = static_cast<int>(*luminosityBlock);
    const ULong64_t evtNoEvt = *event;
    const int nvtxEvt = useStandardLikeJets ? static_cast<int>(**nVtxMCReader)
                                            : static_cast<int>(**nVtxDataReader);
    const float rhoVal = *rhoValReader;
    const float metVal = *metPtReader;
    const float metPhiVal = *metPhiReader;
    const int nJetEvt = *nJetReader;

    const size_t nJets = jetPt.GetSize();
    if (!warnedMismatchedNJet && nJets != static_cast<size_t>(nJetEvt)) {
      std::cerr << "[analysisClass] WARNING: " << nJetBranch
                << " != jet array size in one event; proceeding with array size.\n";
      warnedMismatchedNJet = true;
    }

    rawPt.assign(nJets, 0.0);
    jecFacInput.assign(nJets, 1.0);
    jecUncInput.assign(nJets, 0.0);
    chf.assign(nJets, 0.0);
    nhf.assign(nJets, 0.0);
    phf.assign(nJets, 0.0);
    muf.assign(nJets, 0.0);
    elf.assign(nJets, 0.0);
    cemf.assign(nJets, 0.0);
    nemf.assign(nJets, 0.0);
    jecFactor.assign(nJets, 1.0);
    jecUncRel.assign(nJets, 0.0);
    sortedIdx.resize(nJets);
    std::iota(sortedIdx.begin(), sortedIdx.end(), 0);
    jetID.assign(nJets, 0);

    const bool doReapplyJEC = reapplyJEC && JetCorrector && JetCorrector_data;
    if (reapplyJEC && !doReapplyJEC && !warnedMissingJEC) {
      std::cerr << "[analysisClass] WARNING: useJECs=1 but JEC correctors are not fully initialized. Falling back to input jet kinematics.\n";
      warnedMissingJEC = true;
    }
    if (doReapplyJEC && !unc && !warnedMissingJECUnc) {
      std::cerr << "[analysisClass] WARNING: no JEC uncertainty payload is available. "
                   "Relative JEC uncertainties and shiftJECs will be disabled.\n";
      warnedMissingJECUnc = true;
    }

    if (doReapplyJEC && isDataEvt && (jecResidualRunCached != static_cast<long>(runNoEvt))) {
      auto RdataRun = JECTool::resolveList(dataList, era, static_cast<long>(runNoEvt));
      auto dataCorrRun = JECTool::makeCorrector(RdataRun, /*forData=*/true);
      if (dataCorrRun) {
        delete JetCorrector_data;
        JetCorrector_data = dataCorrRun.release();
        jecResidualRunCached = static_cast<long>(runNoEvt);
      } else {
        std::cerr << "[analysisClass] WARNING: failed to build data JEC corrector for run "
                  << runNoEvt << " — continuing with previous one.\n";
      }
    }

    for (size_t i = 0; i < nJets; ++i) {
      const double pt = jetPt[i];
      const double eta = jetEta[i];
      const double phi = jetPhi[i];
      const double mass = jetMass[i];
      const double area = jetArea[i];

      double rawFactor = 0.0;
      if (useStandardLikeJets && jetRawFactorMC) {
        rawFactor = static_cast<double>((*jetRawFactorMC)[i]);
      }
      rawPt[i] = pt * (1.0 - rawFactor);
      if (useStandardLikeJets) {
        const double oneMinusRaw = (1.0 - rawFactor);
        if (std::fabs(oneMinusRaw) > 1e-8) jecFacInput[i] = 1.0 / oneMinusRaw;
      }

      int chMultVal = 0;
      int neMultVal = 0;
      int numConstVal = 0;
      int numChargedParticlesVal = 0;
      bool passID = false;

      if (useStandardLikeJets) {
        chf[i] = static_cast<double>((*jetChHEFMC)[i]);
        nhf[i] = static_cast<double>((*jetNeHEFMC)[i]);
        muf[i] = static_cast<double>((*jetMuEFMC)[i]);
        cemf[i] = static_cast<double>((*jetChEmEFMC)[i]);
        elf[i] = cemf[i];
        nemf[i] = static_cast<double>((*jetNeEmEFMC)[i]);

        const double hfEmF = jetHFEmEFMC ? static_cast<double>((*jetHFEmEFMC)[i]) : 0.0;
        phf[i] = std::max(0.0, nemf[i] - hfEmF);

        chMultVal = chHadMultMC ? static_cast<int>((*chHadMultMC)[i]) : 0;
        neMultVal = neHadMultMC ? static_cast<int>((*neHadMultMC)[i]) : 0;
        numConstVal = chMultVal + neMultVal;
        numChargedParticlesVal = chMultVal;

        passID = (std::abs(eta) < 2.6) &&
                 (numConstVal > 1) &&
                 (numChargedParticlesVal > 0) &&
                 (nemf[i] < 0.90) &&
                 (muf[i] < 0.80) &&
                 (nhf[i] < 0.99);
      } else {
        const double chHadE = (*jetChHadEData)[i];
        const double neHadE = (*jetNeHadEData)[i];
        const double phoE = (*jetPhoEData)[i];
        const double muE = (*jetMuEData)[i];
        const double elE = (*jetElEData)[i];
        const double hfHadE = (*jetHFHadEData)[i];
        const double hfEmE = (*jetHFEMEData)[i];
        const double hoE = (*jetHOEData)[i];

        double jetEnergyRaw = chHadE + neHadE + phoE + muE + elE + hfHadE + hfEmE;
        if (jetEnergyRaw <= 0.0) {
          const double nominalJec = (std::fabs(jecFacInput[i]) > 1e-8) ? jecFacInput[i] : 1.0;
          const double massRaw = mass / nominalJec;
          const double pRaw = rawPt[i] * std::cosh(eta);
          jetEnergyRaw = std::sqrt(std::max(0.0, pRaw * pRaw + massRaw * massRaw));
        }
        if (jetEnergyRaw <= 0.0) {
          const double eComp = chHadE + neHadE + phoE + muE + elE + hfHadE + hfEmE + hoE;
          jetEnergyRaw = (eComp > 0.0 ? eComp : 0.0);
        }

        chf[i] = safeFrac(chHadE, jetEnergyRaw);
        nhf[i] = safeFrac(neHadE + hfHadE, jetEnergyRaw);
        phf[i] = safeFrac(phoE, jetEnergyRaw);
        muf[i] = safeFrac(muE, jetEnergyRaw);
        elf[i] = safeFrac(elE, jetEnergyRaw);
        cemf[i] = safeFrac(elE, jetEnergyRaw);
        nemf[i] = safeFrac(phoE + hfEmE, jetEnergyRaw);

        chMultVal = static_cast<int>((*chHadMultData)[i]);
        neMultVal = static_cast<int>((*neHadMultData)[i]);
        const int phoMultVal = static_cast<int>((*phoMultData)[i]);
        const int muMultVal = static_cast<int>((*muMultData)[i]);
        const int elMultVal = static_cast<int>((*elMultData)[i]);
        const int hfHadMultVal = static_cast<int>((*hfHadMultData)[i]);
        numConstVal = chMultVal + neMultVal + muMultVal + elMultVal + phoMultVal;
        numChargedParticlesVal = chMultVal + hfHadMultVal;
        passID = (std::abs(eta) < 2.6) &&
                 (numConstVal > 1) &&
                 (numChargedParticlesVal > 0) &&
                 (nemf[i] < 0.90) &&
                 (muf[i] < 0.80) &&
                 (nhf[i] < 0.99);
      }

      jetID[i] = passID ? 1 : 0;

      if (doReapplyJEC) {
        JetCorrector->setRho(rhoVal);
        JetCorrector->setJetEta(eta);
        JetCorrector->setJetPhi(phi);
        JetCorrector->setJetA(area);
        JetCorrector->setJetPt(rawPt[i]);

        JetCorrector_data->setRho(rhoVal);
        JetCorrector_data->setJetEta(eta);
        JetCorrector_data->setJetPhi(phi);
        JetCorrector_data->setJetA(area);
        JetCorrector_data->setJetPt(rawPt[i]);

        const double corr = isDataEvt ? JetCorrector_data->getCorrection() : JetCorrector->getCorrection();

        jecFactor[i] = corr;
        if (unc) {
          unc->setJetEta(eta);
          unc->setJetPt(rawPt[i] * corr);
          jecUncRel[i] = unc->getUncertainty(true);
        } else {
          jecUncRel[i] = 0.0;
        }
      } else if (useRawOnly) {
        jecFactor[i] = 1.0;
        jecUncRel[i] = 0.0;
      } else {
        jecFactor[i] = jecFacInput[i];
        jecUncRel[i] = jecUncInput[i];
      }
    }

    auto getCorrPt = [&](size_t i) -> double {
      if (doReapplyJEC) return rawPt[i] * jecFactor[i];
      if (useRawOnly) return rawPt[i];
      return jetPt[i];
    };

    auto getMassForMode = [&](size_t i) -> double {
      const double nominalJec = (std::fabs(jecFacInput[i]) > 0.0) ? jecFacInput[i] : 1.0;
      if (doReapplyJEC) return jetMass[i] * (jecFactor[i] / nominalJec);
      if (useRawOnly) return jetMass[i] / nominalJec;
      return jetMass[i];
    };

    auto shiftFactorForJet = [&](size_t i) -> double {
      return shiftJECs ? (1.0 + (shiftSign * jecUncRel[i])) : 1.0;
    };

    std::vector<TLorentzVector> recoJetP4(nJets);
    std::vector<TLorentzVector> recoJetP4Shift(nJets);
    for (size_t i = 0; i < nJets; ++i) {
      const double pTj = getCorrPt(i);
      const double massj = getMassForMode(i);
      const double shiftFactor = shiftFactorForJet(i);
      recoJetP4[i].SetPtEtaPhiM(pTj, jetEta[i], jetPhi[i], massj);
      recoJetP4Shift[i].SetPtEtaPhiM(pTj * shiftFactor, jetEta[i], jetPhi[i], massj * shiftFactor);
    }

    if (applyMuonSelectionCleaning) {
      auto leafValue = [](TLeaf* leaf, Int_t idx = 0) -> double {
        return leaf ? static_cast<double>(leaf->GetValue(idx)) : 0.0;
      };

      TLeaf* nMuonLeaf = fChain->GetLeaf("nScoutingMuonVtx");
      TLeaf* muPtLeaf = fChain->GetLeaf("ScoutingMuonVtx_pt");
      TLeaf* muEtaLeaf = fChain->GetLeaf("ScoutingMuonVtx_eta");
      TLeaf* muPhiLeaf = fChain->GetLeaf("ScoutingMuonVtx_phi");
      TLeaf* muDxyLeaf = fChain->GetLeaf("ScoutingMuonVtx_trk_dxy");
      TLeaf* muDzLeaf = fChain->GetLeaf("ScoutingMuonVtx_trk_dz");
      TLeaf* muNormChi2Leaf = fChain->GetLeaf("ScoutingMuonVtx_normchi2");
      TLeaf* muHitsLeaf = fChain->GetLeaf("ScoutingMuonVtx_nValidRecoMuonHits");
      TLeaf* muStationsLeaf = fChain->GetLeaf("ScoutingMuonVtx_nRecoMuonMatchedStations");
      TLeaf* muPixelHitsLeaf = fChain->GetLeaf("ScoutingMuonVtx_nValidPixelHits");
      TLeaf* muLayersLeaf = fChain->GetLeaf("ScoutingMuonVtx_nTrackerLayersWithMeasurement");

      struct MuonInfo {
        double pt;
        double eta;
        double phi;
      };

      const int nMuons = static_cast<int>(leafValue(nMuonLeaf));
      std::vector<MuonInfo> allMuons;
      allMuons.reserve(static_cast<size_t>(std::max(0, nMuons)));
      int nGoodMuons = 0;

      for (int i = 0; i < nMuons; ++i) {
        const double muPt = leafValue(muPtLeaf, i);
        const double muEta = leafValue(muEtaLeaf, i);
        const double muPhi = leafValue(muPhiLeaf, i);
        const double muDxy = leafValue(muDxyLeaf, i);
        const double muDz = leafValue(muDzLeaf, i);
        const double muNormChi2 = leafValue(muNormChi2Leaf, i);
        const double muHits = leafValue(muHitsLeaf, i);
        const double muStations = leafValue(muStationsLeaf, i);
        const double muPixelHits = leafValue(muPixelHitsLeaf, i);
        const double muLayers = leafValue(muLayersLeaf, i);

        allMuons.push_back({muPt, muEta, muPhi});
        if (passesScoutingMuonID(muPt, muEta, muDxy, muDz, muNormChi2, muHits, muStations, muPixelHits,
                                 muLayers)) {
          ++nGoodMuons;
        }
      }

      if (nGoodMuons < 1) continue;

      std::vector<size_t> muCleanJetIdx;
      muCleanJetIdx.reserve(nJets);
      for (size_t j = 0; j < nJets; ++j) {
        if (std::abs(jetEta[j]) > 5.0) continue;
        if (jetID[j] != tightJetIDFlag) continue;
        if (getCorrPt(j) <= ptCut) continue;
        muCleanJetIdx.push_back(j);
      }

      for (const auto j : muCleanJetIdx) {
        TLorentzVector cleanedJetP4;
        cleanedJetP4.SetPtEtaPhiM(jetPt[j], jetEta[j], jetPhi[j], jetMass[j]);
        TLorentzVector cleanedJetP4Shift = recoJetP4Shift[j];

        bool hadOverlap = false;
        for (const auto& mu : allMuons) {
          if (deltaRDistance(jetEta[j], jetPhi[j], mu.eta, mu.phi) >= 0.4) continue;
          TLorentzVector muP4;
          muP4.SetPtEtaPhiM(mu.pt, mu.eta, mu.phi, 0.105);
          cleanedJetP4 -= muP4;
          cleanedJetP4Shift -= muP4;
          hadOverlap = true;
        }

        if (hadOverlap && cleanedJetP4.Pt() < 1.0) {
          recoJetP4[j].SetPxPyPzE(0.0, 0.0, 0.0, 0.0);
          recoJetP4Shift[j].SetPxPyPzE(0.0, 0.0, 0.0, 0.0);
          continue;
        }

        recoJetP4[j] = cleanedJetP4;
        recoJetP4Shift[j] = cleanedJetP4Shift;
      }
    }

    std::sort(sortedIdx.begin(), sortedIdx.end(),
              [&](unsigned a, unsigned b) { return recoJetP4[a].Pt() > recoJetP4[b].Pt(); });

    int NAK4PF = 0;
    double HTAK4PF = 0.0;
    double htAK4RawForUnclustered = 0.0;
    for (size_t k = 0; k < nJets; ++k) {
      const auto j = sortedIdx[k];
      const double pTj = recoJetP4[j].Pt();
      if (pTj > ptCut) htAK4RawForUnclustered += rawPt[j];
      if (std::fabs(recoJetP4[j].Eta()) >= jetFidRegion) continue;
      if (jetID[j] != tightJetIDFlag) continue;
      if (pTj <= ptCut) continue;
      ++NAK4PF;
      HTAK4PF += pTj;
    }

    TLorentzVector wj1, wj2, wj1_shift, wj2_shift;
    TLorentzVector AK4j1, AK4j2;

    if (nJets >= 2U) {
      const auto j0 = sortedIdx[0];
      const auto j1 = sortedIdx[1];

      if (recoJetP4[j0].Pt() > pt0Cut && recoJetP4[j1].Pt() > pt1Cut) {
        AK4j1 = recoJetP4[j0];
        AK4j2 = recoJetP4[j1];

        if (useFastJet) {
          std::vector<fastjet::PseudoJet> fjInputs;
          std::vector<fastjet::PseudoJet> fjInputsShift;
          fjInputs.reserve(nJets);
          fjInputsShift.reserve(nJets);

          for (size_t k = 0; k < nJets; ++k) {
            const auto j = sortedIdx[k];
            const double pTj = recoJetP4[j].Pt();
            const double minPt = (k == 0) ? pt0Cut : ((k == 1) ? pt1Cut : ptCut);
            if (std::fabs(recoJetP4[j].Eta()) >= jetFidRegion) continue;
            if (jetID[j] != tightJetIDFlag) continue;
            if (pTj <= minPt) continue;

            const TLorentzVector& jet = recoJetP4[j];
            const TLorentzVector& jetShift = recoJetP4Shift[j];

            fjInputs.emplace_back(jet.Px(), jet.Py(), jet.Pz(), jet.E());
            fjInputsShift.emplace_back(jetShift.Px(), jetShift.Py(), jetShift.Pz(), jetShift.E());
          }

          if (fjInputs.size() > 1U) {
            fastjet::ClusterSequence clusterSeq(fjInputs, *fjJetDefinition);
            fastjet::ClusterSequence clusterSeqShift(fjInputsShift, *fjJetDefinition);
            const auto wideJets = fastjet::sorted_by_pt(clusterSeq.inclusive_jets(0.0));
            const auto wideJetsShift = fastjet::sorted_by_pt(clusterSeqShift.inclusive_jets(0.0));
            if (wideJets.size() > 1U && wideJetsShift.size() > 1U) {
              wj1.SetPxPyPzE(wideJets[0].px(), wideJets[0].py(), wideJets[0].pz(), wideJets[0].e());
              wj2.SetPxPyPzE(wideJets[1].px(), wideJets[1].py(), wideJets[1].pz(), wideJets[1].e());
              wj1_shift.SetPxPyPzE(wideJetsShift[0].px(), wideJetsShift[0].py(), wideJetsShift[0].pz(),
                                   wideJetsShift[0].e());
              wj2_shift.SetPxPyPzE(wideJetsShift[1].px(), wideJetsShift[1].py(), wideJetsShift[1].pz(),
                                   wideJetsShift[1].e());
            }
          }
        } else {

          TLorentzVector wj1_tmp, wj2_tmp, wj1s_tmp, wj2s_tmp;

          for (size_t k = 0; k < nJets; ++k) {
            const auto j = sortedIdx[k];
            const double pTj = recoJetP4[j].Pt();
            if (std::fabs(recoJetP4[j].Eta()) >= jetFidRegion) continue;
            if (jetID[j] != tightJetIDFlag) continue;
            if (pTj <= ptCut) continue;
            const TLorentzVector& cj = recoJetP4[j];
            const TLorentzVector& cjs = recoJetP4Shift[j];

            const double dR1 = cj.DeltaR(AK4j1);
            const double dR2 = cj.DeltaR(AK4j2);

            if (dR1 < dR2 && dR1 < wideJetDeltaR) {
              wj1_tmp += cj;
              wj1s_tmp += cjs;
            } else if (dR2 < wideJetDeltaR) {
              wj2_tmp += cj;
              wj2s_tmp += cjs;
            }
          }

          if (wj2_tmp.Pt() > wj1_tmp.Pt()) {
            std::swap(wj1_tmp, wj2_tmp);
            std::swap(wj1s_tmp, wj2s_tmp);
          }
          wj1 = wj1_tmp;
          wj2 = wj2_tmp;
          wj1_shift = wj1s_tmp;
          wj2_shift = wj2s_tmp;
        }
      }
    }

    double mjjWide = 0.0, dEtaWide = 0.0, dPhiWide = 0.0, mjjWide_shift = 0.0;
    if (wj1.Pt() > 0.0 && wj2.Pt() > 0.0) {
      const TLorentzVector wdijet = wj1 + wj2;
      const TLorentzVector wdijetShift = wj1_shift + wj2_shift;
      mjjWide = wdijet.M();
      dEtaWide = std::fabs(wj1.Eta() - wj2.Eta());
      dPhiWide = std::fabs(wj1.DeltaPhi(wj2));
      mjjWide_shift = wdijetShift.M();
    }

    double mjjAK4 = 0.0, dEtaAK4 = 0.0, dPhiAK4 = 0.0;
    if (AK4j1.Pt() > 0.0 && AK4j2.Pt() > 0.0) {
      const TLorentzVector dij = AK4j1 + AK4j2;
      mjjAK4 = dij.M();
      dEtaAK4 = std::fabs(AK4j1.Eta() - AK4j2.Eta());
      dPhiAK4 = std::fabs(AK4j1.DeltaPhi(AK4j2));
    }

    // sumEt = scalar sum of jet pT.
    double sumEtVal = 0.0;
    for (size_t i = 0; i < nJets; ++i) {
      const double pTj = getCorrPt(i);
      if (pTj > 0.0) sumEtVal += pTj;
    }
    if (sumEtVal <= 0.0) sumEtVal = -1.0;

    float metSigVal = -1.0f;
    if (metVal >= 0.0f && sumEtVal > 0.0) {
      metSigVal = static_cast<float>(static_cast<double>(metVal) / std::sqrt(sumEtVal));
    }

    float unclusteredEnFracVal = (sumEtVal > 0.0)
                                     ? static_cast<float>((sumEtVal - htAK4RawForUnclustered) / sumEtVal)
                                     : -1.0f;
    if (unclusteredEnFracVal > 1.0f) unclusteredEnFracVal = 1.0f;
    if (unclusteredEnFracVal < -1.0f) unclusteredEnFracVal = -1.0f;

    auto getChMultOut = [&](size_t idx) -> int {
      if (useStandardLikeJets) return static_cast<int>((*chHadMultMC)[idx]);
      const int chVal = static_cast<int>((*chHadMultData)[idx]);
      const int hfHadVal = static_cast<int>((*hfHadMultData)[idx]);
      return chVal + hfHadVal;
    };
    auto getNeMultOut = [&](size_t idx) -> int {
      if (useStandardLikeJets) return static_cast<int>((*neHadMultMC)[idx]);
      const int neVal = static_cast<int>((*neHadMultData)[idx]);
      const int hfEmVal = static_cast<int>((*hfEmMultData)[idx]);
      return neVal + hfEmVal;
    };
    auto getPhoMultOut = [&](size_t idx) -> int {
      return useStandardLikeJets ? 0 : static_cast<int>((*phoMultData)[idx]);
    };

    resetCuts();

    fillVariableWithValue("run", runNoEvt);
    fillVariableWithValue("event", static_cast<double>(evtNoEvt));
    fillVariableWithValue("lumi", lumiEvt);
    fillVariableWithValue("nVtx", nvtxEvt);

    fillVariableWithValue("nJet", (wj1.Pt() > 0.0 && wj2.Pt() > 0.0) ? 2 : 0);
    fillVariableWithValue("met", metVal);
    fillVariableWithValue("metphi", metPhiVal);
    fillVariableWithValue("metSig", metSigVal);
    fillVariableWithValue("NAK4PF", NAK4PF);
    fillVariableWithValue("PassJSON", passJSON(runNoEvt, lumiEvt, isDataEvt));

    if (AK4j1.Pt() > 0.0 && nJets >= 1U) {
      const auto j0 = sortedIdx[0];
      fillVariableWithValue("IdTight_j1", jetID[j0]);
      fillVariableWithValue("pTAK4PF_j1", AK4j1.Pt());
      fillVariableWithValue("etaAK4PF_j1", AK4j1.Eta());
      fillVariableWithValue("phiAK4PF_j1", AK4j1.Phi());
      fillVariableWithValue("jetJecAK4PF_j1", doReapplyJEC ? jecFactor[j0] : jecFacInput[j0]);
      fillVariableWithValue("jetJecUncAK4PF_j1", doReapplyJEC ? jecUncRel[j0] : jecUncInput[j0]);

      fillVariableWithValue("neutrHadEnFrac_j1", nhf[j0]);
      fillVariableWithValue("chargedHadEnFrac_j1", chf[j0]);
      fillVariableWithValue("photonEnFrac_j1", phf[j0]);
      // fillVariableWithValue("eleEnFract_j1", elf[j0]);
      fillVariableWithValue("muEnFract_j1", muf[j0]);
      fillVariableWithValue("neutrElectromFrac_j1", nemf[j0]);
      // fillVariableWithValue("chargedElectromFrac_j1", cemf[j0]);
      fillVariableWithValue("chargedMult_j1", getChMultOut(j0));
      fillVariableWithValue("neutrMult_j1", getNeMultOut(j0));
      fillVariableWithValue("photonMult_j1", getPhoMultOut(j0));
    }

    if (AK4j2.Pt() > 0.0 && nJets >= 2U) {
      const auto j1 = sortedIdx[1];
      fillVariableWithValue("IdTight_j2", jetID[j1]);
      fillVariableWithValue("pTAK4PF_j2", AK4j2.Pt());
      fillVariableWithValue("etaAK4PF_j2", AK4j2.Eta());
      fillVariableWithValue("phiAK4PF_j2", AK4j2.Phi());
      fillVariableWithValue("jetJecAK4PF_j2", doReapplyJEC ? jecFactor[j1] : jecFacInput[j1]);
      fillVariableWithValue("jetJecUncAK4PF_j2", doReapplyJEC ? jecUncRel[j1] : jecUncInput[j1]);

      fillVariableWithValue("neutrHadEnFrac_j2", nhf[j1]);
      fillVariableWithValue("chargedHadEnFrac_j2", chf[j1]);
      fillVariableWithValue("photonEnFrac_j2", phf[j1]);
      // fillVariableWithValue("eleEnFract_j2", elf[j1]);
      fillVariableWithValue("muEnFract_j2", muf[j1]);
      fillVariableWithValue("neutrElectromFrac_j2", nemf[j1]);
      // fillVariableWithValue("chargedElectromFrac_j2", cemf[j1]);
      fillVariableWithValue("chargedMult_j2", getChMultOut(j1));
      fillVariableWithValue("neutrMult_j2", getNeMultOut(j1));
      fillVariableWithValue("photonMult_j2", getPhoMultOut(j1));

      fillVariableWithValue("Dijet_MassAK4PF", mjjAK4);
      fillVariableWithValue("CosThetaStarAK4PF", TMath::TanH((AK4j1.Eta() - AK4j2.Eta()) / 2.0));
      fillVariableWithValue("deltaETAjjAK4PF", dEtaAK4);
      fillVariableWithValue("deltaPHIjjAK4PF", dPhiAK4);
    }

    if (wj1.Pt() > 0.0) {
      fillVariableWithValue("pTWJ_j1", wj1.Pt());
      fillVariableWithValue("etaWJ_j1", wj1.Eta());
      fillVariableWithValue("massWJ_j1", wj1.M());
      fillVariableWithValue("phiWJ_j1", wj1.Phi());
      fillVariableWithValue("rapidityWJ_j1", wj1.Rapidity());
    }

    if (wj2.Pt() > 0.0) {
      fillVariableWithValue("pTWJ_j2", wj2.Pt());
      fillVariableWithValue("etaWJ_j2", wj2.Eta());
      fillVariableWithValue("deltaETAjj", dEtaWide);
      fillVariableWithValue("mjj", mjjWide);
      fillVariableWithValue("mjj_shiftJEC", mjjWide_shift);
      fillVariableWithValue("massWJ_j2", wj2.M());
      fillVariableWithValue("phiWJ_j2", wj2.Phi());
      fillVariableWithValue("CosThetaStarWJ", TMath::TanH((wj1.Eta() - wj2.Eta()) / 2.0));
      fillVariableWithValue("deltaPHIjj", dPhiWide);
      fillVariableWithValue("rapidityWJ_j2", wj2.Rapidity());
    }

    const double METoverHTAK4PF = (HTAK4PF > 0.0) ? double(metVal / HTAK4PF) : 0.0;
    fillVariableWithValue("HTAK4PF", HTAK4PF);
    fillVariableWithValue("METoverHTAK4PF", METoverHTAK4PF);
    fillVariableWithValue("unclusteredEnFracAK4PF", unclusteredEnFracVal);

    auto abs_dphi = [](double a, double b) { return std::abs(TVector2::Phi_mpi_pi(a - b)); };

    double dphi_min2 = 1e9;
    const double met_phi = getVariableValue("metphi");
    if (variableIsFilled("phiAK4PF_j1")) dphi_min2 = std::min(dphi_min2, abs_dphi(met_phi, getVariableValue("phiAK4PF_j1")));
    if (variableIsFilled("phiAK4PF_j2")) dphi_min2 = std::min(dphi_min2, abs_dphi(met_phi, getVariableValue("phiAK4PF_j2")));
    fillVariableWithValue("minDPhiMetAK4PF", dphi_min2 < 1e8 ? dphi_min2 : -1.0);

    const bool passPFScoutingHT = trigJetHT ? static_cast<bool>(**trigJetHT) : false;
    const bool passPFScoutingSingleMuon = trigSingleMuon ? static_cast<bool>(**trigSingleMuon) : false;
    bool passRequiredL1 = true;
    bool passVetoL1 = true;
    if (applyL1TriggerVeto) {
      passRequiredL1 = false;
      for (const auto& bitReader : requiredL1Readers) {
        if (static_cast<bool>(**bitReader)) {
          passRequiredL1 = true;
          break;
        }
      }
      for (const auto& bitReader : vetoL1Readers) {
        if (static_cast<bool>(**bitReader)) {
          passVetoL1 = false;
          break;
        }
      }
    }
    const bool passPFScoutingHTForHist = passPFScoutingHT && passRequiredL1 && passVetoL1;
    fillVariableWithValue("passHLT_PFScoutingHT", passPFScoutingHTForHist ? 1 : 0);
    fillVariableWithValue("passHLT_PFScouting_SingleMuon", passPFScoutingSingleMuon ? 1 : 0);

    evaluateCuts();

    const bool fullAnalysis =
        passedCut("PassJSON") &&
        passedCut("nVtx") &&
        passedCut("IdTight_j1") &&
        passedCut("IdTight_j2") &&
        (getVariableValue("pTWJ_j1") > getPreCutValue1("pT_WJ1")) &&
        (getVariableValue("pTWJ_j2") > getPreCutValue1("pT_WJ2")) &&
        (getVariableValue("etaWJ_j1") > getPreCutValue1("eta_WJ1")) &&
        (getVariableValue("etaWJ_j1") < getPreCutValue2("eta_WJ1")) &&
        (getVariableValue("etaWJ_j2") > getPreCutValue1("eta_WJ2")) &&
        (getVariableValue("etaWJ_j2") < getPreCutValue2("eta_WJ2")) &&
        (getVariableValue("deltaETAjj") < getPreCutValue1("Deta_WJJ"));

    if (fullAnalysis) {
      h_mjj_HLTpass[0]->Fill(mjjWide);
      h_mjj_noTrig_1GeVbin->Fill(mjjWide);
      if (passPFScoutingHTForHist) h_mjj_HLTpass[1]->Fill(mjjWide);
      if (passPFScoutingSingleMuon) h_mjj_HLTpass[2]->Fill(mjjWide);
    }

    fillReducedSkimTree();
  }

  for (int i = 0; i < 3; ++i) {
    h_mjj_HLTpass[i]->Write();
  }
  std::cout << "[analysisClass] Loop end\n";
}

