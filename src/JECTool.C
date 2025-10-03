#include "JECTool.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

// --- JEC interfaces used by analysisClass.C ---
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

namespace {

inline std::string trim(std::string s) {
  auto not_space = [](int ch){ return !std::isspace(ch); };
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space));
  s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
  return s;
}

inline bool starts_with(const std::string& s, const std::string& p) {
  return s.size() >= p.size() && std::equal(p.begin(), p.end(), s.begin());
}

inline std::string valueAfterColon(const std::string& line) {
  auto pos = line.find(':');
  if (pos == std::string::npos) return "";
  return trim(line.substr(pos + 1));
}

// Parse bracket content like: [ -1:-1:path, 382298:383247:path, ... ]
// Choose entry that matches runNo (start <= runNo < end), or the (-1:-1:...) fallback.
std::string pickResidualFromBracket(const std::string& bracketSpec, long runNo) {
  // Extract inside [ ... ]
  auto l = bracketSpec.find('[');
  auto r = bracketSpec.rfind(']');
  if (l == std::string::npos || r == std::string::npos || r <= l) return trim(bracketSpec);

  std::string inside = bracketSpec.substr(l + 1, r - l - 1);
  // Split by ','
  std::vector<std::string> entries;
  {
    std::stringstream ss(inside);
    std::string tok;
    while (std::getline(ss, tok, ',')) {
      if (!trim(tok).empty()) entries.push_back(trim(tok));
    }
  }

  std::string fallback;
  for (const auto& e : entries) {
    // format: start:end:path
    // allow extra spaces
    std::string item = trim(e);
    // Remove optional leading '-' from singletons like '-1:-1:path'
    std::vector<std::string> parts;
    {
      std::stringstream ss(item);
      std::string p;
      while (std::getline(ss, p, ':')) parts.push_back(trim(p));
    }
    if (parts.size() < 3) continue;

    long start = std::stol(parts[0]);
    long end   = std::stol(parts[1]);
    std::string path = parts[2];

    if (start == -1 && end == -1) {
      // universal default
      fallback = path;
    }
    if (runNo >= 0) {
      bool ge_start = (start == -1) ? true : (runNo >= start);
      bool lt_end   = (end   == -1) ? true : (runNo <  end);
      if (ge_start && lt_end) return path;
    }
  }
  // if no matching range or runNo<0, return fallback (may be empty)
  return fallback;
}

struct Block {
  std::string tag;
  std::string L1, L2, L3, L2L3Residual, Unc, JetVetoMap;
};

} // anonymous

namespace JECTool {

Resolved resolveList(const std::string& listFile,
                     const std::string& yearTag,
                     long runNo)
{
  std::ifstream fin(listFile);
  if (!fin) {
    std::cerr << "[JECTool] ERROR: cannot open list file: " << listFile << std::endl;
    return {};
  }

  std::vector<Block> blocks;
  Block current;
  bool inBlock = false;

  std::string line;
  while (std::getline(fin, line)) {
    std::string s = trim(line);
    if (s.empty()) continue;

    // Comment lines
    if (starts_with(s, "#")) {
      // extract tag after "Year:"
      auto pos = s.find("Year:");
      if (pos != std::string::npos) {
        if (inBlock) {
          if (!current.tag.empty())
            blocks.push_back(current);
          current = Block{};
        }
        inBlock = true;
        std::string after = trim(s.substr(pos + 5));
        // remove trailing ':' if present
        if (!after.empty() && after.back() == ':') after.pop_back();
        current.tag = trim(after);
      }
      continue;
    }

    if (!inBlock) continue;

    if (starts_with(s, "L1FastJet:"))    current.L1  = trim(valueAfterColon(s));
    else if (starts_with(s, "L2Relative:")) current.L2  = trim(valueAfterColon(s));
    else if (starts_with(s, "L3Absolute:")) current.L3  = trim(valueAfterColon(s));
    else if (starts_with(s, "L2L3Residual:")) current.L2L3Residual = trim(valueAfterColon(s));
    else if (starts_with(s, "Unc:"))       current.Unc = trim(valueAfterColon(s));
    else if (starts_with(s, "JetVetoMap:")) current.JetVetoMap = trim(valueAfterColon(s));
  }
  if (inBlock && !current.tag.empty()) blocks.push_back(current);

  if (blocks.empty()) {
    std::cerr << "[JECTool] ERROR: no blocks parsed in " << listFile << std::endl;
    return {};
  }

  // Choose block: exact tag match, otherwise first block
  const Block* chosen = nullptr;
  if (!yearTag.empty()) {
    for (const auto& b : blocks) {
      if (b.tag == yearTag) { chosen = &b; break; }
    }
  }
  if (!chosen) chosen = &blocks.front();

  Resolved out;
  out.L1  = chosen->L1;
  out.L2  = chosen->L2;
  out.L3  = chosen->L3;
  out.Unc = chosen->Unc;
  out.JetVetoMap = chosen->JetVetoMap;

  // L2L3Residual may be bracket list with run ranges
  if (!chosen->L2L3Residual.empty()) {
    if (chosen->L2L3Residual.find('[') != std::string::npos) {
      out.L2L3Residual = pickResidualFromBracket(chosen->L2L3Residual, runNo);
    } else {
      out.L2L3Residual = chosen->L2L3Residual;
    }
  }

  return out;
}

std::unique_ptr<FactorizedJetCorrector>
makeCorrector(const Resolved& r, bool forData)
{
  std::vector<JetCorrectorParameters> v;
  if (!r.L1.empty()) v.emplace_back(r.L1);
  if (!r.L2.empty()) v.emplace_back(r.L2);
  if (!r.L3.empty()) v.emplace_back(r.L3);
  if (forData && !r.L2L3Residual.empty()) v.emplace_back(r.L2L3Residual);

  if (v.empty()) {
    std::cerr << "[JECTool] WARNING: no JEC parameters provided, returning nullptr\n";
    return nullptr;
  }
  return std::make_unique<FactorizedJetCorrector>(v);
}

std::unique_ptr<JetCorrectionUncertainty>
makeUncertainty(const Resolved& r)
{
  if (r.Unc.empty()) {
    std::cerr << "[JECTool] WARNING: empty Unc path, returning nullptr\n";
    return nullptr;
  }
  return std::make_unique<JetCorrectionUncertainty>(r.Unc);
}

} // namespace JECTool


