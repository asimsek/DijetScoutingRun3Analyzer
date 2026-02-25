#!/usr/bin/env python3

import argparse
import os
import re
import shutil
import subprocess
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Tuple


DEFAULT_CMSSW_VER = "CMSSW_15_0_6"
DEFAULT_SCRAM_ARCH = "el9_amd64_gcc12"
DEFAULT_TREE_NAME = "Events"
DEFAULT_EOS_PATH = "/store/group/lpcjj/Run3PFScouting/nanoAODnTuples"
DEFAULT_CUT_FILE = "config/cutFile_mainDijetPFScoutingSelection_Run3.txt"
DEFAULT_REQUEST_MEMORY_MB = 4096
DEFAULT_SUBMIT_WORKERS = 8
DEFAULT_XROOTD_REDIRECTOR = "root://cmsxrootd.fnal.gov"
DEFAULT_CMS_CONNECT_IMAGE = "/cvmfs/unpacked.cern.ch/registry.hub.docker.com/cmssw/el9:x86_64"
#DEFAULT_XROOTD_REDIRECTOR = "root://cms-xrd-global.cern.ch"

COLOR_RESET = "\033[0m"
COLOR_BRIGHT_RED = "\033[91m"
COLOR_BRIGHT_YELLOW = "\033[93m"


def log_info(message: str) -> None:
    print(f"{COLOR_BRIGHT_YELLOW}[INFO]{COLOR_RESET} {message}")


def log_cmd(message: str) -> None:
    print(f"{COLOR_BRIGHT_RED}[CMD]{COLOR_RESET} {message}")


def log_warn(message: str) -> None:
    print(f"[WARN] {message}")


@dataclass
class JobConfig:
    dataset: str
    input_list: str
    interval: int
    analyzer_script: str
    golden_json: str
    era: str


@dataclass
class DatasetInfo:
    dataset_type: str
    year: str
    reco_type: str
    year_number: str
    list_basename: str


def run_cmd(
    cmd: List[str],
    cwd: Path = None,
    input_text: str = None,
    stream_output: bool = False,
) -> subprocess.CompletedProcess:
    log_cmd(" ".join(cmd))
    try:
        if stream_output:
            return subprocess.run(
                cmd,
                cwd=str(cwd) if cwd else None,
                input=input_text,
                text=True,
                check=True,
            )
        return subprocess.run(
            cmd,
            cwd=str(cwd) if cwd else None,
            input=input_text,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=True,
        )
    except subprocess.CalledProcessError as exc:
        msg = f"Command failed: {' '.join(cmd)}"
        stdout = (exc.stdout or "").strip()
        stderr = (exc.stderr or "").strip()
        if stdout:
            msg += f"\nSTDOUT:\n{stdout}"
        if stderr:
            msg += f"\nSTDERR:\n{stderr}"
        raise RuntimeError(msg) from exc


def sanitize_token(token: str) -> str:
    return re.sub(r"[^A-Za-z0-9._-]+", "_", token).strip("_")


def parse_key_value_file(config_path: Path) -> Dict[str, str]:
    kv: Dict[str, str] = {}
    for line in config_path.read_text().splitlines():
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue
        if "=" not in stripped:
            continue
        key, value = stripped.split("=", 1)
        kv[key.strip()] = value.strip()
    return kv


def parse_config(config_path: Path) -> JobConfig:
    kv = parse_key_value_file(config_path)
    required = ["Dataset", "InputList", "Interval", "analyzerScript", "goldenJSON", "era"]
    missing = [key for key in required if key not in kv]
    if missing:
        raise RuntimeError(f"Missing required config key(s): {', '.join(missing)}")

    interval = int(kv["Interval"])
    if interval <= 0:
        raise RuntimeError("Interval must be > 0.")

    return JobConfig(
        dataset=kv["Dataset"],
        input_list=kv["InputList"],
        interval=interval,
        analyzer_script=kv["analyzerScript"],
        golden_json=kv["goldenJSON"],
        era=kv["era"],
    )


def update_config_value(config_path: Path, key: str, value: str) -> None:
    lines = config_path.read_text().splitlines()
    out: List[str] = []
    replaced = False

    for line in lines:
        if re.match(rf"^\s*{re.escape(key)}\s*=", line):
            out.append(f"{key}={value}")
            replaced = True
        else:
            out.append(line)

    if not replaced:
        out.append(f"{key}={value}")

    config_path.write_text("\n".join(out) + "\n")


def dataset_info(dataset: str, era: str) -> DatasetInfo:
    parts = [p for p in dataset.split("/") if p]
    if len(parts) < 3:
        raise RuntimeError(f"Dataset format is invalid: {dataset}")

    dataset_type = sanitize_token(parts[0])
    year = sanitize_token(parts[1].replace("-", "_"))
    reco_type = sanitize_token(parts[2].replace("-", "_"))

    year_match = re.search(r"20\d{2}", era) or re.search(r"20\d{2}", dataset)
    year_number = year_match.group(0) if year_match else "0000"
    list_basename = sanitize_token("__".join(parts))

    return DatasetInfo(
        dataset_type=dataset_type,
        year=year,
        reco_type=reco_type,
        year_number=year_number,
        list_basename=list_basename,
    )


def resolve_input_list_path(raw_path: str, config_path: Path, repo_root: Path, cmssw_base: Path) -> Path:
    expanded = Path(os.path.expandvars(raw_path))
    if expanded.is_absolute():
        return expanded

    candidates = [
        Path(raw_path),
        config_path.parent / raw_path,
        repo_root / raw_path,
        cmssw_base / "src" / raw_path,
    ]
    for cand in candidates:
        if cand.exists():
            return cand.resolve()
    return (config_path.parent / raw_path).resolve()


def ensure_das_input_list(
    cfg: JobConfig,
    cfg_path: Path,
    repo_root: Path,
    cmssw_base: Path,
    force_new_list: bool,
) -> Tuple[Path, DatasetInfo]:
    info = dataset_info(cfg.dataset, cfg.era)
    # Keep DAS-generated lists in the same repo-local lists tree used for split job lists.
    list_dir = repo_root / "lists" / "nanoAOD" / "PFScouting"
    list_dir.mkdir(parents=True, exist_ok=True)
    list_path = list_dir / f"{info.list_basename}.txt"

    use_das = force_new_list or not cfg.input_list.strip()
    if not use_das and cfg.input_list.strip():
        input_list_path = resolve_input_list_path(cfg.input_list, cfg_path, repo_root, cmssw_base)
        if input_list_path.exists():
            return input_list_path, info
        log_warn(f"InputList does not exist: {input_list_path}. Creating a DAS list.")
        use_das = True

    if not use_das:
        raise RuntimeError("Internal logic error while resolving InputList.")

    if shutil.which("dasgoclient") is None:
        raise RuntimeError("dasgoclient is not available. Ensure CMSSW runtime is loaded.")

    query = f"file dataset={cfg.dataset}"
    result = run_cmd(["dasgoclient", "-query", query])
    files_raw = [line.strip() for line in result.stdout.splitlines() if line.strip()]
    if not files_raw:
        raise RuntimeError(f"DAS returned no files for dataset: {cfg.dataset}")

    files: List[str] = []
    seen = set()
    for entry in files_raw:
        if entry.startswith("/store/"):
            entry = f"{DEFAULT_XROOTD_REDIRECTOR}/{entry}"
        if entry not in seen:
            files.append(entry)
            seen.add(entry)

    list_path.write_text("\n".join(files) + "\n")

    new_expr = f"lists/nanoAOD/PFScouting/{list_path.name}"
    update_config_value(cfg_path, "InputList", new_expr)
    log_info(f"Wrote DAS list: {list_path}")
    log_info(f"Updated config InputList={new_expr}")

    return (repo_root / new_expr).resolve(), info


def update_analyzer_era(repo_root: Path, analyzer_script: str, era: str) -> None:
    analyzer_path = Path(analyzer_script)
    if not analyzer_path.is_absolute():
        analyzer_path = repo_root / "src" / analyzer_script
    if not analyzer_path.exists():
        raise RuntimeError(f"Analyzer script not found: {analyzer_path}")

    lines = analyzer_path.read_text().splitlines()
    replaced = False
    out = []
    for line in lines:
        if re.match(r"^\s*const\s+std::string\s+era\s*=", line):
            out.append(f'const std::string era = "{era}";')
            replaced = True
        else:
            out.append(line)
    if not replaced:
        raise RuntimeError(f"Could not find era declaration in analyzer file: {analyzer_path}")
    analyzer_path.write_text("\n".join(out) + "\n")

    link_path = repo_root / "src" / "analysisClass.C"
    if link_path.exists() or link_path.is_symlink():
        link_path.unlink()
    os.symlink(analyzer_path.name, link_path)


def update_json_path(repo_root: Path, cut_file: str, golden_json: str) -> None:
    cut_path = Path(cut_file)
    if not cut_path.is_absolute():
        cut_path = repo_root / cut_file
    if not cut_path.exists():
        raise RuntimeError(f"Cut file not found: {cut_path}")

    lines = cut_path.read_text().splitlines()
    out = []
    replaced = False
    for line in lines:
        if line.strip().startswith("JSON"):
            out.append(f"JSON {golden_json}")
            replaced = True
        else:
            out.append(line)
    if not replaced:
        out.append(f"JSON {golden_json}")
    cut_path.write_text("\n".join(out) + "\n")


def read_input_files(input_list_path: Path) -> List[str]:
    if not input_list_path.exists():
        raise RuntimeError(f"Input list does not exist: {input_list_path}")
    files = [line.strip() for line in input_list_path.read_text().splitlines() if line.strip()]
    if not files:
        raise RuntimeError(f"Input list is empty: {input_list_path}")
    return files


def prepare_framework(repo_root: Path, sample_file: str, tree_name: str) -> None:
    log_info("Generating rootNtupleClass from sample file...")
    run_cmd(
        ["./scripts/make_rootNtupleClass.sh", "-f", sample_file, "-t", tree_name],
        cwd=repo_root,
        input_text="y\n",
    )

    log_info("Building framework...")
    run_cmd(["make", "clean"], cwd=repo_root)
    run_cmd(["make", "-j8"], cwd=repo_root)


def split_into_chunks(items: List[str], chunk_size: int) -> List[List[str]]:
    return [items[i:i + chunk_size] for i in range(0, len(items), chunk_size)]


def create_condor_folder(script_dir: Path, info: DatasetInfo) -> Tuple[Path, str]:
    date_time = datetime.now().strftime("%d%B%Y_%H")
    condor_folder = script_dir / f"cjobs_{info.dataset_type}_{info.year}_{info.reco_type}_{date_time}"
    condor_folder.mkdir(parents=True, exist_ok=True)
    return condor_folder, date_time


def write_chunk_lists(
    repo_root: Path,
    chunks: List[List[str]],
    info: DatasetInfo,
    date_time: str,
) -> List[str]:
    list_dir = repo_root / "lists" / info.dataset_type / f"{info.dataset_type}_{info.year}_{info.reco_type}_{date_time}"
    list_dir.mkdir(parents=True, exist_ok=True)

    rel_paths: List[str] = []
    for idx, chunk in enumerate(chunks):
        file_name = f"{info.dataset_type}_{info.year}_{info.reco_type}_n{idx}.txt"
        list_path = list_dir / file_name
        list_path.write_text("\n".join(chunk) + "\n")
        rel_paths.append(list_path.relative_to(repo_root).as_posix())
    return rel_paths


def build_cmssw_tarball(cmssw_base: Path, condor_folder: Path) -> str:
    cmssw_ver = cmssw_base.name
    tar_file = condor_folder / f"{cmssw_ver}.tar.gz"
    log_info(f"Creating tarball: {tar_file}")
    run_cmd(
        [
            "tar",
            "--exclude-vcs",
            "-czf",
            str(tar_file),
            "--exclude=tmp",
            "--exclude=Limits",
            "--exclude=*.root",
            "--exclude=*.tar.gz",
            "-C",
            str(cmssw_base.parent),
            cmssw_ver,
        ]
    )
    return cmssw_ver


def create_job_shell(
    cmssw_ver: str,
    scram_arch: str,
    analyzer_script: str,
    cut_file: str,
    tree_name: str,
    list_rel_path: str,
    job_name: str,
    eos_path: str,
    info: DatasetInfo,
) -> str:
    eos_path = eos_path.strip()
    if not eos_path.startswith("/"):
        eos_path = f"/{eos_path}"

    eos_dir = f"{eos_path}/{info.year_number}/{info.dataset_type}/{info.dataset_type}_{info.year}"
    eos_file = f"{eos_dir}/{job_name}_reduced_skim.root"

    return f"""#!/usr/bin/env bash
set -eo pipefail

export VO_CMS_SW_DIR="${{VO_CMS_SW_DIR:-/cvmfs/cms.cern.ch}}"
source "${{VO_CMS_SW_DIR}}/cmsset_default.sh"
export SCRAM_ARCH={scram_arch}

tar -xzf {cmssw_ver}.tar.gz
rm -f {cmssw_ver}.tar.gz

cd {cmssw_ver}/src
scramv1 b ProjectRename
eval "$(scramv1 runtime -sh)"

cd DijetScoutingRun3Analyzer
ln -sf {analyzer_script} src/analysisClass.C

./main {list_rel_path} {cut_file} {tree_name} {job_name} {job_name}

xrdfs root://cmseos.fnal.gov mkdir -p {eos_dir}
xrdcp -f {job_name}_reduced_skim.root root://cmseos.fnal.gov/{eos_file}
"""


def create_job_jdl(
    cmssw_ver: str,
    sh_file_name: str,
    request_memory_mb: int,
    use_cms_connect: bool,
    cms_connect_image: str,
) -> str:
    cms_connect_block = ""
    if use_cms_connect:
        cms_connect_block = (
            f'+SingularityImage = "{cms_connect_image}"\n'
            "Requirements = (HAS_CVMFS_cms_cern_ch =?= true)\n"
        )

    return f"""universe = vanilla
Executable = {sh_file_name}
Should_Transfer_Files = YES
WhenToTransferOutput = ON_EXIT_OR_EVICT
Transfer_Input_Files = {cmssw_ver}.tar.gz, {sh_file_name}
request_cpus = 1
request_memory = {request_memory_mb}
Output = cjob_$(Cluster)_$(Process).stdout
Error = cjob_$(Cluster)_$(Process).stderr
Log = cjob_$(Cluster)_$(Process).log
stream_output = True
stream_error = True
use_x509userproxy = true
{cms_connect_block}+JobFlavour = "nextweek"
Queue 1
"""


def create_submit_all(condor_folder: Path, n_jobs: int, info: DatasetInfo) -> None:
    submit_path = condor_folder / "submit_all.py"
    submit_path.write_text(
        f"""#!/usr/bin/env python3
import shlex
import subprocess
from concurrent.futures import ThreadPoolExecutor, as_completed
from tqdm import tqdm

N_WORKERS = {DEFAULT_SUBMIT_WORKERS}


def submit_one(jdl):
    cmd = ["condor_submit", "-terse"]
    cmd.append(jdl)
    shell_cmd = " ".join(shlex.quote(part) for part in cmd)
    proc = subprocess.run(
        ["bash", "-lc", shell_cmd],
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    return jdl, proc.returncode, proc.stdout.strip(), proc.stderr.strip()


def main():
    jdls = ["{info.dataset_type}_{info.year}_{info.reco_type}_n{{}}.jdl".format(i) for i in range({n_jobs})]

    with ThreadPoolExecutor(max_workers=max(1, N_WORKERS)) as executor:
        futures = [executor.submit(submit_one, jdl) for jdl in jdls]
        with tqdm(total=len(jdls), unit="job") as pbar:
            for future in as_completed(futures):
                jdl, rc, out, err = future.result()
                if rc == 0:
                    cluster_id = out.splitlines()[-1] if out else ""
                    if cluster_id:
                        pbar.set_postfix_str(cluster_id)
                pbar.update(1)


if __name__ == "__main__":
    main()
"""
    )
    submit_path.chmod(0o755)


def create_condor_job_files(
    condor_folder: Path,
    cmssw_ver: str,
    list_rel_paths: List[str],
    cfg: JobConfig,
    info: DatasetInfo,
    cut_file: str,
    tree_name: str,
    scram_arch: str,
    eos_path: str,
    request_memory_mb: int,
    use_cms_connect: bool,
    cms_connect_image: str,
) -> None:
    for idx, list_rel in enumerate(list_rel_paths):
        job_name = f"{info.dataset_type}_{info.year}_Condor_n{idx}"

        sh_name = f"{info.dataset_type}_{info.year}_{info.reco_type}_n{idx}.sh"
        sh_path = condor_folder / sh_name
        sh_path.write_text(
            create_job_shell(
                cmssw_ver=cmssw_ver,
                scram_arch=scram_arch,
                analyzer_script=cfg.analyzer_script,
                cut_file=cut_file,
                tree_name=tree_name,
                list_rel_path=list_rel,
                job_name=job_name,
                eos_path=eos_path,
                info=info,
            )
        )
        sh_path.chmod(0o755)

        jdl_name = f"{info.dataset_type}_{info.year}_{info.reco_type}_n{idx}.jdl"
        jdl_path = condor_folder / jdl_name
        jdl_path.write_text(
            create_job_jdl(
                cmssw_ver=cmssw_ver,
                sh_file_name=sh_name,
                request_memory_mb=request_memory_mb,
                use_cms_connect=use_cms_connect,
                cms_connect_image=cms_connect_image,
            )
        )


def submit_jobs(condor_folder: Path) -> None:
    log_info("Submitting jobs to Condor...")
    run_cmd(["python3", "submit_all.py"], cwd=condor_folder, stream_output=True)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Prepare and submit NanoAOD scouting Condor jobs.")
    parser.add_argument("-c", "--config", required=True, help="Path to job config file.")
    parser.add_argument("--force-new-list", action="store_true", help="Force DAS list regeneration and config update.")
    parser.add_argument("--tree-name", default=DEFAULT_TREE_NAME, help=f"Input tree name (default: {DEFAULT_TREE_NAME}).")
    parser.add_argument("--cut-file", default=DEFAULT_CUT_FILE, help=f"Cut file path relative to repo root (default: {DEFAULT_CUT_FILE}).")
    parser.add_argument("--eos-path", default=DEFAULT_EOS_PATH, help=f"EOS output base path (default: {DEFAULT_EOS_PATH}).")
    parser.add_argument("--scram-arch", default=DEFAULT_SCRAM_ARCH, help=f"SCRAM_ARCH value (default: {DEFAULT_SCRAM_ARCH}).")
    parser.add_argument(
        "--request-memory-mb",
        type=int,
        default=DEFAULT_REQUEST_MEMORY_MB,
        help=f"Condor request_memory in MB (default: {DEFAULT_REQUEST_MEMORY_MB}).",
    )
    parser.add_argument(
        "--cms-connect",
        action="store_true",
        help="Enable CMS Connect JDL settings (EL9 Singularity image + CVMFS requirement).",
    )
    parser.add_argument(
        "--cms-connect-image",
        default=DEFAULT_CMS_CONNECT_IMAGE,
        help=f'Container image used with --cms-connect (default: "{DEFAULT_CMS_CONNECT_IMAGE}").',
    )
    parser.add_argument("--no-submit", action="store_true", help="Prepare files but do not submit.")
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    script_dir = Path(__file__).resolve().parent
    repo_root = script_dir.parent
    cfg_path = Path(args.config).expanduser().resolve()
    if not cfg_path.exists():
        raise RuntimeError(f"Config file not found: {cfg_path}")

    cmssw_base_env = os.getenv("CMSSW_BASE", "").strip()
    if not cmssw_base_env:
        raise RuntimeError("CMSSW_BASE is not set. Run inside a CMSSW environment.")
    cmssw_base = Path(cmssw_base_env).resolve()
    cmssw_ver = cmssw_base.name
    if cmssw_ver != DEFAULT_CMSSW_VER:
        log_warn(f"CMSSW version is {cmssw_ver}, expected {DEFAULT_CMSSW_VER}. Proceeding with {cmssw_ver}.")

    cfg = parse_config(cfg_path)
    input_list_path, info = ensure_das_input_list(
        cfg=cfg,
        cfg_path=cfg_path,
        repo_root=repo_root,
        cmssw_base=cmssw_base,
        force_new_list=args.force_new_list,
    )

    cfg = parse_config(cfg_path)
    input_list_path = resolve_input_list_path(cfg.input_list, cfg_path, repo_root, cmssw_base)
    input_files = read_input_files(input_list_path)

    update_analyzer_era(repo_root, cfg.analyzer_script, cfg.era)
    update_json_path(repo_root, args.cut_file, cfg.golden_json)

    prepare_framework(repo_root, sample_file=input_files[0], tree_name=args.tree_name)
    chunks = split_into_chunks(input_files, cfg.interval)
    condor_folder, date_time = create_condor_folder(script_dir, info)
    list_rel_paths = write_chunk_lists(repo_root, chunks, info, date_time)
    cmssw_ver = build_cmssw_tarball(cmssw_base, condor_folder)

    create_condor_job_files(
        condor_folder=condor_folder,
        cmssw_ver=cmssw_ver,
        list_rel_paths=list_rel_paths,
        cfg=cfg,
        info=info,
        cut_file=args.cut_file,
        tree_name=args.tree_name,
        scram_arch=args.scram_arch,
        eos_path=args.eos_path,
        request_memory_mb=args.request_memory_mb,
        use_cms_connect=args.cms_connect,
        cms_connect_image=args.cms_connect_image,
    )
    create_submit_all(condor_folder, len(chunks), info)

    log_info(f"Condor folder: {condor_folder}")
    log_info(f"Number of jobs: {len(chunks)}")
    if args.cms_connect:
        log_info(f'CMS Connect mode enabled: +SingularityImage="{args.cms_connect_image}"')
    if args.no_submit:
        log_info("--no-submit is set. Skipping Condor submission.")
    else:
        submit_jobs(condor_folder=condor_folder)


if __name__ == "__main__":
    main()

