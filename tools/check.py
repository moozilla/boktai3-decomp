#!/usr/bin/env python3
"""Check one C file against the ROM without a full build, and show a diff.

  check.py src/fn/sub_08033568.c [--quiet]
  check.py sub_08033568              (same; looks in src/fn/)

Compiles the file exactly as build.py does, links it alone at the address of
its first function (every other symbol resolved from the last full build's
build/boktai3.elf), and compares the bytes with baserom.gba. Prints a
side-by-side disassembly with mismatching lines marked '!'. Exit status 0 on
a match. A full `tools/build.py` run is still needed before committing (it
checks the layout of the whole ROM); this is the fast inner loop.
"""
import argparse
import os
import re
import subprocess
import sys

import build
from romlib import ROOT, load_rom, off

try:
    import capstone
except ImportError:  # pragma: no cover
    capstone = None


def elf_symbols():
    elf = os.path.join(ROOT, "build", "boktai3.elf")
    if not os.path.exists(elf):
        sys.exit("build/boktai3.elf missing: run tools/build.py once first")
    # readelf, not nm: nm clears bit 0 of Thumb function symbols
    out = build.sh(["arm-none-eabi-readelf", "-sW", elf])
    syms = {}
    for ln in out.splitlines():
        p = ln.split()
        if len(p) == 8 and p[0][:-1].isdigit() and p[6] != "UND":
            syms[p[7]] = (int(p[1], 16), p[3])
    return syms


def disasm(code, addr):
    if capstone is None:
        return [(addr + i, code[i:i + 2].hex()) for i in range(0, len(code), 2)]
    md = capstone.Cs(capstone.CS_ARCH_ARM, capstone.CS_MODE_THUMB)
    out, i = [], 0
    while i < len(code):
        ins = next(md.disasm(code[i:i + 4], addr + i), None)
        if ins is None:
            out.append((addr + i, ".2byte 0x%04x" % int.from_bytes(code[i:i + 2], "little")))
            i += 2
        else:
            out.append((addr + i, f"{ins.mnemonic} {ins.op_str}".strip()))
            i += ins.size
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("--quiet", action="store_true")
    a = ap.parse_args()
    src = a.src
    if not src.endswith(".c"):
        src = os.path.join("src", "fn", src + ".c")
    src = os.path.relpath(os.path.join(ROOT, src), ROOT)
    work = os.path.join("build", "check")
    os.makedirs(os.path.join(ROOT, work), exist_ok=True)
    base = os.path.join(work, os.path.basename(src)[:-2])
    obj = base + ".o"
    build.build_c(src, obj)

    defined = []
    undefined = []
    for ln in build.sh(["arm-none-eabi-nm", obj]).splitlines():
        p = ln.split()
        if len(p) == 2 and p[0] == "U":
            undefined.append(p[1])
        elif len(p) == 3 and p[1] in "Tt" and not p[2].endswith("__addr") and not p[2].startswith("."):
            defined.append((int(p[0], 16), p[2]))
    if not defined:
        sys.exit("no functions defined in " + src)
    defined.sort()
    syms = elf_symbols()
    first = defined[0][1]
    if first not in syms:
        m = re.search(r"_([0-9A-F]{8})$", first)
        if not m:
            sys.exit(f"cannot find the address of {first} (not in build/boktai3.elf)")
        addr = int(m.group(1), 16)
    else:
        addr = syms[first][0] & ~1
    addr -= defined[0][0]

    stub = ["\t.syntax unified"]
    missing = []
    for name in undefined:
        key = name[:-6] if name.endswith("__addr") else name
        if key in syms:
            v, t = syms[key]
            # Thumb functions keep bit 0 (BL ignores it, a .word needs it);
            # NAME__addr is the plain even address.
            if name.endswith("__addr"):
                v &= ~1
            stub += [f"\t.global {name}", f"\t.set {name}, 0x{v:08X}"]
        else:
            m = re.search(r"_([0-9A-Fa-f]{8})$", name)
            if m:
                stub += [f"\t.global {name}", f"\t.set {name}, 0x{m.group(1)}"]
            else:
                missing.append(name)
    if missing:
        sys.exit("unresolved symbols: " + ", ".join(missing))
    open(os.path.join(ROOT, base + "_stub.s"), "w").write("\n".join(stub) + "\n")
    build.sh(build.AS + ["-o", base + "_stub.o", base + "_stub.s"])
    build.sh(["arm-none-eabi-ld", f"-Ttext=0x{addr:08X}", "-e", "0", "-o", base + ".elf",
              obj, base + "_stub.o"])
    build.sh(["arm-none-eabi-objcopy", "-O", "binary", "-j", ".text", base + ".elf", base + ".bin"])
    mine = open(os.path.join(ROOT, base + ".bin"), "rb").read()
    rom = load_rom(verify=False)
    orig = rom[off(addr):off(addr) + len(mine)]
    ok = mine == orig
    if not ok or not a.quiet:
        la, lb = disasm(orig, addr), disasm(mine, addr)
        n = max(len(la), len(lb))
        print(f"{'address':8}  {'original':38} {'yours':38}")
        for i in range(n):
            x = la[i] if i < len(la) else (0, "")
            y = lb[i] if i < len(lb) else (0, "")
            mark = " " if x[1] == y[1] and x[0] == y[0] else "!"
            print(f"{(x[0] or y[0]):08X}{mark} {x[1]:38.38} {y[1]:38.38}")
    names = ", ".join(n for _, n in defined)
    print(f"{names}: {'MATCH' if ok else 'MISMATCH'} ({len(mine)} bytes at 0x{addr:08X})")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
