#!/usr/bin/env python3
"""isolate.py UNIT.c FUNC OUT.c [--nonmatching]: copy one function out of a unit.

Keeps the unit's includes, types, globals and static helpers; every other
non-static function definition becomes a prototype and every other
INCLUDE_ASM line is dropped, so tools/check.py, tools/regdiag.py and
tools/permute.py see a file with one ROM function. With --nonmatching, FUNC's
`#ifdef NONMATCHING` draft is selected (other drafts are dropped).
Multi-name declarations (`int a, b;`) are split, which decomp-permuter needs.
"""
import re
import sys

from merge_unit import items

DEF = re.compile(r"^(static\s+)?([A-Za-z_][\w \*]*?\b(\w+)\s*\([^;{]*\))\s*\{", re.S)


def strip_comments(item):
    return re.sub(r"^(\s*(//[^\n]*|/\*.*?\*/)\s*)+", "", item, flags=re.S)


def select_drafts(text, func, nonmatching):
    """Resolve #ifdef NONMATCHING blocks: FUNC's draft or fallback, others' fallback."""
    block = re.compile(r"#\s*ifdef\s+NONMATCHING[^\n]*\n(.*?)#\s*else\s*\n(.*?)#\s*endif[^\n]*\n", re.S)

    def pick(m):
        draft, fallback = m.group(1), m.group(2)
        mine = re.search(r"\b" + re.escape(func) + r"\b", fallback)
        if mine and nonmatching:
            return draft
        if not mine:
            # another function's draft: keep only its prototype, if any
            d = DEF.search(strip_comments(draft))
            return d.group(2) + ";\n" if d and not d.group(1) else ""
        return fallback
    return block.sub(pick, text)


def split_decls(text):
    def split(m):
        ind, typ, names = m.group(1), m.group(2), m.group(3)
        return "".join(f"{ind}{typ} {n.strip()};\n" for n in names.split(","))
    return re.sub(r"^(\s+)((?:const\s+)?(?:struct \w+|unsigned \w+|s32|u32|u16|u8|s16|s8|int|bool8|bool32))\s+"
                  r"(\w+(?:\s*,\s*\w+)+);\n", split, text, flags=re.M)


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    nonmatching = "--nonmatching" in sys.argv
    unit, func, out = args
    text = select_drafts(open(unit).read(), func, nonmatching)
    kept = []
    for item in items(text):
        code = strip_comments(item)
        inc = re.match(r"\s*INCLUDE_ASM\([^,]*,\s*(\w+)\)", code)
        if inc:
            if inc.group(1) == func:
                kept.append(item)
            continue
        m = DEF.match(code)
        if m and not m.group(1) and m.group(3) != func:
            kept.append(m.group(2).strip() + ";")
            continue
        kept.append(item)
    open(out, "w").write(split_decls("\n".join(kept) + "\n"))


if __name__ == "__main__":
    main()
