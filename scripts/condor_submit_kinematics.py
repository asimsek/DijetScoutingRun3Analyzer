#!/usr/bin/env python3

from __future__ import annotations

import argparse
from datetime import datetime
import json
import os
from pathlib import Path
import shlex
import subprocess
import tempfile

from eos_utils import normalize_path, parse_remote, resolve_condor_proxy
from condor_submit_jobs import write_batch_submission


DEFAULT_REQUEST_MEMORY_MB = 4096
DEFAULT_THREADS = 4
DEFAULT_EOS_OUTPUT_BASE = "/eos/uscms/store/group/lpcjj/Run3PFScouting/Kinematics/2024"
RUNTIME_INPUT_DIR = "_job_inputs"


def log_info(message: str) -> None:
    print(f"[INFO] {message}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Submit one C++ kinematics Condor job per plot.",
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("--data-list", help="Local data input list")
    parser.add_argument("--mc-list", help="Local MC input list (source and cross section per line)")
    parser.add_argument("--lumi-pb", type=float, required=True, help="Integrated luminosity in pb^-1")
    parser.add_argument("--threads", type=int, default=DEFAULT_THREADS, help="C++ file-processing threads per job")
    parser.add_argument("--request-memory-mb", type=int, default=DEFAULT_REQUEST_MEMORY_MB)
    parser.add_argument("--plots", default="all", help="Comma-separated C++ plot names, or 'all'")
    parser.add_argument("--output-prefix", default="", help="Output filename prefix")
    parser.add_argument("--eos-output-base", default=DEFAULT_EOS_OUTPUT_BASE, help="EOS output base path or root:// URL")
    parser.add_argument("--workdir", help="Directory for generated Condor files")
    parser.add_argument("--cmssw-base", default=os.environ.get("CMSSW_BASE", ""))
    parser.add_argument("--scram-arch", default=os.environ.get("SCRAM_ARCH", "el9_amd64_gcc12"))
    parser.add_argument("--job-flavour", default="nextweek", help="CERN Condor JobFlavour")
    parser.add_argument("--proxy", help="Active proxy file; defaults to X509_USER_PROXY or voms-proxy-info -path. /tmp proxies are automatically staged on shared storage.")
    parser.add_argument("--shared-dir", help="Shared proxy and credential staging directory; defaults to CONDOR_SHARED_DIR, CERN work/private, or LPC home/private.")
    parser.add_argument("--resume", action="store_true", help="Opt in to duplicate checks (default: submit all selected plots in one call).")
    parser.add_argument("--no-submit", action="store_true", help="Create job files without submitting")
    return parser.parse_args()


def get_plot_metadata(repo_root: Path) -> dict:
    """Read names/defaults from the C++ source rather than duplicating them in Python."""
    flags = shlex.split(subprocess.check_output(["root-config", "--cflags", "--libs"], text=True))
    compiler = shlex.split(os.environ.get("CXX", "c++"))
    with tempfile.TemporaryDirectory(prefix="kinematics_metadata_") as directory:
        executable = Path(directory) / "plot_kinematics"
        subprocess.run(compiler + ["-O2", str(repo_root / "scripts/plot_kinematics.cpp"),
                                   "-o", str(executable)] + flags, check=True)
        return json.loads(subprocess.check_output([str(executable), "--describe"], text=True))


def resolve_plot_names(raw_value: str, available: list[str]) -> list[str]:
    names = available if raw_value == "all" else list(dict.fromkeys(name.strip() for name in raw_value.split(",") if name.strip()))
    unknown = [name for name in names if name not in available]
    if not names or unknown:
        raise RuntimeError(f"Unknown or empty plot selection: {raw_value}. Available: {', '.join(available)}")
    return names


def format_tag_number(value: object) -> str:
    return f"{float(value):g}".replace(".", "p").replace("-", "m")


def make_run_label(metadata: dict) -> str:
    timestamp = datetime.now().strftime("%d%B%Y_%H")
    return (f"plots_deta{format_tag_number(metadata['delta_eta_cut'])}_"
            f"M{format_tag_number(metadata['min_mjj'])}GeV_{timestamp}")


def prepare_input_list(source: str, destination: Path, repo_root: Path, cmssw_base: Path, mc: bool) -> None:
    source_path = Path(source).expanduser().resolve()
    runtime_dir = repo_root / "scripts" / RUNTIME_INPUT_DIR
    lines = []
    for raw_line in source_path.read_text().splitlines():
        line = raw_line.strip()
        if not line or line.startswith("#"):
            continue
        fields = line.split(maxsplit=1) if mc else [line]
        if mc and len(fields) != 2:
            raise RuntimeError(f"MC input requires a source and cross section: {line}")
        token = os.path.expandvars(os.path.expanduser(fields[0]))
        if parse_remote(token) or "://" in token:
            token = normalize_path(token)
        else:
            candidate = Path(token)
            if not candidate.is_absolute():
                candidate = source_path.parent / candidate
            candidate = candidate.resolve()
            try:
                candidate.relative_to(cmssw_base)
            except ValueError as exc:
                raise RuntimeError(f"Local job input must be inside CMSSW_BASE or use an XRootD URL: {candidate}") from exc
            token = os.path.relpath(candidate, runtime_dir)
        lines.append(token + (" " + fields[1] if mc else ""))
    if not lines:
        raise RuntimeError(f"Input list is empty: {source_path}")
    destination.write_text("\n".join(lines) + "\n")


def build_cmssw_tarball(cmssw_base: Path, workdir: Path) -> str:
    cmssw_ver = cmssw_base.name
    log_info(f"Creating CMSSW tarball: {workdir / (cmssw_ver + '.tar.gz')}")
    subprocess.run(["tar", "--exclude-vcs", "--exclude=*.tar.gz", "--exclude=dijetCondor",
                    "--exclude=Limits", "--exclude=cjobs_*", "-czf", str(workdir / f"{cmssw_ver}.tar.gz"),
                    "-C", str(cmssw_base.parent), cmssw_ver], check=True)
    return cmssw_ver


def build_cpp_command(args: argparse.Namespace, plot_name: str, input_names: dict[str, str]) -> list[str]:
    command = ["./plot_kinematics", "--plot", plot_name, "--lumi-pb", str(args.lumi_pb),
               "--threads", str(args.threads), "--output-dir", "."]
    if args.output_prefix:
        command += ["--output-prefix", args.output_prefix]
    for option, filename in input_names.items():
        command += [option, f"{RUNTIME_INPUT_DIR}/{filename}"]
    return command


def build_shell_script(shell_path: Path, *, args: argparse.Namespace, cmssw_ver: str,
                       eos_output_dir: str, output_files: list[str], cpp_command: list[str],
                       input_names: dict[str, str]) -> None:
    destination = parse_remote(eos_output_dir)
    if destination is None:
        raise RuntimeError("--eos-output-base must identify an EOS path or root:// URL")
    links = [f'ln -sf "$job_scratch"/{shlex.quote(name)} {RUNTIME_INPUT_DIR}/{shlex.quote(name)}'
             for name in input_names.values()]
    stat = "xrdfs " + shlex.quote(destination.endpoint) + " stat -q IsDir " + shlex.quote(destination.path)
    mkdir = "xrdfs " + shlex.quote(destination.endpoint) + " mkdir -p " + shlex.quote(destination.path)
    transfers = [f"if ! {stat} >/dev/null 2>&1; then", f"  {mkdir} || {stat}", "fi"]
    transfers += ["xrdcp -f " + shlex.quote(name) + " " + shlex.quote(destination.url + "/" + name)
                  for name in output_files]
    shell_path.write_text(f'''#!/usr/bin/env bash
set -eo pipefail
export VO_CMS_SW_DIR="${{VO_CMS_SW_DIR:-/cvmfs/cms.cern.ch}}"
export SCRAM_ARCH={shlex.quote(args.scram_arch)}
source "${{VO_CMS_SW_DIR}}/cmsset_default.sh"
job_scratch="${{_CONDOR_SCRATCH_DIR:-${{CONDOR_SCRATCH_DIR:-$PWD}}}}"
cd "$job_scratch"
tar -xzf {shlex.quote(cmssw_ver)}.tar.gz
rm -f {shlex.quote(cmssw_ver)}.tar.gz
cd {shlex.quote(cmssw_ver)}/src
scramv1 b ProjectRename
eval "$(scramv1 runtime -sh)"
cd DijetScoutingRun3Analyzer/scripts
c++ -O3 -o plot_kinematics plot_kinematics.cpp $(root-config --cflags --libs)
mkdir -p {RUNTIME_INPUT_DIR}
{chr(10).join(links)}
{shlex.join(cpp_command)}
{chr(10).join(transfers)}
''')
    shell_path.chmod(0o755)


def condor_quote(value: str) -> str:
    return '"' + value.replace('\\', '\\\\').replace('"', '\\"') + '"'


def build_jdl(jdl_path: Path, *, job_tag: str, transfer_inputs: list[str], shell_name: str,
              args: argparse.Namespace) -> str:
    # Filename submit commands take literal paths, unlike +ClassAd string values.
    lines = ["universe = vanilla", f"batch_name = {jdl_path.parent.name}",
             '+DijetDataset = "plot_kinematics"', f"executable = {shell_name}",
             f"output = {job_tag}.stdout", f"error = {job_tag}.stderr", f"log = {job_tag}.log",
             "should_transfer_files = YES", "when_to_transfer_output = ON_EXIT",
             "transfer_input_files = " + ", ".join(transfer_inputs),
             f"request_cpus = {max(args.threads, 1)}", f"request_memory = {args.request_memory_mb}MB",
             "use_x509userproxy = true"]
    lines.append("x509userproxy = " + args.proxy)
    if args.job_flavour:
        lines.append("+JobFlavour = " + condor_quote(args.job_flavour))
    description = "\n".join(lines + ["queue 1"]) + "\n"
    jdl_path.write_text(description)
    return description


def create_submit_all(workdir: Path, jdl_names: list[str]) -> None:
    submit_path = workdir / "submit_all.py"
    helper = Path(__file__).resolve().parent / "condor_submit_jobs.py"
    (workdir / "prepared_jobs.txt").write_text("\n".join(jdl_names) + "\n")
    submit_path.write_text(f'''#!/usr/bin/env python3
import os
from pathlib import Path
import subprocess
import sys

if __name__ == "__main__":
    env = os.environ.copy()
    env.pop("PYTHONHOME", None)
    env.pop("PYTHONPATH", None)
    raise SystemExit(subprocess.call([sys.executable, {str(helper)!r},
                     str(Path(__file__).resolve().parent), "--jdl-list", "prepared_jobs.txt",
                     "--batch-file", "prepared_batch.sub"] + sys.argv[1:], env=env))
''')
    submit_path.chmod(0o755)


def main() -> int:
    args = parse_args()
    if not args.data_list and not args.mc_list:
        raise RuntimeError("At least one of --data-list or --mc-list must be provided.")
    if not args.cmssw_base:
        raise RuntimeError("CMSSW_BASE is not set. Pass --cmssw-base explicitly.")
    if args.threads < 1 or args.request_memory_mb < 1:
        raise RuntimeError("--threads and --request-memory-mb must be positive.")
    if Path(args.output_prefix).name != args.output_prefix and args.output_prefix:
        raise RuntimeError("--output-prefix must be a filename prefix without directories.")
    script_dir = Path(__file__).resolve().parent
    repo_root = script_dir.parent
    cmssw_base = Path(args.cmssw_base).expanduser().resolve()
    if repo_root != cmssw_base / "src" / "DijetScoutingRun3Analyzer":
        raise RuntimeError("Run this checkout from CMSSW_BASE/src/DijetScoutingRun3Analyzer.")
    if args.shared_dir:
        os.environ["CONDOR_SHARED_DIR"] = args.shared_dir
    args.proxy = resolve_condor_proxy(args.proxy)
    log_info(f"Submission proxy: {args.proxy}")
    metadata = get_plot_metadata(repo_root)
    plot_names = resolve_plot_names(args.plots, metadata["plots"])
    run_label = make_run_label(metadata)
    base = parse_remote(args.eos_output_base)
    if base is None:
        raise RuntimeError("--eos-output-base must identify an EOS path or root:// URL")
    eos_output_dir = base.url.rstrip("/") + "/" + run_label
    workdir = Path(args.workdir).expanduser() if args.workdir else script_dir / f"cjobs_plot_kinematics_{run_label}"
    if not workdir.is_absolute():
        workdir = script_dir / workdir
    workdir = workdir.resolve()
    workdir.mkdir(parents=True, exist_ok=True)
    input_names = {}
    for option, source, filename, mc in (("--data-list", args.data_list, "data_inputs.txt", False),
                                         ("--mc-list", args.mc_list, "mc_inputs.txt", True)):
        if source:
            prepare_input_list(source, workdir / filename, repo_root, cmssw_base, mc)
            input_names[option] = filename
    cmssw_ver = build_cmssw_tarball(cmssw_base, workdir)
    log_info(f"Workdir: {workdir}")
    log_info(f"EOS output directory: {eos_output_dir}")
    jdl_names = []
    descriptions = []
    for plot_name in plot_names:
        stem = args.output_prefix + plot_name
        output_files = [f"{stem}.root"] + [f"{stem}.{fmt}" for fmt in metadata["output_formats"]]
        shell_path, jdl_path = workdir / f"run_{plot_name}.sh", workdir / f"run_{plot_name}.jdl"
        build_shell_script(shell_path, args=args, cmssw_ver=cmssw_ver, eos_output_dir=eos_output_dir,
                           output_files=output_files, cpp_command=build_cpp_command(args, plot_name, input_names),
                           input_names=input_names)
        transfers = [str(workdir / f"{cmssw_ver}.tar.gz"), str(shell_path)] + [str(workdir / name) for name in input_names.values()]
        description = build_jdl(jdl_path, job_tag=f"run_{plot_name}", transfer_inputs=transfers, shell_name=shell_path.name, args=args)
        descriptions.append((jdl_path.name, description))
        jdl_names.append(jdl_path.name)
        log_info(f"Prepared C++ job: {plot_name}")
    write_batch_submission(workdir, descriptions, args.proxy)
    create_submit_all(workdir, jdl_names)
    if args.no_submit:
        log_info("Job files prepared; submission skipped (--no-submit).")
    else:
        env = os.environ.copy()
        env.pop("PYTHONHOME", None)
        env.pop("PYTHONPATH", None)
        result = subprocess.run(["python3", "submit_all.py"] + (["--resume"] if args.resume else []), cwd=workdir, env=env)
        if result.returncode:
            log_info(f"Submission stopped. Prepared jobs remain in {workdir}; rerun its submit_all.py with --resume to continue.")
            return result.returncode
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
