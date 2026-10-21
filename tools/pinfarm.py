#!/usr/bin/env python3
"""Remove register pins from matched code with the register solver.

  pinfarm.py list [UNIT.c ...] [--out DIR] [--shards N]   write DIR/list.tsv (default wip/farm)
  pinfarm.py apply RESULTS.tsv [--commit]      splice matches into src/

`list` finds every function in a unit marked
`// MATCHING_CONCESSION: register-pinning` (default: all of src/) whose body
still holds a pin or an empty asm() barrier, isolates it (tools/isolate.py)
and writes the copy with its hints removed (tools/unpin.py) to
DIR/iso/NAME.c. The rows `SYMBOL<TAB>DIR/iso/NAME.c<TAB>UNIT.c<TAB>PINS<TAB>NAME`
(SYMBOL differs from the C name when a `#define` renames the function) are
input for tools/autosolve.py, which solves the unpinned copy as is.

`apply` reads autosolve's results.tsv. For each MATCHES or TRANSLATED row it
replaces the function's definition in its unit with the matching one,
removes the concession marker when no hints are left in the unit, runs
clang-format and tools/check.py on the unit, and keeps the change only if
the unit still matches. With --commit, each kept function is one commit.
"""

import argparse
import difflib
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import unpin  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MARK = unpin.MARK
DEF = re.compile(r"^(static\s+)?[A-Za-z_][\w \*]*?\b(\w+)\s*\([^;{]*\)\s*\{", re.S | re.M)
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
        if not m.group(1):  # static helpers are not ROM functions
            out.append((m.group(2), text[m.start():i]))
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
            # the solver needs the ROM symbol when a macro renames the function
            alias = re.search(r"^#define\s+" + name + r"\s+(\w+)\s*$", text, re.M)
            sym = alias.group(1) if alias else name
            rows.append((sym, os.path.relpath(dst, ROOT), unit, str(len(unpin.PIN.findall(item))), name))
    with open(os.path.join(a.out, "list.tsv"), "w") as f:
        for r in rows:
            f.write("\t".join(r) + "\n")
    for k in range(a.shards if a.shards > 1 else 0):
        with open(os.path.join(a.out, f"list-{k}.tsv"), "w") as f:
            for r in rows[k::a.shards]:
                f.write("\t".join(r) + "\n")
    print(f"{len(rows)} hinted functions in {len(set(r[2] for r in rows))} units -> {a.out}/list.tsv")


def check(unit):
    r = subprocess.run([sys.executable, os.path.join(ROOT, "tools/check.py"), unit, "--quiet"],
                       cwd=ROOT, capture_output=True, text=True)
    return r.returncode == 0


def splice(unit, func, src, sym=None):
    """Replace FUNC's definition in UNIT with the one in SRC (where it may be
    called SYM: translate.py's output is preprocessed); returns the old text or None."""
    path = os.path.join(ROOT, unit)
    old = open(path).read()
    defs = dict(definitions(open(src).read()))
    new_def = defs.get(func)
    if new_def is None and sym in defs:
        new_def = re.sub(r"\b" + sym + r"(\s*\()", func + r"\1", defs[sym], count=1)
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


def tidy(unit, name, orig):
    """Undo each hunk of the translation that the match does not need: the
    permuter re-spells lines (parentheses, braces) along with the real edit."""
    path = os.path.join(ROOT, unit)
    want = orig.split("\n")
    while True:
        text = open(path).read()
        cur = dict(definitions(text))[name]
        have = cur.split("\n")
        ops = [o for o in difflib.SequenceMatcher(None, have, want, autojunk=False).get_opcodes()
               if o[0] != "equal"]
        cands = []
        for tag, i1, i2, j1, j2 in ops:
            cands.append(have[:i1] + want[j1:j2] + have[i2:])
            # a line re-spelled inside a larger hunk (one that also added braces)
            for i in range(i1, i2):
                near = difflib.get_close_matches(have[i], want[j1:j2], 1, 0.8)
                if near and near[0] != have[i]:
                    cands.append(have[:i] + near + have[i + 1:])
        for cand in cands:
            open(path, "w").write(text.replace(cur, "\n".join(cand)))
            if check(unit):
                break
        else:
            open(path, "w").write(text)
            return


def find_unit(name):
    r = subprocess.run(["git", "grep", "-l", "-w", "-F", name, "--", "src/*.c"],
                       cwd=ROOT, capture_output=True, text=True)
    for unit in r.stdout.split():
        if name in dict(definitions(open(os.path.join(ROOT, unit)).read())):
            return unit
    return None


def current_name(iso_path):
    """The function's name now in symbols/functions.csv, by its ROM address
    (tools/check.py on the listed copy says where it is)."""
    r = subprocess.run([sys.executable, os.path.join(ROOT, "tools/check.py"), iso_path],
                       cwd=ROOT, capture_output=True, text=True)
    m = re.search(r"\(\d+ bytes at 0x([0-9A-Fa-f]{8})\)", r.stdout + r.stderr)
    addr = m.group(1).upper() if m else old_address(os.path.splitext(os.path.basename(iso_path))[0])
    if not addr:
        return None
    for ln in open(os.path.join(ROOT, "symbols/functions.csv")):
        if ln.upper().startswith(addr + ","):
            return ln.split(",")[1]
    return None


def old_address(name):
    """NAME's address in an earlier symbols/functions.csv (renamed since)."""
    if re.fullmatch(r"sub_[0-9A-F]{8}", name):
        return name[4:]
    revs = subprocess.run(["git", "log", "--format=%H", "-n", "60", "--", "symbols/functions.csv"],
                          cwd=ROOT, capture_output=True, text=True).stdout.split()
    for rev in revs:
        text = subprocess.run(["git", "show", f"{rev}:symbols/functions.csv"], cwd=ROOT,
                              capture_output=True, text=True).stdout
        m = re.search(r"^([0-9A-F]{8})," + re.escape(name) + ",", text, re.M)
        if m:
            return m.group(1)
    return None


def renamed_copy(src, *names):
    """SRC with the function's old names (symbol, C name) replaced by the new one."""
    sym, name, new, results = names
    text = open(src).read()
    for old in {sym, name}:
        text = re.sub(r"\b" + re.escape(old) + r"\b", new, text)
    dst = os.path.join(os.path.dirname(os.path.abspath(results)), "renamed-" + new + ".c")
    open(dst, "w").write(text)
    return dst


def cmd_apply(a):
    units, iso = {}, {}
    lst = os.path.join(os.path.dirname(a.results), "..", "list.tsv")
    for ln in open(a.list or lst):
        p = ln.rstrip("\n").split("\t")
        units[p[0]] = (p[2], p[4] if len(p) > 4 else p[0])
        iso[p[0]] = p[1]
    seen = set()
    for ln in open(a.results):
        p = ln.rstrip("\n").split("\t")
        if len(p) < 7 or p[3] not in ("MATCHES", "TRANSLATED") or p[0] in seen:
            continue
        func, src = p[0], p[6]
        seen.add(func)
        if func not in units:
            print(f"{func}: not in the list")
            continue
        unit, name = units[func]
        if not os.path.exists(os.path.join(ROOT, unit)) or name not in dict(definitions(open(os.path.join(ROOT, unit)).read())):
            # moved (or moved and renamed) since the list was made
            found = find_unit(name)
            if not found:
                new = current_name(iso[func])
                found = find_unit(new) if new else None
                if found:
                    src = renamed_copy(src, func, name, new, a.results)
                    func, name = new, new
            if not found:
                print(f"{name}: no longer found in src/; skipped")
                continue
            unit = found
        current = dict(definitions(open(os.path.join(ROOT, unit)).read())).get(name)
        if current is not None and not hinted(current):
            print(f"{name}: already free of hints in {unit}; skipped")
            continue
        old = splice(unit, name, src, func)
        if old is None:
            print(f"{func}: could not splice into {unit}")
            continue
        if open(os.path.join(ROOT, unit)).read() == old:
            print(f"{name}: already applied")
            continue
        if not check(unit):
            open(os.path.join(ROOT, unit), "w").write(old)
            print(f"{func}: {unit} no longer matches after splicing; reverted")
            continue
        if p[3] == "TRANSLATED":
            tidy(unit, name, unpin.strip_code(dict(definitions(old))[name]))
        if subprocess.run(["git", "diff", "--quiet", "--", unit], cwd=ROOT).returncode == 0:
            print(f"{name}: already applied")
            continue
        how = "solver translation" if p[3] == "TRANSLATED" else "pins removed"
        print(f"{name}: applied to {unit} ({how})")
        if a.commit:
            msg = f"{name}: drop register pins ({how})\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>\n"
            subprocess.run(["git", "commit", "-q", "-m", msg, unit], cwd=ROOT, check=True)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    li = sub.add_parser("list")
    li.add_argument("units", nargs="*")
    li.add_argument("--out", default=os.path.join(ROOT, "wip", "farm"))
    li.add_argument("--shards", type=int, default=1, help="also write list-K.tsv for N parallel workers")
    ap_ = sub.add_parser("apply")
    ap_.add_argument("results")
    ap_.add_argument("--list", help="list.tsv written by `list` (default: next to the results' parent)")
    ap_.add_argument("--commit", action="store_true")
    a = ap.parse_args()
    cmd_list(a) if a.cmd == "list" else cmd_apply(a)


if __name__ == "__main__":
    main()
