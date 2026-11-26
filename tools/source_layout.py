"""Resolve source files from the symbol map and the declarations in the tree."""
from __future__ import annotations

import argparse
import csv
from bisect import bisect_right
from collections import defaultdict
from dataclasses import dataclass
from functools import lru_cache
from pathlib import Path
import re

from romlib import ROOT

ADDRESS = re.compile(r"(?:sub_|0x)?([0-9a-f]{8})", re.I)
ALIASES = re.compile(r"^#define[ \t]+(\w+)[ \t]+(sub_[0-9A-F]{8})\b[ \t]*(?://[^\n]*)?$", re.M)
CATEGORY_DIRS = {"ui": "menus", "libraries": "lib"}
LIBRARY_DIRS = {
    "MP2K (m4a)": "m4a",
    "RFU SDK 1024": "rfu",
    "libagbsyscall": "bios",
    "EEPROM SDK V124": "eeprom",
    "SIIRTC": "rtc",
    "libgcc runtime": "libgcc",
    "libm (Sun fdlibm)": "libm",
    "libc (newlib)": "libc",
}

UPSTREAM_SOURCES = {
    "m4a.c", "libc_memcpy.c", "libc_memset.c", "libgcc_dp.c", "libgcc_fp.c",
    "librfu_intr.c", "librfu_rfu.c", "librfu_sio32id.c", "librfu_stwi.c",
} | {f"libm_{stem}.c" for stem in (
    "e_acos", "e_asin", "e_fmod", "e_rem_pio2", "e_sqrt", "k_cos", "k_rem_pio2", "k_sin", "k_tan",
    "s_atan", "s_copysign", "s_cos", "s_fabs", "s_floor", "s_isnan", "s_matherr", "s_scalbn", "s_sin",
    "s_tan", "w_acos", "w_asin", "w_fmod",
)}


def slug(name):
    """Snake case, including acronym boundaries such as RFU_Init and CRC32."""
    name = re.sub(r"([A-Z]+)([A-Z][a-z])", r"\1_\2", name)
    name = re.sub(r"([a-z0-9])([A-Z])", r"\1_\2", name)
    return re.sub(r"[^a-z0-9]+", "_", name.lower()).strip("_")


def new_filename(name, address):
    return (f"sub_{address:08X}" if ADDRESS.fullmatch(name) else slug(name)) + ".c"


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
            return parts[-1] in UPSTREAM_SOURCES
    return False


@lru_cache(maxsize=4)
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
    return scan(root)


def locate(value, root=ROOT):
    return manifest(root).locate(value)


def resolve(value, root=ROOT):
    """Resolve a repo path, file basename, symbol name, or ROM address."""
    path = Path(root) / value
    if path.is_file():
        return path.relative_to(root).as_posix() if path.is_relative_to(root) else str(path)
    name = Path(value).name
    stem = Path(name).stem
    symbols = symbol_names(root)
    address = address_of(stem, {name: address for address, name in symbols.items()})
    if address is not None:
        directory = Path("src") / directory_for(address, root)
        named = symbols.get(address, f"sub_{address:08X}")
        for filename in (new_filename(named, address), f"sub_{address:08X}.c"):
            candidate = directory / filename
            if (Path(root) / candidate).is_file():
                return candidate.as_posix()
    entry = locate(stem, root)
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


@lru_cache(maxsize=4)
def groups(root=ROOT):
    from progress import category_of, load_categories

    symbols = Path(root) / "symbols"
    if not (symbols / "progress_subsystems.csv").is_file():
        return (), (), ()
    categories = load_categories(symbols / "progress_categories.csv")
    starts, ends, directories = [], [], []
    for table in ("progress_subsystems.csv", "progress_libraries.csv"):
        with (symbols / table).open(newline="") as stream:
            for row in csv.DictReader(stream):
                start, end = int(row["start"], 16), int(row["end"], 16)
                title = row["name"]
                if table == "progress_libraries.csv":
                    directory = Path("lib", LIBRARY_DIRS.get(title, slug(title)))
                else:
                    category = category_of(categories, row | {"start": start})
                    category = CATEGORY_DIRS.get(category, category)
                    unit = slug(re.sub(r"\s+\(tentative\)$", "", title))
                    directory = Path(category) if unit == category else Path(category, unit)
                starts.append(start)
                ends.append(end)
                directories.append(directory)
    return tuple(zip(*sorted(zip(starts, ends, directories))))


def directory_for(address, root=ROOT):
    starts, ends, directories = groups(root)
    index = bisect_right(starts, address) - 1
    if index >= 0 and address < ends[index]:
        return directories[index]
    return Path("other", "unclassified")


def new_source(address, name, root=ROOT):
    return (Path("src") / directory_for(address, root) / new_filename(name, address)).as_posix()


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
