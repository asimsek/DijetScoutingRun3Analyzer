#ifndef DIJET_SCOUTING_EOS_UTILS_H
#define DIJET_SCOUTING_EOS_UTILS_H

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fnmatch.h>
#include <functional>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/wait.h>
#include <vector>

namespace eos_utils {

inline bool starts_with(const std::string &value, const std::string &prefix) {
  return value.compare(0, prefix.size(), prefix) == 0;
}

inline std::string trim(const std::string &value) {
  const auto first = value.find_first_not_of(" \t\r\n");
  return first == std::string::npos ? "" : value.substr(first, value.find_last_not_of(" \t\r\n") - first + 1);
}

inline std::string shell_quote(const std::string &value) {
  std::string quoted = "'";
  for (char ch : value)
    quoted += ch == '\'' ? "'\"'\"'" : std::string(1, ch);
  return quoted + "'";
}

struct XRootDPath {
  std::string endpoint;
  std::string path;
  std::string url() const { return endpoint + "/" + path; }
};

inline std::optional<XRootDPath> parse_xrootd_path(std::string value) {
  if (starts_with(value, "/eos/uscms/store/"))
    value = "root://cmseos.fnal.gov/" + value.substr(10);
  else if (starts_with(value, "/eos/cms/store/"))
    value = "root://eoscms.cern.ch/" + value.substr(8);
  else if (starts_with(value, "/store/"))
    value = "root://cmseos.fnal.gov/" + value;
  if (!starts_with(value, "root://"))
    return std::nullopt;
  const auto slash = value.find('/', 7);
  if (slash == std::string::npos || slash == 7)
    throw std::runtime_error("expected root://HOST//PATH: " + value);
  std::string path = value.substr(slash);
  const auto first = path.find_first_not_of('/');
  path = first == std::string::npos ? "/" : "/" + path.substr(first);
  while (path.size() > 1 && path.back() == '/')
    path.pop_back();
  return XRootDPath{value.substr(0, slash), path};
}

inline std::string normalize_storage_path(const std::string &value) {
  if (const auto remote = parse_xrootd_path(value))
    return remote->url();
  if (value.find("://") != std::string::npos)
    return value;
  return std::filesystem::absolute(value).lexically_normal().string();
}

inline std::string storage_filename(const std::string &value) {
  const auto slash = value.find_last_of('/');
  return slash == std::string::npos ? value : value.substr(slash + 1);
}

inline std::string run_xrdfs(const XRootDPath &remote, const std::string &operation) {
  const std::string command =
      "xrdfs " + shell_quote(remote.endpoint) + " " + operation + " " + shell_quote(remote.path) + " 2>&1";
  FILE *pipe = popen(command.c_str(), "r");
  if (!pipe)
    throw std::runtime_error("cannot run xrdfs for " + remote.url());
  std::string output;
  char buffer[4096];
  while (std::fgets(buffer, sizeof(buffer), pipe))
    output += buffer;
  const int status = pclose(pipe);
  if (status == -1 || !WIFEXITED(status) || WEXITSTATUS(status) != 0 || output.find("[ERROR]") != std::string::npos) {
    throw std::runtime_error("XRootD " + operation + " failed for " + remote.url() + ": " + trim(output) +
                             "\nCheck xrdfs availability, your active CMS VOMS proxy, and storage permissions.");
  }
  return output;
}

inline bool storage_is_directory(const std::string &value) {
  if (const auto remote = parse_xrootd_path(value)) {
    std::istringstream lines(run_xrdfs(*remote, "stat"));
    std::string line;
    while (std::getline(lines, line)) {
      if (starts_with(trim(line), "Flags:"))
        return line.find("IsDir") != std::string::npos;
    }
    throw std::runtime_error("could not determine XRootD file type: " + remote->url());
  }
  return std::filesystem::is_directory(value);
}

inline std::vector<std::string> remote_entries(const XRootDPath &directory, bool recursive) {
  std::istringstream lines(run_xrdfs(directory, recursive ? "ls -u -R" : "ls -u"));
  std::vector<std::string> entries;
  std::string line;
  while (std::getline(lines, line)) {
    line = trim(line);
    if (line.empty())
      continue;
    if (!starts_with(line, "root://"))
      throw std::runtime_error("unexpected XRootD listing output for " + directory.url() + ": " + line);
    entries.push_back(normalize_storage_path(line));
  }
  std::sort(entries.begin(), entries.end());
  entries.erase(std::unique(entries.begin(), entries.end()), entries.end());
  return entries;
}

inline std::vector<std::string> list_root_files(const std::string &directory, bool recursive,
                                                const std::string &pattern = "*.root") {
  std::vector<std::string> files;
  if (const auto remote = parse_xrootd_path(directory)) {
    for (const auto &entry : remote_entries(*remote, recursive)) {
      if (fnmatch(pattern.c_str(), storage_filename(entry).c_str(), 0) == 0)
        files.push_back(entry);
    }
  } else {
    const auto add = [&](const auto &entry) {
      if (entry.is_regular_file() && fnmatch(pattern.c_str(), entry.path().filename().c_str(), 0) == 0)
        files.push_back(normalize_storage_path(entry.path().string()));
    };
    if (recursive)
      for (const auto &entry : std::filesystem::recursive_directory_iterator(directory))
        add(entry);
    else
      for (const auto &entry : std::filesystem::directory_iterator(directory))
        add(entry);
  }
  std::sort(files.begin(), files.end());
  return files;
}

inline bool match_path(const std::string &path, const std::string &pattern) {
  const auto split = [](const std::string &value) {
    std::istringstream input(value);
    std::vector<std::string> parts;
    std::string part;
    while (std::getline(input, part, '/'))
      parts.push_back(part);
    return parts;
  };
  const auto parts = split(path), patterns = split(pattern);
  std::function<bool(std::size_t, std::size_t)> match = [&](std::size_t i, std::size_t j) {
    if (j == patterns.size())
      return i == parts.size();
    if (patterns[j] == "**")
      return match(i, j + 1) || (i < parts.size() && match(i + 1, j));
    return i < parts.size() && fnmatch(patterns[j].c_str(), parts[i].c_str(), 0) == 0 && match(i + 1, j + 1);
  };
  return match(0, 0);
}

inline std::vector<std::string> expand_remote_input(const std::string &value, const std::string &pattern = "*.root") {
  const auto remote = parse_xrootd_path(value);
  if (!remote)
    throw std::runtime_error("expected an EOS path or root:// URL: " + value);
  const auto first = remote->path.find_first_of("*?[");
  if (first != std::string::npos) {
    const auto slash = remote->path.rfind('/', first);
    XRootDPath parent{remote->endpoint, slash == 0 ? "/" : remote->path.substr(0, slash)};
    const bool recursive = remote->path.find('/', slash + 1) != std::string::npos;
    std::vector<std::string> files;
    for (const auto &entry : remote_entries(parent, recursive)) {
      const auto candidate = parse_xrootd_path(entry);
      if (match_path(candidate->path, remote->path) && fnmatch("*.root", storage_filename(entry).c_str(), 0) == 0)
        files.push_back(entry);
    }
    return files;
  }
  if (fnmatch("*.root", remote->path.c_str(), 0) == 0 || !storage_is_directory(remote->url()))
    return {remote->url()};
  return list_root_files(remote->url(), true, pattern);
}

} // namespace eos_utils
#endif
