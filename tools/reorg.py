#!/usr/bin/env python3
"""Replay the reviewed source and header placement from either earlier layout."""
import argparse
from collections import Counter, defaultdict
import csv
import io
import os
from pathlib import Path
import re
import subprocess
import sys

from source_layout import (
    ADDRESS, HEADER_DIRS, SOURCE_DIRS, SOURCE_FILES, directory_for, groups, header_paths, is_upstream,
    layout_path, manifest, new_filename, scan, source_paths, source_placements, source_rules, symbol_names,
)

ROOT = Path(__file__).resolve().parents[1]
INCLUDE = re.compile(r'(?P<begin>#[ \t]*include[ \t]+\\?")(?P<name>[^"\\\n]+)(?P<end>\\?")')
HEADER_LITERAL = re.compile(r'''(?P<quote>["'])(?P<name>[\w.-]+/[\w/.-]+\.h)(?P=quote)''')
PATH = re.compile(r'(?<![\w/])((?:\.\./)*)(?P<path>(?:src|include)/[\w./-]+\.[ch])(?![\w.-])')
PATCH_HEAD = re.compile(r'^(diff --git |--- |\+\+\+ |rename (?:from|to) )([^\n]*)', re.M)
PATCH_PATH = re.compile(r'(?<!\S)(?P<side>[ab]/)?(?P<path>(?:src|include)/[\w./-]+\.[ch])(?=$|\s)')
TEXT_SUFFIXES = {".c", ".h", ".csv", ".ld", ".md", ".py", ".js", ".mjs", ".html", ".sh", ".ts", ".tsx", ".yml"}
IDENTIFIER = re.compile(r"[A-Za-z_][A-Za-z_0-9]*")
SYMBOL = re.compile(r"\b[A-Za-z_][A-Za-z_0-9]*\b")
FILE_SUFFIX = re.compile(r"\.(?:c|h|s|inc|py|ts|md)\b")
TYPE_TAG = re.compile(r"\b(?:struct|union|enum)\s+([A-Za-z_]\w*)")
# Layout audit records: their old paths and names are the record, so replay leaves them alone.
LAYOUT_RECORDS = (Path("symbols/identity_corrections.csv"), Path("symbols/identity_prefix_evidence.csv"))
IDENTITY_RECORDS = {Path("symbols/identities.csv"), *LAYOUT_RECORDS}
PENDING_PATCH_DIRS = {("tools", "autopilot", "pending_src_fixes"), ("tools", "testbench", "pending_src_fixes")}


def pending_patch(path):
    return path.suffix == ".patch" and path.parts[:3] in PENDING_PATCH_DIRS


def tracked_paths():
    if file_list := os.environ.get("BOKTAI3_REPLAY_PATHS"):
        paths = {Path(path) for path in Path(file_list).read_text().split("\0") if path}
        if any(path.is_absolute() or ".." in path.parts for path in paths):
            raise ValueError(f"{file_list}: expected repository-relative paths")
        return paths
    out = subprocess.check_output(["git", "ls-files", "--cached", "--others", "--exclude-standard", "-z"], cwd=ROOT)
    return {Path(path) for path in out.decode().split("\0") if path}


def table_rows(path, fields):
    with path.open(newline="") as stream:
        reader = csv.DictReader(stream)
        if not set(fields).issubset(reader.fieldnames or ()):
            raise ValueError(f"{path}: expected columns {', '.join(fields)}")
        return list(reader)


def write_updates(updates):
    """Restore earlier files if a table rewrite is interrupted."""
    originals = []
    try:
        for path, original, updated in updates:
            originals.append((path, original))
            path.write_bytes(updated)
    except BaseException:
        for path, original in reversed(originals):
            if path.read_bytes() != original:
                path.write_bytes(original)
        raise


def prefix_rules(table):
    rules = []
    for row in table_rows(table, ("old_prefix", "new_prefix")):
        old, new = row["old_prefix"] or "", row["new_prefix"] or ""
        if not all(IDENTIFIER.fullmatch(prefix) for prefix in (old, new)):
            raise ValueError(f"{table}: invalid prefix rule {old!r} -> {new!r}")
        rules.append((old, new, (row.get("scope") or "all").rstrip("/")))
    return rules


def in_scope(path, scope):
    return scope == "all" or path == scope or path.startswith(scope + "/")


def prefix_targets(name, prefixes):
    """Use the most specific spelling when a particular helper needs its own name."""
    matches = [(len(old), new + name[len(old):]) for old, new in prefixes if name.startswith(old)]
    longest = max((length for length, _ in matches), default=0)
    return {target for length, target in matches if length == longest}


def prefix_rows(table, rules, names):
    """Resolve prefix rules in the directories that define their functions."""
    files = scan(ROOT).files if any(scope != "all" for _, _, scope in rules) else {}
    starts = tuple(old for old, _, _ in rules)
    rows = []
    for address, name in names.items():
        if not name.startswith(starts):
            continue
        path, _ = files.get(address, ("", ""))
        choices = prefix_targets(name, [(old, new) for old, new, scope in rules if in_scope(path, scope)])
        if len(choices) > 1:
            raise ValueError(f"{table}: overlapping prefix rules for {name} at {address:08X}")
        if choices:
            rows.append({"addr": f"{address:08X}", "old_name": name, "new_name": choices.pop()})
    return rows


def skip_prefix_collisions(table, rows, names):
    remaining = {int(row["addr"], 16): row for row in rows}
    protected = set()
    while True:
        current = names | {address: row["new_name"] for address, row in remaining.items()}
        counts = Counter(current.values())
        collisions = [address for address, row in remaining.items() if counts[row["new_name"]] > 1]
        if not collisions:
            return list(remaining.values()), protected
        for address in collisions:
            row = remaining.pop(address)
            protected.add(row["old_name"])
            print(f"skip {table}: {address:08X} {row['old_name']} -> {row['new_name']} would collide",
                  file=sys.stderr)


def function_renames(table, prefixes=False, apply=False, *, skip_diverged=False,
                     skip_collisions=False, warn_types=False):
    """Rewrite reviewed function symbols as one simultaneous substitution."""
    names = symbol_names(ROOT)
    rules = prefix_rules(table) if prefixes else []
    rows = prefix_rows(table, rules, names) if prefixes else table_rows(table, ("addr", "old_name", "new_name"))
    protected = set()
    if skip_collisions:
        rows, protected = skip_prefix_collisions(table, rows, names)
    new_names, replacements, settled, seen = {}, {}, set(), set()
    for row in rows:
        address = int(row["addr"] or "", 16)
        old, new = row["old_name"] or "", row["new_name"] or ""
        prior = (row.get("prior_names") or "").split("|") if row.get("prior_names") else ()
        if not all(IDENTIFIER.fullmatch(name) for name in (old, new, *prior)) or address in seen:
            raise ValueError(f"{table}: invalid or repeated rename at {address:08X}")
        seen.add(address)
        current = names.get(address)
        if current not in (old, new, *prior):
            message = f"{table}: {address:08X} is {current!r}, expected {old!r} or {new!r}"
            if not skip_diverged:
                raise ValueError(message)
            print(f"skip {message}", file=sys.stderr)
            continue
        new_names[address] = new
        if current == new:
            settled.add(new)
        elif replacements.setdefault(current, new) != new:
            raise ValueError(f"{table}: {current} cannot be renamed unambiguously")
    collisions = Counter((names | new_names).values())
    for name in new_names.values():
        if collisions[name] != 1:
            raise ValueError(f"{table}: function name {name} would collide")
    if settled & replacements.keys():
        raise ValueError(f"{table}: a completed rename overlaps a pending old name")
    replacements.update({old + "__addr": new + "__addr" for old, new in list(replacements.items())})
    return rewrite_identifiers(table, replacements, apply, prefix_scopes=rules,
                               protected=protected, warn_types=warn_types)


def type_renames(table, apply=False):
    """Replay whole-identifier type and inline-helper names without changing the symbol map."""
    by_scope = defaultdict(dict)
    for row in table_rows(table, ("old_name", "new_name")):
        old, new = row["old_name"] or "", row["new_name"] or ""
        scope = (row.get("scope") or "all").rstrip("/")
        if not all(IDENTIFIER.fullmatch(name) for name in (old, new)) or old in by_scope[scope]:
            raise ValueError(f"{table}: invalid or repeated type rename {old!r} -> {new!r}")
        by_scope[scope][old] = new
    for replacements in by_scope.values():
        names = set(replacements.values())
        if len(names) != len(replacements) or names & replacements.keys():
            raise ValueError(f"{table}: overlapping type renames")
    return rewrite_identifiers(table, by_scope.pop("all", {}), apply, exact_scopes=by_scope)


def rewrite_identifiers(table, replacements, apply, *, prefix_scopes=(), exact_scopes=None,
                        protected=(), warn_types=False):
    if not (replacements or prefix_scopes or exact_scopes):
        return 0

    warned = set()

    def rewrite(raw, rel):
        path = rel.as_posix()
        exact = [names for scope, names in (exact_scopes or {}).items() if in_scope(path, scope)]
        prefixes = [(old, new) for old, new, scope in prefix_scopes
                    if rel.suffix in (".c", ".h") and in_scope(path, scope)]
        starts = tuple(old for old, _ in prefixes)
        if warn_types and prefixes:
            type_prefixes = tuple(old.rstrip("_") for old in starts if old.endswith("_"))
            for name in TYPE_TAG.findall(raw):
                if name not in warned and name.startswith(type_prefixes):
                    warned.add(name)
                    print(f"warning: {rel}: check legacy type {name} against the reviewed type table",
                          file=sys.stderr)

        def replace(match):
            name = match[0]
            if protected and name.removesuffix("__addr") in protected:
                return name
            renamed = replacements.get(name)
            if renamed is None and (exact or name.startswith(starts)):
                choices = {names[name] for names in exact if name in names}
                choices.update(prefix_targets(name, prefixes))
                if len(choices) > 1:
                    raise ValueError(f"{rel}: conflicting scoped renames for {name}")
                renamed = next(iter(choices), None)
            return name if renamed is None or FILE_SUFFIX.match(raw, match.end()) else renamed

        return SYMBOL.sub(replace, raw)

    paths = tracked_paths() | {Path("symbols/functions.csv")}
    paths.update(p.relative_to(ROOT) for p in source_paths(ROOT) + header_paths(ROOT))
    changed = []
    input_table = table.resolve()
    for rel in sorted(paths):
        if (rel.parts[0] in ("build", "notes") or rel in IDENTITY_RECORDS or
                rel.parts[:2] == ("docs", "history") or
                rel.name in ("PROGRESS.json", "PROGRESS.md") or
                rel in (Path("symbols/function_renames.csv"), Path("symbols/type_renames.csv"),
                        Path("symbols/prefix_renames.csv")) or
                rel.suffix not in TEXT_SUFFIXES | {".s", ".inc", ".json", ".txt", ".tsv"}):
            continue
        path = ROOT / rel
        if not path.is_file() or path.resolve() == input_table:
            continue
        original = path.read_bytes()
        raw = original.decode("utf-8", errors="surrogateescape")
        updated = rewrite(raw, rel)
        if updated != raw:
            print(f"rewrite {rel}")
            changed.append((path, original, updated.encode("utf-8", errors="surrogateescape")))
    if apply:
        write_updates(changed)
        manifest.cache_clear()  # symbol_names reads the table on each call
    return len(changed)


def recorded_moves():
    """Map prior paths to the recorded destinations."""
    sources = {}
    for record in source_placements(ROOT):
        for previous in (record.original, record.current, record.pass2, record.target, *record.prior_paths):
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
            aliases.extend(layout_path(path, "include") for path in (row.get("prior_paths") or "").split("|") if path)
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


def plan_files(recorded, rename, manifest):
    names = symbol_names(ROOT)
    plans = {record.start: record.planned_basename for record in source_placements(ROOT)
             if record.planned_basename} if rename else {}
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
        if rename and not is_upstream(old):
            if first in plans:
                new = new.with_name(plans[first])
            elif len(addresses) == 1 or ADDRESS.fullmatch(old.stem):
                name = names.get(first, f"sub_{first:08X}")
                if not ADDRESS.fullmatch(name) or ADDRESS.fullmatch(new.stem):
                    new = new.with_name(new_filename(name, first))
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
    if "src/" not in raw and "include/" not in raw:
        return raw

    if pending_patch(path):
        def patch_path(match):
            old = Path(match["path"])
            return (match["side"] or "") + moves.get(old, old).as_posix()

        raw = PATCH_HEAD.sub(lambda match: match[1] + PATCH_PATH.sub(patch_path, match[2]), raw)

    def reference(match):
        old = Path(match["path"])
        target = moves.get(old, old)
        if match[1] and Path(os.path.normpath(path.parent / match[0])) == old:
            return os.path.relpath(target, path.parent)
        return match[1] + target.as_posix()

    return PATH.sub(reference, raw)


def migrate(rename=False, apply=False):
    recorded = recorded_moves()
    manifest = scan(ROOT)
    moves = plan_files(recorded, rename, manifest)

    # An already renamed file can still have historical paths in the placement table.
    live_sources = {}
    for placement in source_placements(ROOT):
        if placement.target in moves:
            continue
        entry = manifest.files.get(placement.start)
        if entry is not None:
            source = Path(entry[0])
            if moves[source].parent == placement.target.parent:
                live_sources[placement.target] = source
    if live_sources:
        recorded = {old: live_sources.get(target, target) for old, target in recorded.items()}
    return rewrite_moves(recorded, moves, apply)


def rewrite_moves(recorded, moves, apply, *, input_table=None):
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
    paths = set(moves)
    for path in tracked_paths():
        if (path.parts[0] not in ("src", "include", "tests", "build")
                and (path.suffix in TEXT_SUFFIXES or pending_patch(path))
                and path not in (SOURCE_DIRS, SOURCE_FILES, HEADER_DIRS, *LAYOUT_RECORDS)
                and path not in (Path("PROGRESS.md"), Path("docs/SOURCE_LAYOUT_DECISIONS.md"))
                and path.parts[:2] != ("docs", "history")):
            paths.add(path)

    changed = []
    for old in sorted(paths):
        if ROOT / old == input_table or not (ROOT / old).is_file():
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


def csv_text(rows):
    with io.StringIO(newline="") as stream:
        writer = csv.DictWriter(stream, fieldnames=list(dict.fromkeys(key for row in rows for key in row)),
                                lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)
        return stream.getvalue()


def player_moves(table, apply=False):
    """Replay reviewed per-file Player routes, then record them in the placement tables."""
    placements = source_placements(ROOT)
    aliases = {}
    for record in placements:
        for path in (record.original, record.current, record.pass2, record.target, *record.prior_paths):
            if path is not None and aliases.setdefault(path, record) != record:
                raise ValueError(f"{SOURCE_FILES}: ambiguous source path {path}")
    moves, references, pending = {}, {}, {}
    already_moved = None
    for row in table_rows(table, ("file", "new_path")):
        path = layout_path(row["file"] or "", "src")
        directory = layout_path(row["new_path"] or "", "src")
        if (path.parts[:2] != ("src", "player") or path.suffix != ".c" or
                not (directory.parts[:2] == ("src", "player") or directory.parts == ("src", "link", "player")) or
                not 3 <= len(directory.parts) <= 4 or directory.suffix):
            raise ValueError(f"{table}: invalid Player route {path} -> {directory}")
        record = aliases.get(path)
        if record is None or record.current in pending:
            raise ValueError(f"{table}: missing or repeated placement for {path}")
        old = record.target
        target = directory / old.name
        pending[record.current] = (record, target, row.get("note") or "reviewed Player placement")
        if old == target:
            continue
        if (ROOT / old).is_file():
            moves[old] = target
        else:
            # A previous apply may have moved the file before writing the tables.
            if already_moved is None:
                already_moved = scan(ROOT).files
            if already_moved.get(record.start, (None,))[0] != target.as_posix():
                raise ValueError(f"{table}: expected source at {old} or {target}")
        for previous in (path, old, record.original, record.current, record.pass2, *record.prior_paths):
            if previous is not None:
                references[previous] = target
    expected = [pending[record.current][1] if record.current in pending else record.target for record in placements]
    if len({path.as_posix().casefold() for path in expected}) != len(expected):
        raise ValueError(f"{table}: colliding Player destinations")
    pending = {path: plan for path, plan in pending.items() if plan[0].target != plan[1]}
    if not pending:
        return 0

    file_rows = table_rows(ROOT / SOURCE_FILES, ("current_path", "rule"))
    rule_rows = table_rows(ROOT / SOURCE_DIRS, ("current_path", "new_path", "note"))
    rules = {row["current_path"]: row for row in rule_rows}
    for row in file_rows:
        key = Path(row["current_path"])
        if key not in pending:
            continue
        record, target, note = pending[key]
        row["rule"] = key.as_posix()
        rules[row["rule"]] = {"current_path": row["rule"], "new_path": target.parent.as_posix(), "note": note}
        row["prior_paths"] = "|".join(path.as_posix() for path in dict.fromkeys((*record.prior_paths, record.target)))
    tables = {SOURCE_DIRS: csv_text(list(rules.values())), SOURCE_FILES: csv_text(file_rows)}
    metadata = [(ROOT / path, (ROOT / path).read_bytes(), contents.encode()) for path, contents in tables.items()]
    changes = rewrite_moves(references, moves, apply, input_table=table.resolve())
    for path in tables:
        print(f"rewrite {path}")
    if apply:
        write_updates(metadata)
        source_placements.cache_clear()
        groups.cache_clear()
    return changes + len(metadata)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    action = parser.add_mutually_exclusive_group()
    action.add_argument("--apply", action="store_true", help="perform the printed plan")
    action.add_argument("--check", action="store_true", help="fail if the tree disagrees with the reviewed layout")
    operation = parser.add_mutually_exclusive_group()
    operation.add_argument("--rename", action="store_true", help="also plan source basenames from ROM order and the naming table")
    operation.add_argument("--function-renames", type=Path, help="read exact addr,old_name,new_name rows")
    operation.add_argument("--function-prefixes", type=Path, help="read old_prefix,new_prefix[,scope] rows")
    operation.add_argument("--player-moves", type=Path, help="read file,new_path[,note] Player routes")
    operation.add_argument("--type-renames", type=Path, help="read exact old_name,new_name identifier rows")
    parser.add_argument("--skip-diverged", action="store_true",
                        help="report and skip exact rows with a different symbol at the address")
    parser.add_argument("--skip-collisions", action="store_true", help="report and skip colliding prefix renames")
    parser.add_argument("--warn-types", action="store_true", help="report remaining scoped legacy type tags")
    args = parser.parse_args()
    if args.skip_diverged and not args.function_renames:
        parser.error("--skip-diverged requires --function-renames")
    if (args.skip_collisions or args.warn_types) and not args.function_prefixes:
        parser.error("--skip-collisions and --warn-types require --function-prefixes")
    try:
        if table := args.function_renames or args.function_prefixes:
            changes = function_renames(table, prefixes=args.function_prefixes is not None,
                                       apply=args.apply, skip_diverged=args.skip_diverged,
                                       skip_collisions=args.skip_collisions, warn_types=args.warn_types)
        elif args.type_renames:
            changes = type_renames(args.type_renames, args.apply)
        elif args.player_moves:
            changes = player_moves(args.player_moves, args.apply)
        else:
            changes = migrate(args.rename, args.apply)
    except ValueError as error:
        parser.error(str(error))
    print(f"{changes} paths {'updated' if args.apply else 'to update'}")
    return int(args.check and changes != 0)


if __name__ == "__main__":
    raise SystemExit(main())
