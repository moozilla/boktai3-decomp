#!/usr/bin/env python3
"""Compile many variants of one function fast: a fork server inside agbcc,
and scoring from the object file without ld or objdump.

A candidate source is the same declarations (types, prototypes, inline
helpers) followed by one function. The instrumented agbcc
(tools/agbcc-instr.sh) parses the declarations once, stops at a marker byte
and forks a child per candidate that parses only the function. The child's
assembly is the candidate's, after the prefix's. `as` makes the object;
the function's bytes are then resolved here (Thumb BL and absolute words)
against the ROM's symbol table, so no link or disassembly runs.

  agbserve.py --selftest FILE.c [FUNC]   compare with plain agbcc and time it

Library use:
    with Server(prefix_text) as s:
        asm = s.compile(function_text, env={"AGBCC_PRI": "28:5000"})
    code = Resolver(symbols, address).bytes_of(asm, func)
"""
import argparse
import os
import re
import struct
import subprocess
import sys
import tempfile
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import build  # noqa: E402
from romlib import ROOT  # noqa: E402

INSTR = os.path.join(ROOT, "build", "agbcc-instr", "gcc", "agbcc")
MARK = "\x01"  # right after the prefix's last newline: no extra line


def split_source(text, func):
    """(prefix, suffix): suffix starts at FUNC's definition. None if FUNC is
    not the last definition or the text contains the marker byte."""
    if "\x01" in text:
        return None
    m = None
    for m in re.finditer(r"^[^\n;#]*\b" + re.escape(func) + r"\s*\([^;{]*\)\s*\{", text, re.M):
        pass
    if not m:
        return None
    return text[:m.start()], text[m.start():]


class Server:
    def __init__(self, prefix, flags=None, compiler=INSTR, workdir=None):
        self.flags = flags or build.CFLAGS
        self.dir = tempfile.mkdtemp(prefix="agbserve-", dir=workdir or os.path.join(ROOT, "build"))
        self.prefix = prefix
        pre_path = os.path.join(self.dir, "prefix.c")
        open(pre_path, "w").write(prefix + MARK)
        self.prefix_s = os.path.join(self.dir, "prefix.s")
        req_r, self.req_w = os.pipe()
        self.rep_r, rep_w = os.pipe()
        env = dict(os.environ, AGBCC_SERVER=f"{req_r}:{rep_w}")
        self.proc = subprocess.Popen([compiler, *self.flags, "-o", self.prefix_s, pre_path],
                                     env=env, pass_fds=(req_r, rep_w),
                                     stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
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
        """Full assembly text for prefix + SUFFIX, or None on a compile error."""
        self.n += 1
        src = os.path.join(self.dir, f"s{self.n & 7}.c")
        out = os.path.join(self.dir, f"s{self.n & 7}.s")
        open(src, "w").write(suffix)
        extra = " ".join(f"{k}={v}" for k, v in (env or {}).items() if v)
        if " " in "".join((env or {}).values()):
            raise ValueError("server environment values may not contain spaces")
        os.write(self.req_w, f"{src} {out} {extra}\n".encode())
        status = self.reply.readline()
        if not status:
            raise RuntimeError("agbcc server exited")
        if int(status) != 0:
            return None
        return self.prefix_asm() + open(out).read()

    def close(self):
        try:
            os.close(self.req_w)
        except OSError:
            pass
        self.proc.wait()

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.close()


def compile_plain(text, flags=None, compiler=None, env=None):
    r = subprocess.run([compiler or os.path.join(ROOT, build.AGBCC), *(flags or build.CFLAGS), "-o", "-"],
                       input=text, text=True, capture_output=True, env=env)
    return r.stdout if r.returncode == 0 else None


# --- object bytes without ld ------------------------------------------------

def elf_sections(data):
    shoff, = struct.unpack_from("<I", data, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", data, 0x2E)
    secs = []
    for i in range(shnum):
        name, typ, flags, addr, off, size, link, info, align, entsize = \
            struct.unpack_from("<IIIIIIIIII", data, shoff + i * shentsize)
        secs.append(dict(name=name, type=typ, off=off, size=size, link=link, info=info, entsize=entsize))
    strtab = secs[shstrndx]
    for s in secs:
        end = data.index(b"\0", strtab["off"] + s["name"])
        s["name"] = data[strtab["off"] + s["name"]:end].decode()
    return secs


class Resolver:
    """Bytes of a function from an unlinked object, as the linker would place
    them at ADDRESS (the function's ROM address)."""

    def __init__(self, symbols, address, assembler=None):
        self.symbols = symbols
        self.address = address
        self.assembler = assembler or build.AS

    def assemble(self, asm, workdir):
        s_path, o_path = os.path.join(workdir, "r.s"), os.path.join(workdir, "r.o")
        # the same even-address aliases build.py adds
        asm = re.sub(r"^(\w+):\n", lambda m: f"{m[1]}:\n\t.global {m[1]}__addr\n{m[1]}__addr:\n"
                     if not m[1].startswith(".") else m[0], asm, flags=re.M)
        open(s_path, "w").write(asm + "\t.text\n\t.align 2, 0\n")
        r = subprocess.run(self.assembler + ["-o", o_path, s_path], capture_output=True)
        return open(o_path, "rb").read() if r.returncode == 0 else None

    def bytes_of(self, obj, func):
        """(code bytes of FUNC through the end of .text, unresolved names)."""
        secs = elf_sections(obj)
        text_i = next(i for i, s in enumerate(secs) if s["name"] == ".text")
        text = bytearray(obj[secs[text_i]["off"]:secs[text_i]["off"] + secs[text_i]["size"]])
        symtab = next(s for s in secs if s["type"] == 2)
        strtab = secs[symtab["link"]]
        syms = []
        for i in range(symtab["size"] // 16):
            name, value, size, info, other, shndx = struct.unpack_from("<IIIBBH", obj, symtab["off"] + i * 16)
            end = obj.index(b"\0", strtab["off"] + name)
            syms.append((obj[strtab["off"] + name:end].decode(), value, shndx))
        start = next((v & ~1 for n, v, sh in syms if n == func and sh == text_i), None)
        if start is None:
            return None, ["no " + func]
        base = self.address - start  # address of .text offset 0
        local = {n: base + (v & ~1) + (v & 1) for n, v, sh in syms if sh == text_i and n}
        missing = []

        def value_of(name):
            if name in local:
                return local[name]
            if name in self.symbols:
                v = self.symbols[name][0]
                return v
            if name.endswith("__addr") and name[:-6] in self.symbols:
                return self.symbols[name[:-6]][0] & ~1
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
                name, value, shndx = syms[sym_i]
                if not name and shndx == text_i:
                    target = base + value
                else:
                    target = value_of(name)
                place = base + off
                if typ == 2:  # R_ARM_ABS32
                    addend, = struct.unpack_from("<I", text, off)
                    struct.pack_into("<I", text, off, (target + addend) & 0xFFFFFFFF)
                elif typ == 10:  # R_ARM_THM_CALL (BL pair)
                    hi, lo = struct.unpack_from("<HH", text, off)
                    addend = (((hi & 0x7FF) << 12) | ((lo & 0x7FF) << 1))
                    if addend & 0x400000:
                        addend -= 0x800000
                    rel = (target & ~1) + addend - place
                    rel = (rel >> 1) & 0x3FFFFF
                    struct.pack_into("<HH", text, off, 0xF000 | (rel >> 11), 0xF800 | (rel & 0x7FF))
                else:
                    missing.append(f"reloc type {typ}")
        return bytes(text[start:]), missing


def selftest(src, func, n_variants=40):
    import contextlib
    import io
    import random
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    import check
    import permute
    sys.path.insert(0, permute.PERMUTER)
    from src import ast_util
    from src.randomizer import Randomizer
    from src.helpers import get_default_randomization_weights
    pre, _ = permute.prepare_source(os.path.relpath(src, ROOT), func)
    # Candidates as the permuter prints them: one generated text per variant.
    ast = ast_util.parse_c(pre)
    texts = [ast_util.to_c(ast)]
    weights = get_default_randomization_weights("gcc")
    random.seed(1)
    import copy
    while len(texts) < n_variants:
        a2 = copy.deepcopy(ast)
        fn, _ = ast_util.extract_fn(a2, func)
        ast_util.normalize_ast(fn, a2)
        try:
            Randomizer(weights, random.randrange(1 << 30)).randomize(a2, func)
            texts.append(ast_util.to_c(a2))
        except Exception:
            pass
    parts = [split_source(t, func) for t in texts]
    prefix = parts[0][0]
    same = [p for p in parts if p and p[0] == prefix]
    print(f"{len(same)} of {len(texts)} variants share the declarations")
    symbols = check.elf_symbols()
    address = symbols[func][0] & ~1
    res = Resolver(symbols, address)
    work = tempfile.mkdtemp(dir=os.path.join(ROOT, "build"))
    bad = 0
    t0 = time.perf_counter()
    plain = [compile_plain(prefix + suf, compiler=INSTR) for _, suf in same]
    t_plain = time.perf_counter() - t0
    with Server(prefix) as server:
        t0 = time.perf_counter()
        served = [server.compile(suf) for _, suf in same]
        t_served = time.perf_counter() - t0
    for a, b in zip(plain, served):
        if a != b:
            bad += 1
    print(f"assembly identical for {len(same) - bad} of {len(same)}; "
          f"plain {t_plain / len(same) * 1000:.1f} ms, server {t_served / len(same) * 1000:.1f} ms per compile")
    # bytes: resolver vs the checked link path
    d = tempfile.mkdtemp(dir=work)
    mism = 0
    t0 = time.perf_counter()
    for asm in served[:10]:
        if asm is None:
            continue
        obj = res.assemble(asm, d)
        code, missing = res.bytes_of(obj, func)
        o_path = os.path.join(d, "r.o")
        with contextlib.redirect_stdout(io.StringIO()):
            check.link_object(o_path, os.path.join(d, "r.elf"), syms=symbols, address=address)
        build.sh(["arm-none-eabi-objcopy", "-O", "binary", "-j", ".text", os.path.join(d, "r.elf"),
                  os.path.join(d, "r.bin")])
        linked = open(os.path.join(d, "r.bin"), "rb").read()
        if code != linked[:len(code)] or missing:
            mism += 1
            print("  resolver differs", missing[:3])
    print(f"resolver bytes equal the linked bytes for {min(10, len(served)) - mism} of {min(10, len(served))}")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--selftest", required=True)
    ap.add_argument("func", nargs="?")
    ap.add_argument("-n", type=int, default=40)
    a = ap.parse_args()
    import regdiag
    src = os.path.abspath(a.selftest)
    selftest(src, a.func or regdiag.first_function(src), a.n)


if __name__ == "__main__":
    main()
