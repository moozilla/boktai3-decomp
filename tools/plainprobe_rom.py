#!/usr/bin/env python3
"""Build compiler-measured flag_order targets for the fields found by plainprobe.

Run after plainprobe has written its inventory:
  build/venv/bin/python tools/plainprobe_rom.py
  build/venv/bin/python tools/plainprobe.py --resume

The report distinguishes layouts with no classified instructions from layouts this
tool could not measure. Private tags with conflicting definitions are omitted.
"""
from __future__ import annotations

import argparse
import csv
import json
from collections import defaultdict
from pathlib import Path
import re
import subprocess
import sys

import concessions
import flag_order
import keepcheck
import rawfield

ROOT = Path(keepcheck.ROOT)
OUT = ROOT / "build/plainprobe"
RECORD = re.compile(r"\b(?P<type>struct|union)\s+(?P<tag>\w+)\s*\{")
EMBED = re.compile(r"^(?P<type>struct|union)\s+(?P<tag>\w+)\s+(?P<name>\w+)$")
SCALAR = re.compile(r"\b(?:u|s)(8|16|32)\s+(\w+)\s*;\s*$")


def records(paths):
    """Index unique named records and their direct, by-value child records."""
    shapes = defaultdict(dict)
    for path in paths:
        code = keepcheck.blank_comments(path.read_text(errors="replace"))
        for head in RECORD.finditer(code):
            body = code[head.end():concessions._scope_end(code, head.end())]
            key = f"{head['type']} {head['tag']}"
            shape = re.sub(r"\s+", "", body)
            shapes[key].setdefault(shape, (path, body))
    declarations, members, ambiguous = {}, {}, set()
    for owner, found in shapes.items():
        if len(found) > 1:
            ambiguous.add(owner)
            continue
        path, body = next(iter(found.values()))
        declarations[owner] = path
        child = {}
        for match in re.finditer(r"(?:^|;)\s*([^;{}]+)(?=;)", body):
            pos = match.start(1)
            if body.count("{", 0, pos) != body.count("}", 0, pos):
                continue
            if member := EMBED.fullmatch(" ".join(match[1].split())):
                child[member["name"]] = f"{member['type']} {member['tag']}"
        members[owner] = child
    return declarations, members, ambiguous


def canonical(owner, member, members):
    parts = member.split(".")
    while len(parts) > 1 and parts[0] in members.get(owner, {}):
        owner = members[owner][parts.pop(0)]
    return owner, ".".join(parts)


def spell(owner, member):
    return member if owner == "(global)" else f"{owner.removeprefix('struct ')}.{member}"


def scan_layouts(sites, out):
    source_paths = defaultdict(set)
    globals_ = set()
    for site in sites:
        for owner, member, _ in json.loads(site["field_keys"]):
            if owner == "(global)":
                globals_.add(member)
            elif owner.startswith(("struct ", "union ")):
                source_paths[owner, member].add(site["path"])

    sources = {ROOT / p for paths in source_paths.values() for p in paths}
    declarations, members, ambiguous = records([*sorted(ROOT.glob("include/**/*.h")), *sorted(sources)])
    aliases = {key: canonical(*key, members) for key in source_paths if key[0] not in ambiguous}
    parents = defaultdict(list)
    for parent, children in members.items():
        for member, child in children.items():
            parents[child].append((parent, member))

    # Include the same field through any declared by-value wrapper. A single
    # ROM operation still reaches just one canonical field and is counted once.
    wanted = defaultdict(dict)
    pending = [*(pair for pair in source_paths if pair[0] not in ambiguous), *aliases.values()]
    for owner, field in pending:
        destination = canonical(owner, field, members)
        if field in wanted[owner]:
            continue
        wanted[owner][field] = destination
        pending.extend((parent, f"{member}.{field}") for parent, member in parents[owner])

    signatures = flag_order.signatures()
    parameter_types = {f"struct {arg.owner}" for _, args in signatures.values() for arg in args.values()}
    source_for = defaultdict(set)
    for (owner, _), paths in source_paths.items():
        source_for[owner].update(paths)
    probe = out / "layout_unit.c"
    targets = defaultdict(set)
    unmeasured = set()
    for owner, fields in sorted(wanted.items()):
        if owner not in parameter_types or owner in ambiguous:
            continue
        candidates = sorted(source_for[owner], key=lambda p: ((ROOT / p).stat().st_size, p))[:3]
        defining = declarations.get(owner)
        if defining and defining.is_relative_to(ROOT / "include"):
            header = defining.relative_to(ROOT / "include")
            probe.write_text(f'#include "global.h"\n#include "{header}"\n')
            candidates.insert(0, str(probe))
        layout = {}
        for path in candidates:
            layout = rawfield.layouts(path, [owner]).get(owner, [])
            if layout:
                break
        leaves = {leaf["name"]: leaf for leaf in layout
                  if leaf["base"] == "int" and leaf["bits"] is None and not leaf["array"]
                  and leaf["width"] in (1, 2, 4)}
        for field, destination in fields.items():
            if field not in leaves:
                unmeasured.add((owner, field))
                continue
            leaf = leaves[field]
            targets[owner.removeprefix("struct "), leaf["off"], leaf["width"]].add(spell(*destination))

    global_rows = []
    with (ROOT / "symbols/ram.csv").open() as stream:
        for row in csv.DictReader(stream):
            m = SCALAR.search(row["decl"])
            if m and row["name"] == m[2] and m[2] in globals_:
                global_rows.append({"addr": row["addr"], "width": int(m[1]) // 8, "field": m[2]})

    layout = {
        "targets": [{"owner": owner, "offset": offset, "width": width, "field": next(iter(keys))}
                    for (owner, offset, width), keys in sorted(targets.items()) if len(keys) == 1],
        "globals": global_rows,
        "unmeasured": sorted(unmeasured),
        "ambiguous_types": sorted(ambiguous.intersection(wanted.keys() | {owner for owner, _ in source_paths})),
        "ambiguous_offsets": sum(len(keys) > 1 for keys in targets.values()),
    }
    return layout, [(*key, *destination) for key, destination in sorted(aliases.items()) if key != destination]


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", type=Path, default=OUT)
    args = parser.parse_args(argv)
    out = args.out.resolve()
    with (out / "sites.csv").open() as stream:
        sites = list(csv.DictReader(stream))
    layout, aliases = scan_layouts(sites, out)
    layout_path = out / "flag_targets.json"
    layout_path.write_text(json.dumps(layout, indent=2) + "\n")
    target = out / "flag_sites.tsv"
    temporary = target.with_suffix(".tmp")
    with temporary.open("w") as stream:
        subprocess.run([sys.executable, "tools/flag_order.py", str(layout_path)], cwd=ROOT,
                       stdout=stream, check=True)
    temporary.replace(target)
    target.with_suffix(".aliases.json").write_text(json.dumps(aliases, indent=2) + "\n")
    print(f"{len(layout['targets']):,} typed offsets and {len(layout['globals']):,} globals scanned")
    print(f"{len(layout['unmeasured']):,} typed paths unavailable, "
          f"{len(layout['ambiguous_types']):,} conflicting types, "
          f"{layout['ambiguous_offsets']:,} ambiguous offsets")


if __name__ == "__main__":
    main()
