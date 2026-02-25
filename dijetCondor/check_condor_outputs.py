#!/usr/bin/env python3

import argparse
import json
import re
import subprocess
import sys
import time
from pathlib import Path
from typing import Dict, List, Optional, Tuple


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Check missing Condor output ROOT files from a cjobs_* directory and an EOS output directory. "
            "Optionally resubmit missing jobs."
        )
    )
    parser.add_argument("cjobs_dir", help="Path to cjobs_* directory containing .jdl/.sh files.")
    parser.add_argument("eos_output_dir", help="Path to directory containing *_reduced_skim.root files.")
    parser.add_argument("--resubmit", action="store_true", help="Resubmit missing jobs using corresponding .jdl files.")
    parser.add_argument(
        "--total-events",
        action="store_true",
        help="Compute total number of entries over all *_reduced_skim.root files in eos_output_dir.",
    )
    parser.add_argument(
        "--dataset",
        default="",
        help="Optional DAS dataset name. If provided, expected event count is fetched from DAS and compared.",
    )
    parser.add_argument(
        "--request-memory-mb",
        type=int,
        default=0,
        help=(
            "If >0 and used with --resubmit, set request_memory in missing-job .jdl files before submission. "
            "Existing request_memory is replaced; if absent, it is added."
        ),
    )
    return parser.parse_args()


def format_with_dots(value: int) -> str:
    sign = "-" if value < 0 else ""
    return f"{sign}{abs(value):,}".replace(",", ".")


def get_jdl_files(cjobs_dir: Path) -> List[Path]:
    return sorted([p for p in cjobs_dir.glob("*.jdl") if p.is_file()])


def read_executable_from_jdl(jdl_path: Path) -> Optional[str]:
    for line in jdl_path.read_text().splitlines():
        s = line.strip()
        if s.startswith("Executable"):
            parts = s.split("=", 1)
            if len(parts) == 2:
                return parts[1].strip()
    return None


def expected_root_from_sh(sh_path: Path) -> Optional[str]:
    if not sh_path.exists():
        return None

    text = sh_path.read_text()
    matches = re.findall(r"([A-Za-z0-9._-]+_reduced_skim\.root)", text)
    if not matches:
        return None
    # first match is the local produced file name in generated job scripts
    return matches[0]


def build_expected_map(cjobs_dir: Path, jdl_files: List[Path]) -> Tuple[Dict[str, Path], List[Path]]:
    expected_to_jdl: Dict[str, Path] = {}
    unresolved: List[Path] = []

    for jdl in jdl_files:
        sh_name = read_executable_from_jdl(jdl)
        if not sh_name:
            unresolved.append(jdl)
            continue
        sh_path = cjobs_dir / sh_name
        root_name = expected_root_from_sh(sh_path)
        if not root_name:
            unresolved.append(jdl)
            continue
        expected_to_jdl[root_name] = jdl

    return expected_to_jdl, unresolved


def get_existing_root_files(eos_dir: Path) -> List[Path]:
    # Intentionally simple and fast: only check this directory level.
    return sorted([p for p in eos_dir.glob("*_reduced_skim.root") if p.is_file()])


def _iter_with_progress(items: List[Path], desc: str):
    try:
        from tqdm import tqdm  # type: ignore

        return tqdm(items, total=len(items), unit="file", desc=desc)
    except Exception:
        # Fallback: no tqdm, return plain iterable.
        return items


def sum_events_fast(root_files: List[Path], show_progress: bool = False) -> Tuple[int, int, str]:
    if not root_files:
        return 0, 0, "rootTupleTree/tree"

    import ROOT  # type: ignore

    ROOT.gROOT.SetBatch(True)

    primary_tree = "rootTupleTree/tree"
    fallback_tree = "tree"

    chain = ROOT.TChain(primary_tree)
    added = 0
    iterable = _iter_with_progress(root_files, "Counting entries (rootTupleTree/tree)") if show_progress else root_files
    t0 = time.monotonic()
    for rf in iterable:
        if chain.Add(str(rf)) > 0:
            added += 1
    entries = int(chain.GetEntries())
    used_tree = primary_tree
    if show_progress and not hasattr(iterable, "close"):
        dt = max(0.0, time.monotonic() - t0)
        print(f"[INFO] counting time  : {dt:.1f}s")
    elif show_progress and hasattr(iterable, "close"):
        iterable.close()

    if entries == 0:
        chain_fb = ROOT.TChain(fallback_tree)
        added_fb = 0
        iterable_fb = _iter_with_progress(root_files, "Counting entries (tree)") if show_progress else root_files
        t1 = time.monotonic()
        for rf in iterable_fb:
            if chain_fb.Add(str(rf)) > 0:
                added_fb += 1
        entries_fb = int(chain_fb.GetEntries())
        if show_progress and not hasattr(iterable_fb, "close"):
            dt = max(0.0, time.monotonic() - t1)
            print(f"[INFO] fallback time  : {dt:.1f}s")
        elif show_progress and hasattr(iterable_fb, "close"):
            iterable_fb.close()
        if entries_fb > 0 or added_fb > 0:
            entries = entries_fb
            added = added_fb
            used_tree = fallback_tree

    return entries, added, used_tree


def _find_numeric_key(obj, keys: set) -> Optional[int]:
    if isinstance(obj, dict):
        for k, v in obj.items():
            if k in keys:
                if isinstance(v, (int, float)):
                    return int(v)
                if isinstance(v, str) and re.fullmatch(r"\d+", v):
                    return int(v)
            found = _find_numeric_key(v, keys)
            if found is not None:
                return found
    elif isinstance(obj, list):
        for item in obj:
            found = _find_numeric_key(item, keys)
            if found is not None:
                return found
    return None


def query_dataset_expected_events(dataset: str) -> int:
    cmd = ["dasgoclient", "-query", f"summary dataset={dataset}", "-json"]
    proc = subprocess.run(cmd, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if proc.returncode != 0:
        raise RuntimeError(f"dasgoclient failed for dataset={dataset}: {proc.stderr.strip()}")
    try:
        payload = json.loads(proc.stdout)
    except json.JSONDecodeError as exc:
        raise RuntimeError(f"Invalid DAS JSON response for dataset={dataset}") from exc

    value = _find_numeric_key(payload, {"num_event", "nevents", "num_events"})
    if value is None:
        raise RuntimeError("Could not find expected event count (num_event/nevents) in DAS response.")
    return int(value)


def write_missing_list(cjobs_dir: Path, missing_roots: List[str], missing_jdls: List[Path]) -> Path:
    out_path = cjobs_dir / "missing_outputs.txt"
    with out_path.open("w") as f:
        for root_name, jdl in zip(missing_roots, missing_jdls):
            f.write(f"{root_name}\t{jdl.name}\n")
    return out_path


def update_jdl_request_memory(jdl_path: Path, memory_mb: int) -> bool:
    text = jdl_path.read_text()
    lines = text.splitlines()

    request_re = re.compile(r"^\s*request_memory\s*=", re.IGNORECASE)
    new_line = f"request_memory = {memory_mb}"
    changed = False
    found = False
    new_lines: List[str] = []

    for line in lines:
        if request_re.match(line):
            found = True
            if line.strip() != new_line:
                new_lines.append(new_line)
                changed = True
            else:
                new_lines.append(line)
            continue
        new_lines.append(line)

    if not found:
        insert_idx = None
        queue_re = re.compile(r"^\s*queue\b", re.IGNORECASE)
        for i, line in enumerate(new_lines):
            if queue_re.match(line):
                insert_idx = i
                break
        if insert_idx is None:
            new_lines.append(new_line)
        else:
            new_lines.insert(insert_idx, new_line)
        changed = True

    if changed:
        jdl_path.write_text("\n".join(new_lines) + "\n")
    return changed


def update_missing_jdls_request_memory(missing_jdls: List[Path], memory_mb: int) -> None:
    if memory_mb <= 0:
        return
    updated = 0
    for jdl in missing_jdls:
        try:
            if update_jdl_request_memory(jdl, memory_mb):
                updated += 1
        except Exception as exc:
            print(f"[WARN] Failed to update request_memory for {jdl.name}: {exc}")
    print(f"[INFO] request_memory  : {memory_mb} MB")
    print(f"[INFO] jdl updated     : {updated}/{len(missing_jdls)}")


def resubmit_missing(cjobs_dir: Path, missing_jdls: List[Path]) -> None:
    if not missing_jdls:
        print("[INFO] No missing jobs to resubmit.")
        return

    print(f"[INFO] Resubmitting {len(missing_jdls)} missing job(s)...")
    for jdl in missing_jdls:
        cmd = f"condor_submit -terse {jdl.name}"
        proc = subprocess.run(
            ["bash", "-lc", cmd],
            cwd=str(cjobs_dir),
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        if proc.returncode != 0:
            print(f"[ERROR] Failed to resubmit {jdl.name}")
            if proc.stderr.strip():
                print(proc.stderr.strip())
            continue
        cluster = proc.stdout.strip().splitlines()[-1] if proc.stdout.strip() else ""
        if cluster:
            print(f"[INFO] {jdl.name} -> {cluster}")
        else:
            print(f"[INFO] {jdl.name} submitted")


def main() -> int:
    args = parse_args()

    cjobs_dir = Path(args.cjobs_dir).expanduser().resolve()
    eos_dir = Path(args.eos_output_dir).expanduser().resolve()

    if not cjobs_dir.exists() or not cjobs_dir.is_dir():
        print(f"[ERROR] cjobs directory not found: {cjobs_dir}", file=sys.stderr)
        return 2
    if not eos_dir.exists() or not eos_dir.is_dir():
        print(f"[ERROR] eos output directory not found: {eos_dir}", file=sys.stderr)
        return 2

    if args.dataset and not args.total_events:
        print("[INFO] --dataset provided; enabling --total-events.")
        args.total_events = True

    if args.total_events and args.resubmit:
        print("[WARN] --total-events used with --resubmit. Resubmission is disabled for this run.")
    if args.request_memory_mb > 0 and not args.resubmit:
        print("[WARN] --request-memory-mb is set but --resubmit is not enabled. No .jdl will be modified.")

    jdl_files = get_jdl_files(cjobs_dir)
    print(f"[INFO] cjobs directory : {cjobs_dir}")
    print(f"[INFO] eos directory   : {eos_dir}")
    print(f"[INFO] .jdl count      : {len(jdl_files)}")

    if not jdl_files:
        print("[WARN] No .jdl files found. Nothing to check.")
        return 0

    expected_map, unresolved_jdls = build_expected_map(cjobs_dir, jdl_files)
    if unresolved_jdls:
        print(f"[WARN] Could not resolve expected output name for {len(unresolved_jdls)} .jdl file(s).")
        for j in unresolved_jdls[:10]:
            print(f"  - {j.name}")
        if len(unresolved_jdls) > 10:
            print(f"  ... and {len(unresolved_jdls) - 10} more")

    existing_root_files = get_existing_root_files(eos_dir)
    existing_roots = {p.name for p in existing_root_files}
    print(f"[INFO] existing roots  : {len(existing_roots)}")

    missing_roots = sorted([name for name in expected_map.keys() if name not in existing_roots])
    missing_jdls = [expected_map[name] for name in missing_roots]

    print(f"[INFO] missing roots   : {len(missing_roots)}")
    if missing_roots:
        for name in missing_roots:
            print(name)

    out_list = write_missing_list(cjobs_dir, missing_roots, missing_jdls)
    print(f"[INFO] missing list file: {out_list}")

    if args.total_events:
        try:
            total_events, files_in_chain, tree_used = sum_events_fast(existing_root_files, show_progress=True)
            print(f"[INFO] event tree      : {tree_used}")
            print(f"[INFO] files in chain : {format_with_dots(files_in_chain)}")
            if args.dataset:
                expected_events = query_dataset_expected_events(args.dataset)
                diff = expected_events - total_events
                print(f"[INFO] dataset         : {args.dataset}")
                print(f"[INFO] expected events : {format_with_dots(expected_events)}")
                print(f"[INFO] counted events  : {format_with_dots(total_events)}")
                print(f"[INFO] difference      : {format_with_dots(diff)}")
            else:
                print(f"[INFO] counted events  : {format_with_dots(total_events)}")
        except Exception as exc:
            print(f"[ERROR] failed to sum events: {exc}", file=sys.stderr)
            return 3

    if args.resubmit and not args.total_events:
        if args.request_memory_mb > 0:
            update_missing_jdls_request_memory(missing_jdls, args.request_memory_mb)
        resubmit_missing(cjobs_dir, missing_jdls)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())

