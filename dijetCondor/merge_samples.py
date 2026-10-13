#!/usr/bin/env python3

import argparse
import re
import shutil
import subprocess
import sys
import tempfile
from collections import Counter
from pathlib import Path
from typing import List

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "scripts"))
from eos_utils import (
    basename, copy_to_storage, discover_directories, discover_files,
    is_directory, join_path, normalize_path, parse_remote,
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Merge sample ROOT outputs into one file per sample folder. "
            "Input files are discovered recursively as *_reduced_skim.root."
        )
    )
    parser.add_argument(
        "base_dir",
        help="Base directory containing sample folders.",
    )
    parser.add_argument(
        "--sample-glob",
        default="*",
        help='Glob pattern for sample folders under base_dir (default: "*").',
    )
    parser.add_argument(
        "--file-glob",
        default="*_reduced_skim.root",
        help='Glob pattern for target ROOT files inside each sample folder (default: "*_reduced_skim.root").',
    )
    parser.add_argument(
        "--hadd-cmd",
        default="hadd",
        help='hadd executable (default: "hadd").',
    )
    parser.add_argument("--output-dir", help="Merged output directory (local or EOS; defaults to base_dir).")
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Print planned merges without executing hadd.",
    )
    return parser.parse_args()


def discover_sample_dirs(base_dir: str, sample_glob: str) -> List[str]:
    return discover_directories(base_dir, sample_glob)


def discover_root_files(sample_dir: str, file_glob: str) -> List[str]:
    return discover_files(sample_dir, file_glob)


def merged_name_from_files(root_files: List[str]) -> str:
    normalized = []
    for p in root_files:
        name = basename(p)
        name = re.sub(r"_n\d+_reduced_skim\.root$", "_reduced_skim.root", name)
        normalized.append(name)
    counts = Counter(normalized)
    return counts.most_common(1)[0][0]


def run_hadd(hadd_cmd: str, output_file: Path, inputs: List[str]) -> None:
    with tempfile.NamedTemporaryFile(mode="w", suffix=".txt", delete=False) as tmp:
        for p in inputs:
            tmp.write(f"{p}\n")
        list_path = Path(tmp.name)

    try:
        cmd = [hadd_cmd, "-f", str(output_file), f"@{list_path}"]
        proc = subprocess.run(cmd, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if proc.returncode != 0:
            msg = (
                f"hadd failed for {output_file}\n"
                f"STDOUT:\n{proc.stdout.strip()}\n"
                f"STDERR:\n{proc.stderr.strip()}"
            )
            raise RuntimeError(msg)
    finally:
        list_path.unlink(missing_ok=True)


def main() -> int:
    args = parse_args()
    base_dir = normalize_path(args.base_dir)
    output_dir = normalize_path(args.output_dir or args.base_dir)

    if not is_directory(base_dir):
        print(f"[ERROR] base_dir not found: {base_dir}", file=sys.stderr)
        return 2

    if shutil.which(args.hadd_cmd) is None and not args.dry_run:
        print(f"[ERROR] hadd executable not found: {args.hadd_cmd}", file=sys.stderr)
        return 2

    sample_dirs = discover_sample_dirs(base_dir, args.sample_glob)
    print(f"[INFO] base dir         : {base_dir}")
    print(f"[INFO] sample glob      : {args.sample_glob}")
    print(f"[INFO] matched folders  : {len(sample_dirs)}")

    if not sample_dirs:
        print("[WARN] No sample folders matched.")
        return 0

    merged_ok = 0
    merged_fail = 0
    merged_skip = 0

    for sample_dir in sample_dirs:
        root_files = discover_root_files(sample_dir, args.file_glob)
        if not root_files:
            print(f"[WARN] no files in    : {basename(sample_dir)}")
            merged_skip += 1
            continue

        out_name = merged_name_from_files(root_files)
        out_path = join_path(output_dir, out_name)
        print(f"[INFO] sample          : {basename(sample_dir)}")
        print(f"[INFO] inputs          : {len(root_files)}")
        print(f"[INFO] output          : {out_path}")

        if args.dry_run:
            continue

        staging_dir = None
        try:
            if parse_remote(out_path):
                staging_dir = Path(tempfile.mkdtemp(prefix="merge_samples_"))
                local_output = staging_dir / out_name
            else:
                local_output = Path(out_path)
                local_output.parent.mkdir(parents=True, exist_ok=True)
            run_hadd(args.hadd_cmd, local_output, root_files)
            if staging_dir:
                copy_to_storage(local_output, out_path)
                shutil.rmtree(staging_dir)
            merged_ok += 1
        except Exception as exc:
            print(f"[ERROR] merge failed   : {basename(sample_dir)}")
            print(f"[ERROR] reason         : {exc}")
            if staging_dir:
                print(f"[INFO] Local merge files retained in: {staging_dir}")
            merged_fail += 1

    print(f"[INFO] merged ok       : {merged_ok}")
    print(f"[INFO] merged failed   : {merged_fail}")
    print(f"[INFO] skipped         : {merged_skip}")
    return 0 if merged_fail == 0 else 1


if __name__ == "__main__":
    raise SystemExit(main())

