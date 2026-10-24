#!/usr/bin/env python3
"""Look inside agbcc at one draft: what really differs from the ROM, and where it came from.

  scope.py FILE.c [FUNC]        compile the draft, align it with the ROM, print the real differences
  scope.py FILE.c KNOB=V ...    the same with instrumented-compiler knobs set (a what-if)
  scope.py FILE.c spill         is the whole difference reload's spill rotation? (searches AGBCC_SPILL)
  scope.py FILE.c why all       why on the first differing insn of each hunk
  scope.py FILE.c why UID       why insn UID has the operands it has: pass by pass, and for cse the
                                equivalence class and the canonical-register decision, with C lines
  scope.py insn UID [FUNC]      one RTL insn through every pass (from the last compile of FUNC)
  scope.py uses REG... [PASS...] [FUNC]   insns (uid@C line) using each pseudo after each pass

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
import pickle
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

CACHE = os.path.join(ROOT, "build", "scope", "cache")
CLANG = "/Library/Developer/CommandLineTools/usr/bin/clang"
if sys.platform == "darwin" and "CPP" not in os.environ and os.path.exists(CLANG):
    build.CPP = [CLANG, "-E"]  # same output as `cc -E`, without the xcrun shim


_MEMO = {}


def cached(name, key_paths, make):
    """make() memoised in memory and on disk, keyed by the paths' mtimes and sizes
    (the ROM ELF changes only on a full build)."""
    key = "|".join(f"{p}:{os.stat(p).st_mtime_ns}:{os.stat(p).st_size}" for p in key_paths)
    if _MEMO.get(name, (None,))[0] == key:
        return _MEMO[name][1]
    path = os.path.join(CACHE, name + ".pickle")
    try:
        with open(path, "rb") as f:
            k, value = pickle.load(f)
        if k != key:
            raise ValueError
    except (OSError, ValueError, EOFError, pickle.UnpicklingError):
        value = make()
        os.makedirs(CACHE, exist_ok=True)
        with open(path, "wb") as f:
            pickle.dump((key, value), f)
    _MEMO[name] = (key, value)
    return value


def elf_syms():
    elf = os.path.join(ROOT, "build", "boktai3.elf")
    return cached("elf_symbols", [elf], check.elf_symbols)


def inventory():
    csv = os.path.join(ROOT, "symbols", "functions.csv")
    by_label, addr, sizes = cached("inventory", [csv, os.path.join(ROOT, "build", "boktai3.elf")],
                                   lambda: tuple(check.inventory()))
    return by_label, addr, sizes


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
    _, addr, _ = inventory()
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
    tf = (env or {}).get("SCOPE_TRANSFORM") or os.environ.get("SCOPE_TRANSFORM")
    if tf:  # a what-if over the preprocessed source: a module with transform(text) -> text
        import importlib.util
        spec = importlib.util.spec_from_file_location("scope_transform", os.path.join(ROOT, tf))
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        pre = mod.transform(pre)
    open(os.path.join(wd, "u.i"), "w").write(pre)
    for p in PASSES:
        try:
            os.remove(os.path.join(wd, "u.i." + p))
        except FileNotFoundError:
            pass
    names = os.path.join(wd, "names.txt")  # AGBCC_DUMPPRI appends: pseudo numbers with C variable names
    if os.path.exists(names):
        os.remove(names)
    r = subprocess.run([INSTR] + build.cflags_for(src) + ["-g", "-dp", "-da", "u.i", "-o", "u.s"],
                       cwd=wd, capture_output=True, text=True,
                       env=dict(os.environ, AGBCC_DUMPPRI=names, **(env or {})))
    open(os.path.join(wd, "stderr.txt"), "w").write(r.stderr)
    if r.returncode or "error" in r.stderr:
        sys.exit(r.stderr.strip())
    asm = open(os.path.join(wd, "u.s")).read()
    text = re.sub(r"^(\w+):\n", lambda m: f"{m.group(1)}:\n\t.global {m.group(1)}__addr\n{m.group(1)}__addr:\n"
                  if not m.group(1).startswith(".") else m.group(0), asm, flags=re.M)
    open(os.path.join(wd, "a.s"), "w").write(text + "\t.text\n\t.align\t2, 0\n")
    rel = os.path.relpath(wd, ROOT)
    build.sh(build.AS + ["-o", f"{rel}/u.o", f"{rel}/a.s"])
    check.link_object(f"{rel}/u.o", f"{rel}/u.elf", syms=elf_syms())
    build.sh(["arm-none-eabi-objcopy", "-O", "binary", "-j", ".text", f"{rel}/u.elf", f"{rel}/u.bin"])
    whole = open(os.path.join(wd, "u.bin"), "rb").read()
    syms = sorted((v, n) for n, (v, t) in symtab(os.path.join(wd, "u.o")).items() if t == "FUNC")
    names = [n for _, n in syms]
    obj_name = next((n for n in (func, rom_name(src, func)) if n in names), names[0] if len(names) == 1 else None)
    if obj_name is None:
        sys.exit(f"{func} is not in the object (functions: {', '.join(names)})")
    start = next(v for v, n in syms if n == obj_name) & ~1
    end = next((v & ~1 for v, n in syms if (v & ~1) > start), len(whole))
    base = syms[0][0] & ~1
    _, addr, _ = inventory()
    a = addr[rom_name(src, func)]
    return whole[start - base:end - base], a, {"obj_offset": start, "wd": wd, "env": env or {}, "obj_name": obj_name}


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
        for m in re.finditer(r"^\((?:insn|jump_insn|call_insn)(?:/\w)* (\d+) .*?(?=^\(|\Z)", text, re.S | re.M):
            u = int(m.group(1))
            if u not in born:
                born[u] = (p, one_line(m.group(0)))
    return born


def one_line(rtl):
    rtl = re.sub(r"\(insn_list.*|\(expr_list.*", "", rtl, flags=re.S)
    rtl = re.sub(r"^\((?:insn|jump_insn|call_insn)(?:/\w)* \d+ \d+ \d+ ", "", rtl)
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
    ann = draft_annotations(wd, info["obj_name"], dr, info["obj_offset"])
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
    print("\nnext: scope.py FILE.c why UID (why this insn has these registers) | FILE.c spill (only reload"
          " rotation?) | insn UID (RTL through the passes) | uses REG (who uses a pseudo)")
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
        m = re.search(r"^\((?:insn|jump_insn|call_insn)(?:/\w)* " + str(uid) + r" .*?(?=^\(|\Z)", text, re.S | re.M)
        cur = one_line(m.group(0)) if m else None
        if cur != last:
            print(f"  {PASS_NOTE.get(p, p):12} {cur if cur else '(gone)'}")
            last = cur


def cmd_uses(regs, func, passes):
    """For each pseudo, the insns (uid@C line) that mention it after each pass."""
    wd = workdir(func)
    for reg in regs:
        print(f"pseudo {reg} in {func}:")
        for p in passes:
            path = os.path.join(wd, "u.i." + p)
            if not os.path.exists(path):
                continue
            line, out = 0, []
            for blk in re.split(r"\n\n", open(path).read()):
                m = re.search(r'\(note \d+ \d+ \d+ \("[^"]*"\) (\d+)\)', blk)
                if m:
                    line = int(m.group(1))
                    continue
                m = re.match(r"\((?:insn|jump_insn|call_insn)(?:/\w)* (\d+)", blk)
                if m and re.search(r"\(reg(?:/[a-z]+)*:\w+ %d\)" % reg, blk):
                    out.append(f"{m.group(1)}@{line}")
            print(f"  after {p:8} {' '.join(out) or '(none)'}")


# ---------------------------------------------------------------- why

def pseudo_names(wd, func):
    """pseudo -> C variable name, from AGBCC_DUMPPRI (local-alloc 'L' and global-alloc 'G' lines)."""
    out = {}
    path = os.path.join(wd, "names.txt")
    if not os.path.exists(path):
        return out
    for ln in open(path):
        p = ln.split()
        if len(p) > 11 and p[0] == "L" and p[1] == func and p[11] != "-":
            out[int(p[3])] = p[11]
        elif len(p) > 8 and p[0] == "G" and p[1] == func and p[8] != "-":
            out[int(p[2])] = p[8]
    return out


def uid_lines(wd):
    """insn uid -> C line, from the line notes in the RTL dumps."""
    out = {}
    for p in PASSES:
        path = os.path.join(wd, "u.i." + p)
        if not os.path.exists(path):
            continue
        line = 0
        for blk in re.split(r"\n\n", open(path).read()):
            m = re.search(r'\(note \d+ \d+ \d+ \("[^"]*"\) (\d+)\)', blk)
            if m:
                line = int(m.group(1))
                continue
            m = re.match(r"\((?:insn|jump_insn|call_insn|code_label|note)(?:/\w)* (\d+)", blk)
            if m:
                out.setdefault(int(m.group(1)), line)
    return out


def dispositions(wd):
    """pseudo -> hard register, from global-alloc's 'Register dispositions'."""
    path = os.path.join(wd, "u.i.greg")
    out = {}
    if os.path.exists(path):
        text = open(path).read()
        m = re.search(r";; Register dispositions:\n(.*?)\n\n", text, re.S)
        if m:
            for a, b in re.findall(r"(\d+) in (\d+)", m.group(1)):
                out[int(a)] = int(b)
    return out


def regs_of(rtl):
    return [int(n) for n in re.findall(r"\(reg(?:/[a-z]+)*:\w+ (\d+)", rtl or "")]


def parse_trace(text):
    """Split CSE_WHY / CSE_DEBUG output into per-pass lists of eqv events and why-blocks."""
    passes, cur = {}, None
    block = None
    for seq, ln in enumerate(text.splitlines()):
        m = re.match(r"cse pass (\d)", ln)
        if m:
            cur = passes.setdefault(int(m.group(1)), {"eqv": [], "why": []})
            continue
        if cur is None:
            continue
        if ln.startswith("path "):
            d = re.match(r"path pass=\d from=(\d+) to=(\d+) cuid=(\d+)\.\.(\d+) branches=\d+:(.*)", ln)
            if d:
                cur.setdefault("paths", {})[(int(d.group(3)), int(d.group(4)))] = (
                    int(d.group(1)), int(d.group(2)), d.group(5).split())
            continue
        if ln.startswith("eqv "):
            d = dict(re.findall(r"(\w+)=(-?\d+)", ln))
            cur["eqv"].append(dict({k: int(v) for k, v in d.items()}, seq=seq))
        elif ln.startswith("cse_why"):
            block = {"head": ln, "rows": [], "seq": seq}
            cur["why"].append(block)
        elif block is not None and ln.startswith("  ") and not ln.startswith("  (equal"):
            m = re.match(r"\s+(\w+)\s+cost\s+(-?\d+)\s+(?:entered at insn\s+(\d+)\s+)?(?:canon\s+(\d+)\s+)?(.*)", ln)
            if m:
                block["rows"].append({"kind": m.group(1), "cost": int(m.group(2)),
                                      "entered": int(m.group(3)) if m.group(3) else None,
                                      "canon": int(m.group(4)) if m.group(4) else None,
                                      "exp": m.group(5).strip()})
            elif block["rows"]:
                block["rows"][-1]["exp"] += " " + ln.strip()
    return passes


def insn_list(wd, p):
    """[(uid, kind, rtl)] for every insn in pass P's dump, in order."""
    path = os.path.join(wd, "u.i." + p)
    if not os.path.exists(path):
        return []
    return [(int(m.group(2)), m.group(1), one_line(m.group(0)))
            for m in re.finditer(r"^\((insn|jump_insn|call_insn)(?:/\w)* (\d+) .*?(?=^\(|\Z)", open(path).read(), re.S | re.M)]


def hard_of(wd):
    """pseudo -> hard register, read off the insns: each operand before allocation (lreg) against the
    same operand after reload (greg). Covers local-alloc pseudos, which 'Register dispositions' omits."""
    after = {u: r for u, _, r in insn_list(wd, "greg")}
    out = {}
    for u, _, r in insn_list(wd, "lreg"):
        a, b = regs_of(r), regs_of(after.get(u))
        if len(a) == len(b):
            for x, y in zip(a, b):
                if x >= 16 and y < 16:
                    out.setdefault(x, y)
    return out


def alloc_records(wd, func):
    """pseudo -> ('G', rank, priority, refs, live, calls, hard) or ('L', block, priority, birth, death, refs, hard)."""
    out = {}
    path = os.path.join(wd, "names.txt")
    rank = 0
    for ln in open(path) if os.path.exists(path) else []:
        p = ln.split()
        if p[0] == "G" and p[1] == func:
            if out.get(int(p[2]), ("G",))[0] != "L":
                out[int(p[2])] = ("G", rank, int(p[3]), int(p[4]), int(p[5]), int(p[6]), int(p[7]))
            rank += 1
        elif p[0] == "L" and p[1] == func and int(p[10]) >= 0:
            # local alloc runs first; a pseudo it placed also shows up in the G list, so L wins
            out[int(p[3])] = ("L", int(p[2]), int(p[5]), int(p[6]), int(p[7]), int(p[8]), int(p[10]))
    return out


def conflicts(wd):
    """pseudo -> (pseudo conflicts, hard-register conflicts), from global-alloc's dump."""
    out = {}
    path = os.path.join(wd, "u.i.greg")
    for m in re.finditer(r"^;; (\d+) conflicts:(.*)$", open(path).read() if os.path.exists(path) else "", re.M):
        ns = [int(x) for x in m.group(2).split()]
        out[int(m.group(1))] = ({n for n in ns if n >= 16}, {n for n in ns if n < 16})
    return out


def explain_alloc(wd, func, pseudo, want, got, nm, at):
    """Why PSEUDO got hard register GOT and not WANT (the ROM's)."""
    recs, conf, hard = alloc_records(wd, func), conflicts(wd), hard_of(wd)
    rec = recs.get(pseudo)
    if rec and rec[0] == "G":
        _, rank, pri, refs, live, calls, _ = rec
        total = sum(1 for r in recs.values() if r[0] == "G")
        print(f"    {nm(pseudo)} is a global-alloc pseudo: allocated {rank + 1} of {total} by priority {pri}"
              f" ({refs} refs, live {live} insns, crosses {calls} call(s)); it got r{got}, the ROM has r{want}.")
    elif rec:
        _, blk, pri, birth, death, refs, _ = rec
        print(f"    {nm(pseudo)} is a local-alloc pseudo (block {blk}, priority {pri}, {refs} refs);"
              f" it got r{got}, the ROM has r{want}.")
    else:
        print(f"    {nm(pseudo)} has no allocation record; it got r{got}, the ROM has r{want}.")
    if rec and rec[0] == "G" and rec[5] and want < 4:
        print(f"    it lives across a call, so r0-r3 are unavailable to it: the ROM's value does not cross that call.")
        return
    pc, hc = conf.get(pseudo, (set(), set()))
    if want in hc:
        seq = insn_list(wd, "lreg")
        idx = [i for i, (_, _, r) in enumerate(seq) if pseudo in regs_of(r)]
        holders, calls = {}, []
        if idx:
            for u, kind, r in seq[idx[0]:idx[-1] + 1]:
                if kind == "call_insn" and want < 4:
                    calls.append(u)
                for x in regs_of(r):
                    if x != pseudo and (x == want or hard.get(x) == want):
                        holders.setdefault(x, u)
        what = [f"{nm(x) if x >= 16 else 'hard r%d' % x} (from {at(u)})" for x, u in list(holders.items())[:5]]
        if calls:
            what.append(f"a call at {at(calls[0])}")
        print(f"    r{want} was already taken during its life (local alloc or a fixed use runs first): "
              + (", ".join(what) if what else "holder not found in its span"))
        print(f"    lever: shorten {nm(pseudo)}'s life or move the other value's, so they don't overlap in"
              f" r{want}; or make the other value land elsewhere")
        return
    takers = [x for x in pc if hard.get(x, recs.get(x, (0,) * 7)[6]) == want and x != pseudo]
    before = [x for x in takers if recs.get(x, ("L",))[0] == "L" or
              (recs.get(x) and rec and rec[0] == "G" and recs[x][1] < rec[1])]
    if before:
        ds = ", ".join(f"{nm(x)} (priority {recs[x][2]})" for x in sorted(before)[:5] if x in recs)
        print(f"    r{want} went to a conflicting pseudo allocated earlier: {ds}")
        print(f"    lever: raise {nm(pseudo)}'s priority above theirs (more uses or a shorter life), or end the"
              f" overlap; what-if: AGBCC_PRI={pseudo}:+N or AGBCC_FORCE={pseudo}:{want}")
        return
    who = "local alloc" if rec and rec[0] == "L" else "global alloc"
    print(f"    r{want} was free for it; {who} preferred r{got} (copy/tie preferences or register order)."
          f" If the ROM's value was instead a constant that reload loaded (an integer address or offset"
          f" rather than a symbol, or a variable left without a register), it would take the rotation's"
          f" register: try `spill`. what-if: AGBCC_FORCE={pseudo}:{want}")


def reload_records(wd, func):
    """[(k, uid, spill_reg, line, label)] from AGBCC_DUMPPRI's R lines: reload register choices in order."""
    path = os.path.join(wd, "names.txt")
    out = []
    for ln in open(path) if os.path.exists(path) else []:
        p = ln.split()
        if p[0] == "R" and p[1] == func:
            out.append((int(p[2]), int(p[3]), int(p[4]), int(p[5]), p[7] if len(p) > 7 else "-"))
    return out


def score(src, func, env):
    """Compile with ENV and return the number of differing instructions (0 = MATCH), silently."""
    import contextlib
    import io
    mine, addr, info = compile_draft(src, func, env)
    _, _, sizes = inventory()
    rom_code = load_rom(verify=False)[off(addr):off(addr) + sizes[rom_name(src, func)]]
    with contextlib.redirect_stdout(io.StringIO()):
        report(src, func, rom_code, addr, mine, info, 0)
    rows = json.load(open(os.path.join(info["wd"], "diff.json")))
    return sum(1 for r in rows if r["tag"] != "="), info


def rotation_hints(src, wd, func, rel, k, d, regs, order):
    """What lies between reload choice K-1 and K that decides how many choices the ROM made there."""
    prev = next((r for r in rel if r[0] == k - 1), None)
    cur = next(r for r in rel if r[0] == k)
    lo = order.index(prev[1]) if prev and prev[1] in order else 0
    hi = order.index(cur[1]) if cur[1] in order else len(order)
    span = set(order[lo:hi + 1])
    lines = uid_lines(wd)
    names = pseudo_names(wd, func)
    recs = alloc_records(wd, func)
    hard = hard_of(wd)
    text = open(os.path.join(ROOT, src)).read().splitlines()
    cl = sorted({lines[u] for u in span if lines.get(u)})
    print(f"    between reload {k - 1} (line {prev[3] if prev else '?'}) and {k} (line {cur[3]}): the ROM's"
          f" rotation is {d} step(s) on (list {' '.join('r%d' % r for r in regs)}), so it made more (or"
          f" fewer, mod {len(regs)}) reload choices here. C lines in the span: {', '.join(map(str, cl))}")
    pins = [i + 1 for i, t in enumerate(text) if re.search(r"register\b[^;]*asm\s*\(", t)]
    if pins:
        print(f"      register pins in the file (each removes the reload of its value): lines"
              f" {', '.join(map(str, pins))}")
    used = {}
    for u, _, r in insn_list(wd, "lreg"):
        if u in span:
            for x in regs_of(r):
                if x >= 16:
                    used.setdefault(x, u)
    high = [x for x in used if 8 <= hard.get(x, recs.get(x, (0,) * 7)[6]) <= 12]
    none = [x for x in used if x in recs and recs[x][0] == "G" and recs[x][6] < 0]
    def nm(x):
        return f"{x} ({names[x]})" if x in names else str(x)
    if high:
        print(f"      values in high registers used here (each use by a low-register insn is a reload):"
              f" {', '.join(nm(x) + ' r%d' % hard.get(x, recs.get(x, (0,) * 7)[6]) for x in high)}")
    if none:
        print(f"      values with no register (reloaded from a constant or the stack at each use):"
              f" {', '.join(nm(x) for x in none)}")
    print("      levers: a value moved from a low register to a high one, or to no register (make it a"
          " longer-lived variable set earlier), adds reloads; a pin, or a value already in a low register,"
          " removes them")


def cmd_spill(src, func, env):
    """Is the whole difference reload's spill rotation? Greedily shift the rotation (AGBCC_SPILL) before
    each reload choice, from the first differing instruction on, keeping shifts that reduce the diff."""
    base, info = score(src, func, env)
    wd = info["wd"]
    rel = reload_records(wd, info["obj_name"])
    order = [u for u, _, _ in insn_list(wd, "greg")]
    pos = {u: i for i, u in enumerate(order)}
    rows = json.load(open(os.path.join(wd, "diff.json")))
    first = min((pos[r["uid"]] for r in rows if r["tag"] != "=" and r["uid"] in pos), default=0)
    regs = sorted({r[2] for r in rel})
    print(f"{func}: {base} differing insns; {len(rel)} reload register choices, spill registers seen"
          f" {', '.join('r%d' % r for r in regs)}")
    spec, best, tried, accepted = [], base, 0, []
    for k, uid, reg, line, lab in rel:
        if best == 0:
            break
        if pos.get(uid, 1 << 30) < first - 2:
            continue
        cand = None
        for d in range(1, 8):
            tried += 1
            n, _ = score(src, func, dict(env, AGBCC_SPILL=",".join(spec + [f"{k}:{d}"])))
            if n < best and (cand is None or n < cand[0]):
                cand = (n, d)
        if cand:
            best = cand[0]
            spec.append(f"{k}:{cand[1]}")
            print(f"  reload {k} (insn {uid}, line {line}, {lab}, got r{reg}): shift {cand[1]} -> {best} differ")
            accepted.append((k, cand[1]))
    for k, d in accepted:
        rotation_hints(src, wd, info["obj_name"], rel, k, d, regs, order)
    print(f"  {tried} compiles. " + (f"MATCH with AGBCC_SPILL={','.join(spec)}: the C's shape is right; only"
          " reload's round-robin spill choice differs (last_spill_reg, reload1.c). The ROM made a different"
          " number of reload choices, or had a different spill set, before these points."
          if best == 0 else f"best {best} differing with AGBCC_SPILL={','.join(spec) or '(none)'}:"
          " the rotation is not the whole story."))
    return 0


def cmd_why_all(src, func, env):
    """why for the first differing insn (with a uid) of each hunk, up to 8."""
    n, info = score(src, func, env)
    if n == 0:
        print(f"{func}: MATCH")
        return 0
    rows = json.load(open(os.path.join(info["wd"], "diff.json")))
    picks, last = [], -10
    for i, r in enumerate(rows):
        if r["tag"] == "=":
            continue
        if i - last > 4:  # a new hunk (same rule as the diff view)
            picks.append(None)
        last = i
        if picks[-1] is None and r["uid"] and r["tag"] != "→":  # a branch that only lands elsewhere is no cause
            picks[-1] = r["uid"]
    picks = [u for u in picks if u][:8]
    print(f"{func}: {n} differing insns; why on the first of each hunk: {', '.join(map(str, picks))}\n")
    for u in picks:
        cmd_why(src, func, u, env)
        print()
    return 1


def cmd_why(src, func, uid, env):
    import contextlib
    import io
    env = dict(env, CSE_WHY=str(uid), CSE_DEBUG="1", CSE_PATHS="1")
    mine, addr, info = compile_draft(src, func, env)
    _, _, sizes = inventory()
    rom_code = load_rom(verify=False)[off(addr):off(addr) + sizes[rom_name(src, func)]]
    with contextlib.redirect_stdout(io.StringIO()):
        report(src, func, rom_code, addr, mine, info, 0)
    wd = info["wd"]
    names = pseudo_names(wd, info["obj_name"])
    lines = uid_lines(wd)
    hard = dispositions(wd)
    srclines = open(os.path.join(ROOT, src)).read().splitlines()

    def nm(r):
        return f"{r} ({names[r]})" if r in names else str(r)

    def at(u):
        ln = lines.get(u)
        return f"insn {u} (line {ln})" if ln else f"insn {u}"

    alloc_diffs = []
    rows = json.load(open(os.path.join(wd, "diff.json")))
    row = next((r for r in rows if r["uid"] == uid), None)
    ln = lines.get(uid)
    code = srclines[ln - 1].strip() if ln and ln <= len(srclines) else ""
    print(f"why insn {uid}  line {ln}: {code}")
    if row:
        print(f"  ROM: {row['rom'] or '(none)'}   draft: {row['draft']}   [{'same' if row['tag'] == '=' else 'differs'}]")
        rr = re.findall(r"\br(\d+)\b", row["rom"] or "")
        dr = re.findall(r"\br(\d+)\b", row["draft"] or "")
        hmap = hard_of(wd)
        pre = next((r for u, _, r in insn_list(wd, "lreg") if u == uid), None)
        for a, b in zip(rr, dr):
            if a == b:
                continue
            ps = [x for x in regs_of(pre) if hmap.get(x, x) == int(b)]
            alloc_diffs.append((ps[0] if ps else None, int(a), int(b)))
            print(f"  ROM r{a} where the draft has r{b}" + (f" = pseudo {nm(ps[0])}" if ps else ""))
    # operand through the passes
    trail, last, changes = [], None, []
    for p in PASSES:
        path = os.path.join(wd, "u.i." + p)
        if not os.path.exists(path):
            continue
        m = re.search(r"^\((?:insn|jump_insn|call_insn)(?:/\w)* " + str(uid) + r" .*?(?=^\(|\Z)", open(path).read(), re.S | re.M)
        rtl = one_line(m.group(0)) if m else None
        regs = regs_of(rtl)
        if rtl and regs != last:
            if last is not None:
                changes.append((p, [r for r in last if r not in regs], [r for r in regs if r not in last]))
            trail.append(f"{PASS_NOTE.get(p, p)}: {', '.join(nm(r) for r in regs)}")
            last = regs
    if row and row["tag"] == "→":
        print("  same branch; it lands somewhere else because the code between differs (see the other hunks)")
    if not trail:
        print(f"  insn {uid} is in no RTL dump: final emitted it while writing assembly (an output-time"
              " split or branch), so its cause is the insns around it")
    else:
        print("  registers through the passes: " + "  ->  ".join(trail))
    if row and row["rom"] and row["draft"] and row["rom"].split()[0] != row["draft"].split()[0]:
        print(f"  this is a shape difference ({row['rom'].split()[0]} vs {row['draft'].split()[0]}), not a"
              " register choice: compare the RTL (`scope.py insn UID`) with what the ROM implies")
    trace = parse_trace(open(os.path.join(wd, "stderr.txt")).read())
    for p, gone, new in changes:
        if p not in ("cse", "cse2") or not new:
            if p not in ("greg", "lreg"):
                print(f"  {p} replaced {', '.join(nm(r) for r in gone)} by {', '.join(nm(r) for r in new)}"
                      " (no explainer for this pass yet)")
            continue
        n = 1 if p == "cse" else 2
        t = trace.get(n, {"why": [], "eqv": []})
        print(f"  cse pass {n} replaced {', '.join(nm(r) for r in gone)} by {', '.join(nm(r) for r in new)}.")
        if t["why"]:
            b = t["why"][0]
            cls = []
            for r in b["rows"]:
                if r["kind"] != "class":
                    cls.append(f"{r['kind']} {r['exp']}")
                    continue
                mreg = re.match(r"\(reg(?:/[a-z]+)*:\w+ (\d+)\)$", r["exp"])
                if mreg:
                    g = int(mreg.group(1))
                    cls.append(nm(g) + (f"->{r['canon']}" if r["canon"] and r["canon"] != g else ""))
                else:
                    cls.append(r["exp"][:40])
            print(f"    values equal to the source here, cheapest first ({len(t['why'])} looks; first shown):")
            print("      " + " | ".join(cls))
            paths = t.get("paths", {})
            for w in t["why"][:5]:
                mc = re.search(r"block cuid (\d+)\.\.(\d+)", w["head"])
                pth = paths.get((int(mc.group(1)), int(mc.group(2)))) if mc else None
                if pth:
                    br = " ".join(f"{b}({lines.get(int(b[:-1]), '?')})" for b in pth[2]) or "none"
                    print(f"      look in the extended block from {at(pth[0])}, path {br}")
            print("      (path: branch uid + T taken / N not taken / A skipped around, with its C line; the first"
                  " look that substitutes wins, since it rewrites the insn)")
        first_look = t["why"][0]["seq"] if t["why"] else 1 << 30
        shown = 0
        for target in new:
            for g in gone:
                # the takeover: g's class merged with target's and target won the head (make_regs_eqv)
                evs = [e for e in t["eqv"] if e["head"] == target and g in (e["first"], e["old"])
                       and e["seq"] < first_look]
                if not evs:
                    continue
                e = evs[-1]
                print(f"    {nm(target)} became canonical over {nm(g)} at {at(e['at'])}: its last use,"
                      f" {at(e['newlastuid'])}, is later than {nm(g)}'s last use, {at(e['firstlastuid'])}."
                      " make_regs_eqv (cse.c) keeps the register that lives longest as the class head.")
                print(f"    lever: use {nm(g)} after line {lines.get(e['newlastuid'], '?')}, or stop using"
                      f" {nm(target)} after line {lines.get(e['firstlastuid'], '?')}, or keep the two values"
                      f" from being known equal at line {lines.get(e['at'], '?')}")
                shown += 1
        if not shown:
            print("    no class-head takeover found for these registers; see the raw trace")
    changed = {r for _, _, new in changes for r in new}
    for ps, want, got in alloc_diffs:
        if ps is not None and (ps in changed and any(p in ("cse", "cse2") for p, _, new in changes if ps in new)):
            continue
        if ps is not None:
            print(f"  allocation: {nm(ps)}")
            explain_alloc(wd, info["obj_name"], ps, want, got, nm, at)
            continue
        rel = reload_records(wd, info["obj_name"])
        order = [u for u, _, _ in insn_list(wd, "greg")]
        nxt = order[order.index(uid):][:4] if uid in order else []
        if row:  # an insn split after reload (mach) serves the next insns in the listing
            i = rows.index(row)
            nxt += [r["uid"] for r in rows[i + 1:i + 4] if r["uid"]]
        rec = next((r for u in nxt for r in rel if r[1] == u and r[2] == got), None)
        if rec:
            k, u, reg, line, lab = rec
            served = "" if u == uid else f" for {at(u)}"
            print(f"  reload: r{got} is reload register choice #{k}{served} (value {lab}). Reload hands out"
                  f" spill registers round-robin (last_spill_reg, reload1.c), so the ROM's r{want} means a"
                  f" different rotation state here: a different number of earlier reloads or spill set.")
            print(f"    what-if: AGBCC_SPILL={k}:D shifts the rotation before this choice;"
                  f" `scope.py FILE.c spill` searches all of them")
        else:
            print(f"  r{got} is not a pseudo before allocation and no reload record names it here")
    print(f"  raw traces: {os.path.relpath(wd, ROOT)}/stderr.txt")
    return 0



def last_func():
    p = os.path.join(ROOT, "build", "scope", "LAST")
    return open(p).read().strip() if os.path.exists(p) else None


def main():
    args = sys.argv[1:]
    if not args or args[0] in ("-h", "--help"):
        print(__doc__)
        return 0
    if args[0] == "uses":
        regs = [int(a) for a in args[1:] if a.isdigit()]
        names = [a for a in args[1:] if not a.isdigit()]
        passes = [a for a in names if a in PASSES] or ["cse", "cse2", "combine", "lreg"]
        func = next((a for a in names if a not in PASSES), None) or last_func()
        cmd_uses(regs, func, passes)
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
    why_uid = None
    if "spill" in rest:
        rest.remove("spill")
        func = rest[0] if rest else nonmatching_func(src)
        return cmd_spill(src, func, env)
    if "why" in rest:
        i = rest.index("why")
        why_uid = -1 if rest[i + 1] == "all" else int(rest[i + 1])
        rest = rest[:i] + rest[i + 2:]
    func = rest[0] if rest else nonmatching_func(src)
    if not func:
        sys.exit("name the function: scope.py FILE.c FUNC")
    os.makedirs(os.path.join(ROOT, "build", "scope"), exist_ok=True)
    open(os.path.join(ROOT, "build", "scope", "LAST"), "w").write(func)
    if why_uid == -1:
        return cmd_why_all(src, func, env)
    if why_uid is not None:
        return cmd_why(src, func, why_uid, env)
    t0 = time.time()
    mine, addr, info = compile_draft(src, func, env)
    t1 = time.time() - t0
    _, _, sizes = inventory()
    rom_code = load_rom(verify=False)[off(addr):off(addr) + sizes[rom_name(src, func)]]
    return report(src, func, rom_code, addr, mine, info, t1)


if __name__ == "__main__":
    sys.exit(main())
