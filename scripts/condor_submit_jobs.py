#!/usr/bin/env python3
"""Submit prepared jobs in one Condor call; duplicate checks are opt-in with --resume."""

from __future__ import annotations

import argparse
import fcntl
import getpass
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile

from eos_utils import condor_shared_directory, resolve_condor_proxy


def natural_key(path: Path) -> list:
    return [int(part) if part.isdigit() else part for part in re.split(r"(\d+)", path.name)]


def query_accepted(workdir: Path, options: list[str], env: dict) -> set[str]:
    """Include history so completed jobs are not repeated after an interrupted submission."""
    constraint = f"Owner == {json.dumps(getpass.getuser())} && Iwd == {json.dumps(str(workdir))}"
    accepted = set()
    for tool in ("condor_q", "condor_history"):
        # Empty successful queries do not always produce a JSON array. Use an
        # explicit record format for both tools, independent of site defaults.
        command = [tool] + options + ["-constraint", constraint]
        if tool == "condor_q":
            command += ["-nobatch"]
        if tool == "condor_history":
            command += ["-limit", "-1"]
        command += ["-format", "DIJET_JOB\t%d\t", "ClusterId",
                    "-format", "%d\t", "ProcId",
                    "-format", "%s\n", 'ifThenElse(isString(DijetJobScript), DijetJobScript, ifThenElse(isString(Cmd), Cmd, ""))']
        result = subprocess.run(command, text=True, capture_output=True, env=env, check=True)
        accepted.update(parse_accepted_output(result.stdout, tool))
    return accepted


def parse_accepted_output(output: str, tool: str) -> set[str]:
    """Accept an empty result, but refuse malformed or unidentifiable job rows."""
    accepted = set()
    reported = None
    records = 0
    for line in output.splitlines():
        if not line.strip() or line.lstrip().startswith("-- Schedd:"):
            continue
        summary = re.match(r"\s*Total for query:\s*(\d+)\s+jobs\b", line)
        if summary:
            reported = int(summary.group(1))
            continue
        fields = line.split("\t")
        if (len(fields) != 4 or fields[0] != "DIJET_JOB" or not fields[1].isdigit()
                or not fields[2].isdigit() or not fields[3]):
            raise RuntimeError(f"{tool} returned unexpected output; cannot safely resume. "
                               f"First unexpected line: {line[:250]!r}")
        accepted.add(Path(fields[3]).name)
        records += 1
    if reported is not None and reported != records:
        raise RuntimeError(f"{tool} reported {reported} jobs but returned {records} identifiable records; "
                           "cannot safely resume.")
    return accepted


def read_journal(path: Path) -> set[str]:
    accepted = set()
    if path.exists():
        for line in path.read_text().splitlines():
            if line.strip():
                try:
                    entry = json.loads(line)
                    if "jdl" in entry:
                        accepted.add(entry["jdl"])
                    else:
                        names = entry["jdls"]
                        if not isinstance(names, list) or any(not isinstance(name, str) for name in names):
                            raise ValueError("Invalid batch receipt")
                        accepted.update(names)
                except (json.JSONDecodeError, KeyError, TypeError, ValueError) as exc:
                    raise RuntimeError(f"Invalid submission journal: {path}; inspect it before resuming.") from exc
    return accepted


def executable_name(jdl: Path) -> str:
    match = re.search(r"^\s*executable\s*=\s*(.+?)\s*$", jdl.read_text(), re.MULTILINE | re.IGNORECASE)
    if not match or "$" in match.group(1):
        raise RuntimeError(f"Cannot identify the literal executable in {jdl}.")
    return Path(match.group(1).strip('"')).name


def write_batch_submission(workdir: Path, descriptions: list[tuple[str, str]], proxy: str,
                           filename: str = "prepared_batch.sub") -> Path:
    """Keep one executable for the cluster and dispatch each original job script.

    Input descriptions come directly from the generators, avoiding a second
    read of thousands of JDLs on AFS. The individual JDLs remain untouched.
    """
    launcher = workdir / "run_condor_batch.sh"
    blocks = []
    expected_keys = None
    for jdl_name, description in descriptions:
        settings = {}
        lines = []
        queues = 0
        for line in description.splitlines():
            if not line.strip() or line.lstrip().startswith("#"):
                continue
            if re.match(r"^\s*queue\b", line, re.IGNORECASE):
                queues += 1
                if not re.fullmatch(r"\s*queue\s+1\s*", line, re.IGNORECASE):
                    raise RuntimeError(f"Batch mode requires queue 1 in {jdl_name}; use --individual for other JDLs.")
                continue
            match = re.match(r"^\s*([^=\s]+)\s*=\s*(.*?)\s*$", line)
            if not match:
                raise RuntimeError(f"Unsupported submit syntax in {jdl_name}; use --individual.")
            key, value = match.group(1).lower(), match.group(2)
            if key in settings:
                raise RuntimeError(f"Repeated submit setting {key} in {jdl_name}; use --individual.")
            settings[key] = value
            if key not in ("universe", "executable", "arguments", "use_x509userproxy", "x509userproxy"):
                lines.append(line)
        if queues != 1:
            raise RuntimeError(f"Batch mode requires one queue statement in {jdl_name}; use --individual.")
        if settings.get("universe", "vanilla").lower() != "vanilla" or settings.get("arguments"):
            raise RuntimeError(f"Batch mode expects vanilla script jobs without arguments: {jdl_name}. Use --individual.")
        original = settings.get("executable", "").strip('"')
        if not original or "$" in original:
            raise RuntimeError(f"Cannot identify the job script in {jdl_name}.")
        script_name = Path(original).name
        if not script_name.lower().endswith(".sh"):
            raise RuntimeError(f"Batch mode requires Bash .sh job scripts: {jdl_name}. Use --individual for other executables.")
        keys = set(settings) - {"universe", "executable", "arguments", "use_x509userproxy", "x509userproxy"}
        if expected_keys is not None and keys != expected_keys:
            raise RuntimeError("Prepared jobs have different sets of submit options; use --individual.")
        expected_keys = keys
        inputs = settings.get("transfer_input_files", "")
        if script_name not in {Path(token.strip()).name for token in inputs.split(",") if token.strip()}:
            inputs = inputs + ", " + original if inputs else original
            lines = [line for line in lines if not re.match(r"^\s*transfer_input_files\s*=", line, re.IGNORECASE)]
            lines.append("transfer_input_files = " + inputs)
        if "batch_name" not in settings:
            label = re.sub(r"_n\d+$", "", Path(jdl_name).stem)
            lines.append("batch_name = " + label)
        if "+dijetdataset" not in settings:
            label = re.sub(r"_n\d+$", "", Path(jdl_name).stem)
            lines.append("+DijetDataset = " + json.dumps(label))
        # Condor's new arguments syntax: one quoted filename, including spaces.
        argument = "'" + script_name.replace("'", "''").replace('"', '""') + "'"
        lines += ["arguments = \"" + argument + "\"",
                  "+DijetJobScript = " + json.dumps(script_name), "queue 1"]
        blocks.append("\n".join(lines))
    if not blocks:
        raise RuntimeError("No jobs selected for batch submission.")
    launcher.write_text('''#!/usr/bin/env bash
set -euo pipefail
if [[ $# -ne 1 ]]; then
  echo "[ERROR] Expected one job script filename." >&2
  exit 2
fi
exec bash -- "$1"
''')
    launcher.chmod(0o755)
    batch = workdir / filename
    batch.write_text("universe = vanilla\nexecutable = " + launcher.name +
                     "\nuse_x509userproxy = true\nx509userproxy = " + proxy + "\n\n" +
                     "\n\n".join(blocks) + "\n")
    return batch


def submit_batch(workdir: Path, jdls: list[Path], batch: Path, proxy: str,
                 options: list[str], env: dict, staging: Path) -> int:
    command = ["condor_submit"] + options + ["-terse", "-single-cluster", "-maxjobs", str(len(jdls)), "-append",
               f"x509userproxy = {proxy}", str(batch)]
    print(f"[INFO] Submitting {len(jdls)} jobs in one Condor transaction.")
    with tempfile.TemporaryDirectory(prefix=".condor_submit_", dir=staging) as temporary:
        result = subprocess.run(command, cwd=workdir,
                                env=dict(env, TMPDIR=temporary, TMP=temporary, TEMP=temporary),
                                text=True, capture_output=True)
    if result.returncode:
        print(f"[ERROR] Batch submission failed:\n{result.stderr or result.stdout}", file=sys.stderr)
        return 1
    job_ids = result.stdout.strip()
    match = re.fullmatch(r"(\d+)\.(\d+)(?:\s*-\s*(\d+)\.(\d+))?", job_ids)
    count = (int(match.group(4)) - int(match.group(2)) + 1) if match and match.group(4) else 1
    if not match or (match.group(3) and match.group(1) != match.group(3)) or count != len(jdls):
        raise RuntimeError(f"Unexpected batch submission response: {job_ids!r}. "
                           "Jobs may have been accepted; use --resume before retrying.")
    # One receipt and one fsync per dataset, not one per job.
    with (workdir / "submitted_jobs.jsonl").open("a") as journal:
        journal.write(json.dumps({"jdls": [jdl.name for jdl in jdls], "job_ids": job_ids}) + "\n")
        journal.flush()
        os.fsync(journal.fileno())
    print(f"[INFO] Submitted {len(jdls)} jobs: {job_ids}")
    return 0


def submit_prepared(workdir: Path, *, resume: bool = False, proxy: str | None = None,
                    shared_dir: str | None = None, schedd: str | None = None,
                    pool: str | None = None, jdl_list: Path | None = None,
                    batch_file: Path | None = None, individual: bool = False) -> int:
    workdir = workdir.expanduser().resolve()
    if not workdir.is_dir():
        raise RuntimeError(f"Job directory does not exist: {workdir}")
    if jdl_list is not None:
        list_path = jdl_list if jdl_list.is_absolute() else workdir / jdl_list
        names = list(dict.fromkeys(line.strip() for line in list_path.read_text().splitlines() if line.strip()))
        if any(Path(name).name != name or not name.endswith(".jdl") for name in names):
            raise RuntimeError(f"Expected JDL filenames in {list_path}.")
        jdls = [workdir / name for name in names]
        if (resume or individual or batch_file is None) and any(not path.is_file() for path in jdls):
            raise RuntimeError(f"A prepared JDL is missing from {workdir}.")
    else:
        jdls = sorted((path for path in workdir.glob("*.jdl") if path.name != "submit_all.jdl"), key=natural_key)
    if not jdls:
        raise RuntimeError(f"No prepared JDLs found in {workdir}")
    staging = condor_shared_directory(shared_dir)
    staging.mkdir(mode=0o700, parents=True, exist_ok=True)
    proxy = resolve_condor_proxy(proxy, staging)
    env = os.environ.copy()
    env.pop("PYTHONHOME", None)
    env.pop("PYTHONPATH", None)
    options = (["-name", schedd] if schedd else []) + (["-pool", pool] if pool else [])
    journal_path = workdir / "submitted_jobs.jsonl"
    print(f"[INFO] Submission proxy: {proxy}")
    print(f"[INFO] Credential temporary files: {staging}")
    with (workdir / ".submission.lock").open("a") as lock:
        try:
            fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except BlockingIOError as exc:
            raise RuntimeError(f"Another submission is already using {workdir}.") from exc
        recorded = read_journal(journal_path) if resume else set()
        accepted = query_accepted(workdir, options, env) if resume else set()
        unrecorded = [jdl for jdl in jdls if jdl.name not in recorded]
        known = [jdl for jdl in unrecorded if executable_name(jdl) in accepted] if resume else []
        known_names = {jdl.name for jdl in known}
        pending = [jdl for jdl in unrecorded if jdl.name not in known_names]
        skipped = len(jdls) - len(pending)
        if resume:
            print(f"[INFO] Prepared: {len(jdls)}; already accepted: {skipped}; remaining: {len(pending)}")
        else:
            print(f"[INFO] Duplicate checks disabled; submitting all {len(pending)} prepared jobs.")
        if not pending and not known:
            return 0
        submitted = 0
        if known:
            with journal_path.open("a") as journal:
                journal.write("".join(json.dumps({"jdl": jdl.name, "source": "queue/history"}) + "\n" for jdl in known))
                journal.flush()
                os.fsync(journal.fileno())
        if not pending:
            return 0
        if not individual:
            if batch_file is not None and not resume:
                batch = batch_file if batch_file.is_absolute() else workdir / batch_file
                if not batch.is_file():
                    raise RuntimeError(f"Prepared batch submit file is missing: {batch}")
            else:
                batch = write_batch_submission(workdir, [(jdl.name, jdl.read_text()) for jdl in pending],
                                               proxy, "selected_batch.sub")
            return submit_batch(workdir, pending, batch, proxy, options, env, staging)
        # Keep accepted IDs on disk even when this process is interrupted later.
        with journal_path.open("a") as journal:
            for jdl in pending:
                label = re.sub(r"_n\d+$", "", jdl.stem) if re.search(r"_n\d+$", jdl.stem) else workdir.name
                command = ["condor_submit"] + options + ["-terse", "-append", f"x509userproxy = {proxy}"]
                description = jdl.read_text()
                if not re.search(r"^\s*batch_name\s*=", description, re.MULTILINE | re.IGNORECASE):
                    command += ["-batch-name", label]
                if not re.search(r"^\s*\+DijetDataset\s*=", description, re.MULTILINE | re.IGNORECASE):
                    command += ["-append", "+DijetDataset = " + json.dumps(label)]
                command.append(jdl.name)
                # The credential producer inherits TMPDIR; isolate and clean each invocation.
                with tempfile.TemporaryDirectory(prefix=".condor_submit_", dir=staging) as temporary:
                    submit_env = dict(env, TMPDIR=temporary, TMP=temporary, TEMP=temporary)
                    result = subprocess.run(command, cwd=workdir, env=submit_env, text=True, capture_output=True)
                if result.returncode:
                    print(f"\n[ERROR] Submission failed for {jdl.name}:\n{result.stderr or result.stdout}", file=sys.stderr)
                    print(f"[INFO] Newly submitted: {submitted}; skipped: {skipped}; still pending: {len(pending) - submitted}.", file=sys.stderr)
                    print("[INFO] Stopped at the first failure. Fix the reported issue, then rerun with --resume.", file=sys.stderr)
                    return 1
                job_ids = result.stdout.strip()
                if not re.fullmatch(r"\d+\.\d+(?:\s*-\s*\d+\.\d+)?", job_ids):
                    raise RuntimeError(f"Unexpected condor_submit response for {jdl.name}: {job_ids}. "
                                       "The job may have been accepted; use --resume to query Condor before retrying.")
                journal.write(json.dumps({"jdl": jdl.name, "job_ids": job_ids}) + "\n")
                journal.flush()
                os.fsync(journal.fileno())
                submitted += 1
                print(f"\r[SUB] Accepted {submitted}/{len(pending)} new jobs ({skipped} already accepted)", end="", flush=True)
        print(f"\n[INFO] Submitted {submitted} new jobs; {skipped} were already accepted.")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("job_directory", type=Path, help="Existing folder containing prepared .jdl files")
    parser.add_argument("--resume", action="store_true", help="Skip accepted jobs found in the queue, history, and submission journal")
    parser.add_argument("--proxy", help="Active proxy; /tmp proxies are staged automatically")
    parser.add_argument("--shared-dir", help="Credential staging directory (default: CERN work/private or LPC home/private)")
    parser.add_argument("--schedd", help="Scheduler name, if different from your default")
    parser.add_argument("--pool", help="Condor central manager, if needed")
    parser.add_argument("--jdl-list", type=Path, help="File listing selected JDL filenames (default: all JDLs in the folder)")
    parser.add_argument("--batch-file", type=Path, help="Prebuilt batch submit file; --resume rebuilds it for the remaining jobs")
    parser.add_argument("--individual", action="store_true", help="Use a separate condor_submit call per JDL (compatibility mode)")
    args = parser.parse_args()
    try:
        return submit_prepared(args.job_directory, resume=args.resume, proxy=args.proxy,
                               shared_dir=args.shared_dir, schedd=args.schedd, pool=args.pool, jdl_list=args.jdl_list,
                               batch_file=args.batch_file, individual=args.individual)
    except subprocess.CalledProcessError as exc:
        print(f"[ERROR] {' '.join(exc.cmd)} failed: {(exc.stderr or exc.stdout or '').strip()}", file=sys.stderr)
        return 1
    except (OSError, RuntimeError) as exc:
        print(f"[ERROR] {exc}", file=sys.stderr)
        return 1
    except KeyboardInterrupt:
        print("\n[INFO] Interrupted. Rerun with --resume to continue without repeating accepted jobs.", file=sys.stderr)
        return 130


if __name__ == "__main__":
    raise SystemExit(main())
