#!/usr/bin/env python3
"""Depin with natural C edits only: deterministic search, no compiler targets.

  natfarm.py LIST.tsv OUT_DIR [--seconds N] [--steps N] [--beam N] [-j N] [--classic]

LIST.tsv is `pinfarm.py list` output. For each function, the isolated copy
with every hint stripped (tools/unpin.py) goes to tools/translate.py with
`--targets none --natural`: every single application of each natural pass
(NATURAL_PASSES there: use a parameter instead of its copy, early return,
re-set a loop constant, move/split/reorder declarations, swap independent
statements, reorder call arguments, re-spell conditions and arithmetic),
then the best states again, scored by differing bytes. No empty blocks,
`*(&x)` re-reads or asm, so a match reads like original source. A
`do { } while (0)` around statements is allowed: it is how a statement macro
expands, so write it as a named macro when the match is applied (an empty
one becomes DEBUG_NOP(), global.h); an `if (1)` block is rejected.

The search is translate.py's --lazy mode: neighbors are generated one pass
at a time, the passes that most often lead to matches first (LEARNED_ORDER
there), and it stops at the first match. On two 48-function samples it found
the same matches as the old breadth-first search in half the CPU time, so the
default budget is 120 s instead of 300 s (notes/depin-search-2026-10-23.md).
--classic runs the old search.

A triage pass first compiles every stripped copy once: copies that already
match are recorded straight away, and the search then takes the rest in
order of promise (allocation-only differences first, then fewest differing
bytes; OUT_DIR/triage.tsv). --no-triage keeps the list order.

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


def solve(row, a, seconds=None):
    func, iso = row[0], row[1]
    seconds = a.seconds if seconds is None else seconds
    t0 = time.time()
    work = os.path.join(a.out, func)
    os.makedirs(work, exist_ok=True)
    stripped = os.path.join(work, "unpinned.c")
    open(stripped, "w").write(unpin.strip_code(open(os.path.join(ROOT, iso)).read()))
    try:
        r = subprocess.run([PY, "tools/translate.py", stripped, func, "--targets", "none", "--natural",
                            *([] if a.classic else ["--lazy"]),
                            "--seconds", str(seconds), "--steps", str(a.steps), "--beam", str(a.beam),
                            "-j", "1"], cwd=ROOT, capture_output=True, text=True,
                           timeout=seconds + 300)
        log = r.stdout + r.stderr
    except subprocess.TimeoutExpired:
        log = ""
    open(os.path.join(work, "translate.log"), "w").write(log)
    m = re.search(r"base: (\d+) differing bytes(?: \(shape (\d+)\))?", log)
    base = m.group(1) if m else ""
    if seconds == 0 and not re.search(r"^MATCH after", log, re.M):
        return (func, "triage", base, m.group(2) if m and m.group(2) else "", "", round(time.time() - t0), "")
    secs = round(time.time() - t0)
    if re.search(r"^MATCH after", log, re.M):
        dst = os.path.join(work, "match.c")
        text = open(os.path.join(ROOT, "build", "translate", func, "best.c")).read()
        if re.search(r"\bif\s*\(\s*1\s*\)", text):
            return (func, "natural", base, "if (1) block", 0, secs, "")
        # an empty do/while(0) is a compiled-out debug macro (global.h)
        text = re.sub(r"\bdo\s*\{\s*\}\s*while\s*\(\s*0\s*\)\s*;", "DEBUG_NOP();", text)
        open(dst, "w").write(text)
        return (func, "natural", base, "TRANSLATED", 0, secs, os.path.relpath(dst, ROOT))
    m = re.search(r"^(\d+) differing bytes after", log, re.M)
    return (func, "natural", base, "no natural edit", m.group(1) if m else "", secs, "")


def triage(rows, a, results):
    """Compile every function's stripped copy once (a few seconds each):
    record the ones that already match, and order the rest so that
    allocation-only differences (shape 0: the ROM's instructions in other
    registers, matched about 4x as often) come first, then by differing bytes;
    functions whose copy does not compile go last."""
    path = os.path.join(a.out, "triage.tsv")
    known = {}
    if os.path.exists(path):
        for ln in open(path):
            p = ln.rstrip("\n").split("\t")
            known[p[0]] = p
    todo = [r for r in rows if r[0] not in known]
    with ThreadPoolExecutor(a.j) as ex, open(path, "a") as tf, open(results, "a") as rf:
        for row in ex.map(lambda r: solve(r, a, 0), todo):
            if row[1] == "triage":
                known[row[0]] = list(row)
                tf.write("\t".join(map(str, row)) + "\n")
                tf.flush()
            else:  # matched (or failed) without any search
                rf.write("\t".join(map(str, row)) + "\n")
                rf.flush()
                print("\t".join(map(str, row)), flush=True)
    rows = [r for r in rows if r[0] in known]

    def key(r):
        _, _, base, shape = known[r[0]][:4]
        if not base:
            return (2, 0)
        return (0 if shape == "0" else 1, int(base))
    return sorted(rows, key=key)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("list")
    ap.add_argument("out")
    ap.add_argument("--seconds", type=int, default=120)
    ap.add_argument("--classic", action="store_true", help="breadth-first search (before 2026-10-23)")
    ap.add_argument("--no-triage", action="store_true", help="search in list order, without the triage pass")
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
    if not a.no_triage:
        rows = triage(rows, a, results)
    with ThreadPoolExecutor(a.j) as ex, open(results, "a") as f:
        for row in ex.map(lambda r: solve(r, a), rows):
            f.write("\t".join(map(str, row)) + "\n")
            f.flush()
            print("\t".join(map(str, row)), flush=True)


if __name__ == "__main__":
    main()
