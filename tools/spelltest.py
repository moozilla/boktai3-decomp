#!/usr/bin/env python3
"""Does a spelling reproduce its harvested fixes? One-step check against tools/harvest.py cases.

  spelltest.py [SPELLING ...] [--cases FUNC ...]

For each harvested case (unpinned), the draft is put back into today's source
(tools/replay.py), and each named spelling (default: all of tools/spellings.py)
is applied to the draft's function once. A case counts for a spelling when one
of its outputs prints the same as the matched function (pycparser form, names
of new locals aside) or compiles to the ROM's code. The point is a fast unit
test while writing a spelling: its own source cases must come out exact.
"""
import argparse
import os
import re
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fix  # noqa: E402  (puts the permuter on sys.path)
import harvest  # noqa: E402
import replay  # noqa: E402
import spellings  # noqa: E402
from romlib import ROOT  # noqa: E402
from src import ast_util  # noqa: E402


def norm(t):
    t = re.sub(r"\b(q|v|w|off|k|t|j)\d+\b", "_", t)
    return re.sub(r"\s+", " ", t).strip()


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("names", nargs="*")
    ap.add_argument("--cases", nargs="*")
    ap.add_argument("--compile", action="store_true", help="also score each output against the ROM")
    a = ap.parse_args()
    sps = [sp for sp in spellings.SPELLINGS + spellings.HELPER_SPELLINGS if not a.names or sp.__name__ in a.names]
    cases = [c for c in harvest.load() if not c["pinned"] and (not a.cases or c["key"] in a.cases or c["name"] in a.cases)]
    homes = replay.current_homes()
    hits = {sp.__name__: [] for sp in sps}
    applied = {sp.__name__: 0 for sp in sps}
    for c in cases:
        built = replay.build_case(c, homes)
        if isinstance(built, str):
            continue
        iso, label, name = built
        with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as wd:
            try:
                d = fix.Draft(open(iso).read(), label, wd)
                final_text = open(os.path.join(ROOT, homes.get(c["key"], homes.get(c["name"]))[0])).read()
                import isolate
                fin_iso = os.path.join(wd, "final.c")
                isolate.isolate(final_text, name, fin_iso, False)
                f = fix.Draft(open(fin_iso).read(), label, wd)
            except (Exception, SystemExit):
                continue
            want = norm(ast_util.to_c(f.fn))
            for sp in sps:
                try:
                    outs = list(sp(d.fn, d.helpers) if sp in spellings.HELPER_SPELLINGS else sp(d.fn))
                except Exception as e:
                    print(f"  {sp.__name__} crashed on {c['name']}: {type(e).__name__}: {e}")
                    continue
                applied[sp.__name__] += bool(outs)
                for desc, f2 in outs:
                    if isinstance(f2, tuple):
                        f2 = f2[0]
                    ok = norm(ast_util.to_c(f2)) == want
                    if not ok and a.compile:
                        sc = d.score(f2)
                        ok = sc is not None and sc[0] == 0
                    if ok:
                        hits[sp.__name__].append(c["name"])
                        print(f"  {sp.__name__}: {c['name']} exact ({desc})", flush=True)
                        break
    for n in hits:
        print(f"{n:22} applies to {applied[n]:3} drafts, reproduces {len(hits[n])}: {' '.join(hits[n])[:150]}")


if __name__ == "__main__":
    main()
