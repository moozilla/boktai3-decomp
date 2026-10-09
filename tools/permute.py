#!/usr/bin/env python3
"""Set up a decomp-permuter run for one function.

  permute.py src/fn/sub_08033568.c [FUNC] [--run N_SECONDS] [-j THREADS]

Creates build/permute/FUNC/ with base.c (preprocessed C), target.o (the
original function assembled from gen/), compile.sh (agbcc) and settings.toml.
With --run, runs the permuter for that long and prints the best score; the
best candidates land in build/permute/FUNC/output-*/.

Needs decomp-permuter (https://github.com/simonlindholm/decomp-permuter):
set PERMUTER=/path/to/decomp-permuter or clone it next to this repo.
"""
import argparse
import os
import re
import signal
import subprocess
import sys

import build
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


PERMUTER = _find_permuter()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("func", nargs="?")
    ap.add_argument("--run", type=int, default=0, help="seconds to run the permuter")
    ap.add_argument("-j", type=int, default=4)
    a = ap.parse_args()
    src = a.src if a.src.endswith(".c") else os.path.join("src", "fn", a.src + ".c")
    func = a.func or os.path.basename(src)[:-2]
    alias = dict(re.findall(r"^#define\s+(\w+)\s+(sub_[0-9A-F]{8})\s*$", open(os.path.join(ROOT, src)).read(), re.M))
    label = func = alias.get(func, func)  # cpp expands "#define Name sub_X"
    d = os.path.join(ROOT, "build", "permute", func)
    os.makedirs(d, exist_ok=True)

    # base.c: preprocessed, without the top-level asm(".include") pycparser can't parse
    pre = build.preprocess(src)
    pre = re.sub(r'^\s*asm\s*\(".*?"\);\s*$', "", pre, flags=re.M)
    open(os.path.join(d, "base.c"), "w").write(pre)
    subprocess.run([sys.executable, os.path.join(PERMUTER, "strip_other_fns.py"),
                    os.path.join(d, "base.c"), func], check=True)

    # target.o: the original function, from the split asm
    build.sh([sys.executable, "tools/split.py"])
    asm = os.path.join(ROOT, "build", "asm", "nonmatching", label + ".s")
    if not os.path.exists(asm):
        sys.exit(f"no original asm for {label} (expected {asm})")
    body = open(asm).read().replace(label, func)
    tgt = os.path.join(d, "target.s")
    open(tgt, "w").write('\t.include "asm/macros.inc"\n' + body)
    build.sh(build.AS + ["-I", ".", "-o", os.path.join(d, "target.o"), tgt])

    cc = os.path.join(d, "compile.sh")
    open(cc, "w").write(f"""#!/bin/sh
# invoked as: compile.sh input.c -o output.o
set -e
T=$(mktemp -d)
{build.AGBCC} {' '.join(build.CFLAGS)} "$1" -o "$T/out.s"
printf '\\t.text\\n\\t.align 2, 0\\n' >> "$T/out.s"
{' '.join(build.AS)} -o "$3" "$T/out.s"
rm -rf "$T"
""")
    os.chmod(cc, 0o755)
    open(os.path.join(d, "settings.toml"), "w").write(f'func_name = "{func}"\ncompiler_type = "gcc"\n')
    print(d)
    if a.run:
        # GNU timeout is not supplied by macOS. Terminate the entire worker
        # process group so the permuter's compiler children cannot outlive it.
        proc = subprocess.Popen([sys.executable, os.path.join(PERMUTER, "permuter.py"),
                                 d, "-j", str(a.j), "--best-only", "--quiet"],
                                start_new_session=True)
        try:
            proc.wait(timeout=a.run)
        except (subprocess.TimeoutExpired, KeyboardInterrupt):
            try:
                os.killpg(proc.pid, signal.SIGTERM)
            except ProcessLookupError:
                pass
            try:
                proc.wait(timeout=5)
            except subprocess.TimeoutExpired:
                os.killpg(proc.pid, signal.SIGKILL)
                proc.wait()
        outs = sorted(x for x in os.listdir(d) if x.startswith("output-"))
        print("\n".join(outs[:5]) if outs else "no improvement found")


if __name__ == "__main__":
    main()
