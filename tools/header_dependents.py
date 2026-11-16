#!/usr/bin/env python3
"""List (and check) every source unit that includes a header, directly or through others.

  header_dependents.py include/player.h ...            list the dependent src/ units
  header_dependents.py include/player.h --check [-j N] run tools/check.py on each; exit 1 on a mismatch
  header_dependents.py --since REV --check             the headers changed since REV (git diff)

A header change that is meant to change no bytes (a field renamed, a byte array
given its real struct type) must leave every dependent unit byte-identical. The
include graph comes from `#include "..."` lines in src/ and include/, matched by
file name, the way the build's -I include resolves them.
"""
import argparse
import concurrent.futures
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
INCLUDE = re.compile(r'^\s*#\s*include\s+"([^"]+)"', re.M)


def tree():
    files = {}
    for top in ("src", "include"):
        for d, _, names in os.walk(os.path.join(ROOT, top)):
            for n in names:
                if n.endswith((".c", ".h")):
                    p = os.path.join(d, n)
                    with open(p, encoding="utf-8", errors="replace") as f:
                        files[os.path.relpath(p, ROOT)] = {os.path.basename(x) for x in INCLUDE.findall(f.read())}
    return files


def dependents(headers, files=None):
    """Sorted src/ .c files that include any of headers, transitively."""
    files = files or tree()
    closure = {os.path.basename(h) for h in headers}
    grew = True
    while grew:
        grew = False
        for p, inc in files.items():
            b = os.path.basename(p)
            if p.endswith(".h") and b not in closure and inc & closure:
                closure.add(b)
                grew = True
    return sorted(p for p, inc in files.items()
                  if p.endswith(".c") and p.startswith("src" + os.sep) and inc & closure)


def check(path):
    r = subprocess.run([sys.executable, os.path.join(ROOT, "tools", "check.py"), path, "--quiet"],
                       cwd=ROOT, capture_output=True, text=True)
    lines = (r.stdout + r.stderr).strip().splitlines()
    # a compile failure: show agbcc's first errors ("stdin:LINE: message", not warnings)
    errors = [ln for ln in lines if re.match(r"stdin:\d+: (?!warning)", ln) or "error:" in ln]
    return path, r.returncode, errors[:3] or lines[-1:] or [""]


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("headers", nargs="*")
    ap.add_argument("--since", metavar="REV", help="use the headers changed since REV")
    ap.add_argument("--check", action="store_true", help="run tools/check.py on every dependent")
    ap.add_argument("-j", type=int, default=os.cpu_count() or 4)
    a = ap.parse_args()
    headers = list(a.headers)
    if a.since:
        out = subprocess.run(["git", "diff", "--name-only", a.since, "--", "include", "src"],
                             cwd=ROOT, capture_output=True, text=True, check=True).stdout.split()
        headers += [p for p in out if p.endswith(".h")]
    if not headers:
        sys.exit("no headers given")
    deps = dependents(headers)
    if not a.check:
        print("\n".join(deps))
        return
    bad = []
    with concurrent.futures.ThreadPoolExecutor(a.j) as ex:
        for path, rc, tail in ex.map(check, deps):
            if rc:
                bad.append(path)
                print(f"MISMATCH {path}: " + "\n    ".join(tail))
    print(f"{len(deps) - len(bad)} of {len(deps)} dependents match")
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
