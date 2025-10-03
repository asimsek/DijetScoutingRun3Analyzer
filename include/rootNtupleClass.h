//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Oct  3 00:41:00 2025 by ROOT version 6.32.13
// from TChain dijetScouting/events/
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
#include "vector"
#include "vector"
#include "vector"
#include "vector"

class rootNtupleClass {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.
   static constexpr Int_t kMaxisData = 1;
   static constexpr Int_t kMaxrun = 1;
   static constexpr Int_t kMaxevt = 1;
   static constexpr Int_t kMaxlumi = 1;
   static constexpr Int_t kMaxnVtx = 1;
   static constexpr Int_t kMaxrho = 1;
   static constexpr Int_t kMaxmet = 1;
   static constexpr Int_t kMaxmetphi = 1;
   static constexpr Int_t kMaxsumEt = 1;
   static constexpr Int_t kMaxmetSig = 1;
   static constexpr Int_t kMaxmetOverSumEt = 1;
   static constexpr Int_t kMaxunclusteredEnFrac = 1;
   static constexpr Int_t kMaxminDPhiMetJet2 = 1;
   static constexpr Int_t kMaxminDPhiMetJet4 = 1;
   static constexpr Int_t kMaxnPFJets = 1;
   static constexpr Int_t kMaxhtAK4 = 1;
   static constexpr Int_t kMaxmjjAK4 = 1;
   static constexpr Int_t kMaxdEtajjAK4 = 1;
   static constexpr Int_t kMaxdPhijjAK4 = 1;
   static constexpr Int_t kMaxnJetInVetoMap = 1;

   // Declaration of leaf types
   Int_t           isData;
   Int_t           runNo;
   Long64_t        evtNo;
   Int_t           lumi;
   Int_t           nvtx;
   Float_t         rho;
   Float_t         met;
   Float_t         metphi;
   Float_t         sumEt;
   Float_t         metSig;
   Float_t         metOverSumEt;
   Float_t         unclusteredEnFrac;
   Float_t         minDPhiMetJet2;
   Float_t         minDPhiMetJet4;
   Int_t           nPFJets;
   Float_t         htAK4;
   Float_t         mjjAK4;
   Float_t         dEtajjAK4;
   Float_t         dPhijjAK4;
   vector<float>   *jetPtAK4;
   vector<float>   *jetRawPtAK4;
   vector<float>   *jetEtaAK4;
   vector<float>   *jetPhiAK4;
   vector<float>   *jetMassAK4;
   vector<float>   *jetEnergyAK4;
   vector<float>   *jetAreaAK4;
   vector<float>   *jetChfAK4;
   vector<float>   *jetNhfAK4;
   vector<float>   *jetPhfAK4;
   vector<float>   *jetMufAK4;
   vector<float>   *jetElfAK4;
   vector<float>   *jetHf_hfAK4;
   vector<float>   *jetHf_emfAK4;
   vector<float>   *jetHofAK4;
   vector<int>     *idTAK4;
   vector<int>     *idLAK4;
   vector<int>     *chHadMultAK4;
   vector<int>     *neHadMultAK4;
   vector<int>     *phoMultAK4;
   vector<int>     *elMultAK4;
   vector<int>     *muMultAK4;
   vector<int>     *hfHadMultAK4;
   vector<int>     *hfEmMultAK4;
   vector<float>   *jetJECFactorAK4;
   vector<float>   *jetJECUncRelAK4;
   vector<float>   *jetJECUpFactorAK4;
   vector<float>   *jetJECDownFactorAK4;
   vector<float>   *jetRapidityAK4;
   vector<int>     *jetVetoMapAK4;
   Int_t           nJetInVetoMap;
   vector<float>   *jetChEmEAK4;
   vector<float>   *jetNeEmEAK4;
   vector<float>   *jetChEmFAK4;
   vector<float>   *jetNeEmFAK4;
   vector<bool>    *triggerResult;
   vector<string>  *triggerName;
   vector<bool>    *l1Result;
   vector<string>  *l1Name;

   // List of branches
   TBranch        *b_isData_;   //!
   TBranch        *b_run_;   //!
   TBranch        *b_evt_;   //!
   TBranch        *b_lumi_;   //!
   TBranch        *b_nVtx_;   //!
   TBranch        *b_rho_;   //!
   TBranch        *b_met_;   //!
   TBranch        *b_metphi_;   //!
   TBranch        *b_sumEt_;   //!
   TBranch        *b_metSig_;   //!
   TBranch        *b_metOverSumEt_;   //!
   TBranch        *b_unclusteredEnFrac_;   //!
   TBranch        *b_minDPhiMetJet2_;   //!
   TBranch        *b_minDPhiMetJet4_;   //!
   TBranch        *b_nPFJets_;   //!
   TBranch        *b_htAK4_;   //!
   TBranch        *b_mjjAK4_;   //!
   TBranch        *b_dEtajjAK4_;   //!
   TBranch        *b_dPhijjAK4_;   //!
   TBranch        *b_jetPtAK4;   //!
   TBranch        *b_jetRawPtAK4;   //!
   TBranch        *b_jetEtaAK4;   //!
   TBranch        *b_jetPhiAK4;   //!
   TBranch        *b_jetMassAK4;   //!
   TBranch        *b_jetEnergyAK4;   //!
   TBranch        *b_jetAreaAK4;   //!
   TBranch        *b_jetChfAK4;   //!
   TBranch        *b_jetNhfAK4;   //!
   TBranch        *b_jetPhfAK4;   //!
   TBranch        *b_jetMufAK4;   //!
   TBranch        *b_jetElfAK4;   //!
   TBranch        *b_jetHf_hfAK4;   //!
   TBranch        *b_jetHf_emfAK4;   //!
   TBranch        *b_jetHofAK4;   //!
   TBranch        *b_idTAK4;   //!
   TBranch        *b_idLAK4;   //!
   TBranch        *b_chHadMultAK4;   //!
   TBranch        *b_neHadMultAK4;   //!
   TBranch        *b_phoMultAK4;   //!
   TBranch        *b_elMultAK4;   //!
   TBranch        *b_muMultAK4;   //!
   TBranch        *b_hfHadMultAK4;   //!
   TBranch        *b_hfEmMultAK4;   //!
   TBranch        *b_jetJECFactorAK4;   //!
   TBranch        *b_jetJECUncRelAK4;   //!
   TBranch        *b_jetJECUpFactorAK4;   //!
   TBranch        *b_jetJECDownFactorAK4;   //!
   TBranch        *b_jetRapidityAK4;   //!
   TBranch        *b_jetVetoMapAK4;   //!
   TBranch        *b_nJetInVetoMap_;   //!
   TBranch        *b_jetChEmEAK4;   //!
   TBranch        *b_jetNeEmEAK4;   //!
   TBranch        *b_jetChEmFAK4;   //!
   TBranch        *b_jetNeEmFAK4;   //!
   TBranch        *b_triggerResult;   //!
   TBranch        *b_triggerName;   //!
   TBranch        *b_l1Result;   //!
   TBranch        *b_l1Name;   //!

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
      f->GetObject("dijetScouting/events",tree);

#else // SINGLE_TREE

      // The following code should be used if you want this class to access a chain
      // of trees.
      TChain * chain = new TChain("dijetScouting/events","");
      chain->Add("/uscms/home/asimsek/nobackup/Dijet_Run3/treeMaker/CMSSW_15_0_6/src/DijetScoutingRun3NTupleMaker/test_scouting.root/dijetScouting/events");
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

   // Set object pointer
   jetPtAK4 = 0;
   jetRawPtAK4 = 0;
   jetEtaAK4 = 0;
   jetPhiAK4 = 0;
   jetMassAK4 = 0;
   jetEnergyAK4 = 0;
   jetAreaAK4 = 0;
   jetChfAK4 = 0;
   jetNhfAK4 = 0;
   jetPhfAK4 = 0;
   jetMufAK4 = 0;
   jetElfAK4 = 0;
   jetHf_hfAK4 = 0;
   jetHf_emfAK4 = 0;
   jetHofAK4 = 0;
   idTAK4 = 0;
   idLAK4 = 0;
   chHadMultAK4 = 0;
   neHadMultAK4 = 0;
   phoMultAK4 = 0;
   elMultAK4 = 0;
   muMultAK4 = 0;
   hfHadMultAK4 = 0;
   hfEmMultAK4 = 0;
   jetJECFactorAK4 = 0;
   jetJECUncRelAK4 = 0;
   jetJECUpFactorAK4 = 0;
   jetJECDownFactorAK4 = 0;
   jetRapidityAK4 = 0;
   jetVetoMapAK4 = 0;
   jetChEmEAK4 = 0;
   jetNeEmEAK4 = 0;
   jetChEmFAK4 = 0;
   jetNeEmFAK4 = 0;
   triggerResult = 0;
   triggerName = 0;
   l1Result = 0;
   l1Name = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("isData", &isData, &b_isData_);
   fChain->SetBranchAddress("runNo", &runNo, &b_run_);
   fChain->SetBranchAddress("evtNo", &evtNo, &b_evt_);
   fChain->SetBranchAddress("lumi", &lumi, &b_lumi_);
   fChain->SetBranchAddress("nvtx", &nvtx, &b_nVtx_);
   fChain->SetBranchAddress("rho", &rho, &b_rho_);
   fChain->SetBranchAddress("met", &met, &b_met_);
   fChain->SetBranchAddress("metphi", &metphi, &b_metphi_);
   fChain->SetBranchAddress("sumEt", &sumEt, &b_sumEt_);
   fChain->SetBranchAddress("metSig", &metSig, &b_metSig_);
   fChain->SetBranchAddress("metOverSumEt", &metOverSumEt, &b_metOverSumEt_);
   fChain->SetBranchAddress("unclusteredEnFrac", &unclusteredEnFrac, &b_unclusteredEnFrac_);
   fChain->SetBranchAddress("minDPhiMetJet2", &minDPhiMetJet2, &b_minDPhiMetJet2_);
   fChain->SetBranchAddress("minDPhiMetJet4", &minDPhiMetJet4, &b_minDPhiMetJet4_);
   fChain->SetBranchAddress("nPFJets", &nPFJets, &b_nPFJets_);
   fChain->SetBranchAddress("htAK4", &htAK4, &b_htAK4_);
   fChain->SetBranchAddress("mjjAK4", &mjjAK4, &b_mjjAK4_);
   fChain->SetBranchAddress("dEtajjAK4", &dEtajjAK4, &b_dEtajjAK4_);
   fChain->SetBranchAddress("dPhijjAK4", &dPhijjAK4, &b_dPhijjAK4_);
   fChain->SetBranchAddress("jetPtAK4", &jetPtAK4, &b_jetPtAK4);
   fChain->SetBranchAddress("jetRawPtAK4", &jetRawPtAK4, &b_jetRawPtAK4);
   fChain->SetBranchAddress("jetEtaAK4", &jetEtaAK4, &b_jetEtaAK4);
   fChain->SetBranchAddress("jetPhiAK4", &jetPhiAK4, &b_jetPhiAK4);
   fChain->SetBranchAddress("jetMassAK4", &jetMassAK4, &b_jetMassAK4);
   fChain->SetBranchAddress("jetEnergyAK4", &jetEnergyAK4, &b_jetEnergyAK4);
   fChain->SetBranchAddress("jetAreaAK4", &jetAreaAK4, &b_jetAreaAK4);
   fChain->SetBranchAddress("jetChfAK4", &jetChfAK4, &b_jetChfAK4);
   fChain->SetBranchAddress("jetNhfAK4", &jetNhfAK4, &b_jetNhfAK4);
   fChain->SetBranchAddress("jetPhfAK4", &jetPhfAK4, &b_jetPhfAK4);
   fChain->SetBranchAddress("jetMufAK4", &jetMufAK4, &b_jetMufAK4);
   fChain->SetBranchAddress("jetElfAK4", &jetElfAK4, &b_jetElfAK4);
   fChain->SetBranchAddress("jetHf_hfAK4", &jetHf_hfAK4, &b_jetHf_hfAK4);
   fChain->SetBranchAddress("jetHf_emfAK4", &jetHf_emfAK4, &b_jetHf_emfAK4);
   fChain->SetBranchAddress("jetHofAK4", &jetHofAK4, &b_jetHofAK4);
   fChain->SetBranchAddress("idTAK4", &idTAK4, &b_idTAK4);
   fChain->SetBranchAddress("idLAK4", &idLAK4, &b_idLAK4);
   fChain->SetBranchAddress("chHadMultAK4", &chHadMultAK4, &b_chHadMultAK4);
   fChain->SetBranchAddress("neHadMultAK4", &neHadMultAK4, &b_neHadMultAK4);
   fChain->SetBranchAddress("phoMultAK4", &phoMultAK4, &b_phoMultAK4);
   fChain->SetBranchAddress("elMultAK4", &elMultAK4, &b_elMultAK4);
   fChain->SetBranchAddress("muMultAK4", &muMultAK4, &b_muMultAK4);
   fChain->SetBranchAddress("hfHadMultAK4", &hfHadMultAK4, &b_hfHadMultAK4);
   fChain->SetBranchAddress("hfEmMultAK4", &hfEmMultAK4, &b_hfEmMultAK4);
   fChain->SetBranchAddress("jetJECFactorAK4", &jetJECFactorAK4, &b_jetJECFactorAK4);
   fChain->SetBranchAddress("jetJECUncRelAK4", &jetJECUncRelAK4, &b_jetJECUncRelAK4);
   fChain->SetBranchAddress("jetJECUpFactorAK4", &jetJECUpFactorAK4, &b_jetJECUpFactorAK4);
   fChain->SetBranchAddress("jetJECDownFactorAK4", &jetJECDownFactorAK4, &b_jetJECDownFactorAK4);
   fChain->SetBranchAddress("jetRapidityAK4", &jetRapidityAK4, &b_jetRapidityAK4);
   fChain->SetBranchAddress("jetVetoMapAK4", &jetVetoMapAK4, &b_jetVetoMapAK4);
   fChain->SetBranchAddress("nJetInVetoMap", &nJetInVetoMap, &b_nJetInVetoMap_);
   fChain->SetBranchAddress("jetChEmEAK4", &jetChEmEAK4, &b_jetChEmEAK4);
   fChain->SetBranchAddress("jetNeEmEAK4", &jetNeEmEAK4, &b_jetNeEmEAK4);
   fChain->SetBranchAddress("jetChEmFAK4", &jetChEmFAK4, &b_jetChEmFAK4);
   fChain->SetBranchAddress("jetNeEmFAK4", &jetNeEmFAK4, &b_jetNeEmFAK4);
   fChain->SetBranchAddress("triggerResult", &triggerResult, &b_triggerResult);
   fChain->SetBranchAddress("triggerName", &triggerName, &b_triggerName);
   fChain->SetBranchAddress("l1Result", &l1Result, &b_l1Result);
   fChain->SetBranchAddress("l1Name", &l1Name, &b_l1Name);
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
