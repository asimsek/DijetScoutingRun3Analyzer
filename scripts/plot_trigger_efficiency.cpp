#include <TCanvas.h>
#include <TEfficiency.h>
#include <TError.h>
#include <TFile.h>
#include <TGraphAsymmErrors.h>
#include <TH1D.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>
#include <TMath.h>
#include <TPaveText.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include <ROOT/TThreadExecutor.hxx>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <glob.h>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <regex>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>
#include <unistd.h>
#include <wordexp.h>

namespace fs = std::filesystem;

namespace {

constexpr const char* kTreePathDefault = "rootTupleTree/tree";
constexpr const char* kDefaultCmsLabel = "CMS";
constexpr const char* kDefaultExtraLabel = "Preliminary";
constexpr const char* kDefaultSqrtS = "13.6 TeV";
constexpr const char* kAnsiReset = "\033[0m";
constexpr const char* kAnsiBrightBlue = "\033[94m";
constexpr const char* kAnsiBrightCyan = "\033[96m";
constexpr const char* kAnsiBrightRed = "\033[91m";

constexpr double kEtaCut = 2.5;
constexpr double kDeltaEtaCut = 1.3;

constexpr double kMinPtJ1 = 60.0;
constexpr double kMinPtJ2 = 30.0;
constexpr double kMinJetCount = 1.0;

constexpr double kEfficiencyMassDisplayMin = 220.0;
constexpr double kEfficiencyMassDisplayMax = 6000.0;
constexpr double kEfficiencyZoomMassDisplayMin = 100.0;
constexpr double kEfficiencyZoomMassDisplayMax = 650.0;
constexpr double kCountsMassDisplayMin = 0.0;
constexpr double kCountsMassDisplayMax = 6000.0;
constexpr double kEfficiencyYMin = 0.0;
constexpr double kEfficiencyYMax = 1.30;
constexpr int kTurnOnConsecutivePoints = 3;
constexpr size_t kObservableCount = 3;
constexpr int kAk4EvenBinWidthGeV = 10;
constexpr int kAk4EvenBinMinGeV = 0;
constexpr int kAk4EvenBinMaxRequestedGeV = 6000;
constexpr size_t kAk4EvenBinCount =
    (kAk4EvenBinMaxRequestedGeV - kAk4EvenBinMinGeV + kAk4EvenBinWidthGeV - 1) / kAk4EvenBinWidthGeV;
constexpr int kAk4EvenBinMaxGeV = kAk4EvenBinMinGeV + static_cast<int>(kAk4EvenBinCount) * kAk4EvenBinWidthGeV;


const std::array<std::string, 2> kJetHtBranchCandidates = {
    "passHLT_PFScoutingHT",
    "DST_PFScouting_JetHT",
};

const std::array<std::string, 2> kSingleMuonBranchCandidates = {
    "passHLT_PFScouting_SingleMuon",
    "DST_PFScouting_SingleMuon",
};

const std::vector<double> kMassBins = {
    1, 3, 6, 10, 16, 23, 31, 40, 50, 61, 74, 88, 103, 119, 137, 156, 176, 197, 220,
    244, 270, 296, 325, 354, 386, 419, 453, 489, 526, 565, 606, 649, 693, 740, 788,
    838, 890, 944, 1000, 1058, 1118, 1181, 1246, 1313, 1383, 1455, 1530, 1607, 1687,
    1770, 1856, 1945, 2037, 2132, 2231, 2332, 2438, 2546, 2659, 2775, 2895, 3019,
    3147, 3279, 3416, 3558, 3704, 3854, 4010, 4171, 4337, 4509, 4686, 4869, 5058,
    5253, 5455, 5663, 5877, 6099, 6328, 6564, 6808, 7060, 7320, 7589, 7866, 8152,
    8447, 8752, 9067, 9391, 9726, 10072, 10430, 10798, 11179, 11571, 11977, 12395,
    12827, 13272, 13732, 14000,
};

enum class SelectionMode {
  kWideJet,
  kAk4,
};

struct Options {
  std::string monitoring_list;
  std::string scouting_list;
  std::string output_dir;
  std::string output_prefix = "pfscouting_trigger_";
  double lumi_pb = 5490.0;
  std::string year = "2024";
  int threads = 8;
  bool use_scouting_for_efficiency = false;
};

struct SummaryRow {
  int mass_low = 0;
  int mass_high = 0;
  std::string mass_range;
  double efficiency = std::numeric_limits<double>::quiet_NaN();
  double eff_err_low = std::numeric_limits<double>::quiet_NaN();
  double eff_err_up = std::numeric_limits<double>::quiet_NaN();
  double inefficiency = std::numeric_limits<double>::quiet_NaN();
  double inv_sqrt_n = 0.0;
  double diff_stat_minus_ineff = std::numeric_limits<double>::quiet_NaN();
  double denominator_count = 0.0;
  double numerator_count = 0.0;
  double scouting_count = 0.0;
};

struct MassObservable {
  std::string tag;
  std::string mass_branch;
  std::string jet_label;
  std::string axis_title;
  SelectionMode selection_mode = SelectionMode::kWideJet;
};

struct ProcessingTask {
  std::string file_name;
  bool use_for_efficiency = false;
  bool use_for_scouting = false;
  std::string jetht_branch;
  std::string singlemuon_branch;
};

struct TaskCounts {
  std::vector<double> denominator;
  std::vector<double> numerator;
  std::vector<double> scouting;

  TaskCounts()
      : denominator(kMassBins.size() - 1, 0.0),
        numerator(kMassBins.size() - 1, 0.0),
        scouting(kMassBins.size() - 1, 0.0) {}
};

struct UniformTaskCounts {
  std::vector<double> denominator;
  std::vector<double> numerator;
  std::vector<double> scouting;

  UniformTaskCounts()
      : denominator(kAk4EvenBinCount, 0.0),
        numerator(kAk4EvenBinCount, 0.0),
        scouting(kAk4EvenBinCount, 0.0) {}
};

struct AllObservableTaskCounts {
  std::array<TaskCounts, kObservableCount> observables;
  UniformTaskCounts ak4_even;
};

struct ObservableOutputPaths {
  fs::path directory;
  fs::path root_file;
  fs::path summary_csv;
  fs::path summary_tsv;
  fs::path eff_pdf;
  fs::path eff_zoom_pdf;
  fs::path eff_even_pdf;
  fs::path eff_even_zoom_pdf;
  fs::path counts_pdf;
  std::string stem;
};

const std::array<MassObservable, kObservableCount> kMassObservables = {{
    {"wideJet", "mjj", "Wide PF-jets", "Wide-jet dijet mass [TeV]", SelectionMode::kWideJet},
    {"AK4PFJet", "Dijet_MassAK4PF", "AK4 PF-jets", "AK4 PF dijet mass [TeV]", SelectionMode::kAk4},
    {"HTAK4PF", "HTAK4PF", "AK4 PF H_{T}", "AK4 PF H_{T} [TeV]", SelectionMode::kAk4},
}};

static_assert(kMassObservables.size() == kObservableCount, "Observable count mismatch");

[[noreturn]] void die(const std::string& message) {
  throw std::runtime_error(message);
}

void print_usage(const char* argv0) {
  std::cout
      << "Usage: " << argv0
      << " --scouting-list <file> [options]\n\n"
      << "Options:\n"
      << "  --monitoring-list <file>   Reduced nTuple list for ScoutingPFMonitoring data.\n"
      << "                            Required unless --use-scouting-for-efficiency is given.\n"
      << "  --scouting-list <file>     Reduced nTuple list for regular PFScouting data used for 1/sqrt(N).\n"
      << "  --use-scouting-for-efficiency\n"
      << "                            Use --scouting-list for both efficiency and 1/sqrt(N) in one pass.\n"
      << "  --output-dir <dir>         Output directory (default: trigger_efficiency_<year>_deta1p3).\n"
      << "  --output-prefix <prefix>   Prefix for output file names (default: pfscouting_trigger_).\n"
      << "  --lumi-pb <value>          Integrated luminosity in pb^-1 (default: 5490).\n"
      << "  --year <label>             Data-taking year label shown in legend (default: 2024).\n"
      << "  --threads <n>              ROOT implicit MT thread count, 0 disables MT (default: 8).\n"
      << "  --help                     Show this help message.\n";
}

Options parse_args(int argc, char** argv) {
  Options opts;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    auto require_value = [&](const std::string& name) -> std::string {
      if (i + 1 >= argc) {
        die("Missing value for argument: " + name);
      }
      return argv[++i];
    };

    if (arg == "--monitoring-list") {
      opts.monitoring_list = require_value(arg);
    } else if (arg == "--scouting-list") {
      opts.scouting_list = require_value(arg);
    } else if (arg == "--use-scouting-for-efficiency") {
      opts.use_scouting_for_efficiency = true;
    } else if (arg == "--output-dir") {
      opts.output_dir = require_value(arg);
    } else if (arg == "--output-prefix") {
      opts.output_prefix = require_value(arg);
    } else if (arg == "--lumi-pb") {
      opts.lumi_pb = std::stod(require_value(arg));
    } else if (arg == "--year") {
      opts.year = require_value(arg);
    } else if (arg == "--threads") {
      opts.threads = std::stoi(require_value(arg));
    } else if (arg == "--help" || arg == "-h") {
      print_usage(argv[0]);
      std::exit(0);
    } else {
      die("Unknown argument: " + arg);
    }
  }

  if (!opts.use_scouting_for_efficiency && opts.monitoring_list.empty()) {
    die("--monitoring-list is required");
  }
  if (opts.scouting_list.empty()) {
    die("--scouting-list is required");
  }
  return opts;
}

std::string colored_tag(const std::string& tag, const char* color);
std::string sanitize_year_tag(const std::string& year);

void print_stage(int current, int total, const std::string& message) {
  std::cout << colored_tag("[PROGRESS]", kAnsiBrightBlue) << " [" << current << "/" << total << "] " << message
            << std::endl;
}

bool stdout_is_tty() {
#if defined(_WIN32)
  return false;
#else
  return ::isatty(fileno(stdout));
#endif
}

std::string colored_tag(const std::string& tag, const char* color) {
  if (stdout_is_tty()) {
    return std::string(color) + tag + kAnsiReset;
  }
  return tag;
}

std::string info_tag() {
  return colored_tag("[INFO]", kAnsiBrightCyan);
}

std::string warn_tag() {
  return colored_tag("[WARN]", kAnsiBrightRed);
}

std::string format_elapsed_time(double elapsed_seconds) {
  int total_seconds = std::max(0, static_cast<int>(elapsed_seconds));
  int hours = total_seconds / 3600;
  int remainder = total_seconds % 3600;
  int minutes = remainder / 60;
  int seconds = remainder % 60;
  std::ostringstream os;
  if (hours > 0) {
    os << hours << " hour(s) ";
  }
  if (minutes > 0 || hours > 0) {
    os << minutes << " minute(s) ";
  }
  os << seconds << " second(s)";
  return os.str();
}

bool has_wildcards(const std::string& text) {
  return text.find_first_of("*?[") != std::string::npos;
}

std::string expand_user_and_env(const std::string& token) {
  wordexp_t expansion;
  std::memset(&expansion, 0, sizeof(expansion));
  const int rc = wordexp(token.c_str(), &expansion, WRDE_NOCMD);
  if (rc == 0 && expansion.we_wordc > 0) {
    std::string result = expansion.we_wordv[0];
    wordfree(&expansion);
    return result;
  }
  if (rc == 0) {
    wordfree(&expansion);
  }
  return token;
}

std::string normalize_path_token(const std::string& token, const fs::path& base_dir) {
  if (token.find("://") != std::string::npos) {
    return token;
  }
  fs::path candidate = expand_user_and_env(token);
  if (candidate.is_relative()) {
    candidate = base_dir / candidate;
  }
  return candidate.lexically_normal().string();
}

std::vector<std::string> expand_glob(const std::string& pattern) {
  glob_t result;
  std::memset(&result, 0, sizeof(result));
  std::vector<std::string> matches;
  const int rc = glob(pattern.c_str(), GLOB_TILDE, nullptr, &result);
  if (rc == 0) {
    matches.reserve(result.gl_pathc);
    for (size_t i = 0; i < result.gl_pathc; ++i) {
      matches.emplace_back(result.gl_pathv[i]);
    }
  }
  globfree(&result);
  std::sort(matches.begin(), matches.end());
  return matches;
}

std::vector<std::string> expand_source_token(const std::string& token, const fs::path& base_dir) {
  std::vector<std::string> out;
  const std::string normalized = normalize_path_token(token, base_dir);
  if (normalized.find("://") != std::string::npos) {
    out.push_back(normalized);
    return out;
  }
  if (has_wildcards(normalized)) {
    return expand_glob(normalized);
  }

  fs::path path(normalized);
  if (fs::is_directory(path)) {
    for (const auto& entry : fs::recursive_directory_iterator(path)) {
      if (!entry.is_regular_file()) {
        continue;
      }
      const std::string name = entry.path().filename().string();
      if (name.size() >= 18 && name.rfind("_reduced_skim.root") == name.size() - 18) {
        out.push_back(entry.path().string());
      }
    }
    std::sort(out.begin(), out.end());
    return out;
  }

  out.push_back(path.string());
  return out;
}

std::vector<std::string> deduplicate(const std::vector<std::string>& seq) {
  std::unordered_set<std::string> seen;
  std::vector<std::string> ordered;
  ordered.reserve(seq.size());
  for (const auto& item : seq) {
    if (seen.insert(item).second) {
      ordered.push_back(item);
    }
  }
  return ordered;
}

std::vector<std::string> read_data_inputs(const std::string& list_path) {
  const fs::path path = fs::absolute(list_path);
  std::ifstream handle(path);
  if (!handle) {
    die("Could not open list file: " + path.string());
  }

  std::vector<std::string> files;
  std::string line;
  while (std::getline(handle, line)) {
    line = std::regex_replace(line, std::regex("^\\s+|\\s+$"), "");
    if (line.empty() || line.front() == '#') {
      continue;
    }
    const auto expanded = expand_source_token(line, path.parent_path());
    files.insert(files.end(), expanded.begin(), expanded.end());
  }
  files = deduplicate(files);
  if (files.empty()) {
    die("No data ROOT files found from list: " + path.string());
  }
  return files;
}

void configure_root_style() {
  gROOT->SetBatch(kTRUE);
  TH1::SetDefaultSumw2(kTRUE);
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetPadTopMargin(0.06);
  gStyle->SetPadBottomMargin(0.13);
  gStyle->SetPadLeftMargin(0.16);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetLegendBorderSize(0);
  gStyle->SetLegendFont(42);
  gStyle->SetLabelFont(42, "XYZ");
  gStyle->SetTitleFont(42, "XYZ");
  gStyle->SetTitleSize(0.055, "XYZ");
  gStyle->SetLabelSize(0.045, "XYZ");
  gStyle->SetTitleYOffset(1.35);
  gStyle->SetEndErrorSize(0);
}

std::set<std::string> get_branch_names(TTree& tree) {
  std::set<std::string> names;
  TObjArray* branches = tree.GetListOfBranches();
  if (!branches) {
    return names;
  }
  for (int i = 0; i < branches->GetEntries(); ++i) {
    auto* branch = dynamic_cast<TBranch*>(branches->At(i));
    if (branch) {
      names.insert(branch->GetName());
    }
  }
  return names;
}

std::unique_ptr<TFile> open_input_file(const std::string& file_name) {
  std::unique_ptr<TFile> file(TFile::Open(file_name.c_str(), "READ"));
  if (!file || file->IsZombie()) {
    die("Could not open ROOT file: " + file_name);
  }
  return file;
}

TTree* get_input_tree(TFile& file, const std::string& file_name) {
  auto* tree = dynamic_cast<TTree*>(file.Get(kTreePathDefault));
  if (!tree) {
    die("Could not find tree '" + std::string(kTreePathDefault) + "' in file: " + file_name);
  }
  return tree;
}

std::string find_branch_name_in_file(const std::string& file_name, const std::array<std::string, 2>& candidates,
                                     const std::string& label) {
  auto file = open_input_file(file_name);
  TTree* tree = get_input_tree(*file, file_name);
  const auto branch_names = get_branch_names(*tree);
  for (const auto& candidate : candidates) {
    if (branch_names.count(candidate)) {
      return candidate;
    }
  }
  std::ostringstream os;
  os << "Could not find a " << label << " trigger branch in " << kTreePathDefault << ". Tried: ";
  for (size_t i = 0; i < candidates.size(); ++i) {
    if (i != 0) {
      os << ", ";
    }
    os << candidates[i];
  }
  die(os.str());
}

void configure_tree_for_reading(TTree& tree, const std::set<std::string>& enabled_branches) {
  const auto branch_names = get_branch_names(tree);
  tree.SetBranchStatus("*", 0);
  for (const auto& branch_name : enabled_branches) {
    if (branch_names.count(branch_name)) {
      tree.SetBranchStatus(branch_name.c_str(), 1);
    }
  }
  tree.SetCacheSize(128LL * 1024LL * 1024LL);
  tree.SetCacheLearnEntries(10);
  for (const auto& branch_name : enabled_branches) {
    if (branch_names.count(branch_name)) {
      tree.AddBranchToCache(branch_name.c_str(), kTRUE);
    }
  }
}

void require_branch_name(const std::set<std::string>& branch_names, const std::string& branch_name,
                         const std::string& file_name) {
  if (!branch_names.count(branch_name)) {
    die("Required branch '" + branch_name + "' was not found in file: " + file_name);
  }
}

int find_mass_bin(double mjj) {
  if (mjj >= kMassBins.back()) {
    return static_cast<int>(kMassBins.size()) - 2;
  }
  auto upper = std::upper_bound(kMassBins.begin(), kMassBins.end(), mjj);
  if (upper == kMassBins.begin() || upper == kMassBins.end()) {
    return -1;
  }
  return static_cast<int>(std::distance(kMassBins.begin(), upper) - 1);
}

int find_ak4_even_bin(double value_gev) {
  if (value_gev < kAk4EvenBinMinGeV) {
    return -1;
  }
  if (value_gev >= kAk4EvenBinMaxGeV) {
    return static_cast<int>(kAk4EvenBinCount) - 1;
  }
  return static_cast<int>((value_gev - kAk4EvenBinMinGeV) / static_cast<double>(kAk4EvenBinWidthGeV));
}

bool passes_widejet_selection(double pass_json, double n_jet, double id_tight_j1, double id_tight_j2, double pt_j1,
                              double pt_j2, double eta_j1, double eta_j2, double delta_eta_jj) {
  return pass_json > 0.5 && n_jet > kMinJetCount && id_tight_j1 > 0.5 &&
         id_tight_j2 > 0.5 && pt_j1 > kMinPtJ1 && pt_j2 > kMinPtJ2 && std::abs(eta_j1) < kEtaCut &&
         std::abs(eta_j2) < kEtaCut && std::abs(delta_eta_jj) < kDeltaEtaCut;
}

bool passes_ak4_selection(double pass_json, double n_ak4, double id_tight_j1, double id_tight_j2, double pt_j1,
                          double pt_j2, double eta_j1, double eta_j2, double delta_eta_jj) {
  return pass_json > 0.5 && n_ak4 > kMinJetCount && id_tight_j1 > 0.5 &&
         id_tight_j2 > 0.5 && pt_j1 > kMinPtJ1 && pt_j2 > kMinPtJ2 && std::abs(eta_j1) < kEtaCut &&
         std::abs(eta_j2) < kEtaCut && std::abs(delta_eta_jj) < kDeltaEtaCut;
}

std::string selection_summary(const MassObservable& observable) {
  std::ostringstream os;
  if (observable.selection_mode == SelectionMode::kWideJet) {
    os << "PassJSON == 1 && nJet > 1 && IdTight_j1 > 0.5 && IdTight_j2 > 0.5"
       << " && pTWJ_j1 > " << kMinPtJ1 << " && pTWJ_j2 > " << kMinPtJ2
       << " && TMath::Abs(etaWJ_j1) < " << kEtaCut
       << " && TMath::Abs(etaWJ_j2) < " << kEtaCut
       << " && TMath::Abs(deltaETAjj) < " << kDeltaEtaCut;
  } else {
    os << "PassJSON == 1 && NAK4PF > 1 && IdTight_j1 > 0.5 && IdTight_j2 > 0.5"
       << " && pTAK4PF_j1 > " << kMinPtJ1 << " && pTAK4PF_j2 > " << kMinPtJ2
       << " && TMath::Abs(etaAK4PF_j1) < " << kEtaCut
       << " && TMath::Abs(etaAK4PF_j2) < " << kEtaCut
       << " && TMath::Abs(deltaETAjjAK4PF) < " << kDeltaEtaCut;
  }
  return os.str();
}

AllObservableTaskCounts process_file_task(const ProcessingTask& task) {
  AllObservableTaskCounts counts;
  auto file = open_input_file(task.file_name);
  TTree* tree = get_input_tree(*file, task.file_name);
  const auto branch_names = get_branch_names(*tree);
  require_branch_name(branch_names, task.jetht_branch, task.file_name);
  if (task.use_for_efficiency) {
    require_branch_name(branch_names, task.singlemuon_branch, task.file_name);
  }
  for (const auto& observable : kMassObservables) {
    require_branch_name(branch_names, observable.mass_branch, task.file_name);
  }

  std::set<std::string> enabled_branches = {
      "PassJSON",         "nJet",        "NAK4PF",      "IdTight_j1",      "IdTight_j2",
      "pTWJ_j1",          "pTWJ_j2",     "etaWJ_j1",    "etaWJ_j2",        "deltaETAjj",
      "pTAK4PF_j1",       "pTAK4PF_j2",  "etaAK4PF_j1", "etaAK4PF_j2",     "deltaETAjjAK4PF",
      task.jetht_branch};
  for (const auto& observable : kMassObservables) {
    enabled_branches.insert(observable.mass_branch);
  }
  if (task.use_for_efficiency) {
    enabled_branches.insert(task.singlemuon_branch);
  }
  configure_tree_for_reading(*tree, enabled_branches);

  TTreeReader reader(tree);
  TTreeReaderValue<double> pass_json(reader, "PassJSON");
  TTreeReaderValue<double> n_jet(reader, "nJet");
  TTreeReaderValue<double> n_ak4(reader, "NAK4PF");
  TTreeReaderValue<double> id_tight_j1(reader, "IdTight_j1");
  TTreeReaderValue<double> id_tight_j2(reader, "IdTight_j2");
  TTreeReaderValue<double> pt_wj1(reader, "pTWJ_j1");
  TTreeReaderValue<double> pt_wj2(reader, "pTWJ_j2");
  TTreeReaderValue<double> eta_wj1(reader, "etaWJ_j1");
  TTreeReaderValue<double> eta_wj2(reader, "etaWJ_j2");
  TTreeReaderValue<double> delta_eta_wj(reader, "deltaETAjj");
  TTreeReaderValue<double> pt_ak4_j1(reader, "pTAK4PF_j1");
  TTreeReaderValue<double> pt_ak4_j2(reader, "pTAK4PF_j2");
  TTreeReaderValue<double> eta_ak4_j1(reader, "etaAK4PF_j1");
  TTreeReaderValue<double> eta_ak4_j2(reader, "etaAK4PF_j2");
  TTreeReaderValue<double> delta_eta_ak4(reader, "deltaETAjjAK4PF");
  TTreeReaderValue<double> mjj(reader, kMassObservables[0].mass_branch.c_str());
  TTreeReaderValue<double> dijet_mass_ak4(reader, kMassObservables[1].mass_branch.c_str());
  TTreeReaderValue<double> ht_ak4(reader, kMassObservables[2].mass_branch.c_str());
  TTreeReaderValue<double> jetht(reader, task.jetht_branch.c_str());
  std::unique_ptr<TTreeReaderValue<double>> single_muon;
  if (task.use_for_efficiency) {
    single_muon = std::make_unique<TTreeReaderValue<double>>(reader, task.singlemuon_branch.c_str());
  }

  while (reader.Next()) {
    const bool pass_jetht = *jetht > 0.5;
    const bool pass_single_muon = task.use_for_efficiency ? ((**single_muon) > 0.5) : false;
    const bool contributes_to_efficiency = task.use_for_efficiency && pass_single_muon;
    const bool contributes_to_scouting = task.use_for_scouting && pass_jetht;
    if (!contributes_to_efficiency && !contributes_to_scouting) {
      continue;
    }

    const std::array<double, kObservableCount> mass_values = {
        *mjj,
        *dijet_mass_ak4,
        *ht_ak4,
    };
    const std::array<bool, kObservableCount> pass_selection = {
        passes_widejet_selection(*pass_json, *n_jet, *id_tight_j1, *id_tight_j2, *pt_wj1, *pt_wj2, *eta_wj1,
                                 *eta_wj2, *delta_eta_wj),
        passes_ak4_selection(*pass_json, *n_ak4, *id_tight_j1, *id_tight_j2, *pt_ak4_j1, *pt_ak4_j2,
                             *eta_ak4_j1, *eta_ak4_j2, *delta_eta_ak4),
        passes_ak4_selection(*pass_json, *n_ak4, *id_tight_j1, *id_tight_j2, *pt_ak4_j1, *pt_ak4_j2,
                             *eta_ak4_j1, *eta_ak4_j2, *delta_eta_ak4),
    };

    bool any_selection_passed = false;
    for (bool pass : pass_selection) {
      any_selection_passed = any_selection_passed || pass;
    }
    if (!any_selection_passed) {
      continue;
    }

    for (size_t observable_index = 0; observable_index < kObservableCount; ++observable_index) {
      if (!pass_selection[observable_index]) {
        continue;
      }
      const int bin_index = find_mass_bin(mass_values[observable_index]);
      if (bin_index < 0) {
        continue;
      }

      TaskCounts& observable_counts = counts.observables[observable_index];
      if (contributes_to_efficiency) {
        observable_counts.denominator[bin_index] += 1.0;
        if (pass_jetht) {
          observable_counts.numerator[bin_index] += 1.0;
        }
      }
      if (contributes_to_scouting) {
        observable_counts.scouting[bin_index] += 1.0;
      }
    }

    const int ak4_even_bin_index = find_ak4_even_bin(*dijet_mass_ak4);
    if (pass_selection[1] && ak4_even_bin_index >= 0) {
      if (contributes_to_efficiency) {
        counts.ak4_even.denominator[ak4_even_bin_index] += 1.0;
        if (pass_jetht) {
          counts.ak4_even.numerator[ak4_even_bin_index] += 1.0;
        }
      }
      if (contributes_to_scouting) {
        counts.ak4_even.scouting[ak4_even_bin_index] += 1.0;
      }
    }
  }

  return counts;
}

AllObservableTaskCounts process_all_files(const std::vector<ProcessingTask>& tasks, int threads, const std::string& label) {
  if (tasks.empty()) {
    die("No files were provided for " + label);
  }

  ROOT::TThreadExecutor executor(threads > 0 ? threads : 1);
  std::atomic<size_t> processed{0};
  std::mutex io_mutex;
  const size_t total = tasks.size();
  const size_t report_every = std::max<size_t>(1, total / 20);

  auto task_runner = [&](const ProcessingTask& task) {
    AllObservableTaskCounts counts = process_file_task(task);
    const size_t done = processed.fetch_add(1) + 1;
    if (done == total || done % report_every == 0) {
      std::lock_guard<std::mutex> lock(io_mutex);
      std::cout << colored_tag("[PROGRESS]", kAnsiBrightBlue) << ' ' << label << " event loop: [" << done << "/"
                << total << "] "
                << fs::path(task.file_name).filename().string() << std::endl;
    }
    return counts;
  };

  AllObservableTaskCounts merged;
  const auto partials = executor.Map(task_runner, tasks);
  for (const auto& partial : partials) {
    for (size_t observable_index = 0; observable_index < kObservableCount; ++observable_index) {
      TaskCounts& merged_counts = merged.observables[observable_index];
      const TaskCounts& partial_counts = partial.observables[observable_index];
      for (size_t i = 0; i < merged_counts.denominator.size(); ++i) {
        merged_counts.denominator[i] += partial_counts.denominator[i];
        merged_counts.numerator[i] += partial_counts.numerator[i];
        merged_counts.scouting[i] += partial_counts.scouting[i];
      }
    }
    for (size_t i = 0; i < merged.ak4_even.denominator.size(); ++i) {
      merged.ak4_even.denominator[i] += partial.ak4_even.denominator[i];
      merged.ak4_even.numerator[i] += partial.ak4_even.numerator[i];
      merged.ak4_even.scouting[i] += partial.ak4_even.scouting[i];
    }
  }
  return merged;
}

std::unique_ptr<TH1D> make_hist_from_counts(const std::string& name, const std::vector<double>& counts) {
  auto hist = std::make_unique<TH1D>(name.c_str(), "", static_cast<int>(kMassBins.size()) - 1, kMassBins.data());
  hist->SetDirectory(nullptr);
  hist->Sumw2();
  for (size_t i = 0; i < counts.size(); ++i) {
    hist->SetBinContent(static_cast<int>(i) + 1, counts[i]);
    hist->SetBinError(static_cast<int>(i) + 1, std::sqrt(counts[i]));
  }
  return hist;
}

std::unique_ptr<TH1D> make_uniform_hist_from_counts(const std::string& name, const std::vector<double>& counts,
                                                    int nbins, double xmin, double xmax) {
  auto hist = std::make_unique<TH1D>(name.c_str(), "", nbins, xmin, xmax);
  hist->SetDirectory(nullptr);
  hist->Sumw2();
  for (size_t i = 0; i < counts.size(); ++i) {
    hist->SetBinContent(static_cast<int>(i) + 1, counts[i]);
    hist->SetBinError(static_cast<int>(i) + 1, std::sqrt(counts[i]));
  }
  return hist;
}

std::string sanitize_year_tag(const std::string& year) {
  return std::regex_replace(year, std::regex(R"([^A-Za-z0-9_.-]+)"), "_");
}

std::string format_lumi_fb(double lumi_pb) {
  std::ostringstream os;
  os << static_cast<int>(lumi_pb / 1000.0);
  return os.str();
}

std::string format_lumi_label(double lumi_pb) {
  std::ostringstream os;
  os << static_cast<int>(lumi_pb / 1000.0) << " fb^{-1} (" << kDefaultSqrtS << ")";
  return os.str();
}

std::string format_double_value(double value, int precision) {
  std::ostringstream os;
  os << std::fixed << std::setprecision(precision) << value;
  return os.str();
}

std::string format_cut_value(double value) {
  std::ostringstream os;
  os << std::fixed << std::setprecision(1) << value;
  return os.str();
}

std::string ak4_even_bin_suffix() {
  return "even" + std::to_string(kAk4EvenBinWidthGeV) + "GeV";
}

std::string ak4_even_bin_label() {
  return std::to_string(kAk4EvenBinWidthGeV) + " GeV";
}

std::string format_cut_tag(double value) {
  std::string text = format_cut_value(value);
  std::replace(text.begin(), text.end(), '.', 'p');
  return text;
}

std::string default_output_dir_name(const std::string& year) {
  return "trigger_efficiency_" + sanitize_year_tag(year) + "_deta" + format_cut_tag(kDeltaEtaCut);
}

std::string format_table_value(double value) {
  if (std::isnan(value)) {
    return "";
  }
  std::ostringstream os;
  os << std::fixed << std::setprecision(9) << value;
  return os.str();
}

std::vector<SummaryRow> build_summary_rows(TEfficiency& efficiency, const TH1D& denom_hist, const TH1D& numer_hist,
                                           const TH1D& scouting_hist) {
  std::vector<SummaryRow> rows;
  rows.reserve(denom_hist.GetNbinsX());
  const TAxis* xaxis = denom_hist.GetXaxis();
  for (int bin_idx = 1; bin_idx <= denom_hist.GetNbinsX(); ++bin_idx) {
    SummaryRow row;
    row.mass_low = static_cast<int>(std::lround(xaxis->GetBinLowEdge(bin_idx)));
    row.mass_high = static_cast<int>(std::lround(xaxis->GetBinUpEdge(bin_idx)));
    row.mass_range = std::to_string(row.mass_low) + "-" + std::to_string(row.mass_high);
    row.denominator_count = denom_hist.GetBinContent(bin_idx);
    row.numerator_count = numer_hist.GetBinContent(bin_idx);
    row.scouting_count = scouting_hist.GetBinContent(bin_idx);
    if (row.denominator_count > 0.0) {
      row.efficiency = efficiency.GetEfficiency(bin_idx);
      row.eff_err_low = efficiency.GetEfficiencyErrorLow(bin_idx);
      row.eff_err_up = efficiency.GetEfficiencyErrorUp(bin_idx);
      row.inefficiency = 1.0 - row.efficiency;
    }
    row.inv_sqrt_n = (row.scouting_count > 0.0) ? 1.0 / std::sqrt(row.scouting_count) : 0.0;
    if (!std::isnan(row.inefficiency)) {
      row.diff_stat_minus_ineff = row.inv_sqrt_n - row.inefficiency;
    }
    rows.push_back(row);
  }
  return rows;
}

void write_summary_csv(const fs::path& path, const std::vector<SummaryRow>& rows) {
  std::ofstream out(path);
  if (!out) {
    die("Could not write summary CSV: " + path.string());
  }
  out << "mass_range,mass_low,mass_high,efficiency,eff_err_low,eff_err_up,inefficiency,inv_sqrt_n,diff_stat_minus_ineff,denominator_count,numerator_count,scouting_count\n";
  for (const auto& row : rows) {
    out << row.mass_range << ','
        << row.mass_low << ','
        << row.mass_high << ','
        << format_table_value(row.efficiency) << ','
        << format_table_value(row.eff_err_low) << ','
        << format_table_value(row.eff_err_up) << ','
        << format_table_value(row.inefficiency) << ','
        << format_table_value(row.inv_sqrt_n) << ','
        << format_table_value(row.diff_stat_minus_ineff) << ','
        << static_cast<long long>(std::llround(row.denominator_count)) << ','
        << static_cast<long long>(std::llround(row.numerator_count)) << ','
        << static_cast<long long>(std::llround(row.scouting_count)) << '\n';
  }
}

void write_summary_tsv(const fs::path& path, const std::vector<SummaryRow>& rows) {
  std::ofstream out(path);
  if (!out) {
    die("Could not write summary TSV: " + path.string());
  }
  out << "Mass Range [GeV]\tEff.\tIneff.\t1/sqrtN\tDiff\tDenominator\tNumerator\tScouting\n";
  for (const auto& row : rows) {
    out << row.mass_range << '\t'
        << format_table_value(row.efficiency) << '\t'
        << format_table_value(row.inefficiency) << '\t'
        << format_table_value(row.inv_sqrt_n) << '\t'
        << format_table_value(row.diff_stat_minus_ineff) << '\t'
        << static_cast<long long>(std::llround(row.denominator_count)) << '\t'
        << static_cast<long long>(std::llround(row.numerator_count)) << '\t'
        << static_cast<long long>(std::llround(row.scouting_count)) << '\n';
  }
}

std::unique_ptr<TGraphAsymmErrors> graph_to_tev(const TGraphAsymmErrors& source, const std::string& name) {
  auto graph = std::make_unique<TGraphAsymmErrors>(source);
  graph->SetName(name.c_str());
  for (int i = 0; i < graph->GetN(); ++i) {
    graph->GetX()[i] /= 1000.0;
    graph->GetEXlow()[i] /= 1000.0;
    graph->GetEXhigh()[i] /= 1000.0;
  }
  return graph;
}

std::unique_ptr<TH1D> hist_to_tev(const TH1D& source, const std::string& name) {
  std::vector<double> tev_bins;
  const TAxis* xaxis = source.GetXaxis();
  tev_bins.reserve(source.GetNbinsX() + 1);
  for (int bin = 1; bin <= source.GetNbinsX(); ++bin) {
    tev_bins.push_back(xaxis->GetBinLowEdge(bin) / 1000.0);
  }
  tev_bins.push_back(xaxis->GetBinUpEdge(source.GetNbinsX()) / 1000.0);
  auto hist = std::make_unique<TH1D>(name.c_str(), "", static_cast<int>(tev_bins.size()) - 1, tev_bins.data());
  hist->SetDirectory(nullptr);
  for (int bin = 1; bin <= source.GetNbinsX(); ++bin) {
    hist->SetBinContent(bin, source.GetBinContent(bin));
    hist->SetBinError(bin, source.GetBinError(bin));
  }
  return hist;
}

void draw_labels_right(const std::string& right_label) {
  TLatex latex;
  latex.SetNDC(true);
  latex.SetTextAlign(13);
  latex.SetTextFont(62);
  latex.SetTextSize(0.055);
  latex.DrawLatex(0.20, 0.88, kDefaultCmsLabel);
  latex.SetTextFont(52);
  latex.SetTextSize(0.040);
  latex.DrawLatex(0.20, 0.82, kDefaultExtraLabel);
  latex.SetTextFont(42);
  latex.SetTextAlign(31);
  latex.SetTextSize(0.040);
  latex.DrawLatex(0.95, 0.94, right_label.c_str());
}

void draw_efficiency_plot(TGraphAsymmErrors& graph, const fs::path& output_pdf, TFile& root_out, double lumi_pb,
                          const std::string& year, double x_min_gev, double x_max_gev,
                          const MassObservable& observable, const std::string& plot_tag, int marker_style = 20,
                          int marker_color = kBlack, bool draw_turnon_lines = false) {
  TCanvas canvas(("c_trigger_efficiency_" + observable.tag + "_" + plot_tag).c_str(), "", 800, 800);
  canvas.cd();
  gPad->SetTopMargin(0.08);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.16);
  gPad->SetBottomMargin(0.13);

  auto graph_tev = graph_to_tev(graph, "g_pfscouting_jetht_efficiency_tev_" + observable.tag + "_" + plot_tag);
  graph_tev->SetTitle("");
  graph_tev->GetXaxis()->SetTitle(observable.axis_title.c_str());
  graph_tev->GetYaxis()->SetTitle("Trigger efficiency");
  graph_tev->GetXaxis()->SetLimits(x_min_gev / 1000.0, x_max_gev / 1000.0);
  graph_tev->GetXaxis()->SetTitleSize(0.05);
  graph_tev->GetYaxis()->SetTitleSize(0.05);
  graph_tev->GetXaxis()->SetLabelSize(0.045);
  graph_tev->GetYaxis()->SetLabelSize(0.045);
  graph_tev->GetYaxis()->SetTitleOffset(1.35);
  graph_tev->SetMarkerStyle(marker_style);
  graph_tev->SetMarkerSize(1.0);
  graph_tev->SetMarkerColor(marker_color);
  graph_tev->SetLineColor(marker_color);
  graph_tev->SetMinimum(kEfficiencyYMin);
  graph_tev->SetMaximum(kEfficiencyYMax);
  graph_tev->Draw("AP");

  std::vector<std::unique_ptr<TLine>> turnon_lines;
  if (draw_turnon_lines) {
    auto find_turnon_x = [&](double threshold) -> double {
      const int n_points = graph_tev->GetN();
      if (n_points <= 0) {
        return std::numeric_limits<double>::quiet_NaN();
      }

      const int required_points = std::min(kTurnOnConsecutivePoints, n_points);
      for (int i = 0; i < graph_tev->GetN(); ++i) {
        bool sustained = true;
        for (int j = 0; j < required_points; ++j) {
          const int idx = i + j;
          if (idx >= n_points || graph_tev->GetY()[idx] < threshold) {
            sustained = false;
            break;
          }
        }
        if (sustained) {
          return graph_tev->GetX()[i];
        }
      }
      return std::numeric_limits<double>::quiet_NaN();
    };

    const double x99 = find_turnon_x(0.99);
    const double x100 = find_turnon_x(0.999999);

    if (!std::isnan(x99)) {
      auto line99 = std::make_unique<TLine>(x99, kEfficiencyYMin, x99, kEfficiencyYMax);
      line99->SetLineColor(kCyan + 1);
      line99->SetLineStyle(2);
      line99->SetLineWidth(2);
      line99->Draw();
      turnon_lines.push_back(std::move(line99));
    }
    if (!std::isnan(x100)) {
      auto line100 = std::make_unique<TLine>(x100, kEfficiencyYMin, x100, kEfficiencyYMax);
      line100->SetLineColor(kRed + 1);
      line100->SetLineStyle(2);
      line100->SetLineWidth(2);
      line100->Draw();
      turnon_lines.push_back(std::move(line100));
    }
  }

  TLegend legend(0.62, 0.80, 0.94, 0.90);
  legend.SetBorderSize(0);
  legend.SetFillStyle(0);
  legend.SetTextSize(0.034);
  const std::string legend_label = "Data [" + year + "]";
  legend.AddEntry(graph_tev.get(), legend_label.c_str(), "pe");
  legend.Draw();

  TPaveText cut_box(0.59, 0.20, 0.94, 0.28, "NDC");
  cut_box.SetBorderSize(0);
  cut_box.SetFillStyle(0);
  cut_box.SetTextAlign(23);
  cut_box.SetTextSize(0.034);
  cut_box.SetTextFont(42);
  cut_box.AddText(observable.jet_label.c_str());
  cut_box.AddText((std::string("|#eta| < ") + format_cut_value(kEtaCut) + ", |#Delta#eta| < " +
                   format_cut_value(kDeltaEtaCut))
                      .c_str());
  cut_box.Draw();

  draw_labels_right(format_lumi_label(lumi_pb));
  gPad->RedrawAxis();

  root_out.cd();
  graph_tev->Write();
  canvas.Write();
  canvas.SaveAs(output_pdf.string().c_str());
}

void draw_count_overlay_plot(const TH1D& denom_hist, const TH1D& numer_hist, const fs::path& output_pdf, TFile& root_out,
                             const std::string& year, const MassObservable& observable) {
  TCanvas canvas(("c_trigger_event_counts_" + observable.tag).c_str(), "", 800, 800);
  canvas.cd();
  gPad->SetTopMargin(0.08);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.16);
  gPad->SetBottomMargin(0.13);
  gPad->SetLogy();

  auto h_denom_plot = hist_to_tev(denom_hist, "h_denom_counts_plot_" + observable.tag);
  auto h_numer_plot = hist_to_tev(numer_hist, "h_numer_counts_plot_" + observable.tag);

  h_denom_plot->SetTitle("");
  h_denom_plot->GetXaxis()->SetTitle(observable.axis_title.c_str());
  h_denom_plot->GetYaxis()->SetTitle("Events / bin");
  h_denom_plot->GetXaxis()->SetRangeUser(kCountsMassDisplayMin / 1000.0, kCountsMassDisplayMax / 1000.0);
  h_denom_plot->GetXaxis()->SetTitleSize(0.05);
  h_denom_plot->GetYaxis()->SetTitleSize(0.05);
  h_denom_plot->GetXaxis()->SetLabelSize(0.045);
  h_denom_plot->GetYaxis()->SetLabelSize(0.045);
  h_denom_plot->GetYaxis()->SetTitleOffset(1.35);

  const double y_max = std::max(h_denom_plot->GetMaximum(), h_numer_plot->GetMaximum());
  h_denom_plot->SetMinimum(0.5);
  h_denom_plot->SetMaximum(std::max(10.0, y_max * 5.0));
  h_denom_plot->SetLineColor(kBlack);
  h_denom_plot->SetLineWidth(2);

  h_numer_plot->SetLineColor(kRed + 1);
  h_numer_plot->SetLineWidth(2);

  h_denom_plot->Draw("HIST");
  h_numer_plot->Draw("HIST SAME");

  TLegend legend(0.58, 0.76, 0.94, 0.90);
  legend.SetBorderSize(0);
  legend.SetFillStyle(0);
  legend.SetTextSize(0.032);
  legend.AddEntry(h_denom_plot.get(), "Denominator", "l");
  legend.AddEntry(h_numer_plot.get(), "Numerator", "l");
  legend.Draw();

  draw_labels_right(year + " (" + kDefaultSqrtS + ")");
  gPad->RedrawAxis();

  root_out.cd();
  canvas.Write();
  canvas.SaveAs(output_pdf.string().c_str());
}

ObservableOutputPaths make_output_paths(const fs::path& base_output_dir, const std::string& stem,
                                        const MassObservable& observable) {
  ObservableOutputPaths paths;
  paths.directory = base_output_dir / observable.tag;
  fs::create_directories(paths.directory);
  paths.stem = stem + "_" + observable.tag;
  paths.root_file = paths.directory / (paths.stem + ".root");
  paths.summary_csv = paths.directory / (paths.stem + "_summary.csv");
  paths.summary_tsv = paths.directory / (paths.stem + "_summary.tsv");
  paths.eff_pdf = paths.directory / (paths.stem + "_efficiency.pdf");
  paths.eff_zoom_pdf = paths.directory / (paths.stem + "_efficiency_zoom.pdf");
  if (observable.tag == "AK4PFJet") {
    paths.eff_even_pdf = paths.directory / (paths.stem + "_efficiency_" + ak4_even_bin_suffix() + ".pdf");
    paths.eff_even_zoom_pdf =
        paths.directory / (paths.stem + "_efficiency_zoom_" + ak4_even_bin_suffix() + ".pdf");
  }
  paths.counts_pdf = paths.directory / (paths.stem + "_counts.pdf");
  return paths;
}

std::vector<ProcessingTask> build_processing_tasks(const std::vector<std::string>& monitoring_files,
                                                   const std::vector<std::string>& scouting_files,
                                                   const std::string& monitoring_jetht,
                                                   const std::string& monitoring_singlemuon,
                                                   const std::string& scouting_jetht,
                                                   const std::string& scouting_singlemuon,
                                                   bool use_scouting_for_efficiency) {
  std::vector<ProcessingTask> tasks;
  const size_t total_files =
      use_scouting_for_efficiency ? scouting_files.size() : (monitoring_files.size() + scouting_files.size());
  tasks.reserve(total_files);
  if (!use_scouting_for_efficiency) {
    for (const auto& file_name : monitoring_files) {
      tasks.push_back(ProcessingTask{file_name, true, false, monitoring_jetht, monitoring_singlemuon});
    }
  }
  for (const auto& file_name : scouting_files) {
    tasks.push_back(
        ProcessingTask{file_name, use_scouting_for_efficiency, true, scouting_jetht,
                       use_scouting_for_efficiency ? scouting_singlemuon : ""});
  }
  return tasks;
}

void print_output_info(const ObservableOutputPaths& paths, const MassObservable& observable) {
  const auto info = info_tag();
  std::cout << info << " Observable      : " << observable.tag << " [" << observable.mass_branch << "]" << std::endl;
  std::cout << info << " Output ROOT file : " << paths.root_file << std::endl;
  std::cout << info << " Summary CSV      : " << paths.summary_csv << std::endl;
  std::cout << info << " Summary TSV      : " << paths.summary_tsv << std::endl;
  std::cout << info << " Efficiency PDF   : " << paths.eff_pdf << std::endl;
  std::cout << info << " Efficiency Zoom  : " << paths.eff_zoom_pdf << std::endl;
  if (!paths.eff_even_pdf.empty()) {
    std::cout << info << " Efficiency " << ak4_even_bin_label() << " : " << paths.eff_even_pdf << std::endl;
    std::cout << info << " Eff. Zoom " << ak4_even_bin_label() << "  : " << paths.eff_even_zoom_pdf << std::endl;
  }
  std::cout << info << " Counts PDF       : " << paths.counts_pdf << std::endl;
}

void print_key_mass_points(const std::vector<SummaryRow>& summary_rows, const MassObservable& observable) {
  const auto info = info_tag();
  std::cout << info << " Key mass points [" << observable.tag << "]:" << std::endl;
  for (const auto& row : summary_rows) {
    if (row.denominator_count <= 0.0) {
      continue;
    }
    std::ostringstream line;
    line << std::setw(11) << row.mass_range << " : "
         << "eff=" << format_double_value(row.efficiency, 4) << ' '
         << '-' << format_double_value(row.eff_err_low, 4) << "/+" << format_double_value(row.eff_err_up, 4) << ' '
         << "1-eff=" << format_double_value(row.inefficiency, 4) << ' '
         << "1/sqrt(N)=" << format_double_value(row.inv_sqrt_n, 4) << ' '
         << "diff=(1/sqrt(N)-ineff)=" << format_double_value(row.diff_stat_minus_ineff, 4);
    const bool warn = row.inv_sqrt_n > row.inefficiency;
    std::cout << (warn ? warn_tag() : info_tag()) << "   " << line.str() << std::endl;
  }
}

}  // namespace

int main(int argc, char** argv) {
  try {
    const auto start_time = std::chrono::steady_clock::now();
    const Options args = parse_args(argc, argv);
    const int total_stages = args.use_scouting_for_efficiency ? 5 : 6;
    int stage = 1;

    std::vector<std::string> monitoring_files;
    if (!args.use_scouting_for_efficiency) {
      print_stage(stage++, total_stages, "Reading Monitoring reduced-nTuple list");
      monitoring_files = read_data_inputs(args.monitoring_list);
    }
    print_stage(stage++, total_stages, "Reading Scouting reduced-nTuple list");
    const auto scouting_files = read_data_inputs(args.scouting_list);

    configure_root_style();
    ROOT::EnableThreadSafety();

    const std::string output_dir_name = args.output_dir.empty() ? default_output_dir_name(args.year) : args.output_dir;
    const fs::path output_dir = fs::absolute(output_dir_name);
    fs::create_directories(output_dir);
    const std::string year_tag = sanitize_year_tag(args.year);
    const std::string stem = args.output_prefix + std::string("run3_pfscouting_") + year_tag;

    print_stage(stage++, total_stages, "Resolving trigger branches from input files");
    std::string monitoring_jetht;
    std::string monitoring_singlemuon;
    if (!args.use_scouting_for_efficiency) {
      monitoring_jetht = find_branch_name_in_file(monitoring_files.front(), kJetHtBranchCandidates, "JetHT");
      monitoring_singlemuon =
          find_branch_name_in_file(monitoring_files.front(), kSingleMuonBranchCandidates, "SingleMuon");
    }
    const std::string scouting_jetht =
        find_branch_name_in_file(scouting_files.front(), kJetHtBranchCandidates, "JetHT");
    const std::string scouting_singlemuon = args.use_scouting_for_efficiency
                                                ? find_branch_name_in_file(
                                                      scouting_files.front(), kSingleMuonBranchCandidates, "SingleMuon")
                                                : std::string{};

    const auto info = info_tag();
    if (!args.use_scouting_for_efficiency) {
      std::cout << info << " Monitoring files  : " << monitoring_files.size() << std::endl;
    }
    std::cout << info << " Scouting files    : " << scouting_files.size() << std::endl;
    std::cout << info << " Tree             : " << kTreePathDefault << std::endl;
    std::cout << info << " Worker threads   : " << (args.threads > 0 ? args.threads : 1) << std::endl;
    if (!args.use_scouting_for_efficiency) {
      std::cout << info << " Monitoring JetHT : " << monitoring_jetht << std::endl;
      std::cout << info << " SingleMuon branch: " << monitoring_singlemuon << std::endl;
      std::cout << info << " Efficiency source: Monitoring data (DST_* / passHLT_* branches)" << std::endl;
      std::cout << info << " 1/sqrt(N) source : Scouting data" << std::endl;
    } else {
      std::cout << info << " Scouting SingleMuon: " << scouting_singlemuon << std::endl;
      std::cout << info << " Efficiency source: Scouting data (--scouting-list)" << std::endl;
      std::cout << info << " 1/sqrt(N) source : Scouting data (--scouting-list, shared loop)" << std::endl;
    }
    std::cout << info << " Scouting JetHT   : " << scouting_jetht << std::endl;
    std::cout << info << " Lumi [fb^-1]     : " << format_lumi_fb(args.lumi_pb) << std::endl;

    const auto tasks = build_processing_tasks(monitoring_files, scouting_files, monitoring_jetht, monitoring_singlemuon,
                                              scouting_jetht, scouting_singlemuon, args.use_scouting_for_efficiency);
    print_stage(stage++, total_stages, "Running one shared event loop for mjj, Dijet_MassAK4PF, and HTAK4PF");
    const AllObservableTaskCounts all_counts = process_all_files(tasks, args.threads, "Trigger-efficiency");

    print_stage(stage++, total_stages, "Writing outputs for all observables");
    for (size_t observable_index = 0; observable_index < kMassObservables.size(); ++observable_index) {
      const auto& observable = kMassObservables[observable_index];
      const TaskCounts& observable_counts = all_counts.observables[observable_index];
      std::cout << info << " Producing outputs for " << observable.tag << " [" << observable.mass_branch << "]"
                << std::endl;
      std::cout << info << " Selection        : " << selection_summary(observable) << std::endl;

      auto h_denom =
          make_hist_from_counts("h_monitoring_singlemuon_" + observable.tag, observable_counts.denominator);
      auto h_numer =
          make_hist_from_counts("h_monitoring_jetht_and_singlemuon_" + observable.tag, observable_counts.numerator);
      auto h_scouting = make_hist_from_counts("h_scouting_jetht_" + observable.tag, observable_counts.scouting);

      if (!TEfficiency::CheckConsistency(*h_numer, *h_denom)) {
        die("Numerator/denominator histograms are not consistent for TEfficiency for observable '" +
            observable.tag + "'.");
      }

      TEfficiency efficiency(*h_numer, *h_denom);
      efficiency.SetName(("teff_pfscouting_jetht_" + observable.tag).c_str());
      efficiency.SetStatisticOption(TEfficiency::kFCP);
      auto graph = std::unique_ptr<TGraphAsymmErrors>(efficiency.CreateGraph());
      graph->SetName(("g_pfscouting_jetht_efficiency_" + observable.tag).c_str());

      const auto summary_rows = build_summary_rows(efficiency, *h_denom, *h_numer, *h_scouting);
      const auto paths = make_output_paths(output_dir, stem, observable);

      write_summary_csv(paths.summary_csv, summary_rows);
      write_summary_tsv(paths.summary_tsv, summary_rows);

      TFile root_out(paths.root_file.string().c_str(), "RECREATE");
      if (root_out.IsZombie()) {
        die("Could not create output ROOT file: " + paths.root_file.string());
      }

      root_out.cd();
      h_denom->Write();
      h_numer->Write();
      h_scouting->Write();
      efficiency.Write();
      graph->Write();

      draw_efficiency_plot(*graph, paths.eff_pdf, root_out, args.lumi_pb, args.year, kEfficiencyMassDisplayMin,
                           kEfficiencyMassDisplayMax, observable, "full");
      draw_efficiency_plot(*graph, paths.eff_zoom_pdf, root_out, args.lumi_pb, args.year,
                           kEfficiencyZoomMassDisplayMin, kEfficiencyZoomMassDisplayMax, observable, "zoom");
      draw_count_overlay_plot(*h_denom, *h_numer, paths.counts_pdf, root_out, args.year, observable);

      if (observable.tag == "AK4PFJet") {
        const std::string even_suffix = ak4_even_bin_suffix();
        auto h_denom_even = make_uniform_hist_from_counts(
            "h_monitoring_singlemuon_" + observable.tag + "_" + even_suffix,
            all_counts.ak4_even.denominator,
            static_cast<int>(kAk4EvenBinCount),
            kAk4EvenBinMinGeV,
            kAk4EvenBinMaxGeV);
        auto h_numer_even = make_uniform_hist_from_counts(
            "h_monitoring_jetht_and_singlemuon_" + observable.tag + "_" + even_suffix,
            all_counts.ak4_even.numerator,
            static_cast<int>(kAk4EvenBinCount),
            kAk4EvenBinMinGeV,
            kAk4EvenBinMaxGeV);
        auto h_scouting_even = make_uniform_hist_from_counts(
            "h_scouting_jetht_" + observable.tag + "_" + even_suffix,
            all_counts.ak4_even.scouting,
            static_cast<int>(kAk4EvenBinCount),
            kAk4EvenBinMinGeV,
            kAk4EvenBinMaxGeV);

        if (!TEfficiency::CheckConsistency(*h_numer_even, *h_denom_even)) {
          die("Numerator/denominator histograms are not consistent for " + ak4_even_bin_label() +
              " AK4PFJet TEfficiency.");
        }

        TEfficiency efficiency_even(*h_numer_even, *h_denom_even);
        efficiency_even.SetName(("teff_pfscouting_jetht_" + observable.tag + "_" + even_suffix).c_str());
        efficiency_even.SetStatisticOption(TEfficiency::kFCP);
        auto graph_even = std::unique_ptr<TGraphAsymmErrors>(efficiency_even.CreateGraph());
        graph_even->SetName(("g_pfscouting_jetht_efficiency_" + observable.tag + "_" + even_suffix).c_str());

        root_out.cd();
        h_denom_even->Write();
        h_numer_even->Write();
        h_scouting_even->Write();
        efficiency_even.Write();
        graph_even->Write();

        draw_efficiency_plot(*graph_even, paths.eff_even_pdf, root_out, args.lumi_pb, args.year,
                             kEfficiencyMassDisplayMin, kEfficiencyMassDisplayMax, observable,
                             even_suffix + "_full");
        draw_efficiency_plot(*graph_even, paths.eff_even_zoom_pdf, root_out, args.lumi_pb, args.year,
                             kEfficiencyZoomMassDisplayMin, kEfficiencyZoomMassDisplayMax, observable,
                             even_suffix + "_zoom", 21, kBlue + 1, true);
      }
      root_out.Close();

      print_output_info(paths, observable);
      print_key_mass_points(summary_rows, observable);
    }

    print_stage(stage++, total_stages, "Finalizing");
    const auto end_time = std::chrono::steady_clock::now();
    const std::chrono::duration<double> elapsed = end_time - start_time;
    std::cout << info << " Total elapsed time: " << format_elapsed_time(elapsed.count()) << std::endl;
    std::cout << info << " Finished." << std::endl;
    return 0;
  } catch (const std::exception& ex) {
    std::cerr << "[ERROR] " << ex.what() << std::endl;
    return 1;
  }
}

