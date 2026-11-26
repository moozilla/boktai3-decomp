#!/usr/bin/env python3
"""Promote `#define Name sub_XXXXXXXX` aliases in C sources to real names.

  promote_names.py [--dry-run | --check] [--only-table CSV]

Workers name functions with a file-local alias so they need not touch the
shared symbols/functions.csv (which renames labels in gen/). At integration
this tool moves every alias into symbols/functions.csv (confidence=low,
source=src-alias), rewrites each `sub_XXXXXXXX` reference in src/ and include/
and symbols/ram.ld to the name, and deletes the alias lines. It then refreshes
the informational name column of the keyed_callbacks tables in data/manifest.csv
(the script command table, data/actor_registry.csv) from symbols/functions.csv. `make` then regenerates gen/ with the
new labels. Aliases of hidden boundaries without a sub_ label are left
as they are. Refuses to run if an address has two names or a name two addresses,
or if an alias disagrees with the name functions.csv already has for its address.
A selected row can give a prior file-local spelling and a confirmed ARM or
Thumb mode for an entry without a disassembler function label.
"""
import argparse
from collections import defaultdict
import csv
from dataclasses import dataclass
import glob
import os
from pathlib import Path
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
                       Path(path).read_text(), re.M)
    return {label: mode for mode, label in found}


@dataclass(frozen=True)
class Selection:
    name: str
    prior: str
    mode: str


def selected_aliases(path):
    selections = {}
    with path.open(newline="") as stream:
        for row in csv.DictReader(stream):
            addr, name = row["addr"].upper(), row["alias"]
            label = f"sub_{addr}"
            prior, mode = row.get("prior_alias") or "", row.get("mode") or ""
            if (not re.fullmatch(r"[0-9A-F]{8}", addr) or not name.isidentifier()
                    or label in selections or (prior and not prior.isidentifier()) or mode not in ("", "arm", "thumb")):
                sys.exit(f"{path}: invalid or repeated alias selection at {addr}")
            selections[label] = Selection(name, prior, mode)
    return selections


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    action = ap.add_mutually_exclusive_group()
    action.add_argument("--dry-run", action="store_true")
    action.add_argument("--check", action="store_true", help="verify that the selected addresses are named")
    ap.add_argument("--only-table", type=Path, help="promote the selected addr,alias rows")
    ap.add_argument("--wrap-renames", action="store_true", help="format C lines pushed beyond the configured width")
    args = ap.parse_args()
    if args.check and not args.only_table:
        ap.error("--check requires --only-table")

    with open(CSV, newline="") as stream:
        reader = csv.DictReader(stream)
        rows = list(reader)
        fields = reader.fieldnames
    for row in rows:
        if extra := row.pop(None, ()):
            # Older rows may have an unquoted comma in the final notes column.
            row[fields[-1]] = ",".join((row[fields[-1]], *extra))
    by_addr = {r["addr"].upper(): r["name"] for r in rows}
    names = set(by_addr.values())
    selected = selected_aliases(args.only_table) if args.only_table else None
    if args.check:
        missing = []
        for label, selection in selected.items():
            name = selection.name
            current = by_addr.get(label[4:])
            if current is None:
                missing.append(f"{label}: {name}")
            elif current != name:
                print(f"skip {label}: branch retained {current} (reviewed name {name})", file=sys.stderr)
        if missing:
            sys.exit("Selected aliases still absent from functions.csv:\n" + "\n".join(missing))
        return
    labels = func_labels()
    alias = {}  # sub_X -> Name
    alternatives = defaultdict(set)
    local_aliases = defaultdict(dict)
    for path in sources():
        with open(path) as stream:
            definitions = ALIAS.findall(stream.read())
        for name, label, _ in definitions:
            if selected is not None:
                selection = selected.get(label)
                if selection is None or by_addr.get(label[4:]) not in (None, selection.name):
                    continue
                if local_aliases[path].setdefault(name, label) != label:
                    sys.exit(f"{path}: {name} aliases multiple selected addresses")
                alternatives[name].add(label)
                if by_addr.get(label[4:]) == selection.name:
                    alias[label] = selection.name
                elif (name in (selection.name, selection.prior)
                      and (label in labels or selection.mode)):
                    alias[label] = selection.name
                continue
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

    if selected is not None:
        for label, selection in selected.items():
            if label not in alias and label[4:] not in by_addr:
                print(f"skip {label}: {selection.name} has no promotable source alias", file=sys.stderr)
    new = {label: name for label, name in alias.items() if label[4:] not in by_addr}
    # Workers based on an older checkout can still call a previously promoted
    # sub_ label. Normalize those references as well as this batch's aliases.
    renames = {"sub_" + addr: name for addr, name in by_addr.items()} if selected is None else {}
    renames.update(alias)
    print(f"{len(alias)} aliases, {len(new)} new names for functions.csv")
    if args.dry_run or (selected is not None and not alias):
        return

    rewrites = {name: alias[label] for name, candidates in alternatives.items() if len(candidates) == 1
                for label in candidates if label in alias and name not in names and name not in alias.values()}
    rewrites.update(renames)
    ref = re.compile(r"\b[A-Za-z_][A-Za-z_0-9]*\b(?!\.[ch]\b)")
    if args.wrap_renames:
        from reorg import column_limit, format_lines, is_upstream, overlong_rewrites
        limit = column_limit()
    else:
        limit = 0
    changed = 0
    for path in sources():
        text = Path(path).read_text()
        stripped = ALIAS.sub(lambda m: ((m.group(3) or "").strip() + "\n").lstrip("\n")
                             if ((selected is not None and m.group(2) in alias)
                                 or renames.get(m.group(2)) == m.group(1)) else m.group(0), text)
        names_in_file = {name: alias[label] for name, label in local_aliases[path].items() if label in alias}
        out = ref.sub(lambda m: names_in_file.get(m[0], rewrites.get(m[0], m[0])), stripped)
        if limit and not is_upstream(path):
            out = format_lines(Path(path), out, overlong_rewrites(stripped, out, limit))
        if out != text:
            Path(path).write_text(out)
            changed += 1
    print(f"rewrote {changed} source files")

    # Linker aliases: "Name = sub_X;" becomes redundant once sub_X is Name.
    ld = Path(ROOT, "symbols", "ram.ld")
    text = ld.read_text()
    out = re.sub(r"^(\w+) = (sub_[0-9A-F]{8});\n",
                 lambda m: "" if rewrites.get(m.group(2)) == rewrites.get(m.group(1), m.group(1))
                 else m.group(0), text, flags=re.M)
    out = ref.sub(lambda m: rewrites.get(m.group(0), m.group(0)), out)
    if out != text:
        ld.write_text(out)

    for label, name in new.items():
        mode = labels.get(label) or selected[label].mode
        rows.append({"addr": label[4:], "name": name, "mode": "arm" if mode == "arm" else "thumb",
                     "confidence": "low", "source": "src-alias", "notes": ""})
    rows.sort(key=lambda r: int(r["addr"], 16))
    with open(CSV, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=fields, lineterminator="\n")
        w.writeheader()
        w.writerows(rows)
    refresh_keyed_names({int(r["addr"], 16): r["name"] for r in rows})


def refresh_keyed_names(names):
    """Rewrite the name column of every keyed_callbacks table (data/manifest.csv)."""
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    from datafmt import load_manifest
    from datafmt.keyed_callbacks import refresh_names
    repo = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    for e in load_manifest():
        # Resolve the table under ROOT (the tree being promoted), not this checkout.
        path = os.path.join(ROOT, os.path.relpath(e.file, repo))
        if e.format != "keyed_callbacks" or not os.path.exists(path):
            continue
        text = Path(path).read_text(encoding="utf-8")
        out = refresh_names(text, names)
        if out != text:
            Path(path).write_text(out, encoding="utf-8")
            print(f"refreshed names in {os.path.relpath(path, ROOT)}")


if __name__ == "__main__":
    main()
