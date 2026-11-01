#!/usr/bin/env python3
"""Find private inline helpers that copy a shared header helper.

  shared_inline.py FILE.c ...        report each static inline in FILE that resembles one in include/
  shared_inline.py --queue QUEUE.md  the same for every concession file a queue lists

Pinned matches often carry a private copy of a step the headers already share
(PlayerCue_StepSequence and its Ended variants in include/player_cue_sequence.h,
VulcanCannon_StepAnimation): the copy holds the pins, and calling the shared
helper instead drops them (EnemyD459_Update, Ooze_BeginAttack,
VulcanCannon_InitializeFromScript matched that way, unchanged otherwise). The
comparison is on the helper bodies' tokens with identifiers, register pins,
asm barriers and casts' types folded away, so renamed fields and pointer
spellings still line up; the score is difflib's ratio on those tokens.
"""
from __future__ import annotations

import argparse
import difflib
import functools
import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
HELPER = re.compile(r"^(?:static\s+inline|inline\s+static)\s+[^;{}()]*?\b(\w+)\s*\(([^)]*)\)\s*\{", re.M)
_TOKEN = re.compile(r"0x[0-9A-Fa-f]+|\d+|\w+|->|<<|>>|[<>=!&|+\-]=|&&|\|\||\S")
_KEYWORDS = {"if", "else", "return", "while", "do", "for", "switch", "case", "break"}
_NOISE = [
    re.compile(r"/\*.*?\*/|//[^\n]*", re.S),
    re.compile(r'\basm\s*\(\s*"[^"]*"[^;]*;'),                  # barriers
    re.compile(r'\s*\basm\s*\("r\d+"\)'),                         # register pins
    re.compile(r"\bregister\b|\bvolatile\b|\bconst\b"),
    re.compile(r"\(\s*(?:struct\s+)?\w+\s*\*+\s*\)"),             # pointer casts
    re.compile(r"\{\s*\}"),
]


def bodies(text: str) -> dict[str, str]:
    """{name: body text} for each static inline function TEXT defines."""
    out = {}
    for m in HELPER.finditer(text):
        depth, i = 1, m.end()
        while depth and i < len(text):
            depth += {"{": 1, "}": -1}.get(text[i], 0)
            i += 1
        out[m.group(1)] = text[m.end():i - 1]
    return out


def tokens(body: str) -> list[str]:
    for pat in _NOISE:
        body = pat.sub(" ", body)
    out = []
    for t in _TOKEN.findall(body):
        if t[0].isdigit():
            out.append(t if t.startswith("0x") else hex(int(t)))
        elif t[0].isalpha() or t[0] == "_":
            out.append(t if t in _KEYWORDS else "ID")
        else:
            out.append(t)
    return out


@functools.lru_cache(maxsize=None)
def shared(root: str = ROOT) -> tuple[tuple[str, str, tuple[str, ...]], ...]:
    """(header, name, tokens) for every static inline in include/."""
    rows = []
    for path in sorted(glob.glob(os.path.join(root, "include", "**", "*.h"), recursive=True)):
        rel = os.path.relpath(path, root)
        for name, body in bodies(open(path, errors="replace").read()).items():
            toks = tuple(tokens(body))
            if len(toks) >= 40:  # a one-line accessor matches everything
                rows.append((rel, name, toks))
    return tuple(rows)


def matches(text: str, root: str = ROOT, threshold: float = 0.6, top: int = 3):
    """[(private helper, [(score, header, shared helper)])] for TEXT's helpers that resemble a shared one."""
    found = []
    for name, body in bodies(text).items():
        mine = tokens(body)
        if len(mine) < 40:
            continue
        scored = []
        for header, other, toks in shared(root):
            if other == name:
                continue
            sm = difflib.SequenceMatcher(None, mine, toks, autojunk=False)
            if sm.real_quick_ratio() < threshold or sm.quick_ratio() < threshold:
                continue
            r = sm.ratio()
            if r >= threshold:
                scored.append((round(r, 3), header, other))
        if scored:
            found.append((name, sorted(scored, reverse=True)[:top]))
    return found


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("files", nargs="*")
    ap.add_argument("--queue", help="a queue table: check every listed src file still marked as a concession")
    ap.add_argument("--threshold", type=float, default=0.6)
    a = ap.parse_args(argv)
    files = list(a.files)
    if a.queue:
        for rel in re.findall(r"`(src/[^`]+\.c)`", open(a.queue).read()):
            path = os.path.join(ROOT, rel)
            if os.path.exists(path) and "MATCHING_CONCESSION" in open(path).read():
                files.append(rel)
    for f in dict.fromkeys(files):
        path = f if os.path.isabs(f) else os.path.join(ROOT, f)
        for name, rows in matches(open(path, errors="replace").read(), threshold=a.threshold):
            best = ", ".join(f"{h}:{o} {s:.2f}" for s, h, o in rows)
            print(f"{os.path.relpath(path, ROOT)}: {name} ~ {best}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
