#!/usr/bin/env python3
"""Port a matched twin's spelling into a draft, one differing hunk at a time.

  twin.py DRAFT.c FUNC TWIN.c TWINFUNC [--nonmatching] [--keep K]

Families of functions (Ghost/Lun/Axe_TurnToward..., Skeleton/Mummy...) share
code over other struct fields. Once one member matches, its spelling usually
fixes the others, but the edits that got it there are often neutral one at a
time, so tools/fix.py's beam cannot rediscover them (Axe_UpdateTurnAnimation
stalled at 17 insns; the twin's three changes together match).

Both functions are printed the same way (pycparser), and their lines are
aligned with every identifier abstracted away. Equal lines teach a renaming
from the twin's names to the draft's (fields, locals, callees); each differing
hunk is then replaced by the twin's lines, renamed, alone and in combination,
and every candidate is compiled and scored against the ROM like fix.py does.
A match is written to build/twin/FUNC.c as the whole source file
(tools/writeback.py takes it); otherwise the best candidate goes there as
FUNC.partial.c, ready for tools/fix.py. Exit 0 on a match.
"""
import argparse
import collections
import copy
import difflib
import itertools
import os
import re
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fix  # noqa: E402  (puts the permuter on sys.path)
import fixsearch  # noqa: E402
from romlib import ROOT  # noqa: E402
from perm_pycparser import c_ast as ca  # noqa: E402
from rewrites import walk  # noqa: E402
from src import ast_util  # noqa: E402

OUT = os.path.join(ROOT, "build", "twin")
KEEP = set("""auto break case char const continue default do double else enum extern float for goto if inline
int long register return short signed sizeof static struct switch typedef union unsigned void volatile while
u8 u16 u32 u64 s8 s16 s32 s64 bool8 bool16 bool32 vu8 vu16 vu32""".split())
TOKEN = re.compile(r"[A-Za-z_]\w*|0[xX][0-9A-Fa-f]+|\d+|->|<<|>>|[^\s\w]")


def tokens(line):
    return TOKEN.findall(line)


def is_ident(t):
    return re.match(r"[A-Za-z_]\w*$", t) is not None and t not in KEEP


def shape(line):
    return " ".join("$" if is_ident(t) else t for t in tokens(line))


def items_of(node):
    """The statement lists inside NODE: (owner, attribute) pairs whose value is a list of statements."""
    out = []
    if isinstance(node, ca.Compound):
        out.append((node, "block_items"))
    for _, child in node.children():
        if isinstance(child, ca.Node) and not isinstance(child, ca.Compound) and hasattr(child, "children"):
            if isinstance(child, (ca.If, ca.For, ca.While, ca.DoWhile, ca.Switch, ca.Label, ca.Case, ca.Default)):
                out += items_of(child)
        elif isinstance(child, ca.Compound):
            out += items_of(child)
    return out


def text_of(node):
    return ast_util.to_c(node).strip() if node is not None else ""


def align(tlist, dlist, sites, pairs):
    """Align two statement lists by shape; record replaceable draft runs (SITES) and same-shape
    statement pairs (PAIRS, to learn names from)."""
    tl, dl = tlist or [], dlist or []
    sm = difflib.SequenceMatcher(None, [shape(text_of(x)) for x in tl], [shape(text_of(x)) for x in dl],
                                 autojunk=False)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            pairs += [(tl[i1 + k], dl[j1 + k]) for k in range(i2 - i1)]
            continue
        sites.append((dl, j1, j2, tl[i1:i2]))
        # pair the run's statements by kind (an if with an if), then look inside each pair
        kinds = difflib.SequenceMatcher(None, [type(x).__name__ for x in tl[i1:i2]],
                                        [type(x).__name__ for x in dl[j1:j2]], autojunk=False)
        for ktag, a1, a2, b1, b2 in kinds.get_opcodes():
            if ktag != "equal":
                continue
            for k in range(a2 - a1):
                t, dd = tl[i1 + a1 + k], dl[j1 + b1 + k]
                if (i2 - i1, j2 - j1) != (1, 1):
                    sites.append((dl, j1 + b1 + k, j1 + b1 + k + 1, [t]))
                pairs.append((t, dd))
                inside(t, dd, sites, pairs)


def inside(t, dd, sites, pairs):
    """Align the statement lists inside two statements of the same kind."""
    if type(t) is not type(dd):
        return
    if isinstance(t, ca.If):
        if shape(text_of(t.cond)) == shape(text_of(dd.cond)):
            pairs.append((t.cond, dd.cond))
        for attr in ("iftrue", "iffalse"):
            a, b = getattr(t, attr), getattr(dd, attr)
            if isinstance(a, ca.Compound) and isinstance(b, ca.Compound):
                align(a.block_items, b.block_items, sites, pairs)
    elif isinstance(t, ca.Compound):
        align(t.block_items, dd.block_items, sites, pairs)
    elif isinstance(t, (ca.For, ca.While, ca.DoWhile)) and isinstance(t.stmt, ca.Compound) \
            and isinstance(dd.stmt, ca.Compound):
        align(t.stmt.block_items, dd.stmt.block_items, sites, pairs)


def renamed(node, names):
    node = copy.deepcopy(node)
    for n, _, _ in walk(node):
        if isinstance(n, ca.ID) and n.name in names:
            n.name = names[n.name]
        elif isinstance(n, ca.StructRef) and isinstance(n.field, ca.ID) and n.field.name in names:
            n.field.name = names[n.field.name]
    return node


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("draft")
    ap.add_argument("func")
    ap.add_argument("twin")
    ap.add_argument("twin_func")
    ap.add_argument("--nonmatching", action="store_true")
    ap.add_argument("--keep", type=int, default=5, help="best candidates to print")
    a = ap.parse_args()
    text = open(a.draft).read()
    if a.nonmatching:
        text = fix.strip_draft(text)
    twin_text = fix.strip_draft(open(a.twin).read())
    with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as wd:
        d = fix.Draft(text, a.func, wd)
        tdir = os.path.join(wd, "twin")
        os.makedirs(tdir)
        tpath = os.path.join(tdir, f"{a.twin_func}.c")
        open(tpath, "w").write(twin_text)
        _, _, tfn, _ = fixsearch.prepare(tpath, a.twin_func)
        sites, pairs = [], []
        align(tfn.body.block_items, d.fn.body.block_items, sites, pairs)
        votes = collections.defaultdict(collections.Counter)
        for t, dd in pairs:
            a_, b_ = tokens(text_of(t)), tokens(text_of(dd))
            if len(a_) == len(b_):
                for x, y in zip(a_, b_):
                    if is_ident(x) and is_ident(y):
                        votes[x][y] += 1
        names = {x: c.most_common(1)[0][0] for x, c in votes.items()}
        names[a.twin_func] = a.func
        base = d.score()
        print(f"{a.func}: {base[0]} differing insns; {len(sites)} statement runs differ from {a.twin_func} "
              f"({len(names)} names mapped)")

        def build(chosen):
            saved = []
            for lst, j1, j2, tnodes in sorted(chosen, key=lambda c: -c[1]):  # later runs first
                saved.append((lst, j1, list(lst)))
                lst[j1:j2] = [renamed(x, names) for x in tnodes]
            fn_text = ast_util.to_c(d.fn).strip()
            for lst, _, old in reversed(saved):
                lst[:] = old
            return fn_text

        def disjoint(combo):
            spans = [(id(c[0]), c[1], c[2]) for c in combo]
            return all(not (x[0] == y[0] and x[1] < y[2] and y[1] < x[2]) for x, y in itertools.combinations(spans, 2))

        hunks = sites
        combos = [(h,) for h in hunks]
        if len(hunks) <= 12:
            combos += [c for c in itertools.combinations(hunks, 2) if disjoint(c)]
            combos += [c for c in itertools.combinations(hunks, 3) if disjoint(c)]
        results, seen, failed = [], set(), 0
        for combo in combos:
            fn_text = build(list(combo))
            if fn_text in seen:
                continue
            seen.add(fn_text)
            span = fix.find_def(d.text, d.src_name)
            body = fn_text
            if d.src_name != d.func:
                body = re.sub(r"\b" + d.func + r"\b", d.src_name, body, count=1)
            cand = d.text[:span[0]] + body + "\n" + d.text[span[1]:]
            cpath = os.path.join(wd, "cand", f"{a.func}.c")
            os.makedirs(os.path.dirname(cpath), exist_ok=True)
            open(cpath, "w").write(cand)
            try:
                _, _, cfn, _ = fixsearch.prepare(cpath, a.func)
            except (Exception, SystemExit):
                failed += 1
                continue
            sc = d.score(cfn)
            if sc is None:
                failed += 1
                continue
            results.append((sc, combo, cand))
            if sc[0] == 0:
                break
        results.sort(key=lambda r: (r[0][0], r[0][2], len(r[1])))
        if failed:
            print(f"   {failed} of {len(seen)} candidates do not compile (a twin field or helper the draft lacks)")
        for sc, combo, _ in results[:a.keep]:
            where = "; ".join(text_of(c[3][0]).split("\n")[0][:40] if c[3] else "(delete)" for c in combo)
            print(f"   {sc[0]:4} insns ({sc[2]} beyond registers)  twin's {where}")
        os.makedirs(OUT, exist_ok=True)
        if not results or results[0][0][0] >= base[0]:
            print("no twin hunk helps")
            return 1
        sc, combo, cand = results[0]
        out = os.path.join(OUT, a.func + (".c" if sc[0] == 0 else ".partial.c"))
        open(out, "w").write(cand)
        if sc[0] == 0:
            print(f"MATCH: {os.path.relpath(out, ROOT)} (tools/writeback.py {a.draft} {a.func} "
                  f"{os.path.relpath(out, ROOT)})")
            return 0
        print(f"best {sc[0]} insns: {os.path.relpath(out, ROOT)}; continue with tools/fix.py on it")
        return 1


if __name__ == "__main__":
    sys.exit(main())
