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
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Print planned merges without executing hadd.",
    )
    return parser.parse_args()


def discover_sample_dirs(base_dir: Path, sample_glob: str) -> List[Path]:
    return sorted([p for p in base_dir.glob(sample_glob) if p.is_dir()])


def discover_root_files(sample_dir: Path, file_glob: str) -> List[Path]:
    return sorted([p for p in sample_dir.rglob(file_glob) if p.is_file()])


def merged_name_from_files(root_files: List[Path]) -> str:
    normalized = []
    for p in root_files:
        name = p.name
        name = re.sub(r"_n\d+_reduced_skim\.root$", "_reduced_skim.root", name)
        normalized.append(name)
    counts = Counter(normalized)
    return counts.most_common(1)[0][0]


def run_hadd(hadd_cmd: str, output_file: Path, inputs: List[Path]) -> None:
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
    base_dir = Path(args.base_dir).expanduser().resolve()

    if not base_dir.exists() or not base_dir.is_dir():
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
            print(f"[WARN] no files in    : {sample_dir.name}")
            merged_skip += 1
            continue

        out_name = merged_name_from_files(root_files)
        out_path = base_dir / out_name
        print(f"[INFO] sample          : {sample_dir.name}")
        print(f"[INFO] inputs          : {len(root_files)}")
        print(f"[INFO] output          : {out_path.name}")

        if args.dry_run:
            continue

        try:
            run_hadd(args.hadd_cmd, out_path, root_files)
            merged_ok += 1
        except Exception as exc:
            print(f"[ERROR] merge failed   : {sample_dir.name}")
            print(f"[ERROR] reason         : {exc}")
            merged_fail += 1

    print(f"[INFO] merged ok       : {merged_ok}")
    print(f"[INFO] merged failed   : {merged_fail}")
    print(f"[INFO] skipped         : {merged_skip}")
    return 0 if merged_fail == 0 else 1


if __name__ == "__main__":
    raise SystemExit(main())

