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

const std::string era = "2024I";
const std::string dataList = "data/cfg/data_jec_list.txt";
const std::string mcList = "data/cfg/mc_jec_list.txt";

// Input branch groups for scouting data, MC, and monitoring samples.
const std::array<const char*, 35> kNanoInputBranchesData = {
    "run",
    "luminosityBlock",
    "event",
    "nScoutingPrimaryVertex",
    "ScoutingRho_fixedGridRhoFastjetAll",
    "ScoutingMET_pt",
    "ScoutingMET_phi",
    "nScoutingPFJetRecluster",
    "ScoutingPFJetRecluster_nConstituents",
    "ScoutingPFJetRecluster_nCh",
    "ScoutingPFJetRecluster_nElectrons",
    "ScoutingPFJetRecluster_nMuons",
    "ScoutingPFJetRecluster_nNh",
    "ScoutingPFJetRecluster_nPhotons",
    "ScoutingPFJetRecluster_area",
    "ScoutingPFJetRecluster_chEmEF",
    "ScoutingPFJetRecluster_chHEF",
    "ScoutingPFJetRecluster_eta",
    "ScoutingPFJetRecluster_hfEmEF",
    "ScoutingPFJetRecluster_hfHEF",
    "ScoutingPFJetRecluster_mass",
    "ScoutingPFJetRecluster_muEF",
    "ScoutingPFJetRecluster_neEmEF",
    "ScoutingPFJetRecluster_neHEF",
    "ScoutingPFJetRecluster_phi",
    "ScoutingPFJetRecluster_pt",
    "ScoutingPFJetRecluster_particleNet_prob_b",
    "ScoutingPFJetRecluster_particleNet_prob_bb",
    "ScoutingPFJetRecluster_particleNet_prob_c",
    "ScoutingPFJetRecluster_particleNet_prob_cc",
    "ScoutingPFJetRecluster_particleNet_prob_g",
    "ScoutingPFJetRecluster_particleNet_prob_uds",
    "ScoutingPFJetRecluster_particleNet_prob_undef",
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

// Scouting muon inputs for the good-muon control sample and muon-corrected jets.
// These branches support the muon-control-sample-like trigger-efficiency workflow.
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

// L1 bits used to build the saved JetHT-side L1 decision flag.
const std::array<const char*, 4> kRequiredL1DecisionBranches = {
    "L1_HTT280er",
    "L1_SingleJet180",
    "L1_DoubleJet30er2p5_Mass_Min250_dEta_Max1p5",
    "L1_ETT2000"};

const std::array<const char*, 2> kVetoL1DecisionBranches = {
    "L1_HTT200er",
    "L1_HTT255er"};

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

// Shared good-muon definition used by the trigger-efficiency control sample.
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
  if (nRecoMuonMatchedStations <= 2.0) return false;
  if (nValidPixelHits <= 1.0) return false;
  if (nTrackerLayersWithMeasurement <= 7.0) return false;
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

  // Build the JEC tools once up front so the event loop can switch between raw and corrected jets cheaply.
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
      std::cerr << "[analysisClass] WARNING: no JEC uncertainty file configured. JEC uncertainties and JEC shifts will be disabled.\n";
    }
  }

  std::cout << "[analysisClass] Constructor (ctor) end\n";
}

analysisClass::~analysisClass() { std::cout << "[analysisClass] Destructor (dtor)\n"; }

void analysisClass::Loop() {
  std::cout << "[analysisClass] Loop begin\n";
  if (fChain == nullptr) return;

  // Infer which branch family to read from the file content instead of hard-coding the sample type.
  const bool hasGenWeight = hasBranch(fChain, "genWeight");
  const bool hasScoutingRecoJets = hasBranch(fChain, "ScoutingPFJetRecluster_pt");
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
    std::cerr << "[analysisClass] ERROR: Data mode selected but neither 'ScoutingPFJetRecluster_pt' nor 'Jet_pt' is available.\n";
    return;
  }

  const bool useStandardMC = (inputMode == InputSampleMode::kMC);
  const bool useMonitoringData = (inputMode == InputSampleMode::kMonitoringData);
  const bool useStandardLikeJets = useStandardMC || useMonitoringData;
  const bool isDataSample = !hasGenWeight;
  const bool produceAdditionalTriggerStudyPlots =
      (getPreCutValue1("produceAdditionalTriggerStudyPlots") > 0.5);
  const bool applyGoodMuonSelection_analysis =
      (inputMode == InputSampleMode::kScoutingData) && (getPreCutValue1("applyGoodMuonSelection_analysis") > 0.5);
  const bool needScoutingMuonBranches =
      (inputMode == InputSampleMode::kScoutingData) &&
      (applyGoodMuonSelection_analysis || produceAdditionalTriggerStudyPlots);

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

  // Turn on only the branch group needed for the detected input mode.
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
  if (isDataSample) {
    enableBranches(kRequiredL1DecisionBranches, 0U);
    enableBranches(kVetoL1DecisionBranches, 0U);
  }
  if (needScoutingMuonBranches) {
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

  // Build the event-level and object-level readers once after the input mode is fixed.
  TTreeReader reader(fChain);

  TTreeReaderValue<UInt_t> run(reader, "run");
  TTreeReaderValue<UInt_t> luminosityBlock(reader, "luminosityBlock");
  TTreeReaderValue<ULong64_t> event(reader, "event");

  const char* rhoBranch = useStandardLikeJets ? "Rho_fixedGridRhoFastjetAll" : "ScoutingRho_fixedGridRhoFastjetAll";
  const char* metPtBranch = useStandardLikeJets ? "PFMET_pt" : "ScoutingMET_pt";
  const char* metPhiBranch = useStandardLikeJets ? "PFMET_phi" : "ScoutingMET_phi";
  const char* nJetBranch = useStandardLikeJets ? "nJet" : "nScoutingPFJetRecluster";
  const char* jetPtBranch = useStandardLikeJets ? "Jet_pt" : "ScoutingPFJetRecluster_pt";
  const char* jetEtaBranch = useStandardLikeJets ? "Jet_eta" : "ScoutingPFJetRecluster_eta";
  const char* jetPhiBranch = useStandardLikeJets ? "Jet_phi" : "ScoutingPFJetRecluster_phi";
  const char* jetMassBranch = useStandardLikeJets ? "Jet_mass" : "ScoutingPFJetRecluster_mass";
  const char* jetAreaBranch = useStandardLikeJets ? "Jet_area" : "ScoutingPFJetRecluster_area";

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

  std::unique_ptr<TTreeReaderArray<Float_t>> jetChHEFData;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetNeHEFData;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetNeEmEFData;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetChEmEFData;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetMuEFData;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetHFEmEFData;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetHFHadEFData;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetPNetBData;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetPNetBBData;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetPNetCData;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetPNetCCData;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetPNetGData;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetPNetUDSData;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetPNetUndefData;

  std::unique_ptr<TTreeReaderArray<Int_t>> chHadMultData;
  std::unique_ptr<TTreeReaderArray<Int_t>> neHadMultData;
  std::unique_ptr<TTreeReaderArray<Int_t>> phoMultData;
  std::unique_ptr<TTreeReaderArray<Int_t>> elMultData;
  std::unique_ptr<TTreeReaderArray<Int_t>> muMultData;
  std::unique_ptr<TTreeReaderArray<UChar_t>> nConstituentsData;

  std::unique_ptr<TTreeReaderArray<Float_t>> jetChHEFMC;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetNeHEFMC;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetNeEmEFMC;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetChEmEFMC;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetMuEFMC;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetHFEmEFMC;
  std::unique_ptr<TTreeReaderArray<Float_t>> jetRawFactorMC;
  std::unique_ptr<TTreeReaderArray<UChar_t>> chHadMultMC;
  std::unique_ptr<TTreeReaderArray<UChar_t>> neHadMultMC;

  std::unique_ptr<TTreeReaderValue<Int_t>> nScoutingMuonVtxReader;
  std::unique_ptr<TTreeReaderArray<Float_t>> scoutingMuonPtReader;
  std::unique_ptr<TTreeReaderArray<Float_t>> scoutingMuonEtaReader;
  std::unique_ptr<TTreeReaderArray<Float_t>> scoutingMuonPhiReader;
  std::unique_ptr<TTreeReaderArray<Float_t>> scoutingMuonDxyReader;
  std::unique_ptr<TTreeReaderArray<Float_t>> scoutingMuonDzReader;
  std::unique_ptr<TTreeReaderArray<Float_t>> scoutingMuonNormChi2Reader;
  std::unique_ptr<TTreeReaderArray<Int_t>> scoutingMuonHitsReader;
  std::unique_ptr<TTreeReaderArray<Int_t>> scoutingMuonStationsReader;
  std::unique_ptr<TTreeReaderArray<Int_t>> scoutingMuonPixelHitsReader;
  std::unique_ptr<TTreeReaderArray<Int_t>> scoutingMuonLayersReader;

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

    jetChHEFData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJetRecluster_chHEF");
    jetNeHEFData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJetRecluster_neHEF");
    jetNeEmEFData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJetRecluster_neEmEF");
    jetChEmEFData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJetRecluster_chEmEF");
    jetMuEFData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJetRecluster_muEF");
    jetHFEmEFData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJetRecluster_hfEmEF");
    jetHFHadEFData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJetRecluster_hfHEF");
    jetPNetBData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJetRecluster_particleNet_prob_b");
    jetPNetBBData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJetRecluster_particleNet_prob_bb");
    jetPNetCData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJetRecluster_particleNet_prob_c");
    jetPNetCCData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJetRecluster_particleNet_prob_cc");
    jetPNetGData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJetRecluster_particleNet_prob_g");
    jetPNetUDSData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJetRecluster_particleNet_prob_uds");
    jetPNetUndefData = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingPFJetRecluster_particleNet_prob_undef");

    chHadMultData = std::make_unique<TTreeReaderArray<Int_t>>(reader, "ScoutingPFJetRecluster_nCh");
    neHadMultData = std::make_unique<TTreeReaderArray<Int_t>>(reader, "ScoutingPFJetRecluster_nNh");
    phoMultData = std::make_unique<TTreeReaderArray<Int_t>>(reader, "ScoutingPFJetRecluster_nPhotons");
    elMultData = std::make_unique<TTreeReaderArray<Int_t>>(reader, "ScoutingPFJetRecluster_nElectrons");
    muMultData = std::make_unique<TTreeReaderArray<Int_t>>(reader, "ScoutingPFJetRecluster_nMuons");
    nConstituentsData = std::make_unique<TTreeReaderArray<UChar_t>>(reader, "ScoutingPFJetRecluster_nConstituents");
  }

  if (needScoutingMuonBranches) {
    nScoutingMuonVtxReader = std::make_unique<TTreeReaderValue<Int_t>>(reader, "nScoutingMuonVtx");
    scoutingMuonPtReader = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingMuonVtx_pt");
    scoutingMuonEtaReader = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingMuonVtx_eta");
    scoutingMuonPhiReader = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingMuonVtx_phi");
    scoutingMuonDxyReader = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingMuonVtx_trk_dxy");
    scoutingMuonDzReader = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingMuonVtx_trk_dz");
    scoutingMuonNormChi2Reader = std::make_unique<TTreeReaderArray<Float_t>>(reader, "ScoutingMuonVtx_normchi2");
    scoutingMuonHitsReader = std::make_unique<TTreeReaderArray<Int_t>>(reader, "ScoutingMuonVtx_nValidRecoMuonHits");
    scoutingMuonStationsReader =
        std::make_unique<TTreeReaderArray<Int_t>>(reader, "ScoutingMuonVtx_nRecoMuonMatchedStations");
    scoutingMuonPixelHitsReader =
        std::make_unique<TTreeReaderArray<Int_t>>(reader, "ScoutingMuonVtx_nValidPixelHits");
    scoutingMuonLayersReader =
        std::make_unique<TTreeReaderArray<Int_t>>(reader, "ScoutingMuonVtx_nTrackerLayersWithMeasurement");
  }

  std::unique_ptr<TTreeReaderValue<Bool_t>> trigJetHT;
  std::unique_ptr<TTreeReaderValue<Bool_t>> trigSingleMuon;
  if (hasTrigJetHT) {
    trigJetHT = std::make_unique<TTreeReaderValue<Bool_t>>(reader, kTriggerJetHTBranch);
  }
  if (hasTrigSingleMuon) {
    trigSingleMuon = std::make_unique<TTreeReaderValue<Bool_t>>(reader, kTriggerSingleMuonBranch);
  }

  const bool hasAllRequiredL1Branches =
      std::all_of(kRequiredL1DecisionBranches.begin(), kRequiredL1DecisionBranches.end(),
                  [&](const char* branchName) { return hasBranch(fChain, branchName); });
  const bool hasAllVetoL1Branches =
      std::all_of(kVetoL1DecisionBranches.begin(), kVetoL1DecisionBranches.end(),
                  [&](const char* branchName) { return hasBranch(fChain, branchName); });
  const bool canEvaluateL1ForPFScoutingHT = isDataSample && hasAllRequiredL1Branches && hasAllVetoL1Branches;

  std::vector<std::unique_ptr<TTreeReaderValue<Bool_t>>> requiredL1Readers;
  std::vector<std::unique_ptr<TTreeReaderValue<Bool_t>>> vetoL1Readers;
  // Materialize the optional L1 readers only when all bits needed for the JetHT-side decision are present.
  auto buildL1Readers = [&](const auto& branchNames, auto& readers) {
    readers.reserve(branchNames.size());
    for (const char* branchName : branchNames) {
      readers.push_back(std::make_unique<TTreeReaderValue<Bool_t>>(reader, branchName));
    }
  };
  if (canEvaluateL1ForPFScoutingHT) {
    buildL1Readers(kRequiredL1DecisionBranches, requiredL1Readers);
    buildL1Readers(kVetoL1DecisionBranches, vetoL1Readers);
  }

  static const int nMassBins = 103;
  static const int nEvenDijetBins = 14000;
  static const double massBoundaries[nMassBins + 1] = {
      1, 3, 6, 10, 16, 23, 31, 40, 50, 61, 74, 88, 103, 119, 137, 156, 176, 197, 220, 244, 270, 296, 325,
      354, 386, 419, 453, 489, 526, 565, 606, 649, 693, 740, 788, 838, 890, 944, 1000, 1058, 1118, 1181, 1246, 1313, 1383, 1455, 1530, 1607, 1687,
      1770, 1856, 1945, 2037, 2132, 2231, 2332, 2438, 2546, 2659, 2775, 2895, 3019, 3147, 3279, 3416, 3558, 3704, 3854, 4010, 4171, 4337, 4509,
      4686, 4869, 5058, 5253, 5455, 5663, 5877, 6099, 6328, 6564, 6808, 7060, 7320, 7589, 7866, 8152, 8447, 8752, 9067, 9391, 9726, 10072, 10430,
      10798, 11179, 11571, 11977, 12395, 12827, 13272, 13732, 14000};

  enum TriggerStudyTrigger { kNoTrig = 0, kJetHTTrig = 1, kSingleMuonTrig = 2, kJetHTAndSingleMuonTrig = 3, kNTriggerStudyTriggers = 4 };
  const char* HLTname[kNTriggerStudyTriggers] = {"noTrig", "PFScouting_JetHT", "PFScouting_SingleMuon",
                                                 "PFScouting_JetHTAndSingleMuon"};
  enum TriggerStudyObservable { kWideDijet = 0, kAk4Dijet = 1, kHtAk4 = 2, kNTriggerStudyObservables = 3 };
  enum TriggerStudyMethod { kDefaultStudy = 0, kL1Study = 1, kGoodMuonStudy = 2, kGoodMuonL1Study = 3, kNTriggerStudyMethods = 4 };
  const char* triggerStudyObservableName[kNTriggerStudyObservables] = {"wideDijet", "AK4Jets", "HTAK4PF"};
  const char* triggerStudyMethodName[kNTriggerStudyMethods] = {"default", "L1", "goodMuon", "goodMuonL1"};

  TH1F* h_mjj_HLTpass[kNTriggerStudyTriggers];
  TH1F* h_mjj_noTrig_1GeVbin = new TH1F("h_mjj_noTrig_1GeVbin", "", 14000, 0, 14000);
  for (int i = 0; i < kNTriggerStudyTriggers; ++i) {
    h_mjj_HLTpass[i] = new TH1F(Form("h_mjj_HLTpass_%s", HLTname[i]), "", nMassBins, massBoundaries);
  }

  // Book the trigger-study histograms once per observable and per control-sample method.
  TH1F* h_trigStudy[kNTriggerStudyObservables][kNTriggerStudyMethods][kNTriggerStudyTriggers] = {};
  TH1F* h_trigStudyEven1GeV[kNTriggerStudyObservables][kNTriggerStudyMethods][kNTriggerStudyTriggers] = {};
  for (int obs = 0; obs < kNTriggerStudyObservables; ++obs) {
    for (int method = 0; method < kNTriggerStudyMethods; ++method) {
      if (!produceAdditionalTriggerStudyPlots && method != kDefaultStudy) continue;
      for (int trig = 0; trig < kNTriggerStudyTriggers; ++trig) {
        const TString hname = Form("h_%s_HLTpass_%s_%s",
                                   triggerStudyObservableName[obs],
                                   triggerStudyMethodName[method],
                                   HLTname[trig]);
        if (obs == kHtAk4) {
          h_trigStudy[obs][method][trig] = new TH1F(hname, "", 1500, 0.0, 1500.0);
        } else {
          h_trigStudy[obs][method][trig] = new TH1F(hname, "", nMassBins, massBoundaries);
        }
        if (obs == kWideDijet || obs == kAk4Dijet) {
          const TString even_hname =
              Form("h_%sEven1GeV_HLTpass_%s_%s", triggerStudyObservableName[obs], triggerStudyMethodName[method], HLTname[trig]);
          h_trigStudyEven1GeV[obs][method][trig] = new TH1F(even_hname, "", nEvenDijetBins, 0.0, 14000.0);
        }
      }
    }
  }

  const double jetFidRegion = getPreCutValue1("jetFidRegion");
  const int tightJetIDFlag = int(getPreCutValue1("tightJetID"));
  const double ptCut = getPreCutValue1("ptCut");
  const double pt0Cut = getPreCutValue1("pt0Cut");
  const double pt1Cut = getPreCutValue1("pt1Cut");
  const double wideJetDeltaR = getPreCutValue1("DeltaR");
  const double pTWJ1Min = getPreCutValue1("pT_WJ1");
  const double pTWJ2Min = getPreCutValue1("pT_WJ2");
  const double etaWJ1Min = getPreCutValue1("eta_WJ1");
  const double etaWJ1Max = getPreCutValue2("eta_WJ1");
  const double etaWJ2Min = getPreCutValue1("eta_WJ2");
  const double etaWJ2Max = getPreCutValue2("eta_WJ2");
  const double detaWJJMax = getPreCutValue1("Deta_WJJ");
  constexpr double kTriggerStudyHtEtaMax = 2.5;
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
  std::vector<int> jetID;
  std::vector<TLorentzVector> recoJetP4;
  std::vector<TLorentzVector> recoJetP4Shift;
  std::vector<TLorentzVector> scoutingAllMuons;
  std::vector<TLorentzVector> analysisJetP4Storage;
  std::vector<TLorentzVector> analysisJetP4ShiftStorage;
  std::vector<TLorentzVector> trigEffGoodMuonJetP4Storage;
  std::vector<TLorentzVector> trigEffGoodMuonJetP4ShiftStorage;

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
  jetID.reserve(512);
  recoJetP4.reserve(512);
  recoJetP4Shift.reserve(512);
  scoutingAllMuons.reserve(16);
  analysisJetP4Storage.reserve(512);
  analysisJetP4ShiftStorage.reserve(512);
  trigEffGoodMuonJetP4Storage.reserve(512);
  trigEffGoodMuonJetP4ShiftStorage.reserve(512);

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
    jetID.assign(nJets, 0);

    // Recompute jet corrections and JetID inputs before building any derived observables.
    const bool doReapplyJEC = reapplyJEC && JetCorrector && JetCorrector_data;
    if (reapplyJEC && !doReapplyJEC && !warnedMissingJEC) {
      std::cerr << "[analysisClass] WARNING: useJECs=1 but JEC correctors are not fully initialized. Falling back to input jet kinematics.\n";
      warnedMissingJEC = true;
    }
    if (doReapplyJEC && !unc && !warnedMissingJECUnc) {
      std::cerr << "[analysisClass] WARNING: no JEC uncertainty payload is available. Relative JEC uncertainties and shiftJECs will be disabled.\n";
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
        std::cerr << "[analysisClass] WARNING: failed to build data JEC corrector for run " << runNoEvt << " - continuing with previous one.\n";
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
        chf[i] = static_cast<double>((*jetChHEFData)[i]);
        nhf[i] = static_cast<double>((*jetNeHEFData)[i]) + static_cast<double>((*jetHFHadEFData)[i]);
        muf[i] = static_cast<double>((*jetMuEFData)[i]);
        cemf[i] = static_cast<double>((*jetChEmEFData)[i]);
        elf[i] = cemf[i];
        nemf[i] = static_cast<double>((*jetNeEmEFData)[i]);

        const double hfEmF = static_cast<double>((*jetHFEmEFData)[i]);
        phf[i] = std::max(0.0, nemf[i] - hfEmF);

        chMultVal = static_cast<int>((*chHadMultData)[i]);
        neMultVal = static_cast<int>((*neHadMultData)[i]);
        const int phoMultVal = static_cast<int>((*phoMultData)[i]);
        const int elMultVal = static_cast<int>((*elMultData)[i]);
        const int muMultVal = static_cast<int>((*muMultData)[i]);
        numConstVal = chMultVal + neMultVal + muMultVal + elMultVal + phoMultVal;
        numChargedParticlesVal = chMultVal;
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

    recoJetP4.resize(nJets);
    recoJetP4Shift.resize(nJets);
    for (size_t i = 0; i < nJets; ++i) {
      const double pTj = getCorrPt(i);
      const double massj = getMassForMode(i);
      const double shiftFactor = shiftFactorForJet(i);
      recoJetP4[i].SetPtEtaPhiM(pTj, jetEta[i], jetPhi[i], massj);
      recoJetP4Shift[i].SetPtEtaPhiM(pTj * shiftFactor, jetEta[i], jetPhi[i], massj * shiftFactor);
    }

    // Read the scouting muons once so we can build both trigger-study and analysis-side jet collections.
    scoutingAllMuons.clear();
    int nGoodScoutingMuons = 0;
    if (needScoutingMuonBranches) {
      const int nMuons = static_cast<int>(**nScoutingMuonVtxReader);
      scoutingAllMuons.reserve(static_cast<size_t>(std::max(0, nMuons)));

      for (int i = 0; i < nMuons; ++i) {
        const double muPt = static_cast<double>((*scoutingMuonPtReader)[i]);
        const double muEta = static_cast<double>((*scoutingMuonEtaReader)[i]);
        const double muPhi = static_cast<double>((*scoutingMuonPhiReader)[i]);
        const double muDxy = static_cast<double>((*scoutingMuonDxyReader)[i]);
        const double muDz = static_cast<double>((*scoutingMuonDzReader)[i]);
        const double muNormChi2 = static_cast<double>((*scoutingMuonNormChi2Reader)[i]);
        const double muHits = static_cast<double>((*scoutingMuonHitsReader)[i]);
        const double muStations = static_cast<double>((*scoutingMuonStationsReader)[i]);
        const double muPixelHits = static_cast<double>((*scoutingMuonPixelHitsReader)[i]);
        const double muLayers = static_cast<double>((*scoutingMuonLayersReader)[i]);

        TLorentzVector muP4;
        muP4.SetPtEtaPhiM(muPt, muEta, muPhi, 0.105);
        scoutingAllMuons.push_back(muP4);
        if (passesScoutingMuonID(muPt, muEta, muDxy, muDz, muNormChi2, muHits, muStations, muPixelHits,
                                 muLayers)) {
          ++nGoodScoutingMuons;
        }
      }
    }

    const bool hasGoodScoutingMuon =
        (inputMode == InputSampleMode::kScoutingData) && (nGoodScoutingMuons >= 1);

    // Remove nearby muon p4 from accepted scouting jets without mutating the input jet collection in place.
    auto makeMuonCleanedJets = [&](const std::vector<TLorentzVector>& inputJets,
                                   const std::vector<TLorentzVector>& inputJetsShift,
                                   std::vector<TLorentzVector>& cleanedJets,
                                   std::vector<TLorentzVector>& cleanedJetsShift) {
      cleanedJets = inputJets;
      cleanedJetsShift = inputJetsShift;

      for (size_t j = 0; j < nJets; ++j) {
        if (std::abs(jetEta[j]) > 5.0) continue;
        if (jetID[j] != tightJetIDFlag) continue;
        if (inputJets[j].Pt() <= ptCut) continue;

        TLorentzVector cleanedJetP4 = cleanedJets[j];
        TLorentzVector cleanedJetP4Shift = cleanedJetsShift[j];
        bool hadOverlap = false;
        for (const auto& muP4 : scoutingAllMuons) {
          if (deltaRDistance(jetEta[j], jetPhi[j], muP4.Eta(), muP4.Phi()) >= 0.4) continue;
          cleanedJetP4 -= muP4;
          cleanedJetP4Shift -= muP4;
          hadOverlap = true;
        }

        if (hadOverlap && cleanedJetP4.Pt() < 1.0) {
          cleanedJets[j].SetPxPyPzE(0.0, 0.0, 0.0, 0.0);
          cleanedJetsShift[j].SetPxPyPzE(0.0, 0.0, 0.0, 0.0);
          continue;
        }

        cleanedJets[j] = cleanedJetP4;
        cleanedJetsShift[j] = cleanedJetP4Shift;
      }
    };

    // Start from the nominal reco jets and optionally replace them with the muon-cleaned version for analysis outputs.
    // Only the analysis-side switch is allowed to change the branches written to the reduced ntuple.
    const std::vector<TLorentzVector>* analysisJetP4 = &recoJetP4;
    const std::vector<TLorentzVector>* analysisJetP4Shift = &recoJetP4Shift;
    if (applyGoodMuonSelection_analysis) {
      if (!hasGoodScoutingMuon) continue;
      makeMuonCleanedJets(recoJetP4, recoJetP4Shift, analysisJetP4Storage, analysisJetP4ShiftStorage);
      analysisJetP4 = &analysisJetP4Storage;
      analysisJetP4Shift = &analysisJetP4ShiftStorage;
    }

    const bool analysisUsesMuonCleanedJets = applyGoodMuonSelection_analysis;
    const std::vector<TLorentzVector>* trigEffGoodMuonJetP4 = nullptr;
    const std::vector<TLorentzVector>* trigEffGoodMuonJetP4Shift = nullptr;
    if (hasGoodScoutingMuon) {
      if (analysisUsesMuonCleanedJets) {
        trigEffGoodMuonJetP4 = analysisJetP4;
        trigEffGoodMuonJetP4Shift = analysisJetP4Shift;
      } else {
        makeMuonCleanedJets(recoJetP4,
                            recoJetP4Shift,
                            trigEffGoodMuonJetP4Storage,
                            trigEffGoodMuonJetP4ShiftStorage);
        trigEffGoodMuonJetP4 = &trigEffGoodMuonJetP4Storage;
        trigEffGoodMuonJetP4Shift = &trigEffGoodMuonJetP4ShiftStorage;
      }
    }

    struct JetRecoSummary {
      std::vector<unsigned> sortedIdx;
      int nAk4Analysis = 0;
      double htAk4Analysis = 0.0;
      double htAk4TrigEff = 0.0;
      double htAk4RawForUnclustered = 0.0;
      TLorentzVector ak4j1, ak4j2;
      TLorentzVector wj1, wj2, wj1Shift, wj2Shift;
      double mjjWide = 0.0;
      double dEtaWide = 0.0;
      double dPhiWide = 0.0;
      double mjjWideShift = 0.0;
      double mjjAk4 = 0.0;
      double dEtaAk4 = 0.0;
      double dPhiAk4 = 0.0;
    };

    // Rebuild AK4, HT, and wide-dijet observables from a chosen jet collection.
    auto buildJetRecoSummary = [&](const std::vector<TLorentzVector>& jets,
                                   const std::vector<TLorentzVector>& jetsShift) {
      JetRecoSummary out;
      out.sortedIdx.resize(nJets);
      std::iota(out.sortedIdx.begin(), out.sortedIdx.end(), 0U);
      std::sort(out.sortedIdx.begin(), out.sortedIdx.end(),
                [&](unsigned a, unsigned b) { return jets[a].Pt() > jets[b].Pt(); });

      // Cache the sorted-order jet selections once so the HT and wide-jet steps can reuse them verbatim.
      std::vector<unsigned> legacyWideIdx;
      std::vector<unsigned> fastJetInputIdx;
      legacyWideIdx.reserve(nJets);
      fastJetInputIdx.reserve(nJets);

      for (size_t k = 0; k < nJets; ++k) {
        const auto j = out.sortedIdx[k];
        const double pTj = jets[j].Pt();
        const double absEta = std::fabs(jets[j].Eta());
        const bool passTightID = (jetID[j] == tightJetIDFlag);
        if (pTj > ptCut) out.htAk4RawForUnclustered += rawPt[j];

        if (passTightID && pTj > ptCut) {
          if (absEta < jetFidRegion) {
            ++out.nAk4Analysis;
            out.htAk4Analysis += pTj;
            legacyWideIdx.push_back(j);
          }
          if (absEta < kTriggerStudyHtEtaMax) {
            out.htAk4TrigEff += pTj;
          }
        }

        const double minPt = (k == 0) ? pt0Cut : ((k == 1) ? pt1Cut : ptCut);
        if (passTightID && absEta < jetFidRegion && pTj > minPt) {
          fastJetInputIdx.push_back(j);
        }
      }

      if (nJets >= 2U) {
        const auto j0 = out.sortedIdx[0];
        const auto j1 = out.sortedIdx[1];

        if (jets[j0].Pt() > pt0Cut && jets[j1].Pt() > pt1Cut) {
          out.ak4j1 = jets[j0];
          out.ak4j2 = jets[j1];

          if (useFastJet) {
            // FastJet mode reclusters the accepted AK4 jets directly into the wide-jet pair.
            std::vector<fastjet::PseudoJet> fjInputs;
            std::vector<fastjet::PseudoJet> fjInputsShift;
            fjInputs.reserve(fastJetInputIdx.size());
            fjInputsShift.reserve(fastJetInputIdx.size());

            for (const auto j : fastJetInputIdx) {
              fjInputs.emplace_back(jets[j].Px(), jets[j].Py(), jets[j].Pz(), jets[j].E());
              fjInputsShift.emplace_back(jetsShift[j].Px(), jetsShift[j].Py(), jetsShift[j].Pz(), jetsShift[j].E());
            }

            if (fjInputs.size() > 1U) {
              fastjet::ClusterSequence clusterSeq(fjInputs, *fjJetDefinition);
              fastjet::ClusterSequence clusterSeqShift(fjInputsShift, *fjJetDefinition);
              const auto wideJets = fastjet::sorted_by_pt(clusterSeq.inclusive_jets(0.0));
              const auto wideJetsShift = fastjet::sorted_by_pt(clusterSeqShift.inclusive_jets(0.0));
              if (wideJets.size() > 1U && wideJetsShift.size() > 1U) {
                out.wj1.SetPxPyPzE(wideJets[0].px(), wideJets[0].py(), wideJets[0].pz(), wideJets[0].e());
                out.wj2.SetPxPyPzE(wideJets[1].px(), wideJets[1].py(), wideJets[1].pz(), wideJets[1].e());
                out.wj1Shift.SetPxPyPzE(wideJetsShift[0].px(), wideJetsShift[0].py(), wideJetsShift[0].pz(),
                                        wideJetsShift[0].e());
                out.wj2Shift.SetPxPyPzE(wideJetsShift[1].px(), wideJetsShift[1].py(), wideJetsShift[1].pz(),
                                        wideJetsShift[1].e());
              }
            }
          } else {
            // Legacy mode grows each wide jet around the two leading AK4 seeds.
            TLorentzVector wj1Tmp, wj2Tmp, wj1ShiftTmp, wj2ShiftTmp;

            for (const auto j : legacyWideIdx) {
              const double dR1 = jets[j].DeltaR(out.ak4j1);
              const double dR2 = jets[j].DeltaR(out.ak4j2);

              if (dR1 < dR2 && dR1 < wideJetDeltaR) {
                wj1Tmp += jets[j];
                wj1ShiftTmp += jetsShift[j];
              } else if (dR2 < wideJetDeltaR) {
                wj2Tmp += jets[j];
                wj2ShiftTmp += jetsShift[j];
              }
            }

            if (wj2Tmp.Pt() > wj1Tmp.Pt()) {
              std::swap(wj1Tmp, wj2Tmp);
              std::swap(wj1ShiftTmp, wj2ShiftTmp);
            }
            out.wj1 = wj1Tmp;
            out.wj2 = wj2Tmp;
            out.wj1Shift = wj1ShiftTmp;
            out.wj2Shift = wj2ShiftTmp;
          }
        }
      }

      if (out.wj1.Pt() > 0.0 && out.wj2.Pt() > 0.0) {
        const TLorentzVector wdijet = out.wj1 + out.wj2;
        const TLorentzVector wdijetShift = out.wj1Shift + out.wj2Shift;
        out.mjjWide = wdijet.M();
        out.dEtaWide = std::fabs(out.wj1.Eta() - out.wj2.Eta());
        out.dPhiWide = std::fabs(out.wj1.DeltaPhi(out.wj2));
        out.mjjWideShift = wdijetShift.M();
      }

      if (out.ak4j1.Pt() > 0.0 && out.ak4j2.Pt() > 0.0) {
        const TLorentzVector dij = out.ak4j1 + out.ak4j2;
        out.mjjAk4 = dij.M();
        out.dEtaAk4 = std::fabs(out.ak4j1.Eta() - out.ak4j2.Eta());
        out.dPhiAk4 = std::fabs(out.ak4j1.DeltaPhi(out.ak4j2));
      }

      return out;
    };

    // Build one jet summary for the saved analysis branches, plus the default and good-muon trigger-study summaries.
    // A jet summary packages the reconstructed AK4, HT, and wide-dijet observables for one jet collection.
    const JetRecoSummary defaultSummary = buildJetRecoSummary(recoJetP4, recoJetP4Shift);
    const JetRecoSummary* analysisSummaryPtr = &defaultSummary;
    JetRecoSummary analysisSummaryStorage;
    if (analysisUsesMuonCleanedJets) {
      analysisSummaryStorage = buildJetRecoSummary(*analysisJetP4, *analysisJetP4Shift);
      analysisSummaryPtr = &analysisSummaryStorage;
    }
    const JetRecoSummary& analysisSummary = *analysisSummaryPtr;
    // Default trigger study histograms
    const JetRecoSummary& defaultSummaryRef = defaultSummary;
    // Good-muon trigger-study histograms
    JetRecoSummary emptyGoodMuonSummary;
    JetRecoSummary goodMuonSummaryStorage;
    const JetRecoSummary* goodMuonSummaryPtr = &emptyGoodMuonSummary;
    if (trigEffGoodMuonJetP4 != nullptr) {
      if (analysisUsesMuonCleanedJets) {
        goodMuonSummaryPtr = analysisSummaryPtr;
      } else {
        goodMuonSummaryStorage = buildJetRecoSummary(*trigEffGoodMuonJetP4, *trigEffGoodMuonJetP4Shift);
        goodMuonSummaryPtr = &goodMuonSummaryStorage;
      }
    }
    const JetRecoSummary& goodMuonSummary = *goodMuonSummaryPtr;

    // Derive MET-based event variables from the nominal corrected jet collection used for the saved branches.
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
                                     ? static_cast<float>((sumEtVal - analysisSummary.htAk4RawForUnclustered) / sumEtVal)
                                     : -1.0f;
    if (unclusteredEnFracVal > 1.0f) unclusteredEnFracVal = 1.0f;
    if (unclusteredEnFracVal < -1.0f) unclusteredEnFracVal = -1.0f;

    // Map the saved jet-composition outputs back to the branch family used for this event.
    auto getChMultOut = [&](size_t idx) -> int {
      return useStandardLikeJets ? static_cast<int>((*chHadMultMC)[idx]) : static_cast<int>((*chHadMultData)[idx]);
    };
    auto getNeMultOut = [&](size_t idx) -> int {
      return useStandardLikeJets ? static_cast<int>((*neHadMultMC)[idx]) : static_cast<int>((*neHadMultData)[idx]);
    };
    auto getPhoMultOut = [&](size_t idx) -> int {
      return useStandardLikeJets ? 0 : static_cast<int>((*phoMultData)[idx]);
    };
    auto getPNetBOut = [&](size_t idx) -> double {
      return useStandardLikeJets ? -1.0 : static_cast<double>((*jetPNetBData)[idx]);
    };
    auto getPNetBBOut = [&](size_t idx) -> double {
      return useStandardLikeJets ? -1.0 : static_cast<double>((*jetPNetBBData)[idx]);
    };
    auto getPNetCOut = [&](size_t idx) -> double {
      return useStandardLikeJets ? -1.0 : static_cast<double>((*jetPNetCData)[idx]);
    };
    auto getPNetCCOut = [&](size_t idx) -> double {
      return useStandardLikeJets ? -1.0 : static_cast<double>((*jetPNetCCData)[idx]);
    };
    auto getPNetGOut = [&](size_t idx) -> double {
      return useStandardLikeJets ? -1.0 : static_cast<double>((*jetPNetGData)[idx]);
    };
    auto getPNetUDSOut = [&](size_t idx) -> double {
      return useStandardLikeJets ? -1.0 : static_cast<double>((*jetPNetUDSData)[idx]);
    };
    auto getPNetUndefOut = [&](size_t idx) -> double {
      return useStandardLikeJets ? -1.0 : static_cast<double>((*jetPNetUndefData)[idx]);
    };

    resetCuts();

    // Fill the reduced ntuple from the nominal analysis summary only; trigger-study variants stay in histograms.
    fillVariableWithValue("run", runNoEvt);
    fillVariableWithValue("event", static_cast<double>(evtNoEvt));
    fillVariableWithValue("lumi", lumiEvt);
    fillVariableWithValue("nVtx", nvtxEvt);

    const auto& branchSortedIdx = analysisSummary.sortedIdx;
    fillVariableWithValue("nJet", (analysisSummary.wj1.Pt() > 0.0 && analysisSummary.wj2.Pt() > 0.0) ? 2 : 0);
    fillVariableWithValue("met", metVal);
    fillVariableWithValue("metphi", metPhiVal);
    fillVariableWithValue("metSig", metSigVal);
    fillVariableWithValue("NAK4PF", analysisSummary.nAk4Analysis);
    fillVariableWithValue("PassJSON", passJSON(runNoEvt, lumiEvt, isDataEvt));

    if (analysisSummary.ak4j1.Pt() > 0.0 && branchSortedIdx.size() >= 1U) {
      const auto j0 = branchSortedIdx[0];
      fillVariableWithValue("IdTight_j1", jetID[j0]);
      fillVariableWithValue("pTAK4PF_j1", analysisSummary.ak4j1.Pt());
      fillVariableWithValue("etaAK4PF_j1", analysisSummary.ak4j1.Eta());
      fillVariableWithValue("phiAK4PF_j1", analysisSummary.ak4j1.Phi());
      fillVariableWithValue("jetJecAK4PF_j1", doReapplyJEC ? jecFactor[j0] : jecFacInput[j0]);
      fillVariableWithValue("jetJecUncAK4PF_j1", doReapplyJEC ? jecUncRel[j0] : jecUncInput[j0]);

      fillVariableWithValue("neutrHadEnFrac_j1", nhf[j0]);
      fillVariableWithValue("chargedHadEnFrac_j1", chf[j0]);
      fillVariableWithValue("photonEnFrac_j1", phf[j0]);
      fillVariableWithValue("muEnFract_j1", muf[j0]);
      fillVariableWithValue("neutrElectromFrac_j1", nemf[j0]);
      fillVariableWithValue("chargedMult_j1", getChMultOut(j0));
      fillVariableWithValue("neutrMult_j1", getNeMultOut(j0));
      fillVariableWithValue("photonMult_j1", getPhoMultOut(j0));
      fillVariableWithValue("particleNet_prob_b_j1", getPNetBOut(j0));
      fillVariableWithValue("particleNet_prob_bb_j1", getPNetBBOut(j0));
      fillVariableWithValue("particleNet_prob_c_j1", getPNetCOut(j0));
      fillVariableWithValue("particleNet_prob_cc_j1", getPNetCCOut(j0));
      fillVariableWithValue("particleNet_prob_g_j1", getPNetGOut(j0));
      fillVariableWithValue("particleNet_prob_uds_j1", getPNetUDSOut(j0));
      fillVariableWithValue("particleNet_prob_undef_j1", getPNetUndefOut(j0));
    }

    if (analysisSummary.ak4j2.Pt() > 0.0 && branchSortedIdx.size() >= 2U) {
      const auto j1 = branchSortedIdx[1];
      fillVariableWithValue("IdTight_j2", jetID[j1]);
      fillVariableWithValue("pTAK4PF_j2", analysisSummary.ak4j2.Pt());
      fillVariableWithValue("etaAK4PF_j2", analysisSummary.ak4j2.Eta());
      fillVariableWithValue("phiAK4PF_j2", analysisSummary.ak4j2.Phi());
      fillVariableWithValue("jetJecAK4PF_j2", doReapplyJEC ? jecFactor[j1] : jecFacInput[j1]);
      fillVariableWithValue("jetJecUncAK4PF_j2", doReapplyJEC ? jecUncRel[j1] : jecUncInput[j1]);

      fillVariableWithValue("neutrHadEnFrac_j2", nhf[j1]);
      fillVariableWithValue("chargedHadEnFrac_j2", chf[j1]);
      fillVariableWithValue("photonEnFrac_j2", phf[j1]);
      fillVariableWithValue("muEnFract_j2", muf[j1]);
      fillVariableWithValue("neutrElectromFrac_j2", nemf[j1]);
      fillVariableWithValue("chargedMult_j2", getChMultOut(j1));
      fillVariableWithValue("neutrMult_j2", getNeMultOut(j1));
      fillVariableWithValue("photonMult_j2", getPhoMultOut(j1));
      fillVariableWithValue("particleNet_prob_b_j2", getPNetBOut(j1));
      fillVariableWithValue("particleNet_prob_bb_j2", getPNetBBOut(j1));
      fillVariableWithValue("particleNet_prob_c_j2", getPNetCOut(j1));
      fillVariableWithValue("particleNet_prob_cc_j2", getPNetCCOut(j1));
      fillVariableWithValue("particleNet_prob_g_j2", getPNetGOut(j1));
      fillVariableWithValue("particleNet_prob_uds_j2", getPNetUDSOut(j1));
      fillVariableWithValue("particleNet_prob_undef_j2", getPNetUndefOut(j1));

      fillVariableWithValue("Dijet_MassAK4PF", analysisSummary.mjjAk4);
      fillVariableWithValue("CosThetaStarAK4PF",
                            TMath::TanH((analysisSummary.ak4j1.Eta() - analysisSummary.ak4j2.Eta()) / 2.0));
      fillVariableWithValue("deltaETAjjAK4PF", analysisSummary.dEtaAk4);
      fillVariableWithValue("deltaPHIjjAK4PF", analysisSummary.dPhiAk4);
    }

    if (analysisSummary.wj1.Pt() > 0.0) {
      fillVariableWithValue("pTWJ_j1", analysisSummary.wj1.Pt());
      fillVariableWithValue("etaWJ_j1", analysisSummary.wj1.Eta());
      fillVariableWithValue("massWJ_j1", analysisSummary.wj1.M());
      fillVariableWithValue("phiWJ_j1", analysisSummary.wj1.Phi());
      fillVariableWithValue("rapidityWJ_j1", analysisSummary.wj1.Rapidity());
    }

    if (analysisSummary.wj2.Pt() > 0.0) {
      fillVariableWithValue("pTWJ_j2", analysisSummary.wj2.Pt());
      fillVariableWithValue("etaWJ_j2", analysisSummary.wj2.Eta());
      fillVariableWithValue("deltaETAjj", analysisSummary.dEtaWide);
      fillVariableWithValue("mjj", analysisSummary.mjjWide);
      fillVariableWithValue("mjj_shiftJEC", analysisSummary.mjjWideShift);
      fillVariableWithValue("massWJ_j2", analysisSummary.wj2.M());
      fillVariableWithValue("phiWJ_j2", analysisSummary.wj2.Phi());
      fillVariableWithValue("CosThetaStarWJ",
                            TMath::TanH((analysisSummary.wj1.Eta() - analysisSummary.wj2.Eta()) / 2.0));
      fillVariableWithValue("deltaPHIjj", analysisSummary.dPhiWide);
      fillVariableWithValue("rapidityWJ_j2", analysisSummary.wj2.Rapidity());
    }

    const double METoverHTAK4PF =
        (analysisSummary.htAk4Analysis > 0.0) ? double(metVal / analysisSummary.htAk4Analysis) : 0.0;
    fillVariableWithValue("HTAK4PF", analysisSummary.htAk4Analysis);
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
    // Evaluate the optional JetHT-side L1 flag once so both the ntuple and trigger-study histograms can reuse it.
    bool passRequiredL1 = true;
    bool passVetoL1 = true;
    if (canEvaluateL1ForPFScoutingHT) {
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
    const bool passL1ForPFScoutingHT = canEvaluateL1ForPFScoutingHT ? (passRequiredL1 && passVetoL1) : false;
    fillVariableWithValue("passHLT_PFScoutingHT", passPFScoutingHT ? 1 : 0);
    fillVariableWithValue("passHLT_PFScouting_SingleMuon", passPFScoutingSingleMuon ? 1 : 0);
    fillVariableWithValue("passL1ForPFScoutingHT",
                          canEvaluateL1ForPFScoutingHT ? (passL1ForPFScoutingHT ? 1 : 0) : -1);

    evaluateCuts();

    // Apply the trigger-study selections on top of the shared JSON and vertex requirements.
    const bool passTriggerStudyBaseSelection = passedCut("PassJSON") && passedCut("nVtx");
    auto passesWideDijetTriggerStudy = [&](const JetRecoSummary& summary) {
      if (!passTriggerStudyBaseSelection || summary.sortedIdx.size() < 2U) return false;
      const auto j0 = summary.sortedIdx[0];
      const auto j1 = summary.sortedIdx[1];
      return (jetID[j0] == tightJetIDFlag) &&
             (jetID[j1] == tightJetIDFlag) &&
             (summary.wj1.Pt() > pTWJ1Min) &&
             (summary.wj2.Pt() > pTWJ2Min) &&
             (summary.wj1.Eta() > etaWJ1Min) &&
             (summary.wj1.Eta() < etaWJ1Max) &&
             (summary.wj2.Eta() > etaWJ2Min) &&
             (summary.wj2.Eta() < etaWJ2Max) &&
             (summary.dEtaWide < detaWJJMax);
    };
    auto passesAk4TriggerStudy = [&](const JetRecoSummary& summary) {
      if (!passTriggerStudyBaseSelection || summary.sortedIdx.size() < 2U) return false;
      const auto j0 = summary.sortedIdx[0];
      const auto j1 = summary.sortedIdx[1];
      return (jetID[j0] == tightJetIDFlag) &&
             (jetID[j1] == tightJetIDFlag) &&
             (summary.ak4j1.Pt() > pTWJ1Min) &&
             (summary.ak4j2.Pt() > pTWJ2Min) &&
             (summary.ak4j1.Eta() > etaWJ1Min) &&
             (summary.ak4j1.Eta() < etaWJ1Max) &&
             (summary.ak4j2.Eta() > etaWJ2Min) &&
             (summary.ak4j2.Eta() < etaWJ2Max) &&
             (summary.dEtaAk4 < detaWJJMax);
    };
    // Fill each trigger-study variant without changing the nominal analysis branches.
    auto fillTriggerStudy = [&](TriggerStudyMethod method,
                                const JetRecoSummary& summary,
                                bool requireGoodMuon,
                                bool requireL1ForJetHT) {
      if (!h_trigStudy[kWideDijet][method][0]) return;
      if (requireGoodMuon && !hasGoodScoutingMuon) return;
      if (requireL1ForJetHT && !canEvaluateL1ForPFScoutingHT) return;

      if (passesWideDijetTriggerStudy(summary)) {
        h_trigStudy[kWideDijet][method][kNoTrig]->Fill(summary.mjjWide);
        if (h_trigStudyEven1GeV[kWideDijet][method][kNoTrig]) {
          h_trigStudyEven1GeV[kWideDijet][method][kNoTrig]->Fill(summary.mjjWide);
        }
        if (passPFScoutingHT && (!requireL1ForJetHT || passL1ForPFScoutingHT)) {
          h_trigStudy[kWideDijet][method][kJetHTTrig]->Fill(summary.mjjWide);
          if (h_trigStudyEven1GeV[kWideDijet][method][kJetHTTrig]) {
            h_trigStudyEven1GeV[kWideDijet][method][kJetHTTrig]->Fill(summary.mjjWide);
          }
        }
        if (passPFScoutingSingleMuon) h_trigStudy[kWideDijet][method][kSingleMuonTrig]->Fill(summary.mjjWide);
        if (passPFScoutingSingleMuon && h_trigStudyEven1GeV[kWideDijet][method][kSingleMuonTrig]) {
          h_trigStudyEven1GeV[kWideDijet][method][kSingleMuonTrig]->Fill(summary.mjjWide);
        }
        if (passPFScoutingSingleMuon && passPFScoutingHT &&
            (!requireL1ForJetHT || passL1ForPFScoutingHT)) {
          h_trigStudy[kWideDijet][method][kJetHTAndSingleMuonTrig]->Fill(summary.mjjWide);
          if (h_trigStudyEven1GeV[kWideDijet][method][kJetHTAndSingleMuonTrig]) {
            h_trigStudyEven1GeV[kWideDijet][method][kJetHTAndSingleMuonTrig]->Fill(summary.mjjWide);
          }
        }
      }

      if (passesAk4TriggerStudy(summary)) {
        h_trigStudy[kAk4Dijet][method][kNoTrig]->Fill(summary.mjjAk4);
        if (h_trigStudyEven1GeV[kAk4Dijet][method][kNoTrig]) {
          h_trigStudyEven1GeV[kAk4Dijet][method][kNoTrig]->Fill(summary.mjjAk4);
        }
        if (passPFScoutingHT && (!requireL1ForJetHT || passL1ForPFScoutingHT)) {
          h_trigStudy[kAk4Dijet][method][kJetHTTrig]->Fill(summary.mjjAk4);
          if (h_trigStudyEven1GeV[kAk4Dijet][method][kJetHTTrig]) {
            h_trigStudyEven1GeV[kAk4Dijet][method][kJetHTTrig]->Fill(summary.mjjAk4);
          }
        }
        if (passPFScoutingSingleMuon) h_trigStudy[kAk4Dijet][method][kSingleMuonTrig]->Fill(summary.mjjAk4);
        if (passPFScoutingSingleMuon && h_trigStudyEven1GeV[kAk4Dijet][method][kSingleMuonTrig]) {
          h_trigStudyEven1GeV[kAk4Dijet][method][kSingleMuonTrig]->Fill(summary.mjjAk4);
        }
        if (passPFScoutingSingleMuon && passPFScoutingHT &&
            (!requireL1ForJetHT || passL1ForPFScoutingHT)) {
          h_trigStudy[kAk4Dijet][method][kJetHTAndSingleMuonTrig]->Fill(summary.mjjAk4);
          if (h_trigStudyEven1GeV[kAk4Dijet][method][kJetHTAndSingleMuonTrig]) {
            h_trigStudyEven1GeV[kAk4Dijet][method][kJetHTAndSingleMuonTrig]->Fill(summary.mjjAk4);
          }
        }
      }

      if (passTriggerStudyBaseSelection) {
        h_trigStudy[kHtAk4][method][kNoTrig]->Fill(summary.htAk4TrigEff);
        if (passPFScoutingHT && (!requireL1ForJetHT || passL1ForPFScoutingHT)) {
          h_trigStudy[kHtAk4][method][kJetHTTrig]->Fill(summary.htAk4TrigEff);
        }
        if (passPFScoutingSingleMuon) h_trigStudy[kHtAk4][method][kSingleMuonTrig]->Fill(summary.htAk4TrigEff);
        if (passPFScoutingSingleMuon && passPFScoutingHT &&
            (!requireL1ForJetHT || passL1ForPFScoutingHT)) {
          h_trigStudy[kHtAk4][method][kJetHTAndSingleMuonTrig]->Fill(summary.htAk4TrigEff);
        }
      }
    };

    // Nominal (Legacy) wide-dijet histograms
    const bool legacyWideSelection = passesWideDijetTriggerStudy(defaultSummaryRef);
    if (legacyWideSelection) {
      h_mjj_HLTpass[kNoTrig]->Fill(defaultSummaryRef.mjjWide);
      h_mjj_noTrig_1GeVbin->Fill(defaultSummaryRef.mjjWide);
      if (passPFScoutingHT) h_mjj_HLTpass[kJetHTTrig]->Fill(defaultSummaryRef.mjjWide);
      if (passPFScoutingSingleMuon) h_mjj_HLTpass[kSingleMuonTrig]->Fill(defaultSummaryRef.mjjWide);
      if (passPFScoutingSingleMuon && passPFScoutingHT) {
        h_mjj_HLTpass[kJetHTAndSingleMuonTrig]->Fill(defaultSummaryRef.mjjWide);
      }
    }

    fillTriggerStudy(kDefaultStudy, defaultSummaryRef, false, false);
    if (produceAdditionalTriggerStudyPlots) {
      fillTriggerStudy(kL1Study, defaultSummaryRef, false, true);
      fillTriggerStudy(kGoodMuonStudy, goodMuonSummary, true, false);
      fillTriggerStudy(kGoodMuonL1Study, goodMuonSummary, true, true);
    }

    fillReducedSkimTree();
  }

  for (int i = 0; i < kNTriggerStudyTriggers; ++i) {
    h_mjj_HLTpass[i]->Write();
  }
  h_mjj_noTrig_1GeVbin->Write();
  // Write the full trigger-study histogram grid after the event loop so downstream scripts can choose the method later.
  for (int obs = 0; obs < kNTriggerStudyObservables; ++obs) {
    for (int method = 0; method < kNTriggerStudyMethods; ++method) {
      for (int trig = 0; trig < kNTriggerStudyTriggers; ++trig) {
        if (!h_trigStudy[obs][method][trig]) continue;
        h_trigStudy[obs][method][trig]->Write();
      }
    }
  }
  for (int obs = 0; obs < kNTriggerStudyObservables; ++obs) {
    for (int method = 0; method < kNTriggerStudyMethods; ++method) {
      for (int trig = 0; trig < kNTriggerStudyTriggers; ++trig) {
        if (!h_trigStudyEven1GeV[obs][method][trig]) continue;
        h_trigStudyEven1GeV[obs][method][trig]->Write();
      }
    }
  }
  std::cout << "[analysisClass] Loop end\n";
}

