#include <TCanvas.h>
#include <TEfficiency.h>
#include <TError.h>
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
  std::string output_dir;
  std::string output_prefix = "pfscouting_saved_hist_trigger";
  double lumi_pb = 5490.0;
  std::string year = "2024";
};

struct SummaryRow {
  int bin_low = 0;
  int bin_high = 0;
  std::string bin_range;
  double efficiency = std::numeric_limits<double>::quiet_NaN();
  double eff_err_low = std::numeric_limits<double>::quiet_NaN();
  double eff_err_up = std::numeric_limits<double>::quiet_NaN();
  double denominator_count = 0.0;
  double numerator_count = 0.0;
};

struct ObservableConfig {
  std::string folder_tag;
  std::string source_tag;
  std::string jet_label;
  std::string axis_title;
  std::string selection_label;
  double full_min_gev = 0.0;
  double full_max_gev = 0.0;
  double zoom_min_gev = 0.0;
  double zoom_max_gev = 0.0;
};

struct CaseConfig {
  std::string folder_tag;
  std::string source_tag;
};

struct MergedHistSet {
  std::unique_ptr<TH1D> all;
  std::unique_ptr<TH1D> numerator;
  std::unique_ptr<TH1D> denominator;
  size_t files_with_any_hist = 0;
};

struct OutputPaths {
  fs::path directory;
  fs::path root_file;
  fs::path summary_csv;
  fs::path summary_tsv;
  fs::path eff_pdf;
  fs::path eff_zoom_pdf;
  fs::path counts_pdf;
  std::string stem;
};

const std::array<ObservableConfig, 3> kObservables = {{
    {"wideJet", "wideDijet", "Wide PF-jets", "Wide-jet dijet mass [TeV]", "|#eta| < 2.5, |#Delta#eta| < 1.3", 220.0, 6000.0, 100.0, 650.0},
    {"AK4PFJet", "AK4Jets", "AK4 PF-jets", "AK4 PF dijet mass [TeV]", "|#eta| < 2.5, |#Delta#eta| < 1.3", 220.0, 6000.0, 100.0, 650.0},
    {"HTAK4PF", "HTAK4PF", "AK4 PF H_{T}", "AK4 PF H_{T} [TeV]", "p_{T} > 30 GeV, |#eta| < 2.5", 0.0, 1500.0, 0.0, 800.0},
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

void print_usage(const char* argv0) {
  std::cout
      << "Usage: " << argv0 << " --input-list <file> [options]\n\n"
      << "Options:\n"
      << "  --input-list <file>      Text file listing ROOT files to read.\n"
      << "  --output-dir <dir>       Output directory (default: trigger_efficiency_saved_hists_<year>).\n"
      << "  --output-prefix <text>   Prefix for output file names.\n"
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
    } else if (arg == "--output-dir") {
      opts.output_dir = require_value(arg);
    } else if (arg == "--output-prefix") {
      opts.output_prefix = require_value(arg);
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

  if (opts.input_list.empty()) {
    die("--input-list is required");
  }
  if (opts.output_dir.empty()) {
    opts.output_dir = "trigger_efficiency_saved_hists_" +
                      std::regex_replace(opts.year, std::regex(R"([^A-Za-z0-9_.-]+)"), "_");
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

std::string trigger_hist_name(const ObservableConfig& observable, const CaseConfig& study_case, const char* trig_name) {
  return "h_" + observable.source_tag + "_HLTpass_" + study_case.source_tag + "_" + trig_name;
}

std::string legacy_wide_hist_name(const char* trig_name) {
  return std::string("h_mjj_HLTpass_") + trig_name;
}

MergedHistSet merge_case_histograms(const std::vector<std::string>& files,
                                    const ObservableConfig& observable,
                                    const CaseConfig& study_case) {
  MergedHistSet merged;
  for (const auto& file_name : files) {
    std::unique_ptr<TFile> file(TFile::Open(file_name.c_str(), "READ"));
    if (!file || file->IsZombie()) {
      std::cerr << warn_tag() << " Could not open " << file_name << std::endl;
      continue;
    }

    const bool allow_legacy_wide_fallback =
        (observable.folder_tag == "wideJet" && study_case.folder_tag == "nominal");
    const std::string no_trig_name = trigger_hist_name(observable, study_case, "noTrig");
    const std::string jetht_name = trigger_hist_name(observable, study_case, "PFScouting_JetHT");
    const std::string singlemu_name = trigger_hist_name(observable, study_case, "PFScouting_SingleMuon");

    bool found_any = false;
    found_any |= merge_histogram(*file,
                                 no_trig_name,
                                 allow_legacy_wide_fallback ? legacy_wide_hist_name("noTrig") : "",
                                 observable.folder_tag + "_" + study_case.folder_tag + "_all",
                                 merged.all);
    found_any |= merge_histogram(*file,
                                 jetht_name,
                                 allow_legacy_wide_fallback ? legacy_wide_hist_name("PFScouting_JetHT") : "",
                                 observable.folder_tag + "_" + study_case.folder_tag + "_numerator",
                                 merged.numerator);
    found_any |= merge_histogram(*file,
                                 singlemu_name,
                                 allow_legacy_wide_fallback ? legacy_wide_hist_name("PFScouting_SingleMuon") : "",
                                 observable.folder_tag + "_" + study_case.folder_tag + "_denominator",
                                 merged.denominator);
    if (found_any) {
      ++merged.files_with_any_hist;
    }
  }
  return merged;
}

std::vector<SummaryRow> build_summary_rows(TEfficiency& efficiency, const TH1D& denom_hist, const TH1D& numer_hist) {
  std::vector<SummaryRow> rows;
  rows.reserve(denom_hist.GetNbinsX());
  const TAxis* xaxis = denom_hist.GetXaxis();
  for (int bin_idx = 1; bin_idx <= denom_hist.GetNbinsX(); ++bin_idx) {
    SummaryRow row;
    row.bin_low = static_cast<int>(std::lround(xaxis->GetBinLowEdge(bin_idx)));
    row.bin_high = static_cast<int>(std::lround(xaxis->GetBinUpEdge(bin_idx)));
    row.bin_range = std::to_string(row.bin_low) + "-" + std::to_string(row.bin_high);
    row.denominator_count = denom_hist.GetBinContent(bin_idx);
    row.numerator_count = numer_hist.GetBinContent(bin_idx);
    if (row.denominator_count > 0.0) {
      row.efficiency = efficiency.GetEfficiency(bin_idx);
      row.eff_err_low = efficiency.GetEfficiencyErrorLow(bin_idx);
      row.eff_err_up = efficiency.GetEfficiencyErrorUp(bin_idx);
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
  out << "bin_range,bin_low,bin_high,efficiency,eff_err_low,eff_err_up,denominator_count,numerator_count\n";
  for (const auto& row : rows) {
    out << row.bin_range << ','
        << row.bin_low << ','
        << row.bin_high << ','
        << format_table_value(row.efficiency) << ','
        << format_table_value(row.eff_err_low) << ','
        << format_table_value(row.eff_err_up) << ','
        << static_cast<long long>(std::llround(row.denominator_count)) << ','
        << static_cast<long long>(std::llround(row.numerator_count)) << '\n';
  }
}

void write_summary_tsv(const fs::path& path, const std::vector<SummaryRow>& rows) {
  std::ofstream out(path);
  if (!out) {
    die("Could not write summary TSV: " + path.string());
  }
  out << "Bin Range\tEff.\tEff. Err Low\tEff. Err Up\tDenominator\tNumerator\n";
  for (const auto& row : rows) {
    out << row.bin_range << '\t'
        << format_table_value(row.efficiency) << '\t'
        << format_table_value(row.eff_err_low) << '\t'
        << format_table_value(row.eff_err_up) << '\t'
        << static_cast<long long>(std::llround(row.denominator_count)) << '\t'
        << static_cast<long long>(std::llround(row.numerator_count)) << '\n';
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
                          const ObservableConfig& observable,
                          const std::string& plot_tag,
                          const std::string& case_label,
                          double x_min_gev,
                          double x_max_gev,
                          int marker_style = 20,
                          int marker_color = kBlack,
                          bool draw_turnon_lines = false) {
  TCanvas canvas(("c_trigger_efficiency_" + observable.folder_tag + "_" + plot_tag).c_str(), "", 800, 800);
  canvas.cd();
  gPad->SetTopMargin(0.08);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.16);
  gPad->SetBottomMargin(0.13);

  auto graph_tev = graph_to_tev(graph, "g_pfscouting_jetht_efficiency_tev_" + observable.folder_tag + "_" + plot_tag);
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

  TPaveText cut_box(0.56, 0.18, 0.94, 0.31, "NDC");
  cut_box.SetBorderSize(0);
  cut_box.SetFillStyle(0);
  cut_box.SetTextAlign(23);
  cut_box.SetTextSize(0.032);
  cut_box.SetTextFont(42);
  cut_box.AddText(observable.jet_label.c_str());
  cut_box.AddText(observable.selection_label.c_str());
  cut_box.AddText(case_label.c_str());
  cut_box.Draw();

  draw_labels_right(format_lumi_label(lumi_pb));
  gPad->RedrawAxis();

  root_out.cd();
  graph_tev->Write();
  canvas.Write();
  canvas.SaveAs(output_pdf.string().c_str());
}

void draw_count_overlay_plot(const TH1D& denom_hist,
                             const TH1D& numer_hist,
                             const fs::path& output_pdf,
                             TFile& root_out,
                             const std::string& year,
                             const ObservableConfig& observable,
                             const std::string& case_label) {
  TCanvas canvas(("c_trigger_event_counts_" + observable.folder_tag + "_" + case_label).c_str(), "", 800, 800);
  canvas.cd();
  gPad->SetTopMargin(0.08);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.16);
  gPad->SetBottomMargin(0.13);
  gPad->SetLogy();

  auto h_denom_plot = hist_to_tev(denom_hist, "h_denom_counts_plot_" + observable.folder_tag + "_" + case_label);
  auto h_numer_plot = hist_to_tev(numer_hist, "h_numer_counts_plot_" + observable.folder_tag + "_" + case_label);

  h_denom_plot->SetTitle("");
  h_denom_plot->GetXaxis()->SetTitle(observable.axis_title.c_str());
  h_denom_plot->GetYaxis()->SetTitle("Events / bin");
  h_denom_plot->GetXaxis()->SetRangeUser(observable.full_min_gev / 1000.0, observable.full_max_gev / 1000.0);
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

  TPaveText case_box(0.56, 0.18, 0.94, 0.24, "NDC");
  case_box.SetBorderSize(0);
  case_box.SetFillStyle(0);
  case_box.SetTextAlign(23);
  case_box.SetTextSize(0.032);
  case_box.SetTextFont(42);
  case_box.AddText(case_label.c_str());
  case_box.Draw();

  draw_labels_right(year + " (" + kDefaultSqrtS + ")");
  gPad->RedrawAxis();

  root_out.cd();
  canvas.Write();
  canvas.SaveAs(output_pdf.string().c_str());
}

OutputPaths make_output_paths(const fs::path& base_output_dir,
                              const std::string& output_prefix,
                              const ObservableConfig& observable,
                              const CaseConfig& study_case) {
  OutputPaths paths;
  paths.directory = base_output_dir / observable.folder_tag / study_case.folder_tag;
  fs::create_directories(paths.directory);
  paths.stem = output_prefix + "_" + observable.folder_tag + "_" + study_case.folder_tag;
  paths.root_file = paths.directory / (paths.stem + ".root");
  paths.summary_csv = paths.directory / (paths.stem + "_summary.csv");
  paths.summary_tsv = paths.directory / (paths.stem + "_summary.tsv");
  paths.eff_pdf = paths.directory / (paths.stem + "_efficiency.pdf");
  paths.eff_zoom_pdf = paths.directory / (paths.stem + "_efficiency_zoom.pdf");
  paths.counts_pdf = paths.directory / (paths.stem + "_counts.pdf");
  return paths;
}

void print_case_output_info(const OutputPaths& paths, const ObservableConfig& observable, const CaseConfig& study_case) {
  const auto info = info_tag();
  std::cout << info << " Observable       : " << observable.folder_tag << std::endl;
  std::cout << info << " Case             : " << study_case.folder_tag << std::endl;
  std::cout << info << " Output ROOT file : " << paths.root_file << std::endl;
  std::cout << info << " Summary CSV      : " << paths.summary_csv << std::endl;
  std::cout << info << " Summary TSV      : " << paths.summary_tsv << std::endl;
  std::cout << info << " Efficiency PDF   : " << paths.eff_pdf << std::endl;
  std::cout << info << " Efficiency Zoom  : " << paths.eff_zoom_pdf << std::endl;
  std::cout << info << " Counts PDF       : " << paths.counts_pdf << std::endl;
}

}  // namespace

int main(int argc, char** argv) {
  try {
    TH1::AddDirectory(kFALSE);
    gErrorIgnoreLevel = kWarning;
    configure_root_style();

    const Options opts = parse_args(argc, argv);
    const std::vector<std::string> input_files = read_data_inputs(opts.input_list);
    const fs::path base_output_dir = fs::absolute(opts.output_dir);
    fs::create_directories(base_output_dir);

    std::cout << info_tag() << " Reading " << input_files.size() << " ROOT file(s)" << std::endl;

    for (const auto& observable : kObservables) {
      for (const auto& study_case : kCases) {
        const auto merged = merge_case_histograms(input_files, observable, study_case);

        if (!merged.denominator || !merged.numerator) {
          std::cout << warn_tag() << ' ' << observable.folder_tag << '/' << study_case.folder_tag
                    << ": required histograms are missing, skipping." << std::endl;
          continue;
        }

        if (!has_hist_contents(*merged.denominator)) {
          std::cout << warn_tag() << ' ' << observable.folder_tag << '/' << study_case.folder_tag
                    << ": denominator histogram is empty, skipping." << std::endl;
          continue;
        }

        if (!TEfficiency::CheckConsistency(*merged.numerator, *merged.denominator)) {
          std::cout << warn_tag() << ' ' << observable.folder_tag << '/' << study_case.folder_tag
                    << ": numerator/denominator histograms are inconsistent, skipping." << std::endl;
          continue;
        }

        const OutputPaths paths = make_output_paths(base_output_dir, opts.output_prefix, observable, study_case);
        print_case_output_info(paths, observable, study_case);
        std::cout << info_tag() << " Files contributing histograms: " << merged.files_with_any_hist << std::endl;

        TFile root_out(paths.root_file.string().c_str(), "RECREATE");
        if (root_out.IsZombie()) {
          die("Could not create output ROOT file: " + paths.root_file.string());
        }

        merged.denominator->SetName(("h_denominator_" + observable.folder_tag + "_" + study_case.folder_tag).c_str());
        merged.numerator->SetName(("h_numerator_" + observable.folder_tag + "_" + study_case.folder_tag).c_str());
        merged.denominator->Write();
        merged.numerator->Write();
        if (merged.all) {
          merged.all->SetName(("h_all_" + observable.folder_tag + "_" + study_case.folder_tag).c_str());
          merged.all->Write();
        }

        TEfficiency efficiency(*merged.numerator, *merged.denominator);
        efficiency.SetName(("teff_" + observable.folder_tag + "_" + study_case.folder_tag).c_str());
        efficiency.SetStatisticOption(TEfficiency::kFCP);
        auto graph = std::unique_ptr<TGraphAsymmErrors>(efficiency.CreateGraph());
        graph->SetName(("g_efficiency_" + observable.folder_tag + "_" + study_case.folder_tag).c_str());

        const auto summary_rows = build_summary_rows(efficiency, *merged.denominator, *merged.numerator);
        write_summary_csv(paths.summary_csv, summary_rows);
        write_summary_tsv(paths.summary_tsv, summary_rows);

        efficiency.Write();
        graph->Write();

        draw_efficiency_plot(*graph,
                             paths.eff_pdf,
                             root_out,
                             opts.lumi_pb,
                             opts.year,
                             observable,
                             study_case.folder_tag + "_full",
                             study_case.folder_tag,
                             observable.full_min_gev,
                             observable.full_max_gev,
                             20,
                             kBlue + 1,
                             true);
        draw_efficiency_plot(*graph,
                             paths.eff_zoom_pdf,
                             root_out,
                             opts.lumi_pb,
                             opts.year,
                             observable,
                             study_case.folder_tag + "_zoom",
                             study_case.folder_tag,
                             observable.zoom_min_gev,
                             observable.zoom_max_gev,
                             20,
                             kBlue + 1,
                             true);
        draw_count_overlay_plot(*merged.denominator,
                                *merged.numerator,
                                paths.counts_pdf,
                                root_out,
                                opts.year,
                                observable,
                                study_case.folder_tag);
      }
    }

    std::cout << info_tag() << " Done." << std::endl;
    return 0;
  } catch (const std::exception& ex) {
    std::cerr << warn_tag() << ' ' << ex.what() << std::endl;
    return 1;
  }
}

