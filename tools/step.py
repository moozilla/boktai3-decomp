#!/usr/bin/env python3
"""Step through agbcc's decisions for a draft and stop at the first one that differs from the ROM.

  step.py FILE.c [FUNC] [--nonmatching] [--all]

For every instruction that differs from the ROM, shape or register alike, it
follows the RTL insn that produced it through the passes (`-da` dumps of the
instrumented agbcc) and names the decision events on the way:

  born     the pass that created the insn (loop invariant, gcse, combine, regmove copy, reload)
  swap     a pass reordered the operands of a commutative operation (cse puts an operand
           it knows to be a constant second; combine canonicalizes), which decides which
           operand a two-operand Thumb instruction copies
  tie      regmove gave the output the register of an input
  alloc    nothing changed before allocation: only the register choice differs

Differences are ordered by the pass of their earliest event (pipeline order),
and only the first is explained in full unless --all: a later difference is
often a consequence of the first. For a two-operand instruction with a copy
(`adds rD, rX, #0; ands rD, rY`) whose ROM version copies the other operand,
it says which value each side copies and where your operand order was set.

The point is one decision at a time, in source terms, small enough to read:
fix the first divergence, compile, step again. Rules for acting on a decision
are in docs/AGBCC_RULES.md; tools/why.py runs counterfactual probes.
"""
import argparse
import os
import re
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import regdiag  # noqa: E402
import why  # noqa: E402

COMMUTATIVE = {"plus", "mult", "and", "ior", "xor", "eq", "ne", "smin", "smax", "umin", "umax"}
OPS = re.compile(r"\((plus|minus|mult|and|ior|xor|ashift|ashiftrt|lshiftrt|neg|not|compare|eq|ne)"
                 r":\w+ (\((?:reg|const_int|mem|subreg)[^()]*(?:\([^()]*\))*[^()]*\)|\S+) "
                 r"(\((?:reg|const_int|mem|subreg)[^()]*(?:\([^()]*\))*[^()]*\)|\S+)\)")
REGNO = re.compile(r"\(reg(?:/\w)*:\w+ (\d+)")
PASS_RANK = {p: i for i, p in enumerate(why.PASSES + ["reload"])}
WHY_SWAP = {
    "cse": "cse.c fold_rtx puts an operand it knows to be a constant (or a register holding one) second; "
           "with two plain registers it keeps the order. The ROM's order means the original's operand was not "
           "a known constant at this point, or the expression was written the other way round",
    "combine": "combine.c canonicalizes commutative operands when it rebuilds the insn (complex operands first)",
    "gcse": "gcse.c copy/constant propagation substituted an operand",
    "cse2": "the second CSE pass (after loop) reordered or substituted an operand",
}


def operands(rtl):
    """(code, op1, op2) of the first binary operation in RTL, or None."""
    m = OPS.search(rtl or "")
    return (m.group(1), m.group(2), m.group(3)) if m else None


def regno(op):
    m = REGNO.match(op or "")
    return int(m.group(1)) if m else None


def events(tr, uid, tied, first_uid_after):
    """[(pass, kind, text)] decision events along an insn's trail."""
    out = []
    if not tr:
        return out
    born = tr[0][0]
    if born in ("greg", "mach") and len(tr) <= 2:
        born = "reload"
    if born not in ("rtl", "jump"):
        kind = "copy" if born == "regmove" and uid >= first_uid_after else "born"
        out.append((born, kind, why.BORN.get(born, f"made by {born}")))
    prev = None
    for p, rtl in tr:
        cur = operands(rtl)
        if prev and cur and prev[0] == cur[0] and prev[0] in COMMUTATIVE:
            a1, b1 = prev[1:]
            a2, b2 = cur[1:]
            if (a2, b2) != (a1, b1) and (a2 == b1 or b2 == a1):
                out.append((p, "swap", f"{p} reordered the operands of `{cur[0]}`: ({a1} {b1}) -> ({a2} {b2}). "
                                       + WHY_SWAP.get(p, "")))
        prev = cur or prev
    if uid in tied:
        out.append(("regmove", "tie", f"regmove tied the output to operand {tied[uid]} (same register)"))
    return out


DELETED_BY = {
    "cse": "cse found the same value already in a register (an earlier identical expression in this "
           "extended basic block) and reused it",
    "gcse": "gcse.c (PRE / copy propagation) reused a value computed in an earlier block",
    "cse2": "the second cse pass reused an equivalent value",
    "loop": "loop.c moved or eliminated it",
    "combine": "combine.c merged it into a later insn",
    "flow": "flow.c deleted it as dead",
    "jump": "jump.c deleted it",
}


def vanished(dumps, func, lines):
    """[(uid, rtl as written, pass that deleted it)] for insns of source LINES that the
    front end emitted and a later pass deleted (not merged into a surviving insn's place)."""
    rtl = dumps.get("rtl", {}).get(func, "")
    by_line = regdiag.insn_lines(rtl)
    out = []
    for uid, ln in sorted(by_line.items()):
        if ln not in lines:
            continue
        tr = why.trail(dumps, func, uid)
        if tr and tr[-1][1] == "(deleted)" and len(tr) >= 2 and tr[-1][0] in ("cse", "gcse", "cse2", "loop"):
            first = tr[0][1]
            if re.match(r"\(set \(reg[^()]*\) \((reg|const_int)[^()]*\)\)$", first):
                continue  # copies and constant loads come and go
            if first.startswith("(set (pc)") or "(use " in first or "(clobber" in first:
                continue
            out.append((uid, first, tr[-1][0]))
    return out


def dead_stores(dumps, func):
    """[(uid, rtl as written, source line)] for memory stores that flow.c deleted as dead."""
    rtl = dumps.get("rtl", {}).get(func, "")
    out = []
    for uid, ln in sorted(regdiag.insn_lines(rtl).items()):
        tr = why.trail(dumps, func, uid)
        if tr and tr[-1] == ("flow", "(deleted)") and tr[0][1].startswith("(set (mem"):
            out.append((uid, tr[0][1], ln))
    return out


class Step:
    """The explained differences of one draft: entries sorted first-divergence first.
    Each entry is a dict: rank, idx, kind (order/registers/insert/delete/replace), cand and
    orig ([(offset, text)]), uids, evs ([(uid, pass, kind, text)]), note, lines (source lines)."""


def analyze(src, func=None, nonmatching=False):
    """A Step for SRC, or one with .matched set. Compiles twice (regdiag and the -da dumps)."""
    st = Step()
    an = regdiag.analyze(src, func, nonmatching)
    st.an, st.func, st.src = an, an.func, src
    st.matched = an.matched
    st.entries = []
    if an.matched:
        return st
    func = an.func
    lines = open(src).read().splitlines()
    st.lines = lines

    def show(ln):
        return f"line {ln}: {lines[ln - 1].strip()[:80]}" if ln and 0 < ln <= len(lines) else "line ?"
    st.show = show

    with tempfile.TemporaryDirectory() as wd:
        dumps = why.compile_dumps(src, wd)
    st.dumps = dumps
    tied = why.ties(dumps, func)
    combine_uids = regdiag.insns_by_uid(dumps.get("combine", {}).get(func, ""))
    first_new = max(combine_uids) + 1 if combine_uids else 1 << 30

    def pseudo_name(p):
        ls = sorted(an.pseudo_lines.get(p, ()))
        return f"pseudo {p} ({show(ls[0])})" if ls else f"pseudo {p}"

    # every differing candidate instruction, with its ROM counterpart
    diffs = []
    for tag, i1, i2, j1, j2 in an.opcodes:
        if tag == "equal":
            for k in range(i2 - i1):
                (co, ct), (oo, ot) = an.cd[i1 + k], an.od[j1 + k]
                if regdiag.norm_regs(ct) != regdiag.norm_regs(ot):
                    diffs.append(("registers", i1 + k, [(co, ct)], [(oo, ot)]))
        else:
            diffs.append((tag, i1, an.cd[i1:i2], an.od[j1:j2]))
    st.n_places = len(diffs)
    st.n_register = sum(1 for d in diffs if d[0] == "registers")

    explained = []
    # an instruction the ROM has earlier (or later) than you: a pure ordering difference
    import idioms
    shp = lambda xs: [idioms.shape(t) for _, t in xs]
    orders, used = [], set()
    for a_, (k1, i1, c1, o1) in enumerate(diffs):
        if k1 != "delete" or a_ in used:  # opcodes run candidate -> ROM: "delete" is yours only
            continue
        for b_, (k2, i2, c2, o2) in enumerate(diffs):
            if k2 == "insert" and b_ not in used and shp(o2) == shp(c1):
                used |= {a_, b_}
                orders.append((i1, c1, i2, o2))
                break
    diffs = [d for n_, d in enumerate(diffs) if n_ not in used]
    frame = re.compile(r"^(push|pop|add sp|sub sp|bx)\b")
    diffs = [d for d in diffs if not all(frame.match(t) for _, t in d[2] + d[3])] or diffs
    raw = re.compile(r"^[0-9a-f]{4}$|^\.(\dbyte|word|hword|short)")  # literal pool words, not code
    diffs = [d for d in diffs if not all(raw.match(t) for _, t in d[2] + d[3])] or diffs
    for kind, idx, cand, orig in diffs:
        uids = [an.by_off[o][0] for o, _ in cand if an.by_off.get(o)]
        if not uids and idx < len(an.cd):
            uids = [an.by_off[an.cd[idx][0]][0]]
        evs = []
        for u in uids:
            for e in events(why.trail(dumps, func, u), u, tied, first_new):
                evs.append((u,) + e)
        # two-operand copy: does the ROM copy the other operand?
        note = None
        if idx >= 1 and kind == "registers":
            (_, c_prev), (_, c_cur) = an.cd[idx - 1], an.cd[idx]
            j = next((jj for t_, ii1, ii2, jj1, jj2 in an.opcodes if t_ == "equal" and ii1 <= idx < ii2
                      for jj in [jj1 + idx - ii1]), None)
            if j is not None and j >= 1:
                o_prev, o_cur = an.od[j - 1][1], an.od[j][1]
                mc = re.match(r"adds (r\d+), (r\d+), #0$", c_prev), re.match(r"(\w+) (r\d+), (r\d+)$", c_cur)
                mo = re.match(r"adds (r\d+), (r\d+), #0$", o_prev), re.match(r"(\w+) (r\d+), (r\d+)$", o_cur)
                if all(mc) and all(mo) and mc[1].group(3) == mo[0].group(2) and mo[1].group(3) == mc[0].group(2):
                    u = an.by_off[an.cd[idx][0]][0]
                    tr = why.trail(dumps, func, u)
                    ops = next((operands(r) for p, r in reversed(tr) if p in ("combine", "flow", "cse2", "cse")
                                and operands(r)), None)
                    if ops:
                        p1, p2 = regno(ops[1]), regno(ops[2])
                        note = (f"two-operand `{mc[1].group(1)}`: you copy operand 1 = {pseudo_name(p1)} and use "
                                f"operand 2 = {pseudo_name(p2)}; the ROM copies the other one. The decision is the "
                                f"operand order of insn {u} before regmove, not register allocation.")
        if kind == "registers" or kind == "replace":
            # a signed/unsigned condition: the comparison's type
            SIGNED = {"blt": "bcc", "bge": "bcs", "bgt": "bhi", "ble": "bls"}
            pair = SIGNED | {v: k for k, v in SIGNED.items()}
            for (_, ct), (_, ot) in zip(cand, orig):
                c0, o0 = ct.split(" ")[0], ot.split(" ")[0]
                if pair.get(c0) == o0:
                    sgn = "signed" if o0 in SIGNED else "unsigned"
                    note = (f"comparison type: the ROM branches with `{o0}` ({sgn}), you with `{c0}`; make the "
                            f"compared value {sgn} (a cast or the field's type).")
        if kind in ("insert", "delete", "replace"):
            # a narrowing right after a call: the callee's declared return type
            for side, xs, other in (("ROM", orig, cand), ("yours", cand, orig)):
                m = re.match(r"lsls (r\d+), \1, #(0x18|0x10)$", xs[0][1]) if xs else None
                seq = an.od if side == "ROM" else an.cd
                k = next((n_ for n_, (o, _) in enumerate(seq) if xs and o == xs[0][0]), None)
                if m and k and seq[k - 1][1].startswith("bl ") and not any(t.startswith("lsls") for _, t in other):
                    width = "u8/s8" if m.group(2) == "0x18" else "u16/s16"
                    note = (f"call result: {'the ROM narrows' if side == 'ROM' else 'you narrow'} the value returned by "
                            f"`{seq[k - 1][1]}` to {width}; {'declare or cast' if side == 'ROM' else 'do not cast'} "
                            f"the callee's return type that way.")
        if kind in ("delete", "replace") and orig:
            near = [an.off_line.get(an.cd[k][0]) for k in (idx - 1, idx, idx + 1) if 0 <= k < len(an.cd)]
            gone = vanished(dumps, func, {x for x in near if x})
            for u, first, p in gone[:3]:
                evs.append((u, p, "deleted", f"your insn `{first[:70]}` was deleted by {p}: "
                                             + DELETED_BY.get(p, "")))
        n_str = lambda xs: sum(t.startswith("str") for _, t in xs)
        if kind in ("insert", "replace") and n_str(orig) > n_str(cand) and not note:
            gone = dead_stores(dumps, func)
            near = [an.off_line.get(an.cd[k][0]) for k in (idx - 1, idx) if 0 <= k < len(an.cd)]
            near = [x for x in near if x]
            if gone:
                u, first, ln = min(gone, key=lambda g: min((abs(g[2] - x) for x in near), default=0))
                note = (f"dead store: the ROM keeps a store that flow.c deleted as dead in yours, insn {u} "
                        f"`{first[:60]}` ({show(ln)}). flow deletes a store when a later store in the block "
                        f"writes the same address and nothing between them may read it. agbcc -O2 gives each "
                        f"type an alias set: a read of another type (a pointer field read into a u32) never "
                        f"aliases, so the original's read between the two stores had the stored type: "
                        f"`*(u32 *)&x->f`, or the field declared that type.")
        rank = min([PASS_RANK.get(e[1], 99) for e in evs] or [PASS_RANK["greg"] + (kind == "registers")])
        # shape differences in code order, then register-only ones (usually consequences)
        rank = 100 if kind == "registers" else 0
        explained.append((rank, idx, kind, cand, orig, uids, evs, note))

    for i1, c1, i2, o2 in orders:
        mine = an.off_line.get(c1[0][0])
        when = "later" if i2 > i1 else "earlier"
        k0 = next((n_ for n_, (o, _) in enumerate(an.od) if o == o2[0][0]), None)
        k1 = next((n_ for n_, (o, _) in enumerate(an.od) if o == o2[-1][0]), None)
        before = an.od[k0 - 1][1] if k0 else "(start)"
        after = an.od[k1 + 1][1] if k1 is not None and k1 + 1 < len(an.od) else "(end)"
        at = an.off_line.get(an.cd[min(i2, len(an.cd) - 1)][0]) if an.cd else None
        where = f" ({show(at)})" if at and at != mine else ""
        note = (f"order: the ROM computes `{'; '.join(t for _, t in c1)}` ({show(mine)}) {when} than you, between "
                f"`{before}` and `{after}`{where}. agbcc has no instruction scheduler: code comes out in source "
                f"evaluation order, minus what cse/loop/combine delete or move. So evaluate that value at that "
                f"point in the C: a local initialized there, an argument (evaluated before the call or inline "
                f"body), or statement order.")
        explained.append((0, min(i1, i2), "order", c1, [], [], [], note))
    # the frame: which registers are live at exit, and how many are saved
    def frame(xs):
        pops = [t for _, t in xs if t.startswith("pop")]
        push = next((t for _, t in xs if t.startswith("push")), "")
        return pops, len(re.findall(r"r\d+", push))
    (cp, cn), (op, on) = frame(an.cd), frame(an.od)
    ret_c, ret_o = any("{r0}" in t for t in cp), any("{r0}" in t for t in op)
    if ret_c != ret_o and (cp and op):
        note = ("frame: the ROM's epilogue pops the return address into "
                + ("r0, so nothing is returned: make the function void."
                   if ret_o else
                   "r1, so r0 is live at exit: the function returns a value. If nothing in the ROM sets r0, the "
                   "original is non-void with no return statement (r0 still holds the argument, which is then "
                   "copied out of r0)."))
        explained.append((-1, 0, "frame", [], [], [], [], note))
    elif cn != on:
        explained.append((101, 0, "frame", [], [], [], [], (
            f"frame: the ROM saves {on - 1} registers, you save {cn - 1}: "
            f"{'more' if on > cn else 'fewer'} values are live at once in the original (a value kept instead "
            f"of recomputed, or a constant held in a register).")))
    explained.sort(key=lambda x: (x[0], x[1]))
    for rank, idx, kind, cand, orig, uids, evs, note in explained:
        src_lines = sorted({an.off_line.get(o) for o, _ in cand if an.off_line.get(o)})
        if not src_lines and kind == "order":
            src_lines = [x for x in [an.off_line.get(cand[0][0])] if x]
        st.entries.append(dict(rank=rank, idx=idx, kind=kind, cand=cand, orig=orig, uids=uids, evs=evs,
                               note=note, lines=src_lines))
    return st


def render(st, e, title, trails=2):
    """The lines that explain entry E of Step ST."""
    out = [f"== {title}"]
    if e["kind"] == "frame":
        return out + [f"   {e['note']}"]
    for ln in e["lines"][:3]:
        out.append(f"   {st.show(ln)}")
    out.append(f"   ROM:   {'; '.join(t for _, t in e['orig']) or '(nothing)'}")
    out.append(f"   yours: {'; '.join(t for _, t in e['cand']) or '(nothing)'}")
    if e["note"]:
        out.append(f"   {e['note']}")
    if e["kind"] == "order":
        pass
    elif e["note"] and e["note"].startswith(("call result", "dead store")):
        pass
    elif e["evs"]:
        for u, p, k, text in sorted(e["evs"], key=lambda x: PASS_RANK.get(x[1], 99)):
            out.append(f"   [{p}] {k}, insn {u}: {text}")
    else:
        out.append("   no decision before allocation touched these insns: the difference is the register "
                   "choice (tools/regdiag.py, tools/rasolve.py)")
    for u in e["uids"][:trails]:
        tr = why.trail(st.dumps, st.func, u)
        if tr:
            out.append(f"   trail of insn {u}:")
            for p, r in tr:
                out.append(f"      [{p:8}] {r[:120]}")
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("src")
    ap.add_argument("func", nargs="?")
    ap.add_argument("--nonmatching", action="store_true")
    ap.add_argument("--all", action="store_true", help="explain every difference, not only the first")
    a = ap.parse_args()
    st = analyze(os.path.abspath(a.src), a.func, a.nonmatching)
    if st.matched:
        print(f"{st.func}: matches the ROM")
        return 0
    print(f"{st.func}: {st.n_places} differing places ({st.n_register} register-only)")
    for n, e in enumerate(st.entries):
        if n and not a.all:
            rest = len(st.entries) - 1
            print(f"\n({rest} later difference{'s' * (rest != 1)}; fix this one first, or --all)")
            break
        print()
        print("\n".join(render(st, e, "first divergence" if n == 0 else f"difference {n + 1}")))
    return 1


if __name__ == "__main__":
    sys.exit(main())
