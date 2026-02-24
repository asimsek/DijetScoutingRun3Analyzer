#define analysisClass_cxx
#include "analysisClass.h"
#include "JECTool.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <vector>
#include <TH1F.h>
#include <TMath.h>
#include <TLorentzVector.h>
#include <TStyle.h>

using std::size_t;

namespace {
std::string formatWithCommas(Long64_t value) {
  const bool negative = value < 0;
  const unsigned long long absValue =
      negative ? (static_cast<unsigned long long>(-(value + 1)) + 1ULL)
               : static_cast<unsigned long long>(value);
  std::string out = std::to_string(absValue);
  for (int i = static_cast<int>(out.size()) - 3; i > 0; i -= 3) {
    out.insert(static_cast<size_t>(i), ",");
  }
  if (negative) out.insert(out.begin(), '-');
  return out;
}
}  // namespace


// Choose era block: leave empty ("") to take the FIRST block in the list file.
// If you want a specific era (e.g. "2024H"), set it here:
const std::string era = "2024H";

// The external lists (data vs mc)
const std::string dataList = "data/cfg/data_jec_list.txt";
const std::string mcList   = "data/cfg/mc_jec_list.txt";

analysisClass::analysisClass(std::string* inputList,
                             std::string* cutFile,
                             std::string* treeName,
                             std::string* outputFileName,
                             std::string* cutEfficFile)
    : baseClass(inputList, cutFile, treeName, outputFileName, cutEfficFile) {
  std::cout << "[analysisClass] Constructor (ctor) begin\n";

  // JEC reapplication via JECTool (reads external lists)
  if (int(getPreCutValue1("useJECs")) == 1) {
    std::cout << "[analysisClass] Reapplying JECs via JECTool...\n";

    // Resolve paths from the lists (no run-based selection here; uses default)
    auto Rmc   = JECTool::resolveList(mcList,   era /*, runNo */);
    auto Rdata = JECTool::resolveList(dataList, era /*, runNo */);

    // Build correctors (MC set only L1/L2/L3; Data adds L2L3Residual if present)
    auto mcCorr   = JECTool::makeCorrector(Rmc,   /*forData=*/false);
    auto dataCorr = JECTool::makeCorrector(Rdata, /*forData=*/true);
    auto jecUnc   = JECTool::makeUncertainty(Rmc);

    // JEC pointers (used later in Loop)
    JetCorrector      = mcCorr.release();
    JetCorrector_data = dataCorr.release();
    unc               = jecUnc.release();

    if (!JetCorrector || !JetCorrector_data || !unc) {
      std::cerr << "[analysisClass] ERROR: JEC initialization failed. Check list files.\n";
    }
  }

  std::cout << "[analysisClass] Constructor (ctor) end\n";
}


analysisClass::~analysisClass() {
  std::cout << "[analysisClass] Destructor (dtor)\n";
}


void analysisClass::Loop() {
  std::cout << "[analysisClass] Loop begin\n";
  if (fChain == nullptr) return;

  // Variable-binned mjj edges
  static const int nMassBins = 103;
  static const double massBoundaries[nMassBins + 1] = {
      1, 3, 6, 10, 16, 23, 31, 40, 50, 61, 74, 88, 103, 119, 137, 156, 176, 197, 220, 244, 270, 296, 325,
      354, 386, 419, 453, 489, 526, 565, 606, 649, 693, 740, 788, 838, 890, 944, 1000, 1058, 1118, 1181, 1246, 1313, 1383, 1455, 1530, 1607, 1687,
      1770, 1856, 1945, 2037, 2132, 2231, 2332, 2438, 2546, 2659, 2775, 2895, 3019, 3147, 3279, 3416, 3558, 3704, 3854, 4010, 4171, 4337, 4509,
      4686, 4869, 5058, 5253, 5455, 5663, 5877, 6099, 6328, 6564, 6808, 7060, 7320, 7589, 7866, 8152, 8447, 8752, 9067, 9391, 9726, 10072, 10430,
      10798, 11179, 11571, 11977, 12395, 12827, 13272, 13732, 14000};

  // HLT
  const char* HLTname[20] = {"noTrig", "PFScouting_JetHT", "PFScouting_SingleMuon", 
                            "Mu50", "Mu55", "IsoMu20", "IsoMu24", "IsoMu27", 
                            "PFJet40", "PFJet60", "PFJet80", "PFJet140", "PFJet200", "PFJet260", 
                            "PFJet320", "PFJet400", "PFJet450", "PFJet500", "PFJet550", "AllJet"
                          };

  TH1F* h_mjj_HLTpass[20];
  TH1F* h_mjj_noTrig_1GeVbin = new TH1F("h_mjj_noTrig_1GeVbin", "", 14000, 0, 14000);
  for (int i = 0; i < 20; ++i) {
    h_mjj_HLTpass[i]    = new TH1F(Form("h_mjj_HLTpass_%s", HLTname[i]), "", nMassBins, massBoundaries);
  }

  // Cache cut values (avoids repeated string lookups)
  const double jetFidRegion   = getPreCutValue1("jetFidRegion");
  const int    tightJetIDFlag = int(getPreCutValue1("tightJetID"));
  const double ptCut          = getPreCutValue1("ptCut");
  const double pt0Cut         = getPreCutValue1("pt0Cut");
  const double pt1Cut         = getPreCutValue1("pt1Cut");
  const double wideJetDeltaR  = getPreCutValue1("DeltaR");
  const bool   reapplyJEC     = (int(getPreCutValue1("useJECs")) == 1);
  const bool   useRawOnly     = (int(getPreCutValue1("noJECs")) == 1);
  const bool   shiftJECs      = (int(getPreCutValue1("shiftJECs")) == 1);
  const double shiftSign      = getPreCutValue2("shiftJECs");


  // --- L2L3Residual run-range aware corrector (DATA only) ---
  // If your data list uses bracketed residuals like:
  // L2L3Residual: [ -1:-1:path_default.txt, 382298:383247:path_H1.txt, 383247:-1:path_H2.txt ]
  // this block rebuilds the data corrector whenever 'runNo' changes.
  if (reapplyJEC) {
    static long jecResidualRunCached = -1;
    if ((isData != 0) && jecResidualRunCached != (long)runNo) {

      auto Rdata_run    = JECTool::resolveList(dataList, era, (long)runNo);
      auto dataCorr_run = JECTool::makeCorrector(Rdata_run, /*forData=*/true);

      if (dataCorr_run) {
        // swap in the new data corrector (with the right L2L3Residual for this run)
        delete JetCorrector_data;
        JetCorrector_data = dataCorr_run.release();
        jecResidualRunCached = (long)runNo;

        std::cout << "[analysisClass] Updated Data JEC corrector for run "
                  << runNo << " (Residual: " << Rdata_run.L2L3Residual << ")\n";
      } else {
        std::cerr << "[analysisClass] WARNING: failed to build data JEC corrector for run "
                  << runNo << " — continuing with previous one.\n";
      }
    }
  }

  const Long64_t nentries = fChain->GetEntriesFast();
  std::cout << "[analysisClass] entries = " << formatWithCommas(nentries) << "\n";

  // Reusable containers
  std::vector<double> jecFactor;
  std::vector<double> jecUncRel;
  std::vector<unsigned> sortedIdx;
  std::vector<int> jetID;

  jecFactor.reserve(512);
  jecUncRel.reserve(512);
  sortedIdx.reserve(512);
  jetID.reserve(512);

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry = 0; jentry < nentries; ++jentry) {
    const Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry); nbytes += nb;
    if (jentry < 10 || jentry % 100000 == 0)
      std::cout << "[analysisClass] jentry " << formatWithCommas(jentry) << "\n";

    // --- Short aliases ---
    const bool   isDataEvt  = (isData != 0);         // branch: isData_/I
    const float  rhoVal     = rho;                   // branch: rho_/F

    const auto& pt      = *jetPtAK4;                 // corrected pT in scouting
    const auto& rawPt   = *jetRawPtAK4;
    const auto& eta     = *jetEtaAK4;
    const auto& phi     = *jetPhiAK4;
    const auto& mass    = *jetMassAK4;
    const auto& area    = *jetAreaAK4;

    const auto& chf     = *jetChfAK4;
    const auto& nhf     = *jetNhfAK4;
    const auto& phf     = *jetPhfAK4;
    const auto& muf     = *jetMufAK4;
    const auto& elf     = *jetElfAK4;
    const auto& cemf    = *jetChEmFAK4;             // charged EM fraction
    const auto& nemf    = *jetNeEmFAK4;             // neutral EM fraction

    const auto& chMult  = *chHadMultAK4;
    const auto& neMult  = *neHadMultAK4;
    const auto& phoMult = *phoMultAK4;

    const auto& jecFac  = *jetJECFactorAK4;
    const auto& jecUnc  = *jetJECUncRelAK4;         // relative unc (fraction)

    const size_t nJets = pt.size();

    // --- Pre-size & clear reusable containers ---
    jecFactor.assign(nJets, 1.0);
    jecUncRel.assign(nJets, 0.0);
    sortedIdx.resize(nJets);
    std::iota(sortedIdx.begin(), sortedIdx.end(), 0);
    jetID.assign(nJets, 0);

    // --- Build JetID ---
    for (size_t i = 0; i < nJets; ++i) {
      const int numConstVal = chMult[i] + neMult[i];
      const bool passID = (std::abs(eta[i]) < 2.6) &&
                          (numConstVal > 1) &&
                          (nemf[i] < 0.90) &&
                          (muf[i] < 0.80) &&
                          (nhf[i] < 0.99);

      jetID[i] = passID ? 1 : 0;

      if (reapplyJEC) {
        // Recompute JECs on the fly.
        JetCorrector->setRho(rhoVal);
        JetCorrector->setJetEta(eta[i]);
        JetCorrector->setJetPhi(phi[i]);
        JetCorrector->setJetA(area[i]);
        JetCorrector->setJetPt(rawPt[i]);

        JetCorrector_data->setRho(rhoVal);
        JetCorrector_data->setJetEta(eta[i]);
        JetCorrector_data->setJetPhi(phi[i]);
        JetCorrector_data->setJetA(area[i]);
        JetCorrector_data->setJetPt(rawPt[i]);

        double corr = isDataEvt ? JetCorrector_data->getCorrection()
                                : JetCorrector->getCorrection();

        unc->setJetEta(eta[i]);
        unc->setJetPt(rawPt[i] * corr);
        double u = unc->getUncertainty(true);

        jecFactor[i] = corr;
        jecUncRel[i] = u;
      } else if (useRawOnly) {
        // Use *raw* kinematics only
        jecFactor[i] = 1.0;   // treat pt = rawPt[i]
        jecUncRel[i] = 0.0;
      } else {
        // Use *scouting-provided* corrected kinematics
        jecFactor[i] = jecFac[i];
        jecUncRel[i] = jecUnc[i];
      }
    }

    // --- Jet ordering by corrected pT ---
    auto getCorrPt = [&](size_t i) -> double {
      if (reapplyJEC)   return rawPt[i] * jecFactor[i];
      if (useRawOnly)   return rawPt[i];
      /* default */     return pt[i]; // Don't re-apply/apply JECs
    };
    std::sort(sortedIdx.begin(), sortedIdx.end(),
              [&](unsigned a, unsigned b) { return getCorrPt(a) > getCorrPt(b); });

    // --- Count AKPF Jets & calculate HT passing 'JetID, fiducial, pT' cuts ---
    int   NAK4PF = 0;
    double HTAK4PF = 0.0;
    for (size_t k = 0; k < nJets; ++k) {
      const auto j = sortedIdx[k];
      if (std::fabs(eta[j]) >= jetFidRegion) continue; // Jet Fiducial Region (|eta|<2.5)
      if (jetID[j] != tightJetIDFlag)        continue; // JetID criteria

      const double pTj = getCorrPt(j);
      if (pTj <= ptCut) continue;                      // minPt cut

      ++NAK4PF;
      HTAK4PF += pTj;
    }

    // === Build wide jets ===
    TLorentzVector wj1, wj2, wj1_shift, wj2_shift;
    TLorentzVector AK4j1, AK4j2;  // leading two "narrow" jets for AK4 dijet

    if (nJets >= 2) {
      const auto j0 = sortedIdx[0];
      const auto j1 = sortedIdx[1];

      const auto acceptLeading = [&](size_t j, double pTcut) {
        return (std::fabs(eta[j]) < jetFidRegion) &&
               (jetID[j] == tightJetIDFlag) &&
               (getCorrPt(j) > pTcut);
      };

      if (acceptLeading(j0, pt0Cut) && acceptLeading(j1, pt1Cut)) {
        // Seed narrow jets
        const double s0 = reapplyJEC ? (rawPt[j0] * jecFactor[j0]) :
                         (useRawOnly ? rawPt[j0] : pt[j0]);
        const double s1 = reapplyJEC ? (rawPt[j1] * jecFactor[j1]) :
                         (useRawOnly ? rawPt[j1] : pt[j1]);

        AK4j1.SetPtEtaPhiM(s0, eta[j0], phi[j0],
                           reapplyJEC ? (mass[j0] * (jecFactor[j0]/jecFac[j0]))
                                      : (useRawOnly ? (mass[j0]/jecFac[j0]) : mass[j0]));
        AK4j2.SetPtEtaPhiM(s1, eta[j1], phi[j1],
                           reapplyJEC ? (mass[j1] * (jecFactor[j1]/jecFac[j1]))
                                      : (useRawOnly ? (mass[j1]/jecFac[j1]) : mass[j1]));

        // Manual wide-jet clustering around the two leading seeds
        TLorentzVector wj1_tmp = AK4j1;
        TLorentzVector wj2_tmp = AK4j2;
        TLorentzVector wj1s_tmp = AK4j1; // shifted by JEC unc (if used)
        TLorentzVector wj2s_tmp = AK4j2;

        for (size_t k = 0; k < nJets; ++k) {
          const auto j = sortedIdx[k];
          if (std::fabs(eta[j]) >= jetFidRegion) continue; // Jet Fiducial Region (|eta|<2.5)
          if (jetID[j] != tightJetIDFlag)        continue; // JetID criteria

          const double pTj = getCorrPt(j);                 // minPt cut
          if (pTj <= ptCut) continue;

          TLorentzVector cj;
          cj.SetPtEtaPhiM(pTj, eta[j], phi[j],
                          reapplyJEC ? (mass[j] * (jecFactor[j]/jecFac[j]))
                                     : (useRawOnly ? (mass[j]/jecFac[j]) : mass[j]));

          const double dR1 = cj.DeltaR(AK4j1);
          const double dR2 = cj.DeltaR(AK4j2);

          if (dR1 < dR2 && dR1 < wideJetDeltaR) {
            wj1_tmp += cj;
            if (reapplyJEC) {
              const double pTshift = pTj * (1.0 + (shiftSign * jecUncRel[j]));
              TLorentzVector cjs; cjs.SetPtEtaPhiM(pTshift, eta[j], phi[j],
                                                   cj.M() * (1.0 + (shiftSign * jecUncRel[j])));
              wj1s_tmp += cjs;
            }
          } else if (dR2 < wideJetDeltaR) {
            wj2_tmp += cj;
            if (reapplyJEC) {
              const double pTshift = pTj * (1.0 + (shiftSign * jecUncRel[j]));
              TLorentzVector cjs; cjs.SetPtEtaPhiM(pTshift, eta[j], phi[j],
                                                   cj.M() * (1.0 + (shiftSign * jecUncRel[j])));
              wj2s_tmp += cjs;
            }
          }
        }

        // Order by pT
        if (wj2_tmp.Pt() > wj1_tmp.Pt()) {
          std::swap(wj1_tmp, wj2_tmp);
          std::swap(wj1s_tmp, wj2s_tmp);
        }
        wj1 = wj1_tmp; wj2 = wj2_tmp;
        wj1_shift = wj1s_tmp; wj2_shift = wj2s_tmp;
      }
    }

    // === Dijet from wide jets ===
    double mjjWide = 0.0, dEtaWide = 0.0, dPhiWide = 0.0, mjjWide_shift = 0.0;
    if (wj1.Pt() > 0.0 && wj2.Pt() > 0.0) {
      const TLorentzVector wdijet      = wj1 + wj2;
      const TLorentzVector wdijetShift = wj1_shift + wj2_shift;

      mjjWide       = wdijet.M();
      dEtaWide      = std::fabs(wj1.Eta() - wj2.Eta());
      dPhiWide      = std::fabs(wj1.DeltaPhi(wj2));
      mjjWide_shift = wdijetShift.M();
    }

    // === AK4 dijet from narrow leading two ===
    double mjjAK4 = 0.0, dEtaAK4 = 0.0, dPhiAK4 = 0.0;
    if (AK4j1.Pt() > 0.0 && AK4j2.Pt() > 0.0) {
      const TLorentzVector dij = AK4j1 + AK4j2;
      mjjAK4  = dij.M();
      dEtaAK4 = std::fabs(AK4j1.Eta() - AK4j2.Eta());
      dPhiAK4 = std::fabs(AK4j1.DeltaPhi(AK4j2));
    }

    // === Fill variables ===
    resetCuts();

    fillVariableWithValue("run"                       ,runNo                                     );
    fillVariableWithValue("event"                     ,evtNo                                     );
    fillVariableWithValue("lumi"                      ,lumi                                      );
    fillVariableWithValue("nVtx"                      ,nvtx                                      );

    fillVariableWithValue("nJet"                      ,(wj1.Pt() > 0.0 && wj2.Pt() > 0.0) ? 2 : 0);
    fillVariableWithValue("met"                       ,met                                       );
    fillVariableWithValue("metphi"                    ,metphi                                    );
    fillVariableWithValue("metSig"                    ,metOverSumEt                              );
    fillVariableWithValue("NAK4PF"                    ,NAK4PF                                    );
    fillVariableWithValue("PassJSON"                  ,passJSON(runNo, lumi, isDataEvt)          );

    if (AK4j1.Pt() > 0.0) {
      const auto j0 = sortedIdx[0];
      fillVariableWithValue("IdTight_j1"              ,jetID[j0]                                 );
      fillVariableWithValue("pTAK4PF_j1"              ,AK4j1.Pt()                                );
      fillVariableWithValue("etaAK4PF_j1"             ,AK4j1.Eta()                               );
      fillVariableWithValue("phiAK4PF_j1"             ,AK4j1.Phi()                               );
      fillVariableWithValue("jetJecAK4PF_j1"          ,reapplyJEC ? jecFactor[j0] : jecFac[j0]   );
      fillVariableWithValue("jetJecUncAK4PF_j1"       ,reapplyJEC ? jecUncRel[j0] : jecUnc[j0]   );

      fillVariableWithValue("neutrHadEnFrac_j1"       ,nhf[j0]                                   );
      fillVariableWithValue("chargedHadEnFrac_j1"     ,chf[j0]                                   );
      fillVariableWithValue("photonEnFrac_j1"         ,phf[j0]                                   );
      // fillVariableWithValue("eleEnFract_j1"           ,elf[j0]                                   );
      fillVariableWithValue("muEnFract_j1"            ,muf[j0]                                   );
      fillVariableWithValue("neutrElectromFrac_j1"    ,nemf[j0]                                  );
      // fillVariableWithValue("chargedElectromFrac_j1"  ,cemf[j0]                                  );
      fillVariableWithValue("chargedMult_j1"          ,chMult[j0]                                );
      fillVariableWithValue("neutrMult_j1"            ,neMult[j0]                                );
      fillVariableWithValue("photonMult_j1"           ,phoMult[j0]                               );
    }
    if (AK4j2.Pt() > 0.0) {
      const auto j1 = sortedIdx[1];
      fillVariableWithValue("IdTight_j2"              ,jetID[j1]                                 );
      fillVariableWithValue("pTAK4PF_j2"              ,AK4j2.Pt()                                );
      fillVariableWithValue("etaAK4PF_j2"             ,AK4j2.Eta()                               );
      fillVariableWithValue("phiAK4PF_j2"             ,AK4j2.Phi()                               );
      fillVariableWithValue("jetJecAK4PF_j2"          ,reapplyJEC ? jecFactor[j1] : jecFac[j1]   );
      fillVariableWithValue("jetJecUncAK4PF_j2"       ,reapplyJEC ? jecUncRel[j1] : jecUnc[j1]   );

      fillVariableWithValue("neutrHadEnFrac_j2"       ,nhf[j1]                                   );
      fillVariableWithValue("chargedHadEnFrac_j2"     ,chf[j1]                                   );
      fillVariableWithValue("photonEnFrac_j2"         ,phf[j1]                                   );
      // fillVariableWithValue("eleEnFract_j2"           ,elf[j1]                                   );
      fillVariableWithValue("muEnFract_j2"            ,muf[j1]                                   );
      fillVariableWithValue("neutrElectromFrac_j2"    ,nemf[j1]                                  );
      // fillVariableWithValue("chargedElectromFrac_j2"  ,cemf[j1]                                  );
      fillVariableWithValue("chargedMult_j2"          ,chMult[j1]                                );
      fillVariableWithValue("neutrMult_j2"            ,neMult[j1]                                );
      fillVariableWithValue("photonMult_j2"           ,phoMult[j1]                               );

      // AK4 dijet
      fillVariableWithValue("Dijet_MassAK4PF"         ,mjjAK4                                    );
      fillVariableWithValue("CosThetaStarAK4PF"       ,TMath::TanH((AK4j1.Eta()-AK4j2.Eta())/2.0));
      fillVariableWithValue("deltaETAjjAK4PF"         ,dEtaAK4                                   );
      fillVariableWithValue("deltaPHIjjAK4PF"         ,dPhiAK4                                   );
    }

    if (wj1.Pt() > 0.0) {
      fillVariableWithValue("pTWJ_j1"                 ,wj1.Pt()                                  );
      fillVariableWithValue("etaWJ_j1"                ,wj1.Eta()                                 );
      fillVariableWithValue("massWJ_j1"               ,wj1.M()                                   );
      fillVariableWithValue("phiWJ_j1"                ,wj1.Phi()                                 );
      fillVariableWithValue("rapidityWJ_j1"           ,wj1.Rapidity()                            );
    }
    if (wj2.Pt() > 0.0) {
      fillVariableWithValue("pTWJ_j2"                 ,wj2.Pt()                                  );
      fillVariableWithValue("etaWJ_j2"                ,wj2.Eta()                                 );
      fillVariableWithValue("deltaETAjj"              ,dEtaWide                                  );
      fillVariableWithValue("mjj"                     ,mjjWide                                   );
      fillVariableWithValue("mjj_shiftJEC"            ,mjjWide_shift                             );
      fillVariableWithValue("massWJ_j2"               ,wj2.M()                                   );
      fillVariableWithValue("phiWJ_j2"                ,wj2.Phi()                                 );
      fillVariableWithValue("CosThetaStarWJ"          ,TMath::TanH((wj1.Eta() - wj2.Eta()) / 2.0));
      fillVariableWithValue("deltaPHIjj"              ,dPhiWide                                  );
      fillVariableWithValue("rapidityWJ_j2"           ,wj2.Rapidity()                            );
    }

    // MET & HT
    const double METoverHTAK4PF = (HTAK4PF > 0.0) ? double(met / HTAK4PF) : 0.0;
    fillVariableWithValue("HTAK4PF"                   ,HTAK4PF                                   );
    fillVariableWithValue("METoverHTAK4PF"            ,METoverHTAK4PF                            );
    fillVariableWithValue("unclusteredEnFracAK4PF"    ,unclusteredEnFrac                         );


    // --- minDeltaPhi(MET, wide-jets) among leading two jets.
    // --- Small minDeltaPhi => MET aligned with a hard jet => likely mis-measurement/noise rather than genuine MET.
    auto abs_dphi = [](double a, double b) {
      return std::abs(TVector2::Phi_mpi_pi(a - b));
    };

    double dphi_min2 = 1e9;
    const double met_phi = getVariableValue("metphi");
    std::vector<double> ak4_phi;
    if (variableIsFilled("phiAK4PF_j1")) ak4_phi.push_back(getVariableValue("phiAK4PF_j1"));
    if (variableIsFilled("phiAK4PF_j2")) ak4_phi.push_back(getVariableValue("phiAK4PF_j2"));

    for (size_t i = 0; i < ak4_phi.size() && i < 2; ++i)
      dphi_min2 = std::min(dphi_min2, abs_dphi(met_phi, ak4_phi[i]));

    fillVariableWithValue("minDPhiMetAK4PF", dphi_min2 < 1e8 ? dphi_min2 : -1.0);

    // --- Trigger bookkeeping ---
    bool passPFScoutingHT = false, passPFScouting_SingleMuon = false;
    for (size_t i = 0; i  < triggerName    ->size(); ++i) {
      const auto& name    = triggerName    ->at(i);
      const bool   res    = triggerResult  ->at(i);

      if (name.find("DST_PFScouting_JetHT_v")      == 0)   passPFScoutingHT          |= res;
      if (name.find("DST_PFScouting_SingleMuon_v") == 0)   passPFScouting_SingleMuon |= res;
    }
    fillVariableWithValue("passHLT_PFScoutingHT"          ,passPFScoutingHT ? 1 : 0              );
    fillVariableWithValue("passHLT_PFScouting_SingleMuon" ,passPFScouting_SingleMuon ? 1 : 0     );

    // --- Evaluate (do not cut away) ---
    evaluateCuts();

    const bool fullAnalysis =
        passedCut("PassJSON")   &&
        passedCut("nVtx")       &&
        passedCut("IdTight_j1") &&
        passedCut("IdTight_j2") &&
        (getVariableValue("pTWJ_j1"   )  >  getPreCutValue1("pT_WJ1") ) &&
        (getVariableValue("pTWJ_j2"   )  >  getPreCutValue1("pT_WJ2") ) &&
        (getVariableValue("etaWJ_j1"  )  >  getPreCutValue1("eta_WJ1")) &&
        (getVariableValue("etaWJ_j1"  )  <  getPreCutValue2("eta_WJ1")) &&
        (getVariableValue("etaWJ_j2"  )  >  getPreCutValue1("eta_WJ2")) &&
        (getVariableValue("etaWJ_j2"  )  <  getPreCutValue2("eta_WJ2")) &&
        (getVariableValue("deltaETAjj")  <  getPreCutValue1("Deta_WJJ"));

    if (fullAnalysis) {
      h_mjj_HLTpass[0]->Fill(mjjWide); // noTrig
      h_mjj_noTrig_1GeVbin->Fill(mjjWide); // noTrig -> '1 GeV Binning'
      if (passPFScoutingHT) h_mjj_HLTpass[1]->Fill(mjjWide);
      if (passPFScouting_SingleMuon) h_mjj_HLTpass[2]->Fill(mjjWide);
    }

    // Fill Tree
    fillReducedSkimTree();
  }

  // Write histograms
  for (int i = 0; i < 20; ++i) {
    h_mjj_HLTpass[i]->Write();
  }
  std::cout << "[analysisClass] Loop end\n";
}

