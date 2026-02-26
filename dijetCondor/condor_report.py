#!/usr/bin/env python3

import argparse
import bisect
import json
import os
import re
import shutil
import subprocess
import sys
import time
from pathlib import Path
from typing import Dict, Iterable, List, Optional, Sequence, Tuple


RUN_KEYS = {
    "run",
    "runnum",
    "run_number",
    "runnumber",
}

LUMI_KEYS = {
    "lumi",
    "lumis",
    "luminum",
    "lumi_number",
    "lumi_section_num",
    "lumisection",
    "lumisectionnum",
    "number",
}


class _SimpleProgress:
    def __init__(self, total: int, desc: str, unit: str = "it") -> None:
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


def make_progress(total: int, desc: str, unit: str = "item"):
    try:
        from tqdm import tqdm  # type: ignore

        return tqdm(total=total, desc=desc, unit=unit)
    except Exception:
        return _SimpleProgress(total=total, desc=desc, unit=unit)


def format_with_dots(value: int) -> str:
    sign = "-" if value < 0 else ""
    return f"{sign}{abs(value):,}".replace(",", ".")


def parse_key_value_file(path: Path) -> Dict[str, str]:
    out: Dict[str, str] = {}
    for line in path.read_text().splitlines():
        s = line.strip()
        if not s or s.startswith("#") or "=" not in s:
            continue
        k, v = s.split("=", 1)
        out[k.strip()] = v.strip()
    return out


def parse_config(config_path: Path) -> Tuple[str, Optional[Path]]:
    kv = parse_key_value_file(config_path)
    if "Dataset" not in kv:
        raise RuntimeError(f"Missing Dataset in config: {config_path}")
    dataset = kv["Dataset"]

    golden = kv.get("goldenJSON", "").strip()
    if not golden:
        return dataset, None

    golden_path = Path(golden)
    if not golden_path.is_absolute():
        # Preferred behavior for cfg-provided relative goldenJSON:
        # resolve from one directory above dijetCondor, i.e. "../<goldenJSON>".
        condor_dir = Path(__file__).resolve().parent
        analyzer_root = (condor_dir / "..").resolve()
        candidates = [
            analyzer_root / golden,
            Path.cwd() / golden,
            config_path.parent / golden,
            config_path.parent.parent / golden,
        ]
        for c in candidates:
            if c.exists():
                return dataset, c.resolve()
        # Keep analyzer-root as the default fallback for relative goldenJSON paths.
        return dataset, (analyzer_root / golden).resolve()
    return dataset, golden_path.resolve()


def run_cmd(
    cmd: Sequence[str],
    capture: bool = True,
    env: Optional[Dict[str, str]] = None,
) -> subprocess.CompletedProcess:
    kwargs = dict(text=True, check=True)
    if capture:
        kwargs["stdout"] = subprocess.PIPE
        kwargs["stderr"] = subprocess.PIPE
    if env is not None:
        kwargs["env"] = env
    return subprocess.run(list(cmd), **kwargs)


def run_cmd_optional(cmd: Sequence[str]) -> Optional[subprocess.CompletedProcess]:
    try:
        return run_cmd(cmd, capture=True)
    except subprocess.CalledProcessError:
        return None


def ensure_dasgoclient() -> None:
    if shutil.which("dasgoclient") is None:
        raise RuntimeError("dasgoclient not found. Load CMSSW runtime first.")


def resolve_brilcalc() -> str:
    exe = shutil.which("brilcalc")
    if exe:
        return exe

    candidates = [
        Path.home() / ".local" / "bin" / "brilcalc",
        Path("/cvmfs/cms-bril.cern.ch/brilconda310/bin/brilcalc"),
        Path("/cvmfs/cms-bril.cern.ch/brilconda3/bin/brilcalc"),
        Path("/cvmfs/cms-bril.cern.ch/brilconda/bin/brilcalc"),
    ]
    for p in candidates:
        if p.exists() and p.is_file():
            return str(p)

    raise RuntimeError(
        "brilcalc not found in PATH or common CVMFS locations. "
        "Try one of:\n"
        "  1) source /cvmfs/cms-bril.cern.ch/brilconda3/bin/activate\n"
        "  2) python3 -m pip install --user brilws && export PATH=$HOME/.local/bin:$PATH"
    )


def has_brilcalc_connection_arg(extra_args: Sequence[str]) -> bool:
    for i, arg in enumerate(extra_args):
        if arg in ("-c", "--connect", "--connect-string"):
            return i < (len(extra_args) - 1)
        if arg.startswith("-c=") or arg.startswith("--connect=") or arg.startswith("--connect-string="):
            return True
    return False


def resolve_site_local_config() -> Optional[str]:
    candidates: List[Path] = []
    cms_path = os.environ.get("CMS_PATH", "").strip()
    if cms_path:
        candidates.append(Path(cms_path) / "SITECONF" / "local" / "JobConfig" / "site-local-config.xml")
    candidates.append(Path("/cvmfs/cms.cern.ch/SITECONF/local/JobConfig/site-local-config.xml"))
    for p in candidates:
        if p.exists() and p.is_file():
            return str(p)
    return None


def flatten_ints(value) -> List[int]:
    out: List[int] = []
    if isinstance(value, bool) or value is None:
        return out
    if isinstance(value, (int, float)):
        out.append(int(value))
        return out
    if isinstance(value, str):
        s = value.strip()
        if not s:
            return out
        # Accept "1,2,3", "1 2 3", "1-3", "[1,2,3]" and similar.
        # Range-like tokens "a-b" are expanded.
        chunks = re.split(r"[\s,;]+", s)
        for c in chunks:
            c = c.strip("[](){}")
            if not c:
                continue
            m = re.fullmatch(r"(\d+)-(\d+)", c)
            if m:
                a = int(m.group(1))
                b = int(m.group(2))
                if a <= b and (b - a) <= 100000:
                    out.extend(range(a, b + 1))
                elif b < a and (a - b) <= 100000:
                    out.extend(range(b, a + 1))
                continue
            if c.isdigit():
                out.append(int(c))
        return out
    if isinstance(value, list):
        for item in value:
            out.extend(flatten_ints(item))
        return out
    if isinstance(value, dict):
        for _, item in value.items():
            out.extend(flatten_ints(item))
    return out


def extract_run(value) -> Optional[int]:
    if isinstance(value, bool) or value is None:
        return None
    if isinstance(value, (int, float)):
        r = int(value)
        return r if r > 0 else None
    if isinstance(value, str):
        s = value.strip()
        if not s:
            return None
        if s.isdigit():
            r = int(s)
            return r if r > 0 else None
        m = re.search(r"\d+", s)
        if m:
            r = int(m.group(0))
            return r if r > 0 else None
    return None


def collect_runs(value) -> List[int]:
    out: List[int] = []
    if isinstance(value, dict):
        for k, v in value.items():
            if k.lower() in RUN_KEYS:
                r = extract_run(v)
                if r is not None:
                    out.append(r)
            out.extend(collect_runs(v))
        return out
    if isinstance(value, list):
        for item in value:
            out.extend(collect_runs(item))
        return out
    return out


def merge_ranges(ranges: List[Tuple[int, int]]) -> List[Tuple[int, int]]:
    if not ranges:
        return []
    ranges.sort()
    merged: List[Tuple[int, int]] = [ranges[0]]
    for start, end in ranges[1:]:
        ps, pe = merged[-1]
        if start <= pe + 1:
            merged[-1] = (ps, max(pe, end))
        else:
            merged.append((start, end))
    return merged


def lumis_to_ranges(lumis: Iterable[int]) -> List[Tuple[int, int]]:
    vals = sorted(set(x for x in lumis if x > 0))
    if not vals:
        return []
    out: List[Tuple[int, int]] = []
    start = vals[0]
    prev = vals[0]
    for x in vals[1:]:
        if x == prev + 1:
            prev = x
        else:
            out.append((start, prev))
            start = x
            prev = x
    out.append((start, prev))
    return out


def add_run_lumis(run_to_ranges: Dict[int, List[Tuple[int, int]]], run: int, lumis: List[int]) -> None:
    if run <= 0 or not lumis:
        return
    new_ranges = lumis_to_ranges(lumis)
    if not new_ranges:
        return
    if run not in run_to_ranges:
        run_to_ranges[run] = new_ranges
        return
    run_to_ranges[run] = merge_ranges(run_to_ranges[run] + new_ranges)


def parse_object(obj, run_to_ranges: Dict[int, List[Tuple[int, int]]], current_run: Optional[int] = None) -> None:
    if isinstance(obj, list):
        for item in obj:
            parse_object(item, run_to_ranges, current_run=current_run)
        return

    if not isinstance(obj, dict):
        return

    local_run = current_run
    for k, v in obj.items():
        if k.lower() in RUN_KEYS:
            parsed = extract_run(v)
            if parsed is not None:
                local_run = parsed
                break

    # DAS can store run and lumi as sibling blocks; recover a single run
    # from nested structures if direct run key is not scalar.
    if local_run is None:
        inferred_runs = collect_runs(obj)
        inferred_unique = sorted(set(inferred_runs))
        if len(inferred_unique) == 1:
            local_run = inferred_unique[0]

    for k, v in obj.items():
        if k.lower() in LUMI_KEYS and local_run is not None:
            lumis = flatten_ints(v)
            add_run_lumis(run_to_ranges, local_run, lumis)

    if local_run is not None and "lumi" in obj and isinstance(obj["lumi"], dict):
        lumis = flatten_ints(obj["lumi"])
        add_run_lumis(run_to_ranges, local_run, lumis)

    for _, v in obj.items():
        parse_object(v, run_to_ranges, current_run=local_run)


def parse_das_payload(
    payload,
    run_to_ranges: Dict[int, List[Tuple[int, int]]],
    desc: str,
    forced_run: Optional[int] = None,
    show_progress: bool = True,
) -> None:
    if not isinstance(payload, list):
        return
    if show_progress:
        progress = make_progress(total=len(payload), desc=desc, unit="record")
        for item in payload:
            parse_object(item, run_to_ranges, current_run=forced_run)
            progress.update(1)
        progress.close()
    else:
        for item in payload:
            parse_object(item, run_to_ranges, current_run=forced_run)


def fetch_dataset_lumis_from_das(dataset: str) -> Dict[int, List[Tuple[int, int]]]:
    ensure_dasgoclient()
    run_to_ranges: Dict[int, List[Tuple[int, int]]] = {}

    # Try direct run+lumi queries first.
    direct_queries = [
        f"run,lumi dataset={dataset}",
        f"lumi,run dataset={dataset}",
        f"lumi dataset={dataset}",
    ]
    for q in direct_queries:
        cmd = ["dasgoclient", "-query", q, "-json"]
        print(f"[CMD] {' '.join(cmd)}")
        proc = run_cmd_optional(cmd)
        if proc is None:
            continue
        text = (proc.stdout or "").strip()
        if not text:
            continue
        try:
            payload = json.loads(text)
        except json.JSONDecodeError:
            continue
        parse_das_payload(payload, run_to_ranges, desc="Parsing DAS lumi")
        if run_to_ranges:
            return run_to_ranges

    # Fallback: query runs first, then lumis run-by-run.
    run_query_cmd = ["dasgoclient", "-query", f"run dataset={dataset}", "-json"]
    print(f"[CMD] {' '.join(run_query_cmd)}")
    run_proc = run_cmd_optional(run_query_cmd)
    if run_proc is None or not (run_proc.stdout or "").strip():
        raise RuntimeError(f"Failed to retrieve runs for dataset: {dataset}")

    try:
        run_payload = json.loads(run_proc.stdout)
    except json.JSONDecodeError as exc:
        raise RuntimeError("Failed to parse DAS run JSON payload.") from exc

    runs = sorted(set(collect_runs(run_payload)))
    if not runs:
        raise RuntimeError(f"No runs found for dataset: {dataset}")

    progress = make_progress(total=len(runs), desc="Querying lumis per run", unit="run")
    for run in runs:
        lumi_cmd = ["dasgoclient", "-query", f"lumi dataset={dataset} run={run}", "-json"]
        proc = run_cmd_optional(lumi_cmd)
        if proc is None:
            progress.update(1)
            continue
        text = (proc.stdout or "").strip()
        if not text:
            progress.update(1)
            continue
        try:
            payload = json.loads(text)
        except json.JSONDecodeError:
            progress.update(1)
            continue
        parse_das_payload(payload, run_to_ranges, desc=f"Run {run}", forced_run=run, show_progress=False)
        progress.update(1)
    progress.close()

    if not run_to_ranges:
        raise RuntimeError(
            "No run/lumi information could be extracted from DAS response. "
            "DAS returned lumi values without run mapping for this query mode."
        )
    return run_to_ranges


def load_golden_ranges(path: Path) -> Dict[int, List[Tuple[int, int]]]:
    payload = json.loads(path.read_text())
    if not isinstance(payload, dict):
        raise RuntimeError(f"Invalid golden JSON format: {path}")

    out: Dict[int, List[Tuple[int, int]]] = {}
    for run_str, ranges in payload.items():
        try:
            run = int(run_str)
        except ValueError:
            continue
        if not isinstance(ranges, list):
            continue
        local: List[Tuple[int, int]] = []
        for r in ranges:
            if not isinstance(r, list) or len(r) != 2:
                continue
            a = int(r[0])
            b = int(r[1])
            local.append((min(a, b), max(a, b)))
        if local:
            out[run] = merge_ranges(local)
    return out


def intersect_ranges(a: List[Tuple[int, int]], b: List[Tuple[int, int]]) -> List[Tuple[int, int]]:
    i = 0
    j = 0
    out: List[Tuple[int, int]] = []
    while i < len(a) and j < len(b):
        s = max(a[i][0], b[j][0])
        e = min(a[i][1], b[j][1])
        if s <= e:
            out.append((s, e))
        if a[i][1] < b[j][1]:
            i += 1
        else:
            j += 1
    return out


def apply_golden_mask(
    run_to_ranges: Dict[int, List[Tuple[int, int]]],
    golden: Dict[int, List[Tuple[int, int]]],
) -> Dict[int, List[Tuple[int, int]]]:
    out: Dict[int, List[Tuple[int, int]]] = {}
    for run, rr in run_to_ranges.items():
        gr = golden.get(run)
        if not gr:
            continue
        inter = intersect_ranges(rr, gr)
        if inter:
            out[run] = inter
    return out


def to_golden_json_dict(run_to_ranges: Dict[int, List[Tuple[int, int]]]) -> Dict[str, List[List[int]]]:
    out: Dict[str, List[List[int]]] = {}
    for run in sorted(run_to_ranges):
        out[str(run)] = [[a, b] for a, b in run_to_ranges[run]]
    return out


def count_lumisections(run_to_ranges: Dict[int, List[Tuple[int, int]]]) -> int:
    total = 0
    for _, ranges in run_to_ranges.items():
        for a, b in ranges:
            total += (b - a + 1)
    return total


def write_json(path: Path, payload: Dict[str, List[List[int]]]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n")


def resolve_output_json_path(output_arg: str) -> Path:
    out = Path(output_arg).expanduser()
    if out.is_absolute():
        return out.resolve()

    # Keep report JSON files under dijetCondor/reports when only a filename is given.
    if out.parent == Path("."):
        reports_dir = Path(__file__).resolve().parent / "reports_condor"
        return (reports_dir / out.name).resolve()

    return out.resolve()


def run_brilcalc(
    json_path: Path,
    unit: str,
    normtag: str,
    extra_args: Sequence[str],
) -> None:
    brilcalc_exe = resolve_brilcalc()
    cmd = [brilcalc_exe, "lumi", "-u", unit, "-i", str(json_path)]

    if not has_brilcalc_connection_arg(extra_args):
        site_cfg = resolve_site_local_config()
        if site_cfg:
            cmd.extend(["-c", site_cfg])
            print(f"[INFO] Using site-local config for brilcalc: {site_cfg}")
        else:
            print("[WARN] site-local-config.xml not found; running brilcalc without explicit -c.")

    if normtag:
        cmd.extend(["--normtag", normtag])
    if extra_args:
        cmd.extend(extra_args)
    print(f"[CMD] {' '.join(cmd)}")
    env = dict(os.environ)
    frontier_server = env.get("FRONTIER_SERVER", "").strip()
    if (not frontier_server) or ("/Frontier" not in frontier_server):
        env["FRONTIER_SERVER"] = (
            "(serverurl=http://cmsfrontier.cern.ch:8000/FrontierProd)"
            "(serverurl=http://cmsfrontier1.cern.ch:8000/FrontierProd)"
            "(serverurl=http://cmsfrontier2.cern.ch:8000/FrontierProd)"
        )
        print("[INFO] Using fallback FRONTIER_SERVER for brilcalc.")

    # If user runs a local brilcalc in web mode from within a CMSSW shell,
    # CMSSW PYTHONPATH/LD_LIBRARY_PATH can force frontier/oracle backends and fail.
    # Use a clean env so local brilws uses its own dependencies.
    is_web_mode = any(a.strip().lower() == "web" for a in extra_args)
    is_local_brilcalc = (str(Path(brilcalc_exe)).startswith(str(Path.home() / ".local")))
    if is_web_mode and is_local_brilcalc:
        env.pop("PYTHONPATH", None)
        env.pop("LD_LIBRARY_PATH", None)
        env["PATH"] = f"{Path.home() / '.local' / 'bin'}:{env.get('PATH', '')}"
        print("[INFO] Running local brilcalc in clean env for web mode.")
    run_cmd(cmd, capture=False, env=env)


def validate_brilcalc_extra(extra_args: Sequence[str]) -> None:
    misplaced = []
    if "--golden-json" in extra_args:
        misplaced.append("--golden-json")
    if "--no-golden" in extra_args:
        misplaced.append("--no-golden")
    if not misplaced:
        return
    joined = ", ".join(misplaced)
    raise RuntimeError(
        f"Option(s) {joined} belong to condor_report.py, not brilcalc. "
        "Place them before --brilcalc-extra (or remove them if not needed)."
    )


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Produce crab-like processed-input-lumi JSON from DAS using Dataset in a condor cfg, "
            "and optionally run brilcalc."
        )
    )
    parser.add_argument(
        "-c",
        "--config",
        default="",
        help="Condor config file containing Dataset=... (e.g. inputFiles_PFScouting_NanoAOD/PFScouting_2024H_cfg.txt).",
    )
    parser.add_argument(
        "--input-json",
        default="",
        help="Existing lumi JSON file. Use this to run brilcalc without a config file.",
    )
    parser.add_argument(
        "-o",
        "--output-json",
        default="processed_input_lumis.json",
        help=(
            'Output JSON path for generated report (default filename: "processed_input_lumis.json"). '
            "If only a filename is given, it is written under dijetCondor/reports_condor/."
        ),
    )
    parser.add_argument(
        "--no-golden",
        action="store_true",
        help="Do not apply goldenJSON even if defined in the config file.",
    )
    parser.add_argument(
        "--golden-json",
        nargs="?",
        const="__USE_CONFIG_GOLDEN__",
        default="",
        help=(
            "Optional override golden JSON path. "
            "If provided without a value, uses goldenJSON from the config file."
        ),
    )
    parser.add_argument(
        "--brilcalc",
        action="store_true",
        help="Run brilcalc on the generated JSON (or on --input-json).",
    )
    parser.add_argument(
        "--unit",
        default="/fb",
        help='brilcalc unit (default: "/fb").',
    )
    parser.add_argument(
        "--normtag",
        default="/cvmfs/cms-bril.cern.ch/cms-lumi-pog/Normtags/normtag_PHYSICS.json",
        help="Optional brilcalc normtag path.",
    )
    parser.add_argument(
        "--brilcalc-extra",
        nargs=argparse.REMAINDER,
        default=[],
        help=(
            "Extra args passed to brilcalc (use after --brilcalc-extra). "
            "If no -c/--connect is provided here, site-local config is used automatically."
        ),
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        validate_brilcalc_extra(args.brilcalc_extra)
    except RuntimeError as exc:
        print(f"[ERROR] {exc}", file=sys.stderr)
        return 2

    if bool(args.config) and bool(args.input_json):
        print("[ERROR] Use either --config or --input-json, not both.", file=sys.stderr)
        return 2
    if not args.config and not args.input_json:
        print("[ERROR] Provide one of: --config or --input-json.", file=sys.stderr)
        return 2

    t0 = time.monotonic()
    output_json_path: Optional[Path] = None

    if args.config:
        cfg_path = Path(args.config).expanduser().resolve()
        if not cfg_path.exists():
            print(f"[ERROR] Config not found: {cfg_path}", file=sys.stderr)
            return 2

        dataset, cfg_golden = parse_config(cfg_path)
        print(f"[INFO] Dataset          : {dataset}")
        run_to_ranges = fetch_dataset_lumis_from_das(dataset)
        before_runs = len(run_to_ranges)
        before_ls = count_lumisections(run_to_ranges)
        print(f"[INFO] DAS runs         : {format_with_dots(before_runs)}")
        print(f"[INFO] DAS lumisections : {format_with_dots(before_ls)}")

        golden_path: Optional[Path] = None
        if not args.no_golden:
            if args.golden_json and args.golden_json != "__USE_CONFIG_GOLDEN__":
                golden_path = Path(args.golden_json).expanduser().resolve()
            else:
                golden_path = cfg_golden

        if golden_path is not None:
            if not golden_path.exists():
                print(f"[ERROR] Golden JSON not found: {golden_path}", file=sys.stderr)
                return 2
            print(f"[INFO] Golden JSON      : {golden_path}")
            golden_ranges = load_golden_ranges(golden_path)
            run_to_ranges = apply_golden_mask(run_to_ranges, golden_ranges)
            after_runs = len(run_to_ranges)
            after_ls = count_lumisections(run_to_ranges)
            print(f"[INFO] Filtered runs    : {format_with_dots(after_runs)}")
            print(f"[INFO] Filtered lumis   : {format_with_dots(after_ls)}")

        payload = to_golden_json_dict(run_to_ranges)
        output_json_path = resolve_output_json_path(args.output_json)
        write_json(output_json_path, payload)
        print(f"[INFO] Output JSON       : {output_json_path}")

    else:
        output_json_path = Path(args.input_json).expanduser().resolve()
        if not output_json_path.exists():
            print(f"[ERROR] Input JSON not found: {output_json_path}", file=sys.stderr)
            return 2
        print(f"[INFO] Input JSON        : {output_json_path}")

    if args.brilcalc:
        run_brilcalc(
            json_path=output_json_path,
            unit=args.unit,
            normtag=args.normtag,
            extra_args=args.brilcalc_extra,
        )

    dt = max(0.0, time.monotonic() - t0)
    print(f"[INFO] Wall time [s]    : {dt:.1f}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

