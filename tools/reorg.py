#!/usr/bin/env python3
"""Replay the reviewed source and header placement from either earlier layout."""
import argparse
from collections import defaultdict
import csv
import os
from pathlib import Path
import re
import subprocess

from source_layout import (
    HEADER_DIRS, SOURCE_DIRS, SOURCE_FILES, directory_for, header_paths, is_upstream,
    layout_path, new_filename, scan, source_paths, source_placements, source_rules, symbol_names,
)

ROOT = Path(__file__).resolve().parents[1]
INCLUDE = re.compile(r'(?P<begin>#[ \t]*include[ \t]+\\?")(?P<name>[^"\\\n]+)(?P<end>\\?")')
HEADER_LITERAL = re.compile(r'''(?P<quote>["'])(?P<name>[\w.-]+/[\w/.-]+\.h)(?P=quote)''')
PATH = re.compile(r'(?<![\w/])((?:\.\./)*)(?P<path>(?:src|include)/[\w./-]+\.[ch])(?![\w.-])')
TEXT_SUFFIXES = {".c", ".h", ".csv", ".ld", ".md", ".py", ".js", ".mjs", ".html", ".sh", ".ts", ".yml"}


def recorded_moves():
    """Map prior paths to the recorded destinations."""
    sources = {}
    for record in source_placements(ROOT):
        for previous in (record.original, record.current, record.pass2, record.target):
            if previous is not None and sources.setdefault(previous, record.target) != record.target:
                raise ValueError(f"{SOURCE_FILES}: conflicting destination for {previous}")
    headers = {}
    targets = set()
    with (ROOT / HEADER_DIRS).open(newline="") as stream:
        for row in csv.DictReader(stream):
            current = layout_path(row["current_path"], "include")
            target = layout_path(row["new_path"], "include")
            if not 2 <= len(target.parts) <= 4 or target.suffix != ".h":
                raise ValueError(f"{HEADER_DIRS}: invalid header destination {target}")
            if target.as_posix().casefold() in targets:
                raise ValueError(f"{HEADER_DIRS}: duplicate destination {target}")
            targets.add(target.as_posix().casefold())
            aliases = [Path("include", current.name), current, target]
            if row.get("pass2_path"):
                aliases.append(layout_path(row["pass2_path"], "include"))
            for previous in aliases:
                if headers.setdefault(previous, target) != target:
                    raise ValueError(f"{HEADER_DIRS}: conflicting destination for {previous}")
    return sources | headers


def header_destination(old, recorded, folders, owners, rules):
    if old in recorded or old.parts[1] == "gba":
        return recorded.get(old, old)
    # Replay a new header in a previously recorded unit by following its owner.
    unit = Path("src", *old.parent.parts[1:]).as_posix()
    if unit in rules:
        folder = Path(rules[unit])
        if folder.suffix == ".c":
            folder = folder.parent
        return Path("include", *folder.parts[1:3], old.name)
    if old.parent in folders:
        return old
    candidates = owners.get(old.stem, set())
    if len(candidates) == 1:
        return next(iter(candidates)) / old.name
    reason = "ambiguous owner" if candidates else "unknown owner"
    raise ValueError(f"{old}: {reason}; add a placement in {HEADER_DIRS}")


def plan_files(recorded, rename):
    manifest = scan(ROOT)
    names = symbol_names(ROOT)
    spellings = {}
    for old, target in recorded.items():
        casefolded = old.as_posix().casefold()
        if spellings.setdefault(casefolded, target) != target:
            raise ValueError(f"{old}: placement aliases have conflicting capitalization")
    moves = {}
    owners = defaultdict(set)
    for path in source_paths(ROOT):
        old = path.relative_to(ROOT)
        addresses = manifest.by_file.get(old.as_posix(), ())
        if not addresses:
            raise ValueError(f"{old}: no external function with a known ROM address")
        first = min(addresses)
        directory = Path("src") / directory_for(first, ROOT, source=old)
        new = spellings.get(old.as_posix().casefold(), directory / old.name)
        if new.parent != directory:
            raise ValueError(f"{old}: first function {first:08X} belongs under {directory}")
        if rename and len(addresses) == 1 and not is_upstream(old):
            new = new.with_name(new_filename(names.get(first, f"sub_{first:08X}"), first))
        moves[old] = new
        folder = Path("include", *new.parent.parts[1:3])
        owners[old.stem].add(folder)
        owners[new.stem].add(folder)

    folders = {target.parent for target in recorded.values() if target.parts[0] == "include"}
    folders.update(folder for candidates in owners.values() for folder in candidates)
    # The include root is reserved for explicit project-wide headers.
    folders.discard(Path("include"))
    rules = source_rules(ROOT)
    for path in header_paths(ROOT):
        old = path.relative_to(ROOT)
        moves[old] = header_destination(old, recorded, folders, owners, rules)
    return moves


def transform(path, raw, moves, includes):
    def include(match):
        return match["begin"] + includes.get(match["name"], match["name"]) + match["end"]

    def literal(match):
        quote, name = match["quote"], match["name"]
        return quote + includes.get(name, name) + quote

    raw = INCLUDE.sub(include, raw)
    raw = HEADER_LITERAL.sub(literal, raw)

    def reference(match):
        old = Path(match["path"])
        target = moves.get(old, old)
        if match[1] and Path(os.path.normpath(path.parent / match[0])) == old:
            return os.path.relpath(target, path.parent)
        return match[1] + target.as_posix()

    return PATH.sub(reference, raw)


def migrate(rename=False, apply=False):
    recorded = recorded_moves()
    moves = plan_files(recorded, rename)
    occupied = {}
    for old, new in moves.items():
        previous = occupied.setdefault(new.as_posix().casefold(), old)
        if previous != old:
            raise ValueError(f"{previous} and {old} both map to {new}")
        if old != new and (ROOT / new).exists() and not (ROOT / old).samefile(ROOT / new):
            raise ValueError(f"{old}: {new} already exists")

    references = {old: moves.get(new, new) for old, new in recorded.items()} | moves
    includes = {old.relative_to("include").as_posix(): new.relative_to("include").as_posix()
                for old, new in references.items() if old.parts[0] == "include"}
    tracked = subprocess.check_output(["git", "ls-files", "--cached", "--others", "--exclude-standard", "-z"], cwd=ROOT).decode().split("\0")
    paths = set(moves)
    for value in filter(None, tracked):
        path = Path(value)
        if (path.parts[0] not in ("src", "include", "tests") and path.suffix in TEXT_SUFFIXES
                and path not in (SOURCE_DIRS, SOURCE_FILES, HEADER_DIRS)
                and path not in (Path("PROGRESS.md"), Path("docs/SOURCE_LAYOUT_DECISIONS.md"))
                and path.parts[:2] != ("docs", "history")):
            paths.add(path)

    changed = []
    for old in sorted(paths):
        if not (ROOT / old).is_file():
            continue
        raw = (ROOT / old).read_bytes().decode("utf-8", errors="surrogateescape")
        new = moves.get(old, old)
        updated = transform(old, raw, references, includes)
        if new != old:
            print(f"{old} -> {new}")
        elif updated != raw:
            print(f"rewrite {old}")
        if new != old or updated != raw:
            changed.append((old, new, updated if updated != raw else None))

    if apply:
        for old, new, updated in changed:
            if new != old:
                (ROOT / new).parent.mkdir(parents=True, exist_ok=True)
                (ROOT / old).rename(ROOT / new)
            if updated is not None:
                (ROOT / new).write_bytes(updated.encode("utf-8", errors="surrogateescape"))
        for top in ("src", "include"):
            for parent, _, _ in os.walk(ROOT / top, topdown=False):
                path = Path(parent)
                if path != ROOT / top and not any(path.iterdir()):
                    path.rmdir()
    return len(changed)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    action = parser.add_mutually_exclusive_group()
    action.add_argument("--apply", action="store_true", help="perform the printed plan")
    action.add_argument("--check", action="store_true", help="fail if the tree disagrees with the reviewed layout")
    parser.add_argument("--rename", action="store_true", help="also name one-function files from the symbol map")
    args = parser.parse_args()
    try:
        changes = migrate(args.rename, args.apply)
    except ValueError as error:
        parser.error(str(error))
    print(f"{changes} paths {'updated' if args.apply else 'to update'}")
    return int(args.check and changes != 0)


if __name__ == "__main__":
    raise SystemExit(main())
