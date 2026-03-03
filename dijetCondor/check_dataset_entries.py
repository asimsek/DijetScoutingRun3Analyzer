#!/usr/bin/env python3

import argparse
import concurrent.futures as cf
import json
import math
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path
from typing import Iterable, List, Optional, Sequence, Tuple


def format_with_dots(value: int) -> str:
    sign = "-" if value < 0 else ""
    return f"{sign}{abs(value):,}".replace(",", ".")


class _SimpleProgress:
    def __init__(self, total: int, desc: str, unit: str = "file") -> None:
        self.total = max(1, total)
        self.desc = desc
        self.unit = unit
        self.count = 0
        self._last_draw = 0.0
        self._draw(force=True)

    def update(self, n: int = 1) -> None:
        self.count += n
        self._draw()

    def close(self) -> None:
        self._draw(force=True)
        sys.stdout.write("\n")
        sys.stdout.flush()

    def _draw(self, force: bool = False) -> None:
        now = time.monotonic()
        if not force and (now - self._last_draw) < 0.10 and self.count < self.total:
            return
        self._last_draw = now
        frac = min(1.0, self.count / self.total)
        width = 30
        fill = int(width * frac)
        bar = "#" * fill + "-" * (width - fill)
        msg = f"\r{self.desc}: [{bar}] {frac*100:6.2f}% ({self.count}/{self.total} {self.unit})"
        sys.stdout.write(msg)
        sys.stdout.flush()


def make_progress(total: int, desc: str, unit: str = "file"):
    try:
        from tqdm import tqdm  # type: ignore

        return tqdm(total=total, desc=desc, unit=unit)
    except Exception:
        return _SimpleProgress(total=total, desc=desc, unit=unit)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Count total entries in the Events tree for all files in a DAS dataset "
            "and compare against DAS nevents."
        )
    )
    parser.add_argument(
        "dataset",
        help="CMS DAS dataset name, e.g. /ScoutingPFRun3/Run2024H-ScoutNano-v1/NANOAOD",
    )
    parser.add_argument(
        "--tree",
        default="Events",
        help="TTree name to inspect (default: Events).",
    )
    parser.add_argument(
        "--workers",
        type=int,
        default=8,
        help="Parallel workers for file checks (default: 8).",
    )
    parser.add_argument(
        "--redirector",
        default="root://cms-xrd-global.cern.ch/",
        help="Redirector prefix for DAS LFNs (default: root://cms-xrd-global.cern.ch/).",
    )
    parser.add_argument(
        "--backend",
        choices=("auto", "root", "uproot"),
        default="auto",
        help="Reader backend (default: auto).",
    )
    parser.add_argument(
        "--max-bad-files",
        type=int,
        default=20,
        help="Maximum number of unreadable files to print (default: 20).",
    )
    parser.add_argument(
        "--no-compare-das",
        action="store_true",
        help="Skip DAS nevents summary comparison and only sum file entries.",
    )
    parser.add_argument(
        "--show-all-bad-files",
        action="store_true",
        help="Print all unreadable files instead of truncating the list.",
    )
    return parser.parse_args()


def run_cmd(cmd: Sequence[str]) -> subprocess.CompletedProcess:
    return subprocess.run(
        list(cmd),
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=True,
    )


def ensure_dasgoclient() -> None:
    if shutil.which("dasgoclient") is None:
        raise RuntimeError("dasgoclient not found. Load CMSSW runtime first.")


def fetch_dataset_files(dataset: str) -> List[str]:
    proc = run_cmd(["dasgoclient", "-query", f"file dataset={dataset}"])
    return [line.strip() for line in proc.stdout.splitlines() if line.strip()]


def fetch_dataset_nevents(dataset: str) -> Optional[int]:
    proc = run_cmd(["dasgoclient", "-query", f"summary dataset={dataset}", "-json"])
    payload = json.loads(proc.stdout)
    try:
        return int(payload[0]["summary"][0]["nevents"])
    except Exception:
        return None


def normalize_url(path_or_lfn: str, redirector: str) -> str:
    if "://" in path_or_lfn:
        return path_or_lfn
    if path_or_lfn.startswith("/store/"):
        # XRootD absolute LFNs must be formatted as:
        #   root://host//store/...
        # not:
        #   root://host/store/...
        return redirector.rstrip("/") + "//" + path_or_lfn.lstrip("/")
    return path_or_lfn


def choose_backend(requested: str) -> str:
    if requested == "root":
        return "root"
    if requested == "uproot":
        return "uproot"

    try:
        import ROOT  # type: ignore  # noqa: F401

        return "root"
    except Exception:
        return "uproot"


def _count_entries_root_one(args: Tuple[str, str, str]) -> Tuple[str, int, Optional[str]]:
    file_lfn, tree_name, redirector = args
    url = normalize_url(file_lfn, redirector)
    try:
        import ROOT  # type: ignore

        ROOT.gROOT.SetBatch(True)
        root_file = ROOT.TFile.Open(url)
        if not root_file or root_file.IsZombie():
            return file_lfn, 0, "failed to open ROOT file"

        tree = root_file.Get(tree_name)
        if not tree:
            try:
                root_file.Close()
            except Exception:
                pass
            return file_lfn, 0, f"tree '{tree_name}' not found"

        entries = int(tree.GetEntriesFast())
        try:
            root_file.Close()
        except Exception:
            pass
        return file_lfn, entries, None
    except Exception as exc:
        return file_lfn, 0, str(exc)


def count_entries_root(
    files: Sequence[str],
    tree_name: str,
    redirector: str,
    workers: int,
) -> Tuple[int, List[Tuple[str, str]]]:
    total = 0
    bad: List[Tuple[str, str]] = []
    progress = make_progress(len(files), "Checking files", unit="file")

    nworkers = max(1, min(int(workers), len(files)))
    job_args = [(f, tree_name, redirector) for f in files]
    chunksize = max(1, math.ceil(len(job_args) / max(1, nworkers * 8)))

    with cf.ProcessPoolExecutor(max_workers=nworkers) as ex:
        for file_lfn, entries, err in ex.map(_count_entries_root_one, job_args, chunksize=chunksize):
            total += entries
            if err:
                bad.append((file_lfn, err))
            progress.update(1)

    progress.close()
    return total, bad


def _count_entries_uproot_one(args: Tuple[str, str, str]) -> Tuple[str, int, Optional[str]]:
    file_lfn, tree_name, redirector = args
    url = normalize_url(file_lfn, redirector)
    try:
        import uproot  # type: ignore

        with uproot.open(f"{url}:{tree_name}") as tree:
            return file_lfn, int(tree.num_entries), None
    except Exception as exc:
        return file_lfn, 0, str(exc)


def count_entries_uproot(
    files: Sequence[str],
    tree_name: str,
    redirector: str,
    workers: int,
) -> Tuple[int, List[Tuple[str, str]]]:
    if any(f.startswith("/store/") or f.startswith("root://") for f in files):
        try:
            import fsspec_xrootd  # type: ignore  # noqa: F401
        except Exception as exc:
            raise RuntimeError(
                "uproot backend requires fsspec-xrootd for root:// access. "
                "Either install it with `python3 -m pip install --user fsspec-xrootd uproot` "
                "or use `--backend root` inside a CMSSW/PyROOT environment."
            ) from exc

    total = 0
    bad: List[Tuple[str, str]] = []
    progress = make_progress(len(files), "Checking files", unit="file")

    nworkers = max(1, min(int(workers), len(files)))
    job_args = [(f, tree_name, redirector) for f in files]

    with cf.ThreadPoolExecutor(max_workers=nworkers) as ex:
        futures = [ex.submit(_count_entries_uproot_one, item) for item in job_args]
        for fut in cf.as_completed(futures):
            file_lfn, entries, err = fut.result()
            total += entries
            if err:
                bad.append((file_lfn, err))
            progress.update(1)

    progress.close()
    return total, bad


def main() -> int:
    args = parse_args()

    ensure_dasgoclient()

    dataset = args.dataset.strip()
    if not dataset:
        raise RuntimeError("Dataset must not be empty.")

    backend = choose_backend(args.backend)

    print(f"[INFO] Dataset          : {dataset}")
    print(f"[INFO] Tree             : {args.tree}")
    print(f"[INFO] Redirector       : {args.redirector}")
    print(f"[INFO] Backend          : {backend}")
    print(f"[INFO] Workers          : {args.workers}")

    print(f"[CMD] dasgoclient -query file dataset={dataset}")
    files = fetch_dataset_files(dataset)
    if not files:
        raise RuntimeError(f"No files found in DAS for dataset: {dataset}")

    print(f"[INFO] Dataset files    : {format_with_dots(len(files))}")

    das_nevents: Optional[int] = None
    if not args.no_compare_das:
        print(f"[CMD] dasgoclient -query summary dataset={dataset} -json")
        das_nevents = fetch_dataset_nevents(dataset)
        if das_nevents is None:
            print("[WARN] Could not extract DAS nevents from summary response.")
        else:
            print(f"[INFO] DAS nevents      : {format_with_dots(das_nevents)}")

    t0 = time.monotonic()
    if backend == "root":
        total_entries, bad_files = count_entries_root(
            files=files,
            tree_name=args.tree,
            redirector=args.redirector,
            workers=args.workers,
        )
    else:
        total_entries, bad_files = count_entries_uproot(
            files=files,
            tree_name=args.tree,
            redirector=args.redirector,
            workers=args.workers,
        )
    dt = max(0.0, time.monotonic() - t0)

    print()
    print(f"[INFO] File-summed      : {format_with_dots(total_entries)}")
    print(f"[INFO] Unreadable files : {format_with_dots(len(bad_files))}")
    print(f"[INFO] Elapsed time     : {dt:.1f} s")

    if das_nevents is not None:
        diff = total_entries - das_nevents
        print(f"[INFO] Difference       : {format_with_dots(diff)}")
        if diff == 0 and not bad_files:
            print("[INFO] Result           : file count matches DAS exactly.")
        elif diff == 0:
            print("[WARN] Result           : total matches DAS, but some files were unreadable.")
        else:
            print("[WARN] Result           : file count does not match DAS.")

    if bad_files:
        print()
        print("[WARN] Bad files:")
        limit = len(bad_files) if args.show_all_bad_files else max(0, args.max_bad_files)
        for file_lfn, err in bad_files[:limit]:
            print(f"  {file_lfn}")
            print(f"    {err}")
        if limit < len(bad_files):
            print(f"[WARN] ... truncated {len(bad_files) - limit} additional bad files.")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())

