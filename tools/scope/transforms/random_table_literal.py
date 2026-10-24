"""Source transform: write the random table as its fixed address, not the symbol.

gUnk_0203B400 (the 1024-entry u16 random table in EWRAM) becomes
((T *)0x0203B400), T being the declared element type. As a symbol the address
is its own pseudo from expand on; as an integer it is a constant that reload
loads, which changes reload's spill rotation (docs/REGALLOC.md). Applied to
preprocessed text, so declarations in headers and file-local macros are
covered. Use: SCOPE_TRANSFORM=tools/scope/transforms/random_table_literal.py
"""
import re

SYM = "gUnk_0203B400"


def transform(text):
    m = re.search(r"extern\s+((?:const\s+)?\w+)\s+" + SYM + r"\s*\[", text)
    if not m:
        return text
    elem = m.group(1)
    out = []
    for line in text.split("\n"):
        if re.search(r"\bextern\b[^;]*\b" + SYM + r"\b", line):
            out.append(line)
        else:
            out.append(re.sub(r"\b" + SYM + r"\b", f"(({elem} *)0x0203B400)", line))
    return "\n".join(out)
