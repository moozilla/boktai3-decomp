#!/usr/bin/env python3
"""Run tools/fix.py on every draft in tests/fix/ and report what it matched.

  fix_suite.py [NAME...]

Each tests/fix/FUNC.c is a draft as a worker left it (before the edit that
matched), named after the function. A change to fix.py, spellings.py or
rewrites.py should keep every one matching; the compile counts show whether
the search got cheaper or dearer.
"""
import glob
import os
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fix  # noqa: E402
from romlib import ROOT  # noqa: E402


def main():
    os.chdir(ROOT)
    fix.AVOID.clear()  # the suite measures every family, avoided ones included
    paths = sorted(glob.glob(os.path.join(ROOT, "tests", "fix", "*.c")))
    if sys.argv[1:]:
        paths = [p for p in paths if os.path.basename(p)[:-2] in sys.argv[1:]]
    ok = 0
    for p in paths:
        func = os.path.basename(p)[:-2]
        t0 = time.time()
        rc, path, compiles = fix.run(p, func, depth=6, log=lambda *a: None)
        ok += rc == 0
        print(f"{'MATCH' if rc == 0 else 'no   '} {func:28} {len(path)} edits {compiles:5} compiles "
              f"{time.time() - t0:5.1f} s  {'; '.join(path)[:90]}")
    print(f"{ok}/{len(paths)} matched")
    return 0 if ok == len(paths) else 1


if __name__ == "__main__":
    sys.exit(main())
