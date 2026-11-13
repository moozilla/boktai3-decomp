#!/usr/bin/env python3
"""Merge contiguous single-function C files into one translation unit.

  merge_unit.py OUT.c IN.c... [--write]      (inputs in ROM order)

Concatenates the inputs' top-level items. Identical repeats (prototypes,
externs, struct definitions, static helpers, #defines) are kept once; items
whose name repeats with a different body are reported as conflicts, which must
be unified (usually into a shared header) before merging. Inputs must also use
the same compiler and effective flags; mixed settings cannot share one unit. With --write the
output is written and the inputs are removed from disk. An existing output must
be listed among the inputs, and inputs must name distinct files. Run make afterwards:
merging can change code generation only through such declarations.
"""
import argparse
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from split import c_functions  # noqa: E402
from build import cflags_for, compiler_for  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


# Ignore punctuation inside literals and comments when measuring C nesting.
_NONCODE = re.compile(r""""(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|/\*.*?\*/|//(?:\\\n|[^\n])*""", re.S)


def items(text):
    """Top-level items: preprocessor lines, comments and declarations/definitions."""
    out, cur, depth = [], [], 0
    lines = text.split("\n")
    code_lines = _NONCODE.sub(lambda m: " " + "\n" * m[0].count("\n"), text).split("\n")
    i = 0
    has_code = False
    while i < len(lines):
        line, code = lines[i], code_lines[i]
        i += 1
        while line.rstrip().endswith("\\") and i < len(lines):  # one logical line
            line += "\n" + lines[i]
            code += "\n" + code_lines[i]
            i += 1
        if not cur and not line.strip():
            continue
        if not cur and code.lstrip().startswith("#"):
            out.append(line)
            continue
        cur.append(line)
        if code.lstrip().startswith("#"):
            continue  # a directive inside a definition doesn't change nesting
        depth += code.count("{") + code.count("(") - code.count("}") - code.count(")")
        has_code = has_code or bool(code.strip())
        comment_end = not has_code and (line.lstrip().startswith("//") or line.rstrip().endswith("*/"))
        if depth == 0 and (code.rstrip().endswith((";", "}")) or comment_end):
            out.append("\n".join(cur))
            cur = []
            has_code = False
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
    settings = None
    settings_path = None
    for path in paths:
        current = (compiler_for(path), tuple(cflags_for(path)))
        if settings is None:
            settings, settings_path = current, path
        elif current[0] != settings[0]:
            names = f"{os.path.basename(settings[0])} / {os.path.basename(current[0])}"
            conflicts.append((("compiler", names), settings_path, path))
        elif current[1] != settings[1]:
            conflicts.append((("compiler flags", "different effective options"), settings_path, path))
        with open(os.path.join(ROOT, path)) as source:
            text = source.read()
        pinned = MARK in text
        for item in items(text):
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


def write_merge(out, inputs, text):
    """Write the complete output before removing inputs; leave Git to the caller."""
    output = os.path.abspath(os.path.join(ROOT, out))
    sources = [os.path.abspath(os.path.join(ROOT, path)) for path in inputs]
    output_identity = os.path.realpath(output)
    identities = [os.path.realpath(source) for source in sources]
    if len(identities) != len(set(identities)):
        raise ValueError("merge inputs must name distinct files")
    if os.path.exists(output) and output_identity not in identities:
        raise ValueError("existing output must be included among merge inputs: " + out)
    with open(output, "w") as stream:
        stream.write(text)
    for source, identity in zip(sources, identities):
        if identity != output_identity:
            os.unlink(source)


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
        try:
            write_merge(a.out, a.inputs, text)
        except ValueError as error:
            ap.error(str(error))


if __name__ == "__main__":
    main()
