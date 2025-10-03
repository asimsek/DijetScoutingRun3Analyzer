#ifndef analysisClass_h
#define analysisClass_h

#include "baseClass.h"
#include <string>

// --- JEC interfaces used by analysisClass.C ---
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

class analysisClass : public baseClass {
public:
  analysisClass(std::string* inputList,
                std::string* cutFile,
                std::string* treeName,
                std::string* outputFileName = nullptr,
                std::string* cutEfficFile   = nullptr);
  virtual ~analysisClass();

  void Loop();

private:
  // --- On-the-fly JEC (as used in analysisClass.C) ---
  JetCorrectorParameters *L1Par          {nullptr};
  JetCorrectorParameters *L2Par          {nullptr};
  JetCorrectorParameters *L3Par          {nullptr};

  JetCorrectorParameters *L1DATAPar      {nullptr};
  JetCorrectorParameters *L2DATAPar      {nullptr};
  JetCorrectorParameters *L3DATAPar      {nullptr};
  JetCorrectorParameters *L2L3Residual   {nullptr};

  FactorizedJetCorrector *JetCorrector       {nullptr}; // MC
  FactorizedJetCorrector *JetCorrector_data  {nullptr}; // Data

  JetCorrectionUncertainty *unc              {nullptr}; // relative JEC uncertainty
};

#endif // analysisClass_h

