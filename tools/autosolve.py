#!/usr/bin/env python3
"""Run the register-allocation solver over many functions.

  autosolve.py LIST.tsv [--translate-seconds N] [-j N] [--out DIR]
  autosolve.py FILE.c FUNC [...]

LIST.tsv rows are "FUNC<TAB>FILE.c" (FILE.c isolated with tools/isolate.py,
extra columns ignored). For each function, and for a copy with its register
pins and empty asm barriers removed (tools/unpin.py):

1. if it already matches, report that (an unpinned copy that matches means
   the hints were the problem);
2. tools/rasolve.py looks for allocation-priority and reload-rotation
   changes, inside the instrumented compiler, that make the bytes match;
3. when it finds an exact one, tools/translate.py searches C for it.

Results go to DIR/results.tsv (default build/autosolve) with the matching
sources in DIR/FUNC/. Nothing in src/ is changed; review and commit by hand.
"""
import argparse
import json
import os
import re
import signal
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import unpin  # noqa: E402
from romlib import ROOT  # noqa: E402

PY = sys.executable


def killpg(pid):
    try:
        os.killpg(pid, signal.SIGKILL)
    except OSError:  # nothing left (macOS says EPERM once only zombies remain)
        pass


def run(cmd, timeout):
    # its own process group, so a timeout also stops the compiler servers it forked
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, cwd=ROOT,
                         start_new_session=True)
    try:
        out, _ = p.communicate(timeout=timeout)
    except subprocess.TimeoutExpired:
        killpg(p.pid)
        out, _ = p.communicate()
    killpg(p.pid)  # servers left behind by a normal exit
    return out or ""


def distance(path, func):
    """Halfwords from the ROM after alignment, from one compile (None on failure)."""
    try:
        import rasolve
        import permute
        cc = rasolve.Compiler(os.path.abspath(path), func, lines=False)
        cc.pre, _ = permute.prepare_source(os.path.relpath(path, ROOT), func)
        b, _ = cc.run()
        if cc.server:
            cc.server.close()
        return b
    except BaseException:
        return None


def solve_one(func, path, out, a):
    rows = []
    variants = [("as is", path)]
    text = open(path).read()
    if a.triage:
        # one compile: a copy too far for the C search is not worth solving
        b = distance(path, func)
        if b is not None and b > a.max_bytes and unpin.strip_code(text) == text:
            return [(func, "as is", b, "too far; skipped", "", 0, "")]
    stripped = unpin.strip_code(text)
    if stripped != text:
        up = os.path.join(out, func, "unpinned.c")
        os.makedirs(os.path.dirname(up), exist_ok=True)
        open(up, "w").write(stripped)
        # one compile says whether the unpinned copy is worth solving
        try:
            import rasolve
            import permute
            cc = rasolve.Compiler(os.path.abspath(up), func, lines=False)
            cc.pre, _ = permute.prepare_source(os.path.relpath(up, ROOT), func)
            b, _ = cc.run()
            if cc.server:
                cc.server.close()
        except BaseException:
            b = None
        if b == 0:
            return [(func, "unpinned", 0, "MATCHES", 0, 0, up)]
        if b is not None and b <= a.max_bytes:
            variants.append(("unpinned", up))
        else:
            print(f"{func}: unpinned copy differs in {b} bytes; skipped", flush=True)
    for name, src in variants:
        t0 = time.time()
        tj = os.path.join(out, func, f"targets-{name.replace(' ', '_')}.json")
        os.makedirs(os.path.dirname(tj), exist_ok=True)
        log = run([PY, "tools/rasolve.py", src, func, "--seconds", str(a.solve_seconds), "--depth", "3",
                   "--json-out", tj, "-j", str(a.j), "--max-compiles", str(a.solve_compiles)],
                  a.solve_seconds + a.grace)
        m = re.search(rf"{re.escape(func)}: (\d+) differing bytes", log)
        base = int(m.group(1)) if m else None
        if "already matches" in log:
            rows.append((func, name, base, "MATCHES", 0, round(time.time() - t0), src))
            return rows
        if not os.path.exists(tj):
            kind = "not allocation" if "no priority" in log else "no solution"
            targets = "none"
        else:
            sol = json.load(open(tj))
            kind = "solved in compiler" if sol["score"] == 0 else f"partial in compiler ({sol['score']} left)"
            targets = tj
        # how far C has to go: the draft's distance, or less when the
        # compiler-space solution leaves only a small residue
        reach = base if targets == "none" else min(base or 10 ** 9, sol["score"] + 16)
        if base is None or reach > a.max_bytes:
            rows.append((func, name, base, kind + "; too far for C search", "", round(time.time() - t0), ""))
            continue
        budget = a.translate_seconds
        tlog = run([PY, "tools/translate.py", src, func, "--targets", targets, "--seconds",
                    str(budget), "-j", str(a.j), "--steps", str(a.steps), "--beam", str(a.beam)],
                   budget + a.grace)
        mm = re.search(r"\n(MATCH|(\d+) differing bytes) after (\d+) edit", tlog)
        best = os.path.join(ROOT, "build", "translate", func, "best.c")
        if mm and mm.group(1) == "MATCH":
            dst = os.path.join(out, func, f"match-{name.replace(' ', '_')}.c")
            os.replace(best, dst)
            rows.append((func, name, base, "TRANSLATED", 0, round(time.time() - t0), dst))
            return rows
        left = mm.group(2) if mm else "?"
        rows.append((func, name, base, f"{kind}; C search at {left} bytes", left,
                     round(time.time() - t0), ""))
    return rows


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("items", nargs="+")
    ap.add_argument("--solve-seconds", type=int, default=120)
    ap.add_argument("--translate-seconds", type=int, default=300)
    ap.add_argument("-j", type=int, default=max(1, (os.cpu_count() or 2) // 2),
                    help="worker processes (default: half the cores; all of them saturated a 16 GB machine)")
    ap.add_argument("--max-bytes", type=int, default=64, help="skip C search beyond this many differing bytes")
    ap.add_argument("--out", default=os.path.join(ROOT, "build", "autosolve"))
    ap.add_argument("--solve-compiles", type=int, default=0, help="rasolve.py compile budget (0: time only)")
    ap.add_argument("--beam", type=int, default=6, help="translate.py beam")
    ap.add_argument("--steps", type=int, default=0, help="translate.py steps (0: until the time is up)")
    ap.add_argument("--grace", type=int, default=600, help="seconds past a budget before a run is killed")
    ap.add_argument("--triage", action="store_true",
                    help="skip a function whose first compile is beyond --max-bytes without solving it")
    ap.add_argument("--fast", action="store_true",
                    help="farm settings: --triage, 5000 compiles in the compiler search, 2 C steps with beam 2, at least -j 2")
    a = ap.parse_args()
    if a.fast:
        # work budgets, not time, so the yield does not depend on the machine's load
        a.triage, a.solve_seconds, a.solve_compiles = True, 300, 5000
        a.translate_seconds, a.steps, a.beam, a.grace = 600, 2, 2, 60
        a.j = max(a.j, 2)
    if a.items[0].endswith(".tsv"):
        work = [(ln.split("\t")[0], ln.split("\t")[1].strip()) for ln in open(a.items[0]) if ln.strip()]
    else:
        work = [(a.items[i + 1], a.items[i]) for i in range(0, len(a.items), 2)]
    os.makedirs(a.out, exist_ok=True)
    res_path = os.path.join(a.out, "results.tsv")
    done = {ln.split("\t")[0] for ln in open(res_path)} if os.path.exists(res_path) else set()
    with open(res_path, "a") as res:
        for func, path in work:
            if func in done:
                continue
            for row in solve_one(func, path, a.out, a):
                line = "\t".join(str(x) for x in row)
                print(line, flush=True)
                res.write(line + "\n")
                res.flush()


if __name__ == "__main__":
    main()
