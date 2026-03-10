#!/usr/bin/env python3

from __future__ import annotations

import argparse
import os
import re
from pathlib import Path


EOS_PREFIX = "/eos/uscms"
XROOTD_PREFIX = "root://cmseos.fnal.gov/"
TARGET_SUBDIR = Path("lists/reducedNtuple_lists")
TARGET_PATTERN = "*_reduced_skim.root"


def natural_key(text: str):
    return [int(part) if part.isdigit() else part.lower() for part in re.split(r"(\d+)", text)]


def resolve_output_dir(repo_root: Path) -> Path:
    cmssw_base = os.environ.get("CMSSW_BASE")
    if cmssw_base:
        return (Path(cmssw_base).resolve() / "src" / "DijetScoutingRun3Analyzer" / TARGET_SUBDIR).resolve()
    return (repo_root / TARGET_SUBDIR).resolve()


def discover_root_files(input_path: str) -> list[Path]:
    expanded = Path(os.path.expandvars(os.path.expanduser(input_path)))
    if any(char in input_path for char in "*?[]"):
        matches = [Path(p).resolve() for p in expanded.parent.glob(expanded.name)]
        return sorted((p for p in matches if p.is_file()), key=lambda p: natural_key(str(p)))

    resolved = expanded.resolve()
    if resolved.is_file():
        return [resolved]
    if resolved.is_dir():
        files = [path.resolve() for path in resolved.rglob(TARGET_PATTERN) if path.is_file()]
        return sorted(files, key=lambda p: natural_key(str(p)))
    raise RuntimeError(f"Input path does not exist: {input_path}")


def to_xrootd_path(path: Path) -> str:
    path_str = str(path)
    if path_str.startswith(EOS_PREFIX):
        return path_str.replace(EOS_PREFIX, XROOTD_PREFIX, 1)
    return path_str


def derive_output_name(input_path: str) -> str:
    expanded = Path(os.path.expandvars(os.path.expanduser(input_path)))
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

    print(f"[INFO] Input path   : {Path(os.path.expandvars(os.path.expanduser(args.input_path))).resolve()}")
    print(f"[INFO] Files found  : {len(root_files):,}")
    print(f"[INFO] Output list  : {output_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

