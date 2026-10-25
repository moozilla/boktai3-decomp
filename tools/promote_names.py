#!/usr/bin/env python3
"""Promote `#define Name sub_XXXXXXXX` aliases in C sources to real names.

  promote_names.py [--dry-run]

Workers name functions with a file-local alias so they need not touch the
shared symbols/functions.csv (which renames labels in gen/). At integration
this tool moves every alias into symbols/functions.csv (confidence=low,
source=src-alias), rewrites each `sub_XXXXXXXX` reference in src/ and include/
and symbols/ram.ld to the name, and deletes the alias lines. `make` then regenerates gen/ with the
new labels. Aliases of hidden boundaries without a sub_ label are left
as they are. Refuses to run if an address has two names or a name two addresses,
or if an alias disagrees with the name functions.csv already has for its address.
"""
import argparse
import csv
import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
# a trailing `// comment` (or clang-format's wrapped form of a long line) is kept on its own line
ALIAS = re.compile(r"^#define[ \t]+(\w+)(?:[ \t]+|[ \t]*\\\n[ \t]*)(sub_[0-9A-F]{8})[ \t]*(//[^\n]*)?\n", re.M)
CSV = os.path.join(ROOT, "symbols", "functions.csv")


def sources():
    pats = ("src/**/*.c", "src/**/*.h", "include/**/*.h")
    return sorted({p for pat in pats for p in glob.glob(os.path.join(ROOT, pat), recursive=True)})


def func_labels():
    """Disassembler function labels by mode. Hidden boundaries added later
    (inventory starts split.py cuts from raw bytes) are not sub_ labels and keep aliases."""
    path = os.path.join(ROOT, "gen", "code.s")
    if not os.path.exists(path):
        sys.exit("gen/code.s missing: run make disasm")
    found = re.findall(r"^\t(arm|thumb|non_word_aligned_thumb)_func_start (sub_[0-9A-F]{8})",
                       open(path).read(), re.M)
    return {label: mode for mode, label in found}


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    rows = list(csv.DictReader(open(CSV)))
    by_addr = {r["addr"].upper(): r["name"] for r in rows}
    names = set(by_addr.values())
    labels = func_labels()
    alias = {}  # sub_X -> Name
    for path in sources():
        for name, label, _ in ALIAS.findall(open(path).read()):
            if label not in labels:
                # already named (gen/ labels it by name) or a hidden boundary: never rewrite
                # the alias to `#define Alias ExistingName`, report the disagreement instead
                if by_addr.get(label[4:], name) != name:
                    sys.exit(f"{label} is already {by_addr[label[4:]]} in functions.csv, alias says {name}"
                             f" ({os.path.relpath(path, ROOT)}): rename one of them")
                continue
            if alias.setdefault(label, name) != name:
                sys.exit(f"{label} has two names: {alias[label]}, {name}")
    errors = []
    seen = {}
    for label, name in alias.items():
        addr = label[4:]
        if seen.setdefault(name, label) != label:
            errors.append(f"{name} names {seen[name]} and {label}")
        if addr in by_addr and by_addr[addr] != name:
            errors.append(f"{label} is already {by_addr[addr]} in functions.csv, alias says {name}")
        elif addr not in by_addr and name in names:
            errors.append(f"{name} is already used in functions.csv")
    if errors:
        sys.exit("\n".join(errors))

    new = {label: name for label, name in alias.items() if label[4:] not in by_addr}
    # Workers based on an older checkout can still call a previously promoted
    # sub_ label. Normalize those references as well as this batch's aliases.
    renames = {"sub_" + addr: name for addr, name in by_addr.items()}
    renames.update(alias)
    print(f"{len(alias)} aliases, {len(new)} new names for functions.csv")
    if args.dry_run:
        return

    ref = re.compile(r"\bsub_[0-9A-F]{8}\b")
    changed = 0
    for path in sources():
        text = open(path).read()
        out = ALIAS.sub(lambda m: ((m.group(3) or "").strip() + "\n").lstrip("\n")
                        if renames.get(m.group(2)) == m.group(1) else m.group(0), text)
        out = ref.sub(lambda m: renames.get(m.group(0), m.group(0)), out)
        if out != text:
            open(path, "w").write(out)
            changed += 1
    print(f"rewrote {changed} source files")

    # Linker aliases: "Name = sub_X;" becomes redundant once sub_X is Name.
    ld = os.path.join(ROOT, "symbols", "ram.ld")
    text = open(ld).read()
    out = re.sub(r"^(\w+) = (sub_[0-9A-F]{8});\n",
                 lambda m: "" if renames.get(m.group(2)) == m.group(1) else m.group(0), text, flags=re.M)
    out = ref.sub(lambda m: renames.get(m.group(0), m.group(0)), out)
    if out != text:
        open(ld, "w").write(out)

    fields = list(rows[0].keys())
    for label, name in new.items():
        rows.append({"addr": label[4:], "name": name, "mode": "arm" if labels[label] == "arm" else "thumb",
                     "confidence": "low", "source": "src-alias", "notes": ""})
    rows.sort(key=lambda r: int(r["addr"], 16))
    with open(CSV, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=fields, lineterminator="\n")
        w.writeheader()
        w.writerows(rows)


if __name__ == "__main__":
    main()
