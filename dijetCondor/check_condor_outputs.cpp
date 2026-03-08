#include <TChain.h>
#include <TFile.h>
#include <TROOT.h>
#include <TTree.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <regex>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/wait.h>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

namespace {

constexpr const char* kInfo = "\033[1;96m[INFO]\033[0m";
constexpr const char* kError = "[ERROR]";
constexpr const char* kWarnRed = "\033[1;31m[WARN]\033[0m";
constexpr const char* kSubGreen = "\033[1;92m[SUB]\033[0m";

struct Options {
  std::string path1;
  std::string path2;
  bool resubmit = false;
  bool verbose = false;
  std::string config;
  std::string input_tree = "Events";
  std::optional<int> job_start;
  std::optional<int> job_end;
  bool check_subdirs = false;
  bool no_per_file = false;
  bool total_events = false;
  std::string dataset;
  int request_memory_mb = 0;
  int threads = 0;
};

struct CommandResult {
  int exit_code = 0;
  std::string output;
};

struct CountSpec {
  std::vector<std::string> files;
  std::vector<std::string> trees;
};

struct CountResult {
  long long entries = 0;
  int added = 0;
  std::string tree;
  std::string error;
};

struct FileScanResult {
  long long entries = 0;
  int added = 0;
  std::string tree;
  std::string error;
};

struct FileTask {
  std::size_t group_index = 0;
  std::string group_id;
  std::string file_path;
};

struct GroupAccumulator {
  long long entries = 0;
  int added = 0;
  std::string tree;
  bool mixed_tree = false;
  std::string error;
};

struct DasInputPlan {
  long long known_events = 0;
  std::size_t matched_files = 0;
  std::vector<std::string> missing_files;
};

struct ProgressState {
  std::atomic<std::size_t> done{0};
  std::atomic<bool> stop{false};
};

struct SelectedJob {
  int index = 0;
  std::vector<std::string> input_files;
  std::optional<fs::path> jdl;
  std::string job_label;
  std::string expected_root;
  std::optional<fs::path> output_path;
};

struct JobRow {
  int job_index = 0;
  std::string status;
  std::size_t input_files = 0;
  long long input_events = 0;
  long long output_events = 0;
  std::string input_tree;
  std::string output_tree;
  std::string expected_root;
  std::string output_root;
  std::string jdl;
  std::string note;
};

struct JobSummary {
  long long total_input_events = 0;
  long long total_output_events = 0;
  int checked_jobs = 0;
  int ok_jobs = 0;
  int missing_jobs = 0;
  int mismatch_jobs = 0;
  int error_jobs = 0;
};

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

bool ends_with(const std::string& s, const std::string& suffix) {
  return s.size() >= suffix.size() && s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::vector<std::string> split(const std::string& s, char delim) {
  std::vector<std::string> out;
  std::string token;
  std::stringstream ss(s);
  while (std::getline(ss, token, delim)) {
    token = trim(token);
    if (!token.empty()) {
      out.push_back(token);
    }
  }
  return out;
}

std::string sanitize_tsv(const std::string& s) {
  std::string out = s;
  for (char& c : out) {
    if (c == '\t' || c == '\n' || c == '\r') {
      c = ' ';
    }
  }
  return out;
}

std::string format_with_dots(long long value) {
  const bool neg = value < 0;
  std::string digits = std::to_string(neg ? -value : value);
  for (int i = static_cast<int>(digits.size()) - 3; i > 0; i -= 3) {
    digits.insert(static_cast<std::size_t>(i), ".");
  }
  return neg ? "-" + digits : digits;
}

std::string shell_quote(const std::string& s) {
  std::string out = "'";
  for (char c : s) {
    if (c == '\'') {
      out += "'\"'\"'";
    } else {
      out.push_back(c);
    }
  }
  out.push_back('\'');
  return out;
}

fs::path absolute_path(const fs::path& p) {
  std::error_code ec;
  fs::path out = fs::absolute(p, ec);
  if (ec) {
    return p;
  }
  return out.lexically_normal();
}

std::string read_text(const fs::path& path) {
  std::ifstream in(path);
  if (!in.is_open()) {
    throw std::runtime_error("cannot open file: " + path.string());
  }
  std::ostringstream ss;
  ss << in.rdbuf();
  return ss.str();
}

std::vector<std::string> read_lines(const fs::path& path) {
  std::ifstream in(path);
  if (!in.is_open()) {
    throw std::runtime_error("cannot open file: " + path.string());
  }
  std::vector<std::string> out;
  std::string line;
  while (std::getline(in, line)) {
    out.push_back(line);
  }
  return out;
}

void write_text(const fs::path& path, const std::string& text) {
  std::ofstream out(path);
  if (!out.is_open()) {
    throw std::runtime_error("cannot write file: " + path.string());
  }
  out << text;
}

CommandResult run_command_capture(const std::string& cmd) {
  CommandResult result;
  std::string full_cmd = cmd + " 2>&1";
  FILE* pipe = popen(full_cmd.c_str(), "r");
  if (!pipe) {
    throw std::runtime_error("failed to run command: " + cmd);
  }
  char buffer[4096];
  while (std::fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    result.output += buffer;
  }
  int status = pclose(pipe);
  if (status == -1) {
    result.exit_code = 1;
  } else if (WIFEXITED(status)) {
    result.exit_code = WEXITSTATUS(status);
  } else {
    result.exit_code = status;
  }
  return result;
}

std::vector<std::string> split_whitespace(const std::string& s) {
  std::vector<std::string> out;
  std::istringstream iss(s);
  std::string token;
  while (iss >> token) {
    out.push_back(token);
  }
  return out;
}

bool is_all_digits(const std::string& s) {
  if (s.empty()) {
    return false;
  }
  for (char c : s) {
    if (!std::isdigit(static_cast<unsigned char>(c))) {
      return false;
    }
  }
  return true;
}

std::optional<std::string> extract_store_lfn(const std::string& text) {
  const std::size_t pos = text.find("/store/");
  if (pos == std::string::npos) {
    return std::nullopt;
  }
  std::size_t end = pos;
  while (end < text.size()) {
    const char c = text[end];
    if (std::isspace(static_cast<unsigned char>(c)) || c == '"' || c == '\'' || c == ',' || c == ']' || c == '}') {
      break;
    }
    ++end;
  }
  return text.substr(pos, end - pos);
}

std::string sanitize_filename_component(const std::string& s) {
  std::string out;
  out.reserve(s.size());
  for (char c : s) {
    if (std::isalnum(static_cast<unsigned char>(c)) || c == '.' || c == '_' || c == '-') {
      out.push_back(c);
    } else {
      out.push_back('_');
    }
  }
  return out;
}

std::string make_root_open_path(const std::string& file_path) {
  if (file_path.find("://") != std::string::npos) {
    return file_path;
  }
  if (!file_path.empty() && file_path.front() == '/') {
    return "file://" + file_path;
  }
  return file_path;
}

std::optional<std::string> eos_xrootd_fallback(const std::string& file_path) {
  if (starts_with(file_path, "/eos/uscms/store/")) {
    return "root://cmseos.fnal.gov//store/" + file_path.substr(std::string("/eos/uscms/store/").size());
  }
  if (starts_with(file_path, "/eos/cms/store/")) {
    return "root://eoscms.cern.ch//store/" + file_path.substr(std::string("/eos/cms/store/").size());
  }
  return std::nullopt;
}

void print_progress_line(const std::string& label, std::size_t done, std::size_t total) {
  static constexpr std::size_t kBarWidth = 32;
  const double frac = total == 0 ? 1.0 : static_cast<double>(done) / static_cast<double>(total);
  std::size_t filled = static_cast<std::size_t>(frac * static_cast<double>(kBarWidth));
  if (filled > kBarWidth) {
    filled = kBarWidth;
  }

  std::ostringstream bar;
  bar << '\r' << kInfo << ' ' << label << " [";
  for (std::size_t i = 0; i < kBarWidth; ++i) {
    bar << (i < filled ? '=' : ' ');
  }
  bar << "] " << format_with_dots(static_cast<long long>(done)) << "/" << format_with_dots(static_cast<long long>(total));
  if (total > 0) {
    bar << " (" << static_cast<int>(frac * 100.0) << "%)";
  }
  std::cout << bar.str() << std::flush;
}

void run_progress_monitor(const std::string& label, std::size_t total, ProgressState& state) {
  using namespace std::chrono_literals;
  while (!state.stop.load()) {
    print_progress_line(label, state.done.load(), total);
    std::this_thread::sleep_for(250ms);
  }
  print_progress_line(label, state.done.load(), total);
  std::cout << '\n';
}

fs::path dataset_event_cache_path(const fs::path& cjobs_dir, const std::string& dataset) {
  return cjobs_dir / ("das_file_event_cache_" + sanitize_filename_component(dataset) + ".tsv");
}

std::unordered_map<std::string, long long> load_dataset_event_cache(const fs::path& cache_path) {
  std::unordered_map<std::string, long long> out;
  if (!fs::exists(cache_path)) {
    return out;
  }
  for (const std::string& raw : read_lines(cache_path)) {
    const std::string line = trim(raw);
    if (line.empty() || line[0] == '#') {
      continue;
    }
    const std::size_t tab = line.find('\t');
    if (tab == std::string::npos) {
      continue;
    }
    const std::string lfn = trim(line.substr(0, tab));
    const std::string events = trim(line.substr(tab + 1));
    if (!lfn.empty() && is_all_digits(events)) {
      out[lfn] = std::stoll(events);
    }
  }
  return out;
}

void write_dataset_event_cache(const fs::path& cache_path, const std::unordered_map<std::string, long long>& event_map) {
  std::vector<std::pair<std::string, long long>> ordered(event_map.begin(), event_map.end());
  std::sort(ordered.begin(), ordered.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

  std::ostringstream ss;
  for (const auto& item : ordered) {
    ss << item.first << '\t' << item.second << '\n';
  }
  write_text(cache_path, ss.str());
}

std::unordered_map<std::string, long long> query_dataset_file_event_map(const std::string& dataset) {
  const std::string cmd =
      "dasgoclient -query " + shell_quote("file dataset=" + dataset + " | grep file.name,file.nevents");
  const CommandResult result = run_command_capture(cmd);
  if (result.exit_code != 0) {
    throw std::runtime_error("dasgoclient failed for dataset=" + dataset + ": " + trim(result.output));
  }

  std::unordered_map<std::string, long long> out;
  std::istringstream iss(result.output);
  std::string line;
  while (std::getline(iss, line)) {
    line = trim(line);
    if (line.empty()) {
      continue;
    }

    std::optional<std::string> lfn = extract_store_lfn(line);
    std::optional<long long> events;
    for (const std::string& token : split_whitespace(line)) {
      if (!lfn) {
        lfn = extract_store_lfn(token);
      }
      std::string cleaned = token;
      while (!cleaned.empty() && (cleaned.back() == ',' || cleaned.back() == ';')) {
        cleaned.pop_back();
      }
      if (!events && is_all_digits(cleaned)) {
        events = std::stoll(cleaned);
      }
    }
    if (lfn && events) {
      out[*lfn] = *events;
    }
  }

  if (out.empty()) {
    throw std::runtime_error("DAS file-event query returned no parseable file entries.");
  }
  return out;
}

std::unordered_map<std::string, long long> get_dataset_file_event_map(
    const fs::path& cjobs_dir,
    const std::string& dataset,
    bool& from_cache) {
  const fs::path cache_path = dataset_event_cache_path(cjobs_dir, dataset);
  auto cached = load_dataset_event_cache(cache_path);
  if (!cached.empty()) {
    from_cache = true;
    return cached;
  }

  auto fresh = query_dataset_file_event_map(dataset);
  write_dataset_event_cache(cache_path, fresh);
  from_cache = false;
  return fresh;
}

DasInputPlan build_das_input_plan(
    const std::vector<std::string>& input_files,
    const std::unordered_map<std::string, long long>& das_event_map) {
  DasInputPlan plan;
  for (const std::string& input_file : input_files) {
    const auto lfn = extract_store_lfn(input_file);
    if (!lfn) {
      plan.missing_files.push_back(input_file);
      continue;
    }
    const auto it = das_event_map.find(*lfn);
    if (it == das_event_map.end()) {
      plan.missing_files.push_back(input_file);
      continue;
    }
    plan.known_events += it->second;
    ++plan.matched_files;
  }
  return plan;
}

void print_help(const char* argv0) {
  std::cout
      << "usage: " << argv0 << " [--resubmit] [--verbose] [--config CONFIG] [--input-tree INPUT_TREE]\n"
      << "       [--job-start N] [--job-end M] [--check-subdirs] [--noPerFile]\n"
      << "       [--total-events] [--dataset DATASET] [--request-memory-mb MB]\n"
      << "       [--threads N] path1 [path2]\n\n"
      << "Mode A: check missing Condor outputs using cjobs_dir + eos_output_dir.\n"
      << "Mode B: with a single path (directory or .root file), count entries automatically.\n";
}

Options parse_args(int argc, char** argv) {
  Options opt;
  std::vector<std::string> positional;

  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    auto need_value = [&](const std::string& name) -> std::string {
      if (i + 1 >= argc) {
        throw std::runtime_error("missing value for " + name);
      }
      return argv[++i];
    };

    if (arg == "-h" || arg == "--help") {
      print_help(argv[0]);
      std::exit(0);
    } else if (arg == "--resubmit") {
      opt.resubmit = true;
    } else if (arg == "--verbose") {
      opt.verbose = true;
    } else if (arg == "--config") {
      opt.config = need_value(arg);
    } else if (arg == "--input-tree") {
      opt.input_tree = need_value(arg);
    } else if (arg == "--job-start") {
      opt.job_start = std::stoi(need_value(arg));
    } else if (arg == "--job-end") {
      opt.job_end = std::stoi(need_value(arg));
    } else if (arg == "--check-subdirs") {
      opt.check_subdirs = true;
    } else if (arg == "--noPerFile" || arg == "--no-per-file") {
      opt.no_per_file = true;
    } else if (arg == "--total-events") {
      opt.total_events = true;
    } else if (arg == "--dataset") {
      opt.dataset = need_value(arg);
    } else if (arg == "--request-memory-mb") {
      opt.request_memory_mb = std::stoi(need_value(arg));
    } else if (arg == "--threads") {
      opt.threads = std::stoi(need_value(arg));
    } else if (starts_with(arg, "--")) {
      throw std::runtime_error("unknown option: " + arg);
    } else {
      positional.push_back(arg);
    }
  }

  if (positional.empty() || positional.size() > 2) {
    throw std::runtime_error("expected path1 [path2]");
  }
  opt.path1 = positional[0];
  if (positional.size() == 2) {
    opt.path2 = positional[1];
  }
  return opt;
}

std::optional<std::string> validate_job_range(const Options& opt) {
  if (opt.job_start && *opt.job_start < 0) {
    return "--job-start must be >= 0.";
  }
  if (opt.job_end && *opt.job_end < 0) {
    return "--job-end must be >= 0.";
  }
  if (opt.job_start && opt.job_end && *opt.job_start > *opt.job_end) {
    return "--job-start cannot be greater than --job-end.";
  }
  if (opt.threads < 0) {
    return "--threads must be >= 0.";
  }
  return std::nullopt;
}

bool in_job_range(int idx, const std::optional<int>& start, const std::optional<int>& end) {
  if (start && idx < *start) {
    return false;
  }
  if (end && idx > *end) {
    return false;
  }
  return true;
}

std::map<std::string, std::string> parse_key_value_file(const fs::path& path) {
  std::map<std::string, std::string> kv;
  for (const std::string& raw : read_lines(path)) {
    const std::string line = trim(raw);
    if (line.empty() || line[0] == '#') {
      continue;
    }
    const std::size_t eq = line.find('=');
    if (eq == std::string::npos) {
      continue;
    }
    kv[trim(line.substr(0, eq))] = trim(line.substr(eq + 1));
  }
  return kv;
}

fs::path resolve_input_list_path(
    const std::string& raw,
    const fs::path& config_path,
    const fs::path& repo_root,
    const std::optional<fs::path>& cmssw_base) {
  fs::path expanded = raw;
  if (expanded.is_absolute()) {
    return expanded;
  }

  std::vector<fs::path> candidates = {
      fs::path(raw),
      config_path.parent_path() / raw,
      repo_root / raw,
  };
  if (cmssw_base) {
    candidates.push_back(*cmssw_base / "src" / raw);
  }

  for (const fs::path& cand : candidates) {
    if (fs::exists(cand)) {
      return absolute_path(cand);
    }
  }
  return absolute_path(config_path.parent_path() / raw);
}

std::vector<std::string> read_input_files(const fs::path& input_list) {
  std::vector<std::string> files;
  for (const std::string& raw : read_lines(input_list)) {
    const std::string line = trim(raw);
    if (!line.empty()) {
      files.push_back(line);
    }
  }
  if (files.empty()) {
    throw std::runtime_error("Input list is empty: " + input_list.string());
  }
  return files;
}

std::vector<std::vector<std::string>> split_into_chunks(const std::vector<std::string>& items, int chunk_size) {
  if (chunk_size <= 0) {
    throw std::runtime_error("Interval must be > 0.");
  }
  std::vector<std::vector<std::string>> chunks;
  for (std::size_t i = 0; i < items.size(); i += static_cast<std::size_t>(chunk_size)) {
    const std::size_t end = std::min(items.size(), i + static_cast<std::size_t>(chunk_size));
    chunks.emplace_back(items.begin() + static_cast<long>(i), items.begin() + static_cast<long>(end));
  }
  return chunks;
}

struct LoadedChunks {
  std::vector<std::vector<std::string>> chunks;
  int interval = 0;
  std::string dataset;
  fs::path input_list_path;
};

LoadedChunks load_job_chunks_from_config(const fs::path& config_path, const fs::path& repo_root) {
  const auto kv = parse_key_value_file(config_path);
  auto input_it = kv.find("InputList");
  auto interval_it = kv.find("Interval");
  if (input_it == kv.end() || interval_it == kv.end()) {
    throw std::runtime_error("Missing required config key(s): InputList, Interval");
  }

  const int interval = std::stoi(interval_it->second);
  std::optional<fs::path> cmssw_base;
  const char* cmssw_env = std::getenv("CMSSW_BASE");
  if (cmssw_env && *cmssw_env) {
    cmssw_base = absolute_path(fs::path(cmssw_env));
  }
  const fs::path input_list_path = resolve_input_list_path(input_it->second, config_path, repo_root, cmssw_base);
  if (!fs::exists(input_list_path)) {
    throw std::runtime_error("Input list does not exist: " + input_list_path.string());
  }

  LoadedChunks out;
  out.interval = interval;
  out.dataset = kv.count("Dataset") ? kv.at("Dataset") : "";
  out.input_list_path = input_list_path;
  out.chunks = split_into_chunks(read_input_files(input_list_path), interval);
  return out;
}

std::vector<fs::path> get_jdl_files(const fs::path& cjobs_dir) {
  std::vector<fs::path> out;
  std::error_code ec;
  for (fs::directory_iterator it(cjobs_dir, fs::directory_options::skip_permission_denied, ec), end; it != end;
       it.increment(ec)) {
    if (ec) {
      ec.clear();
      continue;
    }
    if (it->is_regular_file(ec) && it->path().extension() == ".jdl") {
      out.push_back(absolute_path(it->path()));
    }
  }
  std::sort(out.begin(), out.end());
  return out;
}

std::optional<std::string> read_executable_from_jdl(const fs::path& jdl_path) {
  for (const std::string& raw : read_lines(jdl_path)) {
    const std::string line = trim(raw);
    if (starts_with(line, "Executable")) {
      const std::size_t eq = line.find('=');
      if (eq != std::string::npos) {
        return trim(line.substr(eq + 1));
      }
    }
  }
  return std::nullopt;
}

std::optional<std::string> expected_root_from_sh(const fs::path& sh_path) {
  if (!fs::exists(sh_path)) {
    return std::nullopt;
  }
  static const std::regex re("([A-Za-z0-9._-]+_reduced_skim\\.root)");
  const std::string text = read_text(sh_path);
  std::smatch match;
  if (!std::regex_search(text, match, re)) {
    return std::nullopt;
  }
  return match[1].str();
}

using ExpectedMap = std::map<std::string, fs::path>;

std::pair<ExpectedMap, std::vector<fs::path>> build_expected_map(const fs::path& cjobs_dir, const std::vector<fs::path>& jdls) {
  ExpectedMap expected;
  std::vector<fs::path> unresolved;
  for (const fs::path& jdl : jdls) {
    const auto sh_name = read_executable_from_jdl(jdl);
    if (!sh_name) {
      unresolved.push_back(jdl);
      continue;
    }
    const auto root_name = expected_root_from_sh(cjobs_dir / *sh_name);
    if (!root_name) {
      unresolved.push_back(jdl);
      continue;
    }
    expected[*root_name] = jdl;
  }
  return {expected, unresolved};
}

std::optional<int> extract_job_index_from_root(const std::string& root_name) {
  static const std::regex re("_n(\\d+)_reduced_skim\\.root$");
  std::smatch match;
  if (!std::regex_search(root_name, match, re)) {
    return std::nullopt;
  }
  return std::stoi(match[1].str());
}

std::optional<int> extract_job_index_from_jdl(const std::string& jdl_name) {
  static const std::regex re("_n(\\d+)\\.jdl$");
  std::smatch match;
  if (!std::regex_search(jdl_name, match, re)) {
    return std::nullopt;
  }
  return std::stoi(match[1].str());
}

std::pair<ExpectedMap, int> filter_expected_map_by_range(
    const ExpectedMap& expected_map,
    const std::optional<int>& job_start,
    const std::optional<int>& job_end) {
  if (!job_start && !job_end) {
    return {expected_map, 0};
  }
  ExpectedMap filtered;
  int skipped_unindexed = 0;
  for (const auto& item : expected_map) {
    const auto idx = extract_job_index_from_root(item.first);
    if (!idx) {
      ++skipped_unindexed;
      continue;
    }
    if (in_job_range(*idx, job_start, job_end)) {
      filtered[item.first] = item.second;
    }
  }
  return {filtered, skipped_unindexed};
}

std::vector<fs::path> get_existing_root_files(const fs::path& eos_dir, bool recursive) {
  std::vector<fs::path> out;
  std::error_code ec;
  if (recursive) {
    for (fs::recursive_directory_iterator it(eos_dir, fs::directory_options::skip_permission_denied, ec), end; it != end;
         it.increment(ec)) {
      if (ec) {
        ec.clear();
        continue;
      }
      if (it->is_regular_file(ec) && ends_with(it->path().filename().string(), "_reduced_skim.root")) {
        out.push_back(absolute_path(it->path()));
      }
    }
  } else {
    for (fs::directory_iterator it(eos_dir, fs::directory_options::skip_permission_denied, ec), end; it != end;
         it.increment(ec)) {
      if (ec) {
        ec.clear();
        continue;
      }
      if (it->is_regular_file(ec) && ends_with(it->path().filename().string(), "_reduced_skim.root")) {
        out.push_back(absolute_path(it->path()));
      }
    }
  }
  std::sort(out.begin(), out.end());
  return out;
}

using ExistingByName = std::unordered_map<std::string, std::vector<fs::path>>;
using ExistingByIndex = std::unordered_map<int, std::vector<fs::path>>;

std::pair<ExistingByName, ExistingByIndex> build_existing_root_maps(const std::vector<fs::path>& files) {
  ExistingByName by_name;
  ExistingByIndex by_index;
  for (const fs::path& path : files) {
    const std::string name = path.filename().string();
    by_name[name].push_back(path);
    const auto idx = extract_job_index_from_root(name);
    if (idx) {
      by_index[*idx].push_back(path);
    }
  }
  return {by_name, by_index};
}

std::pair<std::vector<std::string>, std::map<std::string, std::string>> resolve_missing_with_fallback(
    const std::vector<std::string>& expected_roots,
    const std::vector<std::string>& existing_root_names) {
  std::set<std::string> existing(existing_root_names.begin(), existing_root_names.end());
  std::vector<std::string> missing_exact;
  for (const std::string& name : expected_roots) {
    if (!existing.count(name)) {
      missing_exact.push_back(name);
    }
  }

  std::set<std::string> used_existing;
  for (const std::string& name : expected_roots) {
    if (existing.count(name)) {
      used_existing.insert(name);
    }
  }

  std::vector<std::string> pool;
  for (const std::string& name : existing_root_names) {
    if (ends_with(name, "_reduced_skim.root") && !used_existing.count(name)) {
      pool.push_back(name);
    }
  }

  std::map<std::string, std::string> fallback_matches;
  for (const std::string& expected_name : missing_exact) {
    std::optional<std::size_t> candidate_index;
    const auto idx = extract_job_index_from_root(expected_name);
    if (idx) {
      const std::string suffix = "_n" + std::to_string(*idx) + "_reduced_skim.root";
      for (std::size_t i = 0; i < pool.size(); ++i) {
        if (ends_with(pool[i], suffix)) {
          candidate_index = i;
          break;
        }
      }
    }
    if (!candidate_index && !pool.empty()) {
      candidate_index = 0;
    }
    if (candidate_index) {
      fallback_matches[expected_name] = pool[*candidate_index];
      pool.erase(pool.begin() + static_cast<long>(*candidate_index));
    }
  }

  std::vector<std::string> missing_final;
  for (const std::string& name : missing_exact) {
    if (!fallback_matches.count(name)) {
      missing_final.push_back(name);
    }
  }
  return {missing_final, fallback_matches};
}

int effective_threads(int requested, std::size_t max_tasks) {
  unsigned n = 1;
  if (requested > 0) {
    n = static_cast<unsigned>(requested);
  } else {
    n = std::thread::hardware_concurrency();
    if (n == 0) {
      n = 1;
    }
  }
  if (max_tasks > 0 && n > max_tasks) {
    n = static_cast<unsigned>(max_tasks);
  }
  if (n == 0) {
    n = 1;
  }
  return static_cast<int>(n);
}

CountResult count_group_chain(const CountSpec& spec) {
  CountResult result;
  const std::vector<std::string> trees = spec.trees.empty() ? std::vector<std::string>{"rootTupleTree/tree"} : spec.trees;
  result.tree = trees.front();
  if (spec.files.empty()) {
    return result;
  }

  for (std::size_t i = 0; i < trees.size(); ++i) {
    const std::string& tree_name = trees[i];
    TChain chain(tree_name.c_str());
    int added = 0;
    for (const std::string& path : spec.files) {
      if (chain.Add(path.c_str()) > 0) {
        ++added;
      }
    }
    const long long entries = static_cast<long long>(chain.GetEntries());
    result.entries = entries;
    result.added = added;
    result.tree = tree_name;
    if (entries > 0 || i + 1 == trees.size()) {
      return result;
    }
  }
  return result;
}

FileScanResult count_file_entries(const std::string& file_path, const std::vector<std::string>& trees) {
  FileScanResult result;
  if (file_path.empty()) {
    result.error = "empty file path";
    return result;
  }

  std::vector<std::string> open_candidates;
  open_candidates.push_back(file_path);
  const std::string file_url = make_root_open_path(file_path);
  if (file_url != file_path) {
    open_candidates.push_back(file_url);
  }
  if (const auto xrd = eos_xrootd_fallback(file_path)) {
    if (*xrd != file_path && *xrd != file_url) {
      open_candidates.push_back(*xrd);
    }
  }

  std::unique_ptr<TFile> file;
  for (const std::string& candidate : open_candidates) {
    file.reset(TFile::Open(candidate.c_str(), "READ"));
    if (file && !file->IsZombie()) {
      break;
    }
  }
  if (!file || file->IsZombie()) {
    result.error = "failed to open ROOT file: " + file_path;
    return result;
  }

  result.added = 1;
  const std::vector<std::string> tree_candidates =
      trees.empty() ? std::vector<std::string>{"rootTupleTree/tree"} : trees;
  result.tree = tree_candidates.front();

  for (std::size_t i = 0; i < tree_candidates.size(); ++i) {
    TTree* tree = nullptr;
    file->GetObject(tree_candidates[i].c_str(), tree);
    if (!tree) {
      continue;
    }
    result.entries = static_cast<long long>(tree->GetEntries());
    result.tree = tree_candidates[i];
    if (result.entries > 0 || i + 1 == tree_candidates.size()) {
      return result;
    }
  }
  return result;
}

CountResult count_files_parallel(const CountSpec& spec, int requested_threads) {
  CountResult total;
  const std::vector<std::string> trees = spec.trees.empty() ? std::vector<std::string>{"rootTupleTree/tree"} : spec.trees;
  total.tree = trees.front();
  if (spec.files.empty()) {
    return total;
  }

  ROOT::EnableThreadSafety();
  const int n_threads = effective_threads(requested_threads, spec.files.size());
  std::atomic<std::size_t> next{0};
  std::mutex merge_mutex;
  bool mixed_tree = false;
  ProgressState progress;
  std::thread progress_thread(run_progress_monitor, "counting ROOT files", spec.files.size(), std::ref(progress));

  auto worker = [&]() {
    long long local_entries = 0;
    int local_added = 0;
    std::string local_tree;
    bool local_mixed = false;

    while (true) {
      const std::size_t i = next.fetch_add(1);
      if (i >= spec.files.size()) {
        break;
      }
      const FileScanResult file_result = count_file_entries(spec.files[i], trees);
      local_entries += file_result.entries;
      local_added += file_result.added;
      progress.done.fetch_add(1);
      if (!file_result.tree.empty()) {
        if (local_tree.empty()) {
          local_tree = file_result.tree;
        } else if (local_tree != file_result.tree) {
          local_mixed = true;
        }
      }
    }

    std::lock_guard<std::mutex> lock(merge_mutex);
    total.entries += local_entries;
    total.added += local_added;
    if (!local_tree.empty()) {
      if (total.tree.empty()) {
        total.tree = local_tree;
      } else if (total.tree != local_tree) {
        mixed_tree = true;
      }
    }
    if (local_mixed) {
      mixed_tree = true;
    }
  };

  std::vector<std::thread> workers;
  workers.reserve(static_cast<std::size_t>(n_threads));
  for (int i = 0; i < n_threads; ++i) {
    workers.emplace_back(worker);
  }
  for (std::thread& t : workers) {
    t.join();
  }
  progress.stop.store(true);
  progress_thread.join();

  if (mixed_tree) {
    total.tree = "mixed";
  }
  if (total.tree.empty()) {
    total.tree = trees.front();
  }
  return total;
}

std::map<std::string, CountResult> count_groups_batch(
    const std::vector<std::pair<std::string, CountSpec>>& groups,
    int requested_threads) {
  std::map<std::string, CountResult> out;
  if (groups.empty()) {
    return out;
  }

  ROOT::EnableThreadSafety();
  std::vector<FileTask> tasks;
  tasks.reserve(groups.size() * 2);
  std::unordered_map<std::string, std::string> default_tree_by_group;
  default_tree_by_group.reserve(groups.size());

  for (std::size_t i = 0; i < groups.size(); ++i) {
    const auto& [group_id, spec] = groups[i];
    const std::string default_tree = spec.trees.empty() ? "rootTupleTree/tree" : spec.trees.front();
    default_tree_by_group[group_id] = default_tree;
    for (const std::string& file_path : spec.files) {
      tasks.push_back(FileTask{i, group_id, file_path});
    }
    if (spec.files.empty()) {
      out[group_id] = CountResult{};
      out[group_id].tree = default_tree;
    }
  }

  if (tasks.empty()) {
    return out;
  }

  const int n_threads = effective_threads(requested_threads, tasks.size());
  std::atomic<std::size_t> next{0};
  std::mutex out_mutex;
  ProgressState progress;
  std::thread progress_thread(run_progress_monitor, "checking ROOT files", tasks.size(), std::ref(progress));

  auto worker = [&]() {
    std::unordered_map<std::string, GroupAccumulator> local;
    while (true) {
      const std::size_t i = next.fetch_add(1);
      if (i >= tasks.size()) {
        break;
      }
      const FileTask& task = tasks[i];
      GroupAccumulator& acc = local[task.group_id];
      try {
        const auto& spec = groups[task.group_index].second;
        const FileScanResult result = count_file_entries(task.file_path, spec.trees);
        acc.entries += result.entries;
        acc.added += result.added;
        if (!result.error.empty() && acc.error.empty()) {
          acc.error = result.error;
        }
        if (!result.tree.empty()) {
          if (acc.tree.empty()) {
            acc.tree = result.tree;
          } else if (acc.tree != result.tree) {
            acc.mixed_tree = true;
          }
        }
      } catch (const std::exception& ex) {
        if (acc.error.empty()) {
          acc.error = ex.what();
        }
      }
      progress.done.fetch_add(1);
    }

    std::lock_guard<std::mutex> lock(out_mutex);
    for (const auto& item : local) {
      CountResult& dest = out[item.first];
      dest.entries += item.second.entries;
      dest.added += item.second.added;
      if (dest.tree.empty()) {
        dest.tree = item.second.tree;
      } else if (!item.second.tree.empty() && dest.tree != item.second.tree) {
        dest.tree = "mixed";
      }
      if (item.second.mixed_tree) {
        dest.tree = "mixed";
      }
      if (dest.error.empty() && !item.second.error.empty()) {
        dest.error = item.second.error;
      }
    }
  };

  std::vector<std::thread> workers;
  workers.reserve(static_cast<std::size_t>(n_threads));
  for (int i = 0; i < n_threads; ++i) {
    workers.emplace_back(worker);
  }
  for (std::thread& t : workers) {
    t.join();
  }
  progress.stop.store(true);
  progress_thread.join();

  for (const auto& [group_id, default_tree] : default_tree_by_group) {
    CountResult& result = out[group_id];
    if (result.tree.empty()) {
      result.tree = default_tree;
    }
  }
  return out;
}

long long query_dataset_expected_events(const std::string& dataset) {
  const std::string cmd =
      "dasgoclient -query " + shell_quote("summary dataset=" + dataset) + " -json";
  const CommandResult result = run_command_capture(cmd);
  if (result.exit_code != 0) {
    throw std::runtime_error("dasgoclient failed for dataset=" + dataset + ": " + trim(result.output));
  }

  static const std::regex re("\"(?:num_event|nevents|num_events)\"\\s*:\\s*([0-9]+)");
  std::smatch match;
  if (!std::regex_search(result.output, match, re)) {
    throw std::runtime_error("Could not find expected event count in DAS response.");
  }
  return std::stoll(match[1].str());
}

int report_total_events(const std::vector<fs::path>& root_files, const std::string& dataset, int threads) {
  if (root_files.empty()) {
    std::cout << kWarnRed << " No ROOT files found for event counting.\n";
    return 4;
  }

  CountSpec spec;
  spec.trees = {"rootTupleTree/tree", "tree"};
  for (const fs::path& p : root_files) {
    spec.files.push_back(p.string());
  }

  try {
    const auto t0 = std::chrono::steady_clock::now();
    const CountResult result = count_files_parallel(spec, threads);
    const auto t1 = std::chrono::steady_clock::now();
    const double dt = std::chrono::duration<double>(t1 - t0).count();
    std::cout << kInfo << " event tree      : " << result.tree << "\n";
    std::cout << kInfo << " files in chain : " << format_with_dots(result.added) << "\n";
    std::cout << kInfo << " counting time  : " << dt << "s\n";
    if (!dataset.empty()) {
      const long long expected = query_dataset_expected_events(dataset);
      std::cout << kInfo << " dataset         : " << dataset << "\n";
      std::cout << kInfo << " expected events : " << format_with_dots(expected) << "\n";
      std::cout << kInfo << " counted events  : " << format_with_dots(result.entries) << "\n";
      std::cout << kInfo << " difference      : " << format_with_dots(expected - result.entries) << "\n";
    } else {
      std::cout << kInfo << " counted events  : " << format_with_dots(result.entries) << "\n";
    }
  } catch (const std::exception& ex) {
    std::cerr << kError << " failed to sum events: " << ex.what() << "\n";
    return 3;
  }
  return 0;
}

fs::path write_missing_list(
    const fs::path& cjobs_dir,
    const std::vector<std::string>& missing_roots,
    const std::vector<fs::path>& missing_jdls) {
  const fs::path out_path = cjobs_dir / "missing_outputs.txt";
  std::ostringstream ss;
  for (std::size_t i = 0; i < missing_roots.size() && i < missing_jdls.size(); ++i) {
    ss << missing_roots[i] << '\t' << missing_jdls[i].filename().string() << '\n';
  }
  write_text(out_path, ss.str());
  return out_path;
}

fs::path write_problematic_jdl_list(const fs::path& cjobs_dir, const std::vector<fs::path>& jdls) {
  const fs::path out_path = cjobs_dir / "problematic_jobs.txt";
  std::ostringstream ss;
  for (const fs::path& jdl : jdls) {
    ss << jdl.filename().string() << '\n';
  }
  write_text(out_path, ss.str());
  return out_path;
}

fs::path write_submit_problematic_script(const fs::path& cjobs_dir) {
  const fs::path script = cjobs_dir / "submit_problematic_jobs.sh";
  const std::string text =
      "#!/usr/bin/env bash\n"
      "set -euo pipefail\n\n"
      "cd \"$(cd \"$(dirname \"${BASH_SOURCE[0]}\")\" && pwd)\"\n\n"
      "if [[ ! -f problematic_jobs.txt ]]; then\n"
      "  echo \"[ERROR] problematic_jobs.txt not found.\"\n"
      "  exit 1\n"
      "fi\n\n"
      "submitted=0\n"
      "while IFS= read -r jdl; do\n"
      "  [[ -z \"${jdl}\" ]] && continue\n"
      "  if [[ ! -f \"${jdl}\" ]]; then\n"
      "    echo \"[WARN] Missing JDL file: ${jdl}\"\n"
      "    continue\n"
      "  fi\n"
      "  echo \"\\033[1;92m[SUB]\\033[0m condor_submit -terse ${jdl}\"\n"
      "  condor_submit -terse \"${jdl}\"\n"
      "  submitted=$((submitted + 1))\n"
      "done < problematic_jobs.txt\n\n"
      "echo \"\\033[1;92m[SUB]\\033[0m submitted jobs: ${submitted}\"\n";
  write_text(script, text);
  fs::permissions(
      script,
      fs::perms::owner_exec | fs::perms::owner_read | fs::perms::owner_write |
          fs::perms::group_exec | fs::perms::group_read |
          fs::perms::others_exec | fs::perms::others_read,
      fs::perm_options::replace);
  return script;
}

void write_per_job_report(const fs::path& cjobs_dir, const std::vector<JobRow>& rows) {
  const fs::path out_path = cjobs_dir / "per_job_event_check.tsv";
  std::ostringstream ss;
  ss << "job_index\tstatus\tinput_files\tinput_events\toutput_events\tinput_tree\toutput_tree\texpected_root\toutput_root\tjdl\tnote\n";
  for (const JobRow& row : rows) {
    ss << row.job_index << '\t'
       << sanitize_tsv(row.status) << '\t'
       << row.input_files << '\t'
       << row.input_events << '\t'
       << row.output_events << '\t'
       << sanitize_tsv(row.input_tree) << '\t'
       << sanitize_tsv(row.output_tree) << '\t'
       << sanitize_tsv(row.expected_root) << '\t'
       << sanitize_tsv(row.output_root) << '\t'
       << sanitize_tsv(row.jdl) << '\t'
       << sanitize_tsv(row.note) << '\n';
  }
  write_text(out_path, ss.str());
}

bool update_jdl_request_memory(const fs::path& jdl_path, int memory_mb) {
  std::vector<std::string> lines = read_lines(jdl_path);
  static const std::regex request_re("^\\s*request_memory\\s*=", std::regex::icase);
  static const std::regex queue_re("^\\s*queue\\b", std::regex::icase);
  const std::string new_line = "request_memory = " + std::to_string(memory_mb);

  bool changed = false;
  bool found = false;
  std::vector<std::string> out;
  out.reserve(lines.size() + 1);

  for (const std::string& line : lines) {
    if (std::regex_search(line, request_re)) {
      found = true;
      if (trim(line) != new_line) {
        out.push_back(new_line);
        changed = true;
      } else {
        out.push_back(line);
      }
    } else {
      out.push_back(line);
    }
  }

  if (!found) {
    auto it = std::find_if(out.begin(), out.end(), [&](const std::string& line) { return std::regex_search(line, queue_re); });
    if (it == out.end()) {
      out.push_back(new_line);
    } else {
      out.insert(it, new_line);
    }
    changed = true;
  }

  if (changed) {
    std::ostringstream ss;
    for (const std::string& line : out) {
      ss << line << '\n';
    }
    write_text(jdl_path, ss.str());
  }
  return changed;
}

void update_missing_jdls_request_memory(const std::vector<fs::path>& jdls, int memory_mb) {
  if (memory_mb <= 0) {
    return;
  }
  int updated = 0;
  for (const fs::path& jdl : jdls) {
    try {
      if (update_jdl_request_memory(jdl, memory_mb)) {
        ++updated;
      }
    } catch (const std::exception& ex) {
      std::cout << kWarnRed << " Failed to update request_memory for " << jdl.filename().string() << ": " << ex.what()
                << "\n";
    }
  }
  std::cout << kInfo << " request_memory  : " << memory_mb << " MB\n";
  std::cout << kInfo << " jdl updated     : " << updated << "/" << jdls.size() << "\n";
}

void resubmit_missing(const fs::path& cjobs_dir, const std::vector<fs::path>& jdls) {
  if (jdls.empty()) {
    std::cout << kInfo << " No missing jobs to resubmit.\n";
    return;
  }
  std::cout << kInfo << " Resubmitting " << jdls.size() << " missing job(s)...\n";
  for (const fs::path& jdl : jdls) {
    const std::string cmd =
        "cd " + shell_quote(cjobs_dir.string()) + " && condor_submit -terse " + shell_quote(jdl.filename().string());
    const CommandResult result = run_command_capture(cmd);
    if (result.exit_code != 0) {
      std::cout << kError << " Failed to resubmit " << jdl.filename().string() << "\n";
      const std::string text = trim(result.output);
      if (!text.empty()) {
        std::cout << text << "\n";
      }
      continue;
    }
    const std::string text = trim(result.output);
    if (!text.empty()) {
      std::cout << kSubGreen << " " << jdl.filename().string() << " -> "
                << text.substr(text.find_last_of('\n') + 1) << "\n";
    } else {
      std::cout << kSubGreen << " " << jdl.filename().string() << " submitted\n";
    }
  }
}

std::vector<SelectedJob> select_jobs(
    const std::vector<std::vector<std::string>>& chunks,
    const std::map<int, fs::path>& jdl_by_index,
    const std::map<int, std::string>& expected_root_by_index,
    const ExistingByName& existing_by_name,
    const ExistingByIndex& existing_by_index,
    const Options& opt,
    JobSummary& summary) {
  std::vector<SelectedJob> jobs;
  for (int idx = 0; idx < static_cast<int>(chunks.size()); ++idx) {
    if (!in_job_range(idx, opt.job_start, opt.job_end)) {
      continue;
    }
    ++summary.checked_jobs;

    SelectedJob job;
    job.index = idx;
    job.input_files = chunks[static_cast<std::size_t>(idx)];
    auto jdl_it = jdl_by_index.find(idx);
    if (jdl_it != jdl_by_index.end()) {
      job.jdl = jdl_it->second;
      job.job_label = jdl_it->second.filename().string();
    } else {
      job.job_label = "n" + std::to_string(idx);
    }
    auto root_it = expected_root_by_index.find(idx);
    if (root_it != expected_root_by_index.end()) {
      job.expected_root = root_it->second;
      auto existing_name_it = existing_by_name.find(root_it->second);
      if (existing_name_it != existing_by_name.end() && !existing_name_it->second.empty()) {
        job.output_path = existing_name_it->second.front();
      }
    }
    if (!job.output_path) {
      auto existing_idx_it = existing_by_index.find(idx);
      if (existing_idx_it != existing_by_index.end() && !existing_idx_it->second.empty()) {
        job.output_path = existing_idx_it->second.front();
      }
    }

    jobs.push_back(std::move(job));
  }
  return jobs;
}

std::tuple<std::vector<JobRow>, std::set<fs::path>, JobSummary> run_per_job_event_check(
    const std::vector<std::vector<std::string>>& chunks,
    const std::map<int, fs::path>& jdl_by_index,
    const std::map<int, std::string>& expected_root_by_index,
    const ExistingByName& existing_by_name,
    const ExistingByIndex& existing_by_index,
    const Options& opt,
    const std::optional<std::unordered_map<std::string, long long>>& das_file_event_map) {
  JobSummary summary;
  std::vector<JobRow> rows;
  std::set<fs::path> problematic;

  std::vector<SelectedJob> jobs =
      select_jobs(chunks, jdl_by_index, expected_root_by_index, existing_by_name, existing_by_index, opt, summary);

  std::vector<std::pair<std::string, CountSpec>> batch_specs;
  batch_specs.reserve(jobs.size() * 2);
  std::unordered_map<int, DasInputPlan> das_input_plans;
  std::size_t total_das_matched_files = 0;
  std::size_t total_das_missing_files = 0;
  for (const SelectedJob& job : jobs) {
    bool need_root_input_count = true;
    if (das_file_event_map) {
      DasInputPlan plan = build_das_input_plan(job.input_files, *das_file_event_map);
      total_das_matched_files += plan.matched_files;
      total_das_missing_files += plan.missing_files.size();
      need_root_input_count = !plan.missing_files.empty();
      das_input_plans[job.index] = std::move(plan);
    }
    if (need_root_input_count) {
      const auto plan_it = das_input_plans.find(job.index);
      if (plan_it != das_input_plans.end() && !plan_it->second.missing_files.empty()) {
        batch_specs.push_back(
            {"in:" + std::to_string(job.index), CountSpec{plan_it->second.missing_files, {opt.input_tree}}});
      } else {
        batch_specs.push_back({"in:" + std::to_string(job.index), CountSpec{job.input_files, {opt.input_tree}}});
      }
    }
    if (job.output_path) {
      batch_specs.push_back({"out:" + std::to_string(job.index),
                             CountSpec{{job.output_path->string()}, {"rootTupleTree/tree", "tree"}}});
    }
  }

  if (das_file_event_map) {
    std::cout << kInfo << " input source    : DAS + ROOT fallback\n";
    std::cout << kInfo << " das matched files: " << format_with_dots(static_cast<long long>(total_das_matched_files))
              << "\n";
    std::cout << kInfo << " das missing files: " << format_with_dots(static_cast<long long>(total_das_missing_files))
              << "\n";
  } else {
    std::cout << kInfo << " input source    : ROOT only\n";
  }

  const std::map<std::string, CountResult> batch_results = count_groups_batch(batch_specs, opt.threads);

  for (const SelectedJob& job : jobs) {
    JobRow row;
    row.job_index = job.index;
    row.status = "OK";
    row.input_files = job.input_files.size();
    row.input_tree = opt.input_tree;
    row.output_tree = "-";
    row.expected_root = job.expected_root.empty() ? "-" : job.expected_root;
    row.output_root = job.output_path ? job.output_path->string() : "-";
    row.jdl = job.jdl ? job.jdl->filename().string() : "-";

    const auto das_plan_it = das_input_plans.find(job.index);
    if (das_plan_it != das_input_plans.end()) {
      row.input_events = das_plan_it->second.known_events;
      if (das_plan_it->second.missing_files.empty()) {
        row.input_tree = "das";
      }
    }

    bool input_count_ok = true;
    const auto in_it = batch_results.find("in:" + std::to_string(job.index));
    if (das_plan_it != das_input_plans.end() && das_plan_it->second.missing_files.empty()) {
      input_count_ok = true;
    } else if (in_it == batch_results.end() || !in_it->second.error.empty()) {
      input_count_ok = false;
      row.status = "COUNT_ERROR";
      row.note = in_it == batch_results.end() ? "missing batched input result" : in_it->second.error;
    } else {
      row.input_events += in_it->second.entries;
      if (das_plan_it != das_input_plans.end() && das_plan_it->second.matched_files > 0) {
        row.input_tree = "das+" + in_it->second.tree;
      } else {
        row.input_tree = in_it->second.tree;
      }
    }
    if (input_count_ok) {
      summary.total_input_events += row.input_events;
    }

    if (!job.output_path) {
      if (row.status == "OK") {
        row.status = "MISSING_OUTPUT";
        row.note = "no output root file found for this job index";
      } else {
        row.note += "; no output root file found for this job index";
      }
    } else {
      const auto out_it = batch_results.find("out:" + std::to_string(job.index));
      if (out_it == batch_results.end() || !out_it->second.error.empty()) {
        if (row.status == "OK") {
          row.status = "COUNT_ERROR";
          row.note = out_it == batch_results.end() ? "missing batched output result" : out_it->second.error;
        } else {
          row.note += "; " + (out_it == batch_results.end() ? std::string("missing batched output result")
                                                            : out_it->second.error);
        }
      } else {
        row.output_events = out_it->second.entries;
        row.output_tree = out_it->second.tree;
        summary.total_output_events += row.output_events;
      }
    }

    if (row.status == "OK") {
      if (row.input_events != row.output_events) {
        row.status = "EVENT_MISMATCH";
        row.note = "input_events != output_events";
      } else {
        ++summary.ok_jobs;
      }
    }

    if (row.status == "MISSING_OUTPUT") {
      ++summary.missing_jobs;
    } else if (row.status == "EVENT_MISMATCH") {
      ++summary.mismatch_jobs;
    } else if (row.status == "COUNT_ERROR") {
      ++summary.error_jobs;
    }

    if (row.status != "OK" && job.jdl) {
      problematic.insert(*job.jdl);
    }

    const std::string line =
        "job result     : " + job.job_label + " | " + row.status + " | in=" + format_with_dots(row.input_events) +
        " out=" + format_with_dots(row.output_events);
    if (row.status == "OK") {
      if (opt.verbose) {
        std::cout << kInfo << " " << line << "\n";
      }
    } else {
      std::cout << kWarnRed << " " << line << "\n";
    }
    rows.push_back(std::move(row));
  }

  return {rows, problematic, summary};
}

int main_impl(const Options& opt, const fs::path& exe_path) {
  const std::optional<std::string> range_err = validate_job_range(opt);
  if (range_err) {
    std::cerr << kError << " " << *range_err << "\n";
    return 2;
  }

  const fs::path path1 = absolute_path(fs::path(opt.path1));
  const bool single_path_mode = trim(opt.path2).empty();

  if (single_path_mode) {
    if (!fs::exists(path1)) {
      std::cerr << kError << " path not found: " << path1 << "\n";
      return 2;
    }
    if (opt.resubmit) {
      std::cout << kWarnRed << " --resubmit is ignored in single-path mode.\n";
    }
    if (opt.request_memory_mb > 0) {
      std::cout << kWarnRed << " --request-memory-mb is ignored in single-path mode.\n";
    }
    if (opt.no_per_file) {
      std::cout << kWarnRed << " --noPerFile is redundant in single-path mode (already total-only).\n";
    }
    if (opt.job_start || opt.job_end) {
      std::cout << kWarnRed << " --job-start/--job-end are ignored in single-path mode.\n";
    }

    if (fs::is_regular_file(path1)) {
      std::cout << kInfo << " input mode      : single ROOT file\n";
      std::cout << kInfo << " root file       : " << path1 << "\n";
      return report_total_events({path1}, opt.dataset, opt.threads);
    }

    bool recursive_used = opt.check_subdirs;
    std::vector<fs::path> roots = get_existing_root_files(path1, recursive_used);
    if (roots.empty() && !recursive_used) {
      roots = get_existing_root_files(path1, true);
      recursive_used = true;
      if (!roots.empty()) {
        std::cout << kInfo << " no top-level ROOT files found, switching to recursive scan.\n";
      }
    }
    std::cout << kInfo << " input mode      : directory\n";
    std::cout << kInfo << " directory       : " << path1 << "\n";
    std::cout << kInfo << " check subdirs   : " << (recursive_used ? "yes" : "no") << "\n";
    return report_total_events(roots, opt.dataset, opt.threads);
  }

  const fs::path cjobs_dir = path1;
  const fs::path eos_dir = absolute_path(fs::path(opt.path2));
  if (!fs::exists(cjobs_dir) || !fs::is_directory(cjobs_dir)) {
    std::cerr << kError << " cjobs directory not found: " << cjobs_dir << "\n";
    return 2;
  }
  if (!fs::exists(eos_dir) || !fs::is_directory(eos_dir)) {
    std::cerr << kError << " eos output directory not found: " << eos_dir << "\n";
    return 2;
  }

  if (opt.no_per_file) {
    if (opt.resubmit) {
      std::cout << kWarnRed << " --resubmit is ignored with --noPerFile.\n";
    }
    if (opt.request_memory_mb > 0) {
      std::cout << kWarnRed << " --request-memory-mb is ignored with --noPerFile.\n";
    }
    if (!opt.config.empty()) {
      std::cout << kWarnRed << " --config is ignored with --noPerFile.\n";
    }
    if (opt.job_start || opt.job_end) {
      std::cout << kWarnRed << " --job-start/--job-end are ignored with --noPerFile.\n";
    }

    const std::vector<fs::path> roots = get_existing_root_files(eos_dir, opt.check_subdirs);
    std::cout << kInfo << " cjobs directory : " << cjobs_dir << "\n";
    std::cout << kInfo << " eos directory   : " << eos_dir << "\n";
    std::cout << kInfo << " noPerFile mode  : yes\n";
    std::cout << kInfo << " check subdirs   : " << (opt.check_subdirs ? "yes" : "no") << "\n";
    return report_total_events(roots, opt.dataset, opt.threads);
  }

  const bool has_config = !opt.config.empty();
  if (has_config && opt.total_events) {
    std::cout << kInfo
              << " --total-events is redundant with --config; using per-job total summary and skipping extra scan.\n";
  }
  if (!opt.dataset.empty() && !opt.total_events && !has_config) {
    std::cout << kInfo << " --dataset provided; enabling --total-events.\n";
  }

  const bool total_events = !has_config && (opt.total_events || !opt.dataset.empty());
  const bool allow_resubmit = opt.resubmit;
  if (opt.request_memory_mb > 0 && !opt.resubmit) {
    std::cout << kWarnRed << " --request-memory-mb is set but --resubmit is not enabled. No .jdl will be modified.\n";
  }

  const std::vector<fs::path> jdl_files = get_jdl_files(cjobs_dir);
  std::cout << kInfo << " cjobs directory : " << cjobs_dir << "\n";
  std::cout << kInfo << " eos directory   : " << eos_dir << "\n";
  std::cout << kInfo << " .jdl count      : " << jdl_files.size() << "\n";
  std::cout << kInfo << " job range       : start="
            << (opt.job_start ? std::to_string(*opt.job_start) : "begin")
            << " end=" << (opt.job_end ? std::to_string(*opt.job_end) : "end") << "\n";
  if (!jdl_files.empty()) {
    std::cout << kInfo << " threads         : "
              << effective_threads(opt.threads, std::max<std::size_t>(1, jdl_files.size())) << "\n";
  }

  if (jdl_files.empty()) {
    std::cout << kWarnRed << " No .jdl files found. Nothing to check.\n";
    return 0;
  }

  auto [expected_map_raw, unresolved_jdls] = build_expected_map(cjobs_dir, jdl_files);
  auto [expected_map, skipped_unindexed] =
      filter_expected_map_by_range(expected_map_raw, opt.job_start, opt.job_end);

  if (!unresolved_jdls.empty()) {
    std::cout << kWarnRed << " Could not resolve expected output name for " << unresolved_jdls.size()
              << " .jdl file(s).\n";
    const std::size_t limit = std::min<std::size_t>(10, unresolved_jdls.size());
    for (std::size_t i = 0; i < limit; ++i) {
      std::cout << "  - " << unresolved_jdls[i].filename().string() << "\n";
    }
    if (unresolved_jdls.size() > limit) {
      std::cout << "  ... and " << (unresolved_jdls.size() - limit) << " more\n";
    }
  }
  if (skipped_unindexed > 0) {
    std::cout << kWarnRed << " skipped unresolved-index outputs due to job range filtering: " << skipped_unindexed
              << "\n";
  }

  const std::vector<fs::path> existing_root_files = get_existing_root_files(eos_dir, opt.check_subdirs);
  std::cout << kInfo << " check subdirs   : " << (opt.check_subdirs ? "yes" : "no") << "\n";
  std::vector<std::string> existing_root_names;
  existing_root_names.reserve(existing_root_files.size());
  for (const fs::path& p : existing_root_files) {
    existing_root_names.push_back(p.filename().string());
  }
  std::sort(existing_root_names.begin(), existing_root_names.end());
  existing_root_names.erase(std::unique(existing_root_names.begin(), existing_root_names.end()), existing_root_names.end());
  std::cout << kInfo << " existing roots  : " << existing_root_names.size() << "\n";

  std::vector<std::string> expected_roots;
  for (const auto& item : expected_map) {
    expected_roots.push_back(item.first);
  }

  auto [missing_roots, fallback_matches] = resolve_missing_with_fallback(expected_roots, existing_root_names);
  if (!fallback_matches.empty()) {
    std::cout << kInfo << " fallback match  : " << fallback_matches.size() << "\n";
  }

  std::vector<fs::path> missing_jdls;
  missing_jdls.reserve(missing_roots.size());
  for (const std::string& name : missing_roots) {
    missing_jdls.push_back(expected_map.at(name));
  }

  if (!missing_roots.empty()) {
    std::cout << kWarnRed << " missing roots   : " << missing_roots.size() << "\n";
    for (const std::string& name : missing_roots) {
      std::cout << kWarnRed << " " << name << "\n";
    }
  } else {
    std::cout << kInfo << " missing roots   : 0\n";
  }

  const fs::path missing_list = write_missing_list(cjobs_dir, missing_roots, missing_jdls);
  std::cout << kInfo << " missing list file: " << missing_list << "\n";

  std::set<fs::path> problematic_jdls(missing_jdls.begin(), missing_jdls.end());
  std::string dataset_for_total = opt.dataset;

  if (!opt.config.empty()) {
    const fs::path config_path = absolute_path(fs::path(opt.config));
    if (!fs::exists(config_path)) {
      std::cerr << kError << " config file not found: " << config_path << "\n";
      return 2;
    }

    const fs::path script_dir = absolute_path(exe_path).parent_path();
    const fs::path repo_root = script_dir.parent_path();
    LoadedChunks loaded;
    try {
      loaded = load_job_chunks_from_config(config_path, repo_root);
    } catch (const std::exception& ex) {
      std::cerr << kError << " failed to load config/job chunks: " << ex.what() << "\n";
      return 2;
    }

    std::cout << kInfo << " config file     : " << config_path << "\n";
    std::cout << kInfo << " input list      : " << loaded.input_list_path << "\n";
    std::cout << kInfo << " chunk interval  : " << loaded.interval << "\n";
    std::cout << kInfo << " jobs from config: " << loaded.chunks.size() << "\n";
    if (opt.job_start || opt.job_end) {
      const int start = opt.job_start ? *opt.job_start : 0;
      const int end = opt.job_end ? *opt.job_end : static_cast<int>(loaded.chunks.size()) - 1;
      std::cout << kInfo << " effective range : " << start << ".." << end << " (inclusive)\n";
    }
    std::cout << kInfo << " input tree      : " << opt.input_tree << "\n";
    const std::string input_dataset = !opt.dataset.empty() ? opt.dataset : loaded.dataset;
    if (!input_dataset.empty()) {
      std::cout << kInfo << " dataset         : " << input_dataset << "\n";
      if (dataset_for_total.empty()) {
        dataset_for_total = input_dataset;
      }
    }
    std::optional<std::unordered_map<std::string, long long>> das_file_event_map;
    if (!input_dataset.empty()) {
      bool from_cache = false;
      try {
        auto event_map = get_dataset_file_event_map(cjobs_dir, input_dataset, from_cache);
        std::cout << kInfo << " das file cache  : "
                  << dataset_event_cache_path(cjobs_dir, input_dataset) << "\n";
        std::cout << kInfo << " das file map    : " << format_with_dots(static_cast<long long>(event_map.size()))
                  << " files"
                  << (from_cache ? " (cache)" : " (fresh query)") << "\n";
        das_file_event_map = std::move(event_map);
      } catch (const std::exception& ex) {
        std::cout << kWarnRed
                  << " DAS input lookup unavailable, falling back to ROOT for unresolved input files: "
                  << ex.what() << "\n";
      }
    } else {
      std::cout << kWarnRed << " No dataset available for DAS input lookup. Falling back to ROOT for input files.\n";
    }

    std::map<int, fs::path> jdl_by_index;
    for (const fs::path& jdl : jdl_files) {
      const auto idx = extract_job_index_from_jdl(jdl.filename().string());
      if (idx) {
        jdl_by_index[*idx] = jdl;
      }
    }

    std::map<int, std::string> expected_root_by_index;
    for (const auto& item : expected_map) {
      const auto idx = extract_job_index_from_root(item.first);
      if (idx) {
        expected_root_by_index[*idx] = item.first;
      }
    }

    const auto [existing_by_name, existing_by_index] = build_existing_root_maps(existing_root_files);
    auto [rows, problematic_from_events, summary] =
        run_per_job_event_check(
            loaded.chunks,
            jdl_by_index,
            expected_root_by_index,
            existing_by_name,
            existing_by_index,
            opt,
            das_file_event_map);
    problematic_jdls.insert(problematic_from_events.begin(), problematic_from_events.end());

    write_per_job_report(cjobs_dir, rows);
    std::cout << kInfo << " per-job report  : " << (cjobs_dir / "per_job_event_check.tsv") << "\n";

    std::vector<JobRow> problematic_rows;
    for (const JobRow& row : rows) {
      if (row.status != "OK") {
        problematic_rows.push_back(row);
      }
    }

    if (summary.checked_jobs == 0) {
      std::cout << kWarnRed << " per-job check  : no jobs selected in the requested range.\n";
    } else if (!problematic_rows.empty()) {
      std::cout << kWarnRed << " problematic jobs: " << problematic_rows.size() << "\n";
      if (opt.verbose) {
        const std::size_t limit = std::min<std::size_t>(50, problematic_rows.size());
        for (std::size_t i = 0; i < limit; ++i) {
          const JobRow& row = problematic_rows[i];
          std::cout << kWarnRed << " n" << row.job_index << ": " << row.status
                    << " | in=" << row.input_events << " out=" << row.output_events
                    << " | jdl=" << row.jdl << " | out=" << row.output_root << "\n";
        }
        if (problematic_rows.size() > limit) {
          std::cout << kWarnRed << " ... and " << (problematic_rows.size() - limit) << " more\n";
        }
      }
    } else {
      std::cout << kInfo << " per-job check  : all jobs matched input/output event counts.\n";
    }

    std::cout << kInfo << " per-job summary : checked=" << summary.checked_jobs
              << ", ok=" << summary.ok_jobs
              << ", missing=" << summary.missing_jobs
              << ", mismatch=" << summary.mismatch_jobs
              << ", errors=" << summary.error_jobs << "\n";
    std::cout << kInfo << " total input evts: " << format_with_dots(summary.total_input_events) << "\n";
    std::cout << kInfo << " total output evts: " << format_with_dots(summary.total_output_events) << "\n";
    std::cout << kInfo << " total difference: "
              << format_with_dots(summary.total_input_events - summary.total_output_events) << "\n";

    std::set<int> extra_output_indices;
    for (const auto& item : existing_by_index) {
      if (item.first >= static_cast<int>(loaded.chunks.size())) {
        extra_output_indices.insert(item.first);
      }
    }
    if (!extra_output_indices.empty()) {
      std::ostringstream shown;
      int count = 0;
      for (int idx : extra_output_indices) {
        if (count > 0) {
          shown << ", ";
        }
        shown << idx;
        ++count;
        if (count == 10) {
          break;
        }
      }
      std::cout << kWarnRed << " extra output job indices not present in config chunking: " << shown.str() << "\n";
      if (extra_output_indices.size() > 10) {
        std::cout << kWarnRed << " ... and " << (extra_output_indices.size() - 10) << " more\n";
      }
    }
  }

  std::vector<fs::path> problematic_vec(problematic_jdls.begin(), problematic_jdls.end());
  const fs::path problematic_list = write_problematic_jdl_list(cjobs_dir, problematic_vec);
  const fs::path submit_script = write_submit_problematic_script(cjobs_dir);
  if (!problematic_vec.empty()) {
    std::cout << kWarnRed << " problematic list: " << problematic_list << "\n";
    std::cout << kWarnRed << " problematic jdls: " << problematic_vec.size() << "\n";
  } else {
    std::cout << kInfo << " problematic list: " << problematic_list << "\n";
    std::cout << kInfo << " problematic jdls: 0\n";
  }
  std::cout << kInfo << " submit script   : " << submit_script << "\n";

  if (total_events) {
    const int rc = report_total_events(existing_root_files, dataset_for_total, opt.threads);
    if (rc != 0) {
      return rc;
    }
  }

  if (allow_resubmit) {
    if (opt.request_memory_mb > 0) {
      update_missing_jdls_request_memory(problematic_vec, opt.request_memory_mb);
    }
    resubmit_missing(cjobs_dir, problematic_vec);
  }

  return 0;
}

}  // namespace

int main(int argc, char** argv) {
  try {
    gROOT->SetBatch(kTRUE);
    const Options opt = parse_args(argc, argv);
    return main_impl(opt, fs::path(argv[0]));
  } catch (const std::exception& ex) {
    std::cerr << kError << " " << ex.what() << "\n";
    return 2;
  }
}

