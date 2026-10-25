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
  scope.py FILE.c shape         the diff with low registers blanked: order and instruction differences only
  scope.py FILE.c frame         stack slots, ROM against draft, with the pseudo (C variable) in each
  scope.py FILE.c hoist         loop-invariant motion pass by pass: what loop.c moved out, what stayed, why
  scope.py FILE.c giv           loop strength reduction: each induction value, loop.c's benefit and decision
  scope.py todo [N] [addr|prior|size]   functions with no C yet (phase 1), in ROM order
  scope.py new FUNC             open FUNC: a draft in build/scope/work/ (its unit with a stub, or a twin's C)
  scope.py land FUNC [D.c] [--name NEW] [--note T] [--rom] [--nonmatching]   put a matching draft into
                                src/ (unit merge, or a new src/fn file and the hidden boundary), check the
                                file, name it; --nonmatching: an unfinished draft under #ifdef NONMATCHING
  scope.py rename FUNC NEW [--note T]   functions.csv and every C spelling of the old name or sub_ label
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
import shlex
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
CS_ARM = capstone.Cs(capstone.CS_ARCH_ARM, capstone.CS_MODE_ARM)


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
    # progress_functions.csv holds the starts (a newly registered hidden function shortens its neighbor)
    deps = [os.path.join(ROOT, "symbols", f) for f in ("functions.csv", "progress_functions.csv")]
    by_label, addr, sizes = cached("inventory", deps + [os.path.join(ROOT, "build", "boktai3.elf")],
                                   lambda: tuple(check.inventory()))
    return by_label, addr, sizes


def workdir(func):
    d = os.path.join(WORK, func)
    os.makedirs(d, exist_ok=True)
    return d


def nonmatching_func(src):
    """The function a draft file is about: a workspace draft's own (build/scope/work/FUNC.c, from `new`,
    or any copy of it, by its `// scope: write FUNC here` marker), else the first definition inside
    #ifdef NONMATCHING."""
    text = open(os.path.join(ROOT, src)).read()
    ws = os.path.relpath(os.path.join(ROOT, src), os.path.join(WORK, "work"))
    if not ws.startswith(".."):
        name = os.path.basename(ws).split(".")[0]
        if re.search(rf"\b{re.escape(name)}\s*\([^;{{]*\)\s*\{{", text):
            return name
    m = re.search(r"^// scope: write (\w+) here", text, re.M)  # `new`'s marker: also in copies made for try
    if m:
        return m.group(1)
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
    # Friendly C names need the INCLUDE_ASM from their own block, not the
    # first draft in a multi-function unit.
    names = []
    for m in re.finditer(r"#ifdef NONMATCHING(.*?)#else(.*?)#endif", text, re.S):
        if re.search(r"\b%s\s*\([^;{}]*\)\s*\{" % re.escape(func), m.group(1)):
            names = re.findall(r'INCLUDE_ASM\(\s*"[^"]*"\s*,\s*(\w+)\s*\)', m.group(2))
            break
    hit = next((n for n in names if n in addr), None)
    for alias in re.findall(r"^#define[ \t]+" + re.escape(func) + r"[ \t]+(\w+)[ \t]*(?://[^\n]*)?$", text, re.M):
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
    blobs = []  # (start, end) of .incbin runs: raw bytes the pool scan must classify
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
        elif s.startswith(".incbin"):
            m = re.search(r",\s*(0x[0-9A-Fa-f]+|\d+)\s*,\s*(0x[0-9A-Fa-f]+|\d+)", s)
            if m:
                n = int(m.group(2), 0)
                blobs.append((cur - addr, cur - addr + n)); cur += n
        elif s.startswith("."):
            continue
        else:
            cur += 4 if re.match(r"bl\b", s) else 2
    return data, blobs


def rom_pool_ranges(code, addr, arm=False):
    """Fallback: literal-pool words found from pc-relative loads."""
    data = set()
    if arm:  # ldr rN, [pc, #+-imm12]
        for i in range(0, len(code) - 3, 4):
            w = int.from_bytes(code[i:i + 4], "little")
            if w & 0x0F7F0000 == 0x051F0000:
                t = i + 8 + (w & 0xFFF if w & (1 << 23) else -(w & 0xFFF))
                if 0 <= t < len(code):
                    data.add(t)
        return sorted((t, t + 4) for t in data)
    for i in range(0, len(code) - 1, 2):
        h = int.from_bytes(code[i:i + 2], "little")
        if any(t <= i < t + 4 for t in data):
            continue  # a pool word's halfword (0x08604A48 holds 0x4A48, an `ldr r2, [pc]`) is not a load
        if h & 0xF800 == 0x4800:
            t = ((addr + i + 4) & ~3) + (h & 0xFF) * 4 - addr
            if 0 <= t < len(code):
                data.add(t)
    return sorted((t, t + 4) for t in data)


def rom_data(func, addr, code, arm=False):
    """The ROM function's data ranges: its generated assembly's, or (a function the disassembler
    left as .incbin has no labels at all) the literal pools its pc-relative loads reach."""
    found = rom_data_ranges(func, addr)
    if not found or not (found[0] or found[1]):
        return rom_pool_ranges(code, addr, arm)
    labeled, blobs = found
    # the disassembler's data labels are exact; bytes it left as .incbin get the pool scan
    pools = [r for r in rom_pool_ranges(code, addr, arm) if any(a <= r[0] < b for a, b in blobs)]
    return sorted(set(labeled) | set(pools))


def is_arm(name, addr, index=None):
    """FUNC is ARM code: its functions.csv or build_extra_functions.csv mode, or its unit is built
    with agbcc_arm."""
    import land
    if any(a == addr and m == "arm" for a, m in arm_starts()):
        return True
    row = land.hidden_row(ROOT, addr)
    if row:
        return row["isa"] == "arm"
    kind, dest, _ = land.where(ROOT, name, addr, index)
    path = os.path.join(ROOT, dest)
    return kind == "unit" and "// COMPILER: agbcc_arm" in open(path).read()


# ---------------------------------------------------------------- decode

def decode(code, addr, data, arm=False):
    """[(offset, size, text, key, target_offset)]; data words decode as .word. ARM=True for ARM code."""
    cs, step, pc = (CS_ARM, 4, 8) if arm else (CS, 2, 4)
    rows, i = [], 0
    data = sorted(data)
    lo, hi = addr, addr + len(code)
    while i < len(code):
        d = next(((a, b) for a, b in data if a <= i < b), None)
        if d:
            n = min(4, d[1] - i, 4 - (addr + i) % 4 if (addr + i) % 4 else 4)
            v = int.from_bytes(code[i:i + n], "little")
            text = f".word 0x{v:08X}" if n == 4 else f".{n}byte 0x{v:0{n * 2}X}"
            if n == 2 and v == 0 and not arm:
                text = "movs r0, r0"  # pool alignment: the other side may decode it as code
            tgt = (v & ~1) - addr if n == 4 and lo <= (v & ~1) < hi else None
            rows.append((i, n, text, ".word ->" if tgt is not None else text, tgt))
            i += n
            continue
        ins = next(cs.disasm(code[i:i + 4], addr + i), None)
        if ins is None:
            v = int.from_bytes(code[i:i + step], "little")
            rows.append((i, step, f".{step}byte 0x{v:0{step * 2}X}", f".{step}byte 0x{v:0{step * 2}X}", None))
            i += step
            continue
        text = f"{ins.mnemonic} {ins.op_str}".strip()
        key, tgt = text, None
        m = re.match(r"ldr (\w+), \[pc, #(-?0x[0-9a-f]+|-?\d+)\]", text)
        if m:
            la = ((addr + i + pc) & ~3) + int(m.group(2), 0) - addr
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


def draft_annotations(wd, func, rows, obj_offset, lines=None, data=()):
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
    # decode renders zero halfwords in literal-pool alignment as `movs r0, r0`
    # to compare them with ROM padding. They have no assembly instruction or
    # RTL uid: including even one would discard annotations for the whole unit.
    code = [r for r in rows if not r[2].startswith(".")
            and not any(lo <= r[0] < hi for lo, hi in data)]
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
    ann = draft_annotations(wd, info["obj_name"], dr, info["obj_offset"], info.get("lines"), mdata)
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
          + f"; shape {shape_distance(rom, dr)} (register-blind distance, what try ranks by)"
          + f"; {shifted} offset-only shifts hidden.  full listing: {rel}/diff.txt (json: diff.json)")
    tail = []
    for x, y, t in reversed(pairs):
        if y is not None or x is None:
            break
        tail.insert(0, rom[x])
    if tail and (tail[0][2] == "bx lr" or re.match(r"push \{.*lr\}", tail[0][2])):
        # gbadisasm folded a following function into this one (often after a literal pool)
        at = addr + tail[0][0]
        print(f"ROM goes on past the draft's end with what starts a function at {at:08X} ({tail[0][2]}).  If it is"
              f" one, register it and rescore: python3 tools/hidden_tails.py split '{at:08X},,DESCRIPTION'")
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


def function_trace(text, func):
    """Only FUNC's trace records. UIDs and decision indices restart in each function.
    PRE runs after cse1, so its unlabelled records belong to that cse marker;
    loop traces carry their own function marker before the next cse pass.
    """
    out, current = [], None
    for ln in text.splitlines():
        m = re.match(r"cse pass \d (\w+)$|loopopt fn=(\w+)$", ln)
        if m:
            current = m.group(1) or m.group(2)
        if current == func:
            out.append(ln)
    return "\n".join(out)


def parse_trace(text, func):
    """FUNC's CSE_WHY / CSE_DEBUG output, split by pass; never mix units' UIDs."""
    text = function_trace(text, func)
    passes, cur = {}, None
    block = None
    for seq, ln in enumerate(text.splitlines()):
        m = re.match(r"cse pass (\d)", ln)
        if m:
            cur = passes.setdefault(int(m.group(1)), {"eqv": [], "why": []})
            block = None
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


_CPP1 = {}


def cpp_command(src):
    """The preprocessor command for SRC. build.CPP is a compiler driver (`cc -E`), which on macOS goes
    through the xcrun shim and then forks the real preprocessor: 23 ms where the preprocessor itself
    takes 9. Ask the driver once (-###) what it would run, and run that directly with SRC swapped in.
    Falls back to build.CPP if the driver's answer cannot be used."""
    args = ["-nostdinc", "-undef"] + build.DEFINES + ["-DNONMATCHING", "-I", "include", "-iquote", "."]
    if "cmd" not in _CPP1:
        _CPP1["cmd"] = None
        try:
            r = subprocess.run(build.CPP + args + [src, "-###"], cwd=ROOT, capture_output=True, text=True)
            last = [ln for ln in r.stderr.splitlines() if ln.startswith((' "', '"', " /"))]
            cmd = shlex.split(last[-1]) if r.returncode == 0 and len(last) == 1 else None
            if cmd and cmd.count(src) == 1 and "-E" in cmd:
                _CPP1["cmd"] = cmd
                _CPP1["src"] = src
        except (OSError, ValueError):
            pass
    cmd = _CPP1["cmd"]
    if not cmd:
        return build.CPP + args + [src]
    old = _CPP1["src"]
    return [src if a == old else os.path.basename(src) if a == os.path.basename(old) else a for a in cmd]


def _preprocess(src, env=None):
    pre = build.sh(cpp_command(src))
    tf = (env or {}).get("SCOPE_TRANSFORM") or os.environ.get("SCOPE_TRANSFORM")
    if tf:  # a what-if over the preprocessed source: a module with transform(text) -> text
        import importlib.util
        spec = importlib.util.spec_from_file_location("scope_transform", os.path.join(ROOT, tf))
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        pre = mod.transform(pre)
    return pre


_HEADS = {}
_BODY_TOKEN = re.compile(r'"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'|/\*.*?\*/|//[^\n]*|[A-Za-z_]\w*|.', re.S)
_DYNAMIC = {"__LINE__", "__FILE__", "__DATE__", "__TIME__", "__COUNTER__", "__INCLUDE_LEVEL__", "__BASE_FILE__"}


def fast_preprocess(src, env=None):
    """preprocess() for the fork server, without running cpp on every variant. A draft is a block of
    #includes and then plain C; cpp's output for the include block is cached (keyed by its text and the
    headers' mtimes), and the C after it passes through with comments blanked, as cpp would leave it,
    when it holds no directive, line splice, trigraph or name of a macro the headers define. Anything else
    goes to cpp. Bytes only: the line markers name a stand-in file."""
    if (env or {}).get("SCOPE_TRANSFORM") or os.environ.get("SCOPE_TRANSFORM"):
        return preprocess(src, env)
    text = open(os.path.join(ROOT, src)).read()
    lines = text.split("\n")
    last = max((i for i, ln in enumerate(lines) if ln.lstrip().startswith("#")), default=-1)
    head = "\n".join(lines[:last + 1]) + "\n"
    body = "\n".join(lines[last + 1:])
    if last < 0 or any(not re.match(r"\s*(#\s*include\b.*)?$", ln) and not re.match(r"\s*(//.*)?$", ln)
                       for ln in lines[:last + 1]):
        return preprocess(src, env)  # a #define, #if, or code between the includes
    h = _HEADS.get(head)
    if h is not None and any(os.path.getmtime(os.path.join(ROOT, f)) != t for f, t in h[2]):
        h = None
    if h is None:
        work = os.path.join(WORK, "cache")
        os.makedirs(work, exist_ok=True)
        tmp = os.path.join(work, "head.c")
        open(tmp, "w").write(head)
        rel = os.path.relpath(tmp, ROOT)
        # how many newlines cpp leaves after its last marker depends on the last include (a guarded
        # header emits none); a marker of our own says where the body starts
        out = re.sub(r"\n+\Z", "\n", build.sh(cpp_command(rel))) + f'# {last + 2} "{rel}"\n'
        cmd = cpp_command(rel)
        e = cmd.index("-E") + 1
        defs = build.sh(cmd[:e] + ["-dM"] + cmd[e:])
        macros = set(re.findall(r"^#define (\w+)", defs, re.M)) | _DYNAMIC
        files = sorted({f for f in re.findall(r'^# \d+ "([^"<][^"]*)"', out, re.M) if os.path.exists(os.path.join(ROOT, f))})
        h = _HEADS[head] = (out, macros, [(f, os.path.getmtime(os.path.join(ROOT, f))) for f in files])
    out, macros, _ = h
    if "\\\n" in body or "??" in body:
        return preprocess(src, env)
    parts = []
    for m in _BODY_TOKEN.finditer(body):
        t = m.group(0)
        if t[:2] == "/*":
            parts.append(" " + "\n" * t.count("\n"))
        elif t[:2] == "//":
            parts.append(" ")
        elif t == "#" or (t[0].isalpha() or t[0] == "_") and t in macros:
            return preprocess(src, env)
        else:
            parts.append(t)
    return out + "".join(parts)


def inline_macros(text):
    """Inline the macros a top-level asm() includes (gas sometimes misreads the included file)."""
    macros = open(os.path.join(ROOT, "asm", "macros.inc")).read()
    return re.sub(r'^\s*\.include\s+"asm/macros\.inc"\s*$', lambda m: macros, text, flags=re.M)


def fast_compile(src, func, env=None):
    """(FUNC's bytes, address, data ranges) through scope's agbcc fork server (tools/scope/fast.py):
    a few ms per variant, bytes only. None when the unit cannot be served (then use compile_draft)."""
    import fast
    env = resolve_knobs(src, func, env)
    # -g as compile_draft has it: debug notes take insn uids, so without it the uids `why` and `insn`
    # print name other insns here and uid knobs (AGBCC_NOMOVE, AGBCC_NOGIV, ...) in `try` miss
    flags = tuple(build.cflags_for(src)) + ("-g",)
    _, addr_all, _ = inventory()
    addr = addr_all[rom_name(src, func)]
    key = (flags, addr)
    f = _FAST.get(key)
    if f is None:
        f = _FAST[key] = fast.Fast(INSTR, list(flags), build.AS, elf_syms(), addr,
                                   os.path.join(WORK, "cache"), fixup=inline_macros)
        os.makedirs(os.path.join(WORK, "cache"), exist_ok=True)
    knobs = {k: v for k, v in (env or {}).items() if k.startswith(("AGBCC_", "CSE_"))}
    code, err = f.bytes(fast_preprocess(src, env), (func, rom_name(src, func)), knobs)
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
    dr = decode(mine, addr, mdata)
    pairs, _ = align(_ROMDEC[addr], dr)
    rows = [{"tag": t} for _, _, t in pairs]
    fast_score.shape = shape_distance(_ROMDEC[addr], dr)
    fast_score.fuzzy = fuzzy_percent(_ROMDEC[addr], dr)
    return sum(1 for r in rows if r["tag"] != "="), count_text(rows)


def fuzzy_percent(rom, mine):
    """check.py's fuzzy score (instruction-text similarity, addresses masked), for a NONMATCHING draft."""
    norm = lambda rows: [re.sub(r"0x[0-9a-fA-F]{7,8}", "ADDR", r[3]) for r in rows]
    return min(99.9, 100 * difflib.SequenceMatcher(None, norm(rom), norm(mine), autojunk=False).ratio())


def score(src, func, env, full=False):
    """Compile with ENV and return the number of differing instructions (0 = MATCH), silently.
    Through the fork server unless FULL (callers that read the compile's dumps or traces after)."""
    import contextlib
    import io
    if not full and os.environ.get("SCOPE_FAST") != "0":  # SCOPE_FAST=0: one-shot compiles (A/B, debugging)
        r = fast_score(src, func, env)
        if r is not None:
            return r[0], {"counts": r[1], "shape": fast_score.shape if r[0] else 0,
                          "fuzzy": fast_score.fuzzy if r[0] else 100.0}
    mine, addr, info = compile_draft(src, func, env)
    _, _, sizes = inventory()
    rom_code = load_rom(verify=False)[off(addr):off(addr) + sizes[rom_name(src, func)]]
    if mine == rom_code:  # the listing can misalign a literal pool at a 2-byte offset; bytes decide
        info.update(counts="MATCH", shape=0, fuzzy=100.0)
        return 0, info
    with contextlib.redirect_stdout(io.StringIO()):
        report(src, func, rom_code, addr, mine, info, 0)
    rows = json.load(open(os.path.join(info["wd"], "diff.json")))
    data = info["data"] if "data" in info else mapping_ranges(os.path.join(info["wd"], "u.o"),
                                                              info["obj_offset"], len(mine))
    rom_rows = decode(rom_code, addr, rom_data(rom_name(src, func), addr, rom_code))
    draft_rows = decode(mine, addr, data)
    info.update(counts=count_text(rows), shape=shape_distance(rom_rows, draft_rows),
                fuzzy=fuzzy_percent(rom_rows, draft_rows))
    return sum(1 for r in rows if r["tag"] != "="), info


def shape_distance(rom, mine):
    """Insns to delete or insert to turn the draft into the ROM with low registers blanked: what is left
    once register choice is set aside (order and instruction shape). A moved block costs twice its length."""
    a = [norm_insn(r[3]) for r in rom]
    b = [norm_insn(r[3]) for r in mine]
    same = sum(n for _, _, n in difflib.SequenceMatcher(None, a, b, autojunk=False).get_matching_blocks())
    return len(a) + len(b) - 2 * same


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
    values loop.c strength-reduces or not (AGBCC_NOGIV / AGBCC_GIV), invariants loop.c hoists out
    of a loop (AGBCC_NOMOVE). Then each is
    flipped alone; a flip that matches or lowers the count names the decision (rtlprobe.py's
    canon / follow / pre, inside scope's loop and with its counts)."""
    base, info = score(src, func, env, full=True)
    wd = info["wd"]
    tr = subprocess.run([INSTR] + build.cflags_for(src) + ["-g", "u.i", "-o", "/dev/null"], cwd=wd,
                        capture_output=True, text=True,
                        env=dict(os.environ, CSE_DEBUG="1", CSE_PATH="1", AGBCC_PRE_TRACE="1", AGBCC_LOOP_TRACE="1",
                                 **{k: v for k, v in info["env"].items()})).stderr
    tr = function_trace(tr, info["obj_name"])
    heads, copied = set(), {}
    for m in re.finditer(r"eqv new=(\d+) old=(\d+) first=(\d+) newlast=(\d+) newfirst=(\d+) "
                         r"firstlast=(-?\d+) bb=(\d+)\.\.(\d+)", tr):
        new, old, first, nl, nf, fl, b0, b1 = map(int, m.groups())
        if fl >= 0 and (nl > b1 or nf < b0) and nl > fl:
            heads.add(new)
            copied.setdefault(new, old)
    follows = {int(m.group(1)): int(m.group(2))
               for m in re.finditer(r"follow k=(\d+) pass=1 jump=\d+ label=\d+ line=(\d+)", tr)}
    pres = sorted({int(m.group(1)) for m in re.finditer(r"pre k=(\d+) insn=(\d+) expr=\d+", tr)})
    fnames = (func, rom_name(src, func))
    givs = {}  # insn uid -> (addr|reg, reduced?) for loop strength reduction in this function
    for m in re.finditer(r"giv fn=(\w+) insn=(\d+) type=(\w+) (?:benefit=-?\d+ lifetime=\d+ )?reduce=(\w+)", tr):
        if m.group(1) in fnames:
            givs[int(m.group(2))] = (m.group(3), m.group(4) in ("1", "combined"))
    hoists = {}  # insn uid -> (pseudo, threshold, lifetime, insns): invariants loop.c moved out of a loop
    for m in re.finditer(r"move fn=(\w+) insn=(\d+) regno=(\d+) threshold=(-?\d+) savings=(\d+) lifetime=(\d+)"
                         r" insn_count=(\d+) moved_once=(\d) already_moved=(\d)", tr):
        fn, uid, reg, t, sv, life, n, once, done = m.groups()
        if fn in fnames and (done == "1" or int(t) * int(sv) * int(life) >= int(n) * (2 if once == "1" else 1)):
            hoists.setdefault(int(uid), (int(reg), int(t), int(life), int(n)))
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
             + [("AGBCC_NOMOVE", str(u), f"loop invariant hoist of {who(r)} (insn {u}, line {lines.get(u, '?')};"
                 f" threshold {t} x lifetime {life} >= {n} insns)") for u, (r, t, life, n) in sorted(hoists.items())]
             + [("AGBCC_NOGIV" if red else "AGBCC_GIV", str(u),
                 f"loop strength reduction of the {'address' if t == 'addr' else 'value'} computed at line"
                 f" {lines.get(u, '?')} (insn {u}, {'reduced' if red else 'not reduced'})")
                for u, (t, red) in sorted(givs.items())])
    budget = int(env.get("SCOPE_PROBE_MAX") or os.environ.get("SCOPE_PROBE_MAX", "150"))
    print(f"{func}: {info.get('counts') or base} differ; {len(heads)} cse canonical-register choices,"
          f" {len(follows)} cse jump follows, {len(pres)} PRE replacements"
          + f", {len(givs)} loop induction values, {len(hoists)} loop invariant hoists"
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
                        " it. In C walk a pointer (p++ / p += size) instead of indexing.",
           "AGBCC_NOMOVE": "loop.c hoisted this invariant out of the loop; the ROM sets it inside each"
                           " pass. In C make the set unmovable (scope.py FILE.c hoist shows why each one"
                           " moved): assign the same variable again in a later block of the body, past a"
                           " join label, so cse cannot drop the repeat and loop.c sees it set in two blocks"
                           " (flow deletes the dead store afterwards); or set it after a branch in the body"
                           " when its uses span blocks (maybe_never). Each earlier hoist also lowers the"
                           " threshold by 3, but loop runs twice, so luck rarely holds in both passes."}
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


def is_copy(text):
    """A register-to-register copy: what register allocation adds or removes, not the C's order."""
    return bool(re.match(r"(mov|movs) (r\d+|sb|sl|fp|ip), (r\d+|sb|sl|fp|ip)$|adds (r\d+), (r\d+), #0$", text or ""))


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
        if is_copy(rows[rom[a]]["rom"]):
            continue  # a register copy (into a high register, or adds rX, rY, #0) is allocation, not order
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


def rtl_constant_value(expr, symbols):
    """Known 32-bit value of a constant RTL expression; None is never evidence of equality."""
    m = re.fullmatch(r"\(const_int (-?\d+)\)", expr)
    if m:
        return int(m.group(1)) & 0xFFFFFFFF
    m = re.fullmatch(r'\(symbol_ref(?:/\w)*:SI \("([^"]+)"\)\)', expr)
    if m:
        return symbols[m.group(1)][0] & 0xFFFFFFFF if m.group(1) in symbols else None
    m = re.fullmatch(r"\(const:SI (.*)\)", expr)
    if m:
        return rtl_constant_value(m.group(1), symbols)
    m = re.match(r"\((plus|minus):SI ", expr)
    if m and expr[m.end():].startswith("("):
        end = _sexp_end(expr, m.end())
        a = rtl_constant_value(expr[m.end():end], symbols)
        rest = expr[end:].strip()
        if rest.startswith("(") and _sexp_end(rest, 0) == len(rest) - 1 and rest.endswith(")"):
            b = rtl_constant_value(rest[:-1], symbols)
            if a is not None and b is not None:
                return (a + b if m.group(1) == "plus" else a - b) & 0xFFFFFFFF
    return None


def pool_load_origin(wd, draft, line):
    """A pre-final SET of the same hard register with a positively equal constant value.
    Keep raw greg notes: one_line() deliberately strips REG_EQUAL/REG_EQUIV.
    Unknown pool symbols/expressions cannot establish an origin.
    """
    m = re.match(r"ldr r(\d+), =(0x[0-9A-Fa-f]+|\d+)$", draft or "")
    if not m:
        return None
    n, value = int(m.group(1)), int(m.group(2), 0) & 0xFFFFFFFF
    lines, symbols = uid_lines(wd), elf_syms()
    path = os.path.join(wd, "u.i.greg")
    text = open(path).read() if os.path.exists(path) else ""
    cands = []
    for m in re.finditer(r"^\(insn(?:/\w)* (\d+) .*?(?=^\(|\Z)", text, re.S | re.M):
        raw = m.group(0)
        st = re.fullmatch(r"\(set \(reg(?:/\w)*:SI %d(?: \w+)?\) (.*)\)" % n, one_line(raw))
        if not st:
            continue
        source = st.group(1)
        known = rtl_constant_value(source, symbols)
        if known is None and re.match(r"\(mem/u:SI \(symbol_ref", source):
            note = re.search(r"REG_EQU(?:IV|AL) ", raw)
            if note:
                known = rtl_constant_value(" ".join(raw[note.end():_sexp_end(raw, note.end())].split()), symbols)
        if known == value:
            cands.append(int(m.group(1)))
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
            print(f"insn {uid} ({row['draft']}) has the same constant value as pre-final insn {v}"
                  f" (nearest matching C line); explaining {v}:")
            return cmd_why(src, func, v, env, dict(row, uid=v))
        if re.match(r"ldr r\d+, =", row["draft"] or ""):
            print(f"insn {uid}: constant origin is unknown (no equal-valued pre-final SET found);"
                  " retaining the emitted insn rather than guessing from its register")
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
    trace = parse_trace(open(os.path.join(wd, "stderr.txt")).read(), info["obj_name"])
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
    # per-file parse results, reused while the file's mtime and size stay the same (9932 files: 1.6 s
    # to parse, 50 ms to stat)
    path = os.path.join(CACHE, "code_status.pickle")
    try:
        with open(path, "rb") as f:
            old = pickle.load(f)
    except (OSError, EOFError, pickle.UnpicklingError):
        old = {}
    new = {}
    for p in sorted(Path(ROOT, "src").rglob("*.c")):
        rel = str(p.relative_to(ROOT))
        st = p.stat()
        stamp = (st.st_mtime_ns, st.st_size)
        if old.get(rel, (None,))[0] == stamp:
            new[rel] = old[rel]
        else:
            new[rel] = (stamp, progress.c_functions(p, with_asm=False),
                        [n for n, _ in progress.nonmatching_functions(p)])
        _, exact, drafts = new[rel]
        for n in exact:
            if n in known:
                status[known[n]] = "matched"
                files[known[n]] = rel
        for n in drafts:
            if n in known and status[known[n]] != "matched":
                status[known[n]] = "draft"
                files[known[n]] = rel
    if new != old:
        os.makedirs(CACHE, exist_ok=True)
        with open(path + ".tmp", "wb") as f:
            pickle.dump(new, f)
        os.replace(path + ".tmp", path)
    return status, files


def resolve_target(target, funcs):
    """A function name from a name, an address, or a sub_XXXXXXXX label renamed since."""
    m = re.fullmatch(r"(?:sub_|0x)?(0?8[0-9A-Fa-f]{6})", target)
    if m and target not in dict(funcs):
        a = int(m.group(1), 16)
        return next((n for n, x in funcs if x == a), target)
    return target


def cmd_rom(target):
    """FUNC's ROM listing (no draft needed): calls and literal words named from the ROM ELF."""
    import progress
    funcs, sizes = progress.load_functions()
    target = resolve_target(target, funcs)
    addr = dict(funcs).get(target)
    if addr is None:
        sys.exit(f"{target}: not in the function inventory")
    code = rom_bytes()[off(addr):off(addr) + sizes[target]]
    arm = is_arm(target, addr)
    data = rom_data(target, addr, code, arm)
    names = {a & ~1: n for n, a in funcs}
    for n, (v, _) in elf_syms().items():
        if not n.startswith(("_", "$")) and not n.endswith("__addr"):
            names.setdefault(v & ~1, n)
    status, files = code_status()
    print(f"{target} @ {addr:08X}, {len(code)} bytes, {status.get(target, '?')}"
          + (f" ({files[target]})" if target in files else "") + (", ARM" if arm else ""))
    if arm:
        print("  (ARM code: scope lists it, but its compiler is Thumb agbcc only; drafts of ARM units"
              " compile with tools/check.py, without why/try)")
    for o, n, text, _, tgt in decode(code, addr, data, arm):
        note = ""
        m = re.match(r"(bl|blx|b) #0x([0-9a-f]+)$", text)
        if m and int(m.group(2), 16) & ~1 in names:
            text = f"{m.group(1)} " + names[int(m.group(2), 16) & ~1]
        m = re.match(r"(ldr \w+, =)0x([0-9A-Fa-f]+)$", text)
        if m and int(m.group(2), 16) & ~1 in names:
            note = names[int(m.group(2), 16) & ~1]
        m = re.match(r"\.word 0x([0-9A-F]+)$", text)
        if m and int(m.group(1), 16) & ~1 in names:
            note = names[int(m.group(1), 16) & ~1]
        print(f"  {o:4X}  {text:36}" + (f"@ {note}" if note else ""))
    return 0


def rom_refs(name, funcs, sizes, names):
    """What FUNC's ROM refers to, as a Counter: ("call", callee) per bl, ("const", value) per literal-pool
    word and per immediate of a mov/cmp/add/sub (offsets, counts and masks a twin's C may have wrong)."""
    import collections
    addr = dict(funcs)[name]
    code = rom_bytes()[off(addr):off(addr) + sizes[name]]
    out = collections.Counter()
    for _, _, text, _, _ in decode(code, addr, rom_data(name, addr, code)):
        m = re.match(r"bl #0x([0-9a-f]+)$", text)
        if m:
            t = int(m.group(1), 16) & ~1
            out[("call", names.get(t, f"0x{t:08X}"))] += 1
            continue
        m = re.match(r"\.word 0x([0-9A-F]+)$", text)
        if m:
            v = int(m.group(1), 16)
            out[("const", names.get(v & ~1, f"0x{v:X}"))] += 1
            continue
        m = re.match(r"(?:movs|cmp|adds|subs) \w+, (?:\w+, )?#(0x[0-9a-f]+|\d+)$", text)
        if m and int(m.group(1), 0) > 1:
            out[("const", f"#0x{int(m.group(1), 0):X}")] += 1
    return out


def twin_differences(twin, name, funcs, sizes):
    """Lines saying which calls and constants FUNC's ROM has that its twin's has not, and the reverse."""
    names = {a & ~1: n for n, a in funcs}
    a, b = rom_refs(name, funcs, sizes, names), rom_refs(twin, funcs, sizes, names)
    lines = []
    for kind in ("call", "const"):
        mine = sorted(f"{v}" + (f" x{n}" if n > 1 else "") for (k, v), n in (a - b).items() if k == kind)
        theirs = sorted(f"{v}" + (f" x{n}" if n > 1 else "") for (k, v), n in (b - a).items() if k == kind)
        if mine or theirs:
            lines.append(f"{kind}s only in {name}: {', '.join(mine[:12]) or '-'}; only in {twin}: "
                         f"{', '.join(theirs[:12]) or '-'}")
    return lines


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


def like_index():
    import structure
    import progress
    funcs, sizes = progress.load_functions()
    elf = os.path.join(ROOT, "build", "boktai3.elf")
    return cached("like_index", [elf, os.path.join(ROOT, "symbols", "functions.csv")],
                  lambda: structure.build_index(funcs, sizes, rom_bytes(), decode, rom_pool_ranges))


def like_tables():
    import structure
    elf = os.path.join(ROOT, "build", "boktai3.elf")
    return cached("like_tables", [elf, os.path.join(ROOT, "symbols", "functions.csv")],
                  lambda: structure.tables(like_index()))


def work_paths(name):
    d = os.path.join(WORK, "work")
    return os.path.join(d, name + ".c"), os.path.join(d, name + ".base.c")


def refresh_workspace(draft):
    """A unit workspace draft whose unit changed since `new` (a neighbour landed): three-way merge the
    unit's changes into the draft and move the base forward, so the draft compiles against today's unit."""
    import land
    if not draft.startswith(os.path.relpath(os.path.join(WORK, "work"), ROOT) + os.sep) or not draft.endswith(".c"):
        return
    name = os.path.basename(draft)[:-2]
    path, base = work_paths(name)
    if not (os.path.exists(path) and os.path.exists(base)):
        return
    import progress
    addr = dict(progress.load_functions()[0]).get(name)
    if addr is None:
        return
    kind, unit, _ = land.where(ROOT, name, addr)
    if kind != "unit":
        return
    if not unit or not unit.endswith(".c") or not os.path.exists(os.path.join(ROOT, unit)):
        return
    cur, old = open(os.path.join(ROOT, unit)).read(), open(base).read()
    if cur == old:
        return
    merged = land.merge(cur, old, open(path).read())
    if merged is None:
        print(f"{name}: {unit} changed since the workspace was made and the merge conflicts;"
              f" resolve by hand (base: {os.path.relpath(base, ROOT)})")
        return
    open(path, "w").write(merged)
    open(base, "w").write(cur)
    print(f"{name}: {unit} changed since the workspace was made; merged its changes into the draft")


def ghidra_c(addr):
    """Ghidra's pseudo-C for ADDR from the export at gen/ghidra/decomp.c (tools/ghidra/README.md), or None."""
    path = os.path.join(ROOT, "gen", "ghidra", "decomp.c")
    if not os.path.exists(path):
        return None
    out, take = [], False
    for ln in open(path, encoding="utf-8", errors="replace"):
        if ln.startswith("// ===== "):
            if take:
                break
            take = ln.startswith(f"// ===== {addr:08X} ")
        elif take:
            out.append(ln)
    return "".join(out) or None


SP_SLOT = re.compile(r"\[sp(?:, #(0x[0-9a-fA-F]+|\d+))?\]")


def frame_slots(wd, func):
    """draft sp offset -> (pseudo, bytes, order given out, spilled-from register) from scope's agbcc
    S lines (reload's stack slots; order -1 for a parameter passed on the stack)."""
    func = dump_func(wd, func)
    out = {}
    path = os.path.join(wd, "names.txt")
    for ln in open(path) if os.path.exists(path) else []:
        p = ln.split()
        if len(p) == 7 and p[0] == "S" and p[1] == func:
            out.setdefault(int(p[3]), (int(p[2]), int(p[4]), int(p[5]), int(p[6])))
    return out


def pseudo_value(wd, pseudo):
    """A short reading of what an unnamed pseudo holds: the source of its first set before allocation,
    with the C line."""
    lines = uid_lines(wd)
    for u, _, r in insn_list(wd, "lreg"):
        m = re.match(r"\(set \(reg(?:/\w)*:\w+ %d\) (.*)\)$" % pseudo, r)
        if m:
            v = re.sub(r"\(reg(?:/\w)*:\w+ (\d+)\)", r"r\1", m.group(1))
            v = re.sub(r"\(const_int (-?\d+)\)", r"\1", v)
            v = re.sub(r"\s*\[\s*\d+\s*\]|\(expr_list.*$", "", v)
            return (v[:60] + ("..." if len(v) > 60 else "")), lines.get(u)
    return None, None


def cmd_giv(src, func, env):
    """Loop strength reduction from loop.c's own dump (u.i.loop): each loop's induction variables (bivs:
    counters stepped by a constant) and the values computed from them (givs), with the C line, loop.c's
    benefit, lifetime and decision. A giv is reduced (given its own register, stepped each pass) when
    net benefit x lifetime x threshold >= the loop's insns, where net = the cost of computing it from
    the biv (2 per add or shift) minus 2 per biv increment (and a copy if it is not replaceable); a
    biv used only for counting can then be eliminated and the loop reversed (counting down to 0)."""
    n, info = score(src, func, env, full=True)
    wd = info["wd"]
    path = os.path.join(wd, "u.i.loop")
    if not os.path.exists(path):
        sys.exit(f"{func}: no loop dump (u.i.loop) in {os.path.relpath(wd, ROOT)}")
    lines = uid_lines(wd)
    fnames = {func, rom_name(src, func), info.get("obj_name")}
    text, keep = [], False
    for ln in open(path).read().splitlines():
        m = re.match(r";; Function (\w+)", ln)
        if m:
            keep = m.group(1) in fnames
            continue
        if keep:
            text.append(ln)
    if not text:
        sys.exit(f"{func}: not in the loop dump")
    ln_of = lambda u: lines.get(int(u), "?")
    loops, cur = [], None
    for ln in text:
        m = re.match(r"Loop from (\d+) to (\d+): (\d+) real insns", ln)
        if m:
            cur = dict(span=(int(m.group(1)), int(m.group(2))), insns=int(m.group(3)), bivs={}, givs={}, notes=[])
            loops.append(cur)
            continue
        if cur is None:
            continue
        m = re.match(r"Biv (\d+) initialized at insn (\d+): initial value (.*)", ln)
        if m:
            cur["bivs"][m.group(1)] = dict(init=m.group(3).strip(), fate="")
            continue
        m = re.match(r"(?:Cannot eliminate biv (\d+)(.*)|biv (\d+) can be eliminated)", ln)
        if m:
            r = m.group(1) or m.group(3)
            b = cur["bivs"].setdefault(r, dict(init="?", fate=""))
            if m.group(3):
                b["fate"] = "eliminated (counting only: the loop can run backwards)"
            else:
                u = re.search(r"insn (\d+)", m.group(2) or "")
                b["fate"] = "kept" + (f": used at line {ln_of(u.group(1))} (insn {u.group(1)})" if u else "")
            continue
        m = re.match(r"Insn (\d+): (giv reg \d+|dest address|dest reg \d+) src reg (\d+) benefit (-?\d+) lifetime (\d+)"
                     r"(.*?)mult (\S+) add (.*)", ln)
        if m:
            uid = m.group(1)
            cur["givs"][uid] = dict(kind="address" if "address" in m.group(2) else "value", biv=m.group(3),
                                    benefit=int(m.group(4)), life=int(m.group(5)),
                                    repl="replaceable" in m.group(6), mult=m.group(7), add=m.group(8).strip(),
                                    fate="?")
            continue
        m = re.match(r"giv of insn (\d+) not worth while, (-?\d+) vs (\d+)", ln)
        if m and m.group(1) in cur["givs"]:
            cur["givs"][m.group(1)]["fate"] = f"kept, recomputed each pass ({m.group(2)} < {m.group(3)} insns)"
            continue
        m = re.match(r"giv at (\d+) reduced to", ln)
        if m and m.group(1) in cur["givs"]:
            cur["givs"][m.group(1)]["fate"] = "REDUCED: own register, stepped each pass"
            continue
        m = re.match(r"giv at (\d+) combined with giv at (\d+)", ln)
        if m and m.group(1) in cur["givs"]:
            cur["givs"][m.group(1)]["fate"] = f"combined with insn {m.group(2)} (line {ln_of(m.group(2))})"
    status = "MATCH" if n == 0 else info.get("counts", "")
    print(f"{func}: {status}; loop strength reduction, {len(loops)} loop scan(s) (loop.c runs twice)")
    for L in loops:
        ls = sorted({ln_of(u) for u in list(L["givs"]) if ln_of(u) != "?"})
        print(f"  loop insns {L['span'][0]}-{L['span'][1]} ({L['insns']} insns), givs on line(s) {', '.join(map(str, ls)) or '-'}:")
        for r, b in L["bivs"].items():
            print(f"    biv  pseudo {r} from {b['init'][:40]}: {b['fate'] or 'kept'}")
        for uid, g in L["givs"].items():
            if g["kind"] == "address" and g["benefit"] <= 0:
                continue  # an address used once: never worth it, never interesting
            add = re.sub(r"\s+", " ", g["add"])[:50]
            print(f"    giv  line {ln_of(uid):>4} insn {uid:>4}  {g['kind']} = biv {g['biv']} x {g['mult']} + {add}"
                  f"  benefit {g['benefit']} life {g['life']}{'' if g['repl'] else ' (not replaceable)'}: {g['fate']}")
    print("  A value the ROM recomputes from the counter each pass but the draft REDUCED: make its computation a\n"
          "  single add of the counter and one invariant register (benefit 2, net 0: never reduced), e.g. hold the\n"
          "  invariant part in a variable first. One the ROM steps but the draft recomputes: compute it with two\n"
          "  operations (counter * size + base, counter << k plus an offset). `probe` flips each with AGBCC_NOGIV/GIV.")
    return 0


def cmd_hoist(src, func, env):
    """Loop-invariant motion, pass by pass: every invariant set loop.c saw in each loop, whether it
    moved it out (threshold x savings x lifetime >= insns in the loop; the threshold starts at 13 in
    a loop with a call, 26 without, and drops by 3 after each hoist; loop.c runs twice), and why the
    others stayed: set in two blocks or set again after a use (may_not_optimize), set after a branch
    or read before its set while used across blocks (maybe_never / used_before), or the move test."""
    n, info = score(src, func, env, full=True)
    wd = info["wd"]
    tr = subprocess.run([INSTR] + build.cflags_for(src) + ["-g", "u.i", "-o", "/dev/null"], cwd=wd,
                        capture_output=True, text=True,
                        env=dict(os.environ, AGBCC_LOOP_TRACE="1", **info["env"])).stderr
    fnames = (func, rom_name(src, func))
    names, lines = pseudo_names(wd, info["obj_name"]), uid_lines(wd)
    passes, cur = [], None
    for ln in tr.splitlines():
        m = re.match(r"loopopt fn=(\w+)", ln)
        if m:
            cur = {} if m.group(1) in fnames else None
            if cur is not None:
                passes.append(cur)
            continue
        if cur is None:
            continue
        m = re.match(r"cand fn=\w+ loop=(\d+) insn=(\d+) regno=(\d+) may_not=(\d) maybe_never=(\d) used_before=(\d)"
                     r" in_block=(\d) set_in_loop=(\d+) invariant=(\d)", ln)
        if m:
            loop, uid, reg, mn, never, before, blk, sets, inv = map(int, m.groups())
            last = cur.setdefault(loop, {})  # move_movables runs right after this loop's scan
            if inv:
                cur.setdefault(loop, {})[uid] = dict(reg=reg, why=(
                    f"set {sets} times in the loop, in two blocks or again after a use (may_not_optimize)" if mn else
                    ("set after a branch in the body" if never else "read before its set") +
                    " and used across blocks" if (never or before) and not blk else None), move=None)
            continue
        m = re.match(r"move fn=\w+ insn=(\d+) regno=\d+ threshold=(-?\d+) savings=(\d+) lifetime=(\d+)"
                     r" insn_count=(\d+) moved_once=(\d) already_moved=(\d)", ln)
        if m:
            uid, t, sv, life, cnt, once, done = map(int, m.groups())
            if uid in last:
                need = cnt * (2 if once else 1)
                ok = done or t * sv * life >= need
                last[uid]["move"] = (ok, f"{t} x {sv} x {life} {'>=' if ok else '<'} {need}", life)
    if not passes:
        sys.exit(f"no loop trace for {func}: rebuild scope's agbcc (sh tools/scope/build_agbcc.sh), or it has no loop")
    print(f"{func}: {info.get('counts') or 'MATCH'}; loop invariants, {len(passes)} loop pass(es)"
          " (threshold x savings x lifetime >= insns moves it)")
    for i, ps in enumerate(passes, 1):
        for loop, cands in ps.items():
            first = min(cands, key=lambda u: lines.get(u, 1 << 30)) if cands else None
            print(f"  pass {i}, loop {loop} (from line {lines.get(first, '?')}):")
            quiet = 0
            for uid, c in cands.items():
                name = names.get(c["reg"], "")
                what = f"line {lines.get(uid, '?'):>4}  insn {uid:>5}  pseudo {c['reg']}" + (f" `{name}`" if name else "")
                if c["move"] and (c["move"][0] or c["move"][2] > 2):
                    print(f"    {'moved' if c['move'][0] else 'kept '}  {what}  ({c['move'][1]})")
                elif c["why"]:
                    print(f"    stays  {what}  ({c['why']})")
                else:
                    quiet += 1
            if quiet:
                print(f"    ({quiet} more used once or twice: kept, or folded into the use)")
    print("  probe flips each hoist (AGBCC_NOMOVE=UID); a set the ROM keeps in the loop needs one of the"
          " 'stays' reasons in C (see probe's note).")
    if n:
        hoist_levers(src, func, env, passes, names, lines, n, info)
    return 0


def hoist_levers(src, func, env, passes, names, lines, base, info):
    """For each named variable loop.c hoisted, write the variant that stores it again (dead) just
    before its loop's closing brace and score it: the second set in a later block makes it
    may_not_optimize, and flow deletes the dead store (EffectManager_SpawnParticles and
    EffectManager_SpawnParticlesAt matched this way)."""
    text = open(os.path.join(ROOT, src)).read()
    rows = text.split("\n")
    tried = set()
    for ps in passes:
        for cands in ps.values():
            for uid, c in cands.items():
                name, ln = names.get(c["reg"], ""), lines.get(uid)
                if not (c["move"] and c["move"][0] and re.fullmatch(r"[A-Za-z_]\w*", name) and ln) or name in tried:
                    continue
                tried.add(name)
                m = re.search(rf"\b{name}\s*=\s*([^;=][^;]*);", rows[ln - 1])
                head = next((i for i in range(ln - 1, -1, -1)
                             if re.search(r"\b(for|while)\s*\(|\bdo\b", rows[i])), None)
                if not m or head is None:
                    continue
                depth, close = 0, None
                for i in range(head, len(rows)):
                    for ch in rows[i]:
                        depth += (ch == "{") - (ch == "}")
                    if depth == 0 and "{" in "".join(rows[head:i + 1]) and i > head:
                        close = i
                        break
                if close is None or close < ln:
                    continue
                indent = re.match(r"\s*", rows[close]).group(0) + "    "
                out = rows[:close] + [f"{indent}{name} = {m.group(1).strip()}; /* dead: keeps loop.c from hoisting {name} */"] + rows[close:]
                var = os.path.join(os.path.relpath(os.path.join(WORK, "work"), ROOT),  # the run's own (SCOPE_RUN)
                                   os.path.basename(src)[:-2] + f".lever-{name}.c")
                os.makedirs(os.path.dirname(os.path.join(ROOT, var)), exist_ok=True)
                with open(os.path.join(ROOT, var), "w") as f:
                    f.write("\n".join(out))
                k, inf = score(var, func, env)
                res = "MATCH" if k == 0 else inf.get("counts", f"{k} differ")
                print(f"  lever: `{name} = {m.group(1).strip()};` again before line {close + 1}: {res}"
                      f" ({'better' if k < base else 'same' if k == base else 'worse'}; {var})")


def cmd_frame(src, func, env):
    """The stack frame side by side: each ROM stack slot, the draft slot the aligned insns use for it,
    and the pseudo (C variable) reload put there, in the order reload gave the slots out. Reload gives
    a slot to each pseudo global alloc left without a register, in pseudo-number order (the order the
    variables and temporaries were created), at rising offsets; pseudos it evicts later come after."""
    n, info = score(src, func, env, full=True)
    if n == 0:
        print(f"{func}: MATCH")
        return 0
    wd = info["wd"]
    rows = json.load(open(os.path.join(wd, "diff.json")))
    slots, names = frame_slots(wd, func), pseudo_names(wd, func)
    if not slots:
        path = os.path.join(wd, "names.txt")
        any_s = os.path.exists(path) and any(ln.startswith("S ") for ln in open(path))
        uses_sp = any(SP_SLOT.search(r[k] or "") for r in rows for k in ("rom", "draft"))
        if not any_s and uses_sp and not os.path.exists(path):
            sys.exit("no names.txt from this compile: rebuild scope's agbcc (sh tools/scope/build_agbcc.sh)")
        sys.exit(f"{func}: reload gave no pseudo a stack slot in the draft"
                 + ("; the sp accesses are locals that live in memory (arrays, structs, address-taken"
                    " variables: their order is declaration order) or outgoing call arguments, not spills"
                    if uses_sp else "; neither side uses the stack beyond pushes"))
    off = lambda t: [int(m.group(1) or "0", 0) for m in SP_SLOT.finditer(t or "")]
    size = lambda side: next((int(m.group(1), 0) for r in rows for m in [re.match(r"sub sp, #(\S+)$", r[side] or "")] if m), 0)
    votes, rom_seen, dr_seen = {}, set(), set()
    for r in rows:
        a, b = off(r["rom"]), off(r["draft"])
        rom_seen.update(a)
        dr_seen.update(b)
        if len(a) == 1 and len(b) == 1:
            votes.setdefault(a[0], {}).setdefault(b[0], 0)
            votes[a[0]][b[0]] += 1
    pair = {a: max(v, key=v.get) for a, v in votes.items()}

    def who(o):
        if o not in slots:
            return "", ""
        ps, _, seq, frm = slots[o]
        nm = names.get(ps)
        if not nm:
            v, ln = pseudo_value(wd, ps)
            nm = f"[{v}]" + (f" line {ln}" if ln else "") if v else "-"
        return f"{ps}", nm + ("" if seq != -1 else " (stack parameter)") + (f" (evicted from r{frm})" if seq > 0 and frm >= 0 else "")
    print(f"{func}: frame {size('rom'):#x} in the ROM, {size('draft'):#x} in the draft (sub sp)")
    print(f"  {'ROM':9} {'draft':9} {'pseudo':>6}  value (slots are given out in pseudo order)")
    for a in sorted(rom_seen):
        b = pair.get(a)
        ps, nm = who(b) if b is not None else ("", "")
        mark = " " if b == a else "!"
        print(f"{mark} sp+{a:<#6x} {('sp+%#x' % b) if b is not None else '?':9} {ps:>6}  {nm}")
    extra = sorted(set(slots) - set(pair.values()))
    for b in extra:
        ps, nm = who(b)
        print(f"+ {'':9} sp+{b:<#6x} {ps:>6}  {nm}  (the ROM has no slot for this)")
    order = [pair[a] for a in sorted(pair) if pair[a] in slots and slots[pair[a]][2] > 0]
    want = [slots[b][0] for b in order]
    if want != sorted(want):
        print("\nThe ROM's slot order needs these pseudos created in this order (declare or first compute"
              " them in it):")
        print("  " + " < ".join(f"{slots[b][0]} {names.get(slots[b][0], '')}".strip() for b in order))
    if extra:
        print(f"\n{len(extra)} draft slot(s) the ROM does not have: those values need a register in the ROM's"
              " allocation, or do not exist there (a temporary the ROM's C does not make).")
    return 1


def cmd_shape(src, func, env):
    """The diff with low registers blanked: only hunks where the order or the instructions themselves
    differ, each with its C lines. Register choice is set aside; `why all` explains what is left."""
    n, info = score(src, func, env, full=True)
    if n == 0:
        print(f"{func}: MATCH")
        return 0
    rows = json.load(open(os.path.join(info["wd"], "diff.json")))
    rom = [r for r in rows if r["rom"]]
    dr = [r for r in rows if r["draft"]]
    key = lambda t: re.sub(r"func\+0x[0-9A-F]+", "->", norm_insn(t))
    sm = difflib.SequenceMatcher(None, [key(r["rom"]) for r in rom], [key(r["draft"]) for r in dr], autojunk=False)
    ops = [o for o in sm.get_opcodes() if o[0] != "equal"]
    nr = sum(i2 - i1 for _, i1, i2, _, _ in ops)
    nd = sum(j2 - j1 for _, _, _, j1, j2 in ops)
    print(f"{func}: shape {nr + nd} ({nr} ROM / {nd} draft insns out of place or different, low registers"
          f" blanked) in {len(ops)} hunk(s); {count_text(rows)} with registers")
    for tag, i1, i2, j1, j2 in ops:
        lines = sorted({r["line"] for r in dr[j1:j2] if r["line"]} | {r["line"] for r in dr[max(0, j1 - 1):j1 + 1] if r["line"]},
                       key=lambda x: int(x) if str(x).isdigit() else 0)
        at = rom[i1]["rom_addr"] if i1 < len(rom) else "end"
        print(f"\n@ ROM {at}  C line(s) {', '.join(map(str, lines)) or '?'}  ({tag})")
        for k in range(max(i2 - i1, j2 - j1)):
            a = rom[i1 + k]["rom"] if i1 + k < i2 else ""
            b = dr[j1 + k]["draft"] if j1 + k < j2 else ""
            ln = dr[j1 + k]["line"] if j1 + k < j2 else ""
            print(f"  {a:34} {b:34} {ln}")
    print(f"\nnext: FILE.c why all (registers and moves) | try V.c...")
    return 1


def tree_prototype(name):
    """(definition head, file) from the first declaration of NAME in include/, else in src/ (a caller's
    own prototype); (None, None) when nothing declares it."""
    import land
    files = sorted(land.sources(ROOT), key=lambda p: not p.endswith(".h"))
    pat = re.compile(r"\b%s\s*\(" % re.escape(name))
    for p in files:
        text = open(p).read()
        if pat.search(text):
            head = land.prototype([text], name)
            if head:
                return head, os.path.relpath(p, ROOT)
    return None, None


def cmd_new(target):
    """Open FUNC for writing: a draft in build/scope/work/ that compiles and scores right away. A function
    in a unit gets a copy of the unit with an empty definition in place of its INCLUDE_ASM (landing merges
    it back); a function with no file gets the C of the matched function most like it (`like`, when that
    covers at least half its rare windows), renamed, or an empty definition."""
    import land
    import progress
    import structure
    funcs, sizes = progress.load_functions()
    name = resolve_target(target, funcs)
    addr = dict(funcs).get(name)
    if addr is None:
        sys.exit(f"{target}: not in the function inventory")
    status, files = code_status()
    if status.get(name) != "undrafted":
        sys.exit(f"{name} is {status.get(name)} in {files.get(name)}: work on that file")
    kind, dest, asm_name = land.where(ROOT, name, addr)
    draft, base = work_paths(name)
    if os.path.exists(draft):
        refresh_workspace(os.path.relpath(draft, ROOT))
        print(f"{name}: {os.path.relpath(draft, ROOT)} exists; continuing it (delete it to start over)")
        return 0
    os.makedirs(os.path.dirname(draft), exist_ok=True)
    seed = "an empty definition"
    if kind == "unit":
        text = open(os.path.join(ROOT, dest)).read()
        open(base, "w").write(text)
        incs = re.findall(r'^#include "([^"]+)"', text, re.M)
        hdrs = [open(os.path.join(ROOT, "include", h)).read() for h in incs if os.path.exists(os.path.join(ROOT, "include", h))]
        out = land.stub_unit(text, asm_name, name, hdrs)
        if re.search(r"^void %s\(void\)\n\{\n\}" % re.escape(name), out, re.M):
            # nothing in the unit or its headers declares it: a caller elsewhere may
            head, where_ = tree_prototype(name)
            if head and head != f"void {name}(void)":
                out = re.sub(r"^void %s\(void\)\n" % re.escape(name), head + "\n", out, count=1, flags=re.M)
                seed += f" with the signature {where_} declares"
    else:
        out = None
        best = next(iter(structure.similar(name, like_index(), status, like_tables())), None)
        if best and best[1] >= 0.5 and files.get(best[2]):
            twin, tfile = best[2], files[best[2]]
            ttext = open(os.path.join(ROOT, tfile)).read()
            defined = progress.c_functions(os.path.join(ROOT, tfile), with_asm=False)
            tname = twin if twin in defined else next((d for d in defined if d == f"sub_{dict(funcs)[twin]:08X}"), None)
            if tname:
                out = land.seed_from_twin(ttext, tname, name, [d for d in defined if d != tname])
                seed = f"{twin}'s C ({tfile}, covers {best[1]:.0%} of its rare windows), renamed"
                diffs = twin_differences(twin, name, funcs, sizes)
                pins_ = len(re.findall(r'\basm\("(?:r\d+|sb|sl|fp|ip)"\)', out))
                note = ([f"/* scope: seeded from {twin} ({tfile}); its ROM and {name}'s differ in:"]
                        + [f"   {d}" for d in diffs or ["nothing scope compares (calls, literals, immediates)"]]
                        + ([f"   The twin's C has {pins_} register pin(s): try without them (`pins`)."] if pins_ else [])
                        + ["   The twin's comments and #define alias describe the twin, not this function. */"])
                sp = land.definition_span(out, name)
                if sp:
                    out = (out[:sp[0]] + "\n".join(note) + f"\n// scope: write {name} here (`scope.py rom {name}`)\n"
                           + out[sp[0]:])
                for d in diffs:
                    print(f"  twin difference: {d}")
        if out is None:
            head, where_ = tree_prototype(name)
            if where_:
                seed += f" with the signature {where_} declares"
            out = (f'#include "global.h"\n\n// scope: write {name} here (`scope.py rom {name}`)\n'
                   f"{head or f'void {name}(void)'}\n{{\n}}\n")
    g = ghidra_c(addr)
    if g:  # a reading aid inside the stub: Ghidra's types and control flow, never matching C
        out = out.replace(f"// scope: write {name} here", "/* Ghidra's pseudo-C (a reading aid):\n"
                          + g.replace("*/", "* /") + "*/\n" + f"// scope: write {name} here", 1)
        seed += " (with Ghidra's pseudo-C as a comment)"
    open(draft, "w").write(out)
    rel = os.path.relpath(draft, ROOT)
    print(f"{name} @ {addr:08X}, {sizes[name]} bytes, {kind}: lands in {dest}"
          + ("; ARM: scope cannot compile it (Thumb agbcc only), use tools/check.py" if is_arm(name, addr) else ""))
    print(f"  draft {rel}: {seed}")
    r = fast_score(rel, name, {})
    if r is not None:
        print(f"  now: {r[1]}, shape {fast_score.shape if r[0] else 0}")
    print(f"next: scope.py {rel}  |  rom {name}  |  {rel} try V.c...  |  land {name} [--name NEW]")
    return 0


def cmd_land(target, draft=None, new_name=None, rom=False, note=None, nonmatching=False):
    """Put a matching draft into the source tree: merged into its unit, or a new src/fn file (a hidden
    function also gets its replacement end). Then the concession marker, clang-format, a check of
    every C function in the landed file, and optionally the name and a whole-ROM build.
    NONMATCHING: a draft that does not match yet goes in as `#ifdef NONMATCHING` C with its fuzzy
    score, and the default build keeps the INCLUDE_ASM (docs/WORKER.md); the function counts as drafted."""
    import land
    import progress
    funcs, sizes = progress.load_functions()
    name = resolve_target(target, funcs)
    addr = dict(funcs).get(name)
    if addr is None:
        sys.exit(f"{target}: not in the function inventory")
    wdraft, base = work_paths(name)
    draft = draft or wdraft
    rel = os.path.relpath(os.path.abspath(draft), ROOT)
    n, info = score(rel, name, {})
    if n != 0 and not nonmatching:
        sys.exit(f"{name}: {rel} does not match ({info.get('counts')}); nothing landed"
                 " (`land FUNC --nonmatching` lands it as a draft)")
    if n == 0 and nonmatching:
        nonmatching = False
        print(f"{name}: the draft matches; landing it as exact C")
    kind, dest, asm_name = land.where(ROOT, name, addr)
    # a hidden function's split block has its own asm/nonmatching file, so its INCLUDE_ASM fallback works
    # as a plain function's does (its replacement end stays unset: the default build keeps the assembly)
    if new_name:  # a name that cannot be applied fails before anything is written
        try:
            land.rename(ROOT, addr, name, new_name, dry=True)
        except ValueError as e:
            sys.exit(f"{name}: {e}; nothing landed")
    path = os.path.join(ROOT, dest)
    text = open(os.path.abspath(draft)).read()
    before = {path: open(path).read() if os.path.exists(path) else None}
    extras = os.path.join(ROOT, "symbols", "build_extra_functions.csv")
    before[extras] = open(extras).read()
    if kind == "unit":
        cur = before[path]
        if not os.path.exists(base):
            # no record of the unit the draft started from: take the draft whole only if, outside
            # FUNC, it is the unit as it is now (else it would undo neighbors that landed since)
            sp = land.definition_span(text, name)
            rest = text[:sp[0]] + text[sp[1]:] if sp else text
            now = re.sub(r'^INCLUDE_ASM\("[^"]*", %s\);\n' % re.escape(asm_name or name), "", cur, flags=re.M)
            strip = lambda t: re.sub(r"^// scope: write .*\n|\s+", "", t, flags=re.M)
            if strip(rest) != strip(now):
                sys.exit(f"{name}: no workspace base for {rel}, and outside {name} it differs from {dest};"
                         " land by hand (or reopen with `new`)")
        merged = land.merge(cur, open(base).read() if os.path.exists(base) else cur, text)
        if merged is None:
            sys.exit(f"{name}: {dest} changed since `new` in a way that conflicts with the draft; land by hand")
        text = merged
    elif before[path] is not None:
        sys.exit(f"{name}: {dest} exists already")
    else:
        # a new file holds FUNC (and any hidden neighbor written alongside it). A body of a function that
        # already has a file (a stale twin seed's neighbors) would define it twice and break split.py
        status, _ = code_status()
        names = {a: f for f, a in funcs}
        def owned(f):
            m = re.fullmatch(r"sub_([0-9A-F]{8})", f)
            return status.get(names.get(int(m.group(1), 16), f) if m else f, "undrafted") != "undrafted"
        stale = [f for f in progress.c_functions(os.path.abspath(draft), with_asm=False) if f != name and owned(f)]
        if stale:
            text = land.seed_from_twin(text, name, name, stale)
            print(f"{name}: dropped {', '.join(stale)} from the draft (defined in other files already)")
    text = re.sub(r"^// scope: write .*\n", "", text, flags=re.M)
    text = re.sub(r"/\* Ghidra's pseudo-C \(a reading aid\):\n.*?\*/\n", "", text, flags=re.S)
    text = re.sub(r"/\* scope: seeded from .*?\*/\n", "", text, flags=re.S)
    if nonmatching:
        text = land.wrap_nonmatching(text, name, asm_name or name, info.get("fuzzy", 0.0), info.get("shape"))
        if text is None:
            sys.exit(f"{name}: no definition of {name} in {rel}")
    text, pins, bars = land.concessions(text)

    def restore():
        if getattr(restore, "committed", False):  # validated and named: later steps (the ROM build) keep it
            return
        for p, t in before.items():
            if t is None:
                if os.path.exists(p):
                    os.remove(p)
            else:
                open(p, "w").write(t)
    try:
        return land_write(name, addr, kind, dest, path, text, info, nonmatching, funcs, sizes, restore,
                          pins, bars, new_name, note, rom, wdraft, base, draft)
    except BaseException:
        # any failure after the first write (a compile error in the check, a format crash, ^C) puts the
        # tree back as it was; land_write restores and exits itself for a plain mismatch
        restore()
        raise


def land_write(name, addr, kind, dest, path, text, info, nonmatching, funcs, sizes, restore, pins, bars,
               new_name, note, rom, wdraft, base, draft):
    import land
    import progress
    open(path, "w").write(text)
    fmt = os.path.join(ROOT, "build", "venv", "bin", "clang-format")
    subprocess.run([fmt if os.path.exists(fmt) else "clang-format", "-i", path], cwd=ROOT)
    # every hidden function the landed file now defines gets its reviewed end (one landed into a unit
    # too, and a hidden neighbor the draft wrote alongside it)
    at = dict(funcs)
    for f in progress.c_functions(path, with_asm=False):
        a = at.get(f)
        row = land.hidden_row(ROOT, a) if a is not None else None
        if row and not row.get("replacement_end") and not (nonmatching and f == name):
            land.set_replacement_end(ROOT, a, a + sizes[f], sizes[f])
    bad = []
    for f in progress.c_functions(path, with_asm=False):
        k, inf = score(dest, f, {})
        if k != 0:
            bad.append(f"{f}: {inf.get('counts')}")
    if bad:
        restore()
        sys.exit(f"{name}: after landing, {dest} has functions that do not match ({'; '.join(bad[:4])});"
                 " restored")
    if nonmatching:  # the project's fuzzy figure (progress.py reads it); the system python3, as check.py runs in CI
        subprocess.run(["python3", "tools/check.py", dest, "--nonmatching", "--record"], cwd=ROOT,
                       capture_output=True)
        m = re.search(r"#ifdef NONMATCHING\n// fuzzy: ([\d.]+)%\n(?:.*\n){0,3}?[^\n]*\b" + re.escape(name) + r"\(",
                      open(path).read())
        info["fuzzy"] = float(m.group(1)) if m else info.get("fuzzy", 0.0)
    print(f"{name}: landed in {dest} ({kind}; {pins} pins, {bars} barriers in the file)"
          + (f"; NONMATCHING draft, fuzzy {info.get('fuzzy', 0.0):.1f}%" if nonmatching else "")
          + ("; build_extra_functions.csv replacement end set" if kind == "hidden" and not nonmatching else ""))
    if new_name:
        cmd_rename(name, new_name, note)
        name = new_name
    restore.committed = True
    if rom:
        r = subprocess.run(["make", "-j8"], cwd=ROOT, capture_output=True, text=True)
        ok = "build/boktai3.gba: OK" in r.stdout.splitlines()
        print("build/boktai3.gba: OK" if ok else "ROM build FAILED:\n" + "\n".join((r.stdout + r.stderr).splitlines()[-12:]))
        if not ok:
            return 1
    for p in (wdraft, base):
        if os.path.exists(p) and os.path.abspath(draft) == wdraft:
            os.replace(p, p + ".landed")
    return 0


def cmd_rename(target, new, note=None):
    """Name a function: its functions.csv row and every spelling of the old name or label in src/ and
    include/. Hidden functions work too (split.py names them from the csv)."""
    import land
    import progress
    funcs, _ = progress.load_functions()
    name = resolve_target(target, funcs)
    addr = dict(funcs).get(name)
    if addr is None:
        sys.exit(f"{target}: not in the function inventory")
    row = land.hidden_row(ROOT, addr)
    mode = row["isa"] if row else ("arm" if any(a == addr and m == "arm" for a, m in arm_starts()) else "thumb")
    try:
        changed = land.rename(ROOT, addr, name, new, mode=mode, note=note)
    except ValueError as e:
        sys.exit(str(e))
    print(f"{name} -> {new}: " + ", ".join(f"{f} ({k})" for f, k in changed))
    print("  names reach the ROM link: run `make` (or `land ... --rom`) before committing")
    return 0


def arm_starts():
    """[(addr, 'arm')] from functions.csv, read once per command."""
    if "arm_starts" not in _MEMO:
        import csv as _csv
        with open(os.path.join(ROOT, "symbols", "functions.csv"), newline="") as f:
            _MEMO["arm_starts"] = [(int(r["addr"], 16), r["mode"]) for r in _csv.DictReader(f) if r["mode"] == "arm"]
    return _MEMO["arm_starts"]


def cmd_todo(show, order):
    """Phase-1 work list: undrafted functions, most matched prior art first."""
    import structure
    import progress
    funcs, sizes = progress.load_functions()
    status, _ = code_status()
    import land
    index = land.include_asm_index(ROOT)
    arm = {n for n, a in funcs if status.get(n) == "undrafted" and is_arm(n, a, index)}
    return structure.todo(like_index(), status, funcs, sizes, progress.load_asm_ranges(), show, order,
                          like_tables(), arm)


def cmd_like(target, show):
    import structure
    import progress
    funcs, sizes = progress.load_functions()
    elf = os.path.join(ROOT, "build", "boktai3.elf")
    index = cached("like_index", [elf, os.path.join(ROOT, "symbols", "functions.csv")],
                   lambda: structure.build_index(funcs, sizes, rom_bytes(), decode, rom_pool_ranges))
    target = resolve_target(target, funcs)
    status, files = code_status()
    return structure.like(target, index, status, files, show, like_tables())


def cmd_serve():
    """The resident scope: warm caches in this process, one forked child per command (children can run
    side by side; each starts its own agbcc fork servers). Restarts itself when scope.py or fast.py
    changes, or scope's agbcc is rebuilt; exits after SCOPE_SERVE_IDLE seconds (default 1800) without a command."""
    import signal
    import traceback
    code_files = [os.path.abspath(__file__), os.path.join(HERE, "fast.py"), INSTR]  # INSTR: a rebuilt compiler
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
            rows.append((n, v, info.get("counts", "MATCH" if n == 0 else "?"), "knob", info))
            continue
        path = v if os.path.isabs(v) else os.path.join(os.getcwd(), v)
        if not os.path.exists(path):
            path = os.path.join(ROOT, v)
        rel = os.path.relpath(path, ROOT)
        if not os.path.isfile(path):
            rows.append((None, rel, "source file not found", "missing file", {}))
            continue
        try:
            n, info = score(rel, func, env)
        except SystemExit as e:
            n, info = None, {"counts": "compile fails: " + str(e.code).splitlines()[-1].strip()[:60]}
        rows.append((n, rel, info.get("counts", "MATCH" if n == 0 else "?"), c_change(base_lines, path), info))
    dt = time.perf_counter() - t0
    knobs = " ".join(f"{k}={v}" for k, v in env.items())
    print(f"{func}: base {src}{' [' + knobs + ']' if knobs else ''}: {base_info.get('counts', 'MATCH')}"
          f"{', shape ' + str(base_info['shape']) if base_n else ''}."
          f" {len(variants)} variants in {dt:.2f}s ({dt / max(1, len(variants) + 1) * 1000:.0f} ms each)")
    def key(n, info):
        """Rank: order and shape first (register choice set aside), then the larger side, the total, rows."""
        m = re.match(r"(\d+) ROM / (\d+) draft", info.get("counts") or "")
        if n is None:
            return None
        if n == 0:
            return (0, 0, 0, 0)
        sh = info.get("shape", n)
        return (sh, max(int(m[1]), int(m[2])), int(m[1]) + int(m[2]), n) if m else (sh, n, n, n)
    base_k = key(base_n, base_info)
    rows.sort(key=lambda r: (r[0] is None, key(r[0], r[4]) or (0,)))
    def total(info):
        m = re.match(r"(\d+) ROM / (\d+) draft", info.get("counts") or "")
        return int(m[1]) + int(m[2]) if m else None
    base_total = total(base_info) if base_n else 0
    for n, name, counts, change, info in rows:
        k = key(n, info)
        mark = "MATCH" if n == 0 else ("better" if k is not None and k < base_k else
                                       "same" if k == base_k else "worse" if k is not None else "")
        t = total(info) if n else None
        if mark == "worse" and t is not None and base_total and t < base_total:
            mark = "mixed"  # fewer differing insns, larger shape distance: often a step worth keeping
        if n:
            counts = f"{counts}, shape {info.get('shape', '?')}"
        print(f"  {mark:6} {counts:44} {name}" + ("" if change == "knob" else f"  [{change}]"))
    best = rows[0] if rows else None
    if any(r[0] and base_total and (total(r[4]) or base_total) < base_total and key(r[0], r[4]) >= base_k
           for r in rows if r[0] is not None):
        print("  mixed: fewer differing insns but a larger shape distance (ranked by shape). A change `why` or a\n"
              "  knob asked for is often worth keeping and stacking on (WORKER.md step 5).")
    if best and best[0] is not None and best[3] != "knob":
        print(f"\nnext: scope.py {best[1]} {func}   (the best variant's listing)")
    return 0 if any(r[0] == 0 for r in rows) else 1


def last_func():
    p = os.path.join(WORK, "LAST")
    return open(p).read().strip() if os.path.exists(p) else None


def source_function(src, explicit=None):
    """Select a file command's function once; explicit names must identify a definition.
    Accept a C spelling, its file-local asm alias, or a renamed sub_ADDR spelling.
    """
    if explicit is None:
        func = nonmatching_func(src)
        if not func:
            sys.exit(f"name the function: scope.py {src} COMMAND ... FUNC")
        return func
    import land
    text = open(os.path.join(ROOT, src)).read()
    _, addr, _ = inventory()
    aliases = dict(re.findall(r"^#define[ \t]+(\w+)[ \t]+(\w+)[ \t]*(?://[^\n]*)?$", text, re.M))
    spellings = [explicit]
    target = resolve_target(explicit, list(addr.items()))
    if target in addr:
        spellings.extend((target, f"sub_{addr[target]:08X}"))
    spellings.extend(n for n, target in aliases.items() if target in spellings)
    func = next((n for n in spellings if land.definition_span(text, n)), None)
    if func is None or rom_name(src, func) not in addr:
        sys.exit(f"{explicit}: no ROM function definition by that name in {src}")
    return func


def cli_error(syntax, message=None):
    sys.exit((message + "\n" if message else "") + f"usage: scope.py {syntax}")


def cli_number(value, syntax):
    try:
        n = int(value)
    except ValueError:
        cli_error(syntax, f"{value!r}: expected a nonnegative decimal integer")
    if n < 0:
        cli_error(syntax, f"{value!r}: expected a nonnegative decimal integer")
    return n


def validate_cli(args):
    """Check argument shapes before indexing argv or starting a compile/mutation."""
    cmd, tail = args[0], args[1:]
    fixed = {"rom": (1, 1, "rom FUNC|ADDR"), "new": (1, 1, "new FUNC"),
             "like": (1, 2, "like FUNC|ADDR [N]"), "insn": (1, 2, "insn UID [FUNC]"),
             "fastcheck": (0, 1, "fastcheck [N]"), "census": (0, 1, "census [CALLEE]"),
             "serve": (0, 0, "serve")}
    if cmd in fixed:
        lo, hi, syntax = fixed[cmd]
        if not lo <= len(tail) <= hi:
            cli_error(syntax)
        if cmd == "insn":
            cli_number(tail[0], syntax)
        elif cmd == "like" and len(tail) == 2:
            cli_number(tail[1], syntax)
        elif cmd == "fastcheck" and tail:
            cli_number(tail[0], syntax)
        return
    if cmd == "fields":
        return  # offsets and optional struct name are validated together in main
    if cmd == "rename":
        syntax = "rename FUNC NEW [--note TEXT]"
        if len(tail) not in (2, 4) or len(tail) == 4 and tail[2] != "--note":
            cli_error(syntax)
        return
    if cmd == "land":
        syntax = "land FUNC [DRAFT.c] [--name NEW] [--note TEXT] [--rom] [--nonmatching]"
        if not tail or tail[0].startswith("--"):
            cli_error(syntax)
        i, drafts = 1, 0
        while i < len(tail):
            a = tail[i]
            if a in ("--name", "--note"):
                if i + 1 == len(tail) or tail[i + 1].startswith("--"):
                    cli_error(syntax, f"{a} needs a value")
                i += 2
            elif a in ("--rom", "--nonmatching"):
                i += 1
            elif a.startswith("--"):
                cli_error(syntax, f"unknown option: {a}")
            else:
                drafts += 1
                i += 1
        if drafts > 1:
            cli_error(syntax)
        return
    if cmd == "todo":
        if len(tail) > 2 or any(a not in ("addr", "prior", "size") and not a.isdigit() for a in tail):
            cli_error("todo [N] [addr|prior|size]")
        return
    if cmd == "uses":
        if not any(a.isdigit() for a in tail):
            cli_error("uses REG... [PASS...] [FUNC]")
        return
    rest = [a for a in tail if "=" not in a]
    if "try" in rest:
        i = tail.index("try")
        if not tail[i + 1:] or all("=" not in a and not a.endswith(".c") for a in tail[i + 1:]):
            cli_error("FILE.c [FUNC] try VARIANT.c...|KNOB=V... [FUNC]")
    elif "why" in rest:
        i = rest.index("why")
        if len(rest) not in (2, 3) or i + 1 >= len(rest):
            cli_error("FILE.c why UID|all [FUNC]")
        if rest[i + 1] != "all":
            cli_number(rest[i + 1], "FILE.c why UID|all [FUNC]")
    elif rest and rest[0] == "insn":
        if len(rest) not in (2, 3):
            cli_error("FILE.c insn UID [FUNC]")
        cli_number(rest[1], "FILE.c insn UID [FUNC]")
    elif rest and rest[0] in ("life", "uses"):
        if len(rest) < 2 or rest[0] == "uses" and not any(a.isdigit() for a in rest[1:]):
            cli_error("FILE.c " + rest[0] + " NAME|REG... [FUNC]")
    elif rest and rest[0] == "vs":
        if len(rest) not in (2, 3):
            cli_error("FILE.c vs OTHER.c [FUNC]")
    elif rest and rest[0] == "pins":
        if len([a for a in rest if a != "--all"]) > 2:
            cli_error("FILE.c pins [FUNC] [--all]")
    elif any(a in rest for a in ("probe", "shape", "frame", "hoist", "spill")):
        if len(rest) > 2:
            cli_error("FILE.c COMMAND [FUNC]")
    elif len(rest) > 1:
        cli_error("FILE.c [FUNC] [KNOB=V...]")


def main():
    args = sys.argv[1:]
    if not args or args[0] in ("-h", "--help"):
        print(__doc__)
        return 0
    validate_cli(args)
    if args[0] == "fields":  # fields OFF... [STRUCT]: which structs have members at these offsets
        offs, names = [], []
        for a in args[1:]:
            if re.fullmatch(r"(?:0[xX])?[0-9a-fA-F]+", a):
                offs.append(int(a, 16 if a.lower().startswith("0x") or re.search(r"[a-fA-F]", a) else 10))
            else:
                names.append(a)
        if not offs or len(names) > 1 or names and not re.fullmatch(r"[A-Za-z_]\w*", names[0]):
            cli_error("fields OFF... [STRUCT]", "offsets: decimal, 0xHEX, or bare hex containing A-F")
        return cmd_fields(offs, names[0] if names else None)
    if args[0] == "rom":  # rom FUNC|ADDR: the ROM listing, no draft needed
        return cmd_rom(args[1])
    if args[0] == "new":  # new FUNC: a draft in build/scope/work/ seeded from the unit or a twin
        return cmd_new(args[1])
    if args[0] == "land":  # land FUNC [DRAFT.c] [--name NEW] [--note TEXT] [--rom] [--nonmatching]
        opt = {}
        rest = []
        it = iter(args[2:])
        for a in it:
            if a in ("--name", "--note"):
                opt[a[2:]] = next(it)
            elif a in ("--rom", "--nonmatching"):
                opt[a[2:]] = True
            else:
                rest.append(a)
        return cmd_land(args[1], rest[0] if rest else None, opt.get("name"), opt.get("rom", False), opt.get("note"),
                        opt.get("nonmatching", False))
    if args[0] == "rename":  # rename FUNC NEW [--note TEXT]
        note = args[args.index("--note") + 1] if "--note" in args else None
        return cmd_rename(args[1], args[2], note)
    if args[0] == "todo":  # todo [N] [prior|size|addr]: undrafted functions, most matched prior art first
        n = next((int(a) for a in args[1:] if a.isdigit()), 20)
        order = next((a for a in args[1:] if a in ("prior", "size", "addr")), "addr")
        return cmd_todo(n, order)
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
        if not func:
            cli_error("uses REG... [PASS...] FUNC", "no previous function in this SCOPE_RUN")
        cmd_uses(regs, func, passes)
        return 0
    if args[0] == "insn":
        func = args[2] if len(args) > 2 else last_func()
        if not func:
            cli_error("insn UID FUNC", "no previous function in this SCOPE_RUN")
        cmd_insn(int(args[1]), func)
        return 0
    src = args[0]
    if not src.endswith(".c"):
        src = os.path.join("src", "fn", src + ".c")
    src = os.path.relpath(os.path.join(ROOT, src) if not os.path.isabs(src) else src, ROOT)
    if not os.path.isfile(os.path.join(ROOT, src)):
        sys.exit(f"{src}: source file not found (run scope.py --help for commands)")
    refresh_workspace(src)
    env = dict(a.split("=", 1) for a in args[1:] if "=" in a)
    rest = [a for a in args[1:] if "=" not in a]
    why_uid = None
    if rest and rest[0] == "vs":  # FILE.c vs OTHER.c: allocation side by side
        other = rest[1]
        other = os.path.relpath(os.path.join(ROOT, other) if not os.path.isabs(other) else other, ROOT)
        if not os.path.isfile(os.path.join(ROOT, other)):
            cli_error("FILE.c vs OTHER.c [FUNC]", f"{other}: source file not found")
        func = source_function(src, rest[2] if len(rest) > 2 else None)
        return cmd_vs(src, other, func, env)
    if rest and rest[0] in ("insn", "uses", "life"):  # FILE.c insn UID / uses REG... / life NAME... [FUNC]
        params = rest[1:]
        explicit = None
        if rest[0] == "insn":
            explicit = params[1] if len(params) > 1 else None
            params = params[:1]
        elif rest[0] == "uses":
            if params and not params[-1].isdigit() and params[-1] not in PASSES:
                explicit = params.pop()
        elif params:
            import land
            last = params[-1]
            text = open(os.path.join(ROOT, src)).read()
            if last in inventory()[1] or land.definition_span(text, last) or re.fullmatch(r"sub_[0-9A-Fa-f]{8}", last):
                explicit = params.pop()
        func = source_function(src, explicit)
        if not params:
            cli_error("FILE.c " + rest[0] + " NAME|REG... [FUNC]")
        rest = [rest[0], *params]
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
        func = source_function(src, rest[1] if len(rest) > 1 else None)
        return cmd_pins(src, func, env)
    if "try" in rest:  # FILE.c [KNOB=V...] try VARIANT... [FUNC]: whole-file variants or single knobs
        i = args.index("try")
        before = [a for a in args[1:i] if "=" not in a]
        env = dict(a.split("=", 1) for a in args[1:i] if "=" in a)
        variants = args[i + 1:]
        func = before[0] if before else nonmatching_func(src)
        if variants and not variants[-1].endswith(".c") and "=" not in variants[-1]:
            func = variants.pop()
        func = source_function(src, func)
        return cmd_try(src, func, variants, env)
    if "probe" in rest:
        rest.remove("probe")
        func = source_function(src, rest[0] if rest else None)
        return cmd_probe(src, func, env)
    if "shape" in rest:  # FILE.c shape [FUNC]: only the order / instruction differences, registers blanked
        rest.remove("shape")
        func = source_function(src, rest[0] if rest else None)
        return cmd_shape(src, func, env)
    if "giv" in rest:  # FILE.c giv [FUNC]: loop strength reduction, each induction value and loop.c's decision
        rest.remove("giv")
        func = source_function(src, rest[0] if rest else None)
        return cmd_giv(src, func, env)
    if "hoist" in rest:  # FILE.c hoist [FUNC]: loop-invariant motion pass by pass, and why each stayed
        rest.remove("hoist")
        func = source_function(src, rest[0] if rest else None)
        return cmd_hoist(src, func, env)
    if "frame" in rest:  # FILE.c frame [FUNC]: stack slots, ROM against draft, with the pseudo in each
        rest.remove("frame")
        func = source_function(src, rest[0] if rest else None)
        return cmd_frame(src, func, env)
    if "spill" in rest:
        rest.remove("spill")
        func = source_function(src, rest[0] if rest else None)
        return cmd_spill(src, func, env)
    if "why" in rest:
        i = rest.index("why")
        why_uid = -1 if rest[i + 1] == "all" else int(rest[i + 1])
        rest = rest[:i] + rest[i + 2:]
    func = source_function(src, rest[0] if rest else None)
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
