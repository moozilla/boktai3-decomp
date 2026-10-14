#!/usr/bin/env python3
"""Try removing register-pinning concessions from C files.

  unpin.py [FILE.c ...] [--write] [-j N]

For each file carrying `// MATCHING_CONCESSION: register-pinning` (default: all
of src/), strip every `register T x asm("rN")` pin (keeping a plain
declaration) and every `asm("")` barrier, compile with tools/check.py, and
report whether the file still matches. With --write, a file that still matches
is rewritten without the hints and without the marker.
"""
import argparse
import os
import re
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MARK = "// MATCHING_CONCESSION: register-pinning"
PIN = re.compile(r'\bregister\s+([^;=()]*?)\s*\basm\s*\(\s*"\w+"\s*\)')
BARRIER = re.compile(r'^[ \t]*(?:__)?asm(?:__)?\s*(?:volatile\s*|__volatile__\s*)?\(\s*""\s*(?::[^;]*)?\)\s*;[ \t]*\n', re.M)
BARRIER_INLINE = re.compile(r'(?:__)?asm(?:__)?\s*(?:volatile\s*|__volatile__\s*)?\(\s*""\s*(?::[^;]*)?\)\s*;')


DRAFT = re.compile(r"#\s*ifdef\s+NONMATCHING\b.*?#\s*else\b", re.S)


def strip_code(text):
    text = PIN.sub(lambda m: m.group(1), text)
    text = BARRIER.sub("", text)
    return BARRIER_INLINE.sub("", text)


def strip(text):
    """Remove hints outside NONMATCHING drafts (a draft's hints are left alone:
    the default build compiles its INCLUDE_ASM fallback, so a check would pass
    trivially). The marker goes only if no hints remain."""
    out, last = [], 0
    for m in DRAFT.finditer(text):
        out.append(strip_code(text[last:m.start()]))
        out.append(m.group(0))
        last = m.end()
    out.append(strip_code(text[last:]))
    text = "".join(out)
    if not any(p.search(text) for p in (PIN, BARRIER_INLINE)):
        text = re.sub(r"^" + re.escape(MARK) + r"[^\n]*\n", "", text, flags=re.M)
    return text


def check(path, text):
    # keep the file in its own directory so relative includes keep working
    fd, tmp = tempfile.mkstemp(suffix=".c", prefix=".unpin_", dir=path.parent)
    with os.fdopen(fd, "w") as f:
        f.write(text)
    try:
        r = subprocess.run([sys.executable, str(ROOT / "tools/check.py"), tmp, "--quiet"],
                           cwd=ROOT, capture_output=True, text=True)
        out = r.stdout + r.stderr
        m = re.search(r"(\d+) differing bytes; fuzzy ([\d.]+)%", out)
        if r.returncode == 0:
            return True, "match"
        if m:
            return False, f"{m.group(1)} bytes differ ({m.group(2)}%)"
        tail = [ln for ln in out.splitlines() if ln.strip()][-1:] or ["?"]
        return False, "error: " + tail[0][:100]
    finally:
        os.unlink(tmp)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("files", nargs="*")
    ap.add_argument("--write", action="store_true")
    ap.add_argument("-j", type=int, default=6)
    a = ap.parse_args()
    files = [Path(f).resolve() for f in a.files] or sorted(
        p for p in (ROOT / "src").rglob("*.c") if MARK in p.read_text())

    def one(p):
        text = p.read_text()
        new = strip(text)
        if new == text:
            return p, None, "no hints"
        ok, msg = check(p, new)
        if ok and a.write:
            p.write_text(new)
        return p, ok, msg

    n = 0
    with ThreadPoolExecutor(a.j) as ex:
        for p, ok, msg in ex.map(one, files):
            n += bool(ok)
            print(f"{'OK  ' if ok else '--  '}{p.relative_to(ROOT)}: {msg}", flush=True)
    print(f"{n}/{len(files)} files match without hints")


if __name__ == "__main__":
    main()
