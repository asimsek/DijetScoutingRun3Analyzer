#pragma once

#include <memory>
#include <string>

// Forward decls (headers included in the .C)
class FactorizedJetCorrector;
class JetCorrectionUncertainty;

namespace JECTool {

struct Resolved {
  std::string L1;
  std::string L2;
  std::string L3;
  std::string L2L3Residual;   // empty for MC
  std::string Unc;
  std::string JetVetoMap;     // optional; parsed if present
};

/**
 * Parse a JEC list file and resolve the file paths for a given block (era tag).
 * - listFile: path to the list file (e.g. data/cfg/data_jec_list.txt)
 * - yearTag:  exact tag after 'Year:' in the file (e.g. "2024H", "2024").
 *             If empty, the FIRST block in the file is used.
 * - runNo:    optional; if the Residual line contains bracketed run ranges
 *             (start:end:path), the entry matching runNo is chosen; otherwise
 *             the first/default (-1:-1:...) is used.
 */
Resolved resolveList(const std::string& listFile,
                     const std::string& yearTag = "",
                     long runNo = -1);

/** Construct a FactorizedJetCorrector from the resolved paths. */
std::unique_ptr<FactorizedJetCorrector>
makeCorrector(const Resolved& r, bool forData /* add L2L3Residual if true */);

/** Construct a JetCorrectionUncertainty from the resolved paths. */
std::unique_ptr<JetCorrectionUncertainty>
makeUncertainty(const Resolved& r);

} // namespace JECTool


