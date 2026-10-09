#!/usr/bin/env python3
"""Compile a preprocessed permuter candidate and resolve it at its ROM address.

Called by the generated compile.sh; final acceptance still uses check.py/make.
"""
import argparse
import json
from pathlib import Path
import re
import subprocess
import tempfile

import build
import check


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("config")
    ap.add_argument("source")
    ap.add_argument("-o", required=True)
    args = ap.parse_args()
    config = json.loads(Path(args.config).read_text())
    source, output = Path(args.source).resolve(), Path(args.o).resolve()
    with tempfile.TemporaryDirectory(dir=output.parent, prefix="compile-") as temp:
        asm = subprocess.run([config["compiler"], *config["cflags"], "-o", "-"],
                             input=source.read_text(), text=True, capture_output=True)
        if asm.returncode or "error" in asm.stderr:
            raise SystemExit(asm.stderr or "agbcc failed")
        # Keep the same even-address aliases and final alignment as build_c.
        text = re.sub(r"^(\w+):\n", lambda m:
                      f"{m[1]}:\n\t.global {m[1]}__addr\n{m[1]}__addr:\n"
                      if not m[1].startswith(".") else m[0], asm.stdout, flags=re.M)
        assembly, obj = str(Path(temp) / "candidate.s"), str(Path(temp) / "candidate.o")
        Path(assembly).write_text(text + "\t.text\n\t.align 2, 0\n")
        build.sh(config["assembler"] + ["-o", obj, assembly])
        check.link_object(obj, str(output), syms=config["symbols"], address=config["address"])
        # The caller wants just the ELF, not the short-lived symbol stubs.
        Path(str(output) + "_stub.s").unlink()
        Path(str(output) + "_stub.o").unlink()


if __name__ == "__main__":
    main()
