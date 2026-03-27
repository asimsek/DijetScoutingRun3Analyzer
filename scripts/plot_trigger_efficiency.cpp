#include <TCanvas.h>
#include <TEfficiency.h>
#include <TError.h>
#include <TFile.h>
#include <TGraphAsymmErrors.h>
#include <TH1D.h>
#include <TLatex.h>
#include <TLegend.h>
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

constexpr double kMinPtJ1 = 30.0;
constexpr double kMinPtJ2 = 30.0;
constexpr double kMinJetCount = 1.0;

constexpr double kEfficiencyMassDisplayMin = 220.0;
constexpr double kEfficiencyMassDisplayMax = 6000.0;
constexpr double kEfficiencyZoomMassDisplayMin = 220.0;
constexpr double kEfficiencyZoomMassDisplayMax = 2000.0;
constexpr double kCountsMassDisplayMin = 0.0;
constexpr double kCountsMassDisplayMax = 6000.0;
constexpr double kEfficiencyYMin = 0.0;
constexpr double kEfficiencyYMax = 1.30;


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

const std::string kTriggerSelection = []() {
  std::ostringstream os;
  os << "PassJSON == 1 && nJet > 1 && IdTight_j1 > 0.5 && IdTight_j2 > 0.5"
     << " && pTWJ_j1 > " << kMinPtJ1 << " && pTWJ_j2 > " << kMinPtJ2
     << " && TMath::Abs(etaWJ_j1) < " << kEtaCut
     << " && TMath::Abs(etaWJ_j2) < " << kEtaCut
     << " && TMath::Abs(deltaETAjj) < " << kDeltaEtaCut;
  return os.str();
}();

struct Options {
  std::string monitoring_list;
  std::string scouting_list;
  std::string output_dir;
  std::string output_prefix = "pfscouting_trigger_";
  double lumi_pb = 5490.0;
  std::string year = "2024";
  int threads = 8;
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

struct ProcessingTask {
  std::string file_name;
  bool is_monitoring = false;
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

[[noreturn]] void die(const std::string& message) {
  throw std::runtime_error(message);
}

void print_usage(const char* argv0) {
  std::cout
      << "Usage: " << argv0
      << " --monitoring-list <file> --scouting-list <file> [options]\n\n"
      << "Options:\n"
      << "  --monitoring-list <file>   Reduced nTuple list for ScoutingPFMonitoring data.\n"
      << "  --scouting-list <file>     Reduced nTuple list for regular PFScouting data used for 1/sqrt(N).\n"
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

  if (opts.monitoring_list.empty()) {
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

bool passes_common_selection(double pass_json, double n_jet, double id_tight_j1, double id_tight_j2, double pt_j1,
                             double pt_j2, double eta_j1, double eta_j2, double delta_eta_jj) {
  return pass_json > 0.5 && n_jet > kMinJetCount && id_tight_j1 > 0.5 &&
         id_tight_j2 > 0.5 && pt_j1 > kMinPtJ1 && pt_j2 > kMinPtJ2 && std::abs(eta_j1) < kEtaCut &&
         std::abs(eta_j2) < kEtaCut && std::abs(delta_eta_jj) < kDeltaEtaCut;
}

TaskCounts process_file_task(const ProcessingTask& task) {
  TaskCounts counts;
  auto file = open_input_file(task.file_name);
  TTree* tree = get_input_tree(*file, task.file_name);

  std::set<std::string> enabled_branches = {
      "PassJSON", "nJet",       "IdTight_j1", "IdTight_j2", "pTWJ_j1",
      "pTWJ_j2",  "etaWJ_j1",   "etaWJ_j2",   "deltaETAjj", "mjj",
      task.jetht_branch};
  if (task.is_monitoring) {
    enabled_branches.insert(task.singlemuon_branch);
  }
  configure_tree_for_reading(*tree, enabled_branches);

  TTreeReader reader(tree);
  TTreeReaderValue<double> pass_json(reader, "PassJSON");
  TTreeReaderValue<double> n_jet(reader, "nJet");
  TTreeReaderValue<double> id_tight_j1(reader, "IdTight_j1");
  TTreeReaderValue<double> id_tight_j2(reader, "IdTight_j2");
  TTreeReaderValue<double> pt_j1(reader, "pTWJ_j1");
  TTreeReaderValue<double> pt_j2(reader, "pTWJ_j2");
  TTreeReaderValue<double> eta_j1(reader, "etaWJ_j1");
  TTreeReaderValue<double> eta_j2(reader, "etaWJ_j2");
  TTreeReaderValue<double> delta_eta_jj(reader, "deltaETAjj");
  TTreeReaderValue<double> mjj(reader, "mjj");
  TTreeReaderValue<double> jetht(reader, task.jetht_branch.c_str());
  std::unique_ptr<TTreeReaderValue<double>> single_muon;
  if (task.is_monitoring) {
    single_muon = std::make_unique<TTreeReaderValue<double>>(reader, task.singlemuon_branch.c_str());
  }

  while (reader.Next()) {
    if (!passes_common_selection(*pass_json, *n_jet, *id_tight_j1, *id_tight_j2, *pt_j1, *pt_j2, *eta_j1, *eta_j2,
                                 *delta_eta_jj)) {
      continue;
    }

    const int bin_index = find_mass_bin(*mjj);
    if (bin_index < 0) {
      continue;
    }

    if (task.is_monitoring) {
      const bool pass_single_muon = (**single_muon) > 0.5;
      if (pass_single_muon) {
        counts.denominator[bin_index] += 1.0;
        if (*jetht > 0.5) {
          counts.numerator[bin_index] += 1.0;
        }
      }
    } else if (*jetht > 0.5) {
      counts.scouting[bin_index] += 1.0;
    }
  }

  return counts;
}

TaskCounts process_all_files(const std::vector<ProcessingTask>& tasks, int threads, const std::string& label) {
  if (tasks.empty()) {
    die("No files were provided for " + label);
  }

  ROOT::TThreadExecutor executor(threads > 0 ? threads : 1);
  std::atomic<size_t> processed{0};
  std::mutex io_mutex;
  const size_t total = tasks.size();
  const size_t report_every = std::max<size_t>(1, total / 20);

  auto task_runner = [&](const ProcessingTask& task) {
    TaskCounts counts = process_file_task(task);
    const size_t done = processed.fetch_add(1) + 1;
    if (done == total || done % report_every == 0) {
      std::lock_guard<std::mutex> lock(io_mutex);
      std::cout << colored_tag("[PROGRESS]", kAnsiBrightBlue) << ' ' << label << " event loop: [" << done << "/"
                << total << "] "
                << fs::path(task.file_name).filename().string() << std::endl;
    }
    return counts;
  };

  TaskCounts merged;
  const auto partials = executor.Map(task_runner, tasks);
  for (const auto& partial : partials) {
    for (size_t i = 0; i < merged.denominator.size(); ++i) {
      merged.denominator[i] += partial.denominator[i];
      merged.numerator[i] += partial.numerator[i];
      merged.scouting[i] += partial.scouting[i];
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
  tev_bins.reserve(kMassBins.size());
  for (double edge : kMassBins) {
    tev_bins.push_back(edge / 1000.0);
  }
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
                          const std::string& plot_tag) {
  TCanvas canvas(("c_trigger_efficiency_" + plot_tag).c_str(), "", 800, 800);
  canvas.cd();
  gPad->SetTopMargin(0.08);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.16);
  gPad->SetBottomMargin(0.13);

  auto graph_tev = graph_to_tev(graph, "g_pfscouting_jetht_efficiency_tev_" + plot_tag);
  graph_tev->SetTitle("");
  graph_tev->GetXaxis()->SetTitle("Dijet mass [TeV]");
  graph_tev->GetYaxis()->SetTitle("Trigger efficiency");
  graph_tev->GetXaxis()->SetLimits(x_min_gev / 1000.0, x_max_gev / 1000.0);
  graph_tev->GetXaxis()->SetTitleSize(0.05);
  graph_tev->GetYaxis()->SetTitleSize(0.05);
  graph_tev->GetXaxis()->SetLabelSize(0.045);
  graph_tev->GetYaxis()->SetLabelSize(0.045);
  graph_tev->GetYaxis()->SetTitleOffset(1.35);
  graph_tev->SetMarkerStyle(20);
  graph_tev->SetMarkerSize(1.0);
  graph_tev->SetMarkerColor(kBlack);
  graph_tev->SetLineColor(kBlack);
  graph_tev->SetMinimum(kEfficiencyYMin);
  graph_tev->SetMaximum(kEfficiencyYMax);
  graph_tev->Draw("AP");

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
  cut_box.AddText("Wide PF-jets");
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
                             const std::string& year) {
  TCanvas canvas("c_trigger_event_counts", "", 800, 800);
  canvas.cd();
  gPad->SetTopMargin(0.08);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.16);
  gPad->SetBottomMargin(0.13);
  gPad->SetLogy();

  auto h_denom_plot = hist_to_tev(denom_hist, "h_denom_counts_plot");
  auto h_numer_plot = hist_to_tev(numer_hist, "h_numer_counts_plot");

  h_denom_plot->SetTitle("");
  h_denom_plot->GetXaxis()->SetTitle("Dijet mass [TeV]");
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

}  // namespace

int main(int argc, char** argv) {
  try {
    const auto start_time = std::chrono::steady_clock::now();
    const Options args = parse_args(argc, argv);
    constexpr int total_stages = 10;

    print_stage(1, total_stages, "Reading Monitoring reduced-nTuple list");
    const auto monitoring_files = read_data_inputs(args.monitoring_list);
    print_stage(2, total_stages, "Reading Scouting reduced-nTuple list");
    const auto scouting_files = read_data_inputs(args.scouting_list);

    configure_root_style();
    ROOT::EnableThreadSafety();

    const std::string output_dir_name = args.output_dir.empty() ? default_output_dir_name(args.year) : args.output_dir;
    const fs::path output_dir = fs::absolute(output_dir_name);
    fs::create_directories(output_dir);
    const std::string year_tag = sanitize_year_tag(args.year);
    const std::string stem = args.output_prefix + std::string("run3_pfscouting_") + year_tag;
    const fs::path output_root = output_dir / (stem + ".root");
    const fs::path summary_csv = output_dir / (stem + "_summary.csv");
    const fs::path summary_tsv = output_dir / (stem + "_summary.tsv");
    const fs::path eff_pdf = output_dir / (stem + "_efficiency.pdf");
    const fs::path eff_zoom_pdf = output_dir / (stem + "_efficiency_zoom.pdf");
    const fs::path counts_pdf = output_dir / (stem + "_counts.pdf");

    print_stage(3, total_stages, "Resolving trigger branches from input files");
    const std::string monitoring_jetht =
        find_branch_name_in_file(monitoring_files.front(), kJetHtBranchCandidates, "JetHT");
    const std::string monitoring_singlemuon =
        find_branch_name_in_file(monitoring_files.front(), kSingleMuonBranchCandidates, "SingleMuon");
    const std::string scouting_jetht =
        find_branch_name_in_file(scouting_files.front(), kJetHtBranchCandidates, "JetHT");

    std::vector<ProcessingTask> tasks;
    tasks.reserve(monitoring_files.size() + scouting_files.size());
    for (const auto& file_name : monitoring_files) {
      tasks.push_back(ProcessingTask{file_name, true, monitoring_jetht, monitoring_singlemuon});
    }
    for (const auto& file_name : scouting_files) {
      tasks.push_back(ProcessingTask{file_name, false, scouting_jetht, ""});
    }

    print_stage(4, total_stages, "Running file-parallel event loops for monitoring and scouting");
    const TaskCounts all_counts = process_all_files(tasks, args.threads, "Trigger-efficiency");

    print_stage(5, total_stages, "Materializing denominator, numerator and scouting histograms");
    auto h_denom = make_hist_from_counts("h_monitoring_singlemuon", all_counts.denominator);
    auto h_numer = make_hist_from_counts("h_monitoring_jetht_and_singlemuon", all_counts.numerator);
    auto h_scouting = make_hist_from_counts("h_scouting_jetht", all_counts.scouting);

    if (!TEfficiency::CheckConsistency(*h_numer, *h_denom)) {
      die("Numerator/denominator histograms are not consistent for TEfficiency.");
    }

    print_stage(6, total_stages, "Building trigger efficiency, inefficiency, and statistical uncertainty objects");
    TEfficiency efficiency(*h_numer, *h_denom);
    efficiency.SetName("teff_pfscouting_jetht");
    efficiency.SetStatisticOption(TEfficiency::kFCP);
    auto graph = std::unique_ptr<TGraphAsymmErrors>(efficiency.CreateGraph());
    graph->SetName("g_pfscouting_jetht_efficiency");

    const auto summary_rows = build_summary_rows(efficiency, *h_denom, *h_numer, *h_scouting);

    print_stage(7, total_stages, "Writing CSV/TSV summaries");
    write_summary_csv(summary_csv, summary_rows);
    write_summary_tsv(summary_tsv, summary_rows);

    const auto info = info_tag();
    std::cout << info << " Monitoring files  : " << monitoring_files.size() << std::endl;
    std::cout << info << " Scouting files    : " << scouting_files.size() << std::endl;
    std::cout << info << " Tree             : " << kTreePathDefault << std::endl;
    std::cout << info << " Selection        : " << kTriggerSelection << std::endl;
    std::cout << info << " Worker threads   : " << (args.threads > 0 ? args.threads : 1) << std::endl;
    std::cout << info << " Monitoring JetHT : " << monitoring_jetht << std::endl;
    std::cout << info << " SingleMuon branch: " << monitoring_singlemuon << std::endl;
    std::cout << info << " Scouting JetHT   : " << scouting_jetht << std::endl;
    std::cout << info << " Efficiency source: Monitoring data (DST_* / passHLT_* branches)" << std::endl;
    std::cout << info << " 1/sqrt(N) source : Scouting data" << std::endl;
    std::cout << info << " Lumi [fb^-1]     : " << format_lumi_fb(args.lumi_pb) << std::endl;
    std::cout << info << " Output ROOT file : " << output_root << std::endl;
    std::cout << info << " Summary CSV      : " << summary_csv << std::endl;
    std::cout << info << " Summary TSV      : " << summary_tsv << std::endl;
    std::cout << info << " Efficiency PDF   : " << eff_pdf << std::endl;
    std::cout << info << " Efficiency Zoom  : " << eff_zoom_pdf << std::endl;
    std::cout << info << " Counts PDF       : " << counts_pdf << std::endl;

    print_stage(8, total_stages, "Writing ROOT outputs");
    TFile root_out(output_root.string().c_str(), "RECREATE");
    if (root_out.IsZombie()) {
      die("Could not create output ROOT file: " + output_root.string());
    }

    root_out.cd();
    h_denom->Write();
    h_numer->Write();
    h_scouting->Write();
    efficiency.Write();
    graph->Write();

    draw_efficiency_plot(*graph, eff_pdf, root_out, args.lumi_pb, args.year, kEfficiencyMassDisplayMin,
                         kEfficiencyMassDisplayMax, "full");
    draw_efficiency_plot(*graph, eff_zoom_pdf, root_out, args.lumi_pb, args.year, kEfficiencyZoomMassDisplayMin,
                         kEfficiencyZoomMassDisplayMax, "zoom");
    draw_count_overlay_plot(*h_denom, *h_numer, counts_pdf, root_out, args.year);
    root_out.Close();

    print_stage(9, total_stages, "Printing key mass points");
    std::cout << info << " Key mass points:" << std::endl;
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

    print_stage(10, total_stages, "Finalizing");
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

