#!/usr/bin/env python3
"""Depin with natural C edits only: deterministic search, no compiler targets.

  natfarm.py LIST.tsv OUT_DIR [--seconds N] [--steps N] [--beam N] [-j N]

LIST.tsv is `pinfarm.py list` output. For each function, the isolated copy
with every hint stripped (tools/unpin.py) goes to tools/translate.py with
`--targets none --natural`: every single application of each natural pass
(NATURAL_PASSES there: use a parameter instead of its copy, early return,
re-set a loop constant, move/split/reorder declarations, swap independent
statements, reorder call arguments, re-spell conditions and arithmetic),
then the best states again, scored by differing bytes. No empty blocks,
`*(&x)` re-reads or asm, so a match reads like original source.

OUT_DIR/results.tsv has autosolve's columns, so `pinfarm.py apply
OUT_DIR/results.tsv --list LIST.tsv --commit` splices the matches back
(and keeps each only if tools/check.py still says MATCH). A stopped run
restarts where it left off.
"""
import argparse
import os
import re
import shutil
import subprocess
import sys
import time
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import unpin  # noqa: E402

PY = os.path.join(ROOT, "build", "venv", "bin", "python")


def solve(row, a):
    func, iso = row[0], row[1]
    t0 = time.time()
    work = os.path.join(a.out, func)
    os.makedirs(work, exist_ok=True)
    stripped = os.path.join(work, "unpinned.c")
    open(stripped, "w").write(unpin.strip_code(open(os.path.join(ROOT, iso)).read()))
    try:
        r = subprocess.run([PY, "tools/translate.py", stripped, func, "--targets", "none", "--natural",
                            "--seconds", str(a.seconds), "--steps", str(a.steps), "--beam", str(a.beam),
                            "-j", "1"], cwd=ROOT, capture_output=True, text=True,
                           timeout=a.seconds + 300)
        log = r.stdout + r.stderr
    except subprocess.TimeoutExpired:
        log = ""
    open(os.path.join(work, "translate.log"), "w").write(log)
    m = re.search(r"base: (\d+) differing bytes", log)
    base = m.group(1) if m else ""
    secs = round(time.time() - t0)
    if re.search(r"^MATCH after", log, re.M):
        dst = os.path.join(work, "match.c")
        shutil.copy(os.path.join(ROOT, "build", "translate", func, "best.c"), dst)
        return (func, "natural", base, "TRANSLATED", 0, secs, os.path.relpath(dst, ROOT))
    m = re.search(r"^(\d+) differing bytes after", log, re.M)
    return (func, "natural", base, "no natural edit", m.group(1) if m else "", secs, "")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("list")
    ap.add_argument("out")
    ap.add_argument("--seconds", type=int, default=300)
    ap.add_argument("--steps", type=int, default=3)
    ap.add_argument("--beam", type=int, default=4)
    ap.add_argument("-j", type=int, default=1, help="functions in parallel (one core each)")
    a = ap.parse_args()
    a.out = os.path.abspath(a.out)
    os.makedirs(a.out, exist_ok=True)
    results = os.path.join(a.out, "results.tsv")
    done = {ln.split("\t")[0] for ln in open(results)} if os.path.exists(results) else set()
    rows = [ln.rstrip("\n").split("\t") for ln in open(a.list) if ln.strip()]
    rows = [r for r in rows if r[0] not in done]
    with ThreadPoolExecutor(a.j) as ex, open(results, "a") as f:
        for row in ex.map(lambda r: solve(r, a), rows):
            f.write("\t".join(map(str, row)) + "\n")
            f.flush()
            print("\t".join(map(str, row)), flush=True)


if __name__ == "__main__":
    main()
