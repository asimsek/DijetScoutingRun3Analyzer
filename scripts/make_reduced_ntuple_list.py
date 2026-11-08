#!/usr/bin/env python3

from __future__ import annotations

import argparse
import math
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
    normalized = normalize_path(path)
    remote = parse_remote(normalized)
    if remote and re.fullmatch(r"root://(?:cmseos|cmseosmgm\d+)\.fnal\.gov(?::1094)?", remote.endpoint, re.IGNORECASE):
        storage_path = remote.path
        if storage_path.startswith("/eos/uscms/store/"):
            storage_path = storage_path[len("/eos/uscms"):]
        return "root://cmseos.fnal.gov/" + storage_path
    return normalized


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


def normalize_qcd_bin(value: str) -> str:
    match = re.fullmatch(r"([0-9]+)(?:to([0-9]+|Inf))?", value, re.IGNORECASE)
    if not match:
        raise ValueError(f"Invalid QCD pT bin: {value}; use 50to80 or 3000 (also 3000toInf)")
    lower, upper = match.groups()
    lower = int(lower)
    if upper is None or upper.lower() == "inf":
        return str(lower)
    if int(upper) <= lower:
        raise ValueError(f"Invalid QCD pT range: {value}")
    return f"{lower}to{int(upper)}"


def parse_xsecs(items: list[str] | None) -> dict[str, float]:
    values = {}
    for item in items or []:
        label, separator, raw = item.partition("=")
        if not separator:
            raise ValueError(f"Invalid cross section: {item}; expected BIN=PB")
        key = normalize_qcd_bin(label)
        try:
            value = float(raw)
        except ValueError:
            raise ValueError(f"Invalid cross section for {label}: {raw}") from None
        if not math.isfinite(value) or value <= 0:
            raise ValueError(f"Cross section for {label} must be finite and positive")
        if key in values:
            raise ValueError(f"Repeated cross section for QCD pT bin {key}")
        values[key] = value
    return values


def qcd_list_lines(paths: list[str], xsecs: dict[str, float]) -> list[str]:
    by_bin = {}
    for path in paths:
        remote = parse_remote(path)
        name = Path(remote.path if remote else path).name
        match = re.match(r"QCD_Bin-PT-([0-9]+(?:to(?:[0-9]+|Inf))?)(?=[_.-]|$)", name, re.IGNORECASE)
        if not match:
            raise ValueError(f"Cannot identify QCD pT bin from filename: {name}")
        key = normalize_qcd_bin(match.group(1))
        if key not in xsecs:
            raise ValueError(f"Missing --xsecs value for QCD pT bin {key}: {name}")
        if key in by_bin:
            raise ValueError(f"Multiple ROOT files for QCD pT bin {key}; merge each sample into one file before using --xsecs")
        by_bin[key] = path
    missing = xsecs.keys() - by_bin.keys()
    if missing:
        raise ValueError("No ROOT file found for QCD pT bins: " + ", ".join(sorted(missing, key=natural_key)))
    return [f"{by_bin[key]} {xsecs[key]}" for key in sorted(by_bin, key=natural_key)]


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
    parser.add_argument(
        "--xsecs",
        nargs="+",
        metavar="BIN=PB",
        help="QCD cross sections, e.g. 50to80=16730000.0 3000=0.0005539; requires one merged QCD_Bin-PT-BIN ROOT file per bin.",
    )
    return parser


def main() -> int:
    parser = build_arg_parser()
    args = parser.parse_args()
    try:
        xsecs = parse_xsecs(args.xsecs)
    except ValueError as error:
        parser.error(str(error))
    repo_root = Path(__file__).resolve().parents[1]
    output_dir = resolve_output_dir(repo_root)
    output_dir.mkdir(parents=True, exist_ok=True)

    root_files = discover_root_files(args.input_path)
    if not root_files:
        raise RuntimeError(f"No {TARGET_PATTERN} files found under: {args.input_path}")

    output_name = args.output_name or derive_output_name(args.input_path)
    output_path = output_dir / output_name

    lines = [to_xrootd_path(path) for path in root_files]
    if xsecs:
        try:
            lines = qcd_list_lines(lines, xsecs)
        except ValueError as error:
            parser.error(str(error))
    output_path.write_text("\n".join(lines) + "\n")

    print(f"[INFO] Input path   : {normalize_path(args.input_path)}")
    print(f"[INFO] Files found  : {len(root_files):,}")
    print(f"[INFO] Output list  : {output_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

