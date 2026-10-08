#!/usr/bin/env python3
"""Build the ROM from generated asm + decompiled C, and compare.

  build.py [--shift N] [--out build/boktai3.gba] [--no-compare]

Steps: split.py (cut asm around src/*.c units) -> compile C with agbcc ->
assemble segments + data -> link in ROM order -> objcopy -> SHA-1 check.
Unchanged inputs are not rebuilt (content hash).
"""
import argparse
import hashlib
import os
import re
import subprocess
import sys

from romlib import BASEROM_SHA1, ROOT

TOOLS = os.path.join(ROOT, "build", "tools")
AGBCC = os.environ.get("AGBCC", os.path.join(TOOLS, "agbcc", "agbcc"))
AGBCC_INC = os.path.join(os.path.dirname(AGBCC), "ginclude")
AS = ["arm-none-eabi-as", "-mcpu=arm7tdmi", "-mthumb-interwork", "--no-warn"]
CFLAGS = ["-O2", "-mthumb-interwork", "-fhex-asm"]


def sh(cmd, **kw):
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, **kw)
    if r.returncode:
        sys.stderr.write(r.stdout + r.stderr)
        sys.exit(f"failed: {' '.join(cmd) if isinstance(cmd, list) else cmd}")
    return r.stdout


def stamp_ok(obj, key):
    st = obj + ".hash"
    return os.path.exists(obj) and os.path.exists(st) and open(st).read() == key


def stamp(obj, key):
    open(obj + ".hash", "w").write(key)


def file_hash(*paths, extra=""):
    h = hashlib.sha1(extra.encode())
    for p in paths:
        h.update(open(os.path.join(ROOT, p), "rb").read())
    return h.hexdigest()


def build_asm(src, obj, defsym=None):
    key = file_hash(src, "asm/macros.inc", extra=str(defsym))
    if stamp_ok(os.path.join(ROOT, obj), key):
        return
    cmd = AS + ["-I", ".", "-o", obj, src]
    if defsym:
        cmd[1:1] = ["--defsym", defsym]
    sh(cmd)
    stamp(os.path.join(ROOT, obj), key)


DATA_START = 0x0824DAFC
RAW_ROM = re.compile(r"\b0[xX]0([89][0-9A-Fa-f]{6})\b")


def lint_c(src):
    """A raw ROM-data address in C matches byte-for-byte but stays put when
    the data moves (shift test). Reference the gen/data.s label instead."""
    text = re.sub(r"/\*.*?\*/|//[^\n]*", "", open(os.path.join(ROOT, src)).read(), flags=re.S)
    bad = [m.group(0) for m in RAW_ROM.finditer(text) if int(m.group(0), 16) >= DATA_START]
    if bad:
        sys.exit(f"{src}: raw ROM data address {', '.join(sorted(set(bad)))}; "
                 f"declare `extern const u8 gUnk_XXXXXXXX[];` (the label in gen/data.s) and use that")


def build_c(src, obj):
    lint_c(src)
    deps = [src] + [os.path.join(d, f) for d in ("include", "include/gba")
                    for f in sorted(os.listdir(os.path.join(ROOT, d))) if f.endswith(".h")]
    # per-file flags: a line "// CFLAGS: -O2 ..." replaces the default flags
    m = re.search(r"^// CFLAGS: (.*)$", open(os.path.join(ROOT, src)).read(), re.M)
    cflags = m.group(1).split() + ["-fhex-asm"] if m else CFLAGS
    key = file_hash(*deps, "tools/build.py", extra=" ".join(cflags))
    if stamp_ok(os.path.join(ROOT, obj), key):
        return
    pre = sh(["cpp", "-P", "-nostdinc", "-undef", "-I", "include", "-iquote", ".", src])
    asm = subprocess.run([AGBCC] + cflags + ["-o", "-"], input=pre, cwd=ROOT,
                         capture_output=True, text=True)
    if asm.returncode or "error" in asm.stderr:
        sys.stderr.write(asm.stderr)
        sys.exit(f"agbcc failed on {src}")
    s_path = os.path.join(ROOT, obj[:-2] + ".s")
    # After each function label add a plain (non-Thumb) alias NAME__addr: data
    # that points at the function's even address references it (an ABS32
    # relocation against a Thumb function symbol would set bit 0).
    text = re.sub(r"^(\w+):\n", lambda m: f"{m.group(1)}:\n\t.global {m.group(1)}__addr\n{m.group(1)}__addr:\n"
                  if not m.group(1).startswith(".") else m.group(0), asm.stdout, flags=re.M)
    open(s_path, "w").write(text + "\t.text\n\t.align\t2, 0\n")
    sh(AS + ["-o", obj, s_path])
    stamp(os.path.join(ROOT, obj), key)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--shift", default=None)
    ap.add_argument("--out", default="build/boktai3.gba")
    ap.add_argument("--no-compare", action="store_true")
    a = ap.parse_args()
    sh([sys.executable, "tools/split.py"])
    units = [ln.split() for ln in open(os.path.join(ROOT, "build", "units.txt"))]
    objs = []
    for kind, path in units:
        if kind == "asm":
            obj = path[:-2] + ".o"
            build_asm(path, obj)
        else:
            obj = os.path.join("build", path[:-2] + ".o")
            os.makedirs(os.path.join(ROOT, os.path.dirname(obj)), exist_ok=True)
            build_c(path, obj)
        objs.append(obj)
    # data region: all labels global so code can reference them
    data_g = os.path.join(ROOT, "build", "asm", "data_g.s")
    data = open(os.path.join(ROOT, "gen", "data.s")).read()
    labels = re.findall(r"^(\w+):", data, re.M) + re.findall(r"^\t\.set (\w+),", data, re.M)
    hdr = '\t.include "asm/macros.inc"\n\t.syntax unified\n\t.text\n'
    shift = "\t.ifdef SHIFT\n\t.space SHIFT\n\t.endif\n"
    new = hdr + "".join(f"\t.global {l}\n" for l in labels) + shift + data
    if not os.path.exists(data_g) or open(data_g).read() != new:
        open(data_g, "w").write(new)
    dobj = "build/asm/data_g.o" if not a.shift else "build/asm/data_shift.o"
    build_asm("build/asm/data_g.s", dobj, defsym=f"SHIFT={a.shift}" if a.shift else None)
    objs.append(dobj)

    # RAM symbols used by C (gUnk_02XXXXXX / gUnk_03XXXXXX style names)
    syms = set()
    for kind, path in units:
        if kind == "c":
            out = sh(["arm-none-eabi-nm", "-u", os.path.join("build", path[:-2] + ".o")])
            syms |= set(re.findall(r"\b(\w+_(0[23][0-9A-Fa-f]{6}))\b", out))
    ld = ["SECTIONS {", "  . = 0x08000000;", "  .text : {"]
    ld += [f"    {o}(.text)" for o in objs]
    ld += ["  }", "  /DISCARD/ : { *(.comment) *(.ARM.attributes) }", "}"]
    ld += [f"{name} = 0x{addr};" for name, addr in sorted(syms)]
    ld += open(os.path.join(ROOT, "build", "c_labels.ld")).read().splitlines()
    ram = os.path.join(ROOT, "symbols", "ram.ld")
    if os.path.exists(ram):
        ld += open(ram).read().splitlines()
    open(os.path.join(ROOT, "build", "link.ld"), "w").write("\n".join(ld) + "\n")
    elf = a.out[:-4] + ".elf"
    sh(["arm-none-eabi-ld", "-T", "build/link.ld", "-o", elf])
    sh(["arm-none-eabi-objcopy", "-O", "binary", "-j", ".text", elf, a.out])
    if a.shift:
        b = open(os.path.join(ROOT, a.out), "rb").read()
        if len(b) > 0x1000000 and not any(b[0x1000000:]):
            open(os.path.join(ROOT, a.out), "wb").write(b[:0x1000000])
    if a.no_compare or a.shift:
        return
    h = hashlib.sha1(open(os.path.join(ROOT, a.out), "rb").read()).hexdigest()
    if h != BASEROM_SHA1:
        base = open(os.path.join(ROOT, "baserom.gba"), "rb").read()
        new = open(os.path.join(ROOT, a.out), "rb").read()
        diff = next((i for i in range(min(len(base), len(new))) if base[i] != new[i]), None)
        # Layout shifts make the first byte difference misleading: name the
        # first function whose linked address differs from its original one.
        moved = None
        for ln in sh(["arm-none-eabi-nm", "-n", elf]).splitlines():
            m = re.match(r"([0-9a-f]{8}) [Tt] sub_([0-9A-F]{8})$", ln)
            if m and int(m.group(1), 16) & ~1 != int(m.group(2), 16):
                moved = m.group(2)
                break
        msg = f"{a.out}: MISMATCH (sha1 {h}); first difference at " + (
            f"{0x08000000 + diff:#010x}" if diff is not None else "(length)")
        if moved:
            msg += (f"\n  layout shifted: sub_{moved} is not at its original address -- the C unit "
                    f"just before it compiles to a different size")
        sys.exit(msg)
    print(f"{a.out}: OK")


if __name__ == "__main__":
    main()
