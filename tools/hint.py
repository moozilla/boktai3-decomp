#!/usr/bin/env python3
"""Ask about one difference between your draft and the ROM: the decision, and the agbcc source behind it.

  hint.py FILE.c [FUNC] [--line N | --insn U | --at ADDR] [--flips] [--nonmatching]
  hint.py --about WORDS                  a decision's rule and compiler source, no draft needed
  hint.py FILE.c [FUNC] --causes [--compiles N] [--show K]
  hint.py --eval [--functions N] [--mutants M] [--compiles N] [--seed S]

Point it at the spot you are stuck on: a source line, an RTL insn number (from
tools/step.py), or an address (offset in the function, or ROM address). It
prints only the differences there: the ROM's and your instructions, the
decision events that produced yours (tools/step.py), the rule for that kind of
decision, and the lines of agbcc's source that make it (tools/agbccsrc.py), so
you read the compiler's condition rather than a summary of it. With no spot it
takes the first divergence. --flips also compiles the draft with each single
compiler decision flipped (tools/flip.py, about 10 s) and reports the ones that
close the difference.

--about takes a topic or free words ("cse skip", "biv zero", "move2add").

--causes is the causebook mode: for each differing region, the edit types the
causebook (tools/causebook.py) has seen produce that difference in matched
code; it then applies the highest-ranked edits to your draft (up to
--compiles, through the agbcc fork server) and lists those that remove a
region without adding one. An exact match is written to build/hint/FUNC.c.

--eval measures the causebook's ordering without any model: it takes functions
that the causebook was not built from, breaks each with one random edit, and
counts the compiles until the causebook-ordered and a randomly ordered list of
single edits find one that matches again.
"""
import argparse
import collections
import json
import os
import random
import re
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import causebook as atlas  # noqa: E402
from romlib import ROOT  # noqa: E402


def ordered_edits(at, sig, cands, rng=None):
    """CANDS ({text: (type, desc, fn)}) in the causebook's order for SIG (random order with RNG)."""
    items = list(cands.items())
    if rng is not None:
        rng.shuffle(items)
        return items
    rank = {e: i for i, e in enumerate(at.rank(sig))}
    random.Random(0).shuffle(items)  # ties: no preference for generation order
    return sorted(items, key=lambda kv: rank.get(kv[1][0], len(rank)))


def describe(reg, rom, at):
    lv, c, nf = at.explain(reg)
    kind, _, rest = reg["k0"].partition("|")
    out = [f"region at ROM insn {reg['at']} ({kind}): {rest.replace('|', '  ->  ')[:110]}"]
    if c:
        tot = sum(c.values())
        causes = ", ".join(f"{e} {v / tot:.0%}" for e, v in c.most_common(3))
        out.append(f"  seen in {nf} matched functions ({lv}); edits that cause it: {causes}")
    else:
        out.append("  not in the causebook")
    return out


def cmd_hint(a):
    import fixsearch
    import isolate
    from src import ast_util
    rel = isolate.ensure(a.file, a.func) if not a.nonmatching else a.file
    rel, ast, fn, prefix = fixsearch.prepare(rel, a.func)
    rom = atlas.init_compiler(rel, a.func)
    base_text = ast_util.to_c(fn)
    mine = atlas.texts_of(prefix + base_text)
    if mine is None:
        print("does not compile")
        return 2
    if mine == rom:
        print("MATCH")
        return 0
    at = atlas.Causebook(atlas.load())
    sig = atlas.signature(rom, mine)
    print(f"{a.func}: {len(sig)} differing regions (shapes: registers R, constants K)\n")
    for reg in sig:
        print("\n".join(describe(reg, rom, at)))
    cands = atlas.edits(fn, ast, a.func, seeds=3)
    order = ordered_edits(at, sig, cands)[:a.compiles]
    before = {(s["k0"]) for s in sig}
    good = []
    for t, (etype, desc, _) in order:
        m2 = atlas.texts_of(prefix + t)
        if m2 is None:
            continue
        if m2 == rom:
            os.makedirs(os.path.join(ROOT, "build", "hint"), exist_ok=True)
            path = os.path.join(ROOT, "build", "hint", a.func + ".c")
            open(path, "w").write(prefix + t)
            print(f"\nMATCH with {desc}:")
            print("\n".join("    " + ln for ln in atlas.cdiff(base_text, t)))
            print(f"written to {os.path.relpath(path, ROOT)}")
            return 0
        s2 = atlas.signature(rom, m2)
        after = {s["k0"] for s in s2}
        if len(s2) < len(sig) and after <= before:
            good.append((len(s2), desc, t))
    good.sort(key=lambda x: x[0])
    print(f"\ntested {len(order)} edits; {len(good)} remove a region and add none")
    for n, desc, t in good[:a.show]:
        print(f"  -> {n} regions left: {desc}")
        print("\n".join("      " + ln for ln in atlas.cdiff(base_text, t)))
    return 1


# --- ask mode ------------------------------------------------------------------------------

def topics_for(st, e):
    """agbccsrc topic keys for an explained difference, in pipeline order."""
    import step
    import why
    ts = []
    if e["kind"] == "order":
        ts.append("order")
    if e["note"] and "two-operand" in e["note"]:
        ts += ["cse-swap", "regmove-copy"]
    for u, p, k, text in sorted(e["evs"], key=lambda ev: step.PASS_RANK.get(ev[1], 99)):
        if k == "swap":
            ts.append("cse-swap" if p in ("cse", "cse2") else "combine")
        elif k == "deleted":
            ts.append({"cse": "cse-reuse", "cse2": "cse-reuse", "gcse": "gcse", "loop": "loop-invariant"}.get(p))
            if p == "cse":
                ts.append("cse-skip")
        elif k == "tie":
            ts.append("regmove-tie")
        elif k == "copy":
            ts.append("regmove-copy")
        elif k == "born":
            if p == "loop":
                tr = why.trail(st.dumps, st.func, u)
                zero = tr and "(const_int 0)" in tr[0][1] and tr[0][1].startswith("(set (reg")
                ts.append("biv-final" if zero else "loop-invariant")
            else:
                ts.append({"gcse": "gcse", "combine": "combine", "regmove": "regmove-copy",
                           "reload": "reload"}.get(p))
    if any(re.match(r"(adds|subs) r\d+, #", t) for _, t in e["orig"]) and \
            any(t.startswith("movs") for _, t in e["cand"]):
        ts.append("move2add")
    if not e["evs"] and e["kind"] != "order":
        ts += ["local-alloc", "global-alloc"]
    seen = []
    for t in ts:
        if t and t not in seen:
            seen.append(t)
    return seen


def select(st, a):
    es = st.entries
    if a.line:
        return [e for e in es if a.line in e["lines"]]
    if a.insn:
        return [e for e in es if a.insn in e["uids"] or any(ev[0] == a.insn for ev in e["evs"])]
    if a.at:
        x = int(a.at, 16)
        if x >= 0x08000000:
            x -= st.an.addr
        hit = [e for e in es if any(o == x for o, _ in e["cand"] + e["orig"])]
        if hit:
            return hit
        near = sorted(es, key=lambda e: min([abs(o - x) for o, _ in e["cand"] + e["orig"]] or [1 << 30]))
        return near[:1]
    return es[:1]


def cmd_ask(a):
    import step
    import agbccsrc
    src = os.path.abspath(a.file)
    st = step.analyze(src, a.func, a.nonmatching)
    if st.matched:
        print(f"{st.func}: matches the ROM")
        return 0
    picked = select(st, a)
    print(f"{st.func}: {len(st.entries)} differences")
    if not picked:
        where = sorted({ln for e in st.entries for ln in e["lines"]})
        print(f"nothing differs there; differences are on lines {', '.join(map(str, where)) or '?'}")
        return 1
    for n, e in enumerate(picked[:3]):
        k = st.entries.index(e)
        print()
        print("\n".join(step.render(st, e, f"difference {k + 1} of {len(st.entries)}", trails=1)))
        for t in topics_for(st, e)[:2 if a.brief else 3]:
            print("\n".join(agbccsrc.show(t, source=not a.brief)))
    if len(picked) > 3:
        print(f"\n({len(picked) - 3} more differences there)")
    if a.flips:
        import flip
        flip.wrapper()
        base = flip.score(src, st.func, a.nonmatching)
        print(f"\nflips (as written: {base[1]} differing insns, {base[2]} bytes):")
        found = False
        for flag, meaning, where in flip.FLIPS:
            r = flip.score(src, st.func, a.nonmatching, flag)
            if r is not None and base and r[1] < base[1]:
                found = True
                print(f"  {flag}: {'MATCH' if r[2] == 0 else f'{r[1]} insns, {r[2]} bytes'}; {meaning}")
        if not found:
            print("  none helps: the C itself or register allocation decides it")
    return 1


# --- evaluation --------------------------------------------------------------------------

def _eval_one(job):
    unit, func, n_mut, max_compiles, seed, exclude = job
    import fixsearch
    import isolate
    from src import ast_util
    try:
        rel = isolate.ensure(unit, func)
        rel, ast, fn, prefix = fixsearch.prepare(rel, func)
        rom = atlas.init_compiler(rel, func)
        if atlas.texts_of(prefix + ast_util.to_c(fn)) != rom:
            return func, []
        global _AT
        rng = random.Random(seed + hash(func) % 1000)
        muts = list(atlas.edits(fn, ast, func, seeds=3).items())
        rng.shuffle(muts)
        out = []
        for t, (etype, desc, f2) in muts:
            if len(out) >= n_mut:
                break
            mine = atlas.texts_of(prefix + t)
            if mine is None or mine == rom:
                continue
            sig = atlas.signature(rom, mine)
            ast.ext[-1] = f2
            cands = atlas.edits(f2, ast, func, seeds=3)
            ast.ext[-1] = fn
            res = {"edit": etype, "neighbours": len(cands)}
            for arm, order in (("atlas", ordered_edits(_AT, sig, cands)),
                               ("random", ordered_edits(_AT, sig, cands, rng=random.Random(seed)))):
                res[arm] = None
                for k, (t2, _) in enumerate(order[:max_compiles]):
                    if atlas.texts_of(prefix + t2) == rom:
                        res[arm] = k + 1
                        break
            out.append(res)
        return func, out
    except Exception as e:
        return func, [{"err": str(e)[:80]}]


_AT = None


def cmd_eval(a):
    global _AT
    rows = list(atlas.load())
    mined = {r["func"] for r in rows}
    import check
    _, _, sizes = check.inventory()
    pool_funcs = [x for x in atlas.clean_functions(12, 400) if x[1] not in mined]
    rng = random.Random(a.seed)
    rng.shuffle(pool_funcs)
    test = pool_funcs[:a.functions]
    _AT = atlas.Causebook(rows)
    import multiprocessing
    jobs = [(u, f, a.mutants, a.compiles, a.seed, None) for u, f, _ in test]
    results = []
    t0 = time.time()
    with multiprocessing.get_context("fork").Pool(a.jobs, maxtasksperchild=1) as pool:
        for k, (func, out) in enumerate(pool.imap_unordered(_eval_one, jobs)):
            results += [dict(r, func=func) for r in out if "err" not in r]
            if k % 10 == 0:
                print(f"{k + 1}/{len(test)} functions, {len(results)} mutants, {time.time() - t0:.0f} s", flush=True)
    os.makedirs(atlas.OUT, exist_ok=True)
    json.dump(results, open(os.path.join(atlas.OUT, "eval.json"), "w"))
    n = len(results)
    print(f"\n{n} broken mutants of {len(test)} functions the causebook has not seen; "
          f"mean {sum(r['neighbours'] for r in results) / max(n, 1):.0f} single edits each")
    for arm in ("atlas", "random"):
        found = [r[arm] for r in results if r[arm]]
        for k in (1, 3, 10, a.compiles):
            print(f"  {arm:6} fixed within {k:3} compiles: {sum(1 for x in found if x <= k) / max(n, 1):5.0%}")
        if found:
            print(f"  {arm:6} median compiles when fixed: {sorted(found)[len(found) // 2]}")
    by = collections.defaultdict(lambda: [0, 0, 0])
    for r in results:
        by[r["edit"]][0] += 1
        by[r["edit"]][1] += bool(r["atlas"] and r["atlas"] <= 3)
        by[r["edit"]][2] += bool(r["random"] and r["random"] <= 3)
    print("\nper breaking edit: mutants, fixed within 3 (atlas / random)")
    for e, (m, x, y) in sorted(by.items(), key=lambda kv: -kv[1][0]):
        print(f"  {e:28} {m:5}  {x / m:5.0%} / {y / m:5.0%}")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("file", nargs="?")
    ap.add_argument("func", nargs="?")
    ap.add_argument("--nonmatching", action="store_true")
    ap.add_argument("--compiles", type=int, default=60)
    ap.add_argument("--show", type=int, default=5)
    ap.add_argument("--eval", action="store_true")
    ap.add_argument("--causes", action="store_true", help="the causebook mode")
    ap.add_argument("--about", nargs="+", metavar="WORDS")
    ap.add_argument("--line", type=int)
    ap.add_argument("--insn", type=int)
    ap.add_argument("--at")
    ap.add_argument("--flips", action="store_true")
    ap.add_argument("--brief", action="store_true", help="rules without the compiler source")
    ap.add_argument("--functions", type=int, default=60)
    ap.add_argument("--mutants", type=int, default=4)
    ap.add_argument("--seed", type=int, default=7)
    ap.add_argument("-j", "--jobs", type=int, default=os.cpu_count())
    a = ap.parse_args()
    if a.about:
        import agbccsrc
        sys.argv = ["agbccsrc"] + a.about
        return agbccsrc.main()
    if not a.eval and not a.causes:
        return cmd_ask(a)
    os.chdir(ROOT)
    import permute
    sys.path.insert(0, permute.PERMUTER)
    if a.eval:
        return cmd_eval(a)
    if a.func is None:
        a.func = re.findall(r"^\w[\w \*]*?\b(\w+)\s*\([^;]*?\)\s*\{", open(a.file).read(), re.M)[-1]
    return cmd_hint(a)


if __name__ == "__main__":
    sys.exit(main())
