#!/usr/bin/env python3
"""Write a tool's result for one function back into its translation unit.

  writeback.py UNIT.c FUNC RESULT.c [--draft] [--fuzzy PCT] [--note TEXT]

RESULT.c is a whole source that defines FUNC, as tools/fix.py (build/fix/FUNC.c),
tools/sweep.py (build/sweep/FUNC.c) and tools/replay.py write them: usually the
isolated copy of the unit (tools/isolate.py) with the function rewritten and,
sometimes, new or edited static inline helpers. The function's definition
replaces FUNC's draft inside its `#ifdef NONMATCHING` block in UNIT.c; helpers
that RESULT.c defines and UNIT.c does not (or defines differently) go into the
block just before the function, renamed when the unit already uses the name.
Then the block is promoted to plain C (a match), or with --draft kept as a
draft with a new `// fuzzy:` line and note. The file is formatted with
clang-format. Check the unit afterwards with tools/check.py.
"""
import argparse
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import harvest  # noqa: E402
from romlib import ROOT  # noqa: E402

BLOCK = (r"^#\s*ifdef\s+NONMATCHING[^\n]*\n((?:(?!^#\s*(?:ifdef\s+NONMATCHING|else)\b).)*?)^#\s*else[^\n]*\n"
         r"((?:(?!^#\s*endif).)*?INCLUDE_ASM\([^)]*\b{}\b[^)]*\);(?:(?!^#\s*endif).)*?)^#\s*endif[^\n]*\n?")


def helpers_of(text):
    return {n: d for n, d in harvest.definitions(text).items() if "inline" in d.split("(")[0]}


def writeback(unit, func, result, draft=False, fuzzy=None, note=None):
    text = open(unit).read()
    res = open(result).read()
    alias = dict((a, n) for n, a in harvest.ALIAS.findall(text))
    names = [func] + ([alias[func]] if func in alias else []) + [n for n, a in harvest.ALIAS.findall(text) if n == func]
    m = None
    for n in names:
        m = re.search(BLOCK.format(re.escape(n)), text, re.S | re.M)
        if m:
            break
    if not m:
        sys.exit(f"no NONMATCHING block for {func} in {unit}")
    body, fallback = m.group(1), m.group(2)
    defs = harvest.definitions(res)
    src_name = next((n for n in [alias.get(func)] + names if n and n in defs), None)
    if src_name is None:
        sys.exit(f"{result} does not define {func}")
    new_fn = defs[src_name]
    old = harvest.definitions(body)
    old_name = next((n for n in names + [alias.get(func)] if n and n in old), None)
    if old_name is None:
        sys.exit(f"the draft block does not define {func}")
    # helpers the result has and the unit lacks or spells differently
    unit_helpers = helpers_of(text)
    extra = []
    for n, d in helpers_of(res).items():
        if n in unit_helpers and re.sub(r"\s+", "", unit_helpers[n]) == re.sub(r"\s+", "", d):
            continue
        if n in unit_helpers or re.search(r"\b" + re.escape(n) + r"\s*\(", text):
            new = n + "_" + func.split("_")[-1]
            d = re.sub(r"\b" + re.escape(n) + r"\b", new, d, count=1)
            new_fn = re.sub(r"\b" + re.escape(n) + r"(\s*\()", new + r"\1", new_fn)
            n = new
        if re.search(r"\b" + re.escape(n) + r"\s*\(", new_fn):
            extra.append(d)
    if src_name != old_name:
        new_fn = re.sub(r"\b" + re.escape(src_name) + r"(\s*\()", old_name + r"\1", new_fn, count=1)
    i = body.index(old[old_name])
    body = body[:i] + "".join(e.strip() + "\n\n" for e in extra) + new_fn.strip() + "\n" + body[i + len(old[old_name]):]
    if draft:
        body = re.sub(r"^//\s*fuzzy:[^\n]*\n", "", body, flags=re.M)
        head = (f"// fuzzy: {fuzzy}%\n" if fuzzy else "") + (f"// {note}\n" if note else "")
        out = f"#ifdef NONMATCHING\n{head}{body}#else\n{fallback}#endif\n"
    else:
        body = re.sub(r"^//\s*fuzzy:[^\n]*\n", "", body, flags=re.M)
        body = re.sub(r"^// \d+ differing instruction runs[^\n]*\n", "", body, flags=re.M)
        out = body
    text = text[:m.start()] + out + text[m.end():]
    open(unit, "w").write(text)
    fmt = os.environ.get("CLANG_FORMAT", "clang-format")
    subprocess.run([fmt, "-i", unit], cwd=ROOT, check=False)
    print(f"{'updated draft' if draft else 'matched'} {func} in {os.path.relpath(unit, ROOT)}"
          + (f" (+{len(extra)} helper(s))" if extra else ""))


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("unit")
    ap.add_argument("func")
    ap.add_argument("result")
    ap.add_argument("--draft", action="store_true")
    ap.add_argument("--fuzzy")
    ap.add_argument("--note")
    a = ap.parse_args()
    writeback(a.unit, a.func, a.result, a.draft, a.fuzzy, a.note)


if __name__ == "__main__":
    main()
