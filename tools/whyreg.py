#!/usr/bin/env python3
"""Say which compiler pass put a different register into an instruction.

  whyreg.py FILE.c [FUNC] [--nonmatching] [--all]

regdiag.py and rasolve.py assume a register difference is an allocation
choice. Most of the small register-only differences left in drafts are not:
cse, combine or reload replaced one value by another *equal* value, so the
instruction names a different pseudo than the C wrote, and no allocation
priority can fix it. (On the 1-3 instruction drafts, rasolve answers "no
priority change helps" for most of them.)

For each instruction whose registers differ from the ROM, this prints the
instruction's operands after every pass (expand, cse, cse2, combine, local
alloc) with each pseudo named by its C variable where it has one, marks the
pass where an operand changed to a different pseudo, and names the pseudo
that holds the ROM's register at that point. Typical readings:

* "cse replaced `phase` by `result`": both are known to hold the same value
  on this path (a constant, or a copy). The ROM kept the written one, so its
  cse did not know they were equal there (a different path or label shape),
  or the C named the other one.
* "operands swapped by combine/cse": a commutative operation whose operand
  order differs; write the operands the other way round or change which one
  is the older pseudo.
* "same pseudo in every pass": allocation after all; use rasolve.py.

Uses the instrumented agbcc (build/agbcc-instr, tools/agbcc-instr.sh) only for
pseudo names (AGBCC_DUMPPRI); the code it emits is agbcc's.
"""
import argparse
import collections
import os
import re
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import build  # noqa: E402
import regdiag  # noqa: E402
from romlib import ROOT  # noqa: E402

INSTR = os.path.join(ROOT, "build", "agbcc-instr", "gcc", "agbcc")
PASSES = [("jump", "as expanded"), ("cse", "cse"), ("cse2", "cse2"), ("combine", "combine"),
          ("lreg", "local alloc"), ("greg", "after reload")]
HARD = {"sb": 9, "sl": 10, "fp": 11, "ip": 12, "sp": 13, "lr": 14, "pc": 15}


def hardno(name):
    return HARD[name] if name in HARD else int(name[1:])


def compile_all(src, func, wd):
    i_path = os.path.join(wd, "u.i")
    with open(i_path, "w") as f:
        f.write(build.sh(build.CPP + ["-nostdinc", "-undef"] + build.DEFINES +
                         ["-I", "include", "-iquote", ".", src]))
    cc = INSTR if os.path.exists(INSTR) else build.compiler_for(src)
    pri = os.path.join(wd, "pri.txt")
    env = dict(os.environ, AGBCC_DUMPPRI=pri)
    r = subprocess.run([cc] + build.cflags_for(src) + ["-g", "-dp", "-dj", "-ds", "-dt", "-dc", "-dl", "-dg",
                                                       i_path, "-o", os.path.join(wd, "u.s")],
                       cwd=ROOT, capture_output=True, text=True, env=env)
    if r.returncode:
        sys.exit(r.stderr)
    dumps = {}
    for key, _ in PASSES:
        p = f"{i_path}.{key}"
        dumps[key] = regdiag.insns_by_uid(regdiag.functions_of(open(p).read()).get(func, "")) \
            if os.path.exists(p) else {}
    names, phys = {}, {}
    if os.path.exists(pri):
        for ln in open(pri):
            p = ln.split()
            if len(p) < 3 or p[1] != func:
                continue
            if p[0] == "L":
                names[int(p[3])] = p[11] if len(p) > 11 else "-"
                phys[int(p[3])] = int(p[10])
            elif p[0] == "G" and int(p[2]) not in phys:
                names[int(p[2])] = p[8] if len(p) > 8 else "-"
                phys[int(p[2])] = int(p[7])
    lines = regdiag.insn_lines(regdiag.functions_of(open(f"{i_path}.lreg").read()).get(func, ""))
    return dumps, names, phys, lines


def operands(rtl):
    """Registers in the insn's pattern (not its notes), in order."""
    body = re.split(r"\(expr_list:REG_|\(insn_list", rtl)[0]
    return [n for n, _ in regdiag.regs_in(body)]


def label(n, names):
    if n < 16:
        return f"r{n}"
    nm = names.get(n, "-")
    return f"{n} `{nm}`" if nm not in ("-", None) else f"{n}"


def defining_text(n, dump):
    for uid in sorted(dump):
        m = re.search(rf"\(set \(reg(?:/[a-z]+)*:[A-Z]+ {n}\)\s+(.*)", dump[uid], re.S)
        if m:
            return re.sub(r"\s+", " ", m.group(1))[:70]
    return ""


def explain(src, func=None, nonmatching=False, show_all=False, out=sys.stdout):
    an = regdiag.analyze(src, func, nonmatching)
    func = an.func
    with tempfile.TemporaryDirectory() as wd:
        dumps, names, phys, lines = compile_all(src, func, wd)
    rows = [ln for ln in an.listing if ln.startswith("!")]
    if an.struct_ops:
        print(f"{func}: {len(an.struct_ops)} structural difference(s) too (regdiag.py --asm); "
              f"registers below", file=out)
    if not rows:
        print(f"{func}: no register-only differences", file=out)
        return []
    verdicts = []
    for row in rows if show_all else rows[:6]:
        m = re.match(r"! (\w+) (.*?)\s+\| (.*?)\s+insn (\S+) line (\S+)", row)
        if not m:
            continue
        o, ours, rom, uid, line = m.groups()
        uid = int(uid) if uid.isdigit() else None
        print(f"\n{o} {ours}   ROM: {rom}   (insn {uid}, line {line})", file=out)
        if uid is None or uid not in dumps["lreg"]:
            print("   made by reload (no insn of its own before reload): rasolve.py's reload rotation", file=out)
            verdicts.append("reload")
            continue
        diff = [(x, y) for x, y in zip(regdiag.norm_regs(ours), regdiag.norm_regs(rom)) if x != y]
        hist, prev = [], None
        for key, title in PASSES[:-1]:
            if uid not in dumps[key]:
                continue
            ops = operands(dumps[key][uid])
            hist.append((key, title, ops))
        for key, title, ops in hist:
            print(f"   {title:12s} " + ", ".join(label(n, names) for n in ops), file=out)
        # which pseudo has the ROM's register in our build, among those this insn ever named
        named = {n for _, _, ops in hist for n in ops if n >= 16}
        holders = set()
        for x, y in diff:
            hs = [n for n in named if phys.get(n) == hardno(y)]
            holders.update(hs)
            if hs:
                print(f"   the ROM's {y} is {', '.join(label(n, names) for n in hs)} in your build", file=out)
        swap = len(diff) >= 2 and sorted(x for x, _ in diff) == sorted(y for _, y in diff)
        # the pass that dropped a value the ROM still uses here
        dropped = None
        for (_, _, a), (_, t1, b) in zip(hist, hist[1:]):
            gone = [n for n in a if n not in b and n in holders]
            if gone and dropped is None:
                dropped = (t1, gone, [n for n in b if n not in a and n >= 16])
        if swap:
            print("   -> same values in the other order: commutative operand order (write the operands "
                  "the other way round, or make the other one the older pseudo)", file=out)
            verdicts.append("operand order")
        elif dropped:
            t1, gone, new = dropped
            print(f"   -> {t1} replaced {', '.join(label(n, names) for n in gone)} with "
                  f"{', '.join(label(n, names) for n in new) or 'another value'}: it knew they are equal here; "
                  f"the ROM's compiler did not", file=out)
            verdicts.append(f"{t1} substitution")
        else:
            print("   -> the same value in every pass: allocation (rasolve.py)", file=out)
            verdicts.append("allocation")
    if len(rows) > 6 and not show_all:
        print(f"\n({len(rows) - 6} more; --all)", file=out)
    return verdicts


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("func", nargs="?")
    ap.add_argument("--nonmatching", action="store_true")
    ap.add_argument("--all", action="store_true")
    a = ap.parse_args()
    explain(a.src, a.func, a.nonmatching, a.all)


if __name__ == "__main__":
    main()
