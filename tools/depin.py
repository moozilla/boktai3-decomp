#!/usr/bin/env python3
"""Remove register pins and empty asm() barriers that a match does not need.

  depin.py src/fn/sub_080390C4.c [...]   [-j 8] [--write]

For each file, tries deleting each hint on its own (``register T x asm("rN")``
becomes ``T x``; ``asm("" ...);`` is dropped) and keeps the deletion whenever
tools/check.py still reports an exact match. Greedy, one hint at a time, so the
result is a locally minimal set of hints, not a global minimum. Candidates are
checked from wip/depin/; with --write, an improved file replaces the original
(and loses its MATCHING_CONCESSION marker when no hints remain).
"""
import argparse
import os
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PIN = r'\bregister\s+[^;=(){}]*?\s+asm\s*\(\s*"[a-z0-9]+"\s*\)'
BAR = r'[ \t]*\b(?:__)?asm(?:__)?\s*(?:volatile\s*)?\(\s*""[^;]*\)\s*;[ \t]*\n?'
HINT = re.compile(f'(?:{PIN})|(?:{BAR})')
MARK = "// MATCHING_CONCESSION: register-pinning\n"


def drop(text, m):
    s = m.group(0)
    if s.lstrip().startswith("register"):
        s = re.sub(r'^register\s+', '', s)
        s = re.sub(r'\s+asm\s*\(\s*"[a-z0-9]+"\s*\)$', '', s)
    else:
        s = "\n" if s.endswith("\n") and m.start() > 0 and text[m.start() - 1] != "\n" else ""
    return text[:m.start()] + s + text[m.end():]


def matches(path, text):
    with open(path, "w") as f:
        f.write(text)
    r = subprocess.run([sys.executable, os.path.join(ROOT, "tools/check.py"), path, "--quiet"],
                       cwd=ROOT, capture_output=True, text=True)
    return r.returncode == 0


def minimize(src):
    text = open(src).read()
    trial = os.path.join(ROOT, "wip/depin", os.path.basename(src))
    before = len(HINT.findall(text))
    if not matches(trial, text):
        return src, before, None, text
    i = 0
    while True:
        hints = list(HINT.finditer(text))
        if i >= len(hints):
            break
        cand = drop(text, hints[i])
        if matches(trial, cand):
            text = cand
        else:
            i += 1
    if not HINT.search(text):
        text = text.replace(MARK, "")
    return src, before, len(HINT.findall(text)), text


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("files", nargs="+")
    ap.add_argument("-j", type=int, default=os.cpu_count())
    ap.add_argument("--write", action="store_true")
    args = ap.parse_args()
    os.makedirs(os.path.join(ROOT, "wip/depin"), exist_ok=True)
    with ThreadPoolExecutor(args.j) as ex:
        for src, before, after, text in ex.map(minimize, args.files):
            if after is None:
                print(f"{src}: does not match as committed; skipped")
                continue
            print(f"{src}: {before} -> {after} hints", flush=True)
            if args.write and after < before:
                open(src, "w").write(text)


if __name__ == "__main__":
    main()
