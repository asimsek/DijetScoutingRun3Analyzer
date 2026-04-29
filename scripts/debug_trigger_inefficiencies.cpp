#include <ROOT/TThreadExecutor.hxx>

#include <TBranch.h>
#include <TFile.h>
#include <TObjArray.h>
#include <TROOT.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <glob.h>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <unistd.h>
#include <wordexp.h>

namespace fs = std::filesystem;

namespace {

constexpr const char* kDefaultTreePath = "rootTupleTree/tree";
constexpr const char* kFallbackTreePath = "tree";
constexpr const char* kAnsiReset = "\033[0m";
constexpr const char* kAnsiBlue = "\033[94m";
constexpr const char* kAnsiCyan = "\033[96m";
constexpr const char* kAnsiRed = "\033[91m";

constexpr double kEtaCut = 2.5;
constexpr double kDeltaEtaCut = 1.3;
constexpr double kMinPtJ1 = 30.0;
constexpr double kMinPtJ2 = 30.0;
constexpr double kMinJetCount = 1.0;
constexpr bool kUseGoodMuonBaseSelectionFlagIfAvailable = true;
constexpr const char* kGoodMuonBaseSelectionFlagBranch = "passGoodMuonBaseSelection";

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

enum class Observable {
  kAK4PFJet,
  kWideJet,
  kHTAK4PF,
};

struct Options {
  std::string input_list;
  std::vector<std::string> input_roots;
  std::string output_dir;
  std::string tree_path;
  Observable observable = Observable::kAK4PFJet;
  int threads = 8;
  double mass_min_gev = 606.0;
  int run_min = std::numeric_limits<int>::min();
  int run_max = std::numeric_limits<int>::max();
  int cache_mb = 128;
  int progress_every_sec = 5;
  bool require_l1_for_jetht = false;
};

struct ProgressState {
  std::atomic<std::size_t> files_done{0};
  std::size_t total_files = 0;
  bool tty = false;
  std::chrono::steady_clock::time_point start_time;
  std::mutex print_mutex;
};

struct FileTask {
  std::size_t index = 0;
  std::string file_name;
  fs::path csv_part_path;
};

struct FileResult {
  std::size_t index = 0;
  std::string file_name;
  fs::path csv_part_path;
  bool has_tree = false;
  bool processed = false;
  std::string warning;
  std::uint64_t problem_events = 0;
  std::unordered_map<int, std::uint64_t> run_counts;
  std::unordered_map<int, std::set<int>> run_lumis;
  std::unordered_map<int, std::set<int>> json_lumis;
  std::unordered_map<int, std::set<int>> bad_lumis;
};

struct CsvField {
  std::string header;
  std::string branch_name;
  bool optional = false;
};

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

std::string colored(const std::string& tag, const char* color) {
  return stdout_is_tty() ? std::string(color) + tag + kAnsiReset : tag;
}

std::string info_tag() {
  return colored("[INFO]", kAnsiCyan);
}

std::string progress_tag() {
  return colored("[PROGRESS]", kAnsiBlue);
}

std::string warn_tag() {
  return colored("[WARN]", kAnsiRed);
}

std::string observable_name(Observable observable) {
  switch (observable) {
    case Observable::kAK4PFJet:
      return "AK4PFJet";
    case Observable::kWideJet:
      return "wideJet";
    case Observable::kHTAK4PF:
      return "HTAK4PF";
  }
  return "AK4PFJet";
}

Observable parse_observable(const std::string& text) {
  if (text == "AK4PFJet" || text == "ak4" || text == "AK4") {
    return Observable::kAK4PFJet;
  }
  if (text == "wideJet" || text == "wide" || text == "mjj") {
    return Observable::kWideJet;
  }
  if (text == "HTAK4PF" || text == "ht" || text == "HT") {
    return Observable::kHTAK4PF;
  }
  die("Unknown observable '" + text + "'. Use AK4PFJet, wideJet, or HTAK4PF.");
}

std::string mass_branch(Observable observable) {
  switch (observable) {
    case Observable::kAK4PFJet:
      return "Dijet_MassAK4PF";
    case Observable::kWideJet:
      return "mjj";
    case Observable::kHTAK4PF:
      return "HTAK4PF";
  }
  return "Dijet_MassAK4PF";
}

void print_usage(const char* argv0) {
  std::cout
      << "Usage: " << argv0 << " --input-list <file> [options]\n"
      << "       " << argv0 << " --input-root '<glob/or/root/file>' [options]\n\n"
      << "Tree-based PFScouting trigger inefficiency export.\n\n"
      << "Input options:\n"
      << "  --input-list <file>        Text file with ROOT files, directories, or globs.\n"
      << "  --input-root <path/glob>   ROOT file, directory, or glob. Can be given more than once.\n"
      << "  --tree-path <path>         Tree path override (default: rootTupleTree/tree, fallback: tree).\n\n"
      << "Selection options:\n"
      << "  --observable <name>        AK4PFJet, wideJet, or HTAK4PF (default: AK4PFJet).\n"
      << "  --mass-min-gev <value>     Only keep inefficiencies with observable mass >= this value (default: 606).\n"
      << "  --run-min <run>            Only process runs >= this run number.\n"
      << "  --run-max <run>            Only process runs <= this run number.\n"
      << "  --require-l1-for-jetht     Numerator requires passL1ForPFScoutingHT in addition to JetHT.\n\n"
      << "Performance/output options:\n"
      << "  --output-dir <dir>         Output directory (default: trigger_inefficiencies_<observable>).\n"
      << "  --threads <n>              Files processed in parallel; 0 or 1 disables parallelism (default: 8).\n"
      << "  --cache-mb <n>             TTree cache per open file in MB (default: 128).\n"
      << "  --progress-every-sec <n>   Kept for compatibility; 0 disables progress, otherwise updates once per finished file.\n"
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
    } else if (arg == "--input-root") {
      opts.input_roots.push_back(require_value(arg));
    } else if (arg == "--output-dir") {
      opts.output_dir = require_value(arg);
    } else if (arg == "--tree-path") {
      opts.tree_path = require_value(arg);
    } else if (arg == "--observable") {
      opts.observable = parse_observable(require_value(arg));
    } else if (arg == "--threads") {
      opts.threads = std::stoi(require_value(arg));
    } else if (arg == "--cache-mb") {
      opts.cache_mb = std::stoi(require_value(arg));
    } else if (arg == "--progress-every-sec") {
      opts.progress_every_sec = std::stoi(require_value(arg));
    } else if (arg == "--mass-min-gev") {
      opts.mass_min_gev = std::stod(require_value(arg));
    } else if (arg == "--run-min") {
      opts.run_min = std::stoi(require_value(arg));
    } else if (arg == "--run-max") {
      opts.run_max = std::stoi(require_value(arg));
    } else if (arg == "--require-l1-for-jetht") {
      opts.require_l1_for_jetht = true;
    } else if (arg == "--help" || arg == "-h") {
      print_usage(argv[0]);
      std::exit(0);
    } else {
      die("Unknown argument: " + arg);
    }
  }

  if (opts.input_list.empty() && opts.input_roots.empty()) {
    die("Provide --input-list and/or --input-root.");
  }
  if (opts.mass_min_gev < 0.0) {
    die("--mass-min-gev must be non-negative.");
  }
  if (opts.run_min > opts.run_max) {
    die("--run-min cannot be larger than --run-max.");
  }
  if (opts.cache_mb <= 0) {
    die("--cache-mb must be positive.");
  }
  if (opts.progress_every_sec < 0) {
    die("--progress-every-sec cannot be negative.");
  }
  if (opts.output_dir.empty()) {
    opts.output_dir = "trigger_inefficiencies_" + observable_name(opts.observable);
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

std::string normalize_path_token(const std::string& token, const fs::path& base_dir) {
  if (token.find("://") != std::string::npos) {
    return token;
  }
  fs::path path(expand_user_and_env(token));
  if (path.is_relative()) {
    path = base_dir / path;
  }
  return path.lexically_normal().string();
}

bool looks_like_root_file(const fs::path& path) {
  return path.has_extension() && path.extension() == ".root";
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
      if (entry.is_regular_file() && looks_like_root_file(entry.path())) {
        out.push_back(entry.path().string());
      }
    }
    std::sort(out.begin(), out.end());
    return out;
  }
  out.push_back(path.string());
  return out;
}

std::vector<std::string> deduplicate(const std::vector<std::string>& items) {
  std::unordered_set<std::string> seen;
  std::vector<std::string> unique;
  unique.reserve(items.size());
  for (const auto& item : items) {
    if (seen.insert(item).second) {
      unique.push_back(item);
    }
  }
  return unique;
}

std::vector<std::string> read_list_inputs(const std::string& list_path) {
  const fs::path path = fs::absolute(list_path);
  std::ifstream in(path);
  if (!in) {
    die("Could not open input list: " + path.string());
  }

  std::vector<std::string> files;
  std::string line;
  while (std::getline(in, line)) {
    const auto comment_pos = line.find('#');
    if (comment_pos != std::string::npos) {
      line = line.substr(0, comment_pos);
    }
    const auto first = line.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
      continue;
    }
    const auto last = line.find_last_not_of(" \t\r\n");
    line = line.substr(first, last - first + 1);
    const auto expanded = expand_source_token(line, path.parent_path());
    files.insert(files.end(), expanded.begin(), expanded.end());
  }
  return files;
}

std::vector<std::string> resolve_inputs(const Options& opts) {
  std::vector<std::string> files;
  if (!opts.input_list.empty()) {
    const auto from_list = read_list_inputs(opts.input_list);
    files.insert(files.end(), from_list.begin(), from_list.end());
  }
  const fs::path base_dir = fs::current_path();
  for (const auto& token : opts.input_roots) {
    const auto expanded = expand_source_token(token, base_dir);
    files.insert(files.end(), expanded.begin(), expanded.end());
  }
  files = deduplicate(files);
  if (files.empty()) {
    die("No ROOT input files were found.");
  }
  return files;
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

std::string find_branch(const std::set<std::string>& branch_names,
                        const std::array<std::string, 2>& candidates,
                        const std::string& label,
                        bool required) {
  for (const auto& name : candidates) {
    if (branch_names.count(name)) {
      return name;
    }
  }
  if (required) {
    std::ostringstream os;
    os << "Could not find " << label << " branch. Tried ";
    for (size_t i = 0; i < candidates.size(); ++i) {
      if (i != 0) {
        os << ", ";
      }
      os << candidates[i];
    }
    die(os.str());
  }
  return "";
}

std::string find_l1_branch(const std::set<std::string>& branch_names) {
  for (const auto& name : kL1BranchCandidates) {
    if (branch_names.count(name)) {
      return name;
    }
  }
  return "";
}

void require_branch(const std::set<std::string>& branch_names, const std::string& branch, const std::string& file_name) {
  if (!branch_names.count(branch)) {
    die("Required branch '" + branch + "' is missing in " + file_name);
  }
}

TTree* find_tree(TFile& file, const Options& opts, std::string& used_path) {
  if (!opts.tree_path.empty()) {
    used_path = opts.tree_path;
    return dynamic_cast<TTree*>(file.Get(opts.tree_path.c_str()));
  }
  used_path = kDefaultTreePath;
  auto* tree = dynamic_cast<TTree*>(file.Get(kDefaultTreePath));
  if (tree) {
    return tree;
  }
  used_path = kFallbackTreePath;
  return dynamic_cast<TTree*>(file.Get(kFallbackTreePath));
}

void configure_tree_for_reading(TTree& tree, const std::set<std::string>& enabled_branches, int cache_mb) {
  const auto branch_names = get_branch_names(tree);
  tree.SetBranchStatus("*", 0);
  for (const auto& branch : enabled_branches) {
    if (branch_names.count(branch)) {
      tree.SetBranchStatus(branch.c_str(), 1);
    }
  }
  tree.SetCacheSize(static_cast<Long64_t>(cache_mb) * 1024LL * 1024LL);
  tree.SetCacheLearnEntries(20);
  for (const auto& branch : enabled_branches) {
    if (branch_names.count(branch)) {
      tree.AddBranchToCache(branch.c_str(), kTRUE);
    }
  }
}

bool has_run_filter(const Options& opts) {
  return opts.run_min != std::numeric_limits<int>::min() || opts.run_max != std::numeric_limits<int>::max();
}

bool run_is_allowed(int run, const Options& opts) {
  return run >= opts.run_min && run <= opts.run_max;
}

bool passes_widejet_selection(double pass_json, double n_jet, double id_tight_j1, double id_tight_j2, double pt_j1,
                              double pt_j2, double eta_j1, double eta_j2, double delta_eta_jj) {
  return pass_json > 0.5 && n_jet > kMinJetCount && id_tight_j1 > 0.5 && id_tight_j2 > 0.5 &&
         pt_j1 > kMinPtJ1 && pt_j2 > kMinPtJ2 && std::abs(eta_j1) < kEtaCut &&
         std::abs(eta_j2) < kEtaCut && std::abs(delta_eta_jj) < kDeltaEtaCut;
}

bool passes_ak4_selection(double pass_json, double n_ak4, double id_tight_j1, double id_tight_j2, double pt_j1,
                          double pt_j2, double eta_j1, double eta_j2, double delta_eta_jj) {
  return pass_json > 0.5 && n_ak4 > kMinJetCount && id_tight_j1 > 0.5 && id_tight_j2 > 0.5 &&
         pt_j1 > kMinPtJ1 && pt_j2 > kMinPtJ2 && std::abs(eta_j1) < kEtaCut &&
         std::abs(eta_j2) < kEtaCut && std::abs(delta_eta_jj) < kDeltaEtaCut;
}

std::string format_duration(double seconds) {
  if (!std::isfinite(seconds) || seconds < 0.0) {
    return "n/a";
  }
  const auto total = static_cast<long long>(seconds + 0.5);
  const long long hours = total / 3600;
  const long long minutes = (total % 3600) / 60;
  const long long secs = total % 60;
  std::ostringstream os;
  if (hours > 0) {
    os << hours << "h ";
  }
  if (hours > 0 || minutes > 0) {
    os << minutes << "m ";
  }
  os << secs << "s";
  return os.str();
}

void print_progress(ProgressState* progress, const std::string& last_file = "") {
  if (!progress) {
    return;
  }

  std::lock_guard<std::mutex> lock(progress->print_mutex);
  const auto files_done = progress->files_done.load();
  const double elapsed =
      std::max(1.0e-9, std::chrono::duration<double>(std::chrono::steady_clock::now() - progress->start_time).count());
  const double fraction = progress->total_files > 0
                              ? 100.0 * static_cast<double>(files_done) / static_cast<double>(progress->total_files)
                              : 0.0;

  std::ostringstream line;
  line << progress_tag()
       << " files " << files_done << "/" << progress->total_files
       << " (" << std::fixed << std::setprecision(1) << fraction << "%)"
       << " | elapsed " << format_duration(elapsed);
  if (!last_file.empty()) {
    line << " | last " << fs::path(last_file).filename().string();
  }

  if (progress->tty) {
    std::cout << '\r' << line.str() << "        " << std::flush;
    if (files_done >= progress->total_files) {
      std::cout << std::endl;
    }
  } else {
    std::cout << line.str() << std::endl;
  }
}

std::string format_csv_value(double value) {
  if (!std::isfinite(value)) {
    return "";
  }
  if (std::fabs(value - std::round(value)) < 1e-9) {
    return std::to_string(static_cast<long long>(std::llround(value)));
  }
  std::ostringstream os;
  os << std::fixed << std::setprecision(9) << value;
  std::string text = os.str();
  while (!text.empty() && text.back() == '0') {
    text.pop_back();
  }
  if (!text.empty() && text.back() == '.') {
    text.pop_back();
  }
  return text;
}

std::vector<CsvField> build_csv_fields(const std::string& jetht_branch,
                                       const std::string& singlemuon_branch,
                                       const std::string& l1_branch,
                                       const std::string& goodmuon_flag_branch) {
  return {
      {"Run Number", "run", false},
      {"Event Number", "event", false},
      {"LS", "lumi", false},
      {"mjj", "mjj", false},
      {"deltaEta", "deltaETAjj", false},
      {"deltaPhi", "deltaPHIjj", false},
      {"metSig", "metSig", false},
      {"PassJSON", "PassJSON", false},
      {"passGoodMuonBaseSelection", goodmuon_flag_branch, goodmuon_flag_branch.empty()},
      {"etaWJ_j1", "etaWJ_j1", false},
      {"etaWJ_j2", "etaWJ_j2", false},
      {"phiWJ_j1", "phiWJ_j1", false},
      {"phiWJ_j2", "phiWJ_j2", false},
      {"pTWJ_j1", "pTWJ_j1", false},
      {"pTWJ_j2", "pTWJ_j2", false},
      {"rapidityWJ_j1", "rapidityWJ_j1", false},
      {"rapidityWJ_j2", "rapidityWJ_j2", false},
      {"massWJ_j1", "massWJ_j1", false},
      {"massWJ_j2", "massWJ_j2", false},
      {"Dijet_MassAK4PF", "Dijet_MassAK4PF", false},
      {"etaAK4PF_j1", "etaAK4PF_j1", false},
      {"etaAK4PF_j2", "etaAK4PF_j2", false},
      {"pTAK4PF_j1", "pTAK4PF_j1", false},
      {"pTAK4PF_j2", "pTAK4PF_j2", false},
      {"phiAK4PF_j1", "phiAK4PF_j1", false},
      {"phiAK4PF_j2", "phiAK4PF_j2", false},
      {"deltaETAjjAK4PF", "deltaETAjjAK4PF", false},
      {"deltaPHIjjAK4PF", "deltaPHIjjAK4PF", false},
      {"HTAK4PF", "HTAK4PF", false},
      {"neutrHadEnFrac_j1", "neutrHadEnFrac_j1", false},
      {"neutrHadEnFrac_j2", "neutrHadEnFrac_j2", false},
      {"chargedHadEnFrac_j1", "chargedHadEnFrac_j1", false},
      {"chargedHadEnFrac_j2", "chargedHadEnFrac_j2", false},
      {"photonEnFrac_j1", "photonEnFrac_j1", false},
      {"photonEnFrac_j2", "photonEnFrac_j2", false},
      {"muEnFract_j1", "muEnFract_j1", false},
      {"muEnFract_j2", "muEnFract_j2", false},
      {"neutrElectromFrac_j1", "neutrElectromFrac_j1", false},
      {"neutrElectromFrac_j2", "neutrElectromFrac_j2", false},
      {"chargedMult_j1", "chargedMult_j1", false},
      {"chargedMult_j2", "chargedMult_j2", false},
      {"neutrMult_j1", "neutrMult_j1", false},
      {"neutrMult_j2", "neutrMult_j2", false},
      {"photonMult_j1", "photonMult_j1", false},
      {"photonMult_j2", "photonMult_j2", false},
      {"METoverHTAK4PF", "METoverHTAK4PF", false},
      {"minDPhiMetAK4PF", "minDPhiMetAK4PF", false},
      {"passHLT_PFScoutingHT", jetht_branch, false},
      {"passHLT_PFScouting_SingleMuon", singlemuon_branch, false},
      {"passL1ForPFScoutingHT", l1_branch, true},
  };
}

void write_csv_header(std::ostream& out, const std::vector<CsvField>& fields) {
  for (std::size_t i = 0; i < fields.size(); ++i) {
    if (i != 0) {
      out << ',';
    }
    out << fields[i].header;
  }
  out << '\n';
}

void write_csv_row(std::ostream& out,
                   const std::vector<CsvField>& fields,
                   const std::unordered_map<std::string, std::unique_ptr<TTreeReaderValue<double>>>& readers) {
  for (std::size_t i = 0; i < fields.size(); ++i) {
    if (i != 0) {
      out << ',';
    }
    const auto& field = fields[i];
    if (field.optional && field.branch_name.empty()) {
      continue;
    }
    const auto it = readers.find(field.branch_name);
    if (it == readers.end()) {
      if (field.optional) {
        continue;
      }
      die("Missing reader for branch '" + field.branch_name + "'");
    }
    out << format_csv_value(**it->second);
  }
  out << '\n';
}

FileResult process_file(const FileTask& task, const Options& opts) {
  FileResult result;
  result.index = task.index;
  result.file_name = task.file_name;
  result.csv_part_path = task.csv_part_path;

  std::unique_ptr<TFile> file(TFile::Open(task.file_name.c_str(), "READ"));
  if (!file || file->IsZombie()) {
    result.warning = "Could not open ROOT file.";
    return result;
  }

  std::string used_tree_path;
  TTree* tree = find_tree(*file, opts, used_tree_path);
  if (!tree) {
    result.warning = "No input tree found. This script requires reduced ntuples with rootTupleTree/tree.";
    return result;
  }
  result.has_tree = true;

  try {
    const auto branch_names = get_branch_names(*tree);
    const std::string jetht_branch =
        find_branch(branch_names, kJetHtBranchCandidates, "JetHT trigger", true);
    const std::string singlemuon_branch =
        find_branch(branch_names, kSingleMuonBranchCandidates, "SingleMuon trigger", true);
    const std::string l1_branch = find_l1_branch(branch_names);
    if (opts.require_l1_for_jetht && l1_branch.empty()) {
      die("--require-l1-for-jetht was requested, but passL1ForPFScoutingHT is missing.");
    }

    const std::string goodmuon_flag_branch =
        (kUseGoodMuonBaseSelectionFlagIfAvailable &&
         std::strlen(kGoodMuonBaseSelectionFlagBranch) > 0 &&
         branch_names.count(kGoodMuonBaseSelectionFlagBranch))
            ? std::string(kGoodMuonBaseSelectionFlagBranch)
            : std::string();

    const std::vector<CsvField> csv_fields =
        build_csv_fields(jetht_branch, singlemuon_branch, l1_branch, goodmuon_flag_branch);

    std::set<std::string> enabled = {
        "run",
        "lumi",
        "event",
        "PassJSON",
        "IdTight_j1",
        "IdTight_j2",
        "nJet",
        "NAK4PF",
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
        mass_branch(opts.observable),
        jetht_branch,
        singlemuon_branch,
    };
    if (!goodmuon_flag_branch.empty()) {
      enabled.insert(goodmuon_flag_branch);
    }
    if (!l1_branch.empty()) {
      enabled.insert(l1_branch);
    }
    for (const auto& field : csv_fields) {
      if (!field.branch_name.empty()) {
        enabled.insert(field.branch_name);
      }
    }
    for (const auto& branch : enabled) {
      require_branch(branch_names, branch, task.file_name);
    }

    configure_tree_for_reading(*tree, enabled, opts.cache_mb);

    TTreeReader reader(tree);
    std::unordered_map<std::string, std::unique_ptr<TTreeReaderValue<double>>> readers;
    readers.reserve(enabled.size());
    for (const auto& branch : enabled) {
      readers.emplace(branch, std::make_unique<TTreeReaderValue<double>>(reader, branch.c_str()));
    }

    auto value = [&](const std::string& branch) -> double {
      return **readers.at(branch);
    };

    std::ofstream csv_out;
    const bool use_run_filter = has_run_filter(opts);

    while (reader.Next()) {
      const int run_i = static_cast<int>(std::llround(value("run")));
      const int lumi_i = static_cast<int>(std::llround(value("lumi")));
      if (use_run_filter && !run_is_allowed(run_i, opts)) {
        continue;
      }
      if (!goodmuon_flag_branch.empty() && value(goodmuon_flag_branch) <= 0.5) {
        continue;
      }
      if (value("PassJSON") > 0.5) {
        result.json_lumis[run_i].insert(lumi_i);
      }

      if (value(singlemuon_branch) <= 0.5) {
        continue;
      }

      const double selected_mass = value(mass_branch(opts.observable));
      if (!std::isfinite(selected_mass) || selected_mass < opts.mass_min_gev) {
        continue;
      }

      const bool pass_selection =
          (opts.observable == Observable::kWideJet)
              ? passes_widejet_selection(value("PassJSON"), value("nJet"), value("IdTight_j1"), value("IdTight_j2"),
                                         value("pTWJ_j1"), value("pTWJ_j2"), value("etaWJ_j1"), value("etaWJ_j2"),
                                         value("deltaETAjj"))
              : passes_ak4_selection(value("PassJSON"), value("NAK4PF"), value("IdTight_j1"), value("IdTight_j2"),
                                     value("pTAK4PF_j1"), value("pTAK4PF_j2"), value("etaAK4PF_j1"),
                                     value("etaAK4PF_j2"), value("deltaETAjjAK4PF"));
      if (!pass_selection) {
        continue;
      }

      const bool pass_jetht = value(jetht_branch) > 0.5;
      const bool pass_l1 = !l1_branch.empty() && value(l1_branch) > 0.5;
      const bool pass_numerator = pass_jetht && (!opts.require_l1_for_jetht || pass_l1);
      if (pass_numerator) {
        continue;
      }
      result.bad_lumis[run_i].insert(lumi_i);

      if (!csv_out.is_open()) {
        csv_out.open(task.csv_part_path);
        if (!csv_out) {
          die("Could not create temporary CSV part: " + task.csv_part_path.string());
        }
      }
      write_csv_row(csv_out, csv_fields, readers);
      ++result.problem_events;
      ++result.run_counts[run_i];
      result.run_lumis[run_i].insert(lumi_i);
    }

    result.processed = true;
  } catch (const std::exception& ex) {
    result.warning = ex.what();
    result.processed = false;
  }

  return result;
}

void merge_csv_parts(const fs::path& output_csv, const std::vector<FileResult>& results) {
  std::ofstream out(output_csv);
  if (!out) {
    die("Could not write CSV output: " + output_csv.string());
  }

  const std::vector<CsvField> header_fields = build_csv_fields(
      "passHLT_PFScoutingHT",
      "passHLT_PFScouting_SingleMuon",
      "passL1ForPFScoutingHT",
      (kUseGoodMuonBaseSelectionFlagIfAvailable && std::strlen(kGoodMuonBaseSelectionFlagBranch) > 0)
          ? std::string(kGoodMuonBaseSelectionFlagBranch)
          : std::string());
  write_csv_header(out, header_fields);

  for (const auto& result : results) {
    if (result.problem_events == 0) {
      continue;
    }
    std::ifstream in(result.csv_part_path);
    if (!in) {
      die("Could not read temporary CSV part: " + result.csv_part_path.string());
    }
    out << in.rdbuf();
  }
}

std::string join_sorted_lumis(const std::set<int>& lumis) {
  std::ostringstream os;
  bool first = true;
  for (int lumi : lumis) {
    if (!first) {
      os << ',';
    }
    os << lumi;
    first = false;
  }
  return os.str();
}

using LumiMask = std::unordered_map<int, std::set<int>>;

std::vector<std::pair<int, int>> make_lumi_ranges(const std::set<int>& lumis) {
  std::vector<std::pair<int, int>> ranges;
  if (lumis.empty()) {
    return ranges;
  }

  auto it = lumis.begin();
  int range_start = *it;
  int range_end = *it;
  ++it;
  for (; it != lumis.end(); ++it) {
    if (*it == range_end + 1) {
      range_end = *it;
      continue;
    }
    ranges.emplace_back(range_start, range_end);
    range_start = *it;
    range_end = *it;
  }
  ranges.emplace_back(range_start, range_end);
  return ranges;
}

void write_updated_golden_json(const fs::path& output_json,
                               LumiMask json_lumis,
                               const std::unordered_map<int, std::set<int>>& bad_lumis) {
  for (const auto& [run, lumis] : bad_lumis) {
    auto it = json_lumis.find(run);
    if (it == json_lumis.end()) {
      continue;
    }
    for (int lumi : lumis) {
      it->second.erase(lumi);
    }
  }

  std::vector<int> runs;
  runs.reserve(json_lumis.size());
  for (const auto& [run, _] : json_lumis) {
    runs.push_back(run);
  }
  std::sort(runs.begin(), runs.end());

  std::ofstream out(output_json);
  if (!out) {
    die("Could not write updated golden JSON: " + output_json.string());
  }

  out << "{\n";
  bool first_run = true;
  for (int run : runs) {
    auto kept_it = json_lumis.find(run);
    if (kept_it == json_lumis.end() || kept_it->second.empty()) {
      continue;
    }

    const auto ranges = make_lumi_ranges(kept_it->second);
    if (ranges.empty()) {
      continue;
    }

    if (!first_run) {
      out << ",\n";
    }
    first_run = false;
    out << "  \"" << run << "\": [";
    for (std::size_t i = 0; i < ranges.size(); ++i) {
      if (i != 0) {
        out << ", ";
      }
      out << '[' << ranges[i].first << ", " << ranges[i].second << ']';
    }
    out << ']';
  }
  out << "\n}\n";
}

void write_run_summary(const fs::path& output_txt,
                       const std::unordered_map<int, std::uint64_t>& run_counts,
                       const std::unordered_map<int, std::set<int>>& run_lumis) {
  std::vector<std::pair<int, std::uint64_t>> rows(run_counts.begin(), run_counts.end());
  std::sort(rows.begin(), rows.end(), [](const auto& a, const auto& b) {
    if (a.second != b.second) {
      return a.second > b.second;
    }
    return a.first < b.first;
  });

  std::ofstream out(output_txt);
  if (!out) {
    die("Could not write run summary: " + output_txt.string());
  }
  out << "Run Number\tProblematic Events\tLS Values\n";
  for (const auto& [run, count] : rows) {
    const auto lumi_it = run_lumis.find(run);
    out << run << '\t' << count << '\t';
    if (lumi_it != run_lumis.end()) {
      out << join_sorted_lumis(lumi_it->second);
    }
    out << '\n';
  }
}

}  // namespace

int main(int argc, char** argv) {
  try {
    const Options opts = parse_args(argc, argv);
    const std::vector<std::string> inputs = resolve_inputs(opts);

    ROOT::EnableThreadSafety();

    const fs::path output_dir = fs::absolute(opts.output_dir);
    fs::create_directories(output_dir);
    const fs::path temp_dir = output_dir / ".csv_parts";
    fs::create_directories(temp_dir);

    std::vector<FileTask> tasks;
    tasks.reserve(inputs.size());
    for (std::size_t i = 0; i < inputs.size(); ++i) {
      std::ostringstream part_name;
      part_name << "part_" << std::setw(6) << std::setfill('0') << i << ".csv";
      tasks.push_back(FileTask{i, inputs[i], temp_dir / part_name.str()});
    }

    std::cout << info_tag() << " Inputs: " << tasks.size() << " file(s)" << std::endl;
    std::cout << info_tag() << " Observable: " << observable_name(opts.observable)
              << ", threshold " << mass_branch(opts.observable) << " >= "
              << format_csv_value(opts.mass_min_gev) << " GeV" << std::endl;

    ProgressState progress;
    ProgressState* progress_ptr = nullptr;
    if (opts.progress_every_sec > 0) {
      progress.total_files = tasks.size();
      progress.tty = stdout_is_tty();
      progress.start_time = std::chrono::steady_clock::now();
      progress_ptr = &progress;
      print_progress(progress_ptr);
    }

    auto runner = [&](const FileTask& task) {
      FileResult result = process_file(task, opts);
      if (progress_ptr) {
        progress.files_done.fetch_add(1);
        print_progress(progress_ptr, task.file_name);
      }
      return result;
    };

    std::vector<FileResult> results;
    if (opts.threads <= 1 || tasks.size() == 1) {
      results.reserve(tasks.size());
      for (const auto& task : tasks) {
        results.push_back(runner(task));
      }
    } else {
      ROOT::TThreadExecutor executor(opts.threads);
      results = executor.Map(runner, tasks);
    }

    std::sort(results.begin(), results.end(), [](const FileResult& a, const FileResult& b) {
      return a.index < b.index;
    });

    std::unordered_map<int, std::uint64_t> run_counts;
    std::unordered_map<int, std::set<int>> run_lumis;
    std::unordered_map<int, std::set<int>> json_lumis;
    std::unordered_map<int, std::set<int>> bad_lumis;
    std::uint64_t total_problem_events = 0;
    std::size_t processed_files = 0;
    for (const auto& result : results) {
      if (result.processed) {
        ++processed_files;
      }
      total_problem_events += result.problem_events;
      for (const auto& [run, count] : result.run_counts) {
        run_counts[run] += count;
      }
      for (const auto& [run, lumis] : result.run_lumis) {
        auto& merged = run_lumis[run];
        merged.insert(lumis.begin(), lumis.end());
      }
      for (const auto& [run, lumis] : result.json_lumis) {
        auto& merged = json_lumis[run];
        merged.insert(lumis.begin(), lumis.end());
      }
      for (const auto& [run, lumis] : result.bad_lumis) {
        auto& merged = bad_lumis[run];
        merged.insert(lumis.begin(), lumis.end());
      }
    }

    if (processed_files == 0) {
      for (const auto& result : results) {
        if (!result.warning.empty()) {
          std::cerr << warn_tag() << " " << fs::path(result.file_name).filename().string() << ": "
                    << result.warning << std::endl;
        }
      }
      die("No input trees were processed. This script requires reduced ntuples with rootTupleTree/tree.");
    }

    const fs::path csv_path = output_dir / "inefficiency_events.csv";
    const fs::path summary_path = output_dir / "inefficiency_runs_summary.txt";
    const fs::path updated_json_path = output_dir / "updated_golden_json.json";
    merge_csv_parts(csv_path, results);
    write_run_summary(summary_path, run_counts, run_lumis);
    write_updated_golden_json(updated_json_path, std::move(json_lumis), bad_lumis);

    for (const auto& result : results) {
      if (!result.csv_part_path.empty()) {
        std::error_code ec;
        fs::remove(result.csv_part_path, ec);
      }
      if (!result.warning.empty()) {
        std::cerr << warn_tag() << " " << fs::path(result.file_name).filename().string() << ": "
                  << result.warning << std::endl;
      }
    }
    std::error_code ec;
    fs::remove(temp_dir, ec);

    std::cout << info_tag() << " Problematic events: " << total_problem_events << std::endl;
    std::cout << info_tag() << " CSV: " << csv_path.string() << std::endl;
    std::cout << info_tag() << " Summary: " << summary_path.string() << std::endl;
    std::cout << info_tag() << " Updated JSON: " << updated_json_path.string() << std::endl;
    return 0;
  } catch (const std::exception& ex) {
    std::cerr << "[ERROR] " << ex.what() << std::endl;
    return 1;
  }
}

