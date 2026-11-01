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


@functools.lru_cache(maxsize=None)
def _scan_file(path: str, mtime: float) -> tuple[frozenset[str], tuple[tuple[str, int], ...]]:
    defs, uses = scan(Path(path).read_text(errors="replace"))
    return frozenset(defs), tuple(uses.items())


def corpus(root: Path = ROOT, exclude: Path | None = None) -> tuple[set[str], dict[str, int]]:
    """Statement macros defined and all call-like uses across ROOT's src/ and include/."""
    skip = Path(exclude).resolve() if exclude else None
    defs: set[str] = set()
    uses: dict[str, int] = {}
    for sub in ROOTS:
        for dirpath, _, files in os.walk(Path(root) / sub):
            for f in files:
                if not f.endswith((".c", ".h")):
                    continue
                path = Path(dirpath, f)
                if skip and path.resolve() == skip:
                    continue
                d, u = _scan_file(str(path), path.stat().st_mtime)
                rel = path.relative_to(root).as_posix()
                if not any(rel.startswith(lib + "/") for lib in LIBRARY):
                    defs |= d
                for name, n in u:
                    uses[name] = uses.get(name, 0) + n
    return defs, uses


@functools.lru_cache(maxsize=None)
def _single_use_names(root: str) -> frozenset[str]:
    defs, uses = corpus(Path(root))
    return frozenset(name for name in defs if uses.get(name, 0) == 1)


def single_use_names(root: Path = ROOT) -> frozenset[str]:
    """Statement macros used at exactly one site across the whole tree (one scan per process)."""
    return _single_use_names(str(Path(root).resolve()))


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
