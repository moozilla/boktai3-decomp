#!/usr/bin/env python3
"""Consolidate src/fn/ files into subsystem units, one headless worker per subsystem.

  unitize.py list [--min 8]                subsystems with at least --min src/fn files
  unitize.py run [--jobs 4] [--backend codex|claude] [--model M] [--min 8] [--only NAME,...] [--limit K]
                                           a worktree per subsystem (tools/worktree.sh,
                                           branch unit/<slug>), a worker (default codex exec
                                           gpt-6.1-sol, workspace-write: no Claude usage)
                                           following docs/UNIT_WORKER.md, then this
                                           script's own verification
  unitize.py status                        per subsystem: worker state and verification

Verification after each worker: the worktree's tools/build.py prints
`build/boktai3.gba: OK`, the unit file exists, and tools/progress.py reports
no fewer matched bytes than before. Verified branches are listed for batch
integration (merge each unit/<slug> branch, combined build, one PR). Logs and
state live in build/unitize/.
"""
import argparse
import csv
import json
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import progress  # noqa: E402
from romlib import ROOT  # noqa: E402
from split import c_functions  # noqa: E402

OUT = os.path.join(ROOT, "build", "unitize")
WT = os.path.join(os.path.dirname(ROOT), "wt")
TRANSIENT = ("at capacity", "rate limit", "Rate limit", "overloaded", "Internal server error", "stream disconnected")
QUOTA_FLOOR = 15        # % of the Codex weekly quota to leave for the user (tools/codex_quota.py)


def slug(name):
    s = re.sub(r"\(.*?\)", "", name).lower()
    return re.sub(r"[^a-z0-9]+", "_", s).strip("_")[:40]


def subsystems(min_files):
    rows = list(csv.DictReader(open(os.path.join(ROOT, "symbols", "progress_subsystems.csv"))))
    funcs, _ = progress.load_functions()
    addr = {n: a for n, a in funcs}
    addr.update({f"sub_{a:08X}": a for n, a in funcs})
    where = []                                     # (first address, file) for every C file
    for p in sorted(Path(ROOT, "src").rglob("*.c")):
        a = [addr[f] for f in c_functions(p) if f in addr]
        if a:
            where.append((min(a), max(a), str(p.relative_to(ROOT))))
    out = []
    for r in rows:
        lo, hi = int(r["start"], 16), int(r["end"], 16)
        files = [f for a0, a1, f in sorted(where) if lo <= a0 < hi or lo <= a1 < hi]
        nfn = sum(1 for f in files if f.startswith("src/fn/"))
        if nfn >= min_files:
            out.append(dict(name=r["name"], start=r["start"], end=r["end"], slug=slug(r["name"]),
                            files=files, fn=nfn, size=hi - lo))
    return out


def matched_bytes(root):
    r = subprocess.run([sys.executable, "tools/progress.py"], cwd=root, capture_output=True, text=True)
    m = re.search(r"code bytes: (\d+)/\d+", r.stdout + r.stderr)
    return int(m.group(1).replace(",", "")) if m else None


BRIEF = """Follow docs/UNIT_WORKER.md (read CLAUDE.md first) for one subsystem:

  name:  {name}
  range: {start}-{end}  (END exclusive)
  unit:  src/{slug}.c with shared types in include/{slug}.h (reuse an existing header
         in include/ if this is the same object)

`build/venv/bin/python tools/unit_files.py {start} {end}` lists the {n} files to merge.
Do the whole end state: shared header types, one merged unit in ROM order, every
function still matching, `tools/build.py` printing `build/boktai3.gba: OK`. Name
functions you understand with a file-local `#define NewName sub_XXXXXXXX` alias
(integration promotes them). Do not run git commit (the driver commits after its own
verification; your sandbox cannot write git metadata); leave the finished work in the tree.
Don't edit shared docs (docs/HANDOFF.md, INDEX.md, history/, ENGINE.md, GAME_LAYOUT.md):
every unit worker would collide there. Put findings in notes/{slug}.md instead.
Finish with the brief's short report.
"""


def state_path(s):
    return os.path.join(OUT, s["slug"] + ".json")


def run_one(s, model, base, backend="codex", effort="medium", recheck=False, fallback=None):
    st = {"name": s["name"], "slug": s["slug"]}
    if os.path.exists(state_path(s)):
        st = json.load(open(state_path(s)))
        transient = st.get("state") == "failed" and os.path.exists(os.path.join(OUT, s["slug"] + ".log")) and \
            any(e in open(os.path.join(OUT, s["slug"] + ".log"), errors="replace").read()
                for e in TRANSIENT) and st.get("retries", 0) < 2
        if transient:          # the worker died on a service error: start it again from a clean tree
            wt = os.path.join(WT, "unit-" + s["slug"])
            subprocess.run(["git", "checkout", "--", "."], cwd=wt, capture_output=True)
            subprocess.run(["git", "clean", "-fdq", "src", "include", "notes", "wip"], cwd=wt, capture_output=True)
            st = {**st, "state": "retry", "retries": st.get("retries", 0) + 1}
        elif st.get("state") == "verified" or (st.get("state") == "failed" and not recheck):
            return st
    if backend == "codex" and subprocess.run([sys.executable, os.path.join(ROOT, "tools", "codex_quota.py"),
                                              "--floor", str(QUOTA_FLOOR)]).returncode:
        if not fallback:
            return {**st, "state": "paused (Codex quota below floor)"}
        backend, model = "claude", fallback          # Codex budget spent: continue on Claude
    d = os.path.join(WT, "unit-" + s["slug"])
    if not os.path.isdir(d):
        subprocess.run(["sh", "tools/worktree.sh", "unit-" + s["slug"], base], cwd=ROOT, check=True,
                       env={**os.environ, "BRANCH_PREFIX": "unit"}, capture_output=True)
    before = st.get("before") or matched_bytes(ROOT)     # the base this worktree was cut from
    log = os.path.join(OUT, s["slug"] + ".log")
    if recheck:
        return verify(s, st, d, before)
    st["backend"] = f"{backend}:{model}"
    brief = BRIEF.format(n=len(s["files"]), **s)
    with open(log, "w") as fh:
        try:
            if backend == "codex":       # bills to the OpenAI plan, not Claude usage
                subprocess.run(["codex", "exec", "-m", model, "-s", "workspace-write", "-C", d,
                                "-c", f"model_reasoning_effort={effort}", brief],
                               cwd=d, stdout=fh, stderr=subprocess.STDOUT, timeout=4 * 3600,
                               stdin=subprocess.DEVNULL)
            else:
                subprocess.run(["claude", "-p", brief, "--model", model,
                                "--permission-mode", "bypassPermissions", "--output-format", "text"],
                               cwd=d, stdout=fh, stderr=subprocess.STDOUT, timeout=4 * 3600)
        except subprocess.TimeoutExpired:
            fh.write("\n# worker timed out\n")
    return verify(s, st, d, before)


def verify(s, st, d, before):
    """The driver's own checks on a finished worker's tree, then one commit per subsystem."""
    subprocess.run(["make", "format"], cwd=d, capture_output=True)
    b = subprocess.run([sys.executable, "tools/build.py"], cwd=d, capture_output=True, text=True)
    ok = "build/boktai3.gba: OK" in b.stdout + b.stderr
    after = matched_bytes(d)
    unit = os.path.exists(os.path.join(d, "src", s["slug"] + ".c"))
    good = ok and unit and after is not None and before is not None and after >= before
    if good:                      # the worker leaves its changes; commit them here, one per subsystem
        subprocess.run(["git", "add", "-A", "src", "include", "symbols", "notes"], cwd=d, check=True)
        subprocess.run(["git", "checkout", "--", "docs"], cwd=d, capture_output=True)   # shared docs collide
        subprocess.run(["git", "commit", "-q", "-m", f"Unit {s['slug']}: merge {s['fn']} files into src/{s['slug']}.c\n\n"
                        f"{s['name']} ({s['start']}-{s['end']}); tools/unitize.py, build OK, "
                        f"matched bytes {before} -> {after}.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>"],
                       cwd=d, capture_output=True)
    dirty = subprocess.run(["git", "status", "--porcelain", "--", "src", "include", "symbols"], cwd=d,
                           capture_output=True, text=True).stdout.strip()
    for sub in ("src", "asm", "check"):          # regenerable outputs (tools/clean.py's set): save disk
        p = os.path.join(d, "build", sub)
        if os.path.isdir(p) and not os.path.islink(p):
            shutil.rmtree(p)
    st.update(state="verified" if good and not dirty else "failed",
              build_ok=ok, unit=unit, before=before, after=after, uncommitted=bool(dirty),
              branch="unit/unit-" + s["slug"], worktree=d)
    json.dump(st, open(state_path(s), "w"), indent=1)
    return st


def cmd_list(a):
    for s in subsystems(a.min):
        print(f"{s['start']}-{s['end']} {s['fn']:4d} fn files  {s['slug']:40s} {s['name']}")


def cmd_run(a):
    os.makedirs(OUT, exist_ok=True)
    todo = subsystems(a.min)
    if a.only:
        want = set(a.only.split(","))
        todo = [s for s in todo if s["slug"] in want or s["name"] in want]
    if a.range:
        lo, hi = (int(x, 16) for x in a.range.split("-"))
        todo = [s for s in todo if lo <= int(s["start"], 16) and int(s["end"], 16) <= hi]
    todo = sorted(todo, key=lambda s: -s["fn"])[:a.limit]
    base = subprocess.run(["git", "rev-parse", "HEAD"], cwd=ROOT, capture_output=True, text=True).stdout.strip()
    print(f"{len(todo)} subsystems from {base[:8]}, {a.jobs} at a time", flush=True)
    with ThreadPoolExecutor(a.jobs) as ex:
        for st in ex.map(lambda s: run_one(s, a.model, base, a.backend, a.effort, a.recheck, a.fallback), todo):
            print(f"{st['slug']}: {st.get('state')} (build {st.get('build_ok')}, "
                  f"bytes {st.get('before')} -> {st.get('after')})", flush=True)


def cmd_status(a):
    for p in sorted(os.listdir(OUT)) if os.path.isdir(OUT) else []:
        if p.endswith(".json"):
            st = json.load(open(os.path.join(OUT, p)))
            print(f"{st['slug']:40s} {st.get('state'):9s} build={st.get('build_ok')} "
                  f"bytes {st.get('before')} -> {st.get('after')} {st.get('branch', '')}")


def main():
    ap = argparse.ArgumentParser()
    sp = ap.add_subparsers(dest="cmd", required=True)
    p = sp.add_parser("list")
    p.add_argument("--min", type=int, default=8)
    p = sp.add_parser("run")
    p.add_argument("--jobs", type=int, default=4)
    p.add_argument("--backend", choices=["codex", "claude"], default="codex")
    p.add_argument("--model", default="gpt-6.1-sol")
    p.add_argument("--effort", default="medium", help="codex reasoning effort")
    p.add_argument("--min", type=int, default=8)
    p.add_argument("--only")
    p.add_argument("--limit", type=int, default=10 ** 6)
    p.add_argument("--recheck", action="store_true", help="re-verify finished worktrees; run no workers")
    p.add_argument("--range", help="START-END: only subsystems inside this address range")
    p.add_argument("--fallback", help="Claude model (e.g. sonnet) to use once the Codex quota floor is reached")
    sp.add_parser("status")
    a = ap.parse_args()
    {"list": cmd_list, "run": cmd_run, "status": cmd_status}[a.cmd](a)


if __name__ == "__main__":
    main()
