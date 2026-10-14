#!/usr/bin/env python3
"""Explain register-allocation differences between a C function and the ROM.

  regdiag.py FILE.c [FUNC] [--asm]

Compiles FILE.c as build.py does, plus agbcc's `-dp -dl -dg` dumps, aligns the
candidate instructions with the original, and for each instruction whose
registers differ says which allocator chose the register:

* local  - local-alloc (pseudo used in one basic block);
* global - global-alloc (pseudo live across blocks); listed in priority order;
* reload - a register reload chose (insn created by reload: constants folded
           into an address or a large add operand). Reload rotates through
           the spill registers in instruction order (reload1.c,
           allocate_reload_reg), so an extra or missing reload earlier in the
           function shifts every later reload register.

Per pseudo it prints the register it got, the register the original wants
there, and agbcc's own statistics (uses, live length, calls crossed), which is
what local/global allocation priorities are computed from. See
docs/REGALLOC.md for how to act on the report.
"""
import argparse
import collections
import difflib
import os
import re
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import build  # noqa: E402
import check  # noqa: E402
from romlib import ROOT, load_rom, off  # noqa: E402

REG = re.compile(r"\b(r\d+|sb|sl|fp|ip|sp|lr|pc)\b")
ALIAS = {"sb": "r9", "sl": "r10", "fp": "r11", "ip": "r12"}


def norm_regs(text):
    return [ALIAS.get(r, r) for r in REG.findall(text)]


def shape(text):
    """Instruction without registers or pc-relative offsets, for alignment."""
    t = REG.sub("R", text)
    t = re.sub(r"\[R, #0x[0-9a-f]+\]$", "[R, #X]", t) if t.startswith("ldr R, [R") and "pc" in text else t
    t = re.sub(r"#0x[0-9a-f]{5,}", "#ADDR", t)  # branch targets
    return t.replace("adds", "add").replace("subs", "sub").replace("movs", "mov")


def functions_of(dump):
    """{function name: dump text} from an agbcc RTL dump."""
    out, cur, buf = {}, None, []
    for ln in dump.splitlines(keepends=True):
        m = re.match(r";; Function (\S+)", ln)
        if m:
            if cur:
                out[cur] = "".join(buf)
            cur, buf = m.group(1), []
        buf.append(ln)
    if cur:
        out[cur] = "".join(buf)
    return out


def insns_by_uid(dump):
    """{uid: rtl text} for insn/call_insn/jump_insn expressions."""
    out = {}
    for m in re.finditer(r"^\((insn|call_insn|jump_insn) (\d+) .*?(?=^\(|^;;|\Z)", dump, re.S | re.M):
        out[int(m.group(2))] = m.group(0)
    return out


def insn_lines(dump):
    """{uid: source line} from the line-number notes preceding each insn."""
    out, line = {}, None
    for m in re.finditer(r'^\(note \d+ \d+ \d+ \("[^"]*"\) (\d+)\)|^\((?:insn|call_insn|jump_insn) (\d+) ', dump, re.M):
        if m.group(1):
            line = int(m.group(1))
        elif line is not None:
            out[int(m.group(2))] = line
    return out


def regs_in(rtl):
    return [(int(n), name) for n, name in re.findall(r"\(reg(?:/[a-z]+)*:[A-Z]+ (\d+)(?: (\w+))?\)", rtl)]


def compile_dumps(src, workdir):
    # keep line markers (no -P) and compile with -g so the dumps carry source
    # line notes; -g only renumbers labels, which compile_dumps verifies
    i_path = os.path.join(workdir, "u.i")
    with open(i_path, "w") as f:
        f.write(build.sh(build.CPP + ["-nostdinc", "-undef"] + build.DEFINES +
                         ["-I", "include", "-iquote", ".", src]))
    cc = build.compiler_for(src)
    flags = build.cflags_for(src)
    s_path = os.path.join(workdir, "u.s")
    r = subprocess.run([cc] + flags + ["-g", "-dp", "-dl", "-dg", i_path, "-o", s_path],
                       cwd=ROOT, capture_output=True, text=True)
    if r.returncode:
        sys.exit(r.stderr)
    o_path = os.path.join(workdir, "u.o")
    build.sh(build.AS + ["-o", o_path, s_path])
    # reference object exactly as build.py compiles it
    p_path = os.path.join(workdir, "p.i")
    with open(p_path, "w") as f:
        f.write(build.preprocess(src))
    ref_s, ref_o = os.path.join(workdir, "ref.s"), os.path.join(workdir, "ref.o")
    build.sh([cc] + flags + [p_path, "-o", ref_s])
    build.sh(build.AS + ["-o", ref_o, ref_s])
    return s_path, o_path, i_path + ".lreg", i_path + ".greg", ref_o


def asm_lines(s_path, func):
    """[(offset, uid or None, text)] for FUNC's instructions in the -dp asm."""
    out, offset, inside = [], 0, False
    for ln in open(s_path):
        if re.match(rf"{re.escape(func)}:\s*$", ln):
            inside, offset = True, 0
            continue
        if not inside:
            continue
        if re.match(r"\s*\.(size|end)\b", ln) or (re.match(r"\w+:\s*$", ln) and not ln.startswith(".")):
            if out:
                break
        body = ln.split("@", 1)[0].strip()
        if not body or body.endswith(":"):
            continue
        if body.startswith(".align"):
            n = 1 << int(body.split()[1].rstrip(","))
            offset = (offset + n - 1) // n * n
            continue
        if body.startswith((".word", ".4byte", ".long")):
            offset += 4
            continue
        if body.startswith((".short", ".hword", ".2byte")):
            offset += 2
            continue
        if body.startswith("."):
            if body.startswith(("thumb_func", ".code", ".thumb", ".globl", ".type", ".text", ".section")):
                continue
            continue
        m = re.search(r"@ (\d+)\t", ln)
        uid = int(m.group(1)) if m else None
        out.append((offset, uid, body))
        offset += 4 if body.split()[0] in ("bl", "blx") else 2
    return out


def strip_pools(dis, limit):
    """Drop literal-pool words (targets of pc-relative loads) and their padding."""
    data = set()
    for o, t in dis:  # pools follow their loads, so skip loads already known to be data
        m = re.match(r"ldr r\d+, \[pc, #(0x[0-9a-f]+|\d+)\]", t)
        if m and o not in data:
            target = ((o + 4) & ~3) + int(m.group(1), 0)
            data.update((target, target + 2))
    out = []
    for o, t in dis:
        if o in data or o >= limit:
            continue
        if t == "movs r0, r0" and o % 4 == 2 and o + 2 in data:
            continue
        out.append((o, t))
    return out


def object_function(o_path, func):
    out = build.sh(["arm-none-eabi-readelf", "-sW", o_path])
    syms = []
    for ln in out.splitlines():
        p = ln.split()
        if len(p) == 8 and p[0][:-1].isdigit() and p[3] == "FUNC":
            syms.append((int(p[1], 16) & ~1, int(p[2]), p[7]))
    bin_path = o_path + ".bin"
    build.sh(["arm-none-eabi-objcopy", "-O", "binary", "-j", ".text", o_path, bin_path])
    text = open(bin_path, "rb").read()
    starts = sorted(s for s, _, _ in syms)
    for start, size, name in syms:
        if name == func:
            if not size:
                later = [s for s in starts if s > start]
                size = (later[0] if later else len(text)) - start
            return start, text[start:start + size]
    sys.exit(f"{func} not in {o_path}")


def first_function(src):
    text = re.sub(r"/\*.*?\*/|//[^\n]*", "", open(src).read(), flags=re.S)
    for m in re.finditer(r"^(static\s+)?[A-Za-z_][\w \*]*?\b(\w+)\s*\([^;{]*\)\s*\{", text, re.M):
        if not m.group(1):
            return m.group(2)
    return os.path.splitext(os.path.basename(src))[0]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("func", nargs="?")
    ap.add_argument("--asm", action="store_true", help="print the aligned listing")
    ap.add_argument("--nonmatching", action="store_true", help="compile #ifdef NONMATCHING drafts")
    a = ap.parse_args()
    if a.nonmatching:
        build.DEFINES.append("-DNONMATCHING")
    src = a.src if os.path.exists(a.src) else os.path.join(ROOT, "src/fn", a.src + ".c")
    func = a.func or first_function(src)

    with tempfile.TemporaryDirectory() as wd:
        s_path, o_path, lreg_p, greg_p, ref_o = compile_dumps(src, wd)
        lines = asm_lines(s_path, func)
        start, cand = object_function(o_path, func)
        if object_function(ref_o, func)[1] != cand:
            print("warning: -g changed the code; insn numbers may not line up")
        lreg = functions_of(open(lreg_p).read()).get(func, "")
        greg = functions_of(open(greg_p).read()).get(func, "")

    syms = check.elf_symbols()
    if func not in syms:
        sys.exit(f"{func} not in build/boktai3.elf (run make)")
    addr = syms[func][0] & ~1
    rom = load_rom()
    orig = rom[off(addr):off(addr) + len(cand) + 64]

    by_off = {o: (uid, t) for o, uid, t in lines}
    cd = [(x - addr, t) for x, t in check.disasm(cand, addr)]
    od = [(x - addr, t) for x, t in check.disasm(orig, addr)]
    # only instructions (drop pool words the walker knows about)
    cd = [(o, t) for o, t in cd if o in by_off]
    od = strip_pools(od, len(cand))
    sm = difflib.SequenceMatcher(None, [shape(t) for _, t in cd], [shape(t) for _, t in od], autojunk=False)

    l_insns, g_insns = insns_by_uid(lreg), insns_by_uid(greg)
    uid_line = insn_lines(lreg)
    src_text = open(src).read().splitlines()
    pseudo_lines = collections.defaultdict(set)
    for uid, rtl in l_insns.items():
        for pn, _ in regs_in(rtl):
            if pn >= 16 and uid in uid_line:
                pseudo_lines[pn].add(uid_line[uid])
    # reload insns have no note of their own: use the next ordinary insn's line
    off_line, nxt = {}, None
    for o, uid, _ in reversed(lines):
        if uid in uid_line:
            nxt = uid_line[uid]
        off_line[o] = nxt

    def show_lines(ls, limit=4):
        out = []
        for ln in sorted(ls)[:limit]:
            text = src_text[ln - 1].strip() if 0 < ln <= len(src_text) else ""
            out.append(f"      {ln:4d}: {text[:90]}")
        return "\n".join(out)
    max_uid = max(l_insns) if l_insns else 0
    global_order = []
    m = re.search(r";; \d+ regs to allocate:([^\n]*)", greg)
    if m:
        global_order = [int(x) for x in m.group(1).split()]
    stats = dict(re.findall(r"^Register (\d+) (used .*)$", lreg, re.M))
    disp = {int(p): int(h) for p, h in re.findall(r"(\d+) in (\d+)", greg.split(";; Register dispositions:")[-1])} \
        if ";; Register dispositions:" in greg else {}

    wants = collections.defaultdict(collections.Counter)
    reloads = []
    listing = []
    struct_ops = []
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag != "equal":
            struct_ops.append((tag, cd[i1:i2], od[j1:j2]))
            for o, t in cd[i1:i2]:
                listing.append(f"  - {o:04X} {t}")
            for o, t in od[j1:j2]:
                listing.append(f"  + {o:04X} {t}")
            continue
        for (co, ct), (oo, ot) in zip(cd[i1:i2], od[j1:j2]):
            cr, orr = norm_regs(ct), norm_regs(ot)
            uid = by_off[co][0]
            mark = " "
            if cr != orr and len(cr) == len(orr):
                mark = "!"
                pairs = {(x, y) for x, y in zip(cr, orr) if x != y}
                if uid and uid > max_uid:
                    reloads.append((co, uid, ct, ot))
                elif uid in l_insns and uid in g_insns:
                    lp, gp = regs_in(l_insns[uid]), regs_in(g_insns[uid])
                    for (pn, _), (hn, hname) in zip(lp, gp):
                        if pn >= 16:  # a pseudo
                            for x, y in pairs:
                                if hname == x or f"r{hn}" == x:
                                    wants[pn][y] += 1
            listing.append(f"{mark} {co:04X} {ct:28s} | {ot:28s} insn {uid} line {off_line.get(co)}")

    def kind(p):
        return f"global #{global_order.index(p) + 1}" if p in global_order else "local"

    print(f"{func}: {len(cd)} candidate instructions, "
          f"{sum(1 for x in listing if x.startswith('!'))} register diffs, "
          f"{len(struct_ops)} structural diffs")
    if a.asm:
        print("\n".join(listing))
    elif struct_ops:
        print("\nstructural (instruction) differences, - candidate / + original:")
        for tag, c, o in struct_ops[:12]:
            for x, t in c:
                print(f"  - {x:04X} {t}")
            for x, t in o:
                print(f"  + {x:04X} {t}")
            print("  --")
    if wants:
        print("\npseudos that got a different register than the original uses:")
        for p in sorted(wants, key=lambda p: (p not in global_order, global_order.index(p) if p in global_order else p)):
            got = f"r{disp[p]}" if p in disp else "?"
            w = ", ".join(f"{r} x{n}" for r, n in wants[p].most_common())
            print(f"  pseudo {p:4d} {kind(p):10s} got {got:4s} wants {w:14s} {stats.get(str(p), '')}")
            if pseudo_lines.get(p):
                print(show_lines(pseudo_lines[p]))
    if reloads:
        print("\nreload registers (spill rotation; earlier reloads shift later ones):")
        for co, uid, ct, ot in reloads:
            print(f"  {co:04X} insn {uid}: {ct}  | original {ot}")
            if off_line.get(co):
                print(show_lines({off_line[co]}))
        rel = [(o, u, t) for o, u, t in lines if u and u > max_uid]
        print("  all reload insns in order: " + "; ".join(f"{o:04X} {t}" for o, u, t in rel[:30]))
    if global_order and wants:
        print("\nglobal allocation order (priority, highest first), got -> wants:")
        for i, p in enumerate(global_order, 1):
            w = wants.get(p)
            got = f"r{disp[p]}" if p in disp else "?"
            flag = f" -> {w.most_common(1)[0][0]}" if w else ""
            print(f"  #{i:2d} pseudo {p:4d} {got}{flag:7s} {stats.get(str(p), '')}")


if __name__ == "__main__":
    main()
