#!/usr/bin/env python3
"""isolate.py UNIT.c FUNC OUT.c [--nonmatching]: copy one function out of a unit.

Keeps the unit's includes, types, globals and static helpers; every other
non-static function definition becomes a prototype and every other
INCLUDE_ASM line is dropped, so tools/check.py, tools/regdiag.py and
tools/permute.py see a file with one ROM function. With --nonmatching, FUNC's
`#ifdef NONMATCHING` draft is selected (other drafts are dropped).
Multi-name declarations (`int a, b;`) are split, which decomp-permuter needs.

Library use: `ensure(src, func)` returns SRC unchanged when it holds one ROM
function, else writes the isolated copy (draft selected) to
build/isolated/UNIT__FUNC.c and returns that repo-relative path. permute.py,
rasolve.py and translate.py call it, so they accept a function inside a
multi-function unit directly (line numbers then refer to the isolated copy).
"""
import os
import re
import sys

from merge_unit import items

DEF = re.compile(r"^((?:inline\s+)?static\s+)?([A-Za-z_][\w \*]*?\b(\w+)\s*\([^;{]*\))\s*\{", re.S)


def strip_comments(item):
    return re.sub(r"^(\s*(//[^\n]*|/\*.*?\*/)\s*)+", "", item, flags=re.S)


def select_drafts(text, func, nonmatching):
    """Resolve #ifdef NONMATCHING blocks: FUNC's draft or fallback, others' fallback."""
    al = dict(re.findall(r"^#define\s+(\w+)\s+(sub_[0-9A-F]{8})\s*$", text, re.M))
    block = re.compile(r"#\s*ifdef\s+NONMATCHING[^\n]*\n(.*?)#\s*else\s*\n(.*?)#\s*endif[^\n]*\n", re.S)

    def pick(m):
        draft, fallback = m.group(1), m.group(2)
        names = {func} | {n for n, a in al.items() if a == func or n == func} | {a for n, a in al.items() if n == func}
        mine = any(re.search(r"\b" + re.escape(n) + r"\b", fallback) for n in names)
        if mine and nonmatching:
            return draft
        if not mine:
            # another function's draft: keep only its prototype, if any
            # (unless the unit declares it already: a draft's signature may
            # differ from the declaration the matched code uses)
            d = DEF.search(strip_comments(draft))
            if not d or d.group(1):
                return ""
            declared = re.compile(r"\b" + re.escape(d.group(3)) + r"\s*\([^;{}]*\)\s*;")
            return "" if declared.search(text) else d.group(2) + ";\n"
        return fallback
    return block.sub(pick, text)


def split_decls(text):
    def split(m):
        ind, typ, names = m.group(1), m.group(2), m.group(3)
        return "".join(f"{ind}{typ} {n.strip()};\n" for n in names.split(","))
    return re.sub(r"^(\s+)((?:const\s+)?(?:struct \w+|unsigned \w+|s32|u32|u16|u8|s16|s8|int|bool8|bool32))\s+"
                  r"(\w+(?:\s*,\s*\w+)+);\n", split, text, flags=re.M)


def isolate(text, func, out, nonmatching):
    text = select_drafts(text, func, nonmatching)
    # `#define Name sub_X` aliases: Name defines the same ROM function as sub_X
    alias = dict(re.findall(r"^#define\s+(\w+)\s+(sub_[0-9A-F]{8})\s*$", text, re.M))
    kept = []
    for item in items(text):
        code = strip_comments(item)
        inc = re.match(r"\s*INCLUDE_ASM\([^,]*,\s*(\w+)\)", code)
        if inc:
            if alias.get(inc.group(1), inc.group(1)) == alias.get(func, func):
                kept.append(item)
            continue
        m = DEF.match(code)
        if m and not m.group(1) and alias.get(m.group(3), m.group(3)) != alias.get(func, func):
            kept.append(m.group(2).strip() + ";")
            continue
        kept.append(item)
    with open(out, "w") as stream:
        stream.write(split_decls("\n".join(kept) + "\n"))


def ensure(src, func):
    """Repo-relative path of a file holding only FUNC (a name or its sub_ address)."""
    import split
    from romlib import ROOT
    path = src if os.path.isabs(src) else os.path.join(ROOT, src)
    text = open(path).read()
    names = split.c_functions(path)
    # a `#ifdef NONMATCHING` draft of FUNC (or of its alias): tools want the draft, not the INCLUDE_ASM
    aliases = [n for n, a in re.findall(r"^#define\s+(\w+)\s+(sub_[0-9A-F]{8})\s*$", text, re.M) if a == func]
    has_draft = any(f in dict(split.nonmatching_functions(path)) for f in [func] + aliases)
    if len(names) <= 1 and not has_draft:
        return os.path.relpath(path, ROOT)
    # `#define Name sub_XXXXXXXX` aliases: the source may spell FUNC either way
    spell = [func] + [n for n, a in re.findall(r"^#define\s+(\w+)\s+(sub_[0-9A-F]{8})\s*$", text, re.M) if a == func]
    name = next((n for n in spell if re.search(r"\b" + n + r"\b", text)), func)
    out = os.path.join(ROOT, "build", "isolated", f"{os.path.basename(path)[:-2]}__{func}.c")
    os.makedirs(os.path.dirname(out), exist_ok=True)
    isolate(text, name, out, True)
    return os.path.relpath(out, ROOT)


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    nonmatching = "--nonmatching" in sys.argv
    unit, func, out = args
    isolate(open(unit).read(), func, out, nonmatching)


if __name__ == "__main__":
    main()
