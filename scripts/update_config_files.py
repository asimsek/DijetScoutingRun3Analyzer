#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path

FULL_MASS_BINS = [
    1, 3, 6, 10, 16, 23, 31, 40, 50, 61, 74, 88, 103, 119, 137, 156, 176, 197, 220, 244,
    270, 296, 325, 354, 386, 419, 453, 489, 526, 565, 606, 649, 693, 740, 788, 838, 890, 944,
    1000, 1058, 1118, 1181, 1246, 1313, 1383, 1455, 1530, 1607, 1687, 1770, 1856, 1945, 2037,
    2132, 2231, 2332, 2438, 2546, 2659, 2775, 2895, 3019, 3147, 3279, 3416, 3558, 3704, 3854,
    4010, 4171, 4337, 4509, 4686, 4869, 5058, 5253, 5455, 5663, 5877, 6099, 6328, 6564, 6808,
    7060, 7320, 7589, 7866, 8152, 8447, 8752, 9067, 9391, 9726, 10072, 10430, 10798, 11179,
    11571, 11977, 12395, 12827, 13272, 13732, 14000,
]


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=("Update mjj/th1x/variables_range/signal_mjj/signal_th1x in fit config files."))
    parser.add_argument("paths", nargs="+", help="One or more config files and/or directories containing .config files")
    parser.add_argument("--mass-start", type=int, required=True, help="New mjj lower edge")
    parser.add_argument("--mass-end", type=int, required=True, help="New mjj upper edge")
    parser.add_argument("--recursive", action="store_true", help="When a directory is given, search for .config files recursively")
    parser.add_argument(
        "--tag",
        action="append",
        default=[],
        help="Optional section tag to update, e.g. PFDijet2024 or [PFDijet2024]. Can be repeated or comma-separated.",
    )
    parser.add_argument("--dry-run", action="store_true", help="Show what would change without writing files")
    parser.add_argument("--backup-suffix", default=".bak", help="Backup suffix written before updating; use empty string to disable backups",
    )
    return parser.parse_args()


def collect_paths(args: argparse.Namespace) -> list[Path]:
    collected: list[Path] = []
    seen: set[Path] = set()
    missing: list[str] = []
    empty_dirs: list[str] = []

    def add_path(path: Path) -> None:
        resolved = path.resolve()
        if resolved not in seen:
            seen.add(resolved)
            collected.append(resolved)

    for raw_item in args.paths:
        candidate = Path(raw_item).resolve()
        if not candidate.exists():
            missing.append(str(candidate))
            continue

        if candidate.is_file():
            add_path(candidate)
            continue

        if candidate.is_dir():
            matcher = candidate.rglob("*.config") if args.recursive else candidate.glob("*.config")
            matched = sorted(path.resolve() for path in matcher if path.is_file())
            if not matched:
                empty_dirs.append(str(candidate))
                continue
            for path in matched:
                add_path(path)
            continue

        missing.append(str(candidate))

    if missing:
        raise SystemExit("[ERROR] Missing path(s):\n  " + "\n  ".join(missing))
    if empty_dirs:
        raise SystemExit("[ERROR] No .config files found under:\n  " + "\n  ".join(empty_dirs))
    if not collected:
        raise SystemExit("[ERROR] No config files found")
    return collected


def validate_mass_range(mass_start: int, mass_end: int) -> tuple[int, int, int, int]:
    if mass_start not in FULL_MASS_BINS:
        raise SystemExit(f"[ERROR] mass-start={mass_start} is not in the supported dijet binning")
    if mass_end not in FULL_MASS_BINS:
        raise SystemExit(f"[ERROR] mass-end={mass_end} is not in the supported dijet binning")

    start_idx = FULL_MASS_BINS.index(mass_start)
    end_idx = FULL_MASS_BINS.index(mass_end)
    if start_idx > end_idx:
        raise SystemExit("[ERROR] mass-start must be <= mass-end")
    return mass_start, mass_end, start_idx, end_idx


def build_signal_masses(start_idx: int, end_idx: int) -> list[int]:
    return FULL_MASS_BINS[start_idx : end_idx + 1]


def format_mjj_value(value: int) -> str:
    return f"{value}."


def split_comment(line: str) -> tuple[str, str]:
    if "#" not in line:
        return line.rstrip("\n"), ""
    content, comment = line.split("#", 1)
    return content.rstrip(), "#" + comment.rstrip("\n")


def normalize_tags(raw_tags: list[str]) -> set[str]:
    tags: set[str] = set()
    for raw in raw_tags:
        for item in raw.split(","):
            tag = item.strip()
            if not tag:
                continue
            if tag.startswith("[") and tag.endswith("]"):
                tag = tag[1:-1].strip()
            if tag:
                tags.add(tag)
    return tags


def replace_target_lines(
    text: str,
    mass_start: int,
    mass_end: int,
    signal_masses: list[int],
    selected_tags: set[str],
) -> tuple[str, dict[str, int], set[str]]:
    th1x_entries = len(signal_masses)
    variables_value = (
        f"variables = ['mjj[{format_mjj_value(mass_start)},{format_mjj_value(mass_start)},{format_mjj_value(mass_end)}]',"
        f"'th1x[0,0,{th1x_entries}]']"
    )
    variables_value = "".join(variables_value)
    variables_range_value = (
        f"variables_range = ['mjj_Low[{format_mjj_value(mass_start)},{format_mjj_value(mass_end)}]', "
        f"'mjj_Blind[{format_mjj_value(mass_start)},{format_mjj_value(mass_end)}]', "
        f"'mjj_High[{format_mjj_value(mass_start)},{format_mjj_value(mass_end)}]']"
    )
    signal_mjj_value = "signal_mjj = [" + ", ".join(str(x) for x in signal_masses) + "]"
    signal_th1x_value = f"signal_th1x = range(0,{th1x_entries})"

    replacements = {
        "variables": 0,
        "variables_range": 0,
        "signal_mjj": 0,
        "signal_th1x": 0,
    }
    matched_tags: set[str] = set()

    out_lines: list[str] = []
    current_section = ""
    update_current_section = not selected_tags
    for original in text.splitlines(keepends=True):
        stripped = original.lstrip()
        indent = original[: len(original) - len(stripped)]
        newline = "\n" if original.endswith("\n") else ""
        stripped_no_nl = stripped.rstrip("\n")

        if stripped_no_nl.startswith("[") and stripped_no_nl.endswith("]"):
            current_section = stripped_no_nl[1:-1].strip()
            update_current_section = not selected_tags or current_section in selected_tags
            if update_current_section and selected_tags:
                matched_tags.add(current_section)
            out_lines.append(original)
            continue

        if update_current_section and stripped.startswith("variables ="):
            out_lines.append(indent + variables_value + newline)
            replacements["variables"] += 1
            continue

        if update_current_section and stripped.startswith("variables_range ="):
            _, comment = split_comment(stripped)
            new_line = indent + variables_range_value
            if comment:
                new_line += " " + comment
            out_lines.append(new_line + newline)
            replacements["variables_range"] += 1
            continue

        if update_current_section and stripped.startswith("signal_mjj ="):
            out_lines.append(indent + signal_mjj_value + newline)
            replacements["signal_mjj"] += 1
            continue

        if update_current_section and stripped.startswith("signal_th1x ="):
            out_lines.append(indent + signal_th1x_value + newline)
            replacements["signal_th1x"] += 1
            continue

        out_lines.append(original)

    return "".join(out_lines), replacements, matched_tags


def backup_file(path: Path, backup_suffix: str) -> None:
    if not backup_suffix:
        return
    backup_path = Path(str(path) + backup_suffix)
    backup_path.write_text(path.read_text())


def ensure_found(path: Path, replacements: dict[str, int]) -> None:
    missing = [name for name, count in replacements.items() if count == 0]
    if missing:
        raise SystemExit(
            f"[ERROR] {path} is missing expected key(s): " + ", ".join(missing)
        )


def main() -> int:
    args = parse_args()
    paths = collect_paths(args)
    mass_start, mass_end, start_idx, end_idx = validate_mass_range(args.mass_start, args.mass_end)
    signal_masses = build_signal_masses(start_idx, end_idx)
    th1x_entries = len(signal_masses)
    selected_tags = normalize_tags(args.tag)

    print(f"[INFO] mjj range        : {mass_start} -> {mass_end}")
    print(f"[INFO] signal_mjj range : {signal_masses[0]} -> {signal_masses[-1]}")
    print(f"[INFO] signal_mjj count : {th1x_entries}")
    if selected_tags:
        print(f"[INFO] selected tag(s) : {', '.join(sorted(selected_tags))}")

    for path in paths:
        original = path.read_text()
        updated, replacements, matched_tags = replace_target_lines(
            original, mass_start, mass_end, signal_masses, selected_tags
        )
        if selected_tags:
            if not matched_tags:
                print(f"[INFO] skipped      : {path} (no matching tag)")
                continue
        ensure_found(path, replacements)

        changed = updated != original
        status = "would update" if args.dry_run else "updated"
        if changed:
            if not args.dry_run:
                backup_file(path, args.backup_suffix)
                path.write_text(updated)
            if matched_tags:
                print(f"[INFO] {status:12}: {path} [{', '.join(sorted(matched_tags))}]")
            else:
                print(f"[INFO] {status:12}: {path}")
        else:
            if matched_tags:
                print(f"[INFO] unchanged    : {path} [{', '.join(sorted(matched_tags))}]")
            else:
                print(f"[INFO] unchanged    : {path}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())

