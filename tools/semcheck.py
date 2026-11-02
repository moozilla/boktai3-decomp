#!/usr/bin/env python3
"""Does a draft do what the ROM function does? Differential execution.

  semcheck.py SRC [FUNC ...] [--nonmatching] [--trials N] [-v]
  semcheck.py --all [--out FILE]      audit every NONMATCHING draft in src/

Byte comparison says how close a draft's code is; it cannot say whether the
draft is right. A permuter edit can move a statement past a use, read a
variable before it is set or drop a store, and still score higher. This
tool runs the ROM function and the compiled draft side by side in a Thumb
emulator (unicorn) on the same inputs and compares what each one does:

* every call, in order: callee (by name), r0-r3, the outgoing stack
  arguments, and a digest of the memory the function had written so far;
* the final value of every non-stack byte either version wrote;
* the set of non-stack addresses each version read (outside its own code and
  literal pool), so a read of the wrong field shows even when its value does
  not change the path;
* r0 at return, when the C declares a return value.

Inputs are deterministic per trial: arguments are pointers into separate
heap blocks or small integers, every page is filled on first touch from a
seeded generator (so both runs read the same "memory"), and each call returns
a value derived from its callee and arguments. Callees are not executed.
Several trials vary the arguments and the memory; the report includes the
share of the ROM function's code the trials reached, which says how much an
EQUIVALENT verdict covers.

Verdicts: EQUIVALENT (all trials agree), DIFFERENT (first disagreement
shown), INCOMPLETE (a run hit the step limit or faulted the same way in
both: compared as far as it got). Exit status 1 if any function differs.

It is a test, not a proof: code no trial reaches is unchecked, and callees
that would read the function's writes are assumed to see them at the call
(the memory digest). Use it before writing a non-matching search result
back as a draft, and to audit existing drafts.
"""
import argparse
import ctypes
import hashlib
import os
import re
import struct
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import build  # noqa: E402
import check  # noqa: E402
from romlib import ROOT, load_rom, rom_data_ranges  # noqa: E402

try:
    import unicorn
    from unicorn import arm_const as A
except ImportError:  # pragma: no cover
    sys.exit("semcheck needs unicorn: build/venv/bin/pip install unicorn")

ROM_BASE = 0x08000000
PAGE = 0x1000
SENTINEL = 0x0E000000          # return address of the outermost call
STACK_TOP = 0x03007E00
STACK_LO = STACK_TOP - 0x2000
HEAP = 0x40000000              # argument blocks, 64 KB apart: no two alias
POOL = 0x10000000              # where pointers found in memory point (256 MB: no collisions)
STEP_LIMIT = 200000
CALLEE_SAVED = [A.UC_ARM_REG_R4, A.UC_ARM_REG_R5, A.UC_ARM_REG_R6, A.UC_ARM_REG_R7,
                A.UC_ARM_REG_R8, A.UC_ARM_REG_R9, A.UC_ARM_REG_R10, A.UC_ARM_REG_R11]
ARGS = [A.UC_ARM_REG_R0, A.UC_ARM_REG_R1, A.UC_ARM_REG_R2, A.UC_ARM_REG_R3]


def h32(*parts):
    d = hashlib.blake2b(repr(parts).encode(), digest_size=4).digest()
    return int.from_bytes(d, "little")


def page_bytes(seed, page):
    """Deterministic page content: words that look like heap pointers, small
    integers and noise, so pointer chains and flag tests both get exercised."""
    out = bytearray(PAGE)
    for i in range(0, PAGE, 4):
        r = h32(seed, page + i)
        kind = r % 10
        if kind < 4:
            v = POOL + ((r >> 2) & 0x0FFFFFFC)
        elif kind < 7:
            v = (r >> 8) & 7
        else:
            v = h32(seed, page + i, 1)
        struct.pack_into("<I", out, i, v)
    return bytes(out)


def trial_args(trial, kinds=None):
    """(seed, r0-r3) for trial number `trial`. With the parameters' kinds
    ('p' pointer, 'i' integer, from the C definition) pointers get separate
    blocks and integers vary over 0, 1 and small values; without them the
    trials mix the two."""
    seed = 1000 + trial
    ptr = [HEAP + (trial * 4 + k) * 0x10000 for k in range(4)]
    ints = [(0, 1, h32(seed, k) % 8, (h32(seed, k, 2) % 48 - 8) & 0xFFFFFFFF)[(trial + k) % 4] for k in range(4)]
    if kinds:
        kinds = (list(kinds) + ["i"] * 4)[:4]
        return seed, [ptr[k] if kinds[k] == "p" else ints[k] for k in range(4)]
    pattern = trial % 4
    if pattern == 0:
        regs = ptr
    elif pattern == 1:
        regs = [ptr[0]] + ints[1:]
    elif pattern == 2:
        regs = ptr[:2] + ints[2:]
    else:
        regs = ints
    return seed, regs


def param_kinds(text, func):
    """'p'/'i' for each parameter of func's definition in C text, or None."""
    m = re.search(r"\b%s\s*\(([^()]*(?:\([^()]*\)[^()]*)*)\)\s*\{" % re.escape(func), text)
    if not m:
        return None
    params = m.group(1).strip()
    if params in ("", "void"):
        return []
    out, depth, cur = [], 0, ""
    for ch in params + ",":
        if ch == "," and depth == 0:
            out.append("p" if "*" in cur or "[" in cur else "i")
            cur = ""
            continue
        depth += ch == "("
        depth -= ch == ")"
        cur += ch
    return out


_shared = {}


def map_rom(uc, image, overlay):
    """Map the ROM without copying it (one host buffer shared by every run),
    except the pages the candidate's code is written over."""
    key = id(image)
    if key not in _shared:
        _shared.clear()
        _shared[key] = (image, ctypes.create_string_buffer(bytes(image), len(image)))
    buf = _shared[key][1]
    size = (len(image) + PAGE - 1) & ~(PAGE - 1)
    rx = unicorn.UC_PROT_READ | unicorn.UC_PROT_EXEC
    lo = hi = ROM_BASE
    if overlay:
        addr, data = overlay
        lo = addr & ~(PAGE - 1)
        hi = (addr + len(data) + PAGE - 1) & ~(PAGE - 1)
    base = ctypes.addressof(buf)
    if lo > ROM_BASE:
        uc.mem_map_ptr(ROM_BASE, lo - ROM_BASE, rx, base)
    if hi > lo:
        uc.mem_map(lo, hi - lo, rx)
        uc.mem_write(lo, bytes(image[lo - ROM_BASE:hi - ROM_BASE]).ljust(hi - lo, b"\0"))
        uc.mem_write(overlay[0], overlay[1])
    if ROM_BASE + size > hi and len(image) > hi - ROM_BASE:
        tail = len(image) - (hi - ROM_BASE)
        uc.mem_map_ptr(hi, tail & ~(PAGE - 1), rx, base + hi - ROM_BASE)


class Run:
    """One execution of one function body."""

    def __init__(self, image, start, end, names, seed, regs, overlay=None, arity=None):
        self.start, self.end, self.names, self.seed = start, end, names, seed
        self.arity = arity or {}
        self.calls, self.writes, self.stackw, self.writer = [], {}, {}, {}
        self.reads = {}  # address -> pc of the first read (outside the stack and the function's own code)
        self.blocks = set()
        self.status = "ok"
        if os.environ.get("CODEX_SANDBOX") == "seatbelt":
            # Codex's seatbelt sandbox allows no JIT memory: unicorn dies with SIGILL (and macOS shows a
            # crash dialog) instead of raising, so refuse before touching it.
            raise RuntimeError("unicorn cannot run in Codex's seatbelt sandbox (no JIT memory)")
        uc = unicorn.Uc(unicorn.UC_ARCH_ARM, unicorn.UC_MODE_THUMB)
        self.uc = uc
        map_rom(uc, image, overlay)
        uc.mem_map(SENTINEL, PAGE, unicorn.UC_PROT_ALL)
        uc.mem_write(SENTINEL, b"\x70\x47" * (PAGE // 2))   # bx lr
        lo, hi = STACK_LO & ~(PAGE - 1), (STACK_TOP + PAGE - 1) & ~(PAGE - 1)
        uc.mem_map(lo, hi - lo, unicorn.UC_PROT_ALL)
        # Unicorn zero-fills the stack. Bytes sharing its edge pages are ordinary RAM.
        if lo < STACK_LO:
            uc.mem_write(lo, page_bytes(seed, lo)[:STACK_LO - lo])
        if STACK_TOP < hi:
            uc.mem_write(STACK_TOP, page_bytes(seed, hi - PAGE)[STACK_TOP - (hi - PAGE):])
        uc.hook_add(unicorn.UC_HOOK_MEM_UNMAPPED, self.on_unmapped)
        uc.hook_add(unicorn.UC_HOOK_MEM_WRITE, self.on_write)
        uc.hook_add(unicorn.UC_HOOK_MEM_READ, self.on_read)
        uc.hook_add(unicorn.UC_HOOK_BLOCK, self.on_block)
        for k, r in enumerate(CALLEE_SAVED):
            uc.reg_write(r, h32(seed, "saved", k))
        for r, v in zip(ARGS, regs):
            uc.reg_write(r, v)
        uc.reg_write(A.UC_ARM_REG_R12, h32(seed, "ip"))
        uc.reg_write(A.UC_ARM_REG_SP, STACK_TOP)
        uc.reg_write(A.UC_ARM_REG_LR, SENTINEL | 1)
        self.call_marks = [0]  # write-log length at each call (for stack args)
        self.log = []
        try:
            uc.emu_start(start | 1, SENTINEL, count=STEP_LIMIT)
            if uc.reg_read(A.UC_ARM_REG_PC) & ~1 != SENTINEL:
                self.status = "step limit"
        except unicorn.UcError as e:
            self.status = f"fault: {e}"
        self.r0 = uc.reg_read(A.UC_ARM_REG_R0)

    def value(self, v):
        """A register value as compared: pointers into the stack frame differ
        with the frame layout, so they compare by the word they point at."""
        if STACK_LO <= v < STACK_TOP:
            return ("stack", int.from_bytes(self.uc.mem_read(v & ~3, 4), "little"))
        return v

    def on_unmapped(self, uc, access, address, size, value, data):
        page = address & ~(PAGE - 1)
        try:  # any address: noise read as a pointer must not end the run
            uc.mem_map(page, PAGE, unicorn.UC_PROT_ALL)
        except unicorn.UcError:
            return False
        uc.mem_write(page, page_bytes(self.seed, page))
        return True

    def on_read(self, uc, access, address, size, value, data):
        if STACK_LO <= address < STACK_TOP or self.start <= address < self.end:
            return
        self.reads.setdefault(address & ~(size - 1) if size in (2, 4) else address, uc.reg_read(A.UC_ARM_REG_PC))

    def on_write(self, uc, access, address, size, value, data):
        v = value & ((1 << (8 * size)) - 1)
        if STACK_LO <= address < STACK_TOP:
            self.log.append((address, size, v))
            return
        pc = uc.reg_read(A.UC_ARM_REG_PC)
        for i in range(size):
            self.writes[address + i] = (v >> (8 * i)) & 0xFF
            self.writer[address + i] = pc

    def on_block(self, uc, address, size, data):
        a = address & ~1
        if self.start <= a < self.end:
            self.blocks.add(a)
            return
        if a == SENTINEL:
            return
        regs = [self.value(uc.reg_read(r)) for r in ARGS]
        name = self.names.get(a, f"0x{a:08X}")
        if name.startswith("_call_via_"):
            reg = int(name[len("_call_via_r"):]) if name[len("_call_via_r"):].isdigit() else None
            if reg is not None:
                t = uc.reg_read(getattr(A, f"UC_ARM_REG_R{reg}")) & ~1
                name = "via " + self.names.get(t, f"0x{t:08X}")
        sp = uc.reg_read(A.UC_ARM_REG_SP)
        stack = {}
        for addr, sz, v in self.log[self.call_marks[-1]:]:
            if sp <= addr < sp + 16:
                stack[addr - sp] = (sz, self.value(v) if sz == 4 else v)
        self.call_marks.append(len(self.log))
        digest = (dict(self.writes), dict(self.writer))
        n = sum(1 for c in self.calls if c[0] == name)
        self.calls.append((name, tuple(regs), tuple(sorted(stack.items())), digest))
        # the result depends only on what the callee reads
        ret = h32(self.seed, name, n, repr(regs[:min(4, self.arity.get(name.replace("via ", ""), 4))]))
        ret = (0, 1, ret, POOL + (ret & 0x0FFFFFFC))[ret % 4]
        uc.reg_write(A.UC_ARM_REG_R0, ret)
        for k, r in enumerate(ARGS[1:] + [A.UC_ARM_REG_R12]):
            uc.reg_write(r, h32(self.seed, name, n, "clobber", k))
        uc.reg_write(A.UC_ARM_REG_PC, uc.reg_read(A.UC_ARM_REG_LR) | 1)


def arities(text):
    """Parameter counts of the functions declared in preprocessed C, so a
    call compares only the argument registers its callee reads."""
    # libgcc helpers the compiler calls for `/` and `%`
    out = {n: 2 for n in ("__divsi3", "__udivsi3", "__modsi3", "__umodsi3")}
    for m in re.finditer(r"\b(\w+)\s*\(([^()]*(?:\([^()]*\)[^()]*)*)\)\s*(?:;|\{|__attribute__)", text):
        name, params = m.group(1), m.group(2).strip()
        if name in ("if", "while", "for", "switch", "return", "sizeof"):
            continue
        if params in ("", "void"):
            n = 0
        elif "..." in params:
            n = 4
        else:
            depth, n = 0, 1
            for ch in params:
                depth += ch == "("
                depth -= ch == ")"
                n += ch == "," and depth == 0
        out.setdefault(name, n)
    return out


def _byte(v, pc):
    return "unwritten" if v is None else f"0x{v:02X} (stored at 0x{pc:08X})"


def compare(rom, cand, check_r0, arity=None):
    """First disagreement between two runs, or None."""
    arity = arity or {}
    for k, (a, b) in enumerate(zip(rom.calls, cand.calls)):
        if a[0] != b[0]:
            return f"call {k + 1}: ROM calls {a[0]}, draft calls {b[0]}"
        n = min(4, arity.get(a[0].replace("via ", ""), 4))
        for i in range(n):
            if a[1][i] != b[1][i]:
                fmt = lambda v: f"&stack (holding 0x{v[1]:X})" if isinstance(v, tuple) else f"0x{v:08X}"
                return f"call {k + 1} ({a[0]}): r{i} is {fmt(a[1][i])} in ROM, {fmt(b[1][i])} in draft"
        sa, sb = dict(a[2]), dict(b[2])
        for off, v in (sa.items() if arity.get(a[0], 5) > 4 else ()):
            if sb.get(off) != v:
                return f"call {k + 1} ({a[0]}): stack argument sp+{off} differs"
        if a[3][0] != b[3][0]:
            wa, wb = a[3][0], b[3][0]
            addr = min(x for x in set(wa) | set(wb) if wa.get(x) != wb.get(x))
            return (f"call {k + 1} ({a[0]}): before the call, memory 0x{addr:08X} is "
                    f"{_byte(wa.get(addr), a[3][1].get(addr))} in ROM, {_byte(wb.get(addr), b[3][1].get(addr))} in draft")
    if len(rom.calls) != len(cand.calls):
        more = rom if len(rom.calls) > len(cand.calls) else cand
        shorter = cand if more is rom else rom
        if shorter.status != "step limit":
            who = "ROM" if more is rom else "draft"
            extra = more.calls[min(len(rom.calls), len(cand.calls))][0]
            return f"{who} makes {abs(len(rom.calls) - len(cand.calls))} more call(s), first {extra}"
    rom_limited = rom.status == "step limit"
    cand_limited = cand.status == "step limit"
    if rom_limited and cand_limited:
        return None  # only the shared call prefix is synchronized
    # A partial run can still prove it touched an address the other run never
    # touched. Values at shared addresses can change in the unobserved suffix.
    if rom_limited:
        comparable = rom.writes.keys() - cand.writes.keys()
    elif cand_limited:
        comparable = cand.writes.keys() - rom.writes.keys()
    else:
        comparable = rom.writes.keys() | cand.writes.keys()
    for addr in sorted(comparable):
        a, b = rom.writes.get(addr), cand.writes.get(addr)
        if a != b:
            return (f"memory 0x{addr:08X}: {_byte(a, rom.writer.get(addr))} in ROM, "
                    f"{_byte(b, cand.writer.get(addr))} in draft")
    extra = sorted(set(cand.reads) - set(rom.reads))
    if extra and not rom_limited:
        return (f"the draft reads 0x{extra[0]:08X} (at 0x{cand.reads[extra[0]]:08X}), which the ROM does not"
                + (f" ({len(extra)} such addresses)" if len(extra) > 1 else ""))
    missing = sorted(set(rom.reads) - set(cand.reads))
    if missing and not cand_limited:
        return (f"the ROM reads 0x{missing[0]:08X} (at 0x{rom.reads[missing[0]]:08X}), which the draft does not"
                + (f" ({len(missing)} such addresses)" if len(missing) > 1 else ""))
    if check_r0 and not (rom_limited or cand_limited) and rom.r0 != cand.r0:
        return f"return value 0x{rom.r0:08X} in ROM, 0x{cand.r0:08X} in draft"
    return None


def returns_value(src_text, func):
    m = re.search(r"^([A-Za-z_][\w \*]*?)\b%s\s*\(" % re.escape(func), src_text, re.M)
    return bool(m) and not re.match(r"\s*(static\s+)?(inline\s+)?void\s*$", m.group(1))


INCLUDE_ASM = re.compile(r'INCLUDE_ASM\(\s*"[^"]*"\s*,\s*(\w+)\s*\)')


def enable_drafts(raw, funcs):
    """The source with only the NONMATCHING blocks whose fallback covers one
    of funcs switched on, so a unit's other (possibly broken) drafts stay
    in assembly."""
    lines = raw.split("\n")
    out = list(lines)
    i = 0
    while i < len(lines):
        if re.match(r"^#\s*ifdef\s+NONMATCHING\b", lines[i].strip()):
            depth, j, fallback, in_fb = 0, i + 1, [], False
            while j < len(lines):
                t = lines[j].strip()
                if re.match(r"^#\s*if", t):
                    depth += 1
                elif re.match(r"^#\s*endif", t):
                    if depth == 0:
                        break
                    depth -= 1
                elif depth == 0 and re.match(r"^#\s*else", t):
                    in_fb = True
                elif in_fb:
                    fallback.append(lines[j])
                j += 1
            names = INCLUDE_ASM.findall("\n".join(fallback))
            out[i] = "#if 1" if set(names) & set(funcs) else "#if 0"
            i = j
        i += 1
    return "\n".join(out)


def compile_unit(src, drafts=()):
    """Compile src (with the given drafts switched on) and link it as in the ROM."""
    parent = os.path.join(ROOT, "build", "semcheck")
    os.makedirs(parent, exist_ok=True)
    work = os.path.relpath(tempfile.mkdtemp(prefix="run-", dir=parent), ROOT)
    base = os.path.join(work, os.path.basename(src)[:-2])
    if drafts:
        tmp = base + ".c"
        with open(os.path.join(ROOT, tmp), "w") as f:
            f.write(enable_drafts(open(os.path.join(ROOT, src)).read(), drafts))
        src = tmp
    compile_unit.last_src = src
    build.build_c(src, base + ".o")
    addr, defined = check.link_object(base + ".o", base + ".elf")
    build.sh(["arm-none-eabi-objcopy", "-O", "binary", "-j", ".text", base + ".elf", base + ".bin"])
    data = open(os.path.join(ROOT, base + ".bin"), "rb").read()
    syms = []
    for ln in build.sh(["arm-none-eabi-nm", base + ".elf"]).splitlines():
        p = ln.split()
        if len(p) == 3 and p[1] in "Tt" and not p[2].startswith("."):
            syms.append((int(p[0], 16) & ~1, p[2]))
    return addr, data, sorted(syms), defined


def symbol_names():
    names = {}
    for n, (v, t) in check.elf_symbols().items():
        if t == "FUNC" or n.startswith(("sub_", "_call_via")):
            names.setdefault(v & ~1, n)
    return names


class Oracle:
    """The ROM function's behaviour on the trial inputs, computed once, to
    test any number of candidates against it (a search's guard)."""

    def __init__(self, func, trials=8, check_r0=False, rom=None, names=None, arity=None, kinds=None):
        self.arity = arity or {}
        self.rom = rom if rom is not None else load_rom(verify=False)
        by_label, rom_addr, sizes = check.inventory()
        key = by_label.get(func, func)
        if key not in rom_addr:
            raise KeyError(f"{func}: not in the function inventory")
        self.func, self.check_r0 = func, check_r0
        self.start = rom_addr[key]
        self.end = self.start + sizes[key]
        self.names = names if names is not None else symbol_names()
        self.runs = [(trial_args(t, kinds), Run(self.rom, self.start, self.end, self.names,
                                                *trial_args(t, kinds), arity=self.arity))
                     for t in range(trials)]
        covered = set().union(*(r.blocks for _, r in self.runs)) if self.runs else set()
        blocks = rom_blocks(self.rom, self.start, self.end, func)
        self.coverage = 100.0 * len(covered & blocks) / max(1, len(blocks))

    def check(self, data, addr=None, cstart=None, cend=None, names=None, verbose=False):
        """(verdict, detail) for candidate code `data` linked at `addr`
        (default: the function's ROM address, as rasolve's resolver links it)."""
        addr = self.start if addr is None else addr
        cstart = addr if cstart is None else cstart
        cend = addr + len(data) if cend is None else cend
        verdict, detail = "EQUIVALENT", None
        for (seed, regs), r in self.runs:
            c = Run(self.rom, cstart, cend, names or self.names, seed, regs, overlay=(addr, data),
                    arity=self.arity)
            diff = compare(r, c, self.check_r0 and r.status == "ok", self.arity)
            if r.status != c.status and "step limit" not in (r.status, c.status):
                diff = (diff + "; " if diff else "") + f"ROM run: {r.status}, draft run: {c.status}"
            if verbose:
                print(f"  trial {seed - 1000}: {len(r.calls)} calls, {len(r.writes)} bytes written, "
                      f"ROM {r.status}, draft {c.status}" + (f" -> {diff}" if diff else ""))
            if diff:
                args = ", ".join(f"0x{v:X}" for v in regs)
                return "DIFFERENT", f"trial {seed - 1000} (args {args}): {diff}"
            if r.status != "ok" or c.status != "ok":
                verdict = "INCOMPLETE"
                if detail is None:
                    detail = f"trial {seed - 1000}: ROM run: {r.status}, draft run: {c.status}"
        return verdict, detail


def semcheck(src, funcs, nonmatching, trials, verbose):
    rom = load_rom(verify=False)
    names = symbol_names()
    addr, data, syms, defined = compile_unit(src, (funcs or ()) if nonmatching else ())
    text = open(os.path.join(ROOT, src)).read()
    pre = build.preprocess(src if not nonmatching else compile_unit.last_src)
    arity = arities(pre)
    cand_names = dict(names)
    for a, n in syms:
        if not n.endswith("__addr"):
            cand_names[a] = n
    results = {}
    for func in funcs or defined:
        here = [a for a, n in syms if n == func]
        if not here:
            print(f"{func}: not defined by this build (use --nonmatching for a draft)")
            continue
        try:
            oracle = Oracle(func, trials, returns_value(text, func), rom, names, arity, param_kinds(pre, func))
        except KeyError as e:
            print(f"{e}, skipped")
            continue
        cstart = here[0]
        # the next function, or the end of the unit's code (linker symbols such as
        # __bss_end__ sit past it, and the ROM's next functions are callees)
        cend = min([a for a, _ in syms if a > cstart] + [addr + len(data)])
        verdict, detail = oracle.check(data, addr, cstart, cend, cand_names, verbose)
        cov = oracle.coverage
        print(f"{func}: {verdict}" + (f": {detail}" if detail else "") + f" (ROM blocks reached {cov:.0f}%)")
        results[func] = (verdict, detail, cov)
    return results


def rom_blocks(rom, start, end, func=None):
    """Thumb block starts, excluding annotated literal pools and alignment."""
    import capstone
    from capstone.arm_const import ARM_INS_B, ARM_INS_BL, ARM_INS_BLX
    md = capstone.Cs(capstone.CS_ARCH_ARM, capstone.CS_MODE_THUMB)
    code = rom[start - ROM_BASE:end - ROM_BASE]
    layout = rom_data_ranges(func, start) if func else None
    data = sorted(layout[0]) if layout else []
    starts, decoded = {start}, set()
    offset = 0
    for lo, hi in data + [(len(code), len(code))]:
        for ins in md.disasm(code[offset:lo], start + offset):
            decoded.add(ins.address)
            if ins.id in (ARM_INS_BL, ARM_INS_BLX):
                starts.add(ins.address + ins.size)
            elif ins.id == ARM_INS_B:
                # Only conditional branches have a fall-through edge.
                if ins.mnemonic.split(".")[0] != "b":
                    starts.add(ins.address + ins.size)
                starts.add(int(ins.op_str[1:], 0))
        offset = max(offset, hi)
    return starts & decoded


def audit(out, trials):
    """Every NONMATCHING draft, one compile per file; a TSV of verdicts."""
    import glob
    from split import nonmatching_functions
    rows = []
    files = sorted(glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True))
    for path in files:
        drafts = [n for n, _ in nonmatching_functions(path)]
        if not drafts:
            continue
        src = os.path.relpath(path, ROOT)
        try:
            res = semcheck(src, drafts, True, trials, False)
        except SystemExit as e:
            print(f"{src}: build failed ({e})")
            res = {n: ("BUILD FAILED", str(e), 0.0) for n in drafts}
        except Exception as e:  # keep auditing the rest
            print(f"{src}: error {e!r}")
            res = {n: ("ERROR", repr(e), 0.0) for n in drafts}
        for n, (v, d, cov) in res.items():
            rows.append((src, n, v, f"{cov:.0f}", d or ""))
    with open(out, "w") as f:
        f.write("src\tfunction\tverdict\tcoverage\tdetail\n")
        for r in rows:
            f.write("\t".join(r) + "\n")
    from collections import Counter
    print(dict(Counter(r[2] for r in rows)), "->", out)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("src", nargs="?")
    ap.add_argument("--all", action="store_true", help="audit every NONMATCHING draft")
    ap.add_argument("--out", default=os.path.join(ROOT, "build", "semcheck", "audit.tsv"))
    ap.add_argument("funcs", nargs="*")
    ap.add_argument("--nonmatching", action="store_true", help="compile the #ifdef NONMATCHING drafts")
    ap.add_argument("--trials", type=int, default=8)
    ap.add_argument("-v", "--verbose", action="store_true")
    a = ap.parse_args()
    if a.all:
        os.makedirs(os.path.dirname(a.out), exist_ok=True)
        audit(a.out, a.trials)
        return
    if not a.src:
        ap.error("give SRC or --all")
    src = a.src
    if not src.endswith(".c"):
        src = os.path.join("src", "fn", src + ".c")
    src = os.path.relpath(os.path.join(ROOT, src), ROOT)
    res = semcheck(src, a.funcs, a.nonmatching, a.trials, a.verbose)
    sys.exit(1 if any(v[0] == "DIFFERENT" for v in res.values()) else 0)


if __name__ == "__main__":
    main()
