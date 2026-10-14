#!/usr/bin/env python3
"""Verify that every data/manifest.csv entry re-encodes to the original ROM bytes.

  check.py              check all entries
  check.py --extract N  (re)write entry N's source file from the ROM
"""
import argparse
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from datafmt import ROM_BASE, load_manifest, to_bytes  # noqa: E402
from romlib import load_rom  # noqa: E402
from symbolize import load_func_names  # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--extract", metavar="NAME")
    a = ap.parse_args()
    rom = load_rom()
    bad = 0
    for e in load_manifest():
        if a.extract and e.name != a.extract:
            continue
        if a.extract:
            if hasattr(e.module(), "extract"):
                e.extract()
            else:
                text = e.module().decode(rom, e.start, e.end, load_func_names())
                with open(e.file, "w", encoding="utf-8") as f:
                    f.write(text)
            print(f"wrote {os.path.relpath(e.file)}")
        got = to_bytes(e.encode())
        want = rom[e.start - ROM_BASE:e.end - ROM_BASE]
        if got != want:
            first = next(i for i in range(len(want)) if i >= len(got) or got[i] != want[i])
            print(f"{e.name}: MISMATCH at {e.start + first:08X}")
            bad += 1
        else:
            print(f"{e.name}: OK ({e.end - e.start:#x} bytes, {e.format})")
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
