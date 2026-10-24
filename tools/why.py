#!/usr/bin/env python3
"""Say, in source terms, why a draft's code differs from the ROM and what would flip it.

  why.py FILE.c [FUNC] [--nonmatching] [--no-probe]

For each run of instructions whose shape differs from the ROM (regdiag's
structural differences), it prints:

* the source lines that produced them, and the ROM's instructions there;
* the decision trail: each pass that created or rewrote those insns
  (instrumented agbcc, `-da` dumps), with regmove's ties named;
* counterfactual probes: the instrumented compiler is told to skip one
  decision (for now: tying an output to an input at an insn, AGBCC_NOTIE)
  and the probe is reported when that makes this window take the ROM's shape;
* the source levers known to change that decision (docs/AGBCC_RULES.md).

Register-only differences follow, as regdiag reports them. The point is to
replace guessing: a draft that is wrong because of one compiler decision
should say which decision, and what in the source controls it.
"""
import argparse
import json
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
PASSES = ["rtl", "jump", "cse", "addressof", "gcse", "loop", "cse2", "flow", "combine", "regmove",
          "lreg", "greg", "mach"]

# What each probe means, and the source levers that produce the ROM's choice.
# Keep in step with docs/AGBCC_RULES.md.
LEVERS = {
    "notie": (
        "agbcc gave the result of this two-operand instruction the register of an input that dies here "
        "(regmove ties output to input, local-alloc does the same); the ROM did not, so it copies an operand "
        "first (`adds rD, rS, #0`) and keeps the result in its own register.",
        [
            "the tie needs the input to die here: use that input again later (e.g. keep the variable and reuse it "
            "below instead of re-reading memory)",
            "regmove ties only while the result dies before the next branch or label; the result living past "
            "the `if` (used after it) blocks the tie",
            "local-alloc ties only registers used in one basic block; a variable assigned or used in another "
            "block (shared by two blocks, set before an `if`) is left to global-alloc, which does not tie",
            "operand order picks which operand is copied: `a & b` copies `a`; agbcc canonicalises "
            "`const-in-reg & x` so check which one the ROM copies",
        ]),
}


# What it means when an insn first appears in a pass (its trail starts there).
BORN = {
    "loop": "made by loop.c: a loop invariant hoisted out of the loop, or strength reduction's pointer/counter. "
            "Hoisted invariants are emitted just before the loop start, then strength reduction's inits after "
            "them (`for (i = 0; i < N; i++) a[i] = c` becomes `c` in a register, a pointer, and a count-down from "
            "N-1). If the ROM computes something before the loop that you compute inside it (or the reverse), "
            "this is the decision; see docs/AGBCC_RULES.md, loops",
    "gcse": "made by gcse.c (partial redundancy elimination or constant/copy propagation across blocks)",
    "cse2": "made by the second CSE pass (after loop)",
    "combine": "made by combine.c, which merged this insn with the insns that fed it",
    "reload": "made by reload (spill, constant or address reload) after register allocation",
}


def compile_dumps(src, wd):
    i_path = os.path.join(wd, "u.i")
    with open(i_path, "w") as f:
        f.write(build.sh(build.CPP + ["-nostdinc", "-undef"] + build.DEFINES +
                         ["-I", "include", "-iquote", ".", src]))
    r = subprocess.run([INSTR] + build.cflags_for(src) + ["-g", "-dp", "-da", "u.i", "-o", "u.s"],
                       cwd=wd, capture_output=True, text=True)
    if r.returncode:
        sys.exit(r.stderr)
    return {p: regdiag.functions_of(open(os.path.join(wd, "u.i." + p)).read())
            for p in PASSES if os.path.exists(os.path.join(wd, "u.i." + p))}


def one_line(rtl):
    rtl = re.sub(r"\(insn_list.*", "", rtl, flags=re.S)  # drop LOG_LINKS and notes
    rtl = re.sub(r"\(expr_list.*", "", rtl, flags=re.S)
    rtl = re.sub(r"^\((?:insn|jump_insn|call_insn)(?:/\w+)* \d+ \d+ \d+ ", "", rtl)
    rtl = re.sub(r"\s+", " ", rtl).strip()
    rtl = re.sub(r"( \(nil\))+\)?$", "", rtl)
    rtl = re.sub(r" (\d+ \{[^}]*\}|-1)$", "", rtl)
    return rtl


def trail(dumps, func, uid):
    """[(pass, rtl)] where insn UID first appears and each time it changes, then where it vanishes."""
    out, last = [], None
    for p in PASSES:
        text = dumps.get(p, {}).get(func)
        if text is None:
            continue
        insn = regdiag.insns_by_uid(text).get(uid)
        cur = one_line(insn) if insn else None
        if cur != last:
            out.append((p, cur or "(deleted)"))
            last = cur
    return out


def ties(dumps, func):
    """{uid: operand} for regmove's "Fixed operand N of insn U" lines."""
    text = dumps.get("regmove", {}).get(func, "")
    return {int(u): int(n) for n, u in re.findall(r"Fixed operand (\d+) of insn (\d+)", text)}


def probe(src, env, nonmatching):
    """(structural differences, differing bytes) with the instrumented compiler under ENV."""
    e = dict(os.environ, AGBCC=INSTR, **env)
    cmd = [sys.executable, os.path.join(ROOT, "tools", "check.py"), src, "--json", "--quiet"]
    r = subprocess.run(cmd + ["--nonmatching"] * nonmatching, cwd=ROOT, env=e, capture_output=True, text=True)
    try:
        res = json.loads([ln for ln in r.stdout.splitlines() if ln.startswith("{")][-1])
    except (IndexError, ValueError):
        return None
    ad = res.get("anchordiff", {"structural": 0})
    return ad["structural"], res["different_byte_count"]


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("src")
    ap.add_argument("func", nargs="?")
    ap.add_argument("--nonmatching", action="store_true")
    ap.add_argument("--no-probe", action="store_true", help="skip the counterfactual compiles")
    a = ap.parse_args()
    if not os.path.exists(INSTR):
        subprocess.run(["sh", os.path.join(ROOT, "tools", "agbcc-instr.sh")], check=True)
    src = os.path.abspath(a.src)
    an = regdiag.analyze(src, a.func, a.nonmatching)
    func = an.func
    lines = open(src).read().splitlines()

    def show(ln):
        return f"line {ln}: {lines[ln - 1].strip()[:90]}" if ln and 0 < ln <= len(lines) else "line ?"

    with tempfile.TemporaryDirectory() as wd:
        dumps = compile_dumps(src, wd)
    tied = ties(dumps, func)
    base = None if a.no_probe else probe(src, {}, a.nonmatching)
    nreg = sum(1 for x in an.listing if x.startswith("!"))
    print(f"{func}: {len(an.struct_ops)} shape differences, {nreg} instructions with other registers")
    if an.matched:
        print("matches the ROM")
        return

    # index of each candidate instruction, to find the one after a pure insertion
    pos = {o: k for k, (o, _) in enumerate(an.cd)}
    for n, (tag, cand, orig) in enumerate(an.struct_ops, 1):
        uids = [an.by_off[o][0] for o, _ in cand if an.by_off.get(o, (None,))[0]]
        if not cand or tag != "replace":  # the ROM has extra or fewer insns: look at the neighbours too
            k = pos.get(cand[0][0], None) if cand else None
            if k is None:  # an insertion: find where it goes from the ROM side
                k = next((pos[o] for o, _ in an.cd if o >= (orig[0][0] if orig else 0)), None)
            for kk in (k - 1, k, k + 1) if k is not None else ():
                if 0 <= kk < len(an.cd):
                    u = an.by_off.get(an.cd[kk][0], (None,))[0]
                    if u and u not in uids:
                        uids.append(u)
        src_lines = sorted({an.off_line.get(o) for o, _ in cand if an.off_line.get(o)} |
                           {regdiag.insn_lines(dumps.get("lreg", {}).get(func, "")).get(u) for u in uids} - {None})
        print(f"\n== difference {n}")
        for ln in src_lines[:4]:
            print(f"   {show(ln)}")
        print(f"   ROM:   {'; '.join(t for _, t in orig) or '(nothing)'}")
        print(f"   yours: {'; '.join(t for _, t in cand) or '(nothing)'}")
        for u in uids[:4]:
            tr = trail(dumps, func, u)
            if tr:
                print(f"   insn {u}: " + "  ->  ".join(f"[{p}] {r[:110]}" for p, r in tr[-3:]))
                born = tr[0][0]
                if tr[0][0] in ("greg", "mach") and len(tr) == 1:
                    born = "reload"
                if born in BORN:
                    print(f"      ({BORN[born]})")
            if u in tied:
                print(f"   insn {u}: regmove tied the output to operand {tied[u]} (they share a register)")
        if a.no_probe or not uids:
            continue
        got = probe(src, {"AGBCC_NOTIE": ",".join(map(str, uids))}, a.nonmatching)
        if got and base and got[0] < base[0]:
            why, levers = LEVERS["notie"]
            print(f"   PROBE: forbidding output/input ties at insn {','.join(map(str, uids))} removes "
                  f"{base[0] - got[0]} shape difference(s).")
            print(f"   why: {why}")
            for lv in levers:
                print(f"     - {lv}")

    if an.wants:
        print("\nregisters (regdiag): pseudos that got another register than the ROM's")
        for p in sorted(an.wants, key=lambda p: (p not in an.global_order,
                                                 an.global_order.index(p) if p in an.global_order else p)):
            got = f"r{an.disp[p]}" if p in an.disp else "?"
            w = ", ".join(f"{r} x{k}" for r, k in an.wants[p].most_common())
            ls = sorted(an.pseudo_lines.get(p, ()))[:2]
            print(f"  pseudo {p} ({an.kind(p)}) got {got} wants {w}; {an.stats.get(p, '')}")
            for ln in ls:
                print(f"      {show(ln)}")
        print("  (python3 tools/regdiag.py FILE FUNC for the full allocation report; "
              "tools/rasolve.py FILE FUNC solves allocation-only differences inside the compiler)")
    print("\nThe compiler's source is in build/agbcc-instr/gcc (loop.c, cse.c, combine.c, regmove.c, local-alloc.c, "
          "global.c, reload1.c, jump.c, config/arm/thumb.md): read the pass named above to see exactly when it "
          "makes this choice. Known rules: docs/AGBCC_RULES.md.")


if __name__ == "__main__":
    main()
