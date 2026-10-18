#!/usr/bin/env python3
"""Structural distance with register allocation ignored, per region of the ROM function.

  regions.py FILE.c [FUNC] [--all]

Byte and fuzzy scores have no slope on a large function: once one value sits in
another register, the copies and spills that follow change bytes everywhere, so
a correct fix in one place can lower the score. This compares only the core
work: register copies (`adds rX, rY, #0`, `mov`), stack spills and reloads
(`str/ldr rX, [sp, #k]`) and the frame (push/pop/sp adjust) are dropped, and
registers are abstracted. What is left differs only where the C does
different work (an operation, a constant, an offset, a branch, a call).

The ROM function is split into regions at branch targets and after branches
(basic blocks, merged into runs of at least MIN_REGION core instructions), and
each region is LOCKED when its core instructions all line up. The report lists
the regions that differ, with the ROM's and your core instructions, so you can
work region by region: lock one, check, move on. Register allocation comes last
(tools/rasolve.py, tools/autosolve.py; docs/PERMUTER.md).

Exit status 0 when the core distance is 0.
"""
import argparse
import difflib
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import check  # noqa: E402
import permute  # noqa: E402
import rasolve  # noqa: E402
from romlib import ROOT  # noqa: E402

REG = r"(?:r\d+|sb|sl|fp|ip)"
MIN_REGION = 6


def code_only(dis):
    """The listing without literal-pool words, jump-table data and alignment padding."""
    pool = set()
    for a, t in dis:
        m = re.match(r"ldr \w+, \[pc, #(0x[0-9a-f]+|\d+)\]", t)
        if m:
            p = ((a + 4) & ~3) + int(m.group(1), 0)
            pool |= {p, p + 2}
    return [(a, t) for a, t in dis if a not in pool and not t.startswith(".") and t != "movs r0, r0"]


def core(t):
    """The instruction as allocation-independent work, or None for an allocation artifact."""
    if re.match(rf"(adds|mov|movs) {REG}, {REG}(, #0)?$", t):
        return None
    if re.match(r"(str|ldr)\w* \w+, \[sp(, #\w+)?\]$", t):
        return None
    if re.match(r"(push|pop|add sp|sub sp)", t):
        return None
    out = re.sub(rf"\b{REG}\b", "R", t)
    if out.split()[0].startswith("b") or "[pc" in out:
        out = re.sub(r"#(-?0x[0-9a-f]+|-?\d+)", "#K", out)  # targets and pool offsets move with the code
    return out


def blocks(dis):
    """Region index for each instruction of the ROM listing [(addr, text)]."""
    targets = set()
    for a, t in dis:
        m = re.match(r"b\w* #(0x[0-9a-f]+)", t)
        if m and not t.startswith("bl "):
            targets.add(int(m.group(1), 16))
    region, out, n_core, cur = 0, [], 0, 0
    for i, (a, t) in enumerate(dis):
        if a in targets and n_core >= MIN_REGION:
            region += 1
            n_core = 0
        out.append(region)
        if core(t):
            n_core += 1
        if re.match(r"b\w* ", t) and not t.startswith("bl ") and n_core >= MIN_REGION:
            region += 1
            n_core = 0
    return out


class Report:
    def __init__(self, path, func):
        self.cc = rasolve.Compiler(os.path.abspath(path), func, lines=False)
        self.cc.pre, _ = permute.prepare_source(os.path.relpath(os.path.abspath(path), ROOT), func)
        self.func = func

    def run(self):
        cc = self.cc
        d, _ = cc.run()
        if d is None:
            return None
        obj = cc.resolver.assemble(cc.last_asm, cc.work)
        mine, _ = cc.resolver.bytes_of(obj, self.func)
        orig = cc.rom[rasolve.off(cc.address):rasolve.off(cc.address) + (cc.size or len(mine))]
        if cc.server:
            cc.server.close()
        A = code_only(check.disasm(orig, cc.address))
        B = code_only(check.disasm(mine, cc.address))
        reg = blocks(A)
        ca = [(i, core(t)) for i, (_, t) in enumerate(A) if core(t)]
        cb = [(j, core(t)) for j, (_, t) in enumerate(B) if core(t)]
        sm = difflib.SequenceMatcher(None, [x[1] for x in ca], [x[1] for x in cb], autojunk=False)
        n_regions = reg[-1] + 1 if reg else 0
        bad = {}
        dist = 0
        for tag, i1, i2, j1, j2 in sm.get_opcodes():
            if tag == "equal":
                continue
            dist += max(i2 - i1, j2 - j1)
            r = reg[ca[i1][0]] if i1 < len(ca) else reg[ca[-1][0]] if ca else 0
            if i1 == i2 and i1 > 0:
                r = reg[ca[i1 - 1][0]]
            e = bad.setdefault(r, {"rom": [], "you": [], "n": 0})
            e["n"] += max(i2 - i1, j2 - j1)
            e["rom"] += [A[ca[k][0]] for k in range(i1, i2)]
            e["you"] += [B[cb[k][0]] for k in range(j1, j2)]
        return {"bytes": d, "core": dist, "core_insns": len(ca), "regions": n_regions, "bad": bad, "A": A,
                "reg": reg}


def render(res, show_all=False, limit=8):
    if res is None:
        return "does not compile or link"
    locked = res["regions"] - len(res["bad"])
    lines = [f"CORE distance {res['core']} of {res['core_insns']} core instructions (allocation ignored); "
             f"regions locked {locked}/{res['regions']}; byte distance {res['bytes']} halfwords"]
    A, reg = res["A"], res["reg"]
    for r in sorted(res["bad"])[: None if show_all else limit]:
        e = res["bad"][r]
        span = [a for (a, _), x in zip(A, reg) if x == r]
        same = sorted(core(t) for _, t in e["rom"]) == sorted(core(t) for _, t in e["you"])
        lines.append(f"  region {r} at {span[0]:08X}-{span[-1]:08X}: {e['n']} core differences"
                     + (" (same work in another order: statement order, or allocation)" if same else ""))
        for tag, xs in (("ROM", e["rom"]), ("you", e["you"])):
            if xs:
                lines.append(f"    {tag:3s}: " + "; ".join(f"{t}" for _, t in xs[:6]) + (" ..." if len(xs) > 6 else ""))
    if len(res["bad"]) > limit and not show_all:
        lines.append(f"  ... {len(res['bad']) - limit} more regions differ (--all)")
    return "\n".join(lines)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("file")
    ap.add_argument("func", nargs="?")
    ap.add_argument("--all", action="store_true")
    a = ap.parse_args()
    func = a.func or os.path.splitext(os.path.basename(a.file))[0]
    res = Report(a.file, func).run()
    print(render(res, a.all))
    sys.exit(0 if res and res["core"] == 0 else 1)


if __name__ == "__main__":
    main()
