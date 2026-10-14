#!/usr/bin/env python3
"""promote_draft.py FILE.c FUNC: turn FUNC's `#ifdef NONMATCHING` draft into plain C.

Drops the `#ifdef NONMATCHING`, `// fuzzy:` and nonmatching notes, and the
`#else INCLUDE_ASM(...) #endif` fallback for the block that defines FUNC.
"""
import re
import sys

path, func = sys.argv[1], sys.argv[2]
text = open(path).read()
block = re.compile(r"#\s*ifdef\s+NONMATCHING[^\n]*\n(.*?)#\s*else\s*\n\s*INCLUDE_ASM\([^)]*\b" + re.escape(func) +
                   r"\b[^)]*\);\s*\n#\s*endif[^\n]*\n", re.S)
m = block.search(text)
if not m:
    sys.exit(f"no NONMATCHING block for {func} in {path}")
body = re.sub(r"^//\s*fuzzy:[^\n]*\n", "", m.group(1))
body = re.sub(r"^// \d+ differing instruction runs[^\n]*\n", "", body)
open(path, "w").write(text[:m.start()] + body + text[m.end():])
print(f"promoted {func} in {path}")
