#!/usr/bin/env python3
"""Set up a decomp-permuter run for one function.

  permute.py src/fn/sub_08033568.c [FUNC] [--run N_SECONDS] [-j THREADS]

Creates build/permute/FUNC/run-*/ with base.c (preprocessed C), target.o (the
original function assembled and linked), compile.sh (agbcc) and settings.toml.
With --run, runs the permuter for that long and prints the best score; the
best candidates land in that run's output-*/ directories.

Needs decomp-permuter (https://github.com/simonlindholm/decomp-permuter):
set PERMUTER=/path/to/decomp-permuter or clone it next to this repo.
"""
import argparse
import json
import os
import re
import shlex
import signal
import subprocess
import sys
import tempfile

import build
import check
import split
from romlib import ROOT

def _find_permuter():
    if os.environ.get("PERMUTER"):
        return os.environ["PERMUTER"]
    local = os.path.join(ROOT, "build", "tools", "decomp-permuter")
    if os.path.isdir(local):
        return local
    # next to the repo, or next to the main checkout when run from ../wt/NAME
    for base in (os.path.dirname(ROOT), os.path.dirname(os.path.dirname(os.path.dirname(ROOT)))):
        p = os.path.join(base, "decomp-permuter")
        if os.path.isdir(p):
            return p
    return os.path.join(os.path.dirname(ROOT), "decomp-permuter")


PERMUTER = os.path.abspath(_find_permuter())


def run_search(command, directory, seconds):
    """Return False on the time budget; a crashed search is an error."""
    proc = subprocess.Popen(command, cwd=directory, start_new_session=True)
    try:
        proc.wait(timeout=seconds)
    except (subprocess.TimeoutExpired, KeyboardInterrupt) as error:
        try:
            os.killpg(proc.pid, signal.SIGTERM)
        except ProcessLookupError:
            pass
        try:
            proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            os.killpg(proc.pid, signal.SIGKILL)
            proc.wait()
        if isinstance(error, KeyboardInterrupt):
            raise
        return False
    if proc.returncode:
        raise RuntimeError(f"permuter failed (exit {proc.returncode}); run directory: {directory}")
    return True


def prepare_source(src, func):
    # The upstream strip_other_fns regex can mistake a compact if/loop body
    # for another function and silently delete it. Our units already isolate
    # ROM functions: require that shape and retain static inline helpers.
    names = split.c_functions(os.path.join(ROOT, src))
    if names != [func]:
        raise ValueError(f"isolate {func} in a file with one ROM function before permuting; found {names}")
    pre = build.preprocess(src)
    return re.sub(r'^\s*asm\s*\(".*?"\);\s*$', "", pre, flags=re.M)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("func", nargs="?")
    ap.add_argument("--run", type=int, default=0, help="seconds to run the permuter")
    ap.add_argument("--debug", action="store_true", help="score the base once, showing the diff")
    ap.add_argument("-j", type=int, default=4)
    a = ap.parse_args()
    if a.run < 0 or a.j < 1:
        ap.error("--run must be nonnegative and -j must be positive")
    src = a.src if a.src.endswith(".c") else os.path.join("src", "fn", a.src + ".c")
    func = a.func or os.path.basename(src)[:-2]
    alias = dict(re.findall(r"^#define\s+(\w+)\s+(sub_[0-9A-F]{8})\s*$", open(os.path.join(ROOT, src)).read(), re.M))
    label = func = alias.get(func, func)  # cpp expands "#define Name sub_X"
    parent = os.path.join(ROOT, "build", "permute", func)
    os.makedirs(parent, exist_ok=True)
    d = tempfile.mkdtemp(prefix="run-", dir=parent)
    symbols = check.elf_symbols()
    if func not in symbols:
        ap.error(f"{func} is absent from the baseline ELF; rebuild first")
    address = symbols[func][0] & ~1

    # base.c: preprocessed, without the top-level asm(".include") pycparser can't parse
    try:
        pre = prepare_source(src, func)
    except ValueError as error:
        ap.error(str(error))
    open(os.path.join(d, "base.c"), "w").write(pre)

    # target.o: the original function, from the split asm
    build.sh([sys.executable, "tools/split.py"])
    asm = os.path.join(ROOT, "build", "asm", "nonmatching", label + ".s")
    if not os.path.exists(asm):
        sys.exit(f"no original asm for {label} (expected {asm})")
    lines = open(asm).readlines()
    body = "".join(lines[:split.c_replacement_end(lines, 0, len(lines))]).replace(label, func)
    tgt = os.path.join(d, "target.s")
    open(tgt, "w").write('\t.include "asm/macros.inc"\n' + body)
    target_raw = os.path.join(d, "target-unlinked.o")
    build.sh(build.AS + ["-I", ".", "-o", target_raw, tgt])
    check.link_object(target_raw, os.path.join(d, "target.o"), syms=symbols, address=address)

    config = os.path.join(d, "compile.json")
    with open(config, "w") as f:
        json.dump({"compiler": os.path.join(ROOT, build.compiler_for(src)), "assembler": build.AS,
                   "cflags": build.cflags_for(src), "symbols": symbols,
                   "address": address}, f)

    cc = os.path.join(d, "compile.sh")
    open(cc, "w").write(f"""#!/bin/sh
# invoked as: compile.sh input.c -o output.o
exec {shlex.quote(sys.executable)} {shlex.quote(os.path.join(ROOT, 'tools', 'permute_compile.py'))} {shlex.quote(config)} "$@"
""")
    os.chmod(cc, 0o755)
    open(os.path.join(d, "settings.toml"), "w").write(f'func_name = "{func}"\ncompiler_type = "gcc"\n')
    print(d, flush=True)
    if a.run or a.debug:
        # GNU timeout is not supplied by macOS. Terminate the entire worker
        # process group so the permuter's compiler children cannot outlive it.
        cmd = [sys.executable, "-u", os.path.join(PERMUTER, "permuter.py"),
               d, "-j", str(a.j), "--best-only", "--quiet", "--stack-diffs"]
        if a.debug:
            cmd.append("--debug")
        try:
            finished = run_search(cmd, d, a.run or 60)
        except RuntimeError as error:
            sys.exit(str(error))
        if a.debug:
            if not finished:
                sys.exit("permuter diagnostic did not finish")
            return
        outs = sorted(x for x in os.listdir(d) if x.startswith("output-"))
        print("\n".join(os.path.join(d, x) for x in outs[:5]) if outs else "no candidate output saved")


if __name__ == "__main__":
    main()
