#!/usr/bin/env python3
"""Replace a unit's private copy of a shared helper or struct with the shared one.

  inline_twins.py [PATH ...] [--skip build/skip_files.txt] [--dry-run] [--report FILE]

Units often carry their own `static inline` copy of a helper that include/ already shares, or
their own definition of a struct a header already has. A *twin* is exact:

* a helper: the same return type, the same parameter types in the same order, and the same
  body, after comments and whitespace are dropped, numbers are written one way (0x10 == 16) and
  parameters are renamed by position;
* a struct or union (`struct T { ... };` or `typedef struct { ... } T;`): the same name and the
  same members, names and types, after the same normalization. One with the same members under
  another name is reported, not applied: small layouts (three s32 coordinates) recur under many
  names by coincidence, and the name is the meaning.

A helper with several shared twins is applied only when one of them has the private copy's own
name or is the only one the unit already sees through its includes; otherwise it is reported
as ambiguous. A twin whose shared name has no Object_ prefix (`scale`, `add`) is reported, not
applied, in place of a private name that has one. A private helper's definition goes with the
comment lines directly above it (its description; a `confidence=` note stays); a struct's
comments stay.

For each twin the private definition goes, its uses are renamed to the shared name (the helper's
calls, `struct Old` or the typedef name), the header is included when the unit does not already
see it, and the change stays only when tools/check.py still reports MATCH with no new compiler
warning (tools/keepcheck.py). Nothing moves: the bodies are identical, so the compiler sees the
same function. Replacements repeat until none applies, so a helper that calls a private twin
becomes a twin itself once its callee is renamed.

A *near twin* has the same body with parameters renamed but differs in the signature (another
parameter order, another integer type, another return type) while taking the same pointer
types; those are reported for a person to judge and never changed. Headers are never edited.
"""
from __future__ import annotations

import argparse
import collections
import glob
import os
import re
import sys

TOOLS = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, TOOLS)
import keepcheck  # noqa: E402
from keepcheck import ROOT, blank_comments  # noqa: E402

HELPER = re.compile(r"^(?:static[ \t]+inline|inline[ \t]+static)\s+([^;{}()=]*?)\b(\w+)\s*\(([^()]*)\)\s*\{", re.M)
RECORD = re.compile(r"^(typedef[ \t]+)?(struct|union)(?:[ \t]+(\w+))?\s*\{", re.M)
TOKEN = re.compile(r"0[xX][0-9A-Fa-f]+[uUlL]*|\d+[uUlL]*|[A-Za-z_]\w*|->|<<=|>>=|<<|>>|\+\+|--|[-+*/%&|^!=<>]=|&&|\|\||\S")


def match_brace(code, i):
    """Index just past the brace that closes the one before I."""
    depth = 1
    while depth and i < len(code):
        depth += {"{": 1, "}": -1}.get(code[i], 0)
        i += 1
    return i


def number(t):
    m = re.match(r"(0[xX][0-9A-Fa-f]+|[1-9]\d*|0)([uUlL]*)$", t)
    if not m:
        return t  # an octal literal or a fraction's digits: compared as written
    return str(int(m.group(1), 0)) + m.group(2).upper()


def tokens(text):
    return [number(t) for t in TOKEN.findall(text)]


def param_names(params):
    """(names, types) of a parameter list; None if it cannot be read (function pointers)."""
    names, types = [], []
    params = params.strip()
    if params in ("", "void"):
        return names, types
    for p in params.split(","):
        toks = tokens(p)
        idents = [i for i, t in enumerate(toks) if re.match(r"[A-Za-z_]\w*$", t)]
        if not idents or len(toks) < 2 or toks == ["..."]:
            return None
        k = idents[-1]
        names.append(toks[k])
        types.append(" ".join(toks[:k] + toks[k + 1:]))
    return names, types


class Helper(collections.namedtuple("Helper", "name ret types body loose start end file")):
    """A static inline: its normalized signature and body (parameters renamed P0, P1, ...)."""

    @property
    def key(self):
        return (self.ret, tuple(self.types), self.body)


def helpers(text, path=""):
    code = blank_comments(text)
    out = []
    for m in HELPER.finditer(code):
        pn = param_names(m.group(3))
        if pn is None:
            continue
        names, types = pn
        end = match_brace(code, m.end())
        body = tokens(code[m.end():end - 1])
        index = {n: i for i, n in enumerate(names)}
        member = [i > 0 and body[i - 1] in ("->", ".") for i in range(len(body))]
        exact = " ".join(f"P{index[t]}" if t in index and not member[i] else t for i, t in enumerate(body))
        loose = " ".join("P" if t in index and not member[i] else t for i, t in enumerate(body))
        ret = " ".join(t for t in tokens(m.group(1)) if t not in ("static", "inline"))
        start = code.rfind("\n", 0, m.start()) + 1
        if end < len(text) and text[end] == "\n":
            end += 1
        out.append(Helper(m.group(2), ret, types, exact, loose, start, end, path))
    return out


class Record(collections.namedtuple("Record", "kind name typedef body start end file")):
    """A file-scope struct/union definition: `struct T {...};` (name T) or `typedef struct {...} T;`."""


def records(text, path=""):
    code = blank_comments(text)
    out = []
    for m in RECORD.finditer(code):
        end = match_brace(code, m.end())
        tail = re.match(r"\s*(\w*)\s*;", code[end:])
        if not tail:
            continue
        typedef, kind, tag = bool(m.group(1)), m.group(2), m.group(3)
        if typedef:
            if not tail.group(1) or tag:
                continue  # typedef struct T {...} U; spells two names: leave it
            name = tail.group(1)
        else:
            if not tag or tail.group(1):
                continue  # an untagged struct or one that also declares an object
            name = tag
        body = " ".join(tokens(code[m.end():end - 1]))
        stop = end + tail.end()
        if stop < len(text) and text[stop] == "\n":
            stop += 1
        out.append(Record(kind, name, typedef, body, m.start(), stop, path))
    return out


def shared_index(root=ROOT):
    """Helpers and records of every header in include/, keyed for lookup."""
    hk, hl, rk = collections.defaultdict(list), collections.defaultdict(list), collections.defaultdict(list)
    for path in sorted(glob.glob(os.path.join(root, "include", "**", "*.h"), recursive=True)):
        rel = os.path.relpath(path, os.path.join(root, "include"))
        text = open(path, errors="replace").read()
        for h in helpers(text, rel):
            hk[h.key].append(h)
            hl[h.loose].append(h)
        for r in records(text, rel):
            rk[(r.kind, r.typedef, r.body)].append(r)
    return hk, hl, rk


def rename(text, old, new, prefix=""):
    """TEXT with the identifier OLD (after PREFIX, e.g. 'struct ') renamed, outside comments and strings."""
    code = blank_comments(text)
    pat = re.compile((re.escape(prefix) + r"\s*" if prefix else r"(?<![\w.>])(?<!->)") + r"\b%s\b" % re.escape(old))
    out, last = [], 0
    for m in pat.finditer(code):
        out.append(text[last:m.start()])
        out.append((prefix + new) if prefix else new)
        last = m.end()
    out.append(text[last:])
    return "".join(out)


_SEEN = {}


def visible(rel, header):
    """Whether unit REL (as it is on disk now) already includes HEADER, directly or not."""
    import layoutmerge
    text = open(os.path.join(ROOT, rel)).read()
    if _SEEN.get(rel, (None,))[0] != text:
        pre = layoutmerge.preprocess(rel) or ""
        _SEEN[rel] = (text, set(re.findall(r'^#\s*\d+\s+"include/([^"]+)"', pre, re.M)))
    return header in _SEEN[rel][1]


def add_include(text, header):
    """TEXT with `#include "HEADER"` after the last include of the leading include block."""
    line = f'#include "{header}"\n'
    if line in text:
        return text
    pos, at, in_comment = 0, 0, False
    for ln in text.splitlines(keepends=True):
        s = ln.strip()
        if in_comment:
            in_comment = "*/" not in s
        elif s.startswith("#include"):
            at = pos + len(ln)
        elif s.startswith("/*"):
            in_comment = "*/" not in s
        elif s and not s.startswith("//"):
            break
        pos += len(ln)
    return text[:at] + line + text[at:]


def cut(text, start, end, comment=True):
    """TEXT without [START, END), leaving one blank line. COMMENT: also the comment lines directly
    above it (a helper's description), except a `confidence=` note, which names a function."""
    lines_above = text[:start].splitlines(keepends=True)
    while comment and lines_above and re.match(r"\s*(//|/\*.*\*/\s*$)", lines_above[-1]) \
            and "confidence=" not in lines_above[-1]:
        lines_above.pop()
    head = "".join(lines_above)
    tail = text[end:]
    if (head.endswith("\n\n") or not head) and tail.startswith("\n"):
        tail = tail[1:]
    return head + tail


def pointer_types(h):
    """The pointer parameters' types, in order: a near twin works on the same objects."""
    return sorted(t for t in h.types if "*" in t)


def generic(name):
    """A shared name without an Object_ prefix (scale, add): a file's local name that reached a header."""
    return "_" not in name.strip("_")


def plan(text, idx, drafts):
    """[(kind, private, shared candidates)] of exact twins, and near twins as report rows."""
    hk, hl, rk = idx
    exact, near = [], []
    for h in helpers(text):
        if keepcheck.in_ranges(h.start, drafts):
            continue
        same = [s for s in hk.get(h.key, ())]
        if same:
            exact.append(("helper", h, same))
        else:
            loose = [s for s in hl.get(h.loose, ()) if pointer_types(s) == pointer_types(h)]
            if loose:
                near.append(("helper", h, loose))
    for r in records(text):
        if keepcheck.in_ranges(r.start, drafts):
            continue
        same = rk.get((r.kind, r.typedef, r.body), [])
        if any(c.name == r.name for c in same):
            exact.append(("record", r, [c for c in same if c.name == r.name]))
        elif same:
            near.append(("record", r, same))
    return exact, near


def replace(text, kind, priv, shared):
    new = cut(text, priv.start, priv.end, comment=kind == "helper")
    if priv.name != shared.name:
        if kind == "helper" or priv.typedef:
            new = rename(new, priv.name, shared.name)
        else:
            new = rename(new, priv.name, shared.name, prefix=priv.kind + " ")
    return new


def process(rel, idx, dry=False, log=print):
    """Apply every exact twin in REL that keeps the match. Returns (applied, tried, report rows)."""
    unit = keepcheck.Unit(rel)
    applied, tried, report, failed = [], 0, [], set()
    near_seen = set()
    while True:
        drafts = keepcheck.draft_ranges(unit.text)
        exact, near = plan(unit.text, idx, drafts)
        for kind, h, cands in near:
            if h.name not in near_seen:
                near_seen.add(h.name)
                what = "same layout, other name" if kind == "record" else "near twin"
                report.append(f"{rel}: {what} {h.name} ~ " + ", ".join(f"{c.file}:{c.name}" for c in cands[:3])
                              + (f" (+{len(cands) - 3})" if len(cands) > 3 else ""))
        progress = False
        for kind, priv, cands in exact:
            if (kind, priv.name) in failed:
                continue
            if len(cands) > 1 and not any(c.name == priv.name for c in cands):
                seen = [c for c in cands if visible(rel, c.file)]
                if len(seen) == 1:
                    cands = seen  # the one the unit already includes
            if kind == "helper" and generic(cands[0].name) and not generic(priv.name) \
                    and not any(c.name == priv.name for c in cands):
                failed.add((kind, priv.name))
                report.append(f"{rel}: helper {priv.name} is a twin of {cands[0].file}:{cands[0].name}, "
                              "a generic name (left alone)")
                continue
            if len(cands) > 1 and not any(c.name == priv.name for c in cands):
                failed.add((kind, priv.name))
                report.append(f"{rel}: {kind} {priv.name} has {len(cands)} shared twins "
                              + ", ".join(f"{c.file}:{c.name}" for c in cands[:4]) + " (ambiguous, left alone)")
                continue
            cands = sorted(cands, key=lambda c: (c.name != priv.name, c.file))
            shared = cands[0]
            tried += 1
            new = replace(unit.text, kind, priv, shared)
            if not visible(rel, shared.file):
                new = add_include(new, shared.file)
            what = f"{priv.name} -> {shared.file}:{shared.name}"
            if dry:
                log(f"{rel}: would replace {kind} {what}")
                failed.add((kind, priv.name))
                continue
            if unit.attempt(new):
                applied.append(what)
                log(f"{rel}: replaced {kind} {what}")
                progress = True
                break
            failed.add((kind, priv.name))
            report.append(f"{rel}: twin {kind} {what} not applied ({unit.last})")
        if not progress:
            break
    if applied:
        unit.finish()
    return applied, tried, report


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("paths", nargs="*", help="units or directories (default: src/)")
    ap.add_argument("--skip", default=os.path.join(ROOT, "build", "skip_files.txt"),
                    help="file of paths to leave alone (default: build/skip_files.txt, if present)")
    ap.add_argument("--dry-run", action="store_true", help="list twins without editing")
    ap.add_argument("--report", help="append the review rows (near twins, failures) to this file")
    a = ap.parse_args(argv)
    idx = shared_index()
    total_applied = total_tried = 0
    rows = []
    for rel in keepcheck.src_files(a.paths, keepcheck.read_skip(a.skip)):
        applied, tried, report = process(rel, idx, a.dry_run)
        total_applied += len(applied)
        total_tried += tried
        rows += report
    for r in rows:
        print(r)
    if a.report:
        with open(a.report, "a") as f:
            f.write("".join(r + "\n" for r in rows))
    print(f"inline_twins: {total_applied} replaced of {total_tried} tried; {len(rows)} rows for review")
    return 0


if __name__ == "__main__":
    sys.exit(main())
