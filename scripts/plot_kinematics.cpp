#include <ROOT/TThreadExecutor.hxx>
#include <TCanvas.h>
#include <TFile.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>
#include <TPad.h>
#include <TParameter.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <glob.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

namespace {

constexpr const char* kTreePathDefault = "rootTupleTree/tree";
constexpr const char* kEventCounterPath = "DijetFilter/EventCount/EventCounter";
constexpr double kDeltaEtaCut = 1.3;
constexpr double kEtaCut = 2.5;
constexpr double kDefaultMinMjj = 1181.0;
constexpr const char* kDefaultSelection = "dijet";
constexpr const char* kDefaultDataLabel = "Data";
constexpr const char* kDefaultMcLabel = "QCD";
constexpr const char* kDefaultCmsLabel = "CMS";
constexpr const char* kDefaultExtraLabel = "Preliminary";
constexpr const char* kDefaultSqrtS = "13.6 TeV";
constexpr int kCanvasWidth = 800;
constexpr int kCanvasHeight = 900;
constexpr int kDataColor = kBlack;
constexpr int kMcLineColor = kRed;
constexpr int kMarkerStyle = 20;
constexpr double kMarkerSize = 1.0;
constexpr int kLineWidth = 3;
constexpr int kDefaultThreads = 8;

const std::vector<double> kMassBins = {
    1, 3, 6, 10, 16, 23, 31, 40, 50, 61, 74, 88, 103, 119, 137, 156, 176, 197, 220, 244, 270, 296, 325,
    354, 386, 419, 453, 489, 526, 565, 606, 649, 693, 740, 788, 838, 890, 944, 1000, 1058, 1118, 1181,
    1246, 1313, 1383, 1455, 1530, 1607, 1687, 1770, 1856, 1945, 2037, 2132, 2231, 2332, 2438, 2546, 2659,
    2775, 2895, 3019, 3147, 3279, 3416, 3558, 3704, 3854, 4010, 4171, 4337, 4509, 4686, 4869, 5058, 5253,
    5455, 5663, 5877, 6099, 6328, 6564, 6808, 7060, 7320, 7589, 7866, 8152, 8447, 8752, 9067, 9391, 9726,
    10072, 10430, 10798, 11179, 11571, 11977, 12395, 12827, 13272, 13732, 14000,
};

const std::string kAnsiReset = "\033[0m";
const std::string kAnsiBrightYellow = "\033[93m";
const std::string kAnsiBrightRed = "\033[91m";
const std::string kInfoTag = kAnsiBrightYellow + "[INFO]" + kAnsiReset;
const std::string kWarnTag = "[WARN]";

struct PlotSpec {
  std::string expr;
  std::string title;
  bool variable_bins = false;
  std::vector<double> bin_edges;
  int bins = 0;
  double xmin = 0.0;
  double xmax = 0.0;
  std::optional<std::pair<double, double>> display_range;
  std::string units;
  bool logy = false;
  std::pair<double, double> ratio_range = {0.4, 1.6};
};

struct PlotRequest {
  std::string name;
  const PlotSpec* spec = nullptr;
};

struct SelectionPreset {
  std::string data;
  std::string mc;
};

struct Options {
  std::string plot;
  std::string data_list;
  std::string mc_list;
  std::string output_dir = "plots";
  std::string output_prefix;
  double lumi_pb = 1.0;
  int threads = kDefaultThreads;
};

struct McSample {
  std::string source;
  std::string name;
  double xsec_pb = 0.0;
  std::vector<std::string> files;
};

struct FileProcessResult {
  std::vector<std::vector<double>> counts;
  double processed_events = 0.0;
  long long selected_events_without_hlt = 0;
  long long selected_events = 0;
  std::string error;
};

std::map<std::string, PlotSpec> build_plot_specs() {
  std::map<std::string, PlotSpec> specs;
  PlotSpec mjj;
  mjj.expr = "mjj";
  mjj.title = "Dijet mass [GeV]";
  mjj.variable_bins = true;
  mjj.bin_edges = kMassBins;
  mjj.display_range = std::make_pair(1181.0, 2132.0);
  mjj.units = "GeV";
  mjj.logy = true;
  mjj.ratio_range = {0.4, 1.6};
  specs[mjj.expr] = mjj;

  PlotSpec ak4 = mjj;
  ak4.expr = "Dijet_MassAK4PF";
  ak4.title = "AK4 dijet mass [GeV]";
  specs[ak4.expr] = ak4;

  PlotSpec p1;
  p1.expr = "pTWJ_j1";
  p1.title = "Leading wide-jet p_{T} [GeV]";
  p1.bins = 120;
  p1.xmin = 0.0;
  p1.xmax = 5000.0;
  p1.units = "GeV";
  p1.logy = true;
  p1.ratio_range = {0.4, 1.6};
  specs[p1.expr] = p1;

  PlotSpec p2 = p1;
  p2.expr = "pTWJ_j2";
  p2.title = "Subleading wide-jet p_{T} [GeV]";
  specs[p2.expr] = p2;

  PlotSpec eta1;
  eta1.expr = "etaWJ_j1";
  eta1.title = "Leading wide-jet #eta";
  eta1.bins = 60;
  eta1.xmin = -3.0;
  eta1.xmax = 3.0;
  eta1.logy = false;
  eta1.ratio_range = {0.6, 1.4};
  specs[eta1.expr] = eta1;

  PlotSpec eta2 = eta1;
  eta2.expr = "etaWJ_j2";
  eta2.title = "Subleading wide-jet #eta";
  specs[eta2.expr] = eta2;

  PlotSpec deta;
  deta.expr = "deltaETAjj";
  deta.title = "#Delta#eta_{jj}";
  deta.bins = 60;
  deta.xmin = 0.0;
  deta.xmax = kDeltaEtaCut;
  deta.logy = true;
  deta.ratio_range = {0.6, 1.4};
  specs[deta.expr] = deta;

  PlotSpec dphi;
  dphi.expr = "deltaPHIjj";
  dphi.title = "|#Delta#phi_{jj}|";
  dphi.bins = 64;
  dphi.xmin = 0.0;
  dphi.xmax = 3.14;
  dphi.logy = true;
  dphi.ratio_range = {0.0, 3.0};
  specs[dphi.expr] = dphi;

  PlotSpec phi1;
  phi1.expr = "phiWJ_j1";
  phi1.title = "Leading wide-jet #phi";
  phi1.bins = 64;
  phi1.xmin = -3.14;
  phi1.xmax = 3.14;
  phi1.logy = false;
  phi1.ratio_range = {0.6, 1.4};
  specs[phi1.expr] = phi1;

  PlotSpec phi2 = phi1;
  phi2.expr = "phiWJ_j2";
  phi2.title = "Subleading wide-jet #phi";
  specs[phi2.expr] = phi2;

  return specs;
}

std::map<std::string, SelectionPreset> build_selection_presets() {
  std::map<std::string, SelectionPreset> presets;
  presets["none"] = {"1", "1"};
  {
    std::ostringstream data;
    data << "PassJSON == 1 && nJet > 1 && mjj > " << static_cast<int>(kDefaultMinMjj);
    std::ostringstream mc;
    mc << "nJet > 1 && mjj > " << static_cast<int>(kDefaultMinMjj);
    presets["baseline"] = {data.str(), mc.str()};
  }
  {
    std::ostringstream data;
    data << "PassJSON == 1 && nJet > 1 && IdTight_j1 > 0.5 && IdTight_j2 > 0.5"
         << " && passHLT_PFScoutingHT > 0.5"
         << " && pTWJ_j1 > 30 && pTWJ_j2 > 30"
         << " && TMath::Abs(etaWJ_j1) < " << kEtaCut
         << " && TMath::Abs(etaWJ_j2) < " << kEtaCut
         << " && TMath::Abs(deltaETAjj) < " << kDeltaEtaCut
         << " && mjj > " << static_cast<int>(kDefaultMinMjj);
    std::ostringstream mc;
    mc << "nJet > 1 && IdTight_j1 > 0.5 && IdTight_j2 > 0.5"
       << " && pTWJ_j1 > 30 && pTWJ_j2 > 30"
       << " && TMath::Abs(etaWJ_j1) < " << kEtaCut
       << " && TMath::Abs(etaWJ_j2) < " << kEtaCut
       << " && TMath::Abs(deltaETAjj) < " << kDeltaEtaCut
       << " && mjj > " << static_cast<int>(kDefaultMinMjj);
    presets["dijet"] = {data.str(), mc.str()};
  }
  return presets;
}

const std::map<std::string, PlotSpec> kPlotSpecs = build_plot_specs();
const std::map<std::string, SelectionPreset> kSelectionPresets = build_selection_presets();

std::vector<std::string> deduplicate(const std::vector<std::string>& seq);

std::string format_lumi(double value) {
  std::ostringstream ss;
  ss << std::fixed << std::setprecision(6) << value;
  std::string text = ss.str();
  while (!text.empty() && text.back() == '0') {
    text.pop_back();
  }
  if (!text.empty() && text.back() == '.') {
    text.push_back('0');
  }
  return text;
}

std::string format_elapsed_seconds(const std::chrono::steady_clock::duration& elapsed) {
  const double total_seconds = std::chrono::duration<double>(elapsed).count();
  std::ostringstream ss;
  if (total_seconds < 60.0) {
    ss << std::fixed << std::setprecision(2) << total_seconds << " s";
    return ss.str();
  }

  const long long total_seconds_rounded = static_cast<long long>(std::llround(total_seconds));
  const long long hours = total_seconds_rounded / 3600;
  const long long minutes = (total_seconds_rounded % 3600) / 60;
  const long long seconds = total_seconds_rounded % 60;
  if (hours > 0) {
    ss << hours << "h " << minutes << "m " << seconds << "s";
  } else {
    ss << minutes << "m " << seconds << "s";
  }
  return ss.str();
}

std::string format_event_count(long long value) {
  const std::string text = std::to_string(value);
  std::string formatted;
  formatted.reserve(text.size() + text.size() / 3);
  const std::size_t start = !text.empty() && text.front() == '-' ? 1 : 0;
  if (start == 1) {
    formatted.push_back('-');
  }
  const std::size_t digits = text.size() - start;
  for (std::size_t i = 0; i < digits; ++i) {
    if (i != 0 && (digits - i) % 3 == 0) {
      formatted.push_back(',');
    }
    formatted.push_back(text[start + i]);
  }
  return formatted;
}

void log_info(const std::string& message) {
  std::cout << kInfoTag << ' ' << message << std::endl;
}

void log_warn(const std::string& message) {
  std::cout << kWarnTag << ' ' << message << std::endl;
}

std::string trim(const std::string& s) {
  std::size_t start = 0;
  while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
    ++start;
  }
  std::size_t end = s.size();
  while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
    --end;
  }
  return s.substr(start, end - start);
}

bool starts_with(const std::string& s, const std::string& prefix) {
  return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}

std::string combine_cuts(const std::vector<std::string>& cuts) {
  std::ostringstream ss;
  bool first = true;
  for (const std::string& cut : cuts) {
    const std::string stripped = trim(cut);
    if (stripped.empty()) {
      continue;
    }
    if (!first) {
      ss << " && ";
    }
    ss << '(' << stripped << ')';
    first = false;
  }
  return first ? "1" : ss.str();
}

std::string normalize_path_token(const std::string& token, const fs::path& base_dir) {
  std::string expanded = token;
  if (!expanded.empty() && expanded[0] == '~') {
    const char* home = std::getenv("HOME");
    if (home) {
      expanded = std::string(home) + expanded.substr(1);
    }
  }
  std::string out;
  out.reserve(expanded.size());
  for (std::size_t i = 0; i < expanded.size(); ++i) {
    if (expanded[i] == '$') {
      std::size_t j = i + 1;
      while (j < expanded.size() && (std::isalnum(static_cast<unsigned char>(expanded[j])) || expanded[j] == '_')) {
        ++j;
      }
      const std::string var = expanded.substr(i + 1, j - i - 1);
      const char* val = std::getenv(var.c_str());
      if (val) {
        out += val;
      }
      i = j - 1;
    } else {
      out.push_back(expanded[i]);
    }
  }
  if (out.find("://") != std::string::npos) {
    return out;
  }
  fs::path p(out);
  if (!p.is_absolute()) {
    p = (base_dir / p).lexically_normal();
  }
  return p.string();
}

std::vector<std::string> glob_expand(const std::string& pattern) {
  glob_t results{};
  std::vector<std::string> files;
  if (glob(pattern.c_str(), GLOB_TILDE, nullptr, &results) == 0) {
    for (std::size_t i = 0; i < results.gl_pathc; ++i) {
      files.emplace_back(results.gl_pathv[i]);
    }
  }
  globfree(&results);
  std::sort(files.begin(), files.end());
  return files;
}

std::vector<std::string> expand_source_token(const std::string& token, const fs::path& base_dir) {
  const std::string normalized = normalize_path_token(token, base_dir);
  if (normalized.find("://") != std::string::npos) {
    return {normalized};
  }
  if (normalized.find_first_of("*?[]") != std::string::npos) {
    return glob_expand(normalized);
  }
  fs::path path(normalized);
  if (fs::is_directory(path)) {
    std::vector<std::string> files;
    for (const auto& entry : fs::recursive_directory_iterator(path)) {
      if (!entry.is_regular_file()) {
        continue;
      }
      if (entry.path().filename().string().find("_reduced_skim.root") != std::string::npos) {
        files.push_back(entry.path().string());
      }
    }
    std::sort(files.begin(), files.end());
    return files;
  }
  return {path.string()};
}

std::string derive_name(const std::string& token) {
  std::string base = fs::path(token).filename().string();
  const std::vector<std::string> suffixes = {".root", "_reduced_skim"};
  for (const std::string& suffix : suffixes) {
    if (base.size() >= suffix.size() && base.compare(base.size() - suffix.size(), suffix.size(), suffix) == 0) {
      base.erase(base.size() - suffix.size());
    }
  }
  std::string safe;
  for (char c : base) {
    safe.push_back((std::isalnum(static_cast<unsigned char>(c)) || c == '_') ? c : '_');
  }
  while (!safe.empty() && safe.front() == '_') {
    safe.erase(safe.begin());
  }
  while (!safe.empty() && safe.back() == '_') {
    safe.pop_back();
  }
  return safe.empty() ? "sample" : safe;
}

fs::path resolve_output_dir(const std::string& raw_output_dir, const char* argv0) {
  const fs::path requested = fs::absolute(fs::path(raw_output_dir));
  const fs::path executable = fs::absolute(fs::path(argv0));

  if (fs::exists(requested)) {
    if (fs::is_directory(requested)) {
      return requested;
    }
    if (requested == executable) {
      const fs::path fallback = requested.string() + "_outputs";
      log_warn("Requested --output-dir conflicts with the executable path; using " + fallback.string());
      fs::create_directories(fallback);
      return fallback;
    }
    throw std::runtime_error("--output-dir points to an existing file: " + requested.string());
  }

  fs::create_directories(requested);
  return requested;
}

std::vector<std::string> deduplicate(const std::vector<std::string>& seq) {
  std::unordered_set<std::string> seen;
  std::vector<std::string> ordered;
  ordered.reserve(seq.size());
  for (const std::string& item : seq) {
    if (seen.insert(item).second) {
      ordered.push_back(item);
    }
  }
  return ordered;
}

std::vector<std::string> clean_lines(const fs::path& path) {
  std::ifstream in(path);
  if (!in.is_open()) {
    throw std::runtime_error("cannot open list file: " + path.string());
  }
  std::vector<std::string> lines;
  std::string raw;
  while (std::getline(in, raw)) {
    const std::string line = trim(raw);
    if (line.empty() || starts_with(line, "#")) {
      continue;
    }
    lines.push_back(line);
  }
  return lines;
}

std::vector<std::string> read_data_inputs(const std::string& list_path_str) {
  const fs::path list_path = fs::absolute(fs::path(list_path_str));
  std::vector<std::string> files;
  for (const std::string& line : clean_lines(list_path)) {
    const auto expanded = expand_source_token(line, list_path.parent_path());
    files.insert(files.end(), expanded.begin(), expanded.end());
  }
  files = deduplicate(files);
  if (files.empty()) {
    throw std::runtime_error("No data ROOT files found from list: " + list_path.string());
  }
  return files;
}

std::vector<McSample> read_mc_inputs(const std::string& list_path_str) {
  const fs::path list_path = fs::absolute(fs::path(list_path_str));
  std::vector<McSample> samples;
  bool warned_extra_columns = false;
  for (const std::string& line : clean_lines(list_path)) {
    std::istringstream ss(line);
    std::string source_token;
    double xsec_pb = 0.0;
    if (!(ss >> source_token >> xsec_pb)) {
      throw std::runtime_error("Invalid MC list entry in " + list_path.string() + ": " + line +
                               "\nExpected format: path_or_glob xsec_pb");
    }
    std::string extra;
    if (!warned_extra_columns && (ss >> extra)) {
      log_warn("MC list has extra columns. Only the first two columns are used: path_or_glob xsec_pb");
      warned_extra_columns = true;
    }
    std::vector<std::string> files = deduplicate(expand_source_token(source_token, list_path.parent_path()));
    if (files.empty()) {
      throw std::runtime_error("No MC ROOT files found for entry: " + line);
    }
    McSample sample;
    sample.source = source_token;
    sample.name = derive_name(source_token);
    sample.xsec_pb = xsec_pb;
    sample.files = std::move(files);
    samples.push_back(std::move(sample));
  }
  if (samples.empty()) {
    throw std::runtime_error("No MC ROOT files found from list: " + list_path.string());
  }
  return samples;
}

const PlotSpec& get_plot_spec(const std::string& name) {
  auto it = kPlotSpecs.find(name);
  if (it == kPlotSpecs.end()) {
    throw std::runtime_error("Unknown plot spec: " + name);
  }
  return it->second;
}

std::vector<PlotRequest> parse_plot_requests(const std::string& raw_plot_argument) {
  std::vector<PlotRequest> requests;
  std::vector<std::string> names;

  if (raw_plot_argument == "all") {
    for (const auto& item : kPlotSpecs) {
      names.push_back(item.first);
    }
  } else {
    std::istringstream ss(raw_plot_argument);
    std::string token;
    while (std::getline(ss, token, ',')) {
      const std::string trimmed = trim(token);
      if (!trimmed.empty()) {
        names.push_back(trimmed);
      }
    }
  }

  names = deduplicate(names);
  if (names.empty()) {
    throw std::runtime_error("--plot produced no valid plot names");
  }

  requests.reserve(names.size());
  for (const auto& name : names) {
    const PlotSpec& spec = get_plot_spec(name);
    requests.push_back({name, &spec});
  }
  return requests;
}

std::shared_ptr<TH1D> make_hist(const std::string& name, const PlotSpec& spec) {
  TH1D* hist = nullptr;
  if (spec.variable_bins) {
    hist = new TH1D(name.c_str(), "", static_cast<int>(spec.bin_edges.size()) - 1, spec.bin_edges.data());
  } else {
    hist = new TH1D(name.c_str(), "", spec.bins, spec.xmin, spec.xmax);
  }
  hist->SetDirectory(nullptr);
  hist->Sumw2();
  return std::shared_ptr<TH1D>(hist);
}

int nbins_for_spec(const PlotSpec& spec) {
  return spec.variable_bins ? static_cast<int>(spec.bin_edges.size()) - 1 : spec.bins;
}

int find_bin_index(const PlotSpec& spec, double value) {
  if (spec.variable_bins) {
    if (spec.bin_edges.size() < 2) {
      return -1;
    }
    if (value < spec.bin_edges.front()) {
      return -1;
    }
    if (value >= spec.bin_edges.back()) {
      return static_cast<int>(spec.bin_edges.size()) - 2;
    }
    auto upper = std::upper_bound(spec.bin_edges.begin(), spec.bin_edges.end(), value);
    return static_cast<int>(std::distance(spec.bin_edges.begin(), upper) - 1);
  }

  if (spec.bins <= 0 || spec.xmax <= spec.xmin) {
    return -1;
  }
  if (value < spec.xmin) {
    return -1;
  }
  if (value >= spec.xmax) {
    return spec.bins - 1;
  }
  const double width = (spec.xmax - spec.xmin) / static_cast<double>(spec.bins);
  const int bin = static_cast<int>((value - spec.xmin) / width);
  return std::clamp(bin, 0, spec.bins - 1);
}

std::shared_ptr<TH1D> hist_from_counts(const std::string& name, const PlotSpec& spec, const std::vector<double>& counts) {
  auto hist = make_hist(name, spec);
  const int nbins = nbins_for_spec(spec);
  if (static_cast<int>(counts.size()) != nbins) {
    throw std::runtime_error("count vector size does not match histogram bins for " + name);
  }
  for (int i = 0; i < nbins; ++i) {
    hist->SetBinContent(i + 1, counts[i]);
    hist->SetBinError(i + 1, std::sqrt(std::max(0.0, counts[i])));
  }
  return hist;
}

void set_hist_style(TH1D& hist, int color, bool marker) {
  hist.SetLineColor(color);
  hist.SetLineWidth(kLineWidth);
  if (marker) {
    hist.SetMarkerColor(color);
    hist.SetMarkerStyle(kMarkerStyle);
    hist.SetMarkerSize(kMarkerSize);
    hist.SetFillStyle(0);
  } else {
    hist.SetMarkerSize(0.0);
    hist.SetFillStyle(0);
  }
}

void configure_root_style() {
  gROOT->SetBatch(true);
  TH1::SetDefaultSumw2(true);
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

void merge_overflow_bin(TH1D& hist) {
  const int nbins = hist.GetNbinsX();
  hist.SetBinContent(nbins, hist.GetBinContent(nbins) + hist.GetBinContent(nbins + 1));
  hist.SetBinError(nbins, std::hypot(hist.GetBinError(nbins), hist.GetBinError(nbins + 1)));
  hist.SetBinContent(nbins + 1, 0.0);
  hist.SetBinError(nbins + 1, 0.0);
}

std::string infer_y_title(const PlotSpec& spec, const TH1D& hist) {
  if (spec.expr == "mjj" || spec.expr == "Dijet_MassAK4PF") {
    return "Events / GeV";
  }
  std::ostringstream ss;
  const double width = hist.GetXaxis()->GetBinWidth(1);
  if (!spec.units.empty()) {
    ss << "Events / " << std::fixed << std::setprecision(2) << width << ' ' << spec.units;
  } else {
    ss << "Events / " << std::fixed << std::setprecision(2) << width;
  }
  return ss.str();
}

void draw_cms_labels(TPad& pad, double lumi_pb) {
  pad.cd();
  TLatex latex;
  latex.SetNDC(true);
  latex.SetTextAlign(13);
  latex.SetTextFont(62);
  latex.SetTextSize(0.055);
  latex.DrawLatex(0.21, 0.88, kDefaultCmsLabel);
  latex.SetTextFont(52);
  latex.SetTextSize(0.040);
  latex.DrawLatex(0.21, 0.82, kDefaultExtraLabel);
  latex.SetTextFont(42);
  latex.SetTextSize(0.040);
  latex.SetTextAlign(31);
  const std::string lumi = std::to_string(static_cast<int>(std::llround(lumi_pb))) + " pb^{-1} (" + kDefaultSqrtS + ")";
  latex.DrawLatex(0.95, 0.93, lumi.c_str());
}

double compute_maximum(const TH1D* a, const TH1D* b) {
  double maximum = 0.0;
  if (a) maximum = std::max(maximum, a->GetMaximum());
  if (b) maximum = std::max(maximum, b->GetMaximum());
  return maximum;
}

double compute_positive_minimum(const TH1D* a, const TH1D* b) {
  double minimum = 0.0;
  bool set = false;
  for (const TH1D* hist : {a, b}) {
    if (!hist) continue;
    for (int i = 1; i <= hist->GetNbinsX(); ++i) {
      const double value = hist->GetBinContent(i);
      if (value <= 0.0) continue;
      if (!set || value < minimum) {
        minimum = value;
        set = true;
      }
    }
  }
  return set ? minimum : 1e-3;
}

std::pair<double, double> compute_auto_y_range(const TH1D* a, const TH1D* b, bool logy) {
  double ymax = compute_maximum(a, b);
  if (ymax <= 0.0) {
    return logy ? std::make_pair(1e-6, 1.0) : std::make_pair(0.0, 1.0);
  }
  if (logy) {
    const double ymin = std::max(compute_positive_minimum(a, b) * 0.5, 1e-6);
    if (ymax <= ymin) {
      ymax = ymin * 10.0;
    }
    const double log_min = std::log10(ymin);
    const double log_max = std::log10(ymax);
    const double span = std::max(log_max - log_min, 1.0);
    const double top_headroom = 0.22;
    const double extra_span = span * top_headroom / (1.0 - top_headroom);
    return {ymin, std::pow(10.0, log_max + extra_span)};
  }
  const double top_headroom = 0.22;
  return {0.0, ymax / (1.0 - top_headroom)};
}

std::shared_ptr<TH1D> make_ratio_hist(const TH1D& data_hist, const TH1D& mc_hist, const std::string& name) {
  TH1D* ratio = dynamic_cast<TH1D*>(data_hist.Clone(name.c_str()));
  ratio->SetDirectory(nullptr);
  for (int bin = 1; bin <= ratio->GetNbinsX(); ++bin) {
    const double numerator = data_hist.GetBinContent(bin);
    const double numerator_err = data_hist.GetBinError(bin);
    const double denominator = mc_hist.GetBinContent(bin);
    const double denominator_err = mc_hist.GetBinError(bin);
    if (denominator <= 0.0) {
      ratio->SetBinContent(bin, 0.0);
      ratio->SetBinError(bin, 0.0);
      continue;
    }
    const double value = numerator / denominator;
    double error = 0.0;
    if (numerator > 0.0) {
      error = value * std::sqrt(std::pow(numerator_err / numerator, 2) + std::pow(denominator_err / denominator, 2));
    }
    ratio->SetBinContent(bin, value);
    ratio->SetBinError(bin, error);
  }
  return std::shared_ptr<TH1D>(ratio);
}

double read_event_counter(TFile& root_file, const std::string& file_name) {
  if (auto* hist = dynamic_cast<TH1*>(root_file.Get(kEventCounterPath))) {
    return hist->GetBinContent(2);
  }
  if (auto* param = dynamic_cast<TParameter<double>*>(root_file.Get("metaData/expectedEvents"))) {
    return param->GetVal();
  }
  throw std::runtime_error("Could not find DijetFilter/EventCount/EventCounter or metaData/expectedEvents in " + file_name);
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

std::set<std::string> required_branches_for_plots(const std::vector<PlotRequest>& plots) {
  std::set<std::string> branches = {
      "nJet", "IdTight_j1", "IdTight_j2", "pTWJ_j1", "pTWJ_j2", "etaWJ_j1", "etaWJ_j2", "deltaETAjj", "mjj",
  };
  for (const auto& plot : plots) {
    if (plot.name != "mjj") {
      branches.insert(plot.name);
    }
  }
  return branches;
}

bool passes_base_selection(double pass_json, bool require_json, double n_jet, double id_tight_j1, double id_tight_j2,
                           double pt_j1, double pt_j2, double eta_j1, double eta_j2, double delta_eta_jj,
                           double mjj) {
  return (!require_json || pass_json > 0.5) &&
         n_jet > 1.0 && id_tight_j1 > 0.5 && id_tight_j2 > 0.5 &&
         pt_j1 > 30.0 && pt_j2 > 30.0 && std::abs(eta_j1) < kEtaCut && std::abs(eta_j2) < kEtaCut &&
         std::abs(delta_eta_jj) < kDeltaEtaCut && mjj > kDefaultMinMjj;
}

double get_plot_value(const std::string& plot_name, double mjj, double dijet_mass_ak4pf, double ptwj_j1, double ptwj_j2,
                      double etawj_j1, double etawj_j2, double delta_eta_jj, double phiwj_j1, double phiwj_j2,
                      double delta_phi_jj) {
  if (plot_name == "mjj") {
    return mjj;
  }
  if (plot_name == "Dijet_MassAK4PF") {
    return dijet_mass_ak4pf;
  }
  if (plot_name == "pTWJ_j1") {
    return ptwj_j1;
  }
  if (plot_name == "pTWJ_j2") {
    return ptwj_j2;
  }
  if (plot_name == "etaWJ_j1") {
    return etawj_j1;
  }
  if (plot_name == "etaWJ_j2") {
    return etawj_j2;
  }
  if (plot_name == "deltaETAjj") {
    return delta_eta_jj;
  }
  if (plot_name == "phiWJ_j1") {
    return phiwj_j1;
  }
  if (plot_name == "phiWJ_j2") {
    return phiwj_j2;
  }
  if (plot_name == "deltaPHIjj") {
    return delta_phi_jj;
  }
  throw std::runtime_error("Unsupported plot variable in event loop: " + plot_name);
}

FileProcessResult process_one_file(const std::string& file_name, const std::vector<PlotRequest>& plots, bool read_counter,
                                   bool require_json, bool require_hlt_pfscoutinght, const std::string& hist_name) {
  FileProcessResult result;
  result.counts.reserve(plots.size());
  for (const auto& plot : plots) {
    result.counts.emplace_back(static_cast<std::size_t>(nbins_for_spec(*plot.spec)), 0.0);
  }

  std::unique_ptr<TFile> root_file(TFile::Open(file_name.c_str()));
  if (!root_file || root_file->IsZombie()) {
    result.error = "Could not open ROOT file: " + file_name;
    return result;
  }

  if (read_counter) {
    try {
      result.processed_events = read_event_counter(*root_file, file_name);
    } catch (const std::exception& ex) {
      result.error = ex.what();
      return result;
    }
  }

  auto* tree = dynamic_cast<TTree*>(root_file->Get(kTreePathDefault));
  if (!tree) {
    result.error = "Could not find tree " + std::string(kTreePathDefault) + " in " + file_name;
    return result;
  }

  std::set<std::string> enabled_branches = required_branches_for_plots(plots);
  if (require_json) {
    enabled_branches.insert("PassJSON");
  }
  if (require_hlt_pfscoutinght) {
    enabled_branches.insert("passHLT_PFScoutingHT");
  }
  configure_tree_for_reading(*tree, enabled_branches);

  if (require_hlt_pfscoutinght && !get_branch_names(*tree).count("passHLT_PFScoutingHT")) {
    result.error = "Required branch passHLT_PFScoutingHT is missing in " + file_name;
    return result;
  }

  TTreeReader reader(tree);
  TTreeReaderValue<double> n_jet(reader, "nJet");
  TTreeReaderValue<double> id_tight_j1(reader, "IdTight_j1");
  TTreeReaderValue<double> id_tight_j2(reader, "IdTight_j2");
  TTreeReaderValue<double> pt_j1(reader, "pTWJ_j1");
  TTreeReaderValue<double> pt_j2(reader, "pTWJ_j2");
  TTreeReaderValue<double> eta_j1(reader, "etaWJ_j1");
  TTreeReaderValue<double> eta_j2(reader, "etaWJ_j2");
  TTreeReaderValue<double> delta_eta_jj(reader, "deltaETAjj");
  TTreeReaderValue<double> mjj(reader, "mjj");
  std::unique_ptr<TTreeReaderValue<double>> pass_json;
  if (require_json) {
    pass_json = std::make_unique<TTreeReaderValue<double>>(reader, "PassJSON");
  }
  std::unique_ptr<TTreeReaderValue<double>> pass_hlt_pfscoutinght;
  if (require_hlt_pfscoutinght) {
    pass_hlt_pfscoutinght = std::make_unique<TTreeReaderValue<double>>(reader, "passHLT_PFScoutingHT");
  }

  std::unique_ptr<TTreeReaderValue<double>> dijet_mass_ak4pf;
  std::unique_ptr<TTreeReaderValue<double>> phiwj_j1;
  std::unique_ptr<TTreeReaderValue<double>> phiwj_j2;
  std::unique_ptr<TTreeReaderValue<double>> delta_phi_jj;
  const std::set<std::string> plot_names = [&plots]() {
    std::set<std::string> names;
    for (const auto& plot : plots) {
      names.insert(plot.name);
    }
    return names;
  }();
  if (plot_names.count("Dijet_MassAK4PF")) {
    dijet_mass_ak4pf = std::make_unique<TTreeReaderValue<double>>(reader, "Dijet_MassAK4PF");
  }
  if (plot_names.count("phiWJ_j1")) {
    phiwj_j1 = std::make_unique<TTreeReaderValue<double>>(reader, "phiWJ_j1");
  }
  if (plot_names.count("phiWJ_j2")) {
    phiwj_j2 = std::make_unique<TTreeReaderValue<double>>(reader, "phiWJ_j2");
  }
  if (plot_names.count("deltaPHIjj")) {
    delta_phi_jj = std::make_unique<TTreeReaderValue<double>>(reader, "deltaPHIjj");
  }

  while (reader.Next()) {
    const double pass_json_value = pass_json ? **pass_json : 1.0;
    const double pass_hlt_pfscoutinght_value = pass_hlt_pfscoutinght ? **pass_hlt_pfscoutinght : 1.0;
    if (!passes_base_selection(pass_json_value, require_json, *n_jet, *id_tight_j1, *id_tight_j2, *pt_j1, *pt_j2,
                               *eta_j1, *eta_j2, *delta_eta_jj, *mjj)) {
      continue;
    }
    ++result.selected_events_without_hlt;
    if (require_hlt_pfscoutinght && pass_hlt_pfscoutinght_value <= 0.5) {
      continue;
    }

    const double dijet_mass_ak4pf_value = dijet_mass_ak4pf ? **dijet_mass_ak4pf : 0.0;
    const double phiwj_j1_value = phiwj_j1 ? **phiwj_j1 : 0.0;
    const double phiwj_j2_value = phiwj_j2 ? **phiwj_j2 : 0.0;
    const double delta_phi_jj_value = delta_phi_jj ? **delta_phi_jj : 0.0;
    for (std::size_t i = 0; i < plots.size(); ++i) {
      const double value = get_plot_value(plots[i].name, *mjj, dijet_mass_ak4pf_value, *pt_j1, *pt_j2, *eta_j1,
                                          *eta_j2, *delta_eta_jj, phiwj_j1_value, phiwj_j2_value,
                                          delta_phi_jj_value);
      const int bin_index = find_bin_index(*plots[i].spec, value);
      if (bin_index >= 0) {
        result.counts[i][static_cast<std::size_t>(bin_index)] += 1.0;
      }
    }
    ++result.selected_events;
  }
  return result;
}

struct AggregateResult {
  std::vector<std::shared_ptr<TH1D>> hists;
  double processed_events = 0.0;
  long long selected_events_without_hlt = 0;
  long long selected_events = 0;
};

AggregateResult process_files_parallel(const std::vector<std::string>& files, const std::vector<PlotRequest>& plots,
                                       bool read_counter,
                                       bool require_json, bool require_hlt_pfscoutinght, int threads, const std::string& name_prefix,
                                       const std::string& file_log_label,
                                       const std::optional<double>& file_xsec_pb = std::nullopt) {
  AggregateResult aggregate;
  if (files.empty()) {
    return aggregate;
  }

  std::vector<std::vector<double>> merged_counts;
  merged_counts.reserve(plots.size());
  for (const auto& plot : plots) {
    merged_counts.emplace_back(static_cast<std::size_t>(nbins_for_spec(*plot.spec)), 0.0);
  }

  const unsigned int worker_count = threads > 0 ? static_cast<unsigned int>(threads) : 1U;
  ROOT::TThreadExecutor executor(worker_count);
  std::atomic<std::size_t> processed{0};
  std::mutex io_mutex;
  const std::size_t total = files.size();
  const auto results = executor.Map(
      [&](const std::string& file_name) {
        auto result = process_one_file(file_name, plots, read_counter, require_json, require_hlt_pfscoutinght,
                                       name_prefix + "_" + fs::path(file_name).filename().string());
        const std::size_t done = processed.fetch_add(1) + 1;
        if (!file_log_label.empty()) {
          std::lock_guard<std::mutex> lock(io_mutex);
          std::ostringstream msg;
          msg << '[' << done << '/' << total << "] " << fs::path(file_name).filename().string();
          if (file_xsec_pb.has_value()) {
            msg << " xsec_pb=" << *file_xsec_pb;
          }
          log_info(file_log_label + msg.str());
        }
        return result;
      },
      files);

  for (const auto& result : results) {
    if (!result.error.empty()) {
      throw std::runtime_error(result.error);
    }
    for (std::size_t i = 0; i < merged_counts.size(); ++i) {
      for (std::size_t bin = 0; bin < merged_counts[i].size(); ++bin) {
        merged_counts[i][bin] += result.counts[i][bin];
      }
    }
    aggregate.processed_events += result.processed_events;
    aggregate.selected_events_without_hlt += result.selected_events_without_hlt;
    aggregate.selected_events += result.selected_events;
  }

  aggregate.hists.reserve(plots.size());
  for (std::size_t i = 0; i < plots.size(); ++i) {
    aggregate.hists.push_back(hist_from_counts(name_prefix + "_" + plots[i].name, *plots[i].spec, merged_counts[i]));
  }
  return aggregate;
}

void draw_plot(const TH1D* data_hist, const TH1D* mc_hist, const TH1D* ratio_hist, std::optional<double> kfactor,
               const PlotSpec& spec, double lumi_pb, const fs::path& output_stem, TFile& root_out) {
  const bool use_ratio = data_hist != nullptr && mc_hist != nullptr && ratio_hist != nullptr;
  TCanvas canvas(("c_" + output_stem.stem().string()).c_str(), ("c_" + output_stem.stem().string()).c_str(),
                 kCanvasWidth, kCanvasHeight);

  std::unique_ptr<TPad> top_pad;
  std::unique_ptr<TPad> bottom_pad;
  if (use_ratio) {
    top_pad = std::make_unique<TPad>("top_pad", "top_pad", 0.0, 0.30, 1.0, 1.0);
    bottom_pad = std::make_unique<TPad>("bottom_pad", "bottom_pad", 0.0, 0.0, 1.0, 0.30);
    top_pad->SetLeftMargin(0.16);
    top_pad->SetRightMargin(0.05);
    top_pad->SetTopMargin(0.08);
    top_pad->SetBottomMargin(0.02);
    bottom_pad->SetLeftMargin(0.16);
    bottom_pad->SetRightMargin(0.05);
    bottom_pad->SetTopMargin(0.02);
    bottom_pad->SetBottomMargin(0.35);
    bottom_pad->SetGridy(true);
    top_pad->Draw();
    bottom_pad->Draw();
  } else {
    top_pad = std::make_unique<TPad>("top_pad", "top_pad", 0.0, 0.0, 1.0, 1.0);
    top_pad->SetLeftMargin(0.16);
    top_pad->SetRightMargin(0.05);
    top_pad->SetTopMargin(0.08);
    top_pad->SetBottomMargin(0.13);
    top_pad->Draw();
  }

  top_pad->cd();
  top_pad->SetLogy(spec.logy);

  TH1D* draw_first = const_cast<TH1D*>(mc_hist ? mc_hist : data_hist);
  if (!draw_first) {
    throw std::runtime_error("Nothing to draw: both data and MC histograms are missing.");
  }

  draw_first->GetXaxis()->SetTitle(spec.title.c_str());
  const std::string y_title = infer_y_title(spec, *draw_first);
  draw_first->GetYaxis()->SetTitle(y_title.c_str());
  draw_first->GetYaxis()->SetTitleOffset(1.45);
  draw_first->GetYaxis()->SetLabelSize(0.045);
  draw_first->GetYaxis()->SetTitleSize(0.050);
  draw_first->GetXaxis()->SetLabelSize(use_ratio ? 0.0 : 0.045);
  draw_first->GetXaxis()->SetTitleSize(use_ratio ? 0.0 : 0.050);
  if (spec.display_range) {
    draw_first->GetXaxis()->SetRangeUser(spec.display_range->first, spec.display_range->second);
  }
  const auto [ymin, ymax] = compute_auto_y_range(data_hist, mc_hist, spec.logy);
  draw_first->SetMinimum(ymin);
  draw_first->SetMaximum(ymax);

  if (mc_hist) {
    const_cast<TH1D*>(mc_hist)->Draw("HIST");
    if (data_hist) {
      const_cast<TH1D*>(data_hist)->Draw("E1 SAME");
    }
  } else if (data_hist) {
    const_cast<TH1D*>(data_hist)->Draw("E1");
  }

  TLegend legend(0.60, 0.78, 0.94, 0.90);
  legend.SetTextSize(0.035);
  if (mc_hist) {
    std::ostringstream label;
    if (kfactor) {
      label << kDefaultMcLabel << " * " << std::fixed << std::setprecision(2) << *kfactor;
    } else {
      label << kDefaultMcLabel;
    }
    legend.AddEntry(mc_hist, label.str().c_str(), "l");
  }
  if (data_hist) {
    legend.AddEntry(data_hist, kDefaultDataLabel, "lep");
  }
  legend.Draw();

  draw_cms_labels(*top_pad, lumi_pb);
  top_pad->RedrawAxis();

  if (use_ratio && bottom_pad && ratio_hist) {
    bottom_pad->cd();
    TH1D* ratio = const_cast<TH1D*>(ratio_hist);
    ratio->GetYaxis()->SetTitle("Data / MC");
    ratio->GetYaxis()->SetTitleSize(0.11);
    ratio->GetYaxis()->SetTitleOffset(0.55);
    ratio->GetYaxis()->SetLabelSize(0.10);
    ratio->GetYaxis()->SetNdivisions(505);
    ratio->GetXaxis()->SetTitle(spec.title.c_str());
    ratio->GetXaxis()->SetTitleSize(0.12);
    ratio->GetXaxis()->SetTitleOffset(1.10);
    ratio->GetXaxis()->SetLabelSize(0.10);
    ratio->GetXaxis()->SetLabelOffset(0.02);
    ratio->GetYaxis()->SetRangeUser(spec.ratio_range.first, spec.ratio_range.second);
    if (spec.display_range) {
      ratio->GetXaxis()->SetRangeUser(spec.display_range->first, spec.display_range->second);
    }
    ratio->Draw("E1");
    const TAxis* axis = ratio->GetXaxis();
    const int first_bin = axis->GetFirst();
    const int last_bin = axis->GetLast();
    const double x_min = axis->GetBinLowEdge(first_bin);
    const double x_max = axis->GetBinUpEdge(last_bin);
    TLine unity(x_min, 1.0, x_max, 1.0);
    unity.SetLineStyle(2);
    unity.SetLineColor(kBlack);
    unity.Draw("SAME");
    bottom_pad->RedrawAxis();
  }

  root_out.cd();
  canvas.Write();
  canvas.SaveAs((output_stem.string() + ".pdf").c_str());
}

Options parse_args(int argc, char** argv) {
  Options opt;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    auto need_value = [&](const std::string& name) -> std::string {
      if (i + 1 >= argc) {
        throw std::runtime_error("missing value for " + name);
      }
      return argv[++i];
    };

    if (arg == "--plot") {
      opt.plot = need_value(arg);
    } else if (arg == "--data-list") {
      opt.data_list = need_value(arg);
    } else if (arg == "--mc-list") {
      opt.mc_list = need_value(arg);
    } else if (arg == "--output-dir") {
      opt.output_dir = need_value(arg);
    } else if (arg == "--output-prefix") {
      opt.output_prefix = need_value(arg);
    } else if (arg == "--lumi-pb") {
      opt.lumi_pb = std::stod(need_value(arg));
    } else if (arg == "--threads") {
      opt.threads = std::stoi(need_value(arg));
    } else if (arg == "-h" || arg == "--help") {
      std::cout
          << "usage: plot_kinematics --plot NAME[,NAME2,...]|all [--data-list FILE] [--mc-list FILE] [--output-dir DIR]\n"
          << "                       [--output-prefix PREFIX] [--lumi-pb VALUE] [--threads N]\n";
      std::exit(0);
    } else {
      throw std::runtime_error("unknown option: " + arg);
    }
  }
  if (opt.plot.empty()) {
    throw std::runtime_error("--plot is required");
  }
  if (opt.data_list.empty() && opt.mc_list.empty()) {
    throw std::runtime_error("At least one of --data-list or --mc-list must be provided.");
  }
  return opt;
}

}  // namespace

int main(int argc, char** argv) {
  const auto start_time = std::chrono::steady_clock::now();
  try {
    const Options args = parse_args(argc, argv);
    const std::vector<PlotRequest> plots = parse_plot_requests(args.plot);
    const SelectionPreset& selection = kSelectionPresets.at(kDefaultSelection);
    const std::string data_cut = combine_cuts({selection.data});
    const std::string mc_cut = combine_cuts({selection.mc});

    const fs::path output_dir = resolve_output_dir(args.output_dir, argv[0]);
    const std::string run_label = (plots.size() == 1) ? plots.front().name : derive_name(args.plot == "all" ? "all_plots" : args.plot);
    const std::string root_stem_name = args.output_prefix.empty() ? run_label : args.output_prefix + run_label;
    const fs::path output_root = output_dir / (root_stem_name + ".root");

    const std::vector<std::string> data_files = args.data_list.empty() ? std::vector<std::string>{} : read_data_inputs(args.data_list);
    const std::vector<McSample> mc_samples = args.mc_list.empty() ? std::vector<McSample>{} : read_mc_inputs(args.mc_list);

    configure_root_style();
    ROOT::EnableThreadSafety();

    {
      std::ostringstream plot_msg;
      for (std::size_t i = 0; i < plots.size(); ++i) {
        if (i != 0) {
          plot_msg << ',';
        }
        plot_msg << plots[i].name;
      }
      log_info("Plot(s)          : " + plot_msg.str());
    }
    log_info("Tree             : " + std::string(kTreePathDefault));
    log_info("Output ROOT file : " + output_root.string());
    if (!data_files.empty()) {
      log_info("Data files       : " + std::to_string(data_files.size()));
      log_info("Data cut         : " + data_cut);
    }
    if (!mc_samples.empty()) {
      std::size_t mc_files = 0;
      for (const auto& sample : mc_samples) {
        mc_files += sample.files.size();
      }
      log_info("MC files         : " + std::to_string(mc_files));
      log_info("MC cut           : " + mc_cut);
      log_info("Lumi [pb^-1]     : " + format_lumi(args.lumi_pb));
    }

    TFile root_out(output_root.string().c_str(), "RECREATE");
    if (root_out.IsZombie()) {
      throw std::runtime_error("Could not create output ROOT file: " + output_root.string());
    }

    long long data_yield_raw = 0;
    std::vector<std::shared_ptr<TH1D>> data_hists;
    std::vector<std::shared_ptr<TH1D>> mc_total_hists;
    std::vector<std::shared_ptr<TH1D>> ratio_hists;
    std::vector<double> mc_yield_raws(plots.size(), 0.0);
    std::vector<std::optional<double>> kfactors(plots.size());

    if (!data_files.empty()) {
      auto data_result = process_files_parallel(data_files, plots, false, true, true, args.threads, "h_data",
                                                "Data file        : ");
      data_hists = std::move(data_result.hists);
      data_yield_raw = data_result.selected_events;
      log_info("Data sel. no HLT : " + format_event_count(data_result.selected_events_without_hlt));
      log_info("Data sel. w/ HLT : " + format_event_count(data_result.selected_events));
      for (auto& hist : data_hists) {
        set_hist_style(*hist, kDataColor, true);
        root_out.cd();
        hist->Write();
      }
    }

    mc_total_hists.resize(plots.size());
    for (std::size_t index = 0; index < mc_samples.size(); ++index) {
      const auto& sample = mc_samples[index];
      auto sample_result = process_files_parallel(sample.files, plots, true, false, false, args.threads,
                                                  "h_mc_" + sample.name, "",
                                                  sample.xsec_pb);
      const double n_processed = sample_result.processed_events;
      if (n_processed <= 0.0) {
        throw std::runtime_error("Non-positive processed event count for MC entry: " + sample.source);
      }
      const double n_passed = static_cast<double>(sample_result.selected_events);
      const double eff = n_processed > 0.0 ? n_passed / n_processed : 0.0;
      double sample_weight = 0.0;
      const bool is_last_sample = index + 1 == mc_samples.size();
      if (is_last_sample) {
        if (eff <= 0.0) {
          throw std::runtime_error("Non-positive MC efficiency for last entry: " + sample.source);
        }
        sample_weight = args.lumi_pb * sample.xsec_pb / (n_processed * eff);
      } else {
        sample_weight = args.lumi_pb * sample.xsec_pb / n_processed;
      }
      for (std::size_t i = 0; i < sample_result.hists.size(); ++i) {
        sample_result.hists[i]->Scale(sample_weight);
        mc_yield_raws[i] += sample_result.hists[i]->Integral(0, sample_result.hists[i]->GetNbinsX() + 1);
        if (!mc_total_hists[i]) {
          mc_total_hists[i] = std::shared_ptr<TH1D>(dynamic_cast<TH1D*>(sample_result.hists[i]->Clone(
              ("h_mc_total_" + plots[i].name).c_str())));
          mc_total_hists[i]->SetDirectory(nullptr);
        } else {
          mc_total_hists[i]->Add(sample_result.hists[i].get());
        }
      }
      std::ostringstream msg;
      msg << std::left << std::setw(70) << sample.name
          << " | files=" << std::setw(4) << sample.files.size()
          << " | xsec_pb=" << std::scientific << std::setprecision(3) << std::setw(11) << sample.xsec_pb
          << " | Nproc=" << std::fixed << std::setprecision(0) << std::setw(12) << n_processed
          << " | eff=" << std::fixed << std::setprecision(6) << std::setw(10) << eff
          << " | wt=" << std::fixed << std::setprecision(6) << sample_weight;
      log_info("MC component    : " + msg.str());
    }

    for (std::size_t i = 0; i < mc_total_hists.size(); ++i) {
      if (!mc_total_hists[i]) {
        continue;
      }
      if (!data_hists.empty()) {
        if (mc_yield_raws[i] <= 0.0) {
          throw std::runtime_error("MC yield is zero after selection; cannot scale to data for plot: " + plots[i].name);
        }
        kfactors[i] = static_cast<double>(data_yield_raw) / mc_yield_raws[i];
        mc_total_hists[i]->Scale(*kfactors[i]);
        std::ostringstream msg;
        msg << plots[i].name << " = " << std::fixed << std::setprecision(6) << *kfactors[i];
        log_info("kFactor         : " + msg.str());
      }
      {
        std::ostringstream msg;
        msg << plots[i].name << " = " << mc_yield_raws[i];
        log_info("MC yield raw     : " + msg.str());
      }
      set_hist_style(*mc_total_hists[i], kMcLineColor, false);
      root_out.cd();
      mc_total_hists[i]->Write();
    }

    if (!data_hists.empty()) {
      ratio_hists.resize(plots.size());
      for (std::size_t i = 0; i < plots.size(); ++i) {
        if (!mc_total_hists[i]) {
          continue;
        }
        ratio_hists[i] = make_ratio_hist(*data_hists[i], *mc_total_hists[i], "h_ratio_" + plots[i].name);
        set_hist_style(*ratio_hists[i], kDataColor, true);
        root_out.cd();
        ratio_hists[i]->Write();
      }
    }

    for (std::size_t i = 0; i < plots.size(); ++i) {
      const fs::path output_stem = output_dir / (args.output_prefix.empty() ? plots[i].name : args.output_prefix + plots[i].name);
      draw_plot(data_hists.empty() ? nullptr : data_hists[i].get(), mc_total_hists[i].get(),
                ratio_hists.empty() ? nullptr : ratio_hists[i].get(), kfactors[i], *plots[i].spec, args.lumi_pb,
                output_stem, root_out);
    }
    root_out.Close();
    log_info("Elapsed time     : " + format_elapsed_seconds(std::chrono::steady_clock::now() - start_time));
    log_info("Finished.");
    return 0;
  } catch (const std::exception& ex) {
    std::cerr << "[ERROR] " << ex.what() << std::endl;
    std::cout << kInfoTag << " Elapsed time     : "
              << format_elapsed_seconds(std::chrono::steady_clock::now() - start_time) << std::endl;
    return 1;
  }
}

