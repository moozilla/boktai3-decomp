#!/usr/bin/env python3
"""Explain how a C candidate differs from the ROM, by source line.

  explain.py FILE.c [FUNC] [--nonmatching] [--all] [-k N]

Compiles FUNC (default: the last function in FILE.c) through the agbcc fork
server (tools/agbserve.py) with -g, aligns its instructions with the ROM's
(difflib on register- and constant-free shapes), and prints each mismatching
region under the C line(s) that produced it:

  class     register    same instructions, other registers -> regdiag/rasolve
            constant    same instructions, other immediate/offset/pool value
            frame       saved registers or stack size (follow from the rest)
            selection   other instructions for the same work (ldrsh vs
                        ldrh+lsl+asr, add vs sub, mul vs shifts, ...)
            control     branch sense, block order or loop shape
            order       the same instructions in another order
            extra       instructions only yours has
            missing     instructions only the ROM has
  ROM / you the two snippets
  idioms    matched C elsewhere in src/ whose code has the ROM's shape
            (tools/idioms.py; '=N' exact shape seen N times, '~s' nearest)

Exit status 0 when the bytes match. The report stays small (a dozen regions;
--all for every one) so it fits a small model's context.
"""
import argparse
import collections
import difflib
import os
import re
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import agbserve  # noqa: E402
import build  # noqa: E402
import check  # noqa: E402
import idioms  # noqa: E402
import regdiag  # noqa: E402
from romlib import ROOT, load_rom, off  # noqa: E402

REGS = re.compile(r"\b(r1[0-2]|r[0-9]|sb|sl|fp|ip)\b")


# --- compiling the candidate ---------------------------------------------------

def last_function(path):
    text = re.sub(r"/\*.*?\*/|//[^\n]*", "", open(path).read(), flags=re.S)
    text = re.sub(r"^#.*$", "", text, flags=re.M)
    name = None
    for m in re.finditer(r"^[A-Za-z_][\w \*]*?\b(\w+)\s*\([^;{]*\)\s*\{", text, re.M):
        if m.group(1) not in ("if", "for", "while", "switch") and \
                not re.match(r"\s*static\b", text[m.start():m.end()]):
            name = m.group(1)
    return name or os.path.splitext(os.path.basename(path))[0]


def compile_candidate(src, func, defines=()):
    """Assembly (with -g line info) for SRC, through the fork server when
    FUNC is the file's last definition and the instrumented agbcc exists."""
    rel = os.path.relpath(os.path.abspath(src), ROOT)
    text = idioms.preprocess_lines(rel, defines)
    flags = build.cflags_for(rel) + ["-g"]
    parts = agbserve.split_source(text, func)
    if parts and os.path.exists(agbserve.INSTR) and build.compiler_for(rel) == build.AGBCC:
        with agbserve.Server(parts[0], flags) as server:
            return server.compile(parts[1])
    return idioms.compile_g(rel, text)


def candidate_bytes(asm, func, address):
    symbols = check.elf_symbols()
    res = agbserve.Resolver(symbols, address)
    asm = re.sub(r'^\s*\.include\s+"build/asm/[^"]*"\s*$', "", asm, flags=re.M)
    with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as d:
        obj = res.assemble(asm, d)
    if obj is None:
        sys.exit("the candidate assembly does not assemble")
    code, missing = res.bytes_of(obj, func)
    if code is None:
        sys.exit(f"{func} is not defined in the candidate")
    return code, [m for m in missing if not m.startswith("reloc")]


def with_pool_values(ins, code):
    """Rewrite pc-relative loads as `ldr rX, =VALUE` so pool constants compare."""
    out = []
    for o, t in ins:
        m = re.match(r"ldr (r\d), \[pc, #(0x[0-9a-f]+|\d+)\]", t)
        if m:
            a = ((o + 4) & ~3) + int(m.group(2), 0)
            v = int.from_bytes(code[a:a + 4], "little") if a + 4 <= len(code) else None
            t = f"ldr {m.group(1)}, ={v:#x}" if v is not None else t
        out.append((o, t))
    return out


# --- classification ------------------------------------------------------------

def mnems(texts):
    return [t.split()[0] for t in texts]


def strip_regs(t):
    return REGS.sub("R", t)


def classify(rom, you):
    """(class, hint) for a structural region: ROM and candidate instruction
    texts."""
    rm, ym = mnems(rom), mnems(you)
    rs, ys = set(rm), set(ym)
    joined_r, joined_y = " ".join(rom), " ".join(you)

    def has_ext(texts, amount):
        return any(re.match(rf"(lsls|asrs|lsrs) \w+, \w+, #{amount}$", t) for t in texts)

    for wide, amt, signed in (("h", "0x10", "s16"), ("b", "0x18", "s8")):
        if f"ldrs{wide}" in rs and f"ldrs{wide}" not in ys and has_ext(you, amt):
            return "selection", (f"ROM loads signed (ldrs{wide}); you load unsigned and shift: "
                                 f"read the field as {signed}, not u{signed[1:]} cast to {signed}")
        if f"ldrs{wide}" in ys and f"ldrs{wide}" not in rs and has_ext(rom, amt):
            return "selection", (f"ROM loads unsigned then sign-extends (lsl/asr #{amt}); you use ldrs{wide}: "
                                 f"the value is u{signed[1:]}/int converted to {signed} after the load")
    for amt, width in (("0x10", "u16"), ("0x18", "u8")):
        rz = any(re.match(rf"lsrs \w+, \w+, #{amt}$", t) for t in rom) and has_ext(rom, amt)
        yz = any(re.match(rf"lsrs \w+, \w+, #{amt}$", t) for t in you) and has_ext(you, amt)
        if rz != yz:
            who = "ROM" if rz else "you"
            return "selection", (f"{who} truncate(s) to {width} (lsl/lsr #{amt}): a {width} local, "
                                 f"parameter or return type on one side only")
        ra = any(re.match(rf"asrs \w+, \w+, #{amt}$", t) for t in rom) and has_ext(rom, amt)
        ya = any(re.match(rf"asrs \w+, \w+, #{amt}$", t) for t in you) and has_ext(you, amt)
        if ra != ya:
            who = "ROM" if ra else "you"
            return "selection", (f"{who} sign-extend(s) to s{width[1:]} (lsl/asr #{amt}): an s{width[1:]} "
                                 f"local, parameter or return type on one side only")
    if ("muls" in rs) != ("muls" in ys):
        who = "ROM" if "muls" in rs else "you"
        return "selection", (f"{who} multiply (muls) where the other uses shifts/adds: element size or a "
                             f"constant factor differs, or the product is computed in another type")
    extra_r = collections.Counter(map(strip_regs, rom)) - collections.Counter(map(strip_regs, you))
    extra_y = collections.Counter(map(strip_regs, you)) - collections.Counter(map(strip_regs, rom))
    copy = re.compile(r"^(adds R, R, #0|mov R, R|movs R, R)$")
    if (extra_r or extra_y) and all(copy.match(t) for t in list(extra_r) + list(extra_y)):
        who = "ROM" if extra_r and not extra_y else "you" if extra_y and not extra_r else "both"
        return "selection", (f"register copy only ({who}: adds rX, rY, #0 / mov): a value moved to another "
                             f"register; a separate variable, a temporary, or a different expression order")
    if extra_y and not extra_r:
        return "extra", "only yours has: " + "; ".join(sorted(extra_y.elements())[:4])
    if extra_r and not extra_y:
        return "missing", "only the ROM has: " + "; ".join(sorted(extra_r.elements())[:4])
    rb = [m for m in rm if idioms.BRANCH.match(m) and m != "b"]
    yb = [m for m in ym if idioms.BRANCH.match(m) and m != "b"]
    if rb and yb and rb != yb and len(rb) == len(yb):
        return "control", (f"branch sense: ROM {'/'.join(rb)}, you {'/'.join(yb)}: negate the condition "
                           f"and swap the if/else bodies, or use the opposite comparison (< vs >=)")
    if rm.count("b") != ym.count("b") or len(rb) != len(yb):
        return "control", ("block layout differs (unconditional jumps or conditions): try while vs "
                           "do-while vs for, if/else order, or an early return vs a goto to the end")
    if sorted(map(strip_regs, rom)) == sorted(map(strip_regs, you)):
        return "order", ("same instructions, other order: statement order, or which value is computed "
                         "first in an expression (a temporary changes it)")
    if rs ^ ys and rs ^ ys <= {"adds", "subs"}:
        return "selection", "add vs sub: the sign of a constant (x - k vs x + (-k)) or pointer vs index arithmetic"
    if (rs ^ ys) & {"push", "pop"}:
        return "selection", "prologue/epilogue differ: a different number of values live across calls"
    if any(t.startswith("ldr") and "=" in t for t in rom) != any(t.startswith("ldr") and "=" in t for t in you):
        return "selection", ("constant materialization: one side loads a pool word, the other builds the "
                             "value with mov/lsl/add (different value or the compiler reuses a register)")
    if not you:
        return "missing", "the ROM has code your C does not generate here"
    if not rom:
        return "extra", "your C generates code the ROM does not have here"
    hi_r = sum(bool(re.search(r"\b(r8|r9|r1[0-2]|sb|sl|fp|ip)\b", t)) for t in rom)
    hi_y = sum(bool(re.search(r"\b(r8|r9|r1[0-2]|sb|sl|fp|ip)\b", t)) for t in you)
    if hi_r != hi_y:
        return "selection", ("high-register moves differ: register pressure (one more or fewer value "
                             "live here, e.g. a temporary kept or recomputed)")
    if rm == ym:
        if any("sp" in t for t in rom + you):
            return "selection", ("same operations, other stack slots: the frame layout differs (another set "
                                 "of variables spilled or a local array/struct of another size)")
        return "selection", "same operations, other operands (immediates or operand order)"
    if len(rom) != len(you):
        return "selection", f"ROM uses {len(rom)} instructions, you {len(you)}"
    return "selection", "different instructions for this code"


# --- alignment ---------------------------------------------------------------------

class Region:
    """Half-open spans [r0, r1) of ROM and [y0, y1) of candidate instructions."""

    def __init__(self, kind, r0, r1, y0, y1):
        self.kind, self.r0, self.r1, self.y0, self.y1 = kind, r0, r1, y0, y1

    @property
    def r(self):
        return list(range(self.r0, self.r1))

    @property
    def y(self):
        return list(range(self.y0, self.y1))


def align(rom, you):
    """[Region] covering every difference between the instruction lists."""
    a = [idioms.shape(t) for _, t in rom]
    b = [idioms.shape(t) for _, t in you]
    sm = difflib.SequenceMatcher(None, a, b, autojunk=False)
    raw = []
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag != "equal":
            raw.append(Region("struct", i1, i2, j1, j2))
            continue
        for k in range(i2 - i1):
            rt, yt = idioms.exact(rom[i1 + k][1]), idioms.exact(you[j1 + k][1])
            if rt != yt:
                kind = "register" if strip_regs(rt) == strip_regs(yt) else "constant"
                raw.append(Region(kind, i1 + k, i1 + k + 1, j1 + k, j1 + k + 1))
    out = []
    for g in raw:
        p = out[-1] if out else None
        near = p and g.r0 - p.r1 <= 1 and g.y0 - p.y1 <= 1
        if near and (p.kind == g.kind or ("struct" in (p.kind, g.kind) and g.r0 - p.r1 + g.y0 - p.y1 <= 1)):
            p.kind = "struct" if "struct" in (p.kind, g.kind) else p.kind
            p.r1, p.y1 = max(p.r1, g.r1), max(p.y1, g.y1)
            continue
        out.append(g)
    return out


# --- report -------------------------------------------------------------------------

def snippet(ins, lo, hi, limit=6):
    """The differing instructions [lo, hi) in brackets, one neighbour each side."""
    body = [t for _, t in ins[lo:hi]]
    if len(body) > limit:
        body = body[:limit] + [f"... (+{hi - lo - limit})"]
    parts = ([ins[lo - 1][1]] if lo > 0 else []) + ["[" + "; ".join(body) + "]"] + \
        ([ins[hi][1]] if hi < len(ins) else [])
    return "; ".join(parts)


def reg_map(rt, yt):
    pairs = []
    for a, b in zip(rt, yt):
        for x, y in zip(regdiag.norm_regs(a), regdiag.norm_regs(b)):
            if x != y and (x, y) not in pairs:
                pairs.append((x, y))
    return ", ".join(f"ROM {x} = your {y}" for x, y in pairs[:4])


def explain(src, func=None, nonmatching=False, k=3, show_all=False, corpus=None, out=print):
    func = func or last_function(src)
    name, address, size = idioms.resolve_function(func)
    asm = compile_candidate(src, func, ["-DNONMATCHING"] if nonmatching else [])
    if asm is None:
        out(f"{func}: compile error (run tools/check.py for the compiler message)")
        return 2
    lines = idioms.functions_in_asm(asm).get(func)
    if not lines:
        out(f"{func}: not found in the compiled assembly")
        return 2
    code, missing = candidate_bytes(asm, func, address)
    rom_code = load_rom(verify=False)[off(address):off(address) + size]
    end = lines[-1][0] + lines[-1][1]
    cand_len = len(code)
    # the candidate's own extent: through its pool, at most up to the next label
    mine = code[:max(end, min(cand_len, size))]
    if mine[:size] == rom_code and len(mine) >= size:
        out(f"{func}: MATCH ({size} bytes)")
        return 0
    rom = idioms.name_calls(with_pool_values(
        idioms.disasm_function(load_rom(verify=False), address, size), rom_code), address)
    while rom and rom[-1][1] == "movs r0, r0":  # alignment padding
        rom.pop()
    dis = dict(idioms.disasm(code[:end], 0))
    you = idioms.name_calls(with_pool_values([(o, dis.get(o, "?")) for o, _, _, _ in lines], code), address)
    where = [(f, ln) for _, _, f, ln in lines]
    regions = align(rom, you)
    if not regions:
        out(f"{func}: instructions agree but {sum(x != y for x, y in zip(rom_code, mine))} bytes differ "
            f"(sizes ROM {size}, yours {len(mine)}): check pool order and padding with tools/check.py")
        return 1
    corpus = corpus or idioms.Corpus.load()
    counts = collections.Counter(r.kind if r.kind != "struct" else "structural" for r in regions)
    out(f"{func} @{address:08X}: ROM {len(rom)} insns, yours {len(you)}; "
        + ", ".join(f"{n} {c}" for c, n in counts.most_common()) + " region(s)"
        + (f"; unresolved {' '.join(missing[:3])}" if missing else ""))

    def line_of(g):
        if g.y:
            ls = sorted({where[j] for j in g.y if where[j][1]}, key=lambda x: x[1])
            real = [x for x in ls if idioms.source_line(*x) not in ("{", "}", "")]
            if real or ls:
                return real or ls
        j = min(g.y0, len(where) - 1)
        for jj in list(range(j, -1, -1)) + list(range(j, len(where))):
            if where[jj][1]:
                return [where[jj]]
        return [(None, 0)]

    items = []
    for g in regions:
        ls = line_of(g)
        rt = [rom[i][1] for i in g.r]
        yt = [you[j][1] for j in g.y]
        if g.kind == "struct":
            cls, hint = classify(rt, yt)
        elif any(t.startswith(("push", "pop")) for t in rt + yt):
            cls, hint = "frame", ("saved registers differ (ROM " + " ".join(t for t in rt if t[:3] in ("pus", "pop"))
                                  + "): a different number of values live across calls; fix the "
                                  "structural regions first")
        elif any(re.match(r"(add|sub) sp, #", t) for t in rt + yt):
            cls, hint = "frame", "stack frame size differs: locals, arrays or spills of another size"
        elif g.kind == "register":
            cls, hint = "register", reg_map(rt, yt)
        else:
            cls, hint = "constant", ("struct offset or element size" if any("[" in t for t in rt + yt)
                                     else "a literal, comparison bound or pool value")
        items.append((ls[0][1] or 0, ls, cls, hint, g, rt, yt))
    items.sort(key=lambda x: (x[0], x[4].r0))
    structural = [x for x in items if x[2] != "register"]
    regs = [x for x in items if x[2] == "register"]
    shown = items if show_all else structural[:12]
    for _, ls, cls, hint, g, rt, yt in shown:
        out("")
        for f, ln in ls[:3]:
            text = idioms.source_line(f, ln) if f else ""
            tag = f"{ln}" if f and os.path.abspath(os.path.join(ROOT, f)) == os.path.abspath(src) \
                else f"{f}:{ln}"
            out(f"L{tag}: {text}")
        out(f"  {cls}: {hint}")
        out(f"  ROM: {snippet(rom, g.r0, g.r1)}")
        out(f"  you: {snippet(you, g.y0, g.y1)}")
        if cls in ("register", "frame"):
            continue
        # the ROM's shape, with context so short regions still say something
        i0, i1 = g.r0, g.r1
        while i1 - i0 < 6 and (i0 > 0 or i1 < len(rom)):
            i0, i1 = max(0, i0 - 1), min(len(rom), i1 + 1)
        hits = corpus.lookup([idioms.shape(rom[i][1]) for i in range(i0, i1)], k, min_sim=0.75) if k else []
        if hits:
            out("  idioms for the ROM shape:")
            out("\n".join(idioms.format_hits(hits, "    ")))
    if regs and not show_all:
        out("")
        by_line = collections.OrderedDict()
        for _, ls, _, hint, g, _, _ in regs:
            by_line.setdefault(f"L{ls[0][1]}", hint)
        listed = list(by_line.items())
        out(f"register-only ({len(regs)} regions): " + "; ".join(f"{l} {h}" for l, h in listed[:6])
            + (" ..." if len(listed) > 6 else ""))
    hidden = len(items) - len(shown) - (len(regs) if not show_all else 0)
    if hidden:
        out(f"\n(+{hidden} more regions; --all shows them)")
    if counts.get("register") and len(counts) == 1:
        out("Only registers differ: tools/regdiag.py and tools/rasolve.py (docs/PERMUTER.md).")
    elif regs:
        out("Fix the structural regions first; register differences often move with them.")
    return 1


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("src")
    ap.add_argument("func", nargs="?")
    ap.add_argument("--nonmatching", action="store_true", help="compile #ifdef NONMATCHING drafts")
    ap.add_argument("--all", action="store_true", help="every region, not the first dozen")
    ap.add_argument("-k", type=int, default=3, help="idioms per region (0: none)")
    a = ap.parse_args()
    sys.exit(explain(a.src, a.func, a.nonmatching, a.k, a.all))


if __name__ == "__main__":
    main()
