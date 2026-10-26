from __future__ import annotations

import argparse
from contextlib import ExitStack
from datetime import datetime
import fcntl
import hashlib
import json
import math
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import sys
import tempfile
import uuid

from condor_submit_jobs import submit_prepared, write_batch_submission
from eos_utils import condor_shared_directory, discover_files, normalize_path, parse_remote, resolve_condor_proxy


SELECTIONS = ("selected", "all_stored")


def arguments():
    parser = argparse.ArgumentParser(description="Split data JEC reduction across Condor jobs and merge both summaries.",
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    action = parser.add_mutually_exclusive_group()
    action.add_argument("--merge", type=Path, metavar="WORKDIR", help="Merge a completed production; no ROOT reads or proxy needed")
    action.add_argument("--retry-failed", type=Path, metavar="WORKDIR", help="Resubmit missing/bad splits, skipping jobs still in Condor")
    action.add_argument("--submit", type=Path, metavar="WORKDIR", help="Submit a prepared production, skipping previously accepted jobs")
    parser.add_argument("--input", action="append", help="ROOT file, directory, glob or @list; repeat for several inputs")
    parser.add_argument("--jec", choices=("old", "new"))
    parser.add_argument("--dataset")
    parser.add_argument("--output", default="summaries", help="Output directory, relative to your current directory")
    parser.add_argument("--jobs", type=int, default=64, help="Number of disjoint file splits, capped at the input file count")
    parser.add_argument("--threads", type=int, default=4, help="Reducer threads and requested CPUs per job")
    parser.add_argument("--request-memory-mb", type=int, default=4096)
    parser.add_argument("--request-disk-mb", type=int, default=2048, help="Scratch space, plus any transferred local ROOT inputs")
    parser.add_argument("--job-flavour", default="tomorrow", help="CERN runtime class")
    parser.add_argument("--workdir", type=Path, help="New directory for this production; default: OUTPUT/.condor/DATASET_JEC_TIMESTAMP")
    parser.add_argument("--cmssw-version", default=os.environ.get("CMSSW_VERSION", Path(os.environ.get("CMSSW_BASE", "")).name))
    parser.add_argument("--scram-arch", default=os.environ.get("SCRAM_ARCH", ""))
    parser.add_argument("--proxy", help="Active VOMS proxy; node-local proxies are staged automatically")
    parser.add_argument("--shared-dir", help="Shared proxy/credential staging area")
    parser.add_argument("--schedd")
    parser.add_argument("--pool")
    parser.add_argument("--no-submit", action="store_true", help="Prepare the complete production without submitting")
    parser.add_argument("--force", action="store_true", help="With --merge, replace existing final summaries")
    parser.add_argument("--tree", default="rootTupleTree/tree")
    parser.add_argument("--preset", choices=("dijet", "loose", "all"), default="dijet")
    parser.add_argument("--cert-json", type=Path)
    parser.add_argument("--require-pass-json", action="store_true")
    parser.add_argument("--cache-mb", type=int, default=32)
    parser.add_argument("--chunk-entries", type=int, default=1000000)
    for option in ("mjj-min", "pt-min", "eta-max", "deta-max", "balance-alpha-max", "jet-match-dr"):
        parser.add_argument("--" + option, type=float)
    return parser.parse_args()


def read_json(path):
    with Path(path).open() as stream:
        return json.load(stream)


def digest(path):
    checksum = hashlib.sha256()
    with Path(path).open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            checksum.update(block)
    return checksum.hexdigest()


def atomic_json(path, value):
    path = Path(path)
    temporary = None
    try:
        with tempfile.NamedTemporaryFile(mode="w", dir=path.parent, prefix="." + path.name + ".", delete=False) as stream:
            temporary = Path(stream.name)
            json.dump(value, stream, separators=(",", ":"), allow_nan=False)
            stream.write("\n")
            stream.flush()
            os.fsync(stream.fileno())
        os.replace(temporary, path)
    finally:
        if temporary is not None:
            temporary.unlink(missing_ok=True)


def expand_inputs(value, depth=0):
    if depth > 8:
        raise ValueError("Input-list recursion exceeds eight levels")
    value = os.path.expandvars(os.path.expanduser(value))
    path = Path(value[1:] if value.startswith("@") else value)
    if value.startswith("@") or (path.is_file() and path.suffix == ".txt"):
        result = []
        for line in path.read_text().splitlines():
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            nested = line.startswith("@")
            token = line[1:] if nested else line
            token = os.path.expandvars(os.path.expanduser(token))
            if "://" not in token and not Path(token).is_absolute():
                token = str(path.resolve().parent / token)
            result.extend(expand_inputs(("@" if nested else "") + token, depth + 1))
        return result
    return discover_files(value)


def submit_path(path):
    text = str(path)
    if any(c in text for c in '\n\r\t,;=$"\\'):
        raise ValueError(f"Path contains unsupported Condor submit characters: {text}")
    return text


def reducer_arguments(args):
    result = ["--jec", args.jec, "--dataset", args.dataset, "--threads", str(args.threads),
              "--tree", args.tree, "--preset", args.preset, "--cache-mb", str(args.cache_mb),
              "--chunk-entries", str(args.chunk_entries)]
    for key in ("mjj_min", "pt_min", "eta_max", "deta_max", "balance_alpha_max", "jet_match_dr"):
        value = getattr(args, key)
        if value is not None:
            if not math.isfinite(value) or value < 0 or (key in ("eta_max", "deta_max", "jet_match_dr") and value == 0):
                raise ValueError(f"Invalid --{key.replace('_', '-')}")
            result.extend(["--" + key.replace("_", "-"), str(value)])
    if args.require_pass_json:
        result.append("--require-pass-json")
    if args.cert_json:
        result.extend(["--cert-json", "certification.json"])
    return result


def worker_script(manifest, job, common):
    command = ["./reduce_jec_comparison", "--input", "@" + job["list"], *common,
               "--output", "reduced.json", "--force"]
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
receipt = {{"production": job["production"], "index": job["index"], "sha256": {{}}}}
for selection in ("selected", "all_stored"):
    source = Path("reduced_" + selection + ".json")
    doc = json.loads(source.read_text())
    rows = doc["files"][job["jec"]]
    if len(rows) != len(job["files"]):
        raise RuntimeError("Unexpected reducer file count")
    for row, original, runtime in zip(rows, job["files"], job["runtime_files"]):
        expected = runtime if "://" in runtime else str(Path(runtime).resolve())
        if row["path"] != expected or row["entries"] != row["processed"]:
            raise RuntimeError("Incomplete or unexpected reducer input: " + row["path"])
        row["path"] = original
    if doc.get("partial") or doc["processed_entries"] != sum(row["entries"] for row in rows):
        raise RuntimeError("Incomplete reduction")
    if job.get("certification_source"):
        doc["certification_source"] = job["certification_source"]
    target = Path(job["outputs"][selection])
    target.write_text(json.dumps(doc, separators=(",", ":"), allow_nan=False) + "\\n")
    receipt["sha256"][selection] = hashlib.sha256(target.read_bytes()).hexdigest()
Path(job["receipt"]).write_text(json.dumps(receipt) + "\\n")
PY
'''


def prepare(args):
    if not args.input or not args.dataset or not args.jec:
        raise ValueError("New productions require --input, --dataset and --jec")
    if not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9_.-]*", args.dataset):
        raise ValueError("--dataset must be a filename label")
    if args.force:
        raise ValueError("--force is only used with --merge")
    if min(args.jobs, args.threads, args.request_memory_mb, args.request_disk_mb, args.chunk_entries) < 1 or args.cache_mb < 0:
        raise ValueError("Jobs, threads, memory, disk and chunk size must be positive; cache size must be nonnegative")
    if not re.fullmatch(r"CMSSW_[A-Za-z0-9_]+", args.cmssw_version) or not re.fullmatch(r"[A-Za-z0-9_]+", args.scram_arch):
        raise ValueError("Run cmsenv first, or supply --cmssw-version and --scram-arch")
    for key, active in (("cmssw_version", os.environ.get("CMSSW_VERSION")), ("scram_arch", os.environ.get("SCRAM_ARCH"))):
        if active and getattr(args, key) != active:
            raise ValueError("The requested CMSSW version/architecture must match the active build environment; run cmsenv there first")
    common = reducer_arguments(args)
    args.shared_dir = str(condor_shared_directory(args.shared_dir))
    proxy = resolve_condor_proxy(args.proxy, args.shared_dir)
    files = [normalize_path(path) for value in args.input for path in expand_inputs(value)]
    if not files or len(set(files)) != len(files):
        raise ValueError("Inputs must contain at least one ROOT file and no repeated paths")
    for path in files:
        if any(c in path for c in "\n\r"):
            raise ValueError("Input filenames cannot contain newlines")
    output = Path(os.path.expandvars(os.path.expanduser(args.output))).resolve()
    if parse_remote(args.output) or "://" in args.output or output.suffix == ".json":
        raise ValueError("--output must be a local/shared directory such as summaries")
    output.mkdir(parents=True, exist_ok=True)
    label = args.dataset + "_" + args.jec
    workdir = (args.workdir.expanduser().resolve() if args.workdir else
               output / ".condor" / (label + "_" + datetime.now().strftime("%d%B%Y_%H%M")))
    submit_path(workdir)
    submit_path(proxy)
    workdir.mkdir(parents=True, exist_ok=False)
    for name in ("parts", "logs", "inputs"):
        (workdir / name).mkdir()
    if args.cert_json:
        shutil.copyfile(args.cert_json.expanduser().resolve(), workdir / "certification.json")
    script_dir = Path(__file__).resolve().parent
    print("[INFO] Building the reducer once for all jobs (portable CPU instructions).", flush=True)
    flags = shlex.split(subprocess.check_output(["root-config", "--cflags", "--libs"], text=True))
    subprocess.run(shlex.split(os.environ.get("CXX", "c++")) + ["-O3", "-DNDEBUG",
                   str(script_dir / "reduce_jec_comparison.cpp"), "-o", str(workdir / "reduce_jec_comparison")] + flags, check=True)
    runtime = {}
    local_sizes = {}
    for index, path in enumerate(files):
        if "://" in path:
            runtime[path] = path
        else:
            alias = f"input_{index:06d}.root"
            (workdir / "inputs" / alias).symlink_to(path)
            runtime[path] = alias
            local_sizes[path] = Path(path).stat().st_size
    count = min(args.jobs, len(files))
    manifest = {"version": 1, "production": uuid.uuid4().hex, "dataset": args.dataset, "jec": args.jec,
                "output": str(output), "label": label, "files": files, "jobs": [],
                "cmssw_version": args.cmssw_version, "scram_arch": args.scram_arch,
                "reducer_arguments": common, "reducer_sha256": digest(workdir / "reduce_jec_comparison"),
                "schedd": args.schedd, "pool": args.pool, "shared_dir": args.shared_dir}
    descriptions = []
    for index in range(count):
        stem = f"reduce_{label}_{index:04d}"
        assigned = files[index::count]
        job = {"production": manifest["production"], "index": index, "jec": args.jec, "files": assigned,
               "runtime_files": [runtime[path] for path in assigned], "list": stem + ".txt",
               "metadata": stem + ".json", "script": stem + ".sh", "jdl": stem + ".jdl",
               "certification_source": str(args.cert_json.expanduser().resolve()) if args.cert_json else None,
               "outputs": {selection: stem + "_" + selection + ".json" for selection in SELECTIONS},
               "receipt": stem + ".done.json"}
        (workdir / job["list"]).write_text("\n".join(job["runtime_files"]) + "\n")
        atomic_json(workdir / job["metadata"], job)
        (workdir / job["script"]).write_text(worker_script(manifest, job, common))
        (workdir / job["script"]).chmod(0o755)
        inputs = ["reduce_jec_comparison", job["script"], job["list"], job["metadata"]]
        if args.cert_json:
            inputs.append("certification.json")
        inputs += ["inputs/" + runtime[path] for path in assigned if path in local_sizes]
        outputs = list(job["outputs"].values()) + [job["receipt"]]
        disk = args.request_disk_mb + math.ceil(sum(local_sizes.get(path, 0) for path in assigned) / 1024**2)
        lines = ["universe = vanilla", "batch_name = reduce_" + label,
                 "+DijetDataset = " + json.dumps("reduce_" + label), "executable = " + job["script"],
                 "should_transfer_files = YES", "when_to_transfer_output = ON_SUCCESS", "success_exit_code = 0",
                 "transfer_input_files = " + ", ".join(inputs), "transfer_output_files = " + ", ".join(outputs),
                 'transfer_output_remaps = "' + ";".join(name + "=parts/" + name for name in outputs) + '"',
                 f"request_cpus = {args.threads}", f"request_memory = {args.request_memory_mb}MB",
                 f"request_disk = {disk}MB", "use_x509userproxy = true", "x509userproxy = " + proxy,
                 f"output = logs/{stem}.$(Cluster).$(Process).out", f"error = logs/{stem}.$(Cluster).$(Process).err",
                 f"log = logs/{stem}.$(Cluster).$(Process).log"]
        if args.job_flavour:
            lines.append("+JobFlavour = " + json.dumps(args.job_flavour))
        description = "\n".join(lines + ["queue 1"]) + "\n"
        (workdir / job["jdl"]).write_text(description)
        descriptions.append((job["jdl"], description))
        manifest["jobs"].append(job)
    frozen = ["reduce_jec_comparison"]
    if args.cert_json:
        frozen.append("certification.json")
    for job in manifest["jobs"]:
        frozen.extend(job[key] for key in ("script", "list", "metadata"))
    manifest["prepared_sha256"] = {name: digest(workdir / name) for name in frozen}
    (workdir / "prepared_jobs.txt").write_text("\n".join(job["jdl"] for job in manifest["jobs"]) + "\n")
    write_batch_submission(workdir, descriptions, proxy)
    atomic_json(workdir / "production.json", manifest)
    print(f"[INFO] {len(files)} files split across {count} jobs, {args.threads} CPUs per job.")
    print(f"[INFO] Job directory: {workdir}")
    print(f"[INFO] Final summaries: {output / (label + '_selected.json')} and {output / (label + '_all_stored.json')}")
    command = [sys.executable, str(Path(__file__).resolve())]
    print("[INFO] After completion: " + shlex.join(command + ["--merge", str(workdir)]))
    if args.no_submit:
        print("[INFO] To submit: " + shlex.join(command + ["--submit", str(workdir)]))
        return 0
    return submit_prepared(workdir, proxy=proxy, shared_dir=args.shared_dir, schedd=args.schedd, pool=args.pool,
                           jdl_list=Path("prepared_jobs.txt"), batch_file=Path("prepared_batch.sub"))


def load_production(workdir):
    manifest = read_json(workdir / "production.json")
    if manifest.get("version") != 1 or not manifest.get("jobs"):
        raise ValueError("Unsupported or empty production manifest")
    seen = []
    for index, job in enumerate(manifest["jobs"]):
        if job["index"] != index or job["production"] != manifest["production"]:
            raise ValueError("Inconsistent production job IDs")
        for name in [job["jdl"], job["receipt"], *job["outputs"].values()]:
            if Path(name).name != name:
                raise ValueError("Invalid filename in production manifest")
        seen.extend(job["files"])
    if len(set(seen)) != len(seen) or sorted(seen) != sorted(manifest["files"]):
        raise ValueError("Incomplete or repeated inputs in production manifest")
    return manifest


def checked_job(workdir, manifest, job):
    receipt = read_json(workdir / "parts" / job["receipt"])
    if receipt.get("production") != manifest["production"] or receipt.get("index") != job["index"]:
        raise ValueError("Wrong production receipt")
    documents = {}
    for selection in SELECTIONS:
        path = workdir / "parts" / job["outputs"][selection]
        if digest(path) != receipt["sha256"][selection]:
            raise ValueError(f"Incomplete or changed transfer: {path.name}")
        document = read_json(path)
        role = manifest["jec"]
        if (document.get("schema_version") != 1 or document.get("dataset") != manifest["dataset"]
                or document.get("mode") != "single" or document.get("partial") or "qcd" in document
                or set(document["samples"]) != {role} or set(document["files"]) != {role}):
            raise ValueError(f"Unexpected summary: {path.name}")
        rows = document["files"][role]
        if [row["path"] for row in rows] != job["files"] or any(row["entries"] != row["processed"] for row in rows):
            raise ValueError(f"Wrong or incomplete input coverage: {path.name}")
        total = sum(row["entries"] for row in rows)
        sample = document["samples"][role]
        if document["processed_entries"] != total or sample["input_entries"] != total:
            raise ValueError(f"Wrong input accounting: {path.name}")
        if sample["cutflow"][0] + sample["certification_rejected"] + sample["pass_json_rejected"] != total:
            raise ValueError(f"Wrong certification accounting: {path.name}")
        preset = document["config"]["preset"]
        if (selection == "all_stored") != (preset == "all_stored"):
            raise ValueError(f"Wrong selection: {path.name}")
        documents[selection] = document
    if documents["selected"]["files"] != documents["all_stored"]["files"]:
        raise ValueError("Selections have different file coverage")
    return documents


def merge(workdir, manifest, force):
    from plot_jec_comparison import merge_document

    output = Path(manifest["output"])
    output.mkdir(parents=True, exist_ok=True)
    paths = {selection: output / (manifest["label"] + "_" + selection + ".json") for selection in SELECTIONS}
    with ExitStack() as stack:
        try:
            lock = stack.enter_context((output / ("." + manifest["label"] + ".merge.lock")).open("a"))
            fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
            checkpoint = output / ("." + manifest["label"] + ".checkpoint")
            if checkpoint.is_dir():
                local_lock = stack.enter_context((checkpoint / "lock").open("a"))
                fcntl.flock(local_lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except BlockingIOError as exc:
            raise RuntimeError("Another merge or local reducer is using these outputs; wait for it to finish before merging") from exc
        if any(path.exists() and not path.is_file() for path in paths.values()):
            raise ValueError("A final summary path exists and is not a regular file")
        if not force and any(path.exists() for path in paths.values()):
            raise ValueError("Final summaries already exist; use --merge WORKDIR --force to replace them")
        combined = {}
        for job in manifest["jobs"]:
            try:
                documents = checked_job(workdir, manifest, job)
            except (OSError, ValueError, KeyError, TypeError) as exc:
                raise ValueError(f"Split {job['index']} is missing or invalid: {exc}. Wait for completion or use --retry-failed WORKDIR.") from exc
            for selection, document in documents.items():
                if selection not in combined:
                    combined[selection] = document
                else:
                    merge_document(combined[selection], document)
        for selection, document in combined.items():
            rows = {row["path"]: row for row in document["files"][manifest["jec"]]}
            document["files"][manifest["jec"]] = [rows[path] for path in manifest["files"]]
        with tempfile.TemporaryDirectory(prefix=".jec_merge_", dir=output) as temporary:
            for selection, document in combined.items():
                atomic_json(Path(temporary) / paths[selection].name, document)
            for selection, document in combined.items():
                os.replace(Path(temporary) / paths[selection].name, paths[selection])
                print(f"[INFO] Saved {paths[selection]} ({document['processed_entries']} input events)")
    return 0


def main():
    args = arguments()
    existing = args.merge or args.retry_failed or args.submit
    if not existing:
        return prepare(args)
    if args.input or args.dataset or args.jec or args.no_submit or args.workdir:
        raise ValueError("Existing productions use their saved inputs and paths; do not supply new production options")
    if args.force and not args.merge:
        raise ValueError("--force is only used with --merge")
    workdir = existing.expanduser().resolve()
    manifest = load_production(workdir)
    if args.merge:
        return merge(workdir, manifest, args.force)
    for name, checksum in manifest["prepared_sha256"].items():
        if Path(name).name != name or digest(workdir / name) != checksum:
            raise ValueError(f"Prepared input changed: {name}; create a new production")
    names = []
    for job in manifest["jobs"]:
        try:
            checked_job(workdir, manifest, job)
        except (OSError, ValueError, KeyError, TypeError):
            names.append(job["jdl"])
    if not names:
        print("[INFO] All splits are complete; use --merge WORKDIR.")
        return 0
    (workdir / "pending_jobs.txt").write_text("\n".join(names) + "\n")
    return submit_prepared(workdir, resume=bool(args.submit), retry_failed=bool(args.retry_failed),
                           proxy=args.proxy, shared_dir=args.shared_dir or manifest["shared_dir"],
                           schedd=args.schedd or manifest["schedd"], pool=args.pool or manifest["pool"],
                           jdl_list=Path("pending_jobs.txt"))


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, ValueError, RuntimeError, KeyError, subprocess.CalledProcessError) as exc:
        print(f"[ERROR] {exc}", file=sys.stderr)
        raise SystemExit(1)
