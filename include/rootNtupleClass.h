//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Feb 25 19:53:49 2026 by ROOT version 6.32.13
// from TChain Events/
//////////////////////////////////////////////////////////

#ifndef rootNtupleClass_h
#define rootNtupleClass_h

//// Lines added by make_rootNtupleClass.sh - BEGIN 
#include <vector> 
using namespace std; 
//// Lines added by make_rootNtupleClass.sh - END 

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class rootNtupleClass {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   UInt_t          run;
   UInt_t          luminosityBlock;
   ULong64_t       event;
   UInt_t          bunchCrossing;
   UInt_t          orbitNumber;
   Int_t           nL1EG;
   Short_t         L1EG_hwIso[60];   //[nL1EG]
   Short_t         L1EG_bx[60];   //[nL1EG]
   Float_t         L1EG_eta[60];   //[nL1EG]
   Float_t         L1EG_phi[60];   //[nL1EG]
   Float_t         L1EG_pt[60];   //[nL1EG]
   Int_t           nL1EtSum;
   Short_t         L1EtSum_bx[85];   //[nL1EtSum]
   Int_t           L1EtSum_etSumType[85];   //[nL1EtSum]
   Float_t         L1EtSum_phi[85];   //[nL1EtSum]
   Float_t         L1EtSum_pt[85];   //[nL1EtSum]
   Int_t           nL1Jet;
   Short_t         L1Jet_bx[32];   //[nL1Jet]
   Float_t         L1Jet_eta[32];   //[nL1Jet]
   Float_t         L1Jet_phi[32];   //[nL1Jet]
   Float_t         L1Jet_pt[32];   //[nL1Jet]
   Int_t           nL1Mu;
   Short_t         L1Mu_hwCharge[11];   //[nL1Mu]
   Short_t         L1Mu_hwDXY[11];   //[nL1Mu]
   Short_t         L1Mu_bx[11];   //[nL1Mu]
   Int_t           L1Mu_hwQual[11];   //[nL1Mu]
   Float_t         L1Mu_eta[11];   //[nL1Mu]
   Float_t         L1Mu_etaAtVtx[11];   //[nL1Mu]
   Float_t         L1Mu_phi[11];   //[nL1Mu]
   Float_t         L1Mu_phiAtVtx[11];   //[nL1Mu]
   Float_t         L1Mu_pt[11];   //[nL1Mu]
   Float_t         L1Mu_ptUnconstrained[11];   //[nL1Mu]
   Int_t           nL1Tau;
   Short_t         L1Tau_hwIso[60];   //[nL1Tau]
   Short_t         L1Tau_bx[60];   //[nL1Tau]
   Float_t         L1Tau_eta[60];   //[nL1Tau]
   Float_t         L1Tau_phi[60];   //[nL1Tau]
   Float_t         L1Tau_pt[60];   //[nL1Tau]
   Int_t           nScoutingElectron;
   Bool_t          ScoutingElectron_rechitZeroSuppression[10];   //[nScoutingElectron]
   Int_t           ScoutingElectron_missingHits[10];   //[nScoutingElectron]
   Int_t           ScoutingElectron_bestTrack_charge[10];   //[nScoutingElectron]
   UInt_t          ScoutingElectron_nClusters[10];   //[nScoutingElectron]
   UInt_t          ScoutingElectron_nCrystals[10];   //[nScoutingElectron]
   UInt_t          ScoutingElectron_seedId[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_corrEcalEnergyError[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_dEtaIn[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_dPhiIn[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_ecalIso[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_eta[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_hOverE[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_hcalIso[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_m[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_ooEMOop[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_phi[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_preshowerEnergy[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_pt[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_r9[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_rawEnergy[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_sMaj[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_sMin[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_sigmaIetaIeta[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_trackIso[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_trackfbrem[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_bestTrack_chi2overndf[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_bestTrack_d0[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_bestTrack_dz[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_bestTrack_eta[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_bestTrack_etaMode[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_bestTrack_pMode[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_bestTrack_phi[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_bestTrack_phiMode[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_bestTrack_pt[10];   //[nScoutingElectron]
   Float_t         ScoutingElectron_bestTrack_qoverpModeError[10];   //[nScoutingElectron]
   Int_t           nScoutingFatPFJetRecluster;
   UChar_t         ScoutingFatPFJetRecluster_nConstituents[5];   //[nScoutingFatPFJetRecluster]
   Int_t           ScoutingFatPFJetRecluster_nCh[5];   //[nScoutingFatPFJetRecluster]
   Int_t           ScoutingFatPFJetRecluster_nElectrons[5];   //[nScoutingFatPFJetRecluster]
   Int_t           ScoutingFatPFJetRecluster_nMuons[5];   //[nScoutingFatPFJetRecluster]
   Int_t           ScoutingFatPFJetRecluster_nNh[5];   //[nScoutingFatPFJetRecluster]
   Int_t           ScoutingFatPFJetRecluster_nPhotons[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_area[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_chEmEF[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_chHEF[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_eta[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_hfEmEF[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_hfHEF[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_mass[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_muEF[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_neEmEF[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_neHEF[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_phi[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_pt[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_msoftdrop[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_n2b1[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_n3b1[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_particleNet_mass[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_particleNet_prob_Hbb[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_particleNet_prob_Hcc[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_particleNet_prob_Hqq[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_particleNet_prob_QCD[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_scoutGlobalParT_massCorrGeneric[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_scoutGlobalParT_massCorrGenericW2p[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_scoutGlobalParT_massCorrGenericX2p[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_scoutGlobalParT_massCorrResonance[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_scoutGlobalParT_prob_QCD[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xbb[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xbc[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xbs[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xcc[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xcs[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xgg[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xqq[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xss[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xtauhtaue[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xtauhtauh[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xtauhtaum[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xud[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_tau1[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_tau2[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_tau3[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingFatPFJetRecluster_tau4[5];   //[nScoutingFatPFJetRecluster]
   Float_t         ScoutingMET_phi;
   Float_t         ScoutingMET_pt;
   Int_t           nScoutingMuonNoVtxDisplacedVertex;
   Bool_t          ScoutingMuonNoVtxDisplacedVertex_isValidVtx[21];   //[nScoutingMuonNoVtxDisplacedVertex]
   Int_t           ScoutingMuonNoVtxDisplacedVertex_ndof[21];   //[nScoutingMuonNoVtxDisplacedVertex]
   Int_t           ScoutingMuonNoVtxDisplacedVertex_tracksSize[21];   //[nScoutingMuonNoVtxDisplacedVertex]
   Float_t         ScoutingMuonNoVtxDisplacedVertex_chi2[21];   //[nScoutingMuonNoVtxDisplacedVertex]
   Float_t         ScoutingMuonNoVtxDisplacedVertex_x[21];   //[nScoutingMuonNoVtxDisplacedVertex]
   Float_t         ScoutingMuonNoVtxDisplacedVertex_xError[21];   //[nScoutingMuonNoVtxDisplacedVertex]
   Float_t         ScoutingMuonNoVtxDisplacedVertex_xyCov[21];   //[nScoutingMuonNoVtxDisplacedVertex]
   Float_t         ScoutingMuonNoVtxDisplacedVertex_xzCov[21];   //[nScoutingMuonNoVtxDisplacedVertex]
   Float_t         ScoutingMuonNoVtxDisplacedVertex_y[21];   //[nScoutingMuonNoVtxDisplacedVertex]
   Float_t         ScoutingMuonNoVtxDisplacedVertex_yError[21];   //[nScoutingMuonNoVtxDisplacedVertex]
   Float_t         ScoutingMuonNoVtxDisplacedVertex_yzCov[21];   //[nScoutingMuonNoVtxDisplacedVertex]
   Float_t         ScoutingMuonNoVtxDisplacedVertex_z[21];   //[nScoutingMuonNoVtxDisplacedVertex]
   Float_t         ScoutingMuonNoVtxDisplacedVertex_zError[21];   //[nScoutingMuonNoVtxDisplacedVertex]
   Int_t           nScoutingMuonNoVtx;
   UChar_t         ScoutingMuonNoVtx_trk_hitPattern_beginInner[7];   //[nScoutingMuonNoVtx]
   UChar_t         ScoutingMuonNoVtx_trk_hitPattern_beginOuter[7];   //[nScoutingMuonNoVtx]
   UChar_t         ScoutingMuonNoVtx_trk_hitPattern_beginTrackHits[7];   //[nScoutingMuonNoVtx]
   UChar_t         ScoutingMuonNoVtx_trk_hitPattern_endInner[7];   //[nScoutingMuonNoVtx]
   UChar_t         ScoutingMuonNoVtx_trk_hitPattern_endOuter[7];   //[nScoutingMuonNoVtx]
   UChar_t         ScoutingMuonNoVtx_trk_hitPattern_endTrackHits[7];   //[nScoutingMuonNoVtx]
   UChar_t         ScoutingMuonNoVtx_trk_hitPattern_hitCount[7];   //[nScoutingMuonNoVtx]
   UShort_t        ScoutingMuonNoVtx_nScoutingMuonNoVtxHitPattern[7];   //[nScoutingMuonNoVtx]
   UShort_t        ScoutingMuonNoVtx_oScoutingMuonNoVtxHitPattern[7];   //[nScoutingMuonNoVtx]
   UShort_t        ScoutingMuonNoVtx_nScoutingMuonNoVtxVtxIndx[7];   //[nScoutingMuonNoVtx]
   UShort_t        ScoutingMuonNoVtx_oScoutingMuonNoVtxVtxIndx[7];   //[nScoutingMuonNoVtx]
   Int_t           ScoutingMuonNoVtx_charge[7];   //[nScoutingMuonNoVtx]
   Int_t           ScoutingMuonNoVtx_nPixelLayersWithMeasurement[7];   //[nScoutingMuonNoVtx]
   Int_t           ScoutingMuonNoVtx_nRecoMuonChambers[7];   //[nScoutingMuonNoVtx]
   Int_t           ScoutingMuonNoVtx_nRecoMuonChambersCSCorDT[7];   //[nScoutingMuonNoVtx]
   Int_t           ScoutingMuonNoVtx_nRecoMuonExpectedMatchedStations[7];   //[nScoutingMuonNoVtx]
   Int_t           ScoutingMuonNoVtx_nRecoMuonMatchedRPCLayers[7];   //[nScoutingMuonNoVtx]
   Int_t           ScoutingMuonNoVtx_nRecoMuonMatchedStations[7];   //[nScoutingMuonNoVtx]
   Int_t           ScoutingMuonNoVtx_nRecoMuonMatches[7];   //[nScoutingMuonNoVtx]
   Int_t           ScoutingMuonNoVtx_nStandAloneMuonMatchedStations[7];   //[nScoutingMuonNoVtx]
   Int_t           ScoutingMuonNoVtx_nTrackerLayersWithMeasurement[7];   //[nScoutingMuonNoVtx]
   Int_t           ScoutingMuonNoVtx_nValidPixelHits[7];   //[nScoutingMuonNoVtx]
   Int_t           ScoutingMuonNoVtx_nValidRecoMuonHits[7];   //[nScoutingMuonNoVtx]
   Int_t           ScoutingMuonNoVtx_nValidStandAloneMuonHits[7];   //[nScoutingMuonNoVtx]
   Int_t           ScoutingMuonNoVtx_nValidStripHits[7];   //[nScoutingMuonNoVtx]
   Int_t           ScoutingMuonNoVtx_recoMuonRPClayerMask[7];   //[nScoutingMuonNoVtx]
   Int_t           ScoutingMuonNoVtx_recoMuonStationMask[7];   //[nScoutingMuonNoVtx]
   Int_t           ScoutingMuonNoVtx_type[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_ecalIso[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_eta[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_hcalIso[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_m[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_normchi2[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_phi[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_pt[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trackIso[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_chi2[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_dsz[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_dszError[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_dxy[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_dxyError[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_dxy_dsz_cov[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_dz[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_dzError[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_eta[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_lambda[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_lambdaError[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_lambda_dsz_cov[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_lambda_dxy_cov[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_lambda_phi_cov[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_ndof[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_phi[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_phiError[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_phi_dsz_cov[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_phi_dxy_cov[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_pt[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_qoverp[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_qoverpError[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_qoverp_dsz_cov[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_qoverp_dxy_cov[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_qoverp_lambda_cov[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_qoverp_phi_cov[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_vx[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_vy[7];   //[nScoutingMuonNoVtx]
   Float_t         ScoutingMuonNoVtx_trk_vz[7];   //[nScoutingMuonNoVtx]
   Int_t           nScoutingMuonNoVtxHitPattern;
   UShort_t        ScoutingMuonNoVtxHitPattern_hitPattern[399];   //[nScoutingMuonNoVtxHitPattern]
   Int_t           nScoutingMuonNoVtxVtxIndx;
   Int_t           ScoutingMuonNoVtxVtxIndx_vtxIndx[42];   //[nScoutingMuonNoVtxVtxIndx]
   Int_t           nScoutingMuonVtxDisplacedVertex;
   Bool_t          ScoutingMuonVtxDisplacedVertex_isValidVtx[1];   //[nScoutingMuonVtxDisplacedVertex]
   Int_t           ScoutingMuonVtxDisplacedVertex_ndof[1];   //[nScoutingMuonVtxDisplacedVertex]
   Int_t           ScoutingMuonVtxDisplacedVertex_tracksSize[1];   //[nScoutingMuonVtxDisplacedVertex]
   Float_t         ScoutingMuonVtxDisplacedVertex_chi2[1];   //[nScoutingMuonVtxDisplacedVertex]
   Float_t         ScoutingMuonVtxDisplacedVertex_x[1];   //[nScoutingMuonVtxDisplacedVertex]
   Float_t         ScoutingMuonVtxDisplacedVertex_xError[1];   //[nScoutingMuonVtxDisplacedVertex]
   Float_t         ScoutingMuonVtxDisplacedVertex_xyCov[1];   //[nScoutingMuonVtxDisplacedVertex]
   Float_t         ScoutingMuonVtxDisplacedVertex_xzCov[1];   //[nScoutingMuonVtxDisplacedVertex]
   Float_t         ScoutingMuonVtxDisplacedVertex_y[1];   //[nScoutingMuonVtxDisplacedVertex]
   Float_t         ScoutingMuonVtxDisplacedVertex_yError[1];   //[nScoutingMuonVtxDisplacedVertex]
   Float_t         ScoutingMuonVtxDisplacedVertex_yzCov[1];   //[nScoutingMuonVtxDisplacedVertex]
   Float_t         ScoutingMuonVtxDisplacedVertex_z[1];   //[nScoutingMuonVtxDisplacedVertex]
   Float_t         ScoutingMuonVtxDisplacedVertex_zError[1];   //[nScoutingMuonVtxDisplacedVertex]
   Int_t           nScoutingMuonVtx;
   UChar_t         ScoutingMuonVtx_trk_hitPattern_beginInner[7];   //[nScoutingMuonVtx]
   UChar_t         ScoutingMuonVtx_trk_hitPattern_beginOuter[7];   //[nScoutingMuonVtx]
   UChar_t         ScoutingMuonVtx_trk_hitPattern_beginTrackHits[7];   //[nScoutingMuonVtx]
   UChar_t         ScoutingMuonVtx_trk_hitPattern_endInner[7];   //[nScoutingMuonVtx]
   UChar_t         ScoutingMuonVtx_trk_hitPattern_endOuter[7];   //[nScoutingMuonVtx]
   UChar_t         ScoutingMuonVtx_trk_hitPattern_endTrackHits[7];   //[nScoutingMuonVtx]
   UChar_t         ScoutingMuonVtx_trk_hitPattern_hitCount[7];   //[nScoutingMuonVtx]
   UShort_t        ScoutingMuonVtx_nScoutingMuonVtxHitPattern[7];   //[nScoutingMuonVtx]
   UShort_t        ScoutingMuonVtx_oScoutingMuonVtxHitPattern[7];   //[nScoutingMuonVtx]
   UShort_t        ScoutingMuonVtx_nScoutingMuonVtxVtxIndx[7];   //[nScoutingMuonVtx]
   UShort_t        ScoutingMuonVtx_oScoutingMuonVtxVtxIndx[7];   //[nScoutingMuonVtx]
   Int_t           ScoutingMuonVtx_charge[7];   //[nScoutingMuonVtx]
   Int_t           ScoutingMuonVtx_nPixelLayersWithMeasurement[7];   //[nScoutingMuonVtx]
   Int_t           ScoutingMuonVtx_nRecoMuonChambers[7];   //[nScoutingMuonVtx]
   Int_t           ScoutingMuonVtx_nRecoMuonChambersCSCorDT[7];   //[nScoutingMuonVtx]
   Int_t           ScoutingMuonVtx_nRecoMuonExpectedMatchedStations[7];   //[nScoutingMuonVtx]
   Int_t           ScoutingMuonVtx_nRecoMuonMatchedRPCLayers[7];   //[nScoutingMuonVtx]
   Int_t           ScoutingMuonVtx_nRecoMuonMatchedStations[7];   //[nScoutingMuonVtx]
   Int_t           ScoutingMuonVtx_nRecoMuonMatches[7];   //[nScoutingMuonVtx]
   Int_t           ScoutingMuonVtx_nStandAloneMuonMatchedStations[7];   //[nScoutingMuonVtx]
   Int_t           ScoutingMuonVtx_nTrackerLayersWithMeasurement[7];   //[nScoutingMuonVtx]
   Int_t           ScoutingMuonVtx_nValidPixelHits[7];   //[nScoutingMuonVtx]
   Int_t           ScoutingMuonVtx_nValidRecoMuonHits[7];   //[nScoutingMuonVtx]
   Int_t           ScoutingMuonVtx_nValidStandAloneMuonHits[7];   //[nScoutingMuonVtx]
   Int_t           ScoutingMuonVtx_nValidStripHits[7];   //[nScoutingMuonVtx]
   Int_t           ScoutingMuonVtx_recoMuonRPClayerMask[7];   //[nScoutingMuonVtx]
   Int_t           ScoutingMuonVtx_recoMuonStationMask[7];   //[nScoutingMuonVtx]
   Int_t           ScoutingMuonVtx_type[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_ecalIso[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_eta[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_hcalIso[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_m[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_normchi2[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_phi[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_pt[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trackIso[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_chi2[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_dsz[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_dszError[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_dxy[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_dxyError[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_dxy_dsz_cov[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_dz[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_dzError[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_eta[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_lambda[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_lambdaError[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_lambda_dsz_cov[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_lambda_dxy_cov[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_lambda_phi_cov[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_ndof[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_phi[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_phiError[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_phi_dsz_cov[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_phi_dxy_cov[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_pt[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_qoverp[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_qoverpError[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_qoverp_dsz_cov[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_qoverp_dxy_cov[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_qoverp_lambda_cov[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_qoverp_phi_cov[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_vx[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_vy[7];   //[nScoutingMuonVtx]
   Float_t         ScoutingMuonVtx_trk_vz[7];   //[nScoutingMuonVtx]
   Int_t           nScoutingMuonVtxHitPattern;
   UShort_t        ScoutingMuonVtxHitPattern_hitPattern[399];   //[nScoutingMuonVtxHitPattern]
   Int_t           nScoutingMuonVtxVtxIndx;
   Int_t           ScoutingMuonVtxVtxIndx_vtxIndx[1];   //[nScoutingMuonVtxVtxIndx]
   Int_t           nScoutingPFJetRecluster;
   UChar_t         ScoutingPFJetRecluster_nConstituents[39];   //[nScoutingPFJetRecluster]
   Int_t           ScoutingPFJetRecluster_nCh[39];   //[nScoutingPFJetRecluster]
   Int_t           ScoutingPFJetRecluster_nElectrons[39];   //[nScoutingPFJetRecluster]
   Int_t           ScoutingPFJetRecluster_nMuons[39];   //[nScoutingPFJetRecluster]
   Int_t           ScoutingPFJetRecluster_nNh[39];   //[nScoutingPFJetRecluster]
   Int_t           ScoutingPFJetRecluster_nPhotons[39];   //[nScoutingPFJetRecluster]
   Float_t         ScoutingPFJetRecluster_area[39];   //[nScoutingPFJetRecluster]
   Float_t         ScoutingPFJetRecluster_chEmEF[39];   //[nScoutingPFJetRecluster]
   Float_t         ScoutingPFJetRecluster_chHEF[39];   //[nScoutingPFJetRecluster]
   Float_t         ScoutingPFJetRecluster_eta[39];   //[nScoutingPFJetRecluster]
   Float_t         ScoutingPFJetRecluster_hfEmEF[39];   //[nScoutingPFJetRecluster]
   Float_t         ScoutingPFJetRecluster_hfHEF[39];   //[nScoutingPFJetRecluster]
   Float_t         ScoutingPFJetRecluster_mass[39];   //[nScoutingPFJetRecluster]
   Float_t         ScoutingPFJetRecluster_muEF[39];   //[nScoutingPFJetRecluster]
   Float_t         ScoutingPFJetRecluster_neEmEF[39];   //[nScoutingPFJetRecluster]
   Float_t         ScoutingPFJetRecluster_neHEF[39];   //[nScoutingPFJetRecluster]
   Float_t         ScoutingPFJetRecluster_phi[39];   //[nScoutingPFJetRecluster]
   Float_t         ScoutingPFJetRecluster_pt[39];   //[nScoutingPFJetRecluster]
   Float_t         ScoutingPFJetRecluster_particleNet_prob_b[39];   //[nScoutingPFJetRecluster]
   Float_t         ScoutingPFJetRecluster_particleNet_prob_bb[39];   //[nScoutingPFJetRecluster]
   Float_t         ScoutingPFJetRecluster_particleNet_prob_c[39];   //[nScoutingPFJetRecluster]
   Float_t         ScoutingPFJetRecluster_particleNet_prob_cc[39];   //[nScoutingPFJetRecluster]
   Float_t         ScoutingPFJetRecluster_particleNet_prob_g[39];   //[nScoutingPFJetRecluster]
   Float_t         ScoutingPFJetRecluster_particleNet_prob_uds[39];   //[nScoutingPFJetRecluster]
   Float_t         ScoutingPFJetRecluster_particleNet_prob_undef[39];   //[nScoutingPFJetRecluster]
   Int_t           nScoutingPFJet;
   Int_t           ScoutingPFJet_HFEMMultiplicity[90];   //[nScoutingPFJet]
   Int_t           ScoutingPFJet_HFHadronMultiplicity[90];   //[nScoutingPFJet]
   Int_t           ScoutingPFJet_chargedHadronMultiplicity[90];   //[nScoutingPFJet]
   Int_t           ScoutingPFJet_electronMultiplicity[90];   //[nScoutingPFJet]
   Int_t           ScoutingPFJet_muonMultiplicity[90];   //[nScoutingPFJet]
   Int_t           ScoutingPFJet_neutralHadronMultiplicity[90];   //[nScoutingPFJet]
   Int_t           ScoutingPFJet_photonMultiplicity[90];   //[nScoutingPFJet]
   Float_t         ScoutingPFJet_HFEMEnergy[90];   //[nScoutingPFJet]
   Float_t         ScoutingPFJet_HFHadronEnergy[90];   //[nScoutingPFJet]
   Float_t         ScoutingPFJet_HOEnergy[90];   //[nScoutingPFJet]
   Float_t         ScoutingPFJet_chargedHadronEnergy[90];   //[nScoutingPFJet]
   Float_t         ScoutingPFJet_electronEnergy[90];   //[nScoutingPFJet]
   Float_t         ScoutingPFJet_eta[90];   //[nScoutingPFJet]
   Float_t         ScoutingPFJet_jetArea[90];   //[nScoutingPFJet]
   Float_t         ScoutingPFJet_m[90];   //[nScoutingPFJet]
   Float_t         ScoutingPFJet_muonEnergy[90];   //[nScoutingPFJet]
   Float_t         ScoutingPFJet_neutralHadronEnergy[90];   //[nScoutingPFJet]
   Float_t         ScoutingPFJet_phi[90];   //[nScoutingPFJet]
   Float_t         ScoutingPFJet_photonEnergy[90];   //[nScoutingPFJet]
   Float_t         ScoutingPFJet_pt[90];   //[nScoutingPFJet]
   Int_t           nScoutingPhoton;
   Bool_t          ScoutingPhoton_rechitZeroSuppression[34];   //[nScoutingPhoton]
   UInt_t          ScoutingPhoton_nClusters[34];   //[nScoutingPhoton]
   UInt_t          ScoutingPhoton_nCrystals[34];   //[nScoutingPhoton]
   UInt_t          ScoutingPhoton_seedId[34];   //[nScoutingPhoton]
   Float_t         ScoutingPhoton_corrEcalEnergyError[34];   //[nScoutingPhoton]
   Float_t         ScoutingPhoton_ecalIso[34];   //[nScoutingPhoton]
   Float_t         ScoutingPhoton_eta[34];   //[nScoutingPhoton]
   Float_t         ScoutingPhoton_hOverE[34];   //[nScoutingPhoton]
   Float_t         ScoutingPhoton_hcalIso[34];   //[nScoutingPhoton]
   Float_t         ScoutingPhoton_m[34];   //[nScoutingPhoton]
   Float_t         ScoutingPhoton_phi[34];   //[nScoutingPhoton]
   Float_t         ScoutingPhoton_preshowerEnergy[34];   //[nScoutingPhoton]
   Float_t         ScoutingPhoton_pt[34];   //[nScoutingPhoton]
   Float_t         ScoutingPhoton_r9[34];   //[nScoutingPhoton]
   Float_t         ScoutingPhoton_rawEnergy[34];   //[nScoutingPhoton]
   Float_t         ScoutingPhoton_sMaj[34];   //[nScoutingPhoton]
   Float_t         ScoutingPhoton_sMin[34];   //[nScoutingPhoton]
   Float_t         ScoutingPhoton_sigmaIetaIeta[34];   //[nScoutingPhoton]
   Float_t         ScoutingPhoton_trkIso[34];   //[nScoutingPhoton]
   Int_t           nScoutingPrimaryVertex;
   Bool_t          ScoutingPrimaryVertex_isValidVtx[55];   //[nScoutingPrimaryVertex]
   Int_t           ScoutingPrimaryVertex_ndof[55];   //[nScoutingPrimaryVertex]
   Int_t           ScoutingPrimaryVertex_tracksSize[55];   //[nScoutingPrimaryVertex]
   Float_t         ScoutingPrimaryVertex_chi2[55];   //[nScoutingPrimaryVertex]
   Float_t         ScoutingPrimaryVertex_x[55];   //[nScoutingPrimaryVertex]
   Float_t         ScoutingPrimaryVertex_xError[55];   //[nScoutingPrimaryVertex]
   Float_t         ScoutingPrimaryVertex_xyCov[55];   //[nScoutingPrimaryVertex]
   Float_t         ScoutingPrimaryVertex_xzCov[55];   //[nScoutingPrimaryVertex]
   Float_t         ScoutingPrimaryVertex_y[55];   //[nScoutingPrimaryVertex]
   Float_t         ScoutingPrimaryVertex_yError[55];   //[nScoutingPrimaryVertex]
   Float_t         ScoutingPrimaryVertex_yzCov[55];   //[nScoutingPrimaryVertex]
   Float_t         ScoutingPrimaryVertex_z[55];   //[nScoutingPrimaryVertex]
   Float_t         ScoutingPrimaryVertex_zError[55];   //[nScoutingPrimaryVertex]
   Float_t         ScoutingRho_fixedGridRhoFastjetAll;
   Bool_t          L1_AXO_Loose;
   Bool_t          L1_AXO_Nominal;
   Bool_t          L1_AXO_Tight;
   Bool_t          L1_AXO_VLoose;
   Bool_t          L1_AXO_VTight;
   Bool_t          L1_AlwaysTrue;
   Bool_t          L1_BPTX_AND_Ref1_VME;
   Bool_t          L1_BPTX_AND_Ref3_VME;
   Bool_t          L1_BPTX_AND_Ref4_VME;
   Bool_t          L1_BPTX_BeamGas_B1_VME;
   Bool_t          L1_BPTX_BeamGas_B2_VME;
   Bool_t          L1_BPTX_BeamGas_Ref1_VME;
   Bool_t          L1_BPTX_BeamGas_Ref2_VME;
   Bool_t          L1_BPTX_NotOR_VME;
   Bool_t          L1_BPTX_OR_Ref3_VME;
   Bool_t          L1_BPTX_OR_Ref4_VME;
   Bool_t          L1_BPTX_RefAND_VME;
   Bool_t          L1_BptxMinus;
   Bool_t          L1_BptxOR;
   Bool_t          L1_BptxPlus;
   Bool_t          L1_BptxXOR;
   Bool_t          L1_CDC_SingleMu_3_er1p2_TOP120_DPHI2p618_3p142;
   Bool_t          L1_CICADA_Loose;
   Bool_t          L1_CICADA_Medium;
   Bool_t          L1_CICADA_Tight;
   Bool_t          L1_CICADA_VLoose;
   Bool_t          L1_CICADA_VTight;
   Bool_t          L1_DoubleEG11_er1p2_dR_Max0p6;
   Bool_t          L1_DoubleEG8er2p5_HTT280er;
   Bool_t          L1_DoubleEG8er2p5_HTT300er;
   Bool_t          L1_DoubleEG8er2p5_HTT320er;
   Bool_t          L1_DoubleEG_15_10_er2p5;
   Bool_t          L1_DoubleEG_20_10_er2p5;
   Bool_t          L1_DoubleEG_22_10_er2p5;
   Bool_t          L1_DoubleEG_25_12_er2p5;
   Bool_t          L1_DoubleEG_25_14_er2p5;
   Bool_t          L1_DoubleEG_27_14_er2p5;
   Bool_t          L1_DoubleEG_LooseIso16_LooseIso12_er1p5;
   Bool_t          L1_DoubleEG_LooseIso18_LooseIso12_er1p5;
   Bool_t          L1_DoubleEG_LooseIso20_LooseIso12_er1p5;
   Bool_t          L1_DoubleEG_LooseIso22_12_er2p5;
   Bool_t          L1_DoubleEG_LooseIso22_LooseIso12_er1p5;
   Bool_t          L1_DoubleEG_LooseIso25_12_er2p5;
   Bool_t          L1_DoubleEG_LooseIso25_LooseIso12_er1p5;
   Bool_t          L1_DoubleIsoTau26er2p1_Jet55_RmOvlp_dR0p5;
   Bool_t          L1_DoubleIsoTau26er2p1_Jet70_RmOvlp_dR0p5;
   Bool_t          L1_DoubleIsoTau28er2p1;
   Bool_t          L1_DoubleIsoTau30er2p1;
   Bool_t          L1_DoubleIsoTau32er2p1;
   Bool_t          L1_DoubleIsoTau32er2p1_Mass_Max80;
   Bool_t          L1_DoubleIsoTau34er2p1;
   Bool_t          L1_DoubleIsoTau35er2p1;
   Bool_t          L1_DoubleIsoTau36er2p1;
   Bool_t          L1_DoubleJet100er2p3_dEta_Max1p6;
   Bool_t          L1_DoubleJet100er2p5;
   Bool_t          L1_DoubleJet112er2p3_dEta_Max1p6;
   Bool_t          L1_DoubleJet120er2p5;
   Bool_t          L1_DoubleJet120er2p5_Mu3_dR_Max0p8;
   Bool_t          L1_DoubleJet150er2p5;
   Bool_t          L1_DoubleJet16er2p5_Mu3_dR_Max0p4;
   Bool_t          L1_DoubleJet30er2p5_Mass_Min225_dEta_Max1p5;
   Bool_t          L1_DoubleJet30er2p5_Mass_Min250_dEta_Max1p5;
   Bool_t          L1_DoubleJet30er2p5_Mass_Min300_dEta_Max1p5;
   Bool_t          L1_DoubleJet30er2p5_Mass_Min330_dEta_Max1p5;
   Bool_t          L1_DoubleJet35er2p5_Mu3_dR_Max0p4;
   Bool_t          L1_DoubleJet40er2p5;
   Bool_t          L1_DoubleJet45_Mass_Min550_IsoTau45er2p1_RmOvlp_dR0p5;
   Bool_t          L1_DoubleJet45_Mass_Min550_LooseIsoEG20er2p1_RmOvlp_dR0p2;
   Bool_t          L1_DoubleJet45_Mass_Min600_IsoTau45er2p1_RmOvlp_dR0p5;
   Bool_t          L1_DoubleJet45_Mass_Min600_LooseIsoEG20er2p1_RmOvlp_dR0p2;
   Bool_t          L1_DoubleJet60er2p5_Mu3_dR_Max0p4;
   Bool_t          L1_DoubleJet80er2p5_Mu3_dR_Max0p4;
   Bool_t          L1_DoubleJet_110_35_DoubleJet35_Mass_Min800;
   Bool_t          L1_DoubleJet_110_35_DoubleJet35_Mass_Min850;
   Bool_t          L1_DoubleJet_65_35_DoubleJet35_Mass_Min600_DoubleJetCentral50;
   Bool_t          L1_DoubleJet_65_35_DoubleJet35_Mass_Min650_DoubleJetCentral50;
   Bool_t          L1_DoubleJet_70_35_DoubleJet35_Mass_Min500_ETMHF65;
   Bool_t          L1_DoubleJet_70_35_DoubleJet35_Mass_Min550_ETMHF65;
   Bool_t          L1_DoubleJet_85_35_DoubleJet35_Mass_Min600_Mu3OQ;
   Bool_t          L1_DoubleJet_85_35_DoubleJet35_Mass_Min650_Mu3OQ;
   Bool_t          L1_DoubleLLPJet40;
   Bool_t          L1_DoubleLooseIsoEG22er2p1;
   Bool_t          L1_DoubleLooseIsoEG24er2p1;
   Bool_t          L1_DoubleMu0;
   Bool_t          L1_DoubleMu0_Mass_Min1;
   Bool_t          L1_DoubleMu0_OQ;
   Bool_t          L1_DoubleMu0_SQ;
   Bool_t          L1_DoubleMu0_SQ_OS;
   Bool_t          L1_DoubleMu0_Upt15_Upt7;
   Bool_t          L1_DoubleMu0_Upt5_Upt5;
   Bool_t          L1_DoubleMu0_Upt6_IP_Min1_Upt4;
   Bool_t          L1_DoubleMu0_Upt6_SQ_er2p0;
   Bool_t          L1_DoubleMu0_Upt7_SQ_er2p0;
   Bool_t          L1_DoubleMu0_Upt8_SQ_er2p0;
   Bool_t          L1_DoubleMu0_dR_Max1p6_Jet90er2p5_dR_Max0p8;
   Bool_t          L1_DoubleMu0er1p4_OQ_OS_dEta_Max1p6;
   Bool_t          L1_DoubleMu0er1p4_SQ_OS_dEta_Max1p2;
   Bool_t          L1_DoubleMu0er1p4_SQ_OS_dR_Max1p4;
   Bool_t          L1_DoubleMu0er1p5_SQ;
   Bool_t          L1_DoubleMu0er1p5_SQ_OS;
   Bool_t          L1_DoubleMu0er1p5_SQ_OS_dEta_Max1p2;
   Bool_t          L1_DoubleMu0er1p5_SQ_OS_dR_Max1p4;
   Bool_t          L1_DoubleMu0er1p5_SQ_dR_Max1p4;
   Bool_t          L1_DoubleMu0er2p0_SQ_OS_dEta_Max0p3_dPhi_0p8to1p2;
   Bool_t          L1_DoubleMu0er2p0_SQ_OS_dEta_Max1p5;
   Bool_t          L1_DoubleMu0er2p0_SQ_OS_dEta_Max1p6;
   Bool_t          L1_DoubleMu0er2p0_SQ_dEta_Max1p5;
   Bool_t          L1_DoubleMu0er2p0_SQ_dEta_Max1p6;
   Bool_t          L1_DoubleMu18er2p1_SQ;
   Bool_t          L1_DoubleMu3_OS_er2p3_Mass_Max14_DoubleEG7p5_er2p1_Mass_Max20;
   Bool_t          L1_DoubleMu3_SQ_ETMHF30_HTT60er;
   Bool_t          L1_DoubleMu3_SQ_ETMHF30_Jet60er2p5_OR_DoubleJet40er2p5;
   Bool_t          L1_DoubleMu3_SQ_ETMHF40_HTT60er;
   Bool_t          L1_DoubleMu3_SQ_ETMHF40_Jet60er2p5_OR_DoubleJet40er2p5;
   Bool_t          L1_DoubleMu3_SQ_ETMHF50_HTT60er;
   Bool_t          L1_DoubleMu3_SQ_ETMHF50_Jet60er2p5;
   Bool_t          L1_DoubleMu3_SQ_ETMHF50_Jet60er2p5_OR_DoubleJet40er2p5;
   Bool_t          L1_DoubleMu3_SQ_ETMHF60_Jet60er2p5;
   Bool_t          L1_DoubleMu3_SQ_HTT220er;
   Bool_t          L1_DoubleMu3er2p0_SQ_OS_dR_Max1p6;
   Bool_t          L1_DoubleMu4_SQ_EG9er2p5;
   Bool_t          L1_DoubleMu4_SQ_OS;
   Bool_t          L1_DoubleMu4_SQ_OS_dR_Max1p2;
   Bool_t          L1_DoubleMu4er2p0_SQ_OS_dR_Max1p6;
   Bool_t          L1_DoubleMu4p5_SQ_OS;
   Bool_t          L1_DoubleMu4p5_SQ_OS_dR_Max1p2;
   Bool_t          L1_DoubleMu4p5er2p0_SQ_OS;
   Bool_t          L1_DoubleMu4p5er2p0_SQ_OS_Mass_7to18;
   Bool_t          L1_DoubleMu4p5er2p0_SQ_OS_Mass_Min7;
   Bool_t          L1_DoubleMu5_OS_er2p3_Mass_8to14_DoubleEG3er2p1_Mass_Max20;
   Bool_t          L1_DoubleMu5_SQ_EG9er2p5;
   Bool_t          L1_DoubleMu5_SQ_OS_dR_Max1p6;
   Bool_t          L1_DoubleMu6_Upt6_SQ_er2p0;
   Bool_t          L1_DoubleMu7_Upt7_SQ_er2p0;
   Bool_t          L1_DoubleMu8_SQ;
   Bool_t          L1_DoubleMu8_Upt8_SQ_er2p0;
   Bool_t          L1_DoubleMu9_SQ;
   Bool_t          L1_DoubleMu_12_5;
   Bool_t          L1_DoubleMu_15_5_SQ;
   Bool_t          L1_DoubleMu_15_7;
   Bool_t          L1_DoubleMu_15_7_Mass_Min1;
   Bool_t          L1_DoubleMu_15_7_SQ;
   Bool_t          L1_DoubleTau70er2p1;
   Bool_t          L1_ETM120;
   Bool_t          L1_ETM150;
   Bool_t          L1_ETMHF100;
   Bool_t          L1_ETMHF100_HTT60er;
   Bool_t          L1_ETMHF110;
   Bool_t          L1_ETMHF110_HTT60er;
   Bool_t          L1_ETMHF120;
   Bool_t          L1_ETMHF120_HTT60er;
   Bool_t          L1_ETMHF130;
   Bool_t          L1_ETMHF130_HTT60er;
   Bool_t          L1_ETMHF140;
   Bool_t          L1_ETMHF150;
   Bool_t          L1_ETMHF70;
   Bool_t          L1_ETMHF70_HTT60er;
   Bool_t          L1_ETMHF80;
   Bool_t          L1_ETMHF80_HTT60er;
   Bool_t          L1_ETMHF80_SingleJet55er2p5_dPhi_Min2p1;
   Bool_t          L1_ETMHF80_SingleJet55er2p5_dPhi_Min2p6;
   Bool_t          L1_ETMHF90;
   Bool_t          L1_ETMHF90_HTT60er;
   Bool_t          L1_ETMHF90_SingleJet60er2p5_dPhi_Min2p1;
   Bool_t          L1_ETMHF90_SingleJet60er2p5_dPhi_Min2p6;
   Bool_t          L1_ETT2000;
   Bool_t          L1_FirstBunchAfterTrain;
   Bool_t          L1_FirstBunchBeforeTrain;
   Bool_t          L1_FirstBunchInTrain;
   Bool_t          L1_FirstCollisionInOrbit;
   Bool_t          L1_FirstCollisionInTrain;
   Bool_t          L1_HCAL_LaserMon_Trig;
   Bool_t          L1_HCAL_LaserMon_Veto;
   Bool_t          L1_HTMHF100;
   Bool_t          L1_HTMHF120;
   Bool_t          L1_HTMHF125;
   Bool_t          L1_HTMHF130;
   Bool_t          L1_HTMHF150;
   Bool_t          L1_HTT120_SingleLLPJet40;
   Bool_t          L1_HTT120er;
   Bool_t          L1_HTT160_SingleLLPJet50;
   Bool_t          L1_HTT160er;
   Bool_t          L1_HTT200_SingleLLPJet60;
   Bool_t          L1_HTT200er;
   Bool_t          L1_HTT240_SingleLLPJet70;
   Bool_t          L1_HTT255er;
   Bool_t          L1_HTT280er;
   Bool_t          L1_HTT280er_QuadJet_70_55_40_35_er2p5;
   Bool_t          L1_HTT320er;
   Bool_t          L1_HTT320er_QuadJet_70_55_40_40_er2p5;
   Bool_t          L1_HTT320er_QuadJet_80_60_er2p1_45_40_er2p3;
   Bool_t          L1_HTT320er_QuadJet_80_60_er2p1_50_45_er2p3;
   Bool_t          L1_HTT360er;
   Bool_t          L1_HTT400er;
   Bool_t          L1_HTT450er;
   Bool_t          L1_IsoEG32er2p5_Mt40;
   Bool_t          L1_IsoTau52er2p1_QuadJet36er2p5;
   Bool_t          L1_IsolatedBunch;
   Bool_t          L1_LastBunchInTrain;
   Bool_t          L1_LastCollisionInTrain;
   Bool_t          L1_LooseIsoEG14er2p5_HTT200er;
   Bool_t          L1_LooseIsoEG16er2p5_HTT200er;
   Bool_t          L1_LooseIsoEG22er2p1_IsoTau26er2p1_dR_Min0p3;
   Bool_t          L1_LooseIsoEG22er2p1_Tau70er2p1_dR_Min0p3;
   Bool_t          L1_LooseIsoEG24er2p1_HTT100er;
   Bool_t          L1_LooseIsoEG24er2p1_IsoTau27er2p1_dR_Min0p3;
   Bool_t          L1_LooseIsoEG26er2p1_HTT100er;
   Bool_t          L1_LooseIsoEG26er2p1_Jet34er2p5_dR_Min0p3;
   Bool_t          L1_LooseIsoEG28er2p1_HTT100er;
   Bool_t          L1_LooseIsoEG28er2p1_Jet34er2p5_dR_Min0p3;
   Bool_t          L1_LooseIsoEG30er2p1_Jet34er2p5_dR_Min0p3;
   Bool_t          L1_MinimumBiasHF0;
   Bool_t          L1_MinimumBiasHF0_AND_BptxAND;
   Bool_t          L1_Mu10er2p3_Jet32er2p3_dR_Max0p4_DoubleJet32er2p3_dEta_Max1p6;
   Bool_t          L1_Mu12_HTT150er;
   Bool_t          L1_Mu12er2p3_Jet40er2p3_dR_Max0p4_DoubleJet40er2p3_dEta_Max1p6;
   Bool_t          L1_Mu14_HTT150er;
   Bool_t          L1_Mu18er2p1_Tau24er2p1;
   Bool_t          L1_Mu18er2p1_Tau26er2p1;
   Bool_t          L1_Mu18er2p1_Tau26er2p1_Jet55;
   Bool_t          L1_Mu18er2p1_Tau26er2p1_Jet70;
   Bool_t          L1_Mu20_EG10er2p5;
   Bool_t          L1_Mu22er2p1_IsoTau30er2p1;
   Bool_t          L1_Mu22er2p1_IsoTau32er2p1;
   Bool_t          L1_Mu22er2p1_IsoTau34er2p1;
   Bool_t          L1_Mu22er2p1_IsoTau40er2p1;
   Bool_t          L1_Mu22er2p1_Tau70er2p1;
   Bool_t          L1_Mu3_Jet120er2p5_dR_Max0p4;
   Bool_t          L1_Mu3_Jet16er2p5_dR_Max0p4;
   Bool_t          L1_Mu3_Jet30er2p5;
   Bool_t          L1_Mu3_Jet60er2p5_dR_Max0p4;
   Bool_t          L1_Mu3er1p5_Jet100er2p5_ETMHF30;
   Bool_t          L1_Mu3er1p5_Jet100er2p5_ETMHF40;
   Bool_t          L1_Mu3er1p5_Jet100er2p5_ETMHF50;
   Bool_t          L1_Mu5_EG23er2p5;
   Bool_t          L1_Mu5_LooseIsoEG20er2p5;
   Bool_t          L1_Mu6_DoubleEG10er2p5;
   Bool_t          L1_Mu6_DoubleEG12er2p5;
   Bool_t          L1_Mu6_DoubleEG15er2p5;
   Bool_t          L1_Mu6_DoubleEG17er2p5;
   Bool_t          L1_Mu6_HTT240er;
   Bool_t          L1_Mu6_HTT250er;
   Bool_t          L1_Mu7_EG20er2p5;
   Bool_t          L1_Mu7_EG23er2p5;
   Bool_t          L1_Mu7_LooseIsoEG20er2p5;
   Bool_t          L1_Mu7_LooseIsoEG23er2p5;
   Bool_t          L1_NotBptxOR;
   Bool_t          L1_QuadJet60er2p5;
   Bool_t          L1_QuadJet_95_75_65_20_DoubleJet_75_65_er2p5_Jet20_FWD3p0;
   Bool_t          L1_QuadMu0;
   Bool_t          L1_QuadMu0_OQ;
   Bool_t          L1_QuadMu0_SQ;
   Bool_t          L1_SecondBunchInTrain;
   Bool_t          L1_SecondLastBunchInTrain;
   Bool_t          L1_SingleEG10er2p5;
   Bool_t          L1_SingleEG15er2p5;
   Bool_t          L1_SingleEG26er2p5;
   Bool_t          L1_SingleEG28_FWD2p5;
   Bool_t          L1_SingleEG28er1p5;
   Bool_t          L1_SingleEG28er2p1;
   Bool_t          L1_SingleEG28er2p5;
   Bool_t          L1_SingleEG34er2p5;
   Bool_t          L1_SingleEG36er2p5;
   Bool_t          L1_SingleEG38er2p5;
   Bool_t          L1_SingleEG40er2p5;
   Bool_t          L1_SingleEG42er2p5;
   Bool_t          L1_SingleEG45er2p5;
   Bool_t          L1_SingleEG50;
   Bool_t          L1_SingleEG60;
   Bool_t          L1_SingleEG8er2p5;
   Bool_t          L1_SingleIsoEG24er2p1;
   Bool_t          L1_SingleIsoEG26er2p1;
   Bool_t          L1_SingleIsoEG26er2p5;
   Bool_t          L1_SingleIsoEG28_FWD2p5;
   Bool_t          L1_SingleIsoEG28er1p5;
   Bool_t          L1_SingleIsoEG28er2p1;
   Bool_t          L1_SingleIsoEG28er2p5;
   Bool_t          L1_SingleIsoEG30er2p1;
   Bool_t          L1_SingleIsoEG30er2p5;
   Bool_t          L1_SingleIsoEG32er2p1;
   Bool_t          L1_SingleIsoEG32er2p5;
   Bool_t          L1_SingleIsoEG34er2p5;
   Bool_t          L1_SingleJet10erHE;
   Bool_t          L1_SingleJet120;
   Bool_t          L1_SingleJet120_FWD2p5;
   Bool_t          L1_SingleJet120_FWD3p0;
   Bool_t          L1_SingleJet120er1p3;
   Bool_t          L1_SingleJet120er2p5;
   Bool_t          L1_SingleJet12erHE;
   Bool_t          L1_SingleJet140er2p5;
   Bool_t          L1_SingleJet160er2p5;
   Bool_t          L1_SingleJet180;
   Bool_t          L1_SingleJet180er2p5;
   Bool_t          L1_SingleJet200;
   Bool_t          L1_SingleJet20er2p5_NotBptxOR;
   Bool_t          L1_SingleJet20er2p5_NotBptxOR_3BX;
   Bool_t          L1_SingleJet35;
   Bool_t          L1_SingleJet35_FWD2p5;
   Bool_t          L1_SingleJet35_FWD3p0;
   Bool_t          L1_SingleJet35er1p3;
   Bool_t          L1_SingleJet35er2p5;
   Bool_t          L1_SingleJet43er2p5_NotBptxOR_3BX;
   Bool_t          L1_SingleJet46er2p5_NotBptxOR_3BX;
   Bool_t          L1_SingleJet60;
   Bool_t          L1_SingleJet60_FWD2p5;
   Bool_t          L1_SingleJet8erHE;
   Bool_t          L1_SingleJet90;
   Bool_t          L1_SingleJet90_FWD2p5;
   Bool_t          L1_SingleLooseIsoEG26er1p5;
   Bool_t          L1_SingleLooseIsoEG26er2p5;
   Bool_t          L1_SingleLooseIsoEG28_FWD2p5;
   Bool_t          L1_SingleLooseIsoEG28er1p5;
   Bool_t          L1_SingleLooseIsoEG28er2p1;
   Bool_t          L1_SingleLooseIsoEG28er2p5;
   Bool_t          L1_SingleLooseIsoEG30er1p5;
   Bool_t          L1_SingleLooseIsoEG30er2p5;
   Bool_t          L1_SingleMu0_BMTF;
   Bool_t          L1_SingleMu0_DQ;
   Bool_t          L1_SingleMu0_EMTF;
   Bool_t          L1_SingleMu0_OMTF;
   Bool_t          L1_SingleMu0_SQ13_BMTF;
   Bool_t          L1_SingleMu0_SQ14_BMTF;
   Bool_t          L1_SingleMu0_SQ15_BMTF;
   Bool_t          L1_SingleMu0_Upt10;
   Bool_t          L1_SingleMu0_Upt10_BMTF;
   Bool_t          L1_SingleMu0_Upt10_EMTF;
   Bool_t          L1_SingleMu0_Upt10_OMTF;
   Bool_t          L1_SingleMu0_Upt10_SQ14_BMTF;
   Bool_t          L1_SingleMu0_Upt15_SQ14_BMTF;
   Bool_t          L1_SingleMu0_Upt20_SQ14_BMTF;
   Bool_t          L1_SingleMu0_Upt25_SQ14_BMTF;
   Bool_t          L1_SingleMu10_SQ14_BMTF;
   Bool_t          L1_SingleMu11_SQ14_BMTF;
   Bool_t          L1_SingleMu12_DQ_BMTF;
   Bool_t          L1_SingleMu12_DQ_EMTF;
   Bool_t          L1_SingleMu12_DQ_OMTF;
   Bool_t          L1_SingleMu15_DQ;
   Bool_t          L1_SingleMu18;
   Bool_t          L1_SingleMu20;
   Bool_t          L1_SingleMu22;
   Bool_t          L1_SingleMu22_BMTF;
   Bool_t          L1_SingleMu22_BMTF_NEG;
   Bool_t          L1_SingleMu22_BMTF_POS;
   Bool_t          L1_SingleMu22_DQ;
   Bool_t          L1_SingleMu22_EMTF;
   Bool_t          L1_SingleMu22_EMTF_NEG;
   Bool_t          L1_SingleMu22_EMTF_POS;
   Bool_t          L1_SingleMu22_OMTF;
   Bool_t          L1_SingleMu22_OMTF_NEG;
   Bool_t          L1_SingleMu22_OMTF_POS;
   Bool_t          L1_SingleMu22_OQ;
   Bool_t          L1_SingleMu25;
   Bool_t          L1_SingleMu3;
   Bool_t          L1_SingleMu5;
   Bool_t          L1_SingleMu5_SQ14_BMTF;
   Bool_t          L1_SingleMu6_SQ14_BMTF;
   Bool_t          L1_SingleMu7;
   Bool_t          L1_SingleMu7_DQ;
   Bool_t          L1_SingleMu7_SQ14_BMTF;
   Bool_t          L1_SingleMu8_SQ14_BMTF;
   Bool_t          L1_SingleMu9_SQ14_BMTF;
   Bool_t          L1_SingleMuCosmics;
   Bool_t          L1_SingleMuCosmics_BMTF;
   Bool_t          L1_SingleMuCosmics_EMTF;
   Bool_t          L1_SingleMuCosmics_OMTF;
   Bool_t          L1_SingleMuOpen;
   Bool_t          L1_SingleMuOpen_BMTF;
   Bool_t          L1_SingleMuOpen_EMTF;
   Bool_t          L1_SingleMuOpen_NotBptxOR;
   Bool_t          L1_SingleMuOpen_OMTF;
   Bool_t          L1_SingleMuOpen_er1p1_NotBptxOR_3BX;
   Bool_t          L1_SingleMuOpen_er1p4_NotBptxOR_3BX;
   Bool_t          L1_SingleMuShower_Nominal;
   Bool_t          L1_SingleMuShower_Tight;
   Bool_t          L1_SingleTau120er2p1;
   Bool_t          L1_SingleTau130er2p1;
   Bool_t          L1_TOTEM_1;
   Bool_t          L1_TOTEM_2;
   Bool_t          L1_TOTEM_3;
   Bool_t          L1_TOTEM_4;
   Bool_t          L1_TripleEG_18_17_8_er2p5;
   Bool_t          L1_TripleEG_18_18_12_er2p5;
   Bool_t          L1_TripleJet_100_80_70_DoubleJet_80_70_er2p5;
   Bool_t          L1_TripleJet_105_85_75_DoubleJet_85_75_er2p5;
   Bool_t          L1_TripleJet_95_75_65_DoubleJet_75_65_er2p5;
   Bool_t          L1_TripleMu0;
   Bool_t          L1_TripleMu0_OQ;
   Bool_t          L1_TripleMu0_SQ;
   Bool_t          L1_TripleMu3;
   Bool_t          L1_TripleMu3_SQ;
   Bool_t          L1_TripleMu_3SQ_2p5SQ_0;
   Bool_t          L1_TripleMu_3SQ_2p5SQ_0_Mass_Max12;
   Bool_t          L1_TripleMu_3SQ_2p5SQ_0_OS_Mass_Max12;
   Bool_t          L1_TripleMu_4SQ_2p5SQ_0_OS_Mass_Max12;
   Bool_t          L1_TripleMu_5SQ_3SQ_0OQ;
   Bool_t          L1_TripleMu_5SQ_3SQ_0OQ_DoubleMu_5_3_SQ_OS_Mass_Max9;
   Bool_t          L1_TripleMu_5SQ_3SQ_0_DoubleMu_5_3_SQ_OS_Mass_Max9;
   Bool_t          L1_TripleMu_5_3_3;
   Bool_t          L1_TripleMu_5_3_3_SQ;
   Bool_t          L1_TripleMu_5_3p5_2p5;
   Bool_t          L1_TripleMu_5_3p5_2p5_DoubleMu_5_2p5_OS_Mass_5to17;
   Bool_t          L1_TripleMu_5_4_2p5_DoubleMu_5_2p5_OS_Mass_5to17;
   Bool_t          L1_TripleMu_5_5_3;
   Bool_t          L1_TwoMuShower_Loose;
   Bool_t          L1_UnpairedBunchBptxMinus;
   Bool_t          L1_UnpairedBunchBptxPlus;
   Bool_t          L1_ZeroBias;
   Bool_t          L1_ZeroBias_copy;
   Bool_t          L1_UnprefireableEvent_TriggerRules;
   Bool_t          L1_UnprefireableEvent_FirstBxInTrain;
   Bool_t          L1_FinalOR_BXmin1;
   Bool_t          L1_FinalOR_BXmin2;
   Bool_t          HLTriggerFirstPath;
   Bool_t          DST_PFScouting_DoubleMuon;
   Bool_t          DST_PFScouting_DoubleEG;
   Bool_t          DST_PFScouting_JetHT;
   Bool_t          DST_PFScouting_DatasetMuon;
   Bool_t          DST_PFScouting_AXOVLoose;
   Bool_t          DST_PFScouting_AXOLoose;
   Bool_t          DST_PFScouting_AXONominal;
   Bool_t          DST_PFScouting_AXOTight;
   Bool_t          DST_PFScouting_AXOVTight;
   Bool_t          DST_PFScouting_CICADAVLoose;
   Bool_t          DST_PFScouting_CICADALoose;
   Bool_t          DST_PFScouting_CICADAMedium;
   Bool_t          DST_PFScouting_CICADATight;
   Bool_t          DST_PFScouting_CICADAVTight;
   Bool_t          DST_PFScouting_SingleMuon;
   Bool_t          DST_PFScouting_SinglePhotonEB;
   Bool_t          DST_PFScouting_ZeroBias;
   Bool_t          HLT_EphemeralPhysics;
   Bool_t          HLT_EphemeralZeroBias;
   Bool_t          HLT_EcalCalibration;
   Bool_t          HLT_HcalCalibration;
   Bool_t          HLT_HcalNZS;
   Bool_t          HLT_HcalPhiSym;
   Bool_t          HLT_Random;
   Bool_t          HLT_Physics;
   Bool_t          HLT_ZeroBias;
   Bool_t          HLT_ZeroBias_Alignment;
   Bool_t          HLT_ZeroBias_Beamspot;
   Bool_t          HLT_ZeroBias_IsolatedBunches;
   Bool_t          HLT_ZeroBias_FirstBXAfterTrain;
   Bool_t          HLT_ZeroBias_FirstCollisionAfterAbortGap;
   Bool_t          HLT_ZeroBias_FirstCollisionInTrain;
   Bool_t          HLT_ZeroBias_LastCollisionInTrain;
   Bool_t          HLT_HT300_Beamspot;
   Bool_t          HLT_IsoTrackHB;
   Bool_t          HLT_IsoTrackHE;
   Bool_t          HLT_PFJet40_GPUvsCPU;
   Bool_t          HLT_AK8PFJet380_SoftDropMass30;
   Bool_t          HLT_AK8PFJet400_SoftDropMass30;
   Bool_t          HLT_AK8PFJet425_SoftDropMass30;
   Bool_t          HLT_AK8PFJet450_SoftDropMass30;
   Bool_t          HLT_AK8DiPFJet250_250_SoftDropMass40;
   Bool_t          HLT_AK8DiPFJet250_250_SoftDropMass50;
   Bool_t          HLT_AK8DiPFJet260_260_SoftDropMass30;
   Bool_t          HLT_AK8DiPFJet260_260_SoftDropMass40;
   Bool_t          HLT_AK8DiPFJet270_270_SoftDropMass30;
   Bool_t          HLT_AK8DiPFJet280_280_SoftDropMass30;
   Bool_t          HLT_AK8DiPFJet290_290_SoftDropMass30;
   Bool_t          HLT_CaloJet500_NoJetID;
   Bool_t          HLT_CaloJet550_NoJetID;
   Bool_t          HLT_DoubleMu5_Upsilon_DoubleEle3_CaloIdL_TrackIdL;
   Bool_t          HLT_DoubleMu3_DoubleEle7p5_CaloIdL_TrackIdL_Upsilon;
   Bool_t          HLT_Trimuon5_3p5_2_Upsilon_Muon;
   Bool_t          HLT_TrimuonOpen_5_3p5_2_Upsilon_Muon;
   Bool_t          HLT_DoubleEle25_CaloIdL_MW;
   Bool_t          HLT_DoubleEle27_CaloIdL_MW;
   Bool_t          HLT_DoubleEle33_CaloIdL_MW;
   Bool_t          HLT_DoubleEle24_eta2p1_WPTight_Gsf;
   Bool_t          HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_DZ_PFHT350;
   Bool_t          HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_PFHT350;
   Bool_t          HLT_Mu27_Ele37_CaloIdL_MW;
   Bool_t          HLT_Mu37_Ele27_CaloIdL_MW;
   Bool_t          HLT_Mu37_TkMu27;
   Bool_t          HLT_DoubleMu4_3_Bs;
   Bool_t          HLT_DoubleMu4_3_Jpsi;
   Bool_t          HLT_DoubleMu4_3_LowMass;
   Bool_t          HLT_DoubleMu4_LowMass_Displaced;
   Bool_t          HLT_Mu0_L1DoubleMu;
   Bool_t          HLT_Mu4_L1DoubleMu;
   Bool_t          HLT_DoubleMu2_Jpsi_LowPt;
   Bool_t          HLT_DoubleMu4_3_Photon4_BsToMMG;
   Bool_t          HLT_DoubleMu4_3_Displaced_Photon4_BsToMMG;
   Bool_t          HLT_DoubleMu3_Trk_Tau3mu;
   Bool_t          HLT_DoubleMu3_TkMu_DsTau3Mu;
   Bool_t          HLT_DoubleMu4_Mass3p8_DZ_PFHT350;
   Bool_t          HLT_DoubleMu4_MuMuTrk_Displaced;
   Bool_t          HLT_Mu3_PFJet40;
   Bool_t          HLT_Mu7p5_L2Mu2_Jpsi;
   Bool_t          HLT_Mu7p5_L2Mu2_Upsilon;
   Bool_t          HLT_Mu3_L1SingleMu5orSingleMu7;
   Bool_t          HLT_Mu0_Barrel;
   Bool_t          HLT_Mu0_Barrel_L1HP6;
   Bool_t          HLT_Mu0_Barrel_L1HP7;
   Bool_t          HLT_Mu0_Barrel_L1HP8;
   Bool_t          HLT_Mu0_Barrel_L1HP9;
   Bool_t          HLT_Mu0_Barrel_L1HP10;
   Bool_t          HLT_Mu0_Barrel_L1HP11;
   Bool_t          HLT_Mu0_Barrel_L1HP6_IP6;
   Bool_t          HLT_Mu6_Barrel_L1HP7_IP6;
   Bool_t          HLT_Mu7_Barrel_L1HP8_IP6;
   Bool_t          HLT_Mu8_Barrel_L1HP9_IP6;
   Bool_t          HLT_Mu9_Barrel_L1HP10_IP6;
   Bool_t          HLT_Mu10_Barrel_L1HP11_IP6;
   Bool_t          HLT_DoublePhoton33_CaloIdL;
   Bool_t          HLT_DoublePhoton70;
   Bool_t          HLT_DoublePhoton85;
   Bool_t          HLT_DiEle27_WPTightCaloOnly_L1DoubleEG;
   Bool_t          HLT_Ele30_WPTight_Gsf;
   Bool_t          HLT_Ele32_WPTight_Gsf;
   Bool_t          HLT_Ele35_WPTight_Gsf;
   Bool_t          HLT_Ele38_WPTight_Gsf;
   Bool_t          HLT_Ele40_WPTight_Gsf;
   Bool_t          HLT_Ele32_WPTight_Gsf_L1DoubleEG;
   Bool_t          HLT_IsoMu20;
   Bool_t          HLT_IsoMu24;
   Bool_t          HLT_IsoMu24_eta2p1;
   Bool_t          HLT_IsoMu27;
   Bool_t          HLT_UncorrectedJetE30_NoBPTX;
   Bool_t          HLT_UncorrectedJetE30_NoBPTX3BX;
   Bool_t          HLT_UncorrectedJetE60_NoBPTX3BX;
   Bool_t          HLT_UncorrectedJetE70_NoBPTX3BX;
   Bool_t          HLT_L1SingleMuCosmics;
   Bool_t          HLT_L2Mu10_NoVertex_NoBPTX3BX;
   Bool_t          HLT_L2Mu10_NoVertex_NoBPTX;
   Bool_t          HLT_L2Mu45_NoVertex_3Sta_NoBPTX3BX;
   Bool_t          HLT_L2Mu40_NoVertex_3Sta_NoBPTX3BX;
   Bool_t          HLT_L2Mu23NoVtx_2Cha;
   Bool_t          HLT_L2Mu23NoVtx_2Cha_CosmicSeed;
   Bool_t          HLT_DoubleL2Mu30NoVtx_2Cha_CosmicSeed_Eta2p4;
   Bool_t          HLT_DoubleL2Mu30NoVtx_2Cha_Eta2p4;
   Bool_t          HLT_DoubleL2Mu50;
   Bool_t          HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed;
   Bool_t          HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed;
   Bool_t          HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed_Eta2p4;
   Bool_t          HLT_DoubleL2Mu23NoVtx_2Cha;
   Bool_t          HLT_DoubleL2Mu25NoVtx_2Cha;
   Bool_t          HLT_DoubleL2Mu25NoVtx_2Cha_Eta2p4;
   Bool_t          HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL;
   Bool_t          HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL;
   Bool_t          HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ;
   Bool_t          HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ;
   Bool_t          HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8;
   Bool_t          HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_PFJet30;
   Bool_t          HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass8;
   Bool_t          HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8;
   Bool_t          HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_CaloJet30;
   Bool_t          HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_AK8PFJet30;
   Bool_t          HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_AK8CaloJet30;
   Bool_t          HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass3p8;
   Bool_t          HLT_Mu30_TkMu0_Psi;
   Bool_t          HLT_Mu30_TkMu0_Upsilon;
   Bool_t          HLT_Mu25_TkMu0_Phi;
   Bool_t          HLT_Mu15;
   Bool_t          HLT_Mu20;
   Bool_t          HLT_Mu27;
   Bool_t          HLT_Mu50;
   Bool_t          HLT_Mu55;
   Bool_t          HLT_CascadeMu100;
   Bool_t          HLT_HighPtTkMu100;
   Bool_t          HLT_DiPFJetAve40;
   Bool_t          HLT_DiPFJetAve60;
   Bool_t          HLT_DiPFJetAve80;
   Bool_t          HLT_DiPFJetAve140;
   Bool_t          HLT_DiPFJetAve200;
   Bool_t          HLT_DiPFJetAve260;
   Bool_t          HLT_DiPFJetAve320;
   Bool_t          HLT_DiPFJetAve400;
   Bool_t          HLT_DiPFJetAve500;
   Bool_t          HLT_DiPFJetAve60_HFJEC;
   Bool_t          HLT_DiPFJetAve80_HFJEC;
   Bool_t          HLT_DiPFJetAve100_HFJEC;
   Bool_t          HLT_DiPFJetAve160_HFJEC;
   Bool_t          HLT_DiPFJetAve220_HFJEC;
   Bool_t          HLT_DiPFJetAve260_HFJEC;
   Bool_t          HLT_DiPFJetAve300_HFJEC;
   Bool_t          HLT_DiPFJetAve180_PPSMatch_Xi0p3_QuadJet_Max2ProtPerRP;
   Bool_t          HLT_AK8PFJet40;
   Bool_t          HLT_AK8PFJet60;
   Bool_t          HLT_AK8PFJet80;
   Bool_t          HLT_AK8PFJet140;
   Bool_t          HLT_AK8PFJet200;
   Bool_t          HLT_AK8PFJet260;
   Bool_t          HLT_AK8PFJet320;
   Bool_t          HLT_AK8PFJet400;
   Bool_t          HLT_AK8PFJet450;
   Bool_t          HLT_AK8PFJet500;
   Bool_t          HLT_AK8PFJet550;
   Bool_t          HLT_PFJet40;
   Bool_t          HLT_PFJet60;
   Bool_t          HLT_PFJet80;
   Bool_t          HLT_PFJet110;
   Bool_t          HLT_PFJet140;
   Bool_t          HLT_PFJet200;
   Bool_t          HLT_PFJet260;
   Bool_t          HLT_PFJet320;
   Bool_t          HLT_PFJet400;
   Bool_t          HLT_PFJet450;
   Bool_t          HLT_PFJet500;
   Bool_t          HLT_PFJet550;
   Bool_t          HLT_PFJetFwd40;
   Bool_t          HLT_PFJetFwd60;
   Bool_t          HLT_PFJetFwd80;
   Bool_t          HLT_PFJetFwd140;
   Bool_t          HLT_PFJetFwd200;
   Bool_t          HLT_PFJetFwd260;
   Bool_t          HLT_PFJetFwd320;
   Bool_t          HLT_PFJetFwd400;
   Bool_t          HLT_PFJetFwd450;
   Bool_t          HLT_PFJetFwd500;
   Bool_t          HLT_AK8PFJetFwd40;
   Bool_t          HLT_AK8PFJetFwd60;
   Bool_t          HLT_AK8PFJetFwd80;
   Bool_t          HLT_AK8PFJetFwd140;
   Bool_t          HLT_AK8PFJetFwd200;
   Bool_t          HLT_AK8PFJetFwd260;
   Bool_t          HLT_AK8PFJetFwd320;
   Bool_t          HLT_AK8PFJetFwd400;
   Bool_t          HLT_AK8PFJetFwd450;
   Bool_t          HLT_AK8PFJetFwd500;
   Bool_t          HLT_PFHT180;
   Bool_t          HLT_PFHT250;
   Bool_t          HLT_PFHT370;
   Bool_t          HLT_PFHT430;
   Bool_t          HLT_PFHT510;
   Bool_t          HLT_PFHT590;
   Bool_t          HLT_PFHT680;
   Bool_t          HLT_PFHT780;
   Bool_t          HLT_PFHT890;
   Bool_t          HLT_PFHT1050;
   Bool_t          HLT_PFHT500_PFMET100_PFMHT100_IDTight;
   Bool_t          HLT_PFHT500_PFMET110_PFMHT110_IDTight;
   Bool_t          HLT_PFHT700_PFMET85_PFMHT85_IDTight;
   Bool_t          HLT_PFHT800_PFMET75_PFMHT75_IDTight;
   Bool_t          HLT_PFMET120_PFMHT120_IDTight;
   Bool_t          HLT_PFMET130_PFMHT130_IDTight;
   Bool_t          HLT_PFMET140_PFMHT140_IDTight;
   Bool_t          HLT_PFMET120_PFMHT120_IDTight_PFHT60;
   Bool_t          HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60;
   Bool_t          HLT_PFMETTypeOne140_PFMHT140_IDTight;
   Bool_t          HLT_PFMETNoMu120_PFMHTNoMu120_IDTight;
   Bool_t          HLT_PFMETNoMu130_PFMHTNoMu130_IDTight;
   Bool_t          HLT_PFMETNoMu140_PFMHTNoMu140_IDTight;
   Bool_t          HLT_PFMETNoMu110_PFMHTNoMu110_IDTight_FilterHF;
   Bool_t          HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_FilterHF;
   Bool_t          HLT_PFMETNoMu130_PFMHTNoMu130_IDTight_FilterHF;
   Bool_t          HLT_PFMETNoMu140_PFMHTNoMu140_IDTight_FilterHF;
   Bool_t          HLT_L1ETMHadSeeds;
   Bool_t          HLT_CaloMHT90;
   Bool_t          HLT_CaloMET90_NotCleaned;
   Bool_t          HLT_CaloMET350_NotCleaned;
   Bool_t          HLT_PFMET200_NotCleaned;
   Bool_t          HLT_PFMET250_NotCleaned;
   Bool_t          HLT_PFMET300_NotCleaned;
   Bool_t          HLT_PFMET200_BeamHaloCleaned;
   Bool_t          HLT_PFMETTypeOne200_BeamHaloCleaned;
   Bool_t          HLT_MET105_IsoTrk50;
   Bool_t          HLT_MET120_IsoTrk50;
   Bool_t          HLT_Mu12eta2p3;
   Bool_t          HLT_Mu12eta2p3_PFJet40;
   Bool_t          HLT_Mu12_DoublePFJets40_PNetBTag_0p11;
   Bool_t          HLT_Mu12_DoublePFJets100_PNetBTag_0p11;
   Bool_t          HLT_Mu12_DoublePFJets200_PNetBTag_0p11;
   Bool_t          HLT_Mu12_DoublePFJets350_PNetBTag_0p11;
   Bool_t          HLT_Mu12_DoublePFJets40MaxDeta1p6_PNet2BTag_0p11;
   Bool_t          HLT_Mu12_DoublePFJets54MaxDeta1p6_PNet2BTag_0p11;
   Bool_t          HLT_DoublePFJets40_PNetBTag_0p11;
   Bool_t          HLT_DoublePFJets100_PNetBTag_0p11;
   Bool_t          HLT_DoublePFJets200_PNetBTag_0p11;
   Bool_t          HLT_DoublePFJets350_PNetBTag_0p11;
   Bool_t          HLT_DoublePFJets116MaxDeta1p6_PNet2BTag_0p11;
   Bool_t          HLT_DoublePFJets128MaxDeta1p6_PNet2BTag_0p11;
   Bool_t          HLT_Photon300_NoHE;
   Bool_t          HLT_Mu8_TrkIsoVVL;
   Bool_t          HLT_Mu8_DiEle12_CaloIdL_TrackIdL_DZ;
   Bool_t          HLT_Mu8_DiEle12_CaloIdL_TrackIdL;
   Bool_t          HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_DZ;
   Bool_t          HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350;
   Bool_t          HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ;
   Bool_t          HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30;
   Bool_t          HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_CaloDiJet30;
   Bool_t          HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL;
   Bool_t          HLT_Mu17_TrkIsoVVL;
   Bool_t          HLT_Mu19_TrkIsoVVL;
   Bool_t          HLT_BTagMu_AK4DiJet20_Mu5;
   Bool_t          HLT_BTagMu_AK4DiJet40_Mu5;
   Bool_t          HLT_BTagMu_AK4DiJet70_Mu5;
   Bool_t          HLT_BTagMu_AK4DiJet110_Mu5;
   Bool_t          HLT_BTagMu_AK4DiJet170_Mu5;
   Bool_t          HLT_BTagMu_AK4Jet300_Mu5;
   Bool_t          HLT_BTagMu_AK8DiJet170_Mu5;
   Bool_t          HLT_BTagMu_AK8Jet170_DoubleMu5;
   Bool_t          HLT_BTagMu_AK8Jet300_Mu5;
   Bool_t          HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ;
   Bool_t          HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL;
   Bool_t          HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ;
   Bool_t          HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL;
   Bool_t          HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL;
   Bool_t          HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ;
   Bool_t          HLT_Mu12_IsoVVL_PFHT150_PNetBTag0p53;
   Bool_t          HLT_Photon33;
   Bool_t          HLT_Photon50;
   Bool_t          HLT_Photon75;
   Bool_t          HLT_Photon90;
   Bool_t          HLT_Photon120;
   Bool_t          HLT_Photon150;
   Bool_t          HLT_Photon175;
   Bool_t          HLT_Photon200;
   Bool_t          HLT_Photon45EB;
   Bool_t          HLT_Photon40EB;
   Bool_t          HLT_Photon50EB;
   Bool_t          HLT_Photon30EB_TightID_TightIso;
   Bool_t          HLT_Photon40EB_TightID_TightIso;
   Bool_t          HLT_Photon45EB_TightID_TightIso;
   Bool_t          HLT_Photon50EB_TightID_TightIso_PFJet30;
   Bool_t          HLT_Photon50EB_TightID_TightIso_CaloJet30;
   Bool_t          HLT_Photon50EB_TightID_TightIso_AK8PFJet30;
   Bool_t          HLT_Photon50EB_TightID_TightIso_AK8CaloJet30;
   Bool_t          HLT_Photon50EB_TightID_TightIso;
   Bool_t          HLT_Photon55EB_TightID_TightIso;
   Bool_t          HLT_Photon75EB_TightID_TightIso;
   Bool_t          HLT_Photon90EB_TightID_TightIso;
   Bool_t          HLT_Photon110EB_TightID_TightIso;
   Bool_t          HLT_Photon110EB_TightID_TightIso_PFJet30;
   Bool_t          HLT_Photon110EB_TightID_TightIso_CaloJet30;
   Bool_t          HLT_Photon110EB_TightID_TightIso_AK8PFJet30;
   Bool_t          HLT_Photon110EB_TightID_TightIso_AK8CaloJet30;
   Bool_t          HLT_Photon100EBHE10;
   Bool_t          HLT_Photon50_R9Id90_HE10_IsoM;
   Bool_t          HLT_Photon75_R9Id90_HE10_IsoM;
   Bool_t          HLT_Photon90_R9Id90_HE10_IsoM;
   Bool_t          HLT_Photon120_R9Id90_HE10_IsoM;
   Bool_t          HLT_Photon165_R9Id90_HE10_IsoM;
   Bool_t          HLT_Diphoton30_22_R9Id_OR_IsoCaloId_AND_HE_R9Id_Mass90;
   Bool_t          HLT_Diphoton30_22_R9Id_OR_IsoCaloId_AND_HE_R9Id_Mass95;
   Bool_t          HLT_DiphotonMVA14p25_Mass90;
   Bool_t          HLT_DiphotonMVA14p25_Tight_Mass90;
   Bool_t          HLT_Photon35_TwoProngs35;
   Bool_t          HLT_IsoMu24_TwoProngs35;
   Bool_t          HLT_Dimuon0_Jpsi_L1_NoOS;
   Bool_t          HLT_Dimuon0_Jpsi_NoVertexing_NoOS;
   Bool_t          HLT_Dimuon0_Jpsi;
   Bool_t          HLT_Dimuon0_Jpsi_NoVertexing;
   Bool_t          HLT_Dimuon0_Jpsi_L1_4R_0er1p5R;
   Bool_t          HLT_Dimuon0_Jpsi_NoVertexing_L1_4R_0er1p5R;
   Bool_t          HLT_Dimuon0_Jpsi3p5_Muon2;
   Bool_t          HLT_Dimuon0_Upsilon_L1_4p5;
   Bool_t          HLT_Dimuon0_Upsilon_L1_4p5er2p0;
   Bool_t          HLT_Dimuon0_Upsilon_L1_4p5er2p0M;
   Bool_t          HLT_Dimuon0_Upsilon_NoVertexing;
   Bool_t          HLT_Dimuon0_LowMass_L1_0er1p5;
   Bool_t          HLT_Dimuon0_LowMass;
   Bool_t          HLT_Dimuon0_LowMass_L1_4;
   Bool_t          HLT_Dimuon0_LowMass_L1_TM530;
   Bool_t          HLT_Dimuon0_Upsilon_Muon_NoL1Mass;
   Bool_t          HLT_TripleMu_5_3_3_Mass3p8_DZ;
   Bool_t          HLT_TripleMu_10_5_5_DZ;
   Bool_t          HLT_TripleMu_12_10_5;
   Bool_t          HLT_Tau3Mu_Mu7_Mu1_TkMu1_Tau15;
   Bool_t          HLT_Tau3Mu_Mu7_Mu1_TkMu1_Tau15_Charge1;
   Bool_t          HLT_Tau3Mu_Mu7_Mu1_TkMu1_IsoTau15;
   Bool_t          HLT_Tau3Mu_Mu7_Mu1_TkMu1_IsoTau15_Charge1;
   Bool_t          HLT_DoubleMu3_DZ_PFMET50_PFMHT60;
   Bool_t          HLT_DoubleMu3_DZ_PFMET70_PFMHT70;
   Bool_t          HLT_DoubleMu3_DZ_PFMET90_PFMHT90;
   Bool_t          HLT_DoubleMu3_Trk_Tau3mu_NoL1Mass;
   Bool_t          HLT_DoubleMu4_3_LowMass_SS;
   Bool_t          HLT_DoubleMu4_Jpsi_Displaced;
   Bool_t          HLT_DoubleMu4_Jpsi_NoVertexing;
   Bool_t          HLT_DoubleMu4_JpsiTrkTrk_Displaced;
   Bool_t          HLT_DoubleMu4_JpsiTrk_Bc;
   Bool_t          HLT_DoubleMu43NoFiltersNoVtx;
   Bool_t          HLT_DoubleMu48NoFiltersNoVtx;
   Bool_t          HLT_Mu43NoFiltersNoVtx_Photon43_CaloIdL;
   Bool_t          HLT_Mu48NoFiltersNoVtx_Photon48_CaloIdL;
   Bool_t          HLT_Mu38NoFiltersNoVtxDisplaced_Photon38_CaloIdL;
   Bool_t          HLT_Mu43NoFiltersNoVtxDisplaced_Photon43_CaloIdL;
   Bool_t          HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned;
   Bool_t          HLT_Ele28_eta2p1_WPTight_Gsf_HT150;
   Bool_t          HLT_Ele28_HighEta_SC20_Mass55;
   Bool_t          HLT_Ele15_IsoVVVL_PFHT450_PFMET50;
   Bool_t          HLT_Ele15_IsoVVVL_PFHT450;
   Bool_t          HLT_Ele50_IsoVVVL_PFHT450;
   Bool_t          HLT_Ele15_IsoVVVL_PFHT600;
   Bool_t          HLT_Mu15_IsoVVVL_PFHT450_PFMET50;
   Bool_t          HLT_Mu15_IsoVVVL_PFHT450;
   Bool_t          HLT_Mu50_IsoVVVL_PFHT450;
   Bool_t          HLT_Mu15_IsoVVVL_PFHT600;
   Bool_t          HLT_Mu3er1p5_PFJet100er2p5_PFMET80_PFMHT80_IDTight;
   Bool_t          HLT_Mu3er1p5_PFJet100er2p5_PFMET90_PFMHT90_IDTight;
   Bool_t          HLT_Mu3er1p5_PFJet100er2p5_PFMET100_PFMHT100_IDTight;
   Bool_t          HLT_Mu3er1p5_PFJet100er2p5_PFMETNoMu80_PFMHTNoMu80_IDTight;
   Bool_t          HLT_Mu3er1p5_PFJet100er2p5_PFMETNoMu90_PFMHTNoMu90_IDTight;
   Bool_t          HLT_Mu3er1p5_PFJet100er2p5_PFMETNoMu100_PFMHTNoMu100_IDTight;
   Bool_t          HLT_Dimuon10_Upsilon_y1p4;
   Bool_t          HLT_Dimuon12_Upsilon_y1p4;
   Bool_t          HLT_Dimuon14_Phi_Barrel_Seagulls;
   Bool_t          HLT_Dimuon25_Jpsi;
   Bool_t          HLT_Dimuon14_PsiPrime;
   Bool_t          HLT_Dimuon14_PsiPrime_noCorrL1;
   Bool_t          HLT_Dimuon18_PsiPrime;
   Bool_t          HLT_Dimuon18_PsiPrime_noCorrL1;
   Bool_t          HLT_Dimuon24_Upsilon_noCorrL1;
   Bool_t          HLT_Dimuon24_Phi_noCorrL1;
   Bool_t          HLT_Dimuon25_Jpsi_noCorrL1;
   Bool_t          HLT_DiMu4_Ele9_CaloIdL_TrackIdL_DZ_Mass3p8;
   Bool_t          HLT_DiMu9_Ele9_CaloIdL_TrackIdL_DZ;
   Bool_t          HLT_DiMu9_Ele9_CaloIdL_TrackIdL;
   Bool_t          HLT_DoubleIsoMu20_eta2p1;
   Bool_t          HLT_TrkMu12_DoubleTrkMu5NoFiltersNoVtx;
   Bool_t          HLT_Mu8;
   Bool_t          HLT_Mu17;
   Bool_t          HLT_Mu19;
   Bool_t          HLT_Mu17_Photon30_IsoCaloId;
   Bool_t          HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30;
   Bool_t          HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30;
   Bool_t          HLT_Ele14_eta2p5_IsoVVVL_Gsf_PFHT200_PNetBTag0p53;
   Bool_t          HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30;
   Bool_t          HLT_Ele8_CaloIdM_TrackIdM_PFJet30;
   Bool_t          HLT_Ele17_CaloIdM_TrackIdM_PFJet30;
   Bool_t          HLT_Ele23_CaloIdM_TrackIdM_PFJet30;
   Bool_t          HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165;
   Bool_t          HLT_Ele115_CaloIdVT_GsfTrkIdT;
   Bool_t          HLT_Ele135_CaloIdVT_GsfTrkIdT;
   Bool_t          HLT_PFHT330PT30_QuadPFJet_75_60_45_40;
   Bool_t          HLT_PFHT400_SixPFJet32;
   Bool_t          HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50;
   Bool_t          HLT_PFHT450_SixPFJet36;
   Bool_t          HLT_PFHT450_SixPFJet36_PNetBTag0p35;
   Bool_t          HLT_PFHT400_FivePFJet_120_120_60_30_30;
   Bool_t          HLT_PFHT350;
   Bool_t          HLT_Photon60_R9Id90_CaloIdL_IsoL_DisplacedIdL_PFHT350;
   Bool_t          HLT_Photon60_R9Id90_CaloIdL_IsoL_DisplacedIdL_PFHT380;
   Bool_t          HLT_Photon60_R9Id90_CaloIdL_IsoL_DisplacedIdL_PFHT400;
   Bool_t          HLT_ECALHT800;
   Bool_t          HLT_DiSC30_18_EIso_AND_HE_Mass70;
   Bool_t          HLT_Photon20_HoverELoose;
   Bool_t          HLT_Photon30_HoverELoose;
   Bool_t          HLT_L1_CDC_SingleMu_3_er1p2_TOP120_DPHI2p618_3p142;
   Bool_t          HLT_CDC_L2cosmic_10_er1p0;
   Bool_t          HLT_CDC_L2cosmic_5p5_er1p0;
   Bool_t          HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL;
   Bool_t          HLT_VBF_DoubleMediumDeepTauPFTauHPS20_eta2p1;
   Bool_t          HLT_Photon60_R9Id90_HE10_IsoM_EBOnly_PFJetsMJJ300DEta3;
   Bool_t          HLT_Photon75_R9Id90_HE10_IsoM_EBOnly_PFJetsMJJ300DEta3;
   Bool_t          HLT_Mu18_Mu9_SameSign;
   Bool_t          HLT_DoubleMu2_Jpsi_DoubleTrk1_Phi1p05;
   Bool_t          HLT_DoubleMu3_DCA_PFMET50_PFMHT60;
   Bool_t          HLT_DoubleMu3_DCA_PFMET50_PFMHT60_Mass2p0;
   Bool_t          HLT_DoubleMu3_DCA_PFMET50_PFMHT60_Mass2p0_noDCA;
   Bool_t          HLT_TripleMu_5_3_3_Mass3p8_DCA;
   Bool_t          HLT_QuadPFJet103_88_75_15;
   Bool_t          HLT_QuadPFJet105_88_76_15;
   Bool_t          HLT_QuadPFJet111_90_80_15;
   Bool_t          HLT_Diphoton30_18_R9IdL_AND_HE_AND_IsoCaloId;
   Bool_t          HLT_Diphoton30_18_R9IdL_AND_HE_AND_IsoCaloId_Mass55;
   Bool_t          HLT_DoubleMediumDeepTauPFTauHPS35_L2NN_eta2p1;
   Bool_t          HLT_Ele24_eta2p1_WPTight_Gsf_LooseDeepTauPFTauHPS30_eta2p1_CrossL1;
   Bool_t          HLT_Ele24_eta2p1_WPTight_Gsf_PNetTauhPFJet30_Tight_eta2p3_CrossL1;
   Bool_t          HLT_Ele24_eta2p1_WPTight_Gsf_PNetTauhPFJet30_Medium_eta2p3_CrossL1;
   Bool_t          HLT_Ele24_eta2p1_WPTight_Gsf_PNetTauhPFJet30_Loose_eta2p3_CrossL1;
   Bool_t          HLT_IsoMu20_eta2p1_LooseDeepTauPFTauHPS27_eta2p1_CrossL1;
   Bool_t          HLT_IsoMu24_eta2p1_LooseDeepTauPFTauHPS180_eta2p1;
   Bool_t          HLT_IsoMu24_eta2p1_LooseDeepTauPFTauHPS30_eta2p1_CrossL1;
   Bool_t          HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS35_L2NN_eta2p1_CrossL1;
   Bool_t          HLT_LooseDeepTauPFTauHPS180_L2NN_eta2p1;
   Bool_t          HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5;
   Bool_t          HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3;
   Bool_t          HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_2p0;
   Bool_t          HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_5p6;
   Bool_t          HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_4p3;
   Bool_t          HLT_QuadPFJet103_88_75_15_PNetBTag_0p4_VBF2;
   Bool_t          HLT_QuadPFJet103_88_75_15_PNet2BTag_0p4_0p12_VBF1;
   Bool_t          HLT_QuadPFJet105_88_76_15_PNetBTag_0p4_VBF2;
   Bool_t          HLT_QuadPFJet105_88_76_15_PNet2BTag_0p4_0p12_VBF1;
   Bool_t          HLT_QuadPFJet111_90_80_15_PNetBTag_0p4_VBF2;
   Bool_t          HLT_QuadPFJet111_90_80_15_PNet2BTag_0p4_0p12_VBF1;
   Bool_t          HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30_PFBtagDeepJet_1p5;
   Bool_t          HLT_PFHT250_QuadPFJet25;
   Bool_t          HLT_PFHT250_QuadPFJet25_PNet2BTagMean0p55;
   Bool_t          HLT_PFHT250_QuadPFJet25_PNet1BTag0p20_PNet1Tauh0p50;
   Bool_t          HLT_PFHT250_QuadPFJet30_PNet2BTagMean0p55;
   Bool_t          HLT_PFHT250_QuadPFJet30_PNet1BTag0p20_PNet1Tauh0p50;
   Bool_t          HLT_PFHT280_QuadPFJet30_PNet1BTag0p20_PNet1Tauh0p50;
   Bool_t          HLT_PFHT280_QuadPFJet30;
   Bool_t          HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p55;
   Bool_t          HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p60;
   Bool_t          HLT_PFHT280_QuadPFJet35_PNet2BTagMean0p60;
   Bool_t          HLT_PFHT340_QuadPFJet70_50_40_40_PNet2BTagMean0p70;
   Bool_t          HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT280;
   Bool_t          HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT280_QuadPFJet30;
   Bool_t          HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT280_QuadPFJet30_PNet2BTagMean0p55;
   Bool_t          HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30_PNet2BTagMean0p50;
   Bool_t          HLT_QuadPFJet100_88_70_30;
   Bool_t          HLT_QuadPFJet105_88_75_30;
   Bool_t          HLT_QuadPFJet111_90_80_30;
   Bool_t          HLT_QuadPFJet100_88_70_30_PNet1CvsAll0p5_VBF3Tight;
   Bool_t          HLT_QuadPFJet105_88_75_30_PNet1CvsAll0p5_VBF3Tight;
   Bool_t          HLT_QuadPFJet111_90_80_30_PNet1CvsAll0p6_VBF3Tight;
   Bool_t          HLT_AK8PFJet220_SoftDropMass40;
   Bool_t          HLT_AK8PFJet220_SoftDropMass40_PNetBB0p06_DoubleAK4PFJet60_30_PNet2BTagMean0p50;
   Bool_t          HLT_AK8PFJet220_SoftDropMass40_PNetBB0p06_DoubleAK4PFJet60_30_PNet2BTagMean0p53;
   Bool_t          HLT_AK8PFJet220_SoftDropMass40_PNetBB0p06_DoubleAK4PFJet60_30_PNet2BTagMean0p55;
   Bool_t          HLT_AK8PFJet220_SoftDropMass40_PNetBB0p06_DoubleAK4PFJet60_30_PNet2BTagMean0p60;
   Bool_t          HLT_AK8PFJet230_SoftDropMass40;
   Bool_t          HLT_AK8PFJet230_SoftDropMass40_PNetBB0p06;
   Bool_t          HLT_AK8PFJet230_SoftDropMass40_PNetBB0p10;
   Bool_t          HLT_AK8PFJet230_SoftDropMass40_PNetTauTau0p03;
   Bool_t          HLT_AK8PFJet230_SoftDropMass40_PNetTauTau0p05;
   Bool_t          HLT_AK8PFJet250_SoftDropMass40_PNetBB0p06;
   Bool_t          HLT_AK8PFJet250_SoftDropMass40_PNetBB0p10;
   Bool_t          HLT_AK8PFJet250_SoftDropMass40_PNetTauTau0p03;
   Bool_t          HLT_AK8PFJet250_SoftDropMass40_PNetTauTau0p05;
   Bool_t          HLT_AK8PFJet275_SoftDropMass40_PNetBB0p06;
   Bool_t          HLT_AK8PFJet275_SoftDropMass40_PNetBB0p10;
   Bool_t          HLT_AK8PFJet275_SoftDropMass40_PNetTauTau0p03;
   Bool_t          HLT_AK8PFJet275_SoftDropMass40_PNetTauTau0p05;
   Bool_t          HLT_AK8PFJet275_Nch45;
   Bool_t          HLT_AK8PFJet275_Nch40;
   Bool_t          HLT_IsoMu50_AK8PFJet220_SoftDropMass40;
   Bool_t          HLT_IsoMu50_AK8PFJet220_SoftDropMass40_PNetBB0p06;
   Bool_t          HLT_IsoMu50_AK8PFJet230_SoftDropMass40;
   Bool_t          HLT_IsoMu50_AK8PFJet230_SoftDropMass40_PNetBB0p06;
   Bool_t          HLT_IsoMu50_AK8PFJet230_SoftDropMass40_PNetBB0p10;
   Bool_t          HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet220_SoftDropMass40;
   Bool_t          HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet220_SoftDropMass40_PNetBB0p06;
   Bool_t          HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet230_SoftDropMass40;
   Bool_t          HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet230_SoftDropMass40_PNetBB0p06;
   Bool_t          HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet230_SoftDropMass40_PNetBB0p10;
   Bool_t          HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_DoubleAK4PFJet60_30;
   Bool_t          HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_DoubleAK4PFJet60_30_PNet2BTagMean0p50;
   Bool_t          HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT250;
   Bool_t          HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT250_QuadPFJet25;
   Bool_t          HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT250_QuadPFJet25_PNet2BTagMean0p55;
   Bool_t          HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT250_QuadPFJet25_PNet1BTag0p20;
   Bool_t          HLT_DoubleMediumDeepTauPFTauHPS30_L2NN_eta2p1_PFJet60;
   Bool_t          HLT_DoubleMediumDeepTauPFTauHPS30_L2NN_eta2p1_PFJet75;
   Bool_t          HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS30_L2NN_eta2p1_CrossL1;
   Bool_t          HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS30_L2NN_eta2p1_PFJet60_CrossL1;
   Bool_t          HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS30_L2NN_eta2p1_PFJet75_CrossL1;
   Bool_t          HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS20_eta2p1_SingleL1;
   Bool_t          HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS45_L2NN_eta2p1_CrossL1;
   Bool_t          HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Medium_L2NN_eta2p3_CrossL1;
   Bool_t          HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Tight_L2NN_eta2p3_CrossL1;
   Bool_t          HLT_IsoMu24_eta2p1_PNetTauhPFJet26_L2NN_eta2p3_CrossL1;
   Bool_t          HLT_IsoMu24_eta2p1_PNetTauhPFJet26_L2NN_eta2p3_CrossL1_PFJet60;
   Bool_t          HLT_IsoMu24_eta2p1_PNetTauhPFJet26_L2NN_eta2p3_CrossL1_PFJet75;
   Bool_t          HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Tight_eta2p3_CrossL1_ETau_Monitoring;
   Bool_t          HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Medium_eta2p3_CrossL1_ETau_Monitoring;
   Bool_t          HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Loose_eta2p3_CrossL1_ETau_Monitoring;
   Bool_t          HLT_IsoMu20_eta2p1_PNetTauhPFJet27_Tight_eta2p3_CrossL1;
   Bool_t          HLT_IsoMu20_eta2p1_PNetTauhPFJet27_Medium_eta2p3_CrossL1;
   Bool_t          HLT_IsoMu20_eta2p1_PNetTauhPFJet27_Loose_eta2p3_CrossL1;
   Bool_t          HLT_IsoMu24_eta2p1_PNetTauhPFJet20_eta2p2_SingleL1;
   Bool_t          HLT_IsoMu24_eta2p1_PNetTauhPFJet45_L2NN_eta2p3_CrossL1;
   Bool_t          HLT_IsoMu24_eta2p1_PFHT250;
   Bool_t          HLT_IsoMu24_eta2p1_PFHT250_QuadPFJet25;
   Bool_t          HLT_IsoMu24_eta2p1_PFHT250_QuadPFJet25_PNet1Tauh0p50;
   Bool_t          HLT_IsoMu24_eta2p1_SinglePFJet25_PNet1Tauh0p50;
   Bool_t          HLT_DoublePNetTauhPFJet30_Medium_L2NN_eta2p3;
   Bool_t          HLT_DoublePNetTauhPFJet30_Tight_L2NN_eta2p3;
   Bool_t          HLT_DoublePNetTauhPFJet26_L2NN_eta2p3_PFJet60;
   Bool_t          HLT_DoublePNetTauhPFJet26_L2NN_eta2p3_PFJet75;
   Bool_t          HLT_IsoMu24_eta2p1_PNetTauhPFJet130_Loose_L2NN_eta2p3_CrossL1;
   Bool_t          HLT_IsoMu24_eta2p1_PNetTauhPFJet130_Medium_L2NN_eta2p3_CrossL1;
   Bool_t          HLT_IsoMu24_eta2p1_PNetTauhPFJet130_Tight_L2NN_eta2p3_CrossL1;
   Bool_t          HLT_SinglePNetTauhPFJet130_Loose_L2NN_eta2p3;
   Bool_t          HLT_SinglePNetTauhPFJet130_Medium_L2NN_eta2p3;
   Bool_t          HLT_SinglePNetTauhPFJet130_Tight_L2NN_eta2p3;
   Bool_t          HLT_DoubleL2Mu10NoVtx_2Cha_VetoL3Mu0DxyMax1cm;
   Bool_t          HLT_DoubleL2Mu12NoVtx_2Cha_VetoL3Mu0DxyMax1cm;
   Bool_t          HLT_DoubleL2Mu14NoVtx_2Cha_VetoL3Mu0DxyMax1cm;
   Bool_t          HLT_DoubleL3Mu16_10NoVtx_DxyMin0p01cm;
   Bool_t          HLT_DoubleL3Mu18_10NoVtx_DxyMin0p01cm;
   Bool_t          HLT_DoubleL3Mu20_10NoVtx_DxyMin0p01cm;
   Bool_t          HLT_L2Mu10NoVtx_2Cha;
   Bool_t          HLT_L2Mu10NoVtx_2Cha_VetoL3Mu0DxyMax1cm;
   Bool_t          HLT_L3Mu10NoVtx;
   Bool_t          HLT_L3Mu10NoVtx_DxyMin0p01cm;
   Bool_t          HLT_DoubleL2Mu_L3Mu16NoVtx_VetoL3Mu0DxyMax0p1cm;
   Bool_t          HLT_DoubleL2Mu_L3Mu18NoVtx_VetoL3Mu0DxyMax0p1cm;
   Bool_t          HLT_DoubleL2Mu10NoVtx_2Cha_CosmicSeed_VetoL3Mu0DxyMax1cm;
   Bool_t          HLT_DoubleL2Mu12NoVtx_2Cha_CosmicSeed_VetoL3Mu0DxyMax1cm;
   Bool_t          HLT_L2Mu10NoVtx_2Cha_CosmicSeed;
   Bool_t          HLT_L2Mu10NoVtx_2Cha_CosmicSeed_VetoL3Mu0DxyMax1cm;
   Bool_t          HLT_DoubleL3dTksMu16_10NoVtx_DxyMin0p01cm;
   Bool_t          HLT_L2Mu50NoVtx_3Cha_CosmicSeed_VetoL3Mu0DxyMax1cm;
   Bool_t          HLT_L2Mu50NoVtx_3Cha_VetoL3Mu0DxyMax1cm;
   Bool_t          HLT_L3Mu30NoVtx_DxyMin0p01cm;
   Bool_t          HLT_L3Mu50NoVtx_DxyMin0p01cm;
   Bool_t          HLT_L3dTksMu10_NoVtx_DxyMin0p01cm;
   Bool_t          HLT_Mu20NoFiltersNoVtxDisplaced_Photon20_CaloCustomId;
   Bool_t          HLT_DisplacedMu24_MediumChargedIsoDisplacedPFTauHPS24;
   Bool_t          HLT_Photon34_R9Id90_CaloIdL_IsoL_DisplacedIdL_MediumChargedIsoDisplacedPFTauHPS34;
   Bool_t          HLT_DoubleMediumChargedIsoDisplacedPFTauHPS32_Trk1_eta2p1;
   Bool_t          HLT_DoubleMediumChargedIsoDisplacedPFTauHPS36_Trk1_eta2p1;
   Bool_t          HLT_DoubleMediumChargedIsoDisplacedPFTauHPS32_Trk1_eta2p1_noDxy;
   Bool_t          HLT_IsoMu27_MediumChargedIsoDisplacedPFTauHPS24_eta2p1_SingleL1;
   Bool_t          HLT_HT350_DelayedJet40_SingleDelay1p5To3p5nsInclusive;
   Bool_t          HLT_HT350_DelayedJet40_SingleDelay1p6To3p5nsInclusive;
   Bool_t          HLT_HT350_DelayedJet40_SingleDelay1p75To3p5nsInclusive;
   Bool_t          HLT_HT350_DelayedJet40_SingleDelay3nsInclusive;
   Bool_t          HLT_HT350_DelayedJet40_SingleDelay3p25nsInclusive;
   Bool_t          HLT_HT350_DelayedJet40_SingleDelay3p5nsInclusive;
   Bool_t          HLT_HT430_DelayedJet40_SingleDelay0p5nsInclusive;
   Bool_t          HLT_HT430_DelayedJet40_SingleDelay1nsInclusive;
   Bool_t          HLT_HT430_DelayedJet40_SingleDelay1To1p5nsInclusive;
   Bool_t          HLT_HT430_DelayedJet40_SingleDelay1p1To1p6nsInclusive;
   Bool_t          HLT_HT430_DelayedJet40_SingleDelay1p25To1p75nsInclusive;
   Bool_t          HLT_HT430_DelayedJet40_SingleDelay1p5nsInclusive;
   Bool_t          HLT_HT430_DelayedJet40_SingleDelay2nsInclusive;
   Bool_t          HLT_HT430_DelayedJet40_SingleDelay2p25nsInclusive;
   Bool_t          HLT_HT430_DelayedJet40_SingleDelay2p5nsInclusive;
   Bool_t          HLT_HT430_DelayedJet40_SingleDelay0p5nsTrackless;
   Bool_t          HLT_HT430_DelayedJet40_SingleDelay1nsTrackless;
   Bool_t          HLT_HT430_DelayedJet40_SingleDelay1p25nsTrackless;
   Bool_t          HLT_HT430_DelayedJet40_SingleDelay1p5nsTrackless;
   Bool_t          HLT_HT430_DelayedJet40_DoubleDelay0p5nsInclusive;
   Bool_t          HLT_HT430_DelayedJet40_DoubleDelay1nsInclusive;
   Bool_t          HLT_HT430_DelayedJet40_DoubleDelay1p25nsInclusive;
   Bool_t          HLT_HT430_DelayedJet40_DoubleDelay1p5nsInclusive;
   Bool_t          HLT_HT430_DelayedJet40_DoubleDelay0p5nsTrackless;
   Bool_t          HLT_HT430_DelayedJet40_DoubleDelay0p75nsTrackless;
   Bool_t          HLT_HT430_DelayedJet40_DoubleDelay1nsTrackless;
   Bool_t          HLT_L1Tau_DelayedJet40_SingleDelay2p5To4nsInclusive;
   Bool_t          HLT_L1Tau_DelayedJet40_SingleDelay2p6To4nsInclusive;
   Bool_t          HLT_L1Tau_DelayedJet40_SingleDelay2p75To4nsInclusive;
   Bool_t          HLT_L1Tau_DelayedJet40_SingleDelay3p5nsInclusive;
   Bool_t          HLT_L1Tau_DelayedJet40_SingleDelay3p75nsInclusive;
   Bool_t          HLT_L1Tau_DelayedJet40_SingleDelay4nsInclusive;
   Bool_t          HLT_L1Tau_DelayedJet40_SingleDelay2p5nsTrackless;
   Bool_t          HLT_L1Tau_DelayedJet40_SingleDelay2p75nsTrackless;
   Bool_t          HLT_L1Tau_DelayedJet40_SingleDelay3nsTrackless;
   Bool_t          HLT_L1Tau_DelayedJet40_DoubleDelay0p75nsInclusive;
   Bool_t          HLT_L1Tau_DelayedJet40_DoubleDelay1p25nsInclusive;
   Bool_t          HLT_L1Tau_DelayedJet40_DoubleDelay1p5nsInclusive;
   Bool_t          HLT_L1Tau_DelayedJet40_DoubleDelay1p75nsInclusive;
   Bool_t          HLT_L1Tau_DelayedJet40_DoubleDelay0p5nsTrackless;
   Bool_t          HLT_L1Tau_DelayedJet40_DoubleDelay1nsTrackless;
   Bool_t          HLT_L1Tau_DelayedJet40_DoubleDelay1p25nsTrackless;
   Bool_t          HLT_L1Tau_DelayedJet40_DoubleDelay1p5nsTrackless;
   Bool_t          HLT_L1Mu6HT240;
   Bool_t          HLT_Mu6HT240_DisplacedDijet30_Inclusive1PtrkShortSig5_DisplacedLoose;
   Bool_t          HLT_Mu6HT240_DisplacedDijet35_Inclusive0PtrkShortSig5;
   Bool_t          HLT_Mu6HT240_DisplacedDijet35_Inclusive1PtrkShortSig5_DisplacedLoose;
   Bool_t          HLT_Mu6HT240_DisplacedDijet40_Inclusive0PtrkShortSig5;
   Bool_t          HLT_Mu6HT240_DisplacedDijet40_Inclusive1PtrkShortSig5_DisplacedLoose;
   Bool_t          HLT_Mu6HT240_DisplacedDijet45_Inclusive0PtrkShortSig5;
   Bool_t          HLT_Mu6HT240_DisplacedDijet50_Inclusive0PtrkShortSig5;
   Bool_t          HLT_HT350;
   Bool_t          HLT_HT425;
   Bool_t          HLT_HT360_DisplacedDijet40_Inclusive1PtrkShortSig5;
   Bool_t          HLT_HT360_DisplacedDijet45_Inclusive1PtrkShortSig5;
   Bool_t          HLT_HT390_DisplacedDijet40_Inclusive1PtrkShortSig5;
   Bool_t          HLT_HT390_DisplacedDijet45_Inclusive1PtrkShortSig5;
   Bool_t          HLT_HT390eta2p0_DisplacedDijet40_Inclusive1PtrkShortSig5;
   Bool_t          HLT_HT430_DisplacedDijet40_Inclusive1PtrkShortSig5;
   Bool_t          HLT_HT400_DisplacedDijet40_DisplacedTrack;
   Bool_t          HLT_HT430_DisplacedDijet40_DisplacedTrack;
   Bool_t          HLT_HT550_DisplacedDijet60_Inclusive;
   Bool_t          HLT_HT650_DisplacedDijet60_Inclusive;
   Bool_t          HLT_CaloMET60_DTCluster50;
   Bool_t          HLT_CaloMET60_DTClusterNoMB1S50;
   Bool_t          HLT_L1MET_DTCluster50;
   Bool_t          HLT_L1MET_DTClusterNoMB1S50;
   Bool_t          HLT_CscCluster_Loose;
   Bool_t          HLT_CscCluster_Medium;
   Bool_t          HLT_CscCluster_Tight;
   Bool_t          HLT_CscCluster50_Photon20Unseeded;
   Bool_t          HLT_CscCluster50_Photon30Unseeded;
   Bool_t          HLT_CscCluster100_Ele5;
   Bool_t          HLT_CscCluster100_Mu5;
   Bool_t          HLT_CscCluster100_PNetTauhPFJet10_Loose;
   Bool_t          HLT_DoubleCscCluster75;
   Bool_t          HLT_IsoTrk200_L1SingleMuShower;
   Bool_t          HLT_IsoTrk400_L1SingleMuShower;
   Bool_t          HLT_DoubleCscCluster100;
   Bool_t          HLT_L1CSCShower_DTCluster50;
   Bool_t          HLT_L1CSCShower_DTCluster75;
   Bool_t          HLT_PFMET105_IsoTrk50;
   Bool_t          HLT_L1SingleLLPJet;
   Bool_t          HLT_HT170_L1SingleLLPJet_DisplacedDijet40_DisplacedTrack;
   Bool_t          HLT_HT200_L1SingleLLPJet_DisplacedDijet40_DisplacedTrack;
   Bool_t          HLT_HT200_L1SingleLLPJet_DisplacedDijet60_DisplacedTrack;
   Bool_t          HLT_HT270_L1SingleLLPJet_DisplacedDijet40_DisplacedTrack;
   Bool_t          HLT_HT200_L1SingleLLPJet_DisplacedDijet40_Inclusive1PtrkShortSig5;
   Bool_t          HLT_HT240_L1SingleLLPJet_DisplacedDijet40_Inclusive1PtrkShortSig5;
   Bool_t          HLT_HT280_L1SingleLLPJet_DisplacedDijet40_Inclusive1PtrkShortSig5;
   Bool_t          HLT_HT320_L1SingleLLPJet_DisplacedDijet60_Inclusive;
   Bool_t          HLT_HT420_L1SingleLLPJet_DisplacedDijet60_Inclusive;
   Bool_t          HLT_HT200_L1SingleLLPJet_DelayedJet40_SingleDelay1nsTrackless;
   Bool_t          HLT_HT200_L1SingleLLPJet_DelayedJet40_SingleDelay2nsInclusive;
   Bool_t          HLT_HT200_L1SingleLLPJet_DelayedJet40_DoubleDelay0p5nsTrackless;
   Bool_t          HLT_HT200_L1SingleLLPJet_DelayedJet40_DoubleDelay1nsInclusive;
   Bool_t          HLT_HT200_L1SingleLLPJet_PFJet60_NeutralHadronFrac0p7;
   Bool_t          HLT_HT200_L1SingleLLPJet_PFJet60_NeutralHadronFrac0p8;
   Bool_t          HLT_DiPhoton10Time1ns;
   Bool_t          HLT_DiPhoton10Time1p2ns;
   Bool_t          HLT_DiPhoton10Time1p4ns;
   Bool_t          HLT_DiPhoton10Time1p6ns;
   Bool_t          HLT_DiPhoton10Time1p8ns;
   Bool_t          HLT_DiPhoton10Time2ns;
   Bool_t          HLT_DiPhoton10_CaloIdL;
   Bool_t          HLT_DoubleEle6p5_eta1p22_mMax6;
   Bool_t          HLT_DoubleEle8_eta1p22_mMax6;
   Bool_t          HLT_DoubleEle10_eta1p22_mMax6;
   Bool_t          HLT_SingleEle8;
   Bool_t          HLT_SingleEle8_SingleEGL1;
   Bool_t          HLT_Diphoton20_14_eta1p5_R9IdL_AND_HE_AND_IsoTCaloIdT;
   Bool_t          HLT_Diphoton20_14_eta1p5_R9IdL_AND_HET_AND_IsoTCaloIdT;
   Bool_t          HLT_Diphoton22_14_eta1p5_R9IdL_AND_HE_AND_IsoTCaloIdT;
   Bool_t          HLT_Diphoton22_14_eta1p5_R9IdL_AND_HET_AND_IsoTCaloIdT;
   Bool_t          HLT_Diphoton24_14_eta1p5_R9IdL_AND_HET_AND_IsoTCaloIdT;
   Bool_t          HLT_Diphoton24_16_eta1p5_R9IdL_AND_HET_AND_IsoTCaloIdT;
   Bool_t          HLT_Mu50_L1SingleMuShower;
   Bool_t          HLT_IsoMu24_OneProng32;
   Bool_t          HLT_Photon32_OneProng32_M50To105;
   Bool_t          HLT_DoubleMediumDeepTauPFTauHPS30_L2NN_eta2p1_OneProng;
   Bool_t          HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS30_L2NN_eta2p1_OneProng_CrossL1;
   Bool_t          HLT_VBF_DiPFJet125_45_Mjj1050;
   Bool_t          HLT_VBF_DiPFJet125_45_Mjj1200;
   Bool_t          HLT_VBF_DiPFJet75_45_Mjj800_DiPFJet60;
   Bool_t          HLT_VBF_DiPFJet75_45_Mjj850_DiPFJet60;
   Bool_t          HLT_VBF_DiPFJet80_45_Mjj650_PFMETNoMu85;
   Bool_t          HLT_VBF_DiPFJet80_45_Mjj750_PFMETNoMu85;
   Bool_t          HLT_VBF_DiPFJet95_45_Mjj750_Mu3_TrkIsoVVL;
   Bool_t          HLT_VBF_DiPFJet95_45_Mjj850_Mu3_TrkIsoVVL;
   Bool_t          HLT_VBF_DiPFJet50_Mjj650_Photon22;
   Bool_t          HLT_VBF_DiPFJet50_Mjj750_Photon22;
   Bool_t          HLT_VBF_DiPFJet50_Mjj600_Ele22_eta2p1_WPTight_Gsf;
   Bool_t          HLT_VBF_DiPFJet50_Mjj650_Ele22_eta2p1_WPTight_Gsf;
   Bool_t          HLT_VBF_DiPFJet45_Mjj650_MediumDeepTauPFTauHPS45_L2NN_eta2p1;
   Bool_t          HLT_VBF_DiPFJet45_Mjj750_MediumDeepTauPFTauHPS45_L2NN_eta2p1;
   Bool_t          HLT_VBF_DoublePNetTauhPFJet20_eta2p2;
   Bool_t          HLT_VBF_DiPFJet45_Mjj650_PNetTauhPFJet45_L2NN_eta2p3;
   Bool_t          HLT_VBF_DiPFJet45_Mjj750_PNetTauhPFJet45_L2NN_eta2p3;
   Bool_t          HLT_PFJet200_TimeLtNeg2p5ns;
   Bool_t          HLT_PFJet200_TimeGt2p5ns;
   Bool_t          HLT_Photon50_TimeLtNeg2p5ns;
   Bool_t          HLT_Photon50_TimeGt2p5ns;
   Bool_t          HLT_PPSMaxTracksPerArm1;
   Bool_t          HLT_PPSMaxTracksPerRP4;
   Bool_t          HLT_PPSRandom;
   Bool_t          HLT_L1AXOVTight;
   Bool_t          HLTriggerFinalPath;
   Bool_t          Dataset_ScoutingPFMonitor;
   Bool_t          Dataset_ScoutingPFRun3;

   // List of branches
   TBranch        *b_run;   //!
   TBranch        *b_luminosityBlock;   //!
   TBranch        *b_event;   //!
   TBranch        *b_bunchCrossing;   //!
   TBranch        *b_orbitNumber;   //!
   TBranch        *b_nL1EG;   //!
   TBranch        *b_L1EG_hwIso;   //!
   TBranch        *b_L1EG_bx;   //!
   TBranch        *b_L1EG_eta;   //!
   TBranch        *b_L1EG_phi;   //!
   TBranch        *b_L1EG_pt;   //!
   TBranch        *b_nL1EtSum;   //!
   TBranch        *b_L1EtSum_bx;   //!
   TBranch        *b_L1EtSum_etSumType;   //!
   TBranch        *b_L1EtSum_phi;   //!
   TBranch        *b_L1EtSum_pt;   //!
   TBranch        *b_nL1Jet;   //!
   TBranch        *b_L1Jet_bx;   //!
   TBranch        *b_L1Jet_eta;   //!
   TBranch        *b_L1Jet_phi;   //!
   TBranch        *b_L1Jet_pt;   //!
   TBranch        *b_nL1Mu;   //!
   TBranch        *b_L1Mu_hwCharge;   //!
   TBranch        *b_L1Mu_hwDXY;   //!
   TBranch        *b_L1Mu_bx;   //!
   TBranch        *b_L1Mu_hwQual;   //!
   TBranch        *b_L1Mu_eta;   //!
   TBranch        *b_L1Mu_etaAtVtx;   //!
   TBranch        *b_L1Mu_phi;   //!
   TBranch        *b_L1Mu_phiAtVtx;   //!
   TBranch        *b_L1Mu_pt;   //!
   TBranch        *b_L1Mu_ptUnconstrained;   //!
   TBranch        *b_nL1Tau;   //!
   TBranch        *b_L1Tau_hwIso;   //!
   TBranch        *b_L1Tau_bx;   //!
   TBranch        *b_L1Tau_eta;   //!
   TBranch        *b_L1Tau_phi;   //!
   TBranch        *b_L1Tau_pt;   //!
   TBranch        *b_nScoutingElectron;   //!
   TBranch        *b_ScoutingElectron_rechitZeroSuppression;   //!
   TBranch        *b_ScoutingElectron_missingHits;   //!
   TBranch        *b_ScoutingElectron_bestTrack_charge;   //!
   TBranch        *b_ScoutingElectron_nClusters;   //!
   TBranch        *b_ScoutingElectron_nCrystals;   //!
   TBranch        *b_ScoutingElectron_seedId;   //!
   TBranch        *b_ScoutingElectron_corrEcalEnergyError;   //!
   TBranch        *b_ScoutingElectron_dEtaIn;   //!
   TBranch        *b_ScoutingElectron_dPhiIn;   //!
   TBranch        *b_ScoutingElectron_ecalIso;   //!
   TBranch        *b_ScoutingElectron_eta;   //!
   TBranch        *b_ScoutingElectron_hOverE;   //!
   TBranch        *b_ScoutingElectron_hcalIso;   //!
   TBranch        *b_ScoutingElectron_m;   //!
   TBranch        *b_ScoutingElectron_ooEMOop;   //!
   TBranch        *b_ScoutingElectron_phi;   //!
   TBranch        *b_ScoutingElectron_preshowerEnergy;   //!
   TBranch        *b_ScoutingElectron_pt;   //!
   TBranch        *b_ScoutingElectron_r9;   //!
   TBranch        *b_ScoutingElectron_rawEnergy;   //!
   TBranch        *b_ScoutingElectron_sMaj;   //!
   TBranch        *b_ScoutingElectron_sMin;   //!
   TBranch        *b_ScoutingElectron_sigmaIetaIeta;   //!
   TBranch        *b_ScoutingElectron_trackIso;   //!
   TBranch        *b_ScoutingElectron_trackfbrem;   //!
   TBranch        *b_ScoutingElectron_bestTrack_chi2overndf;   //!
   TBranch        *b_ScoutingElectron_bestTrack_d0;   //!
   TBranch        *b_ScoutingElectron_bestTrack_dz;   //!
   TBranch        *b_ScoutingElectron_bestTrack_eta;   //!
   TBranch        *b_ScoutingElectron_bestTrack_etaMode;   //!
   TBranch        *b_ScoutingElectron_bestTrack_pMode;   //!
   TBranch        *b_ScoutingElectron_bestTrack_phi;   //!
   TBranch        *b_ScoutingElectron_bestTrack_phiMode;   //!
   TBranch        *b_ScoutingElectron_bestTrack_pt;   //!
   TBranch        *b_ScoutingElectron_bestTrack_qoverpModeError;   //!
   TBranch        *b_nScoutingFatPFJetRecluster;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_nConstituents;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_nCh;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_nElectrons;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_nMuons;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_nNh;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_nPhotons;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_area;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_chEmEF;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_chHEF;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_eta;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_hfEmEF;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_hfHEF;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_mass;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_muEF;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_neEmEF;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_neHEF;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_phi;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_pt;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_msoftdrop;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_n2b1;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_n3b1;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_particleNet_mass;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_particleNet_prob_Hbb;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_particleNet_prob_Hcc;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_particleNet_prob_Hqq;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_particleNet_prob_QCD;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_scoutGlobalParT_massCorrGeneric;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_scoutGlobalParT_massCorrGenericW2p;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_scoutGlobalParT_massCorrGenericX2p;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_scoutGlobalParT_massCorrResonance;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_QCD;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xbb;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xbc;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xbs;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xcc;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xcs;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xgg;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xqq;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xss;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xtauhtaue;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xtauhtauh;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xtauhtaum;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xud;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_tau1;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_tau2;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_tau3;   //!
   TBranch        *b_ScoutingFatPFJetRecluster_tau4;   //!
   TBranch        *b_ScoutingMET_phi;   //!
   TBranch        *b_ScoutingMET_pt;   //!
   TBranch        *b_nScoutingMuonNoVtxDisplacedVertex;   //!
   TBranch        *b_ScoutingMuonNoVtxDisplacedVertex_isValidVtx;   //!
   TBranch        *b_ScoutingMuonNoVtxDisplacedVertex_ndof;   //!
   TBranch        *b_ScoutingMuonNoVtxDisplacedVertex_tracksSize;   //!
   TBranch        *b_ScoutingMuonNoVtxDisplacedVertex_chi2;   //!
   TBranch        *b_ScoutingMuonNoVtxDisplacedVertex_x;   //!
   TBranch        *b_ScoutingMuonNoVtxDisplacedVertex_xError;   //!
   TBranch        *b_ScoutingMuonNoVtxDisplacedVertex_xyCov;   //!
   TBranch        *b_ScoutingMuonNoVtxDisplacedVertex_xzCov;   //!
   TBranch        *b_ScoutingMuonNoVtxDisplacedVertex_y;   //!
   TBranch        *b_ScoutingMuonNoVtxDisplacedVertex_yError;   //!
   TBranch        *b_ScoutingMuonNoVtxDisplacedVertex_yzCov;   //!
   TBranch        *b_ScoutingMuonNoVtxDisplacedVertex_z;   //!
   TBranch        *b_ScoutingMuonNoVtxDisplacedVertex_zError;   //!
   TBranch        *b_nScoutingMuonNoVtx;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_hitPattern_beginInner;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_hitPattern_beginOuter;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_hitPattern_beginTrackHits;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_hitPattern_endInner;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_hitPattern_endOuter;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_hitPattern_endTrackHits;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_hitPattern_hitCount;   //!
   TBranch        *b_ScoutingMuonNoVtx_nScoutingMuonNoVtxHitPattern;   //!
   TBranch        *b_ScoutingMuonNoVtx_oScoutingMuonNoVtxHitPattern;   //!
   TBranch        *b_ScoutingMuonNoVtx_nScoutingMuonNoVtxVtxIndx;   //!
   TBranch        *b_ScoutingMuonNoVtx_oScoutingMuonNoVtxVtxIndx;   //!
   TBranch        *b_ScoutingMuonNoVtx_charge;   //!
   TBranch        *b_ScoutingMuonNoVtx_nPixelLayersWithMeasurement;   //!
   TBranch        *b_ScoutingMuonNoVtx_nRecoMuonChambers;   //!
   TBranch        *b_ScoutingMuonNoVtx_nRecoMuonChambersCSCorDT;   //!
   TBranch        *b_ScoutingMuonNoVtx_nRecoMuonExpectedMatchedStations;   //!
   TBranch        *b_ScoutingMuonNoVtx_nRecoMuonMatchedRPCLayers;   //!
   TBranch        *b_ScoutingMuonNoVtx_nRecoMuonMatchedStations;   //!
   TBranch        *b_ScoutingMuonNoVtx_nRecoMuonMatches;   //!
   TBranch        *b_ScoutingMuonNoVtx_nStandAloneMuonMatchedStations;   //!
   TBranch        *b_ScoutingMuonNoVtx_nTrackerLayersWithMeasurement;   //!
   TBranch        *b_ScoutingMuonNoVtx_nValidPixelHits;   //!
   TBranch        *b_ScoutingMuonNoVtx_nValidRecoMuonHits;   //!
   TBranch        *b_ScoutingMuonNoVtx_nValidStandAloneMuonHits;   //!
   TBranch        *b_ScoutingMuonNoVtx_nValidStripHits;   //!
   TBranch        *b_ScoutingMuonNoVtx_recoMuonRPClayerMask;   //!
   TBranch        *b_ScoutingMuonNoVtx_recoMuonStationMask;   //!
   TBranch        *b_ScoutingMuonNoVtx_type;   //!
   TBranch        *b_ScoutingMuonNoVtx_ecalIso;   //!
   TBranch        *b_ScoutingMuonNoVtx_eta;   //!
   TBranch        *b_ScoutingMuonNoVtx_hcalIso;   //!
   TBranch        *b_ScoutingMuonNoVtx_m;   //!
   TBranch        *b_ScoutingMuonNoVtx_normchi2;   //!
   TBranch        *b_ScoutingMuonNoVtx_phi;   //!
   TBranch        *b_ScoutingMuonNoVtx_pt;   //!
   TBranch        *b_ScoutingMuonNoVtx_trackIso;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_chi2;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_dsz;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_dszError;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_dxy;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_dxyError;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_dxy_dsz_cov;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_dz;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_dzError;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_eta;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_lambda;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_lambdaError;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_lambda_dsz_cov;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_lambda_dxy_cov;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_lambda_phi_cov;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_ndof;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_phi;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_phiError;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_phi_dsz_cov;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_phi_dxy_cov;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_pt;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_qoverp;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_qoverpError;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_qoverp_dsz_cov;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_qoverp_dxy_cov;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_qoverp_lambda_cov;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_qoverp_phi_cov;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_vx;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_vy;   //!
   TBranch        *b_ScoutingMuonNoVtx_trk_vz;   //!
   TBranch        *b_nScoutingMuonNoVtxHitPattern;   //!
   TBranch        *b_ScoutingMuonNoVtxHitPattern_hitPattern;   //!
   TBranch        *b_nScoutingMuonNoVtxVtxIndx;   //!
   TBranch        *b_ScoutingMuonNoVtxVtxIndx_vtxIndx;   //!
   TBranch        *b_nScoutingMuonVtxDisplacedVertex;   //!
   TBranch        *b_ScoutingMuonVtxDisplacedVertex_isValidVtx;   //!
   TBranch        *b_ScoutingMuonVtxDisplacedVertex_ndof;   //!
   TBranch        *b_ScoutingMuonVtxDisplacedVertex_tracksSize;   //!
   TBranch        *b_ScoutingMuonVtxDisplacedVertex_chi2;   //!
   TBranch        *b_ScoutingMuonVtxDisplacedVertex_x;   //!
   TBranch        *b_ScoutingMuonVtxDisplacedVertex_xError;   //!
   TBranch        *b_ScoutingMuonVtxDisplacedVertex_xyCov;   //!
   TBranch        *b_ScoutingMuonVtxDisplacedVertex_xzCov;   //!
   TBranch        *b_ScoutingMuonVtxDisplacedVertex_y;   //!
   TBranch        *b_ScoutingMuonVtxDisplacedVertex_yError;   //!
   TBranch        *b_ScoutingMuonVtxDisplacedVertex_yzCov;   //!
   TBranch        *b_ScoutingMuonVtxDisplacedVertex_z;   //!
   TBranch        *b_ScoutingMuonVtxDisplacedVertex_zError;   //!
   TBranch        *b_nScoutingMuonVtx;   //!
   TBranch        *b_ScoutingMuonVtx_trk_hitPattern_beginInner;   //!
   TBranch        *b_ScoutingMuonVtx_trk_hitPattern_beginOuter;   //!
   TBranch        *b_ScoutingMuonVtx_trk_hitPattern_beginTrackHits;   //!
   TBranch        *b_ScoutingMuonVtx_trk_hitPattern_endInner;   //!
   TBranch        *b_ScoutingMuonVtx_trk_hitPattern_endOuter;   //!
   TBranch        *b_ScoutingMuonVtx_trk_hitPattern_endTrackHits;   //!
   TBranch        *b_ScoutingMuonVtx_trk_hitPattern_hitCount;   //!
   TBranch        *b_ScoutingMuonVtx_nScoutingMuonVtxHitPattern;   //!
   TBranch        *b_ScoutingMuonVtx_oScoutingMuonVtxHitPattern;   //!
   TBranch        *b_ScoutingMuonVtx_nScoutingMuonVtxVtxIndx;   //!
   TBranch        *b_ScoutingMuonVtx_oScoutingMuonVtxVtxIndx;   //!
   TBranch        *b_ScoutingMuonVtx_charge;   //!
   TBranch        *b_ScoutingMuonVtx_nPixelLayersWithMeasurement;   //!
   TBranch        *b_ScoutingMuonVtx_nRecoMuonChambers;   //!
   TBranch        *b_ScoutingMuonVtx_nRecoMuonChambersCSCorDT;   //!
   TBranch        *b_ScoutingMuonVtx_nRecoMuonExpectedMatchedStations;   //!
   TBranch        *b_ScoutingMuonVtx_nRecoMuonMatchedRPCLayers;   //!
   TBranch        *b_ScoutingMuonVtx_nRecoMuonMatchedStations;   //!
   TBranch        *b_ScoutingMuonVtx_nRecoMuonMatches;   //!
   TBranch        *b_ScoutingMuonVtx_nStandAloneMuonMatchedStations;   //!
   TBranch        *b_ScoutingMuonVtx_nTrackerLayersWithMeasurement;   //!
   TBranch        *b_ScoutingMuonVtx_nValidPixelHits;   //!
   TBranch        *b_ScoutingMuonVtx_nValidRecoMuonHits;   //!
   TBranch        *b_ScoutingMuonVtx_nValidStandAloneMuonHits;   //!
   TBranch        *b_ScoutingMuonVtx_nValidStripHits;   //!
   TBranch        *b_ScoutingMuonVtx_recoMuonRPClayerMask;   //!
   TBranch        *b_ScoutingMuonVtx_recoMuonStationMask;   //!
   TBranch        *b_ScoutingMuonVtx_type;   //!
   TBranch        *b_ScoutingMuonVtx_ecalIso;   //!
   TBranch        *b_ScoutingMuonVtx_eta;   //!
   TBranch        *b_ScoutingMuonVtx_hcalIso;   //!
   TBranch        *b_ScoutingMuonVtx_m;   //!
   TBranch        *b_ScoutingMuonVtx_normchi2;   //!
   TBranch        *b_ScoutingMuonVtx_phi;   //!
   TBranch        *b_ScoutingMuonVtx_pt;   //!
   TBranch        *b_ScoutingMuonVtx_trackIso;   //!
   TBranch        *b_ScoutingMuonVtx_trk_chi2;   //!
   TBranch        *b_ScoutingMuonVtx_trk_dsz;   //!
   TBranch        *b_ScoutingMuonVtx_trk_dszError;   //!
   TBranch        *b_ScoutingMuonVtx_trk_dxy;   //!
   TBranch        *b_ScoutingMuonVtx_trk_dxyError;   //!
   TBranch        *b_ScoutingMuonVtx_trk_dxy_dsz_cov;   //!
   TBranch        *b_ScoutingMuonVtx_trk_dz;   //!
   TBranch        *b_ScoutingMuonVtx_trk_dzError;   //!
   TBranch        *b_ScoutingMuonVtx_trk_eta;   //!
   TBranch        *b_ScoutingMuonVtx_trk_lambda;   //!
   TBranch        *b_ScoutingMuonVtx_trk_lambdaError;   //!
   TBranch        *b_ScoutingMuonVtx_trk_lambda_dsz_cov;   //!
   TBranch        *b_ScoutingMuonVtx_trk_lambda_dxy_cov;   //!
   TBranch        *b_ScoutingMuonVtx_trk_lambda_phi_cov;   //!
   TBranch        *b_ScoutingMuonVtx_trk_ndof;   //!
   TBranch        *b_ScoutingMuonVtx_trk_phi;   //!
   TBranch        *b_ScoutingMuonVtx_trk_phiError;   //!
   TBranch        *b_ScoutingMuonVtx_trk_phi_dsz_cov;   //!
   TBranch        *b_ScoutingMuonVtx_trk_phi_dxy_cov;   //!
   TBranch        *b_ScoutingMuonVtx_trk_pt;   //!
   TBranch        *b_ScoutingMuonVtx_trk_qoverp;   //!
   TBranch        *b_ScoutingMuonVtx_trk_qoverpError;   //!
   TBranch        *b_ScoutingMuonVtx_trk_qoverp_dsz_cov;   //!
   TBranch        *b_ScoutingMuonVtx_trk_qoverp_dxy_cov;   //!
   TBranch        *b_ScoutingMuonVtx_trk_qoverp_lambda_cov;   //!
   TBranch        *b_ScoutingMuonVtx_trk_qoverp_phi_cov;   //!
   TBranch        *b_ScoutingMuonVtx_trk_vx;   //!
   TBranch        *b_ScoutingMuonVtx_trk_vy;   //!
   TBranch        *b_ScoutingMuonVtx_trk_vz;   //!
   TBranch        *b_nScoutingMuonVtxHitPattern;   //!
   TBranch        *b_ScoutingMuonVtxHitPattern_hitPattern;   //!
   TBranch        *b_nScoutingMuonVtxVtxIndx;   //!
   TBranch        *b_ScoutingMuonVtxVtxIndx_vtxIndx;   //!
   TBranch        *b_nScoutingPFJetRecluster;   //!
   TBranch        *b_ScoutingPFJetRecluster_nConstituents;   //!
   TBranch        *b_ScoutingPFJetRecluster_nCh;   //!
   TBranch        *b_ScoutingPFJetRecluster_nElectrons;   //!
   TBranch        *b_ScoutingPFJetRecluster_nMuons;   //!
   TBranch        *b_ScoutingPFJetRecluster_nNh;   //!
   TBranch        *b_ScoutingPFJetRecluster_nPhotons;   //!
   TBranch        *b_ScoutingPFJetRecluster_area;   //!
   TBranch        *b_ScoutingPFJetRecluster_chEmEF;   //!
   TBranch        *b_ScoutingPFJetRecluster_chHEF;   //!
   TBranch        *b_ScoutingPFJetRecluster_eta;   //!
   TBranch        *b_ScoutingPFJetRecluster_hfEmEF;   //!
   TBranch        *b_ScoutingPFJetRecluster_hfHEF;   //!
   TBranch        *b_ScoutingPFJetRecluster_mass;   //!
   TBranch        *b_ScoutingPFJetRecluster_muEF;   //!
   TBranch        *b_ScoutingPFJetRecluster_neEmEF;   //!
   TBranch        *b_ScoutingPFJetRecluster_neHEF;   //!
   TBranch        *b_ScoutingPFJetRecluster_phi;   //!
   TBranch        *b_ScoutingPFJetRecluster_pt;   //!
   TBranch        *b_ScoutingPFJetRecluster_particleNet_prob_b;   //!
   TBranch        *b_ScoutingPFJetRecluster_particleNet_prob_bb;   //!
   TBranch        *b_ScoutingPFJetRecluster_particleNet_prob_c;   //!
   TBranch        *b_ScoutingPFJetRecluster_particleNet_prob_cc;   //!
   TBranch        *b_ScoutingPFJetRecluster_particleNet_prob_g;   //!
   TBranch        *b_ScoutingPFJetRecluster_particleNet_prob_uds;   //!
   TBranch        *b_ScoutingPFJetRecluster_particleNet_prob_undef;   //!
   TBranch        *b_nScoutingPFJet;   //!
   TBranch        *b_ScoutingPFJet_HFEMMultiplicity;   //!
   TBranch        *b_ScoutingPFJet_HFHadronMultiplicity;   //!
   TBranch        *b_ScoutingPFJet_chargedHadronMultiplicity;   //!
   TBranch        *b_ScoutingPFJet_electronMultiplicity;   //!
   TBranch        *b_ScoutingPFJet_muonMultiplicity;   //!
   TBranch        *b_ScoutingPFJet_neutralHadronMultiplicity;   //!
   TBranch        *b_ScoutingPFJet_photonMultiplicity;   //!
   TBranch        *b_ScoutingPFJet_HFEMEnergy;   //!
   TBranch        *b_ScoutingPFJet_HFHadronEnergy;   //!
   TBranch        *b_ScoutingPFJet_HOEnergy;   //!
   TBranch        *b_ScoutingPFJet_chargedHadronEnergy;   //!
   TBranch        *b_ScoutingPFJet_electronEnergy;   //!
   TBranch        *b_ScoutingPFJet_eta;   //!
   TBranch        *b_ScoutingPFJet_jetArea;   //!
   TBranch        *b_ScoutingPFJet_m;   //!
   TBranch        *b_ScoutingPFJet_muonEnergy;   //!
   TBranch        *b_ScoutingPFJet_neutralHadronEnergy;   //!
   TBranch        *b_ScoutingPFJet_phi;   //!
   TBranch        *b_ScoutingPFJet_photonEnergy;   //!
   TBranch        *b_ScoutingPFJet_pt;   //!
   TBranch        *b_nScoutingPhoton;   //!
   TBranch        *b_ScoutingPhoton_rechitZeroSuppression;   //!
   TBranch        *b_ScoutingPhoton_nClusters;   //!
   TBranch        *b_ScoutingPhoton_nCrystals;   //!
   TBranch        *b_ScoutingPhoton_seedId;   //!
   TBranch        *b_ScoutingPhoton_corrEcalEnergyError;   //!
   TBranch        *b_ScoutingPhoton_ecalIso;   //!
   TBranch        *b_ScoutingPhoton_eta;   //!
   TBranch        *b_ScoutingPhoton_hOverE;   //!
   TBranch        *b_ScoutingPhoton_hcalIso;   //!
   TBranch        *b_ScoutingPhoton_m;   //!
   TBranch        *b_ScoutingPhoton_phi;   //!
   TBranch        *b_ScoutingPhoton_preshowerEnergy;   //!
   TBranch        *b_ScoutingPhoton_pt;   //!
   TBranch        *b_ScoutingPhoton_r9;   //!
   TBranch        *b_ScoutingPhoton_rawEnergy;   //!
   TBranch        *b_ScoutingPhoton_sMaj;   //!
   TBranch        *b_ScoutingPhoton_sMin;   //!
   TBranch        *b_ScoutingPhoton_sigmaIetaIeta;   //!
   TBranch        *b_ScoutingPhoton_trkIso;   //!
   TBranch        *b_nScoutingPrimaryVertex;   //!
   TBranch        *b_ScoutingPrimaryVertex_isValidVtx;   //!
   TBranch        *b_ScoutingPrimaryVertex_ndof;   //!
   TBranch        *b_ScoutingPrimaryVertex_tracksSize;   //!
   TBranch        *b_ScoutingPrimaryVertex_chi2;   //!
   TBranch        *b_ScoutingPrimaryVertex_x;   //!
   TBranch        *b_ScoutingPrimaryVertex_xError;   //!
   TBranch        *b_ScoutingPrimaryVertex_xyCov;   //!
   TBranch        *b_ScoutingPrimaryVertex_xzCov;   //!
   TBranch        *b_ScoutingPrimaryVertex_y;   //!
   TBranch        *b_ScoutingPrimaryVertex_yError;   //!
   TBranch        *b_ScoutingPrimaryVertex_yzCov;   //!
   TBranch        *b_ScoutingPrimaryVertex_z;   //!
   TBranch        *b_ScoutingPrimaryVertex_zError;   //!
   TBranch        *b_ScoutingRho_fixedGridRhoFastjetAll;   //!
   TBranch        *b_L1_AXO_Loose;   //!
   TBranch        *b_L1_AXO_Nominal;   //!
   TBranch        *b_L1_AXO_Tight;   //!
   TBranch        *b_L1_AXO_VLoose;   //!
   TBranch        *b_L1_AXO_VTight;   //!
   TBranch        *b_L1_AlwaysTrue;   //!
   TBranch        *b_L1_BPTX_AND_Ref1_VME;   //!
   TBranch        *b_L1_BPTX_AND_Ref3_VME;   //!
   TBranch        *b_L1_BPTX_AND_Ref4_VME;   //!
   TBranch        *b_L1_BPTX_BeamGas_B1_VME;   //!
   TBranch        *b_L1_BPTX_BeamGas_B2_VME;   //!
   TBranch        *b_L1_BPTX_BeamGas_Ref1_VME;   //!
   TBranch        *b_L1_BPTX_BeamGas_Ref2_VME;   //!
   TBranch        *b_L1_BPTX_NotOR_VME;   //!
   TBranch        *b_L1_BPTX_OR_Ref3_VME;   //!
   TBranch        *b_L1_BPTX_OR_Ref4_VME;   //!
   TBranch        *b_L1_BPTX_RefAND_VME;   //!
   TBranch        *b_L1_BptxMinus;   //!
   TBranch        *b_L1_BptxOR;   //!
   TBranch        *b_L1_BptxPlus;   //!
   TBranch        *b_L1_BptxXOR;   //!
   TBranch        *b_L1_CDC_SingleMu_3_er1p2_TOP120_DPHI2p618_3p142;   //!
   TBranch        *b_L1_CICADA_Loose;   //!
   TBranch        *b_L1_CICADA_Medium;   //!
   TBranch        *b_L1_CICADA_Tight;   //!
   TBranch        *b_L1_CICADA_VLoose;   //!
   TBranch        *b_L1_CICADA_VTight;   //!
   TBranch        *b_L1_DoubleEG11_er1p2_dR_Max0p6;   //!
   TBranch        *b_L1_DoubleEG8er2p5_HTT280er;   //!
   TBranch        *b_L1_DoubleEG8er2p5_HTT300er;   //!
   TBranch        *b_L1_DoubleEG8er2p5_HTT320er;   //!
   TBranch        *b_L1_DoubleEG_15_10_er2p5;   //!
   TBranch        *b_L1_DoubleEG_20_10_er2p5;   //!
   TBranch        *b_L1_DoubleEG_22_10_er2p5;   //!
   TBranch        *b_L1_DoubleEG_25_12_er2p5;   //!
   TBranch        *b_L1_DoubleEG_25_14_er2p5;   //!
   TBranch        *b_L1_DoubleEG_27_14_er2p5;   //!
   TBranch        *b_L1_DoubleEG_LooseIso16_LooseIso12_er1p5;   //!
   TBranch        *b_L1_DoubleEG_LooseIso18_LooseIso12_er1p5;   //!
   TBranch        *b_L1_DoubleEG_LooseIso20_LooseIso12_er1p5;   //!
   TBranch        *b_L1_DoubleEG_LooseIso22_12_er2p5;   //!
   TBranch        *b_L1_DoubleEG_LooseIso22_LooseIso12_er1p5;   //!
   TBranch        *b_L1_DoubleEG_LooseIso25_12_er2p5;   //!
   TBranch        *b_L1_DoubleEG_LooseIso25_LooseIso12_er1p5;   //!
   TBranch        *b_L1_DoubleIsoTau26er2p1_Jet55_RmOvlp_dR0p5;   //!
   TBranch        *b_L1_DoubleIsoTau26er2p1_Jet70_RmOvlp_dR0p5;   //!
   TBranch        *b_L1_DoubleIsoTau28er2p1;   //!
   TBranch        *b_L1_DoubleIsoTau30er2p1;   //!
   TBranch        *b_L1_DoubleIsoTau32er2p1;   //!
   TBranch        *b_L1_DoubleIsoTau32er2p1_Mass_Max80;   //!
   TBranch        *b_L1_DoubleIsoTau34er2p1;   //!
   TBranch        *b_L1_DoubleIsoTau35er2p1;   //!
   TBranch        *b_L1_DoubleIsoTau36er2p1;   //!
   TBranch        *b_L1_DoubleJet100er2p3_dEta_Max1p6;   //!
   TBranch        *b_L1_DoubleJet100er2p5;   //!
   TBranch        *b_L1_DoubleJet112er2p3_dEta_Max1p6;   //!
   TBranch        *b_L1_DoubleJet120er2p5;   //!
   TBranch        *b_L1_DoubleJet120er2p5_Mu3_dR_Max0p8;   //!
   TBranch        *b_L1_DoubleJet150er2p5;   //!
   TBranch        *b_L1_DoubleJet16er2p5_Mu3_dR_Max0p4;   //!
   TBranch        *b_L1_DoubleJet30er2p5_Mass_Min225_dEta_Max1p5;   //!
   TBranch        *b_L1_DoubleJet30er2p5_Mass_Min250_dEta_Max1p5;   //!
   TBranch        *b_L1_DoubleJet30er2p5_Mass_Min300_dEta_Max1p5;   //!
   TBranch        *b_L1_DoubleJet30er2p5_Mass_Min330_dEta_Max1p5;   //!
   TBranch        *b_L1_DoubleJet35er2p5_Mu3_dR_Max0p4;   //!
   TBranch        *b_L1_DoubleJet40er2p5;   //!
   TBranch        *b_L1_DoubleJet45_Mass_Min550_IsoTau45er2p1_RmOvlp_dR0p5;   //!
   TBranch        *b_L1_DoubleJet45_Mass_Min550_LooseIsoEG20er2p1_RmOvlp_dR0p2;   //!
   TBranch        *b_L1_DoubleJet45_Mass_Min600_IsoTau45er2p1_RmOvlp_dR0p5;   //!
   TBranch        *b_L1_DoubleJet45_Mass_Min600_LooseIsoEG20er2p1_RmOvlp_dR0p2;   //!
   TBranch        *b_L1_DoubleJet60er2p5_Mu3_dR_Max0p4;   //!
   TBranch        *b_L1_DoubleJet80er2p5_Mu3_dR_Max0p4;   //!
   TBranch        *b_L1_DoubleJet_110_35_DoubleJet35_Mass_Min800;   //!
   TBranch        *b_L1_DoubleJet_110_35_DoubleJet35_Mass_Min850;   //!
   TBranch        *b_L1_DoubleJet_65_35_DoubleJet35_Mass_Min600_DoubleJetCentral50;   //!
   TBranch        *b_L1_DoubleJet_65_35_DoubleJet35_Mass_Min650_DoubleJetCentral50;   //!
   TBranch        *b_L1_DoubleJet_70_35_DoubleJet35_Mass_Min500_ETMHF65;   //!
   TBranch        *b_L1_DoubleJet_70_35_DoubleJet35_Mass_Min550_ETMHF65;   //!
   TBranch        *b_L1_DoubleJet_85_35_DoubleJet35_Mass_Min600_Mu3OQ;   //!
   TBranch        *b_L1_DoubleJet_85_35_DoubleJet35_Mass_Min650_Mu3OQ;   //!
   TBranch        *b_L1_DoubleLLPJet40;   //!
   TBranch        *b_L1_DoubleLooseIsoEG22er2p1;   //!
   TBranch        *b_L1_DoubleLooseIsoEG24er2p1;   //!
   TBranch        *b_L1_DoubleMu0;   //!
   TBranch        *b_L1_DoubleMu0_Mass_Min1;   //!
   TBranch        *b_L1_DoubleMu0_OQ;   //!
   TBranch        *b_L1_DoubleMu0_SQ;   //!
   TBranch        *b_L1_DoubleMu0_SQ_OS;   //!
   TBranch        *b_L1_DoubleMu0_Upt15_Upt7;   //!
   TBranch        *b_L1_DoubleMu0_Upt5_Upt5;   //!
   TBranch        *b_L1_DoubleMu0_Upt6_IP_Min1_Upt4;   //!
   TBranch        *b_L1_DoubleMu0_Upt6_SQ_er2p0;   //!
   TBranch        *b_L1_DoubleMu0_Upt7_SQ_er2p0;   //!
   TBranch        *b_L1_DoubleMu0_Upt8_SQ_er2p0;   //!
   TBranch        *b_L1_DoubleMu0_dR_Max1p6_Jet90er2p5_dR_Max0p8;   //!
   TBranch        *b_L1_DoubleMu0er1p4_OQ_OS_dEta_Max1p6;   //!
   TBranch        *b_L1_DoubleMu0er1p4_SQ_OS_dEta_Max1p2;   //!
   TBranch        *b_L1_DoubleMu0er1p4_SQ_OS_dR_Max1p4;   //!
   TBranch        *b_L1_DoubleMu0er1p5_SQ;   //!
   TBranch        *b_L1_DoubleMu0er1p5_SQ_OS;   //!
   TBranch        *b_L1_DoubleMu0er1p5_SQ_OS_dEta_Max1p2;   //!
   TBranch        *b_L1_DoubleMu0er1p5_SQ_OS_dR_Max1p4;   //!
   TBranch        *b_L1_DoubleMu0er1p5_SQ_dR_Max1p4;   //!
   TBranch        *b_L1_DoubleMu0er2p0_SQ_OS_dEta_Max0p3_dPhi_0p8to1p2;   //!
   TBranch        *b_L1_DoubleMu0er2p0_SQ_OS_dEta_Max1p5;   //!
   TBranch        *b_L1_DoubleMu0er2p0_SQ_OS_dEta_Max1p6;   //!
   TBranch        *b_L1_DoubleMu0er2p0_SQ_dEta_Max1p5;   //!
   TBranch        *b_L1_DoubleMu0er2p0_SQ_dEta_Max1p6;   //!
   TBranch        *b_L1_DoubleMu18er2p1_SQ;   //!
   TBranch        *b_L1_DoubleMu3_OS_er2p3_Mass_Max14_DoubleEG7p5_er2p1_Mass_Max20;   //!
   TBranch        *b_L1_DoubleMu3_SQ_ETMHF30_HTT60er;   //!
   TBranch        *b_L1_DoubleMu3_SQ_ETMHF30_Jet60er2p5_OR_DoubleJet40er2p5;   //!
   TBranch        *b_L1_DoubleMu3_SQ_ETMHF40_HTT60er;   //!
   TBranch        *b_L1_DoubleMu3_SQ_ETMHF40_Jet60er2p5_OR_DoubleJet40er2p5;   //!
   TBranch        *b_L1_DoubleMu3_SQ_ETMHF50_HTT60er;   //!
   TBranch        *b_L1_DoubleMu3_SQ_ETMHF50_Jet60er2p5;   //!
   TBranch        *b_L1_DoubleMu3_SQ_ETMHF50_Jet60er2p5_OR_DoubleJet40er2p5;   //!
   TBranch        *b_L1_DoubleMu3_SQ_ETMHF60_Jet60er2p5;   //!
   TBranch        *b_L1_DoubleMu3_SQ_HTT220er;   //!
   TBranch        *b_L1_DoubleMu3er2p0_SQ_OS_dR_Max1p6;   //!
   TBranch        *b_L1_DoubleMu4_SQ_EG9er2p5;   //!
   TBranch        *b_L1_DoubleMu4_SQ_OS;   //!
   TBranch        *b_L1_DoubleMu4_SQ_OS_dR_Max1p2;   //!
   TBranch        *b_L1_DoubleMu4er2p0_SQ_OS_dR_Max1p6;   //!
   TBranch        *b_L1_DoubleMu4p5_SQ_OS;   //!
   TBranch        *b_L1_DoubleMu4p5_SQ_OS_dR_Max1p2;   //!
   TBranch        *b_L1_DoubleMu4p5er2p0_SQ_OS;   //!
   TBranch        *b_L1_DoubleMu4p5er2p0_SQ_OS_Mass_7to18;   //!
   TBranch        *b_L1_DoubleMu4p5er2p0_SQ_OS_Mass_Min7;   //!
   TBranch        *b_L1_DoubleMu5_OS_er2p3_Mass_8to14_DoubleEG3er2p1_Mass_Max20;   //!
   TBranch        *b_L1_DoubleMu5_SQ_EG9er2p5;   //!
   TBranch        *b_L1_DoubleMu5_SQ_OS_dR_Max1p6;   //!
   TBranch        *b_L1_DoubleMu6_Upt6_SQ_er2p0;   //!
   TBranch        *b_L1_DoubleMu7_Upt7_SQ_er2p0;   //!
   TBranch        *b_L1_DoubleMu8_SQ;   //!
   TBranch        *b_L1_DoubleMu8_Upt8_SQ_er2p0;   //!
   TBranch        *b_L1_DoubleMu9_SQ;   //!
   TBranch        *b_L1_DoubleMu_12_5;   //!
   TBranch        *b_L1_DoubleMu_15_5_SQ;   //!
   TBranch        *b_L1_DoubleMu_15_7;   //!
   TBranch        *b_L1_DoubleMu_15_7_Mass_Min1;   //!
   TBranch        *b_L1_DoubleMu_15_7_SQ;   //!
   TBranch        *b_L1_DoubleTau70er2p1;   //!
   TBranch        *b_L1_ETM120;   //!
   TBranch        *b_L1_ETM150;   //!
   TBranch        *b_L1_ETMHF100;   //!
   TBranch        *b_L1_ETMHF100_HTT60er;   //!
   TBranch        *b_L1_ETMHF110;   //!
   TBranch        *b_L1_ETMHF110_HTT60er;   //!
   TBranch        *b_L1_ETMHF120;   //!
   TBranch        *b_L1_ETMHF120_HTT60er;   //!
   TBranch        *b_L1_ETMHF130;   //!
   TBranch        *b_L1_ETMHF130_HTT60er;   //!
   TBranch        *b_L1_ETMHF140;   //!
   TBranch        *b_L1_ETMHF150;   //!
   TBranch        *b_L1_ETMHF70;   //!
   TBranch        *b_L1_ETMHF70_HTT60er;   //!
   TBranch        *b_L1_ETMHF80;   //!
   TBranch        *b_L1_ETMHF80_HTT60er;   //!
   TBranch        *b_L1_ETMHF80_SingleJet55er2p5_dPhi_Min2p1;   //!
   TBranch        *b_L1_ETMHF80_SingleJet55er2p5_dPhi_Min2p6;   //!
   TBranch        *b_L1_ETMHF90;   //!
   TBranch        *b_L1_ETMHF90_HTT60er;   //!
   TBranch        *b_L1_ETMHF90_SingleJet60er2p5_dPhi_Min2p1;   //!
   TBranch        *b_L1_ETMHF90_SingleJet60er2p5_dPhi_Min2p6;   //!
   TBranch        *b_L1_ETT2000;   //!
   TBranch        *b_L1_FirstBunchAfterTrain;   //!
   TBranch        *b_L1_FirstBunchBeforeTrain;   //!
   TBranch        *b_L1_FirstBunchInTrain;   //!
   TBranch        *b_L1_FirstCollisionInOrbit;   //!
   TBranch        *b_L1_FirstCollisionInTrain;   //!
   TBranch        *b_L1_HCAL_LaserMon_Trig;   //!
   TBranch        *b_L1_HCAL_LaserMon_Veto;   //!
   TBranch        *b_L1_HTMHF100;   //!
   TBranch        *b_L1_HTMHF120;   //!
   TBranch        *b_L1_HTMHF125;   //!
   TBranch        *b_L1_HTMHF130;   //!
   TBranch        *b_L1_HTMHF150;   //!
   TBranch        *b_L1_HTT120_SingleLLPJet40;   //!
   TBranch        *b_L1_HTT120er;   //!
   TBranch        *b_L1_HTT160_SingleLLPJet50;   //!
   TBranch        *b_L1_HTT160er;   //!
   TBranch        *b_L1_HTT200_SingleLLPJet60;   //!
   TBranch        *b_L1_HTT200er;   //!
   TBranch        *b_L1_HTT240_SingleLLPJet70;   //!
   TBranch        *b_L1_HTT255er;   //!
   TBranch        *b_L1_HTT280er;   //!
   TBranch        *b_L1_HTT280er_QuadJet_70_55_40_35_er2p5;   //!
   TBranch        *b_L1_HTT320er;   //!
   TBranch        *b_L1_HTT320er_QuadJet_70_55_40_40_er2p5;   //!
   TBranch        *b_L1_HTT320er_QuadJet_80_60_er2p1_45_40_er2p3;   //!
   TBranch        *b_L1_HTT320er_QuadJet_80_60_er2p1_50_45_er2p3;   //!
   TBranch        *b_L1_HTT360er;   //!
   TBranch        *b_L1_HTT400er;   //!
   TBranch        *b_L1_HTT450er;   //!
   TBranch        *b_L1_IsoEG32er2p5_Mt40;   //!
   TBranch        *b_L1_IsoTau52er2p1_QuadJet36er2p5;   //!
   TBranch        *b_L1_IsolatedBunch;   //!
   TBranch        *b_L1_LastBunchInTrain;   //!
   TBranch        *b_L1_LastCollisionInTrain;   //!
   TBranch        *b_L1_LooseIsoEG14er2p5_HTT200er;   //!
   TBranch        *b_L1_LooseIsoEG16er2p5_HTT200er;   //!
   TBranch        *b_L1_LooseIsoEG22er2p1_IsoTau26er2p1_dR_Min0p3;   //!
   TBranch        *b_L1_LooseIsoEG22er2p1_Tau70er2p1_dR_Min0p3;   //!
   TBranch        *b_L1_LooseIsoEG24er2p1_HTT100er;   //!
   TBranch        *b_L1_LooseIsoEG24er2p1_IsoTau27er2p1_dR_Min0p3;   //!
   TBranch        *b_L1_LooseIsoEG26er2p1_HTT100er;   //!
   TBranch        *b_L1_LooseIsoEG26er2p1_Jet34er2p5_dR_Min0p3;   //!
   TBranch        *b_L1_LooseIsoEG28er2p1_HTT100er;   //!
   TBranch        *b_L1_LooseIsoEG28er2p1_Jet34er2p5_dR_Min0p3;   //!
   TBranch        *b_L1_LooseIsoEG30er2p1_Jet34er2p5_dR_Min0p3;   //!
   TBranch        *b_L1_MinimumBiasHF0;   //!
   TBranch        *b_L1_MinimumBiasHF0_AND_BptxAND;   //!
   TBranch        *b_L1_Mu10er2p3_Jet32er2p3_dR_Max0p4_DoubleJet32er2p3_dEta_Max1p6;   //!
   TBranch        *b_L1_Mu12_HTT150er;   //!
   TBranch        *b_L1_Mu12er2p3_Jet40er2p3_dR_Max0p4_DoubleJet40er2p3_dEta_Max1p6;   //!
   TBranch        *b_L1_Mu14_HTT150er;   //!
   TBranch        *b_L1_Mu18er2p1_Tau24er2p1;   //!
   TBranch        *b_L1_Mu18er2p1_Tau26er2p1;   //!
   TBranch        *b_L1_Mu18er2p1_Tau26er2p1_Jet55;   //!
   TBranch        *b_L1_Mu18er2p1_Tau26er2p1_Jet70;   //!
   TBranch        *b_L1_Mu20_EG10er2p5;   //!
   TBranch        *b_L1_Mu22er2p1_IsoTau30er2p1;   //!
   TBranch        *b_L1_Mu22er2p1_IsoTau32er2p1;   //!
   TBranch        *b_L1_Mu22er2p1_IsoTau34er2p1;   //!
   TBranch        *b_L1_Mu22er2p1_IsoTau40er2p1;   //!
   TBranch        *b_L1_Mu22er2p1_Tau70er2p1;   //!
   TBranch        *b_L1_Mu3_Jet120er2p5_dR_Max0p4;   //!
   TBranch        *b_L1_Mu3_Jet16er2p5_dR_Max0p4;   //!
   TBranch        *b_L1_Mu3_Jet30er2p5;   //!
   TBranch        *b_L1_Mu3_Jet60er2p5_dR_Max0p4;   //!
   TBranch        *b_L1_Mu3er1p5_Jet100er2p5_ETMHF30;   //!
   TBranch        *b_L1_Mu3er1p5_Jet100er2p5_ETMHF40;   //!
   TBranch        *b_L1_Mu3er1p5_Jet100er2p5_ETMHF50;   //!
   TBranch        *b_L1_Mu5_EG23er2p5;   //!
   TBranch        *b_L1_Mu5_LooseIsoEG20er2p5;   //!
   TBranch        *b_L1_Mu6_DoubleEG10er2p5;   //!
   TBranch        *b_L1_Mu6_DoubleEG12er2p5;   //!
   TBranch        *b_L1_Mu6_DoubleEG15er2p5;   //!
   TBranch        *b_L1_Mu6_DoubleEG17er2p5;   //!
   TBranch        *b_L1_Mu6_HTT240er;   //!
   TBranch        *b_L1_Mu6_HTT250er;   //!
   TBranch        *b_L1_Mu7_EG20er2p5;   //!
   TBranch        *b_L1_Mu7_EG23er2p5;   //!
   TBranch        *b_L1_Mu7_LooseIsoEG20er2p5;   //!
   TBranch        *b_L1_Mu7_LooseIsoEG23er2p5;   //!
   TBranch        *b_L1_NotBptxOR;   //!
   TBranch        *b_L1_QuadJet60er2p5;   //!
   TBranch        *b_L1_QuadJet_95_75_65_20_DoubleJet_75_65_er2p5_Jet20_FWD3p0;   //!
   TBranch        *b_L1_QuadMu0;   //!
   TBranch        *b_L1_QuadMu0_OQ;   //!
   TBranch        *b_L1_QuadMu0_SQ;   //!
   TBranch        *b_L1_SecondBunchInTrain;   //!
   TBranch        *b_L1_SecondLastBunchInTrain;   //!
   TBranch        *b_L1_SingleEG10er2p5;   //!
   TBranch        *b_L1_SingleEG15er2p5;   //!
   TBranch        *b_L1_SingleEG26er2p5;   //!
   TBranch        *b_L1_SingleEG28_FWD2p5;   //!
   TBranch        *b_L1_SingleEG28er1p5;   //!
   TBranch        *b_L1_SingleEG28er2p1;   //!
   TBranch        *b_L1_SingleEG28er2p5;   //!
   TBranch        *b_L1_SingleEG34er2p5;   //!
   TBranch        *b_L1_SingleEG36er2p5;   //!
   TBranch        *b_L1_SingleEG38er2p5;   //!
   TBranch        *b_L1_SingleEG40er2p5;   //!
   TBranch        *b_L1_SingleEG42er2p5;   //!
   TBranch        *b_L1_SingleEG45er2p5;   //!
   TBranch        *b_L1_SingleEG50;   //!
   TBranch        *b_L1_SingleEG60;   //!
   TBranch        *b_L1_SingleEG8er2p5;   //!
   TBranch        *b_L1_SingleIsoEG24er2p1;   //!
   TBranch        *b_L1_SingleIsoEG26er2p1;   //!
   TBranch        *b_L1_SingleIsoEG26er2p5;   //!
   TBranch        *b_L1_SingleIsoEG28_FWD2p5;   //!
   TBranch        *b_L1_SingleIsoEG28er1p5;   //!
   TBranch        *b_L1_SingleIsoEG28er2p1;   //!
   TBranch        *b_L1_SingleIsoEG28er2p5;   //!
   TBranch        *b_L1_SingleIsoEG30er2p1;   //!
   TBranch        *b_L1_SingleIsoEG30er2p5;   //!
   TBranch        *b_L1_SingleIsoEG32er2p1;   //!
   TBranch        *b_L1_SingleIsoEG32er2p5;   //!
   TBranch        *b_L1_SingleIsoEG34er2p5;   //!
   TBranch        *b_L1_SingleJet10erHE;   //!
   TBranch        *b_L1_SingleJet120;   //!
   TBranch        *b_L1_SingleJet120_FWD2p5;   //!
   TBranch        *b_L1_SingleJet120_FWD3p0;   //!
   TBranch        *b_L1_SingleJet120er1p3;   //!
   TBranch        *b_L1_SingleJet120er2p5;   //!
   TBranch        *b_L1_SingleJet12erHE;   //!
   TBranch        *b_L1_SingleJet140er2p5;   //!
   TBranch        *b_L1_SingleJet160er2p5;   //!
   TBranch        *b_L1_SingleJet180;   //!
   TBranch        *b_L1_SingleJet180er2p5;   //!
   TBranch        *b_L1_SingleJet200;   //!
   TBranch        *b_L1_SingleJet20er2p5_NotBptxOR;   //!
   TBranch        *b_L1_SingleJet20er2p5_NotBptxOR_3BX;   //!
   TBranch        *b_L1_SingleJet35;   //!
   TBranch        *b_L1_SingleJet35_FWD2p5;   //!
   TBranch        *b_L1_SingleJet35_FWD3p0;   //!
   TBranch        *b_L1_SingleJet35er1p3;   //!
   TBranch        *b_L1_SingleJet35er2p5;   //!
   TBranch        *b_L1_SingleJet43er2p5_NotBptxOR_3BX;   //!
   TBranch        *b_L1_SingleJet46er2p5_NotBptxOR_3BX;   //!
   TBranch        *b_L1_SingleJet60;   //!
   TBranch        *b_L1_SingleJet60_FWD2p5;   //!
   TBranch        *b_L1_SingleJet8erHE;   //!
   TBranch        *b_L1_SingleJet90;   //!
   TBranch        *b_L1_SingleJet90_FWD2p5;   //!
   TBranch        *b_L1_SingleLooseIsoEG26er1p5;   //!
   TBranch        *b_L1_SingleLooseIsoEG26er2p5;   //!
   TBranch        *b_L1_SingleLooseIsoEG28_FWD2p5;   //!
   TBranch        *b_L1_SingleLooseIsoEG28er1p5;   //!
   TBranch        *b_L1_SingleLooseIsoEG28er2p1;   //!
   TBranch        *b_L1_SingleLooseIsoEG28er2p5;   //!
   TBranch        *b_L1_SingleLooseIsoEG30er1p5;   //!
   TBranch        *b_L1_SingleLooseIsoEG30er2p5;   //!
   TBranch        *b_L1_SingleMu0_BMTF;   //!
   TBranch        *b_L1_SingleMu0_DQ;   //!
   TBranch        *b_L1_SingleMu0_EMTF;   //!
   TBranch        *b_L1_SingleMu0_OMTF;   //!
   TBranch        *b_L1_SingleMu0_SQ13_BMTF;   //!
   TBranch        *b_L1_SingleMu0_SQ14_BMTF;   //!
   TBranch        *b_L1_SingleMu0_SQ15_BMTF;   //!
   TBranch        *b_L1_SingleMu0_Upt10;   //!
   TBranch        *b_L1_SingleMu0_Upt10_BMTF;   //!
   TBranch        *b_L1_SingleMu0_Upt10_EMTF;   //!
   TBranch        *b_L1_SingleMu0_Upt10_OMTF;   //!
   TBranch        *b_L1_SingleMu0_Upt10_SQ14_BMTF;   //!
   TBranch        *b_L1_SingleMu0_Upt15_SQ14_BMTF;   //!
   TBranch        *b_L1_SingleMu0_Upt20_SQ14_BMTF;   //!
   TBranch        *b_L1_SingleMu0_Upt25_SQ14_BMTF;   //!
   TBranch        *b_L1_SingleMu10_SQ14_BMTF;   //!
   TBranch        *b_L1_SingleMu11_SQ14_BMTF;   //!
   TBranch        *b_L1_SingleMu12_DQ_BMTF;   //!
   TBranch        *b_L1_SingleMu12_DQ_EMTF;   //!
   TBranch        *b_L1_SingleMu12_DQ_OMTF;   //!
   TBranch        *b_L1_SingleMu15_DQ;   //!
   TBranch        *b_L1_SingleMu18;   //!
   TBranch        *b_L1_SingleMu20;   //!
   TBranch        *b_L1_SingleMu22;   //!
   TBranch        *b_L1_SingleMu22_BMTF;   //!
   TBranch        *b_L1_SingleMu22_BMTF_NEG;   //!
   TBranch        *b_L1_SingleMu22_BMTF_POS;   //!
   TBranch        *b_L1_SingleMu22_DQ;   //!
   TBranch        *b_L1_SingleMu22_EMTF;   //!
   TBranch        *b_L1_SingleMu22_EMTF_NEG;   //!
   TBranch        *b_L1_SingleMu22_EMTF_POS;   //!
   TBranch        *b_L1_SingleMu22_OMTF;   //!
   TBranch        *b_L1_SingleMu22_OMTF_NEG;   //!
   TBranch        *b_L1_SingleMu22_OMTF_POS;   //!
   TBranch        *b_L1_SingleMu22_OQ;   //!
   TBranch        *b_L1_SingleMu25;   //!
   TBranch        *b_L1_SingleMu3;   //!
   TBranch        *b_L1_SingleMu5;   //!
   TBranch        *b_L1_SingleMu5_SQ14_BMTF;   //!
   TBranch        *b_L1_SingleMu6_SQ14_BMTF;   //!
   TBranch        *b_L1_SingleMu7;   //!
   TBranch        *b_L1_SingleMu7_DQ;   //!
   TBranch        *b_L1_SingleMu7_SQ14_BMTF;   //!
   TBranch        *b_L1_SingleMu8_SQ14_BMTF;   //!
   TBranch        *b_L1_SingleMu9_SQ14_BMTF;   //!
   TBranch        *b_L1_SingleMuCosmics;   //!
   TBranch        *b_L1_SingleMuCosmics_BMTF;   //!
   TBranch        *b_L1_SingleMuCosmics_EMTF;   //!
   TBranch        *b_L1_SingleMuCosmics_OMTF;   //!
   TBranch        *b_L1_SingleMuOpen;   //!
   TBranch        *b_L1_SingleMuOpen_BMTF;   //!
   TBranch        *b_L1_SingleMuOpen_EMTF;   //!
   TBranch        *b_L1_SingleMuOpen_NotBptxOR;   //!
   TBranch        *b_L1_SingleMuOpen_OMTF;   //!
   TBranch        *b_L1_SingleMuOpen_er1p1_NotBptxOR_3BX;   //!
   TBranch        *b_L1_SingleMuOpen_er1p4_NotBptxOR_3BX;   //!
   TBranch        *b_L1_SingleMuShower_Nominal;   //!
   TBranch        *b_L1_SingleMuShower_Tight;   //!
   TBranch        *b_L1_SingleTau120er2p1;   //!
   TBranch        *b_L1_SingleTau130er2p1;   //!
   TBranch        *b_L1_TOTEM_1;   //!
   TBranch        *b_L1_TOTEM_2;   //!
   TBranch        *b_L1_TOTEM_3;   //!
   TBranch        *b_L1_TOTEM_4;   //!
   TBranch        *b_L1_TripleEG_18_17_8_er2p5;   //!
   TBranch        *b_L1_TripleEG_18_18_12_er2p5;   //!
   TBranch        *b_L1_TripleJet_100_80_70_DoubleJet_80_70_er2p5;   //!
   TBranch        *b_L1_TripleJet_105_85_75_DoubleJet_85_75_er2p5;   //!
   TBranch        *b_L1_TripleJet_95_75_65_DoubleJet_75_65_er2p5;   //!
   TBranch        *b_L1_TripleMu0;   //!
   TBranch        *b_L1_TripleMu0_OQ;   //!
   TBranch        *b_L1_TripleMu0_SQ;   //!
   TBranch        *b_L1_TripleMu3;   //!
   TBranch        *b_L1_TripleMu3_SQ;   //!
   TBranch        *b_L1_TripleMu_3SQ_2p5SQ_0;   //!
   TBranch        *b_L1_TripleMu_3SQ_2p5SQ_0_Mass_Max12;   //!
   TBranch        *b_L1_TripleMu_3SQ_2p5SQ_0_OS_Mass_Max12;   //!
   TBranch        *b_L1_TripleMu_4SQ_2p5SQ_0_OS_Mass_Max12;   //!
   TBranch        *b_L1_TripleMu_5SQ_3SQ_0OQ;   //!
   TBranch        *b_L1_TripleMu_5SQ_3SQ_0OQ_DoubleMu_5_3_SQ_OS_Mass_Max9;   //!
   TBranch        *b_L1_TripleMu_5SQ_3SQ_0_DoubleMu_5_3_SQ_OS_Mass_Max9;   //!
   TBranch        *b_L1_TripleMu_5_3_3;   //!
   TBranch        *b_L1_TripleMu_5_3_3_SQ;   //!
   TBranch        *b_L1_TripleMu_5_3p5_2p5;   //!
   TBranch        *b_L1_TripleMu_5_3p5_2p5_DoubleMu_5_2p5_OS_Mass_5to17;   //!
   TBranch        *b_L1_TripleMu_5_4_2p5_DoubleMu_5_2p5_OS_Mass_5to17;   //!
   TBranch        *b_L1_TripleMu_5_5_3;   //!
   TBranch        *b_L1_TwoMuShower_Loose;   //!
   TBranch        *b_L1_UnpairedBunchBptxMinus;   //!
   TBranch        *b_L1_UnpairedBunchBptxPlus;   //!
   TBranch        *b_L1_ZeroBias;   //!
   TBranch        *b_L1_ZeroBias_copy;   //!
   TBranch        *b_L1_UnprefireableEvent_TriggerRules;   //!
   TBranch        *b_L1_UnprefireableEvent_FirstBxInTrain;   //!
   TBranch        *b_L1_FinalOR_BXmin1;   //!
   TBranch        *b_L1_FinalOR_BXmin2;   //!
   TBranch        *b_HLTriggerFirstPath;   //!
   TBranch        *b_DST_PFScouting_DoubleMuon;   //!
   TBranch        *b_DST_PFScouting_DoubleEG;   //!
   TBranch        *b_DST_PFScouting_JetHT;   //!
   TBranch        *b_DST_PFScouting_DatasetMuon;   //!
   TBranch        *b_DST_PFScouting_AXOVLoose;   //!
   TBranch        *b_DST_PFScouting_AXOLoose;   //!
   TBranch        *b_DST_PFScouting_AXONominal;   //!
   TBranch        *b_DST_PFScouting_AXOTight;   //!
   TBranch        *b_DST_PFScouting_AXOVTight;   //!
   TBranch        *b_DST_PFScouting_CICADAVLoose;   //!
   TBranch        *b_DST_PFScouting_CICADALoose;   //!
   TBranch        *b_DST_PFScouting_CICADAMedium;   //!
   TBranch        *b_DST_PFScouting_CICADATight;   //!
   TBranch        *b_DST_PFScouting_CICADAVTight;   //!
   TBranch        *b_DST_PFScouting_SingleMuon;   //!
   TBranch        *b_DST_PFScouting_SinglePhotonEB;   //!
   TBranch        *b_DST_PFScouting_ZeroBias;   //!
   TBranch        *b_HLT_EphemeralPhysics;   //!
   TBranch        *b_HLT_EphemeralZeroBias;   //!
   TBranch        *b_HLT_EcalCalibration;   //!
   TBranch        *b_HLT_HcalCalibration;   //!
   TBranch        *b_HLT_HcalNZS;   //!
   TBranch        *b_HLT_HcalPhiSym;   //!
   TBranch        *b_HLT_Random;   //!
   TBranch        *b_HLT_Physics;   //!
   TBranch        *b_HLT_ZeroBias;   //!
   TBranch        *b_HLT_ZeroBias_Alignment;   //!
   TBranch        *b_HLT_ZeroBias_Beamspot;   //!
   TBranch        *b_HLT_ZeroBias_IsolatedBunches;   //!
   TBranch        *b_HLT_ZeroBias_FirstBXAfterTrain;   //!
   TBranch        *b_HLT_ZeroBias_FirstCollisionAfterAbortGap;   //!
   TBranch        *b_HLT_ZeroBias_FirstCollisionInTrain;   //!
   TBranch        *b_HLT_ZeroBias_LastCollisionInTrain;   //!
   TBranch        *b_HLT_HT300_Beamspot;   //!
   TBranch        *b_HLT_IsoTrackHB;   //!
   TBranch        *b_HLT_IsoTrackHE;   //!
   TBranch        *b_HLT_PFJet40_GPUvsCPU;   //!
   TBranch        *b_HLT_AK8PFJet380_SoftDropMass30;   //!
   TBranch        *b_HLT_AK8PFJet400_SoftDropMass30;   //!
   TBranch        *b_HLT_AK8PFJet425_SoftDropMass30;   //!
   TBranch        *b_HLT_AK8PFJet450_SoftDropMass30;   //!
   TBranch        *b_HLT_AK8DiPFJet250_250_SoftDropMass40;   //!
   TBranch        *b_HLT_AK8DiPFJet250_250_SoftDropMass50;   //!
   TBranch        *b_HLT_AK8DiPFJet260_260_SoftDropMass30;   //!
   TBranch        *b_HLT_AK8DiPFJet260_260_SoftDropMass40;   //!
   TBranch        *b_HLT_AK8DiPFJet270_270_SoftDropMass30;   //!
   TBranch        *b_HLT_AK8DiPFJet280_280_SoftDropMass30;   //!
   TBranch        *b_HLT_AK8DiPFJet290_290_SoftDropMass30;   //!
   TBranch        *b_HLT_CaloJet500_NoJetID;   //!
   TBranch        *b_HLT_CaloJet550_NoJetID;   //!
   TBranch        *b_HLT_DoubleMu5_Upsilon_DoubleEle3_CaloIdL_TrackIdL;   //!
   TBranch        *b_HLT_DoubleMu3_DoubleEle7p5_CaloIdL_TrackIdL_Upsilon;   //!
   TBranch        *b_HLT_Trimuon5_3p5_2_Upsilon_Muon;   //!
   TBranch        *b_HLT_TrimuonOpen_5_3p5_2_Upsilon_Muon;   //!
   TBranch        *b_HLT_DoubleEle25_CaloIdL_MW;   //!
   TBranch        *b_HLT_DoubleEle27_CaloIdL_MW;   //!
   TBranch        *b_HLT_DoubleEle33_CaloIdL_MW;   //!
   TBranch        *b_HLT_DoubleEle24_eta2p1_WPTight_Gsf;   //!
   TBranch        *b_HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_DZ_PFHT350;   //!
   TBranch        *b_HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_PFHT350;   //!
   TBranch        *b_HLT_Mu27_Ele37_CaloIdL_MW;   //!
   TBranch        *b_HLT_Mu37_Ele27_CaloIdL_MW;   //!
   TBranch        *b_HLT_Mu37_TkMu27;   //!
   TBranch        *b_HLT_DoubleMu4_3_Bs;   //!
   TBranch        *b_HLT_DoubleMu4_3_Jpsi;   //!
   TBranch        *b_HLT_DoubleMu4_3_LowMass;   //!
   TBranch        *b_HLT_DoubleMu4_LowMass_Displaced;   //!
   TBranch        *b_HLT_Mu0_L1DoubleMu;   //!
   TBranch        *b_HLT_Mu4_L1DoubleMu;   //!
   TBranch        *b_HLT_DoubleMu2_Jpsi_LowPt;   //!
   TBranch        *b_HLT_DoubleMu4_3_Photon4_BsToMMG;   //!
   TBranch        *b_HLT_DoubleMu4_3_Displaced_Photon4_BsToMMG;   //!
   TBranch        *b_HLT_DoubleMu3_Trk_Tau3mu;   //!
   TBranch        *b_HLT_DoubleMu3_TkMu_DsTau3Mu;   //!
   TBranch        *b_HLT_DoubleMu4_Mass3p8_DZ_PFHT350;   //!
   TBranch        *b_HLT_DoubleMu4_MuMuTrk_Displaced;   //!
   TBranch        *b_HLT_Mu3_PFJet40;   //!
   TBranch        *b_HLT_Mu7p5_L2Mu2_Jpsi;   //!
   TBranch        *b_HLT_Mu7p5_L2Mu2_Upsilon;   //!
   TBranch        *b_HLT_Mu3_L1SingleMu5orSingleMu7;   //!
   TBranch        *b_HLT_Mu0_Barrel;   //!
   TBranch        *b_HLT_Mu0_Barrel_L1HP6;   //!
   TBranch        *b_HLT_Mu0_Barrel_L1HP7;   //!
   TBranch        *b_HLT_Mu0_Barrel_L1HP8;   //!
   TBranch        *b_HLT_Mu0_Barrel_L1HP9;   //!
   TBranch        *b_HLT_Mu0_Barrel_L1HP10;   //!
   TBranch        *b_HLT_Mu0_Barrel_L1HP11;   //!
   TBranch        *b_HLT_Mu0_Barrel_L1HP6_IP6;   //!
   TBranch        *b_HLT_Mu6_Barrel_L1HP7_IP6;   //!
   TBranch        *b_HLT_Mu7_Barrel_L1HP8_IP6;   //!
   TBranch        *b_HLT_Mu8_Barrel_L1HP9_IP6;   //!
   TBranch        *b_HLT_Mu9_Barrel_L1HP10_IP6;   //!
   TBranch        *b_HLT_Mu10_Barrel_L1HP11_IP6;   //!
   TBranch        *b_HLT_DoublePhoton33_CaloIdL;   //!
   TBranch        *b_HLT_DoublePhoton70;   //!
   TBranch        *b_HLT_DoublePhoton85;   //!
   TBranch        *b_HLT_DiEle27_WPTightCaloOnly_L1DoubleEG;   //!
   TBranch        *b_HLT_Ele30_WPTight_Gsf;   //!
   TBranch        *b_HLT_Ele32_WPTight_Gsf;   //!
   TBranch        *b_HLT_Ele35_WPTight_Gsf;   //!
   TBranch        *b_HLT_Ele38_WPTight_Gsf;   //!
   TBranch        *b_HLT_Ele40_WPTight_Gsf;   //!
   TBranch        *b_HLT_Ele32_WPTight_Gsf_L1DoubleEG;   //!
   TBranch        *b_HLT_IsoMu20;   //!
   TBranch        *b_HLT_IsoMu24;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1;   //!
   TBranch        *b_HLT_IsoMu27;   //!
   TBranch        *b_HLT_UncorrectedJetE30_NoBPTX;   //!
   TBranch        *b_HLT_UncorrectedJetE30_NoBPTX3BX;   //!
   TBranch        *b_HLT_UncorrectedJetE60_NoBPTX3BX;   //!
   TBranch        *b_HLT_UncorrectedJetE70_NoBPTX3BX;   //!
   TBranch        *b_HLT_L1SingleMuCosmics;   //!
   TBranch        *b_HLT_L2Mu10_NoVertex_NoBPTX3BX;   //!
   TBranch        *b_HLT_L2Mu10_NoVertex_NoBPTX;   //!
   TBranch        *b_HLT_L2Mu45_NoVertex_3Sta_NoBPTX3BX;   //!
   TBranch        *b_HLT_L2Mu40_NoVertex_3Sta_NoBPTX3BX;   //!
   TBranch        *b_HLT_L2Mu23NoVtx_2Cha;   //!
   TBranch        *b_HLT_L2Mu23NoVtx_2Cha_CosmicSeed;   //!
   TBranch        *b_HLT_DoubleL2Mu30NoVtx_2Cha_CosmicSeed_Eta2p4;   //!
   TBranch        *b_HLT_DoubleL2Mu30NoVtx_2Cha_Eta2p4;   //!
   TBranch        *b_HLT_DoubleL2Mu50;   //!
   TBranch        *b_HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed;   //!
   TBranch        *b_HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed;   //!
   TBranch        *b_HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed_Eta2p4;   //!
   TBranch        *b_HLT_DoubleL2Mu23NoVtx_2Cha;   //!
   TBranch        *b_HLT_DoubleL2Mu25NoVtx_2Cha;   //!
   TBranch        *b_HLT_DoubleL2Mu25NoVtx_2Cha_Eta2p4;   //!
   TBranch        *b_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL;   //!
   TBranch        *b_HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL;   //!
   TBranch        *b_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ;   //!
   TBranch        *b_HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ;   //!
   TBranch        *b_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8;   //!
   TBranch        *b_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_PFJet30;   //!
   TBranch        *b_HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass8;   //!
   TBranch        *b_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8;   //!
   TBranch        *b_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_CaloJet30;   //!
   TBranch        *b_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_AK8PFJet30;   //!
   TBranch        *b_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_AK8CaloJet30;   //!
   TBranch        *b_HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass3p8;   //!
   TBranch        *b_HLT_Mu30_TkMu0_Psi;   //!
   TBranch        *b_HLT_Mu30_TkMu0_Upsilon;   //!
   TBranch        *b_HLT_Mu25_TkMu0_Phi;   //!
   TBranch        *b_HLT_Mu15;   //!
   TBranch        *b_HLT_Mu20;   //!
   TBranch        *b_HLT_Mu27;   //!
   TBranch        *b_HLT_Mu50;   //!
   TBranch        *b_HLT_Mu55;   //!
   TBranch        *b_HLT_CascadeMu100;   //!
   TBranch        *b_HLT_HighPtTkMu100;   //!
   TBranch        *b_HLT_DiPFJetAve40;   //!
   TBranch        *b_HLT_DiPFJetAve60;   //!
   TBranch        *b_HLT_DiPFJetAve80;   //!
   TBranch        *b_HLT_DiPFJetAve140;   //!
   TBranch        *b_HLT_DiPFJetAve200;   //!
   TBranch        *b_HLT_DiPFJetAve260;   //!
   TBranch        *b_HLT_DiPFJetAve320;   //!
   TBranch        *b_HLT_DiPFJetAve400;   //!
   TBranch        *b_HLT_DiPFJetAve500;   //!
   TBranch        *b_HLT_DiPFJetAve60_HFJEC;   //!
   TBranch        *b_HLT_DiPFJetAve80_HFJEC;   //!
   TBranch        *b_HLT_DiPFJetAve100_HFJEC;   //!
   TBranch        *b_HLT_DiPFJetAve160_HFJEC;   //!
   TBranch        *b_HLT_DiPFJetAve220_HFJEC;   //!
   TBranch        *b_HLT_DiPFJetAve260_HFJEC;   //!
   TBranch        *b_HLT_DiPFJetAve300_HFJEC;   //!
   TBranch        *b_HLT_DiPFJetAve180_PPSMatch_Xi0p3_QuadJet_Max2ProtPerRP;   //!
   TBranch        *b_HLT_AK8PFJet40;   //!
   TBranch        *b_HLT_AK8PFJet60;   //!
   TBranch        *b_HLT_AK8PFJet80;   //!
   TBranch        *b_HLT_AK8PFJet140;   //!
   TBranch        *b_HLT_AK8PFJet200;   //!
   TBranch        *b_HLT_AK8PFJet260;   //!
   TBranch        *b_HLT_AK8PFJet320;   //!
   TBranch        *b_HLT_AK8PFJet400;   //!
   TBranch        *b_HLT_AK8PFJet450;   //!
   TBranch        *b_HLT_AK8PFJet500;   //!
   TBranch        *b_HLT_AK8PFJet550;   //!
   TBranch        *b_HLT_PFJet40;   //!
   TBranch        *b_HLT_PFJet60;   //!
   TBranch        *b_HLT_PFJet80;   //!
   TBranch        *b_HLT_PFJet110;   //!
   TBranch        *b_HLT_PFJet140;   //!
   TBranch        *b_HLT_PFJet200;   //!
   TBranch        *b_HLT_PFJet260;   //!
   TBranch        *b_HLT_PFJet320;   //!
   TBranch        *b_HLT_PFJet400;   //!
   TBranch        *b_HLT_PFJet450;   //!
   TBranch        *b_HLT_PFJet500;   //!
   TBranch        *b_HLT_PFJet550;   //!
   TBranch        *b_HLT_PFJetFwd40;   //!
   TBranch        *b_HLT_PFJetFwd60;   //!
   TBranch        *b_HLT_PFJetFwd80;   //!
   TBranch        *b_HLT_PFJetFwd140;   //!
   TBranch        *b_HLT_PFJetFwd200;   //!
   TBranch        *b_HLT_PFJetFwd260;   //!
   TBranch        *b_HLT_PFJetFwd320;   //!
   TBranch        *b_HLT_PFJetFwd400;   //!
   TBranch        *b_HLT_PFJetFwd450;   //!
   TBranch        *b_HLT_PFJetFwd500;   //!
   TBranch        *b_HLT_AK8PFJetFwd40;   //!
   TBranch        *b_HLT_AK8PFJetFwd60;   //!
   TBranch        *b_HLT_AK8PFJetFwd80;   //!
   TBranch        *b_HLT_AK8PFJetFwd140;   //!
   TBranch        *b_HLT_AK8PFJetFwd200;   //!
   TBranch        *b_HLT_AK8PFJetFwd260;   //!
   TBranch        *b_HLT_AK8PFJetFwd320;   //!
   TBranch        *b_HLT_AK8PFJetFwd400;   //!
   TBranch        *b_HLT_AK8PFJetFwd450;   //!
   TBranch        *b_HLT_AK8PFJetFwd500;   //!
   TBranch        *b_HLT_PFHT180;   //!
   TBranch        *b_HLT_PFHT250;   //!
   TBranch        *b_HLT_PFHT370;   //!
   TBranch        *b_HLT_PFHT430;   //!
   TBranch        *b_HLT_PFHT510;   //!
   TBranch        *b_HLT_PFHT590;   //!
   TBranch        *b_HLT_PFHT680;   //!
   TBranch        *b_HLT_PFHT780;   //!
   TBranch        *b_HLT_PFHT890;   //!
   TBranch        *b_HLT_PFHT1050;   //!
   TBranch        *b_HLT_PFHT500_PFMET100_PFMHT100_IDTight;   //!
   TBranch        *b_HLT_PFHT500_PFMET110_PFMHT110_IDTight;   //!
   TBranch        *b_HLT_PFHT700_PFMET85_PFMHT85_IDTight;   //!
   TBranch        *b_HLT_PFHT800_PFMET75_PFMHT75_IDTight;   //!
   TBranch        *b_HLT_PFMET120_PFMHT120_IDTight;   //!
   TBranch        *b_HLT_PFMET130_PFMHT130_IDTight;   //!
   TBranch        *b_HLT_PFMET140_PFMHT140_IDTight;   //!
   TBranch        *b_HLT_PFMET120_PFMHT120_IDTight_PFHT60;   //!
   TBranch        *b_HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60;   //!
   TBranch        *b_HLT_PFMETTypeOne140_PFMHT140_IDTight;   //!
   TBranch        *b_HLT_PFMETNoMu120_PFMHTNoMu120_IDTight;   //!
   TBranch        *b_HLT_PFMETNoMu130_PFMHTNoMu130_IDTight;   //!
   TBranch        *b_HLT_PFMETNoMu140_PFMHTNoMu140_IDTight;   //!
   TBranch        *b_HLT_PFMETNoMu110_PFMHTNoMu110_IDTight_FilterHF;   //!
   TBranch        *b_HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_FilterHF;   //!
   TBranch        *b_HLT_PFMETNoMu130_PFMHTNoMu130_IDTight_FilterHF;   //!
   TBranch        *b_HLT_PFMETNoMu140_PFMHTNoMu140_IDTight_FilterHF;   //!
   TBranch        *b_HLT_L1ETMHadSeeds;   //!
   TBranch        *b_HLT_CaloMHT90;   //!
   TBranch        *b_HLT_CaloMET90_NotCleaned;   //!
   TBranch        *b_HLT_CaloMET350_NotCleaned;   //!
   TBranch        *b_HLT_PFMET200_NotCleaned;   //!
   TBranch        *b_HLT_PFMET250_NotCleaned;   //!
   TBranch        *b_HLT_PFMET300_NotCleaned;   //!
   TBranch        *b_HLT_PFMET200_BeamHaloCleaned;   //!
   TBranch        *b_HLT_PFMETTypeOne200_BeamHaloCleaned;   //!
   TBranch        *b_HLT_MET105_IsoTrk50;   //!
   TBranch        *b_HLT_MET120_IsoTrk50;   //!
   TBranch        *b_HLT_Mu12eta2p3;   //!
   TBranch        *b_HLT_Mu12eta2p3_PFJet40;   //!
   TBranch        *b_HLT_Mu12_DoublePFJets40_PNetBTag_0p11;   //!
   TBranch        *b_HLT_Mu12_DoublePFJets100_PNetBTag_0p11;   //!
   TBranch        *b_HLT_Mu12_DoublePFJets200_PNetBTag_0p11;   //!
   TBranch        *b_HLT_Mu12_DoublePFJets350_PNetBTag_0p11;   //!
   TBranch        *b_HLT_Mu12_DoublePFJets40MaxDeta1p6_PNet2BTag_0p11;   //!
   TBranch        *b_HLT_Mu12_DoublePFJets54MaxDeta1p6_PNet2BTag_0p11;   //!
   TBranch        *b_HLT_DoublePFJets40_PNetBTag_0p11;   //!
   TBranch        *b_HLT_DoublePFJets100_PNetBTag_0p11;   //!
   TBranch        *b_HLT_DoublePFJets200_PNetBTag_0p11;   //!
   TBranch        *b_HLT_DoublePFJets350_PNetBTag_0p11;   //!
   TBranch        *b_HLT_DoublePFJets116MaxDeta1p6_PNet2BTag_0p11;   //!
   TBranch        *b_HLT_DoublePFJets128MaxDeta1p6_PNet2BTag_0p11;   //!
   TBranch        *b_HLT_Photon300_NoHE;   //!
   TBranch        *b_HLT_Mu8_TrkIsoVVL;   //!
   TBranch        *b_HLT_Mu8_DiEle12_CaloIdL_TrackIdL_DZ;   //!
   TBranch        *b_HLT_Mu8_DiEle12_CaloIdL_TrackIdL;   //!
   TBranch        *b_HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_DZ;   //!
   TBranch        *b_HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350;   //!
   TBranch        *b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ;   //!
   TBranch        *b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30;   //!
   TBranch        *b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_CaloDiJet30;   //!
   TBranch        *b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL;   //!
   TBranch        *b_HLT_Mu17_TrkIsoVVL;   //!
   TBranch        *b_HLT_Mu19_TrkIsoVVL;   //!
   TBranch        *b_HLT_BTagMu_AK4DiJet20_Mu5;   //!
   TBranch        *b_HLT_BTagMu_AK4DiJet40_Mu5;   //!
   TBranch        *b_HLT_BTagMu_AK4DiJet70_Mu5;   //!
   TBranch        *b_HLT_BTagMu_AK4DiJet110_Mu5;   //!
   TBranch        *b_HLT_BTagMu_AK4DiJet170_Mu5;   //!
   TBranch        *b_HLT_BTagMu_AK4Jet300_Mu5;   //!
   TBranch        *b_HLT_BTagMu_AK8DiJet170_Mu5;   //!
   TBranch        *b_HLT_BTagMu_AK8Jet170_DoubleMu5;   //!
   TBranch        *b_HLT_BTagMu_AK8Jet300_Mu5;   //!
   TBranch        *b_HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ;   //!
   TBranch        *b_HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL;   //!
   TBranch        *b_HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ;   //!
   TBranch        *b_HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL;   //!
   TBranch        *b_HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL;   //!
   TBranch        *b_HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ;   //!
   TBranch        *b_HLT_Mu12_IsoVVL_PFHT150_PNetBTag0p53;   //!
   TBranch        *b_HLT_Photon33;   //!
   TBranch        *b_HLT_Photon50;   //!
   TBranch        *b_HLT_Photon75;   //!
   TBranch        *b_HLT_Photon90;   //!
   TBranch        *b_HLT_Photon120;   //!
   TBranch        *b_HLT_Photon150;   //!
   TBranch        *b_HLT_Photon175;   //!
   TBranch        *b_HLT_Photon200;   //!
   TBranch        *b_HLT_Photon45EB;   //!
   TBranch        *b_HLT_Photon40EB;   //!
   TBranch        *b_HLT_Photon50EB;   //!
   TBranch        *b_HLT_Photon30EB_TightID_TightIso;   //!
   TBranch        *b_HLT_Photon40EB_TightID_TightIso;   //!
   TBranch        *b_HLT_Photon45EB_TightID_TightIso;   //!
   TBranch        *b_HLT_Photon50EB_TightID_TightIso_PFJet30;   //!
   TBranch        *b_HLT_Photon50EB_TightID_TightIso_CaloJet30;   //!
   TBranch        *b_HLT_Photon50EB_TightID_TightIso_AK8PFJet30;   //!
   TBranch        *b_HLT_Photon50EB_TightID_TightIso_AK8CaloJet30;   //!
   TBranch        *b_HLT_Photon50EB_TightID_TightIso;   //!
   TBranch        *b_HLT_Photon55EB_TightID_TightIso;   //!
   TBranch        *b_HLT_Photon75EB_TightID_TightIso;   //!
   TBranch        *b_HLT_Photon90EB_TightID_TightIso;   //!
   TBranch        *b_HLT_Photon110EB_TightID_TightIso;   //!
   TBranch        *b_HLT_Photon110EB_TightID_TightIso_PFJet30;   //!
   TBranch        *b_HLT_Photon110EB_TightID_TightIso_CaloJet30;   //!
   TBranch        *b_HLT_Photon110EB_TightID_TightIso_AK8PFJet30;   //!
   TBranch        *b_HLT_Photon110EB_TightID_TightIso_AK8CaloJet30;   //!
   TBranch        *b_HLT_Photon100EBHE10;   //!
   TBranch        *b_HLT_Photon50_R9Id90_HE10_IsoM;   //!
   TBranch        *b_HLT_Photon75_R9Id90_HE10_IsoM;   //!
   TBranch        *b_HLT_Photon90_R9Id90_HE10_IsoM;   //!
   TBranch        *b_HLT_Photon120_R9Id90_HE10_IsoM;   //!
   TBranch        *b_HLT_Photon165_R9Id90_HE10_IsoM;   //!
   TBranch        *b_HLT_Diphoton30_22_R9Id_OR_IsoCaloId_AND_HE_R9Id_Mass90;   //!
   TBranch        *b_HLT_Diphoton30_22_R9Id_OR_IsoCaloId_AND_HE_R9Id_Mass95;   //!
   TBranch        *b_HLT_DiphotonMVA14p25_Mass90;   //!
   TBranch        *b_HLT_DiphotonMVA14p25_Tight_Mass90;   //!
   TBranch        *b_HLT_Photon35_TwoProngs35;   //!
   TBranch        *b_HLT_IsoMu24_TwoProngs35;   //!
   TBranch        *b_HLT_Dimuon0_Jpsi_L1_NoOS;   //!
   TBranch        *b_HLT_Dimuon0_Jpsi_NoVertexing_NoOS;   //!
   TBranch        *b_HLT_Dimuon0_Jpsi;   //!
   TBranch        *b_HLT_Dimuon0_Jpsi_NoVertexing;   //!
   TBranch        *b_HLT_Dimuon0_Jpsi_L1_4R_0er1p5R;   //!
   TBranch        *b_HLT_Dimuon0_Jpsi_NoVertexing_L1_4R_0er1p5R;   //!
   TBranch        *b_HLT_Dimuon0_Jpsi3p5_Muon2;   //!
   TBranch        *b_HLT_Dimuon0_Upsilon_L1_4p5;   //!
   TBranch        *b_HLT_Dimuon0_Upsilon_L1_4p5er2p0;   //!
   TBranch        *b_HLT_Dimuon0_Upsilon_L1_4p5er2p0M;   //!
   TBranch        *b_HLT_Dimuon0_Upsilon_NoVertexing;   //!
   TBranch        *b_HLT_Dimuon0_LowMass_L1_0er1p5;   //!
   TBranch        *b_HLT_Dimuon0_LowMass;   //!
   TBranch        *b_HLT_Dimuon0_LowMass_L1_4;   //!
   TBranch        *b_HLT_Dimuon0_LowMass_L1_TM530;   //!
   TBranch        *b_HLT_Dimuon0_Upsilon_Muon_NoL1Mass;   //!
   TBranch        *b_HLT_TripleMu_5_3_3_Mass3p8_DZ;   //!
   TBranch        *b_HLT_TripleMu_10_5_5_DZ;   //!
   TBranch        *b_HLT_TripleMu_12_10_5;   //!
   TBranch        *b_HLT_Tau3Mu_Mu7_Mu1_TkMu1_Tau15;   //!
   TBranch        *b_HLT_Tau3Mu_Mu7_Mu1_TkMu1_Tau15_Charge1;   //!
   TBranch        *b_HLT_Tau3Mu_Mu7_Mu1_TkMu1_IsoTau15;   //!
   TBranch        *b_HLT_Tau3Mu_Mu7_Mu1_TkMu1_IsoTau15_Charge1;   //!
   TBranch        *b_HLT_DoubleMu3_DZ_PFMET50_PFMHT60;   //!
   TBranch        *b_HLT_DoubleMu3_DZ_PFMET70_PFMHT70;   //!
   TBranch        *b_HLT_DoubleMu3_DZ_PFMET90_PFMHT90;   //!
   TBranch        *b_HLT_DoubleMu3_Trk_Tau3mu_NoL1Mass;   //!
   TBranch        *b_HLT_DoubleMu4_3_LowMass_SS;   //!
   TBranch        *b_HLT_DoubleMu4_Jpsi_Displaced;   //!
   TBranch        *b_HLT_DoubleMu4_Jpsi_NoVertexing;   //!
   TBranch        *b_HLT_DoubleMu4_JpsiTrkTrk_Displaced;   //!
   TBranch        *b_HLT_DoubleMu4_JpsiTrk_Bc;   //!
   TBranch        *b_HLT_DoubleMu43NoFiltersNoVtx;   //!
   TBranch        *b_HLT_DoubleMu48NoFiltersNoVtx;   //!
   TBranch        *b_HLT_Mu43NoFiltersNoVtx_Photon43_CaloIdL;   //!
   TBranch        *b_HLT_Mu48NoFiltersNoVtx_Photon48_CaloIdL;   //!
   TBranch        *b_HLT_Mu38NoFiltersNoVtxDisplaced_Photon38_CaloIdL;   //!
   TBranch        *b_HLT_Mu43NoFiltersNoVtxDisplaced_Photon43_CaloIdL;   //!
   TBranch        *b_HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned;   //!
   TBranch        *b_HLT_Ele28_eta2p1_WPTight_Gsf_HT150;   //!
   TBranch        *b_HLT_Ele28_HighEta_SC20_Mass55;   //!
   TBranch        *b_HLT_Ele15_IsoVVVL_PFHT450_PFMET50;   //!
   TBranch        *b_HLT_Ele15_IsoVVVL_PFHT450;   //!
   TBranch        *b_HLT_Ele50_IsoVVVL_PFHT450;   //!
   TBranch        *b_HLT_Ele15_IsoVVVL_PFHT600;   //!
   TBranch        *b_HLT_Mu15_IsoVVVL_PFHT450_PFMET50;   //!
   TBranch        *b_HLT_Mu15_IsoVVVL_PFHT450;   //!
   TBranch        *b_HLT_Mu50_IsoVVVL_PFHT450;   //!
   TBranch        *b_HLT_Mu15_IsoVVVL_PFHT600;   //!
   TBranch        *b_HLT_Mu3er1p5_PFJet100er2p5_PFMET80_PFMHT80_IDTight;   //!
   TBranch        *b_HLT_Mu3er1p5_PFJet100er2p5_PFMET90_PFMHT90_IDTight;   //!
   TBranch        *b_HLT_Mu3er1p5_PFJet100er2p5_PFMET100_PFMHT100_IDTight;   //!
   TBranch        *b_HLT_Mu3er1p5_PFJet100er2p5_PFMETNoMu80_PFMHTNoMu80_IDTight;   //!
   TBranch        *b_HLT_Mu3er1p5_PFJet100er2p5_PFMETNoMu90_PFMHTNoMu90_IDTight;   //!
   TBranch        *b_HLT_Mu3er1p5_PFJet100er2p5_PFMETNoMu100_PFMHTNoMu100_IDTight;   //!
   TBranch        *b_HLT_Dimuon10_Upsilon_y1p4;   //!
   TBranch        *b_HLT_Dimuon12_Upsilon_y1p4;   //!
   TBranch        *b_HLT_Dimuon14_Phi_Barrel_Seagulls;   //!
   TBranch        *b_HLT_Dimuon25_Jpsi;   //!
   TBranch        *b_HLT_Dimuon14_PsiPrime;   //!
   TBranch        *b_HLT_Dimuon14_PsiPrime_noCorrL1;   //!
   TBranch        *b_HLT_Dimuon18_PsiPrime;   //!
   TBranch        *b_HLT_Dimuon18_PsiPrime_noCorrL1;   //!
   TBranch        *b_HLT_Dimuon24_Upsilon_noCorrL1;   //!
   TBranch        *b_HLT_Dimuon24_Phi_noCorrL1;   //!
   TBranch        *b_HLT_Dimuon25_Jpsi_noCorrL1;   //!
   TBranch        *b_HLT_DiMu4_Ele9_CaloIdL_TrackIdL_DZ_Mass3p8;   //!
   TBranch        *b_HLT_DiMu9_Ele9_CaloIdL_TrackIdL_DZ;   //!
   TBranch        *b_HLT_DiMu9_Ele9_CaloIdL_TrackIdL;   //!
   TBranch        *b_HLT_DoubleIsoMu20_eta2p1;   //!
   TBranch        *b_HLT_TrkMu12_DoubleTrkMu5NoFiltersNoVtx;   //!
   TBranch        *b_HLT_Mu8;   //!
   TBranch        *b_HLT_Mu17;   //!
   TBranch        *b_HLT_Mu19;   //!
   TBranch        *b_HLT_Mu17_Photon30_IsoCaloId;   //!
   TBranch        *b_HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30;   //!
   TBranch        *b_HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30;   //!
   TBranch        *b_HLT_Ele14_eta2p5_IsoVVVL_Gsf_PFHT200_PNetBTag0p53;   //!
   TBranch        *b_HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30;   //!
   TBranch        *b_HLT_Ele8_CaloIdM_TrackIdM_PFJet30;   //!
   TBranch        *b_HLT_Ele17_CaloIdM_TrackIdM_PFJet30;   //!
   TBranch        *b_HLT_Ele23_CaloIdM_TrackIdM_PFJet30;   //!
   TBranch        *b_HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165;   //!
   TBranch        *b_HLT_Ele115_CaloIdVT_GsfTrkIdT;   //!
   TBranch        *b_HLT_Ele135_CaloIdVT_GsfTrkIdT;   //!
   TBranch        *b_HLT_PFHT330PT30_QuadPFJet_75_60_45_40;   //!
   TBranch        *b_HLT_PFHT400_SixPFJet32;   //!
   TBranch        *b_HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50;   //!
   TBranch        *b_HLT_PFHT450_SixPFJet36;   //!
   TBranch        *b_HLT_PFHT450_SixPFJet36_PNetBTag0p35;   //!
   TBranch        *b_HLT_PFHT400_FivePFJet_120_120_60_30_30;   //!
   TBranch        *b_HLT_PFHT350;   //!
   TBranch        *b_HLT_Photon60_R9Id90_CaloIdL_IsoL_DisplacedIdL_PFHT350;   //!
   TBranch        *b_HLT_Photon60_R9Id90_CaloIdL_IsoL_DisplacedIdL_PFHT380;   //!
   TBranch        *b_HLT_Photon60_R9Id90_CaloIdL_IsoL_DisplacedIdL_PFHT400;   //!
   TBranch        *b_HLT_ECALHT800;   //!
   TBranch        *b_HLT_DiSC30_18_EIso_AND_HE_Mass70;   //!
   TBranch        *b_HLT_Photon20_HoverELoose;   //!
   TBranch        *b_HLT_Photon30_HoverELoose;   //!
   TBranch        *b_HLT_L1_CDC_SingleMu_3_er1p2_TOP120_DPHI2p618_3p142;   //!
   TBranch        *b_HLT_CDC_L2cosmic_10_er1p0;   //!
   TBranch        *b_HLT_CDC_L2cosmic_5p5_er1p0;   //!
   TBranch        *b_HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL;   //!
   TBranch        *b_HLT_VBF_DoubleMediumDeepTauPFTauHPS20_eta2p1;   //!
   TBranch        *b_HLT_Photon60_R9Id90_HE10_IsoM_EBOnly_PFJetsMJJ300DEta3;   //!
   TBranch        *b_HLT_Photon75_R9Id90_HE10_IsoM_EBOnly_PFJetsMJJ300DEta3;   //!
   TBranch        *b_HLT_Mu18_Mu9_SameSign;   //!
   TBranch        *b_HLT_DoubleMu2_Jpsi_DoubleTrk1_Phi1p05;   //!
   TBranch        *b_HLT_DoubleMu3_DCA_PFMET50_PFMHT60;   //!
   TBranch        *b_HLT_DoubleMu3_DCA_PFMET50_PFMHT60_Mass2p0;   //!
   TBranch        *b_HLT_DoubleMu3_DCA_PFMET50_PFMHT60_Mass2p0_noDCA;   //!
   TBranch        *b_HLT_TripleMu_5_3_3_Mass3p8_DCA;   //!
   TBranch        *b_HLT_QuadPFJet103_88_75_15;   //!
   TBranch        *b_HLT_QuadPFJet105_88_76_15;   //!
   TBranch        *b_HLT_QuadPFJet111_90_80_15;   //!
   TBranch        *b_HLT_Diphoton30_18_R9IdL_AND_HE_AND_IsoCaloId;   //!
   TBranch        *b_HLT_Diphoton30_18_R9IdL_AND_HE_AND_IsoCaloId_Mass55;   //!
   TBranch        *b_HLT_DoubleMediumDeepTauPFTauHPS35_L2NN_eta2p1;   //!
   TBranch        *b_HLT_Ele24_eta2p1_WPTight_Gsf_LooseDeepTauPFTauHPS30_eta2p1_CrossL1;   //!
   TBranch        *b_HLT_Ele24_eta2p1_WPTight_Gsf_PNetTauhPFJet30_Tight_eta2p3_CrossL1;   //!
   TBranch        *b_HLT_Ele24_eta2p1_WPTight_Gsf_PNetTauhPFJet30_Medium_eta2p3_CrossL1;   //!
   TBranch        *b_HLT_Ele24_eta2p1_WPTight_Gsf_PNetTauhPFJet30_Loose_eta2p3_CrossL1;   //!
   TBranch        *b_HLT_IsoMu20_eta2p1_LooseDeepTauPFTauHPS27_eta2p1_CrossL1;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_LooseDeepTauPFTauHPS180_eta2p1;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_LooseDeepTauPFTauHPS30_eta2p1_CrossL1;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS35_L2NN_eta2p1_CrossL1;   //!
   TBranch        *b_HLT_LooseDeepTauPFTauHPS180_L2NN_eta2p1;   //!
   TBranch        *b_HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5;   //!
   TBranch        *b_HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3;   //!
   TBranch        *b_HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_2p0;   //!
   TBranch        *b_HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_5p6;   //!
   TBranch        *b_HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_4p3;   //!
   TBranch        *b_HLT_QuadPFJet103_88_75_15_PNetBTag_0p4_VBF2;   //!
   TBranch        *b_HLT_QuadPFJet103_88_75_15_PNet2BTag_0p4_0p12_VBF1;   //!
   TBranch        *b_HLT_QuadPFJet105_88_76_15_PNetBTag_0p4_VBF2;   //!
   TBranch        *b_HLT_QuadPFJet105_88_76_15_PNet2BTag_0p4_0p12_VBF1;   //!
   TBranch        *b_HLT_QuadPFJet111_90_80_15_PNetBTag_0p4_VBF2;   //!
   TBranch        *b_HLT_QuadPFJet111_90_80_15_PNet2BTag_0p4_0p12_VBF1;   //!
   TBranch        *b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30_PFBtagDeepJet_1p5;   //!
   TBranch        *b_HLT_PFHT250_QuadPFJet25;   //!
   TBranch        *b_HLT_PFHT250_QuadPFJet25_PNet2BTagMean0p55;   //!
   TBranch        *b_HLT_PFHT250_QuadPFJet25_PNet1BTag0p20_PNet1Tauh0p50;   //!
   TBranch        *b_HLT_PFHT250_QuadPFJet30_PNet2BTagMean0p55;   //!
   TBranch        *b_HLT_PFHT250_QuadPFJet30_PNet1BTag0p20_PNet1Tauh0p50;   //!
   TBranch        *b_HLT_PFHT280_QuadPFJet30_PNet1BTag0p20_PNet1Tauh0p50;   //!
   TBranch        *b_HLT_PFHT280_QuadPFJet30;   //!
   TBranch        *b_HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p55;   //!
   TBranch        *b_HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p60;   //!
   TBranch        *b_HLT_PFHT280_QuadPFJet35_PNet2BTagMean0p60;   //!
   TBranch        *b_HLT_PFHT340_QuadPFJet70_50_40_40_PNet2BTagMean0p70;   //!
   TBranch        *b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT280;   //!
   TBranch        *b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT280_QuadPFJet30;   //!
   TBranch        *b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT280_QuadPFJet30_PNet2BTagMean0p55;   //!
   TBranch        *b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30_PNet2BTagMean0p50;   //!
   TBranch        *b_HLT_QuadPFJet100_88_70_30;   //!
   TBranch        *b_HLT_QuadPFJet105_88_75_30;   //!
   TBranch        *b_HLT_QuadPFJet111_90_80_30;   //!
   TBranch        *b_HLT_QuadPFJet100_88_70_30_PNet1CvsAll0p5_VBF3Tight;   //!
   TBranch        *b_HLT_QuadPFJet105_88_75_30_PNet1CvsAll0p5_VBF3Tight;   //!
   TBranch        *b_HLT_QuadPFJet111_90_80_30_PNet1CvsAll0p6_VBF3Tight;   //!
   TBranch        *b_HLT_AK8PFJet220_SoftDropMass40;   //!
   TBranch        *b_HLT_AK8PFJet220_SoftDropMass40_PNetBB0p06_DoubleAK4PFJet60_30_PNet2BTagMean0p50;   //!
   TBranch        *b_HLT_AK8PFJet220_SoftDropMass40_PNetBB0p06_DoubleAK4PFJet60_30_PNet2BTagMean0p53;   //!
   TBranch        *b_HLT_AK8PFJet220_SoftDropMass40_PNetBB0p06_DoubleAK4PFJet60_30_PNet2BTagMean0p55;   //!
   TBranch        *b_HLT_AK8PFJet220_SoftDropMass40_PNetBB0p06_DoubleAK4PFJet60_30_PNet2BTagMean0p60;   //!
   TBranch        *b_HLT_AK8PFJet230_SoftDropMass40;   //!
   TBranch        *b_HLT_AK8PFJet230_SoftDropMass40_PNetBB0p06;   //!
   TBranch        *b_HLT_AK8PFJet230_SoftDropMass40_PNetBB0p10;   //!
   TBranch        *b_HLT_AK8PFJet230_SoftDropMass40_PNetTauTau0p03;   //!
   TBranch        *b_HLT_AK8PFJet230_SoftDropMass40_PNetTauTau0p05;   //!
   TBranch        *b_HLT_AK8PFJet250_SoftDropMass40_PNetBB0p06;   //!
   TBranch        *b_HLT_AK8PFJet250_SoftDropMass40_PNetBB0p10;   //!
   TBranch        *b_HLT_AK8PFJet250_SoftDropMass40_PNetTauTau0p03;   //!
   TBranch        *b_HLT_AK8PFJet250_SoftDropMass40_PNetTauTau0p05;   //!
   TBranch        *b_HLT_AK8PFJet275_SoftDropMass40_PNetBB0p06;   //!
   TBranch        *b_HLT_AK8PFJet275_SoftDropMass40_PNetBB0p10;   //!
   TBranch        *b_HLT_AK8PFJet275_SoftDropMass40_PNetTauTau0p03;   //!
   TBranch        *b_HLT_AK8PFJet275_SoftDropMass40_PNetTauTau0p05;   //!
   TBranch        *b_HLT_AK8PFJet275_Nch45;   //!
   TBranch        *b_HLT_AK8PFJet275_Nch40;   //!
   TBranch        *b_HLT_IsoMu50_AK8PFJet220_SoftDropMass40;   //!
   TBranch        *b_HLT_IsoMu50_AK8PFJet220_SoftDropMass40_PNetBB0p06;   //!
   TBranch        *b_HLT_IsoMu50_AK8PFJet230_SoftDropMass40;   //!
   TBranch        *b_HLT_IsoMu50_AK8PFJet230_SoftDropMass40_PNetBB0p06;   //!
   TBranch        *b_HLT_IsoMu50_AK8PFJet230_SoftDropMass40_PNetBB0p10;   //!
   TBranch        *b_HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet220_SoftDropMass40;   //!
   TBranch        *b_HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet220_SoftDropMass40_PNetBB0p06;   //!
   TBranch        *b_HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet230_SoftDropMass40;   //!
   TBranch        *b_HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet230_SoftDropMass40_PNetBB0p06;   //!
   TBranch        *b_HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet230_SoftDropMass40_PNetBB0p10;   //!
   TBranch        *b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_DoubleAK4PFJet60_30;   //!
   TBranch        *b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_DoubleAK4PFJet60_30_PNet2BTagMean0p50;   //!
   TBranch        *b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT250;   //!
   TBranch        *b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT250_QuadPFJet25;   //!
   TBranch        *b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT250_QuadPFJet25_PNet2BTagMean0p55;   //!
   TBranch        *b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT250_QuadPFJet25_PNet1BTag0p20;   //!
   TBranch        *b_HLT_DoubleMediumDeepTauPFTauHPS30_L2NN_eta2p1_PFJet60;   //!
   TBranch        *b_HLT_DoubleMediumDeepTauPFTauHPS30_L2NN_eta2p1_PFJet75;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS30_L2NN_eta2p1_CrossL1;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS30_L2NN_eta2p1_PFJet60_CrossL1;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS30_L2NN_eta2p1_PFJet75_CrossL1;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS20_eta2p1_SingleL1;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS45_L2NN_eta2p1_CrossL1;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Medium_L2NN_eta2p3_CrossL1;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Tight_L2NN_eta2p3_CrossL1;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_PNetTauhPFJet26_L2NN_eta2p3_CrossL1;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_PNetTauhPFJet26_L2NN_eta2p3_CrossL1_PFJet60;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_PNetTauhPFJet26_L2NN_eta2p3_CrossL1_PFJet75;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Tight_eta2p3_CrossL1_ETau_Monitoring;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Medium_eta2p3_CrossL1_ETau_Monitoring;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Loose_eta2p3_CrossL1_ETau_Monitoring;   //!
   TBranch        *b_HLT_IsoMu20_eta2p1_PNetTauhPFJet27_Tight_eta2p3_CrossL1;   //!
   TBranch        *b_HLT_IsoMu20_eta2p1_PNetTauhPFJet27_Medium_eta2p3_CrossL1;   //!
   TBranch        *b_HLT_IsoMu20_eta2p1_PNetTauhPFJet27_Loose_eta2p3_CrossL1;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_PNetTauhPFJet20_eta2p2_SingleL1;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_PNetTauhPFJet45_L2NN_eta2p3_CrossL1;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_PFHT250;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_PFHT250_QuadPFJet25;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_PFHT250_QuadPFJet25_PNet1Tauh0p50;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_SinglePFJet25_PNet1Tauh0p50;   //!
   TBranch        *b_HLT_DoublePNetTauhPFJet30_Medium_L2NN_eta2p3;   //!
   TBranch        *b_HLT_DoublePNetTauhPFJet30_Tight_L2NN_eta2p3;   //!
   TBranch        *b_HLT_DoublePNetTauhPFJet26_L2NN_eta2p3_PFJet60;   //!
   TBranch        *b_HLT_DoublePNetTauhPFJet26_L2NN_eta2p3_PFJet75;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_PNetTauhPFJet130_Loose_L2NN_eta2p3_CrossL1;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_PNetTauhPFJet130_Medium_L2NN_eta2p3_CrossL1;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_PNetTauhPFJet130_Tight_L2NN_eta2p3_CrossL1;   //!
   TBranch        *b_HLT_SinglePNetTauhPFJet130_Loose_L2NN_eta2p3;   //!
   TBranch        *b_HLT_SinglePNetTauhPFJet130_Medium_L2NN_eta2p3;   //!
   TBranch        *b_HLT_SinglePNetTauhPFJet130_Tight_L2NN_eta2p3;   //!
   TBranch        *b_HLT_DoubleL2Mu10NoVtx_2Cha_VetoL3Mu0DxyMax1cm;   //!
   TBranch        *b_HLT_DoubleL2Mu12NoVtx_2Cha_VetoL3Mu0DxyMax1cm;   //!
   TBranch        *b_HLT_DoubleL2Mu14NoVtx_2Cha_VetoL3Mu0DxyMax1cm;   //!
   TBranch        *b_HLT_DoubleL3Mu16_10NoVtx_DxyMin0p01cm;   //!
   TBranch        *b_HLT_DoubleL3Mu18_10NoVtx_DxyMin0p01cm;   //!
   TBranch        *b_HLT_DoubleL3Mu20_10NoVtx_DxyMin0p01cm;   //!
   TBranch        *b_HLT_L2Mu10NoVtx_2Cha;   //!
   TBranch        *b_HLT_L2Mu10NoVtx_2Cha_VetoL3Mu0DxyMax1cm;   //!
   TBranch        *b_HLT_L3Mu10NoVtx;   //!
   TBranch        *b_HLT_L3Mu10NoVtx_DxyMin0p01cm;   //!
   TBranch        *b_HLT_DoubleL2Mu_L3Mu16NoVtx_VetoL3Mu0DxyMax0p1cm;   //!
   TBranch        *b_HLT_DoubleL2Mu_L3Mu18NoVtx_VetoL3Mu0DxyMax0p1cm;   //!
   TBranch        *b_HLT_DoubleL2Mu10NoVtx_2Cha_CosmicSeed_VetoL3Mu0DxyMax1cm;   //!
   TBranch        *b_HLT_DoubleL2Mu12NoVtx_2Cha_CosmicSeed_VetoL3Mu0DxyMax1cm;   //!
   TBranch        *b_HLT_L2Mu10NoVtx_2Cha_CosmicSeed;   //!
   TBranch        *b_HLT_L2Mu10NoVtx_2Cha_CosmicSeed_VetoL3Mu0DxyMax1cm;   //!
   TBranch        *b_HLT_DoubleL3dTksMu16_10NoVtx_DxyMin0p01cm;   //!
   TBranch        *b_HLT_L2Mu50NoVtx_3Cha_CosmicSeed_VetoL3Mu0DxyMax1cm;   //!
   TBranch        *b_HLT_L2Mu50NoVtx_3Cha_VetoL3Mu0DxyMax1cm;   //!
   TBranch        *b_HLT_L3Mu30NoVtx_DxyMin0p01cm;   //!
   TBranch        *b_HLT_L3Mu50NoVtx_DxyMin0p01cm;   //!
   TBranch        *b_HLT_L3dTksMu10_NoVtx_DxyMin0p01cm;   //!
   TBranch        *b_HLT_Mu20NoFiltersNoVtxDisplaced_Photon20_CaloCustomId;   //!
   TBranch        *b_HLT_DisplacedMu24_MediumChargedIsoDisplacedPFTauHPS24;   //!
   TBranch        *b_HLT_Photon34_R9Id90_CaloIdL_IsoL_DisplacedIdL_MediumChargedIsoDisplacedPFTauHPS34;   //!
   TBranch        *b_HLT_DoubleMediumChargedIsoDisplacedPFTauHPS32_Trk1_eta2p1;   //!
   TBranch        *b_HLT_DoubleMediumChargedIsoDisplacedPFTauHPS36_Trk1_eta2p1;   //!
   TBranch        *b_HLT_DoubleMediumChargedIsoDisplacedPFTauHPS32_Trk1_eta2p1_noDxy;   //!
   TBranch        *b_HLT_IsoMu27_MediumChargedIsoDisplacedPFTauHPS24_eta2p1_SingleL1;   //!
   TBranch        *b_HLT_HT350_DelayedJet40_SingleDelay1p5To3p5nsInclusive;   //!
   TBranch        *b_HLT_HT350_DelayedJet40_SingleDelay1p6To3p5nsInclusive;   //!
   TBranch        *b_HLT_HT350_DelayedJet40_SingleDelay1p75To3p5nsInclusive;   //!
   TBranch        *b_HLT_HT350_DelayedJet40_SingleDelay3nsInclusive;   //!
   TBranch        *b_HLT_HT350_DelayedJet40_SingleDelay3p25nsInclusive;   //!
   TBranch        *b_HLT_HT350_DelayedJet40_SingleDelay3p5nsInclusive;   //!
   TBranch        *b_HLT_HT430_DelayedJet40_SingleDelay0p5nsInclusive;   //!
   TBranch        *b_HLT_HT430_DelayedJet40_SingleDelay1nsInclusive;   //!
   TBranch        *b_HLT_HT430_DelayedJet40_SingleDelay1To1p5nsInclusive;   //!
   TBranch        *b_HLT_HT430_DelayedJet40_SingleDelay1p1To1p6nsInclusive;   //!
   TBranch        *b_HLT_HT430_DelayedJet40_SingleDelay1p25To1p75nsInclusive;   //!
   TBranch        *b_HLT_HT430_DelayedJet40_SingleDelay1p5nsInclusive;   //!
   TBranch        *b_HLT_HT430_DelayedJet40_SingleDelay2nsInclusive;   //!
   TBranch        *b_HLT_HT430_DelayedJet40_SingleDelay2p25nsInclusive;   //!
   TBranch        *b_HLT_HT430_DelayedJet40_SingleDelay2p5nsInclusive;   //!
   TBranch        *b_HLT_HT430_DelayedJet40_SingleDelay0p5nsTrackless;   //!
   TBranch        *b_HLT_HT430_DelayedJet40_SingleDelay1nsTrackless;   //!
   TBranch        *b_HLT_HT430_DelayedJet40_SingleDelay1p25nsTrackless;   //!
   TBranch        *b_HLT_HT430_DelayedJet40_SingleDelay1p5nsTrackless;   //!
   TBranch        *b_HLT_HT430_DelayedJet40_DoubleDelay0p5nsInclusive;   //!
   TBranch        *b_HLT_HT430_DelayedJet40_DoubleDelay1nsInclusive;   //!
   TBranch        *b_HLT_HT430_DelayedJet40_DoubleDelay1p25nsInclusive;   //!
   TBranch        *b_HLT_HT430_DelayedJet40_DoubleDelay1p5nsInclusive;   //!
   TBranch        *b_HLT_HT430_DelayedJet40_DoubleDelay0p5nsTrackless;   //!
   TBranch        *b_HLT_HT430_DelayedJet40_DoubleDelay0p75nsTrackless;   //!
   TBranch        *b_HLT_HT430_DelayedJet40_DoubleDelay1nsTrackless;   //!
   TBranch        *b_HLT_L1Tau_DelayedJet40_SingleDelay2p5To4nsInclusive;   //!
   TBranch        *b_HLT_L1Tau_DelayedJet40_SingleDelay2p6To4nsInclusive;   //!
   TBranch        *b_HLT_L1Tau_DelayedJet40_SingleDelay2p75To4nsInclusive;   //!
   TBranch        *b_HLT_L1Tau_DelayedJet40_SingleDelay3p5nsInclusive;   //!
   TBranch        *b_HLT_L1Tau_DelayedJet40_SingleDelay3p75nsInclusive;   //!
   TBranch        *b_HLT_L1Tau_DelayedJet40_SingleDelay4nsInclusive;   //!
   TBranch        *b_HLT_L1Tau_DelayedJet40_SingleDelay2p5nsTrackless;   //!
   TBranch        *b_HLT_L1Tau_DelayedJet40_SingleDelay2p75nsTrackless;   //!
   TBranch        *b_HLT_L1Tau_DelayedJet40_SingleDelay3nsTrackless;   //!
   TBranch        *b_HLT_L1Tau_DelayedJet40_DoubleDelay0p75nsInclusive;   //!
   TBranch        *b_HLT_L1Tau_DelayedJet40_DoubleDelay1p25nsInclusive;   //!
   TBranch        *b_HLT_L1Tau_DelayedJet40_DoubleDelay1p5nsInclusive;   //!
   TBranch        *b_HLT_L1Tau_DelayedJet40_DoubleDelay1p75nsInclusive;   //!
   TBranch        *b_HLT_L1Tau_DelayedJet40_DoubleDelay0p5nsTrackless;   //!
   TBranch        *b_HLT_L1Tau_DelayedJet40_DoubleDelay1nsTrackless;   //!
   TBranch        *b_HLT_L1Tau_DelayedJet40_DoubleDelay1p25nsTrackless;   //!
   TBranch        *b_HLT_L1Tau_DelayedJet40_DoubleDelay1p5nsTrackless;   //!
   TBranch        *b_HLT_L1Mu6HT240;   //!
   TBranch        *b_HLT_Mu6HT240_DisplacedDijet30_Inclusive1PtrkShortSig5_DisplacedLoose;   //!
   TBranch        *b_HLT_Mu6HT240_DisplacedDijet35_Inclusive0PtrkShortSig5;   //!
   TBranch        *b_HLT_Mu6HT240_DisplacedDijet35_Inclusive1PtrkShortSig5_DisplacedLoose;   //!
   TBranch        *b_HLT_Mu6HT240_DisplacedDijet40_Inclusive0PtrkShortSig5;   //!
   TBranch        *b_HLT_Mu6HT240_DisplacedDijet40_Inclusive1PtrkShortSig5_DisplacedLoose;   //!
   TBranch        *b_HLT_Mu6HT240_DisplacedDijet45_Inclusive0PtrkShortSig5;   //!
   TBranch        *b_HLT_Mu6HT240_DisplacedDijet50_Inclusive0PtrkShortSig5;   //!
   TBranch        *b_HLT_HT350;   //!
   TBranch        *b_HLT_HT425;   //!
   TBranch        *b_HLT_HT360_DisplacedDijet40_Inclusive1PtrkShortSig5;   //!
   TBranch        *b_HLT_HT360_DisplacedDijet45_Inclusive1PtrkShortSig5;   //!
   TBranch        *b_HLT_HT390_DisplacedDijet40_Inclusive1PtrkShortSig5;   //!
   TBranch        *b_HLT_HT390_DisplacedDijet45_Inclusive1PtrkShortSig5;   //!
   TBranch        *b_HLT_HT390eta2p0_DisplacedDijet40_Inclusive1PtrkShortSig5;   //!
   TBranch        *b_HLT_HT430_DisplacedDijet40_Inclusive1PtrkShortSig5;   //!
   TBranch        *b_HLT_HT400_DisplacedDijet40_DisplacedTrack;   //!
   TBranch        *b_HLT_HT430_DisplacedDijet40_DisplacedTrack;   //!
   TBranch        *b_HLT_HT550_DisplacedDijet60_Inclusive;   //!
   TBranch        *b_HLT_HT650_DisplacedDijet60_Inclusive;   //!
   TBranch        *b_HLT_CaloMET60_DTCluster50;   //!
   TBranch        *b_HLT_CaloMET60_DTClusterNoMB1S50;   //!
   TBranch        *b_HLT_L1MET_DTCluster50;   //!
   TBranch        *b_HLT_L1MET_DTClusterNoMB1S50;   //!
   TBranch        *b_HLT_CscCluster_Loose;   //!
   TBranch        *b_HLT_CscCluster_Medium;   //!
   TBranch        *b_HLT_CscCluster_Tight;   //!
   TBranch        *b_HLT_CscCluster50_Photon20Unseeded;   //!
   TBranch        *b_HLT_CscCluster50_Photon30Unseeded;   //!
   TBranch        *b_HLT_CscCluster100_Ele5;   //!
   TBranch        *b_HLT_CscCluster100_Mu5;   //!
   TBranch        *b_HLT_CscCluster100_PNetTauhPFJet10_Loose;   //!
   TBranch        *b_HLT_DoubleCscCluster75;   //!
   TBranch        *b_HLT_IsoTrk200_L1SingleMuShower;   //!
   TBranch        *b_HLT_IsoTrk400_L1SingleMuShower;   //!
   TBranch        *b_HLT_DoubleCscCluster100;   //!
   TBranch        *b_HLT_L1CSCShower_DTCluster50;   //!
   TBranch        *b_HLT_L1CSCShower_DTCluster75;   //!
   TBranch        *b_HLT_PFMET105_IsoTrk50;   //!
   TBranch        *b_HLT_L1SingleLLPJet;   //!
   TBranch        *b_HLT_HT170_L1SingleLLPJet_DisplacedDijet40_DisplacedTrack;   //!
   TBranch        *b_HLT_HT200_L1SingleLLPJet_DisplacedDijet40_DisplacedTrack;   //!
   TBranch        *b_HLT_HT200_L1SingleLLPJet_DisplacedDijet60_DisplacedTrack;   //!
   TBranch        *b_HLT_HT270_L1SingleLLPJet_DisplacedDijet40_DisplacedTrack;   //!
   TBranch        *b_HLT_HT200_L1SingleLLPJet_DisplacedDijet40_Inclusive1PtrkShortSig5;   //!
   TBranch        *b_HLT_HT240_L1SingleLLPJet_DisplacedDijet40_Inclusive1PtrkShortSig5;   //!
   TBranch        *b_HLT_HT280_L1SingleLLPJet_DisplacedDijet40_Inclusive1PtrkShortSig5;   //!
   TBranch        *b_HLT_HT320_L1SingleLLPJet_DisplacedDijet60_Inclusive;   //!
   TBranch        *b_HLT_HT420_L1SingleLLPJet_DisplacedDijet60_Inclusive;   //!
   TBranch        *b_HLT_HT200_L1SingleLLPJet_DelayedJet40_SingleDelay1nsTrackless;   //!
   TBranch        *b_HLT_HT200_L1SingleLLPJet_DelayedJet40_SingleDelay2nsInclusive;   //!
   TBranch        *b_HLT_HT200_L1SingleLLPJet_DelayedJet40_DoubleDelay0p5nsTrackless;   //!
   TBranch        *b_HLT_HT200_L1SingleLLPJet_DelayedJet40_DoubleDelay1nsInclusive;   //!
   TBranch        *b_HLT_HT200_L1SingleLLPJet_PFJet60_NeutralHadronFrac0p7;   //!
   TBranch        *b_HLT_HT200_L1SingleLLPJet_PFJet60_NeutralHadronFrac0p8;   //!
   TBranch        *b_HLT_DiPhoton10Time1ns;   //!
   TBranch        *b_HLT_DiPhoton10Time1p2ns;   //!
   TBranch        *b_HLT_DiPhoton10Time1p4ns;   //!
   TBranch        *b_HLT_DiPhoton10Time1p6ns;   //!
   TBranch        *b_HLT_DiPhoton10Time1p8ns;   //!
   TBranch        *b_HLT_DiPhoton10Time2ns;   //!
   TBranch        *b_HLT_DiPhoton10_CaloIdL;   //!
   TBranch        *b_HLT_DoubleEle6p5_eta1p22_mMax6;   //!
   TBranch        *b_HLT_DoubleEle8_eta1p22_mMax6;   //!
   TBranch        *b_HLT_DoubleEle10_eta1p22_mMax6;   //!
   TBranch        *b_HLT_SingleEle8;   //!
   TBranch        *b_HLT_SingleEle8_SingleEGL1;   //!
   TBranch        *b_HLT_Diphoton20_14_eta1p5_R9IdL_AND_HE_AND_IsoTCaloIdT;   //!
   TBranch        *b_HLT_Diphoton20_14_eta1p5_R9IdL_AND_HET_AND_IsoTCaloIdT;   //!
   TBranch        *b_HLT_Diphoton22_14_eta1p5_R9IdL_AND_HE_AND_IsoTCaloIdT;   //!
   TBranch        *b_HLT_Diphoton22_14_eta1p5_R9IdL_AND_HET_AND_IsoTCaloIdT;   //!
   TBranch        *b_HLT_Diphoton24_14_eta1p5_R9IdL_AND_HET_AND_IsoTCaloIdT;   //!
   TBranch        *b_HLT_Diphoton24_16_eta1p5_R9IdL_AND_HET_AND_IsoTCaloIdT;   //!
   TBranch        *b_HLT_Mu50_L1SingleMuShower;   //!
   TBranch        *b_HLT_IsoMu24_OneProng32;   //!
   TBranch        *b_HLT_Photon32_OneProng32_M50To105;   //!
   TBranch        *b_HLT_DoubleMediumDeepTauPFTauHPS30_L2NN_eta2p1_OneProng;   //!
   TBranch        *b_HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS30_L2NN_eta2p1_OneProng_CrossL1;   //!
   TBranch        *b_HLT_VBF_DiPFJet125_45_Mjj1050;   //!
   TBranch        *b_HLT_VBF_DiPFJet125_45_Mjj1200;   //!
   TBranch        *b_HLT_VBF_DiPFJet75_45_Mjj800_DiPFJet60;   //!
   TBranch        *b_HLT_VBF_DiPFJet75_45_Mjj850_DiPFJet60;   //!
   TBranch        *b_HLT_VBF_DiPFJet80_45_Mjj650_PFMETNoMu85;   //!
   TBranch        *b_HLT_VBF_DiPFJet80_45_Mjj750_PFMETNoMu85;   //!
   TBranch        *b_HLT_VBF_DiPFJet95_45_Mjj750_Mu3_TrkIsoVVL;   //!
   TBranch        *b_HLT_VBF_DiPFJet95_45_Mjj850_Mu3_TrkIsoVVL;   //!
   TBranch        *b_HLT_VBF_DiPFJet50_Mjj650_Photon22;   //!
   TBranch        *b_HLT_VBF_DiPFJet50_Mjj750_Photon22;   //!
   TBranch        *b_HLT_VBF_DiPFJet50_Mjj600_Ele22_eta2p1_WPTight_Gsf;   //!
   TBranch        *b_HLT_VBF_DiPFJet50_Mjj650_Ele22_eta2p1_WPTight_Gsf;   //!
   TBranch        *b_HLT_VBF_DiPFJet45_Mjj650_MediumDeepTauPFTauHPS45_L2NN_eta2p1;   //!
   TBranch        *b_HLT_VBF_DiPFJet45_Mjj750_MediumDeepTauPFTauHPS45_L2NN_eta2p1;   //!
   TBranch        *b_HLT_VBF_DoublePNetTauhPFJet20_eta2p2;   //!
   TBranch        *b_HLT_VBF_DiPFJet45_Mjj650_PNetTauhPFJet45_L2NN_eta2p3;   //!
   TBranch        *b_HLT_VBF_DiPFJet45_Mjj750_PNetTauhPFJet45_L2NN_eta2p3;   //!
   TBranch        *b_HLT_PFJet200_TimeLtNeg2p5ns;   //!
   TBranch        *b_HLT_PFJet200_TimeGt2p5ns;   //!
   TBranch        *b_HLT_Photon50_TimeLtNeg2p5ns;   //!
   TBranch        *b_HLT_Photon50_TimeGt2p5ns;   //!
   TBranch        *b_HLT_PPSMaxTracksPerArm1;   //!
   TBranch        *b_HLT_PPSMaxTracksPerRP4;   //!
   TBranch        *b_HLT_PPSRandom;   //!
   TBranch        *b_HLT_L1AXOVTight;   //!
   TBranch        *b_HLTriggerFinalPath;   //!
   TBranch        *b_Dataset_ScoutingPFMonitor;   //!
   TBranch        *b_Dataset_ScoutingPFRun3;   //!

   rootNtupleClass(TTree *tree=0);
   virtual ~rootNtupleClass();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual bool     Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef rootNtupleClass_cxx
rootNtupleClass::rootNtupleClass(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {

#ifdef SINGLE_TREE
      // The following code should be used if you want this class to access
      // a single tree instead of a chain
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("Memory Directory");
      if (!f || !f->IsOpen()) {
         f = new TFile("Memory Directory");
      }
      f->GetObject("Events",tree);

#else // SINGLE_TREE

      // The following code should be used if you want this class to access a chain
      // of trees.
      TChain * chain = new TChain("Events","");
      chain->Add("root://cmsxrootd.fnal.gov//store/data/Run2024G/ScoutingPFRun3/NANOAOD/ScoutNano-v1/2520000/9410d0f4-37b9-4e06-b482-12b12ab65aeb.root/Events");
      tree = chain;
#endif // SINGLE_TREE

   }
   Init(tree);
}

rootNtupleClass::~rootNtupleClass()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t rootNtupleClass::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t rootNtupleClass::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void rootNtupleClass::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("run", &run, &b_run);
   fChain->SetBranchAddress("luminosityBlock", &luminosityBlock, &b_luminosityBlock);
   fChain->SetBranchAddress("event", &event, &b_event);
   fChain->SetBranchAddress("bunchCrossing", &bunchCrossing, &b_bunchCrossing);
   fChain->SetBranchAddress("orbitNumber", &orbitNumber, &b_orbitNumber);
   fChain->SetBranchAddress("nL1EG", &nL1EG, &b_nL1EG);
   fChain->SetBranchAddress("L1EG_hwIso", L1EG_hwIso, &b_L1EG_hwIso);
   fChain->SetBranchAddress("L1EG_bx", L1EG_bx, &b_L1EG_bx);
   fChain->SetBranchAddress("L1EG_eta", L1EG_eta, &b_L1EG_eta);
   fChain->SetBranchAddress("L1EG_phi", L1EG_phi, &b_L1EG_phi);
   fChain->SetBranchAddress("L1EG_pt", L1EG_pt, &b_L1EG_pt);
   fChain->SetBranchAddress("nL1EtSum", &nL1EtSum, &b_nL1EtSum);
   fChain->SetBranchAddress("L1EtSum_bx", L1EtSum_bx, &b_L1EtSum_bx);
   fChain->SetBranchAddress("L1EtSum_etSumType", L1EtSum_etSumType, &b_L1EtSum_etSumType);
   fChain->SetBranchAddress("L1EtSum_phi", L1EtSum_phi, &b_L1EtSum_phi);
   fChain->SetBranchAddress("L1EtSum_pt", L1EtSum_pt, &b_L1EtSum_pt);
   fChain->SetBranchAddress("nL1Jet", &nL1Jet, &b_nL1Jet);
   fChain->SetBranchAddress("L1Jet_bx", L1Jet_bx, &b_L1Jet_bx);
   fChain->SetBranchAddress("L1Jet_eta", L1Jet_eta, &b_L1Jet_eta);
   fChain->SetBranchAddress("L1Jet_phi", L1Jet_phi, &b_L1Jet_phi);
   fChain->SetBranchAddress("L1Jet_pt", L1Jet_pt, &b_L1Jet_pt);
   fChain->SetBranchAddress("nL1Mu", &nL1Mu, &b_nL1Mu);
   fChain->SetBranchAddress("L1Mu_hwCharge", L1Mu_hwCharge, &b_L1Mu_hwCharge);
   fChain->SetBranchAddress("L1Mu_hwDXY", L1Mu_hwDXY, &b_L1Mu_hwDXY);
   fChain->SetBranchAddress("L1Mu_bx", L1Mu_bx, &b_L1Mu_bx);
   fChain->SetBranchAddress("L1Mu_hwQual", L1Mu_hwQual, &b_L1Mu_hwQual);
   fChain->SetBranchAddress("L1Mu_eta", L1Mu_eta, &b_L1Mu_eta);
   fChain->SetBranchAddress("L1Mu_etaAtVtx", L1Mu_etaAtVtx, &b_L1Mu_etaAtVtx);
   fChain->SetBranchAddress("L1Mu_phi", L1Mu_phi, &b_L1Mu_phi);
   fChain->SetBranchAddress("L1Mu_phiAtVtx", L1Mu_phiAtVtx, &b_L1Mu_phiAtVtx);
   fChain->SetBranchAddress("L1Mu_pt", L1Mu_pt, &b_L1Mu_pt);
   fChain->SetBranchAddress("L1Mu_ptUnconstrained", L1Mu_ptUnconstrained, &b_L1Mu_ptUnconstrained);
   fChain->SetBranchAddress("nL1Tau", &nL1Tau, &b_nL1Tau);
   fChain->SetBranchAddress("L1Tau_hwIso", L1Tau_hwIso, &b_L1Tau_hwIso);
   fChain->SetBranchAddress("L1Tau_bx", L1Tau_bx, &b_L1Tau_bx);
   fChain->SetBranchAddress("L1Tau_eta", L1Tau_eta, &b_L1Tau_eta);
   fChain->SetBranchAddress("L1Tau_phi", L1Tau_phi, &b_L1Tau_phi);
   fChain->SetBranchAddress("L1Tau_pt", L1Tau_pt, &b_L1Tau_pt);
   fChain->SetBranchAddress("nScoutingElectron", &nScoutingElectron, &b_nScoutingElectron);
   fChain->SetBranchAddress("ScoutingElectron_rechitZeroSuppression", ScoutingElectron_rechitZeroSuppression, &b_ScoutingElectron_rechitZeroSuppression);
   fChain->SetBranchAddress("ScoutingElectron_missingHits", ScoutingElectron_missingHits, &b_ScoutingElectron_missingHits);
   fChain->SetBranchAddress("ScoutingElectron_bestTrack_charge", ScoutingElectron_bestTrack_charge, &b_ScoutingElectron_bestTrack_charge);
   fChain->SetBranchAddress("ScoutingElectron_nClusters", ScoutingElectron_nClusters, &b_ScoutingElectron_nClusters);
   fChain->SetBranchAddress("ScoutingElectron_nCrystals", ScoutingElectron_nCrystals, &b_ScoutingElectron_nCrystals);
   fChain->SetBranchAddress("ScoutingElectron_seedId", ScoutingElectron_seedId, &b_ScoutingElectron_seedId);
   fChain->SetBranchAddress("ScoutingElectron_corrEcalEnergyError", ScoutingElectron_corrEcalEnergyError, &b_ScoutingElectron_corrEcalEnergyError);
   fChain->SetBranchAddress("ScoutingElectron_dEtaIn", ScoutingElectron_dEtaIn, &b_ScoutingElectron_dEtaIn);
   fChain->SetBranchAddress("ScoutingElectron_dPhiIn", ScoutingElectron_dPhiIn, &b_ScoutingElectron_dPhiIn);
   fChain->SetBranchAddress("ScoutingElectron_ecalIso", ScoutingElectron_ecalIso, &b_ScoutingElectron_ecalIso);
   fChain->SetBranchAddress("ScoutingElectron_eta", ScoutingElectron_eta, &b_ScoutingElectron_eta);
   fChain->SetBranchAddress("ScoutingElectron_hOverE", ScoutingElectron_hOverE, &b_ScoutingElectron_hOverE);
   fChain->SetBranchAddress("ScoutingElectron_hcalIso", ScoutingElectron_hcalIso, &b_ScoutingElectron_hcalIso);
   fChain->SetBranchAddress("ScoutingElectron_m", ScoutingElectron_m, &b_ScoutingElectron_m);
   fChain->SetBranchAddress("ScoutingElectron_ooEMOop", ScoutingElectron_ooEMOop, &b_ScoutingElectron_ooEMOop);
   fChain->SetBranchAddress("ScoutingElectron_phi", ScoutingElectron_phi, &b_ScoutingElectron_phi);
   fChain->SetBranchAddress("ScoutingElectron_preshowerEnergy", ScoutingElectron_preshowerEnergy, &b_ScoutingElectron_preshowerEnergy);
   fChain->SetBranchAddress("ScoutingElectron_pt", ScoutingElectron_pt, &b_ScoutingElectron_pt);
   fChain->SetBranchAddress("ScoutingElectron_r9", ScoutingElectron_r9, &b_ScoutingElectron_r9);
   fChain->SetBranchAddress("ScoutingElectron_rawEnergy", ScoutingElectron_rawEnergy, &b_ScoutingElectron_rawEnergy);
   fChain->SetBranchAddress("ScoutingElectron_sMaj", ScoutingElectron_sMaj, &b_ScoutingElectron_sMaj);
   fChain->SetBranchAddress("ScoutingElectron_sMin", ScoutingElectron_sMin, &b_ScoutingElectron_sMin);
   fChain->SetBranchAddress("ScoutingElectron_sigmaIetaIeta", ScoutingElectron_sigmaIetaIeta, &b_ScoutingElectron_sigmaIetaIeta);
   fChain->SetBranchAddress("ScoutingElectron_trackIso", ScoutingElectron_trackIso, &b_ScoutingElectron_trackIso);
   fChain->SetBranchAddress("ScoutingElectron_trackfbrem", ScoutingElectron_trackfbrem, &b_ScoutingElectron_trackfbrem);
   fChain->SetBranchAddress("ScoutingElectron_bestTrack_chi2overndf", ScoutingElectron_bestTrack_chi2overndf, &b_ScoutingElectron_bestTrack_chi2overndf);
   fChain->SetBranchAddress("ScoutingElectron_bestTrack_d0", ScoutingElectron_bestTrack_d0, &b_ScoutingElectron_bestTrack_d0);
   fChain->SetBranchAddress("ScoutingElectron_bestTrack_dz", ScoutingElectron_bestTrack_dz, &b_ScoutingElectron_bestTrack_dz);
   fChain->SetBranchAddress("ScoutingElectron_bestTrack_eta", ScoutingElectron_bestTrack_eta, &b_ScoutingElectron_bestTrack_eta);
   fChain->SetBranchAddress("ScoutingElectron_bestTrack_etaMode", ScoutingElectron_bestTrack_etaMode, &b_ScoutingElectron_bestTrack_etaMode);
   fChain->SetBranchAddress("ScoutingElectron_bestTrack_pMode", ScoutingElectron_bestTrack_pMode, &b_ScoutingElectron_bestTrack_pMode);
   fChain->SetBranchAddress("ScoutingElectron_bestTrack_phi", ScoutingElectron_bestTrack_phi, &b_ScoutingElectron_bestTrack_phi);
   fChain->SetBranchAddress("ScoutingElectron_bestTrack_phiMode", ScoutingElectron_bestTrack_phiMode, &b_ScoutingElectron_bestTrack_phiMode);
   fChain->SetBranchAddress("ScoutingElectron_bestTrack_pt", ScoutingElectron_bestTrack_pt, &b_ScoutingElectron_bestTrack_pt);
   fChain->SetBranchAddress("ScoutingElectron_bestTrack_qoverpModeError", ScoutingElectron_bestTrack_qoverpModeError, &b_ScoutingElectron_bestTrack_qoverpModeError);
   fChain->SetBranchAddress("nScoutingFatPFJetRecluster", &nScoutingFatPFJetRecluster, &b_nScoutingFatPFJetRecluster);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_nConstituents", ScoutingFatPFJetRecluster_nConstituents, &b_ScoutingFatPFJetRecluster_nConstituents);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_nCh", ScoutingFatPFJetRecluster_nCh, &b_ScoutingFatPFJetRecluster_nCh);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_nElectrons", ScoutingFatPFJetRecluster_nElectrons, &b_ScoutingFatPFJetRecluster_nElectrons);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_nMuons", ScoutingFatPFJetRecluster_nMuons, &b_ScoutingFatPFJetRecluster_nMuons);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_nNh", ScoutingFatPFJetRecluster_nNh, &b_ScoutingFatPFJetRecluster_nNh);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_nPhotons", ScoutingFatPFJetRecluster_nPhotons, &b_ScoutingFatPFJetRecluster_nPhotons);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_area", ScoutingFatPFJetRecluster_area, &b_ScoutingFatPFJetRecluster_area);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_chEmEF", ScoutingFatPFJetRecluster_chEmEF, &b_ScoutingFatPFJetRecluster_chEmEF);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_chHEF", ScoutingFatPFJetRecluster_chHEF, &b_ScoutingFatPFJetRecluster_chHEF);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_eta", ScoutingFatPFJetRecluster_eta, &b_ScoutingFatPFJetRecluster_eta);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_hfEmEF", ScoutingFatPFJetRecluster_hfEmEF, &b_ScoutingFatPFJetRecluster_hfEmEF);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_hfHEF", ScoutingFatPFJetRecluster_hfHEF, &b_ScoutingFatPFJetRecluster_hfHEF);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_mass", ScoutingFatPFJetRecluster_mass, &b_ScoutingFatPFJetRecluster_mass);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_muEF", ScoutingFatPFJetRecluster_muEF, &b_ScoutingFatPFJetRecluster_muEF);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_neEmEF", ScoutingFatPFJetRecluster_neEmEF, &b_ScoutingFatPFJetRecluster_neEmEF);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_neHEF", ScoutingFatPFJetRecluster_neHEF, &b_ScoutingFatPFJetRecluster_neHEF);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_phi", ScoutingFatPFJetRecluster_phi, &b_ScoutingFatPFJetRecluster_phi);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_pt", ScoutingFatPFJetRecluster_pt, &b_ScoutingFatPFJetRecluster_pt);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_msoftdrop", ScoutingFatPFJetRecluster_msoftdrop, &b_ScoutingFatPFJetRecluster_msoftdrop);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_n2b1", ScoutingFatPFJetRecluster_n2b1, &b_ScoutingFatPFJetRecluster_n2b1);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_n3b1", ScoutingFatPFJetRecluster_n3b1, &b_ScoutingFatPFJetRecluster_n3b1);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_particleNet_mass", ScoutingFatPFJetRecluster_particleNet_mass, &b_ScoutingFatPFJetRecluster_particleNet_mass);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_particleNet_prob_Hbb", ScoutingFatPFJetRecluster_particleNet_prob_Hbb, &b_ScoutingFatPFJetRecluster_particleNet_prob_Hbb);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_particleNet_prob_Hcc", ScoutingFatPFJetRecluster_particleNet_prob_Hcc, &b_ScoutingFatPFJetRecluster_particleNet_prob_Hcc);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_particleNet_prob_Hqq", ScoutingFatPFJetRecluster_particleNet_prob_Hqq, &b_ScoutingFatPFJetRecluster_particleNet_prob_Hqq);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_particleNet_prob_QCD", ScoutingFatPFJetRecluster_particleNet_prob_QCD, &b_ScoutingFatPFJetRecluster_particleNet_prob_QCD);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_scoutGlobalParT_massCorrGeneric", ScoutingFatPFJetRecluster_scoutGlobalParT_massCorrGeneric, &b_ScoutingFatPFJetRecluster_scoutGlobalParT_massCorrGeneric);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_scoutGlobalParT_massCorrGenericW2p", ScoutingFatPFJetRecluster_scoutGlobalParT_massCorrGenericW2p, &b_ScoutingFatPFJetRecluster_scoutGlobalParT_massCorrGenericW2p);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_scoutGlobalParT_massCorrGenericX2p", ScoutingFatPFJetRecluster_scoutGlobalParT_massCorrGenericX2p, &b_ScoutingFatPFJetRecluster_scoutGlobalParT_massCorrGenericX2p);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_scoutGlobalParT_massCorrResonance", ScoutingFatPFJetRecluster_scoutGlobalParT_massCorrResonance, &b_ScoutingFatPFJetRecluster_scoutGlobalParT_massCorrResonance);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_scoutGlobalParT_prob_QCD", ScoutingFatPFJetRecluster_scoutGlobalParT_prob_QCD, &b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_QCD);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xbb", ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xbb, &b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xbb);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xbc", ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xbc, &b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xbc);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xbs", ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xbs, &b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xbs);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xcc", ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xcc, &b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xcc);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xcs", ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xcs, &b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xcs);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xgg", ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xgg, &b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xgg);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xqq", ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xqq, &b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xqq);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xss", ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xss, &b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xss);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xtauhtaue", ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xtauhtaue, &b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xtauhtaue);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xtauhtauh", ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xtauhtauh, &b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xtauhtauh);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xtauhtaum", ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xtauhtaum, &b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xtauhtaum);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xud", ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xud, &b_ScoutingFatPFJetRecluster_scoutGlobalParT_prob_Xud);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_tau1", ScoutingFatPFJetRecluster_tau1, &b_ScoutingFatPFJetRecluster_tau1);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_tau2", ScoutingFatPFJetRecluster_tau2, &b_ScoutingFatPFJetRecluster_tau2);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_tau3", ScoutingFatPFJetRecluster_tau3, &b_ScoutingFatPFJetRecluster_tau3);
   fChain->SetBranchAddress("ScoutingFatPFJetRecluster_tau4", ScoutingFatPFJetRecluster_tau4, &b_ScoutingFatPFJetRecluster_tau4);
   fChain->SetBranchAddress("ScoutingMET_phi", &ScoutingMET_phi, &b_ScoutingMET_phi);
   fChain->SetBranchAddress("ScoutingMET_pt", &ScoutingMET_pt, &b_ScoutingMET_pt);
   fChain->SetBranchAddress("nScoutingMuonNoVtxDisplacedVertex", &nScoutingMuonNoVtxDisplacedVertex, &b_nScoutingMuonNoVtxDisplacedVertex);
   fChain->SetBranchAddress("ScoutingMuonNoVtxDisplacedVertex_isValidVtx", ScoutingMuonNoVtxDisplacedVertex_isValidVtx, &b_ScoutingMuonNoVtxDisplacedVertex_isValidVtx);
   fChain->SetBranchAddress("ScoutingMuonNoVtxDisplacedVertex_ndof", ScoutingMuonNoVtxDisplacedVertex_ndof, &b_ScoutingMuonNoVtxDisplacedVertex_ndof);
   fChain->SetBranchAddress("ScoutingMuonNoVtxDisplacedVertex_tracksSize", ScoutingMuonNoVtxDisplacedVertex_tracksSize, &b_ScoutingMuonNoVtxDisplacedVertex_tracksSize);
   fChain->SetBranchAddress("ScoutingMuonNoVtxDisplacedVertex_chi2", ScoutingMuonNoVtxDisplacedVertex_chi2, &b_ScoutingMuonNoVtxDisplacedVertex_chi2);
   fChain->SetBranchAddress("ScoutingMuonNoVtxDisplacedVertex_x", ScoutingMuonNoVtxDisplacedVertex_x, &b_ScoutingMuonNoVtxDisplacedVertex_x);
   fChain->SetBranchAddress("ScoutingMuonNoVtxDisplacedVertex_xError", ScoutingMuonNoVtxDisplacedVertex_xError, &b_ScoutingMuonNoVtxDisplacedVertex_xError);
   fChain->SetBranchAddress("ScoutingMuonNoVtxDisplacedVertex_xyCov", ScoutingMuonNoVtxDisplacedVertex_xyCov, &b_ScoutingMuonNoVtxDisplacedVertex_xyCov);
   fChain->SetBranchAddress("ScoutingMuonNoVtxDisplacedVertex_xzCov", ScoutingMuonNoVtxDisplacedVertex_xzCov, &b_ScoutingMuonNoVtxDisplacedVertex_xzCov);
   fChain->SetBranchAddress("ScoutingMuonNoVtxDisplacedVertex_y", ScoutingMuonNoVtxDisplacedVertex_y, &b_ScoutingMuonNoVtxDisplacedVertex_y);
   fChain->SetBranchAddress("ScoutingMuonNoVtxDisplacedVertex_yError", ScoutingMuonNoVtxDisplacedVertex_yError, &b_ScoutingMuonNoVtxDisplacedVertex_yError);
   fChain->SetBranchAddress("ScoutingMuonNoVtxDisplacedVertex_yzCov", ScoutingMuonNoVtxDisplacedVertex_yzCov, &b_ScoutingMuonNoVtxDisplacedVertex_yzCov);
   fChain->SetBranchAddress("ScoutingMuonNoVtxDisplacedVertex_z", ScoutingMuonNoVtxDisplacedVertex_z, &b_ScoutingMuonNoVtxDisplacedVertex_z);
   fChain->SetBranchAddress("ScoutingMuonNoVtxDisplacedVertex_zError", ScoutingMuonNoVtxDisplacedVertex_zError, &b_ScoutingMuonNoVtxDisplacedVertex_zError);
   fChain->SetBranchAddress("nScoutingMuonNoVtx", &nScoutingMuonNoVtx, &b_nScoutingMuonNoVtx);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_hitPattern_beginInner", ScoutingMuonNoVtx_trk_hitPattern_beginInner, &b_ScoutingMuonNoVtx_trk_hitPattern_beginInner);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_hitPattern_beginOuter", ScoutingMuonNoVtx_trk_hitPattern_beginOuter, &b_ScoutingMuonNoVtx_trk_hitPattern_beginOuter);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_hitPattern_beginTrackHits", ScoutingMuonNoVtx_trk_hitPattern_beginTrackHits, &b_ScoutingMuonNoVtx_trk_hitPattern_beginTrackHits);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_hitPattern_endInner", ScoutingMuonNoVtx_trk_hitPattern_endInner, &b_ScoutingMuonNoVtx_trk_hitPattern_endInner);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_hitPattern_endOuter", ScoutingMuonNoVtx_trk_hitPattern_endOuter, &b_ScoutingMuonNoVtx_trk_hitPattern_endOuter);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_hitPattern_endTrackHits", ScoutingMuonNoVtx_trk_hitPattern_endTrackHits, &b_ScoutingMuonNoVtx_trk_hitPattern_endTrackHits);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_hitPattern_hitCount", ScoutingMuonNoVtx_trk_hitPattern_hitCount, &b_ScoutingMuonNoVtx_trk_hitPattern_hitCount);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_nScoutingMuonNoVtxHitPattern", ScoutingMuonNoVtx_nScoutingMuonNoVtxHitPattern, &b_ScoutingMuonNoVtx_nScoutingMuonNoVtxHitPattern);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_oScoutingMuonNoVtxHitPattern", ScoutingMuonNoVtx_oScoutingMuonNoVtxHitPattern, &b_ScoutingMuonNoVtx_oScoutingMuonNoVtxHitPattern);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_nScoutingMuonNoVtxVtxIndx", ScoutingMuonNoVtx_nScoutingMuonNoVtxVtxIndx, &b_ScoutingMuonNoVtx_nScoutingMuonNoVtxVtxIndx);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_oScoutingMuonNoVtxVtxIndx", ScoutingMuonNoVtx_oScoutingMuonNoVtxVtxIndx, &b_ScoutingMuonNoVtx_oScoutingMuonNoVtxVtxIndx);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_charge", ScoutingMuonNoVtx_charge, &b_ScoutingMuonNoVtx_charge);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_nPixelLayersWithMeasurement", ScoutingMuonNoVtx_nPixelLayersWithMeasurement, &b_ScoutingMuonNoVtx_nPixelLayersWithMeasurement);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_nRecoMuonChambers", ScoutingMuonNoVtx_nRecoMuonChambers, &b_ScoutingMuonNoVtx_nRecoMuonChambers);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_nRecoMuonChambersCSCorDT", ScoutingMuonNoVtx_nRecoMuonChambersCSCorDT, &b_ScoutingMuonNoVtx_nRecoMuonChambersCSCorDT);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_nRecoMuonExpectedMatchedStations", ScoutingMuonNoVtx_nRecoMuonExpectedMatchedStations, &b_ScoutingMuonNoVtx_nRecoMuonExpectedMatchedStations);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_nRecoMuonMatchedRPCLayers", ScoutingMuonNoVtx_nRecoMuonMatchedRPCLayers, &b_ScoutingMuonNoVtx_nRecoMuonMatchedRPCLayers);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_nRecoMuonMatchedStations", ScoutingMuonNoVtx_nRecoMuonMatchedStations, &b_ScoutingMuonNoVtx_nRecoMuonMatchedStations);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_nRecoMuonMatches", ScoutingMuonNoVtx_nRecoMuonMatches, &b_ScoutingMuonNoVtx_nRecoMuonMatches);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_nStandAloneMuonMatchedStations", ScoutingMuonNoVtx_nStandAloneMuonMatchedStations, &b_ScoutingMuonNoVtx_nStandAloneMuonMatchedStations);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_nTrackerLayersWithMeasurement", ScoutingMuonNoVtx_nTrackerLayersWithMeasurement, &b_ScoutingMuonNoVtx_nTrackerLayersWithMeasurement);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_nValidPixelHits", ScoutingMuonNoVtx_nValidPixelHits, &b_ScoutingMuonNoVtx_nValidPixelHits);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_nValidRecoMuonHits", ScoutingMuonNoVtx_nValidRecoMuonHits, &b_ScoutingMuonNoVtx_nValidRecoMuonHits);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_nValidStandAloneMuonHits", ScoutingMuonNoVtx_nValidStandAloneMuonHits, &b_ScoutingMuonNoVtx_nValidStandAloneMuonHits);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_nValidStripHits", ScoutingMuonNoVtx_nValidStripHits, &b_ScoutingMuonNoVtx_nValidStripHits);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_recoMuonRPClayerMask", ScoutingMuonNoVtx_recoMuonRPClayerMask, &b_ScoutingMuonNoVtx_recoMuonRPClayerMask);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_recoMuonStationMask", ScoutingMuonNoVtx_recoMuonStationMask, &b_ScoutingMuonNoVtx_recoMuonStationMask);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_type", ScoutingMuonNoVtx_type, &b_ScoutingMuonNoVtx_type);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_ecalIso", ScoutingMuonNoVtx_ecalIso, &b_ScoutingMuonNoVtx_ecalIso);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_eta", ScoutingMuonNoVtx_eta, &b_ScoutingMuonNoVtx_eta);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_hcalIso", ScoutingMuonNoVtx_hcalIso, &b_ScoutingMuonNoVtx_hcalIso);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_m", ScoutingMuonNoVtx_m, &b_ScoutingMuonNoVtx_m);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_normchi2", ScoutingMuonNoVtx_normchi2, &b_ScoutingMuonNoVtx_normchi2);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_phi", ScoutingMuonNoVtx_phi, &b_ScoutingMuonNoVtx_phi);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_pt", ScoutingMuonNoVtx_pt, &b_ScoutingMuonNoVtx_pt);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trackIso", ScoutingMuonNoVtx_trackIso, &b_ScoutingMuonNoVtx_trackIso);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_chi2", ScoutingMuonNoVtx_trk_chi2, &b_ScoutingMuonNoVtx_trk_chi2);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_dsz", ScoutingMuonNoVtx_trk_dsz, &b_ScoutingMuonNoVtx_trk_dsz);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_dszError", ScoutingMuonNoVtx_trk_dszError, &b_ScoutingMuonNoVtx_trk_dszError);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_dxy", ScoutingMuonNoVtx_trk_dxy, &b_ScoutingMuonNoVtx_trk_dxy);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_dxyError", ScoutingMuonNoVtx_trk_dxyError, &b_ScoutingMuonNoVtx_trk_dxyError);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_dxy_dsz_cov", ScoutingMuonNoVtx_trk_dxy_dsz_cov, &b_ScoutingMuonNoVtx_trk_dxy_dsz_cov);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_dz", ScoutingMuonNoVtx_trk_dz, &b_ScoutingMuonNoVtx_trk_dz);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_dzError", ScoutingMuonNoVtx_trk_dzError, &b_ScoutingMuonNoVtx_trk_dzError);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_eta", ScoutingMuonNoVtx_trk_eta, &b_ScoutingMuonNoVtx_trk_eta);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_lambda", ScoutingMuonNoVtx_trk_lambda, &b_ScoutingMuonNoVtx_trk_lambda);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_lambdaError", ScoutingMuonNoVtx_trk_lambdaError, &b_ScoutingMuonNoVtx_trk_lambdaError);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_lambda_dsz_cov", ScoutingMuonNoVtx_trk_lambda_dsz_cov, &b_ScoutingMuonNoVtx_trk_lambda_dsz_cov);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_lambda_dxy_cov", ScoutingMuonNoVtx_trk_lambda_dxy_cov, &b_ScoutingMuonNoVtx_trk_lambda_dxy_cov);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_lambda_phi_cov", ScoutingMuonNoVtx_trk_lambda_phi_cov, &b_ScoutingMuonNoVtx_trk_lambda_phi_cov);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_ndof", ScoutingMuonNoVtx_trk_ndof, &b_ScoutingMuonNoVtx_trk_ndof);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_phi", ScoutingMuonNoVtx_trk_phi, &b_ScoutingMuonNoVtx_trk_phi);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_phiError", ScoutingMuonNoVtx_trk_phiError, &b_ScoutingMuonNoVtx_trk_phiError);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_phi_dsz_cov", ScoutingMuonNoVtx_trk_phi_dsz_cov, &b_ScoutingMuonNoVtx_trk_phi_dsz_cov);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_phi_dxy_cov", ScoutingMuonNoVtx_trk_phi_dxy_cov, &b_ScoutingMuonNoVtx_trk_phi_dxy_cov);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_pt", ScoutingMuonNoVtx_trk_pt, &b_ScoutingMuonNoVtx_trk_pt);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_qoverp", ScoutingMuonNoVtx_trk_qoverp, &b_ScoutingMuonNoVtx_trk_qoverp);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_qoverpError", ScoutingMuonNoVtx_trk_qoverpError, &b_ScoutingMuonNoVtx_trk_qoverpError);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_qoverp_dsz_cov", ScoutingMuonNoVtx_trk_qoverp_dsz_cov, &b_ScoutingMuonNoVtx_trk_qoverp_dsz_cov);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_qoverp_dxy_cov", ScoutingMuonNoVtx_trk_qoverp_dxy_cov, &b_ScoutingMuonNoVtx_trk_qoverp_dxy_cov);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_qoverp_lambda_cov", ScoutingMuonNoVtx_trk_qoverp_lambda_cov, &b_ScoutingMuonNoVtx_trk_qoverp_lambda_cov);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_qoverp_phi_cov", ScoutingMuonNoVtx_trk_qoverp_phi_cov, &b_ScoutingMuonNoVtx_trk_qoverp_phi_cov);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_vx", ScoutingMuonNoVtx_trk_vx, &b_ScoutingMuonNoVtx_trk_vx);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_vy", ScoutingMuonNoVtx_trk_vy, &b_ScoutingMuonNoVtx_trk_vy);
   fChain->SetBranchAddress("ScoutingMuonNoVtx_trk_vz", ScoutingMuonNoVtx_trk_vz, &b_ScoutingMuonNoVtx_trk_vz);
   fChain->SetBranchAddress("nScoutingMuonNoVtxHitPattern", &nScoutingMuonNoVtxHitPattern, &b_nScoutingMuonNoVtxHitPattern);
   fChain->SetBranchAddress("ScoutingMuonNoVtxHitPattern_hitPattern", ScoutingMuonNoVtxHitPattern_hitPattern, &b_ScoutingMuonNoVtxHitPattern_hitPattern);
   fChain->SetBranchAddress("nScoutingMuonNoVtxVtxIndx", &nScoutingMuonNoVtxVtxIndx, &b_nScoutingMuonNoVtxVtxIndx);
   fChain->SetBranchAddress("ScoutingMuonNoVtxVtxIndx_vtxIndx", ScoutingMuonNoVtxVtxIndx_vtxIndx, &b_ScoutingMuonNoVtxVtxIndx_vtxIndx);
   fChain->SetBranchAddress("nScoutingMuonVtxDisplacedVertex", &nScoutingMuonVtxDisplacedVertex, &b_nScoutingMuonVtxDisplacedVertex);
   fChain->SetBranchAddress("ScoutingMuonVtxDisplacedVertex_isValidVtx", &ScoutingMuonVtxDisplacedVertex_isValidVtx, &b_ScoutingMuonVtxDisplacedVertex_isValidVtx);
   fChain->SetBranchAddress("ScoutingMuonVtxDisplacedVertex_ndof", &ScoutingMuonVtxDisplacedVertex_ndof, &b_ScoutingMuonVtxDisplacedVertex_ndof);
   fChain->SetBranchAddress("ScoutingMuonVtxDisplacedVertex_tracksSize", &ScoutingMuonVtxDisplacedVertex_tracksSize, &b_ScoutingMuonVtxDisplacedVertex_tracksSize);
   fChain->SetBranchAddress("ScoutingMuonVtxDisplacedVertex_chi2", &ScoutingMuonVtxDisplacedVertex_chi2, &b_ScoutingMuonVtxDisplacedVertex_chi2);
   fChain->SetBranchAddress("ScoutingMuonVtxDisplacedVertex_x", &ScoutingMuonVtxDisplacedVertex_x, &b_ScoutingMuonVtxDisplacedVertex_x);
   fChain->SetBranchAddress("ScoutingMuonVtxDisplacedVertex_xError", &ScoutingMuonVtxDisplacedVertex_xError, &b_ScoutingMuonVtxDisplacedVertex_xError);
   fChain->SetBranchAddress("ScoutingMuonVtxDisplacedVertex_xyCov", &ScoutingMuonVtxDisplacedVertex_xyCov, &b_ScoutingMuonVtxDisplacedVertex_xyCov);
   fChain->SetBranchAddress("ScoutingMuonVtxDisplacedVertex_xzCov", &ScoutingMuonVtxDisplacedVertex_xzCov, &b_ScoutingMuonVtxDisplacedVertex_xzCov);
   fChain->SetBranchAddress("ScoutingMuonVtxDisplacedVertex_y", &ScoutingMuonVtxDisplacedVertex_y, &b_ScoutingMuonVtxDisplacedVertex_y);
   fChain->SetBranchAddress("ScoutingMuonVtxDisplacedVertex_yError", &ScoutingMuonVtxDisplacedVertex_yError, &b_ScoutingMuonVtxDisplacedVertex_yError);
   fChain->SetBranchAddress("ScoutingMuonVtxDisplacedVertex_yzCov", &ScoutingMuonVtxDisplacedVertex_yzCov, &b_ScoutingMuonVtxDisplacedVertex_yzCov);
   fChain->SetBranchAddress("ScoutingMuonVtxDisplacedVertex_z", &ScoutingMuonVtxDisplacedVertex_z, &b_ScoutingMuonVtxDisplacedVertex_z);
   fChain->SetBranchAddress("ScoutingMuonVtxDisplacedVertex_zError", &ScoutingMuonVtxDisplacedVertex_zError, &b_ScoutingMuonVtxDisplacedVertex_zError);
   fChain->SetBranchAddress("nScoutingMuonVtx", &nScoutingMuonVtx, &b_nScoutingMuonVtx);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_hitPattern_beginInner", ScoutingMuonVtx_trk_hitPattern_beginInner, &b_ScoutingMuonVtx_trk_hitPattern_beginInner);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_hitPattern_beginOuter", ScoutingMuonVtx_trk_hitPattern_beginOuter, &b_ScoutingMuonVtx_trk_hitPattern_beginOuter);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_hitPattern_beginTrackHits", ScoutingMuonVtx_trk_hitPattern_beginTrackHits, &b_ScoutingMuonVtx_trk_hitPattern_beginTrackHits);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_hitPattern_endInner", ScoutingMuonVtx_trk_hitPattern_endInner, &b_ScoutingMuonVtx_trk_hitPattern_endInner);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_hitPattern_endOuter", ScoutingMuonVtx_trk_hitPattern_endOuter, &b_ScoutingMuonVtx_trk_hitPattern_endOuter);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_hitPattern_endTrackHits", ScoutingMuonVtx_trk_hitPattern_endTrackHits, &b_ScoutingMuonVtx_trk_hitPattern_endTrackHits);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_hitPattern_hitCount", ScoutingMuonVtx_trk_hitPattern_hitCount, &b_ScoutingMuonVtx_trk_hitPattern_hitCount);
   fChain->SetBranchAddress("ScoutingMuonVtx_nScoutingMuonVtxHitPattern", ScoutingMuonVtx_nScoutingMuonVtxHitPattern, &b_ScoutingMuonVtx_nScoutingMuonVtxHitPattern);
   fChain->SetBranchAddress("ScoutingMuonVtx_oScoutingMuonVtxHitPattern", ScoutingMuonVtx_oScoutingMuonVtxHitPattern, &b_ScoutingMuonVtx_oScoutingMuonVtxHitPattern);
   fChain->SetBranchAddress("ScoutingMuonVtx_nScoutingMuonVtxVtxIndx", ScoutingMuonVtx_nScoutingMuonVtxVtxIndx, &b_ScoutingMuonVtx_nScoutingMuonVtxVtxIndx);
   fChain->SetBranchAddress("ScoutingMuonVtx_oScoutingMuonVtxVtxIndx", ScoutingMuonVtx_oScoutingMuonVtxVtxIndx, &b_ScoutingMuonVtx_oScoutingMuonVtxVtxIndx);
   fChain->SetBranchAddress("ScoutingMuonVtx_charge", ScoutingMuonVtx_charge, &b_ScoutingMuonVtx_charge);
   fChain->SetBranchAddress("ScoutingMuonVtx_nPixelLayersWithMeasurement", ScoutingMuonVtx_nPixelLayersWithMeasurement, &b_ScoutingMuonVtx_nPixelLayersWithMeasurement);
   fChain->SetBranchAddress("ScoutingMuonVtx_nRecoMuonChambers", ScoutingMuonVtx_nRecoMuonChambers, &b_ScoutingMuonVtx_nRecoMuonChambers);
   fChain->SetBranchAddress("ScoutingMuonVtx_nRecoMuonChambersCSCorDT", ScoutingMuonVtx_nRecoMuonChambersCSCorDT, &b_ScoutingMuonVtx_nRecoMuonChambersCSCorDT);
   fChain->SetBranchAddress("ScoutingMuonVtx_nRecoMuonExpectedMatchedStations", ScoutingMuonVtx_nRecoMuonExpectedMatchedStations, &b_ScoutingMuonVtx_nRecoMuonExpectedMatchedStations);
   fChain->SetBranchAddress("ScoutingMuonVtx_nRecoMuonMatchedRPCLayers", ScoutingMuonVtx_nRecoMuonMatchedRPCLayers, &b_ScoutingMuonVtx_nRecoMuonMatchedRPCLayers);
   fChain->SetBranchAddress("ScoutingMuonVtx_nRecoMuonMatchedStations", ScoutingMuonVtx_nRecoMuonMatchedStations, &b_ScoutingMuonVtx_nRecoMuonMatchedStations);
   fChain->SetBranchAddress("ScoutingMuonVtx_nRecoMuonMatches", ScoutingMuonVtx_nRecoMuonMatches, &b_ScoutingMuonVtx_nRecoMuonMatches);
   fChain->SetBranchAddress("ScoutingMuonVtx_nStandAloneMuonMatchedStations", ScoutingMuonVtx_nStandAloneMuonMatchedStations, &b_ScoutingMuonVtx_nStandAloneMuonMatchedStations);
   fChain->SetBranchAddress("ScoutingMuonVtx_nTrackerLayersWithMeasurement", ScoutingMuonVtx_nTrackerLayersWithMeasurement, &b_ScoutingMuonVtx_nTrackerLayersWithMeasurement);
   fChain->SetBranchAddress("ScoutingMuonVtx_nValidPixelHits", ScoutingMuonVtx_nValidPixelHits, &b_ScoutingMuonVtx_nValidPixelHits);
   fChain->SetBranchAddress("ScoutingMuonVtx_nValidRecoMuonHits", ScoutingMuonVtx_nValidRecoMuonHits, &b_ScoutingMuonVtx_nValidRecoMuonHits);
   fChain->SetBranchAddress("ScoutingMuonVtx_nValidStandAloneMuonHits", ScoutingMuonVtx_nValidStandAloneMuonHits, &b_ScoutingMuonVtx_nValidStandAloneMuonHits);
   fChain->SetBranchAddress("ScoutingMuonVtx_nValidStripHits", ScoutingMuonVtx_nValidStripHits, &b_ScoutingMuonVtx_nValidStripHits);
   fChain->SetBranchAddress("ScoutingMuonVtx_recoMuonRPClayerMask", ScoutingMuonVtx_recoMuonRPClayerMask, &b_ScoutingMuonVtx_recoMuonRPClayerMask);
   fChain->SetBranchAddress("ScoutingMuonVtx_recoMuonStationMask", ScoutingMuonVtx_recoMuonStationMask, &b_ScoutingMuonVtx_recoMuonStationMask);
   fChain->SetBranchAddress("ScoutingMuonVtx_type", ScoutingMuonVtx_type, &b_ScoutingMuonVtx_type);
   fChain->SetBranchAddress("ScoutingMuonVtx_ecalIso", ScoutingMuonVtx_ecalIso, &b_ScoutingMuonVtx_ecalIso);
   fChain->SetBranchAddress("ScoutingMuonVtx_eta", ScoutingMuonVtx_eta, &b_ScoutingMuonVtx_eta);
   fChain->SetBranchAddress("ScoutingMuonVtx_hcalIso", ScoutingMuonVtx_hcalIso, &b_ScoutingMuonVtx_hcalIso);
   fChain->SetBranchAddress("ScoutingMuonVtx_m", ScoutingMuonVtx_m, &b_ScoutingMuonVtx_m);
   fChain->SetBranchAddress("ScoutingMuonVtx_normchi2", ScoutingMuonVtx_normchi2, &b_ScoutingMuonVtx_normchi2);
   fChain->SetBranchAddress("ScoutingMuonVtx_phi", ScoutingMuonVtx_phi, &b_ScoutingMuonVtx_phi);
   fChain->SetBranchAddress("ScoutingMuonVtx_pt", ScoutingMuonVtx_pt, &b_ScoutingMuonVtx_pt);
   fChain->SetBranchAddress("ScoutingMuonVtx_trackIso", ScoutingMuonVtx_trackIso, &b_ScoutingMuonVtx_trackIso);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_chi2", ScoutingMuonVtx_trk_chi2, &b_ScoutingMuonVtx_trk_chi2);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_dsz", ScoutingMuonVtx_trk_dsz, &b_ScoutingMuonVtx_trk_dsz);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_dszError", ScoutingMuonVtx_trk_dszError, &b_ScoutingMuonVtx_trk_dszError);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_dxy", ScoutingMuonVtx_trk_dxy, &b_ScoutingMuonVtx_trk_dxy);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_dxyError", ScoutingMuonVtx_trk_dxyError, &b_ScoutingMuonVtx_trk_dxyError);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_dxy_dsz_cov", ScoutingMuonVtx_trk_dxy_dsz_cov, &b_ScoutingMuonVtx_trk_dxy_dsz_cov);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_dz", ScoutingMuonVtx_trk_dz, &b_ScoutingMuonVtx_trk_dz);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_dzError", ScoutingMuonVtx_trk_dzError, &b_ScoutingMuonVtx_trk_dzError);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_eta", ScoutingMuonVtx_trk_eta, &b_ScoutingMuonVtx_trk_eta);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_lambda", ScoutingMuonVtx_trk_lambda, &b_ScoutingMuonVtx_trk_lambda);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_lambdaError", ScoutingMuonVtx_trk_lambdaError, &b_ScoutingMuonVtx_trk_lambdaError);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_lambda_dsz_cov", ScoutingMuonVtx_trk_lambda_dsz_cov, &b_ScoutingMuonVtx_trk_lambda_dsz_cov);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_lambda_dxy_cov", ScoutingMuonVtx_trk_lambda_dxy_cov, &b_ScoutingMuonVtx_trk_lambda_dxy_cov);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_lambda_phi_cov", ScoutingMuonVtx_trk_lambda_phi_cov, &b_ScoutingMuonVtx_trk_lambda_phi_cov);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_ndof", ScoutingMuonVtx_trk_ndof, &b_ScoutingMuonVtx_trk_ndof);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_phi", ScoutingMuonVtx_trk_phi, &b_ScoutingMuonVtx_trk_phi);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_phiError", ScoutingMuonVtx_trk_phiError, &b_ScoutingMuonVtx_trk_phiError);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_phi_dsz_cov", ScoutingMuonVtx_trk_phi_dsz_cov, &b_ScoutingMuonVtx_trk_phi_dsz_cov);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_phi_dxy_cov", ScoutingMuonVtx_trk_phi_dxy_cov, &b_ScoutingMuonVtx_trk_phi_dxy_cov);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_pt", ScoutingMuonVtx_trk_pt, &b_ScoutingMuonVtx_trk_pt);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_qoverp", ScoutingMuonVtx_trk_qoverp, &b_ScoutingMuonVtx_trk_qoverp);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_qoverpError", ScoutingMuonVtx_trk_qoverpError, &b_ScoutingMuonVtx_trk_qoverpError);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_qoverp_dsz_cov", ScoutingMuonVtx_trk_qoverp_dsz_cov, &b_ScoutingMuonVtx_trk_qoverp_dsz_cov);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_qoverp_dxy_cov", ScoutingMuonVtx_trk_qoverp_dxy_cov, &b_ScoutingMuonVtx_trk_qoverp_dxy_cov);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_qoverp_lambda_cov", ScoutingMuonVtx_trk_qoverp_lambda_cov, &b_ScoutingMuonVtx_trk_qoverp_lambda_cov);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_qoverp_phi_cov", ScoutingMuonVtx_trk_qoverp_phi_cov, &b_ScoutingMuonVtx_trk_qoverp_phi_cov);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_vx", ScoutingMuonVtx_trk_vx, &b_ScoutingMuonVtx_trk_vx);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_vy", ScoutingMuonVtx_trk_vy, &b_ScoutingMuonVtx_trk_vy);
   fChain->SetBranchAddress("ScoutingMuonVtx_trk_vz", ScoutingMuonVtx_trk_vz, &b_ScoutingMuonVtx_trk_vz);
   fChain->SetBranchAddress("nScoutingMuonVtxHitPattern", &nScoutingMuonVtxHitPattern, &b_nScoutingMuonVtxHitPattern);
   fChain->SetBranchAddress("ScoutingMuonVtxHitPattern_hitPattern", ScoutingMuonVtxHitPattern_hitPattern, &b_ScoutingMuonVtxHitPattern_hitPattern);
   fChain->SetBranchAddress("nScoutingMuonVtxVtxIndx", &nScoutingMuonVtxVtxIndx, &b_nScoutingMuonVtxVtxIndx);
   fChain->SetBranchAddress("ScoutingMuonVtxVtxIndx_vtxIndx", &ScoutingMuonVtxVtxIndx_vtxIndx, &b_ScoutingMuonVtxVtxIndx_vtxIndx);
   fChain->SetBranchAddress("nScoutingPFJetRecluster", &nScoutingPFJetRecluster, &b_nScoutingPFJetRecluster);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_nConstituents", ScoutingPFJetRecluster_nConstituents, &b_ScoutingPFJetRecluster_nConstituents);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_nCh", ScoutingPFJetRecluster_nCh, &b_ScoutingPFJetRecluster_nCh);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_nElectrons", ScoutingPFJetRecluster_nElectrons, &b_ScoutingPFJetRecluster_nElectrons);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_nMuons", ScoutingPFJetRecluster_nMuons, &b_ScoutingPFJetRecluster_nMuons);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_nNh", ScoutingPFJetRecluster_nNh, &b_ScoutingPFJetRecluster_nNh);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_nPhotons", ScoutingPFJetRecluster_nPhotons, &b_ScoutingPFJetRecluster_nPhotons);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_area", ScoutingPFJetRecluster_area, &b_ScoutingPFJetRecluster_area);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_chEmEF", ScoutingPFJetRecluster_chEmEF, &b_ScoutingPFJetRecluster_chEmEF);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_chHEF", ScoutingPFJetRecluster_chHEF, &b_ScoutingPFJetRecluster_chHEF);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_eta", ScoutingPFJetRecluster_eta, &b_ScoutingPFJetRecluster_eta);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_hfEmEF", ScoutingPFJetRecluster_hfEmEF, &b_ScoutingPFJetRecluster_hfEmEF);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_hfHEF", ScoutingPFJetRecluster_hfHEF, &b_ScoutingPFJetRecluster_hfHEF);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_mass", ScoutingPFJetRecluster_mass, &b_ScoutingPFJetRecluster_mass);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_muEF", ScoutingPFJetRecluster_muEF, &b_ScoutingPFJetRecluster_muEF);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_neEmEF", ScoutingPFJetRecluster_neEmEF, &b_ScoutingPFJetRecluster_neEmEF);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_neHEF", ScoutingPFJetRecluster_neHEF, &b_ScoutingPFJetRecluster_neHEF);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_phi", ScoutingPFJetRecluster_phi, &b_ScoutingPFJetRecluster_phi);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_pt", ScoutingPFJetRecluster_pt, &b_ScoutingPFJetRecluster_pt);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_particleNet_prob_b", ScoutingPFJetRecluster_particleNet_prob_b, &b_ScoutingPFJetRecluster_particleNet_prob_b);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_particleNet_prob_bb", ScoutingPFJetRecluster_particleNet_prob_bb, &b_ScoutingPFJetRecluster_particleNet_prob_bb);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_particleNet_prob_c", ScoutingPFJetRecluster_particleNet_prob_c, &b_ScoutingPFJetRecluster_particleNet_prob_c);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_particleNet_prob_cc", ScoutingPFJetRecluster_particleNet_prob_cc, &b_ScoutingPFJetRecluster_particleNet_prob_cc);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_particleNet_prob_g", ScoutingPFJetRecluster_particleNet_prob_g, &b_ScoutingPFJetRecluster_particleNet_prob_g);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_particleNet_prob_uds", ScoutingPFJetRecluster_particleNet_prob_uds, &b_ScoutingPFJetRecluster_particleNet_prob_uds);
   fChain->SetBranchAddress("ScoutingPFJetRecluster_particleNet_prob_undef", ScoutingPFJetRecluster_particleNet_prob_undef, &b_ScoutingPFJetRecluster_particleNet_prob_undef);
   fChain->SetBranchAddress("nScoutingPFJet", &nScoutingPFJet, &b_nScoutingPFJet);
   fChain->SetBranchAddress("ScoutingPFJet_HFEMMultiplicity", ScoutingPFJet_HFEMMultiplicity, &b_ScoutingPFJet_HFEMMultiplicity);
   fChain->SetBranchAddress("ScoutingPFJet_HFHadronMultiplicity", ScoutingPFJet_HFHadronMultiplicity, &b_ScoutingPFJet_HFHadronMultiplicity);
   fChain->SetBranchAddress("ScoutingPFJet_chargedHadronMultiplicity", ScoutingPFJet_chargedHadronMultiplicity, &b_ScoutingPFJet_chargedHadronMultiplicity);
   fChain->SetBranchAddress("ScoutingPFJet_electronMultiplicity", ScoutingPFJet_electronMultiplicity, &b_ScoutingPFJet_electronMultiplicity);
   fChain->SetBranchAddress("ScoutingPFJet_muonMultiplicity", ScoutingPFJet_muonMultiplicity, &b_ScoutingPFJet_muonMultiplicity);
   fChain->SetBranchAddress("ScoutingPFJet_neutralHadronMultiplicity", ScoutingPFJet_neutralHadronMultiplicity, &b_ScoutingPFJet_neutralHadronMultiplicity);
   fChain->SetBranchAddress("ScoutingPFJet_photonMultiplicity", ScoutingPFJet_photonMultiplicity, &b_ScoutingPFJet_photonMultiplicity);
   fChain->SetBranchAddress("ScoutingPFJet_HFEMEnergy", ScoutingPFJet_HFEMEnergy, &b_ScoutingPFJet_HFEMEnergy);
   fChain->SetBranchAddress("ScoutingPFJet_HFHadronEnergy", ScoutingPFJet_HFHadronEnergy, &b_ScoutingPFJet_HFHadronEnergy);
   fChain->SetBranchAddress("ScoutingPFJet_HOEnergy", ScoutingPFJet_HOEnergy, &b_ScoutingPFJet_HOEnergy);
   fChain->SetBranchAddress("ScoutingPFJet_chargedHadronEnergy", ScoutingPFJet_chargedHadronEnergy, &b_ScoutingPFJet_chargedHadronEnergy);
   fChain->SetBranchAddress("ScoutingPFJet_electronEnergy", ScoutingPFJet_electronEnergy, &b_ScoutingPFJet_electronEnergy);
   fChain->SetBranchAddress("ScoutingPFJet_eta", ScoutingPFJet_eta, &b_ScoutingPFJet_eta);
   fChain->SetBranchAddress("ScoutingPFJet_jetArea", ScoutingPFJet_jetArea, &b_ScoutingPFJet_jetArea);
   fChain->SetBranchAddress("ScoutingPFJet_m", ScoutingPFJet_m, &b_ScoutingPFJet_m);
   fChain->SetBranchAddress("ScoutingPFJet_muonEnergy", ScoutingPFJet_muonEnergy, &b_ScoutingPFJet_muonEnergy);
   fChain->SetBranchAddress("ScoutingPFJet_neutralHadronEnergy", ScoutingPFJet_neutralHadronEnergy, &b_ScoutingPFJet_neutralHadronEnergy);
   fChain->SetBranchAddress("ScoutingPFJet_phi", ScoutingPFJet_phi, &b_ScoutingPFJet_phi);
   fChain->SetBranchAddress("ScoutingPFJet_photonEnergy", ScoutingPFJet_photonEnergy, &b_ScoutingPFJet_photonEnergy);
   fChain->SetBranchAddress("ScoutingPFJet_pt", ScoutingPFJet_pt, &b_ScoutingPFJet_pt);
   fChain->SetBranchAddress("nScoutingPhoton", &nScoutingPhoton, &b_nScoutingPhoton);
   fChain->SetBranchAddress("ScoutingPhoton_rechitZeroSuppression", ScoutingPhoton_rechitZeroSuppression, &b_ScoutingPhoton_rechitZeroSuppression);
   fChain->SetBranchAddress("ScoutingPhoton_nClusters", ScoutingPhoton_nClusters, &b_ScoutingPhoton_nClusters);
   fChain->SetBranchAddress("ScoutingPhoton_nCrystals", ScoutingPhoton_nCrystals, &b_ScoutingPhoton_nCrystals);
   fChain->SetBranchAddress("ScoutingPhoton_seedId", ScoutingPhoton_seedId, &b_ScoutingPhoton_seedId);
   fChain->SetBranchAddress("ScoutingPhoton_corrEcalEnergyError", ScoutingPhoton_corrEcalEnergyError, &b_ScoutingPhoton_corrEcalEnergyError);
   fChain->SetBranchAddress("ScoutingPhoton_ecalIso", ScoutingPhoton_ecalIso, &b_ScoutingPhoton_ecalIso);
   fChain->SetBranchAddress("ScoutingPhoton_eta", ScoutingPhoton_eta, &b_ScoutingPhoton_eta);
   fChain->SetBranchAddress("ScoutingPhoton_hOverE", ScoutingPhoton_hOverE, &b_ScoutingPhoton_hOverE);
   fChain->SetBranchAddress("ScoutingPhoton_hcalIso", ScoutingPhoton_hcalIso, &b_ScoutingPhoton_hcalIso);
   fChain->SetBranchAddress("ScoutingPhoton_m", ScoutingPhoton_m, &b_ScoutingPhoton_m);
   fChain->SetBranchAddress("ScoutingPhoton_phi", ScoutingPhoton_phi, &b_ScoutingPhoton_phi);
   fChain->SetBranchAddress("ScoutingPhoton_preshowerEnergy", ScoutingPhoton_preshowerEnergy, &b_ScoutingPhoton_preshowerEnergy);
   fChain->SetBranchAddress("ScoutingPhoton_pt", ScoutingPhoton_pt, &b_ScoutingPhoton_pt);
   fChain->SetBranchAddress("ScoutingPhoton_r9", ScoutingPhoton_r9, &b_ScoutingPhoton_r9);
   fChain->SetBranchAddress("ScoutingPhoton_rawEnergy", ScoutingPhoton_rawEnergy, &b_ScoutingPhoton_rawEnergy);
   fChain->SetBranchAddress("ScoutingPhoton_sMaj", ScoutingPhoton_sMaj, &b_ScoutingPhoton_sMaj);
   fChain->SetBranchAddress("ScoutingPhoton_sMin", ScoutingPhoton_sMin, &b_ScoutingPhoton_sMin);
   fChain->SetBranchAddress("ScoutingPhoton_sigmaIetaIeta", ScoutingPhoton_sigmaIetaIeta, &b_ScoutingPhoton_sigmaIetaIeta);
   fChain->SetBranchAddress("ScoutingPhoton_trkIso", ScoutingPhoton_trkIso, &b_ScoutingPhoton_trkIso);
   fChain->SetBranchAddress("nScoutingPrimaryVertex", &nScoutingPrimaryVertex, &b_nScoutingPrimaryVertex);
   fChain->SetBranchAddress("ScoutingPrimaryVertex_isValidVtx", ScoutingPrimaryVertex_isValidVtx, &b_ScoutingPrimaryVertex_isValidVtx);
   fChain->SetBranchAddress("ScoutingPrimaryVertex_ndof", ScoutingPrimaryVertex_ndof, &b_ScoutingPrimaryVertex_ndof);
   fChain->SetBranchAddress("ScoutingPrimaryVertex_tracksSize", ScoutingPrimaryVertex_tracksSize, &b_ScoutingPrimaryVertex_tracksSize);
   fChain->SetBranchAddress("ScoutingPrimaryVertex_chi2", ScoutingPrimaryVertex_chi2, &b_ScoutingPrimaryVertex_chi2);
   fChain->SetBranchAddress("ScoutingPrimaryVertex_x", ScoutingPrimaryVertex_x, &b_ScoutingPrimaryVertex_x);
   fChain->SetBranchAddress("ScoutingPrimaryVertex_xError", ScoutingPrimaryVertex_xError, &b_ScoutingPrimaryVertex_xError);
   fChain->SetBranchAddress("ScoutingPrimaryVertex_xyCov", ScoutingPrimaryVertex_xyCov, &b_ScoutingPrimaryVertex_xyCov);
   fChain->SetBranchAddress("ScoutingPrimaryVertex_xzCov", ScoutingPrimaryVertex_xzCov, &b_ScoutingPrimaryVertex_xzCov);
   fChain->SetBranchAddress("ScoutingPrimaryVertex_y", ScoutingPrimaryVertex_y, &b_ScoutingPrimaryVertex_y);
   fChain->SetBranchAddress("ScoutingPrimaryVertex_yError", ScoutingPrimaryVertex_yError, &b_ScoutingPrimaryVertex_yError);
   fChain->SetBranchAddress("ScoutingPrimaryVertex_yzCov", ScoutingPrimaryVertex_yzCov, &b_ScoutingPrimaryVertex_yzCov);
   fChain->SetBranchAddress("ScoutingPrimaryVertex_z", ScoutingPrimaryVertex_z, &b_ScoutingPrimaryVertex_z);
   fChain->SetBranchAddress("ScoutingPrimaryVertex_zError", ScoutingPrimaryVertex_zError, &b_ScoutingPrimaryVertex_zError);
   fChain->SetBranchAddress("ScoutingRho_fixedGridRhoFastjetAll", &ScoutingRho_fixedGridRhoFastjetAll, &b_ScoutingRho_fixedGridRhoFastjetAll);
   fChain->SetBranchAddress("L1_AXO_Loose", &L1_AXO_Loose, &b_L1_AXO_Loose);
   fChain->SetBranchAddress("L1_AXO_Nominal", &L1_AXO_Nominal, &b_L1_AXO_Nominal);
   fChain->SetBranchAddress("L1_AXO_Tight", &L1_AXO_Tight, &b_L1_AXO_Tight);
   fChain->SetBranchAddress("L1_AXO_VLoose", &L1_AXO_VLoose, &b_L1_AXO_VLoose);
   fChain->SetBranchAddress("L1_AXO_VTight", &L1_AXO_VTight, &b_L1_AXO_VTight);
   fChain->SetBranchAddress("L1_AlwaysTrue", &L1_AlwaysTrue, &b_L1_AlwaysTrue);
   fChain->SetBranchAddress("L1_BPTX_AND_Ref1_VME", &L1_BPTX_AND_Ref1_VME, &b_L1_BPTX_AND_Ref1_VME);
   fChain->SetBranchAddress("L1_BPTX_AND_Ref3_VME", &L1_BPTX_AND_Ref3_VME, &b_L1_BPTX_AND_Ref3_VME);
   fChain->SetBranchAddress("L1_BPTX_AND_Ref4_VME", &L1_BPTX_AND_Ref4_VME, &b_L1_BPTX_AND_Ref4_VME);
   fChain->SetBranchAddress("L1_BPTX_BeamGas_B1_VME", &L1_BPTX_BeamGas_B1_VME, &b_L1_BPTX_BeamGas_B1_VME);
   fChain->SetBranchAddress("L1_BPTX_BeamGas_B2_VME", &L1_BPTX_BeamGas_B2_VME, &b_L1_BPTX_BeamGas_B2_VME);
   fChain->SetBranchAddress("L1_BPTX_BeamGas_Ref1_VME", &L1_BPTX_BeamGas_Ref1_VME, &b_L1_BPTX_BeamGas_Ref1_VME);
   fChain->SetBranchAddress("L1_BPTX_BeamGas_Ref2_VME", &L1_BPTX_BeamGas_Ref2_VME, &b_L1_BPTX_BeamGas_Ref2_VME);
   fChain->SetBranchAddress("L1_BPTX_NotOR_VME", &L1_BPTX_NotOR_VME, &b_L1_BPTX_NotOR_VME);
   fChain->SetBranchAddress("L1_BPTX_OR_Ref3_VME", &L1_BPTX_OR_Ref3_VME, &b_L1_BPTX_OR_Ref3_VME);
   fChain->SetBranchAddress("L1_BPTX_OR_Ref4_VME", &L1_BPTX_OR_Ref4_VME, &b_L1_BPTX_OR_Ref4_VME);
   fChain->SetBranchAddress("L1_BPTX_RefAND_VME", &L1_BPTX_RefAND_VME, &b_L1_BPTX_RefAND_VME);
   fChain->SetBranchAddress("L1_BptxMinus", &L1_BptxMinus, &b_L1_BptxMinus);
   fChain->SetBranchAddress("L1_BptxOR", &L1_BptxOR, &b_L1_BptxOR);
   fChain->SetBranchAddress("L1_BptxPlus", &L1_BptxPlus, &b_L1_BptxPlus);
   fChain->SetBranchAddress("L1_BptxXOR", &L1_BptxXOR, &b_L1_BptxXOR);
   fChain->SetBranchAddress("L1_CDC_SingleMu_3_er1p2_TOP120_DPHI2p618_3p142", &L1_CDC_SingleMu_3_er1p2_TOP120_DPHI2p618_3p142, &b_L1_CDC_SingleMu_3_er1p2_TOP120_DPHI2p618_3p142);
   fChain->SetBranchAddress("L1_CICADA_Loose", &L1_CICADA_Loose, &b_L1_CICADA_Loose);
   fChain->SetBranchAddress("L1_CICADA_Medium", &L1_CICADA_Medium, &b_L1_CICADA_Medium);
   fChain->SetBranchAddress("L1_CICADA_Tight", &L1_CICADA_Tight, &b_L1_CICADA_Tight);
   fChain->SetBranchAddress("L1_CICADA_VLoose", &L1_CICADA_VLoose, &b_L1_CICADA_VLoose);
   fChain->SetBranchAddress("L1_CICADA_VTight", &L1_CICADA_VTight, &b_L1_CICADA_VTight);
   fChain->SetBranchAddress("L1_DoubleEG11_er1p2_dR_Max0p6", &L1_DoubleEG11_er1p2_dR_Max0p6, &b_L1_DoubleEG11_er1p2_dR_Max0p6);
   fChain->SetBranchAddress("L1_DoubleEG8er2p5_HTT280er", &L1_DoubleEG8er2p5_HTT280er, &b_L1_DoubleEG8er2p5_HTT280er);
   fChain->SetBranchAddress("L1_DoubleEG8er2p5_HTT300er", &L1_DoubleEG8er2p5_HTT300er, &b_L1_DoubleEG8er2p5_HTT300er);
   fChain->SetBranchAddress("L1_DoubleEG8er2p5_HTT320er", &L1_DoubleEG8er2p5_HTT320er, &b_L1_DoubleEG8er2p5_HTT320er);
   fChain->SetBranchAddress("L1_DoubleEG_15_10_er2p5", &L1_DoubleEG_15_10_er2p5, &b_L1_DoubleEG_15_10_er2p5);
   fChain->SetBranchAddress("L1_DoubleEG_20_10_er2p5", &L1_DoubleEG_20_10_er2p5, &b_L1_DoubleEG_20_10_er2p5);
   fChain->SetBranchAddress("L1_DoubleEG_22_10_er2p5", &L1_DoubleEG_22_10_er2p5, &b_L1_DoubleEG_22_10_er2p5);
   fChain->SetBranchAddress("L1_DoubleEG_25_12_er2p5", &L1_DoubleEG_25_12_er2p5, &b_L1_DoubleEG_25_12_er2p5);
   fChain->SetBranchAddress("L1_DoubleEG_25_14_er2p5", &L1_DoubleEG_25_14_er2p5, &b_L1_DoubleEG_25_14_er2p5);
   fChain->SetBranchAddress("L1_DoubleEG_27_14_er2p5", &L1_DoubleEG_27_14_er2p5, &b_L1_DoubleEG_27_14_er2p5);
   fChain->SetBranchAddress("L1_DoubleEG_LooseIso16_LooseIso12_er1p5", &L1_DoubleEG_LooseIso16_LooseIso12_er1p5, &b_L1_DoubleEG_LooseIso16_LooseIso12_er1p5);
   fChain->SetBranchAddress("L1_DoubleEG_LooseIso18_LooseIso12_er1p5", &L1_DoubleEG_LooseIso18_LooseIso12_er1p5, &b_L1_DoubleEG_LooseIso18_LooseIso12_er1p5);
   fChain->SetBranchAddress("L1_DoubleEG_LooseIso20_LooseIso12_er1p5", &L1_DoubleEG_LooseIso20_LooseIso12_er1p5, &b_L1_DoubleEG_LooseIso20_LooseIso12_er1p5);
   fChain->SetBranchAddress("L1_DoubleEG_LooseIso22_12_er2p5", &L1_DoubleEG_LooseIso22_12_er2p5, &b_L1_DoubleEG_LooseIso22_12_er2p5);
   fChain->SetBranchAddress("L1_DoubleEG_LooseIso22_LooseIso12_er1p5", &L1_DoubleEG_LooseIso22_LooseIso12_er1p5, &b_L1_DoubleEG_LooseIso22_LooseIso12_er1p5);
   fChain->SetBranchAddress("L1_DoubleEG_LooseIso25_12_er2p5", &L1_DoubleEG_LooseIso25_12_er2p5, &b_L1_DoubleEG_LooseIso25_12_er2p5);
   fChain->SetBranchAddress("L1_DoubleEG_LooseIso25_LooseIso12_er1p5", &L1_DoubleEG_LooseIso25_LooseIso12_er1p5, &b_L1_DoubleEG_LooseIso25_LooseIso12_er1p5);
   fChain->SetBranchAddress("L1_DoubleIsoTau26er2p1_Jet55_RmOvlp_dR0p5", &L1_DoubleIsoTau26er2p1_Jet55_RmOvlp_dR0p5, &b_L1_DoubleIsoTau26er2p1_Jet55_RmOvlp_dR0p5);
   fChain->SetBranchAddress("L1_DoubleIsoTau26er2p1_Jet70_RmOvlp_dR0p5", &L1_DoubleIsoTau26er2p1_Jet70_RmOvlp_dR0p5, &b_L1_DoubleIsoTau26er2p1_Jet70_RmOvlp_dR0p5);
   fChain->SetBranchAddress("L1_DoubleIsoTau28er2p1", &L1_DoubleIsoTau28er2p1, &b_L1_DoubleIsoTau28er2p1);
   fChain->SetBranchAddress("L1_DoubleIsoTau30er2p1", &L1_DoubleIsoTau30er2p1, &b_L1_DoubleIsoTau30er2p1);
   fChain->SetBranchAddress("L1_DoubleIsoTau32er2p1", &L1_DoubleIsoTau32er2p1, &b_L1_DoubleIsoTau32er2p1);
   fChain->SetBranchAddress("L1_DoubleIsoTau32er2p1_Mass_Max80", &L1_DoubleIsoTau32er2p1_Mass_Max80, &b_L1_DoubleIsoTau32er2p1_Mass_Max80);
   fChain->SetBranchAddress("L1_DoubleIsoTau34er2p1", &L1_DoubleIsoTau34er2p1, &b_L1_DoubleIsoTau34er2p1);
   fChain->SetBranchAddress("L1_DoubleIsoTau35er2p1", &L1_DoubleIsoTau35er2p1, &b_L1_DoubleIsoTau35er2p1);
   fChain->SetBranchAddress("L1_DoubleIsoTau36er2p1", &L1_DoubleIsoTau36er2p1, &b_L1_DoubleIsoTau36er2p1);
   fChain->SetBranchAddress("L1_DoubleJet100er2p3_dEta_Max1p6", &L1_DoubleJet100er2p3_dEta_Max1p6, &b_L1_DoubleJet100er2p3_dEta_Max1p6);
   fChain->SetBranchAddress("L1_DoubleJet100er2p5", &L1_DoubleJet100er2p5, &b_L1_DoubleJet100er2p5);
   fChain->SetBranchAddress("L1_DoubleJet112er2p3_dEta_Max1p6", &L1_DoubleJet112er2p3_dEta_Max1p6, &b_L1_DoubleJet112er2p3_dEta_Max1p6);
   fChain->SetBranchAddress("L1_DoubleJet120er2p5", &L1_DoubleJet120er2p5, &b_L1_DoubleJet120er2p5);
   fChain->SetBranchAddress("L1_DoubleJet120er2p5_Mu3_dR_Max0p8", &L1_DoubleJet120er2p5_Mu3_dR_Max0p8, &b_L1_DoubleJet120er2p5_Mu3_dR_Max0p8);
   fChain->SetBranchAddress("L1_DoubleJet150er2p5", &L1_DoubleJet150er2p5, &b_L1_DoubleJet150er2p5);
   fChain->SetBranchAddress("L1_DoubleJet16er2p5_Mu3_dR_Max0p4", &L1_DoubleJet16er2p5_Mu3_dR_Max0p4, &b_L1_DoubleJet16er2p5_Mu3_dR_Max0p4);
   fChain->SetBranchAddress("L1_DoubleJet30er2p5_Mass_Min225_dEta_Max1p5", &L1_DoubleJet30er2p5_Mass_Min225_dEta_Max1p5, &b_L1_DoubleJet30er2p5_Mass_Min225_dEta_Max1p5);
   fChain->SetBranchAddress("L1_DoubleJet30er2p5_Mass_Min250_dEta_Max1p5", &L1_DoubleJet30er2p5_Mass_Min250_dEta_Max1p5, &b_L1_DoubleJet30er2p5_Mass_Min250_dEta_Max1p5);
   fChain->SetBranchAddress("L1_DoubleJet30er2p5_Mass_Min300_dEta_Max1p5", &L1_DoubleJet30er2p5_Mass_Min300_dEta_Max1p5, &b_L1_DoubleJet30er2p5_Mass_Min300_dEta_Max1p5);
   fChain->SetBranchAddress("L1_DoubleJet30er2p5_Mass_Min330_dEta_Max1p5", &L1_DoubleJet30er2p5_Mass_Min330_dEta_Max1p5, &b_L1_DoubleJet30er2p5_Mass_Min330_dEta_Max1p5);
   fChain->SetBranchAddress("L1_DoubleJet35er2p5_Mu3_dR_Max0p4", &L1_DoubleJet35er2p5_Mu3_dR_Max0p4, &b_L1_DoubleJet35er2p5_Mu3_dR_Max0p4);
   fChain->SetBranchAddress("L1_DoubleJet40er2p5", &L1_DoubleJet40er2p5, &b_L1_DoubleJet40er2p5);
   fChain->SetBranchAddress("L1_DoubleJet45_Mass_Min550_IsoTau45er2p1_RmOvlp_dR0p5", &L1_DoubleJet45_Mass_Min550_IsoTau45er2p1_RmOvlp_dR0p5, &b_L1_DoubleJet45_Mass_Min550_IsoTau45er2p1_RmOvlp_dR0p5);
   fChain->SetBranchAddress("L1_DoubleJet45_Mass_Min550_LooseIsoEG20er2p1_RmOvlp_dR0p2", &L1_DoubleJet45_Mass_Min550_LooseIsoEG20er2p1_RmOvlp_dR0p2, &b_L1_DoubleJet45_Mass_Min550_LooseIsoEG20er2p1_RmOvlp_dR0p2);
   fChain->SetBranchAddress("L1_DoubleJet45_Mass_Min600_IsoTau45er2p1_RmOvlp_dR0p5", &L1_DoubleJet45_Mass_Min600_IsoTau45er2p1_RmOvlp_dR0p5, &b_L1_DoubleJet45_Mass_Min600_IsoTau45er2p1_RmOvlp_dR0p5);
   fChain->SetBranchAddress("L1_DoubleJet45_Mass_Min600_LooseIsoEG20er2p1_RmOvlp_dR0p2", &L1_DoubleJet45_Mass_Min600_LooseIsoEG20er2p1_RmOvlp_dR0p2, &b_L1_DoubleJet45_Mass_Min600_LooseIsoEG20er2p1_RmOvlp_dR0p2);
   fChain->SetBranchAddress("L1_DoubleJet60er2p5_Mu3_dR_Max0p4", &L1_DoubleJet60er2p5_Mu3_dR_Max0p4, &b_L1_DoubleJet60er2p5_Mu3_dR_Max0p4);
   fChain->SetBranchAddress("L1_DoubleJet80er2p5_Mu3_dR_Max0p4", &L1_DoubleJet80er2p5_Mu3_dR_Max0p4, &b_L1_DoubleJet80er2p5_Mu3_dR_Max0p4);
   fChain->SetBranchAddress("L1_DoubleJet_110_35_DoubleJet35_Mass_Min800", &L1_DoubleJet_110_35_DoubleJet35_Mass_Min800, &b_L1_DoubleJet_110_35_DoubleJet35_Mass_Min800);
   fChain->SetBranchAddress("L1_DoubleJet_110_35_DoubleJet35_Mass_Min850", &L1_DoubleJet_110_35_DoubleJet35_Mass_Min850, &b_L1_DoubleJet_110_35_DoubleJet35_Mass_Min850);
   fChain->SetBranchAddress("L1_DoubleJet_65_35_DoubleJet35_Mass_Min600_DoubleJetCentral50", &L1_DoubleJet_65_35_DoubleJet35_Mass_Min600_DoubleJetCentral50, &b_L1_DoubleJet_65_35_DoubleJet35_Mass_Min600_DoubleJetCentral50);
   fChain->SetBranchAddress("L1_DoubleJet_65_35_DoubleJet35_Mass_Min650_DoubleJetCentral50", &L1_DoubleJet_65_35_DoubleJet35_Mass_Min650_DoubleJetCentral50, &b_L1_DoubleJet_65_35_DoubleJet35_Mass_Min650_DoubleJetCentral50);
   fChain->SetBranchAddress("L1_DoubleJet_70_35_DoubleJet35_Mass_Min500_ETMHF65", &L1_DoubleJet_70_35_DoubleJet35_Mass_Min500_ETMHF65, &b_L1_DoubleJet_70_35_DoubleJet35_Mass_Min500_ETMHF65);
   fChain->SetBranchAddress("L1_DoubleJet_70_35_DoubleJet35_Mass_Min550_ETMHF65", &L1_DoubleJet_70_35_DoubleJet35_Mass_Min550_ETMHF65, &b_L1_DoubleJet_70_35_DoubleJet35_Mass_Min550_ETMHF65);
   fChain->SetBranchAddress("L1_DoubleJet_85_35_DoubleJet35_Mass_Min600_Mu3OQ", &L1_DoubleJet_85_35_DoubleJet35_Mass_Min600_Mu3OQ, &b_L1_DoubleJet_85_35_DoubleJet35_Mass_Min600_Mu3OQ);
   fChain->SetBranchAddress("L1_DoubleJet_85_35_DoubleJet35_Mass_Min650_Mu3OQ", &L1_DoubleJet_85_35_DoubleJet35_Mass_Min650_Mu3OQ, &b_L1_DoubleJet_85_35_DoubleJet35_Mass_Min650_Mu3OQ);
   fChain->SetBranchAddress("L1_DoubleLLPJet40", &L1_DoubleLLPJet40, &b_L1_DoubleLLPJet40);
   fChain->SetBranchAddress("L1_DoubleLooseIsoEG22er2p1", &L1_DoubleLooseIsoEG22er2p1, &b_L1_DoubleLooseIsoEG22er2p1);
   fChain->SetBranchAddress("L1_DoubleLooseIsoEG24er2p1", &L1_DoubleLooseIsoEG24er2p1, &b_L1_DoubleLooseIsoEG24er2p1);
   fChain->SetBranchAddress("L1_DoubleMu0", &L1_DoubleMu0, &b_L1_DoubleMu0);
   fChain->SetBranchAddress("L1_DoubleMu0_Mass_Min1", &L1_DoubleMu0_Mass_Min1, &b_L1_DoubleMu0_Mass_Min1);
   fChain->SetBranchAddress("L1_DoubleMu0_OQ", &L1_DoubleMu0_OQ, &b_L1_DoubleMu0_OQ);
   fChain->SetBranchAddress("L1_DoubleMu0_SQ", &L1_DoubleMu0_SQ, &b_L1_DoubleMu0_SQ);
   fChain->SetBranchAddress("L1_DoubleMu0_SQ_OS", &L1_DoubleMu0_SQ_OS, &b_L1_DoubleMu0_SQ_OS);
   fChain->SetBranchAddress("L1_DoubleMu0_Upt15_Upt7", &L1_DoubleMu0_Upt15_Upt7, &b_L1_DoubleMu0_Upt15_Upt7);
   fChain->SetBranchAddress("L1_DoubleMu0_Upt5_Upt5", &L1_DoubleMu0_Upt5_Upt5, &b_L1_DoubleMu0_Upt5_Upt5);
   fChain->SetBranchAddress("L1_DoubleMu0_Upt6_IP_Min1_Upt4", &L1_DoubleMu0_Upt6_IP_Min1_Upt4, &b_L1_DoubleMu0_Upt6_IP_Min1_Upt4);
   fChain->SetBranchAddress("L1_DoubleMu0_Upt6_SQ_er2p0", &L1_DoubleMu0_Upt6_SQ_er2p0, &b_L1_DoubleMu0_Upt6_SQ_er2p0);
   fChain->SetBranchAddress("L1_DoubleMu0_Upt7_SQ_er2p0", &L1_DoubleMu0_Upt7_SQ_er2p0, &b_L1_DoubleMu0_Upt7_SQ_er2p0);
   fChain->SetBranchAddress("L1_DoubleMu0_Upt8_SQ_er2p0", &L1_DoubleMu0_Upt8_SQ_er2p0, &b_L1_DoubleMu0_Upt8_SQ_er2p0);
   fChain->SetBranchAddress("L1_DoubleMu0_dR_Max1p6_Jet90er2p5_dR_Max0p8", &L1_DoubleMu0_dR_Max1p6_Jet90er2p5_dR_Max0p8, &b_L1_DoubleMu0_dR_Max1p6_Jet90er2p5_dR_Max0p8);
   fChain->SetBranchAddress("L1_DoubleMu0er1p4_OQ_OS_dEta_Max1p6", &L1_DoubleMu0er1p4_OQ_OS_dEta_Max1p6, &b_L1_DoubleMu0er1p4_OQ_OS_dEta_Max1p6);
   fChain->SetBranchAddress("L1_DoubleMu0er1p4_SQ_OS_dEta_Max1p2", &L1_DoubleMu0er1p4_SQ_OS_dEta_Max1p2, &b_L1_DoubleMu0er1p4_SQ_OS_dEta_Max1p2);
   fChain->SetBranchAddress("L1_DoubleMu0er1p4_SQ_OS_dR_Max1p4", &L1_DoubleMu0er1p4_SQ_OS_dR_Max1p4, &b_L1_DoubleMu0er1p4_SQ_OS_dR_Max1p4);
   fChain->SetBranchAddress("L1_DoubleMu0er1p5_SQ", &L1_DoubleMu0er1p5_SQ, &b_L1_DoubleMu0er1p5_SQ);
   fChain->SetBranchAddress("L1_DoubleMu0er1p5_SQ_OS", &L1_DoubleMu0er1p5_SQ_OS, &b_L1_DoubleMu0er1p5_SQ_OS);
   fChain->SetBranchAddress("L1_DoubleMu0er1p5_SQ_OS_dEta_Max1p2", &L1_DoubleMu0er1p5_SQ_OS_dEta_Max1p2, &b_L1_DoubleMu0er1p5_SQ_OS_dEta_Max1p2);
   fChain->SetBranchAddress("L1_DoubleMu0er1p5_SQ_OS_dR_Max1p4", &L1_DoubleMu0er1p5_SQ_OS_dR_Max1p4, &b_L1_DoubleMu0er1p5_SQ_OS_dR_Max1p4);
   fChain->SetBranchAddress("L1_DoubleMu0er1p5_SQ_dR_Max1p4", &L1_DoubleMu0er1p5_SQ_dR_Max1p4, &b_L1_DoubleMu0er1p5_SQ_dR_Max1p4);
   fChain->SetBranchAddress("L1_DoubleMu0er2p0_SQ_OS_dEta_Max0p3_dPhi_0p8to1p2", &L1_DoubleMu0er2p0_SQ_OS_dEta_Max0p3_dPhi_0p8to1p2, &b_L1_DoubleMu0er2p0_SQ_OS_dEta_Max0p3_dPhi_0p8to1p2);
   fChain->SetBranchAddress("L1_DoubleMu0er2p0_SQ_OS_dEta_Max1p5", &L1_DoubleMu0er2p0_SQ_OS_dEta_Max1p5, &b_L1_DoubleMu0er2p0_SQ_OS_dEta_Max1p5);
   fChain->SetBranchAddress("L1_DoubleMu0er2p0_SQ_OS_dEta_Max1p6", &L1_DoubleMu0er2p0_SQ_OS_dEta_Max1p6, &b_L1_DoubleMu0er2p0_SQ_OS_dEta_Max1p6);
   fChain->SetBranchAddress("L1_DoubleMu0er2p0_SQ_dEta_Max1p5", &L1_DoubleMu0er2p0_SQ_dEta_Max1p5, &b_L1_DoubleMu0er2p0_SQ_dEta_Max1p5);
   fChain->SetBranchAddress("L1_DoubleMu0er2p0_SQ_dEta_Max1p6", &L1_DoubleMu0er2p0_SQ_dEta_Max1p6, &b_L1_DoubleMu0er2p0_SQ_dEta_Max1p6);
   fChain->SetBranchAddress("L1_DoubleMu18er2p1_SQ", &L1_DoubleMu18er2p1_SQ, &b_L1_DoubleMu18er2p1_SQ);
   fChain->SetBranchAddress("L1_DoubleMu3_OS_er2p3_Mass_Max14_DoubleEG7p5_er2p1_Mass_Max20", &L1_DoubleMu3_OS_er2p3_Mass_Max14_DoubleEG7p5_er2p1_Mass_Max20, &b_L1_DoubleMu3_OS_er2p3_Mass_Max14_DoubleEG7p5_er2p1_Mass_Max20);
   fChain->SetBranchAddress("L1_DoubleMu3_SQ_ETMHF30_HTT60er", &L1_DoubleMu3_SQ_ETMHF30_HTT60er, &b_L1_DoubleMu3_SQ_ETMHF30_HTT60er);
   fChain->SetBranchAddress("L1_DoubleMu3_SQ_ETMHF30_Jet60er2p5_OR_DoubleJet40er2p5", &L1_DoubleMu3_SQ_ETMHF30_Jet60er2p5_OR_DoubleJet40er2p5, &b_L1_DoubleMu3_SQ_ETMHF30_Jet60er2p5_OR_DoubleJet40er2p5);
   fChain->SetBranchAddress("L1_DoubleMu3_SQ_ETMHF40_HTT60er", &L1_DoubleMu3_SQ_ETMHF40_HTT60er, &b_L1_DoubleMu3_SQ_ETMHF40_HTT60er);
   fChain->SetBranchAddress("L1_DoubleMu3_SQ_ETMHF40_Jet60er2p5_OR_DoubleJet40er2p5", &L1_DoubleMu3_SQ_ETMHF40_Jet60er2p5_OR_DoubleJet40er2p5, &b_L1_DoubleMu3_SQ_ETMHF40_Jet60er2p5_OR_DoubleJet40er2p5);
   fChain->SetBranchAddress("L1_DoubleMu3_SQ_ETMHF50_HTT60er", &L1_DoubleMu3_SQ_ETMHF50_HTT60er, &b_L1_DoubleMu3_SQ_ETMHF50_HTT60er);
   fChain->SetBranchAddress("L1_DoubleMu3_SQ_ETMHF50_Jet60er2p5", &L1_DoubleMu3_SQ_ETMHF50_Jet60er2p5, &b_L1_DoubleMu3_SQ_ETMHF50_Jet60er2p5);
   fChain->SetBranchAddress("L1_DoubleMu3_SQ_ETMHF50_Jet60er2p5_OR_DoubleJet40er2p5", &L1_DoubleMu3_SQ_ETMHF50_Jet60er2p5_OR_DoubleJet40er2p5, &b_L1_DoubleMu3_SQ_ETMHF50_Jet60er2p5_OR_DoubleJet40er2p5);
   fChain->SetBranchAddress("L1_DoubleMu3_SQ_ETMHF60_Jet60er2p5", &L1_DoubleMu3_SQ_ETMHF60_Jet60er2p5, &b_L1_DoubleMu3_SQ_ETMHF60_Jet60er2p5);
   fChain->SetBranchAddress("L1_DoubleMu3_SQ_HTT220er", &L1_DoubleMu3_SQ_HTT220er, &b_L1_DoubleMu3_SQ_HTT220er);
   fChain->SetBranchAddress("L1_DoubleMu3er2p0_SQ_OS_dR_Max1p6", &L1_DoubleMu3er2p0_SQ_OS_dR_Max1p6, &b_L1_DoubleMu3er2p0_SQ_OS_dR_Max1p6);
   fChain->SetBranchAddress("L1_DoubleMu4_SQ_EG9er2p5", &L1_DoubleMu4_SQ_EG9er2p5, &b_L1_DoubleMu4_SQ_EG9er2p5);
   fChain->SetBranchAddress("L1_DoubleMu4_SQ_OS", &L1_DoubleMu4_SQ_OS, &b_L1_DoubleMu4_SQ_OS);
   fChain->SetBranchAddress("L1_DoubleMu4_SQ_OS_dR_Max1p2", &L1_DoubleMu4_SQ_OS_dR_Max1p2, &b_L1_DoubleMu4_SQ_OS_dR_Max1p2);
   fChain->SetBranchAddress("L1_DoubleMu4er2p0_SQ_OS_dR_Max1p6", &L1_DoubleMu4er2p0_SQ_OS_dR_Max1p6, &b_L1_DoubleMu4er2p0_SQ_OS_dR_Max1p6);
   fChain->SetBranchAddress("L1_DoubleMu4p5_SQ_OS", &L1_DoubleMu4p5_SQ_OS, &b_L1_DoubleMu4p5_SQ_OS);
   fChain->SetBranchAddress("L1_DoubleMu4p5_SQ_OS_dR_Max1p2", &L1_DoubleMu4p5_SQ_OS_dR_Max1p2, &b_L1_DoubleMu4p5_SQ_OS_dR_Max1p2);
   fChain->SetBranchAddress("L1_DoubleMu4p5er2p0_SQ_OS", &L1_DoubleMu4p5er2p0_SQ_OS, &b_L1_DoubleMu4p5er2p0_SQ_OS);
   fChain->SetBranchAddress("L1_DoubleMu4p5er2p0_SQ_OS_Mass_7to18", &L1_DoubleMu4p5er2p0_SQ_OS_Mass_7to18, &b_L1_DoubleMu4p5er2p0_SQ_OS_Mass_7to18);
   fChain->SetBranchAddress("L1_DoubleMu4p5er2p0_SQ_OS_Mass_Min7", &L1_DoubleMu4p5er2p0_SQ_OS_Mass_Min7, &b_L1_DoubleMu4p5er2p0_SQ_OS_Mass_Min7);
   fChain->SetBranchAddress("L1_DoubleMu5_OS_er2p3_Mass_8to14_DoubleEG3er2p1_Mass_Max20", &L1_DoubleMu5_OS_er2p3_Mass_8to14_DoubleEG3er2p1_Mass_Max20, &b_L1_DoubleMu5_OS_er2p3_Mass_8to14_DoubleEG3er2p1_Mass_Max20);
   fChain->SetBranchAddress("L1_DoubleMu5_SQ_EG9er2p5", &L1_DoubleMu5_SQ_EG9er2p5, &b_L1_DoubleMu5_SQ_EG9er2p5);
   fChain->SetBranchAddress("L1_DoubleMu5_SQ_OS_dR_Max1p6", &L1_DoubleMu5_SQ_OS_dR_Max1p6, &b_L1_DoubleMu5_SQ_OS_dR_Max1p6);
   fChain->SetBranchAddress("L1_DoubleMu6_Upt6_SQ_er2p0", &L1_DoubleMu6_Upt6_SQ_er2p0, &b_L1_DoubleMu6_Upt6_SQ_er2p0);
   fChain->SetBranchAddress("L1_DoubleMu7_Upt7_SQ_er2p0", &L1_DoubleMu7_Upt7_SQ_er2p0, &b_L1_DoubleMu7_Upt7_SQ_er2p0);
   fChain->SetBranchAddress("L1_DoubleMu8_SQ", &L1_DoubleMu8_SQ, &b_L1_DoubleMu8_SQ);
   fChain->SetBranchAddress("L1_DoubleMu8_Upt8_SQ_er2p0", &L1_DoubleMu8_Upt8_SQ_er2p0, &b_L1_DoubleMu8_Upt8_SQ_er2p0);
   fChain->SetBranchAddress("L1_DoubleMu9_SQ", &L1_DoubleMu9_SQ, &b_L1_DoubleMu9_SQ);
   fChain->SetBranchAddress("L1_DoubleMu_12_5", &L1_DoubleMu_12_5, &b_L1_DoubleMu_12_5);
   fChain->SetBranchAddress("L1_DoubleMu_15_5_SQ", &L1_DoubleMu_15_5_SQ, &b_L1_DoubleMu_15_5_SQ);
   fChain->SetBranchAddress("L1_DoubleMu_15_7", &L1_DoubleMu_15_7, &b_L1_DoubleMu_15_7);
   fChain->SetBranchAddress("L1_DoubleMu_15_7_Mass_Min1", &L1_DoubleMu_15_7_Mass_Min1, &b_L1_DoubleMu_15_7_Mass_Min1);
   fChain->SetBranchAddress("L1_DoubleMu_15_7_SQ", &L1_DoubleMu_15_7_SQ, &b_L1_DoubleMu_15_7_SQ);
   fChain->SetBranchAddress("L1_DoubleTau70er2p1", &L1_DoubleTau70er2p1, &b_L1_DoubleTau70er2p1);
   fChain->SetBranchAddress("L1_ETM120", &L1_ETM120, &b_L1_ETM120);
   fChain->SetBranchAddress("L1_ETM150", &L1_ETM150, &b_L1_ETM150);
   fChain->SetBranchAddress("L1_ETMHF100", &L1_ETMHF100, &b_L1_ETMHF100);
   fChain->SetBranchAddress("L1_ETMHF100_HTT60er", &L1_ETMHF100_HTT60er, &b_L1_ETMHF100_HTT60er);
   fChain->SetBranchAddress("L1_ETMHF110", &L1_ETMHF110, &b_L1_ETMHF110);
   fChain->SetBranchAddress("L1_ETMHF110_HTT60er", &L1_ETMHF110_HTT60er, &b_L1_ETMHF110_HTT60er);
   fChain->SetBranchAddress("L1_ETMHF120", &L1_ETMHF120, &b_L1_ETMHF120);
   fChain->SetBranchAddress("L1_ETMHF120_HTT60er", &L1_ETMHF120_HTT60er, &b_L1_ETMHF120_HTT60er);
   fChain->SetBranchAddress("L1_ETMHF130", &L1_ETMHF130, &b_L1_ETMHF130);
   fChain->SetBranchAddress("L1_ETMHF130_HTT60er", &L1_ETMHF130_HTT60er, &b_L1_ETMHF130_HTT60er);
   fChain->SetBranchAddress("L1_ETMHF140", &L1_ETMHF140, &b_L1_ETMHF140);
   fChain->SetBranchAddress("L1_ETMHF150", &L1_ETMHF150, &b_L1_ETMHF150);
   fChain->SetBranchAddress("L1_ETMHF70", &L1_ETMHF70, &b_L1_ETMHF70);
   fChain->SetBranchAddress("L1_ETMHF70_HTT60er", &L1_ETMHF70_HTT60er, &b_L1_ETMHF70_HTT60er);
   fChain->SetBranchAddress("L1_ETMHF80", &L1_ETMHF80, &b_L1_ETMHF80);
   fChain->SetBranchAddress("L1_ETMHF80_HTT60er", &L1_ETMHF80_HTT60er, &b_L1_ETMHF80_HTT60er);
   fChain->SetBranchAddress("L1_ETMHF80_SingleJet55er2p5_dPhi_Min2p1", &L1_ETMHF80_SingleJet55er2p5_dPhi_Min2p1, &b_L1_ETMHF80_SingleJet55er2p5_dPhi_Min2p1);
   fChain->SetBranchAddress("L1_ETMHF80_SingleJet55er2p5_dPhi_Min2p6", &L1_ETMHF80_SingleJet55er2p5_dPhi_Min2p6, &b_L1_ETMHF80_SingleJet55er2p5_dPhi_Min2p6);
   fChain->SetBranchAddress("L1_ETMHF90", &L1_ETMHF90, &b_L1_ETMHF90);
   fChain->SetBranchAddress("L1_ETMHF90_HTT60er", &L1_ETMHF90_HTT60er, &b_L1_ETMHF90_HTT60er);
   fChain->SetBranchAddress("L1_ETMHF90_SingleJet60er2p5_dPhi_Min2p1", &L1_ETMHF90_SingleJet60er2p5_dPhi_Min2p1, &b_L1_ETMHF90_SingleJet60er2p5_dPhi_Min2p1);
   fChain->SetBranchAddress("L1_ETMHF90_SingleJet60er2p5_dPhi_Min2p6", &L1_ETMHF90_SingleJet60er2p5_dPhi_Min2p6, &b_L1_ETMHF90_SingleJet60er2p5_dPhi_Min2p6);
   fChain->SetBranchAddress("L1_ETT2000", &L1_ETT2000, &b_L1_ETT2000);
   fChain->SetBranchAddress("L1_FirstBunchAfterTrain", &L1_FirstBunchAfterTrain, &b_L1_FirstBunchAfterTrain);
   fChain->SetBranchAddress("L1_FirstBunchBeforeTrain", &L1_FirstBunchBeforeTrain, &b_L1_FirstBunchBeforeTrain);
   fChain->SetBranchAddress("L1_FirstBunchInTrain", &L1_FirstBunchInTrain, &b_L1_FirstBunchInTrain);
   fChain->SetBranchAddress("L1_FirstCollisionInOrbit", &L1_FirstCollisionInOrbit, &b_L1_FirstCollisionInOrbit);
   fChain->SetBranchAddress("L1_FirstCollisionInTrain", &L1_FirstCollisionInTrain, &b_L1_FirstCollisionInTrain);
   fChain->SetBranchAddress("L1_HCAL_LaserMon_Trig", &L1_HCAL_LaserMon_Trig, &b_L1_HCAL_LaserMon_Trig);
   fChain->SetBranchAddress("L1_HCAL_LaserMon_Veto", &L1_HCAL_LaserMon_Veto, &b_L1_HCAL_LaserMon_Veto);
   fChain->SetBranchAddress("L1_HTMHF100", &L1_HTMHF100, &b_L1_HTMHF100);
   fChain->SetBranchAddress("L1_HTMHF120", &L1_HTMHF120, &b_L1_HTMHF120);
   fChain->SetBranchAddress("L1_HTMHF125", &L1_HTMHF125, &b_L1_HTMHF125);
   fChain->SetBranchAddress("L1_HTMHF130", &L1_HTMHF130, &b_L1_HTMHF130);
   fChain->SetBranchAddress("L1_HTMHF150", &L1_HTMHF150, &b_L1_HTMHF150);
   fChain->SetBranchAddress("L1_HTT120_SingleLLPJet40", &L1_HTT120_SingleLLPJet40, &b_L1_HTT120_SingleLLPJet40);
   fChain->SetBranchAddress("L1_HTT120er", &L1_HTT120er, &b_L1_HTT120er);
   fChain->SetBranchAddress("L1_HTT160_SingleLLPJet50", &L1_HTT160_SingleLLPJet50, &b_L1_HTT160_SingleLLPJet50);
   fChain->SetBranchAddress("L1_HTT160er", &L1_HTT160er, &b_L1_HTT160er);
   fChain->SetBranchAddress("L1_HTT200_SingleLLPJet60", &L1_HTT200_SingleLLPJet60, &b_L1_HTT200_SingleLLPJet60);
   fChain->SetBranchAddress("L1_HTT200er", &L1_HTT200er, &b_L1_HTT200er);
   fChain->SetBranchAddress("L1_HTT240_SingleLLPJet70", &L1_HTT240_SingleLLPJet70, &b_L1_HTT240_SingleLLPJet70);
   fChain->SetBranchAddress("L1_HTT255er", &L1_HTT255er, &b_L1_HTT255er);
   fChain->SetBranchAddress("L1_HTT280er", &L1_HTT280er, &b_L1_HTT280er);
   fChain->SetBranchAddress("L1_HTT280er_QuadJet_70_55_40_35_er2p5", &L1_HTT280er_QuadJet_70_55_40_35_er2p5, &b_L1_HTT280er_QuadJet_70_55_40_35_er2p5);
   fChain->SetBranchAddress("L1_HTT320er", &L1_HTT320er, &b_L1_HTT320er);
   fChain->SetBranchAddress("L1_HTT320er_QuadJet_70_55_40_40_er2p5", &L1_HTT320er_QuadJet_70_55_40_40_er2p5, &b_L1_HTT320er_QuadJet_70_55_40_40_er2p5);
   fChain->SetBranchAddress("L1_HTT320er_QuadJet_80_60_er2p1_45_40_er2p3", &L1_HTT320er_QuadJet_80_60_er2p1_45_40_er2p3, &b_L1_HTT320er_QuadJet_80_60_er2p1_45_40_er2p3);
   fChain->SetBranchAddress("L1_HTT320er_QuadJet_80_60_er2p1_50_45_er2p3", &L1_HTT320er_QuadJet_80_60_er2p1_50_45_er2p3, &b_L1_HTT320er_QuadJet_80_60_er2p1_50_45_er2p3);
   fChain->SetBranchAddress("L1_HTT360er", &L1_HTT360er, &b_L1_HTT360er);
   fChain->SetBranchAddress("L1_HTT400er", &L1_HTT400er, &b_L1_HTT400er);
   fChain->SetBranchAddress("L1_HTT450er", &L1_HTT450er, &b_L1_HTT450er);
   fChain->SetBranchAddress("L1_IsoEG32er2p5_Mt40", &L1_IsoEG32er2p5_Mt40, &b_L1_IsoEG32er2p5_Mt40);
   fChain->SetBranchAddress("L1_IsoTau52er2p1_QuadJet36er2p5", &L1_IsoTau52er2p1_QuadJet36er2p5, &b_L1_IsoTau52er2p1_QuadJet36er2p5);
   fChain->SetBranchAddress("L1_IsolatedBunch", &L1_IsolatedBunch, &b_L1_IsolatedBunch);
   fChain->SetBranchAddress("L1_LastBunchInTrain", &L1_LastBunchInTrain, &b_L1_LastBunchInTrain);
   fChain->SetBranchAddress("L1_LastCollisionInTrain", &L1_LastCollisionInTrain, &b_L1_LastCollisionInTrain);
   fChain->SetBranchAddress("L1_LooseIsoEG14er2p5_HTT200er", &L1_LooseIsoEG14er2p5_HTT200er, &b_L1_LooseIsoEG14er2p5_HTT200er);
   fChain->SetBranchAddress("L1_LooseIsoEG16er2p5_HTT200er", &L1_LooseIsoEG16er2p5_HTT200er, &b_L1_LooseIsoEG16er2p5_HTT200er);
   fChain->SetBranchAddress("L1_LooseIsoEG22er2p1_IsoTau26er2p1_dR_Min0p3", &L1_LooseIsoEG22er2p1_IsoTau26er2p1_dR_Min0p3, &b_L1_LooseIsoEG22er2p1_IsoTau26er2p1_dR_Min0p3);
   fChain->SetBranchAddress("L1_LooseIsoEG22er2p1_Tau70er2p1_dR_Min0p3", &L1_LooseIsoEG22er2p1_Tau70er2p1_dR_Min0p3, &b_L1_LooseIsoEG22er2p1_Tau70er2p1_dR_Min0p3);
   fChain->SetBranchAddress("L1_LooseIsoEG24er2p1_HTT100er", &L1_LooseIsoEG24er2p1_HTT100er, &b_L1_LooseIsoEG24er2p1_HTT100er);
   fChain->SetBranchAddress("L1_LooseIsoEG24er2p1_IsoTau27er2p1_dR_Min0p3", &L1_LooseIsoEG24er2p1_IsoTau27er2p1_dR_Min0p3, &b_L1_LooseIsoEG24er2p1_IsoTau27er2p1_dR_Min0p3);
   fChain->SetBranchAddress("L1_LooseIsoEG26er2p1_HTT100er", &L1_LooseIsoEG26er2p1_HTT100er, &b_L1_LooseIsoEG26er2p1_HTT100er);
   fChain->SetBranchAddress("L1_LooseIsoEG26er2p1_Jet34er2p5_dR_Min0p3", &L1_LooseIsoEG26er2p1_Jet34er2p5_dR_Min0p3, &b_L1_LooseIsoEG26er2p1_Jet34er2p5_dR_Min0p3);
   fChain->SetBranchAddress("L1_LooseIsoEG28er2p1_HTT100er", &L1_LooseIsoEG28er2p1_HTT100er, &b_L1_LooseIsoEG28er2p1_HTT100er);
   fChain->SetBranchAddress("L1_LooseIsoEG28er2p1_Jet34er2p5_dR_Min0p3", &L1_LooseIsoEG28er2p1_Jet34er2p5_dR_Min0p3, &b_L1_LooseIsoEG28er2p1_Jet34er2p5_dR_Min0p3);
   fChain->SetBranchAddress("L1_LooseIsoEG30er2p1_Jet34er2p5_dR_Min0p3", &L1_LooseIsoEG30er2p1_Jet34er2p5_dR_Min0p3, &b_L1_LooseIsoEG30er2p1_Jet34er2p5_dR_Min0p3);
   fChain->SetBranchAddress("L1_MinimumBiasHF0", &L1_MinimumBiasHF0, &b_L1_MinimumBiasHF0);
   fChain->SetBranchAddress("L1_MinimumBiasHF0_AND_BptxAND", &L1_MinimumBiasHF0_AND_BptxAND, &b_L1_MinimumBiasHF0_AND_BptxAND);
   fChain->SetBranchAddress("L1_Mu10er2p3_Jet32er2p3_dR_Max0p4_DoubleJet32er2p3_dEta_Max1p6", &L1_Mu10er2p3_Jet32er2p3_dR_Max0p4_DoubleJet32er2p3_dEta_Max1p6, &b_L1_Mu10er2p3_Jet32er2p3_dR_Max0p4_DoubleJet32er2p3_dEta_Max1p6);
   fChain->SetBranchAddress("L1_Mu12_HTT150er", &L1_Mu12_HTT150er, &b_L1_Mu12_HTT150er);
   fChain->SetBranchAddress("L1_Mu12er2p3_Jet40er2p3_dR_Max0p4_DoubleJet40er2p3_dEta_Max1p6", &L1_Mu12er2p3_Jet40er2p3_dR_Max0p4_DoubleJet40er2p3_dEta_Max1p6, &b_L1_Mu12er2p3_Jet40er2p3_dR_Max0p4_DoubleJet40er2p3_dEta_Max1p6);
   fChain->SetBranchAddress("L1_Mu14_HTT150er", &L1_Mu14_HTT150er, &b_L1_Mu14_HTT150er);
   fChain->SetBranchAddress("L1_Mu18er2p1_Tau24er2p1", &L1_Mu18er2p1_Tau24er2p1, &b_L1_Mu18er2p1_Tau24er2p1);
   fChain->SetBranchAddress("L1_Mu18er2p1_Tau26er2p1", &L1_Mu18er2p1_Tau26er2p1, &b_L1_Mu18er2p1_Tau26er2p1);
   fChain->SetBranchAddress("L1_Mu18er2p1_Tau26er2p1_Jet55", &L1_Mu18er2p1_Tau26er2p1_Jet55, &b_L1_Mu18er2p1_Tau26er2p1_Jet55);
   fChain->SetBranchAddress("L1_Mu18er2p1_Tau26er2p1_Jet70", &L1_Mu18er2p1_Tau26er2p1_Jet70, &b_L1_Mu18er2p1_Tau26er2p1_Jet70);
   fChain->SetBranchAddress("L1_Mu20_EG10er2p5", &L1_Mu20_EG10er2p5, &b_L1_Mu20_EG10er2p5);
   fChain->SetBranchAddress("L1_Mu22er2p1_IsoTau30er2p1", &L1_Mu22er2p1_IsoTau30er2p1, &b_L1_Mu22er2p1_IsoTau30er2p1);
   fChain->SetBranchAddress("L1_Mu22er2p1_IsoTau32er2p1", &L1_Mu22er2p1_IsoTau32er2p1, &b_L1_Mu22er2p1_IsoTau32er2p1);
   fChain->SetBranchAddress("L1_Mu22er2p1_IsoTau34er2p1", &L1_Mu22er2p1_IsoTau34er2p1, &b_L1_Mu22er2p1_IsoTau34er2p1);
   fChain->SetBranchAddress("L1_Mu22er2p1_IsoTau40er2p1", &L1_Mu22er2p1_IsoTau40er2p1, &b_L1_Mu22er2p1_IsoTau40er2p1);
   fChain->SetBranchAddress("L1_Mu22er2p1_Tau70er2p1", &L1_Mu22er2p1_Tau70er2p1, &b_L1_Mu22er2p1_Tau70er2p1);
   fChain->SetBranchAddress("L1_Mu3_Jet120er2p5_dR_Max0p4", &L1_Mu3_Jet120er2p5_dR_Max0p4, &b_L1_Mu3_Jet120er2p5_dR_Max0p4);
   fChain->SetBranchAddress("L1_Mu3_Jet16er2p5_dR_Max0p4", &L1_Mu3_Jet16er2p5_dR_Max0p4, &b_L1_Mu3_Jet16er2p5_dR_Max0p4);
   fChain->SetBranchAddress("L1_Mu3_Jet30er2p5", &L1_Mu3_Jet30er2p5, &b_L1_Mu3_Jet30er2p5);
   fChain->SetBranchAddress("L1_Mu3_Jet60er2p5_dR_Max0p4", &L1_Mu3_Jet60er2p5_dR_Max0p4, &b_L1_Mu3_Jet60er2p5_dR_Max0p4);
   fChain->SetBranchAddress("L1_Mu3er1p5_Jet100er2p5_ETMHF30", &L1_Mu3er1p5_Jet100er2p5_ETMHF30, &b_L1_Mu3er1p5_Jet100er2p5_ETMHF30);
   fChain->SetBranchAddress("L1_Mu3er1p5_Jet100er2p5_ETMHF40", &L1_Mu3er1p5_Jet100er2p5_ETMHF40, &b_L1_Mu3er1p5_Jet100er2p5_ETMHF40);
   fChain->SetBranchAddress("L1_Mu3er1p5_Jet100er2p5_ETMHF50", &L1_Mu3er1p5_Jet100er2p5_ETMHF50, &b_L1_Mu3er1p5_Jet100er2p5_ETMHF50);
   fChain->SetBranchAddress("L1_Mu5_EG23er2p5", &L1_Mu5_EG23er2p5, &b_L1_Mu5_EG23er2p5);
   fChain->SetBranchAddress("L1_Mu5_LooseIsoEG20er2p5", &L1_Mu5_LooseIsoEG20er2p5, &b_L1_Mu5_LooseIsoEG20er2p5);
   fChain->SetBranchAddress("L1_Mu6_DoubleEG10er2p5", &L1_Mu6_DoubleEG10er2p5, &b_L1_Mu6_DoubleEG10er2p5);
   fChain->SetBranchAddress("L1_Mu6_DoubleEG12er2p5", &L1_Mu6_DoubleEG12er2p5, &b_L1_Mu6_DoubleEG12er2p5);
   fChain->SetBranchAddress("L1_Mu6_DoubleEG15er2p5", &L1_Mu6_DoubleEG15er2p5, &b_L1_Mu6_DoubleEG15er2p5);
   fChain->SetBranchAddress("L1_Mu6_DoubleEG17er2p5", &L1_Mu6_DoubleEG17er2p5, &b_L1_Mu6_DoubleEG17er2p5);
   fChain->SetBranchAddress("L1_Mu6_HTT240er", &L1_Mu6_HTT240er, &b_L1_Mu6_HTT240er);
   fChain->SetBranchAddress("L1_Mu6_HTT250er", &L1_Mu6_HTT250er, &b_L1_Mu6_HTT250er);
   fChain->SetBranchAddress("L1_Mu7_EG20er2p5", &L1_Mu7_EG20er2p5, &b_L1_Mu7_EG20er2p5);
   fChain->SetBranchAddress("L1_Mu7_EG23er2p5", &L1_Mu7_EG23er2p5, &b_L1_Mu7_EG23er2p5);
   fChain->SetBranchAddress("L1_Mu7_LooseIsoEG20er2p5", &L1_Mu7_LooseIsoEG20er2p5, &b_L1_Mu7_LooseIsoEG20er2p5);
   fChain->SetBranchAddress("L1_Mu7_LooseIsoEG23er2p5", &L1_Mu7_LooseIsoEG23er2p5, &b_L1_Mu7_LooseIsoEG23er2p5);
   fChain->SetBranchAddress("L1_NotBptxOR", &L1_NotBptxOR, &b_L1_NotBptxOR);
   fChain->SetBranchAddress("L1_QuadJet60er2p5", &L1_QuadJet60er2p5, &b_L1_QuadJet60er2p5);
   fChain->SetBranchAddress("L1_QuadJet_95_75_65_20_DoubleJet_75_65_er2p5_Jet20_FWD3p0", &L1_QuadJet_95_75_65_20_DoubleJet_75_65_er2p5_Jet20_FWD3p0, &b_L1_QuadJet_95_75_65_20_DoubleJet_75_65_er2p5_Jet20_FWD3p0);
   fChain->SetBranchAddress("L1_QuadMu0", &L1_QuadMu0, &b_L1_QuadMu0);
   fChain->SetBranchAddress("L1_QuadMu0_OQ", &L1_QuadMu0_OQ, &b_L1_QuadMu0_OQ);
   fChain->SetBranchAddress("L1_QuadMu0_SQ", &L1_QuadMu0_SQ, &b_L1_QuadMu0_SQ);
   fChain->SetBranchAddress("L1_SecondBunchInTrain", &L1_SecondBunchInTrain, &b_L1_SecondBunchInTrain);
   fChain->SetBranchAddress("L1_SecondLastBunchInTrain", &L1_SecondLastBunchInTrain, &b_L1_SecondLastBunchInTrain);
   fChain->SetBranchAddress("L1_SingleEG10er2p5", &L1_SingleEG10er2p5, &b_L1_SingleEG10er2p5);
   fChain->SetBranchAddress("L1_SingleEG15er2p5", &L1_SingleEG15er2p5, &b_L1_SingleEG15er2p5);
   fChain->SetBranchAddress("L1_SingleEG26er2p5", &L1_SingleEG26er2p5, &b_L1_SingleEG26er2p5);
   fChain->SetBranchAddress("L1_SingleEG28_FWD2p5", &L1_SingleEG28_FWD2p5, &b_L1_SingleEG28_FWD2p5);
   fChain->SetBranchAddress("L1_SingleEG28er1p5", &L1_SingleEG28er1p5, &b_L1_SingleEG28er1p5);
   fChain->SetBranchAddress("L1_SingleEG28er2p1", &L1_SingleEG28er2p1, &b_L1_SingleEG28er2p1);
   fChain->SetBranchAddress("L1_SingleEG28er2p5", &L1_SingleEG28er2p5, &b_L1_SingleEG28er2p5);
   fChain->SetBranchAddress("L1_SingleEG34er2p5", &L1_SingleEG34er2p5, &b_L1_SingleEG34er2p5);
   fChain->SetBranchAddress("L1_SingleEG36er2p5", &L1_SingleEG36er2p5, &b_L1_SingleEG36er2p5);
   fChain->SetBranchAddress("L1_SingleEG38er2p5", &L1_SingleEG38er2p5, &b_L1_SingleEG38er2p5);
   fChain->SetBranchAddress("L1_SingleEG40er2p5", &L1_SingleEG40er2p5, &b_L1_SingleEG40er2p5);
   fChain->SetBranchAddress("L1_SingleEG42er2p5", &L1_SingleEG42er2p5, &b_L1_SingleEG42er2p5);
   fChain->SetBranchAddress("L1_SingleEG45er2p5", &L1_SingleEG45er2p5, &b_L1_SingleEG45er2p5);
   fChain->SetBranchAddress("L1_SingleEG50", &L1_SingleEG50, &b_L1_SingleEG50);
   fChain->SetBranchAddress("L1_SingleEG60", &L1_SingleEG60, &b_L1_SingleEG60);
   fChain->SetBranchAddress("L1_SingleEG8er2p5", &L1_SingleEG8er2p5, &b_L1_SingleEG8er2p5);
   fChain->SetBranchAddress("L1_SingleIsoEG24er2p1", &L1_SingleIsoEG24er2p1, &b_L1_SingleIsoEG24er2p1);
   fChain->SetBranchAddress("L1_SingleIsoEG26er2p1", &L1_SingleIsoEG26er2p1, &b_L1_SingleIsoEG26er2p1);
   fChain->SetBranchAddress("L1_SingleIsoEG26er2p5", &L1_SingleIsoEG26er2p5, &b_L1_SingleIsoEG26er2p5);
   fChain->SetBranchAddress("L1_SingleIsoEG28_FWD2p5", &L1_SingleIsoEG28_FWD2p5, &b_L1_SingleIsoEG28_FWD2p5);
   fChain->SetBranchAddress("L1_SingleIsoEG28er1p5", &L1_SingleIsoEG28er1p5, &b_L1_SingleIsoEG28er1p5);
   fChain->SetBranchAddress("L1_SingleIsoEG28er2p1", &L1_SingleIsoEG28er2p1, &b_L1_SingleIsoEG28er2p1);
   fChain->SetBranchAddress("L1_SingleIsoEG28er2p5", &L1_SingleIsoEG28er2p5, &b_L1_SingleIsoEG28er2p5);
   fChain->SetBranchAddress("L1_SingleIsoEG30er2p1", &L1_SingleIsoEG30er2p1, &b_L1_SingleIsoEG30er2p1);
   fChain->SetBranchAddress("L1_SingleIsoEG30er2p5", &L1_SingleIsoEG30er2p5, &b_L1_SingleIsoEG30er2p5);
   fChain->SetBranchAddress("L1_SingleIsoEG32er2p1", &L1_SingleIsoEG32er2p1, &b_L1_SingleIsoEG32er2p1);
   fChain->SetBranchAddress("L1_SingleIsoEG32er2p5", &L1_SingleIsoEG32er2p5, &b_L1_SingleIsoEG32er2p5);
   fChain->SetBranchAddress("L1_SingleIsoEG34er2p5", &L1_SingleIsoEG34er2p5, &b_L1_SingleIsoEG34er2p5);
   fChain->SetBranchAddress("L1_SingleJet10erHE", &L1_SingleJet10erHE, &b_L1_SingleJet10erHE);
   fChain->SetBranchAddress("L1_SingleJet120", &L1_SingleJet120, &b_L1_SingleJet120);
   fChain->SetBranchAddress("L1_SingleJet120_FWD2p5", &L1_SingleJet120_FWD2p5, &b_L1_SingleJet120_FWD2p5);
   fChain->SetBranchAddress("L1_SingleJet120_FWD3p0", &L1_SingleJet120_FWD3p0, &b_L1_SingleJet120_FWD3p0);
   fChain->SetBranchAddress("L1_SingleJet120er1p3", &L1_SingleJet120er1p3, &b_L1_SingleJet120er1p3);
   fChain->SetBranchAddress("L1_SingleJet120er2p5", &L1_SingleJet120er2p5, &b_L1_SingleJet120er2p5);
   fChain->SetBranchAddress("L1_SingleJet12erHE", &L1_SingleJet12erHE, &b_L1_SingleJet12erHE);
   fChain->SetBranchAddress("L1_SingleJet140er2p5", &L1_SingleJet140er2p5, &b_L1_SingleJet140er2p5);
   fChain->SetBranchAddress("L1_SingleJet160er2p5", &L1_SingleJet160er2p5, &b_L1_SingleJet160er2p5);
   fChain->SetBranchAddress("L1_SingleJet180", &L1_SingleJet180, &b_L1_SingleJet180);
   fChain->SetBranchAddress("L1_SingleJet180er2p5", &L1_SingleJet180er2p5, &b_L1_SingleJet180er2p5);
   fChain->SetBranchAddress("L1_SingleJet200", &L1_SingleJet200, &b_L1_SingleJet200);
   fChain->SetBranchAddress("L1_SingleJet20er2p5_NotBptxOR", &L1_SingleJet20er2p5_NotBptxOR, &b_L1_SingleJet20er2p5_NotBptxOR);
   fChain->SetBranchAddress("L1_SingleJet20er2p5_NotBptxOR_3BX", &L1_SingleJet20er2p5_NotBptxOR_3BX, &b_L1_SingleJet20er2p5_NotBptxOR_3BX);
   fChain->SetBranchAddress("L1_SingleJet35", &L1_SingleJet35, &b_L1_SingleJet35);
   fChain->SetBranchAddress("L1_SingleJet35_FWD2p5", &L1_SingleJet35_FWD2p5, &b_L1_SingleJet35_FWD2p5);
   fChain->SetBranchAddress("L1_SingleJet35_FWD3p0", &L1_SingleJet35_FWD3p0, &b_L1_SingleJet35_FWD3p0);
   fChain->SetBranchAddress("L1_SingleJet35er1p3", &L1_SingleJet35er1p3, &b_L1_SingleJet35er1p3);
   fChain->SetBranchAddress("L1_SingleJet35er2p5", &L1_SingleJet35er2p5, &b_L1_SingleJet35er2p5);
   fChain->SetBranchAddress("L1_SingleJet43er2p5_NotBptxOR_3BX", &L1_SingleJet43er2p5_NotBptxOR_3BX, &b_L1_SingleJet43er2p5_NotBptxOR_3BX);
   fChain->SetBranchAddress("L1_SingleJet46er2p5_NotBptxOR_3BX", &L1_SingleJet46er2p5_NotBptxOR_3BX, &b_L1_SingleJet46er2p5_NotBptxOR_3BX);
   fChain->SetBranchAddress("L1_SingleJet60", &L1_SingleJet60, &b_L1_SingleJet60);
   fChain->SetBranchAddress("L1_SingleJet60_FWD2p5", &L1_SingleJet60_FWD2p5, &b_L1_SingleJet60_FWD2p5);
   fChain->SetBranchAddress("L1_SingleJet8erHE", &L1_SingleJet8erHE, &b_L1_SingleJet8erHE);
   fChain->SetBranchAddress("L1_SingleJet90", &L1_SingleJet90, &b_L1_SingleJet90);
   fChain->SetBranchAddress("L1_SingleJet90_FWD2p5", &L1_SingleJet90_FWD2p5, &b_L1_SingleJet90_FWD2p5);
   fChain->SetBranchAddress("L1_SingleLooseIsoEG26er1p5", &L1_SingleLooseIsoEG26er1p5, &b_L1_SingleLooseIsoEG26er1p5);
   fChain->SetBranchAddress("L1_SingleLooseIsoEG26er2p5", &L1_SingleLooseIsoEG26er2p5, &b_L1_SingleLooseIsoEG26er2p5);
   fChain->SetBranchAddress("L1_SingleLooseIsoEG28_FWD2p5", &L1_SingleLooseIsoEG28_FWD2p5, &b_L1_SingleLooseIsoEG28_FWD2p5);
   fChain->SetBranchAddress("L1_SingleLooseIsoEG28er1p5", &L1_SingleLooseIsoEG28er1p5, &b_L1_SingleLooseIsoEG28er1p5);
   fChain->SetBranchAddress("L1_SingleLooseIsoEG28er2p1", &L1_SingleLooseIsoEG28er2p1, &b_L1_SingleLooseIsoEG28er2p1);
   fChain->SetBranchAddress("L1_SingleLooseIsoEG28er2p5", &L1_SingleLooseIsoEG28er2p5, &b_L1_SingleLooseIsoEG28er2p5);
   fChain->SetBranchAddress("L1_SingleLooseIsoEG30er1p5", &L1_SingleLooseIsoEG30er1p5, &b_L1_SingleLooseIsoEG30er1p5);
   fChain->SetBranchAddress("L1_SingleLooseIsoEG30er2p5", &L1_SingleLooseIsoEG30er2p5, &b_L1_SingleLooseIsoEG30er2p5);
   fChain->SetBranchAddress("L1_SingleMu0_BMTF", &L1_SingleMu0_BMTF, &b_L1_SingleMu0_BMTF);
   fChain->SetBranchAddress("L1_SingleMu0_DQ", &L1_SingleMu0_DQ, &b_L1_SingleMu0_DQ);
   fChain->SetBranchAddress("L1_SingleMu0_EMTF", &L1_SingleMu0_EMTF, &b_L1_SingleMu0_EMTF);
   fChain->SetBranchAddress("L1_SingleMu0_OMTF", &L1_SingleMu0_OMTF, &b_L1_SingleMu0_OMTF);
   fChain->SetBranchAddress("L1_SingleMu0_SQ13_BMTF", &L1_SingleMu0_SQ13_BMTF, &b_L1_SingleMu0_SQ13_BMTF);
   fChain->SetBranchAddress("L1_SingleMu0_SQ14_BMTF", &L1_SingleMu0_SQ14_BMTF, &b_L1_SingleMu0_SQ14_BMTF);
   fChain->SetBranchAddress("L1_SingleMu0_SQ15_BMTF", &L1_SingleMu0_SQ15_BMTF, &b_L1_SingleMu0_SQ15_BMTF);
   fChain->SetBranchAddress("L1_SingleMu0_Upt10", &L1_SingleMu0_Upt10, &b_L1_SingleMu0_Upt10);
   fChain->SetBranchAddress("L1_SingleMu0_Upt10_BMTF", &L1_SingleMu0_Upt10_BMTF, &b_L1_SingleMu0_Upt10_BMTF);
   fChain->SetBranchAddress("L1_SingleMu0_Upt10_EMTF", &L1_SingleMu0_Upt10_EMTF, &b_L1_SingleMu0_Upt10_EMTF);
   fChain->SetBranchAddress("L1_SingleMu0_Upt10_OMTF", &L1_SingleMu0_Upt10_OMTF, &b_L1_SingleMu0_Upt10_OMTF);
   fChain->SetBranchAddress("L1_SingleMu0_Upt10_SQ14_BMTF", &L1_SingleMu0_Upt10_SQ14_BMTF, &b_L1_SingleMu0_Upt10_SQ14_BMTF);
   fChain->SetBranchAddress("L1_SingleMu0_Upt15_SQ14_BMTF", &L1_SingleMu0_Upt15_SQ14_BMTF, &b_L1_SingleMu0_Upt15_SQ14_BMTF);
   fChain->SetBranchAddress("L1_SingleMu0_Upt20_SQ14_BMTF", &L1_SingleMu0_Upt20_SQ14_BMTF, &b_L1_SingleMu0_Upt20_SQ14_BMTF);
   fChain->SetBranchAddress("L1_SingleMu0_Upt25_SQ14_BMTF", &L1_SingleMu0_Upt25_SQ14_BMTF, &b_L1_SingleMu0_Upt25_SQ14_BMTF);
   fChain->SetBranchAddress("L1_SingleMu10_SQ14_BMTF", &L1_SingleMu10_SQ14_BMTF, &b_L1_SingleMu10_SQ14_BMTF);
   fChain->SetBranchAddress("L1_SingleMu11_SQ14_BMTF", &L1_SingleMu11_SQ14_BMTF, &b_L1_SingleMu11_SQ14_BMTF);
   fChain->SetBranchAddress("L1_SingleMu12_DQ_BMTF", &L1_SingleMu12_DQ_BMTF, &b_L1_SingleMu12_DQ_BMTF);
   fChain->SetBranchAddress("L1_SingleMu12_DQ_EMTF", &L1_SingleMu12_DQ_EMTF, &b_L1_SingleMu12_DQ_EMTF);
   fChain->SetBranchAddress("L1_SingleMu12_DQ_OMTF", &L1_SingleMu12_DQ_OMTF, &b_L1_SingleMu12_DQ_OMTF);
   fChain->SetBranchAddress("L1_SingleMu15_DQ", &L1_SingleMu15_DQ, &b_L1_SingleMu15_DQ);
   fChain->SetBranchAddress("L1_SingleMu18", &L1_SingleMu18, &b_L1_SingleMu18);
   fChain->SetBranchAddress("L1_SingleMu20", &L1_SingleMu20, &b_L1_SingleMu20);
   fChain->SetBranchAddress("L1_SingleMu22", &L1_SingleMu22, &b_L1_SingleMu22);
   fChain->SetBranchAddress("L1_SingleMu22_BMTF", &L1_SingleMu22_BMTF, &b_L1_SingleMu22_BMTF);
   fChain->SetBranchAddress("L1_SingleMu22_BMTF_NEG", &L1_SingleMu22_BMTF_NEG, &b_L1_SingleMu22_BMTF_NEG);
   fChain->SetBranchAddress("L1_SingleMu22_BMTF_POS", &L1_SingleMu22_BMTF_POS, &b_L1_SingleMu22_BMTF_POS);
   fChain->SetBranchAddress("L1_SingleMu22_DQ", &L1_SingleMu22_DQ, &b_L1_SingleMu22_DQ);
   fChain->SetBranchAddress("L1_SingleMu22_EMTF", &L1_SingleMu22_EMTF, &b_L1_SingleMu22_EMTF);
   fChain->SetBranchAddress("L1_SingleMu22_EMTF_NEG", &L1_SingleMu22_EMTF_NEG, &b_L1_SingleMu22_EMTF_NEG);
   fChain->SetBranchAddress("L1_SingleMu22_EMTF_POS", &L1_SingleMu22_EMTF_POS, &b_L1_SingleMu22_EMTF_POS);
   fChain->SetBranchAddress("L1_SingleMu22_OMTF", &L1_SingleMu22_OMTF, &b_L1_SingleMu22_OMTF);
   fChain->SetBranchAddress("L1_SingleMu22_OMTF_NEG", &L1_SingleMu22_OMTF_NEG, &b_L1_SingleMu22_OMTF_NEG);
   fChain->SetBranchAddress("L1_SingleMu22_OMTF_POS", &L1_SingleMu22_OMTF_POS, &b_L1_SingleMu22_OMTF_POS);
   fChain->SetBranchAddress("L1_SingleMu22_OQ", &L1_SingleMu22_OQ, &b_L1_SingleMu22_OQ);
   fChain->SetBranchAddress("L1_SingleMu25", &L1_SingleMu25, &b_L1_SingleMu25);
   fChain->SetBranchAddress("L1_SingleMu3", &L1_SingleMu3, &b_L1_SingleMu3);
   fChain->SetBranchAddress("L1_SingleMu5", &L1_SingleMu5, &b_L1_SingleMu5);
   fChain->SetBranchAddress("L1_SingleMu5_SQ14_BMTF", &L1_SingleMu5_SQ14_BMTF, &b_L1_SingleMu5_SQ14_BMTF);
   fChain->SetBranchAddress("L1_SingleMu6_SQ14_BMTF", &L1_SingleMu6_SQ14_BMTF, &b_L1_SingleMu6_SQ14_BMTF);
   fChain->SetBranchAddress("L1_SingleMu7", &L1_SingleMu7, &b_L1_SingleMu7);
   fChain->SetBranchAddress("L1_SingleMu7_DQ", &L1_SingleMu7_DQ, &b_L1_SingleMu7_DQ);
   fChain->SetBranchAddress("L1_SingleMu7_SQ14_BMTF", &L1_SingleMu7_SQ14_BMTF, &b_L1_SingleMu7_SQ14_BMTF);
   fChain->SetBranchAddress("L1_SingleMu8_SQ14_BMTF", &L1_SingleMu8_SQ14_BMTF, &b_L1_SingleMu8_SQ14_BMTF);
   fChain->SetBranchAddress("L1_SingleMu9_SQ14_BMTF", &L1_SingleMu9_SQ14_BMTF, &b_L1_SingleMu9_SQ14_BMTF);
   fChain->SetBranchAddress("L1_SingleMuCosmics", &L1_SingleMuCosmics, &b_L1_SingleMuCosmics);
   fChain->SetBranchAddress("L1_SingleMuCosmics_BMTF", &L1_SingleMuCosmics_BMTF, &b_L1_SingleMuCosmics_BMTF);
   fChain->SetBranchAddress("L1_SingleMuCosmics_EMTF", &L1_SingleMuCosmics_EMTF, &b_L1_SingleMuCosmics_EMTF);
   fChain->SetBranchAddress("L1_SingleMuCosmics_OMTF", &L1_SingleMuCosmics_OMTF, &b_L1_SingleMuCosmics_OMTF);
   fChain->SetBranchAddress("L1_SingleMuOpen", &L1_SingleMuOpen, &b_L1_SingleMuOpen);
   fChain->SetBranchAddress("L1_SingleMuOpen_BMTF", &L1_SingleMuOpen_BMTF, &b_L1_SingleMuOpen_BMTF);
   fChain->SetBranchAddress("L1_SingleMuOpen_EMTF", &L1_SingleMuOpen_EMTF, &b_L1_SingleMuOpen_EMTF);
   fChain->SetBranchAddress("L1_SingleMuOpen_NotBptxOR", &L1_SingleMuOpen_NotBptxOR, &b_L1_SingleMuOpen_NotBptxOR);
   fChain->SetBranchAddress("L1_SingleMuOpen_OMTF", &L1_SingleMuOpen_OMTF, &b_L1_SingleMuOpen_OMTF);
   fChain->SetBranchAddress("L1_SingleMuOpen_er1p1_NotBptxOR_3BX", &L1_SingleMuOpen_er1p1_NotBptxOR_3BX, &b_L1_SingleMuOpen_er1p1_NotBptxOR_3BX);
   fChain->SetBranchAddress("L1_SingleMuOpen_er1p4_NotBptxOR_3BX", &L1_SingleMuOpen_er1p4_NotBptxOR_3BX, &b_L1_SingleMuOpen_er1p4_NotBptxOR_3BX);
   fChain->SetBranchAddress("L1_SingleMuShower_Nominal", &L1_SingleMuShower_Nominal, &b_L1_SingleMuShower_Nominal);
   fChain->SetBranchAddress("L1_SingleMuShower_Tight", &L1_SingleMuShower_Tight, &b_L1_SingleMuShower_Tight);
   fChain->SetBranchAddress("L1_SingleTau120er2p1", &L1_SingleTau120er2p1, &b_L1_SingleTau120er2p1);
   fChain->SetBranchAddress("L1_SingleTau130er2p1", &L1_SingleTau130er2p1, &b_L1_SingleTau130er2p1);
   fChain->SetBranchAddress("L1_TOTEM_1", &L1_TOTEM_1, &b_L1_TOTEM_1);
   fChain->SetBranchAddress("L1_TOTEM_2", &L1_TOTEM_2, &b_L1_TOTEM_2);
   fChain->SetBranchAddress("L1_TOTEM_3", &L1_TOTEM_3, &b_L1_TOTEM_3);
   fChain->SetBranchAddress("L1_TOTEM_4", &L1_TOTEM_4, &b_L1_TOTEM_4);
   fChain->SetBranchAddress("L1_TripleEG_18_17_8_er2p5", &L1_TripleEG_18_17_8_er2p5, &b_L1_TripleEG_18_17_8_er2p5);
   fChain->SetBranchAddress("L1_TripleEG_18_18_12_er2p5", &L1_TripleEG_18_18_12_er2p5, &b_L1_TripleEG_18_18_12_er2p5);
   fChain->SetBranchAddress("L1_TripleJet_100_80_70_DoubleJet_80_70_er2p5", &L1_TripleJet_100_80_70_DoubleJet_80_70_er2p5, &b_L1_TripleJet_100_80_70_DoubleJet_80_70_er2p5);
   fChain->SetBranchAddress("L1_TripleJet_105_85_75_DoubleJet_85_75_er2p5", &L1_TripleJet_105_85_75_DoubleJet_85_75_er2p5, &b_L1_TripleJet_105_85_75_DoubleJet_85_75_er2p5);
   fChain->SetBranchAddress("L1_TripleJet_95_75_65_DoubleJet_75_65_er2p5", &L1_TripleJet_95_75_65_DoubleJet_75_65_er2p5, &b_L1_TripleJet_95_75_65_DoubleJet_75_65_er2p5);
   fChain->SetBranchAddress("L1_TripleMu0", &L1_TripleMu0, &b_L1_TripleMu0);
   fChain->SetBranchAddress("L1_TripleMu0_OQ", &L1_TripleMu0_OQ, &b_L1_TripleMu0_OQ);
   fChain->SetBranchAddress("L1_TripleMu0_SQ", &L1_TripleMu0_SQ, &b_L1_TripleMu0_SQ);
   fChain->SetBranchAddress("L1_TripleMu3", &L1_TripleMu3, &b_L1_TripleMu3);
   fChain->SetBranchAddress("L1_TripleMu3_SQ", &L1_TripleMu3_SQ, &b_L1_TripleMu3_SQ);
   fChain->SetBranchAddress("L1_TripleMu_3SQ_2p5SQ_0", &L1_TripleMu_3SQ_2p5SQ_0, &b_L1_TripleMu_3SQ_2p5SQ_0);
   fChain->SetBranchAddress("L1_TripleMu_3SQ_2p5SQ_0_Mass_Max12", &L1_TripleMu_3SQ_2p5SQ_0_Mass_Max12, &b_L1_TripleMu_3SQ_2p5SQ_0_Mass_Max12);
   fChain->SetBranchAddress("L1_TripleMu_3SQ_2p5SQ_0_OS_Mass_Max12", &L1_TripleMu_3SQ_2p5SQ_0_OS_Mass_Max12, &b_L1_TripleMu_3SQ_2p5SQ_0_OS_Mass_Max12);
   fChain->SetBranchAddress("L1_TripleMu_4SQ_2p5SQ_0_OS_Mass_Max12", &L1_TripleMu_4SQ_2p5SQ_0_OS_Mass_Max12, &b_L1_TripleMu_4SQ_2p5SQ_0_OS_Mass_Max12);
   fChain->SetBranchAddress("L1_TripleMu_5SQ_3SQ_0OQ", &L1_TripleMu_5SQ_3SQ_0OQ, &b_L1_TripleMu_5SQ_3SQ_0OQ);
   fChain->SetBranchAddress("L1_TripleMu_5SQ_3SQ_0OQ_DoubleMu_5_3_SQ_OS_Mass_Max9", &L1_TripleMu_5SQ_3SQ_0OQ_DoubleMu_5_3_SQ_OS_Mass_Max9, &b_L1_TripleMu_5SQ_3SQ_0OQ_DoubleMu_5_3_SQ_OS_Mass_Max9);
   fChain->SetBranchAddress("L1_TripleMu_5SQ_3SQ_0_DoubleMu_5_3_SQ_OS_Mass_Max9", &L1_TripleMu_5SQ_3SQ_0_DoubleMu_5_3_SQ_OS_Mass_Max9, &b_L1_TripleMu_5SQ_3SQ_0_DoubleMu_5_3_SQ_OS_Mass_Max9);
   fChain->SetBranchAddress("L1_TripleMu_5_3_3", &L1_TripleMu_5_3_3, &b_L1_TripleMu_5_3_3);
   fChain->SetBranchAddress("L1_TripleMu_5_3_3_SQ", &L1_TripleMu_5_3_3_SQ, &b_L1_TripleMu_5_3_3_SQ);
   fChain->SetBranchAddress("L1_TripleMu_5_3p5_2p5", &L1_TripleMu_5_3p5_2p5, &b_L1_TripleMu_5_3p5_2p5);
   fChain->SetBranchAddress("L1_TripleMu_5_3p5_2p5_DoubleMu_5_2p5_OS_Mass_5to17", &L1_TripleMu_5_3p5_2p5_DoubleMu_5_2p5_OS_Mass_5to17, &b_L1_TripleMu_5_3p5_2p5_DoubleMu_5_2p5_OS_Mass_5to17);
   fChain->SetBranchAddress("L1_TripleMu_5_4_2p5_DoubleMu_5_2p5_OS_Mass_5to17", &L1_TripleMu_5_4_2p5_DoubleMu_5_2p5_OS_Mass_5to17, &b_L1_TripleMu_5_4_2p5_DoubleMu_5_2p5_OS_Mass_5to17);
   fChain->SetBranchAddress("L1_TripleMu_5_5_3", &L1_TripleMu_5_5_3, &b_L1_TripleMu_5_5_3);
   fChain->SetBranchAddress("L1_TwoMuShower_Loose", &L1_TwoMuShower_Loose, &b_L1_TwoMuShower_Loose);
   fChain->SetBranchAddress("L1_UnpairedBunchBptxMinus", &L1_UnpairedBunchBptxMinus, &b_L1_UnpairedBunchBptxMinus);
   fChain->SetBranchAddress("L1_UnpairedBunchBptxPlus", &L1_UnpairedBunchBptxPlus, &b_L1_UnpairedBunchBptxPlus);
   fChain->SetBranchAddress("L1_ZeroBias", &L1_ZeroBias, &b_L1_ZeroBias);
   fChain->SetBranchAddress("L1_ZeroBias_copy", &L1_ZeroBias_copy, &b_L1_ZeroBias_copy);
   fChain->SetBranchAddress("L1_UnprefireableEvent_TriggerRules", &L1_UnprefireableEvent_TriggerRules, &b_L1_UnprefireableEvent_TriggerRules);
   fChain->SetBranchAddress("L1_UnprefireableEvent_FirstBxInTrain", &L1_UnprefireableEvent_FirstBxInTrain, &b_L1_UnprefireableEvent_FirstBxInTrain);
   fChain->SetBranchAddress("L1_FinalOR_BXmin1", &L1_FinalOR_BXmin1, &b_L1_FinalOR_BXmin1);
   fChain->SetBranchAddress("L1_FinalOR_BXmin2", &L1_FinalOR_BXmin2, &b_L1_FinalOR_BXmin2);
   fChain->SetBranchAddress("HLTriggerFirstPath", &HLTriggerFirstPath, &b_HLTriggerFirstPath);
   fChain->SetBranchAddress("DST_PFScouting_DoubleMuon", &DST_PFScouting_DoubleMuon, &b_DST_PFScouting_DoubleMuon);
   fChain->SetBranchAddress("DST_PFScouting_DoubleEG", &DST_PFScouting_DoubleEG, &b_DST_PFScouting_DoubleEG);
   fChain->SetBranchAddress("DST_PFScouting_JetHT", &DST_PFScouting_JetHT, &b_DST_PFScouting_JetHT);
   fChain->SetBranchAddress("DST_PFScouting_DatasetMuon", &DST_PFScouting_DatasetMuon, &b_DST_PFScouting_DatasetMuon);
   fChain->SetBranchAddress("DST_PFScouting_AXOVLoose", &DST_PFScouting_AXOVLoose, &b_DST_PFScouting_AXOVLoose);
   fChain->SetBranchAddress("DST_PFScouting_AXOLoose", &DST_PFScouting_AXOLoose, &b_DST_PFScouting_AXOLoose);
   fChain->SetBranchAddress("DST_PFScouting_AXONominal", &DST_PFScouting_AXONominal, &b_DST_PFScouting_AXONominal);
   fChain->SetBranchAddress("DST_PFScouting_AXOTight", &DST_PFScouting_AXOTight, &b_DST_PFScouting_AXOTight);
   fChain->SetBranchAddress("DST_PFScouting_AXOVTight", &DST_PFScouting_AXOVTight, &b_DST_PFScouting_AXOVTight);
   fChain->SetBranchAddress("DST_PFScouting_CICADAVLoose", &DST_PFScouting_CICADAVLoose, &b_DST_PFScouting_CICADAVLoose);
   fChain->SetBranchAddress("DST_PFScouting_CICADALoose", &DST_PFScouting_CICADALoose, &b_DST_PFScouting_CICADALoose);
   fChain->SetBranchAddress("DST_PFScouting_CICADAMedium", &DST_PFScouting_CICADAMedium, &b_DST_PFScouting_CICADAMedium);
   fChain->SetBranchAddress("DST_PFScouting_CICADATight", &DST_PFScouting_CICADATight, &b_DST_PFScouting_CICADATight);
   fChain->SetBranchAddress("DST_PFScouting_CICADAVTight", &DST_PFScouting_CICADAVTight, &b_DST_PFScouting_CICADAVTight);
   fChain->SetBranchAddress("DST_PFScouting_SingleMuon", &DST_PFScouting_SingleMuon, &b_DST_PFScouting_SingleMuon);
   fChain->SetBranchAddress("DST_PFScouting_SinglePhotonEB", &DST_PFScouting_SinglePhotonEB, &b_DST_PFScouting_SinglePhotonEB);
   fChain->SetBranchAddress("DST_PFScouting_ZeroBias", &DST_PFScouting_ZeroBias, &b_DST_PFScouting_ZeroBias);
   fChain->SetBranchAddress("HLT_EphemeralPhysics", &HLT_EphemeralPhysics, &b_HLT_EphemeralPhysics);
   fChain->SetBranchAddress("HLT_EphemeralZeroBias", &HLT_EphemeralZeroBias, &b_HLT_EphemeralZeroBias);
   fChain->SetBranchAddress("HLT_EcalCalibration", &HLT_EcalCalibration, &b_HLT_EcalCalibration);
   fChain->SetBranchAddress("HLT_HcalCalibration", &HLT_HcalCalibration, &b_HLT_HcalCalibration);
   fChain->SetBranchAddress("HLT_HcalNZS", &HLT_HcalNZS, &b_HLT_HcalNZS);
   fChain->SetBranchAddress("HLT_HcalPhiSym", &HLT_HcalPhiSym, &b_HLT_HcalPhiSym);
   fChain->SetBranchAddress("HLT_Random", &HLT_Random, &b_HLT_Random);
   fChain->SetBranchAddress("HLT_Physics", &HLT_Physics, &b_HLT_Physics);
   fChain->SetBranchAddress("HLT_ZeroBias", &HLT_ZeroBias, &b_HLT_ZeroBias);
   fChain->SetBranchAddress("HLT_ZeroBias_Alignment", &HLT_ZeroBias_Alignment, &b_HLT_ZeroBias_Alignment);
   fChain->SetBranchAddress("HLT_ZeroBias_Beamspot", &HLT_ZeroBias_Beamspot, &b_HLT_ZeroBias_Beamspot);
   fChain->SetBranchAddress("HLT_ZeroBias_IsolatedBunches", &HLT_ZeroBias_IsolatedBunches, &b_HLT_ZeroBias_IsolatedBunches);
   fChain->SetBranchAddress("HLT_ZeroBias_FirstBXAfterTrain", &HLT_ZeroBias_FirstBXAfterTrain, &b_HLT_ZeroBias_FirstBXAfterTrain);
   fChain->SetBranchAddress("HLT_ZeroBias_FirstCollisionAfterAbortGap", &HLT_ZeroBias_FirstCollisionAfterAbortGap, &b_HLT_ZeroBias_FirstCollisionAfterAbortGap);
   fChain->SetBranchAddress("HLT_ZeroBias_FirstCollisionInTrain", &HLT_ZeroBias_FirstCollisionInTrain, &b_HLT_ZeroBias_FirstCollisionInTrain);
   fChain->SetBranchAddress("HLT_ZeroBias_LastCollisionInTrain", &HLT_ZeroBias_LastCollisionInTrain, &b_HLT_ZeroBias_LastCollisionInTrain);
   fChain->SetBranchAddress("HLT_HT300_Beamspot", &HLT_HT300_Beamspot, &b_HLT_HT300_Beamspot);
   fChain->SetBranchAddress("HLT_IsoTrackHB", &HLT_IsoTrackHB, &b_HLT_IsoTrackHB);
   fChain->SetBranchAddress("HLT_IsoTrackHE", &HLT_IsoTrackHE, &b_HLT_IsoTrackHE);
   fChain->SetBranchAddress("HLT_PFJet40_GPUvsCPU", &HLT_PFJet40_GPUvsCPU, &b_HLT_PFJet40_GPUvsCPU);
   fChain->SetBranchAddress("HLT_AK8PFJet380_SoftDropMass30", &HLT_AK8PFJet380_SoftDropMass30, &b_HLT_AK8PFJet380_SoftDropMass30);
   fChain->SetBranchAddress("HLT_AK8PFJet400_SoftDropMass30", &HLT_AK8PFJet400_SoftDropMass30, &b_HLT_AK8PFJet400_SoftDropMass30);
   fChain->SetBranchAddress("HLT_AK8PFJet425_SoftDropMass30", &HLT_AK8PFJet425_SoftDropMass30, &b_HLT_AK8PFJet425_SoftDropMass30);
   fChain->SetBranchAddress("HLT_AK8PFJet450_SoftDropMass30", &HLT_AK8PFJet450_SoftDropMass30, &b_HLT_AK8PFJet450_SoftDropMass30);
   fChain->SetBranchAddress("HLT_AK8DiPFJet250_250_SoftDropMass40", &HLT_AK8DiPFJet250_250_SoftDropMass40, &b_HLT_AK8DiPFJet250_250_SoftDropMass40);
   fChain->SetBranchAddress("HLT_AK8DiPFJet250_250_SoftDropMass50", &HLT_AK8DiPFJet250_250_SoftDropMass50, &b_HLT_AK8DiPFJet250_250_SoftDropMass50);
   fChain->SetBranchAddress("HLT_AK8DiPFJet260_260_SoftDropMass30", &HLT_AK8DiPFJet260_260_SoftDropMass30, &b_HLT_AK8DiPFJet260_260_SoftDropMass30);
   fChain->SetBranchAddress("HLT_AK8DiPFJet260_260_SoftDropMass40", &HLT_AK8DiPFJet260_260_SoftDropMass40, &b_HLT_AK8DiPFJet260_260_SoftDropMass40);
   fChain->SetBranchAddress("HLT_AK8DiPFJet270_270_SoftDropMass30", &HLT_AK8DiPFJet270_270_SoftDropMass30, &b_HLT_AK8DiPFJet270_270_SoftDropMass30);
   fChain->SetBranchAddress("HLT_AK8DiPFJet280_280_SoftDropMass30", &HLT_AK8DiPFJet280_280_SoftDropMass30, &b_HLT_AK8DiPFJet280_280_SoftDropMass30);
   fChain->SetBranchAddress("HLT_AK8DiPFJet290_290_SoftDropMass30", &HLT_AK8DiPFJet290_290_SoftDropMass30, &b_HLT_AK8DiPFJet290_290_SoftDropMass30);
   fChain->SetBranchAddress("HLT_CaloJet500_NoJetID", &HLT_CaloJet500_NoJetID, &b_HLT_CaloJet500_NoJetID);
   fChain->SetBranchAddress("HLT_CaloJet550_NoJetID", &HLT_CaloJet550_NoJetID, &b_HLT_CaloJet550_NoJetID);
   fChain->SetBranchAddress("HLT_DoubleMu5_Upsilon_DoubleEle3_CaloIdL_TrackIdL", &HLT_DoubleMu5_Upsilon_DoubleEle3_CaloIdL_TrackIdL, &b_HLT_DoubleMu5_Upsilon_DoubleEle3_CaloIdL_TrackIdL);
   fChain->SetBranchAddress("HLT_DoubleMu3_DoubleEle7p5_CaloIdL_TrackIdL_Upsilon", &HLT_DoubleMu3_DoubleEle7p5_CaloIdL_TrackIdL_Upsilon, &b_HLT_DoubleMu3_DoubleEle7p5_CaloIdL_TrackIdL_Upsilon);
   fChain->SetBranchAddress("HLT_Trimuon5_3p5_2_Upsilon_Muon", &HLT_Trimuon5_3p5_2_Upsilon_Muon, &b_HLT_Trimuon5_3p5_2_Upsilon_Muon);
   fChain->SetBranchAddress("HLT_TrimuonOpen_5_3p5_2_Upsilon_Muon", &HLT_TrimuonOpen_5_3p5_2_Upsilon_Muon, &b_HLT_TrimuonOpen_5_3p5_2_Upsilon_Muon);
   fChain->SetBranchAddress("HLT_DoubleEle25_CaloIdL_MW", &HLT_DoubleEle25_CaloIdL_MW, &b_HLT_DoubleEle25_CaloIdL_MW);
   fChain->SetBranchAddress("HLT_DoubleEle27_CaloIdL_MW", &HLT_DoubleEle27_CaloIdL_MW, &b_HLT_DoubleEle27_CaloIdL_MW);
   fChain->SetBranchAddress("HLT_DoubleEle33_CaloIdL_MW", &HLT_DoubleEle33_CaloIdL_MW, &b_HLT_DoubleEle33_CaloIdL_MW);
   fChain->SetBranchAddress("HLT_DoubleEle24_eta2p1_WPTight_Gsf", &HLT_DoubleEle24_eta2p1_WPTight_Gsf, &b_HLT_DoubleEle24_eta2p1_WPTight_Gsf);
   fChain->SetBranchAddress("HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_DZ_PFHT350", &HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_DZ_PFHT350, &b_HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_DZ_PFHT350);
   fChain->SetBranchAddress("HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_PFHT350", &HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_PFHT350, &b_HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_PFHT350);
   fChain->SetBranchAddress("HLT_Mu27_Ele37_CaloIdL_MW", &HLT_Mu27_Ele37_CaloIdL_MW, &b_HLT_Mu27_Ele37_CaloIdL_MW);
   fChain->SetBranchAddress("HLT_Mu37_Ele27_CaloIdL_MW", &HLT_Mu37_Ele27_CaloIdL_MW, &b_HLT_Mu37_Ele27_CaloIdL_MW);
   fChain->SetBranchAddress("HLT_Mu37_TkMu27", &HLT_Mu37_TkMu27, &b_HLT_Mu37_TkMu27);
   fChain->SetBranchAddress("HLT_DoubleMu4_3_Bs", &HLT_DoubleMu4_3_Bs, &b_HLT_DoubleMu4_3_Bs);
   fChain->SetBranchAddress("HLT_DoubleMu4_3_Jpsi", &HLT_DoubleMu4_3_Jpsi, &b_HLT_DoubleMu4_3_Jpsi);
   fChain->SetBranchAddress("HLT_DoubleMu4_3_LowMass", &HLT_DoubleMu4_3_LowMass, &b_HLT_DoubleMu4_3_LowMass);
   fChain->SetBranchAddress("HLT_DoubleMu4_LowMass_Displaced", &HLT_DoubleMu4_LowMass_Displaced, &b_HLT_DoubleMu4_LowMass_Displaced);
   fChain->SetBranchAddress("HLT_Mu0_L1DoubleMu", &HLT_Mu0_L1DoubleMu, &b_HLT_Mu0_L1DoubleMu);
   fChain->SetBranchAddress("HLT_Mu4_L1DoubleMu", &HLT_Mu4_L1DoubleMu, &b_HLT_Mu4_L1DoubleMu);
   fChain->SetBranchAddress("HLT_DoubleMu2_Jpsi_LowPt", &HLT_DoubleMu2_Jpsi_LowPt, &b_HLT_DoubleMu2_Jpsi_LowPt);
   fChain->SetBranchAddress("HLT_DoubleMu4_3_Photon4_BsToMMG", &HLT_DoubleMu4_3_Photon4_BsToMMG, &b_HLT_DoubleMu4_3_Photon4_BsToMMG);
   fChain->SetBranchAddress("HLT_DoubleMu4_3_Displaced_Photon4_BsToMMG", &HLT_DoubleMu4_3_Displaced_Photon4_BsToMMG, &b_HLT_DoubleMu4_3_Displaced_Photon4_BsToMMG);
   fChain->SetBranchAddress("HLT_DoubleMu3_Trk_Tau3mu", &HLT_DoubleMu3_Trk_Tau3mu, &b_HLT_DoubleMu3_Trk_Tau3mu);
   fChain->SetBranchAddress("HLT_DoubleMu3_TkMu_DsTau3Mu", &HLT_DoubleMu3_TkMu_DsTau3Mu, &b_HLT_DoubleMu3_TkMu_DsTau3Mu);
   fChain->SetBranchAddress("HLT_DoubleMu4_Mass3p8_DZ_PFHT350", &HLT_DoubleMu4_Mass3p8_DZ_PFHT350, &b_HLT_DoubleMu4_Mass3p8_DZ_PFHT350);
   fChain->SetBranchAddress("HLT_DoubleMu4_MuMuTrk_Displaced", &HLT_DoubleMu4_MuMuTrk_Displaced, &b_HLT_DoubleMu4_MuMuTrk_Displaced);
   fChain->SetBranchAddress("HLT_Mu3_PFJet40", &HLT_Mu3_PFJet40, &b_HLT_Mu3_PFJet40);
   fChain->SetBranchAddress("HLT_Mu7p5_L2Mu2_Jpsi", &HLT_Mu7p5_L2Mu2_Jpsi, &b_HLT_Mu7p5_L2Mu2_Jpsi);
   fChain->SetBranchAddress("HLT_Mu7p5_L2Mu2_Upsilon", &HLT_Mu7p5_L2Mu2_Upsilon, &b_HLT_Mu7p5_L2Mu2_Upsilon);
   fChain->SetBranchAddress("HLT_Mu3_L1SingleMu5orSingleMu7", &HLT_Mu3_L1SingleMu5orSingleMu7, &b_HLT_Mu3_L1SingleMu5orSingleMu7);
   fChain->SetBranchAddress("HLT_Mu0_Barrel", &HLT_Mu0_Barrel, &b_HLT_Mu0_Barrel);
   fChain->SetBranchAddress("HLT_Mu0_Barrel_L1HP6", &HLT_Mu0_Barrel_L1HP6, &b_HLT_Mu0_Barrel_L1HP6);
   fChain->SetBranchAddress("HLT_Mu0_Barrel_L1HP7", &HLT_Mu0_Barrel_L1HP7, &b_HLT_Mu0_Barrel_L1HP7);
   fChain->SetBranchAddress("HLT_Mu0_Barrel_L1HP8", &HLT_Mu0_Barrel_L1HP8, &b_HLT_Mu0_Barrel_L1HP8);
   fChain->SetBranchAddress("HLT_Mu0_Barrel_L1HP9", &HLT_Mu0_Barrel_L1HP9, &b_HLT_Mu0_Barrel_L1HP9);
   fChain->SetBranchAddress("HLT_Mu0_Barrel_L1HP10", &HLT_Mu0_Barrel_L1HP10, &b_HLT_Mu0_Barrel_L1HP10);
   fChain->SetBranchAddress("HLT_Mu0_Barrel_L1HP11", &HLT_Mu0_Barrel_L1HP11, &b_HLT_Mu0_Barrel_L1HP11);
   fChain->SetBranchAddress("HLT_Mu0_Barrel_L1HP6_IP6", &HLT_Mu0_Barrel_L1HP6_IP6, &b_HLT_Mu0_Barrel_L1HP6_IP6);
   fChain->SetBranchAddress("HLT_Mu6_Barrel_L1HP7_IP6", &HLT_Mu6_Barrel_L1HP7_IP6, &b_HLT_Mu6_Barrel_L1HP7_IP6);
   fChain->SetBranchAddress("HLT_Mu7_Barrel_L1HP8_IP6", &HLT_Mu7_Barrel_L1HP8_IP6, &b_HLT_Mu7_Barrel_L1HP8_IP6);
   fChain->SetBranchAddress("HLT_Mu8_Barrel_L1HP9_IP6", &HLT_Mu8_Barrel_L1HP9_IP6, &b_HLT_Mu8_Barrel_L1HP9_IP6);
   fChain->SetBranchAddress("HLT_Mu9_Barrel_L1HP10_IP6", &HLT_Mu9_Barrel_L1HP10_IP6, &b_HLT_Mu9_Barrel_L1HP10_IP6);
   fChain->SetBranchAddress("HLT_Mu10_Barrel_L1HP11_IP6", &HLT_Mu10_Barrel_L1HP11_IP6, &b_HLT_Mu10_Barrel_L1HP11_IP6);
   fChain->SetBranchAddress("HLT_DoublePhoton33_CaloIdL", &HLT_DoublePhoton33_CaloIdL, &b_HLT_DoublePhoton33_CaloIdL);
   fChain->SetBranchAddress("HLT_DoublePhoton70", &HLT_DoublePhoton70, &b_HLT_DoublePhoton70);
   fChain->SetBranchAddress("HLT_DoublePhoton85", &HLT_DoublePhoton85, &b_HLT_DoublePhoton85);
   fChain->SetBranchAddress("HLT_DiEle27_WPTightCaloOnly_L1DoubleEG", &HLT_DiEle27_WPTightCaloOnly_L1DoubleEG, &b_HLT_DiEle27_WPTightCaloOnly_L1DoubleEG);
   fChain->SetBranchAddress("HLT_Ele30_WPTight_Gsf", &HLT_Ele30_WPTight_Gsf, &b_HLT_Ele30_WPTight_Gsf);
   fChain->SetBranchAddress("HLT_Ele32_WPTight_Gsf", &HLT_Ele32_WPTight_Gsf, &b_HLT_Ele32_WPTight_Gsf);
   fChain->SetBranchAddress("HLT_Ele35_WPTight_Gsf", &HLT_Ele35_WPTight_Gsf, &b_HLT_Ele35_WPTight_Gsf);
   fChain->SetBranchAddress("HLT_Ele38_WPTight_Gsf", &HLT_Ele38_WPTight_Gsf, &b_HLT_Ele38_WPTight_Gsf);
   fChain->SetBranchAddress("HLT_Ele40_WPTight_Gsf", &HLT_Ele40_WPTight_Gsf, &b_HLT_Ele40_WPTight_Gsf);
   fChain->SetBranchAddress("HLT_Ele32_WPTight_Gsf_L1DoubleEG", &HLT_Ele32_WPTight_Gsf_L1DoubleEG, &b_HLT_Ele32_WPTight_Gsf_L1DoubleEG);
   fChain->SetBranchAddress("HLT_IsoMu20", &HLT_IsoMu20, &b_HLT_IsoMu20);
   fChain->SetBranchAddress("HLT_IsoMu24", &HLT_IsoMu24, &b_HLT_IsoMu24);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1", &HLT_IsoMu24_eta2p1, &b_HLT_IsoMu24_eta2p1);
   fChain->SetBranchAddress("HLT_IsoMu27", &HLT_IsoMu27, &b_HLT_IsoMu27);
   fChain->SetBranchAddress("HLT_UncorrectedJetE30_NoBPTX", &HLT_UncorrectedJetE30_NoBPTX, &b_HLT_UncorrectedJetE30_NoBPTX);
   fChain->SetBranchAddress("HLT_UncorrectedJetE30_NoBPTX3BX", &HLT_UncorrectedJetE30_NoBPTX3BX, &b_HLT_UncorrectedJetE30_NoBPTX3BX);
   fChain->SetBranchAddress("HLT_UncorrectedJetE60_NoBPTX3BX", &HLT_UncorrectedJetE60_NoBPTX3BX, &b_HLT_UncorrectedJetE60_NoBPTX3BX);
   fChain->SetBranchAddress("HLT_UncorrectedJetE70_NoBPTX3BX", &HLT_UncorrectedJetE70_NoBPTX3BX, &b_HLT_UncorrectedJetE70_NoBPTX3BX);
   fChain->SetBranchAddress("HLT_L1SingleMuCosmics", &HLT_L1SingleMuCosmics, &b_HLT_L1SingleMuCosmics);
   fChain->SetBranchAddress("HLT_L2Mu10_NoVertex_NoBPTX3BX", &HLT_L2Mu10_NoVertex_NoBPTX3BX, &b_HLT_L2Mu10_NoVertex_NoBPTX3BX);
   fChain->SetBranchAddress("HLT_L2Mu10_NoVertex_NoBPTX", &HLT_L2Mu10_NoVertex_NoBPTX, &b_HLT_L2Mu10_NoVertex_NoBPTX);
   fChain->SetBranchAddress("HLT_L2Mu45_NoVertex_3Sta_NoBPTX3BX", &HLT_L2Mu45_NoVertex_3Sta_NoBPTX3BX, &b_HLT_L2Mu45_NoVertex_3Sta_NoBPTX3BX);
   fChain->SetBranchAddress("HLT_L2Mu40_NoVertex_3Sta_NoBPTX3BX", &HLT_L2Mu40_NoVertex_3Sta_NoBPTX3BX, &b_HLT_L2Mu40_NoVertex_3Sta_NoBPTX3BX);
   fChain->SetBranchAddress("HLT_L2Mu23NoVtx_2Cha", &HLT_L2Mu23NoVtx_2Cha, &b_HLT_L2Mu23NoVtx_2Cha);
   fChain->SetBranchAddress("HLT_L2Mu23NoVtx_2Cha_CosmicSeed", &HLT_L2Mu23NoVtx_2Cha_CosmicSeed, &b_HLT_L2Mu23NoVtx_2Cha_CosmicSeed);
   fChain->SetBranchAddress("HLT_DoubleL2Mu30NoVtx_2Cha_CosmicSeed_Eta2p4", &HLT_DoubleL2Mu30NoVtx_2Cha_CosmicSeed_Eta2p4, &b_HLT_DoubleL2Mu30NoVtx_2Cha_CosmicSeed_Eta2p4);
   fChain->SetBranchAddress("HLT_DoubleL2Mu30NoVtx_2Cha_Eta2p4", &HLT_DoubleL2Mu30NoVtx_2Cha_Eta2p4, &b_HLT_DoubleL2Mu30NoVtx_2Cha_Eta2p4);
   fChain->SetBranchAddress("HLT_DoubleL2Mu50", &HLT_DoubleL2Mu50, &b_HLT_DoubleL2Mu50);
   fChain->SetBranchAddress("HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed", &HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed, &b_HLT_DoubleL2Mu23NoVtx_2Cha_CosmicSeed);
   fChain->SetBranchAddress("HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed", &HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed, &b_HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed);
   fChain->SetBranchAddress("HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed_Eta2p4", &HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed_Eta2p4, &b_HLT_DoubleL2Mu25NoVtx_2Cha_CosmicSeed_Eta2p4);
   fChain->SetBranchAddress("HLT_DoubleL2Mu23NoVtx_2Cha", &HLT_DoubleL2Mu23NoVtx_2Cha, &b_HLT_DoubleL2Mu23NoVtx_2Cha);
   fChain->SetBranchAddress("HLT_DoubleL2Mu25NoVtx_2Cha", &HLT_DoubleL2Mu25NoVtx_2Cha, &b_HLT_DoubleL2Mu25NoVtx_2Cha);
   fChain->SetBranchAddress("HLT_DoubleL2Mu25NoVtx_2Cha_Eta2p4", &HLT_DoubleL2Mu25NoVtx_2Cha_Eta2p4, &b_HLT_DoubleL2Mu25NoVtx_2Cha_Eta2p4);
   fChain->SetBranchAddress("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL", &HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL, &b_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL);
   fChain->SetBranchAddress("HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL", &HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL, &b_HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL);
   fChain->SetBranchAddress("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ", &HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ, &b_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ);
   fChain->SetBranchAddress("HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ", &HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ, &b_HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ);
   fChain->SetBranchAddress("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8", &HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8, &b_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8);
   fChain->SetBranchAddress("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_PFJet30", &HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_PFJet30, &b_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_PFJet30);
   fChain->SetBranchAddress("HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass8", &HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass8, &b_HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass8);
   fChain->SetBranchAddress("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8", &HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8, &b_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8);
   fChain->SetBranchAddress("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_CaloJet30", &HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_CaloJet30, &b_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_CaloJet30);
   fChain->SetBranchAddress("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_AK8PFJet30", &HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_AK8PFJet30, &b_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_AK8PFJet30);
   fChain->SetBranchAddress("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_AK8CaloJet30", &HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_AK8CaloJet30, &b_HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8_AK8CaloJet30);
   fChain->SetBranchAddress("HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass3p8", &HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass3p8, &b_HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_Mass3p8);
   fChain->SetBranchAddress("HLT_Mu30_TkMu0_Psi", &HLT_Mu30_TkMu0_Psi, &b_HLT_Mu30_TkMu0_Psi);
   fChain->SetBranchAddress("HLT_Mu30_TkMu0_Upsilon", &HLT_Mu30_TkMu0_Upsilon, &b_HLT_Mu30_TkMu0_Upsilon);
   fChain->SetBranchAddress("HLT_Mu25_TkMu0_Phi", &HLT_Mu25_TkMu0_Phi, &b_HLT_Mu25_TkMu0_Phi);
   fChain->SetBranchAddress("HLT_Mu15", &HLT_Mu15, &b_HLT_Mu15);
   fChain->SetBranchAddress("HLT_Mu20", &HLT_Mu20, &b_HLT_Mu20);
   fChain->SetBranchAddress("HLT_Mu27", &HLT_Mu27, &b_HLT_Mu27);
   fChain->SetBranchAddress("HLT_Mu50", &HLT_Mu50, &b_HLT_Mu50);
   fChain->SetBranchAddress("HLT_Mu55", &HLT_Mu55, &b_HLT_Mu55);
   fChain->SetBranchAddress("HLT_CascadeMu100", &HLT_CascadeMu100, &b_HLT_CascadeMu100);
   fChain->SetBranchAddress("HLT_HighPtTkMu100", &HLT_HighPtTkMu100, &b_HLT_HighPtTkMu100);
   fChain->SetBranchAddress("HLT_DiPFJetAve40", &HLT_DiPFJetAve40, &b_HLT_DiPFJetAve40);
   fChain->SetBranchAddress("HLT_DiPFJetAve60", &HLT_DiPFJetAve60, &b_HLT_DiPFJetAve60);
   fChain->SetBranchAddress("HLT_DiPFJetAve80", &HLT_DiPFJetAve80, &b_HLT_DiPFJetAve80);
   fChain->SetBranchAddress("HLT_DiPFJetAve140", &HLT_DiPFJetAve140, &b_HLT_DiPFJetAve140);
   fChain->SetBranchAddress("HLT_DiPFJetAve200", &HLT_DiPFJetAve200, &b_HLT_DiPFJetAve200);
   fChain->SetBranchAddress("HLT_DiPFJetAve260", &HLT_DiPFJetAve260, &b_HLT_DiPFJetAve260);
   fChain->SetBranchAddress("HLT_DiPFJetAve320", &HLT_DiPFJetAve320, &b_HLT_DiPFJetAve320);
   fChain->SetBranchAddress("HLT_DiPFJetAve400", &HLT_DiPFJetAve400, &b_HLT_DiPFJetAve400);
   fChain->SetBranchAddress("HLT_DiPFJetAve500", &HLT_DiPFJetAve500, &b_HLT_DiPFJetAve500);
   fChain->SetBranchAddress("HLT_DiPFJetAve60_HFJEC", &HLT_DiPFJetAve60_HFJEC, &b_HLT_DiPFJetAve60_HFJEC);
   fChain->SetBranchAddress("HLT_DiPFJetAve80_HFJEC", &HLT_DiPFJetAve80_HFJEC, &b_HLT_DiPFJetAve80_HFJEC);
   fChain->SetBranchAddress("HLT_DiPFJetAve100_HFJEC", &HLT_DiPFJetAve100_HFJEC, &b_HLT_DiPFJetAve100_HFJEC);
   fChain->SetBranchAddress("HLT_DiPFJetAve160_HFJEC", &HLT_DiPFJetAve160_HFJEC, &b_HLT_DiPFJetAve160_HFJEC);
   fChain->SetBranchAddress("HLT_DiPFJetAve220_HFJEC", &HLT_DiPFJetAve220_HFJEC, &b_HLT_DiPFJetAve220_HFJEC);
   fChain->SetBranchAddress("HLT_DiPFJetAve260_HFJEC", &HLT_DiPFJetAve260_HFJEC, &b_HLT_DiPFJetAve260_HFJEC);
   fChain->SetBranchAddress("HLT_DiPFJetAve300_HFJEC", &HLT_DiPFJetAve300_HFJEC, &b_HLT_DiPFJetAve300_HFJEC);
   fChain->SetBranchAddress("HLT_DiPFJetAve180_PPSMatch_Xi0p3_QuadJet_Max2ProtPerRP", &HLT_DiPFJetAve180_PPSMatch_Xi0p3_QuadJet_Max2ProtPerRP, &b_HLT_DiPFJetAve180_PPSMatch_Xi0p3_QuadJet_Max2ProtPerRP);
   fChain->SetBranchAddress("HLT_AK8PFJet40", &HLT_AK8PFJet40, &b_HLT_AK8PFJet40);
   fChain->SetBranchAddress("HLT_AK8PFJet60", &HLT_AK8PFJet60, &b_HLT_AK8PFJet60);
   fChain->SetBranchAddress("HLT_AK8PFJet80", &HLT_AK8PFJet80, &b_HLT_AK8PFJet80);
   fChain->SetBranchAddress("HLT_AK8PFJet140", &HLT_AK8PFJet140, &b_HLT_AK8PFJet140);
   fChain->SetBranchAddress("HLT_AK8PFJet200", &HLT_AK8PFJet200, &b_HLT_AK8PFJet200);
   fChain->SetBranchAddress("HLT_AK8PFJet260", &HLT_AK8PFJet260, &b_HLT_AK8PFJet260);
   fChain->SetBranchAddress("HLT_AK8PFJet320", &HLT_AK8PFJet320, &b_HLT_AK8PFJet320);
   fChain->SetBranchAddress("HLT_AK8PFJet400", &HLT_AK8PFJet400, &b_HLT_AK8PFJet400);
   fChain->SetBranchAddress("HLT_AK8PFJet450", &HLT_AK8PFJet450, &b_HLT_AK8PFJet450);
   fChain->SetBranchAddress("HLT_AK8PFJet500", &HLT_AK8PFJet500, &b_HLT_AK8PFJet500);
   fChain->SetBranchAddress("HLT_AK8PFJet550", &HLT_AK8PFJet550, &b_HLT_AK8PFJet550);
   fChain->SetBranchAddress("HLT_PFJet40", &HLT_PFJet40, &b_HLT_PFJet40);
   fChain->SetBranchAddress("HLT_PFJet60", &HLT_PFJet60, &b_HLT_PFJet60);
   fChain->SetBranchAddress("HLT_PFJet80", &HLT_PFJet80, &b_HLT_PFJet80);
   fChain->SetBranchAddress("HLT_PFJet110", &HLT_PFJet110, &b_HLT_PFJet110);
   fChain->SetBranchAddress("HLT_PFJet140", &HLT_PFJet140, &b_HLT_PFJet140);
   fChain->SetBranchAddress("HLT_PFJet200", &HLT_PFJet200, &b_HLT_PFJet200);
   fChain->SetBranchAddress("HLT_PFJet260", &HLT_PFJet260, &b_HLT_PFJet260);
   fChain->SetBranchAddress("HLT_PFJet320", &HLT_PFJet320, &b_HLT_PFJet320);
   fChain->SetBranchAddress("HLT_PFJet400", &HLT_PFJet400, &b_HLT_PFJet400);
   fChain->SetBranchAddress("HLT_PFJet450", &HLT_PFJet450, &b_HLT_PFJet450);
   fChain->SetBranchAddress("HLT_PFJet500", &HLT_PFJet500, &b_HLT_PFJet500);
   fChain->SetBranchAddress("HLT_PFJet550", &HLT_PFJet550, &b_HLT_PFJet550);
   fChain->SetBranchAddress("HLT_PFJetFwd40", &HLT_PFJetFwd40, &b_HLT_PFJetFwd40);
   fChain->SetBranchAddress("HLT_PFJetFwd60", &HLT_PFJetFwd60, &b_HLT_PFJetFwd60);
   fChain->SetBranchAddress("HLT_PFJetFwd80", &HLT_PFJetFwd80, &b_HLT_PFJetFwd80);
   fChain->SetBranchAddress("HLT_PFJetFwd140", &HLT_PFJetFwd140, &b_HLT_PFJetFwd140);
   fChain->SetBranchAddress("HLT_PFJetFwd200", &HLT_PFJetFwd200, &b_HLT_PFJetFwd200);
   fChain->SetBranchAddress("HLT_PFJetFwd260", &HLT_PFJetFwd260, &b_HLT_PFJetFwd260);
   fChain->SetBranchAddress("HLT_PFJetFwd320", &HLT_PFJetFwd320, &b_HLT_PFJetFwd320);
   fChain->SetBranchAddress("HLT_PFJetFwd400", &HLT_PFJetFwd400, &b_HLT_PFJetFwd400);
   fChain->SetBranchAddress("HLT_PFJetFwd450", &HLT_PFJetFwd450, &b_HLT_PFJetFwd450);
   fChain->SetBranchAddress("HLT_PFJetFwd500", &HLT_PFJetFwd500, &b_HLT_PFJetFwd500);
   fChain->SetBranchAddress("HLT_AK8PFJetFwd40", &HLT_AK8PFJetFwd40, &b_HLT_AK8PFJetFwd40);
   fChain->SetBranchAddress("HLT_AK8PFJetFwd60", &HLT_AK8PFJetFwd60, &b_HLT_AK8PFJetFwd60);
   fChain->SetBranchAddress("HLT_AK8PFJetFwd80", &HLT_AK8PFJetFwd80, &b_HLT_AK8PFJetFwd80);
   fChain->SetBranchAddress("HLT_AK8PFJetFwd140", &HLT_AK8PFJetFwd140, &b_HLT_AK8PFJetFwd140);
   fChain->SetBranchAddress("HLT_AK8PFJetFwd200", &HLT_AK8PFJetFwd200, &b_HLT_AK8PFJetFwd200);
   fChain->SetBranchAddress("HLT_AK8PFJetFwd260", &HLT_AK8PFJetFwd260, &b_HLT_AK8PFJetFwd260);
   fChain->SetBranchAddress("HLT_AK8PFJetFwd320", &HLT_AK8PFJetFwd320, &b_HLT_AK8PFJetFwd320);
   fChain->SetBranchAddress("HLT_AK8PFJetFwd400", &HLT_AK8PFJetFwd400, &b_HLT_AK8PFJetFwd400);
   fChain->SetBranchAddress("HLT_AK8PFJetFwd450", &HLT_AK8PFJetFwd450, &b_HLT_AK8PFJetFwd450);
   fChain->SetBranchAddress("HLT_AK8PFJetFwd500", &HLT_AK8PFJetFwd500, &b_HLT_AK8PFJetFwd500);
   fChain->SetBranchAddress("HLT_PFHT180", &HLT_PFHT180, &b_HLT_PFHT180);
   fChain->SetBranchAddress("HLT_PFHT250", &HLT_PFHT250, &b_HLT_PFHT250);
   fChain->SetBranchAddress("HLT_PFHT370", &HLT_PFHT370, &b_HLT_PFHT370);
   fChain->SetBranchAddress("HLT_PFHT430", &HLT_PFHT430, &b_HLT_PFHT430);
   fChain->SetBranchAddress("HLT_PFHT510", &HLT_PFHT510, &b_HLT_PFHT510);
   fChain->SetBranchAddress("HLT_PFHT590", &HLT_PFHT590, &b_HLT_PFHT590);
   fChain->SetBranchAddress("HLT_PFHT680", &HLT_PFHT680, &b_HLT_PFHT680);
   fChain->SetBranchAddress("HLT_PFHT780", &HLT_PFHT780, &b_HLT_PFHT780);
   fChain->SetBranchAddress("HLT_PFHT890", &HLT_PFHT890, &b_HLT_PFHT890);
   fChain->SetBranchAddress("HLT_PFHT1050", &HLT_PFHT1050, &b_HLT_PFHT1050);
   fChain->SetBranchAddress("HLT_PFHT500_PFMET100_PFMHT100_IDTight", &HLT_PFHT500_PFMET100_PFMHT100_IDTight, &b_HLT_PFHT500_PFMET100_PFMHT100_IDTight);
   fChain->SetBranchAddress("HLT_PFHT500_PFMET110_PFMHT110_IDTight", &HLT_PFHT500_PFMET110_PFMHT110_IDTight, &b_HLT_PFHT500_PFMET110_PFMHT110_IDTight);
   fChain->SetBranchAddress("HLT_PFHT700_PFMET85_PFMHT85_IDTight", &HLT_PFHT700_PFMET85_PFMHT85_IDTight, &b_HLT_PFHT700_PFMET85_PFMHT85_IDTight);
   fChain->SetBranchAddress("HLT_PFHT800_PFMET75_PFMHT75_IDTight", &HLT_PFHT800_PFMET75_PFMHT75_IDTight, &b_HLT_PFHT800_PFMET75_PFMHT75_IDTight);
   fChain->SetBranchAddress("HLT_PFMET120_PFMHT120_IDTight", &HLT_PFMET120_PFMHT120_IDTight, &b_HLT_PFMET120_PFMHT120_IDTight);
   fChain->SetBranchAddress("HLT_PFMET130_PFMHT130_IDTight", &HLT_PFMET130_PFMHT130_IDTight, &b_HLT_PFMET130_PFMHT130_IDTight);
   fChain->SetBranchAddress("HLT_PFMET140_PFMHT140_IDTight", &HLT_PFMET140_PFMHT140_IDTight, &b_HLT_PFMET140_PFMHT140_IDTight);
   fChain->SetBranchAddress("HLT_PFMET120_PFMHT120_IDTight_PFHT60", &HLT_PFMET120_PFMHT120_IDTight_PFHT60, &b_HLT_PFMET120_PFMHT120_IDTight_PFHT60);
   fChain->SetBranchAddress("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60", &HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60, &b_HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60);
   fChain->SetBranchAddress("HLT_PFMETTypeOne140_PFMHT140_IDTight", &HLT_PFMETTypeOne140_PFMHT140_IDTight, &b_HLT_PFMETTypeOne140_PFMHT140_IDTight);
   fChain->SetBranchAddress("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight", &HLT_PFMETNoMu120_PFMHTNoMu120_IDTight, &b_HLT_PFMETNoMu120_PFMHTNoMu120_IDTight);
   fChain->SetBranchAddress("HLT_PFMETNoMu130_PFMHTNoMu130_IDTight", &HLT_PFMETNoMu130_PFMHTNoMu130_IDTight, &b_HLT_PFMETNoMu130_PFMHTNoMu130_IDTight);
   fChain->SetBranchAddress("HLT_PFMETNoMu140_PFMHTNoMu140_IDTight", &HLT_PFMETNoMu140_PFMHTNoMu140_IDTight, &b_HLT_PFMETNoMu140_PFMHTNoMu140_IDTight);
   fChain->SetBranchAddress("HLT_PFMETNoMu110_PFMHTNoMu110_IDTight_FilterHF", &HLT_PFMETNoMu110_PFMHTNoMu110_IDTight_FilterHF, &b_HLT_PFMETNoMu110_PFMHTNoMu110_IDTight_FilterHF);
   fChain->SetBranchAddress("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_FilterHF", &HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_FilterHF, &b_HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_FilterHF);
   fChain->SetBranchAddress("HLT_PFMETNoMu130_PFMHTNoMu130_IDTight_FilterHF", &HLT_PFMETNoMu130_PFMHTNoMu130_IDTight_FilterHF, &b_HLT_PFMETNoMu130_PFMHTNoMu130_IDTight_FilterHF);
   fChain->SetBranchAddress("HLT_PFMETNoMu140_PFMHTNoMu140_IDTight_FilterHF", &HLT_PFMETNoMu140_PFMHTNoMu140_IDTight_FilterHF, &b_HLT_PFMETNoMu140_PFMHTNoMu140_IDTight_FilterHF);
   fChain->SetBranchAddress("HLT_L1ETMHadSeeds", &HLT_L1ETMHadSeeds, &b_HLT_L1ETMHadSeeds);
   fChain->SetBranchAddress("HLT_CaloMHT90", &HLT_CaloMHT90, &b_HLT_CaloMHT90);
   fChain->SetBranchAddress("HLT_CaloMET90_NotCleaned", &HLT_CaloMET90_NotCleaned, &b_HLT_CaloMET90_NotCleaned);
   fChain->SetBranchAddress("HLT_CaloMET350_NotCleaned", &HLT_CaloMET350_NotCleaned, &b_HLT_CaloMET350_NotCleaned);
   fChain->SetBranchAddress("HLT_PFMET200_NotCleaned", &HLT_PFMET200_NotCleaned, &b_HLT_PFMET200_NotCleaned);
   fChain->SetBranchAddress("HLT_PFMET250_NotCleaned", &HLT_PFMET250_NotCleaned, &b_HLT_PFMET250_NotCleaned);
   fChain->SetBranchAddress("HLT_PFMET300_NotCleaned", &HLT_PFMET300_NotCleaned, &b_HLT_PFMET300_NotCleaned);
   fChain->SetBranchAddress("HLT_PFMET200_BeamHaloCleaned", &HLT_PFMET200_BeamHaloCleaned, &b_HLT_PFMET200_BeamHaloCleaned);
   fChain->SetBranchAddress("HLT_PFMETTypeOne200_BeamHaloCleaned", &HLT_PFMETTypeOne200_BeamHaloCleaned, &b_HLT_PFMETTypeOne200_BeamHaloCleaned);
   fChain->SetBranchAddress("HLT_MET105_IsoTrk50", &HLT_MET105_IsoTrk50, &b_HLT_MET105_IsoTrk50);
   fChain->SetBranchAddress("HLT_MET120_IsoTrk50", &HLT_MET120_IsoTrk50, &b_HLT_MET120_IsoTrk50);
   fChain->SetBranchAddress("HLT_Mu12eta2p3", &HLT_Mu12eta2p3, &b_HLT_Mu12eta2p3);
   fChain->SetBranchAddress("HLT_Mu12eta2p3_PFJet40", &HLT_Mu12eta2p3_PFJet40, &b_HLT_Mu12eta2p3_PFJet40);
   fChain->SetBranchAddress("HLT_Mu12_DoublePFJets40_PNetBTag_0p11", &HLT_Mu12_DoublePFJets40_PNetBTag_0p11, &b_HLT_Mu12_DoublePFJets40_PNetBTag_0p11);
   fChain->SetBranchAddress("HLT_Mu12_DoublePFJets100_PNetBTag_0p11", &HLT_Mu12_DoublePFJets100_PNetBTag_0p11, &b_HLT_Mu12_DoublePFJets100_PNetBTag_0p11);
   fChain->SetBranchAddress("HLT_Mu12_DoublePFJets200_PNetBTag_0p11", &HLT_Mu12_DoublePFJets200_PNetBTag_0p11, &b_HLT_Mu12_DoublePFJets200_PNetBTag_0p11);
   fChain->SetBranchAddress("HLT_Mu12_DoublePFJets350_PNetBTag_0p11", &HLT_Mu12_DoublePFJets350_PNetBTag_0p11, &b_HLT_Mu12_DoublePFJets350_PNetBTag_0p11);
   fChain->SetBranchAddress("HLT_Mu12_DoublePFJets40MaxDeta1p6_PNet2BTag_0p11", &HLT_Mu12_DoublePFJets40MaxDeta1p6_PNet2BTag_0p11, &b_HLT_Mu12_DoublePFJets40MaxDeta1p6_PNet2BTag_0p11);
   fChain->SetBranchAddress("HLT_Mu12_DoublePFJets54MaxDeta1p6_PNet2BTag_0p11", &HLT_Mu12_DoublePFJets54MaxDeta1p6_PNet2BTag_0p11, &b_HLT_Mu12_DoublePFJets54MaxDeta1p6_PNet2BTag_0p11);
   fChain->SetBranchAddress("HLT_DoublePFJets40_PNetBTag_0p11", &HLT_DoublePFJets40_PNetBTag_0p11, &b_HLT_DoublePFJets40_PNetBTag_0p11);
   fChain->SetBranchAddress("HLT_DoublePFJets100_PNetBTag_0p11", &HLT_DoublePFJets100_PNetBTag_0p11, &b_HLT_DoublePFJets100_PNetBTag_0p11);
   fChain->SetBranchAddress("HLT_DoublePFJets200_PNetBTag_0p11", &HLT_DoublePFJets200_PNetBTag_0p11, &b_HLT_DoublePFJets200_PNetBTag_0p11);
   fChain->SetBranchAddress("HLT_DoublePFJets350_PNetBTag_0p11", &HLT_DoublePFJets350_PNetBTag_0p11, &b_HLT_DoublePFJets350_PNetBTag_0p11);
   fChain->SetBranchAddress("HLT_DoublePFJets116MaxDeta1p6_PNet2BTag_0p11", &HLT_DoublePFJets116MaxDeta1p6_PNet2BTag_0p11, &b_HLT_DoublePFJets116MaxDeta1p6_PNet2BTag_0p11);
   fChain->SetBranchAddress("HLT_DoublePFJets128MaxDeta1p6_PNet2BTag_0p11", &HLT_DoublePFJets128MaxDeta1p6_PNet2BTag_0p11, &b_HLT_DoublePFJets128MaxDeta1p6_PNet2BTag_0p11);
   fChain->SetBranchAddress("HLT_Photon300_NoHE", &HLT_Photon300_NoHE, &b_HLT_Photon300_NoHE);
   fChain->SetBranchAddress("HLT_Mu8_TrkIsoVVL", &HLT_Mu8_TrkIsoVVL, &b_HLT_Mu8_TrkIsoVVL);
   fChain->SetBranchAddress("HLT_Mu8_DiEle12_CaloIdL_TrackIdL_DZ", &HLT_Mu8_DiEle12_CaloIdL_TrackIdL_DZ, &b_HLT_Mu8_DiEle12_CaloIdL_TrackIdL_DZ);
   fChain->SetBranchAddress("HLT_Mu8_DiEle12_CaloIdL_TrackIdL", &HLT_Mu8_DiEle12_CaloIdL_TrackIdL, &b_HLT_Mu8_DiEle12_CaloIdL_TrackIdL);
   fChain->SetBranchAddress("HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_DZ", &HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_DZ, &b_HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350_DZ);
   fChain->SetBranchAddress("HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350", &HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350, &b_HLT_Mu8_Ele8_CaloIdM_TrackIdM_Mass8_PFHT350);
   fChain->SetBranchAddress("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ", &HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ, &b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ);
   fChain->SetBranchAddress("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30", &HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30, &b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30);
   fChain->SetBranchAddress("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_CaloDiJet30", &HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_CaloDiJet30, &b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_CaloDiJet30);
   fChain->SetBranchAddress("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL", &HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL, &b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL);
   fChain->SetBranchAddress("HLT_Mu17_TrkIsoVVL", &HLT_Mu17_TrkIsoVVL, &b_HLT_Mu17_TrkIsoVVL);
   fChain->SetBranchAddress("HLT_Mu19_TrkIsoVVL", &HLT_Mu19_TrkIsoVVL, &b_HLT_Mu19_TrkIsoVVL);
   fChain->SetBranchAddress("HLT_BTagMu_AK4DiJet20_Mu5", &HLT_BTagMu_AK4DiJet20_Mu5, &b_HLT_BTagMu_AK4DiJet20_Mu5);
   fChain->SetBranchAddress("HLT_BTagMu_AK4DiJet40_Mu5", &HLT_BTagMu_AK4DiJet40_Mu5, &b_HLT_BTagMu_AK4DiJet40_Mu5);
   fChain->SetBranchAddress("HLT_BTagMu_AK4DiJet70_Mu5", &HLT_BTagMu_AK4DiJet70_Mu5, &b_HLT_BTagMu_AK4DiJet70_Mu5);
   fChain->SetBranchAddress("HLT_BTagMu_AK4DiJet110_Mu5", &HLT_BTagMu_AK4DiJet110_Mu5, &b_HLT_BTagMu_AK4DiJet110_Mu5);
   fChain->SetBranchAddress("HLT_BTagMu_AK4DiJet170_Mu5", &HLT_BTagMu_AK4DiJet170_Mu5, &b_HLT_BTagMu_AK4DiJet170_Mu5);
   fChain->SetBranchAddress("HLT_BTagMu_AK4Jet300_Mu5", &HLT_BTagMu_AK4Jet300_Mu5, &b_HLT_BTagMu_AK4Jet300_Mu5);
   fChain->SetBranchAddress("HLT_BTagMu_AK8DiJet170_Mu5", &HLT_BTagMu_AK8DiJet170_Mu5, &b_HLT_BTagMu_AK8DiJet170_Mu5);
   fChain->SetBranchAddress("HLT_BTagMu_AK8Jet170_DoubleMu5", &HLT_BTagMu_AK8Jet170_DoubleMu5, &b_HLT_BTagMu_AK8Jet170_DoubleMu5);
   fChain->SetBranchAddress("HLT_BTagMu_AK8Jet300_Mu5", &HLT_BTagMu_AK8Jet300_Mu5, &b_HLT_BTagMu_AK8Jet300_Mu5);
   fChain->SetBranchAddress("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ", &HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ, &b_HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ);
   fChain->SetBranchAddress("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL", &HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL, &b_HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL);
   fChain->SetBranchAddress("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ", &HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ, &b_HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ);
   fChain->SetBranchAddress("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL", &HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL, &b_HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL);
   fChain->SetBranchAddress("HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL", &HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL, &b_HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL);
   fChain->SetBranchAddress("HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ", &HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ, &b_HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ);
   fChain->SetBranchAddress("HLT_Mu12_IsoVVL_PFHT150_PNetBTag0p53", &HLT_Mu12_IsoVVL_PFHT150_PNetBTag0p53, &b_HLT_Mu12_IsoVVL_PFHT150_PNetBTag0p53);
   fChain->SetBranchAddress("HLT_Photon33", &HLT_Photon33, &b_HLT_Photon33);
   fChain->SetBranchAddress("HLT_Photon50", &HLT_Photon50, &b_HLT_Photon50);
   fChain->SetBranchAddress("HLT_Photon75", &HLT_Photon75, &b_HLT_Photon75);
   fChain->SetBranchAddress("HLT_Photon90", &HLT_Photon90, &b_HLT_Photon90);
   fChain->SetBranchAddress("HLT_Photon120", &HLT_Photon120, &b_HLT_Photon120);
   fChain->SetBranchAddress("HLT_Photon150", &HLT_Photon150, &b_HLT_Photon150);
   fChain->SetBranchAddress("HLT_Photon175", &HLT_Photon175, &b_HLT_Photon175);
   fChain->SetBranchAddress("HLT_Photon200", &HLT_Photon200, &b_HLT_Photon200);
   fChain->SetBranchAddress("HLT_Photon45EB", &HLT_Photon45EB, &b_HLT_Photon45EB);
   fChain->SetBranchAddress("HLT_Photon40EB", &HLT_Photon40EB, &b_HLT_Photon40EB);
   fChain->SetBranchAddress("HLT_Photon50EB", &HLT_Photon50EB, &b_HLT_Photon50EB);
   fChain->SetBranchAddress("HLT_Photon30EB_TightID_TightIso", &HLT_Photon30EB_TightID_TightIso, &b_HLT_Photon30EB_TightID_TightIso);
   fChain->SetBranchAddress("HLT_Photon40EB_TightID_TightIso", &HLT_Photon40EB_TightID_TightIso, &b_HLT_Photon40EB_TightID_TightIso);
   fChain->SetBranchAddress("HLT_Photon45EB_TightID_TightIso", &HLT_Photon45EB_TightID_TightIso, &b_HLT_Photon45EB_TightID_TightIso);
   fChain->SetBranchAddress("HLT_Photon50EB_TightID_TightIso_PFJet30", &HLT_Photon50EB_TightID_TightIso_PFJet30, &b_HLT_Photon50EB_TightID_TightIso_PFJet30);
   fChain->SetBranchAddress("HLT_Photon50EB_TightID_TightIso_CaloJet30", &HLT_Photon50EB_TightID_TightIso_CaloJet30, &b_HLT_Photon50EB_TightID_TightIso_CaloJet30);
   fChain->SetBranchAddress("HLT_Photon50EB_TightID_TightIso_AK8PFJet30", &HLT_Photon50EB_TightID_TightIso_AK8PFJet30, &b_HLT_Photon50EB_TightID_TightIso_AK8PFJet30);
   fChain->SetBranchAddress("HLT_Photon50EB_TightID_TightIso_AK8CaloJet30", &HLT_Photon50EB_TightID_TightIso_AK8CaloJet30, &b_HLT_Photon50EB_TightID_TightIso_AK8CaloJet30);
   fChain->SetBranchAddress("HLT_Photon50EB_TightID_TightIso", &HLT_Photon50EB_TightID_TightIso, &b_HLT_Photon50EB_TightID_TightIso);
   fChain->SetBranchAddress("HLT_Photon55EB_TightID_TightIso", &HLT_Photon55EB_TightID_TightIso, &b_HLT_Photon55EB_TightID_TightIso);
   fChain->SetBranchAddress("HLT_Photon75EB_TightID_TightIso", &HLT_Photon75EB_TightID_TightIso, &b_HLT_Photon75EB_TightID_TightIso);
   fChain->SetBranchAddress("HLT_Photon90EB_TightID_TightIso", &HLT_Photon90EB_TightID_TightIso, &b_HLT_Photon90EB_TightID_TightIso);
   fChain->SetBranchAddress("HLT_Photon110EB_TightID_TightIso", &HLT_Photon110EB_TightID_TightIso, &b_HLT_Photon110EB_TightID_TightIso);
   fChain->SetBranchAddress("HLT_Photon110EB_TightID_TightIso_PFJet30", &HLT_Photon110EB_TightID_TightIso_PFJet30, &b_HLT_Photon110EB_TightID_TightIso_PFJet30);
   fChain->SetBranchAddress("HLT_Photon110EB_TightID_TightIso_CaloJet30", &HLT_Photon110EB_TightID_TightIso_CaloJet30, &b_HLT_Photon110EB_TightID_TightIso_CaloJet30);
   fChain->SetBranchAddress("HLT_Photon110EB_TightID_TightIso_AK8PFJet30", &HLT_Photon110EB_TightID_TightIso_AK8PFJet30, &b_HLT_Photon110EB_TightID_TightIso_AK8PFJet30);
   fChain->SetBranchAddress("HLT_Photon110EB_TightID_TightIso_AK8CaloJet30", &HLT_Photon110EB_TightID_TightIso_AK8CaloJet30, &b_HLT_Photon110EB_TightID_TightIso_AK8CaloJet30);
   fChain->SetBranchAddress("HLT_Photon100EBHE10", &HLT_Photon100EBHE10, &b_HLT_Photon100EBHE10);
   fChain->SetBranchAddress("HLT_Photon50_R9Id90_HE10_IsoM", &HLT_Photon50_R9Id90_HE10_IsoM, &b_HLT_Photon50_R9Id90_HE10_IsoM);
   fChain->SetBranchAddress("HLT_Photon75_R9Id90_HE10_IsoM", &HLT_Photon75_R9Id90_HE10_IsoM, &b_HLT_Photon75_R9Id90_HE10_IsoM);
   fChain->SetBranchAddress("HLT_Photon90_R9Id90_HE10_IsoM", &HLT_Photon90_R9Id90_HE10_IsoM, &b_HLT_Photon90_R9Id90_HE10_IsoM);
   fChain->SetBranchAddress("HLT_Photon120_R9Id90_HE10_IsoM", &HLT_Photon120_R9Id90_HE10_IsoM, &b_HLT_Photon120_R9Id90_HE10_IsoM);
   fChain->SetBranchAddress("HLT_Photon165_R9Id90_HE10_IsoM", &HLT_Photon165_R9Id90_HE10_IsoM, &b_HLT_Photon165_R9Id90_HE10_IsoM);
   fChain->SetBranchAddress("HLT_Diphoton30_22_R9Id_OR_IsoCaloId_AND_HE_R9Id_Mass90", &HLT_Diphoton30_22_R9Id_OR_IsoCaloId_AND_HE_R9Id_Mass90, &b_HLT_Diphoton30_22_R9Id_OR_IsoCaloId_AND_HE_R9Id_Mass90);
   fChain->SetBranchAddress("HLT_Diphoton30_22_R9Id_OR_IsoCaloId_AND_HE_R9Id_Mass95", &HLT_Diphoton30_22_R9Id_OR_IsoCaloId_AND_HE_R9Id_Mass95, &b_HLT_Diphoton30_22_R9Id_OR_IsoCaloId_AND_HE_R9Id_Mass95);
   fChain->SetBranchAddress("HLT_DiphotonMVA14p25_Mass90", &HLT_DiphotonMVA14p25_Mass90, &b_HLT_DiphotonMVA14p25_Mass90);
   fChain->SetBranchAddress("HLT_DiphotonMVA14p25_Tight_Mass90", &HLT_DiphotonMVA14p25_Tight_Mass90, &b_HLT_DiphotonMVA14p25_Tight_Mass90);
   fChain->SetBranchAddress("HLT_Photon35_TwoProngs35", &HLT_Photon35_TwoProngs35, &b_HLT_Photon35_TwoProngs35);
   fChain->SetBranchAddress("HLT_IsoMu24_TwoProngs35", &HLT_IsoMu24_TwoProngs35, &b_HLT_IsoMu24_TwoProngs35);
   fChain->SetBranchAddress("HLT_Dimuon0_Jpsi_L1_NoOS", &HLT_Dimuon0_Jpsi_L1_NoOS, &b_HLT_Dimuon0_Jpsi_L1_NoOS);
   fChain->SetBranchAddress("HLT_Dimuon0_Jpsi_NoVertexing_NoOS", &HLT_Dimuon0_Jpsi_NoVertexing_NoOS, &b_HLT_Dimuon0_Jpsi_NoVertexing_NoOS);
   fChain->SetBranchAddress("HLT_Dimuon0_Jpsi", &HLT_Dimuon0_Jpsi, &b_HLT_Dimuon0_Jpsi);
   fChain->SetBranchAddress("HLT_Dimuon0_Jpsi_NoVertexing", &HLT_Dimuon0_Jpsi_NoVertexing, &b_HLT_Dimuon0_Jpsi_NoVertexing);
   fChain->SetBranchAddress("HLT_Dimuon0_Jpsi_L1_4R_0er1p5R", &HLT_Dimuon0_Jpsi_L1_4R_0er1p5R, &b_HLT_Dimuon0_Jpsi_L1_4R_0er1p5R);
   fChain->SetBranchAddress("HLT_Dimuon0_Jpsi_NoVertexing_L1_4R_0er1p5R", &HLT_Dimuon0_Jpsi_NoVertexing_L1_4R_0er1p5R, &b_HLT_Dimuon0_Jpsi_NoVertexing_L1_4R_0er1p5R);
   fChain->SetBranchAddress("HLT_Dimuon0_Jpsi3p5_Muon2", &HLT_Dimuon0_Jpsi3p5_Muon2, &b_HLT_Dimuon0_Jpsi3p5_Muon2);
   fChain->SetBranchAddress("HLT_Dimuon0_Upsilon_L1_4p5", &HLT_Dimuon0_Upsilon_L1_4p5, &b_HLT_Dimuon0_Upsilon_L1_4p5);
   fChain->SetBranchAddress("HLT_Dimuon0_Upsilon_L1_4p5er2p0", &HLT_Dimuon0_Upsilon_L1_4p5er2p0, &b_HLT_Dimuon0_Upsilon_L1_4p5er2p0);
   fChain->SetBranchAddress("HLT_Dimuon0_Upsilon_L1_4p5er2p0M", &HLT_Dimuon0_Upsilon_L1_4p5er2p0M, &b_HLT_Dimuon0_Upsilon_L1_4p5er2p0M);
   fChain->SetBranchAddress("HLT_Dimuon0_Upsilon_NoVertexing", &HLT_Dimuon0_Upsilon_NoVertexing, &b_HLT_Dimuon0_Upsilon_NoVertexing);
   fChain->SetBranchAddress("HLT_Dimuon0_LowMass_L1_0er1p5", &HLT_Dimuon0_LowMass_L1_0er1p5, &b_HLT_Dimuon0_LowMass_L1_0er1p5);
   fChain->SetBranchAddress("HLT_Dimuon0_LowMass", &HLT_Dimuon0_LowMass, &b_HLT_Dimuon0_LowMass);
   fChain->SetBranchAddress("HLT_Dimuon0_LowMass_L1_4", &HLT_Dimuon0_LowMass_L1_4, &b_HLT_Dimuon0_LowMass_L1_4);
   fChain->SetBranchAddress("HLT_Dimuon0_LowMass_L1_TM530", &HLT_Dimuon0_LowMass_L1_TM530, &b_HLT_Dimuon0_LowMass_L1_TM530);
   fChain->SetBranchAddress("HLT_Dimuon0_Upsilon_Muon_NoL1Mass", &HLT_Dimuon0_Upsilon_Muon_NoL1Mass, &b_HLT_Dimuon0_Upsilon_Muon_NoL1Mass);
   fChain->SetBranchAddress("HLT_TripleMu_5_3_3_Mass3p8_DZ", &HLT_TripleMu_5_3_3_Mass3p8_DZ, &b_HLT_TripleMu_5_3_3_Mass3p8_DZ);
   fChain->SetBranchAddress("HLT_TripleMu_10_5_5_DZ", &HLT_TripleMu_10_5_5_DZ, &b_HLT_TripleMu_10_5_5_DZ);
   fChain->SetBranchAddress("HLT_TripleMu_12_10_5", &HLT_TripleMu_12_10_5, &b_HLT_TripleMu_12_10_5);
   fChain->SetBranchAddress("HLT_Tau3Mu_Mu7_Mu1_TkMu1_Tau15", &HLT_Tau3Mu_Mu7_Mu1_TkMu1_Tau15, &b_HLT_Tau3Mu_Mu7_Mu1_TkMu1_Tau15);
   fChain->SetBranchAddress("HLT_Tau3Mu_Mu7_Mu1_TkMu1_Tau15_Charge1", &HLT_Tau3Mu_Mu7_Mu1_TkMu1_Tau15_Charge1, &b_HLT_Tau3Mu_Mu7_Mu1_TkMu1_Tau15_Charge1);
   fChain->SetBranchAddress("HLT_Tau3Mu_Mu7_Mu1_TkMu1_IsoTau15", &HLT_Tau3Mu_Mu7_Mu1_TkMu1_IsoTau15, &b_HLT_Tau3Mu_Mu7_Mu1_TkMu1_IsoTau15);
   fChain->SetBranchAddress("HLT_Tau3Mu_Mu7_Mu1_TkMu1_IsoTau15_Charge1", &HLT_Tau3Mu_Mu7_Mu1_TkMu1_IsoTau15_Charge1, &b_HLT_Tau3Mu_Mu7_Mu1_TkMu1_IsoTau15_Charge1);
   fChain->SetBranchAddress("HLT_DoubleMu3_DZ_PFMET50_PFMHT60", &HLT_DoubleMu3_DZ_PFMET50_PFMHT60, &b_HLT_DoubleMu3_DZ_PFMET50_PFMHT60);
   fChain->SetBranchAddress("HLT_DoubleMu3_DZ_PFMET70_PFMHT70", &HLT_DoubleMu3_DZ_PFMET70_PFMHT70, &b_HLT_DoubleMu3_DZ_PFMET70_PFMHT70);
   fChain->SetBranchAddress("HLT_DoubleMu3_DZ_PFMET90_PFMHT90", &HLT_DoubleMu3_DZ_PFMET90_PFMHT90, &b_HLT_DoubleMu3_DZ_PFMET90_PFMHT90);
   fChain->SetBranchAddress("HLT_DoubleMu3_Trk_Tau3mu_NoL1Mass", &HLT_DoubleMu3_Trk_Tau3mu_NoL1Mass, &b_HLT_DoubleMu3_Trk_Tau3mu_NoL1Mass);
   fChain->SetBranchAddress("HLT_DoubleMu4_3_LowMass_SS", &HLT_DoubleMu4_3_LowMass_SS, &b_HLT_DoubleMu4_3_LowMass_SS);
   fChain->SetBranchAddress("HLT_DoubleMu4_Jpsi_Displaced", &HLT_DoubleMu4_Jpsi_Displaced, &b_HLT_DoubleMu4_Jpsi_Displaced);
   fChain->SetBranchAddress("HLT_DoubleMu4_Jpsi_NoVertexing", &HLT_DoubleMu4_Jpsi_NoVertexing, &b_HLT_DoubleMu4_Jpsi_NoVertexing);
   fChain->SetBranchAddress("HLT_DoubleMu4_JpsiTrkTrk_Displaced", &HLT_DoubleMu4_JpsiTrkTrk_Displaced, &b_HLT_DoubleMu4_JpsiTrkTrk_Displaced);
   fChain->SetBranchAddress("HLT_DoubleMu4_JpsiTrk_Bc", &HLT_DoubleMu4_JpsiTrk_Bc, &b_HLT_DoubleMu4_JpsiTrk_Bc);
   fChain->SetBranchAddress("HLT_DoubleMu43NoFiltersNoVtx", &HLT_DoubleMu43NoFiltersNoVtx, &b_HLT_DoubleMu43NoFiltersNoVtx);
   fChain->SetBranchAddress("HLT_DoubleMu48NoFiltersNoVtx", &HLT_DoubleMu48NoFiltersNoVtx, &b_HLT_DoubleMu48NoFiltersNoVtx);
   fChain->SetBranchAddress("HLT_Mu43NoFiltersNoVtx_Photon43_CaloIdL", &HLT_Mu43NoFiltersNoVtx_Photon43_CaloIdL, &b_HLT_Mu43NoFiltersNoVtx_Photon43_CaloIdL);
   fChain->SetBranchAddress("HLT_Mu48NoFiltersNoVtx_Photon48_CaloIdL", &HLT_Mu48NoFiltersNoVtx_Photon48_CaloIdL, &b_HLT_Mu48NoFiltersNoVtx_Photon48_CaloIdL);
   fChain->SetBranchAddress("HLT_Mu38NoFiltersNoVtxDisplaced_Photon38_CaloIdL", &HLT_Mu38NoFiltersNoVtxDisplaced_Photon38_CaloIdL, &b_HLT_Mu38NoFiltersNoVtxDisplaced_Photon38_CaloIdL);
   fChain->SetBranchAddress("HLT_Mu43NoFiltersNoVtxDisplaced_Photon43_CaloIdL", &HLT_Mu43NoFiltersNoVtxDisplaced_Photon43_CaloIdL, &b_HLT_Mu43NoFiltersNoVtxDisplaced_Photon43_CaloIdL);
   fChain->SetBranchAddress("HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned", &HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned, &b_HLT_Ele30_eta2p1_WPTight_Gsf_CentralPFJet35_EleCleaned);
   fChain->SetBranchAddress("HLT_Ele28_eta2p1_WPTight_Gsf_HT150", &HLT_Ele28_eta2p1_WPTight_Gsf_HT150, &b_HLT_Ele28_eta2p1_WPTight_Gsf_HT150);
   fChain->SetBranchAddress("HLT_Ele28_HighEta_SC20_Mass55", &HLT_Ele28_HighEta_SC20_Mass55, &b_HLT_Ele28_HighEta_SC20_Mass55);
   fChain->SetBranchAddress("HLT_Ele15_IsoVVVL_PFHT450_PFMET50", &HLT_Ele15_IsoVVVL_PFHT450_PFMET50, &b_HLT_Ele15_IsoVVVL_PFHT450_PFMET50);
   fChain->SetBranchAddress("HLT_Ele15_IsoVVVL_PFHT450", &HLT_Ele15_IsoVVVL_PFHT450, &b_HLT_Ele15_IsoVVVL_PFHT450);
   fChain->SetBranchAddress("HLT_Ele50_IsoVVVL_PFHT450", &HLT_Ele50_IsoVVVL_PFHT450, &b_HLT_Ele50_IsoVVVL_PFHT450);
   fChain->SetBranchAddress("HLT_Ele15_IsoVVVL_PFHT600", &HLT_Ele15_IsoVVVL_PFHT600, &b_HLT_Ele15_IsoVVVL_PFHT600);
   fChain->SetBranchAddress("HLT_Mu15_IsoVVVL_PFHT450_PFMET50", &HLT_Mu15_IsoVVVL_PFHT450_PFMET50, &b_HLT_Mu15_IsoVVVL_PFHT450_PFMET50);
   fChain->SetBranchAddress("HLT_Mu15_IsoVVVL_PFHT450", &HLT_Mu15_IsoVVVL_PFHT450, &b_HLT_Mu15_IsoVVVL_PFHT450);
   fChain->SetBranchAddress("HLT_Mu50_IsoVVVL_PFHT450", &HLT_Mu50_IsoVVVL_PFHT450, &b_HLT_Mu50_IsoVVVL_PFHT450);
   fChain->SetBranchAddress("HLT_Mu15_IsoVVVL_PFHT600", &HLT_Mu15_IsoVVVL_PFHT600, &b_HLT_Mu15_IsoVVVL_PFHT600);
   fChain->SetBranchAddress("HLT_Mu3er1p5_PFJet100er2p5_PFMET80_PFMHT80_IDTight", &HLT_Mu3er1p5_PFJet100er2p5_PFMET80_PFMHT80_IDTight, &b_HLT_Mu3er1p5_PFJet100er2p5_PFMET80_PFMHT80_IDTight);
   fChain->SetBranchAddress("HLT_Mu3er1p5_PFJet100er2p5_PFMET90_PFMHT90_IDTight", &HLT_Mu3er1p5_PFJet100er2p5_PFMET90_PFMHT90_IDTight, &b_HLT_Mu3er1p5_PFJet100er2p5_PFMET90_PFMHT90_IDTight);
   fChain->SetBranchAddress("HLT_Mu3er1p5_PFJet100er2p5_PFMET100_PFMHT100_IDTight", &HLT_Mu3er1p5_PFJet100er2p5_PFMET100_PFMHT100_IDTight, &b_HLT_Mu3er1p5_PFJet100er2p5_PFMET100_PFMHT100_IDTight);
   fChain->SetBranchAddress("HLT_Mu3er1p5_PFJet100er2p5_PFMETNoMu80_PFMHTNoMu80_IDTight", &HLT_Mu3er1p5_PFJet100er2p5_PFMETNoMu80_PFMHTNoMu80_IDTight, &b_HLT_Mu3er1p5_PFJet100er2p5_PFMETNoMu80_PFMHTNoMu80_IDTight);
   fChain->SetBranchAddress("HLT_Mu3er1p5_PFJet100er2p5_PFMETNoMu90_PFMHTNoMu90_IDTight", &HLT_Mu3er1p5_PFJet100er2p5_PFMETNoMu90_PFMHTNoMu90_IDTight, &b_HLT_Mu3er1p5_PFJet100er2p5_PFMETNoMu90_PFMHTNoMu90_IDTight);
   fChain->SetBranchAddress("HLT_Mu3er1p5_PFJet100er2p5_PFMETNoMu100_PFMHTNoMu100_IDTight", &HLT_Mu3er1p5_PFJet100er2p5_PFMETNoMu100_PFMHTNoMu100_IDTight, &b_HLT_Mu3er1p5_PFJet100er2p5_PFMETNoMu100_PFMHTNoMu100_IDTight);
   fChain->SetBranchAddress("HLT_Dimuon10_Upsilon_y1p4", &HLT_Dimuon10_Upsilon_y1p4, &b_HLT_Dimuon10_Upsilon_y1p4);
   fChain->SetBranchAddress("HLT_Dimuon12_Upsilon_y1p4", &HLT_Dimuon12_Upsilon_y1p4, &b_HLT_Dimuon12_Upsilon_y1p4);
   fChain->SetBranchAddress("HLT_Dimuon14_Phi_Barrel_Seagulls", &HLT_Dimuon14_Phi_Barrel_Seagulls, &b_HLT_Dimuon14_Phi_Barrel_Seagulls);
   fChain->SetBranchAddress("HLT_Dimuon25_Jpsi", &HLT_Dimuon25_Jpsi, &b_HLT_Dimuon25_Jpsi);
   fChain->SetBranchAddress("HLT_Dimuon14_PsiPrime", &HLT_Dimuon14_PsiPrime, &b_HLT_Dimuon14_PsiPrime);
   fChain->SetBranchAddress("HLT_Dimuon14_PsiPrime_noCorrL1", &HLT_Dimuon14_PsiPrime_noCorrL1, &b_HLT_Dimuon14_PsiPrime_noCorrL1);
   fChain->SetBranchAddress("HLT_Dimuon18_PsiPrime", &HLT_Dimuon18_PsiPrime, &b_HLT_Dimuon18_PsiPrime);
   fChain->SetBranchAddress("HLT_Dimuon18_PsiPrime_noCorrL1", &HLT_Dimuon18_PsiPrime_noCorrL1, &b_HLT_Dimuon18_PsiPrime_noCorrL1);
   fChain->SetBranchAddress("HLT_Dimuon24_Upsilon_noCorrL1", &HLT_Dimuon24_Upsilon_noCorrL1, &b_HLT_Dimuon24_Upsilon_noCorrL1);
   fChain->SetBranchAddress("HLT_Dimuon24_Phi_noCorrL1", &HLT_Dimuon24_Phi_noCorrL1, &b_HLT_Dimuon24_Phi_noCorrL1);
   fChain->SetBranchAddress("HLT_Dimuon25_Jpsi_noCorrL1", &HLT_Dimuon25_Jpsi_noCorrL1, &b_HLT_Dimuon25_Jpsi_noCorrL1);
   fChain->SetBranchAddress("HLT_DiMu4_Ele9_CaloIdL_TrackIdL_DZ_Mass3p8", &HLT_DiMu4_Ele9_CaloIdL_TrackIdL_DZ_Mass3p8, &b_HLT_DiMu4_Ele9_CaloIdL_TrackIdL_DZ_Mass3p8);
   fChain->SetBranchAddress("HLT_DiMu9_Ele9_CaloIdL_TrackIdL_DZ", &HLT_DiMu9_Ele9_CaloIdL_TrackIdL_DZ, &b_HLT_DiMu9_Ele9_CaloIdL_TrackIdL_DZ);
   fChain->SetBranchAddress("HLT_DiMu9_Ele9_CaloIdL_TrackIdL", &HLT_DiMu9_Ele9_CaloIdL_TrackIdL, &b_HLT_DiMu9_Ele9_CaloIdL_TrackIdL);
   fChain->SetBranchAddress("HLT_DoubleIsoMu20_eta2p1", &HLT_DoubleIsoMu20_eta2p1, &b_HLT_DoubleIsoMu20_eta2p1);
   fChain->SetBranchAddress("HLT_TrkMu12_DoubleTrkMu5NoFiltersNoVtx", &HLT_TrkMu12_DoubleTrkMu5NoFiltersNoVtx, &b_HLT_TrkMu12_DoubleTrkMu5NoFiltersNoVtx);
   fChain->SetBranchAddress("HLT_Mu8", &HLT_Mu8, &b_HLT_Mu8);
   fChain->SetBranchAddress("HLT_Mu17", &HLT_Mu17, &b_HLT_Mu17);
   fChain->SetBranchAddress("HLT_Mu19", &HLT_Mu19, &b_HLT_Mu19);
   fChain->SetBranchAddress("HLT_Mu17_Photon30_IsoCaloId", &HLT_Mu17_Photon30_IsoCaloId, &b_HLT_Mu17_Photon30_IsoCaloId);
   fChain->SetBranchAddress("HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30", &HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30, &b_HLT_Ele8_CaloIdL_TrackIdL_IsoVL_PFJet30);
   fChain->SetBranchAddress("HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30", &HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30, &b_HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30);
   fChain->SetBranchAddress("HLT_Ele14_eta2p5_IsoVVVL_Gsf_PFHT200_PNetBTag0p53", &HLT_Ele14_eta2p5_IsoVVVL_Gsf_PFHT200_PNetBTag0p53, &b_HLT_Ele14_eta2p5_IsoVVVL_Gsf_PFHT200_PNetBTag0p53);
   fChain->SetBranchAddress("HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30", &HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30, &b_HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30);
   fChain->SetBranchAddress("HLT_Ele8_CaloIdM_TrackIdM_PFJet30", &HLT_Ele8_CaloIdM_TrackIdM_PFJet30, &b_HLT_Ele8_CaloIdM_TrackIdM_PFJet30);
   fChain->SetBranchAddress("HLT_Ele17_CaloIdM_TrackIdM_PFJet30", &HLT_Ele17_CaloIdM_TrackIdM_PFJet30, &b_HLT_Ele17_CaloIdM_TrackIdM_PFJet30);
   fChain->SetBranchAddress("HLT_Ele23_CaloIdM_TrackIdM_PFJet30", &HLT_Ele23_CaloIdM_TrackIdM_PFJet30, &b_HLT_Ele23_CaloIdM_TrackIdM_PFJet30);
   fChain->SetBranchAddress("HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165", &HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165, &b_HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165);
   fChain->SetBranchAddress("HLT_Ele115_CaloIdVT_GsfTrkIdT", &HLT_Ele115_CaloIdVT_GsfTrkIdT, &b_HLT_Ele115_CaloIdVT_GsfTrkIdT);
   fChain->SetBranchAddress("HLT_Ele135_CaloIdVT_GsfTrkIdT", &HLT_Ele135_CaloIdVT_GsfTrkIdT, &b_HLT_Ele135_CaloIdVT_GsfTrkIdT);
   fChain->SetBranchAddress("HLT_PFHT330PT30_QuadPFJet_75_60_45_40", &HLT_PFHT330PT30_QuadPFJet_75_60_45_40, &b_HLT_PFHT330PT30_QuadPFJet_75_60_45_40);
   fChain->SetBranchAddress("HLT_PFHT400_SixPFJet32", &HLT_PFHT400_SixPFJet32, &b_HLT_PFHT400_SixPFJet32);
   fChain->SetBranchAddress("HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50", &HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50, &b_HLT_PFHT400_SixPFJet32_PNet2BTagMean0p50);
   fChain->SetBranchAddress("HLT_PFHT450_SixPFJet36", &HLT_PFHT450_SixPFJet36, &b_HLT_PFHT450_SixPFJet36);
   fChain->SetBranchAddress("HLT_PFHT450_SixPFJet36_PNetBTag0p35", &HLT_PFHT450_SixPFJet36_PNetBTag0p35, &b_HLT_PFHT450_SixPFJet36_PNetBTag0p35);
   fChain->SetBranchAddress("HLT_PFHT400_FivePFJet_120_120_60_30_30", &HLT_PFHT400_FivePFJet_120_120_60_30_30, &b_HLT_PFHT400_FivePFJet_120_120_60_30_30);
   fChain->SetBranchAddress("HLT_PFHT350", &HLT_PFHT350, &b_HLT_PFHT350);
   fChain->SetBranchAddress("HLT_Photon60_R9Id90_CaloIdL_IsoL_DisplacedIdL_PFHT350", &HLT_Photon60_R9Id90_CaloIdL_IsoL_DisplacedIdL_PFHT350, &b_HLT_Photon60_R9Id90_CaloIdL_IsoL_DisplacedIdL_PFHT350);
   fChain->SetBranchAddress("HLT_Photon60_R9Id90_CaloIdL_IsoL_DisplacedIdL_PFHT380", &HLT_Photon60_R9Id90_CaloIdL_IsoL_DisplacedIdL_PFHT380, &b_HLT_Photon60_R9Id90_CaloIdL_IsoL_DisplacedIdL_PFHT380);
   fChain->SetBranchAddress("HLT_Photon60_R9Id90_CaloIdL_IsoL_DisplacedIdL_PFHT400", &HLT_Photon60_R9Id90_CaloIdL_IsoL_DisplacedIdL_PFHT400, &b_HLT_Photon60_R9Id90_CaloIdL_IsoL_DisplacedIdL_PFHT400);
   fChain->SetBranchAddress("HLT_ECALHT800", &HLT_ECALHT800, &b_HLT_ECALHT800);
   fChain->SetBranchAddress("HLT_DiSC30_18_EIso_AND_HE_Mass70", &HLT_DiSC30_18_EIso_AND_HE_Mass70, &b_HLT_DiSC30_18_EIso_AND_HE_Mass70);
   fChain->SetBranchAddress("HLT_Photon20_HoverELoose", &HLT_Photon20_HoverELoose, &b_HLT_Photon20_HoverELoose);
   fChain->SetBranchAddress("HLT_Photon30_HoverELoose", &HLT_Photon30_HoverELoose, &b_HLT_Photon30_HoverELoose);
   fChain->SetBranchAddress("HLT_L1_CDC_SingleMu_3_er1p2_TOP120_DPHI2p618_3p142", &HLT_L1_CDC_SingleMu_3_er1p2_TOP120_DPHI2p618_3p142, &b_HLT_L1_CDC_SingleMu_3_er1p2_TOP120_DPHI2p618_3p142);
   fChain->SetBranchAddress("HLT_CDC_L2cosmic_10_er1p0", &HLT_CDC_L2cosmic_10_er1p0, &b_HLT_CDC_L2cosmic_10_er1p0);
   fChain->SetBranchAddress("HLT_CDC_L2cosmic_5p5_er1p0", &HLT_CDC_L2cosmic_5p5_er1p0, &b_HLT_CDC_L2cosmic_5p5_er1p0);
   fChain->SetBranchAddress("HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL", &HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL, &b_HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL);
   fChain->SetBranchAddress("HLT_VBF_DoubleMediumDeepTauPFTauHPS20_eta2p1", &HLT_VBF_DoubleMediumDeepTauPFTauHPS20_eta2p1, &b_HLT_VBF_DoubleMediumDeepTauPFTauHPS20_eta2p1);
   fChain->SetBranchAddress("HLT_Photon60_R9Id90_HE10_IsoM_EBOnly_PFJetsMJJ300DEta3", &HLT_Photon60_R9Id90_HE10_IsoM_EBOnly_PFJetsMJJ300DEta3, &b_HLT_Photon60_R9Id90_HE10_IsoM_EBOnly_PFJetsMJJ300DEta3);
   fChain->SetBranchAddress("HLT_Photon75_R9Id90_HE10_IsoM_EBOnly_PFJetsMJJ300DEta3", &HLT_Photon75_R9Id90_HE10_IsoM_EBOnly_PFJetsMJJ300DEta3, &b_HLT_Photon75_R9Id90_HE10_IsoM_EBOnly_PFJetsMJJ300DEta3);
   fChain->SetBranchAddress("HLT_Mu18_Mu9_SameSign", &HLT_Mu18_Mu9_SameSign, &b_HLT_Mu18_Mu9_SameSign);
   fChain->SetBranchAddress("HLT_DoubleMu2_Jpsi_DoubleTrk1_Phi1p05", &HLT_DoubleMu2_Jpsi_DoubleTrk1_Phi1p05, &b_HLT_DoubleMu2_Jpsi_DoubleTrk1_Phi1p05);
   fChain->SetBranchAddress("HLT_DoubleMu3_DCA_PFMET50_PFMHT60", &HLT_DoubleMu3_DCA_PFMET50_PFMHT60, &b_HLT_DoubleMu3_DCA_PFMET50_PFMHT60);
   fChain->SetBranchAddress("HLT_DoubleMu3_DCA_PFMET50_PFMHT60_Mass2p0", &HLT_DoubleMu3_DCA_PFMET50_PFMHT60_Mass2p0, &b_HLT_DoubleMu3_DCA_PFMET50_PFMHT60_Mass2p0);
   fChain->SetBranchAddress("HLT_DoubleMu3_DCA_PFMET50_PFMHT60_Mass2p0_noDCA", &HLT_DoubleMu3_DCA_PFMET50_PFMHT60_Mass2p0_noDCA, &b_HLT_DoubleMu3_DCA_PFMET50_PFMHT60_Mass2p0_noDCA);
   fChain->SetBranchAddress("HLT_TripleMu_5_3_3_Mass3p8_DCA", &HLT_TripleMu_5_3_3_Mass3p8_DCA, &b_HLT_TripleMu_5_3_3_Mass3p8_DCA);
   fChain->SetBranchAddress("HLT_QuadPFJet103_88_75_15", &HLT_QuadPFJet103_88_75_15, &b_HLT_QuadPFJet103_88_75_15);
   fChain->SetBranchAddress("HLT_QuadPFJet105_88_76_15", &HLT_QuadPFJet105_88_76_15, &b_HLT_QuadPFJet105_88_76_15);
   fChain->SetBranchAddress("HLT_QuadPFJet111_90_80_15", &HLT_QuadPFJet111_90_80_15, &b_HLT_QuadPFJet111_90_80_15);
   fChain->SetBranchAddress("HLT_Diphoton30_18_R9IdL_AND_HE_AND_IsoCaloId", &HLT_Diphoton30_18_R9IdL_AND_HE_AND_IsoCaloId, &b_HLT_Diphoton30_18_R9IdL_AND_HE_AND_IsoCaloId);
   fChain->SetBranchAddress("HLT_Diphoton30_18_R9IdL_AND_HE_AND_IsoCaloId_Mass55", &HLT_Diphoton30_18_R9IdL_AND_HE_AND_IsoCaloId_Mass55, &b_HLT_Diphoton30_18_R9IdL_AND_HE_AND_IsoCaloId_Mass55);
   fChain->SetBranchAddress("HLT_DoubleMediumDeepTauPFTauHPS35_L2NN_eta2p1", &HLT_DoubleMediumDeepTauPFTauHPS35_L2NN_eta2p1, &b_HLT_DoubleMediumDeepTauPFTauHPS35_L2NN_eta2p1);
   fChain->SetBranchAddress("HLT_Ele24_eta2p1_WPTight_Gsf_LooseDeepTauPFTauHPS30_eta2p1_CrossL1", &HLT_Ele24_eta2p1_WPTight_Gsf_LooseDeepTauPFTauHPS30_eta2p1_CrossL1, &b_HLT_Ele24_eta2p1_WPTight_Gsf_LooseDeepTauPFTauHPS30_eta2p1_CrossL1);
   fChain->SetBranchAddress("HLT_Ele24_eta2p1_WPTight_Gsf_PNetTauhPFJet30_Tight_eta2p3_CrossL1", &HLT_Ele24_eta2p1_WPTight_Gsf_PNetTauhPFJet30_Tight_eta2p3_CrossL1, &b_HLT_Ele24_eta2p1_WPTight_Gsf_PNetTauhPFJet30_Tight_eta2p3_CrossL1);
   fChain->SetBranchAddress("HLT_Ele24_eta2p1_WPTight_Gsf_PNetTauhPFJet30_Medium_eta2p3_CrossL1", &HLT_Ele24_eta2p1_WPTight_Gsf_PNetTauhPFJet30_Medium_eta2p3_CrossL1, &b_HLT_Ele24_eta2p1_WPTight_Gsf_PNetTauhPFJet30_Medium_eta2p3_CrossL1);
   fChain->SetBranchAddress("HLT_Ele24_eta2p1_WPTight_Gsf_PNetTauhPFJet30_Loose_eta2p3_CrossL1", &HLT_Ele24_eta2p1_WPTight_Gsf_PNetTauhPFJet30_Loose_eta2p3_CrossL1, &b_HLT_Ele24_eta2p1_WPTight_Gsf_PNetTauhPFJet30_Loose_eta2p3_CrossL1);
   fChain->SetBranchAddress("HLT_IsoMu20_eta2p1_LooseDeepTauPFTauHPS27_eta2p1_CrossL1", &HLT_IsoMu20_eta2p1_LooseDeepTauPFTauHPS27_eta2p1_CrossL1, &b_HLT_IsoMu20_eta2p1_LooseDeepTauPFTauHPS27_eta2p1_CrossL1);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_LooseDeepTauPFTauHPS180_eta2p1", &HLT_IsoMu24_eta2p1_LooseDeepTauPFTauHPS180_eta2p1, &b_HLT_IsoMu24_eta2p1_LooseDeepTauPFTauHPS180_eta2p1);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_LooseDeepTauPFTauHPS30_eta2p1_CrossL1", &HLT_IsoMu24_eta2p1_LooseDeepTauPFTauHPS30_eta2p1_CrossL1, &b_HLT_IsoMu24_eta2p1_LooseDeepTauPFTauHPS30_eta2p1_CrossL1);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS35_L2NN_eta2p1_CrossL1", &HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS35_L2NN_eta2p1_CrossL1, &b_HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS35_L2NN_eta2p1_CrossL1);
   fChain->SetBranchAddress("HLT_LooseDeepTauPFTauHPS180_L2NN_eta2p1", &HLT_LooseDeepTauPFTauHPS180_L2NN_eta2p1, &b_HLT_LooseDeepTauPFTauHPS180_L2NN_eta2p1);
   fChain->SetBranchAddress("HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5", &HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5, &b_HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepJet_4p5);
   fChain->SetBranchAddress("HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3", &HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3, &b_HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_4p3);
   fChain->SetBranchAddress("HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_2p0", &HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_2p0, &b_HLT_PFHT330PT30_QuadPFJet_75_60_45_40_PNet3BTag_2p0);
   fChain->SetBranchAddress("HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_5p6", &HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_5p6, &b_HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_5p6);
   fChain->SetBranchAddress("HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_4p3", &HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_4p3, &b_HLT_PFHT400_FivePFJet_120_120_60_30_30_PNet2BTag_4p3);
   fChain->SetBranchAddress("HLT_QuadPFJet103_88_75_15_PNetBTag_0p4_VBF2", &HLT_QuadPFJet103_88_75_15_PNetBTag_0p4_VBF2, &b_HLT_QuadPFJet103_88_75_15_PNetBTag_0p4_VBF2);
   fChain->SetBranchAddress("HLT_QuadPFJet103_88_75_15_PNet2BTag_0p4_0p12_VBF1", &HLT_QuadPFJet103_88_75_15_PNet2BTag_0p4_0p12_VBF1, &b_HLT_QuadPFJet103_88_75_15_PNet2BTag_0p4_0p12_VBF1);
   fChain->SetBranchAddress("HLT_QuadPFJet105_88_76_15_PNetBTag_0p4_VBF2", &HLT_QuadPFJet105_88_76_15_PNetBTag_0p4_VBF2, &b_HLT_QuadPFJet105_88_76_15_PNetBTag_0p4_VBF2);
   fChain->SetBranchAddress("HLT_QuadPFJet105_88_76_15_PNet2BTag_0p4_0p12_VBF1", &HLT_QuadPFJet105_88_76_15_PNet2BTag_0p4_0p12_VBF1, &b_HLT_QuadPFJet105_88_76_15_PNet2BTag_0p4_0p12_VBF1);
   fChain->SetBranchAddress("HLT_QuadPFJet111_90_80_15_PNetBTag_0p4_VBF2", &HLT_QuadPFJet111_90_80_15_PNetBTag_0p4_VBF2, &b_HLT_QuadPFJet111_90_80_15_PNetBTag_0p4_VBF2);
   fChain->SetBranchAddress("HLT_QuadPFJet111_90_80_15_PNet2BTag_0p4_0p12_VBF1", &HLT_QuadPFJet111_90_80_15_PNet2BTag_0p4_0p12_VBF1, &b_HLT_QuadPFJet111_90_80_15_PNet2BTag_0p4_0p12_VBF1);
   fChain->SetBranchAddress("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30_PFBtagDeepJet_1p5", &HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30_PFBtagDeepJet_1p5, &b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30_PFBtagDeepJet_1p5);
   fChain->SetBranchAddress("HLT_PFHT250_QuadPFJet25", &HLT_PFHT250_QuadPFJet25, &b_HLT_PFHT250_QuadPFJet25);
   fChain->SetBranchAddress("HLT_PFHT250_QuadPFJet25_PNet2BTagMean0p55", &HLT_PFHT250_QuadPFJet25_PNet2BTagMean0p55, &b_HLT_PFHT250_QuadPFJet25_PNet2BTagMean0p55);
   fChain->SetBranchAddress("HLT_PFHT250_QuadPFJet25_PNet1BTag0p20_PNet1Tauh0p50", &HLT_PFHT250_QuadPFJet25_PNet1BTag0p20_PNet1Tauh0p50, &b_HLT_PFHT250_QuadPFJet25_PNet1BTag0p20_PNet1Tauh0p50);
   fChain->SetBranchAddress("HLT_PFHT250_QuadPFJet30_PNet2BTagMean0p55", &HLT_PFHT250_QuadPFJet30_PNet2BTagMean0p55, &b_HLT_PFHT250_QuadPFJet30_PNet2BTagMean0p55);
   fChain->SetBranchAddress("HLT_PFHT250_QuadPFJet30_PNet1BTag0p20_PNet1Tauh0p50", &HLT_PFHT250_QuadPFJet30_PNet1BTag0p20_PNet1Tauh0p50, &b_HLT_PFHT250_QuadPFJet30_PNet1BTag0p20_PNet1Tauh0p50);
   fChain->SetBranchAddress("HLT_PFHT280_QuadPFJet30_PNet1BTag0p20_PNet1Tauh0p50", &HLT_PFHT280_QuadPFJet30_PNet1BTag0p20_PNet1Tauh0p50, &b_HLT_PFHT280_QuadPFJet30_PNet1BTag0p20_PNet1Tauh0p50);
   fChain->SetBranchAddress("HLT_PFHT280_QuadPFJet30", &HLT_PFHT280_QuadPFJet30, &b_HLT_PFHT280_QuadPFJet30);
   fChain->SetBranchAddress("HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p55", &HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p55, &b_HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p55);
   fChain->SetBranchAddress("HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p60", &HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p60, &b_HLT_PFHT280_QuadPFJet30_PNet2BTagMean0p60);
   fChain->SetBranchAddress("HLT_PFHT280_QuadPFJet35_PNet2BTagMean0p60", &HLT_PFHT280_QuadPFJet35_PNet2BTagMean0p60, &b_HLT_PFHT280_QuadPFJet35_PNet2BTagMean0p60);
   fChain->SetBranchAddress("HLT_PFHT340_QuadPFJet70_50_40_40_PNet2BTagMean0p70", &HLT_PFHT340_QuadPFJet70_50_40_40_PNet2BTagMean0p70, &b_HLT_PFHT340_QuadPFJet70_50_40_40_PNet2BTagMean0p70);
   fChain->SetBranchAddress("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT280", &HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT280, &b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT280);
   fChain->SetBranchAddress("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT280_QuadPFJet30", &HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT280_QuadPFJet30, &b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT280_QuadPFJet30);
   fChain->SetBranchAddress("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT280_QuadPFJet30_PNet2BTagMean0p55", &HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT280_QuadPFJet30_PNet2BTagMean0p55, &b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT280_QuadPFJet30_PNet2BTagMean0p55);
   fChain->SetBranchAddress("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30_PNet2BTagMean0p50", &HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30_PNet2BTagMean0p50, &b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFDiJet30_PNet2BTagMean0p50);
   fChain->SetBranchAddress("HLT_QuadPFJet100_88_70_30", &HLT_QuadPFJet100_88_70_30, &b_HLT_QuadPFJet100_88_70_30);
   fChain->SetBranchAddress("HLT_QuadPFJet105_88_75_30", &HLT_QuadPFJet105_88_75_30, &b_HLT_QuadPFJet105_88_75_30);
   fChain->SetBranchAddress("HLT_QuadPFJet111_90_80_30", &HLT_QuadPFJet111_90_80_30, &b_HLT_QuadPFJet111_90_80_30);
   fChain->SetBranchAddress("HLT_QuadPFJet100_88_70_30_PNet1CvsAll0p5_VBF3Tight", &HLT_QuadPFJet100_88_70_30_PNet1CvsAll0p5_VBF3Tight, &b_HLT_QuadPFJet100_88_70_30_PNet1CvsAll0p5_VBF3Tight);
   fChain->SetBranchAddress("HLT_QuadPFJet105_88_75_30_PNet1CvsAll0p5_VBF3Tight", &HLT_QuadPFJet105_88_75_30_PNet1CvsAll0p5_VBF3Tight, &b_HLT_QuadPFJet105_88_75_30_PNet1CvsAll0p5_VBF3Tight);
   fChain->SetBranchAddress("HLT_QuadPFJet111_90_80_30_PNet1CvsAll0p6_VBF3Tight", &HLT_QuadPFJet111_90_80_30_PNet1CvsAll0p6_VBF3Tight, &b_HLT_QuadPFJet111_90_80_30_PNet1CvsAll0p6_VBF3Tight);
   fChain->SetBranchAddress("HLT_AK8PFJet220_SoftDropMass40", &HLT_AK8PFJet220_SoftDropMass40, &b_HLT_AK8PFJet220_SoftDropMass40);
   fChain->SetBranchAddress("HLT_AK8PFJet220_SoftDropMass40_PNetBB0p06_DoubleAK4PFJet60_30_PNet2BTagMean0p50", &HLT_AK8PFJet220_SoftDropMass40_PNetBB0p06_DoubleAK4PFJet60_30_PNet2BTagMean0p50, &b_HLT_AK8PFJet220_SoftDropMass40_PNetBB0p06_DoubleAK4PFJet60_30_PNet2BTagMean0p50);
   fChain->SetBranchAddress("HLT_AK8PFJet220_SoftDropMass40_PNetBB0p06_DoubleAK4PFJet60_30_PNet2BTagMean0p53", &HLT_AK8PFJet220_SoftDropMass40_PNetBB0p06_DoubleAK4PFJet60_30_PNet2BTagMean0p53, &b_HLT_AK8PFJet220_SoftDropMass40_PNetBB0p06_DoubleAK4PFJet60_30_PNet2BTagMean0p53);
   fChain->SetBranchAddress("HLT_AK8PFJet220_SoftDropMass40_PNetBB0p06_DoubleAK4PFJet60_30_PNet2BTagMean0p55", &HLT_AK8PFJet220_SoftDropMass40_PNetBB0p06_DoubleAK4PFJet60_30_PNet2BTagMean0p55, &b_HLT_AK8PFJet220_SoftDropMass40_PNetBB0p06_DoubleAK4PFJet60_30_PNet2BTagMean0p55);
   fChain->SetBranchAddress("HLT_AK8PFJet220_SoftDropMass40_PNetBB0p06_DoubleAK4PFJet60_30_PNet2BTagMean0p60", &HLT_AK8PFJet220_SoftDropMass40_PNetBB0p06_DoubleAK4PFJet60_30_PNet2BTagMean0p60, &b_HLT_AK8PFJet220_SoftDropMass40_PNetBB0p06_DoubleAK4PFJet60_30_PNet2BTagMean0p60);
   fChain->SetBranchAddress("HLT_AK8PFJet230_SoftDropMass40", &HLT_AK8PFJet230_SoftDropMass40, &b_HLT_AK8PFJet230_SoftDropMass40);
   fChain->SetBranchAddress("HLT_AK8PFJet230_SoftDropMass40_PNetBB0p06", &HLT_AK8PFJet230_SoftDropMass40_PNetBB0p06, &b_HLT_AK8PFJet230_SoftDropMass40_PNetBB0p06);
   fChain->SetBranchAddress("HLT_AK8PFJet230_SoftDropMass40_PNetBB0p10", &HLT_AK8PFJet230_SoftDropMass40_PNetBB0p10, &b_HLT_AK8PFJet230_SoftDropMass40_PNetBB0p10);
   fChain->SetBranchAddress("HLT_AK8PFJet230_SoftDropMass40_PNetTauTau0p03", &HLT_AK8PFJet230_SoftDropMass40_PNetTauTau0p03, &b_HLT_AK8PFJet230_SoftDropMass40_PNetTauTau0p03);
   fChain->SetBranchAddress("HLT_AK8PFJet230_SoftDropMass40_PNetTauTau0p05", &HLT_AK8PFJet230_SoftDropMass40_PNetTauTau0p05, &b_HLT_AK8PFJet230_SoftDropMass40_PNetTauTau0p05);
   fChain->SetBranchAddress("HLT_AK8PFJet250_SoftDropMass40_PNetBB0p06", &HLT_AK8PFJet250_SoftDropMass40_PNetBB0p06, &b_HLT_AK8PFJet250_SoftDropMass40_PNetBB0p06);
   fChain->SetBranchAddress("HLT_AK8PFJet250_SoftDropMass40_PNetBB0p10", &HLT_AK8PFJet250_SoftDropMass40_PNetBB0p10, &b_HLT_AK8PFJet250_SoftDropMass40_PNetBB0p10);
   fChain->SetBranchAddress("HLT_AK8PFJet250_SoftDropMass40_PNetTauTau0p03", &HLT_AK8PFJet250_SoftDropMass40_PNetTauTau0p03, &b_HLT_AK8PFJet250_SoftDropMass40_PNetTauTau0p03);
   fChain->SetBranchAddress("HLT_AK8PFJet250_SoftDropMass40_PNetTauTau0p05", &HLT_AK8PFJet250_SoftDropMass40_PNetTauTau0p05, &b_HLT_AK8PFJet250_SoftDropMass40_PNetTauTau0p05);
   fChain->SetBranchAddress("HLT_AK8PFJet275_SoftDropMass40_PNetBB0p06", &HLT_AK8PFJet275_SoftDropMass40_PNetBB0p06, &b_HLT_AK8PFJet275_SoftDropMass40_PNetBB0p06);
   fChain->SetBranchAddress("HLT_AK8PFJet275_SoftDropMass40_PNetBB0p10", &HLT_AK8PFJet275_SoftDropMass40_PNetBB0p10, &b_HLT_AK8PFJet275_SoftDropMass40_PNetBB0p10);
   fChain->SetBranchAddress("HLT_AK8PFJet275_SoftDropMass40_PNetTauTau0p03", &HLT_AK8PFJet275_SoftDropMass40_PNetTauTau0p03, &b_HLT_AK8PFJet275_SoftDropMass40_PNetTauTau0p03);
   fChain->SetBranchAddress("HLT_AK8PFJet275_SoftDropMass40_PNetTauTau0p05", &HLT_AK8PFJet275_SoftDropMass40_PNetTauTau0p05, &b_HLT_AK8PFJet275_SoftDropMass40_PNetTauTau0p05);
   fChain->SetBranchAddress("HLT_AK8PFJet275_Nch45", &HLT_AK8PFJet275_Nch45, &b_HLT_AK8PFJet275_Nch45);
   fChain->SetBranchAddress("HLT_AK8PFJet275_Nch40", &HLT_AK8PFJet275_Nch40, &b_HLT_AK8PFJet275_Nch40);
   fChain->SetBranchAddress("HLT_IsoMu50_AK8PFJet220_SoftDropMass40", &HLT_IsoMu50_AK8PFJet220_SoftDropMass40, &b_HLT_IsoMu50_AK8PFJet220_SoftDropMass40);
   fChain->SetBranchAddress("HLT_IsoMu50_AK8PFJet220_SoftDropMass40_PNetBB0p06", &HLT_IsoMu50_AK8PFJet220_SoftDropMass40_PNetBB0p06, &b_HLT_IsoMu50_AK8PFJet220_SoftDropMass40_PNetBB0p06);
   fChain->SetBranchAddress("HLT_IsoMu50_AK8PFJet230_SoftDropMass40", &HLT_IsoMu50_AK8PFJet230_SoftDropMass40, &b_HLT_IsoMu50_AK8PFJet230_SoftDropMass40);
   fChain->SetBranchAddress("HLT_IsoMu50_AK8PFJet230_SoftDropMass40_PNetBB0p06", &HLT_IsoMu50_AK8PFJet230_SoftDropMass40_PNetBB0p06, &b_HLT_IsoMu50_AK8PFJet230_SoftDropMass40_PNetBB0p06);
   fChain->SetBranchAddress("HLT_IsoMu50_AK8PFJet230_SoftDropMass40_PNetBB0p10", &HLT_IsoMu50_AK8PFJet230_SoftDropMass40_PNetBB0p10, &b_HLT_IsoMu50_AK8PFJet230_SoftDropMass40_PNetBB0p10);
   fChain->SetBranchAddress("HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet220_SoftDropMass40", &HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet220_SoftDropMass40, &b_HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet220_SoftDropMass40);
   fChain->SetBranchAddress("HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet220_SoftDropMass40_PNetBB0p06", &HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet220_SoftDropMass40_PNetBB0p06, &b_HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet220_SoftDropMass40_PNetBB0p06);
   fChain->SetBranchAddress("HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet230_SoftDropMass40", &HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet230_SoftDropMass40, &b_HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet230_SoftDropMass40);
   fChain->SetBranchAddress("HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet230_SoftDropMass40_PNetBB0p06", &HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet230_SoftDropMass40_PNetBB0p06, &b_HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet230_SoftDropMass40_PNetBB0p06);
   fChain->SetBranchAddress("HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet230_SoftDropMass40_PNetBB0p10", &HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet230_SoftDropMass40_PNetBB0p10, &b_HLT_Ele50_CaloIdVT_GsfTrkIdT_AK8PFJet230_SoftDropMass40_PNetBB0p10);
   fChain->SetBranchAddress("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_DoubleAK4PFJet60_30", &HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_DoubleAK4PFJet60_30, &b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_DoubleAK4PFJet60_30);
   fChain->SetBranchAddress("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_DoubleAK4PFJet60_30_PNet2BTagMean0p50", &HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_DoubleAK4PFJet60_30_PNet2BTagMean0p50, &b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_DoubleAK4PFJet60_30_PNet2BTagMean0p50);
   fChain->SetBranchAddress("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT250", &HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT250, &b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT250);
   fChain->SetBranchAddress("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT250_QuadPFJet25", &HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT250_QuadPFJet25, &b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT250_QuadPFJet25);
   fChain->SetBranchAddress("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT250_QuadPFJet25_PNet2BTagMean0p55", &HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT250_QuadPFJet25_PNet2BTagMean0p55, &b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT250_QuadPFJet25_PNet2BTagMean0p55);
   fChain->SetBranchAddress("HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT250_QuadPFJet25_PNet1BTag0p20", &HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT250_QuadPFJet25_PNet1BTag0p20, &b_HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_PFHT250_QuadPFJet25_PNet1BTag0p20);
   fChain->SetBranchAddress("HLT_DoubleMediumDeepTauPFTauHPS30_L2NN_eta2p1_PFJet60", &HLT_DoubleMediumDeepTauPFTauHPS30_L2NN_eta2p1_PFJet60, &b_HLT_DoubleMediumDeepTauPFTauHPS30_L2NN_eta2p1_PFJet60);
   fChain->SetBranchAddress("HLT_DoubleMediumDeepTauPFTauHPS30_L2NN_eta2p1_PFJet75", &HLT_DoubleMediumDeepTauPFTauHPS30_L2NN_eta2p1_PFJet75, &b_HLT_DoubleMediumDeepTauPFTauHPS30_L2NN_eta2p1_PFJet75);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS30_L2NN_eta2p1_CrossL1", &HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS30_L2NN_eta2p1_CrossL1, &b_HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS30_L2NN_eta2p1_CrossL1);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS30_L2NN_eta2p1_PFJet60_CrossL1", &HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS30_L2NN_eta2p1_PFJet60_CrossL1, &b_HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS30_L2NN_eta2p1_PFJet60_CrossL1);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS30_L2NN_eta2p1_PFJet75_CrossL1", &HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS30_L2NN_eta2p1_PFJet75_CrossL1, &b_HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS30_L2NN_eta2p1_PFJet75_CrossL1);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS20_eta2p1_SingleL1", &HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS20_eta2p1_SingleL1, &b_HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS20_eta2p1_SingleL1);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS45_L2NN_eta2p1_CrossL1", &HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS45_L2NN_eta2p1_CrossL1, &b_HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS45_L2NN_eta2p1_CrossL1);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Medium_L2NN_eta2p3_CrossL1", &HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Medium_L2NN_eta2p3_CrossL1, &b_HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Medium_L2NN_eta2p3_CrossL1);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Tight_L2NN_eta2p3_CrossL1", &HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Tight_L2NN_eta2p3_CrossL1, &b_HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Tight_L2NN_eta2p3_CrossL1);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_PNetTauhPFJet26_L2NN_eta2p3_CrossL1", &HLT_IsoMu24_eta2p1_PNetTauhPFJet26_L2NN_eta2p3_CrossL1, &b_HLT_IsoMu24_eta2p1_PNetTauhPFJet26_L2NN_eta2p3_CrossL1);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_PNetTauhPFJet26_L2NN_eta2p3_CrossL1_PFJet60", &HLT_IsoMu24_eta2p1_PNetTauhPFJet26_L2NN_eta2p3_CrossL1_PFJet60, &b_HLT_IsoMu24_eta2p1_PNetTauhPFJet26_L2NN_eta2p3_CrossL1_PFJet60);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_PNetTauhPFJet26_L2NN_eta2p3_CrossL1_PFJet75", &HLT_IsoMu24_eta2p1_PNetTauhPFJet26_L2NN_eta2p3_CrossL1_PFJet75, &b_HLT_IsoMu24_eta2p1_PNetTauhPFJet26_L2NN_eta2p3_CrossL1_PFJet75);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Tight_eta2p3_CrossL1_ETau_Monitoring", &HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Tight_eta2p3_CrossL1_ETau_Monitoring, &b_HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Tight_eta2p3_CrossL1_ETau_Monitoring);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Medium_eta2p3_CrossL1_ETau_Monitoring", &HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Medium_eta2p3_CrossL1_ETau_Monitoring, &b_HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Medium_eta2p3_CrossL1_ETau_Monitoring);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Loose_eta2p3_CrossL1_ETau_Monitoring", &HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Loose_eta2p3_CrossL1_ETau_Monitoring, &b_HLT_IsoMu24_eta2p1_PNetTauhPFJet30_Loose_eta2p3_CrossL1_ETau_Monitoring);
   fChain->SetBranchAddress("HLT_IsoMu20_eta2p1_PNetTauhPFJet27_Tight_eta2p3_CrossL1", &HLT_IsoMu20_eta2p1_PNetTauhPFJet27_Tight_eta2p3_CrossL1, &b_HLT_IsoMu20_eta2p1_PNetTauhPFJet27_Tight_eta2p3_CrossL1);
   fChain->SetBranchAddress("HLT_IsoMu20_eta2p1_PNetTauhPFJet27_Medium_eta2p3_CrossL1", &HLT_IsoMu20_eta2p1_PNetTauhPFJet27_Medium_eta2p3_CrossL1, &b_HLT_IsoMu20_eta2p1_PNetTauhPFJet27_Medium_eta2p3_CrossL1);
   fChain->SetBranchAddress("HLT_IsoMu20_eta2p1_PNetTauhPFJet27_Loose_eta2p3_CrossL1", &HLT_IsoMu20_eta2p1_PNetTauhPFJet27_Loose_eta2p3_CrossL1, &b_HLT_IsoMu20_eta2p1_PNetTauhPFJet27_Loose_eta2p3_CrossL1);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_PNetTauhPFJet20_eta2p2_SingleL1", &HLT_IsoMu24_eta2p1_PNetTauhPFJet20_eta2p2_SingleL1, &b_HLT_IsoMu24_eta2p1_PNetTauhPFJet20_eta2p2_SingleL1);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_PNetTauhPFJet45_L2NN_eta2p3_CrossL1", &HLT_IsoMu24_eta2p1_PNetTauhPFJet45_L2NN_eta2p3_CrossL1, &b_HLT_IsoMu24_eta2p1_PNetTauhPFJet45_L2NN_eta2p3_CrossL1);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_PFHT250", &HLT_IsoMu24_eta2p1_PFHT250, &b_HLT_IsoMu24_eta2p1_PFHT250);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_PFHT250_QuadPFJet25", &HLT_IsoMu24_eta2p1_PFHT250_QuadPFJet25, &b_HLT_IsoMu24_eta2p1_PFHT250_QuadPFJet25);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_PFHT250_QuadPFJet25_PNet1Tauh0p50", &HLT_IsoMu24_eta2p1_PFHT250_QuadPFJet25_PNet1Tauh0p50, &b_HLT_IsoMu24_eta2p1_PFHT250_QuadPFJet25_PNet1Tauh0p50);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_SinglePFJet25_PNet1Tauh0p50", &HLT_IsoMu24_eta2p1_SinglePFJet25_PNet1Tauh0p50, &b_HLT_IsoMu24_eta2p1_SinglePFJet25_PNet1Tauh0p50);
   fChain->SetBranchAddress("HLT_DoublePNetTauhPFJet30_Medium_L2NN_eta2p3", &HLT_DoublePNetTauhPFJet30_Medium_L2NN_eta2p3, &b_HLT_DoublePNetTauhPFJet30_Medium_L2NN_eta2p3);
   fChain->SetBranchAddress("HLT_DoublePNetTauhPFJet30_Tight_L2NN_eta2p3", &HLT_DoublePNetTauhPFJet30_Tight_L2NN_eta2p3, &b_HLT_DoublePNetTauhPFJet30_Tight_L2NN_eta2p3);
   fChain->SetBranchAddress("HLT_DoublePNetTauhPFJet26_L2NN_eta2p3_PFJet60", &HLT_DoublePNetTauhPFJet26_L2NN_eta2p3_PFJet60, &b_HLT_DoublePNetTauhPFJet26_L2NN_eta2p3_PFJet60);
   fChain->SetBranchAddress("HLT_DoublePNetTauhPFJet26_L2NN_eta2p3_PFJet75", &HLT_DoublePNetTauhPFJet26_L2NN_eta2p3_PFJet75, &b_HLT_DoublePNetTauhPFJet26_L2NN_eta2p3_PFJet75);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_PNetTauhPFJet130_Loose_L2NN_eta2p3_CrossL1", &HLT_IsoMu24_eta2p1_PNetTauhPFJet130_Loose_L2NN_eta2p3_CrossL1, &b_HLT_IsoMu24_eta2p1_PNetTauhPFJet130_Loose_L2NN_eta2p3_CrossL1);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_PNetTauhPFJet130_Medium_L2NN_eta2p3_CrossL1", &HLT_IsoMu24_eta2p1_PNetTauhPFJet130_Medium_L2NN_eta2p3_CrossL1, &b_HLT_IsoMu24_eta2p1_PNetTauhPFJet130_Medium_L2NN_eta2p3_CrossL1);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_PNetTauhPFJet130_Tight_L2NN_eta2p3_CrossL1", &HLT_IsoMu24_eta2p1_PNetTauhPFJet130_Tight_L2NN_eta2p3_CrossL1, &b_HLT_IsoMu24_eta2p1_PNetTauhPFJet130_Tight_L2NN_eta2p3_CrossL1);
   fChain->SetBranchAddress("HLT_SinglePNetTauhPFJet130_Loose_L2NN_eta2p3", &HLT_SinglePNetTauhPFJet130_Loose_L2NN_eta2p3, &b_HLT_SinglePNetTauhPFJet130_Loose_L2NN_eta2p3);
   fChain->SetBranchAddress("HLT_SinglePNetTauhPFJet130_Medium_L2NN_eta2p3", &HLT_SinglePNetTauhPFJet130_Medium_L2NN_eta2p3, &b_HLT_SinglePNetTauhPFJet130_Medium_L2NN_eta2p3);
   fChain->SetBranchAddress("HLT_SinglePNetTauhPFJet130_Tight_L2NN_eta2p3", &HLT_SinglePNetTauhPFJet130_Tight_L2NN_eta2p3, &b_HLT_SinglePNetTauhPFJet130_Tight_L2NN_eta2p3);
   fChain->SetBranchAddress("HLT_DoubleL2Mu10NoVtx_2Cha_VetoL3Mu0DxyMax1cm", &HLT_DoubleL2Mu10NoVtx_2Cha_VetoL3Mu0DxyMax1cm, &b_HLT_DoubleL2Mu10NoVtx_2Cha_VetoL3Mu0DxyMax1cm);
   fChain->SetBranchAddress("HLT_DoubleL2Mu12NoVtx_2Cha_VetoL3Mu0DxyMax1cm", &HLT_DoubleL2Mu12NoVtx_2Cha_VetoL3Mu0DxyMax1cm, &b_HLT_DoubleL2Mu12NoVtx_2Cha_VetoL3Mu0DxyMax1cm);
   fChain->SetBranchAddress("HLT_DoubleL2Mu14NoVtx_2Cha_VetoL3Mu0DxyMax1cm", &HLT_DoubleL2Mu14NoVtx_2Cha_VetoL3Mu0DxyMax1cm, &b_HLT_DoubleL2Mu14NoVtx_2Cha_VetoL3Mu0DxyMax1cm);
   fChain->SetBranchAddress("HLT_DoubleL3Mu16_10NoVtx_DxyMin0p01cm", &HLT_DoubleL3Mu16_10NoVtx_DxyMin0p01cm, &b_HLT_DoubleL3Mu16_10NoVtx_DxyMin0p01cm);
   fChain->SetBranchAddress("HLT_DoubleL3Mu18_10NoVtx_DxyMin0p01cm", &HLT_DoubleL3Mu18_10NoVtx_DxyMin0p01cm, &b_HLT_DoubleL3Mu18_10NoVtx_DxyMin0p01cm);
   fChain->SetBranchAddress("HLT_DoubleL3Mu20_10NoVtx_DxyMin0p01cm", &HLT_DoubleL3Mu20_10NoVtx_DxyMin0p01cm, &b_HLT_DoubleL3Mu20_10NoVtx_DxyMin0p01cm);
   fChain->SetBranchAddress("HLT_L2Mu10NoVtx_2Cha", &HLT_L2Mu10NoVtx_2Cha, &b_HLT_L2Mu10NoVtx_2Cha);
   fChain->SetBranchAddress("HLT_L2Mu10NoVtx_2Cha_VetoL3Mu0DxyMax1cm", &HLT_L2Mu10NoVtx_2Cha_VetoL3Mu0DxyMax1cm, &b_HLT_L2Mu10NoVtx_2Cha_VetoL3Mu0DxyMax1cm);
   fChain->SetBranchAddress("HLT_L3Mu10NoVtx", &HLT_L3Mu10NoVtx, &b_HLT_L3Mu10NoVtx);
   fChain->SetBranchAddress("HLT_L3Mu10NoVtx_DxyMin0p01cm", &HLT_L3Mu10NoVtx_DxyMin0p01cm, &b_HLT_L3Mu10NoVtx_DxyMin0p01cm);
   fChain->SetBranchAddress("HLT_DoubleL2Mu_L3Mu16NoVtx_VetoL3Mu0DxyMax0p1cm", &HLT_DoubleL2Mu_L3Mu16NoVtx_VetoL3Mu0DxyMax0p1cm, &b_HLT_DoubleL2Mu_L3Mu16NoVtx_VetoL3Mu0DxyMax0p1cm);
   fChain->SetBranchAddress("HLT_DoubleL2Mu_L3Mu18NoVtx_VetoL3Mu0DxyMax0p1cm", &HLT_DoubleL2Mu_L3Mu18NoVtx_VetoL3Mu0DxyMax0p1cm, &b_HLT_DoubleL2Mu_L3Mu18NoVtx_VetoL3Mu0DxyMax0p1cm);
   fChain->SetBranchAddress("HLT_DoubleL2Mu10NoVtx_2Cha_CosmicSeed_VetoL3Mu0DxyMax1cm", &HLT_DoubleL2Mu10NoVtx_2Cha_CosmicSeed_VetoL3Mu0DxyMax1cm, &b_HLT_DoubleL2Mu10NoVtx_2Cha_CosmicSeed_VetoL3Mu0DxyMax1cm);
   fChain->SetBranchAddress("HLT_DoubleL2Mu12NoVtx_2Cha_CosmicSeed_VetoL3Mu0DxyMax1cm", &HLT_DoubleL2Mu12NoVtx_2Cha_CosmicSeed_VetoL3Mu0DxyMax1cm, &b_HLT_DoubleL2Mu12NoVtx_2Cha_CosmicSeed_VetoL3Mu0DxyMax1cm);
   fChain->SetBranchAddress("HLT_L2Mu10NoVtx_2Cha_CosmicSeed", &HLT_L2Mu10NoVtx_2Cha_CosmicSeed, &b_HLT_L2Mu10NoVtx_2Cha_CosmicSeed);
   fChain->SetBranchAddress("HLT_L2Mu10NoVtx_2Cha_CosmicSeed_VetoL3Mu0DxyMax1cm", &HLT_L2Mu10NoVtx_2Cha_CosmicSeed_VetoL3Mu0DxyMax1cm, &b_HLT_L2Mu10NoVtx_2Cha_CosmicSeed_VetoL3Mu0DxyMax1cm);
   fChain->SetBranchAddress("HLT_DoubleL3dTksMu16_10NoVtx_DxyMin0p01cm", &HLT_DoubleL3dTksMu16_10NoVtx_DxyMin0p01cm, &b_HLT_DoubleL3dTksMu16_10NoVtx_DxyMin0p01cm);
   fChain->SetBranchAddress("HLT_L2Mu50NoVtx_3Cha_CosmicSeed_VetoL3Mu0DxyMax1cm", &HLT_L2Mu50NoVtx_3Cha_CosmicSeed_VetoL3Mu0DxyMax1cm, &b_HLT_L2Mu50NoVtx_3Cha_CosmicSeed_VetoL3Mu0DxyMax1cm);
   fChain->SetBranchAddress("HLT_L2Mu50NoVtx_3Cha_VetoL3Mu0DxyMax1cm", &HLT_L2Mu50NoVtx_3Cha_VetoL3Mu0DxyMax1cm, &b_HLT_L2Mu50NoVtx_3Cha_VetoL3Mu0DxyMax1cm);
   fChain->SetBranchAddress("HLT_L3Mu30NoVtx_DxyMin0p01cm", &HLT_L3Mu30NoVtx_DxyMin0p01cm, &b_HLT_L3Mu30NoVtx_DxyMin0p01cm);
   fChain->SetBranchAddress("HLT_L3Mu50NoVtx_DxyMin0p01cm", &HLT_L3Mu50NoVtx_DxyMin0p01cm, &b_HLT_L3Mu50NoVtx_DxyMin0p01cm);
   fChain->SetBranchAddress("HLT_L3dTksMu10_NoVtx_DxyMin0p01cm", &HLT_L3dTksMu10_NoVtx_DxyMin0p01cm, &b_HLT_L3dTksMu10_NoVtx_DxyMin0p01cm);
   fChain->SetBranchAddress("HLT_Mu20NoFiltersNoVtxDisplaced_Photon20_CaloCustomId", &HLT_Mu20NoFiltersNoVtxDisplaced_Photon20_CaloCustomId, &b_HLT_Mu20NoFiltersNoVtxDisplaced_Photon20_CaloCustomId);
   fChain->SetBranchAddress("HLT_DisplacedMu24_MediumChargedIsoDisplacedPFTauHPS24", &HLT_DisplacedMu24_MediumChargedIsoDisplacedPFTauHPS24, &b_HLT_DisplacedMu24_MediumChargedIsoDisplacedPFTauHPS24);
   fChain->SetBranchAddress("HLT_Photon34_R9Id90_CaloIdL_IsoL_DisplacedIdL_MediumChargedIsoDisplacedPFTauHPS34", &HLT_Photon34_R9Id90_CaloIdL_IsoL_DisplacedIdL_MediumChargedIsoDisplacedPFTauHPS34, &b_HLT_Photon34_R9Id90_CaloIdL_IsoL_DisplacedIdL_MediumChargedIsoDisplacedPFTauHPS34);
   fChain->SetBranchAddress("HLT_DoubleMediumChargedIsoDisplacedPFTauHPS32_Trk1_eta2p1", &HLT_DoubleMediumChargedIsoDisplacedPFTauHPS32_Trk1_eta2p1, &b_HLT_DoubleMediumChargedIsoDisplacedPFTauHPS32_Trk1_eta2p1);
   fChain->SetBranchAddress("HLT_DoubleMediumChargedIsoDisplacedPFTauHPS36_Trk1_eta2p1", &HLT_DoubleMediumChargedIsoDisplacedPFTauHPS36_Trk1_eta2p1, &b_HLT_DoubleMediumChargedIsoDisplacedPFTauHPS36_Trk1_eta2p1);
   fChain->SetBranchAddress("HLT_DoubleMediumChargedIsoDisplacedPFTauHPS32_Trk1_eta2p1_noDxy", &HLT_DoubleMediumChargedIsoDisplacedPFTauHPS32_Trk1_eta2p1_noDxy, &b_HLT_DoubleMediumChargedIsoDisplacedPFTauHPS32_Trk1_eta2p1_noDxy);
   fChain->SetBranchAddress("HLT_IsoMu27_MediumChargedIsoDisplacedPFTauHPS24_eta2p1_SingleL1", &HLT_IsoMu27_MediumChargedIsoDisplacedPFTauHPS24_eta2p1_SingleL1, &b_HLT_IsoMu27_MediumChargedIsoDisplacedPFTauHPS24_eta2p1_SingleL1);
   fChain->SetBranchAddress("HLT_HT350_DelayedJet40_SingleDelay1p5To3p5nsInclusive", &HLT_HT350_DelayedJet40_SingleDelay1p5To3p5nsInclusive, &b_HLT_HT350_DelayedJet40_SingleDelay1p5To3p5nsInclusive);
   fChain->SetBranchAddress("HLT_HT350_DelayedJet40_SingleDelay1p6To3p5nsInclusive", &HLT_HT350_DelayedJet40_SingleDelay1p6To3p5nsInclusive, &b_HLT_HT350_DelayedJet40_SingleDelay1p6To3p5nsInclusive);
   fChain->SetBranchAddress("HLT_HT350_DelayedJet40_SingleDelay1p75To3p5nsInclusive", &HLT_HT350_DelayedJet40_SingleDelay1p75To3p5nsInclusive, &b_HLT_HT350_DelayedJet40_SingleDelay1p75To3p5nsInclusive);
   fChain->SetBranchAddress("HLT_HT350_DelayedJet40_SingleDelay3nsInclusive", &HLT_HT350_DelayedJet40_SingleDelay3nsInclusive, &b_HLT_HT350_DelayedJet40_SingleDelay3nsInclusive);
   fChain->SetBranchAddress("HLT_HT350_DelayedJet40_SingleDelay3p25nsInclusive", &HLT_HT350_DelayedJet40_SingleDelay3p25nsInclusive, &b_HLT_HT350_DelayedJet40_SingleDelay3p25nsInclusive);
   fChain->SetBranchAddress("HLT_HT350_DelayedJet40_SingleDelay3p5nsInclusive", &HLT_HT350_DelayedJet40_SingleDelay3p5nsInclusive, &b_HLT_HT350_DelayedJet40_SingleDelay3p5nsInclusive);
   fChain->SetBranchAddress("HLT_HT430_DelayedJet40_SingleDelay0p5nsInclusive", &HLT_HT430_DelayedJet40_SingleDelay0p5nsInclusive, &b_HLT_HT430_DelayedJet40_SingleDelay0p5nsInclusive);
   fChain->SetBranchAddress("HLT_HT430_DelayedJet40_SingleDelay1nsInclusive", &HLT_HT430_DelayedJet40_SingleDelay1nsInclusive, &b_HLT_HT430_DelayedJet40_SingleDelay1nsInclusive);
   fChain->SetBranchAddress("HLT_HT430_DelayedJet40_SingleDelay1To1p5nsInclusive", &HLT_HT430_DelayedJet40_SingleDelay1To1p5nsInclusive, &b_HLT_HT430_DelayedJet40_SingleDelay1To1p5nsInclusive);
   fChain->SetBranchAddress("HLT_HT430_DelayedJet40_SingleDelay1p1To1p6nsInclusive", &HLT_HT430_DelayedJet40_SingleDelay1p1To1p6nsInclusive, &b_HLT_HT430_DelayedJet40_SingleDelay1p1To1p6nsInclusive);
   fChain->SetBranchAddress("HLT_HT430_DelayedJet40_SingleDelay1p25To1p75nsInclusive", &HLT_HT430_DelayedJet40_SingleDelay1p25To1p75nsInclusive, &b_HLT_HT430_DelayedJet40_SingleDelay1p25To1p75nsInclusive);
   fChain->SetBranchAddress("HLT_HT430_DelayedJet40_SingleDelay1p5nsInclusive", &HLT_HT430_DelayedJet40_SingleDelay1p5nsInclusive, &b_HLT_HT430_DelayedJet40_SingleDelay1p5nsInclusive);
   fChain->SetBranchAddress("HLT_HT430_DelayedJet40_SingleDelay2nsInclusive", &HLT_HT430_DelayedJet40_SingleDelay2nsInclusive, &b_HLT_HT430_DelayedJet40_SingleDelay2nsInclusive);
   fChain->SetBranchAddress("HLT_HT430_DelayedJet40_SingleDelay2p25nsInclusive", &HLT_HT430_DelayedJet40_SingleDelay2p25nsInclusive, &b_HLT_HT430_DelayedJet40_SingleDelay2p25nsInclusive);
   fChain->SetBranchAddress("HLT_HT430_DelayedJet40_SingleDelay2p5nsInclusive", &HLT_HT430_DelayedJet40_SingleDelay2p5nsInclusive, &b_HLT_HT430_DelayedJet40_SingleDelay2p5nsInclusive);
   fChain->SetBranchAddress("HLT_HT430_DelayedJet40_SingleDelay0p5nsTrackless", &HLT_HT430_DelayedJet40_SingleDelay0p5nsTrackless, &b_HLT_HT430_DelayedJet40_SingleDelay0p5nsTrackless);
   fChain->SetBranchAddress("HLT_HT430_DelayedJet40_SingleDelay1nsTrackless", &HLT_HT430_DelayedJet40_SingleDelay1nsTrackless, &b_HLT_HT430_DelayedJet40_SingleDelay1nsTrackless);
   fChain->SetBranchAddress("HLT_HT430_DelayedJet40_SingleDelay1p25nsTrackless", &HLT_HT430_DelayedJet40_SingleDelay1p25nsTrackless, &b_HLT_HT430_DelayedJet40_SingleDelay1p25nsTrackless);
   fChain->SetBranchAddress("HLT_HT430_DelayedJet40_SingleDelay1p5nsTrackless", &HLT_HT430_DelayedJet40_SingleDelay1p5nsTrackless, &b_HLT_HT430_DelayedJet40_SingleDelay1p5nsTrackless);
   fChain->SetBranchAddress("HLT_HT430_DelayedJet40_DoubleDelay0p5nsInclusive", &HLT_HT430_DelayedJet40_DoubleDelay0p5nsInclusive, &b_HLT_HT430_DelayedJet40_DoubleDelay0p5nsInclusive);
   fChain->SetBranchAddress("HLT_HT430_DelayedJet40_DoubleDelay1nsInclusive", &HLT_HT430_DelayedJet40_DoubleDelay1nsInclusive, &b_HLT_HT430_DelayedJet40_DoubleDelay1nsInclusive);
   fChain->SetBranchAddress("HLT_HT430_DelayedJet40_DoubleDelay1p25nsInclusive", &HLT_HT430_DelayedJet40_DoubleDelay1p25nsInclusive, &b_HLT_HT430_DelayedJet40_DoubleDelay1p25nsInclusive);
   fChain->SetBranchAddress("HLT_HT430_DelayedJet40_DoubleDelay1p5nsInclusive", &HLT_HT430_DelayedJet40_DoubleDelay1p5nsInclusive, &b_HLT_HT430_DelayedJet40_DoubleDelay1p5nsInclusive);
   fChain->SetBranchAddress("HLT_HT430_DelayedJet40_DoubleDelay0p5nsTrackless", &HLT_HT430_DelayedJet40_DoubleDelay0p5nsTrackless, &b_HLT_HT430_DelayedJet40_DoubleDelay0p5nsTrackless);
   fChain->SetBranchAddress("HLT_HT430_DelayedJet40_DoubleDelay0p75nsTrackless", &HLT_HT430_DelayedJet40_DoubleDelay0p75nsTrackless, &b_HLT_HT430_DelayedJet40_DoubleDelay0p75nsTrackless);
   fChain->SetBranchAddress("HLT_HT430_DelayedJet40_DoubleDelay1nsTrackless", &HLT_HT430_DelayedJet40_DoubleDelay1nsTrackless, &b_HLT_HT430_DelayedJet40_DoubleDelay1nsTrackless);
   fChain->SetBranchAddress("HLT_L1Tau_DelayedJet40_SingleDelay2p5To4nsInclusive", &HLT_L1Tau_DelayedJet40_SingleDelay2p5To4nsInclusive, &b_HLT_L1Tau_DelayedJet40_SingleDelay2p5To4nsInclusive);
   fChain->SetBranchAddress("HLT_L1Tau_DelayedJet40_SingleDelay2p6To4nsInclusive", &HLT_L1Tau_DelayedJet40_SingleDelay2p6To4nsInclusive, &b_HLT_L1Tau_DelayedJet40_SingleDelay2p6To4nsInclusive);
   fChain->SetBranchAddress("HLT_L1Tau_DelayedJet40_SingleDelay2p75To4nsInclusive", &HLT_L1Tau_DelayedJet40_SingleDelay2p75To4nsInclusive, &b_HLT_L1Tau_DelayedJet40_SingleDelay2p75To4nsInclusive);
   fChain->SetBranchAddress("HLT_L1Tau_DelayedJet40_SingleDelay3p5nsInclusive", &HLT_L1Tau_DelayedJet40_SingleDelay3p5nsInclusive, &b_HLT_L1Tau_DelayedJet40_SingleDelay3p5nsInclusive);
   fChain->SetBranchAddress("HLT_L1Tau_DelayedJet40_SingleDelay3p75nsInclusive", &HLT_L1Tau_DelayedJet40_SingleDelay3p75nsInclusive, &b_HLT_L1Tau_DelayedJet40_SingleDelay3p75nsInclusive);
   fChain->SetBranchAddress("HLT_L1Tau_DelayedJet40_SingleDelay4nsInclusive", &HLT_L1Tau_DelayedJet40_SingleDelay4nsInclusive, &b_HLT_L1Tau_DelayedJet40_SingleDelay4nsInclusive);
   fChain->SetBranchAddress("HLT_L1Tau_DelayedJet40_SingleDelay2p5nsTrackless", &HLT_L1Tau_DelayedJet40_SingleDelay2p5nsTrackless, &b_HLT_L1Tau_DelayedJet40_SingleDelay2p5nsTrackless);
   fChain->SetBranchAddress("HLT_L1Tau_DelayedJet40_SingleDelay2p75nsTrackless", &HLT_L1Tau_DelayedJet40_SingleDelay2p75nsTrackless, &b_HLT_L1Tau_DelayedJet40_SingleDelay2p75nsTrackless);
   fChain->SetBranchAddress("HLT_L1Tau_DelayedJet40_SingleDelay3nsTrackless", &HLT_L1Tau_DelayedJet40_SingleDelay3nsTrackless, &b_HLT_L1Tau_DelayedJet40_SingleDelay3nsTrackless);
   fChain->SetBranchAddress("HLT_L1Tau_DelayedJet40_DoubleDelay0p75nsInclusive", &HLT_L1Tau_DelayedJet40_DoubleDelay0p75nsInclusive, &b_HLT_L1Tau_DelayedJet40_DoubleDelay0p75nsInclusive);
   fChain->SetBranchAddress("HLT_L1Tau_DelayedJet40_DoubleDelay1p25nsInclusive", &HLT_L1Tau_DelayedJet40_DoubleDelay1p25nsInclusive, &b_HLT_L1Tau_DelayedJet40_DoubleDelay1p25nsInclusive);
   fChain->SetBranchAddress("HLT_L1Tau_DelayedJet40_DoubleDelay1p5nsInclusive", &HLT_L1Tau_DelayedJet40_DoubleDelay1p5nsInclusive, &b_HLT_L1Tau_DelayedJet40_DoubleDelay1p5nsInclusive);
   fChain->SetBranchAddress("HLT_L1Tau_DelayedJet40_DoubleDelay1p75nsInclusive", &HLT_L1Tau_DelayedJet40_DoubleDelay1p75nsInclusive, &b_HLT_L1Tau_DelayedJet40_DoubleDelay1p75nsInclusive);
   fChain->SetBranchAddress("HLT_L1Tau_DelayedJet40_DoubleDelay0p5nsTrackless", &HLT_L1Tau_DelayedJet40_DoubleDelay0p5nsTrackless, &b_HLT_L1Tau_DelayedJet40_DoubleDelay0p5nsTrackless);
   fChain->SetBranchAddress("HLT_L1Tau_DelayedJet40_DoubleDelay1nsTrackless", &HLT_L1Tau_DelayedJet40_DoubleDelay1nsTrackless, &b_HLT_L1Tau_DelayedJet40_DoubleDelay1nsTrackless);
   fChain->SetBranchAddress("HLT_L1Tau_DelayedJet40_DoubleDelay1p25nsTrackless", &HLT_L1Tau_DelayedJet40_DoubleDelay1p25nsTrackless, &b_HLT_L1Tau_DelayedJet40_DoubleDelay1p25nsTrackless);
   fChain->SetBranchAddress("HLT_L1Tau_DelayedJet40_DoubleDelay1p5nsTrackless", &HLT_L1Tau_DelayedJet40_DoubleDelay1p5nsTrackless, &b_HLT_L1Tau_DelayedJet40_DoubleDelay1p5nsTrackless);
   fChain->SetBranchAddress("HLT_L1Mu6HT240", &HLT_L1Mu6HT240, &b_HLT_L1Mu6HT240);
   fChain->SetBranchAddress("HLT_Mu6HT240_DisplacedDijet30_Inclusive1PtrkShortSig5_DisplacedLoose", &HLT_Mu6HT240_DisplacedDijet30_Inclusive1PtrkShortSig5_DisplacedLoose, &b_HLT_Mu6HT240_DisplacedDijet30_Inclusive1PtrkShortSig5_DisplacedLoose);
   fChain->SetBranchAddress("HLT_Mu6HT240_DisplacedDijet35_Inclusive0PtrkShortSig5", &HLT_Mu6HT240_DisplacedDijet35_Inclusive0PtrkShortSig5, &b_HLT_Mu6HT240_DisplacedDijet35_Inclusive0PtrkShortSig5);
   fChain->SetBranchAddress("HLT_Mu6HT240_DisplacedDijet35_Inclusive1PtrkShortSig5_DisplacedLoose", &HLT_Mu6HT240_DisplacedDijet35_Inclusive1PtrkShortSig5_DisplacedLoose, &b_HLT_Mu6HT240_DisplacedDijet35_Inclusive1PtrkShortSig5_DisplacedLoose);
   fChain->SetBranchAddress("HLT_Mu6HT240_DisplacedDijet40_Inclusive0PtrkShortSig5", &HLT_Mu6HT240_DisplacedDijet40_Inclusive0PtrkShortSig5, &b_HLT_Mu6HT240_DisplacedDijet40_Inclusive0PtrkShortSig5);
   fChain->SetBranchAddress("HLT_Mu6HT240_DisplacedDijet40_Inclusive1PtrkShortSig5_DisplacedLoose", &HLT_Mu6HT240_DisplacedDijet40_Inclusive1PtrkShortSig5_DisplacedLoose, &b_HLT_Mu6HT240_DisplacedDijet40_Inclusive1PtrkShortSig5_DisplacedLoose);
   fChain->SetBranchAddress("HLT_Mu6HT240_DisplacedDijet45_Inclusive0PtrkShortSig5", &HLT_Mu6HT240_DisplacedDijet45_Inclusive0PtrkShortSig5, &b_HLT_Mu6HT240_DisplacedDijet45_Inclusive0PtrkShortSig5);
   fChain->SetBranchAddress("HLT_Mu6HT240_DisplacedDijet50_Inclusive0PtrkShortSig5", &HLT_Mu6HT240_DisplacedDijet50_Inclusive0PtrkShortSig5, &b_HLT_Mu6HT240_DisplacedDijet50_Inclusive0PtrkShortSig5);
   fChain->SetBranchAddress("HLT_HT350", &HLT_HT350, &b_HLT_HT350);
   fChain->SetBranchAddress("HLT_HT425", &HLT_HT425, &b_HLT_HT425);
   fChain->SetBranchAddress("HLT_HT360_DisplacedDijet40_Inclusive1PtrkShortSig5", &HLT_HT360_DisplacedDijet40_Inclusive1PtrkShortSig5, &b_HLT_HT360_DisplacedDijet40_Inclusive1PtrkShortSig5);
   fChain->SetBranchAddress("HLT_HT360_DisplacedDijet45_Inclusive1PtrkShortSig5", &HLT_HT360_DisplacedDijet45_Inclusive1PtrkShortSig5, &b_HLT_HT360_DisplacedDijet45_Inclusive1PtrkShortSig5);
   fChain->SetBranchAddress("HLT_HT390_DisplacedDijet40_Inclusive1PtrkShortSig5", &HLT_HT390_DisplacedDijet40_Inclusive1PtrkShortSig5, &b_HLT_HT390_DisplacedDijet40_Inclusive1PtrkShortSig5);
   fChain->SetBranchAddress("HLT_HT390_DisplacedDijet45_Inclusive1PtrkShortSig5", &HLT_HT390_DisplacedDijet45_Inclusive1PtrkShortSig5, &b_HLT_HT390_DisplacedDijet45_Inclusive1PtrkShortSig5);
   fChain->SetBranchAddress("HLT_HT390eta2p0_DisplacedDijet40_Inclusive1PtrkShortSig5", &HLT_HT390eta2p0_DisplacedDijet40_Inclusive1PtrkShortSig5, &b_HLT_HT390eta2p0_DisplacedDijet40_Inclusive1PtrkShortSig5);
   fChain->SetBranchAddress("HLT_HT430_DisplacedDijet40_Inclusive1PtrkShortSig5", &HLT_HT430_DisplacedDijet40_Inclusive1PtrkShortSig5, &b_HLT_HT430_DisplacedDijet40_Inclusive1PtrkShortSig5);
   fChain->SetBranchAddress("HLT_HT400_DisplacedDijet40_DisplacedTrack", &HLT_HT400_DisplacedDijet40_DisplacedTrack, &b_HLT_HT400_DisplacedDijet40_DisplacedTrack);
   fChain->SetBranchAddress("HLT_HT430_DisplacedDijet40_DisplacedTrack", &HLT_HT430_DisplacedDijet40_DisplacedTrack, &b_HLT_HT430_DisplacedDijet40_DisplacedTrack);
   fChain->SetBranchAddress("HLT_HT550_DisplacedDijet60_Inclusive", &HLT_HT550_DisplacedDijet60_Inclusive, &b_HLT_HT550_DisplacedDijet60_Inclusive);
   fChain->SetBranchAddress("HLT_HT650_DisplacedDijet60_Inclusive", &HLT_HT650_DisplacedDijet60_Inclusive, &b_HLT_HT650_DisplacedDijet60_Inclusive);
   fChain->SetBranchAddress("HLT_CaloMET60_DTCluster50", &HLT_CaloMET60_DTCluster50, &b_HLT_CaloMET60_DTCluster50);
   fChain->SetBranchAddress("HLT_CaloMET60_DTClusterNoMB1S50", &HLT_CaloMET60_DTClusterNoMB1S50, &b_HLT_CaloMET60_DTClusterNoMB1S50);
   fChain->SetBranchAddress("HLT_L1MET_DTCluster50", &HLT_L1MET_DTCluster50, &b_HLT_L1MET_DTCluster50);
   fChain->SetBranchAddress("HLT_L1MET_DTClusterNoMB1S50", &HLT_L1MET_DTClusterNoMB1S50, &b_HLT_L1MET_DTClusterNoMB1S50);
   fChain->SetBranchAddress("HLT_CscCluster_Loose", &HLT_CscCluster_Loose, &b_HLT_CscCluster_Loose);
   fChain->SetBranchAddress("HLT_CscCluster_Medium", &HLT_CscCluster_Medium, &b_HLT_CscCluster_Medium);
   fChain->SetBranchAddress("HLT_CscCluster_Tight", &HLT_CscCluster_Tight, &b_HLT_CscCluster_Tight);
   fChain->SetBranchAddress("HLT_CscCluster50_Photon20Unseeded", &HLT_CscCluster50_Photon20Unseeded, &b_HLT_CscCluster50_Photon20Unseeded);
   fChain->SetBranchAddress("HLT_CscCluster50_Photon30Unseeded", &HLT_CscCluster50_Photon30Unseeded, &b_HLT_CscCluster50_Photon30Unseeded);
   fChain->SetBranchAddress("HLT_CscCluster100_Ele5", &HLT_CscCluster100_Ele5, &b_HLT_CscCluster100_Ele5);
   fChain->SetBranchAddress("HLT_CscCluster100_Mu5", &HLT_CscCluster100_Mu5, &b_HLT_CscCluster100_Mu5);
   fChain->SetBranchAddress("HLT_CscCluster100_PNetTauhPFJet10_Loose", &HLT_CscCluster100_PNetTauhPFJet10_Loose, &b_HLT_CscCluster100_PNetTauhPFJet10_Loose);
   fChain->SetBranchAddress("HLT_DoubleCscCluster75", &HLT_DoubleCscCluster75, &b_HLT_DoubleCscCluster75);
   fChain->SetBranchAddress("HLT_IsoTrk200_L1SingleMuShower", &HLT_IsoTrk200_L1SingleMuShower, &b_HLT_IsoTrk200_L1SingleMuShower);
   fChain->SetBranchAddress("HLT_IsoTrk400_L1SingleMuShower", &HLT_IsoTrk400_L1SingleMuShower, &b_HLT_IsoTrk400_L1SingleMuShower);
   fChain->SetBranchAddress("HLT_DoubleCscCluster100", &HLT_DoubleCscCluster100, &b_HLT_DoubleCscCluster100);
   fChain->SetBranchAddress("HLT_L1CSCShower_DTCluster50", &HLT_L1CSCShower_DTCluster50, &b_HLT_L1CSCShower_DTCluster50);
   fChain->SetBranchAddress("HLT_L1CSCShower_DTCluster75", &HLT_L1CSCShower_DTCluster75, &b_HLT_L1CSCShower_DTCluster75);
   fChain->SetBranchAddress("HLT_PFMET105_IsoTrk50", &HLT_PFMET105_IsoTrk50, &b_HLT_PFMET105_IsoTrk50);
   fChain->SetBranchAddress("HLT_L1SingleLLPJet", &HLT_L1SingleLLPJet, &b_HLT_L1SingleLLPJet);
   fChain->SetBranchAddress("HLT_HT170_L1SingleLLPJet_DisplacedDijet40_DisplacedTrack", &HLT_HT170_L1SingleLLPJet_DisplacedDijet40_DisplacedTrack, &b_HLT_HT170_L1SingleLLPJet_DisplacedDijet40_DisplacedTrack);
   fChain->SetBranchAddress("HLT_HT200_L1SingleLLPJet_DisplacedDijet40_DisplacedTrack", &HLT_HT200_L1SingleLLPJet_DisplacedDijet40_DisplacedTrack, &b_HLT_HT200_L1SingleLLPJet_DisplacedDijet40_DisplacedTrack);
   fChain->SetBranchAddress("HLT_HT200_L1SingleLLPJet_DisplacedDijet60_DisplacedTrack", &HLT_HT200_L1SingleLLPJet_DisplacedDijet60_DisplacedTrack, &b_HLT_HT200_L1SingleLLPJet_DisplacedDijet60_DisplacedTrack);
   fChain->SetBranchAddress("HLT_HT270_L1SingleLLPJet_DisplacedDijet40_DisplacedTrack", &HLT_HT270_L1SingleLLPJet_DisplacedDijet40_DisplacedTrack, &b_HLT_HT270_L1SingleLLPJet_DisplacedDijet40_DisplacedTrack);
   fChain->SetBranchAddress("HLT_HT200_L1SingleLLPJet_DisplacedDijet40_Inclusive1PtrkShortSig5", &HLT_HT200_L1SingleLLPJet_DisplacedDijet40_Inclusive1PtrkShortSig5, &b_HLT_HT200_L1SingleLLPJet_DisplacedDijet40_Inclusive1PtrkShortSig5);
   fChain->SetBranchAddress("HLT_HT240_L1SingleLLPJet_DisplacedDijet40_Inclusive1PtrkShortSig5", &HLT_HT240_L1SingleLLPJet_DisplacedDijet40_Inclusive1PtrkShortSig5, &b_HLT_HT240_L1SingleLLPJet_DisplacedDijet40_Inclusive1PtrkShortSig5);
   fChain->SetBranchAddress("HLT_HT280_L1SingleLLPJet_DisplacedDijet40_Inclusive1PtrkShortSig5", &HLT_HT280_L1SingleLLPJet_DisplacedDijet40_Inclusive1PtrkShortSig5, &b_HLT_HT280_L1SingleLLPJet_DisplacedDijet40_Inclusive1PtrkShortSig5);
   fChain->SetBranchAddress("HLT_HT320_L1SingleLLPJet_DisplacedDijet60_Inclusive", &HLT_HT320_L1SingleLLPJet_DisplacedDijet60_Inclusive, &b_HLT_HT320_L1SingleLLPJet_DisplacedDijet60_Inclusive);
   fChain->SetBranchAddress("HLT_HT420_L1SingleLLPJet_DisplacedDijet60_Inclusive", &HLT_HT420_L1SingleLLPJet_DisplacedDijet60_Inclusive, &b_HLT_HT420_L1SingleLLPJet_DisplacedDijet60_Inclusive);
   fChain->SetBranchAddress("HLT_HT200_L1SingleLLPJet_DelayedJet40_SingleDelay1nsTrackless", &HLT_HT200_L1SingleLLPJet_DelayedJet40_SingleDelay1nsTrackless, &b_HLT_HT200_L1SingleLLPJet_DelayedJet40_SingleDelay1nsTrackless);
   fChain->SetBranchAddress("HLT_HT200_L1SingleLLPJet_DelayedJet40_SingleDelay2nsInclusive", &HLT_HT200_L1SingleLLPJet_DelayedJet40_SingleDelay2nsInclusive, &b_HLT_HT200_L1SingleLLPJet_DelayedJet40_SingleDelay2nsInclusive);
   fChain->SetBranchAddress("HLT_HT200_L1SingleLLPJet_DelayedJet40_DoubleDelay0p5nsTrackless", &HLT_HT200_L1SingleLLPJet_DelayedJet40_DoubleDelay0p5nsTrackless, &b_HLT_HT200_L1SingleLLPJet_DelayedJet40_DoubleDelay0p5nsTrackless);
   fChain->SetBranchAddress("HLT_HT200_L1SingleLLPJet_DelayedJet40_DoubleDelay1nsInclusive", &HLT_HT200_L1SingleLLPJet_DelayedJet40_DoubleDelay1nsInclusive, &b_HLT_HT200_L1SingleLLPJet_DelayedJet40_DoubleDelay1nsInclusive);
   fChain->SetBranchAddress("HLT_HT200_L1SingleLLPJet_PFJet60_NeutralHadronFrac0p7", &HLT_HT200_L1SingleLLPJet_PFJet60_NeutralHadronFrac0p7, &b_HLT_HT200_L1SingleLLPJet_PFJet60_NeutralHadronFrac0p7);
   fChain->SetBranchAddress("HLT_HT200_L1SingleLLPJet_PFJet60_NeutralHadronFrac0p8", &HLT_HT200_L1SingleLLPJet_PFJet60_NeutralHadronFrac0p8, &b_HLT_HT200_L1SingleLLPJet_PFJet60_NeutralHadronFrac0p8);
   fChain->SetBranchAddress("HLT_DiPhoton10Time1ns", &HLT_DiPhoton10Time1ns, &b_HLT_DiPhoton10Time1ns);
   fChain->SetBranchAddress("HLT_DiPhoton10Time1p2ns", &HLT_DiPhoton10Time1p2ns, &b_HLT_DiPhoton10Time1p2ns);
   fChain->SetBranchAddress("HLT_DiPhoton10Time1p4ns", &HLT_DiPhoton10Time1p4ns, &b_HLT_DiPhoton10Time1p4ns);
   fChain->SetBranchAddress("HLT_DiPhoton10Time1p6ns", &HLT_DiPhoton10Time1p6ns, &b_HLT_DiPhoton10Time1p6ns);
   fChain->SetBranchAddress("HLT_DiPhoton10Time1p8ns", &HLT_DiPhoton10Time1p8ns, &b_HLT_DiPhoton10Time1p8ns);
   fChain->SetBranchAddress("HLT_DiPhoton10Time2ns", &HLT_DiPhoton10Time2ns, &b_HLT_DiPhoton10Time2ns);
   fChain->SetBranchAddress("HLT_DiPhoton10_CaloIdL", &HLT_DiPhoton10_CaloIdL, &b_HLT_DiPhoton10_CaloIdL);
   fChain->SetBranchAddress("HLT_DoubleEle6p5_eta1p22_mMax6", &HLT_DoubleEle6p5_eta1p22_mMax6, &b_HLT_DoubleEle6p5_eta1p22_mMax6);
   fChain->SetBranchAddress("HLT_DoubleEle8_eta1p22_mMax6", &HLT_DoubleEle8_eta1p22_mMax6, &b_HLT_DoubleEle8_eta1p22_mMax6);
   fChain->SetBranchAddress("HLT_DoubleEle10_eta1p22_mMax6", &HLT_DoubleEle10_eta1p22_mMax6, &b_HLT_DoubleEle10_eta1p22_mMax6);
   fChain->SetBranchAddress("HLT_SingleEle8", &HLT_SingleEle8, &b_HLT_SingleEle8);
   fChain->SetBranchAddress("HLT_SingleEle8_SingleEGL1", &HLT_SingleEle8_SingleEGL1, &b_HLT_SingleEle8_SingleEGL1);
   fChain->SetBranchAddress("HLT_Diphoton20_14_eta1p5_R9IdL_AND_HE_AND_IsoTCaloIdT", &HLT_Diphoton20_14_eta1p5_R9IdL_AND_HE_AND_IsoTCaloIdT, &b_HLT_Diphoton20_14_eta1p5_R9IdL_AND_HE_AND_IsoTCaloIdT);
   fChain->SetBranchAddress("HLT_Diphoton20_14_eta1p5_R9IdL_AND_HET_AND_IsoTCaloIdT", &HLT_Diphoton20_14_eta1p5_R9IdL_AND_HET_AND_IsoTCaloIdT, &b_HLT_Diphoton20_14_eta1p5_R9IdL_AND_HET_AND_IsoTCaloIdT);
   fChain->SetBranchAddress("HLT_Diphoton22_14_eta1p5_R9IdL_AND_HE_AND_IsoTCaloIdT", &HLT_Diphoton22_14_eta1p5_R9IdL_AND_HE_AND_IsoTCaloIdT, &b_HLT_Diphoton22_14_eta1p5_R9IdL_AND_HE_AND_IsoTCaloIdT);
   fChain->SetBranchAddress("HLT_Diphoton22_14_eta1p5_R9IdL_AND_HET_AND_IsoTCaloIdT", &HLT_Diphoton22_14_eta1p5_R9IdL_AND_HET_AND_IsoTCaloIdT, &b_HLT_Diphoton22_14_eta1p5_R9IdL_AND_HET_AND_IsoTCaloIdT);
   fChain->SetBranchAddress("HLT_Diphoton24_14_eta1p5_R9IdL_AND_HET_AND_IsoTCaloIdT", &HLT_Diphoton24_14_eta1p5_R9IdL_AND_HET_AND_IsoTCaloIdT, &b_HLT_Diphoton24_14_eta1p5_R9IdL_AND_HET_AND_IsoTCaloIdT);
   fChain->SetBranchAddress("HLT_Diphoton24_16_eta1p5_R9IdL_AND_HET_AND_IsoTCaloIdT", &HLT_Diphoton24_16_eta1p5_R9IdL_AND_HET_AND_IsoTCaloIdT, &b_HLT_Diphoton24_16_eta1p5_R9IdL_AND_HET_AND_IsoTCaloIdT);
   fChain->SetBranchAddress("HLT_Mu50_L1SingleMuShower", &HLT_Mu50_L1SingleMuShower, &b_HLT_Mu50_L1SingleMuShower);
   fChain->SetBranchAddress("HLT_IsoMu24_OneProng32", &HLT_IsoMu24_OneProng32, &b_HLT_IsoMu24_OneProng32);
   fChain->SetBranchAddress("HLT_Photon32_OneProng32_M50To105", &HLT_Photon32_OneProng32_M50To105, &b_HLT_Photon32_OneProng32_M50To105);
   fChain->SetBranchAddress("HLT_DoubleMediumDeepTauPFTauHPS30_L2NN_eta2p1_OneProng", &HLT_DoubleMediumDeepTauPFTauHPS30_L2NN_eta2p1_OneProng, &b_HLT_DoubleMediumDeepTauPFTauHPS30_L2NN_eta2p1_OneProng);
   fChain->SetBranchAddress("HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS30_L2NN_eta2p1_OneProng_CrossL1", &HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS30_L2NN_eta2p1_OneProng_CrossL1, &b_HLT_IsoMu24_eta2p1_MediumDeepTauPFTauHPS30_L2NN_eta2p1_OneProng_CrossL1);
   fChain->SetBranchAddress("HLT_VBF_DiPFJet125_45_Mjj1050", &HLT_VBF_DiPFJet125_45_Mjj1050, &b_HLT_VBF_DiPFJet125_45_Mjj1050);
   fChain->SetBranchAddress("HLT_VBF_DiPFJet125_45_Mjj1200", &HLT_VBF_DiPFJet125_45_Mjj1200, &b_HLT_VBF_DiPFJet125_45_Mjj1200);
   fChain->SetBranchAddress("HLT_VBF_DiPFJet75_45_Mjj800_DiPFJet60", &HLT_VBF_DiPFJet75_45_Mjj800_DiPFJet60, &b_HLT_VBF_DiPFJet75_45_Mjj800_DiPFJet60);
   fChain->SetBranchAddress("HLT_VBF_DiPFJet75_45_Mjj850_DiPFJet60", &HLT_VBF_DiPFJet75_45_Mjj850_DiPFJet60, &b_HLT_VBF_DiPFJet75_45_Mjj850_DiPFJet60);
   fChain->SetBranchAddress("HLT_VBF_DiPFJet80_45_Mjj650_PFMETNoMu85", &HLT_VBF_DiPFJet80_45_Mjj650_PFMETNoMu85, &b_HLT_VBF_DiPFJet80_45_Mjj650_PFMETNoMu85);
   fChain->SetBranchAddress("HLT_VBF_DiPFJet80_45_Mjj750_PFMETNoMu85", &HLT_VBF_DiPFJet80_45_Mjj750_PFMETNoMu85, &b_HLT_VBF_DiPFJet80_45_Mjj750_PFMETNoMu85);
   fChain->SetBranchAddress("HLT_VBF_DiPFJet95_45_Mjj750_Mu3_TrkIsoVVL", &HLT_VBF_DiPFJet95_45_Mjj750_Mu3_TrkIsoVVL, &b_HLT_VBF_DiPFJet95_45_Mjj750_Mu3_TrkIsoVVL);
   fChain->SetBranchAddress("HLT_VBF_DiPFJet95_45_Mjj850_Mu3_TrkIsoVVL", &HLT_VBF_DiPFJet95_45_Mjj850_Mu3_TrkIsoVVL, &b_HLT_VBF_DiPFJet95_45_Mjj850_Mu3_TrkIsoVVL);
   fChain->SetBranchAddress("HLT_VBF_DiPFJet50_Mjj650_Photon22", &HLT_VBF_DiPFJet50_Mjj650_Photon22, &b_HLT_VBF_DiPFJet50_Mjj650_Photon22);
   fChain->SetBranchAddress("HLT_VBF_DiPFJet50_Mjj750_Photon22", &HLT_VBF_DiPFJet50_Mjj750_Photon22, &b_HLT_VBF_DiPFJet50_Mjj750_Photon22);
   fChain->SetBranchAddress("HLT_VBF_DiPFJet50_Mjj600_Ele22_eta2p1_WPTight_Gsf", &HLT_VBF_DiPFJet50_Mjj600_Ele22_eta2p1_WPTight_Gsf, &b_HLT_VBF_DiPFJet50_Mjj600_Ele22_eta2p1_WPTight_Gsf);
   fChain->SetBranchAddress("HLT_VBF_DiPFJet50_Mjj650_Ele22_eta2p1_WPTight_Gsf", &HLT_VBF_DiPFJet50_Mjj650_Ele22_eta2p1_WPTight_Gsf, &b_HLT_VBF_DiPFJet50_Mjj650_Ele22_eta2p1_WPTight_Gsf);
   fChain->SetBranchAddress("HLT_VBF_DiPFJet45_Mjj650_MediumDeepTauPFTauHPS45_L2NN_eta2p1", &HLT_VBF_DiPFJet45_Mjj650_MediumDeepTauPFTauHPS45_L2NN_eta2p1, &b_HLT_VBF_DiPFJet45_Mjj650_MediumDeepTauPFTauHPS45_L2NN_eta2p1);
   fChain->SetBranchAddress("HLT_VBF_DiPFJet45_Mjj750_MediumDeepTauPFTauHPS45_L2NN_eta2p1", &HLT_VBF_DiPFJet45_Mjj750_MediumDeepTauPFTauHPS45_L2NN_eta2p1, &b_HLT_VBF_DiPFJet45_Mjj750_MediumDeepTauPFTauHPS45_L2NN_eta2p1);
   fChain->SetBranchAddress("HLT_VBF_DoublePNetTauhPFJet20_eta2p2", &HLT_VBF_DoublePNetTauhPFJet20_eta2p2, &b_HLT_VBF_DoublePNetTauhPFJet20_eta2p2);
   fChain->SetBranchAddress("HLT_VBF_DiPFJet45_Mjj650_PNetTauhPFJet45_L2NN_eta2p3", &HLT_VBF_DiPFJet45_Mjj650_PNetTauhPFJet45_L2NN_eta2p3, &b_HLT_VBF_DiPFJet45_Mjj650_PNetTauhPFJet45_L2NN_eta2p3);
   fChain->SetBranchAddress("HLT_VBF_DiPFJet45_Mjj750_PNetTauhPFJet45_L2NN_eta2p3", &HLT_VBF_DiPFJet45_Mjj750_PNetTauhPFJet45_L2NN_eta2p3, &b_HLT_VBF_DiPFJet45_Mjj750_PNetTauhPFJet45_L2NN_eta2p3);
   fChain->SetBranchAddress("HLT_PFJet200_TimeLtNeg2p5ns", &HLT_PFJet200_TimeLtNeg2p5ns, &b_HLT_PFJet200_TimeLtNeg2p5ns);
   fChain->SetBranchAddress("HLT_PFJet200_TimeGt2p5ns", &HLT_PFJet200_TimeGt2p5ns, &b_HLT_PFJet200_TimeGt2p5ns);
   fChain->SetBranchAddress("HLT_Photon50_TimeLtNeg2p5ns", &HLT_Photon50_TimeLtNeg2p5ns, &b_HLT_Photon50_TimeLtNeg2p5ns);
   fChain->SetBranchAddress("HLT_Photon50_TimeGt2p5ns", &HLT_Photon50_TimeGt2p5ns, &b_HLT_Photon50_TimeGt2p5ns);
   fChain->SetBranchAddress("HLT_PPSMaxTracksPerArm1", &HLT_PPSMaxTracksPerArm1, &b_HLT_PPSMaxTracksPerArm1);
   fChain->SetBranchAddress("HLT_PPSMaxTracksPerRP4", &HLT_PPSMaxTracksPerRP4, &b_HLT_PPSMaxTracksPerRP4);
   fChain->SetBranchAddress("HLT_PPSRandom", &HLT_PPSRandom, &b_HLT_PPSRandom);
   fChain->SetBranchAddress("HLT_L1AXOVTight", &HLT_L1AXOVTight, &b_HLT_L1AXOVTight);
   fChain->SetBranchAddress("HLTriggerFinalPath", &HLTriggerFinalPath, &b_HLTriggerFinalPath);
   fChain->SetBranchAddress("Dataset_ScoutingPFMonitor", &Dataset_ScoutingPFMonitor, &b_Dataset_ScoutingPFMonitor);
   fChain->SetBranchAddress("Dataset_ScoutingPFRun3", &Dataset_ScoutingPFRun3, &b_Dataset_ScoutingPFRun3);
   Notify();
}

bool rootNtupleClass::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return true;
}

void rootNtupleClass::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t rootNtupleClass::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef rootNtupleClass_cxx
