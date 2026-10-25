#!/usr/bin/env python3
"""Look inside agbcc at one draft: what really differs from the ROM, and where it came from.

  scope.py FILE.c [FUNC]        compile the draft, align it with the ROM, print the real differences
  scope.py FILE.c KNOB=V ...    the same with instrumented-compiler knobs set (a what-if)
  scope.py FILE.c spill         is the whole difference reload's spill rotation? (searches AGBCC_SPILL)
  scope.py FILE.c probe         is it one cse/gcse decision? (CSE_NOCANON, CSE_NOFOLLOW, AGBCC_NOPRE)
  scope.py FILE.c try V.c... | try KNOB=V...   score many variants (whole files, or one knob each;
                                shell globs and {a,b} expansion make the list), ~12 ms each, best first
  scope.py FILE.c why all       why on the first differing insn of each hunk
  scope.py FILE.c why UID       why insn UID has the operands it has: pass by pass, and for cse the
                                equivalence class and the canonical-register decision, with C lines
  scope.py FILE.c life NAME...  the insns flow counts toward a value's live length, by C line
  scope.py A.c vs B.c [FUNC]    two drafts' register allocation side by side (rank, priority, refs/live)
  scope.py FILE.c pins          cost of removing each register pin / asm barrier; greedy free removal
  scope.py census [CALLEE]      every pin/barrier in matched code grouped by the call it serves, with
                                clean and draft sites of the same call: missing helpers show up as groups
  scope.py rom FUNC|ADDR        FUNC's ROM listing with calls and literal symbols named (no draft needed)
  scope.py fields OFF... [STRUCT]  structs in include/ with members at these offsets (agbcc's own layout),
                                or STRUCT's member at each offset
  scope.py like FUNC|ADDR [N]   matched functions whose ROM shares the most rare 3-insn windows with
                                FUNC's, with the calls and constants they share: whose C to read first
  scope.py insn UID [FUNC]      one RTL insn through every pass (from the last compile of FUNC)
  scope.py uses REG... [PASS...] [FUNC]   insns (uid@C line) using each pseudo after each pass

One compile with the instrumented agbcc (-g -dp -da) gives the code, the C line of every
instruction (DWARF), the RTL insn behind it (-dp) and the RTL after every pass (-da). Both
sides are disassembled the same way; literal loads compare by value and branches by where
they land after alignment, so offsets that only moved because code shifted are hidden and
counted rather than listed. Each differing draft instruction is tagged with its C line,
its insn uid and the pass that created that insn.

Speed: the first call starts a resident server (scope.py serve; SCOPE_SERVER=0 opts out) that keeps
the ROM and symbols loaded; search commands (probe, spill, pins, try) compile each variant through
an agbcc fork server in ~12 ms. scope.py fastcheck N checks those paths against ld.

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
import socket
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.dirname(HERE)
SOCK = os.path.join(os.path.dirname(TOOLS), "build", "scope", "serve.sock")
FORWARD = ("AGBCC_", "CSE_", "SCOPE_", "CPP")  # environment a command reads, sent with it to the server


def _client():
    """Run this command in the resident `scope.py serve` when one is up: it keeps the ROM, symbols,
    capstone and inventory warm, so a command costs its own work and not two Python startups and
    cache loads. Without one (or with SCOPE_SERVER=0), start one in the background for next time
    and run here. Returns the exit code, or None to run locally."""
    argv = sys.argv[1:]
    if (os.environ.get("SCOPE_SERVER") == "0" or os.environ.get("SCOPE_IN_SERVER") or os.environ.get("SCOPE_REEXEC")
            or (argv and argv[0] == "serve")):
        return None
    if len(SOCK) > 100:  # AF_UNIX path limit
        return None
    c = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    try:
        c.connect(SOCK)
    except OSError:
        c.close()
        try:
            os.makedirs(os.path.dirname(SOCK), exist_ok=True)
            log = open(SOCK[:-5] + ".log", "a")
            subprocess.Popen([sys.executable, os.path.abspath(__file__), "serve"], stdin=subprocess.DEVNULL,
                             stdout=log, stderr=log, start_new_session=True,
                             env={k: v for k, v in os.environ.items() if not k.startswith(FORWARD[:3])})
        except OSError:
            pass
        return None
    env = {k: v for k, v in os.environ.items() if k.startswith(FORWARD)}
    c.sendall((json.dumps({"argv": argv, "cwd": os.getcwd(), "env": env}) + "\n").encode())
    out = sys.stdout.buffer
    tail = b""
    while True:
        chunk = c.recv(65536)
        if not chunk:
            break
        data = tail + chunk
        cut = max(0, len(data) - 64)  # keep the end back: the exit trailer may straddle chunks
        out.write(data[:cut])
        out.flush()
        tail = data[cut:]
    c.close()
    m = re.search(rb"\0SCOPE-EXIT (-?\d+|RESTART)\n$", tail)
    if not m:
        out.write(tail)
        return 1
    out.write(tail[:m.start()])
    out.flush()
    return None if m.group(1) == b"RESTART" else int(m.group(1))


if __name__ == "__main__":
    try:
        _code = _client()
    except BrokenPipeError:  # `scope.py ... | head`
        os.dup2(os.open(os.devnull, os.O_WRONLY), sys.stdout.fileno())
        _code = 0
    if _code is not None:
        sys.exit(_code)

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

INSTR = os.path.join(ROOT, "build", "scope", "agbcc", "gcc", "agbcc")  # tools/scope/build_agbcc.sh
PASSES = ["rtl", "jump", "cse", "addressof", "gcse", "loop", "cse2", "flow", "combine", "regmove",
          "lreg", "greg", "jump2", "mach"]
PASS_NOTE = {"greg": "greg/reload", "mach": "mach (late split/reorg)"}
CS = capstone.Cs(capstone.CS_ARCH_ARM, capstone.CS_MODE_THUMB)


# ---------------------------------------------------------------- compile

CACHE = os.path.join(ROOT, "build", "scope", "cache")
# SCOPE_RUN=NAME keeps a worker's per-function traces apart from another worker on the same function
WORK = os.path.join(ROOT, "build", "scope", os.environ.get("SCOPE_RUN", ""))
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
    d = os.path.join(WORK, func)
    os.makedirs(d, exist_ok=True)
    return d


def nonmatching_func(src):
    """The function a draft file is about: the first definition inside #ifdef NONMATCHING."""
    text = open(os.path.join(ROOT, src)).read()
    run = os.environ.get("SCOPE_RUN")  # a campaign worker's assignment, when it is drafted in this file
    if run and any(re.search(rf"^[A-Za-z_][\w \*]*?\b{re.escape(run)}\s*\(", b, re.M)
                   for b in re.findall(r"#ifdef NONMATCHING(.*?)#else", text, re.S)):
        return run
    m = re.search(r"#ifdef NONMATCHING(.*?)#else", text, re.S)
    body = m.group(1) if m else text  # a matched file: its last non-inline, non-static definition
    defs = re.findall(r"^[A-Za-z_][\w \*]*?\b(\w+)\s*\([^;{]*\)\s*\{", body, re.M)
    defs = [d for d in defs if not re.search(r"\b(inline|static)\b.*\b" + d + r"\b", body)]
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
    for alias in re.findall(r"^#define\s+" + re.escape(func) + r"\s+(\w+)\s*$", text, re.M):
        hit = hit or (alias if alias in addr else None)  # `#define Name sub_ADDR` in a matched file
    if hit is None and re.fullmatch(r"sub_[0-9A-Fa-f]{8}", func):  # an address name renamed since
        hit = next((n for n, a in addr.items() if a == int(func[4:], 16)), None)
    return hit or func


def kref(wd, func, x):
    """How a what-if knob should name pseudo X: its C variable name when that is unique (names survive
    edits; numbers shift), else the number."""
    pn = pseudo_names(wd, func)
    n = pn.get(x)
    if n and re.fullmatch(r"[A-Za-z_]\w*(#\d+)?", n) and list(pn.values()).count(n) == 1:
        return n
    return str(x)


def resolve_knobs(src, func, env):
    """AGBCC_PRI / AGBCC_FORCE may name pseudos by C variable (`table:+214`): compile once without them to
    learn the numbers, then substitute."""
    env = dict(env or {})
    for k in ("AGBCC_PRI", "AGBCC_FORCE"):
        if k not in env and os.environ.get(k):
            env[k] = os.environ[k]
    keys = [k for k in ("AGBCC_PRI", "AGBCC_FORCE") if re.search(r"(^|,)[A-Za-z_]", env.get(k, ""))]
    if not keys:
        return env
    saved = {k: os.environ.pop(k) for k in keys if k in os.environ}
    try:
        compile_draft(src, func, {k: v for k, v in env.items() if k not in keys})
    finally:
        os.environ.update(saved)
    wd = workdir(func)
    rev = {}
    for p, n in pseudo_names(wd, func).items():
        rev.setdefault(n, []).append(p)
    def sub(tok):
        name, rest = tok.split(":", 1)
        if name.isdigit():
            return tok
        if len(rev.get(name, [])) != 1:
            sys.exit(f"{name}: " + (f"names pseudos {rev[name]}; use a number" if name in rev else
                                     f"no pseudo by that name (known: {', '.join(sorted(rev))[:300]})"))
        return f"{rev[name][0]}:{rest}"
    out = dict(env, **{k: ",".join(sub(t) for t in env[k].split(",")) for k in keys})
    print("  knobs: " + " ".join(f"{k}={out[k]}" for k in keys), file=sys.stderr)
    return out


def compile_draft(src, func, env=None):
    """Compile with the instrumented agbcc; return (bytes, address, info) for FUNC.
    ENV adds knob settings (CSE_NOCANON=N, AGBCC_FORCE=..., see tools/scope/build_agbcc.sh)."""
    env = resolve_knobs(src, func, env)
    wd = workdir(func)
    pre = preprocess(src, env)
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
    # The dumps hold every function of the unit, and insn uids restart in each one: keep only FUNC's
    # section, so uid lookups (why, insn, probe, uses) cannot land in another function.
    for p in PASSES:
        path = os.path.join(wd, "u.i." + p)
        if not os.path.exists(path):
            continue
        dump = open(path).read()
        parts = re.split(r"(?m)^(?=;; Function \w+)", dump)
        mine = [x for x in parts if re.match(r";; Function (\w+)", x)
                and re.match(r";; Function (\w+)", x).group(1) in (func, rom_name(src, func))]
        if mine and len(parts) > 2:
            open(path, "w").write(parts[0] + "".join(mine))
    asm = open(os.path.join(wd, "u.s")).read()
    text = re.sub(r"^(\w+):\n", lambda m: f"{m.group(1)}:\n\t.global {m.group(1)}__addr\n{m.group(1)}__addr:\n"
                  if not m.group(1).startswith(".") else m.group(0), asm, flags=re.M)
    # Inline the macros a top-level asm() includes: gas sometimes misreads the included file's first
    # line (an @ comment) depending on where the .include falls in its input buffer, so the same C
    # failed or assembled depending on the -g/-dp output's size.
    macros = open(os.path.join(ROOT, "asm", "macros.inc")).read()
    text = re.sub(r'^\s*\.include\s+"asm/macros\.inc"\s*$', lambda m: macros, text, flags=re.M)
    # agbcc -g can leave a DWARF location naming a label it deleted (`.4byte .LC2` in .debug_info
    # only); the real build has no -g, so define such labels when no code refers to them.
    dbg = text.find("\t.section\t.debug")
    if dbg >= 0:
        defined = set(re.findall(r"^(\.L\w+):", text, re.M))
        code_refs = set(re.findall(r"(\.L\w+)\b", text[:dbg]))
        dangling = sorted(set(re.findall(r"(\.L\w+)\b", text[dbg:])) - defined - code_refs)
        text += "".join(f"\t.set\t{n}, 0\n" for n in dangling)
    # A call to another function defined in this unit lands where that function's draft sits, which
    # earlier drafts of other sizes move: send such calls to the callee's ROM address instead, so
    # they compare like calls to outside symbols (a phantom bl difference otherwise).
    _, addr_all, _ = inventory()
    own = (func, rom_name(src, func))
    local = set(re.findall(r"^\t\.type\t\s*(\w+),function", text, re.M)) - set(own)
    for n in sorted(local):
        a_rom = addr_all.get(n)
        if a_rom is not None and re.search(rf"^\tbl\t{n}\b", text, re.M):
            text = re.sub(rf"^(\tbl\t){n}\b", rf"\g<1>{n}__rom", text, flags=re.M)
            text += f"\t.set\t{n}__rom, 0x{a_rom | 1:08X}\n"
    open(os.path.join(wd, "a.s"), "w").write(text + "\t.text\n\t.align\t2, 0\n")
    rel = os.path.relpath(wd, ROOT)
    build.sh(build.AS + ["-o", f"{rel}/u.o", f"{rel}/a.s"])
    # Place FUNC at its ROM address from the object itself (fast.py: relocations, mapping symbols and
    # DWARF lines in Python); the linker path below remains for objects it cannot resolve.
    import fast
    obj = open(os.path.join(wd, "u.o"), "rb").read()
    _, addr_all, _ = inventory()
    a = addr_all[rom_name(src, func)]
    code, start, funcs, missing = fast.place(obj, own, elf_syms(), a)
    if code is not None and not missing and os.environ.get("SCOPE_LINK") != "1":  # SCOPE_LINK=1: the ld path
        names = [n for _, n in funcs]
        obj_name = next(n for n in own if n in names)
        return code, a, {"obj_offset": start, "wd": wd, "env": env or {}, "obj_name": obj_name,
                         "data": fast.place.data, "lines": fast.dwarf_lines(obj)}
    # Link so FUNC itself sits at its ROM address: the drafts before it in a multi-function unit
    # compile to their own sizes, and linking the first function at its address would shift every
    # call FUNC makes to an outside symbol (bl targets off by the size difference).
    pre_syms = sorted((v & ~1, n) for n, (v, t) in symtab(os.path.join(wd, "u.o")).items() if t == "FUNC")
    want = next((v for v, n in pre_syms if n in (func, rom_name(src, func))), None)
    link_at = None
    if want is not None and pre_syms:
        _, addr0, _ = inventory()
        a0 = addr0.get(rom_name(src, func))
        if a0 is not None:
            link_at = a0 - (want - pre_syms[0][0])
    check.link_object(f"{rel}/u.o", f"{rel}/u.elf", syms=elf_syms(), address=link_at)
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


def rom_data(func, addr, code):
    """The ROM function's data ranges: its generated assembly's, plus the literal pools its
    pc-relative loads reach (a function the disassembler left as .incbin has no labels at all)."""
    return sorted(set(rom_data_ranges(func, addr) or []) | set(rom_pool_ranges(code, addr)))


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


def draft_annotations(wd, func, rows, obj_offset, lines=None):
    """C line and insn uid for each draft code row. LINES: {text offset: (file, line)} if known."""
    if lines is None:
        lines = {}
        out = build.sh(["arm-none-eabi-objdump", "--dwarf=decodedline", os.path.join(wd, "u.o")])
        for ln in out.splitlines():
            p = ln.split()
            if len(p) >= 3 and p[1].isdigit() and re.match(r"(0x[0-9a-f]+|0)$", p[2]):
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
    rom = decode(rom_code, addr, rom_data(rom_name(src, func), addr, rom_code))
    mdata = info["data"] if "data" in info else mapping_ranges(os.path.join(wd, "u.o"), info["obj_offset"], len(mine))
    dr = decode(mine, addr, mdata)
    ann = draft_annotations(wd, info["obj_name"], dr, info["obj_offset"], info.get("lines"))
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
    nr = sum(1 for _, _, t in pairs if t in "-~")
    nb = sum(1 for _, _, t in pairs if t == "→")
    nm = sum(1 for _, _, t in pairs if t in "+~")
    same = rom_code == mine
    rel = os.path.relpath(wd, ROOT)
    knobs = " ".join(f"{k}={v}" for k, v in info.get("env", {}).items())
    print(f"{func}  {src}  ROM {len(rom_code)} B / draft {len(mine)} B  compile {t_compile:.1f}s"
          + (f"  [{knobs}]" if knobs else ""))
    if same:
        print("MATCH")
        return 0
    only_targets = nb and not nr and not nm and len(rom_code) == len(mine)
    print(f"{len(hunks)} hunk(s): {nr} ROM / {nm} draft insns differ"
          + (f" (+{nb} branch(es) whose target moved"
             + ("" if only_targets else ": they follow from the others") + ")" if nb else "")
          + f"; {shifted} offset-only shifts hidden.  full listing: {rel}/diff.txt (json: diff.json)")
    if only_targets:
        # Same instructions, same size: a label sits before a different instruction. Name both targets.
        at_r = {o: i for i, (o, *_) in enumerate(rom)}
        at_m = {o: i for i, (o, *_) in enumerate(dr)}
        for x, y, t in pairs:
            if t != "→":
                continue
            tr = re.search(r"func\+0x([0-9A-Fa-f]+)", rom[x][2])
            tm = re.search(r"func\+0x([0-9A-Fa-f]+)", dr[y][2])
            if not (tr and tm):
                continue
            ir, im = at_r.get(int(tr.group(1), 16)), at_m.get(int(tm.group(1), 16))
            if ir is None or im is None:
                continue
            # identical code: the draft insn at the ROM target's offset carries its C line
            lr = ctag(at_m.get(rom[ir][0], im))[0]
            lm = ctag(im)[0]
            print(f"\nThe code is identical; only where a label sits differs. ROM `{rom[x][2]}` lands on"
                  f" `{rom[ir][2]}` (line {lr or '?'}), the draft's on `{dr[im][2]}` (line {lm or '?'}).")
            back = int(tr.group(1), 16) <= rom[x][0]
            if back and ir > im:
                print("  Loop head: the insns between the two targets run once before the loop in the ROM but"
                      " on every pass in the draft. Typical cause: a loop invariant the ROM hoisted (set"
                      " before the loop, or left unpinned for loop.c to hoist) that the draft assigns inside"
                      " the loop, often a pinned register.")
            elif back:
                print("  Loop head: the insns between the two targets run on every pass in the ROM but once"
                      " in the draft. Move that assignment into the loop body.")
            elif re.match(r"(cmp|tst|cmn)\b", dr[im][2]) or (
                    re.match(r"movs (r\d+), #", dr[im][2]) and im + 1 < len(dr)
                    and re.match(rf"cmp {re.match(r'movs (r[0-9]+)', dr[im][2]).group(1)}, ", dr[im + 1][2])):
                print("  Jump threading: the draft's branch lands on a second test of a value it has already"
                      " tested; the ROM's skips that test and goes where it leads. Which repeated tests"
                      " GCC threads depends on how each reaches RTL (an inline helper's parameter, a"
                      " macro, an explicit `p && ...`), and rewriting one test can change whether another"
                      " branch is threaded. Try the repeated tests of that value written the other way,"
                      " one at a time; a version that threads too much drops the ROM's cmp/branch pairs."
                      " First run `scope.py FILE.c probe`: threading usually follows a cse"
                      " canonical-register choice it can name. cse also carries a known value"
                      " (a `result = 0` it can test) into a jump's target only when that label"
                      " is used once and nothing falls into it: a label reached by a goto and"
                      " by fall-through stops the threading there; `return 0` paths do not.")
            else:
                print("  The join or block order differs: the insns between the two targets belong to"
                      " different blocks in the ROM and the draft. Compare the C around both lines.")
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
    print("\nnext: FILE.c why UID / why all (why these registers) | probe (one cse/gcse/loop decision?) | spill"
          " (only reload rotation?) | try V1.c V2.c... (score variants, ~12 ms each) | life NAME (live length by"
          " line) | A.c vs B.c (two drafts' allocation) | pins | insn UID | uses REG   (scope.py --help)")
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
        if not has_equiv_note(wd, x):
            return (f" [flow measured {fl}; doubled by update_equiv_regs (local-alloc.c) at its first set,"
                    f" a constant; a later non-constant set dropped the equivalence but not the doubling."
                    f" The doubling goes away only if the non-constant set comes first in the code]")
        return (f" [flow measured {fl}; doubled because it always holds a constant (REG_EQUIV):"
                f" update_equiv_regs, local-alloc.c, halves constant registers' priority]")
    return f" [flow measured {fl}; regmove changed it while rewriting copies]"


def has_equiv_note(wd, x):
    """Pseudo X still carries a REG_EQUIV note on one of its sets after local-alloc."""
    path = os.path.join(wd, "u.i.lreg")
    if not os.path.exists(path):
        return True
    return any(re.search(rf"\(set \(reg(?:/\w)*:\w+ {x}\)", b) and "REG_EQUIV" in b
               for b in re.split(r"\n\n", open(path).read()))


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
            print(f"      what-if: AGBCC_PRI={kref(wd, func, y)}:+{max(rec[2] - recs[y][2] + 1, 1)}")
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
                       + (", or the same life if it were not a constant (in C, give its variable a"
                          " non-constant value *before* the constant one in the code, e.g. reuse it for"
                          " an earlier value the ROM keeps in the same register; a non-constant set"
                          " after it drops the equivalence but keeps the doubled life; fact 20)" if ld == 2
                          and lb * lrefs * 10000 // (llive // 2) > hpri else ""))
    hb = hrefs.bit_length() - 1
    if hb:
        m = hb * hrefs * 10000 // max(lpri, 1) + 1
        out.append(f"{nm(hi)} living {-(-m // hd)}+ insns instead of {hlive // hd}"
                   + (" (flow's count; doubled as a constant register)" if hd == 2 else ""))
    if hrefs > 1:
        r = next((r for r in range(hrefs - 1, 0, -1) if g_priority(r, hlive) < lpri), None)
        if r:
            out.append(f"{nm(hi)} with {r} refs instead of {hrefs}" + (mention_lines(wd, hi) if wd else ""))
    return out


def mention_lines(wd, reg):
    """' (counted at line 43 x2 in a shift, line 47 x2)': where REG's refs come from, by C line, so a worker
    can see which mention to drop. A shift is marked: `i << 6` and `i * 64` count differently once cse and
    combine are done with them (WORKER.md)."""
    ms = ref_mentions(wd, reg)
    if not ms:
        return ""
    lines = uid_lines(wd)
    pats = {u: r for u, _, r in insn_list(wd, "regmove")}
    parts = []
    for u, d in ms:
        shift = re.search(r"\((?:ashift|ashiftrt|lshiftrt):SI \(reg(?:/\w)*:SI %d\)" % reg, pats.get(u, "")) \
            or re.search(r"\(set \(reg(?:/\w)*:SI %d\) \((?:ashift|ashiftrt|lshiftrt):" % reg, pats.get(u, ""))
        parts.append(f"line {lines.get(u, '?')}" + (f" x{d}" if d > 1 else "") + (" in a shift" if shift else ""))
    return " (counted at " + ", ".join(parts) + ")"


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
              f" point. what-if: AGBCC_FORCE={kref(wd, func, pseudo)}:{want}")
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
            print(f"    what-if: AGBCC_PRI={kref(wd, func, pseudo)}:+{recs[top][2] - rec[2] + 1} or AGBCC_FORCE={kref(wd, func, pseudo)}:{want}")
        else:
            print(f"    lever: raise {nm(pseudo)}'s priority above theirs (more uses or a shorter life), or end"
                  f" the overlap; what-if: AGBCC_PRI={kref(wd, func, pseudo)}:+N or AGBCC_FORCE={kref(wd, func, pseudo)}:{want}")
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
            print(f"    what-if: AGBCC_PRI={kref(wd, func, x)}:+{max(pri - xr[2] + 1, 1)} (should match if this is the only cause)")
            return
    if explain_find_reg(wd, func, pseudo, want, got, nm, recs, rec):
        return
    who = "local alloc" if rec and rec[0] == "L" else "global alloc"
    print(f"    {rname(want)} was free for it; {who} preferred {rname(got)} (copy/tie preferences or register order)."
          f" If the ROM's value was instead a constant that reload loaded (an integer address or offset"
          f" rather than a symbol, or a variable left without a register), it would take the rotation's"
          f" register: try `spill`. what-if: AGBCC_FORCE={kref(wd, func, pseudo)}:{want}")


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


_FAST = {}
_ROMDEC = {}


_PRE = {}


def preprocess(src, env=None):
    """The unit's preprocessed text, memoised on its mtime and size (knob-only variants reuse it)."""
    st = os.stat(os.path.join(ROOT, src))
    tf = (env or {}).get("SCOPE_TRANSFORM") or os.environ.get("SCOPE_TRANSFORM")
    key = (src, st.st_mtime_ns, st.st_size, tf)
    if key not in _PRE:
        if len(_PRE) > 64:
            _PRE.clear()
        _PRE[key] = _preprocess(src, env)
    return _PRE[key]


def _preprocess(src, env=None):
    defines = build.DEFINES + ["-DNONMATCHING"]
    pre = build.sh(build.CPP + ["-nostdinc", "-undef"] + defines + ["-I", "include", "-iquote", ".", src])
    tf = (env or {}).get("SCOPE_TRANSFORM") or os.environ.get("SCOPE_TRANSFORM")
    if tf:  # a what-if over the preprocessed source: a module with transform(text) -> text
        import importlib.util
        spec = importlib.util.spec_from_file_location("scope_transform", os.path.join(ROOT, tf))
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        pre = mod.transform(pre)
    return pre


def inline_macros(text):
    """Inline the macros a top-level asm() includes (gas sometimes misreads the included file)."""
    macros = open(os.path.join(ROOT, "asm", "macros.inc")).read()
    return re.sub(r'^\s*\.include\s+"asm/macros\.inc"\s*$', lambda m: macros, text, flags=re.M)


def fast_compile(src, func, env=None):
    """(FUNC's bytes, address, data ranges) through scope's agbcc fork server (tools/scope/fast.py):
    a few ms per variant, bytes only. None when the unit cannot be served (then use compile_draft)."""
    import fast
    env = resolve_knobs(src, func, env)
    flags = tuple(build.cflags_for(src))
    _, addr_all, _ = inventory()
    addr = addr_all[rom_name(src, func)]
    key = (flags, addr)
    f = _FAST.get(key)
    if f is None:
        f = _FAST[key] = fast.Fast(INSTR, list(flags), build.AS, elf_syms(), addr,
                                   os.path.join(WORK, "cache"), fixup=inline_macros)
        os.makedirs(os.path.join(WORK, "cache"), exist_ok=True)
    knobs = {k: v for k, v in (env or {}).items() if k.startswith(("AGBCC_", "CSE_"))}
    code, err = f.bytes(preprocess(src, env), (func, rom_name(src, func)), knobs)
    if code is None:
        return None
    return code, addr, f.data


_ROMBYTES = []


def rom_bytes():
    if not _ROMBYTES:
        _ROMBYTES.append(load_rom(verify=False))
    return _ROMBYTES[0]


def fast_score(src, func, env):
    """(differing rows, count text) through the fork server, or None if the unit cannot be served."""
    r = fast_compile(src, func, env)
    if r is None:
        return None
    mine, addr, mdata = r
    _, _, sizes = inventory()
    rom_code = rom_bytes()[off(addr):off(addr) + sizes[rom_name(src, func)]]
    if mine == rom_code:
        return 0, "MATCH"
    if addr not in _ROMDEC:
        _ROMDEC[addr] = decode(rom_code, addr, rom_data(rom_name(src, func), addr, rom_code))
    pairs, _ = align(_ROMDEC[addr], decode(mine, addr, mdata))
    rows = [{"tag": t} for _, _, t in pairs]
    return sum(1 for r in rows if r["tag"] != "="), count_text(rows)


def score(src, func, env, full=False):
    """Compile with ENV and return the number of differing instructions (0 = MATCH), silently.
    Through the fork server unless FULL (callers that read the compile's dumps or traces after)."""
    import contextlib
    import io
    if not full and os.environ.get("SCOPE_FAST") != "0":  # SCOPE_FAST=0: one-shot compiles (A/B, debugging)
        r = fast_score(src, func, env)
        if r is not None:
            return r[0], {"counts": r[1]}
    mine, addr, info = compile_draft(src, func, env)
    _, _, sizes = inventory()
    rom_code = load_rom(verify=False)[off(addr):off(addr) + sizes[rom_name(src, func)]]
    if mine == rom_code:  # the listing can misalign a literal pool at a 2-byte offset; bytes decide
        return 0, info
    with contextlib.redirect_stdout(io.StringIO()):
        report(src, func, rom_code, addr, mine, info, 0)
    rows = json.load(open(os.path.join(info["wd"], "diff.json")))
    info["counts"] = count_text(rows)
    return sum(1 for r in rows if r["tag"] != "="), info


def count_text(rows):
    """'6 ROM / 6 draft (+2 moved branches)': the same count the main display prints, from diff.json rows."""
    nr = sum(1 for r in rows if r["tag"] in "-~")
    nm = sum(1 for r in rows if r["tag"] in "+~")
    nb = sum(1 for r in rows if r["tag"] == "→")
    return f"{nr} ROM / {nm} draft" + (f" (+{nb} moved branch{'es' if nb > 1 else ''})" if nb else "")


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
    pins_dir = os.path.join(ROOT, "build", "scope", "drafts", ".pins", os.environ.get("SCOPE_RUN", ""))
    memo, compiles, counts = {}, [0], {}

    def run(text, tag):
        if text in memo:  # the same variant again (greedy repeats single removals): no compile
            return memo[text]
        path = os.path.join(pins_dir, f"{func}_{tag}.c")
        os.makedirs(os.path.dirname(path), exist_ok=True)
        open(path, "w").write(text)
        compiles[0] += 1
        try:
            n, info = score(os.path.relpath(path, ROOT), func, env)
        except SystemExit:
            n, info = None, {}
        memo[text] = n
        counts[n] = info.get("counts", "MATCH" if n == 0 else "?")
        return n

    def show(n):
        return "compile fails" if n is None else f"{n} rows ({counts.get(n, '?')})"
    text = open(os.path.join(ROOT, src)).read()
    sites = concession_sites(text)
    if not sites:
        print(f"{src}: no register pins or empty asm barriers")
        return 0
    base = run(text, "base")
    print(f"{func}: {show(base)} with all {len(sites)} concessions"
          f" ({sum(k == 'pin' for _, k, _ in sites)} pins, {sum(k == 'barrier' for _, k, _ in sites)} barriers)")
    lines = text.split("\n")
    cost = []
    for n, (i, kind, rm) in enumerate(sites):
        r = run("\n".join(rm(lines)), f"one{n}")
        cost.append(r)
        d = "compile fails" if r is None else f"{show(r)}, {r - base:+d}"
        print(f"  line {i + 1:4d} {kind:7s} {lines[i].strip()[:70]:70s} -> {d}")
    ls = lines
    for i, _, rm in sorted(sites, key=lambda t: -t[0]):
        ls = rm(ls)
    r = run("\n".join(ls), "none")
    print(f"  all removed: {show(r)}")
    # greedy: cheapest first, keep a removal if the count does not grow; remove from the bottom up so
    # earlier line numbers stay valid
    order = sorted(range(len(sites)), key=lambda n: (cost[n] is None, cost[n] if cost[n] is not None else 0))
    keep_out, cur, best = set(), lines, base
    tryall = "--all" in sys.argv  # by default only removals that are free alone join the greedy pass
    for n in order:
        if cost[n] is None or (not tryall and base is not None and cost[n] > base):
            continue
        trial = set(keep_out) | {n}
        ls = lines
        for m in sorted(trial, key=lambda m: -sites[m][0]):
            ls = sites[m][2](ls)
        r = run("\n".join(ls), f"greedy{n}")
        if r is not None and r <= best:
            keep_out, cur, best = trial, ls, r
    out = os.path.join(ROOT, "build", "scope", "drafts", os.environ.get("SCOPE_RUN", ""), f"{func}.fewer_pins.c")
    os.makedirs(os.path.dirname(out), exist_ok=True)
    open(out, "w").write("\n".join(cur))
    left = len(sites) - len(keep_out)
    print(f"  greedy: removed {len(keep_out)} of {len(sites)} with no added differences -> {best} differ,"
          f" {left} concessions left; written to {os.path.relpath(out, ROOT)}  [{compiles[0]} compiles;"
          f" {'every removal tried' if tryall else 'only removals free alone tried greedily; --all tries the rest'}]")
    print("  A removal that costs instructions shows where the C's shape still differs: ask `why` on the"
          f" first new difference in that variant ({os.path.relpath(pins_dir, ROOT)}/).")
    return 0


def cmd_spill(src, func, env):
    """Is the whole difference reload's spill rotation? Greedily shift the rotation (AGBCC_SPILL) before
    each reload choice, from the first differing instruction on, keeping shifts that reduce the diff."""
    base, info = score(src, func, env, full=True)
    wd = info["wd"]
    rel = reload_records(wd, info["obj_name"])
    order = [u for u, _, _ in insn_list(wd, "greg")]
    pos = {u: i for i, u in enumerate(order)}
    rows = json.load(open(os.path.join(wd, "diff.json")))
    first = min((pos[r["uid"]] for r in rows if r["tag"] != "=" and r["uid"] in pos), default=0)
    regs = sorted({r[2] for r in rel})
    print(f"{func}: {base} differing insns; {len(rel)} reload register choices, spill registers seen"
          f" {', '.join('r%d' % r for r in regs)}")
    budget = int(os.environ.get("SCOPE_SPILL_MAX", "42"))
    spec, best, tried, accepted = [], base, 0, []
    print(f"  searching up to {budget} compiles (SCOPE_SPILL_MAX=N to change); each reload choice costs 7")
    for k, uid, reg, line, lab in rel:
        if best == 0:
            break
        if tried + 7 > budget:
            print(f"  stopped at the budget ({tried} compiles); later reload choices were not tried")
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


def cmd_probe(src, func, env):
    """Which cse / gcse / loop decision is the difference? One traced compile lists the decisions: pseudos
    make_regs_eqv makes a class's canonical register (CSE_NOCANON), jumps cse follows into their
    target (CSE_NOFOLLOW), computations gcse's PRE replaces with a copy (AGBCC_NOPRE), induction
    values loop.c strength-reduces or not (AGBCC_NOGIV / AGBCC_GIV). Then each is
    flipped alone; a flip that matches or lowers the count names the decision (rtlprobe.py's
    canon / follow / pre, inside scope's loop and with its counts)."""
    base, info = score(src, func, env, full=True)
    wd = info["wd"]
    tr = subprocess.run([INSTR] + build.cflags_for(src) + ["-g", "u.i", "-o", "/dev/null"], cwd=wd,
                        capture_output=True, text=True,
                        env=dict(os.environ, CSE_DEBUG="1", CSE_PATH="1", AGBCC_PRE_TRACE="1", AGBCC_LOOP_TRACE="1",
                                 **{k: v for k, v in info["env"].items()})).stderr
    heads, copied = set(), {}
    for m in re.finditer(r"eqv new=(\d+) old=(\d+) first=(\d+) newlast=(\d+) newfirst=(\d+) "
                         r"firstlast=(-?\d+) bb=(\d+)\.\.(\d+)", tr):
        new, old, first, nl, nf, fl, b0, b1 = map(int, m.groups())
        if fl >= 0 and (nl > b1 or nf < b0) and nl > fl:
            heads.add(new)
            copied.setdefault(new, old)
    follows = {int(m.group(1)): int(m.group(2))
               for m in re.finditer(r"follow k=(\d+) pass=1 jump=\d+ label=\d+ line=(\d+)", tr)}
    pres = [int(m.group(1)) for m in re.finditer(r"pre k=(\d+) insn=(\d+) expr=\d+", tr)]
    fnames = (func, rom_name(src, func))
    givs = {}  # insn uid -> (addr|reg, reduced?) for loop strength reduction in this function
    for m in re.finditer(r"giv fn=(\w+) insn=(\d+) type=(\w+) (?:benefit=-?\d+ lifetime=\d+ )?reduce=(\w+)", tr):
        if m.group(1) in fnames:
            givs[int(m.group(2))] = (m.group(3), m.group(4) in ("1", "combined"))
    names = pseudo_names(wd, info["obj_name"])
    lines = uid_lines(wd)
    rtl = open(os.path.join(wd, "u.i.rtl")).read()

    body = {}  # uid -> line of the inline body it came from (the note/i line notes uid_lines skips)
    cur = None
    for blk in re.split(r"\n\n", rtl):
        m = re.search(r'\(note/i \d+ \d+ \d+ \("[^"]*"\) (\d+)\)', blk)
        if m:
            cur = int(m.group(1))
            continue
        if re.search(r'\(note \d+ \d+ \d+ \("[^"]*"\) (\d+)\)', blk):
            cur = None
            continue
        m = re.match(r"\((?:insn|jump_insn|call_insn)(/i)?(?:/\w)* (\d+)", blk)
        if m and m.group(1) and cur:
            body[int(m.group(2))] = cur
    src_lines = open(os.path.join(ROOT, src)).read().split("\n")

    def owner(ln):  # the function whose definition encloses source line LN
        for i in range(min(ln, len(src_lines)) - 1, -1, -1):
            m = re.match(r"(?:static\s+)?(?:inline\s+)?[\w\s\*]+?\b(\w+)\s*\([^;]*$", src_lines[i])
            if m and not src_lines[i].startswith((" ", "\t")):
                return m.group(1)
        return None

    def sets(n):  # C lines of the insns that set pseudo N in the first RTL, with the inline they are in
        us = [int(m.group(1)) for m in re.finditer(r"\n\((?:insn|call_insn)(?:/\w)* (\d+) (?:(?!\n\().)*?"
                                                   rf"\(set \(reg(?:/\w)*:\w+ {n}\)", rtl, re.S)]
        out = []
        for u in us:
            if u in body and owner(body[u]):
                out.append(f"{body[u]} (in {owner(body[u])}, inlined at line {lines.get(u, '?')})")
            elif u in lines:
                out.append(str(lines[u]))
        return list(dict.fromkeys(out))

    def who(n):
        return f"pseudo {n}" + (f" `{names[n]}`" if n in names else "") + (
            f" set at line {', '.join(sets(n))}" if sets(n) else "")
    cands = ([("CSE_NOCANON", str(n), who(n) + (f", made canonical over {who(copied[n])}" if n in copied else ""))
              for n in sorted(heads)]
             + [("CSE_NOFOLLOW", str(k), f"cse follow of the jump at line {ln}") for k, ln in sorted(follows.items())]
             + [("AGBCC_NOPRE", str(k), f"PRE replacement {k}") for k in pres]
             + [("AGBCC_NOGIV" if red else "AGBCC_GIV", str(u),
                 f"loop strength reduction of the {'address' if t == 'addr' else 'value'} computed at line"
                 f" {lines.get(u, '?')} (insn {u}, {'reduced' if red else 'not reduced'})")
                for u, (t, red) in sorted(givs.items())])
    budget = int(env.get("SCOPE_PROBE_MAX") or os.environ.get("SCOPE_PROBE_MAX", "150"))
    print(f"{func}: {info.get('counts') or base} differ; {len(heads)} cse canonical-register choices,"
          f" {len(follows)} cse jump follows, {len(pres)} PRE replacements"
          + f", {len(givs)} loop induction values"
          + (f"; trying the first {budget} (SCOPE_PROBE_MAX=N)" if len(cands) > budget else ""))
    hits = []
    for knob, val, what in cands[:budget]:
        prev = env.get(knob)
        n, inf = score(src, func, dict(env, **{knob: f"{prev},{val}" if prev else val}))
        if n < base:
            hits.append((n, knob, val, what, inf.get("counts", "MATCH") if n else "MATCH"))
    fix = {"CSE_NOCANON": "cse made this value the canonical register of a copy class (it lives longer"
                          " than the value it was copied from), so later tests and uses read it. In C make"
                          " the ROM-side value the longer-lived one (one shared variable, the copy kept"
                          " inside one block, an inline's result tested where it is returned).",
           "CSE_NOFOLLOW": "cse carried what it knew (a constant in a register, an equal value) past"
                           " this jump into its target. In C keep the value from being known there:"
                           " another mode (a `(u8)` narrow constant), or recompute it after the branch.",
           "AGBCC_NOPRE": "gcse found this computation redundant and reused an earlier register. The"
                          " ROM recomputed it: spell it differently (another base or offset form) so"
                          " PRE does not see the same expression.",
           "AGBCC_NOGIV": "loop.c turned this per-iteration computation into its own register stepped"
                          " each pass (strength reduction; often a post-increment address). First check"
                          " the ROM listing: if the ROM also steps it (a post-increment or an add of the"
                          " element size each pass), the gain is an allocation side effect, not the cause."
                          " If the ROM recomputes it from the index: in C index the array where it is used"
                          " (base[i], base + i * size) instead of walking a pointer, keep one loop"
                          " counter, or make the step not a constant multiple of the counter.",
           "AGBCC_GIV": "the ROM steps this value in its own register each pass; the draft recomputes"
                        " it. In C walk a pointer (p++ / p += size) instead of indexing."}
    score(src, func, env, full=True)  # leave the workdir's dumps at the unmodified draft
    if not hits:
        print(f"  {min(len(cands), budget)} compiles: no single cse/gcse/loop decision changes the count;"
              " the difference is elsewhere (allocation: why; reload: spill).")
        return 0
    hits.sort()
    for n, knob, val, what, counts in hits[:8]:
        print(f"  {knob}={val}: {counts}  ({what})")
    seen = []
    for _, knob, *_ in hits[:8]:
        if knob not in seen:
            seen.append(knob)
            print(f"  {knob}: {fix[knob]}")
    print(f"  {min(len(cands), budget)} compiles. A knob is a what-if: confirm the decision with it, then"
          " write C with the same effect.")
    return 0


def cmd_why_all(src, func, env):
    """why for the first differing insn (with a uid) of each hunk, up to 8."""
    n, info = score(src, func, env, full=True)
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
    if not picks:
        rom_only = [r for r in rows if r["tag"] == "-" and r["rom"]]
        if not rom_only:
            print(f"{func}: {count_text(rows)} differ; only branch targets moved, so there is no insn for why."
                  " The plain scope run names both targets; `scope.py FILE.c probe` finds the cse decision")
            return 1
        print(f"{func}: {count_text(rows)} differ; no differing draft insn has an RTL uid, so there is nothing"
              " for why. The ROM has insns the draft does not produce at all:")
        for r in rom_only[:8]:
            print(f"  {r['rom_addr']} {r['rom']}")
        if any(re.match(r"(svc|swi)\b", r["rom"]) for r in rom_only):
            print("  `svc`/`swi` is a BIOS call written inline (the SDK's SystemCall macro: an asm statement);"
                  " agbcc has no C construct that emits it. Not a register or order problem: no knob applies.")
        else:
            print("  Find the C that produces them (a missing statement, a different expression shape); a"
                  " draft-only row that is alignment padding follows from the size change.")
        return 1
    print(f"{func}: {count_text(rows)} differ; per hunk: moved insns, then why on the first insn left\n")
    wd = info["wd"]
    lines = uid_lines(wd)
    srclines = open(os.path.join(ROOT, src)).read().splitlines()
    at = lambda u: f"insn {u} (line {lines[u]})" if lines.get(u) else f"insn {u}"
    for lo, hi in hunk_spans(rows):
        diff = [i for i in range(lo, hi + 1) if rows[i]["tag"] != "="]
        print(f"hunk at ROM {next((rows[i]['rom_addr'] for i in diff if rows[i]['rom_addr']), '?')}:")
        moved = explain_order(wd, rows, lo, hi, at, srclines)
        moves, kept = order_moves(rows, lo, hi)
        placed = {m[0] for m in moves} | {m[1] for m in moves} | {i for pair in kept for i in pair}
        # what is left for the register/shape why: an in-place insn in other registers, or one nothing explains
        regs = [d for r, d in kept if rows[r]["rom"] != rows[d]["draft"] and rows[d]["born"] in PASSES]
        other = [i for i in diff if i not in placed and rows[i]["uid"] and rows[i]["tag"] != "→"]
        u = rows[(regs or other)[0]]["uid"] if regs or other else None
        if u and moved and regs:
            print("  in place but in other registers (this can follow from the moves above):")
        if u:
            cmd_why(src, func, u, env)
        elif not moved:
            print("  (no insn with an RTL uid here)")
        print()
    return 1


def regmove_fixes(wd):
    """[(kind, insn, operand)] from the regmove dump's log, in order. kind: 'tie' (fixup_match_1: the output
    of a two-address insn takes its dying input's register), 'rename' (the backward pass renames the input
    to the output back to the insn that set it), 'offset' (fixup_match_2: an add rewritten from an earlier
    add of the same base), 'could' (a tie it considered and did not make)."""
    path = os.path.join(wd, "u.i.regmove")
    if not os.path.exists(path):
        return []
    out, section = [], 0
    for ln in open(path):
        if ln.startswith("Starting backward pass..."):
            section += 1
        elif ln.startswith("Starting forward pass"):
            section = 0
        m = re.match(r"(Fixed|Could fix) operand (?:(\d+) )?of insn (\d+)", ln)
        if not m:
            continue
        if m.group(1) == "Could fix":
            out.append(("could", int(m.group(3)), m.group(2)))
        elif m.group(2) is None:
            out.append(("offset", int(m.group(3)), None))
        else:
            out.append(("rename" if section >= 3 else "tie", int(m.group(3)), m.group(2)))
    return out


def explain_regmove(wd, uid, gone, new, nm, at):
    """regmove merged one pseudo into another at or before insn UID: name the two-address tie that did it,
    what in the C would have kept the ROM's separate copy, and the knob that forbids the tie."""
    fixes = [f for f in regmove_fixes(wd) if f[0] != "could"]
    pats = {u: r for u, _, r in insn_list(wd, "combine")}
    regs = set(gone) | set(new)
    hit = [f for f in fixes if f[1] == uid] or \
          [f for f in fixes if any(re.search(r"\(reg(?:/\w)*:\w+ %d\)" % r, pats.get(f[1], "")) for r in regs)]
    regs = regs  # noqa: keeps the search above readable
    if not hit:
        print("    (the regmove log names no tie that touches these registers: a copy coalesced by"
              " optimize_reg_copy, which does not log)")
        return
    kind, u, op = hit[0]
    # operand by operand, before and after regmove: which pseudo took which one's place
    before = regs_of(next((r for x, _, r in insn_list(wd, "combine") if x == uid), None))
    after = regs_of(next((r for x, _, r in insn_list(wd, "regmove") if x == uid), None))
    swaps = [(x, y) for x, y in zip(before, after) if x != y] if len(before) == len(after) else []
    if swaps:  # the output pseudo (x) now lives in the input's register (y)
        out_r, in_r = swaps[0]
        b, a = nm(out_r), nm(in_r)
    else:
        a = ", ".join(nm(r) for r in new) or "the input"
        b = ", ".join(nm(r) for r in gone) or "the output"
    what = {"tie": f"insn {u} is a two-address Thumb op (its output must equal its operand {op}); the input {a}"
                   f" died there, so regmove (fixup_match_1) put the output {b} in {a}'s register instead of"
                   f" copying an operand into {b} first",
            "rename": f"insn {u} is a two-address Thumb op; its input {a} died there and was set earlier in the"
                      f" same block with nothing in between using {a} or {b}, so regmove renamed {a} to {b} back"
                      " to that set and the copy disappeared",
            "offset": f"regmove (fixup_match_2) rewrote the add at insn {u} relative to an earlier add of the"
                      " same base"}[kind]
    print(f"    {what} [{at(u)}].")
    if kind != "offset":
        print(f"    If the ROM keeps a separate copy here, its C differs in one of the things regmove checks:"
              f" {a} is still used after insn {u} (so it does not die there), or {a} or {b} is read between"
              f" {a}'s set and insn {u}, or a label, branch or call separates them, or {a} is a constant set in"
              " another block. Levers: use the value again later, compute it in its own statement before an"
              " if, or give the result its own variable that is read before the operation.")
    # the same tie in other inlined copies of this code: same pattern up to register numbers
    shape = lambda r: re.sub(r"\(reg(/\w)*:(\w+) \d+\)", r"(reg:\2)", r or "")
    rowline = {}
    if os.path.exists(os.path.join(wd, "diff.json")):
        rowline = {r["uid"]: r["line"] for r in json.load(open(os.path.join(wd, "diff.json"))) if r.get("uid")}
    twins = [f[1] for f in fixes if f[0] == kind and f[1] != u and shape(pats.get(f[1])) == shape(pats.get(u))
             and rowline.get(f[1]) is not None and rowline.get(f[1]) == rowline.get(u)]
    us = ",".join(map(str, [u] + twins))
    print(f"    what-if: AGBCC_NOTIE={us} (forbids this tie" + (f", here and in {len(twins)} inlined cop"
          f"{'ies' if len(twins) > 1 else 'y'} of the same code" if twins else "") + "; the copy should come back."
          " What differs after that is a separate cause: ask `why` again with the knob set)")


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


def norm_insn(text):
    """An instruction with its low registers blanked, so the same operation in other registers compares
    equal. High registers (r8, sb, ...) stay: there they name a role (a value kept across a call)."""
    return re.sub(r"\br[0-7]\b", "R", text or "")


def hunk_spans(rows):
    """[(lo, hi)] row ranges of the diff's hunks (differing rows at most 4 apart), with up to 4 equal rows of
    context on each side (the same grouping as the diff view)."""
    real = [i for i, r in enumerate(rows) if r["tag"] != "="]
    spans = []
    for i in real:
        if spans and i - spans[-1][1] <= 4:
            spans[-1][1] = i
        else:
            spans.append([i, i])
    return [(max(0, a - 4), min(len(rows) - 1, b + 4)) for a, b in spans]


def order_moves(rows, lo, hi):
    """Instructions of rows[lo..hi] that both sides have, in different places. Compares the two sides with
    registers blanked: what the blanked sequences share in order is in place; a leftover ROM insn with a
    leftover draft twin moved. Returns ([(rom_index, draft_index, anchor)], [(rom_index, draft_index)] in
    place) (indexes into rows), anchor = (row of the nearest in-place insn before the ROM copy, row of the
    nearest one after), either None."""
    rom = [i for i in range(lo, hi + 1) if rows[i]["rom"]]
    dr = [i for i in range(lo, hi + 1) if rows[i]["draft"]]
    sm = difflib.SequenceMatcher(None, [norm_insn(rows[i]["rom"]) for i in rom],
                                 [norm_insn(rows[i]["draft"]) for i in dr], autojunk=False)
    kept = {}  # rom position -> draft position, in place
    for a, b, n in sm.get_matching_blocks():
        for k in range(n):
            kept[a + k] = b + k
    left_d = [j for j in range(len(dr)) if j not in set(kept.values())]
    moves = []
    for a in range(len(rom)):
        if a in kept:
            continue
        key = norm_insn(rows[rom[a]]["rom"])
        twin = next((j for j in left_d if norm_insn(rows[dr[j]]["draft"]) == key), None)
        if twin is None:
            continue
        left_d.remove(twin)
        before = max((k for k in kept if k < a), default=None)
        after = min((k for k in kept if k > a), default=None)
        moves.append((rom[a], dr[twin], (dr[kept[before]] if before is not None else None,
                                         dr[kept[after]] if after is not None else None)))
    return moves, [(rom[a], dr[b]) for a, b in sorted(kept.items())]


def rtl_origin(wd, uid):
    """The insn whose value UID copies, followed back through plain register/subreg copies in the rtl dump:
    (uid, rtl). For `(set (reg 44) (subreg (reg:HI 43)))` it is the insn that set 43."""
    seq = insn_list(wd, "rtl")
    pos = {u: i for i, (u, _, _) in enumerate(seq)}
    cur = uid
    for _ in range(8):
        if cur not in pos:
            return None
        r = seq[pos[cur]][2]
        m = re.match(r"\(set \(reg(?:/\w)*:\w+ \d+\) \((?:subreg:\w+ )?\(?reg(?:/\w)*:\w+ (\d+)\)", r)
        if not m:
            return cur, r
        src = int(m.group(1))
        prev = next((u for u, _, x in reversed(seq[:pos[cur]])
                     if re.match(r"\(set \(reg(?:/\w)*:\w+ %d\)" % src, x)), None)
        if prev is None:
            return cur, r
        cur = prev
    return cur, seq[pos[cur]][2]


def first_real_use(wd, uid):
    """The first insn after UID that uses UID's destination other than to copy it, following copies:
    (uid, rtl), or None."""
    seq = insn_list(wd, "rtl")
    pos = {u: i for i, (u, _, _) in enumerate(seq)}
    cur = uid
    for _ in range(8):
        if cur not in pos:
            return None
        m = re.match(r"\(set \(reg(?:/\w)*:\w+ (\d+)\)", seq[pos[cur]][2])
        if not m:
            return None
        pat = re.compile(r"\(reg(?:/\w)*:\w+ %s\)" % m.group(1))
        nxt = next(((u, r) for u, _, r in seq[pos[cur] + 1:] if pat.search(r)), None)
        if not nxt:
            return None
        if re.match(r"\(set \(reg(?:/\w)*:\w+ \d+\) \((?:subreg:\w+ )?\(?reg(?:/\w)*:\w+ %s\)" % m.group(1), nxt[1]):
            cur = nxt[0]
            continue
        return nxt
    return None


def explain_substitution(wd, p, uid, gone, nm, at):
    """A pass took pseudos out of insn UID: say what each one held (its set in the previous dump) and whether
    the pass folded that set into this insn."""
    prev = PASSES[max(0, PASSES.index(p) - 1)]
    for k in range(PASSES.index(p) - 1, -1, -1):
        if os.path.exists(os.path.join(wd, "u.i." + PASSES[k])):
            prev = PASSES[k]
            break
    before = insn_list(wd, prev)
    after_uids = {u for u, _, _ in insn_list(wd, p)}
    for g in gone:
        st = next(((u, r) for u, _, r in before if re.match(r"\(set \(reg(?:/\w)*:\w+ %d\)" % g, r)), None)
        if not st:
            continue
        folded = st[0] not in after_uids
        what = {"combine": "combine merged it into this insn" if folded else "combine substituted its value here",
                "cse": "cse found the same value already in a register", "cse2": "cse2 found the same value"
                " already in a register", "loop": "loop moved or reduced it", "gcse": "gcse replaced it by an"
                " earlier copy of the same value"}.get(p, f"{p} substituted it")
        print(f"    {nm(g)} was set by {at(st[0])}: {st[1][:90]}; {what}"
              + (" (that insn is gone)." if folded else "."))


def relative_order(wd, u, p):
    """[(pass, u_first)] for each dump where both insns appear, so a pass that reorders them shows."""
    out = []
    for ps in PASSES:
        seq = [x for x, _, _ in insn_list(wd, ps)]
        if u in seq and p in seq:
            out.append((ps, seq.index(u) < seq.index(p)))
    return out


def explain_order(wd, rows, lo, hi, at, srclines):
    """why for a hunk whose insns are the ROM's, in another order: which insns moved, where the ROM has them,
    which pass fixed the draft's order, and for a constant, the C use that put it there. Returns the number
    of moved rows explained."""
    moves, _ = order_moves(rows, lo, hi)
    if not moves:
        return 0
    lines = uid_lines(wd)
    code = lambda ln: srclines[ln - 1].strip() if ln and 0 < ln <= len(srclines) else ""
    # consecutive moved draft rows form one block (an address built in three insns moves as one)
    blocks = []
    for m in sorted(moves, key=lambda m: m[1]):
        if blocks and m[1] - blocks[-1][-1][1] <= 1:
            blocks[-1].append(m)
        else:
            blocks.append([m])
    print(f"  {len(moves)} insn(s) here are the ROM's own, in another place (registers aside):")
    notes = set()
    for blk in blocks:
        d_rows = [rows[m[1]] for m in blk]
        text = "; ".join(r["draft"] for r in d_rows)
        born = [r for r in d_rows if r["uid"] and r["born"] in PASSES]
        lead = min(born, key=lambda r: PASSES.index(r["born"])) if born else None
        bef, aft = blk[0][2][0], blk[-1][2][1]
        anchor = rows[bef] if bef is not None else None
        anchor2 = rows[aft] if aft is not None else None
        where = (f"after `{anchor['draft']}` (line {anchor['line']}" + (f", insn {anchor['uid']})" if anchor["uid"] else ")")
                 if anchor else "at the start")
        if anchor2:
            where += f", before `{anchor2['draft']}` (line {anchor2['line']})"
        print(f"   `{text}` (draft line {d_rows[0]['line'] or '?'}): the ROM has it {where}.")
        if not lead:
            continue
        u = lead["uid"]
        ref = anchor if anchor and anchor["uid"] else anchor2 if anchor2 and anchor2["uid"] else None
        if ref:
            rel = relative_order(wd, u, ref["uid"])
            if rel:
                first_pass, first = rel[0]
                flips = [ps for (ps, f), (_, g) in zip(rel[1:], rel) if f != g]
                if flips:
                    print(f"    {at(u)} and {at(ref['uid'])} change order in {flips[0]}: that pass moved it"
                          + {"combine": " (a merged insn takes the later insn's place)",
                             "loop": " (an invariant hoisted out of a loop)",
                             "gcse": " (a partially redundant expression moved to an earlier block)",
                             "greg": " (reload put a load or copy right before its use)"}.get(flips[0], ""))
                else:
                    print(f"    expansion order: {at(u)} comes {'before' if first else 'after'} {at(ref['uid'])}"
                          f" from {first_pass} on; no pass reorders them.")
                    notes.add("expand")
        # a constant: cse shares one register for every use of it in an extended block, set at the first use
        cse = next((r for x, _, r in insn_list(wd, "cse") if x == u), "")
        c = re.match(r"\(set \(reg(?:/\w)*:\w+ (\d+)\) \(const_int (-?\d+)\)\)", cse)
        if c:
            reg, val = int(c.group(1)), int(c.group(2))
            o = rtl_origin(wd, u)
            users = [(x, lines.get(x)) for x, _, r in insn_list(wd, "cse")
                     if x != u and re.search(r"\(reg(?:/\w)*:\w+ %d\)" % reg, r)]
            first = o[0] if o else u
            use = first_real_use(wd, first)
            print(f"    it is the constant {val}, which cse keeps in one register for all {len(users)} later"
                  f" use(s) (lines {', '.join(str(l) for _, l in users if l) or '?'}). Its set sits where the"
                  f" first use was expanded: {at(first)}: {code(lines.get(first)).rstrip(';')}" +
                  (f"; that first use is {at(use[0])} {use[1][:90]}" if use else ""))
            lo_l = anchor["line"] if anchor else None
            hi_l = anchor2["line"] if anchor2 else None
            span = (f"in line {lo_l}" if lo_l == hi_l or not hi_l else
                    f"after line {lo_l} and before line {hi_l}" if lo_l else f"before line {hi_l}")
            print(f"    so in the ROM's C the first use of {val} is expanded {span}"
                  + "".join(f"\n      {l}: {code(int(l))}" for l in dict.fromkeys(x for x in (lo_l, hi_l) if x and x.isdigit())))
            notes.add("const")
    if "expand" in notes:
        print("  levers: agbcc schedules nothing on Thumb, so expansion order is the C's order. Reorder the"
              " statements, or move the expression into the statement (or inline-call argument) that comes"
              " after the anchor.")
    if "const" in notes:
        print("  constants: the set is placed at the first use. Hidden first uses: a bitfield store's mask (an"
              " s16 :16 store clears with `and 0`), a zero-extension, an inline helper's parameter (bound before"
              " its body, in argument order). Make that use come at the ROM's place, or move the draft's first"
              " use later.")
    return sum(len(b) for b in blocks)


def pool_load_origin(wd, draft, line):
    """A constant-pool load born in the final pass (`ldr rN, =X`) is final's copy of an insn that loaded rN
    from the pool before then; return that insn's uid (same constant, nearest C line)."""
    m = re.match(r"ldr r(\d+), =(0x[0-9A-Fa-f]+|\d+)", draft or "")
    if not m:
        return None
    n, value = int(m.group(1)), int(m.group(2), 0)
    lines = uid_lines(wd)
    cands = []
    for u, _, r in insn_list(wd, "greg"):
        if not re.search(r"\(set \(reg(?:/\w)*:SI %d r%d\) \(mem/u:SI \(symbol_ref/u:SI" % (n, n), r):
            continue
        c = re.search(r"REG_EQU(?:IV|AL) \(const_int (-?\d+)", r)
        if c and int(c.group(1)) & 0xFFFFFFFF != value:
            continue
        cands.append(u)
    return min(cands, key=lambda u: abs((lines.get(u) or 0) - (line or 0))) if cands else None


def cmd_why(src, func, uid, env, via=None):
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
    row = via or next((r for r in rows if r["uid"] == uid), None)
    if row and not via and uid not in {u for u, _, _ in insn_list(wd, "greg")}:
        v = pool_load_origin(wd, row["draft"], lines.get(uid))
        if v:
            print(f"insn {uid} ({row['draft']}) is the final pass's copy of insn {v}, which loads the same"
                  f" constant before reload; explaining {v}:")
            return cmd_why(src, func, v, env, dict(row, uid=v))
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
        rows_all = json.load(open(os.path.join(wd, "diff.json")))
        if all(r["tag"] in "=→" for r in rows_all):
            print("  same branch, and nothing else differs: this target is the whole difference (a label"
                  " sits before another insn). The plain scope run names both targets (loop head or jump"
                  " threading); `scope.py FILE.c probe` finds the cse decision behind it")
        else:
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
            if p == "regmove":
                print(f"  regmove replaced {', '.join(nm(r) for r in gone)} by {', '.join(nm(r) for r in new)}.")
                explain_regmove(wd, uid, gone, new, nm, at)
            elif p not in ("greg", "lreg"):
                print(f"  {p} replaced {', '.join(nm(r) for r in gone)} by {', '.join(nm(r) for r in new) or 'nothing'}.")
                explain_substitution(wd, p, uid, gone, nm, at)
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
        i = next((j for j, r in enumerate(rows) if row and r["uid"] == row["uid"]
                  and r.get("rom_addr") == row.get("rom_addr")), None)
        if i is None and row:  # a redirected row (a final-pass pool load explained via its origin)
            i = next((j for j, r in enumerate(rows) if r.get("rom_addr") == row.get("rom_addr")), None)
        if i is not None:  # an insn split after reload (mach) serves the next insns in the listing
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



def cmd_fastcheck(n):
    """Both of scope's object paths (the fork server, and the one-shot compile placed in Python) must give
    the bytes ld and objcopy give: check N units (drafts first)."""
    import glob
    import random
    srcs = sorted(glob.glob(os.path.join(ROOT, "src", "fn", "*.c")) + glob.glob(os.path.join(ROOT, "src", "*.c")))
    drafts = [p for p in srcs if "#ifdef NONMATCHING" in open(p).read()]
    random.seed(1)
    pick = random.sample(drafts, min(n, len(drafts)))
    pick += random.sample([p for p in srcs if p not in drafts], min(max(0, n - len(pick)), len(srcs)))
    same = diff = skip = 0
    t_fast = t_full = 0.0
    for p in pick[:n]:
        src = os.path.relpath(p, ROOT)
        func = nonmatching_func(src) or next(iter(re.findall(r"^\w[^\n;=(]*\b(\w+)\s*\([^;{]*\)\s*\{",
                                                         open(p).read(), re.M)), None)
        if not func or rom_name(src, func) not in inventory()[1]:
            skip += 1
            continue
        try:
            os.environ["SCOPE_LINK"] = "1"  # the reference: ld, objcopy and objdump
            ref = compile_draft(src, func, {})[0]
            del os.environ["SCOPE_LINK"]
            t0 = time.perf_counter()
            mine = compile_draft(src, func, {})[0]
            t_full += time.perf_counter() - t0
        except SystemExit:
            os.environ.pop("SCOPE_LINK", None)
            skip += 1
            continue
        if mine != ref:
            diff += 1
            print(f"  FULL PATH DIFFERS FROM ld: {src} {func}")
            continue
        t0 = time.perf_counter()
        r = fast_compile(src, func, {})
        t_fast += time.perf_counter() - t0
        if r is None:
            skip += 1
            print(f"  not served: {src} {func}")
        elif r[0] == mine:
            same += 1
        else:
            diff += 1
            print(f"  DIFFERENT: {src} {func}")
    k = max(1, same + diff)
    print(f"{same} same, {diff} different, {skip} skipped; first compiles (server start included):"
          f" fast {t_fast / k * 1000:.0f} ms, full {t_full / k * 1000:.0f} ms per unit")
    return 1 if diff else 0


def code_status():
    """({name: 'matched' | 'draft' | 'undrafted'}, {name: source file}) from the source tree."""
    import progress
    from pathlib import Path
    funcs, _ = progress.load_functions()
    known = {n: n for n, _ in funcs}
    known.update({f"sub_{a:08X}": n for n, a in funcs})
    status = {n: "undrafted" for n, _ in funcs}
    files = {}
    for p in sorted(Path(ROOT, "src").rglob("*.c")):
        rel = str(p.relative_to(ROOT))
        for n in progress.c_functions(p, with_asm=False):
            if n in known:
                status[known[n]] = "matched"
                files[known[n]] = rel
        for n, _ in progress.nonmatching_functions(p):
            if n in known and status[known[n]] != "matched":
                status[known[n]] = "draft"
                files[known[n]] = rel
    return status, files


def cmd_rom(target):
    """FUNC's ROM listing (no draft needed): calls and literal words named from the ROM ELF."""
    import progress
    funcs, sizes = progress.load_functions()
    if re.fullmatch(r"(0x)?0?8[0-9A-Fa-f]{6}", target):
        a = int(target, 16)
        target = next((n for n, x in funcs if x == a), target)
    addr = dict(funcs).get(target)
    if addr is None:
        sys.exit(f"{target}: not in the function inventory")
    code = rom_bytes()[off(addr):off(addr) + sizes[target]]
    data = rom_data(target, addr, code)
    names = {a & ~1: n for n, a in funcs}
    for n, (v, _) in elf_syms().items():
        if not n.startswith(("_", "$")) and not n.endswith("__addr"):
            names.setdefault(v & ~1, n)
    status, files = code_status()
    print(f"{target} @ {addr:08X}, {len(code)} bytes, {status.get(target, '?')}"
          + (f" ({files[target]})" if target in files else ""))
    for o, n, text, _, tgt in decode(code, addr, data):
        note = ""
        m = re.match(r"bl #0x([0-9a-f]+)$", text)
        if m:
            text = "bl " + names.get(int(m.group(1), 16) & ~1, text[3:])
        m = re.match(r"(ldr \w+, =)0x([0-9A-Fa-f]+)$", text)
        if m and int(m.group(2), 16) & ~1 in names:
            note = names[int(m.group(2), 16) & ~1]
        m = re.match(r"\.word 0x([0-9A-F]+)$", text)
        if m and int(m.group(1), 16) & ~1 in names:
            note = names[int(m.group(1), 16) & ~1]
        print(f"  {o:4X}  {text:36}" + (f"@ {note}" if note else ""))
    return 0


def cmd_fields(offs, want):
    import glob
    import structure
    heads = sorted(glob.glob(os.path.join(ROOT, "include", "**", "*.h"), recursive=True))

    def compile_c(text):
        r = subprocess.run(build.CPP + ["-nostdinc", "-undef"] + build.DEFINES + ["-I", "include", "-"],
                           input=text, capture_output=True, text=True, cwd=ROOT)
        if r.returncode:
            return None
        a = subprocess.run([build.AGBCC] + build.CFLAGS + ["-o", "-"], input=r.stdout, capture_output=True,
                           text=True, cwd=ROOT)
        return a.stdout if a.returncode == 0 and "error" not in a.stderr else None
    lay = cached("struct_layouts", heads, lambda: structure.layouts(ROOT, compile_c))
    return structure.fields(offs, lay, want)


def cmd_like(target, show):
    import structure
    import progress
    funcs, sizes = progress.load_functions()
    elf = os.path.join(ROOT, "build", "boktai3.elf")
    index = cached("like_index", [elf, os.path.join(ROOT, "symbols", "functions.csv")],
                   lambda: structure.build_index(funcs, sizes, rom_bytes(), decode, rom_pool_ranges))
    if re.fullmatch(r"(0x)?0?8[0-9A-Fa-f]{6}", target):
        a = int(target, 16)
        target = next((n for n, x in funcs if x == a), target)
    status, files = code_status()
    return structure.like(target, index, status, files, show)


def cmd_serve():
    """The resident scope: warm caches in this process, one forked child per command (children can run
    side by side; each starts its own agbcc fork servers). Restarts itself when scope.py or fast.py
    changes; exits after SCOPE_SERVE_IDLE seconds (default 1800) without a command."""
    import signal
    import traceback
    code_files = [os.path.abspath(__file__), os.path.join(HERE, "fast.py")]
    stamp = [os.stat(f).st_mtime_ns for f in code_files]
    elf_syms(), inventory(), rom_bytes()  # warm: these are what every command loads first
    if os.path.exists(SOCK):
        probe_sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        try:
            probe_sock.connect(SOCK)
            print("a scope server is already running")
            return 0
        except OSError:
            os.remove(SOCK)
        finally:
            probe_sock.close()
    srv = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    srv.bind(SOCK)
    srv.listen(16)
    srv.settimeout(int(os.environ.get("SCOPE_SERVE_IDLE", "1800")))
    signal.signal(signal.SIGCHLD, signal.SIG_IGN)  # children are never waited for
    print(f"scope server {os.getpid()} on {SOCK}", flush=True)
    while True:
        try:
            conn, _ = srv.accept()
        except socket.timeout:
            break
        if [os.stat(f).st_mtime_ns for f in code_files] != stamp:
            conn.sendall(b"\0SCOPE-EXIT RESTART\n")
            conn.close()
            srv.close()
            os.remove(SOCK)
            os.execv(sys.executable, [sys.executable, os.path.abspath(__file__), "serve"])
        if os.fork():
            conn.close()
            continue
        srv.close()
        code = 1
        try:
            f = conn.makefile("rb")
            req = json.loads(f.readline())
            os.chdir(req["cwd"])
            for k in [k for k in os.environ if k.startswith(FORWARD)]:
                del os.environ[k]
            os.environ.update(req["env"])
            os.environ["SCOPE_IN_SERVER"] = "1"
            global WORK
            WORK = os.path.join(ROOT, "build", "scope", os.environ.get("SCOPE_RUN", ""))
            _FAST.clear()  # the parent's agbcc servers are not this child's
            sys.stdout.flush()
            os.dup2(conn.fileno(), 1)
            os.dup2(conn.fileno(), 2)
            sys.stdout = os.fdopen(1, "w", buffering=1, closefd=False)
            sys.stderr = sys.stdout
            sys.argv = [os.path.abspath(__file__)] + req["argv"]
            try:
                code = main() or 0
            except SystemExit as e:
                if isinstance(e.code, int) or e.code is None:
                    code = e.code or 0
                else:
                    print(e.code)
                    code = 1
            except Exception:
                traceback.print_exc()
            sys.stdout.flush()
            for fs in _FAST.values():
                fs.close()
        finally:
            try:
                os.write(1, f"\0SCOPE-EXIT {code}\n".encode())
            except OSError:
                pass
            os._exit(0)
    srv.close()
    if os.path.exists(SOCK):
        os.remove(SOCK)
    return 0


def c_change(base_lines, path):
    """'lines 105-112 (+3 -1)': where a variant's text differs from the base file."""
    other = open(path).read().splitlines()
    sm = difflib.SequenceMatcher(None, base_lines, other, autojunk=False)
    ops = [o for o in sm.get_opcodes() if o[0] != "equal"]
    if not ops:
        return "same text"
    plus = sum(j2 - j1 for _, _, _, j1, j2 in ops)
    minus = sum(i2 - i1 for _, i1, i2, _, _ in ops)
    lo, hi = ops[0][3] + 1, max(ops[-1][4], ops[-1][3] + 1)
    return f"lines {lo}-{hi} (+{plus} -{minus})" if lo != hi else f"line {lo} (+{plus} -{minus})"


def cmd_try(src, func, variants, env):
    """Score each variant (a whole-file .c, or one KNOB=V) through the fork server, best first."""
    t0 = time.perf_counter()
    base_n, base_info = score(src, func, env)
    rows = []
    base_lines = open(os.path.join(ROOT, src)).read().splitlines()
    for v in variants:
        if "=" in v and not v.endswith(".c"):
            k, val = v.split("=", 1)
            prev = env.get(k)
            try:
                n, info = score(src, func, dict(env, **{k: f"{prev},{val}" if prev else val}))
            except SystemExit as e:
                n, info = None, {"counts": str(e.code).splitlines()[0][:60]}
            rows.append((n, v, info.get("counts", "MATCH" if n == 0 else "?"), "knob"))
            continue
        path = v if os.path.isabs(v) else os.path.join(os.getcwd(), v)
        if not os.path.exists(path):
            path = os.path.join(ROOT, v)
        rel = os.path.relpath(path, ROOT)
        try:
            n, info = score(rel, func, env)
        except SystemExit as e:
            n, info = None, {"counts": "compile fails: " + str(e.code).splitlines()[-1].strip()[:60]}
        rows.append((n, rel, info.get("counts", "MATCH" if n == 0 else "?"), c_change(base_lines, path)))
    dt = time.perf_counter() - t0
    knobs = " ".join(f"{k}={v}" for k, v in env.items())
    print(f"{func}: base {src}{' [' + knobs + ']' if knobs else ''}: {base_info.get('counts', 'MATCH')}."
          f" {len(variants)} variants in {dt:.2f}s ({dt / max(1, len(variants) + 1) * 1000:.0f} ms each)")
    def key(n, counts):
        """Rank by the counts shown: the larger side, then the total, then differing rows."""
        m = re.match(r"(\d+) ROM / (\d+) draft", counts or "")
        if n is None:
            return None
        return (0, 0, 0) if n == 0 else (max(int(m[1]), int(m[2])), int(m[1]) + int(m[2]), n) if m else (n, n, n)
    base_k = key(base_n, base_info.get("counts", "MATCH"))
    rows.sort(key=lambda r: (r[0] is None, key(r[0], r[2]) or (0,)))
    for n, name, counts, change in rows:
        k = key(n, counts)
        mark = "MATCH" if n == 0 else ("better" if k is not None and k < base_k else
                                       "same" if k == base_k else "worse" if k is not None else "")
        print(f"  {mark:6} {counts:44} {name}" + ("" if change == "knob" else f"  [{change}]"))
    best = rows[0] if rows else None
    if best and best[0] is not None and best[3] != "knob":
        print(f"\nnext: scope.py {best[1]} {func}   (the best variant's listing)")
    return 0 if any(r[0] == 0 for r in rows) else 1


def last_func():
    p = os.path.join(WORK, "LAST")
    return open(p).read().strip() if os.path.exists(p) else None


def main():
    args = sys.argv[1:]
    if not args or args[0] in ("-h", "--help"):
        print(__doc__)
        return 0
    if args[0] == "fields":  # fields OFF... [STRUCT]: which structs have members at these offsets
        offs = [int(a, 16) if a.lower().startswith("0x") else int(a) for a in args[1:] if re.fullmatch(r"(0x)?[0-9a-fA-F]+", a)]
        want = next((a for a in args[1:] if not re.fullmatch(r"(0x)?[0-9a-fA-F]+", a)), None)
        return cmd_fields(offs, want)
    if args[0] == "rom":  # rom FUNC|ADDR: the ROM listing, no draft needed
        return cmd_rom(args[1])
    if args[0] == "like":  # like FUNC|ADDR: matched functions whose ROM looks like this one's
        return cmd_like(args[1], int(args[2]) if len(args) > 2 else 8)
    if args[0] == "census":  # census [CALLEE]: concessions across src/ grouped by the call they serve
        import structure
        return structure.census(ROOT, want=args[1] if len(args) > 1 else None)
    if args[0] == "serve":
        return cmd_serve()
    if args[0] == "fastcheck":
        return cmd_fastcheck(int(args[1]) if len(args) > 1 else 40)
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
        os.makedirs(WORK, exist_ok=True)
        open(os.path.join(WORK, "LAST"), "w").write(func)
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
        rest = [a for a in rest if a != "--all"]
        func = rest[1] if len(rest) > 1 else nonmatching_func(src) or os.path.basename(src)[:-2]
        return cmd_pins(src, func, env)
    if "try" in rest:  # FILE.c [KNOB=V...] try VARIANT... [FUNC]: whole-file variants or single knobs
        i = args.index("try")
        before = [a for a in args[1:i] if "=" not in a]
        env = dict(a.split("=", 1) for a in args[1:i] if "=" in a)
        variants = args[i + 1:]
        func = before[0] if before else nonmatching_func(src)
        if variants and not variants[-1].endswith(".c") and "=" not in variants[-1]:
            func = variants.pop()
        if not func:
            sys.exit("name the function: scope.py FILE.c try VARIANT... FUNC")
        return cmd_try(src, func, variants, env)
    if "probe" in rest:
        rest.remove("probe")
        func = rest[0] if rest else nonmatching_func(src)
        return cmd_probe(src, func, env)
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
    os.makedirs(WORK, exist_ok=True)
    open(os.path.join(WORK, "LAST"), "w").write(func)
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
