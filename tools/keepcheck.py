"""Keep a source rewrite only when the unit still matches: the gate shared by the cleanup tools.

tools/inline_twins.py, tools/rawfield.py and tools/fold_literals.py each propose an edit to a
src/ unit; `Unit.attempt(text)` writes it and keeps it only if

* tools/check.py reports MATCH,
* agbcc prints no warning it did not print before (a member access whose type differs from
  the cast it replaced, a call through a helper with another prototype), and
* a unit with `#ifdef NONMATCHING` drafts still compiles with -DNONMATCHING, adding no warning
  (check.py never compiles the drafts, so this is their only check).

Otherwise the file is put back byte for byte. `Unit.finish()` then runs clang-format once (only
when the file was format-clean before, so nothing else in it moves) and checks again; attempts
skip formatting so a tool's offsets into the text stay valid between them. `src_files` lists the units a tree-wide run
visits: src/**/*.c minus src/lib (SDK code) and the paths in a skip file (one per line, the
files other branches own).
"""
from __future__ import annotations

import collections
import glob
import os
import re
import subprocess
import sys
import tempfile

TOOLS = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(TOOLS)
sys.path.insert(0, TOOLS)
from source_layout import is_upstream
FORMAT = os.path.join(ROOT, "build", "venv", "bin", "clang-format")
WARNING = re.compile(r"^[^:\n]*:\d+: warning: (.*)$", re.M)


def read_skip(path):
    if not path or not os.path.exists(path):
        return set()
    return {ln.strip() for ln in open(path) if ln.strip() and not ln.startswith("#")}


def src_files(paths=None, skip=()):
    """Units to visit: PATHS (files or directories) or all of src/, without src/lib and SKIP."""
    out = []
    for p in paths or ["src"]:
        full = p if os.path.isabs(p) else os.path.join(ROOT, p)
        if os.path.isdir(full):
            out += sorted(glob.glob(os.path.join(full, "**", "*.c"), recursive=True))
        else:
            out.append(full)
    rels = [os.path.relpath(f, ROOT) for f in out]
    return [r for r in dict.fromkeys(rels) if not is_upstream(r) and r not in set(skip)]


def check(rel):
    """(matched, last line of tools/check.py's output)."""
    r = subprocess.run([sys.executable, os.path.join(TOOLS, "check.py"), rel, "--quiet"], cwd=ROOT,
                       capture_output=True, text=True)
    lines = (r.stdout.strip() or r.stderr.strip() or "?").splitlines()
    matched = r.returncode == 0 and ": MATCH (" in r.stdout
    errors = re.findall(r"^stdin:\d+: (?!warning:).*", r.stderr, re.M)
    return matched, (errors[0] if errors and not matched else lines[-1])[:200]


def compile_warnings(rel, nonmatching=False):
    """Counter of agbcc's warning messages (line numbers dropped), or None if it does not compile."""
    import build
    defines = ["-DNONMATCHING"] if nonmatching else []
    pre = subprocess.run(build.CPP + ["-P", "-nostdinc", "-undef"] + defines + ["-I", "include", "-iquote", ".", rel],
                         cwd=ROOT, capture_output=True, text=True)
    if pre.returncode:
        return None
    with tempfile.TemporaryFile(mode="w+") as inp:
        inp.write(pre.stdout)
        inp.seek(0)
        r = subprocess.run([build.compiler_for(rel)] + build.cflags_for(rel) + ["-o", os.devnull], stdin=inp,
                           cwd=ROOT, capture_output=True, text=True)
    if r.returncode or re.search(r":\d+: (?:error|parse error)|\berror:", r.stderr):
        return None
    return collections.Counter(WARNING.findall(r.stderr))


def formatted(text):
    if not os.path.exists(FORMAT):
        return None
    r = subprocess.run([FORMAT, "--assume-filename=x.c"], input=text, cwd=ROOT, capture_output=True, text=True)
    return r.stdout if r.returncode == 0 else None


def has_drafts(text):
    return re.search(r"^\s*#\s*if(?:def\s+NONMATCHING|\s+defined\s*\(?\s*NONMATCHING)", text, re.M) is not None


class Unit:
    """One src/ unit and the gate its rewrites pass through."""

    def __init__(self, rel):
        self.rel = rel
        self.path = os.path.join(ROOT, rel)
        with open(self.path) as source:
            self.text = source.read()
        self._clean = None
        self._base = {}
        self.last = ""

    @property
    def clean_format(self):
        """Whether the file was clang-format clean when first read."""
        if self._clean is None:
            self._clean = formatted(self.text) == self.text
        return self._clean

    def baseline(self, nonmatching):
        if nonmatching not in self._base:
            self._base[nonmatching] = compile_warnings(self.rel, nonmatching)
        return self._base[nonmatching]

    def attempt(self, text):
        """Write TEXT; keep it (and return True) only if the unit passes the gate."""
        if text == self.text:
            return False
        self.clean_format  # noqa: B018  (judged on the text as first read)
        old = self.text
        base = self.baseline(False)
        base_nm = self.baseline(True) if has_drafts(old) else None
        with open(self.path, "w") as f:
            f.write(text)
        ok, self.last = check(self.rel)
        if ok:
            ok = self._no_new_warnings(base, compile_warnings(self.rel), "warning")
        if ok and has_drafts(old):
            ok = self._no_new_warnings(base_nm, compile_warnings(self.rel, True), "NONMATCHING")
        if not ok:
            with open(self.path, "w") as f:
                f.write(old)
            return False
        self.text = text
        self._base = {}
        return True

    def finish(self):
        """clang-format the kept text (if the file was clean before) and confirm it still matches."""
        if not self.clean_format:
            return
        text = formatted(self.text)
        if text and text != self.text:
            self.attempt(text)

    def _no_new_warnings(self, before, after, what):
        if after is None:
            self.last = f"{what}: does not compile"
            return False
        if before is None:
            return True
        extra = after - before
        if extra:
            self.last = f"{what}: new {next(iter(extra))!r}"
            return False
        return True


def function_spans(text):
    """[(start, end, name)] of every top-level function body ({ ... }) in TEXT, outside comments."""
    spans = []
    code = blank_comments(text)
    for m in re.finditer(r"^(?![ \t#])[^;{}\n]*?\b(\w+)\s*\([^;{}]*\)\s*\{", code, re.M):
        i = m.end()
        depth = 1
        while depth and i < len(code):
            depth += {"{": 1, "}": -1}.get(code[i], 0)
            i += 1
        spans.append((m.start(), i, m.group(1)))
    return spans


def blank_comments(text):
    """TEXT with comments and string/char literals replaced by spaces (offsets kept)."""
    def blank(m):
        return re.sub(r"[^\n]", " ", m.group(0))
    return re.sub(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'', blank, text, flags=re.S)


def draft_ranges(text):
    """[(start, end)] of `#ifdef NONMATCHING` ... `#else`/`#endif` blocks (the code check.py skips)."""
    out = []
    for m in re.finditer(r"^[ \t]*#[ \t]*if(?:def[ \t]+NONMATCHING|[ \t]+defined[ \t]*\(?[ \t]*NONMATCHING)\b.*$",
                         text, re.M):
        depth, i = 1, m.end()
        for d in re.finditer(r"^[ \t]*#[ \t]*(if|ifdef|ifndef|else|elif|endif)\b", text[i:], re.M):
            k = d.group(1)
            if k.startswith("if"):
                depth += 1
            elif k == "endif":
                depth -= 1
            elif depth == 1:
                depth = 0
            if depth == 0:
                out.append((m.start(), i + d.start()))
                break
    return out


def in_ranges(pos, ranges):
    return any(a <= pos < b for a, b in ranges)
