#!/usr/bin/env python3
"""Remove register pins from matched code with the register solver.

  pinfarm.py list [UNIT.c ...] [--out DIR]     write DIR/list.tsv (default wip/farm)
  pinfarm.py apply RESULTS.tsv [--commit]      splice matches into src/

`list` finds every function in a unit marked
`// MATCHING_CONCESSION: register-pinning` (default: all of src/) whose body
still holds a pin or an empty asm() barrier, isolates it (tools/isolate.py)
and writes the copy with its hints removed (tools/unpin.py) to
DIR/iso/FUNC.c. The rows `FUNC<TAB>DIR/iso/FUNC.c<TAB>UNIT.c` are input for
tools/autosolve.py, which solves the unpinned copy as is.

`apply` reads autosolve's results.tsv. For each MATCHES or TRANSLATED row it
replaces the function's definition in its unit with the matching one,
removes the concession marker when no hints are left in the unit, runs
clang-format and tools/check.py on the unit, and keeps the change only if
the unit still matches. With --commit, each kept function is one commit.
"""

import argparse
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import unpin  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MARK = unpin.MARK
DEF = re.compile(r"^(?:static\s+)?[A-Za-z_][\w \*]*?\b(\w+)\s*\([^;{]*\)\s*\{", re.S | re.M)
HINTS = (unpin.PIN, unpin.BARRIER_INLINE)


def blank(m):
    return re.sub(r"[^\n]", " ", m.group(0))


def definitions(text):
    """(name, text) for each function definition outside NONMATCHING drafts."""
    plain = unpin.DRAFT.sub(blank, text)
    plain = re.sub(r"//[^\n]*|/\*.*?\*/|\"(?:\\.|[^\"\\\n])*\"|'(?:\\.|[^'\\\n])*'", blank, plain, flags=re.S)
    out, pos = [], 0
    for m in DEF.finditer(plain):
        if m.start() < pos:
            continue
        depth, i = 0, m.end() - 1
        while i < len(plain):
            depth += {"{": 1, "}": -1}.get(plain[i], 0)
            i += 1
            if depth == 0:
                break
        out.append((m.group(1), text[m.start():i]))
        pos = i
    return out


def hinted(item):
    return any(p.search(item) for p in HINTS)


def cmd_list(a):
    units = a.units or sorted(
        os.path.relpath(os.path.join(d, f), ROOT)
        for d, _, fs in os.walk(os.path.join(ROOT, "src")) for f in fs if f.endswith(".c"))
    iso = os.path.join(a.out, "iso")
    os.makedirs(iso, exist_ok=True)
    rows = []
    for unit in units:
        text = open(os.path.join(ROOT, unit)).read()
        if MARK not in text:
            continue
        for name, item in definitions(text):
            if not hinted(item):
                continue
            dst = os.path.join(iso, name + ".c")
            subprocess.run([sys.executable, os.path.join(ROOT, "tools/isolate.py"), unit, name, dst],
                           cwd=ROOT, check=True)
            text = unpin.strip_code(open(dst).read())
            open(dst, "w").write(text)
            rows.append((name, os.path.relpath(dst, ROOT), unit, str(len(unpin.PIN.findall(item)))))
    with open(os.path.join(a.out, "list.tsv"), "w") as f:
        for r in rows:
            f.write("\t".join(r) + "\n")
    print(f"{len(rows)} hinted functions in {len(set(r[2] for r in rows))} units -> {a.out}/list.tsv")


def check(unit):
    r = subprocess.run([sys.executable, os.path.join(ROOT, "tools/check.py"), unit, "--quiet"],
                       cwd=ROOT, capture_output=True, text=True)
    return r.returncode == 0


def splice(unit, func, src):
    """Replace FUNC's definition in UNIT with the one in SRC; returns the old text or None."""
    path = os.path.join(ROOT, unit)
    old = open(path).read()
    new_def = dict(definitions(open(src).read())).get(func)
    old_def = dict(definitions(old)).get(func)
    if not new_def or not old_def or old.count(old_def) != 1:
        return None
    text = old.replace(old_def, new_def.strip("\n"))
    if not any(p.search(unpin.DRAFT.sub("", text)) for p in HINTS):
        text = re.sub(r"^" + re.escape(MARK) + r"[^\n]*\n", "", text, flags=re.M)
    open(path, "w").write(text)
    fmt = os.path.join(ROOT, "build/venv/bin/clang-format")
    subprocess.run([fmt if os.path.exists(fmt) else "clang-format", "-i", path], cwd=ROOT)
    return old


def cmd_apply(a):
    units = {}
    lst = os.path.join(os.path.dirname(a.results), "..", "list.tsv")
    for ln in open(a.list or lst):
        p = ln.rstrip("\n").split("\t")
        units[p[0]] = p[2]
    seen = set()
    for ln in open(a.results):
        p = ln.rstrip("\n").split("\t")
        if len(p) < 7 or p[3] not in ("MATCHES", "TRANSLATED") or p[0] in seen:
            continue
        func, src = p[0], p[6]
        seen.add(func)
        unit = units.get(func)
        if not unit:
            print(f"{func}: not in the list")
            continue
        old = splice(unit, func, src)
        if old is None:
            print(f"{func}: could not splice into {unit}")
            continue
        if not check(unit):
            open(os.path.join(ROOT, unit), "w").write(old)
            print(f"{func}: {unit} no longer matches after splicing; reverted")
            continue
        how = "solver translation" if p[3] == "TRANSLATED" else "pins removed"
        print(f"{func}: applied to {unit} ({how})")
        if a.commit:
            msg = f"{func}: drop register pins ({how})\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>\n"
            subprocess.run(["git", "commit", "-q", "-m", msg, unit], cwd=ROOT, check=True)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    li = sub.add_parser("list")
    li.add_argument("units", nargs="*")
    li.add_argument("--out", default=os.path.join(ROOT, "wip", "farm"))
    ap_ = sub.add_parser("apply")
    ap_.add_argument("results")
    ap_.add_argument("--list", help="list.tsv written by `list` (default: next to the results' parent)")
    ap_.add_argument("--commit", action="store_true")
    a = ap.parse_args()
    cmd_list(a) if a.cmd == "list" else cmd_apply(a)


if __name__ == "__main__":
    main()
