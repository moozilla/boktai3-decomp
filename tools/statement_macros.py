"""Single-use statement macros: a `do { ... } while (0)` macro used at only one site.

A statement macro is ordinary C when the original plausibly had it, and the usual evidence is
that several functions share it (ARMOR_PLAYER_DELTAS is used by four Armor twins). A macro
that wraps one statement at one site, written only to double that statement's refs in flow
(scope README fact 26), is a priority knob in disguise and counts as a barrier, like an empty
`do { } while (0)`. tools/progress.py, tools/scope/land.py and the matchsearch evaluator count
it as one.
"""
from __future__ import annotations

import functools
import os
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ROOTS = ("src", "include")
# library and SDK sources are the original authors' code: their macros are never knobs
LIBRARY = ("src/lib", "include/gba")

_COMMENTS = re.compile(r'"(?:\\.|[^"\\\n])*"|/\*.*?\*/|//[^\n]*', re.S)
_DEFINE = re.compile(r'^[ \t]*#[ \t]*define[ \t]+(\w+)\(((?:[^\n]*\\\n)*[^\n]*)', re.M)
_DO_WHILE = re.compile(r'\bdo\b.*\bwhile\s*\(\s*0\s*\)', re.S)


def _strip(text: str) -> str:
    return _COMMENTS.sub(lambda m: m.group(0) if m.group(0).startswith('"') else " ", text)


def scan(text: str) -> tuple[set[str], dict[str, int]]:
    """(statement macros TEXT defines, {name: uses}) for one file; a definition is not a use."""
    text = _strip(text)
    defs = {m.group(1) for m in _DEFINE.finditer(text) if _DO_WHILE.search(m.group(2))}
    uses: dict[str, int] = {}
    heads = {m.start(1) for m in _DEFINE.finditer(text)}
    for m in re.finditer(r'\b([A-Za-z_]\w*)\s*\(', text):
        if m.start(1) not in heads:
            uses[m.group(1)] = uses.get(m.group(1), 0) + 1
    return defs, uses


@functools.lru_cache(maxsize=16384)
def _scan_file(path: str, mtime_ns: int, ctime_ns: int, size: int):
    defs, uses = scan(Path(path).read_text(errors="replace"))
    return frozenset(defs), tuple(uses.items())


def _sources(root: Path, exclude: Path | None):
    """File identities for the live tree. A long-running search can see additions and edits."""
    root = root.resolve()
    skip = os.fspath(Path(exclude).resolve()) if exclude else None
    for sub in ROOTS:
        for directory, _, files in os.walk(root / sub):
            folder = os.path.realpath(directory)
            library = Path(directory).relative_to(root).as_posix() + "/"
            count_defs = not any(library.startswith(lib + "/") for lib in LIBRARY)
            for name in sorted(files):
                if not name.endswith((".c", ".h")):
                    continue
                path = os.path.join(folder, name)
                if skip and (path == skip or os.path.islink(path) and os.path.realpath(path) == skip):
                    continue
                stat = os.stat(path)
                yield path, stat.st_mtime_ns, stat.st_ctime_ns, stat.st_size, count_defs


@functools.lru_cache(maxsize=2)
def _counts(sources):
    defs: set[str] = set()
    uses: dict[str, int] = {}
    for path, mtime_ns, ctime_ns, size, count_defs in sources:
        defined, calls = _scan_file(path, mtime_ns, ctime_ns, size)
        if count_defs:
            defs.update(defined)
        for name, count in calls:
            uses[name] = uses.get(name, 0) + count
    return defs, uses


def corpus(root: Path = ROOT, exclude: Path | None = None) -> tuple[set[str], dict[str, int]]:
    """Statement macros defined and all call-like uses across ROOT's src/ and include/."""
    defs, uses = _counts(tuple(_sources(Path(root), exclude)))
    return defs.copy(), uses.copy()


def single_use_names(root: Path = ROOT) -> frozenset[str]:
    """Statement macros used at exactly one site across the current tree."""
    defs, uses = corpus(Path(root))
    return frozenset(name for name in defs if uses.get(name, 0) == 1)


def single_use_in(text: str, exclude: Path | None = None, root: Path = ROOT) -> list[str]:
    """Statement macros TEXT uses whose only use in the tree is that one site. EXCLUDE is the
    file TEXT will replace, so its old copy on disk is not counted."""
    defs, uses = corpus(root, exclude)
    tdefs, tuses = scan(text)
    found = []
    for name in sorted(defs | tdefs):
        here = tuses.get(name, 0)
        if here == 1 and uses.get(name, 0) == 0:
            found.append(name)
    return found
