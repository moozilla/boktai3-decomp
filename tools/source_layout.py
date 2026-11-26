"""Resolve source files from the symbol map and the declarations in the tree."""
from __future__ import annotations

import argparse
import csv
from bisect import bisect_right
from collections import defaultdict
from contextlib import suppress
from dataclasses import dataclass
from functools import lru_cache
from hashlib import sha256
import json
from pathlib import Path
import re
from tempfile import NamedTemporaryFile

from romlib import ROOT

ADDRESS = re.compile(r"(?:sub_|0x)?([0-9a-f]{8})", re.I)
ALIASES = re.compile(r"^#define[ \t]+(\w+)[ \t]+(sub_[0-9A-F]{8})\b[ \t]*(?://[^\n]*)?$", re.M)
SOURCE_DIRS = Path("symbols/source_dirs.csv")
SOURCE_FILES = Path("symbols/source_files.csv")
HEADER_DIRS = Path("symbols/header_dirs.csv")
LAYOUT_RANGE = re.compile(r"([0-9A-F]{8})-([0-9A-F]{8})")


def slug(name):
    """Lowercase words and acronyms, preserving digits within a word."""
    words = []
    for word in re.findall(r"[a-zA-Z0-9]+", name):
        address = re.fullmatch(r"(?:0[xX])?([0-9a-fA-F]{8})", word)
        if address:
            words.append(address[1].upper())
        else:
            word = re.sub(r"([A-Z]+)([A-Z][a-z])", r"\1_\2", word)
            words.append(re.sub(r"([a-z])([A-Z])", r"\1_\2", word).lower())
    return "_".join(words)


def new_filename(name, address):
    return (f"sub_{address:08X}" if ADDRESS.fullmatch(name) else name) + ".c"


def source_paths(root=ROOT):
    return sorted((Path(root) / "src").rglob("*.c"))


def header_paths(root=ROOT):
    return sorted((Path(root) / "include").rglob("*.h"))


def is_upstream(path):
    """Identify the imported SDK and runtime files with their upstream style.

    Hand-recovered wrappers share the library directories and keep the game
    source's linter and formatting rules.
    """
    if path is None:
        return False
    parts = Path(str(path).replace("\\", "/")).parts
    if any(parts[i:i + 2] == ("include", "gba") for i in range(len(parts))):
        return True
    for i in range(len(parts)):
        if parts[i:i + 2] == ("src", "lib"):
            if len(parts) == i + 3:  # Historical revisions.
                return True
            return (parts[-1] == "m4a.c" or
                    parts[-1].startswith(("libc_", "libgcc_", "libm_", "librfu_")))
    return False


def symbol_names(root=ROOT):
    path = Path(root) / "symbols/functions.csv"
    if not path.exists():
        return {}
    with path.open(newline="") as stream:
        return {int(row["addr"], 16): row["name"] for row in csv.DictReader(stream)}


def address_of(value, names):
    if isinstance(value, int):
        return value
    match = ADDRESS.fullmatch(value)
    return int(match[1], 16) if match else names.get(value)


@dataclass
class Manifest:
    """Function address -> (repository-relative file, spelling in its C source)."""
    files: dict[int, tuple[str, str]]
    names: dict[str, int]
    by_file: dict[str, list[int]]

    def locate(self, value):
        return self.files.get(address_of(value, self.names))

    def single_file(self, value):
        entry = self.locate(value)
        if entry and len(self.by_file[entry[0]]) == 1:
            return entry[0]
        return None


def scan(root=ROOT, source_root=None):
    from split import draft_functions, source_functions

    root = Path(root)
    names = {name: address for address, name in symbol_names(root).items()}
    files, by_file = {}, defaultdict(list)
    for path in sorted(Path(source_root or root / "src").rglob("*.c")):
        rel = path.relative_to(root).as_posix()
        text = path.read_text()
        aliases = dict(ALIASES.findall(text))
        definitions = source_functions(text, resolve_aliases=False) + [name for name, _ in draft_functions(text)]
        for definition in dict.fromkeys(definitions):
            address = address_of(aliases.get(definition, definition), names)
            if address is None:
                continue
            names.setdefault(definition, address)
            files.setdefault(address, (rel, definition))
            if address not in by_file[rel]:
                by_file[rel].append(address)
    return Manifest(files, names, dict(by_file))


@lru_cache(maxsize=4)
def manifest(root=ROOT):
    root = Path(root)
    cache = root / "build" / "source-layout.json"
    if not cache.parent.is_dir():
        return scan(root)
    try:
        fingerprint = manifest_fingerprint(root)
    except OSError:
        return scan(root)
    try:
        saved = json.loads(cache.read_text())
        if saved["fingerprint"] == fingerprint:
            return Manifest(
                {int(address): tuple(entry) for address, entry in saved["files"].items()},
                saved["names"],
                saved["by_file"],
            )
    except (OSError, ValueError, KeyError, TypeError, AttributeError):
        pass

    result = scan(root)
    temporary = None
    try:
        with NamedTemporaryFile("w", dir=cache.parent, delete=False) as stream:
            temporary = Path(stream.name)
            json.dump(
                {"fingerprint": fingerprint, "files": result.files,
                 "names": result.names, "by_file": result.by_file},
                stream,
            )
        temporary.replace(cache)
        temporary = None
    except OSError:
        pass
    finally:
        if temporary is not None:
            with suppress(OSError):
                temporary.unlink()
    return result


def manifest_fingerprint(root):
    """Track source edits and moves, symbol renames, and parser changes."""
    fingerprint = sha256()
    symbol_table = root / "symbols/functions.csv"
    parser = Path(__file__)
    paths = [parser, parser.with_name("split.py"), *source_paths(root)]
    if symbol_table.is_file():
        paths.append(symbol_table)
    for path in paths:
        info = path.stat()
        cache_key = (str(path), info.st_ino, info.st_size, info.st_mtime_ns, info.st_ctime_ns)
        fingerprint.update(repr(cache_key).encode())
    return fingerprint.hexdigest()


def locate(value, root=ROOT):
    return manifest(root).locate(value)


def resolve(value, root=ROOT):
    """Resolve a repo path, file basename, symbol name, or ROM address."""
    path = Path(root) / value
    if path.is_file():
        return path.relative_to(root).as_posix() if path.is_relative_to(root) else str(path)
    name = Path(value).name
    entry = locate(Path(name).stem, root)
    if entry:
        return entry[0]
    matches = [p for p in source_paths(root) if p.name == name or p.stem == name]
    if len(matches) == 1:
        return matches[0].relative_to(root).as_posix()
    raise ValueError(f"{value}: " + ("ambiguous source filename" if matches else "source file not found"))


def report_path(tree, address, index, root=ROOT):
    """Decode a source file index. Version 1 sometimes omitted address filenames."""
    if index == -1:
        entry = locate(address, root)
        return entry[0] if entry else None
    if index < 0:
        return None
    path = tree["files"][index]
    if (Path(root) / path).is_file() or tree.get("version") != 1:
        return path
    entry = locate(address, root)
    return entry[0] if entry else path


@dataclass(frozen=True)
class SourcePlacement:
    original: Path
    current: Path
    target: Path
    start: int
    end: int
    pass2: Path | None = None
    prior_paths: tuple[Path, ...] = ()
    planned_basename: str = ""


def layout_path(value, category):
    path = Path(value)
    if not path.parts or path.parts[0] != category or ".." in path.parts:
        raise ValueError(f"invalid {category} path: {value}")
    return path


def source_rules(root=ROOT):
    with (Path(root) / SOURCE_DIRS).open(newline="") as stream:
        rows = list(csv.DictReader(stream))
    rules = {row["current_path"]: row["new_path"] for row in rows}
    if len(rules) != len(rows):
        raise ValueError(f"{SOURCE_DIRS}: duplicate source rule")
    return rules


@lru_cache(maxsize=4)
def source_placements(root=ROOT):
    """Resolve the reviewed directory rules for each file in the base snapshot."""
    root = Path(root)
    if not (root / SOURCE_FILES).is_file():
        return ()
    rules = source_rules(root)
    placements = []
    with (root / SOURCE_FILES).open(newline="") as stream:
        for row in csv.DictReader(stream):
            dest = layout_path(rules[row["rule"]], "src")
            name = row["filename"]
            if Path(name).name != name or not name.endswith(".c"):
                raise ValueError(f"{SOURCE_FILES}: invalid filename {name}")
            target = dest if dest.suffix == ".c" else dest / name
            if not 3 <= len(target.parts) <= 5:
                raise ValueError(f"{SOURCE_DIRS}: unexpected source depth: {target}")
            original = layout_path(row["original_path"], "src")
            current = layout_path(row["current_path"], "src")
            start, end = int(row["start"], 16), int(row["end"], 16)
            if start >= end or placements and start < placements[-1].end:
                raise ValueError(f"{SOURCE_FILES}: invalid or overlapping range at {start:08X}")
            previous = row.get("pass2_path")
            pass2 = layout_path(previous, "src") if previous else None
            prior_paths = tuple(layout_path(path, "src") for path in (row.get("prior_paths") or "").split("|") if path)
            planned = row.get("planned_basename") or ""
            if planned and not re.fullmatch(r"[A-Za-z_][A-Za-z_0-9]*\.c", planned):
                raise ValueError(f"{SOURCE_FILES}: invalid planned basename {planned}")
            placements.append(SourcePlacement(original, current, target, start, end, pass2, prior_paths, planned))
    for field in ("original", "current", "target"):
        paths = [getattr(placement, field).as_posix().casefold() for placement in placements]
        if len(paths) != len(set(paths)):
            raise ValueError(f"{SOURCE_FILES}: duplicate {field} file path")
    return tuple(placements)


@lru_cache(maxsize=4)
def groups(root=ROOT):
    recorded = [(row.start, row.end, row.target.parent) for row in source_placements(root)]
    ranges = []
    if (Path(root) / SOURCE_DIRS).is_file():
        for key, destination in source_rules(root).items():
            if match := LAYOUT_RANGE.fullmatch(key):
                start, end = (int(value, 16) for value in match.groups())
                folder = layout_path(destination, "src")
                if start >= end or not 2 <= len(folder.parts) <= 4 or folder.suffix:
                    raise ValueError(f"{SOURCE_DIRS}: invalid range rule {key} -> {folder}")
                ranges.append((start, end, folder))
    ranges.sort()
    if any(left[1] > right[0] for left, right in zip(ranges, ranges[1:])):
        raise ValueError(f"{SOURCE_DIRS}: overlapping ROM layout ranges")
    return [(tuple(row[0] for row in entries), entries) for entries in (recorded, ranges)]


def directory_for(address, root=ROOT, *, source=None):
    # A recorded translation unit stays intact even if it straddles a new range.
    for starts, ranges in groups(root):
        index = bisect_right(starts, address) - 1
        if index >= 0 and address < ranges[index][1]:
            return ranges[index][2].relative_to("src")
    location = f"{source} ({address:08X})" if source else f"sub_{address:08X}.c"
    raise ValueError(f"{location}: no ROM placement; add a rule in {SOURCE_DIRS} and {SOURCE_FILES}")


def new_source(address, name, root=ROOT):
    filename = new_filename(name, address)
    return (Path("src") / directory_for(address, root, source=filename) / filename).as_posix()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("function", nargs="*")
    parser.add_argument("--format-files", action="store_true", help="list every repo-formatted C/header path")
    args = parser.parse_args()
    if args.format_files:
        for path in source_paths() + header_paths():
            rel = path.relative_to(ROOT).as_posix()
            if not is_upstream(rel) and path.name != "rfu_compat.h":
                print(rel)
    else:
        for value in args.function:
            try:
                print(resolve(value))
            except ValueError as error:
                parser.error(str(error))


if __name__ == "__main__":
    main()
