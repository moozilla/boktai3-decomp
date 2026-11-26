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
import hashlib
import json
import os
import re
import stat
from contextlib import suppress
from pathlib import Path
from tempfile import NamedTemporaryFile

from source_layout import is_upstream

ROOT = Path(__file__).resolve().parents[1]
ROOTS = ("src", "include")
# library and SDK sources are the original authors' code: their macros are never knobs


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
def _scan_file(path: str, inode: int, mtime_ns: int, ctime_ns: int, size: int):
    defs, uses = scan(Path(path).read_text(errors="replace"))
    return frozenset(defs), tuple(uses.items())


def _sources(root: Path, skip: str | None = None):
    """File identities for the live tree. A long-running search can see additions and edits."""
    root = root.resolve()
    for sub in ROOTS:
        for directory, _, files in os.walk(root / sub):
            folder = os.path.realpath(directory)
            for name in sorted(files):
                if not name.endswith((".c", ".h")):
                    continue
                path = os.path.join(folder, name)
                if path == skip:
                    continue
                info = os.lstat(path)
                target = path
                if stat.S_ISLNK(info.st_mode):
                    target = os.path.realpath(path)
                    if target == skip:
                        continue
                    info = os.stat(path)
                identity = path, info.st_ino, info.st_mtime_ns, info.st_ctime_ns, info.st_size
                yield identity, target, not is_upstream(path)


@functools.lru_cache(maxsize=2)
def _counts(sources):
    defs: dict[str, int] = {}
    uses: dict[str, int] = {}
    for identity, _, count_defs in sources:
        defined, calls = _scan_file(*identity)
        if count_defs:
            for name in defined:
                defs[name] = defs.get(name, 0) + 1
        for name, count in calls:
            uses[name] = uses.get(name, 0) + count
    return defs, uses


def _census(root, sources):
    cache = root / "build" / "statement-macros.json"
    cache_enabled = cache.parent.is_dir()
    if cache_enabled:
        fingerprint = hashlib.sha256(Path(__file__).read_bytes())
        for source in sources:
            fingerprint.update(repr(source).encode())
        key = fingerprint.hexdigest()
        try:
            saved = json.loads(cache.read_text())
            defs, uses = saved["definitions"], saved["uses"]
            if saved["fingerprint"] == key and all(
                isinstance(counts, dict) and all(type(n) is int and n > 0 for n in counts.values())
                for counts in (defs, uses)
            ):
                return defs, uses
        except (OSError, ValueError, KeyError, TypeError):
            pass

    defs, uses = _counts(sources)
    if cache_enabled:
        temporary = None
        try:
            with NamedTemporaryFile("w", dir=cache.parent, delete=False) as stream:
                temporary = Path(stream.name)
                json.dump({"fingerprint": key, "definitions": defs, "uses": uses}, stream)
            temporary.replace(cache)
            temporary = None
        except OSError:
            pass
        finally:
            if temporary is not None:
                with suppress(OSError):
                    temporary.unlink()
    return defs, uses


def corpus(root: Path = ROOT, exclude: Path | None = None) -> tuple[set[str], dict[str, int]]:
    """Statement macros defined and all call-like uses across ROOT's src/ and include/."""
    root = Path(root)
    skip = os.fspath(Path(exclude).resolve()) if exclude else None
    try:
        sources = tuple(_sources(root))
        defs, uses = _census(root, sources)
        if skip is not None:
            defs, uses = _except_source(defs, uses, sources, skip)
            return set(defs), uses
    except (OSError, KeyError):
        if skip is None:
            raise
        # A missing edit target or a file changed after inventorying the tree.
        defs, uses = _counts(tuple(_sources(root, skip)))
    return set(defs), uses.copy()


def _except_source(defs, uses, sources, skip):
    """Subtract the live contributions of the file the candidate replaces."""
    defs, uses = defs.copy(), uses.copy()
    for identity, target, count_defs in sources:
        if target != skip:
            continue
        defined, calls = _scan_file(*identity)
        if count_defs:
            for name in defined:
                defs[name] -= 1
                if not defs[name]:
                    del defs[name]
        for name, count in calls:
            uses[name] -= count
            if not uses[name]:
                del uses[name]
    return defs, uses


def single_use_names(root: Path = ROOT) -> frozenset[str]:
    """Statement macros used at exactly one site across the current tree."""
    defs, uses = corpus(root)
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
