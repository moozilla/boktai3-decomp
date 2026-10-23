#!/usr/bin/env python3
"""Flip one compiler decision at a time and see which one gives the ROM's code.

  flip.py FILE.c [FUNC] [--nonmatching]

Compiles the draft once per flip with the instrumented agbcc and reports how
the difference from the ROM changes. A flip that makes the code match, or
closes most of the difference, names the decision your C gets wrong; the
remaining work is to find the C that makes agbcc decide that way by itself
(the flips are never used for the real build). Each flip prints where the
decision lives in the compiler source, to read when the condition is unclear.

A flip is either a -f flag for the whole function (the pass or path choice it
controls) or an instrumented knob (tools/agbcc-instr.sh). About twenty
compiles; a few seconds.
"""
import argparse
import json
import os
import stat
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from romlib import ROOT  # noqa: E402

INSTR = os.path.join(ROOT, "build", "agbcc-instr", "gcc", "agbcc")
WRAP = os.path.join(ROOT, "build", "flip", "agbcc")

# (flip, what it means when it helps, where to read)
FLIPS = [
    ("-fno-cse-skip-blocks",
     "cse carried what it knew across an `if (c) x;` it skipped; the ROM's cse did not, so the original's "
     "branch-around label had a second use or a label inside the skipped block, or the code after it started "
     "a new block",
     "cse.c cse_end_of_basic_block (\"Detect a branch around a block of code\")"),
    ("-fno-cse-follow-jumps",
     "cse followed a jump into its target block; the ROM's did not (the target label has other uses)",
     "cse.c cse_end_of_basic_block (follow_jumps)"),
    ("-fno-rerun-cse-after-loop", "the second cse pass (cse2) shared or folded a value the ROM recomputes",
     "cse.c, run from toplev.c after loop"),
    ("-fno-gcse", "gcse (PRE, constant/copy propagation across blocks) moved or reused a value",
     "gcse.c"),
    ("-fno-strength-reduce",
     "loop.c strength reduction made a pointer or counter (or a final-value insn) the ROM doesn't have; "
     "write the loop with the induction variable the ROM uses",
     "loop.c strength_reduce, unroll.c final_biv_value"),
    ("-fno-rerun-loop-opt", "the second loop pass changed the loop", "loop.c"),
    ("-fno-expensive-optimizations", "an 'expensive' cse/combine step changed it", "cse.c, combine.c"),
    ("-fno-force-mem", "a memory operand was copied into a register before use (or the reverse)",
     "expr.c force_operand / force_reg"),
    ("-fforce-addr", "an address was (or was not) computed into a register first", "explow.c memory_address"),
    ("-fno-regmove", "regmove tied or copied operands differently", "regmove.c"),
    ("-fno-thread-jumps", "jump threading rewrote a branch", "jump.c thread_jumps"),
    ("-fno-defer-pop", "argument pops after calls", "calls.c"),
    ("-fno-caller-saves", "a value lived in a call-clobbered register across a call", "caller-save.c"),
    ("-fno-function-cse", "a function address was loaded into a register once", "calls.c"),
    ("-fno-peephole", "a peephole changed the code", "config/arm/thumb.md"),
    ("-fno-inline-functions", "an inline expansion choice", "integrate.c"),
]


def wrapper():
    os.makedirs(os.path.dirname(WRAP), exist_ok=True)
    with open(WRAP, "w") as f:
        f.write(f'#!/bin/sh\nexec "{INSTR}" "$@" $AGBCC_EXTRA\n')
    os.chmod(WRAP, os.stat(WRAP).st_mode | stat.S_IEXEC)


def score(src, func, nonmatching, extra="", env=None):
    e = dict(os.environ, AGBCC=WRAP, AGBCC_EXTRA=extra, **(env or {}))
    cmd = [sys.executable, os.path.join(ROOT, "tools", "check.py"), src, "--json", "--quiet"]
    r = subprocess.run(cmd + ["--nonmatching"] * nonmatching, cwd=ROOT, env=e, capture_output=True, text=True)
    try:
        res = json.loads([ln for ln in r.stdout.splitlines() if ln.startswith("{")][-1])
    except (IndexError, ValueError):
        return None
    ad = res.get("anchordiff") or {"structural": 0, "total": 0}
    return ad.get("structural", 0), ad.get("total", 0), res["different_byte_count"]


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("src")
    ap.add_argument("func", nargs="?")
    ap.add_argument("--nonmatching", action="store_true")
    a = ap.parse_args()
    if not os.path.exists(INSTR):
        subprocess.run(["sh", os.path.join(ROOT, "tools", "agbcc-instr.sh")], check=True)
    wrapper()
    src = os.path.abspath(a.src)
    base = score(src, a.func, a.nonmatching)
    if base is None:
        sys.exit("does not compile")
    print(f"as written: {base[1]} differing insns ({base[0]} in shape), {base[2]} bytes")
    if base[2] == 0:
        return 0
    rows = []
    for flag, meaning, where in FLIPS:
        s = score(src, a.func, a.nonmatching, flag)
        if s is not None and s[1] < base[1]:
            rows.append((s, flag, meaning, where))
    if not rows:
        print("no single flip helps: the difference is in the C itself (types, expression shape, statement "
              "order) or in register allocation (tools/rasolve.py)")
        return 1
    rows.sort(key=lambda r: (r[0][1], r[0][2]))
    for s, flag, meaning, where in rows:
        tag = "MATCH" if s[2] == 0 else f"{s[1]} insns ({s[0]} in shape), {s[2]} bytes"
        print(f"\n{flag}: {tag}")
        print(f"   means: {meaning}")
        print(f"   read:  build/agbcc-instr/gcc/{where}")
    return 1


if __name__ == "__main__":
    sys.exit(main())
