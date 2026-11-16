#!/usr/bin/env python3
"""Replace a file's selector macros with its own copies of the inlines they select.

  unselect.py FILE.c [--prefix Mummy] [--macro SCENE_ACTOR_CHASE_] [--write]

A selector macro is a `#define X 1` a file sets before including a shared header so
that the header compiles a different body of some inline (`SCENE_ACTOR_CHASE_SCALAR_CELLS`
picks the scalar cell lookup). The project does not allow them: the choice is invisible
at the call, and it breaks as soon as the header is included earlier (in a merged unit,
or because merge_unit.py hoists includes above the define).

This preprocesses the file twice, with its selectors and without, and compares every
static inline function by name. Each inline whose code differs, and each header inline
that calls one of those, gets a copy in the file, renamed after the file's class
(`SceneActorChase_TryRoute` becomes `Mummy_TryRoute`) and placed after the includes; the
file's own code calls the copies, and the `#define`s go. The copies are the selected
code, as the preprocessor resolved it. Without --write it prints the plan. Check the
file afterwards (tools/check.py); clang-format runs on --write.
"""
import argparse
import csv
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import build  # noqa: E402

_INLINE = re.compile(r"\b(?:static\s+inline|inline\s+static)\b[^;{}()]*?\b(\w+)\s*\(")


def preprocess_text(text, path):
    with tempfile.NamedTemporaryFile("w", suffix=".c", dir=os.path.dirname(os.path.abspath(path)),
                                     delete=False) as f:
        f.write(text)
        tmp = f.name
    try:
        return build.preprocess(os.path.relpath(tmp, ROOT))
    finally:
        os.remove(tmp)


def inline_functions(code):
    """name -> full definition text, for every static inline function definition."""
    out = {}
    for m in _INLINE.finditer(code):
        start = m.start()
        i = code.find("{", m.end())
        semi = code.find(";", m.end())
        if i < 0 or (0 <= semi < i):
            continue  # a prototype
        depth, j = 0, i
        while j < len(code):
            depth += code[j] == "{"
            depth -= code[j] == "}"
            j += 1
            if depth == 0:
                break
        out.setdefault(m.group(1), code[start:j])
    return out


def norm(text):
    return " ".join(text.split())


def calls(text, names):
    return {n for n in names if re.search(r"\b%s\s*\(" % re.escape(n), text)}


def class_prefix(path):
    m = re.search(r"sub_([0-9A-Fa-f]{8})", path)
    rows = {r[0].upper(): r[1] for r in csv.reader(open(os.path.join(ROOT, "symbols/functions.csv"))) if r}
    if m and m.group(1).upper() in rows:
        return rows[m.group(1).upper()].split("_")[0]
    return os.path.splitext(os.path.basename(path))[0].split("_")[0].capitalize()


def plan(path, macro, prefix):
    text = open(path).read()
    sel = re.findall(r"^#define (%s\w+)\s+1\s*$" % re.escape(macro), text, re.M)
    if not sel:
        return text, None, sel
    plain = re.sub(r"^#define %s\w+\s+1\s*\n" % re.escape(macro), "", text, flags=re.M)
    with_sel = inline_functions(preprocess_text(text, path))
    without = inline_functions(preprocess_text(plain, path))
    own = inline_functions(text)  # the file's own inlines keep their names
    differ = {n for n, body in with_sel.items() if n not in own and norm(body) != norm(without.get(n, ""))}
    changed = True
    while changed:  # header inlines that call a copied one need copies too
        changed = False
        for n, body in with_sel.items():
            if n not in differ and n not in own and calls(body, differ):
                differ.add(n)
                changed = True
    # only what the file's own code reaches
    reach, todo = set(), [n for n in differ if re.search(r"\b%s\s*\(" % re.escape(n), plain)]
    while todo:
        n = todo.pop()
        if n in reach:
            continue
        reach.add(n)
        todo += [c for c in calls(with_sel[n], differ) if c not in reach]
    rename = {n: f"{prefix}_{re.sub(r'^SceneActorChase_', '', n)}" for n in reach}
    for new in rename.values():
        if re.search(r"\b%s\b" % re.escape(new), plain):
            raise SystemExit(f"{path}: {new} is already used in the file; pass another --prefix")
    return plain, rename, sel, with_sel


def apply(path, macro, prefix, write):
    res = plan(path, macro, prefix)
    if res[1] is None:
        print(f"{path}: no {macro}* selectors")
        return
    plain, rename, sel, with_sel = res
    print(f"{path}: selectors {', '.join(sel)}; copies: " +
          (", ".join(f"{o} -> {n}" for o, n in sorted(rename.items())) or "none"))
    if not write:
        return
    sub = lambda s: re.sub(r"\b(%s)\b(?=\s*\()" % "|".join(map(re.escape, rename)), lambda m: rename[m.group(1)], s) if rename else s
    order = sorted(rename, key=lambda n: with_sel_order(with_sel, n))
    copies = "".join(sub(with_sel[n]) + "\n\n" for n in order)
    incs = list(re.finditer(r"^#include .*\n", plain, re.M))
    pos = incs[-1].end() if incs else 0
    body = sub(plain[pos:])
    header = ("\n/* This class's own copies of the shared chase inlines it compiles differently. */\n"
              if copies else "")
    out = plain[:pos] + header + copies + body
    open(path, "w").write(out)
    subprocess.run([os.path.join(ROOT, "build/venv/bin/clang-format"), "-i", path], cwd=ROOT)


_ORDER = {}


def with_sel_order(with_sel, n):
    if not _ORDER:
        for i, k in enumerate(with_sel):
            _ORDER[k] = i
    return _ORDER.get(n, 1 << 30)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("files", nargs="+")
    ap.add_argument("--prefix")
    ap.add_argument("--macro", default="SCENE_ACTOR_CHASE_")
    ap.add_argument("--write", action="store_true")
    a = ap.parse_args()
    for f in a.files:
        _ORDER.clear()
        apply(f, a.macro, a.prefix or class_prefix(f), a.write)


if __name__ == "__main__":
    main()
