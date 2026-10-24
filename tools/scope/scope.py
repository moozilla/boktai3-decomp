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
    hit = next((n for n in names if n in addr), None)
    if hit is None and re.fullmatch(r"sub_[0-9A-Fa-f]{8}", func):  # an address name renamed since
        hit = next((n for n, a in addr.items() if a == int(func[4:], 16)), None)
    return hit or func


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
        errs = [ln.replace(ROOT + os.sep, "") for ln in r.stderr.splitlines() if ln.strip()]
        hint = ("\n  (agbcc is C89: in each block, declarations come before statements)"
                if any("parse error" in e or "syntax error" in e for e in errs) else "")
        sys.exit(f"COMPILE FAILED: agbcc rejected {src}:\n" + "\n".join("  " + e for e in errs[:15]) + hint)
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


def flow_blocks(wd):
    """Basic blocks of flow's dump: [(live_at_start set, [(uid, kind, pattern text)], label uids, jump
    targets, falls through)]."""
    path = os.path.join(wd, "u.i.flow")
    text = open(path).read() if os.path.exists(path) else ""
    blocks, cur = [], None
    for blk in re.split(r"\n(?=\(|;;)", text):
        m = re.match(r";; Start of basic block \d+, registers live:(.*)", blk)
        if m:
            cur = [set(int(x) for x in re.findall(r"(?<!\[)\b(\d+)\b(?![^\[]*\])", m.group(1))), [], set(), set(), True]
            blocks.append(cur)
            continue
        if cur is None:
            continue
        m = re.match(r"\((insn|jump_insn|call_insn|code_label|barrier)(?:/\w)* (\d+)", blk)
        if not m:
            continue
        kind, uid = m.group(1), int(m.group(2))
        flat = " ".join(blk.split())
        if kind == "code_label":
            if not cur[1]:
                cur[2].add(uid)
            continue
        if kind == "barrier":
            cur[4] = False
            continue
        pat = re.split(r" \((?:expr|insn)_list:REG_| \(insn_list ", flat)[0]
        cur[1].append((uid, kind, pat))
        if kind == "jump_insn":
            cur[3].update(int(x) for x in re.findall(r"\(label_ref (\d+)\)", pat))
            if "(return)" in pat or re.search(r"\(set \(pc\) \(label_ref", pat):
                cur[4] = False
    return blocks


def live_insns(wd, reg):
    """The insns flow counts toward REG's live length (flow.c: one per insn where it is live before the
    insn, plus one where it is set), from block liveness walked backward."""
    blocks = flow_blocks(wd)
    by_label = {lab: i for i, b in enumerate(blocks) for lab in b[2]}
    pat_reg = r"\(reg(?:/\w)*:\w+ %d\)" % reg
    out = []
    for i, (live_in, insns, _, targets, falls) in enumerate(blocks):
        succ = {by_label[t] for t in targets if t in by_label}
        if falls and i + 1 < len(blocks):
            succ.add(i + 1)
        live = any(reg in blocks[j][0] for j in succ)
        counted = []
        for uid, kind, pat in reversed(insns):
            sets = re.findall(r"\((?:set|clobber) " + pat_reg, pat)
            mentions = len(re.findall(pat_reg, pat))
            uses = mentions - len(sets)
            if sets and not uses:
                live_before = False
            else:
                live_before = live or uses > 0
            if live_before or sets:
                counted.append((uid, bool(mentions)))
            live = live_before
        out.extend(reversed(counted))
    return out


def cmd_life(names, func):
    """What makes up a pseudo's live length: the insns flow counted (liveness across branches), grouped
    by C line. Pass pseudo numbers or C variable names (names survive edits; numbers shift)."""
    wd = workdir(func)
    pn = pseudo_names(wd, func)
    lines = uid_lines(wd)
    recs = alloc_records(wd, func)
    flow = live_lengths(wd, "flow")
    for name in names:
        regs = [int(name)] if name.isdigit() else [r for r, n in pn.items() if n == name]
        if not regs:
            print(f"{name}: no pseudo by that name (known: {', '.join(sorted(set(pn.values())))[:300]})")
            continue
        for reg in regs:
            ins = live_insns(wd, reg)
            rec = recs.get(reg)
            pri = (f", priority {rec[2]} ({rec[3]} refs{ref_note(wd, reg, rec[3])}, live {rec[4]} at allocation)"
                   if rec and rec[0] == "G" else "")
            print(f"pseudo {reg} ({pn.get(reg, '-')}): flow live length {flow.get(reg, '?')}{pri};"
                  f" counted here: {len(ins)}")
            by, order = {}, []
            for u, m in ins:
                ln = lines.get(u, 0)
                if ln not in by:
                    by[ln] = []
                    order.append(ln)
                by[ln].append(f"{u}{'*' if m else ''}")
            for ln in order:
                print(f"  line {ln:4}: {len(by[ln]):3}  {' '.join(by[ln])}")
            print("  (* = mentions it; compare two drafts by variable name to see which line gained or lost insns)")


def alloc_table(wd, func):
    """Named pseudos with their allocation facts, keyed by a label that survives edits: the C name or
    constant label plus the C line of the first insn that mentions it (and a counter for repeats)."""
    pn, lines, recs, prefs, hard = pseudo_names(wd, func), uid_lines(wd), alloc_records(wd, func), preferences(wd), hard_of(wd)
    first = {}
    for u, _, r in insn_list(wd, "lreg"):
        for x in regs_of(r):
            if x >= 16:
                first.setdefault(x, u)
    out, seen = {}, {}
    for x in sorted(recs, key=lambda x: (first.get(x, 1 << 30), x)):
        rec = recs[x]
        name = pn.get(x, "-")
        if name == "-" and (rec[0] == "L" or rec[4] < 8):
            continue  # short-lived temporaries: renumbered by every edit, rarely the point
        # named values match by name and order; unnamed ones (inline parameter copies, temporaries)
        # by the C line where they first appear
        key = (name, 0) if name != "-" else ("-", lines.get(first.get(x), 0))
        seen[key] = seen.get(key, 0) + 1
        key = key + (seen[key],)
        out[key] = dict(pseudo=x, kind=rec[0], rank=rec[1] if rec[0] == "G" else None, pri=rec[2],
                        refs=rec[3] if rec[0] == "G" else rec[5], live=rec[4] if rec[0] == "G" else None,
                        prefs=prefs.get(x, []), hard=hard.get(x, rec[6]), galloc=rec[6])
    return out


def cmd_vs(src_a, src_b, func, env):
    """Allocation of two drafts side by side: which named values moved, and what about them changed."""
    import contextlib
    import io
    tabs = []
    for src in (src_a, src_b):
        mine, addr, info = compile_draft(src, func, env)
        _, _, sizes = inventory()
        rom_code = load_rom(verify=False)[off(addr):off(addr) + sizes[rom_name(src, func)]]
        buf = io.StringIO()
        with contextlib.redirect_stdout(buf):
            report(src, func, rom_code, addr, mine, info, 0)
        head = buf.getvalue().splitlines()
        print(f"{src}: {head[1] if len(head) > 1 else ''}")
        tabs.append(alloc_table(info["wd"], info["obj_name"]))
        # keep A's dumps: B's compile reuses the work directory
        if src is src_a:
            import shutil
            keep = info["wd"] + ".A"
            if os.path.exists(keep):
                shutil.move(keep, os.path.join(os.path.expanduser("~/.Trash"), os.path.basename(keep) + f".{int(time.time())}"))
            shutil.copytree(info["wd"], keep)
    a, b = tabs
    def fmt(d):
        if not d:
            return "(absent)"
        g = f"#{d['rank'] + 1} " if d["rank"] is not None else "local "
        pr = ("pref " + ",".join(rname(h) for h in d["prefs"]) + " ") if d["prefs"] else ""
        ev = f" (was {rname(d['galloc'])})" if d["kind"] == "G" and d["galloc"] != d["hard"] and d["galloc"] >= 0 else ""
        lv = f"/{d['live']}" if d["live"] is not None else ""
        return f"{g}pri {d['pri']} ({d['refs']}{lv}) {pr}-> {rname(d['hard'])}{ev}"
    rows = []
    for k in sorted(set(a) | set(b), key=lambda k: (k[1], k[0], k[2])):  # named first, then by line
        da, db = a.get(k), b.get(k)
        if da and db and da["hard"] == db["hard"] and da["prefs"] == db["prefs"] and da["pri"] == db["pri"]:
            continue
        rows.append((k, da, db))
    print(f"named values whose allocation differs (A = {src_a}, B = {src_b}); (refs/live), #rank in global alloc:")
    for (name, line, n), da, db in rows:
        tag = (name if line == 0 else f"(unnamed)@{line}") + (f" #{n}" if n > 1 else "")
        print(f"  {tag:28} A: {fmt(da):44} B: {fmt(db)}")
    if not rows:
        print("  (none: the same allocation for every named value)")
    return 0


# ---------------------------------------------------------------- why

def dump_func(wd, func):
    """The function name AGBCC_DUMPPRI used in WD's names.txt: the draft may #define a friendly name to
    the ROM symbol, so FUNC falls back to the only function recorded there."""
    path = os.path.join(wd, "names.txt")
    seen = []
    for ln in open(path) if os.path.exists(path) else []:
        p = ln.split()
        if len(p) > 1 and p[0] in "GLRF" and p[1] not in seen:
            seen.append(p[1])
    return func if func in seen or len(seen) != 1 else seen[0]


def pseudo_names(wd, func):
    """pseudo -> C variable name, from AGBCC_DUMPPRI (local-alloc 'L' and global-alloc 'G' lines)."""
    func = dump_func(wd, func)
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
    # the operand can be a reload register (a value loaded into r2, then copied to ip): what global
    # alloc decided wins for its pseudos
    out.update(dispositions(wd))
    return out


def alloc_records(wd, func):
    """pseudo -> ('G', rank, priority, refs, live, calls, hard) or ('L', block, priority, birth, death, refs, hard)."""
    func = dump_func(wd, func)
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


REG_ALIASES = {"sb": 9, "sl": 10, "fp": 11, "ip": 12, "sp": 13, "lr": 14, "pc": 15}


def rname(n):
    """The assembler's name for hard register N."""
    return {v: k for k, v in REG_ALIASES.items()}.get(n, f"r{n}")


def reg_nums(text):
    """Register operands of an assembly line in order, as numbers (sb/sl/fp/ip/sp/lr/pc included)."""
    out = []
    for m in re.finditer(r"\b(?:r(\d+)|(sb|sl|fp|ip|sp|lr|pc))\b", (text or "").split(None, 1)[-1] if text else ""):
        out.append(int(m.group(1)) if m.group(1) else REG_ALIASES[m.group(2)])
    return out


def preferences(wd):
    """pseudo -> hard registers global alloc prefers for it (greg dump), after expand_preferences."""
    path = os.path.join(wd, "u.i.greg")
    text = open(path).read() if os.path.exists(path) else ""
    return {int(m.group(1)): [int(x) for x in m.group(2).split()]
            for m in re.finditer(r"^;; (\d+) preferences:(.*)$", text, re.M)}


def pref_sources(wd, x, reg):
    """Where X picked up a preference for hard register REG in expand_preferences (global.c): insns that
    set an allocno while another dies, merging their preferences, walked in insn order and REG_DEAD note
    order as global.c does. Returns [(uid, other pseudo, notes in order)]."""
    prefs = preferences(wd)
    out = []
    path = os.path.join(wd, "u.i.lreg")
    text = open(path).read() if os.path.exists(path) else ""
    for m in re.finditer(r"^\((insn)(?:/\w)* (\d+) .*?(?=^\(|\Z)", text, re.S | re.M):
        flat = " ".join(m.group(0).split())
        d = re.match(r"\(insn(?:/\w)* \d+ \d+ \d+ \((?:parallel\[ \()?set \(reg(?:/\w)*:\w+ (\d+)\)", flat)
        if not d:
            continue
        dest = int(d.group(1))
        dead = [int(n) for n in re.findall(r"REG_DEAD \(reg(?:/\w)*:\w+ (\d+)\)", flat)]
        if x == dest:
            others = [n for n in dead if reg in prefs.get(n, [])]
        elif x in dead and reg in prefs.get(dest, []):
            others = [dest]
        else:
            continue
        if others:
            out.append((int(m.group(2)), others[0], dest, dead))
    return out


def rom_hard(wd):
    """pseudo -> the ROM's register for it: every aligned row with the same mnemonic votes, operand by
    operand, through the pseudos the lreg insn holds in the draft's register at that position."""
    hmap = hard_of(wd)
    pre = {u: r for u, _, r in insn_list(wd, "lreg")}
    votes = {}
    for row in json.load(open(os.path.join(wd, "diff.json"))):
        if not row.get("uid") or not row.get("rom") or not row.get("draft"):
            continue
        if row["rom"].split()[0] != row["draft"].split()[0]:
            continue
        rr, dr = reg_nums(row["rom"]), reg_nums(row["draft"])
        if len(rr) != len(dr):
            continue
        regs = regs_of(pre.get(row["uid"]))
        for a, b in zip(rr, dr):
            for x in regs:
                if x >= 16 and hmap.get(x) == b:
                    votes.setdefault(x, {}).setdefault(a, 0)
                    votes[x][a] += 1
    return {x: max(v, key=v.get) for x, v in votes.items()}


def live_lengths(wd, p):
    """pseudo -> REG_LIVE_LENGTH as the pass's dump prints it ('Register N used R times across L insns')."""
    path = os.path.join(wd, "u.i." + p)
    text = open(path).read() if os.path.exists(path) else ""
    return {int(m.group(1)): int(m.group(2))
            for m in re.finditer(r"^Register (\d+) used \d+ times across (\d+) insns", text, re.M)}


def life_note(wd, x, live):
    """Why global alloc's live length differs from flow's: update_equiv_regs (local-alloc.c) doubles the
    length of a register equivalent to a constant (REG_EQUIV: a symbol address or a number), so it gets half
    the priority, and regmove moves lengths between registers when it rewrites copies."""
    fl = live_lengths(wd, "flow").get(x)
    if fl is None or fl == live:
        return ""
    if live == 2 * fl:
        return (f" [flow measured {fl}; doubled because it always holds a constant (REG_EQUIV):"
                f" update_equiv_regs, local-alloc.c, halves constant registers' priority]")
    return f" [flow measured {fl}; regmove changed it while rewriting copies]"


def g_priority(refs, live):
    """global.c allocno_compare: floor_log2(refs) * refs / live * 10000 (size 1)."""
    return (max(refs, 1).bit_length() - 1) * refs * 10000 // max(live, 1)


def _sexp_end(t, i):
    d = 0
    while i < len(t):
        c = t[i]
        if c == '"':
            i = t.index('"', i + 1)
        elif c == "(":
            d += 1
        elif c == ")":
            d -= 1
            if d == 0:
                return i + 1
        i += 1
    return i


def ref_mentions(wd, reg, dump="regmove"):
    """[(uid, loop depth)] for each mention of REG in insn patterns of a dump (notes excluded). flow.c adds
    the loop depth (1 outside loops, +1 per enclosing loop) to REG_N_REFS for every use and set, so a
    mention inside a loop counts 2; combine and regmove adjust the count afterwards."""
    path = os.path.join(wd, "u.i." + dump)
    t = open(path).read() if os.path.exists(path) else ""
    out, depth = [], 1
    for m in re.finditer(r"^\((insn|jump_insn|call_insn|note)(?:/\w)* (\d+) \d+ \d+ ", t, re.M):
        if m.group(1) == "note":
            line = t[m.start():t.find("\n", m.end())]
            depth += ("NOTE_INSN_LOOP_BEG" in line) - ("NOTE_INSN_LOOP_END" in line)
            continue
        i = m.end()
        while t[i] in " \n\t":
            i += 1
        pat = t[i:_sexp_end(t, i)]
        out += [(int(m.group(2)), depth)] * len(re.findall(r"\(reg(?:/\w)*:\w+ %d\)" % reg, pat))
    return out


def ref_note(wd, reg, refs):
    """' = 3 mentions in a loop (x2) + 1 outside'-style breakdown of an allocation ref count."""
    ms = ref_mentions(wd, reg)
    if not ms:
        return ""
    by = {}
    for _, d in ms:
        by[d] = by.get(d, 0) + 1
    parts = [f"{n} mention{'s' if n > 1 else ''} " + ("outside loops" if d == 1 else f"in a loop (x{d})")
             for d, n in sorted(by.items())]
    total = sum(d for _, d in ms)
    txt = f"; refs = {' + '.join(parts)}"
    if total != refs:
        txt += f" = {total} in the regmove dump; combine/regmove adjusted the allocator's count to {refs}"
    return txt


def find_reg_view(wd, func):
    """pseudo -> [records] from AGBCC_DUMPPRI's F lines, one per find_reg call (global alloc, then reload's
    retries): dict with used1 (conflicts and class limits), sofar (regs_used_so_far), someone
    (regs_someone_prefers), cpref/pref (its preferences), first/pass (what the two passes found),
    final (after preferences), as hard-register sets."""
    func = dump_func(wd, func)
    def bits(h):
        v = int(h, 16)
        return {i for i in range(16) if v >> i & 1}
    out = {}
    path = os.path.join(wd, "names.txt")
    for ln in open(path) if os.path.exists(path) else []:
        p = ln.split()
        if p[0] == "F" and p[1] == func:
            out.setdefault(int(p[2]), []).append(dict(
                retrying=int(p[3]), alt=int(p[4]), used1=bits(p[5]), sofar=bits(p[6]), someone=bits(p[7]),
                cpref=bits(p[8]), pref=bits(p[9]), first=int(p[10]), passno=int(p[11]), final=int(p[12])))
    return out


def regset(rs):
    return "{" + ", ".join(rname(r) for r in sorted(rs)) + "}" if rs else "{}"


def explain_find_reg(wd, func, pseudo, want, got, nm, recs, rec):
    """Walk find_reg (global.c) for PSEUDO as the compiler ran it: pass 0 reuses a register already in use
    (regs_used_so_far) that it doesn't conflict with and no lower-priority conflicting pseudo prefers; pass 1
    takes the first free one from r0; then a preferred register of the same class replaces that choice.
    Says which test kept WANT out and what would let it in. Returns True if it printed."""
    view = find_reg_view(wd, func).get(pseudo)
    if not view or not rec or rec[0] != "G":
        return False
    f = view[0]
    lo = set(range(8))
    used = f["used1"] & lo
    pass0 = (f["sofar"] - f["used1"] - f["someone"]) & lo
    found = f"found {rname(f['first'])} in pass {f['passno']}" if f["first"] >= 0 else "found nothing"
    print(f"    find_reg (global.c) for {nm(pseudo)}: conflicts {regset(used)}; pass 0 may reuse registers"
          f" already in use {regset(f['sofar'] & lo)} minus conflicts and those a later conflicting pseudo"
          f" prefers {regset(f['someone'] & lo)}, leaving {regset(pass0)}; it {found}"
          + (f", then its preference moved it to {rname(f['final'])}" if f["final"] != f["first"] else "") + ".")
    if want in f["used1"]:
        print(f"      {rname(want)} conflicts: a pseudo allocated earlier or a hard-register use overlaps its life.")
        return True
    if want in f["pref"] | f["cpref"]:
        return True
    if want in f["someone"]:
        print(f"      {rname(want)} is held back in pass 0 because a conflicting pseudo allocated later prefers it.")
        return True
    if want not in f["sofar"]:
        hard = hard_of(wd)
        later = sorted((x for x, r in recs.items() if r[0] == "G" and r[1] > rec[1] and
                        hard.get(x, r[6]) == want), key=lambda x: recs[x][1])
        rom = rom_hard(wd)
        later.sort(key=lambda x: (rom.get(x) != want, recs[x][1]))
        txt = (f"      {rname(want)} was not in use yet, so pass 0 could not reuse it, and pass 1 takes the lowest free"
               f" register ({rname(f['first'])}).")
        if later:
            y = later[0]
            txt += (f" {nm(y)} takes {rname(want)} later (allocated {recs[y][1] + 1}, priority {recs[y][2]}"
                    + (", the ROM keeps it there too" if rom.get(y) == want else "") + f"). Had {nm(y)} been"
                    f" allocated before {nm(pseudo)}, {rname(want)} would have been in use and pass 0 would have"
                    f" picked it.")
            print(txt)
            print(f"      to flip the order (priority = floor_log2(refs) * refs / live * 10000): "
                  + "; or ".join(flip_levers(nm, y, recs[y], pseudo, rec, wd)))
            print(f"      what-if: AGBCC_PRI={y}:+{max(rec[2] - recs[y][2] + 1, 1)}")
        else:
            print(txt)
        return True
    return False


def flip_levers(nm, lo, lo_rec, hi, hi_rec, wd=None):
    """What would let LO outrank HI in global alloc: LO's uses or life, or HI's. Lives are given as flow
    counts them in the source; a constant register's is doubled before allocation (see life_note)."""
    _, _, lpri, lrefs, llive, _, _ = lo_rec
    _, _, hpri, hrefs, hlive, _, _ = hi_rec
    fl = live_lengths(wd, "flow") if wd else {}
    ld = 2 if fl.get(lo) and llive == 2 * fl[lo] else 1
    hd = 2 if fl.get(hi) and hlive == 2 * fl[hi] else 1
    out = []
    r = next((r for r in range(lrefs + 1, lrefs + 40) if g_priority(r, llive) > hpri), None)
    if r:
        out.append(f"{nm(lo)} with {r} refs instead of {lrefs} ({g_priority(r, llive)})")
    lb = lrefs.bit_length() - 1
    if lb:
        m = lb * lrefs * 10000 // (hpri + 1)
        if 0 < m < llive:
            out.append(f"{nm(lo)} living {m // ld} insns or fewer instead of {llive // ld}"
                       + (" (flow's count; it is doubled as a constant register)" if ld == 2 else "")
                       + (", or the same life if it were not a constant (killing REG_EQUIV)" if ld == 2
                          and lb * lrefs * 10000 // (llive // 2) > hpri else ""))
    hb = hrefs.bit_length() - 1
    if hb:
        m = hb * hrefs * 10000 // max(lpri, 1) + 1
        out.append(f"{nm(hi)} living {-(-m // hd)}+ insns instead of {hlive // hd}"
                   + (" (flow's count; doubled as a constant register)" if hd == 2 else ""))
    if hrefs > 1:
        r = next((r for r in range(hrefs - 1, 0, -1) if g_priority(r, hlive) < lpri), None)
        if r:
            out.append(f"{nm(hi)} with {r} refs instead of {hrefs}")
    return out


def explain_alloc(wd, func, pseudo, want, got, nm, at):
    """Why PSEUDO got hard register GOT and not WANT (the ROM's)."""
    recs, conf, hard = alloc_records(wd, func), conflicts(wd), hard_of(wd)
    rec = recs.get(pseudo)
    path = os.path.join(wd, "names.txt")
    total = sum(1 for ln in (open(path) if os.path.exists(path) else [])
                if ln.startswith("G " + dump_func(wd, func) + " "))
    if rec and rec[0] == "G":
        _, rank, pri, refs, live, calls, _ = rec
        print(f"    {nm(pseudo)} is a global-alloc pseudo: allocated {rank + 1} of {total} by priority {pri}"
              f" ({refs} refs{ref_note(wd, pseudo, refs)}, live {live} insns, crosses {calls} call(s)); it got"
              f" {rname(got)}, the ROM has {rname(want)}." + life_note(wd, pseudo, live))
    elif rec:
        _, blk, pri, birth, death, refs, _ = rec
        print(f"    {nm(pseudo)} is a local-alloc pseudo (block {blk}, priority {pri}, {refs} refs);"
              f" it got {rname(got)}, the ROM has {rname(want)}.")
    else:
        print(f"    {nm(pseudo)} has no allocation record; it got {rname(got)}, the ROM has {rname(want)}.")
    if rec and rec[0] == "G" and rec[6] != got:
        before = "no register" if rec[6] < 0 else rname(rec[6])
        print(f"    global alloc gave it {before}; reload then needed {before} for its own spill registers, evicted"
              f" it and re-placed it in {rname(got)} (retry_global_alloc -> find_reg: pass 0 only reuses a register"
              f" already in use that no conflicting pseudo prefers, pass 1 takes the first free one, r0 to r15)."
              if rec[6] >= 0 else
              f"    global alloc left it without a register; reload placed it in {rname(got)} afterwards"
              f" (retry_global_alloc -> find_reg, first free register in r0 to r15 order).")
        print(f"    so {rname(got)} vs {rname(want)} is decided by which registers are in use and free at that"
              f" point. what-if: AGBCC_FORCE={pseudo}:{want}")
        rom = rom_hard(wd)
        pc0 = conf.get(pseudo, (set(), set()))[0]
        for x in sorted(y for y in rom if rom[y] == got and y != pseudo)[:3]:
            if x in pc0:
                print(f"    the ROM keeps {nm(x)} in {rname(got)} too, and the two conflict in the draft as well")
            else:
                print(f"    the ROM keeps {nm(x)} in {rname(got)}; in the draft {nm(x)} and {nm(pseudo)} are never live"
                      f" at the same time, so {rname(got)} was free. In the original {nm(x)} must still be live"
                      f" somewhere during {nm(pseudo)}'s life (`life {x}` vs `life {pseudo}`)")
        return
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
        print(f"    {rname(want)} was already taken during its life (local alloc or a fixed use runs first): "
              + (", ".join(what) if what else "holder not found in its span"))
        print(f"    lever: shorten {nm(pseudo)}'s life or move the other value's, so they don't overlap in"
              f" {rname(want)}; or make the other value land elsewhere")
        return
    takers = [x for x in pc if hard.get(x, recs.get(x, (0,) * 7)[6]) == want and x != pseudo]
    before = [x for x in takers if recs.get(x, ("L",))[0] == "L" or
              (recs.get(x) and rec and rec[0] == "G" and recs[x][1] < rec[1])]
    if before:
        ds = ", ".join(f"{nm(x)} (priority {recs[x][2]}{life_note(wd, x, recs[x][4]) if recs[x][0] == 'G' else ''})"
                       for x in sorted(before)[:5] if x in recs)
        print(f"    {rname(want)} went to a conflicting pseudo allocated earlier: {ds}")
        top = max((x for x in before if recs.get(x, ("L",))[0] == "G"), key=lambda x: recs[x][2], default=None)
        if top is not None and rec[0] == "G":
            print(f"    to flip the order (priority = floor_log2(refs) * refs / live * 10000; flow counts refs"
                  f" inside loops weighted by depth): " + "; or ".join(flip_levers(nm, pseudo, rec, top, recs[top], wd)))
            print(f"    what-if: AGBCC_PRI={pseudo}:+{recs[top][2] - rec[2] + 1} or AGBCC_FORCE={pseudo}:{want}")
        else:
            print(f"    lever: raise {nm(pseudo)}'s priority above theirs (more uses or a shorter life), or end"
                  f" the overlap; what-if: AGBCC_PRI={pseudo}:+N or AGBCC_FORCE={pseudo}:{want}")
        return
    # the rival: a conflicting pseudo the ROM keeps in {rname(got)}; we took that register first
    rom = rom_hard(wd)
    rivals = [x for x in pc if rom.get(x) == got and hard.get(x, recs.get(x, (0,) * 7)[6]) != got]
    if rec and rec[0] == "G":
        for x in sorted(rivals, key=lambda x: -recs.get(x, (0, 0, 0))[2])[:2]:
            xr = recs.get(x)
            if not xr or xr[0] != "G":
                continue
            prefs = preferences(wd).get(pseudo, [])
            if xr[1] < rec[1]:
                print(f"    the ROM keeps {nm(x)} in {rname(got)}; it conflicts with {nm(pseudo)} and was allocated"
                      f" first (priority {xr[2]} vs {pri}), but it took {rname(hard.get(x, xr[6]))}, so"
                      f" {rname(got)} was left for {nm(pseudo)}. The first cause is {nm(x)}'s placement: ask `why`"
                      f" on an insn that uses it.")
                return
            print(f"    the ROM keeps {nm(x)} in {rname(got)}; it conflicts with {nm(pseudo)}, and global alloc ran"
                  f" {nm(pseudo)} first (priority {pri} vs {xr[2]}: {xr[3]} refs, live {xr[4]}{life_note(wd, x, xr[4])})," +
                  (f" so it took {rname(got)}, its preferred register (preferences: {' '.join('r%d' % h for h in prefs)})."
                   if got in prefs else f" so it took {rname(got)}.") +
                  f" {nm(x)} then got {rname(hard.get(x, xr[6]))}.")
            if got not in prefs:
                explain_find_reg(wd, func, pseudo, want, got, nm, recs, rec)
            src = pref_sources(wd, pseudo, got) if got in prefs else []
            if src:
                u, other, dest, dead = src[-1]
                print(f"      {nm(pseudo)} inherits {rname(got)} at {at(u)}: it sets {nm(dest)} while"
                      f" {', '.join(nm(n) for n in dead)} die (REG_DEAD notes in that order). expand_preferences"
                      f" (global.c) merges preferences across such an insn note by note: each dying value and the"
                      f" set value swap preferences, so a value whose note comes after another's also receives what"
                      f" that one brought in. Lever: a different note order (combine rewriting that insn rewrites"
                      f" its notes), or {nm(pseudo)} not dying there.")
            elif got in prefs and got not in preferences(wd).get(x, []):
                print(f"      (the preference is inherited: expand_preferences, global.c, merges preferences"
                      f" across an insn where one pseudo dies and another is set, so it spreads along value chains)")
            elif got in prefs:
                print(f"      (both prefer {rname(got)}: expand_preferences, global.c, merges preferences across an insn"
                      f" where one pseudo dies and another is set, so a preference spreads along value chains)")
            lev = flip_levers(nm, x, xr, pseudo, rec, wd)
            print(f"    to flip the order (priority = floor_log2(refs) * refs / live * 10000; flow counts refs"
                  f" inside loops weighted by depth): " + "; or ".join(lev))
            print(f"    what-if: AGBCC_PRI={x}:+{max(pri - xr[2] + 1, 1)} (should match if this is the only cause)")
            return
    if explain_find_reg(wd, func, pseudo, want, got, nm, recs, rec):
        return
    who = "local alloc" if rec and rec[0] == "L" else "global alloc"
    print(f"    {rname(want)} was free for it; {who} preferred {rname(got)} (copy/tie preferences or register order)."
          f" If the ROM's value was instead a constant that reload loaded (an integer address or offset"
          f" rather than a symbol, or a variable left without a register), it would take the rotation's"
          f" register: try `spill`. what-if: AGBCC_FORCE={pseudo}:{want}")


def reload_records(wd, func):
    """[(k, uid, spill_reg, line, label)] from AGBCC_DUMPPRI's R lines: reload register choices in order."""
    func = dump_func(wd, func)
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


PIN_RE = re.compile(r'\s+(?:asm|__asm__)\s*\(\s*"(?:r\d+|sb|sl|fp|ip|lr)"\s*\)')
BARRIER_RE = re.compile(r'^\s*(?:asm|__asm__)(?:\s+(?:volatile|__volatile__))?\s*\(\s*""[^;]*\)\s*;\s*(?://.*)?$')


def concession_sites(text):
    """[(line index, kind, removal)] for each register pin (`register T x asm("rN")`) and each empty asm
    barrier statement (`asm("" : "+r"(x));`) in TEXT; removal(lines) returns the lines without it."""
    lines = text.split("\n")
    out = []
    for i, ln in enumerate(lines):
        if ln.lstrip().startswith("//"):
            continue
        if PIN_RE.search(ln) and "register" in ln:
            out.append((i, "pin", lambda ls, i=i: ls[:i] + [PIN_RE.sub("", ls[i], count=1)] + ls[i + 1:]))
        elif BARRIER_RE.match(ln):
            out.append((i, "barrier", lambda ls, i=i: ls[:i] + ls[i + 1:]))
    return out


def cmd_pins(src, func, env):
    """Phase 3 helper: remove each concession (register pin or empty asm barrier) in turn and report the
    cost, then greedily remove every one that does not add differences, and write the result."""
    def run(text, tag):
        path = os.path.join(ROOT, "build", "scope", "drafts", ".pins", f"{func}_{tag}.c")
        os.makedirs(os.path.dirname(path), exist_ok=True)
        open(path, "w").write(text)
        try:
            n, _ = score(os.path.relpath(path, ROOT), func, env)
        except SystemExit:
            return None
        return n
    text = open(os.path.join(ROOT, src)).read()
    sites = concession_sites(text)
    if not sites:
        print(f"{src}: no register pins or empty asm barriers")
        return 0
    base = run(text, "base")
    print(f"{func}: {base} differing insns with all {len(sites)} concessions"
          f" ({sum(k == 'pin' for _, k, _ in sites)} pins, {sum(k == 'barrier' for _, k, _ in sites)} barriers)")
    lines = text.split("\n")
    cost = []
    for n, (i, kind, rm) in enumerate(sites):
        r = run("\n".join(rm(lines)), f"one{n}")
        cost.append(r)
        d = "compile fails" if r is None else f"{r} differ ({r - base:+d})"
        print(f"  line {i + 1:4d} {kind:7s} {lines[i].strip()[:70]:70s} -> {d}")
    ls = lines
    for i, _, rm in sorted(sites, key=lambda t: -t[0]):
        ls = rm(ls)
    r = run("\n".join(ls), "none")
    print(f"  all removed: {'compile fails' if r is None else f'{r} differ'}")
    # greedy: cheapest first, keep a removal if the count does not grow; remove from the bottom up so
    # earlier line numbers stay valid
    order = sorted(range(len(sites)), key=lambda n: (cost[n] is None, cost[n] if cost[n] is not None else 0))
    keep_out, cur, best = set(), lines, base
    for n in order:
        if cost[n] is None:
            continue
        trial = set(keep_out) | {n}
        ls = lines
        for m in sorted(trial, key=lambda m: -sites[m][0]):
            ls = sites[m][2](ls)
        r = run("\n".join(ls), f"greedy{n}")
        if r is not None and r <= best:
            keep_out, cur, best = trial, ls, r
    out = os.path.join(ROOT, "build", "scope", "drafts", f"{func}.fewer_pins.c")
    open(out, "w").write("\n".join(cur))
    left = len(sites) - len(keep_out)
    print(f"  greedy: removed {len(keep_out)} of {len(sites)} with no added differences -> {best} differ,"
          f" {left} concessions left; written to {os.path.relpath(out, ROOT)}")
    print("  A removal that costs instructions shows where the C's shape still differs: ask `why` on the"
          " first new difference in that variant (build/scope/drafts/.pins/).")
    return 0


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


def explain_move2add(wd, uid, row, at):
    """ROM loads a constant where the draft adds to a register: reload's move2add (reload1.c)."""
    md = re.match(r"(?:adds|subs) r(\d+), #(0x[0-9a-f]+|\d+)$", row["draft"] or "")
    mr = re.match(r"(?:ldr r(\d+), =|movs r(\d+), #)", row["rom"] or "")
    if not (md and mr):
        return
    reg = int(md.group(1))
    seq = insn_list(wd, "greg")
    idx = next((i for i, (u, _, _) in enumerate(seq) if u == uid), None)
    if idx is None:
        return
    src = None
    for u, _, r in reversed(seq[:idx]):
        if re.match(r"\(set \(reg:\w+ %d r%d\) \(const_int (-?\d+)" % (reg, reg), r):
            src = (u, int(re.match(r".*?\(const_int (-?\d+)", r).group(1)))
            break
    known = f", set to {src[1]:#x} at {at(src[0])}" if src else ""
    print(f"  reload's move2add (reload_cse_move2add, reload1.c) rewrote a constant load as an add from"
          f" r{reg}, which still held a known constant{known}. It forgets everything at a CODE_LABEL, so"
          " the ROM had a label in between: an if/else arm, an inline function's early return, or a loop"
          " start. lever: restructure that stretch so control flow joins there (e.g. an inline helper"
          " with if/return instead of a plain if)")


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
        rr, dr = reg_nums(row["rom"]), reg_nums(row["draft"])
        hmap = hard_of(wd)
        pre = next((r for u, _, r in insn_list(wd, "lreg") if u == uid), None)
        for a, b in zip(rr, dr):
            if a == b:
                continue
            ps = [x for x in regs_of(pre) if hmap.get(x, x) == int(b)]
            alloc_diffs.append((ps[0] if ps else None, int(a), int(b)))
            print(f"  ROM {rname(a)} where the draft has {rname(b)}" + (f" = pseudo {nm(ps[0])}" if ps else ""))
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
        explain_move2add(wd, uid, row, at)
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
    for ps, want, got in alloc_diffs:
        # even when cse put this pseudo here, its register is allocation's choice: explain both
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
            print(f"  reload: {rname(got)} is reload register choice #{k}{served} (value {lab}). Reload hands out"
                  f" spill registers round-robin (last_spill_reg, reload1.c), so the ROM's {rname(want)} means a"
                  f" different rotation state here: a different number of earlier reloads or spill set.")
            print(f"    what-if: AGBCC_SPILL={k}:D shifts the rotation before this choice;"
                  f" `scope.py FILE.c spill` searches all of them")
        else:
            print(f"  {rname(got)} is not a pseudo before allocation and no reload record names it here")
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
    if rest and rest[0] == "vs":  # FILE.c vs OTHER.c: allocation side by side
        other = rest[1]
        other = os.path.relpath(os.path.join(ROOT, other) if not os.path.isabs(other) else other, ROOT)
        func = rest[2] if len(rest) > 2 else nonmatching_func(src)
        return cmd_vs(src, other, func, env)
    if rest and rest[0] in ("insn", "uses", "life"):  # FILE.c insn UID / uses REG... / life NAME... [FUNC]
        func = nonmatching_func(src)
        if len(rest) > 2 and rest[-1] in (func, rom_name(src, func) if func else None):
            rest = rest[:-1]
        if not func:
            sys.exit(f"name the function: scope.py {rest[0]} ... FUNC")
        os.makedirs(os.path.join(ROOT, "build", "scope"), exist_ok=True)
        open(os.path.join(ROOT, "build", "scope", "LAST"), "w").write(func)
        compile_draft(src, func, env)
        if rest[0] == "insn":
            cmd_insn(int(rest[1]), func)
        elif rest[0] == "life":
            cmd_life(rest[1:], func)
        else:
            names = [a for a in rest[1:] if not a.isdigit()]
            cmd_uses([int(a) for a in rest[1:] if a.isdigit()], func,
                     [a for a in names if a in PASSES] or ["cse", "cse2", "combine", "lreg"])
        return 0
    if rest and rest[0] == "pins":  # FILE.c pins: cost of removing each pin / barrier, greedy removal
        func = rest[1] if len(rest) > 1 else nonmatching_func(src) or os.path.basename(src)[:-2]
        return cmd_pins(src, func, env)
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
