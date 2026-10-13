#!/usr/bin/env python3
"""Merge contiguous single-function C files into one translation unit.

  merge_unit.py OUT.c IN.c... [--write]      (inputs in ROM order)

Concatenates the inputs' top-level items. Identical repeats (prototypes,
externs, struct definitions, static helpers, #defines) are kept once; items
whose name repeats with a different body are reported as conflicts, which must
be unified (usually into a shared header) before merging. With --write the
output is written and the inputs are removed (git rm). Run make afterwards:
merging can change code generation only through such declarations.
"""
import argparse
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from split import c_functions  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def items(text):
    """Top-level items: preprocessor lines, comments and declarations/definitions."""
    out, cur, depth = [], [], 0
    for line in text.split("\n"):
        if not cur and not line.strip():
            continue
        if not cur and line.startswith("#"):
            out.append(line)
            continue
        cur.append(line)
        code = re.sub(r"//.*|\"(\\.|[^\"])*\"|'(\\.|[^'])*'", "", line)
        depth += code.count("{") + code.count("(") - code.count("}") - code.count(")")
        s = code.strip()
        if depth == 0 and (s.endswith(";") or s.endswith("}") or s.startswith("//") or s.endswith("*/")):
            out.append("\n".join(cur))
            cur = []
    if cur:
        out.append("\n".join(cur))
    return out


def key(item):
    """Name that must be unique in one TU, or None if repeats are legal C."""
    first = item.lstrip()
    m = re.match(r"#\s*define\s+(\w+)", first)
    if m:
        return ("define", m.group(1))
    if first.startswith("#") or first.startswith("//") or first.startswith("/*"):
        return None
    m = re.match(r"(?:typedef\s+)?(struct|union|enum)\s+(\w+)\s*\{", first)
    if m:
        t = re.search(r"\}\s*(\w+)\s*;\s*$", item) if first.startswith("typedef") else None
        return ("type", t.group(1) if t else m.group(1) + " " + m.group(2))
    m = re.match(r"typedef\b.*?(\w+)\s*(?:\[[^\]]*\])?\s*;\s*$", first, re.S)
    if m:
        return ("type", m.group(1))
    if "{" in item and re.match(r"static\b", first):
        m = re.search(r"(\w+)\s*\(", item)
        return ("static", m.group(1))
    return None


MARK = "// MATCHING_CONCESSION: register-pinning"


def is_function(item):
    return re.match(r"(?!static\b)[A-Za-z_][\w \*]*\b\w+\s*\([^;{]*\)\s*\{", item.lstrip(), re.S) is not None


def merge(paths):
    """A pinned input's marker moves to just above its function definitions."""
    seen, conflicts, body, includes = {}, [], [], []
    for path in paths:
        pinned = MARK in open(os.path.join(ROOT, path)).read()
        for item in items(open(os.path.join(ROOT, path)).read()):
            if item.startswith("#include"):
                if item not in includes:
                    includes.append(item)
                continue
            item = "\n".join(l for l in item.split("\n") if l.strip() != MARK).strip("\n")
            if not item:
                continue
            if pinned and is_function(item):
                item = MARK + "\n" + item
            k = key(item)
            if k is None:
                if item.endswith(";") and item in body:
                    continue  # repeated prototype/extern
                body.append(item)
            elif k not in seen:
                seen[k] = (item, path)
                body.append(item)
            elif seen[k][0] != item:
                conflicts.append((k, seen[k][1], path))
    return "\n".join(includes) + "\n\n" + "\n\n".join(body) + "\n", conflicts


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("out")
    ap.add_argument("inputs", nargs="+")
    ap.add_argument("--write", action="store_true")
    a = ap.parse_args()
    text, conflicts = merge(a.inputs)
    for (kind, name), first, other in conflicts:
        print(f"conflict: {kind} {name}: {first} vs {other}")
    if conflicts:
        sys.exit(1)
    names = [n for p in a.inputs for n in c_functions(os.path.join(ROOT, p))]
    print(f"{len(a.inputs)} files -> {a.out}: {len(names)} functions")
    if a.write:
        subprocess.run(["git", "rm", "-q"] + [p for p in a.inputs if os.path.abspath(p) != os.path.abspath(a.out)],
                       cwd=ROOT, check=True)
        open(os.path.join(ROOT, a.out), "w").write(text)


if __name__ == "__main__":
    main()
