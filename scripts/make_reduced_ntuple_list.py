#!/usr/bin/env python3

from __future__ import annotations

import argparse
import os
import re
from pathlib import Path

from eos_utils import discover_files, normalize_path, parse_remote


TARGET_SUBDIR = Path("lists/reducedNtuple_lists")
TARGET_PATTERN = "*_reduced_skim.root"


def natural_key(text: str):
    return [int(part) if part.isdigit() else part.lower() for part in re.split(r"(\d+)", text)]


def resolve_output_dir(repo_root: Path) -> Path:
    cmssw_base = os.environ.get("CMSSW_BASE")
    if cmssw_base:
        return (Path(cmssw_base).resolve() / "src" / "DijetScoutingRun3Analyzer" / TARGET_SUBDIR).resolve()
    return (repo_root / TARGET_SUBDIR).resolve()


def discover_root_files(input_path: str) -> list[str]:
    return sorted(discover_files(input_path, TARGET_PATTERN), key=natural_key)


def to_xrootd_path(path: str | Path) -> str:
    return normalize_path(path)


def derive_output_name(input_path: str) -> str:
    remote = parse_remote(input_path)
    expanded = Path(remote.path if remote else os.path.expandvars(os.path.expanduser(input_path)))
    if any(char in input_path for char in "*?[]"):
        token = expanded.parent.name or expanded.stem or "reduced_ntuple_list"
    else:
        token = expanded.name if expanded.name else expanded.parent.name
        if token.endswith(".root"):
            token = token[:-5]
    token = re.sub(r"[^A-Za-z0-9._-]+", "_", token).strip("_") or "reduced_ntuple_list"
    return f"{token}_reduced.txt"


def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description=(
            "Create a reduced-ntuple list from a given directory, file, or glob. "
            "The output list is written to $CMSSW_BASE/src/DijetScoutingRun3Analyzer/lists/reducedNtuple_lists "
            "when CMSSW_BASE is available."
        ),
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument("input_path", help="Input directory, single ROOT file, or glob pattern to scan.")
    parser.add_argument(
        "-o",
        "--output-name",
        default=None,
        help="Output list filename. Defaults to <input-token>_reduced.txt.",
    )
    return parser


def main() -> int:
    args = build_arg_parser().parse_args()
    repo_root = Path(__file__).resolve().parents[1]
    output_dir = resolve_output_dir(repo_root)
    output_dir.mkdir(parents=True, exist_ok=True)

    root_files = discover_root_files(args.input_path)
    if not root_files:
        raise RuntimeError(f"No {TARGET_PATTERN} files found under: {args.input_path}")

    output_name = args.output_name or derive_output_name(args.input_path)
    output_path = output_dir / output_name

    lines = [to_xrootd_path(path) for path in root_files]
    output_path.write_text("\n".join(lines) + "\n")

    print(f"[INFO] Input path   : {normalize_path(args.input_path)}")
    print(f"[INFO] Files found  : {len(root_files):,}")
    print(f"[INFO] Output list  : {output_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

