"""Local and XRootD storage helpers shared by the analysis utilities.

LPC EOS is identified by its path, independent of the machine running the code.
XRootD commands inherit the caller's authentication environment.
"""

from __future__ import annotations

import argparse
from dataclasses import dataclass
import fnmatch
import glob
import json
import os
from pathlib import Path
import posixpath
import shutil
import subprocess
import tempfile


@dataclass(frozen=True)
class RemotePath:
    endpoint: str
    path: str

    @property
    def url(self) -> str:
        return self.endpoint + "/" + self.path


def parse_remote(value: str | Path) -> RemotePath | None:
    value = os.path.expandvars(os.path.expanduser(str(value)))
    for prefix, endpoint in (("/eos/uscms/store/", "root://cmseos.fnal.gov"),
                             ("/eos/cms/store/", "root://eoscms.cern.ch")):
        if value.startswith(prefix):
            value = endpoint + "//store/" + value[len(prefix):]
            break
    if value.startswith("/store/"):
        value = "root://cmseos.fnal.gov/" + value
    if not value.startswith("root://"):
        return None
    authority, slash, path = value[7:].partition("/")
    if not authority or not slash:
        raise ValueError(f"Expected root://HOST//PATH: {value}")
    return RemotePath("root://" + authority, "/" + path.strip("/"))


def normalize_path(value: str | Path) -> str:
    remote = parse_remote(value)
    if remote:
        return remote.url
    value = os.path.expandvars(os.path.expanduser(str(value)))
    if "://" in value:
        return value
    return str(Path(value).resolve())


def _is_node_local_proxy(proxy: Path) -> bool:
    return any(directory == proxy or directory in proxy.parents
               for directory in (Path("/tmp").resolve(), Path("/var/tmp").resolve()))


def _validate_condor_proxy(proxy: Path) -> None:
    if not proxy.is_file() or not os.access(proxy, os.R_OK):
        raise RuntimeError(f"Proxy file does not exist or is unreadable: {proxy}")
    try:
        result = subprocess.run(["voms-proxy-info", "-file", str(proxy), "-exists"],
                                text=True, capture_output=True)
    except OSError as exc:
        raise RuntimeError("voms-proxy-info is required to validate the submission proxy.") from exc
    if result.returncode:
        raise RuntimeError(f"Proxy is invalid or expired: {proxy}. Renew it with voms-proxy-init -voms cms -out PATH.")


def condor_shared_directory(directory: str | Path | None = None) -> Path:
    """Use CERN's work area for staging, or the shared home at other sites."""
    selected = directory or os.environ.get("CONDOR_SHARED_DIR")
    if selected:
        shared = Path(os.path.expandvars(os.path.expanduser(str(selected)))).resolve()
    else:
        home = Path.home().resolve()
        if home.parts[:4] == ("/", "afs", "cern.ch", "user"):
            shared = Path("/afs/cern.ch/work") / home.name[0] / home.name / "private"
        else:
            shared = home / "private"
        shared = shared.resolve()
    if _is_node_local_proxy(shared):
        raise RuntimeError(f"Condor staging must use shared AFS/NFS storage: {shared}.")
    return shared


def _stage_condor_proxy(proxy: Path, shared_dir: str | Path | None = None) -> Path:
    directory = condor_shared_directory(shared_dir)
    destination = directory / "x509up_condor"
    if proxy == destination:
        return proxy
    temporary = None
    try:
        directory.mkdir(mode=0o700, parents=True, exist_ok=True)
        with tempfile.NamedTemporaryFile(mode="wb", prefix=".x509up_condor.",
                                         dir=directory, delete=False) as target:
            temporary = Path(target.name)
            os.fchmod(target.fileno(), 0o600)
            with proxy.open("rb") as source:
                shutil.copyfileobj(source, target)
            target.flush()
            os.fsync(target.fileno())
        # Publish only a complete, valid copy; preserve any previous copy on failure.
        _validate_condor_proxy(temporary)
        os.replace(temporary, destination)
    except OSError as exc:
        raise RuntimeError(f"Cannot copy the submission proxy to {destination}: {exc}. "
                           "Check the shared staging directory access and quota, or use --proxy PATH.") from exc
    finally:
        if temporary is not None:
            temporary.unlink(missing_ok=True)
    return destination


def resolve_condor_proxy(proxy_path: str | None = None, shared_dir: str | Path | None = None) -> str:
    """Resolve an active proxy, staging node-local proxies in the shared home.

    Explicit arguments take priority over X509_USER_PROXY and VOMS discovery.
    Proxies in /tmp or /var/tmp, and CERN home proxies, are copied to x509up_condor
    in the shared staging directory (CERN work/private, or home/private at LPC).
    Existing CERN work and LPC NFS proxy paths are used directly. Copying does not renew
    the proxy, and it is delegated by Condor rather than added to job tarballs.
    """
    selected = proxy_path or os.environ.get("X509_USER_PROXY", "")
    if not selected:
        try:
            result = subprocess.run(["voms-proxy-info", "-path"], text=True, capture_output=True)
        except OSError as exc:
            raise RuntimeError("voms-proxy-info is unavailable; load your CMS environment and create a proxy.") from exc
        if result.returncode or not result.stdout.strip():
            raise RuntimeError("No active proxy found. Create one with voms-proxy-init -voms cms, or pass --proxy PATH.")
        selected = result.stdout.strip()
    proxy = Path(os.path.expandvars(os.path.expanduser(selected))).resolve()
    _validate_condor_proxy(proxy)
    if _is_node_local_proxy(proxy) or proxy.parts[:4] == ("/", "afs", "cern.ch", "user"):
        proxy = _stage_condor_proxy(proxy, shared_dir)
    os.environ["X509_USER_PROXY"] = str(proxy)
    return str(proxy)


def basename(value: str | Path) -> str:
    remote = parse_remote(value)
    return posixpath.basename(remote.path if remote else str(value).rstrip("/"))


def join_path(directory: str | Path, name: str) -> str:
    remote = parse_remote(directory)
    if remote:
        return RemotePath(remote.endpoint, posixpath.join(remote.path, name)).url
    return str(Path(directory) / name)


def _xrdfs(remote: RemotePath, *operation: str) -> str:
    command = ["xrdfs", remote.endpoint, *operation, remote.path]
    try:
        result = subprocess.run(command, text=True, capture_output=True, check=False)
    except OSError as exc:
        raise RuntimeError("xrdfs is required for remote EOS access; load your CMS environment.") from exc
    if result.returncode or "[ERROR]" in result.stderr or "[ERROR]" in result.stdout:
        raise RuntimeError(
            f"XRootD {' '.join(operation)} failed for {remote.url}: "
            f"{result.stderr.strip() or result.stdout.strip()}\n"
            "Check your active CMS VOMS proxy and storage permissions."
        )
    return result.stdout


def is_directory(value: str | Path) -> bool:
    remote = parse_remote(value)
    if remote:
        for line in _xrdfs(remote, "stat").splitlines():
            if line.strip().startswith("Flags:"):
                return "IsDir" in line
        raise RuntimeError(f"Could not determine XRootD file type: {remote.url}")
    return Path(value).is_dir()


def list_entries(directory: str | Path, recursive: bool = False) -> list[str]:
    remote = parse_remote(directory)
    if remote:
        operation = ("ls", "-u", "-R") if recursive else ("ls", "-u")
        entries = []
        for line in _xrdfs(remote, *operation).splitlines():
            line = line.strip()
            if not line:
                continue
            if not line.startswith("root://"):
                raise RuntimeError(f"Unexpected XRootD listing output for {remote.url}: {line}")
            entries.append(normalize_path(line))
        return sorted(set(entries))
    directory = Path(directory)
    if not directory.is_dir():
        raise RuntimeError(f"Directory not found: {directory}")
    entries = directory.rglob("*") if recursive else directory.iterdir()
    return sorted(str(entry.resolve()) for entry in entries)


def _matches_path(path: str, pattern: str) -> bool:
    parts, patterns = path.split("/"), pattern.split("/")
    def match(i: int, j: int) -> bool:
        if j == len(patterns):
            return i == len(parts)
        if patterns[j] == "**":
            return match(i, j + 1) or (i < len(parts) and match(i + 1, j))
        return i < len(parts) and fnmatch.fnmatchcase(parts[i], patterns[j]) and match(i + 1, j + 1)
    return match(0, 0)


def _remote_glob(remote: RemotePath) -> list[str]:
    positions = [remote.path.find(char) for char in "*?[" if char in remote.path]
    first = min(positions) if positions else len(remote.path)
    parent_end = remote.path.rfind("/", 0, first)
    parent = remote.path[:parent_end] or "/"
    recursive = "/" in remote.path[parent_end + 1:]
    return [entry for entry in list_entries(RemotePath(remote.endpoint, parent).url, recursive)
            if _matches_path(parse_remote(entry).path, remote.path)]


def discover_files(value: str | Path, pattern: str = "*.root", recursive: bool = True) -> list[str]:
    value = normalize_path(value)
    remote = parse_remote(value)
    if remote:
        if any(char in remote.path for char in "*?["):
            return sorted(entry for entry in _remote_glob(remote) if basename(entry).endswith(".root"))
        if remote.path.endswith(".root"):
            return [remote.url]
        if not is_directory(remote.url):
            return [remote.url]
        return [entry for entry in list_entries(remote.url, recursive) if fnmatch.fnmatchcase(basename(entry), pattern)]
    if "://" in value:
        return [value]
    if glob.has_magic(value):
        return sorted(str(Path(entry).resolve()) for entry in glob.glob(value, recursive=True) if Path(entry).is_file())
    path = Path(value)
    if path.is_file():
        return [str(path)]
    if path.is_dir():
        entries = path.rglob(pattern) if recursive else path.glob(pattern)
        return sorted(str(entry.resolve()) for entry in entries if entry.is_file())
    raise RuntimeError(f"Input path does not exist: {value}")


def discover_directories(base: str | Path, pattern: str = "*") -> list[str]:
    remote = parse_remote(base)
    if remote:
        candidates = _remote_glob(RemotePath(remote.endpoint, posixpath.join(remote.path, pattern)))
        return sorted(entry for entry in candidates if is_directory(entry))
    return sorted(str(entry.resolve()) for entry in Path(base).glob(pattern) if entry.is_dir())


def copy_to_storage(local_file: str | Path, destination: str | Path) -> None:
    remote = parse_remote(destination)
    if remote is None:
        raise ValueError("copy_to_storage requires an XRootD destination")
    parent = RemotePath(remote.endpoint, posixpath.dirname(remote.path))
    try:
        _xrdfs(parent, "mkdir", "-p")
    except RuntimeError as exc:
        # Some XRootD servers report EEXIST even with mkdir -p.
        if "[3018]" not in str(exc) or not is_directory(parent.url):
            raise
    subprocess.run(["xrdcp", "-f", str(local_file), remote.url], check=True)


def main() -> None:
    parser = argparse.ArgumentParser(description="Discover local/EOS ROOT files or generate a ROOT MakeClass macro.")
    parser.add_argument("input")
    parser.add_argument("--pattern", default="*.root")
    parser.add_argument("--non-recursive", action="store_true")
    parser.add_argument("--root-macro")
    parser.add_argument("--tree")
    args = parser.parse_args()
    files = discover_files(args.input, args.pattern, not args.non_recursive)
    if not files:
        raise RuntimeError(f"No ROOT files found: {args.input}")
    if args.root_macro:
        if not args.tree:
            parser.error("--tree is required with --root-macro")
        lines = ["{", f"  TChain chain({json.dumps(args.tree)});"]
        lines += [f"  if (chain.Add({json.dumps(name)}, 0) == 0) gSystem->Exit(1);" for name in files]
        lines += ['  if (chain.MakeClass("rootNtupleClass") != 0) gSystem->Exit(1);', "}"]
        Path(args.root_macro).write_text("\n".join(lines) + "\n")
    else:
        print("\n".join(files))


if __name__ == "__main__":
    main()
