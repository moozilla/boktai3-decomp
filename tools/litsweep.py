#!/usr/bin/env python3
"""Try every RAM global reference as a literal address, one or two at a time.

  litsweep.py FILE.c [FUNC] [--pairs] [-j N]

A reload register that differs from the ROM is usually fixed by adding or
removing one reload earlier in the function, and the most common way to add
one is to read a RAM global through its literal address (REGALLOC.md,
"Reload rotation"). The search space is small, so enumerate it instead of
sampling: each reference (and with --pairs, each pair) is replaced by
`(*(T *)0x0203B400)` and scored with the permuter's scorer. Prints the
variants that improve, best first; a score of 0 is an exact match. Write the
literal through a named macro in include/global.h before committing
(tools/check_ram_literals.py ties it to the symbol).

FILE.c holds one ROM function (tools/isolate.py). Uses the decomp-permuter
fork (tools/permute.py finds it).
"""
import argparse
import copy
import itertools
import multiprocessing
import os
import random
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import permute  # noqa: E402
from romlib import ROOT  # noqa: E402

sys.path.insert(0, permute.PERMUTER)
from src import ast_util, agbcc_passes  # noqa: E402
from src.randomizer import Region  # noqa: E402
from src.scorer import Scorer  # noqa: E402


class Pick(random.Random):
    """Makes a pass choose the K-th candidate."""

    def __init__(self, k):
        super().__init__(0)
        self.k = k

    def choice(self, seq):
        if self.k >= len(seq):
            raise IndexError
        return seq[self.k]


def sites(ast, func):
    fn, _ = ast_util.extract_fn(ast, func)
    n = 0
    while True:
        a2 = copy.deepcopy(ast)
        f2, _ = ast_util.extract_fn(a2, func)
        try:
            agbcc_passes.perm_literal_address(f2, a2, ast_util.compute_node_indices(f2),
                                              Region.unbounded(), Pick(n))
        except Exception:
            return n
        n += 1


def apply(ast, func, picks):
    a2 = copy.deepcopy(ast)
    for k in picks:  # later picks index the remaining sites
        f2, _ = ast_util.extract_fn(a2, func)
        agbcc_passes.perm_literal_address(f2, a2, ast_util.compute_node_indices(f2),
                                          Region.unbounded(), Pick(k))
    return a2


def score_variant(job):
    d, func, picks = job
    ast = ast_util.parse_c(open(os.path.join(d, "base.c")).read())
    try:
        a2 = apply(ast, func, picks)
    except Exception:
        return picks, None, None
    src = ast_util.to_c(a2)
    with tempfile.TemporaryDirectory(dir=d) as t:
        c, o = os.path.join(t, "v.c"), os.path.join(t, "v.o")
        open(c, "w").write(src)
        if subprocess.run([os.path.join(d, "compile.sh"), c, "-o", o], capture_output=True).returncode:
            return picks, None, None
        score, _ = Scorer(os.path.join(d, "target.o"), stack_differences=True, algorithm="difflib",
                          debug_mode=False, ign_branch_targets=False, objdump_command=None).score(o)
    return picks, score, src


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("func", nargs="?")
    ap.add_argument("--pairs", action="store_true")
    ap.add_argument("-j", type=int, default=max(1, (os.cpu_count() or 2) // 2),
                    help="worker processes (default: half the cores; all of them saturated a 16 GB machine)")
    a = ap.parse_args()
    out = subprocess.run([sys.executable, os.path.join(ROOT, "tools", "permute.py"), a.src]
                         + ([a.func] if a.func else []), capture_output=True, text=True, cwd=ROOT)
    if out.returncode:
        sys.exit(out.stderr)
    d = out.stdout.strip().splitlines()[-1]
    func = [ln.split('"')[1] for ln in open(os.path.join(d, "settings.toml")) if ln.startswith("func_name")][0]
    for name, value in __import__("toml").load(os.path.join(d, "settings.toml")).get("symbol_addresses", {}).items():
        agbcc_passes.SYMBOL_ADDRESSES[name] = int(value)
    ast = ast_util.parse_c(open(os.path.join(d, "base.c")).read())
    n = sites(ast, func)
    jobs = [(d, func, (k,)) for k in range(n)]
    if a.pairs:
        jobs += [(d, func, (i, j - 1)) for i, j in itertools.combinations(range(n), 2)]
    _, base, _ = score_variant((d, func, ()))
    print(f"{func}: base score {base}; {n} RAM global references; {len(jobs)} variants")
    with multiprocessing.get_context("fork").Pool(a.j) as pool:
        results = pool.map(score_variant, jobs)
    better = sorted((s, p, src) for p, s, src in results if s is not None and s < base)
    for s, p, src in better[:10]:
        print(f"  score {s}: references {list(p)}")
    if better:
        best = os.path.join(d, "litsweep_best.c")
        open(best, "w").write(better[0][2])
        print(f"best: {best}")


if __name__ == "__main__":
    main()
