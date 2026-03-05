#!/usr/bin/env python3

from __future__ import annotations

import argparse
import os
import shlex
import subprocess
from datetime import datetime
from pathlib import Path

from plot_kinematics import DEFAULT_OUTPUT_FORMATS, PLOT_SPECS


DEFAULT_REQUEST_MEMORY_MB = 4096
DEFAULT_THREADS = 4
DEFAULT_TREE_PATH = "rootTupleTree/tree"
DEFAULT_EOS_OUTPUT_BASE = "/eos/uscms/store/group/lpcjj/Run3PFScouting/Kinematics/2024"
EOS_REDIRECTOR = "root://cmseos.fnal.gov"
PLOT_JOB_SPECS = {name: {"plot": name} for name in PLOT_SPECS}
RUNTIME_INPUT_DIR = "_job_inputs"


def log_info(message: str) -> None:
    print(f"[INFO] {message}")


def log_cmd(message: str) -> None:
    print(f"[CMD] {message}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Submit one Condor job per kinematic plot spec.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument("--data-list", help="Data list passed to plot_kinematics.py")
    parser.add_argument("--mc-list", help="MC list passed to plot_kinematics.py")
    parser.add_argument("--lumi-pb", type=float, required=True, help="Integrated luminosity in pb^-1")
    parser.add_argument("--threads", type=int, default=DEFAULT_THREADS, help="ROOT implicit MT threads per job")
    parser.add_argument("--request-memory-mb", type=int, default=DEFAULT_REQUEST_MEMORY_MB, help="Condor memory request")
    parser.add_argument("--plots", default="all", help="Comma-separated subset of plot spec names, or 'all'")
    parser.add_argument("--output-prefix", default="", help="Filename prefix passed to plot_kinematics.py")
    parser.add_argument("--workdir", default=None, help="Directory for generated Condor files")
    parser.add_argument("--cmssw-base", default=os.environ.get("CMSSW_BASE", ""), help="CMSSW base path used to setup runtime")
    parser.add_argument("--scram-arch", default=os.environ.get("SCRAM_ARCH", "el9_amd64_gcc12"), help="SCRAM_ARCH for runtime")
    parser.add_argument("--no-submit", action="store_true", help="Create job files without submitting")
    return parser.parse_args()


def resolve_plot_names(raw_value: str) -> list[str]:
    if raw_value == "all":
        return list(PLOT_JOB_SPECS)
    names = [name.strip() for name in raw_value.split(",") if name.strip()]
    unknown = [name for name in names if name not in PLOT_JOB_SPECS]
    if unknown:
        raise RuntimeError(f"Unknown plot job spec(s): {', '.join(unknown)}")
    return names


def make_timestamp() -> str:
    return datetime.now().strftime("%d%B%Y_%H")


def eos_to_xrd_path(path: str) -> str:
    if path.startswith("/eos/uscms/"):
        return "/" + path[len("/eos/uscms/"):]
    if path.startswith("/store/"):
        return path
    raise RuntimeError(f"Unsupported EOS path format: {path}")


def build_cmssw_tarball(cmssw_base: Path, workdir: Path) -> str:
    cmssw_ver = cmssw_base.name
    tar_file = workdir / f"{cmssw_ver}.tar.gz"
    log_info(f"Creating CMSSW tarball: {tar_file}")
    log_cmd(f"tar -czf {tar_file} ...")
    subprocess.run(
        [
            "tar",
            "--exclude-vcs",
            "-czf",
            str(tar_file),
            "--exclude=*.tar.gz",
            "--exclude=dijetCondor",
            "--exclude=Limits",
            "--exclude=cjobs_*",
            "-C",
            str(cmssw_base.parent),
            cmssw_ver,
        ],
        check=True,
    )
    return cmssw_ver


def resolve_runtime_input(path_str: str, repo_root: Path, cmssw_base: Path) -> tuple[str, Path | None]:
    resolved = Path(path_str).resolve()
    cmssw_src = cmssw_base / "src"

    try:
        rel_repo = resolved.relative_to(repo_root)
        return (Path("..") / rel_repo).as_posix(), None
    except ValueError:
        pass

    try:
        rel_src = resolved.relative_to(cmssw_src)
        return (Path("..") / ".." / rel_src).as_posix(), None
    except ValueError:
        pass

    return f"{RUNTIME_INPUT_DIR}/{resolved.name}", resolved


def build_python_command(
    args: argparse.Namespace,
    plot_name: str,
    repo_root: Path,
    cmssw_base: Path,
) -> tuple[list[str], list[Path]]:
    cmd = [
        "python3",
        "plot_kinematics.py",
        "--plot",
        plot_name,
        "--lumi-pb",
        str(args.lumi_pb),
        "--threads",
        str(args.threads),
        "--output-dir",
        ".",
    ]
    extra_transfers: list[Path] = []
    if args.output_prefix:
        cmd.extend(["--output-prefix", args.output_prefix])
    if args.data_list:
        runtime_path, transfer_path = resolve_runtime_input(args.data_list, repo_root, cmssw_base)
        cmd.extend(["--data-list", runtime_path])
        if transfer_path is not None:
            extra_transfers.append(transfer_path)
    if args.mc_list:
        runtime_path, transfer_path = resolve_runtime_input(args.mc_list, repo_root, cmssw_base)
        cmd.extend(["--mc-list", runtime_path])
        if transfer_path is not None:
            extra_transfers.append(transfer_path)
    return cmd, extra_transfers


def build_shell_script(
    shell_path: Path,
    *,
    args: argparse.Namespace,
    cmssw_ver: str,
    eos_output_dir: str,
    output_files: list[str],
    python_cmd: list[str],
    extra_transfer_names: list[str],
) -> None:
    if not args.cmssw_base:
        raise RuntimeError("CMSSW_BASE is not set. Pass --cmssw-base explicitly.")
    quoted_python = " ".join(shlex.quote(part) for part in python_cmd)
    link_lines = []
    if extra_transfer_names:
        link_lines.append(f'mkdir -p {RUNTIME_INPUT_DIR}')
        for name in extra_transfer_names:
            link_lines.append(
                f'ln -sf "$CONDOR_SCRATCH_DIR"/{shlex.quote(name)} {RUNTIME_INPUT_DIR}/{shlex.quote(name)}'
            )
    link_block = "\n".join(link_lines)
    if link_block:
        link_block += "\n"
    xrd_output_dir = eos_to_xrd_path(eos_output_dir)
    transfer_lines = [
        f'xrdfs {EOS_REDIRECTOR} mkdir -p {shlex.quote(xrd_output_dir)}',
    ]
    for output_file in output_files:
        transfer_lines.append(
            f'xrdcp -f {shlex.quote(output_file)} {EOS_REDIRECTOR}/{xrd_output_dir.rstrip("/")}/{shlex.quote(output_file)}'
        )
    transfer_block = "\n".join(transfer_lines)
    content = f"""#!/bin/bash
	set -eo pipefail
	
	export VO_CMS_SW_DIR="${{VO_CMS_SW_DIR:-/cvmfs/cms.cern.ch}}"
	export SCRAM_ARCH={shlex.quote(args.scram_arch)}
	source "${{VO_CMS_SW_DIR}}/cmsset_default.sh"
	cd "$CONDOR_SCRATCH_DIR"
	tar -xzf {shlex.quote(cmssw_ver)}.tar.gz
	rm -f {shlex.quote(cmssw_ver)}.tar.gz
	cd {shlex.quote(cmssw_ver)}/src
scramv1 b ProjectRename
eval "$(scramv1 runtime -sh)"
cd DijetScoutingRun3Analyzer/scripts
{link_block}{quoted_python}
{transfer_block}
"""
    shell_path.write_text(content)
    shell_path.chmod(0o755)


def build_jdl(
    jdl_path: Path,
    *,
    job_tag: str,
    transfer_inputs: list[str],
    shell_name: str,
    output_files: list[str],
    args: argparse.Namespace,
) -> None:
    transfer_input = ", ".join(transfer_inputs)
    lines = [
        "universe = vanilla",
        f"executable = {shell_name}",
        f"output = {job_tag}.stdout",
        f"error = {job_tag}.stderr",
        f"log = {job_tag}.log",
        "should_transfer_files = YES",
        "when_to_transfer_output = ON_EXIT",
        f"transfer_input_files = {transfer_input}",
        f"request_cpus = {max(args.threads, 1)}",
        f"request_memory = {args.request_memory_mb}MB",
        "queue 1",
    ]
    jdl_path.write_text("\n".join(lines) + "\n")


def create_submit_all(workdir: Path, jdl_names: list[str]) -> None:
    submit_path = workdir / "submit_all.py"
    quoted_jdls = ", ".join(repr(name) for name in jdl_names)
    submit_path.write_text(
        f"""#!/usr/bin/env python3
import os
import shlex
import subprocess

JDLS = [{quoted_jdls}]


def main():
    if not JDLS:
        return

    env = os.environ.copy()
    env.pop("PYTHONHOME", None)
    env.pop("PYTHONPATH", None)
    for jdl in JDLS:
        print(f"[CMD] condor_submit {{jdl}}")
        subprocess.run(
            ["/bin/bash", "--noprofile", "--norc", "-c", f"condor_submit -terse {{shlex.quote(jdl)}}"],
            env=env,
            check=True,
        )


if __name__ == "__main__":
    main()
"""
    )
    submit_path.chmod(0o755)


def submit_jobs(workdir: Path) -> None:
    log_info("Submitting jobs to Condor...")
    submit_env = os.environ.copy()
    submit_env.pop("PYTHONHOME", None)
    submit_env.pop("PYTHONPATH", None)
    log_cmd("python3 submit_all.py")
    subprocess.run(
        ["python3", "submit_all.py"],
        cwd=str(workdir),
        env=submit_env,
        check=True,
    )


def main() -> int:
    args = parse_args()

    if not args.data_list and not args.mc_list:
        raise RuntimeError("At least one of --data-list or --mc-list must be provided.")
    if not args.cmssw_base:
        raise RuntimeError("CMSSW_BASE is not set. Pass --cmssw-base explicitly.")

    script_dir = Path(__file__).resolve().parent
    repo_root = script_dir.parent
    cmssw_base = Path(args.cmssw_base).resolve()
    plot_names = resolve_plot_names(args.plots)
    timestamp = make_timestamp()
    eos_output_dir = f"{DEFAULT_EOS_OUTPUT_BASE.rstrip('/')}/{timestamp}"

    if args.workdir:
        requested_workdir = Path(args.workdir)
        workdir = requested_workdir if requested_workdir.is_absolute() else script_dir / requested_workdir
        workdir = workdir.resolve()
    else:
        workdir = (script_dir / f"cjobs_plot_kinematics_{timestamp}").resolve()
    workdir.mkdir(parents=True, exist_ok=True)
    cmssw_ver = build_cmssw_tarball(cmssw_base, workdir)

    log_info(f"Workdir          : {workdir}")
    log_info(f"Plot jobs        : {len(plot_names)}")
    log_info(f"Tree             : {DEFAULT_TREE_PATH}")
    log_info(f"EOS output dir   : {eos_output_dir}")

    jdl_names: list[str] = []
    for plot_name in plot_names:
        python_cmd, extra_transfer_paths = build_python_command(args, plot_name, repo_root, cmssw_base)
        stem = f"{args.output_prefix}{plot_name}" if args.output_prefix else plot_name
        output_files = [f"{stem}.root"] + [f"{stem}.{fmt}" for fmt in DEFAULT_OUTPUT_FORMATS]
        shell_path = workdir / f"run_{plot_name}.sh"
        jdl_path = workdir / f"run_{plot_name}.jdl"
        extra_transfer_names = [path.name for path in extra_transfer_paths]
        transfer_inputs = [str(workdir / f"{cmssw_ver}.tar.gz"), str(shell_path)] + [str(path) for path in extra_transfer_paths]

        build_shell_script(
            shell_path,
            args=args,
            cmssw_ver=cmssw_ver,
            eos_output_dir=eos_output_dir,
            output_files=output_files,
            python_cmd=python_cmd,
            extra_transfer_names=extra_transfer_names,
        )
        build_jdl(
            jdl_path,
            job_tag=f"run_{plot_name}",
            transfer_inputs=transfer_inputs,
            shell_name=shell_path.name,
            output_files=output_files,
            args=args,
        )

        log_info(f"Prepared job      : {plot_name}")
        jdl_names.append(jdl_path.name)

    create_submit_all(workdir, jdl_names)

    if args.no_submit:
        log_info("Dry run completed. No jobs were submitted.")
    else:
        submit_jobs(workdir)
        log_info("All jobs submitted.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

