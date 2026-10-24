#!/usr/bin/env python3
"""Does a spelling reproduce its harvested fixes? One-step check against tools/harvest.py cases.

  spelltest.py [SPELLING ...] [--cases FUNC ...] [--compile]
  spelltest.py --soundness N [SPELLING ...] [--seed S]

For each harvested case (unpinned), the draft is put back into today's source
(tools/replay.py), and each named spelling (default: all of tools/spellings.py)
is applied to the draft's function once. A case counts for a spelling when one
of its outputs prints the same as the matched function (pycparser form, names
of new locals aside) or compiles to the ROM's code. The point is a fast unit
test while writing a spelling: its own source cases must come out exact.

--soundness N is the other test a spelling needs: it must not change what the
code does. N matched, pin-free functions are sampled; every output of every
spelling on them is compiled and run against the ROM function with
tools/semcheck.py (run with build/venv/bin/python). A DIFFERENT verdict on a
matched function is a bug in the spelling (it changed the program), reported
with the edit and the first disagreement.
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


def soundness(sps, n, seed):
    import random
    import semcheck
    import build
    import isolate
    homes = replay.current_homes()
    funcs = sorted({(p, nm) for p, nm in homes.values()})
    random.Random(seed).shuffle(funcs)
    bad, checked = {}, {}
    taken = 0
    for path, name in funcs:
        if taken >= n:
            break
        text = open(os.path.join(ROOT, path)).read()
        if "MATCHING_CONCESSION" in text or 'asm("' in text:
            continue
        with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as wd:
            iso = os.path.join(wd, "iso.c")
            try:
                isolate.isolate(text, name, iso, False)
                label = replay.rom_label(open(iso).read(), name)
                d = fix.Draft(open(iso).read(), label, wd)
                sc = d.score()
                if not sc or sc[0] != 0:
                    continue
                pre = build.preprocess(os.path.relpath(iso, ROOT))
                oracle = semcheck.Oracle(label, 6, semcheck.returns_value(pre, name), arity=semcheck.arities(pre),
                                         kinds=semcheck.param_kinds(pre, name))
            except (Exception, SystemExit):
                continue
            taken += 1
            for sp in sps:
                try:
                    outs = list(sp(d.fn, d.helpers) if sp in spellings.HELPER_SPELLINGS else sp(d.fn))
                except Exception:
                    continue
                for desc, f2 in outs[:12]:
                    helpers = {}
                    if isinstance(f2, tuple):
                        f2, extra = f2
                        helpers = {e.decl.name: e for e in extra}
                    out = os.path.join(wd, "cand.c")
                    open(out, "w").write(d.materialize(f2, helpers))
                    try:
                        addr, data, syms, _ = semcheck.compile_unit(os.path.relpath(out, ROOT))
                    except (Exception, SystemExit):
                        continue
                    here = [a for a, nm in syms if nm in (name, label)]
                    if not here:
                        continue
                    cend = min([a for a, _ in syms if a > here[0]] + [addr + len(data)])
                    v, detail = oracle.check(data, addr, here[0], cend)
                    checked[sp.__name__] = checked.get(sp.__name__, 0) + 1
                    if v == "DIFFERENT":
                        bad.setdefault(sp.__name__, []).append((name, desc, detail))
                        print(f"  {sp.__name__} on {name}: {desc[:50]}: {detail[:140]}", flush=True)
        print(f"{taken}/{n} {name}", flush=True)
    for nm in sorted(checked):
        print(f"{nm:22} {checked[nm]:4} outputs checked, {len(bad.get(nm, [])):3} DIFFERENT")


def main():
    if "--soundness" in sys.argv:
        ap = argparse.ArgumentParser()
        ap.add_argument("names", nargs="*")
        ap.add_argument("--soundness", type=int)
        ap.add_argument("--seed", type=int, default=1)
        a = ap.parse_args()
        sps = [sp for sp in spellings.SPELLINGS + spellings.HELPER_SPELLINGS if not a.names or sp.__name__ in a.names]
        soundness(sps, a.soundness, a.seed)
        return
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
