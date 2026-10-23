#!/usr/bin/env python3
"""The causebook: what each kind of C edit does to agbcc's output, learned from matched code.

  causebook.py mine [--sample N] [--seed S] [-j N] [--seeds K] [--max-edits M] [--min-insns A] [--max-insns B]
                                 apply every known single edit to matched, pin-free functions
                                 and record the assembly difference each one causes
                                 -> build/causebook/pairs.jsonl (appends; finished functions are skipped)
  causebook.py stats [--top N]       the largest difference shapes and the edits that cause them
  causebook.py heldout [--test-frac F] [--seed S]
                                 transfer check: rank edits for mutants of held-out functions
                                 with a causebook built from the other functions

A matched function's C is a known answer. Any edit to it (the permuter's
passes, tools/rewrites.py's agbcc-specific rewrites) gives a mutant whose
code differs from the ROM, and the difference is the *effect* of a known
*cause*. Recorded over thousands of functions, the table runs backwards: an
agent's draft differs from the ROM by some regions, and the mutants that
differed from the ROM by the same regions say which edits produce them. The
fix is the edit that undoes that difference (most edit families are their own
inverse: reorder, swap, commute, loop form, declaration order...).

A difference is split into regions on the register- and constant-free
instruction shapes (tools/idioms.py), as tools/fixsearch.py scores them;
inside equal-shape stretches, runs whose registers or constants differ are
their own regions. Each region gets three keys, from exact to coarse:

  k0  shapes on both sides             ("replace|movs R,#K;adds R,R,#K|movs R,#K")
  k1  mnemonics on both sides          ("replace|movs;adds|movs")
  k2  kind and sizes                   ("replace|2|1", "regs|3", "const|1")

Lookups back off from k0 to k2. See tools/hint.py for the agent-facing use.
"""
import argparse
import collections
import difflib
import json
import multiprocessing
import os
import random
import re
import signal
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from romlib import ROOT  # noqa: E402

OUT = os.path.join(ROOT, "build", "causebook")
PAIRS = os.path.join(OUT, "pairs.jsonl")
REG = re.compile(r"\b(r\d+|sb|sl|fp|ip|sp|lr|pc)\b")
IMM = re.compile(r"#-?(0x[0-9a-f]+|\d+)")
CONSEQUENTIAL = {"ldr R,=K", "bl F"}


# --- regions -------------------------------------------------------------------------

def mnem(shape):
    return shape.split(" ", 1)[0]


def regions(rom, mine):
    """Regions where MINE's instructions differ from ROM's: [(kind, i1, i2, j1, j2)],
    both lists of capstone texts with literal pools stripped."""
    import idioms
    a = [idioms.shape(t) for t in rom]
    b = [idioms.shape(t) for t in mine]
    out = []
    sm = difflib.SequenceMatcher(None, a, b, autojunk=False)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag != "equal":
            out.append((tag, i1, i2, j1, j2))
            continue
        # branch offsets, pool offsets and push/pop lists follow from the rest
        same = [rom[i1 + x] == mine[j1 + x] or a[i1 + x] in CONSEQUENTIAL or a[i1 + x].startswith(("push", "pop"))
                or mnem(a[i1 + x]).startswith("b") and a[i1 + x].endswith(" L")
                for x in range(i2 - i1)]
        k = 0
        while k < i2 - i1:
            if same[k]:
                k += 1
                continue
            s = k
            while k < i2 - i1 and not same[k]:
                k += 1
            same_imm = all(IMM.findall(rom[i1 + x]) == IMM.findall(mine[j1 + x]) for x in range(s, k))
            out.append(("regs" if same_imm else "const", i1 + s, i1 + k, j1 + s, j1 + k))
    return out


def keys(kind, rom, mine, i1, i2, j1, j2):
    import idioms
    ra = [idioms.shape(t) for t in rom[i1:i2]]
    mb = [idioms.shape(t) for t in mine[j1:j2]]
    if kind in ("regs", "const"):
        k0 = kind + "|" + ";".join(ra)
        k1 = kind + "|" + ";".join(map(mnem, ra))
        k2 = f"{kind}|{min(i2 - i1, 4)}"
    else:
        k0 = f"{kind}|{';'.join(ra)}|{';'.join(mb)}"
        k1 = f"{kind}|{';'.join(map(mnem, ra))}|{';'.join(map(mnem, mb))}"
        k2 = f"{kind}|{min(i2 - i1, 4)}|{min(j2 - j1, 4)}"
    return k0, k1, k2


def signature(rom, mine):
    """[{k0,k1,k2,at}] for the regions where MINE differs from ROM. A deleted run whose
    shapes are inserted elsewhere is one `move` region (statement order)."""
    import idioms
    regs = regions(rom, mine)
    out, used = [], set()
    for a_, (kind, i1, i2, j1, j2) in enumerate(regs):
        if a_ in used:
            continue
        if kind == "delete":
            ra = [idioms.shape(t) for t in rom[i1:i2]]
            for b_, (k2_, x1, x2, y1, y2) in enumerate(regs):
                if b_ not in used and b_ != a_ and k2_ == "insert" and [idioms.shape(t) for t in mine[y1:y2]] == ra:
                    used |= {a_, b_}
                    sh = ";".join(ra)
                    out.append(dict(k0=f"move|{sh}", k1="move|" + ";".join(map(mnem, ra)),
                                    k2=f"move|{min(len(ra), 4)}", at=i1))
                    break
            if a_ in used:
                continue
        if kind == "insert":
            ma = [idioms.shape(t) for t in mine[j1:j2]]
            hit = next((b_ for b_, (k2_, x1, x2, y1, y2) in enumerate(regs) if b_ not in used and k2_ == "delete"
                        and [idioms.shape(t) for t in rom[x1:x2]] == ma), None)
            if hit is not None:
                used |= {a_, hit}
                out.append(dict(k0="move|" + ";".join(ma), k1="move|" + ";".join(map(mnem, ma)),
                                k2=f"move|{min(len(ma), 4)}", at=regs[hit][1]))
                continue
        out.append(dict(zip(("k0", "k1", "k2"), keys(kind, rom, mine, i1, i2, j1, j2)), at=i1))
    return out


# --- compiling a candidate to instruction texts ----------------------------------------

_cc = None
_rom_texts = None


def init_compiler(rel, func):
    global _cc, _rom_texts
    import idioms
    import rasolve
    from romlib import off
    _cc = rasolve.Compiler(rel, func, lines=False)
    _cc.rom_code = _cc.rom[off(_cc.address):off(_cc.address) + _cc.size]
    _rom_texts = [t for _, t in idioms.disasm_function(_cc.rom, _cc.address, _cc.size)]
    return _rom_texts


def texts_of(source):
    """Instruction texts of FUNC compiled from SOURCE (pools stripped), or None."""
    import idioms
    import regdiag
    try:
        asm = _cc._asm(source, {})
    except Exception:
        return None
    if asm is None:
        return None
    obj = _cc.resolver.assemble(asm, _cc.work)
    if obj is None:
        return None
    mine, missing = _cc.resolver.bytes_of(obj, _cc.func)
    if mine is None or missing:
        return None
    dis = regdiag.strip_pools(idioms.disasm(mine, 0), len(mine), mine, _cc.address)
    return [t for _, t in dis]


# --- edits ---------------------------------------------------------------------------

def edits(fn, ast, func, seeds):
    """{function text: (edit type, description, FuncDef)} one edit away from FN."""
    import copy
    import fixsearch
    import rewrites
    from src import ast_util
    from src import randomizer as R
    out = {}
    base = ast_util.to_c(fn)
    for rw in rewrites.REWRITES:
        try:
            for desc, f2 in rw(fn):
                t = ast_util.to_c(f2)
                if t != base and t not in out:
                    out[t] = (rw.__name__, desc, f2)
        except Exception:
            continue
    by_name = {p.__name__: p for p in R.RANDOMIZATION_PASSES}
    for name in fixsearch.PERMUTER_PASSES:
        p = by_name.get(name)
        if p is None:
            continue
        for seed in range(seeds):
            f2 = copy.deepcopy(fn)
            ast.ext[-1] = f2
            try:
                indices = ast_util.compute_node_indices(f2)
                p(f2, ast, indices, R.Region.unbounded(), R.Random(seed * 7919 + len(name)))
                ast_util.compute_node_indices(f2)
                t = ast_util.to_c(f2)
            except Exception:
                continue
            if t != base and t not in out:
                out[t] = (name, name, f2)
    ast.ext[-1] = fn
    return out


def cdiff(a, b, limit=8):
    lines = [ln for ln in difflib.unified_diff(a.splitlines(), b.splitlines(), lineterm="", n=0)
             if ln[:1] in "+-" and not ln.startswith(("+++", "---"))]
    return lines[:limit]


def _timeout(*_):
    raise TimeoutError


def mine_one(job):
    """All single-edit mutants of one matched function -> list of records."""
    unit, func, seeds, max_edits = job
    signal.signal(signal.SIGALRM, _timeout)
    signal.alarm(240)
    try:
        import fixsearch
        import isolate
        from src import ast_util
        rel = isolate.ensure(unit, func)
        rel, ast, fn, prefix = fixsearch.prepare(rel, func)
        rom = init_compiler(rel, func)
        base_text = ast_util.to_c(fn)
        base = texts_of(prefix + base_text)
        if base is None or base != rom:
            return func, unit, len(rom), [], "base does not match"
        cands = list(edits(fn, ast, func, seeds).items())
        random.Random(func).shuffle(cands)
        recs = []
        for t, (etype, desc, _) in cands[:max_edits]:
            mine = texts_of(prefix + t)
            if mine is None:
                continue
            raw = [[k, i1, i2, j1, j2, mine[j1:j2]] for k, i1, i2, j1, j2 in regions(rom, mine)]
            recs.append(dict(edit=etype, desc=desc[:80], sig=signature(rom, mine), raw=raw, c=cdiff(base_text, t)))
        return func, unit, len(rom), recs, None
    except TimeoutError:
        return func, unit, 0, [], "timeout"
    except Exception as e:
        return func, unit, 0, [], f"{type(e).__name__}: {str(e)[:80]}"
    finally:
        signal.alarm(0)
        if _cc is not None:
            for s in getattr(_cc, "servers", {}).values():
                try:
                    s.close()
                except Exception:
                    pass


def clean_functions(min_insns, max_insns):
    """[(unit, func, size)] for matched functions with no register pins or asm barriers."""
    import check
    import pinfarm
    _, _, sizes = check.inventory()
    out = []
    for d, _, fs in os.walk(os.path.join(ROOT, "src")):
        for f in sorted(fs):
            if not f.endswith(".c"):
                continue
            unit = os.path.relpath(os.path.join(d, f), ROOT)
            text = open(os.path.join(ROOT, unit)).read()
            for name, item in pinfarm.definitions(text):
                if pinfarm.hinted(item):
                    continue
                size = sizes.get(name)
                if size and min_insns * 2 <= size <= max_insns * 2:
                    out.append((unit, name, size))
    return sorted(out)


def cmd_mine(a):
    os.makedirs(OUT, exist_ok=True)
    done = set()
    if os.path.exists(PAIRS):
        for ln in open(PAIRS):
            try:
                done.add(json.loads(ln)["func"])
            except Exception:
                pass
    funcs = clean_functions(a.min_insns, a.max_insns)
    random.Random(a.seed).shuffle(funcs)
    funcs = [x for x in funcs[:a.sample] if x[1] not in done]
    print(f"{len(funcs)} functions to mine ({len(done)} already done)", flush=True)
    t0 = time.time()
    n_rec = 0
    with multiprocessing.get_context("fork").Pool(a.jobs, maxtasksperchild=1) as pool, open(PAIRS, "a") as out:
        jobs = [(unit, func, a.seeds, a.max_edits) for unit, func, _ in funcs]
        for k, (func, unit, n, recs, err) in enumerate(pool.imap_unordered(mine_one, jobs)):
            out.write(json.dumps(dict(func=func, unit=unit, n=n, err=err, recs=recs)) + "\n")
            out.flush()
            n_rec += len(recs)
            if k % 20 == 0:
                print(f"{k + 1}/{len(funcs)} functions, {n_rec} mutants, {time.time() - t0:.0f} s", flush=True)
    print(f"done: {n_rec} mutants in {time.time() - t0:.0f} s -> {os.path.relpath(PAIRS, ROOT)}")


# --- the table -----------------------------------------------------------------------

def load(path=PAIRS):
    for ln in open(path):
        try:
            row = json.loads(ln)
        except Exception:
            continue
        if not row.get("err"):
            yield row


class Causebook:
    """key -> Counter(edit type -> weight), with each mutant's weight split over its regions,
    so a mutant whose only change is one region speaks loudest about that region."""

    LEVELS = ("k0", "k1", "k2")

    def __init__(self, rows, exclude=frozenset()):
        self.table = {lv: collections.defaultdict(collections.Counter) for lv in self.LEVELS}
        self.funcs = {lv: collections.defaultdict(set) for lv in self.LEVELS}
        self.prior = collections.Counter()
        self.noop = collections.Counter()
        self.tried = collections.Counter()
        for row in rows:
            if row["func"] in exclude:
                continue
            for r in row["recs"]:
                self.tried[r["edit"]] += 1
                if not r["sig"]:
                    self.noop[r["edit"]] += 1
                    continue
                w = 1.0 / len(r["sig"])
                self.prior[r["edit"]] += 1
                for reg in r["sig"]:
                    for lv in self.LEVELS:
                        self.table[lv][reg[lv]][r["edit"]] += w
                        self.funcs[lv][reg[lv]].add(row["func"])

    def explain(self, reg, min_support=2):
        """(level, Counter of edit types, number of functions) for one region, backing off."""
        for lv in self.LEVELS:
            c = self.table[lv].get(reg[lv])
            if c and len(self.funcs[lv][reg[lv]]) >= min_support:
                return lv, c, len(self.funcs[lv][reg[lv]])
        return None, collections.Counter(), 0

    def rank(self, sig, min_support=2):
        """Edit types ranked for a whole difference: per-region evidence (normalized),
        summed, with the overall prior as the tie-breaker."""
        score = collections.Counter()
        for reg in sig:
            lv, c, _ = self.explain(reg, min_support)
            tot = sum(c.values())
            weight = {"k0": 1.0, "k1": 0.6, "k2": 0.3}.get(lv, 0)
            for e, v in c.items():
                score[e] += weight * v / tot
        ptot = sum(self.prior.values()) or 1
        for e, v in self.prior.items():
            score[e] += 1e-3 * v / ptot
        return [e for e, _ in score.most_common()]


def cmd_stats(a):
    rows = list(load())
    at = Causebook(rows)
    n_mut = sum(len(r["recs"]) for r in rows)
    print(f"{len(rows)} functions, {n_mut} mutants, {len(at.table['k0'])} k0 / {len(at.table['k1'])} k1 / "
          f"{len(at.table['k2'])} k2 keys")
    print("\nedit types: mutants, share that changed nothing")
    for e, n in at.tried.most_common():
        print(f"  {e:28} {n:7}  {at.noop[e] / n:5.0%} no-op")
    print(f"\nlargest k1 shapes (functions, top causes with share):")
    big = sorted(at.table["k1"], key=lambda k: -len(at.funcs["k1"][k]))[:a.top]
    for k in big:
        c = at.table["k1"][k]
        tot = sum(c.values())
        causes = ", ".join(f"{e} {v / tot:.0%}" for e, v in c.most_common(3))
        print(f"  {len(at.funcs['k1'][k]):5}  {k[:60]:60}  {causes}")


def cmd_heldout(a):
    """For each held-out function's mutants with a nonempty difference: the rank of the
    true edit type under the causebook (built without that function), versus the prior alone."""
    rows = list(load())
    funcs = sorted({r["func"] for r in rows})
    rng = random.Random(a.seed)
    test = set(rng.sample(funcs, max(1, int(len(funcs) * a.test_frac))))
    at = Causebook(rows, exclude=test)
    prior = [e for e, _ in at.prior.most_common()]
    ranks, pranks, level = [], [], collections.Counter()
    per_type = collections.defaultdict(list)
    for row in rows:
        if row["func"] not in test:
            continue
        for r in row["recs"]:
            if not r["sig"]:
                continue
            order = at.rank(r["sig"])
            rk = order.index(r["edit"]) + 1 if r["edit"] in order else 99
            pr = prior.index(r["edit"]) + 1 if r["edit"] in prior else 99
            ranks.append(rk)
            pranks.append(pr)
            per_type[r["edit"]].append(rk)
            level[at.explain(r["sig"][0])[0]] += 1

    def top(rs, k):
        return sum(1 for x in rs if x <= k) / max(len(rs), 1)

    print(f"{len(test)} held-out functions, {len(ranks)} mutants with a difference")
    print(f"causebook top-1 {top(ranks, 1):.0%}  top-3 {top(ranks, 3):.0%}  top-5 {top(ranks, 5):.0%}")
    print(f"prior  top-1 {top(pranks, 1):.0%}  top-3 {top(pranks, 3):.0%}  top-5 {top(pranks, 5):.0%}")
    print("first region matched at:", dict(level))
    print("\nper edit type (mutants, causebook top-3):")
    for e, rs in sorted(per_type.items(), key=lambda x: -len(x[1])):
        print(f"  {e:28} {len(rs):6}  {top(rs, 3):5.0%}")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    m = sub.add_parser("mine")
    m.add_argument("--sample", type=int, default=500)
    m.add_argument("--seed", type=int, default=1)
    m.add_argument("-j", "--jobs", type=int, default=os.cpu_count())
    m.add_argument("--seeds", type=int, default=3, help="seeds per permuter pass")
    m.add_argument("--max-edits", type=int, default=150)
    m.add_argument("--min-insns", type=int, default=12)
    m.add_argument("--max-insns", type=int, default=400)
    s = sub.add_parser("stats")
    s.add_argument("--top", type=int, default=30)
    h = sub.add_parser("heldout")
    h.add_argument("--test-frac", type=float, default=0.2)
    h.add_argument("--seed", type=int, default=1)
    a = ap.parse_args()
    os.chdir(ROOT)
    import permute
    sys.path.insert(0, permute.PERMUTER)
    {"mine": cmd_mine, "stats": cmd_stats, "heldout": cmd_heldout}[a.cmd](a)


if __name__ == "__main__":
    main()
