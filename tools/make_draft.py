#!/usr/bin/env python3
"""make_draft.py CANDIDATE.c FUNC OUT.c NOTE: wrap a near-miss as a NONMATCHING draft.

Keeps the candidate's leading #include lines outside the guard, puts the rest
under `#ifdef NONMATCHING` with a `// fuzzy:` line (filled in by
tools/check.py) and NOTE, and falls back to INCLUDE_ASM.
"""
import re
import sys

cand, func, out, note = sys.argv[1:5]
text = open(cand).read()
lines = text.splitlines()
head = []
while lines and (lines[0].startswith("#include") or not lines[0].strip()):
    head.append(lines.pop(0))
body = "\n".join(lines).strip("\n")
open(out, "w").write("\n".join(head) + "\n#ifdef NONMATCHING\n// fuzzy: 0%\n// " + note + "\n" + body +
                     f'\n#else\nINCLUDE_ASM("asm/nonmatching", {func});\n#endif\n')
