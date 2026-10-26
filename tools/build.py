#!/usr/bin/env python3
"""Build the ROM from generated asm + decompiled C, and compare.

  build.py [--shift N] [--out build/boktai3.gba] [--no-compare]

Steps: split.py (cut asm around src/*.c units) -> compile C with agbcc ->
assemble segments + data -> link in ROM order -> objcopy -> SHA-1 check.
Unchanged inputs are not rebuilt (content hash).
"""
import argparse
import concurrent.futures
import functools
import hashlib
import os
import re
import shlex
import subprocess
import sys
import tempfile
import time

from romlib import BASEROM_SHA1, ROOT

TOOLS = os.path.join(ROOT, "build", "tools")
AGBCC = os.environ.get("AGBCC", os.path.join(TOOLS, "agbcc", "agbcc"))
AGBCC_INC = os.path.join(os.path.dirname(AGBCC), "ginclude")
AS = ["arm-none-eabi-as", "-mcpu=arm7tdmi", "-mthumb-interwork", "--no-warn"]
CFLAGS = ["-O2", "-mthumb-interwork", "-fhex-asm"]
CPP = shlex.split(os.environ.get("CPP", "cc -E" if sys.platform == "darwin" else "cpp"))
JOBS = int(os.environ.get("BUILD_JOBS", "4"))   # parallel compiles; 10 cores shared with other workers
# Extra preprocessor defines; check.py --nonmatching adds -DNONMATCHING.
DEFINES = []


def sh(cmd, **kw):
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, **kw)
    if r.returncode:
        sys.stderr.write(r.stdout + r.stderr)
        sys.exit(f"failed: {' '.join(cmd) if isinstance(cmd, list) else cmd}")
    return r.stdout


def preprocess(src):
    # macOS /usr/bin/cpp is a legacy shell wrapper which mishandles separated
    # include arguments. The compiler driver provides the same preprocessing.
    return sh(CPP + ["-P", "-nostdinc", "-undef"] + DEFINES + ["-I", "include", "-iquote", ".", src])


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


def cflags_for(src):
    # per-file flags: a line "// CFLAGS: -O2 ..." replaces the default flags
    with open(os.path.join(ROOT, src)) as f:
        m = re.search(r"^// CFLAGS: (.*)$", f.read(), re.M)
    flags = m.group(1).split() + ["-fhex-asm"] if m else CFLAGS.copy()
    # The bundled ARM compiler does not implement the Thumb fork's option.
    if os.path.basename(compiler_for(src)) == "agbcc_arm":
        flags = [flag for flag in flags if flag != "-fhex-asm"]
    return flags


def compiler_for(src):
    """Library objects can predate the game's compiler variant."""
    with open(os.path.join(ROOT, src)) as f:
        m = re.search(r"^// COMPILER: (\S+)\s*$", f.read(), re.M)
    if not m or m.group(1) == "agbcc":
        return AGBCC
    if m.group(1) in ("old_agbcc", "agbcc_arm"):
        return os.path.join(os.path.dirname(AGBCC), m.group(1))
    sys.exit(f"{src}: unknown compiler {m.group(1)!r}; use agbcc, old_agbcc, or agbcc_arm")


@functools.lru_cache(maxsize=None)
def compiler_identity(path):
    # Both the selected executable and its contents affect generated code.
    # Hash once per invocation, rather than once for every translation unit.
    return os.path.realpath(path) + ":" + hashlib.sha1(open(path, "rb").read()).hexdigest()


@functools.lru_cache(maxsize=None)
def headers_hash():
    # Every unit depends on every header: hash them once per invocation, not per unit.
    return file_hash(*[os.path.join(d, f) for d in ("include", "include/gba")
                       for f in sorted(os.listdir(os.path.join(ROOT, d))) if f.endswith(".h")], "tools/build.py")


def build_c(src, obj):
    lint_c(src)
    # INCLUDE_ASM pulls in split.py output, which changes with gen/ and symbols.
    deps = [src] + [os.path.join("build", "asm", "nonmatching", n + ".s")
                    for n in re.findall(r'INCLUDE_ASM\(\s*"[^"]*"\s*,\s*(\w+)\s*\)', open(os.path.join(ROOT, src)).read())
                    if os.path.exists(os.path.join(ROOT, "build", "asm", "nonmatching", n + ".s"))]
    cflags = cflags_for(src)
    compiler = compiler_for(src)
    key = file_hash(*deps, extra=headers_hash() + compiler_identity(compiler) + " ".join(CPP + DEFINES + cflags))
    if stamp_ok(os.path.join(ROOT, obj), key):
        return
    pre = preprocess(src)
    # agbcc emits assembly before it has consumed all preprocessed input.
    # File-backed stdin avoids a two-pipe deadlock observed on macOS.
    with tempfile.TemporaryFile(mode="w+") as compiler_input:
        compiler_input.write(pre)
        compiler_input.seek(0)
        asm = subprocess.run([compiler] + cflags + ["-o", "-"], stdin=compiler_input,
                             cwd=ROOT, capture_output=True, text=True)
    if asm.returncode or "error" in asm.stderr:
        sys.stderr.write(asm.stderr)
        sys.exit(f"{os.path.basename(compiler)} failed on {src}")
    s_path = os.path.join(ROOT, obj[:-2] + ".s")
    # After each function label add a plain (non-Thumb) alias NAME__addr: data
    # that points at the function's even address references it (an ABS32
    # relocation against a Thumb function symbol would set bit 0).
    text = re.sub(r"^(\w+):\n", lambda m: f"{m.group(1)}:\n\t.global {m.group(1)}__addr\n{m.group(1)}__addr:\n"
                  if not m.group(1).startswith(".") else m.group(0), asm.stdout, flags=re.M)
    open(s_path, "w").write(text + "\t.text\n\t.align\t2, 0\n")
    sh(AS + ["-o", obj, s_path])
    stamp(os.path.join(ROOT, obj), key)


def ram_symbols():
    """Named RAM objects from symbols/ram.csv: {name: address}.

    gUnk_02XXXXXX/gUnk_03XXXXXX names link from their suffix; a real name needs
    its row in the inventory (docs/RAM.md)."""
    import csv
    path = os.path.join(ROOT, "symbols", "ram.csv")
    if not os.path.exists(path):
        return {}
    with open(path, newline="") as f:
        return {r["name"]: int(r["addr"], 16) for r in csv.DictReader(f)
                if r["name"] and not re.search(r"_0[23][0-9A-F]{6}$", r["name"])}


def wait_for_cloud_setup():
    """A cloud session's start hook (tools/cloud_setup.sh --session) may still be
    rebuilding in the background; wait for it instead of racing it."""
    pidfile = os.path.join(ROOT, "build", ".cloud-setup.pid")
    if os.environ.get("B3_SETUP_BG") or not os.path.exists(pidfile):
        return
    try:
        pid = int(open(pidfile).read())
    except (OSError, ValueError):
        return
    # A stale file from an earlier session can name a reused pid (even a thread
    # of a tool that is itself waiting on this build); wait only for the setup.
    try:
        if b"cloud_setup" not in open(f"/proc/{pid}/cmdline", "rb").read():
            return
    except OSError:
        pass
    said = False
    while True:
        try:
            os.kill(pid, 0)
        except OSError:
            return
        if not said:
            print("waiting for the background cloud setup (build/cloud-setup.log)...", file=sys.stderr)
            said = True
        time.sleep(1)


def main():
    wait_for_cloud_setup()
    ap = argparse.ArgumentParser()
    ap.add_argument("--shift", default=None)
    ap.add_argument("--out", default="build/boktai3.gba")
    ap.add_argument("--no-compare", action="store_true")
    a = ap.parse_args()
    sh([sys.executable, "tools/split.py"])
    units = [ln.split() for ln in open(os.path.join(ROOT, "build", "units.txt"))]
    def one(unit):
        kind, path = unit
        if kind == "asm":
            obj = path[:-2] + ".o"
            build_asm(path, obj)
        else:
            obj = os.path.join("build", path[:-2] + ".o")
            os.makedirs(os.path.join(ROOT, os.path.dirname(obj)), exist_ok=True)
            build_c(path, obj)
        return obj
    # Compile in parallel (subprocess-bound, so threads); objects keep ROM order for the link.
    # Unchanged units are skipped by their content-hash stamps.
    with concurrent.futures.ThreadPoolExecutor(JOBS) as ex:
        objs = list(ex.map(one, units))
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
    cobjs = [os.path.join("build", path[:-2] + ".o") for kind, path in units if kind == "c"]
    for i in range(0, len(cobjs), 400):                # one nm per 400 objects, not per object
        out = sh(["arm-none-eabi-nm", "-u"] + cobjs[i:i + 400])
        syms |= set(re.findall(r"\b(\w+_(0[23][0-9A-Fa-f]{6}))\b", out))
    ld = ["SECTIONS {", "  . = 0x08000000;", "  .text : {"]
    ld += [f"    {o}(.text)" for o in objs]
    ld += ["  }", "  /DISCARD/ : { *(.comment) *(.ARM.attributes) }", "}"]
    ld += [f"{name} = 0x{addr};" for name, addr in sorted(syms)]
    ld += [f"{name} = 0x{addr:08X};" for name, addr in sorted(ram_symbols().items())]
    ld += open(os.path.join(ROOT, "build", "c_labels.ld")).read().splitlines()
    ram = os.path.join(ROOT, "symbols", "ram.ld")
    if os.path.exists(ram):
        ld += open(ram).read().splitlines()
    open(os.path.join(ROOT, "build", "link.ld"), "w").write("\n".join(ld) + "\n")
    elf = a.out[:-4] + ".elf"
    sh(["arm-none-eabi-ld", "-T", "build/link.ld", "-o", elf])
    sh(["arm-none-eabi-objcopy", "-O", "binary", "-j", ".text", elf, a.out])
    # Shifted or resized data grows the image into the trailing zero padding:
    # trim back to 16 MiB so EEPROM stays at 0x0D000000.
    if True:
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
    # Every matched function must compile to the whole span progress credits
    # it, or a raw tail after it would be counted as decompiled.
    import progress
    tails = progress.check_tails(elf)
    if tails:
        sys.exit(f"{a.out}: matches, but progress would credit hidden assembly:\n  " + "\n  ".join(tails))
    print(f"{a.out}: OK")


if __name__ == "__main__":
    main()
