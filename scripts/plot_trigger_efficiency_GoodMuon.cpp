#include <ROOT/TThreadExecutor.hxx>

#include <TBranch.h>
#include <TCanvas.h>
#include <TError.h>
#include <TEfficiency.h>
#include <TFile.h>
#include <TGraphAsymmErrors.h>
#include <TH1.h>
#include <TH1D.h>
#include <TH1F.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>
#include <TObjArray.h>
#include <TPaveText.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include <algorithm>
#include <array>
#include <atomic>
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
#include <unordered_set>
#include <utility>
#include <vector>
#include <unistd.h>
#include <wordexp.h>

namespace fs = std::filesystem;

namespace {

constexpr const char* kTreePathDefault = "rootTupleTree/tree";
constexpr const char* kTreePathFallback = "tree";
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
constexpr double kMinNVtx = 0.0;
constexpr double kMaxNVtx = std::numeric_limits<double>::infinity();
constexpr bool kUseGoodMuonBaseSelectionFlagIfAvailable = true;
constexpr const char* kGoodMuonBaseSelectionFlagBranch = "passGoodMuonBaseSelection";
constexpr double kEfficiencyYMin = 0.0;
constexpr double kEfficiencyYMax = 1.30;
constexpr double kHtHistogramMaxGeV = 1500.0;
constexpr double kEvenHistogramMaxGeV = 14000.0;
constexpr double kCountsYMin = 0.5;
constexpr int kTurnOnConsecutivePoints = 3;

const std::array<std::string, 2> kJetHtBranchCandidates = {
    "passHLT_PFScoutingHT",
    "DST_PFScouting_JetHT",
};

const std::array<std::string, 2> kSingleMuonBranchCandidates = {
    "passHLT_PFScouting_SingleMuon",
    "DST_PFScouting_SingleMuon",
};

const std::array<std::string, 1> kL1BranchCandidates = {
    "passL1ForPFScoutingHT",
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

enum class SelectionKind {
  kWideJet,
  kAk4,
  kHtAk4,
};

enum class BinningKind {
  kVariable,
  kUniform,
};

struct Options {
  std::string input_list;
  std::string output_dir;
  std::string output_prefix = "goodmuon_trigger";
  std::string tree_path;
  std::string dataset_label = "2024";
  double lumi_pb = 0.0;
  int threads = 8;
  int cache_mb = 128;
  int even_bin_width_gev = 10;
};

struct PlotSpec {
  std::string observable_folder;
  std::string binning_folder;
  std::string source_tag;
  std::string branch_name;
  std::string jet_label;
  std::string axis_title;
  std::string selection_label;
  SelectionKind selection_kind = SelectionKind::kWideJet;
  BinningKind binning_kind = BinningKind::kVariable;
  double x_scale = 1000.0;
  double full_min_gev = 0.0;
  double full_max_gev = 0.0;
  double zoom_min_gev = 0.0;
  double zoom_max_gev = 0.0;
  double counts_min_gev = 0.0;
  double counts_max_gev = 0.0;
  bool draw_99_efficiency_line = true;
  bool draw_full_efficiency_line = true;
  bool write_zoom_plot = true;
  bool write_counts_plot = true;
  std::vector<double> variable_edges;
  double uniform_min = 0.0;
  double uniform_max = 0.0;
  int uniform_bin_width = 1;
};

struct CaseSpec {
  std::string folder_tag;
  std::string source_tag;
};

struct PlotCounts {
  std::vector<double> all;
  std::vector<double> denominator;
  std::vector<double> numerator_goodmuon;
  std::vector<double> numerator_goodmuon_l1;
};

struct TaskCounts {
  std::vector<PlotCounts> plots;
};

struct SummaryRow {
  double bin_low = std::numeric_limits<double>::quiet_NaN();
  double bin_high = std::numeric_limits<double>::quiet_NaN();
  std::string bin_range;
  double efficiency = std::numeric_limits<double>::quiet_NaN();
  double inefficiency = std::numeric_limits<double>::quiet_NaN();
  double eff_err_low = std::numeric_limits<double>::quiet_NaN();
  double eff_err_up = std::numeric_limits<double>::quiet_NaN();
  double all_count = 0.0;
  double denominator_count = 0.0;
  double numerator_count = 0.0;
  double inv_sqrt_n = std::numeric_limits<double>::quiet_NaN();
  double diff_inv_sqrt_n_minus_inefficiency = std::numeric_limits<double>::quiet_NaN();
  double legacy_mjj_all_count = std::numeric_limits<double>::quiet_NaN();
  double legacy_mjj_inv_sqrt_n = std::numeric_limits<double>::quiet_NaN();
  double legacy_mjj_diff_inv_sqrt_n_minus_inefficiency = std::numeric_limits<double>::quiet_NaN();
};

struct OutputPaths {
  fs::path directory;
  fs::path root_file;
  fs::path summary_csv;
  fs::path eff_pdf;
  fs::path eff_zoom_pdf;
  fs::path counts_pdf;
  std::string stem;
};

const std::array<CaseSpec, 2> kCases = {{
    {"GoodMuon", "goodMuon"},
    {"MuonAndL1", "goodMuonL1"},
}};

[[noreturn]] void die(const std::string& message) {
  throw std::runtime_error(message);
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

std::string progress_tag() {
  return colored_tag("[PROGRESS]", kAnsiBrightBlue);
}

void print_usage(const char* argv0) {
  std::cout
      << "Usage: " << argv0 << " --input-list <file> [options]\n\n"
      << "Offline GoodMuon/GoodMuonL1 trigger-efficiency plotter for reduced ntuples\n"
      << "produced with applyGoodMuonSelection_analysis=1.\n\n"
      << "Options:\n"
      << "  --input-list <file>        Text file listing reduced ntuple ROOT files.\n"
      << "  --output-dir <dir>         Output directory (default: goodmuon_trigger_efficiency_<dataset-label>).\n"
      << "  --output-prefix <prefix>   Prefix for output files (default: goodmuon_trigger).\n"
      << "  --dataset-label <label>    Label used in legend, e.g. 2024I (default: 2024).\n"
      << "  --lumi-pb <value>          Integrated luminosity in pb^-1 (default: 0, omit lumi label).\n"
      << "  --tree-path <path>         Tree path override (default: rootTupleTree/tree, fallback: tree).\n"
      << "  --threads <n>              Files processed in parallel (default: 8).\n"
      << "  --cache-mb <n>             TTree cache per file in MB (default: 128).\n"
      << "  --even-bin-width-gev <n>   Bin width for even-binned wide/AK4/HT plots (default: 10).\n"
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

    if (arg == "--input-list") {
      opts.input_list = require_value(arg);
    } else if (arg == "--output-dir") {
      opts.output_dir = require_value(arg);
    } else if (arg == "--output-prefix") {
      opts.output_prefix = require_value(arg);
    } else if (arg == "--dataset-label") {
      opts.dataset_label = require_value(arg);
    } else if (arg == "--lumi-pb") {
      opts.lumi_pb = std::stod(require_value(arg));
    } else if (arg == "--tree-path") {
      opts.tree_path = require_value(arg);
    } else if (arg == "--threads") {
      opts.threads = std::stoi(require_value(arg));
    } else if (arg == "--cache-mb") {
      opts.cache_mb = std::stoi(require_value(arg));
    } else if (arg == "--even-bin-width-gev") {
      opts.even_bin_width_gev = std::stoi(require_value(arg));
    } else if (arg == "--help" || arg == "-h") {
      print_usage(argv[0]);
      std::exit(0);
    } else {
      die("Unknown argument: " + arg);
    }
  }

  if (opts.input_list.empty()) {
    die("--input-list is required");
  }
  if (opts.even_bin_width_gev <= 0) {
    die("--even-bin-width-gev must be a positive integer");
  }
  if (opts.output_dir.empty()) {
    opts.output_dir = "goodmuon_trigger_efficiency_" +
                      std::regex_replace(opts.dataset_label, std::regex(R"([^A-Za-z0-9_.-]+)"), "_");
  }
  return opts;
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
      if (entry.path().extension() == ".root") {
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
    die("No ROOT files found from list: " + path.string());
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

std::unique_ptr<TFile> open_input_file(const std::string& file_name) {
  std::unique_ptr<TFile> file(TFile::Open(file_name.c_str(), "READ"));
  if (!file || file->IsZombie()) {
    die("Could not open ROOT file: " + file_name);
  }
  return file;
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

TTree* get_input_tree(TFile& file, const Options& opts, const std::string& file_name) {
  if (!opts.tree_path.empty()) {
    auto* tree = dynamic_cast<TTree*>(file.Get(opts.tree_path.c_str()));
    if (!tree) {
      die("Could not find tree '" + opts.tree_path + "' in file: " + file_name);
    }
    return tree;
  }

  if (auto* tree = dynamic_cast<TTree*>(file.Get(kTreePathDefault))) {
    return tree;
  }
  if (auto* tree = dynamic_cast<TTree*>(file.Get(kTreePathFallback))) {
    return tree;
  }
  die("Could not find tree '" + std::string(kTreePathDefault) + "' or '" + kTreePathFallback +
      "' in file: " + file_name);
}

std::unique_ptr<TH1D> clone_hist_as_double(const TH1& source, const std::string& name) {
  const TAxis* xaxis = source.GetXaxis();
  std::vector<double> edges;
  edges.reserve(source.GetNbinsX() + 1);
  for (int bin = 1; bin <= source.GetNbinsX(); ++bin) {
    edges.push_back(xaxis->GetBinLowEdge(bin));
  }
  edges.push_back(xaxis->GetBinUpEdge(source.GetNbinsX()));

  auto hist = std::make_unique<TH1D>(name.c_str(), "", static_cast<int>(edges.size()) - 1, edges.data());
  hist->SetDirectory(nullptr);
  hist->Sumw2();
  for (int bin = 0; bin <= source.GetNbinsX() + 1; ++bin) {
    hist->SetBinContent(bin, source.GetBinContent(bin));
    hist->SetBinError(bin, source.GetBinError(bin));
  }
  hist->SetEntries(source.GetEntries());
  return hist;
}

std::unique_ptr<TH1D> rebin_histogram_even_gev(const TH1D& source,
                                               int target_bin_width_gev,
                                               const std::string& name) {
  const TAxis* xaxis = source.GetXaxis();
  if (source.GetNbinsX() <= 0) {
    return clone_hist_as_double(source, name);
  }

  const double source_bin_width = xaxis->GetBinWidth(1);
  if (source_bin_width <= 0.0) {
    die("Encountered a histogram with non-positive bin width while rebinnning: " + std::string(source.GetName()));
  }

  const double raw_factor = static_cast<double>(target_bin_width_gev) / source_bin_width;
  const int rebin_factor = static_cast<int>(std::llround(raw_factor));
  if (std::fabs(raw_factor - static_cast<double>(rebin_factor)) > 1e-9 || rebin_factor <= 0) {
    die("Requested even bin width is incompatible with histogram binning for " + std::string(source.GetName()));
  }
  if (rebin_factor == 1) {
    return clone_hist_as_double(source, name);
  }
  if (source.GetNbinsX() % rebin_factor != 0) {
    die("Requested even bin width does not divide the histogram binning for " + std::string(source.GetName()));
  }

  auto clone = clone_hist_as_double(source, name + "_tmp");
  auto* rebinned_raw = clone->Rebin(rebin_factor, name.c_str());
  TH1D* rebinned = dynamic_cast<TH1D*>(rebinned_raw);
  if (!rebinned) {
    die("Failed to rebin histogram " + std::string(source.GetName()));
  }
  rebinned->SetDirectory(nullptr);
  return std::unique_ptr<TH1D>(rebinned);
}

bool merge_histogram(TFile& file,
                     const std::string& primary_name,
                     const std::string& merged_name,
                     std::unique_ptr<TH1D>& merged_hist) {
  TH1* input_hist = dynamic_cast<TH1*>(file.Get(primary_name.c_str()));
  if (!input_hist) {
    return false;
  }
  if (!merged_hist) {
    merged_hist = clone_hist_as_double(*input_hist, merged_name);
  } else {
    merged_hist->Add(input_hist);
  }
  return true;
}

std::string trigger_hist_name(const PlotSpec& plot, const CaseSpec& study_case, const char* trig_name) {
  return "h_" + plot.source_tag + "_HLTpass_" + study_case.source_tag + "_" + trig_name;
}

std::string find_branch_name_in_file(const std::string& file_name,
                                     const Options& opts,
                                     const std::array<std::string, 2>& candidates,
                                     const std::string& label) {
  auto file = open_input_file(file_name);
  TTree* tree = get_input_tree(*file, opts, file_name);
  const auto branch_names = get_branch_names(*tree);
  for (const auto& candidate : candidates) {
    if (branch_names.count(candidate)) {
      return candidate;
    }
  }
  std::ostringstream os;
  os << "Could not find " << label << " branch in reduced ntuple. Tried: ";
  for (size_t i = 0; i < candidates.size(); ++i) {
    if (i != 0) {
      os << ", ";
    }
    os << candidates[i];
  }
  die(os.str());
}

std::string find_branch_name_in_file(const std::string& file_name,
                                     const Options& opts,
                                     const std::array<std::string, 1>& candidates,
                                     const std::string& label) {
  auto file = open_input_file(file_name);
  TTree* tree = get_input_tree(*file, opts, file_name);
  const auto branch_names = get_branch_names(*tree);
  for (const auto& candidate : candidates) {
    if (branch_names.count(candidate)) {
      return candidate;
    }
  }
  std::ostringstream os;
  os << "Could not find " << label << " branch in reduced ntuple. Tried: ";
  for (size_t i = 0; i < candidates.size(); ++i) {
    if (i != 0) {
      os << ", ";
    }
    os << candidates[i];
  }
  die(os.str());
}

void require_branch_name(const std::set<std::string>& branch_names,
                         const std::string& branch_name,
                         const std::string& file_name) {
  if (!branch_names.count(branch_name)) {
    die("Required branch '" + branch_name + "' was not found in file: " + file_name);
  }
}

void configure_tree_for_reading(TTree& tree, const std::set<std::string>& enabled_branches, int cache_mb) {
  const auto branch_names = get_branch_names(tree);
  tree.SetBranchStatus("*", 0);
  for (const auto& branch_name : enabled_branches) {
    if (branch_names.count(branch_name)) {
      tree.SetBranchStatus(branch_name.c_str(), 1);
    }
  }
  const auto cache_bytes = static_cast<Long64_t>(std::max(1, cache_mb)) * 1024LL * 1024LL;
  tree.SetCacheSize(cache_bytes);
  tree.SetCacheLearnEntries(10);
  for (const auto& branch_name : enabled_branches) {
    if (branch_names.count(branch_name)) {
      tree.AddBranchToCache(branch_name.c_str(), kTRUE);
    }
  }
}

std::vector<PlotSpec> make_plot_specs(const Options& opts) {
  std::vector<PlotSpec> plots;
  plots.reserve(5);

  PlotSpec wide_binned;
  wide_binned.observable_folder = "wideJet";
  wide_binned.binning_folder = "dijetBinned";
  wide_binned.source_tag = "wideDijet";
  wide_binned.branch_name = "mjj";
  wide_binned.jet_label = "Wide PF-jets";
  wide_binned.axis_title = "Wide-jet dijet mass [TeV]";
  wide_binned.selection_label = "|#eta| < 2.5, |#Delta#eta| < 1.3";
  wide_binned.selection_kind = SelectionKind::kWideJet;
  wide_binned.binning_kind = BinningKind::kVariable;
  wide_binned.x_scale = 1000.0;
  wide_binned.full_min_gev = 100.0;
  wide_binned.full_max_gev = 2000.0;
  wide_binned.zoom_min_gev = 100.0;
  wide_binned.zoom_max_gev = 900.0;
  wide_binned.counts_min_gev = 0.0;
  wide_binned.counts_max_gev = 6000.0;
  wide_binned.variable_edges = kMassBins;
  plots.push_back(wide_binned);

  PlotSpec wide_even = wide_binned;
  wide_even.binning_folder = "even" + std::to_string(opts.even_bin_width_gev) + "GeV";
  wide_even.source_tag = "wideDijetEven1GeV";
  wide_even.binning_kind = BinningKind::kUniform;
  wide_even.uniform_min = 0.0;
  wide_even.uniform_max = kEvenHistogramMaxGeV;
  wide_even.uniform_bin_width = opts.even_bin_width_gev;
  wide_even.variable_edges.clear();
  plots.push_back(wide_even);

  PlotSpec ak4_binned;
  ak4_binned.observable_folder = "AK4PFJet";
  ak4_binned.binning_folder = "dijetBinned";
  ak4_binned.source_tag = "AK4Jets";
  ak4_binned.branch_name = "Dijet_MassAK4PF";
  ak4_binned.jet_label = "AK4 PF-jets";
  ak4_binned.axis_title = "AK4 PF dijet mass [TeV]";
  ak4_binned.selection_label = "|#eta| < 2.5, |#Delta#eta| < 1.3";
  ak4_binned.selection_kind = SelectionKind::kAk4;
  ak4_binned.binning_kind = BinningKind::kVariable;
  ak4_binned.x_scale = 1000.0;
  ak4_binned.full_min_gev = 100.0;
  ak4_binned.full_max_gev = 2000.0;
  ak4_binned.zoom_min_gev = 100.0;
  ak4_binned.zoom_max_gev = 900.0;
  ak4_binned.counts_min_gev = 0.0;
  ak4_binned.counts_max_gev = 6000.0;
  ak4_binned.variable_edges = kMassBins;
  plots.push_back(ak4_binned);

  PlotSpec ak4_even = ak4_binned;
  ak4_even.binning_folder = "even" + std::to_string(opts.even_bin_width_gev) + "GeV";
  ak4_even.source_tag = "AK4JetsEven1GeV";
  ak4_even.binning_kind = BinningKind::kUniform;
  ak4_even.uniform_min = 0.0;
  ak4_even.uniform_max = kEvenHistogramMaxGeV;
  ak4_even.uniform_bin_width = opts.even_bin_width_gev;
  ak4_even.variable_edges.clear();
  plots.push_back(ak4_even);

  PlotSpec ht_even;
  ht_even.observable_folder = "HTAK4PF";
  ht_even.binning_folder = "even" + std::to_string(opts.even_bin_width_gev) + "GeV";
  ht_even.source_tag = "HTAK4PF";
  ht_even.branch_name = "HTAK4PF";
  ht_even.jet_label = "AK4 PF H_{T}";
  ht_even.axis_title = "AK4 PF H_{T} [TeV]";
  ht_even.selection_label = "p_{T} > 30 GeV, |#eta| < 2.5";
  ht_even.selection_kind = SelectionKind::kHtAk4;
  ht_even.binning_kind = BinningKind::kUniform;
  ht_even.x_scale = 1000.0;
  ht_even.full_min_gev = 100.0;
  ht_even.full_max_gev = 1600.0;
  ht_even.zoom_min_gev = 100.0;
  ht_even.zoom_max_gev = 900.0;
  ht_even.counts_min_gev = 0.0;
  ht_even.counts_max_gev = 6000.0;
  ht_even.uniform_min = 0.0;
  ht_even.uniform_max = kHtHistogramMaxGeV;
  ht_even.uniform_bin_width = opts.even_bin_width_gev;
  plots.push_back(ht_even);

  return plots;
}

int plot_nbins(const PlotSpec& plot) {
  if (plot.binning_kind == BinningKind::kVariable) {
    return static_cast<int>(plot.variable_edges.size()) - 1;
  }
  return static_cast<int>((plot.uniform_max - plot.uniform_min) / static_cast<double>(plot.uniform_bin_width));
}

PlotCounts make_empty_plot_counts(const PlotSpec& plot) {
  const int nbins = plot_nbins(plot);
  PlotCounts counts;
  counts.all.assign(nbins, 0.0);
  counts.denominator.assign(nbins, 0.0);
  counts.numerator_goodmuon.assign(nbins, 0.0);
  counts.numerator_goodmuon_l1.assign(nbins, 0.0);
  return counts;
}

TaskCounts make_empty_task_counts(const std::vector<PlotSpec>& plots) {
  TaskCounts counts;
  counts.plots.reserve(plots.size());
  for (const auto& plot : plots) {
    counts.plots.push_back(make_empty_plot_counts(plot));
  }
  return counts;
}

std::string plot_tag(const PlotSpec& plot) {
  return plot.observable_folder + "_" + plot.binning_folder;
}

int find_bin(const PlotSpec& plot, double value) {
  if (plot.binning_kind == BinningKind::kVariable) {
    if (value >= plot.variable_edges.back()) {
      return static_cast<int>(plot.variable_edges.size()) - 2;
    }
    auto upper = std::upper_bound(plot.variable_edges.begin(), plot.variable_edges.end(), value);
    if (upper == plot.variable_edges.begin() || upper == plot.variable_edges.end()) {
      return -1;
    }
    return static_cast<int>(std::distance(plot.variable_edges.begin(), upper) - 1);
  }

  if (value < plot.uniform_min || value >= plot.uniform_max) {
    return -1;
  }
  return static_cast<int>((value - plot.uniform_min) / static_cast<double>(plot.uniform_bin_width));
}

bool passes_widejet_selection(double pass_json,
                              double n_vtx,
                              double n_jet,
                              double id_tight_j1,
                              double id_tight_j2,
                              double pt_j1,
                              double pt_j2,
                              double eta_j1,
                              double eta_j2,
                              double delta_eta_jj) {
  return pass_json > 0.5 && n_vtx >= kMinNVtx && n_vtx <= kMaxNVtx &&
         n_jet > kMinJetCount && id_tight_j1 > 0.5 &&
         id_tight_j2 > 0.5 && pt_j1 > kMinPtJ1 && pt_j2 > kMinPtJ2 && std::abs(eta_j1) < kEtaCut &&
         std::abs(eta_j2) < kEtaCut && std::abs(delta_eta_jj) < kDeltaEtaCut;
}

bool passes_ak4_selection(double pass_json,
                          double n_vtx,
                          double n_ak4,
                          double id_tight_j1,
                          double id_tight_j2,
                          double pt_j1,
                          double pt_j2,
                          double eta_j1,
                          double eta_j2,
                          double delta_eta_jj) {
  return pass_json > 0.5 && n_vtx >= kMinNVtx && n_vtx <= kMaxNVtx &&
         n_ak4 > kMinJetCount && id_tight_j1 > 0.5 &&
         id_tight_j2 > 0.5 && pt_j1 > kMinPtJ1 && pt_j2 > kMinPtJ2 && std::abs(eta_j1) < kEtaCut &&
         std::abs(eta_j2) < kEtaCut && std::abs(delta_eta_jj) < kDeltaEtaCut;
}

bool passes_ht_selection(double pass_json, double n_vtx) {
  return pass_json > 0.5 && n_vtx >= kMinNVtx && n_vtx <= kMaxNVtx;
}

TaskCounts process_file(const std::string& file_name,
                        const Options& opts,
                        const std::vector<PlotSpec>& plots,
                        const std::string& jetht_branch,
                        const std::string& singlemuon_branch,
                        const std::string& l1_branch) {
  TaskCounts counts = make_empty_task_counts(plots);

  auto file = open_input_file(file_name);
  TTree* tree = get_input_tree(*file, opts, file_name);
  const auto branch_names = get_branch_names(*tree);
  require_branch_name(branch_names, jetht_branch, file_name);
  require_branch_name(branch_names, singlemuon_branch, file_name);
  require_branch_name(branch_names, l1_branch, file_name);
  const bool has_goodmuon_base_selection_flag =
      kUseGoodMuonBaseSelectionFlagIfAvailable &&
      std::strlen(kGoodMuonBaseSelectionFlagBranch) > 0 &&
      branch_names.count(kGoodMuonBaseSelectionFlagBranch);

  std::set<std::string> enabled_branches = {
      "PassJSON",
      "nVtx",
      "nJet",
      "NAK4PF",
      "IdTight_j1",
      "IdTight_j2",
      "pTWJ_j1",
      "pTWJ_j2",
      "etaWJ_j1",
      "etaWJ_j2",
      "deltaETAjj",
      "pTAK4PF_j1",
      "pTAK4PF_j2",
      "etaAK4PF_j1",
      "etaAK4PF_j2",
      "deltaETAjjAK4PF",
      "mjj",
      "Dijet_MassAK4PF",
      "HTAK4PF",
      jetht_branch,
      singlemuon_branch,
      l1_branch,
  };
  if (has_goodmuon_base_selection_flag) {
    enabled_branches.insert(kGoodMuonBaseSelectionFlagBranch);
  }
  configure_tree_for_reading(*tree, enabled_branches, opts.cache_mb);

  TTreeReader reader(tree);
  TTreeReaderValue<double> pass_json(reader, "PassJSON");
  TTreeReaderValue<double> n_vtx(reader, "nVtx");
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
  TTreeReaderValue<double> mjj(reader, "mjj");
  TTreeReaderValue<double> dijet_mass_ak4(reader, "Dijet_MassAK4PF");
  TTreeReaderValue<double> ht_ak4(reader, "HTAK4PF");
  TTreeReaderValue<double> pass_jetht(reader, jetht_branch.c_str());
  TTreeReaderValue<double> pass_singlemuon(reader, singlemuon_branch.c_str());
  TTreeReaderValue<double> pass_l1(reader, l1_branch.c_str());
  std::unique_ptr<TTreeReaderValue<double>> pass_goodmuon_base_selection;
  if (has_goodmuon_base_selection_flag) {
    pass_goodmuon_base_selection =
        std::make_unique<TTreeReaderValue<double>>(reader, kGoodMuonBaseSelectionFlagBranch);
  }

  while (reader.Next()) {
    const bool wide_selected = passes_widejet_selection(*pass_json,
                                                        *n_vtx,
                                                        *n_jet,
                                                        *id_tight_j1,
                                                        *id_tight_j2,
                                                        *pt_wj1,
                                                        *pt_wj2,
                                                        *eta_wj1,
                                                        *eta_wj2,
                                                        *delta_eta_wj);
    const bool ak4_selected = passes_ak4_selection(*pass_json,
                                                   *n_vtx,
                                                   *n_ak4,
                                                   *id_tight_j1,
                                                   *id_tight_j2,
                                                   *pt_ak4_j1,
                                                   *pt_ak4_j2,
                                                   *eta_ak4_j1,
                                                   *eta_ak4_j2,
                                                   *delta_eta_ak4);
    const bool ht_selected = passes_ht_selection(*pass_json, *n_vtx);

    const bool trigger_singlemuon = (*pass_singlemuon > 0.5);
    const bool trigger_jetht = (*pass_jetht > 0.5);
    const bool trigger_l1 = (*pass_l1 > 0.5);
    const bool pass_goodmuon_selection =
        !pass_goodmuon_base_selection || (**pass_goodmuon_base_selection > 0.5);

    for (size_t plot_index = 0; plot_index < plots.size(); ++plot_index) {
      const auto& plot = plots[plot_index];
      bool selected = false;
      double value = 0.0;

      switch (plot.selection_kind) {
        case SelectionKind::kWideJet:
          selected = wide_selected;
          value = *mjj;
          break;
        case SelectionKind::kAk4:
          selected = ak4_selected;
          value = *dijet_mass_ak4;
          break;
        case SelectionKind::kHtAk4:
          selected = ht_selected;
          value = *ht_ak4;
          break;
      }

      if (!selected) {
        continue;
      }

      const int bin_index = find_bin(plot, value);
      if (bin_index < 0) {
        continue;
      }

      auto& plot_counts = counts.plots[plot_index];
      plot_counts.all[bin_index] += 1.0;
      if (!pass_goodmuon_selection) {
        continue;
      }
      if (trigger_singlemuon) {
        plot_counts.denominator[bin_index] += 1.0;
        if (trigger_jetht) {
          plot_counts.numerator_goodmuon[bin_index] += 1.0;
          if (trigger_l1) {
            plot_counts.numerator_goodmuon_l1[bin_index] += 1.0;
          }
        }
      }
    }
  }

  return counts;
}

TaskCounts process_all_files(const std::vector<std::string>& files,
                             const Options& opts,
                             const std::vector<PlotSpec>& plots,
                             const std::string& jetht_branch,
                             const std::string& singlemuon_branch,
                             const std::string& l1_branch) {
  if (files.empty()) {
    die("No input ROOT files were provided.");
  }

  ROOT::TThreadExecutor executor(opts.threads > 0 ? opts.threads : 1);
  std::atomic<size_t> processed{0};
  std::mutex io_mutex;
  const size_t total = files.size();
  const size_t report_every = std::max<size_t>(1, total / 20);

  auto task_runner = [&](const std::string& file_name) {
    TaskCounts counts = process_file(file_name, opts, plots, jetht_branch, singlemuon_branch, l1_branch);
    const size_t done = processed.fetch_add(1) + 1;
    if (done == total || done % report_every == 0) {
      std::lock_guard<std::mutex> lock(io_mutex);
      std::cout << progress_tag() << " event loop: [" << done << "/" << total << "] "
                << fs::path(file_name).filename().string() << std::endl;
    }
    return counts;
  };

  TaskCounts merged = make_empty_task_counts(plots);
  const auto partials = executor.Map(task_runner, files);
  for (const auto& partial : partials) {
    for (size_t plot_index = 0; plot_index < plots.size(); ++plot_index) {
      auto& out = merged.plots[plot_index];
      const auto& in = partial.plots[plot_index];
      for (size_t i = 0; i < out.all.size(); ++i) {
        out.all[i] += in.all[i];
        out.denominator[i] += in.denominator[i];
        out.numerator_goodmuon[i] += in.numerator_goodmuon[i];
        out.numerator_goodmuon_l1[i] += in.numerator_goodmuon_l1[i];
      }
    }
  }
  return merged;
}

std::unique_ptr<TH1D> make_hist_from_counts(const PlotSpec& plot,
                                            const std::string& name,
                                            const std::vector<double>& counts) {
  std::unique_ptr<TH1D> hist;
  if (plot.binning_kind == BinningKind::kVariable) {
    hist = std::make_unique<TH1D>(name.c_str(),
                                  "",
                                  static_cast<int>(plot.variable_edges.size()) - 1,
                                  plot.variable_edges.data());
  } else {
    hist = std::make_unique<TH1D>(name.c_str(),
                                  "",
                                  plot_nbins(plot),
                                  plot.uniform_min,
                                  plot.uniform_max);
  }
  hist->SetDirectory(nullptr);
  hist->Sumw2();
  for (size_t i = 0; i < counts.size(); ++i) {
    hist->SetBinContent(static_cast<int>(i) + 1, counts[i]);
    hist->SetBinError(static_cast<int>(i) + 1, std::sqrt(counts[i]));
  }
  return hist;
}

std::string format_fb_value(double lumi_pb, int max_precision = 3) {
  std::ostringstream os;
  os << std::fixed << std::setprecision(max_precision) << (lumi_pb / 1000.0);
  std::string text = os.str();
  while (!text.empty() && text.back() == '0') {
    text.pop_back();
  }
  if (!text.empty() && text.back() == '.') {
    text.pop_back();
  }
  if (text.empty()) {
    text = "0";
  }
  return text;
}

std::string format_lumi_label(double lumi_pb) {
  if (lumi_pb > 0.0) {
    return format_fb_value(lumi_pb) + " fb^{-1} (" + std::string(kDefaultSqrtS) + ")";
  }
  return kDefaultSqrtS;
}

std::string format_table_value(double value) {
  if (std::isnan(value)) {
    return "";
  }
  std::ostringstream os;
  os << std::fixed << std::setprecision(9) << value;
  return os.str();
}

std::string format_bin_edge(double value) {
  if (std::isnan(value)) {
    return "";
  }
  if (std::fabs(value - std::round(value)) < 1e-9) {
    return std::to_string(static_cast<long long>(std::llround(value)));
  }
  std::ostringstream os;
  os << std::fixed << std::setprecision(6) << value;
  std::string text = os.str();
  while (!text.empty() && text.back() == '0') {
    text.pop_back();
  }
  if (!text.empty() && text.back() == '.') {
    text.pop_back();
  }
  return text;
}

std::string format_count_value(double value) {
  return std::to_string(static_cast<long long>(std::llround(value)));
}

std::unique_ptr<TH1D> hist_scale_x(const TH1D& source, const std::string& name, double scale) {
  if (scale == 0.0) {
    die("Encountered zero x-scale while preparing a plot histogram.");
  }

  std::vector<double> scaled_bins;
  const TAxis* xaxis = source.GetXaxis();
  scaled_bins.reserve(source.GetNbinsX() + 1);
  for (int bin = 1; bin <= source.GetNbinsX(); ++bin) {
    scaled_bins.push_back(xaxis->GetBinLowEdge(bin) / scale);
  }
  scaled_bins.push_back(xaxis->GetBinUpEdge(source.GetNbinsX()) / scale);

  auto hist = std::make_unique<TH1D>(name.c_str(), "", static_cast<int>(scaled_bins.size()) - 1, scaled_bins.data());
  hist->SetDirectory(nullptr);
  hist->Sumw2();
  for (int bin = 0; bin <= source.GetNbinsX() + 1; ++bin) {
    hist->SetBinContent(bin, source.GetBinContent(bin));
    hist->SetBinError(bin, source.GetBinError(bin));
  }
  hist->SetEntries(source.GetEntries());
  return hist;
}

std::unique_ptr<TGraphAsymmErrors> graph_scale_x(const TGraphAsymmErrors& source,
                                                 const std::string& name,
                                                 double scale) {
  auto graph = std::make_unique<TGraphAsymmErrors>(source);
  graph->SetName(name.c_str());
  if (scale == 0.0) {
    die("Encountered zero x-scale while preparing a plot graph.");
  }
  for (int i = 0; i < graph->GetN(); ++i) {
    graph->GetX()[i] /= scale;
    graph->GetEXlow()[i] /= scale;
    graph->GetEXhigh()[i] /= scale;
  }
  return graph;
}

std::unique_ptr<TH1D> merge_saved_no_trig_histograms(const std::vector<std::string>& files,
                                                     const PlotSpec& plot,
                                                     const CaseSpec& study_case) {
  std::unique_ptr<TH1D> merged;
  const std::string hist_name = trigger_hist_name(plot, study_case, "noTrig");
  const std::string merged_name = plot.observable_folder + "_" + plot.binning_folder + "_" + study_case.folder_tag + "_all";
  for (const auto& file_name : files) {
    auto file = open_input_file(file_name);
    merge_histogram(*file, hist_name, merged_name, merged);
  }

  if (!merged) {
    die("Could not find noTrig histogram '" + hist_name + "' in the input ROOT files.");
  }
  if (plot.binning_kind == BinningKind::kUniform) {
    return rebin_histogram_even_gev(*merged,
                                    plot.uniform_bin_width,
                                    merged_name + "_rebinned");
  }
  return merged;
}

std::unique_ptr<TH1D> merge_legacy_mjj_histograms(const std::vector<std::string>& files, const PlotSpec& plot) {
  std::unique_ptr<TH1D> merged;
  const std::string hist_name =
      (plot.binning_kind == BinningKind::kUniform) ? "h_mjj_noTrig_1GeVbin" : "h_mjj_HLTpass_noTrig";
  const std::string merged_name = plot.observable_folder + "_" + plot.binning_folder + "_legacy_mjj_all";
  for (const auto& file_name : files) {
    auto file = open_input_file(file_name);
    merge_histogram(*file, hist_name, merged_name, merged);
  }

  if (!merged) {
    die("Could not find legacy noTrig histogram '" + hist_name + "' in the input ROOT files.");
  }
  if (plot.binning_kind == BinningKind::kUniform) {
    return rebin_histogram_even_gev(*merged,
                                    plot.uniform_bin_width,
                                    merged_name + "_rebinned");
  }
  return merged;
}

std::vector<SummaryRow> build_summary_rows(TEfficiency& efficiency,
                                           const TH1D& denominator_hist,
                                           const TH1D& numerator_hist,
                                           const TH1D& all_hist,
                                           const TH1D* legacy_mjj_all_hist) {
  std::vector<SummaryRow> rows;
  rows.reserve(denominator_hist.GetNbinsX());
  const TAxis* xaxis = denominator_hist.GetXaxis();
  for (int bin_idx = 1; bin_idx <= denominator_hist.GetNbinsX(); ++bin_idx) {
    SummaryRow row;
    row.bin_low = xaxis->GetBinLowEdge(bin_idx);
    row.bin_high = xaxis->GetBinUpEdge(bin_idx);
    row.bin_range = format_bin_edge(row.bin_low) + "-" + format_bin_edge(row.bin_high);
    row.all_count = all_hist.GetBinContent(bin_idx);
    if (legacy_mjj_all_hist) {
      row.legacy_mjj_all_count = legacy_mjj_all_hist->GetBinContent(bin_idx);
    }
    row.denominator_count = denominator_hist.GetBinContent(bin_idx);
    row.numerator_count = numerator_hist.GetBinContent(bin_idx);
    if (row.denominator_count > 0.0) {
      row.efficiency = efficiency.GetEfficiency(bin_idx);
      row.inefficiency = 1.0 - row.efficiency;
      row.eff_err_low = efficiency.GetEfficiencyErrorLow(bin_idx);
      row.eff_err_up = efficiency.GetEfficiencyErrorUp(bin_idx);
    }
    if (row.all_count > 0.0) {
      row.inv_sqrt_n = 1.0 / std::sqrt(row.all_count);
    }
    if (!std::isnan(row.inv_sqrt_n) && !std::isnan(row.inefficiency)) {
      row.diff_inv_sqrt_n_minus_inefficiency = row.inv_sqrt_n - row.inefficiency;
    }
    if (row.legacy_mjj_all_count > 0.0) {
      row.legacy_mjj_inv_sqrt_n = 1.0 / std::sqrt(row.legacy_mjj_all_count);
    }
    if (!std::isnan(row.legacy_mjj_inv_sqrt_n) && !std::isnan(row.inefficiency)) {
      row.legacy_mjj_diff_inv_sqrt_n_minus_inefficiency = row.legacy_mjj_inv_sqrt_n - row.inefficiency;
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

  out << "bin_range,bin_low,bin_high,efficiency,inefficiency,eff_err_low,eff_err_up,all_count,denominator_count,numerator_count,inv_sqrt_n,diff_inv_sqrt_n_minus_inefficiency,legacy_mjj_all_count,legacy_mjj_inv_sqrt_n,legacy_mjj_diff_inv_sqrt_n_minus_inefficiency\n";
  for (const auto& row : rows) {
    out << row.bin_range << ','
        << format_bin_edge(row.bin_low) << ','
        << format_bin_edge(row.bin_high) << ','
        << format_table_value(row.efficiency) << ','
        << format_table_value(row.inefficiency) << ','
        << format_table_value(row.eff_err_low) << ','
        << format_table_value(row.eff_err_up) << ','
        << format_count_value(row.all_count) << ','
        << format_count_value(row.denominator_count) << ','
        << format_count_value(row.numerator_count) << ','
        << format_table_value(row.inv_sqrt_n) << ','
        << format_table_value(row.diff_inv_sqrt_n_minus_inefficiency) << ','
        << format_count_value(row.legacy_mjj_all_count) << ','
        << format_table_value(row.legacy_mjj_inv_sqrt_n) << ','
        << format_table_value(row.legacy_mjj_diff_inv_sqrt_n_minus_inefficiency) << '\n';
  }
}

double find_turnon_x_gev(const TGraphAsymmErrors& graph, double threshold) {
  const int n_points = graph.GetN();
  if (n_points <= 0) {
    return std::numeric_limits<double>::quiet_NaN();
  }
  const int required_points = std::min(kTurnOnConsecutivePoints, n_points);
  for (int i = 0; i < n_points; ++i) {
    bool sustained = true;
    for (int j = 0; j < required_points; ++j) {
      const int idx = i + j;
      if (idx >= n_points || graph.GetY()[idx] < threshold) {
        sustained = false;
        break;
      }
    }
    if (sustained) {
      return graph.GetX()[i];
    }
  }
  return std::numeric_limits<double>::quiet_NaN();
}

double find_first_positive_diff_edge_gev_after_turnon(const std::vector<SummaryRow>& rows, double turnon_x_gev) {
  for (const auto& row : rows) {
    if (!std::isnan(turnon_x_gev) &&
        row.bin_low > turnon_x_gev &&
        !std::isnan(row.legacy_mjj_diff_inv_sqrt_n_minus_inefficiency) &&
        row.legacy_mjj_diff_inv_sqrt_n_minus_inefficiency > 0.0) {
      return row.bin_low;
    }
  }
  return std::numeric_limits<double>::quiet_NaN();
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

void draw_efficiency_plot(TGraphAsymmErrors& graph,
                          const fs::path& output_pdf,
                          TFile& root_out,
                          double lumi_pb,
                          const std::string& year,
                          const PlotSpec& plot,
                          const std::string& plot_tag,
                          double x_min_gev,
                          double x_max_gev,
                          double first_positive_diff_gev,
                          int marker_style = 20,
                          int marker_color = kBlue + 1,
                          bool draw_99_efficiency_line = false,
                          bool draw_full_efficiency_line = false) {
  TCanvas canvas(("c_trigger_efficiency_" + plot.observable_folder + "_" + plot.binning_folder + "_" + plot_tag).c_str(),
                 "",
                 800,
                 800);
  canvas.cd();
  gPad->SetTopMargin(0.08);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.16);
  gPad->SetBottomMargin(0.13);

  auto graph_plot =
      graph_scale_x(graph,
                    "g_pfscouting_jetht_efficiency_plot_" + plot.observable_folder + "_" + plot.binning_folder + "_" + plot_tag,
                    plot.x_scale);
  graph_plot->SetTitle("");
  graph_plot->GetXaxis()->SetTitle(plot.axis_title.c_str());
  graph_plot->GetYaxis()->SetTitle("Trigger efficiency");
  graph_plot->GetXaxis()->SetLimits(x_min_gev / plot.x_scale, x_max_gev / plot.x_scale);
  graph_plot->GetXaxis()->SetTitleSize(0.05);
  graph_plot->GetYaxis()->SetTitleSize(0.05);
  graph_plot->GetXaxis()->SetLabelSize(0.045);
  graph_plot->GetYaxis()->SetLabelSize(0.045);
  graph_plot->GetYaxis()->SetTitleOffset(1.35);
  graph_plot->SetMarkerStyle(marker_style);
  graph_plot->SetMarkerSize(1.0);
  graph_plot->SetMarkerColor(marker_color);
  graph_plot->SetLineColor(marker_color);
  graph_plot->SetMinimum(kEfficiencyYMin);
  graph_plot->SetMaximum(kEfficiencyYMax);
  graph_plot->Draw("AP");

  std::vector<std::unique_ptr<TLine>> turnon_lines;
  if (draw_99_efficiency_line || draw_full_efficiency_line) {
    if (draw_99_efficiency_line) {
      const double x99 = find_turnon_x_gev(*graph_plot, 0.99);
      if (!std::isnan(x99)) {
        auto line99 = std::make_unique<TLine>(x99, kEfficiencyYMin, x99, kEfficiencyYMax);
        line99->SetLineColor(kCyan + 1);
        line99->SetLineStyle(2);
        line99->SetLineWidth(2);
        line99->Draw();
        turnon_lines.push_back(std::move(line99));
      }
    }
    if (draw_full_efficiency_line && !std::isnan(first_positive_diff_gev)) {
      const double x_full = first_positive_diff_gev / plot.x_scale;
      auto line_full = std::make_unique<TLine>(x_full, kEfficiencyYMin, x_full, kEfficiencyYMax);
      line_full->SetLineColor(kRed + 1);
      line_full->SetLineStyle(2);
      line_full->SetLineWidth(2);
      line_full->Draw();
      turnon_lines.push_back(std::move(line_full));
    }
  }

  TLegend legend(0.62, 0.80, 0.94, 0.90);
  legend.SetFillStyle(0);
  legend.SetBorderSize(0);
  legend.SetTextFont(42);
  legend.SetTextSize(0.034);
  legend.AddEntry(graph_plot.get(), ("Data [" + year + "]").c_str(), "pe");
  legend.Draw();

  TPaveText cut_box(0.62, 0.18, 0.95, 0.27, "NDC");
  cut_box.SetBorderSize(0);
  cut_box.SetFillStyle(0);
  cut_box.SetTextAlign(23);
  cut_box.SetTextSize(0.032);
  cut_box.SetTextFont(42);
  cut_box.AddText(plot.jet_label.c_str());
  cut_box.AddText(plot.selection_label.c_str());
  cut_box.Draw();

  draw_labels_right(format_lumi_label(lumi_pb));
  gPad->RedrawAxis();

  root_out.cd();
  graph_plot->Write();
  canvas.Write();
  canvas.SaveAs(output_pdf.string().c_str());
}

void draw_count_overlay_plot(const TH1D& denom_hist,
                             const TH1D& numer_hist,
                             const fs::path& output_pdf,
                             TFile& root_out,
                             const std::string& year,
                             const PlotSpec& plot,
                             const std::string& case_label) {
  TCanvas canvas(("c_trigger_event_counts_" + plot.observable_folder + "_" + plot.binning_folder + "_" + case_label).c_str(),
                 "",
                 800,
                 800);
  canvas.cd();
  gPad->SetTopMargin(0.08);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.16);
  gPad->SetBottomMargin(0.13);
  gPad->SetLogy();

  auto denom_plot =
      hist_scale_x(denom_hist, "h_denom_counts_plot_" + plot.observable_folder + "_" + plot.binning_folder + "_" + case_label,
                   plot.x_scale);
  auto numer_plot =
      hist_scale_x(numer_hist, "h_numer_counts_plot_" + plot.observable_folder + "_" + plot.binning_folder + "_" + case_label,
                   plot.x_scale);

  const double y_max = std::max(denom_plot->GetMaximum(), numer_plot->GetMaximum());
  TH1F* frame = gPad->DrawFrame(plot.counts_min_gev / plot.x_scale,
                                kCountsYMin,
                                plot.counts_max_gev / plot.x_scale,
                                std::max(10.0, y_max * 5.0));
  frame->SetTitle("");
  frame->GetXaxis()->SetTitle(plot.axis_title.c_str());
  frame->GetYaxis()->SetTitle("Events / bin");
  frame->GetXaxis()->SetTitleSize(0.05);
  frame->GetYaxis()->SetTitleSize(0.05);
  frame->GetXaxis()->SetLabelSize(0.045);
  frame->GetYaxis()->SetLabelSize(0.045);
  frame->GetYaxis()->SetTitleOffset(1.35);

  denom_plot->SetLineColor(kBlack);
  denom_plot->SetLineWidth(2);
  numer_plot->SetLineColor(kRed + 1);
  numer_plot->SetLineWidth(2);
  denom_plot->Draw("HIST SAME");
  numer_plot->Draw("HIST SAME");

  TLegend legend(0.58, 0.76, 0.94, 0.90);
  legend.SetBorderSize(0);
  legend.SetFillStyle(0);
  legend.SetTextSize(0.032);
  legend.AddEntry(denom_plot.get(), "Denominator", "l");
  legend.AddEntry(numer_plot.get(), "Numerator", "l");
  legend.Draw();

  TPaveText cut_box(0.62, 0.18, 0.95, 0.27, "NDC");
  cut_box.SetBorderSize(0);
  cut_box.SetFillStyle(0);
  cut_box.SetTextAlign(23);
  cut_box.SetTextSize(0.032);
  cut_box.SetTextFont(42);
  cut_box.AddText(plot.jet_label.c_str());
  cut_box.AddText(plot.selection_label.c_str());
  cut_box.Draw();

  draw_labels_right(year + " (" + kDefaultSqrtS + ")");
  gPad->RedrawAxis();

  root_out.cd();
  denom_plot->Write();
  numer_plot->Write();
  canvas.Write();
  canvas.SaveAs(output_pdf.string().c_str());
}

OutputPaths make_output_paths(const fs::path& base_output_dir,
                              const std::string& output_prefix,
                              const PlotSpec& plot,
                              const CaseSpec& study_case) {
  OutputPaths paths;
  paths.directory = base_output_dir / plot.observable_folder / plot.binning_folder / study_case.folder_tag;
  fs::create_directories(paths.directory);
  paths.stem = output_prefix + "_" + plot.observable_folder + "_" + plot.binning_folder + "_" + study_case.folder_tag;
  paths.root_file = paths.directory / (paths.stem + ".root");
  paths.summary_csv = paths.directory / (paths.stem + "_summary.csv");
  paths.eff_pdf = paths.directory / (paths.stem + "_efficiency.pdf");
  paths.eff_zoom_pdf = paths.directory / (paths.stem + "_efficiency_zoom.pdf");
  paths.counts_pdf = paths.directory / (paths.stem + "_counts.pdf");
  return paths;
}

void print_case_output_info(const OutputPaths& paths, const PlotSpec& plot, const CaseSpec& study_case) {
  (void)paths;
  std::cout << info_tag() << ' ' << plot.observable_folder << " | " << plot.binning_folder << " | "
            << study_case.folder_tag << std::endl;
}

void ensure_directory(const fs::path& dir) {
  std::error_code ec;
  fs::create_directories(dir, ec);
  if (ec) {
    die("Could not create output directory '" + dir.string() + "': " + ec.message());
  }
}

}  // namespace

int main(int argc, char** argv) {
  try {
    TH1::AddDirectory(kFALSE);
    gErrorIgnoreLevel = kWarning;
    const Options opts = parse_args(argc, argv);
    configure_root_style();

    const auto files = read_data_inputs(opts.input_list);
    const auto plots = make_plot_specs(opts);
    ensure_directory(opts.output_dir);

    const std::string jetht_branch = find_branch_name_in_file(files.front(), opts, kJetHtBranchCandidates, "JetHT");
    const std::string singlemuon_branch =
        find_branch_name_in_file(files.front(), opts, kSingleMuonBranchCandidates, "SingleMuon");
    const std::string l1_branch = find_branch_name_in_file(files.front(), opts, kL1BranchCandidates, "L1");

    std::cout << info_tag() << " Reading " << files.size() << " ROOT file(s)" << std::endl;
    std::cout << info_tag() << " JetHT branch: " << jetht_branch << std::endl;
    std::cout << info_tag() << " SingleMuon branch: " << singlemuon_branch << std::endl;
    std::cout << info_tag() << " L1 branch: " << l1_branch << std::endl;

    const TaskCounts all_counts =
        process_all_files(files, opts, plots, jetht_branch, singlemuon_branch, l1_branch);

    for (size_t plot_index = 0; plot_index < plots.size(); ++plot_index) {
      const auto& plot = plots[plot_index];
      const auto& counts = all_counts.plots[plot_index];
      const std::string base_tag = plot_tag(plot);

      auto h_den = make_hist_from_counts(plot, "h_denominator_" + base_tag, counts.denominator);
      auto h_num_goodmuon = make_hist_from_counts(plot, "h_numerator_" + base_tag + "_goodMuon",
                                                  counts.numerator_goodmuon);
      auto h_num_goodmuon_l1 = make_hist_from_counts(plot, "h_numerator_" + base_tag + "_goodMuonL1",
                                                     counts.numerator_goodmuon_l1);
      auto h_legacy_mjj_all = merge_legacy_mjj_histograms(files, plot);

      if (!TEfficiency::CheckConsistency(*h_num_goodmuon, *h_den)) {
        die("GoodMuon numerator/denominator are not TEfficiency-consistent for plot '" + base_tag + "'.");
      }
      if (!TEfficiency::CheckConsistency(*h_num_goodmuon_l1, *h_den)) {
        die("GoodMuonL1 numerator/denominator are not TEfficiency-consistent for plot '" + base_tag + "'.");
      }

      TEfficiency eff_goodmuon(*h_num_goodmuon, *h_den);
      TEfficiency eff_goodmuon_l1(*h_num_goodmuon_l1, *h_den);
      eff_goodmuon.SetName(("eff_" + base_tag + "_goodMuon").c_str());
      eff_goodmuon_l1.SetName(("eff_" + base_tag + "_goodMuonL1").c_str());
      eff_goodmuon.SetStatisticOption(TEfficiency::kFCP);
      eff_goodmuon_l1.SetStatisticOption(TEfficiency::kFCP);

      {
        const auto& study_case = kCases[0];
        const OutputPaths paths = make_output_paths(fs::absolute(opts.output_dir), opts.output_prefix, plot, study_case);
        print_case_output_info(paths, plot, study_case);
        auto h_all = merge_saved_no_trig_histograms(files, plot, study_case);
        const auto goodmuon_rows =
            build_summary_rows(eff_goodmuon, *h_den, *h_num_goodmuon, *h_all, h_legacy_mjj_all.get());
        TFile root_out(paths.root_file.string().c_str(), "RECREATE");
        if (root_out.IsZombie()) {
          die("Could not create output ROOT file: " + paths.root_file.string());
        }
        root_out.cd();
        h_all->Write();
        h_den->Write();
        h_num_goodmuon->Write();
        if (h_legacy_mjj_all) {
          h_legacy_mjj_all->Write();
        }
        eff_goodmuon.Write();
        auto graph = std::unique_ptr<TGraphAsymmErrors>(eff_goodmuon.CreateGraph());
        graph->SetName(("g_efficiency_" + base_tag + "_" + study_case.folder_tag).c_str());
        graph->Write();
        write_summary_csv(paths.summary_csv, goodmuon_rows);
        const double x99_gev = find_turnon_x_gev(*graph, 0.99);
        const double first_positive_diff_gev = find_first_positive_diff_edge_gev_after_turnon(goodmuon_rows, x99_gev);
        draw_efficiency_plot(*graph,
                             paths.eff_pdf,
                             root_out,
                             opts.lumi_pb,
                             opts.dataset_label,
                             plot,
                             study_case.folder_tag + "_full",
                             plot.full_min_gev,
                             plot.full_max_gev,
                             first_positive_diff_gev,
                             20,
                             kBlue + 1,
                             plot.draw_99_efficiency_line,
                             plot.draw_full_efficiency_line);
        if (plot.write_zoom_plot) {
          draw_efficiency_plot(*graph,
                               paths.eff_zoom_pdf,
                               root_out,
                               opts.lumi_pb,
                               opts.dataset_label,
                               plot,
                               study_case.folder_tag + "_zoom",
                               plot.zoom_min_gev,
                               plot.zoom_max_gev,
                               first_positive_diff_gev,
                               20,
                               kBlue + 1,
                               plot.draw_99_efficiency_line,
                               plot.draw_full_efficiency_line);
        }
        if (plot.write_counts_plot) {
          draw_count_overlay_plot(*h_den,
                                  *h_num_goodmuon,
                                  paths.counts_pdf,
                                  root_out,
                                  opts.dataset_label,
                                  plot,
                                  study_case.folder_tag);
        }
      }

      {
        const auto& study_case = kCases[1];
        const OutputPaths paths = make_output_paths(fs::absolute(opts.output_dir), opts.output_prefix, plot, study_case);
        print_case_output_info(paths, plot, study_case);
        auto h_all = merge_saved_no_trig_histograms(files, plot, study_case);
        const auto goodmuon_l1_rows =
            build_summary_rows(eff_goodmuon_l1, *h_den, *h_num_goodmuon_l1, *h_all, h_legacy_mjj_all.get());
        TFile root_out(paths.root_file.string().c_str(), "RECREATE");
        if (root_out.IsZombie()) {
          die("Could not create output ROOT file: " + paths.root_file.string());
        }
        root_out.cd();
        h_all->Write();
        h_den->Write();
        h_num_goodmuon_l1->Write();
        if (h_legacy_mjj_all) {
          h_legacy_mjj_all->Write();
        }
        eff_goodmuon_l1.Write();
        auto graph = std::unique_ptr<TGraphAsymmErrors>(eff_goodmuon_l1.CreateGraph());
        graph->SetName(("g_efficiency_" + base_tag + "_" + study_case.folder_tag).c_str());
        graph->Write();
        write_summary_csv(paths.summary_csv, goodmuon_l1_rows);
        const double x99_gev = find_turnon_x_gev(*graph, 0.99);
        const double first_positive_diff_gev = find_first_positive_diff_edge_gev_after_turnon(goodmuon_l1_rows, x99_gev);
        draw_efficiency_plot(*graph,
                             paths.eff_pdf,
                             root_out,
                             opts.lumi_pb,
                             opts.dataset_label,
                             plot,
                             study_case.folder_tag + "_full",
                             plot.full_min_gev,
                             plot.full_max_gev,
                             first_positive_diff_gev,
                             20,
                             kBlue + 1,
                             plot.draw_99_efficiency_line,
                             plot.draw_full_efficiency_line);
        if (plot.write_zoom_plot) {
          draw_efficiency_plot(*graph,
                               paths.eff_zoom_pdf,
                               root_out,
                               opts.lumi_pb,
                               opts.dataset_label,
                               plot,
                               study_case.folder_tag + "_zoom",
                               plot.zoom_min_gev,
                               plot.zoom_max_gev,
                               first_positive_diff_gev,
                               20,
                               kBlue + 1,
                               plot.draw_99_efficiency_line,
                               plot.draw_full_efficiency_line);
        }
        if (plot.write_counts_plot) {
          draw_count_overlay_plot(*h_den,
                                  *h_num_goodmuon_l1,
                                  paths.counts_pdf,
                                  root_out,
                                  opts.dataset_label,
                                  plot,
                                  study_case.folder_tag);
        }
      }
    }
    std::cout << info_tag() << " Done." << std::endl;
    return 0;
  } catch (const std::exception& ex) {
    std::cerr << warn_tag() << ' ' << ex.what() << std::endl;
    return 1;
  }
}

