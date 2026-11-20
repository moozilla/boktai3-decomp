"""Optional PC-to-C evidence from an exact scope build of the running ROM."""
from collections import Counter
import hashlib
import json
import os
import re
import subprocess

from .fields import ROOT

MAX_FUNCTIONS = 6
PYTHON = ROOT / "build/venv/bin/python"
SCOPE = ROOT / "tools/scope/scope.py"


def exact_rows(lines, rows):
    if "MATCH" not in lines.splitlines() or not rows or any(row["tag"] != "=" for row in rows):
        return {}
    return {row["rom_addr"]: row for row in rows if row.get("rom_addr")}


def source_location(row, source):
    label = str(row.get("line", ""))
    if label.isdecimal():
        return source, int(label)
    file, _, number = label.rpartition(":")
    if not number.isdecimal() or not file:
        return None, None
    path = (ROOT / file).resolve()
    if not path.is_relative_to(ROOT) or path.relative_to(ROOT).parts[0] not in {"src", "include"}:
        return None, None
    return path, int(number)


def instruction_evidence(row, source):
    file, line = source_location(row, source)
    detail = {"instruction": row["rom"], "compiler_pass": row.get("born") or None}
    if file and line and file.is_file():
        raw = file.read_bytes()
        lines = raw.decode("utf-8", errors="replace").splitlines()
        if 1 <= line <= len(lines):
            first = max(1, line - 3)
            detail.update(file=str(file.relative_to(ROOT)), line=line, first=first,
                          excerpt=lines[first - 1:line + 3],
                          source_sha256=hashlib.sha256(raw).hexdigest())
    return detail


def annotate(run_id, events):
    candidates = {}
    weight = Counter()
    for event in events:
        if not (fn := event["function"]) or not fn["source"] or event["kind"] != "write":
            continue
        key = fn["address"]
        candidates.setdefault(key, fn)
        weight[key] += 1
    if not (ROOT / "build/scope/agbcc").is_dir() or not PYTHON.is_file():
        return {"available": False, "mapped": [], "reason": "Instrumented scope compiler unavailable"}
    session = f"experiment-{run_id}"
    env = {k: v for k, v in os.environ.items() if not k.startswith(("AGBCC", "CSE_", "SCOPE_")) and k != "CPP"}
    env.update(SCOPE_SERVER="0", SCOPE_RUN=session)
    traces = {}
    for key, _ in weight.most_common(MAX_FUNCTIONS):
        fn = candidates[key]
        name = fn["defined"]
        source = ROOT / fn["source"]
        if not source.is_file() or not isinstance(name, str) or not re.fullmatch(r"[A-Za-z_]\w*", name):
            continue
        try:
            run = subprocess.run([PYTHON, SCOPE, source, name], cwd=ROOT, env=env,
                                 text=True, capture_output=True, timeout=12)
            report = ROOT / "build/scope" / session / name / "diff.json"
            if run.returncode == 0 and report.is_file():
                rows = exact_rows(run.stdout, json.loads(report.read_text()))
                if rows:
                    traces[key] = source, rows
        except (OSError, ValueError, subprocess.SubprocessError):
            continue
    details = {}
    for event in events:
        fn = event["function"]
        if fn and (trace := traces.get(fn["address"])) and (row := trace[1].get(event["pc"])):
            pc = event["pc"]
            if pc not in details:
                details[pc] = instruction_evidence(row, trace[0])
            event["compiler"] = details[pc]
    return {"available": True, "mapped": list(traces)}
