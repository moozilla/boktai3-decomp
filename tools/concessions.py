"""Matching concessions: C that matches only because it steers the compiler.

Phase 3 (docs/ROADMAP.md) removes them. tools/progress.py scores every matched function by
the share of its lines that hold one (decomp.dev counts a function as matched only when it has
none), tools/scope/land.py refuses a match that adds one, and both read the kinds from here.

Scored kinds, in two groups:

* Never the original's C, so no comment excuses them: `pin` (`register T x asm("r4")`),
  `operand` (`asm("" : "+r"(x))`), `clobber` (`asm("" ::: "r4")`), `barrier` (`asm("")`,
  `asm("" ::: "memory")`).
* Judgment calls: `debug-nop` (DEBUG_NOP()), `empty-loop` (`do { } while (0)`), `empty-if`
  (an `if` with nothing in its body), `single-use-macro` (a statement macro used at one site,
  tools/statement_macros.py), `volatile` (a volatile local, a `T *volatile` or a
  `(volatile T *)&x` cast), `alias` (an `extern ... asm("Symbol")` declaration that gives a
  function a second prototype), `cflags` (a `// CFLAGS:` line other than the default flags,
  outside src/lib) and `raw-offset` (`*(u16 *)((u8 *)p + 0x172)`: a constant byte offset where
  a struct field belongs; the original's C named its fields, and the form can steer cse the way
  a field access doesn't). Each is a concession unless a comment on the same line, or alone on
  the line above, says why the original plausibly had it:

      DEBUG_NOP();  // concession-ok(debug-nop): the release build kept this debug hook

  The reason is required, and `lint` (run by tests/test_concessions.py) rejects an allow comment
  with an unknown or never-excusable kind, no reason, or nothing to excuse.

Tracked kinds are counted for the record (PROGRESS.md) but don't lower a function's score:
`goto` (accepted until phase 4 cleanup). A byte offset that varies (`(u8 *)table + offset`) is
ordinary C and isn't counted.

Library sources (src/lib, the SDK's own code) are exempt from the judgment-call kinds.
"""
from __future__ import annotations

import re
from dataclasses import dataclass

HARD = ("pin", "operand", "clobber", "barrier")
EXCUSABLE = ("debug-nop", "empty-loop", "empty-if", "single-use-macro", "volatile", "alias", "cflags", "raw-offset")
SCORED = HARD + EXCUSABLE
TRACKED = ("goto",)
LIBRARY = ("src/lib/", "include/gba/")
DEFAULT_CFLAGS = {"-O2", "-mthumb-interwork"}

_REG = r"(?:r\d+|sl|fp|ip|sb|lr)"
# A register declaration can name its register through a function-like macro parameter.
REGISTER_BINDING = r'\b(?:asm|__asm__)\s*\(\s*(?:"\w+"|[A-Za-z_]\w*)\s*\)'
# an asm("") statement; its operands decide the kind
_ASM_EMPTY = re.compile(r'\b(?:asm|__asm__)\s*(?:volatile\s*|__volatile__\s*)?\(\s*""\s*(?P<rest>(?:[^;()]|\([^()]*\))*)\)')
_REGEX = {
    "pin": re.compile(r'\bregister\b[^;{}]*?' + REGISTER_BINDING + r'|\b(?:asm|__asm__)\s*\(\s*"%s"\s*\)' % _REG),
    "debug-nop": re.compile(r"\bDEBUG_NOP\s*\(\s*\)"),
    "empty-loop": re.compile(r"\bdo\s*\{\s*\}\s*while\s*\(\s*0\s*\)"),
    # an if with nothing in its body; one whose body is DEBUG_PRINT() (include/debug.h) is ordinary C
    "empty-if": re.compile(r"\bif\s*\((?:[^()]|\((?:[^()]|\([^()]*\))*\))*\)\s*(?:\{\s*\}|;)"),
    "volatile": re.compile(
        r"(?<![(*\w])volatile\s+(?:const\s+)?(?:unsigned\s+|signed\s+|struct\s+)?\w+\s+(?:const\s+)?\w+\s*[=;,\[]"
        r"|\*\s*volatile\b"
        r"|\(\s*(?:volatile\s+(?:const\s+)?\w+|v[us](?:8|16|32))\s*\*\s*\)\s*&\s*(?!REG_)"),
    "goto": re.compile(r"\bgoto\b"),
    "raw-offset": re.compile(r"\*\s*\(\s*(?:const\s+)?(?:volatile\s+)?(?:v?[us](?:8|16|32)|s?int|void)\s*\*\s*\)\s*"
                             r"\(\s*\(\s*(?:const\s+)?u8\s*\*\s*\)\s*[\w>.\-\[\]]+\s*\+\s*(?:0x[0-9A-Fa-f]+|\d+)\s*\)"),
}
_ALIAS = re.compile(r'^[^\n#]*\b(\w+)\s*\([^;{}]*\)\s*(?:asm|__asm__)\s*\(\s*"[A-Za-z_]\w*"\s*\)\s*;', re.M)
_CFLAGS = re.compile(r"^// CFLAGS: (.*)$", re.M)
_ALLOW = re.compile(r"concession-ok\(([\w,\s-]*)\)\s*:?\s*(.*?)\s*(?:\*/|$)", re.M)
_COMMENTS = re.compile(r'"(?:\\.|[^"\\\n])*"|/\*.*?\*/|//[^\n]*', re.S)


@dataclass(frozen=True)
class Site:
    line: int  # 1-based, in the text scanned
    kind: str
    excused: bool = False


def strip(text: str) -> str:
    """TEXT with comments blanked out (newlines kept, so line numbers hold)."""
    def blank(m):
        s = m.group(0)
        return s if s.startswith('"') else re.sub(r"[^\n]", " ", s)
    return _COMMENTS.sub(blank, text)


def _line(text: str, pos: int) -> int:
    return text.count("\n", 0, pos) + 1


def allows(text: str) -> dict[int, dict[str, str]]:
    """{line: {kind: reason}}: the lines each concession-ok comment covers (its own line, and the
    next line when the comment stands alone)."""
    out: dict[int, dict[str, str]] = {}
    lines = text.splitlines()
    for m in _ALLOW.finditer(text):
        line = _line(text, m.start())
        kinds = [k.strip() for k in m.group(1).split(",") if k.strip()]
        before = lines[line - 1][:lines[line - 1].find("concession-ok")] if line <= len(lines) else ""
        covered = [line]
        if re.fullmatch(r"\s*(//|/\*)\s*", before):
            covered.append(line + 1)
        for ln in covered:
            for k in kinds:
                out.setdefault(ln, {})[k] = m.group(2)
    return out


def is_library(path) -> bool:
    return path is not None and any(str(path).replace("\\", "/").find(p) >= 0 for p in LIBRARY)


def asm_kind(rest: str) -> str:
    """operand, clobber or barrier, for the text after the "" of an asm("") statement."""
    parts = rest.split(":")
    if len(parts) > 1 and "(" in ":".join(parts[1:3]):
        return "operand"
    if len(parts) > 3 and re.search(r'"%s"' % _REG, parts[3]):
        return "clobber"
    return "barrier"


def sites(text: str, path=None, *, single_use=frozenset(), kinds=SCORED + TRACKED) -> list[Site]:
    """Every concession in TEXT (a function body, a unit or a header), excused or not.
    SINGLE_USE names the statement macros used at only one site in the tree."""
    code = strip(text)
    ok = allows(text)
    library = is_library(path)
    found: list[Site] = []

    def add(pos, kind):
        line = _line(code, pos)
        excused = kind in EXCUSABLE and (library or kind in ok.get(line, {}))
        found.append(Site(line, kind, excused))

    for m in _ASM_EMPTY.finditer(code):
        kind = asm_kind(m.group("rest"))
        if kind in kinds:
            add(m.start(), kind)
    for kind, rx in _REGEX.items():
        if kind in kinds:
            for m in rx.finditer(code):
                add(m.start(), kind)
    if "single-use-macro" in kinds and single_use:
        rx = re.compile(r"\b(" + "|".join(map(re.escape, sorted(single_use))) + r")\s*\(")
        for m in rx.finditer(code):
            if not re.match(r"[ \t]*#[ \t]*define\b", code[code.rfind("\n", 0, m.start()) + 1:m.start()]):
                add(m.start(), "single-use-macro")
    return sorted(found, key=lambda s: (s.line, s.kind))


def aliases(text: str, path=None) -> dict[str, bool]:
    """{name: excused} for the functions TEXT declares with an asm("Symbol") label."""
    code = strip(text)
    ok = allows(text)
    return {m.group(1): is_library(path) or "alias" in ok.get(_line(code, m.start()), {})
            for m in _ALIAS.finditer(code)}


def cflags(text: str, path=None) -> Site | None:
    """The unit's // CFLAGS: override as a site, or None when it has the default flags."""
    m = _CFLAGS.search(text)
    if not m or set(m.group(1).split()) <= DEFAULT_CFLAGS | {"-fhex-asm"} and \
            DEFAULT_CFLAGS <= set(m.group(1).split()):
        return None
    line = _line(text, m.start())
    return Site(line, "cflags", is_library(path) or "cflags" in allows(text).get(line, {}))


def counted(found) -> list[Site]:
    """The sites that lower a score: scored kinds, not excused."""
    return [s for s in found if s.kind in SCORED and not s.excused]


def body_lines(body: str) -> int:
    """Lines of code in a function body or macro: non-blank once comments are gone."""
    return sum(1 for ln in strip(body).splitlines() if ln.strip() and ln.strip() not in ("{", "}"))


def score(body: str, lines_with_concessions: int) -> float:
    """A matched function's progress percent: the share of its code lines without a concession.
    Below 100 whenever there is one."""
    if not lines_with_concessions:
        return 100.0
    n = max(body_lines(body), lines_with_concessions)
    return min(round(100 * (1 - lines_with_concessions / n), 1), 99.9)


def lint(text: str, path=None, *, single_use=frozenset()) -> list[str]:
    """Problems with TEXT's concession-ok comments: unknown kinds, kinds no comment can excuse,
    a missing reason, or a comment with nothing to excuse."""
    problems = []
    used: dict[int, set[str]] = {}
    for s in sites(text, path, single_use=single_use, kinds=EXCUSABLE):
        used.setdefault(s.line, set()).add(s.kind)
    c = cflags(text, path)
    if c:
        used.setdefault(c.line, set()).add("cflags")
    code = strip(text)
    for m in _ALIAS.finditer(code):
        used.setdefault(_line(code, m.start()), set()).add("alias")
    for m in _ALLOW.finditer(text):
        line = _line(text, m.start())
        where = f"{path or '<text>'}:{line}"
        kinds = [k.strip() for k in m.group(1).split(",") if k.strip()]
        if not kinds:
            problems.append(f"{where}: concession-ok() names no kind")
        for k in kinds:
            if k in HARD:
                problems.append(f"{where}: {k} is never the original's C, so no comment excuses it")
            elif k not in EXCUSABLE:
                problems.append(f"{where}: unknown concession kind {k!r} (one of {', '.join(EXCUSABLE)})")
            elif k not in used.get(line, set()) | used.get(line + 1, set()):
                problems.append(f"{where}: concession-ok({k}) has no {k} on this line or the next to excuse")
        if len(m.group(2)) < 10:
            problems.append(f"{where}: concession-ok needs a reason: why the original plausibly had this")
    return problems
