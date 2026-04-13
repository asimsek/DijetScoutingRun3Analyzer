#include <TCanvas.h>
#include <TError.h>
#include <TEfficiency.h>
#include <TFile.h>
#include <TGraphAsymmErrors.h>
#include <TH1.h>
#include <TH1D.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>
#include <TPaveText.h>
#include <TROOT.h>
#include <TStyle.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <glob.h>
#include <iomanip>
#include <iostream>
#include <memory>
#include <regex>
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

constexpr const char* kDefaultCmsLabel = "CMS";
constexpr const char* kDefaultExtraLabel = "Preliminary";
constexpr const char* kDefaultSqrtS = "13.6 TeV";
constexpr const char* kAnsiReset = "\033[0m";
constexpr const char* kAnsiBrightBlue = "\033[94m";
constexpr const char* kAnsiBrightCyan = "\033[96m";
constexpr const char* kAnsiBrightRed = "\033[91m";

constexpr double kEfficiencyYMin = 0.0;
constexpr double kEfficiencyYMax = 1.30;
constexpr int kTurnOnConsecutivePoints = 3;

struct Options {
  std::string input_list;
  std::vector<std::string> input_roots;
  std::string output_dir;
  std::string output_prefix = "pfscouting_saved_hist_trigger";
  double lumi_pb = 5490.0;
  std::string year = "2024";
  int even_bin_width_gev = 10;
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

struct PlotConfig {
  std::string observable_folder;
  std::string binning_folder;
  std::string source_tag;
  std::string output_group_folder;
  std::string jet_label;
  std::string axis_title;
  std::string selection_label;
  bool rebin_even = false;
  bool omit_binning_subfolder = false;
  double full_min_gev = 0.0;
  double full_max_gev = 0.0;
  double zoom_min_gev = 0.0;
  double zoom_max_gev = 0.0;
  double counts_min_gev = 30.0;
  double counts_max_gev = 6000.0;
  double x_scale = 1000.0;
  bool use_legacy_mjj_counts = true;
  bool draw_99_efficiency_line = true;
  bool draw_full_efficiency_line = true;
  bool write_zoom_plot = true;
  bool write_counts_plot = true;
};

struct CaseConfig {
  std::string folder_tag;
  std::string source_tag;
};

struct MergedHistSet {
  std::unique_ptr<TH1D> all;
  std::unique_ptr<TH1D> numerator;
  std::unique_ptr<TH1D> denominator;
  std::unique_ptr<TH1D> legacy_mjj_all;
  size_t files_with_any_hist = 0;
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

// PlotConfig order:
// {observable_folder, binning_folder, source_tag, output_group_folder, jet_label, axis_title, selection_label,
//  rebin_even, omit_binning_subfolder, full_min_gev, full_max_gev, zoom_min_gev, zoom_max_gev,
//  counts_min_gev, counts_max_gev, x_scale, use_legacy_mjj_counts,
//  draw_99_efficiency_line, draw_full_efficiency_line, write_zoom_plot, write_counts_plot}
const std::array<PlotConfig, 8> kPlots = {{
    {"wideJet",  "dijetBinned",  "wideDijet",         "",              "Wide PF-jets",  "Wide-jet dijet mass [TeV]",  "|#eta| < 2.5, |#Delta#eta| < 1.3",  false, false, 100.0, 2000.0, 100.0, 900.0, 0.0, 6000.0, 1000.0, true,  true,  true,  true,  true },
    {"wideJet",  "even10GeV",    "wideDijetEven1GeV", "",              "Wide PF-jets",  "Wide-jet dijet mass [TeV]",  "|#eta| < 2.5, |#Delta#eta| < 1.3",  true,  false, 100.0, 2000.0, 100.0, 900.0, 0.0, 6000.0, 1000.0, true,  true,  true,  true,  true },
    {"AK4PFJet", "dijetBinned",  "AK4Jets",           "",              "AK4 PF-jets",   "AK4 PF dijet mass [TeV]",    "|#eta| < 2.5, |#Delta#eta| < 1.3",  false, false, 100.0, 2000.0, 100.0, 900.0, 0.0, 6000.0, 1000.0, true,  true,  true,  true,  true },
    {"AK4PFJet", "even10GeV",    "AK4JetsEven1GeV",   "",              "AK4 PF-jets",   "AK4 PF dijet mass [TeV]",    "|#eta| < 2.5, |#Delta#eta| < 1.3",  true,  false, 100.0, 2000.0, 100.0, 900.0, 0.0, 6000.0, 1000.0, true,  true,  true,  true,  true },
    {"HTAK4PF",  "even10GeV",    "HTAK4PF",           "",              "AK4 PF H_{T}",  "AK4 PF H_{T} [TeV]",         "p_{T} > 30 GeV, |#eta| < 2.5",      true,  false, 100.0, 1600.0, 100.0, 900.0, 0.0, 6000.0, 1000.0, true,  true,  true,  true,  true },
    {"muonEta",  "fixedBinning", "muonEta",           "muonID_study",  "",              "Muon #eta",                  "",                                  false, true,  -2.4,  2.4,   -2.4,  2.4,  -2.4, 2.4,    1.0,    false, false, false, false, false},
    {"muonPt",   "fixedBinning", "muonPt",            "muonID_study",  "",              "Muon p_{T} [GeV]",           "",                                  false, true,  0.0,   200.0, 0.0,   120.0, 0.0, 200.0,  1.0,    false, false, false, false, false},
    {"muonPV",   "fixedBinning", "muonPV",            "muonID_study",  "",              "Number of primary vertices", "",                                  false, true,  0.0,   80.0,  0.0,   80.0,  0.0, 80.0,   1.0,    false, false, false, false, false},
}};

const std::array<CaseConfig, 4> kCases = {{
    {"nominal", "default"},
    {"L1Veto", "L1"},
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

std::string error_tag() {
  return colored_tag("[ERROR]", kAnsiBrightRed);
}

void print_usage(const char* argv0) {
  std::cout
      << "Usage: " << argv0 << " [--input-list <file>] [--input-root <file>] [options]\n\n"
      << "Options:\n"
      << "  --input-list <file>      Text file listing ROOT files to read.\n"
      << "  --input-root <file>      ROOT file to read directly. Can be given more than once.\n"
      << "  --output-dir <dir>       Output directory (default: trigger_efficiency_saved_hists_<year>).\n"
      << "  --output-prefix <text>   Prefix for output file names.\n"
      << "  --even-bin-width-gev <n> Rebin the saved 1 GeV even histograms to n GeV (default: 10).\n"
      << "  --lumi-pb <value>        Integrated luminosity in pb^-1 (default: 5490).\n"
      << "  --year <label>           Data-taking label shown on the plots (default: 2024).\n"
      << "  --help                   Show this help message.\n";
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
    } else if (arg == "--input-root") {
      opts.input_roots.push_back(require_value(arg));
    } else if (arg == "--output-dir") {
      opts.output_dir = require_value(arg);
    } else if (arg == "--output-prefix") {
      opts.output_prefix = require_value(arg);
    } else if (arg == "--even-bin-width-gev") {
      opts.even_bin_width_gev = std::stoi(require_value(arg));
    } else if (arg == "--lumi-pb") {
      opts.lumi_pb = std::stod(require_value(arg));
    } else if (arg == "--year") {
      opts.year = require_value(arg);
    } else if (arg == "--help" || arg == "-h") {
      print_usage(argv[0]);
      std::exit(0);
    } else {
      die("Unknown argument: " + arg);
    }
  }

  if (opts.input_list.empty() && opts.input_roots.empty()) {
    die("At least one input source is required: use --input-list and/or --input-root");
  }
  if (opts.even_bin_width_gev <= 0) {
    die("--even-bin-width-gev must be a positive integer");
  }
  if (opts.output_dir.empty()) {
    opts.output_dir = "trigger_efficiency_saved_hists_" +
                      std::regex_replace(opts.year, std::regex(R"([^A-Za-z0-9_.-]+)"), "_");
  }
  return opts;
}

PlotConfig resolve_plot_config(const PlotConfig& plot, const Options& opts) {
  PlotConfig resolved = plot;
  if (resolved.rebin_even) {
    resolved.binning_folder = "even" + std::to_string(opts.even_bin_width_gev) + "GeV";
  }
  return resolved;
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
  out.push_back(fs::path(normalized).string());
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

std::vector<std::string> read_direct_inputs(const std::vector<std::string>& input_roots) {
  std::vector<std::string> files;
  files.reserve(input_roots.size());
  const fs::path base_dir = fs::current_path();
  for (const auto& token : input_roots) {
    const auto expanded = expand_source_token(token, base_dir);
    files.insert(files.end(), expanded.begin(), expanded.end());
  }
  return deduplicate(files);
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
  return format_fb_value(lumi_pb) + " fb^{-1} (" + std::string(kDefaultSqrtS) + ")";
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
  if (std::isnan(value)) {
    return "";
  }
  return std::to_string(static_cast<long long>(std::llround(value)));
}

std::unique_ptr<TH1D> clone_hist_as_double(const TH1& source, const std::string& name);

std::unique_ptr<TH1D> rebin_histogram_even_gev(const TH1D& source, int target_bin_width_gev, const std::string& name) {
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

bool merge_histogram(TFile& file,
                     const std::string& primary_name,
                     const std::string& fallback_name,
                     const std::string& merged_name,
                     std::unique_ptr<TH1D>& merged_hist) {
  TH1* input_hist = dynamic_cast<TH1*>(file.Get(primary_name.c_str()));
  if (!input_hist && !fallback_name.empty()) {
    input_hist = dynamic_cast<TH1*>(file.Get(fallback_name.c_str()));
  }
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

bool has_hist_contents(const TH1D& hist) {
  return hist.GetEntries() > 0.0 || hist.Integral(0, hist.GetNbinsX() + 1) > 0.0;
}

std::string trigger_hist_name(const PlotConfig& plot, const CaseConfig& study_case, const char* trig_name) {
  return "h_" + plot.source_tag + "_HLTpass_" + study_case.source_tag + "_" + trig_name;
}

MergedHistSet merge_case_histograms(const std::vector<std::string>& files,
                                    const PlotConfig& plot,
                                    const CaseConfig& study_case) {
  MergedHistSet merged;
  for (const auto& file_name : files) {
    std::unique_ptr<TFile> file(TFile::Open(file_name.c_str(), "READ"));
    if (!file || file->IsZombie()) {
      std::cerr << warn_tag() << " Could not open " << file_name << std::endl;
      continue;
    }

    const std::string no_trig_name = trigger_hist_name(plot, study_case, "noTrig");
    const std::string overlap_name = trigger_hist_name(plot, study_case, "PFScouting_JetHTAndSingleMuon");
    const std::string singlemu_name = trigger_hist_name(plot, study_case, "PFScouting_SingleMuon");

    bool found_any = false;
    found_any |= merge_histogram(*file,
                                 no_trig_name,
                                 "",
                                 plot.observable_folder + "_" + plot.binning_folder + "_" + study_case.folder_tag + "_all",
                                 merged.all);
    found_any |= merge_histogram(*file,
                                 overlap_name,
                                 "",
                                 plot.observable_folder + "_" + plot.binning_folder + "_" + study_case.folder_tag + "_numerator",
                                 merged.numerator);
    found_any |= merge_histogram(*file,
                                 singlemu_name,
                                 "",
                                 plot.observable_folder + "_" + plot.binning_folder + "_" + study_case.folder_tag + "_denominator",
                                 merged.denominator);
    if (plot.use_legacy_mjj_counts) {
      const std::string legacy_no_trig_name = plot.rebin_even ? "h_mjj_noTrig_1GeVbin" : "h_mjj_HLTpass_noTrig";
      found_any |= merge_histogram(*file,
                                   legacy_no_trig_name,
                                   "",
                                   plot.observable_folder + "_" + plot.binning_folder + "_" + study_case.folder_tag +
                                       "_legacy_mjj_all",
                                   merged.legacy_mjj_all);
    }
    if (found_any) {
      ++merged.files_with_any_hist;
    }
  }
  return merged;
}

void rebin_merged_histograms(MergedHistSet& merged,
                             const PlotConfig& plot,
                             const CaseConfig& study_case,
                             int even_bin_width_gev) {
  if (!plot.rebin_even) {
    return;
  }
  if (merged.all) {
    merged.all = rebin_histogram_even_gev(
        *merged.all,
        even_bin_width_gev,
        plot.observable_folder + "_" + plot.binning_folder + "_" + study_case.folder_tag + "_all_rebinned");
  }
  if (merged.numerator) {
    merged.numerator = rebin_histogram_even_gev(
        *merged.numerator,
        even_bin_width_gev,
        plot.observable_folder + "_" + plot.binning_folder + "_" + study_case.folder_tag + "_numerator_rebinned");
  }
  if (merged.denominator) {
    merged.denominator = rebin_histogram_even_gev(
        *merged.denominator,
        even_bin_width_gev,
        plot.observable_folder + "_" + plot.binning_folder + "_" + study_case.folder_tag + "_denominator_rebinned");
  }
  if (merged.legacy_mjj_all) {
    merged.legacy_mjj_all = rebin_histogram_even_gev(
        *merged.legacy_mjj_all,
        even_bin_width_gev,
        plot.observable_folder + "_" + plot.binning_folder + "_" + study_case.folder_tag + "_legacy_mjj_all_rebinned");
  }
}

std::vector<SummaryRow> build_summary_rows(TEfficiency& efficiency,
                                           const TH1D& denom_hist,
                                           const TH1D& numer_hist,
                                           const TH1D* all_hist,
                                           const TH1D* legacy_mjj_all_hist) {
  std::vector<SummaryRow> rows;
  rows.reserve(denom_hist.GetNbinsX());
  const TAxis* xaxis = denom_hist.GetXaxis();
  for (int bin_idx = 1; bin_idx <= denom_hist.GetNbinsX(); ++bin_idx) {
    SummaryRow row;
    row.bin_low = xaxis->GetBinLowEdge(bin_idx);
    row.bin_high = xaxis->GetBinUpEdge(bin_idx);
    row.bin_range = format_bin_edge(row.bin_low) + "-" + format_bin_edge(row.bin_high);
    row.all_count = all_hist ? all_hist->GetBinContent(bin_idx) : 0.0;
    if (legacy_mjj_all_hist) {
      row.legacy_mjj_all_count = legacy_mjj_all_hist->GetBinContent(bin_idx);
    }
    row.denominator_count = denom_hist.GetBinContent(bin_idx);
    row.numerator_count = numer_hist.GetBinContent(bin_idx);
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

std::unique_ptr<TGraphAsymmErrors> graph_scale_x(const TGraphAsymmErrors& source,
                                                 const std::string& name,
                                                 double scale) {
  auto graph = std::make_unique<TGraphAsymmErrors>(source);
  graph->SetName(name.c_str());
  if (scale == 0.0) {
    die("Encountered a zero x-scale while building a plot graph");
  }
  for (int i = 0; i < graph->GetN(); ++i) {
    graph->GetX()[i] /= scale;
    graph->GetEXlow()[i] /= scale;
    graph->GetEXhigh()[i] /= scale;
  }
  return graph;
}

std::unique_ptr<TH1D> hist_scale_x(const TH1D& source, const std::string& name, double scale) {
  if (scale == 0.0) {
    die("Encountered a zero x-scale while building a plot histogram");
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
  for (int bin = 0; bin <= source.GetNbinsX() + 1; ++bin) {
    hist->SetBinContent(bin, source.GetBinContent(bin));
    hist->SetBinError(bin, source.GetBinError(bin));
  }
  hist->SetEntries(source.GetEntries());
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

void draw_efficiency_plot(TGraphAsymmErrors& graph,
                          const fs::path& output_pdf,
                          TFile& root_out,
                          double lumi_pb,
                          const std::string& year,
                          const PlotConfig& plot,
                          const std::string& plot_tag,
                          const std::string& case_label,
                          double x_min_gev,
                          double x_max_gev,
                          double first_positive_legacy_diff_gev,
                          int marker_style = 20,
                          int marker_color = kBlack,
                          bool draw_99_efficiency_line = false,
                          bool draw_full_efficiency_line = false) {
  TCanvas canvas(("c_trigger_efficiency_" + plot.observable_folder + "_" + plot.binning_folder + "_" + plot_tag).c_str(), "", 800, 800);
  canvas.cd();
  gPad->SetTopMargin(0.08);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.16);
  gPad->SetBottomMargin(0.13);

  auto graph_plot =
      graph_scale_x(graph, "g_pfscouting_jetht_efficiency_plot_" + plot.observable_folder + "_" + plot.binning_folder + "_" + plot_tag,
                    plot.x_scale);
  graph_plot->SetTitle("");
  graph_plot->GetXaxis()->SetTitle(plot.axis_title.c_str());
  const bool is_muon_selection_study = (plot.output_group_folder == "muonID_study");
  graph_plot->GetYaxis()->SetTitle(is_muon_selection_study ? "Efficiency" : "Trigger efficiency");
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
    auto find_turnon_x = [&](double threshold) -> double {
      const int n_points = graph_plot->GetN();
      if (n_points <= 0) {
        return std::numeric_limits<double>::quiet_NaN();
      }

      const int required_points = std::min(kTurnOnConsecutivePoints, n_points);
      for (int i = 0; i < graph_plot->GetN(); ++i) {
        bool sustained = true;
        for (int j = 0; j < required_points; ++j) {
          const int idx = i + j;
          if (idx >= n_points || graph_plot->GetY()[idx] < threshold) {
            sustained = false;
            break;
          }
        }
        if (sustained) {
          return graph_plot->GetX()[i];
        }
      }
      return std::numeric_limits<double>::quiet_NaN();
    };

    if (draw_99_efficiency_line) {
      const double x99 = find_turnon_x(0.99);
      if (!std::isnan(x99)) {
        auto line99 = std::make_unique<TLine>(x99, kEfficiencyYMin, x99, kEfficiencyYMax);
        line99->SetLineColor(kCyan + 1);
        line99->SetLineStyle(2);
        line99->SetLineWidth(2);
        line99->Draw();
        turnon_lines.push_back(std::move(line99));
      }
    }
    if (draw_full_efficiency_line && !std::isnan(first_positive_legacy_diff_gev)) {
      const double x_legacy = first_positive_legacy_diff_gev / plot.x_scale;
      auto line_legacy = std::make_unique<TLine>(x_legacy, kEfficiencyYMin, x_legacy, kEfficiencyYMax);
      line_legacy->SetLineColor(kRed + 1);
      line_legacy->SetLineStyle(2);
      line_legacy->SetLineWidth(2);
      line_legacy->Draw();
      turnon_lines.push_back(std::move(line_legacy));
    }
  }

  TLegend legend(0.62, 0.80, 0.94, 0.90);
  legend.SetBorderSize(0);
  legend.SetFillStyle(0);
  legend.SetTextSize(0.034);
  const std::string legend_label = "Data [" + year + "]";
  legend.AddEntry(graph_plot.get(), legend_label.c_str(), "pe");
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
                             const PlotConfig& plot,
                             const std::string& case_label) {
  TCanvas canvas(("c_trigger_event_counts_" + plot.observable_folder + "_" + plot.binning_folder + "_" + case_label).c_str(), "", 800, 800);
  canvas.cd();
  gPad->SetTopMargin(0.08);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.16);
  gPad->SetBottomMargin(0.13);
  gPad->SetLogy();

  auto h_denom_plot =
      hist_scale_x(denom_hist, "h_denom_counts_plot_" + plot.observable_folder + "_" + plot.binning_folder + "_" + case_label,
                   plot.x_scale);
  auto h_numer_plot =
      hist_scale_x(numer_hist, "h_numer_counts_plot_" + plot.observable_folder + "_" + plot.binning_folder + "_" + case_label,
                   plot.x_scale);

  h_denom_plot->SetTitle("");
  h_denom_plot->GetXaxis()->SetTitle(plot.axis_title.c_str());
  h_denom_plot->GetYaxis()->SetTitle("Events / bin");
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

  TH1F* frame =
      gPad->DrawFrame(plot.counts_min_gev / plot.x_scale, 0.5, plot.counts_max_gev / plot.x_scale, std::max(10.0, y_max * 5.0));
  frame->SetTitle("");
  frame->GetXaxis()->SetTitle(plot.axis_title.c_str());
  frame->GetYaxis()->SetTitle("Events / bin");
  frame->GetXaxis()->SetTitleSize(0.05);
  frame->GetYaxis()->SetTitleSize(0.05);
  frame->GetXaxis()->SetLabelSize(0.045);
  frame->GetYaxis()->SetLabelSize(0.045);
  frame->GetYaxis()->SetTitleOffset(1.35);

  h_denom_plot->Draw("HIST SAME");
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

OutputPaths make_output_paths(const fs::path& base_output_dir,
                              const std::string& output_prefix,
                              const PlotConfig& plot,
                              const CaseConfig& study_case) {
  OutputPaths paths;
  const std::string top_folder = plot.output_group_folder.empty() ? plot.observable_folder : plot.output_group_folder;
  paths.directory = base_output_dir / top_folder;
  if (!plot.omit_binning_subfolder && !plot.binning_folder.empty()) {
    paths.directory /= plot.binning_folder;
  }
  paths.directory /= study_case.folder_tag;
  fs::create_directories(paths.directory);
  paths.stem = output_prefix + "_" + plot.observable_folder;
  if (!plot.omit_binning_subfolder && !plot.binning_folder.empty()) {
    paths.stem += "_" + plot.binning_folder;
  }
  paths.stem += "_" + study_case.folder_tag;
  paths.root_file = paths.directory / (paths.stem + ".root");
  paths.summary_csv = paths.directory / (paths.stem + "_summary.csv");
  paths.eff_pdf = paths.directory / (paths.stem + "_efficiency.pdf");
  paths.eff_zoom_pdf = paths.directory / (paths.stem + "_efficiency_zoom.pdf");
  paths.counts_pdf = paths.directory / (paths.stem + "_counts.pdf");
  return paths;
}

void print_case_output_info(const OutputPaths& paths, const PlotConfig& plot, const CaseConfig& study_case) {
  (void)paths;
  const auto info = info_tag();
  std::cout << info << ' ' << plot.observable_folder;
  if (!plot.omit_binning_subfolder && !plot.binning_folder.empty()) {
    std::cout << " | " << plot.binning_folder;
  }
  std::cout << " | " << study_case.folder_tag << std::endl;
}

double find_first_positive_legacy_diff_edge_gev(const std::vector<SummaryRow>& rows) {
  for (const auto& row : rows) {
    if (!std::isnan(row.legacy_mjj_diff_inv_sqrt_n_minus_inefficiency) &&
        row.legacy_mjj_diff_inv_sqrt_n_minus_inefficiency > 0.0) {
      return row.bin_low;
    }
  }
  return std::numeric_limits<double>::quiet_NaN();
}

}  // namespace

int main(int argc, char** argv) {
  try {
    TH1::AddDirectory(kFALSE);
    gErrorIgnoreLevel = kWarning;
    configure_root_style();

    const Options opts = parse_args(argc, argv);
    std::vector<std::string> input_files;
    if (!opts.input_list.empty()) {
      const auto listed_files = read_data_inputs(opts.input_list);
      input_files.insert(input_files.end(), listed_files.begin(), listed_files.end());
    }
    if (!opts.input_roots.empty()) {
      const auto direct_files = read_direct_inputs(opts.input_roots);
      input_files.insert(input_files.end(), direct_files.begin(), direct_files.end());
    }
    input_files = deduplicate(input_files);
    if (input_files.empty()) {
      die("No ROOT files were found from the provided --input-list/--input-root arguments");
    }
    const fs::path base_output_dir = fs::absolute(opts.output_dir);
    fs::create_directories(base_output_dir);

    std::cout << info_tag() << " Reading " << input_files.size() << " ROOT file(s)" << std::endl;

    for (const auto& plot_template : kPlots) {
      for (const auto& study_case : kCases) {
        const PlotConfig plot = resolve_plot_config(plot_template, opts);
        auto rebinned = merge_case_histograms(input_files, plot, study_case);
        rebin_merged_histograms(rebinned, plot, study_case, opts.even_bin_width_gev);

        if (!rebinned.denominator || !rebinned.numerator) {
          std::cerr << error_tag() << ' ' << plot.observable_folder << '/' << plot.binning_folder << '/'
                    << study_case.folder_tag
                    << ": missing required histograms. Need numerator=PFScouting_JetHTAndSingleMuon and "
                       "denominator=PFScouting_SingleMuon."
                    << std::endl;
          continue;
        }

        if (!has_hist_contents(*rebinned.denominator)) {
          std::cerr << error_tag() << ' ' << plot.observable_folder << '/' << plot.binning_folder << '/'
                    << study_case.folder_tag
                    << ": denominator histogram PFScouting_SingleMuon is empty." << std::endl;
          continue;
        }

        if (!has_hist_contents(*rebinned.numerator)) {
          std::cerr << error_tag() << ' ' << plot.observable_folder << '/' << plot.binning_folder << '/'
                    << study_case.folder_tag
                    << ": numerator histogram PFScouting_JetHTAndSingleMuon is empty." << std::endl;
          continue;
        }

        if (!TEfficiency::CheckConsistency(*rebinned.numerator, *rebinned.denominator)) {
          std::cerr << error_tag() << ' ' << plot.observable_folder << '/' << plot.binning_folder << '/'
                    << study_case.folder_tag
                    << ": PFScouting_JetHTAndSingleMuon and PFScouting_SingleMuon are not TEfficiency-consistent."
                    << std::endl;
          continue;
        }

        const OutputPaths paths = make_output_paths(base_output_dir, opts.output_prefix, plot, study_case);
        print_case_output_info(paths, plot, study_case);

        TFile root_out(paths.root_file.string().c_str(), "RECREATE");
        if (root_out.IsZombie()) {
          die("Could not create output ROOT file: " + paths.root_file.string());
        }

        rebinned.denominator->SetName(("h_denominator_" + plot.observable_folder + "_" + plot.binning_folder + "_" + study_case.folder_tag).c_str());
        rebinned.numerator->SetName(("h_numerator_" + plot.observable_folder + "_" + plot.binning_folder + "_" + study_case.folder_tag).c_str());
        rebinned.denominator->Write();
        rebinned.numerator->Write();
        if (rebinned.all) {
          rebinned.all->SetName(("h_all_" + plot.observable_folder + "_" + plot.binning_folder + "_" + study_case.folder_tag).c_str());
          rebinned.all->Write();
        } else {
          std::cerr << warn_tag() << ' ' << plot.observable_folder << '/' << plot.binning_folder << '/'
                    << study_case.folder_tag
                    << ": noTrig histogram is missing, so 1/sqrt(N) will be left empty in the summary." << std::endl;
        }
        if (rebinned.legacy_mjj_all) {
          rebinned.legacy_mjj_all->SetName(
              ("h_legacy_mjj_all_" + plot.observable_folder + "_" + plot.binning_folder + "_" + study_case.folder_tag).c_str());
          rebinned.legacy_mjj_all->Write();
        } else if (plot.use_legacy_mjj_counts) {
          std::cerr << warn_tag() << ' ' << plot.observable_folder << '/' << plot.binning_folder << '/'
                    << study_case.folder_tag
                    << ": legacy noTrig histogram is missing, so the legacy 1/sqrt(N) columns will be left empty."
                    << std::endl;
        }

        auto ratio_hist = clone_hist_as_double(*rebinned.numerator,
                                               "h_ratio_" + plot.observable_folder + "_" + plot.binning_folder + "_" + study_case.folder_tag);
        ratio_hist->Divide(rebinned.denominator.get());
        ratio_hist->Write();

        TEfficiency efficiency(*rebinned.numerator, *rebinned.denominator);
        efficiency.SetName(("teff_" + plot.observable_folder + "_" + plot.binning_folder + "_" + study_case.folder_tag).c_str());
        efficiency.SetStatisticOption(TEfficiency::kFCP);
        auto graph = std::unique_ptr<TGraphAsymmErrors>(efficiency.CreateGraph());
        const auto summary_rows = build_summary_rows(
            efficiency, *rebinned.denominator, *rebinned.numerator, rebinned.all.get(), rebinned.legacy_mjj_all.get());
        const double first_positive_legacy_diff_gev = find_first_positive_legacy_diff_edge_gev(summary_rows);
        efficiency.Write();
        graph->SetName(("g_efficiency_" + plot.observable_folder + "_" + plot.binning_folder + "_" + study_case.folder_tag).c_str());
        write_summary_csv(paths.summary_csv, summary_rows);

        graph->Write();

        draw_efficiency_plot(*graph,
                             paths.eff_pdf,
                             root_out,
                             opts.lumi_pb,
                             opts.year,
                             plot,
                             study_case.folder_tag + "_full",
                             study_case.folder_tag,
                             plot.full_min_gev,
                             plot.full_max_gev,
                             first_positive_legacy_diff_gev,
                             20,
                             kBlue + 1,
                             plot.draw_99_efficiency_line,
                             plot.draw_full_efficiency_line);
        if (plot.write_zoom_plot) {
          draw_efficiency_plot(*graph,
                               paths.eff_zoom_pdf,
                               root_out,
                               opts.lumi_pb,
                               opts.year,
                               plot,
                               study_case.folder_tag + "_zoom",
                               study_case.folder_tag,
                               plot.zoom_min_gev,
                               plot.zoom_max_gev,
                               first_positive_legacy_diff_gev,
                               20,
                               kBlue + 1,
                               plot.draw_99_efficiency_line,
                               plot.draw_full_efficiency_line);
        }
        if (plot.write_counts_plot) {
          draw_count_overlay_plot(*rebinned.denominator,
                                  *rebinned.numerator,
                                  paths.counts_pdf,
                                  root_out,
                                  opts.year,
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

