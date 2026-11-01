"""Milliseconds per compile: scope's agbcc fork server and object bytes without ld.

The instrumented agbcc (tools/scope/agbcc.patch, AGBCC_SERVER) parses a unit's
declarations once, stops at a marker byte and forks a child for each request; the
child parses only the rest of the unit (from FUNC's definition on) with that
request's knobs set. `as` assembles the result and Resolver places FUNC's bytes at
its ROM address from the unlinked object (Thumb BL and absolute words), so no ld,
objcopy, readelf or objdump runs. About 3 ms of compile and 5 ms of `as` per
variant, against ~80 ms for the one-shot path with its ten subprocesses.

The children share the parent's dump files, so this path gives bytes only: RTL
dumps, -dp uids and DWARF lines come from scope.py's full compile.

Adapted from tools/agbserve.py (another thread's tool) so scope owns its copy.
"""
import hashlib
import os
import re
import struct
import subprocess
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
MARK = "\x01"  # right after the prefix's last newline: no extra line


HELPER_CHARS = 12000  # longest unit-local stretch before FUNC sent with every request


def split_source(text, func_names):
    """(prefix, suffix): the suffix starts at the definition of one of FUNC_NAMES.
    None when the text has no such definition or contains the marker byte."""
    if MARK in text:
        return None
    m = None
    ends = []
    for name in func_names:
        # a definition's head has no braces, parentheses or operators before the name: this keeps
        # `} else if (!FUNC(a, b)) {` (a call in a later function) from counting as one
        for m2 in re.finditer(r"^[^\n;#{}()=!&|]*\b" + re.escape(name) + r"\s*\([^;{]*\)\s*\{", text, re.M):
            if m is None:
                m = m2
            ends.append(body_end(text, m2.end() - 1))
    if not m:
        return None
    # start at the line holding the definition, so line numbers stay the same
    start = text.rfind("\n", 0, m.start()) + 1
    # Cut instead where the last #include ends (cpp's `# N "file" 2` return marker) when what follows is
    # small: the unit's own structs and inline helpers then go with each request, so a variant that edits
    # a helper reuses the server instead of parsing every header again. A long stretch (earlier
    # functions of a big unit) stays in the prefix, where it is parsed and compiled once.
    back = None
    for b in re.finditer(r'^# \d+ "[^"]*" 2[^\n]*\n', text[:start], re.M):
        back = b
    if back and start - back.end() <= HELPER_CHARS:
        start = back.end()
    # and end the suffix with FUNC's body: agbcc compiles a function at a time, so what follows cannot
    # change FUNC's code, and a big unit's later functions would otherwise compile with every variant
    end = max(ends)
    return text[:start], text[start:end]


def body_end(text, brace):
    """Index just past the } closing the { at BRACE, skipping strings, chars and comments."""
    depth, i, n = 0, brace, len(text)
    while i < n:
        c = text[i]
        if c in "\"'":
            j = i + 1
            while j < n and text[j] != c:
                j += 2 if text[j] == "\\" else 1
            i = j + 1
            continue
        if text.startswith("/*", i):
            i = text.find("*/", i + 2) + 2 or n
            continue
        if text.startswith("//", i):
            i = text.find("\n", i) + 1 or n
            continue
        if c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                return i + 1
        i += 1
    return n


def canonical_prefix(prefix):
    """PREFIX with the main file's name in cpp's line markers replaced by a fixed one, so drafts of one
    unit saved under different paths share a server (the fork server returns bytes only; the name is
    only debug information)."""
    m = re.match(r'# 1 "([^"]+)"', prefix)
    if not m:
        return prefix
    return re.sub(r'^(# \d+ )"%s"' % re.escape(m.group(1)), r'\1"unit.c"', prefix, flags=re.M)


class Server:
    """One agbcc that has parsed PREFIX and forks a child per compile."""

    def __init__(self, compiler, flags, prefix, workdir):
        self.dir = tempfile.mkdtemp(prefix="srv-", dir=workdir)
        pre_path = os.path.join(self.dir, "prefix.c")
        open(pre_path, "w").write(prefix + MARK)
        self.prefix_s = os.path.join(self.dir, "prefix.s")
        self.stderr_path = os.path.join(self.dir, "stderr.txt")
        self.last_error = ""
        req_r, self.req_w = os.pipe()
        self.rep_r, rep_w = os.pipe()
        env = dict(os.environ, AGBCC_SERVER=f"{req_r}:{rep_w}")
        for k in list(env):
            if k.startswith(("AGBCC_", "CSE_")) and k != "AGBCC_SERVER":
                del env[k]  # knobs go with each request, never with the prefix
        with open(self.stderr_path, "w") as stderr:
            self.proc = subprocess.Popen([compiler, *flags, "-o", self.prefix_s, pre_path],
                                         env=env, pass_fds=(req_r, rep_w),
                                         stdout=subprocess.DEVNULL, stderr=stderr)
        os.close(req_r)
        os.close(rep_w)
        self.reply = os.fdopen(self.rep_r)
        self.n = 0
        self._prefix_asm = None

    def prefix_asm(self):
        if self._prefix_asm is None:
            self._prefix_asm = open(self.prefix_s).read()
        return self._prefix_asm

    def compile(self, suffix, env=None):
        """Assembly for prefix + SUFFIX, or None on a compile error."""
        self.n += 1
        src = os.path.join(self.dir, f"s{self.n & 7}.c")
        out = os.path.join(self.dir, f"s{self.n & 7}.s")
        open(src, "w").write(suffix)
        env = {k: str(v) for k, v in (env or {}).items() if str(v)}
        if any(" " in v for v in env.values()):
            raise ValueError("knob values may not contain spaces")
        os.write(self.req_w, (f"{src} {out} " + " ".join(f"{k}={v}" for k, v in env.items()) + "\n").encode())
        status = self.reply.readline()
        if not status:
            raise RuntimeError("agbcc server exited")
        if int(status) != 0:
            with open(self.stderr_path) as stderr:
                self.last_error = "\n".join(stderr.read().splitlines()[-12:])
            return None
        return self.prefix_asm() + open(out).read()

    def close(self):
        import shutil
        try:
            os.close(self.req_w)
        except OSError:
            pass
        self.proc.wait()
        shutil.rmtree(self.dir, ignore_errors=True)


def elf_sections(data):
    shoff, = struct.unpack_from("<I", data, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", data, 0x2E)
    secs = []
    for i in range(shnum):
        name, typ, flags, addr, off, size, link, info, align, entsize = \
            struct.unpack_from("<IIIIIIIIII", data, shoff + i * shentsize)
        secs.append(dict(name=name, type=typ, off=off, size=size, link=link, info=info))
    strtab = secs[shstrndx]
    for s in secs:
        end = data.index(b"\0", strtab["off"] + s["name"])
        s["name"] = data[strtab["off"] + s["name"]:end].decode()
    return secs


def elf_symbols(obj, secs):
    """[(name, value, shndx, type)] of an object's symbol table."""
    symtab = next(s for s in secs if s["type"] == 2)
    strtab = secs[symtab["link"]]
    syms = []
    for i in range(symtab["size"] // 16):
        name, value, size, info, other, shndx = struct.unpack_from("<IIIBBH", obj, symtab["off"] + i * 16)
        end = obj.index(b"\0", strtab["off"] + name)
        syms.append((obj[strtab["off"] + name:end].decode(), value, shndx, info & 0xF))
    return syms


def place(obj, own, rom_syms, address):
    """Bytes of the function named in OWN (its names) from object OBJ, placed at ADDRESS.
    Calls to other functions of the unit go to their ROM addresses (where the ROM has them),
    so drafts of other sizes before or after FUNC do not move FUNC's call targets.
    Returns (code, start offset in .text, the unit's function starts, unresolved names)."""
    secs = elf_sections(obj)
    text_i = next(i for i, s in enumerate(secs) if s["name"] == ".text")
    sec = secs[text_i]
    text = bytearray(obj[sec["off"]:sec["off"] + sec["size"]])
    syms = elf_symbols(obj, secs)
    funcs = sorted({(v & ~1, n) for n, v, sh, t in syms if sh == text_i and t == 2})
    start = next((v for v, n in funcs if n in own), None)
    if start is None:
        return None, None, funcs, ["no " + "/".join(own)]
    end = next((v for v, n in funcs if v > start and n not in own), len(text))
    base = address - start  # address of .text offset 0
    local = {n: base + (v & ~1) + (v & 1) for n, v, sh, t in syms if sh == text_i and n}
    local_funcs = {n for _, n in funcs} - set(own)
    absolute = {n: v for n, v, sh, t in syms if sh == 0xFFF1 and n}
    missing = []

    def value_of(name):
        if name in local_funcs and name in rom_syms:
            return rom_syms[name][0]
        if name in local:
            return local[name]
        if name in absolute:
            return absolute[name]
        if name in rom_syms:
            return rom_syms[name][0]
        if name.endswith("__addr") and name[:-6] in rom_syms:
            return rom_syms[name[:-6]][0] & ~1
        m = re.search(r"_([0-9A-Fa-f]{8})$", name)
        if m:
            return int(m.group(1), 16)
        missing.append(name)
        return 0

    for s in secs:
        if s["type"] != 9 or s["info"] != text_i:  # SHT_REL for .text
            continue
        for i in range(s["size"] // 8):
            off, info = struct.unpack_from("<II", obj, s["off"] + i * 8)
            sym_i, typ = info >> 8, info & 0xFF
            name, value, shndx, _ = syms[sym_i]
            target = base + value if not name and shndx == text_i else value_of(name)
            at = base + off
            if typ == 2:  # R_ARM_ABS32
                addend, = struct.unpack_from("<I", text, off)
                struct.pack_into("<I", text, off, (target + addend) & 0xFFFFFFFF)
            elif typ == 10:  # R_ARM_THM_CALL (BL pair)
                hi, lo = struct.unpack_from("<HH", text, off)
                addend = ((hi & 0x7FF) << 12) | ((lo & 0x7FF) << 1)
                if addend & 0x400000:
                    addend -= 0x800000
                rel = (((target & ~1) + addend - at) >> 1) & 0x3FFFFF
                struct.pack_into("<HH", text, off, 0xF000 | (rel >> 11), 0xF800 | (rel & 0x7FF))
            elif typ in (102, 103):  # R_ARM_THM_PC11 (b) / R_ARM_THM_PC9 (b<cond>): offset in the insn
                bits = 11 if typ == 102 else 8
                h, = struct.unpack_from("<H", text, off)
                field = h & ((1 << bits) - 1)
                addend = (field - (1 << bits) if field >> (bits - 1) else field) << 1
                rel = (((target & ~1) + addend - at) >> 1) & ((1 << bits) - 1)
                struct.pack_into("<H", text, off, (h & ~((1 << bits) - 1)) | rel)
            else:
                missing.append(f"reloc type {typ}")
    marks = sorted((v, n) for n, v, sh, t in syms if sh == text_i and n in ("$d", "$t"))
    data, cur, at = [], "$t", start
    for v, k in marks + [(end, "$end")]:
        if v <= start:
            cur = k
            continue
        v = min(v, end)
        if cur == "$d" and v > at:
            data.append((at - start, v - start))
        at, cur = v, k
        if v >= end:
            break
    place.data = data  # FUNC's literal-pool ranges, for decoding
    return bytes(text[start:end]), start, funcs, missing


def assemble(assembler, asm, workdir, tag=None):
    """Assemble ASM in a private directory, including when callers share a tag or PID."""
    asm = re.sub(r"^(\w+):\n", lambda m: f"{m[1]}:\n\t.global {m[1]}__addr\n{m[1]}__addr:\n"
                 if not m[1].startswith(".") else m[0], asm, flags=re.M)
    with tempfile.TemporaryDirectory(prefix=(tag or "as") + "-", dir=workdir) as staging:
        s_path = os.path.join(staging, "unit.s")
        o_path = os.path.join(staging, "unit.o")
        with open(s_path, "w") as source:
            source.write(asm + "\t.text\n\t.align\t2, 0\n")
        result = subprocess.run(assembler + ["-o", o_path, s_path], capture_output=True, text=True)
        if result.returncode:
            return None, result.stderr
        with open(o_path, "rb") as obj:
            return obj.read(), ""


class Fast:
    """Fork servers by unit prefix, kept for the life of the process (a resident scope keeps them
    across commands). bytes(pre, own, env) -> (code or None, error text)."""

    def __init__(self, compiler, flags, assembler, rom_syms, address, workdir, fixup=None):
        self.compiler, self.flags, self.assembler = compiler, flags, assembler
        self.rom_syms, self.address, self.workdir = rom_syms, address, workdir
        self.fixup = fixup or (lambda text: text)
        self.servers = {}

    def server(self, prefix):
        stamp = os.stat(self.compiler).st_mtime_ns
        if getattr(self, "stamp", stamp) != stamp:  # the compiler was rebuilt: never answer with the old one
            self.close()
        self.stamp = stamp
        key = hashlib.sha1(prefix.encode()).hexdigest()
        s = self.servers.get(key)
        if s is None or s.proc.poll() is not None:
            if len(self.servers) >= 8:
                for old in self.servers.values():
                    old.close()
                self.servers.clear()
            s = self.servers[key] = Server(self.compiler, self.flags, prefix, self.workdir)
        return s

    def bytes(self, pre, own, env=None):
        parts = split_source(pre, own)
        if parts is None:
            return None, "the fork server needs FUNC's definition in the unit"
        server = self.server(canonical_prefix(parts[0]))
        asm = server.compile(parts[1], env)
        if asm is None:
            return None, server.last_error or "compile failed"
        obj, err = assemble(self.assembler, self.fixup(asm), self.workdir)
        if obj is None:
            return None, "as failed: " + err[:300]
        code, _, _, missing = place(obj, own, self.rom_syms, self.address)
        if code is None or missing:
            return None, "unresolved: " + ", ".join(missing[:5])
        self.data = place.data
        return code, ""

    def close(self):
        for s in self.servers.values():
            s.close()
        self.servers.clear()


def _uleb(b, i):
    v = shift = 0
    while True:
        x = b[i]
        i += 1
        v |= (x & 0x7F) << shift
        shift += 7
        if not x & 0x80:
            return v, i


def _sleb(b, i):
    v = shift = 0
    while True:
        x = b[i]
        i += 1
        v |= (x & 0x7F) << shift
        shift += 7
        if not x & 0x80:
            if x & 0x40:
                v -= 1 << shift
            return v, i


def dwarf_lines(obj):
    """{.text offset: (file name, line)} from the object's DWARF 2 line program (what
    `objdump --dwarf=decodedline` prints; a later row at the same address wins)."""
    secs = elf_sections(obj)
    li = next((i for i, s in enumerate(secs) if s["name"] == ".debug_line"), None)
    if li is None:
        return {}
    sec = secs[li]
    b = bytearray(obj[sec["off"]:sec["off"] + sec["size"]])
    syms = elf_symbols(obj, secs)
    for s in secs:  # apply .rel.debug_line (DW_LNE_set_address operands)
        if s["type"] == 9 and s["info"] == li:
            for k in range(s["size"] // 8):
                off, info = struct.unpack_from("<II", obj, s["off"] + k * 8)
                if info & 0xFF == 2:
                    addend, = struct.unpack_from("<I", b, off)
                    struct.pack_into("<I", b, off, (syms[info >> 8][1] + addend) & 0xFFFFFFFF)
    rows = {}
    i = 0
    while i + 4 <= len(b):
        unit_len, = struct.unpack_from("<I", b, i)
        end = i + 4 + unit_len
        hdr_len, = struct.unpack_from("<I", b, i + 6)
        prog = i + 10 + hdr_len
        min_inst, _, line_base, line_range, op_base = struct.unpack_from("<BBbBB", b, i + 10)
        lens = b[i + 15:i + 15 + op_base - 1]
        j = i + 15 + op_base - 1
        while b[j]:  # include directories
            j = b.index(0, j) + 1
        j += 1
        files = []
        while b[j]:
            z = b.index(0, j)
            files.append(b[j:z].decode())
            j = z + 1
            for _ in range(3):
                _, j = _uleb(b, j)
        j = prog
        addr, fileno, line = 0, 1, 1

        def emit():
            rows[addr] = (files[fileno - 1] if 0 < fileno <= len(files) else "?", line)
        while j < end:
            op = b[j]
            j += 1
            if op >= op_base:
                adj = op - op_base
                addr += (adj // line_range) * min_inst
                line += line_base + adj % line_range
                emit()
            elif op == 0:
                n, j = _uleb(b, j)
                sub = b[j]
                if sub == 1:
                    addr, fileno, line = 0, 1, 1
                elif sub == 2:
                    addr, = struct.unpack_from("<I", b, j + 1)
                j += n
            elif op == 1:
                emit()
            elif op == 2:
                n, j = _uleb(b, j)
                addr += n * min_inst
            elif op == 3:
                n, j = _sleb(b, j)
                line += n
            elif op == 4:
                fileno, j = _uleb(b, j)
            elif op == 5:
                _, j = _uleb(b, j)
            elif op in (6, 7):
                pass
            elif op == 8:
                addr += ((255 - op_base) // line_range) * min_inst
            elif op == 9:
                n, = struct.unpack_from("<H", b, j)
                j += 2
                addr += n
            else:
                for _ in range(lens[op - 1]):
                    _, j = _uleb(b, j)
        i = end
    return rows
