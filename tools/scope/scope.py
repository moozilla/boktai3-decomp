#!/usr/bin/env python3
"""Look inside agbcc at one draft: what really differs from the ROM, and where it came from.

  scope.py FILE.c [FUNC]        compile the draft, align it with the ROM, print the real differences
  scope.py FILE.c KNOB=V ...    the same with instrumented-compiler knobs set (a what-if)
  scope.py insn UID [FUNC]      one RTL insn through every pass (from the last compile of FUNC)

One compile with the instrumented agbcc (-g -dp -da) gives the code, the C line of every
instruction (DWARF), the RTL insn behind it (-dp) and the RTL after every pass (-da). Both
sides are disassembled the same way; literal loads compare by value and branches by where
they land after alignment, so offsets that only moved because code shifted are hidden and
counted rather than listed. Each differing draft instruction is tagged with its C line,
its insn uid and the pass that created that insn.

Everything is kept in build/scope/FUNC/ for follow-up queries:
  diff.txt    the whole aligned listing     diff.json  the same rows, for jq
  u.s         agbcc output with insn uids   u.i.PASS   RTL after each pass
  stderr.txt  the compiler's stderr (knob traces such as CSE_DEBUG go here)
"""
import difflib
import json
import os
import re
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.dirname(HERE)
try:
    import capstone
except ImportError:  # the project venv has it
    venv = os.path.join(os.path.dirname(TOOLS), "build", "venv", "bin", "python")
    if os.path.exists(venv) and not os.environ.get("SCOPE_REEXEC"):
        os.environ["SCOPE_REEXEC"] = "1"
        os.execv(venv, [venv] + sys.argv)
    sys.exit("scope.py needs capstone (build/venv)")

sys.path.insert(0, TOOLS)
import build  # noqa: E402
import check  # noqa: E402
from romlib import ROOT, load_rom, off  # noqa: E402

INSTR = os.path.join(ROOT, "build", "agbcc-instr", "gcc", "agbcc")
PASSES = ["rtl", "jump", "cse", "addressof", "gcse", "loop", "cse2", "flow", "combine", "regmove",
          "lreg", "greg", "jump2", "mach"]
PASS_NOTE = {"greg": "greg/reload", "mach": "mach (late split/reorg)"}
CS = capstone.Cs(capstone.CS_ARCH_ARM, capstone.CS_MODE_THUMB)


# ---------------------------------------------------------------- compile

def workdir(func):
    d = os.path.join(ROOT, "build", "scope", func)
    os.makedirs(d, exist_ok=True)
    return d


def nonmatching_func(src):
    """The function a draft file is about: the first definition inside #ifdef NONMATCHING."""
    text = open(os.path.join(ROOT, src)).read()
    m = re.search(r"#ifdef NONMATCHING(.*?)#else", text, re.S)
    if not m:
        return None
    defs = re.findall(r"^[A-Za-z_][\w \*]*?\b(\w+)\s*\([^;{]*\)\s*\{", m.group(1), re.M)
    defs = [d for d in defs if not re.search(r"\binline\b.*\b" + d + r"\b", m.group(1))]
    return defs[-1] if defs else None


def rom_name(src, func):
    """The ROM-side name of the draft: its INCLUDE_ASM in the #else branch (names may lag the C)."""
    _, addr, _ = check.inventory()
    if func in addr:
        return func
    text = open(os.path.join(ROOT, src)).read()
    m = re.search(r"#ifdef NONMATCHING.*?#else(.*?)#endif", text, re.S)
    names = re.findall(r'INCLUDE_ASM\(\s*"[^"]*"\s*,\s*(\w+)\s*\)', m.group(1) if m else "")
    return next((n for n in names if n in addr), func)


def compile_draft(src, func, env=None):
    """Compile with the instrumented agbcc; return (bytes, address, info) for FUNC.
    ENV adds knob settings (CSE_NOCANON=N, AGBCC_FORCE=..., see tools/agbcc-instr.sh)."""
    wd = workdir(func)
    defines = build.DEFINES + ["-DNONMATCHING"]
    pre = build.sh(build.CPP + ["-nostdinc", "-undef"] + defines + ["-I", "include", "-iquote", ".", src])
    open(os.path.join(wd, "u.i"), "w").write(pre)
    for p in PASSES:
        try:
            os.remove(os.path.join(wd, "u.i." + p))
        except FileNotFoundError:
            pass
    r = subprocess.run([INSTR] + build.cflags_for(src) + ["-g", "-dp", "-da", "u.i", "-o", "u.s"],
                       cwd=wd, capture_output=True, text=True, env=dict(os.environ, **(env or {})))
    open(os.path.join(wd, "stderr.txt"), "w").write(r.stderr)
    if r.returncode or "error" in r.stderr:
        sys.exit(r.stderr.strip())
    asm = open(os.path.join(wd, "u.s")).read()
    text = re.sub(r"^(\w+):\n", lambda m: f"{m.group(1)}:\n\t.global {m.group(1)}__addr\n{m.group(1)}__addr:\n"
                  if not m.group(1).startswith(".") else m.group(0), asm, flags=re.M)
    open(os.path.join(wd, "a.s"), "w").write(text + "\t.text\n\t.align\t2, 0\n")
    rel = os.path.relpath(wd, ROOT)
    build.sh(build.AS + ["-o", f"{rel}/u.o", f"{rel}/a.s"])
    check.link_object(f"{rel}/u.o", f"{rel}/u.elf")
    build.sh(["arm-none-eabi-objcopy", "-O", "binary", "-j", ".text", f"{rel}/u.elf", f"{rel}/u.bin"])
    whole = open(os.path.join(wd, "u.bin"), "rb").read()
    syms = sorted((v, n) for n, (v, t) in symtab(os.path.join(wd, "u.o")).items() if t == "FUNC")
    start = next(v for v, n in syms if n == func) & ~1
    end = next((v & ~1 for v, n in syms if (v & ~1) > start), len(whole))
    base = syms[0][0] & ~1
    _, addr, _ = check.inventory()
    a = addr[rom_name(src, func)]
    return whole[start - base:end - base], a, {"obj_offset": start, "wd": wd, "env": env or {}}


def symtab(obj):
    out = {}
    for ln in build.sh(["arm-none-eabi-readelf", "-sW", obj]).splitlines():
        p = ln.split()
        if len(p) == 8 and p[0][:-1].isdigit() and p[6] != "UND":
            out.setdefault(p[7], (int(p[1], 16), p[3]))
    return out


def mapping_ranges(obj, start, size):
    """Data ranges of the draft's code, from the assembler's $d/$t mapping symbols."""
    marks = []
    for ln in build.sh(["arm-none-eabi-readelf", "-sW", obj]).splitlines():
        p = ln.split()
        if len(p) == 8 and p[7] in ("$d", "$t") and p[6] != "UND":
            marks.append((int(p[1], 16), p[7]))
    marks.sort()
    data, cur, at = [], "$t", start
    for v, k in marks + [(start + size, "$end")]:
        if v <= start:
            cur = k
            continue
        if v > start + size:
            v = start + size
        if cur == "$d" and v > at:
            data.append((at - start, v - start))
        at, cur = v, k
        if v >= start + size:
            break
    return data


def rom_data_ranges(func, addr):
    """Data ranges of the ROM function, from its generated assembly (labels carry addresses)."""
    path = os.path.join(ROOT, "build", "asm", "nonmatching", func + ".s")
    if not os.path.exists(path):
        return None
    data, cur = [], addr
    for ln in open(path):
        s = ln.split("@")[0].strip()
        m = re.match(r"(\w+):\s*(.*)", s)
        if m:
            lab = re.search(r"_([0-9A-F]{8})$", m.group(1))
            if lab:
                cur = int(lab.group(1), 16)
            s = m.group(2).strip()
        if not s or s.startswith((".global", ".syntax", "thumb_func", "arm_func", ".thumb", ".type", ".size")):
            continue
        if s.startswith(".4byte") or s.startswith(".word"):
            data.append((cur - addr, cur - addr + 4)); cur += 4
        elif s.startswith(".2byte") or s.startswith(".hword"):
            data.append((cur - addr, cur - addr + 2)); cur += 2
        elif s.startswith(".byte"):
            data.append((cur - addr, cur - addr + 1)); cur += 1
        elif s.startswith(".align"):
            n = 1 << int(s.split()[1].rstrip(","))
            pad = (-cur) % n
            if pad:
                data.append((cur - addr, cur - addr + pad))
            cur += pad
        elif s.startswith("."):
            continue
        else:
            cur += 4 if re.match(r"bl\b", s) else 2
    return data


def rom_pool_ranges(code, addr):
    """Fallback: literal-pool words found from pc-relative loads."""
    data = set()
    for i in range(0, len(code) - 1, 2):
        h = int.from_bytes(code[i:i + 2], "little")
        if h & 0xF800 == 0x4800:
            t = ((addr + i + 4) & ~3) + (h & 0xFF) * 4 - addr
            if 0 <= t < len(code):
                data.add(t)
    return sorted((t, t + 4) for t in data)


# ---------------------------------------------------------------- decode

def decode(code, addr, data):
    """[(offset, size, text, key, target_offset)]; data words decode as .word."""
    rows, i = [], 0
    data = sorted(data)
    lo, hi = addr, addr + len(code)
    while i < len(code):
        d = next(((a, b) for a, b in data if a <= i < b), None)
        if d:
            n = min(4, d[1] - i, 4 - (addr + i) % 4 if (addr + i) % 4 else 4)
            v = int.from_bytes(code[i:i + n], "little")
            text = f".word 0x{v:08X}" if n == 4 else f".{n}byte 0x{v:0{n * 2}X}"
            tgt = (v & ~1) - addr if n == 4 and lo <= (v & ~1) < hi else None
            rows.append((i, n, text, ".word ->" if tgt is not None else text, tgt))
            i += n
            continue
        ins = next(CS.disasm(code[i:i + 4], addr + i), None)
        if ins is None:
            v = int.from_bytes(code[i:i + 2], "little")
            rows.append((i, 2, f".2byte 0x{v:04X}", f".2byte 0x{v:04X}", None))
            i += 2
            continue
        text = f"{ins.mnemonic} {ins.op_str}".strip()
        key, tgt = text, None
        m = re.match(r"ldr (\w+), \[pc, #(-?0x[0-9a-f]+|-?\d+)\]", text)
        if m:
            la = ((addr + i + 4) & ~3) + int(m.group(2), 0) - addr
            if 0 <= la <= len(code) - 4:
                v = int.from_bytes(code[la:la + 4], "little")
                if lo <= (v & ~1) < hi:
                    key, tgt = f"ldr {m.group(1)}, =->", (v & ~1) - addr
                else:
                    key = f"ldr {m.group(1)}, =0x{v:X}"
                text = key if tgt is None else f"ldr {m.group(1)}, =func+0x{tgt:X}"
        elif ins.mnemonic.startswith("b") and ins.mnemonic not in ("bic", "bics"):
            m = re.match(r"(\w+(?:\.\w+)?) #(0x[0-9a-f]+)$", text)
            if m:
                t = int(m.group(2), 16)
                if lo <= t < hi:
                    key, tgt = f"{m.group(1)} ->", t - addr
                    text = f"{m.group(1)} func+0x{tgt:X}"
        elif re.match(r"(add|adr) \w+, pc", text):
            key = re.sub(r"#.*", "#->", text)
        rows.append((i, ins.size, text, key, tgt))
        i += ins.size
    return rows


def draft_annotations(wd, func, rows, obj_offset):
    """C line and insn uid for each draft code row."""
    lines = {}
    out = build.sh(["arm-none-eabi-objdump", "--dwarf=decodedline", os.path.join(wd, "u.o")])
    for ln in out.splitlines():
        p = ln.split()
        if len(p) >= 3 and p[1].isdigit() and p[2].startswith("0x"):
            lines[int(p[2], 16)] = (p[0], int(p[1]))
    # walk the .s of FUNC: instruction lines carry "@ UID" (-dp)
    asm = open(os.path.join(wd, "u.s")).read()
    m = re.search(r"^" + re.escape(func) + r":\n(.*?)(?=^\s*\.size\s+" + re.escape(func) + r"|^\S+:\n\s*\.globl|\Z)",
                  asm, re.S | re.M)
    body = m.group(1) if m else ""
    seq = []
    for ln in body.splitlines():
        s = ln.strip()
        if not s or s.startswith((".", "@")) or s.endswith(":"):
            continue
        u = re.search(r"@\s+(\d+)\s", s + " ")
        seq.append((s.split()[0], int(u.group(1)) if u else None))
    code = [r for r in rows if not r[2].startswith(".")]
    ann = {}
    if len(seq) == len(code):
        for (mn, uid), r in zip(seq, code):
            ann[r[0]] = {"uid": uid}
    cur = None
    for o, n, *_ in rows:
        a = obj_offset + o
        if a in lines:
            cur = lines[a]
        ann.setdefault(o, {})["line"] = cur
    return ann


def born_passes(wd):
    """uid -> (first pass the insn appears in, its RTL there)."""
    born = {}
    for p in PASSES:
        path = os.path.join(wd, "u.i." + p)
        if not os.path.exists(path):
            continue
        text = open(path).read()
        for m in re.finditer(r"^\((?:insn|jump_insn|call_insn) (\d+) .*?(?=^\(|\Z)", text, re.S | re.M):
            u = int(m.group(1))
            if u not in born:
                born[u] = (p, one_line(m.group(0)))
    return born


def one_line(rtl):
    rtl = re.sub(r"\(insn_list.*|\(expr_list.*", "", rtl, flags=re.S)
    rtl = re.sub(r"^\((?:insn|jump_insn|call_insn) \d+ \d+ \d+ ", "", rtl)
    rtl = re.sub(r"\s+", " ", rtl).strip()
    rtl = re.sub(r"( \(nil\))+\)?$", "", rtl)
    return re.sub(r" (\d+ \{[^}]*\}|-1)$", "", rtl)


# ---------------------------------------------------------------- align

def align(rom, mine):
    sm = difflib.SequenceMatcher(None, [r[3] for r in rom], [r[3] for r in mine], autojunk=False)
    pairs = []  # (rom_index|None, mine_index|None, tag)
    for tag, a, b, c, d in sm.get_opcodes():
        if tag == "equal":
            pairs += [(a + k, c + k, "=") for k in range(b - a)]
        else:
            n = max(b - a, d - c)
            for k in range(n):
                x = a + k if a + k < b else None
                y = c + k if c + k < d else None
                pairs.append((x, y, "~" if x is not None and y is not None else ("-" if y is None else "+")))
    # branch targets: equal pairs must land on aligned rows
    r_at = {r[0]: i for i, r in enumerate(rom)}
    m_at = {r[0]: i for i, r in enumerate(mine)}
    r2m = {x: y for x, y, t in pairs if t == "=" and x is not None}
    shifted = 0
    out = []
    for x, y, t in pairs:
        if t == "=":
            rt, mt = rom[x][4], mine[y][4]
            if rt is not None or mt is not None:
                ri, mi = r_at.get(rt), m_at.get(mt)
                if ri is None or mi is None or r2m.get(ri) != mi:
                    t = "→"  # lands somewhere else
                elif rom[x][2] != mine[y][2] or rom[x][0] != mine[y][0]:
                    shifted += 1
            elif rom[x][0] != mine[y][0]:
                pass
        out.append((x, y, t))
    return out, shifted


# ---------------------------------------------------------------- report

def report(src, func, rom_code, addr, mine, info, t_compile):
    wd = info["wd"]
    rdata = rom_data_ranges(rom_name(src, func), addr)
    if rdata is None:
        rdata = rom_pool_ranges(rom_code, addr)
    rom = decode(rom_code, addr, rdata)
    mdata = mapping_ranges(os.path.join(wd, "u.o"), info["obj_offset"], len(mine))
    dr = decode(mine, addr, mdata)
    ann = draft_annotations(wd, func, dr, info["obj_offset"])
    born = born_passes(wd)
    pairs, shifted = align(rom, dr)

    def ctag(y):
        if y is None:
            return ""
        a = ann.get(dr[y][0], {})
        ln = a.get("line")
        lt = "" if not ln else (f"{ln[1]}" if os.path.basename(ln[0]) == os.path.basename(src)
                                else f"{os.path.basename(ln[0])}:{ln[1]}")
        u = a.get("uid")
        bp = born.get(u, ("", ""))[0] if u else ""
        return lt, u, bp

    rows_json = []
    full = []
    for x, y, t in pairs:
        r = rom[x] if x is not None else None
        m = dr[y] if y is not None else None
        lt, u, bp = ctag(y) if y is not None else ("", None, "")
        rows_json.append({"tag": t, "rom_addr": f"{addr + r[0]:08X}" if r else None, "rom": r[2] if r else None,
                          "draft_off": m[0] if m else None, "draft": m[2] if m else None,
                          "line": lt, "uid": u, "born": bp})
        full.append(f"{t} {(f'{addr + r[0]:08X} ' + r[2]) if r else '':40} {(m[2] if m else ''):32} "
                    f"{lt:>6} {u or '':>5} {bp}")
    open(os.path.join(wd, "diff.txt"), "w").write("\n".join(full) + "\n")
    json.dump(rows_json, open(os.path.join(wd, "diff.json"), "w"), indent=0)

    real = [i for i, (_, _, t) in enumerate(pairs) if t != "="]
    hunks, cur = [], []
    for i in real:
        if cur and i - cur[-1] > 4:
            hunks.append(cur); cur = []
        cur.append(i)
    if cur:
        hunks.append(cur)
    nr = sum(1 for _, _, t in pairs if t in "-~→")
    nm = sum(1 for _, _, t in pairs if t in "+~")
    same = rom_code == mine
    rel = os.path.relpath(wd, ROOT)
    knobs = " ".join(f"{k}={v}" for k, v in info.get("env", {}).items())
    print(f"{func}  {src}  ROM {len(rom_code)} B / draft {len(mine)} B  compile {t_compile:.1f}s"
          + (f"  [{knobs}]" if knobs else ""))
    if same:
        print("MATCH")
        return 0
    print(f"{len(hunks)} hunk(s): {nr} ROM / {nm} draft insns differ; "
          f"{shifted} offset-only shifts hidden.  full listing: {rel}/diff.txt (json: diff.json)")
    shown = 0
    for h in hunks:
        if shown >= 8:
            print(f"... {len(hunks) - shown} more hunks in {rel}/diff.txt")
            break
        shown += 1
        lo, hi = max(0, h[0] - 2), min(len(pairs), h[-1] + 3)
        first = next((pairs[i][0] for i in range(lo, hi) if pairs[i][0] is not None), None)
        lines_here = sorted({ctag(pairs[i][1])[0] for i in range(lo, hi) if pairs[i][1] is not None} - {""},
                            key=lambda s: (len(s), s))
        print(f"\n@ ROM {addr + rom[first][0]:08X}  C lines {', '.join(lines_here[:6])}")
        print(f"  {'ROM':34} {'draft':34} {'line':>5} {'insn':>5}  born")
        for i in range(lo, hi):
            x, y, t = pairs[i]
            r = rom[x][2] if x is not None else ""
            m = dr[y][2] if y is not None else ""
            lt, u, bp = ctag(y) if y is not None else ("", None, "")
            mark = {"=": " ", "~": "!", "-": "-", "+": "+", "→": ">"}[t]
            bp_s = PASS_NOTE.get(bp, bp)
            print(f"{mark} {r:34} {m:34} {lt:>5} {u or '':>5}  {bp_s}")
    print(f"\nnext: scope.py insn UID   (an insn's RTL through every pass)")
    return 1


def cmd_insn(uid, func):
    wd = workdir(func)
    last = None
    print(f"insn {uid} in {func}:")
    for p in PASSES:
        path = os.path.join(wd, "u.i." + p)
        if not os.path.exists(path):
            continue
        text = open(path).read()
        m = re.search(r"^\((?:insn|jump_insn|call_insn) " + str(uid) + r" .*?(?=^\(|\Z)", text, re.S | re.M)
        cur = one_line(m.group(0)) if m else None
        if cur != last:
            print(f"  {PASS_NOTE.get(p, p):12} {cur if cur else '(gone)'}")
            last = cur


def last_func():
    p = os.path.join(ROOT, "build", "scope", "LAST")
    return open(p).read().strip() if os.path.exists(p) else None


def main():
    args = sys.argv[1:]
    if not args or args[0] in ("-h", "--help"):
        print(__doc__)
        return 0
    if args[0] == "insn":
        func = args[2] if len(args) > 2 else last_func()
        cmd_insn(int(args[1]), func)
        return 0
    src = args[0]
    if not src.endswith(".c"):
        src = os.path.join("src", "fn", src + ".c")
    src = os.path.relpath(os.path.join(ROOT, src) if not os.path.isabs(src) else src, ROOT)
    env = dict(a.split("=", 1) for a in args[1:] if "=" in a)
    rest = [a for a in args[1:] if "=" not in a]
    func = rest[0] if rest else nonmatching_func(src)
    if not func:
        sys.exit("name the function: scope.py FILE.c FUNC")
    os.makedirs(os.path.join(ROOT, "build", "scope"), exist_ok=True)
    open(os.path.join(ROOT, "build", "scope", "LAST"), "w").write(func)
    t0 = time.time()
    mine, addr, info = compile_draft(src, func, env)
    t1 = time.time() - t0
    _, _, sizes = check.inventory()
    rom_code = load_rom(verify=False)[off(addr):off(addr) + sizes[rom_name(src, func)]]
    return report(src, func, rom_code, addr, mine, info, t1)


if __name__ == "__main__":
    sys.exit(main())
