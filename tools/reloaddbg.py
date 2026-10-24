#!/usr/bin/env python3
"""Explain each reload register that differs from the ROM, step by step.

  reloaddbg.py FILE.c [FUNC] [--nonmatching] [--all]

Reload picks a register for a constant, a spilled pseudo or an insn's scratch
(the zero index of `ldrsh rD, [rB, rI]`) by rotation (reload1.c,
allocate_reload_reg): it starts after the register the previous reload took
and takes the first spill register that is free at this insn. A spill
register is not free when a live pseudo holds it (U) or another reload of
the same insn took it (b).

The instrumented compiler (tools/agbcc-instr.sh, AGBCC_DUMPPRI) logs that
state for every choice ("RS" lines). This tool aligns the candidate with the
ROM, finds the reloads whose register differs, and says why the compiler took
its register instead of the ROM's:

* "skipped rY: holds pseudo P (name)": P is live across the insn here. The
  ROM's P ended earlier, started later, or lives elsewhere: move its last use
  up, its definition down, or change its allocation.
* "rotation started after rS": the ROM's previous reload ended elsewhere. The
  previous reload (its line is printed) is the one to change, or a reload is
  missing or extra before this one.
"""
import argparse
import os
import re
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import build  # noqa: E402
import regdiag  # noqa: E402
from romlib import ROOT  # noqa: E402

INSTR = os.path.join(ROOT, "build", "agbcc-instr", "gcc", "agbcc")


def parse_rs(text, func):
    """[(k, uid, chosen, start, states {reg: char}, before {reg: pseudo}, after)]"""
    out = []
    for ln in text.splitlines():
        p = ln.split()
        if len(p) < 8 or p[0] != "RS" or p[1] != func:
            continue
        k, uid, chosen, start, _pass = map(int, p[2:7])
        states = {int(m.group(1)): m.group(2) for m in re.finditer(r"(\d+)([Ubc.])", p[7])}
        rest = " ".join(p[8:])
        before_s, _, after_s = rest.partition("|")
        before = {int(h): int(q) for h, q in re.findall(r"r(\d+)=p(\d+)", before_s)}
        after = {int(h): int(q) for h, q in re.findall(r"r(\d+)=p(\d+)", after_s)}
        out.append((k, uid, chosen, start, states, before, after))
    return out


def names_of(text, func):
    """{pseudo: source name} from the L/G dump lines."""
    out = {}
    for ln in text.splitlines():
        p = ln.split()
        if len(p) >= 2 and p[1] == func:
            if p[0] == "L" and len(p) > 11:
                out[int(p[3])] = p[11]
            elif p[0] == "G" and len(p) > 8:
                out.setdefault(int(p[2]), p[8])
    return out


def rotation(states, start):
    """Spill registers in the order the choice tries them."""
    regs = sorted(states)
    if start in regs:
        i = regs.index(start) + 1
        return regs[i:] + regs[:i]
    return regs


def analyze(src, func=None, nonmatching=False):
    """(regdiag analysis, [finding]) where a finding is a dict."""
    if not os.path.exists(INSTR):
        build.sh(["sh", os.path.join(ROOT, "tools", "agbcc-instr.sh")])
    with tempfile.TemporaryDirectory() as wd:
        dump = os.path.join(wd, "pri.txt")
        os.environ["AGBCC_DUMPPRI"] = dump
        orig_cc = build.compiler_for
        build.compiler_for = lambda s: INSTR
        try:
            an = regdiag.analyze(src, func, nonmatching)
        finally:
            build.compiler_for = orig_cc
            os.environ.pop("AGBCC_DUMPPRI", None)
        text = open(dump).read() if os.path.exists(dump) else ""
    rs = parse_rs(text, an.func)
    names = names_of(text, an.func)
    # aligned pairs (candidate offset -> original text), equal-shape only
    pair = {}
    for tag, i1, i2, j1, j2 in an.opcodes:
        if tag == "equal":
            for (co, ct), (oo, ot) in zip(an.cd[i1:i2], an.od[j1:j2]):
                pair[co] = (ct, ot)
    by_uid = {}
    for o, (uid, t) in an.by_off.items():
        by_uid.setdefault(uid, []).append(o)
    offs = sorted(an.by_off)
    findings = []
    for k, uid, chosen, start, states, before, after in rs:
        if uid not in by_uid:
            continue
        # the insn itself plus the reload insns emitted right before it
        first = min(by_uid[uid])
        i = offs.index(first)
        span = list(by_uid[uid])
        j = i - 1
        while j >= 0 and (an.by_off[offs[j]][0] or 0) > an.max_uid:
            span.append(offs[j])
            j -= 1
        want = None
        where = None
        for o in sorted(span):
            if o not in pair:
                continue
            ct, ot = pair[o]
            cr, orr = regdiag.norm_regs(ct), regdiag.norm_regs(ot)
            if len(cr) != len(orr):
                continue
            for x, y in zip(cr, orr):
                if x == f"r{chosen}" and y != x:
                    want, where = y, (o, ct, ot)
                    break
            if want:
                break
        findings.append(dict(k=k, uid=uid, chosen=chosen, start=start, states=states, before=before,
                             after=after, want=want, where=where, line=an.off_line.get(first), off=first))
    # structural differences between consecutive reloads: an instruction the ROM has and the
    # candidate lacks (or the reverse) there is often a reload of its own that shifts the rotation
    prev_off = -1
    for f in findings:
        f["between"] = [(tag, c, o) for tag, c, o in an.struct_ops
                        if any(prev_off < x <= f["off"] for x, _ in c)
                        or (not c and o and _cand_pos(an, o[0][0], prev_off, f["off"]))]
        prev_off = f["off"]
    return an, findings, names


def _cand_pos(an, orig_off, lo, hi):
    """Whether an original-only instruction at ORIG_OFF sits between candidate offsets LO and HI."""
    for tag, i1, i2, j1, j2 in an.opcodes:
        if j1 < len(an.od) and an.od[j1][0] == orig_off:
            pos = an.cd[i1][0] if i1 < len(an.cd) else 1 << 30
            return lo < pos <= hi
    return False


def explain(f, names, prev):
    """Lines saying why reload F took its register instead of the ROM's."""
    out = []
    want = f["want"]
    order = rotation(f["states"], f["start"])
    tried = []
    for r in order:
        tried.append(f"r{r}{f['states'][r]}")
        if r == f["chosen"]:
            break
    out.append(f"    tried after r{f['start']}: " + " ".join(tried) +
               "   (U holds a live pseudo, b another reload of this insn, c wrong class)")
    if not want or not want.startswith("r") or not want[1:].isdigit():
        return out
    w = int(want[1:])
    if w not in f["states"]:
        out.append(f"    r{w} is not a spill register in this function: the ROM's reload set differs"
                   " (another reload elsewhere used it), or the ROM's value here is a pseudo, not a reload")
        return out
    if order.index(w) < order.index(f["chosen"]):
        st = f["states"][w]
        if st == "U":
            p = f["before"].get(w) or f["after"].get(w)
            nm = names.get(p, "-") if p else "-"
            out.append(f"    skipped r{w}: it holds pseudo {p} ({nm}), live across this insn. In the ROM that"
                       " value is dead here, born later, or in another register")
        elif st == "b":
            out.append(f"    skipped r{w}: another reload of this insn took it")
        else:
            out.append(f"    skipped r{w}: wrong class or mode")
    else:
        out.append(f"    r{w} comes after r{f['chosen']} in the rotation from r{f['start']}: the ROM's"
                   " rotation started elsewhere")
        for tag, c, o in f.get("between", [])[:3]:
            ins = "; ".join(t for _, t in o) or "-"
            dels = "; ".join(t for _, t in c) or "-"
            out.append(f"    between the previous reload and this one the ROM has `{ins}` where the candidate has"
                       f" `{dels}`: an instruction that needs a reload there moves the rotation")
        if prev:
            out.append(f"    previous reload #{prev['k']} (line {prev['line']}) took r{prev['chosen']};"
                       f" for the ROM's r{w} it would have ended between r{f['chosen']} and r{w}, or the"
                       " registers between were busy in the ROM")
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("src")
    ap.add_argument("func", nargs="?")
    ap.add_argument("--nonmatching", action="store_true")
    ap.add_argument("--all", action="store_true", help="list every reload, not only the differing ones")
    a = ap.parse_args()
    src = os.path.relpath(os.path.abspath(a.src), ROOT)
    an, findings, names = analyze(src, a.func, a.nonmatching)
    lines = open(os.path.join(ROOT, src)).read().splitlines()
    print(f"{an.func}: {len(findings)} reload choices, "
          f"{sum(1 for f in findings if f['want'])} differ from the ROM")
    prev = None
    for f in findings:
        if f["want"] or a.all:
            text = lines[f["line"] - 1].strip()[:80] if f["line"] and f["line"] <= len(lines) else ""
            print(f"reload #{f['k']} insn {f['uid']} line {f['line']}: got r{f['chosen']}"
                  + (f", ROM {f['want']}" if f["want"] else "") + f"   {text}")
            if f["where"]:
                o, ct, ot = f["where"]
                print(f"    {o:04X} {ct}  | ROM {ot}")
            for ln in explain(f, names, prev):
                print(ln)
        prev = f


if __name__ == "__main__":
    main()
