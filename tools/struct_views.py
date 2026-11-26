#!/usr/bin/env python3
"""Lay several struct types side by side by offset, as agbcc places them.

  struct_views.py "struct A" "struct B" ... [--include h1.h,h2.h] [--from 0x98 --to 0x2B0]
                                            [--markdown] [--json FILE]

The decomp often declares one object many times: a full view in a header, narrower
views in other headers, private copies in units. Before merging them into one type,
this shows every member of every view at its real offset (from layoutmerge's compiler
probes, so packing, bitfields and agbcc's size rounding are exact) and flags each
byte range where views disagree: a different width, a different signedness, a member
that starts inside another view's member, or a name that differs.

The headers to include default to every header that defines one of the types.
"""
import argparse
import collections
import json
import os
import re
import subprocess
import sys

TOOLS = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(TOOLS)
sys.path.insert(0, TOOLS)
import layoutmerge  # noqa: E402  (re-execs into build/venv for pycparser)


def defining_header(spell):
    tag = spell.split()[-1]
    out = subprocess.run(["git", "grep", "--untracked", "-l", "-P", rf"^{spell.split()[0]} {tag}\s*\{{", "--", "include", "src"],
                         cwd=ROOT, capture_output=True, text=True).stdout.split()
    if not out:
        sys.exit(f"no definition of {spell}")
    return out[0]


def probe(types, includes):
    """{spell: {'size', 'file', 'leaves': [...]}} for each type."""
    path = os.path.join(ROOT, "build", "struct_views_probe.c")
    lines = ['#include "engine/shared/global.h"']
    lines += [f'#include "{os.path.relpath(h, "include")}"' if h.startswith("include/") else f'#include "../{h}"' for h in includes]
    with open(path, "w") as f:
        f.write("\n".join(lines) + "\n")
    text = layoutmerge.preprocess(path)
    decl = "\n".join(f"extern {t} *__view_{i};" for i, t in enumerate(types))
    res = layoutmerge._scan_tu(path, text + "\n" + decl + "\n", {f"__view_{i}" for i in range(len(types))}) \
        if text is not None else {"error": "preprocess"}
    if "error" in res:
        if len(types) == 1:
            sys.exit(f"{types[0]}: {res['error']}")
        # headers that cannot share a unit: probe each type with its own header
        out = {}
        for t in types:
            out.update(probe([t], [defining_header(t)]))
        return out
    out = {}
    for v in res["structs"].values():
        if v["spell"] in types:
            out[v["spell"]] = {"size": v["size"], "file": v["file"], "leaves": v["leaves"]}
    return out


def is_pad(name):
    """unknownXX / unkXX / padXX members say nothing about the bytes."""
    last = name.split(".")[-1].split("[")[0]
    return bool(re.match(r"(unknown|unk|pad|reserved|_)[0-9A-Fa-fx_]*$", last))


def leaf_kind(l):
    return (l["width"], l["signed"], l["base"], l["bits"])


def compare(views, lo=0, hi=None):
    """Rows by offset: each view's leaf starting there, plus disagreement notes."""
    starts = collections.defaultdict(dict)
    for spell, v in views.items():
        for l in v["leaves"]:
            if l["off"] < lo or (hi is not None and l["off"] >= hi) or is_pad(l["name"]):
                continue
            starts[l["off"]].setdefault(spell, l)
    rows = []
    for off in sorted(starts):
        here = starts[off]
        notes = []
        kinds = {leaf_kind(l) for l in here.values()}
        if len({(k[0], k[3]) for k in kinds}) > 1:
            notes.append("width")
        elif len({k[1] for k in kinds if k[2] == "int"}) > 1:
            notes.append("signedness")
        names = {l["name"].split(".")[-1] for l in here.values()}
        if len(names) > 1:
            notes.append("name")
        # a member of another view covers this offset without starting here
        for spell, v in views.items():
            if spell in here:
                continue
            for l in v["leaves"]:
                if l["off"] < off < l["off"] + l["size"] and not is_pad(l["name"]) and not l["array"]:
                    notes.append(f"inside {spell.split()[-1]}.{l['name']}")
                    break
        rows.append((off, here, notes))
    return rows


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("types", nargs="+")
    ap.add_argument("--include", help="comma-separated headers (default: each type's defining header)")
    ap.add_argument("--from", dest="lo", type=lambda s: int(s, 0), default=0)
    ap.add_argument("--to", dest="hi", type=lambda s: int(s, 0))
    ap.add_argument("--markdown", action="store_true")
    ap.add_argument("--json", metavar="FILE")
    ap.add_argument("--only-disagreements", action="store_true")
    a = ap.parse_args()
    types = [t if t.startswith(("struct ", "union ")) else "struct " + t for t in a.types]
    includes = a.include.split(",") if a.include else sorted({defining_header(t) for t in types})
    views = probe(types, includes)
    missing = [t for t in types if t not in views]
    if missing:
        sys.exit(f"not found after preprocessing: {missing}")
    rows = compare(views, a.lo, a.hi)
    if a.json:
        with open(a.json, "w") as f:
            json.dump({"views": {t: {"size": v["size"], "file": v["file"]} for t, v in views.items()},
                       "rows": [{"off": o, "members": {t.split()[-1]: {"name": l["name"], "ctype": l["ctype"]}
                                                       for t, l in h.items()}, "notes": n} for o, h, n in rows]},
                      f, indent=1)
    short = [t.split()[-1] for t in types]
    if a.markdown:
        print("| off | " + " | ".join(short) + " | disagreement |")
        print("|---|" + "---|" * len(short) + "---|")
    else:
        for t in types:
            print(f"{t}: {views[t]['size']:#x} bytes ({views[t]['file']})")
    for off, here, notes in rows:
        if a.only_disagreements and not notes:
            continue
        cells = []
        for t in types:
            l = here.get(t)
            cells.append(f"{l['ctype']} {l['name']}" if l else "")
        if a.markdown:
            print(f"| {off:#05x} | " + " | ".join(c.replace("|", "\\|") for c in cells) + f" | {', '.join(notes)} |")
        else:
            print(f"{off:#06x}  " + "  ;  ".join(f"{s}: {c}" for s, c in zip(short, cells) if c)
                  + (f"   <-- {', '.join(notes)}" if notes else ""))


if __name__ == "__main__":
    main()
