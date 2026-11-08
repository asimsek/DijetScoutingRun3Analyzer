import argparse
from datetime import datetime
import fcntl
import hashlib
import json
import math
import os
from pathlib import Path
import re
import shlex
import subprocess
import sys
import uuid

from condor_submit_jobs import submit_prepared, write_batch_submission
from eos_utils import condor_shared_directory, parse_remote, resolve_condor_proxy
from plot_kinematics_from_json import atomic_json, merge_summaries, read_summary


def arguments():
    parser = argparse.ArgumentParser(description="Reduce disjoint ROOT-file splits in Condor, then manually merge exact kinematics summaries.")
    action = parser.add_mutually_exclusive_group()
    action.add_argument("--merge", type=Path, metavar="WORKDIR")
    action.add_argument("--retry-failed", type=Path, metavar="WORKDIR", help="Retry missing/invalid outputs, skipping queued jobs")
    action.add_argument("--submit", type=Path, metavar="WORKDIR", help="Submit a prepared production, skipping accepted jobs")
    source = parser.add_mutually_exclusive_group()
    source.add_argument("--input", action="append", help="Data ROOT file, directory, glob, or @list; repeat as needed")
    source.add_argument("--mc-list", help="QCD list: source and cross section in pb per line")
    parser.add_argument("--dataset")
    parser.add_argument("--jec", choices=("old", "new"))
    parser.add_argument("--output", default="kinematics")
    parser.add_argument("--jobs", type=int, default=64, help="Disjoint file splits, capped at file count")
    parser.add_argument("--threads", type=int, default=4)
    parser.add_argument("--request-memory-mb", type=int, default=4096)
    parser.add_argument("--request-disk-mb", type=int, default=2048)
    parser.add_argument("--workdir", type=Path)
    parser.add_argument("--job-flavour", default="tomorrow")
    parser.add_argument("--cmssw-version", default=os.environ.get("CMSSW_VERSION", Path(os.environ.get("CMSSW_BASE", "")).name))
    parser.add_argument("--scram-arch", default=os.environ.get("SCRAM_ARCH", ""))
    parser.add_argument("--proxy")
    parser.add_argument("--shared-dir")
    parser.add_argument("--schedd")
    parser.add_argument("--pool")
    parser.add_argument("--no-submit", action="store_true")
    parser.add_argument("--force", action="store_true", help="With --merge, replace an existing final summary")
    return parser.parse_args()


def digest(path):
    value = hashlib.sha256()
    with Path(path).open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            value.update(block)
    return value.hexdigest()


def read_json(path):
    with Path(path).open() as stream:
        return json.load(stream)


def submit_path(path):
    value = str(path)
    if any(c in value for c in '\n\r\t,;=$"\\'):
        raise ValueError(f"Unsupported Condor path characters: {value}")
    return value


def worker_script(manifest, job):
    command = ["./reduce_kinematics", "--manifest", job["input"], "--threads", str(manifest["threads"]),
               "--output", job["output"], "--force"]
    return f'''#!/usr/bin/env bash
set -eo pipefail
export SCRAM_ARCH={shlex.quote(manifest['scram_arch'])}
source /cvmfs/cms.cern.ch/cmsset_default.sh
job_scratch="${{_CONDOR_SCRATCH_DIR:-$PWD}}"
cd "$job_scratch"
scramv1 project CMSSW {shlex.quote(manifest['cmssw_version'])}
cd {shlex.quote(manifest['cmssw_version'])}/src
eval "$(scramv1 runtime -sh)"
cd "$job_scratch"
{shlex.join(command)}
python3 - {shlex.quote(job['metadata'])} <<'PY'
import hashlib
import json
from pathlib import Path
import sys
job = json.loads(Path(sys.argv[1]).read_text())
content = Path(job["output"]).read_bytes()
doc = json.loads(content)
if doc.get("complete") is not True:
    raise RuntimeError("Incomplete reduction")
receipt = {{"production": job["production"], "index": job["index"], "sha256": hashlib.sha256(content).hexdigest()}}
Path(job["receipt"]).write_text(json.dumps(receipt) + "\\n")
PY
'''


def prepare(args):
    if not args.dataset or not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9_.-]*", args.dataset):
        raise ValueError("Provide a filename-safe --dataset label")
    if not args.input and not args.mc_list:
        raise ValueError("Provide --input or --mc-list")
    if args.force or (args.mc_list and args.jec):
        raise ValueError("--force is for --merge; --jec is for data")
    if min(args.jobs, args.threads, args.request_memory_mb, args.request_disk_mb) < 1:
        raise ValueError("Job counts and resources must be positive")
    if not re.fullmatch(r"CMSSW_[A-Za-z0-9_]+", args.cmssw_version) or not re.fullmatch(r"[A-Za-z0-9_]+", args.scram_arch):
        raise ValueError("Run cmsenv first, or supply --cmssw-version and --scram-arch")
    for name, environment in (("cmssw_version", "CMSSW_VERSION"), ("scram_arch", "SCRAM_ARCH")):
        if os.environ.get(environment) and getattr(args, name) != os.environ[environment]:
            raise ValueError(f"Requested {name} must match the active build environment")
    if parse_remote(args.output) or "://" in args.output:
        raise ValueError("--output must be a local/shared directory")
    output = Path(os.path.expandvars(args.output)).expanduser().resolve()
    if output.suffix == ".json":
        raise ValueError("--output must be a directory")
    label = args.dataset + ("_" + args.jec if args.jec else "")
    workdir = args.workdir.expanduser().resolve() if args.workdir else output / ".condor" / (label + "_" + datetime.now().strftime("%d%B%Y_%H%M"))
    submit_path(workdir)
    args.shared_dir = str(condor_shared_directory(args.shared_dir))
    proxy = resolve_condor_proxy(args.proxy, args.shared_dir)
    submit_path(proxy)
    output.mkdir(parents=True, exist_ok=True)
    workdir.mkdir(parents=True, exist_ok=False)
    for directory in ("parts", "logs", "inputs"):
        (workdir / directory).mkdir()
    source = Path(__file__).resolve().with_name("reduce_kinematics.cpp")
    flags = shlex.split(subprocess.check_output(["root-config", "--cflags", "--libs"], text=True))
    print("[INFO] Building the reducer once, with portable CPU instructions.", flush=True)
    subprocess.run(shlex.split(os.environ.get("CXX", "c++")) + ["-O3", "-DNDEBUG", str(source), "-o", str(workdir / "reduce_kinematics")] + flags, check=True)
    command = [str(workdir / "reduce_kinematics"), "--plan", "--dataset", args.dataset]
    if args.jec:
        command += ["--jec", args.jec]
    for value in args.input or []:
        command += ["--input", value]
    if args.mc_list:
        command += ["--mc-list", args.mc_list]
    plan = json.loads(subprocess.check_output(command, text=True))
    tasks = plan["tasks"]
    local = {}
    for index, task in enumerate(tasks):
        path = task["path"]
        if "://" not in path:
            alias = f"input_{index:06d}.root"
            (workdir / "inputs" / alias).symlink_to(path)
            local[path] = (alias, Path(path).stat().st_size)
    manifest = {"version": 1, "production": uuid.uuid4().hex, "plan": plan, "label": label,
                "output": str(output / (label + ".json")), "threads": args.threads,
                "cmssw_version": args.cmssw_version, "scram_arch": args.scram_arch,
                "schedd": args.schedd, "pool": args.pool, "shared_dir": args.shared_dir, "jobs": []}
    descriptions = []
    frozen = ["reduce_kinematics"]
    number = min(args.jobs, len(tasks))
    for index in range(number):
        stem = f"reduce_kinematics_{label}_{index:04d}"
        assigned = tasks[index::number]
        ids = {task["component"] for task in assigned}
        part = dict(plan, components=[c for c in plan["components"] if c["id"] in ids],
                    tasks=[dict(t, runtime=local[t["path"]][0]) if t["path"] in local else dict(t) for t in assigned])
        job = {"production": manifest["production"], "index": index, "input": stem + ".input.json",
               "metadata": stem + ".job.json", "script": stem + ".sh", "jdl": stem + ".jdl",
               "output": stem + ".json", "receipt": stem + ".done.json", "tasks": assigned,
               "components": part["components"]}
        atomic_json(workdir / job["input"], part)
        atomic_json(workdir / job["metadata"], job)
        script = workdir / job["script"]
        script.write_text(worker_script(manifest, job))
        script.chmod(0o755)
        inputs = ["reduce_kinematics", job["input"], job["metadata"], job["script"]]
        inputs += ["inputs/" + local[t["path"]][0] for t in assigned if t["path"] in local]
        disk = args.request_disk_mb + math.ceil(sum(local.get(t["path"], ("", 0))[1] for t in assigned) / 1024**2)
        lines = ["universe = vanilla", "batch_name = reduce_kinematics_" + label,
                 "+DijetDataset = " + json.dumps("reduce_kinematics_" + label), "executable = " + job["script"],
                 "should_transfer_files = YES", "when_to_transfer_output = ON_SUCCESS", "success_exit_code = 0",
                 "transfer_input_files = " + ", ".join(inputs), "transfer_output_files = " + job["output"] + ", " + job["receipt"],
                 'transfer_output_remaps = "' + job["output"] + "=parts/" + job["output"] + ";" + job["receipt"] + "=parts/" + job["receipt"] + '"',
                 f"request_cpus = {args.threads}", f"request_memory = {args.request_memory_mb}MB", f"request_disk = {disk}MB",
                 "use_x509userproxy = true", "x509userproxy = " + proxy,
                 f"output = logs/{stem}.$(Cluster).$(Process).out", f"error = logs/{stem}.$(Cluster).$(Process).err",
                 f"log = logs/{stem}.$(Cluster).$(Process).log"]
        if args.job_flavour:
            lines.append("+JobFlavour = " + json.dumps(args.job_flavour))
        description = "\n".join(lines + ["queue 1"]) + "\n"
        (workdir / job["jdl"]).write_text(description)
        descriptions.append((job["jdl"], description))
        frozen += [job["input"], job["metadata"], job["script"]]
        manifest["jobs"].append(job)
    write_batch_submission(workdir, descriptions, proxy)
    frozen.append("run_condor_batch.sh")
    manifest["prepared_sha256"] = {name: digest(workdir / name) for name in frozen}
    atomic_json(workdir / "production.json", manifest)
    (workdir / "prepared_jobs.txt").write_text("\n".join(j["jdl"] for j in manifest["jobs"]) + "\n")
    print(f"[INFO] {len(tasks)} files, {number} jobs, {args.threads} CPUs per job; final output: {manifest['output']}")
    print("[INFO] Merge after completion: " + shlex.join(["python3", str(Path(__file__).resolve()), "--merge", str(workdir)]))
    if args.no_submit:
        print(f"[INFO] Prepared only: {workdir}")
        return 0
    return submit_prepared(workdir, proxy=proxy, shared_dir=args.shared_dir, schedd=args.schedd, pool=args.pool,
                           jdl_list=Path("prepared_jobs.txt"), batch_file=Path("prepared_batch.sub"))


def load_production(workdir):
    manifest = read_json(workdir / "production.json")
    if manifest.get("version") != 1 or not manifest.get("jobs"):
        raise ValueError("Invalid production manifest")
    tasks = []
    for index, job in enumerate(manifest["jobs"]):
        if job["index"] != index or job["production"] != manifest["production"]:
            raise ValueError("Invalid split identity")
        for key in ("input", "metadata", "script", "jdl", "output", "receipt"):
            if Path(job[key]).name != job[key]:
                raise ValueError("Invalid prepared filename")
        tasks.extend(job["tasks"])
    key = lambda row: (row["path"], row["component"])
    if sorted(map(key, tasks)) != sorted(map(key, manifest["plan"]["tasks"])) or len({t["path"] for t in tasks}) != len(tasks):
        raise ValueError("Splits do not cover each input exactly once")
    return manifest


def checked_job(workdir, manifest, job):
    receipt = read_json(workdir / "parts" / job["receipt"])
    if receipt.get("production") != manifest["production"] or receipt.get("index") != job["index"]:
        raise ValueError("Wrong production receipt")
    path = workdir / "parts" / job["output"]
    if digest(path) != receipt["sha256"]:
        raise ValueError("Output checksum differs from completed worker output")
    doc = read_summary(path)
    for key in ("dataset", "jec", "kind", "contract"):
        if doc[key] != manifest["plan"][key]:
            raise ValueError(f"Wrong {key} in split output")
    expected = {(row["path"], row["component"]) for row in job["tasks"]}
    found = {(row["path"], component["id"]) for component in doc["components"] for row in component["files"]}
    if found != expected:
        raise ValueError("Output contains wrong or incomplete input coverage")
    definitions = {c["id"]: c for c in job["components"]}
    for component in doc["components"]:
        if any(component[key] != definitions[component["id"]][key] for key in ("source", "xsec_pb")):
            raise ValueError("QCD component definition changed")
    return doc


def merge(workdir, manifest, force):
    output = Path(manifest["output"])
    output.parent.mkdir(parents=True, exist_ok=True)
    with (output.parent / ("." + output.stem + ".merge.lock")).open("a") as lock:
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        checkpoint = output.parent / ("." + output.stem + ".checkpoint")
        checkpoint.mkdir(exist_ok=True)
        with (checkpoint / "lock").open("a") as reducer_lock:
            fcntl.flock(reducer_lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
            if output.exists() and not force:
                raise ValueError("Final summary exists; use --merge WORKDIR --force to replace it")
            combined = merge_summaries((checked_job(workdir, manifest, job) for job in manifest["jobs"]), manifest["plan"]["dataset"])
            components = {c["id"]: c for c in combined["components"]}
            combined["components"] = [components[c["id"]] for c in manifest["plan"]["components"]]
            for component in combined["components"]:
                rows = {row["path"]: row for row in component["files"]}
                component["files"] = [rows[t["path"]] for t in manifest["plan"]["tasks"] if t["component"] == component["id"]]
                component["generated_events"] = sum(row["generated_events"] for row in component["files"])
            atomic_json(output, combined)
    print(f"[INFO] Saved {output}")
    return 0


def main():
    args = arguments()
    existing = args.merge or args.retry_failed or args.submit
    if existing is None:
        return prepare(args)
    if args.input or args.mc_list or args.dataset or args.jec or args.workdir or args.no_submit:
        raise ValueError("Existing productions use their frozen inputs; do not supply new production options")
    if args.force and not args.merge:
        raise ValueError("--force is only used with --merge")
    workdir = existing.expanduser().resolve()
    manifest = load_production(workdir)
    if args.merge:
        return merge(workdir, manifest, args.force)
    for name, expected in manifest["prepared_sha256"].items():
        if Path(name).name != name or digest(workdir / name) != expected:
            raise ValueError(f"Prepared input changed: {name}; prepare a new production")
    pending = []
    for job in manifest["jobs"]:
        try:
            checked_job(workdir, manifest, job)
        except (OSError, ValueError, KeyError, TypeError):
            pending.append(job["jdl"])
    if not pending:
        print("[INFO] All splits are complete; run --merge WORKDIR")
        return 0
    (workdir / "pending_jobs.txt").write_text("\n".join(pending) + "\n")
    return submit_prepared(workdir, resume=bool(args.submit), retry_failed=bool(args.retry_failed), proxy=args.proxy,
                           shared_dir=args.shared_dir or manifest["shared_dir"], schedd=args.schedd or manifest["schedd"],
                           pool=args.pool or manifest["pool"], jdl_list=Path("pending_jobs.txt"))


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, ValueError, RuntimeError, KeyError, TypeError, subprocess.CalledProcessError) as error:
        print(f"[ERROR] {error}", file=sys.stderr)
        raise SystemExit(1)
