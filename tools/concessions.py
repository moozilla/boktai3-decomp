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
  tools/statement_macros.py), `volatile` (a volatile local, including the `vu32`/`vs16`
  family of typedefs, a `T *volatile` or a
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

Errors and warnings. Run as a linter (`python3 tools/concessions.py [PATH...]`), every unexcused
scored site is an **error**. A **warning** is C a person plausibly would not have written, found by
its spelling, so each one is a question, not a verdict:

* `dead-code`: a call, store or branch the compiler deleted as unreachable after working out a
  condition the C left open (`x < 0` on a value that came from an unsigned halfword). People do
  write dead code by accident; an arm that looks like a real gap in the algorithm can be excused.
* `proved-zero`: an operation an accepted compiler transformation proved is always zero
  (`(u16)x >> 16`, `(x & 31) >> 5`); rejected speculative combinations do not count.
  These two come from the compiler, not from spelling (tools/deadcode.py; they need scope's
  instrumented agbcc, tools/scope/build_agbcc.sh), so respelling the C does not hide them.
* `once-loop`: a `do { ... } while (0)` with a body, outside a macro. Usually the expansion of a
  macro of the original's: give the macro a name in a header and the warning goes.
* `stmt-expr`: a `({ ... })` outside a macro, or an expression macro that wraps a single
  assignment in a one-pass loop before returning the assigned temporary.
* `const-local`: a local named for a number (`zero`, `one`, `two`, `zeroX`, `four`, ...), which
  only holds that constant. A `goto` label or `return` operand is not a declaration.
* `literal-local`: any other local that is initialized to a literal and never changed
  (`u32 mask = 0x17;` then `if (mask & flags)`). A person does name a constant now and then, so
  this is the weakest kind: write the literal where it is used when that matches, and leave
  the rest for the owner to rule on. A loop counter or anything reassigned is not one.
* `addr-deref`: `*(&x)` in place of `x`.
* `placeholder-name`: a local named `new_var`, `dummy...` or `fake...`.
* `volatile-view`: a pointer to volatile that isn't a hardware register (the `volatile` kind only
  covers objects and `&x` casts). A global an interrupt writes is volatile in its header instead.
* `local-union`: a union defined inside a function. Two views of one value are ordinary C; two
  unrelated values sharing storage are not.

Warnings don't lower a score. The order of work is errors to zero, then warnings to zero, adding
none. A warning is silenced the way a linter's is, by a `concession-ok(kind): reason` comment, and
the reason has to argue that the original authors wrote it this way ("the SDK's DmaSet macro reads
the register back"). A reason that talks about the compiler (registers, allocation, reload,
matching) is an admission, not a justification, and `lint` rejects it.
"""
from __future__ import annotations

import os
import re
from dataclasses import dataclass

HARD = ("pin", "operand", "clobber", "barrier")
EXCUSABLE = ("debug-nop", "empty-loop", "empty-if", "single-use-macro", "volatile", "alias", "cflags", "raw-offset")
SCORED = HARD + EXCUSABLE
COMPILER = ("dead-code", "proved-zero")  # found by tools/deadcode.py, not by sites()
WARN = COMPILER + ("once-loop", "stmt-expr", "const-local", "literal-local", "addr-deref",
        "placeholder-name", "volatile-view", "local-union")
TRACKED = ("goto",)
# words that make a concession-ok reason an admission of steering rather than a claim about the original
ADMISSION = re.compile(
    r"\b(?:alloc\w*|regalloc|registers?|reload\w*|regmove|combine|priorit\w*|match\w*|steer\w*|spill\w*|"
    r"lifetimes?|codegen|pseudo\w*|jump2|g?cse|scaffold\w*|pins?|pinned|permut\w*|byte-identical|r\d{1,2}|"
    r"the compiler (?:needs|wants|picks|keeps|puts))\b", re.I)
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
# a local named for a number: zero, one, zeroX, two_, four2 (not zeroed, oneshot, tenth)
_NUMBER_NAME = (r"(?:zero|one|two|three|four|five|six|seven|eight|nine|ten|sixteen|minusOne|negOne|allOnes)"
                r"(?:[A-Z_0-9]\w*)?")
_LITERAL = r"-?(?:0[xX][0-9A-Fa-f]+|\d+)[uUlL]*"
_DECL_TYPE = r"(?:const\s+)?(?:unsigned\s+|signed\s+)?(?!v[us]\d|volatile\b|goto\b|return\b|static\b|else\b|case\b)\w+\s+\*?"
_LITERAL_LOCAL = re.compile(r"(?m)^[ \t]+" + _DECL_TYPE + r"(\w+)\s*=\s*" + _LITERAL + r"\s*;")
_WARN_REGEX = {
    "stmt-expr": re.compile(r"\(\s*\{"),
    "const-local": re.compile(r"(?m)^[ \t]+" + _DECL_TYPE + _NUMBER_NAME + r"\s*(?:=\s*" + _LITERAL + r"\s*)?;"),
    "addr-deref": re.compile(r"\*\s*\(\s*&\s*\w+\s*\)"),
    "placeholder-name": re.compile(r"(?m)^[ \t]+[\w \t]*?\w[ \t]+\**\s*(?:new_var\w*|dummy\w*|fake\w*)\s*[=;,\[]"),
    "volatile-view": re.compile(
        r"\(\s*(?:const\s+)?volatile\s+(?:const\s+)?\w+\s*\*\s*\)\s*(?!&|REG_|0x0?4[0-9A-Fa-f]{6}\b)[\w(*]"
        r"|(?m:^[ \t]+(?:const\s+)?volatile\s+(?:const\s+)?\w+\s*\*\s*\w+\s*[=;])"),
    "local-union": re.compile(r"(?m)^[ \t]+union\b[^;{}()]*\{"),
}
_ONCE_END = re.compile(r"\}\s*while\s*\(\s*0\s*\)")
_MACRO_DEF = re.compile(r"(?m)^[ \t]*#[ \t]*define(?:.*\\\n)*.*$")
_WRAPPED_EXPR_MACRO = re.compile(
    r"[ \t]*#[ \t]*define[ \t]+\w+\([^()\n]*\)\s*\(\s*\{"
    r"\s*(?:[A-Za-z_]\w*\s+)+(?:\*\s*)*(?P<temporary>[A-Za-z_]\w*)\s*;"
    r"\s*do\s*\{\s*(?P=temporary)\s*=(?!=)[^;{}]+;\s*\}\s*while\s*\(\s*0\s*\)\s*;"
    r"\s*(?P=temporary)\s*;\s*\}\s*\)\s*"
)
_ALIAS = re.compile(r'^[^\n#]*\b(\w+)\s*\([^;{}]*\)\s*(?:asm|__asm__)\s*\(\s*"[A-Za-z_]\w*"\s*\)\s*;', re.M)
_CFLAGS = re.compile(r"^// CFLAGS: (.*)$", re.M)
_ALLOW = re.compile(r"concession-ok\(([\w,\s-]*)\)\s*:?\s*(.*?)\s*(?:\*/|$)", re.M)
_COMMENTS = re.compile(r'"(?:\\.|[^"\\\n])*"|/\*.*?\*/|//[^\n]*', re.S)
_VOLATILE_TYPEDEF = re.compile(
    r"\bv(?:[us](?:8|16|32|64)|bool(?:8|16|32))\s+(?:const\s+)?[A-Za-z_]\w*\s*[=;,\[]")
_FUNCTION_BODY = re.compile(r"[A-Za-z_][\w\s\*]*?\b\w+\s*\([^;{]*\)\s*\{", re.S)
_FUNCTION_MACRO = re.compile(r"#\s*define\s+\w+\(")


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


def _volatile_typedef_locals(code: str):
    """Offsets of scalar volatile-typedef declarations in functions and macros.

    Top-level register layouts, globals and typedefs are not automatic locals.
    Pointer declarations are intentionally excluded, just as for volatile T *reg.
    """
    if not _VOLATILE_TYPEDEF.search(code):
        return
    from merge_unit import items

    offset = 0
    for item in items(code):
        start = code.find(item, offset)
        offset = start + len(item)
        if not (_FUNCTION_BODY.match(item.lstrip()) or _FUNCTION_MACRO.match(item.lstrip())):
            continue
        # String contents are data even when they spell a declaration.
        declarations = _COMMENTS.sub(lambda m: re.sub(r"[^\n]", " ", m[0]), item)
        for match in _VOLATILE_TYPEDEF.finditer(declarations):
            yield start + match.start()


def _warn_sites(code: str):
    """(offset, kind) for the warning kinds in CODE (comments already blanked)."""
    for macro in _MACRO_DEF.finditer(code):
        if _WRAPPED_EXPR_MACRO.fullmatch(macro.group(0).replace("\\\n", " ")):
            yield macro.start(), "stmt-expr"
    code = _MACRO_DEF.sub(lambda m: re.sub(r"[^\n]", " ", m.group(0)), code)
    for kind, rx in _WARN_REGEX.items():
        for m in rx.finditer(code):
            yield m.start(), kind
    for m in _LITERAL_LOCAL.finditer(code):
        name = m.group(1)
        if re.fullmatch(_NUMBER_NAME, name):
            continue  # const-local has it
        # the rest of the block the declaration is in
        depth, i = 0, m.end()
        while i < len(code):
            if code[i] == "{":
                depth += 1
            elif code[i] == "}":
                if depth == 0:
                    break
                depth -= 1
            i += 1
        n = re.escape(name)
        changed = re.search(r"\b%s\s*(?:[-+*/|&^%%]|<<|>>)?=(?!=)|\b%s\s*(?:\+\+|--)|(?:\+\+|--)\s*%s\b"
                            r"|(?:[(,=?:]|\breturn)\s*&\s*%s\b" % (n, n, n, n), code[m.end():i])
        if not changed:
            yield m.start(), "literal-local"
    for m in _ONCE_END.finditer(code):
        depth, i = 0, m.start()
        while i >= 0:
            if code[i] == "}":
                depth += 1
            elif code[i] == "{":
                depth -= 1
                if depth == 0:
                    break
            i -= 1
        if i >= 0 and code[i + 1:m.start()].strip():
            yield m.start(), "once-loop"


def sites(text: str, path=None, *, single_use=frozenset(), kinds=SCORED + WARN + TRACKED) -> list[Site]:
    """Every concession in TEXT (a function body, a unit or a header), excused or not.
    SINGLE_USE names the statement macros used at only one site in the tree."""
    code = strip(text)
    ok = allows(text)
    library = is_library(path)
    found: list[Site] = []
    volatile_spans = []

    def add(pos, kind):
        line = _line(code, pos)
        excused = kind in EXCUSABLE + WARN and (library or kind in ok.get(line, {}))
        found.append(Site(line, kind, excused))

    for m in _ASM_EMPTY.finditer(code):
        kind = asm_kind(m.group("rest"))
        if kind in kinds:
            add(m.start(), kind)
    for kind, rx in _REGEX.items():
        if kind in kinds:
            for m in rx.finditer(code):
                add(m.start(), kind)
                if kind == "volatile":
                    volatile_spans.append(m.span())
    if "volatile" in kinds:
        for pos in _volatile_typedef_locals(code):
            if not any(start <= pos < end for start, end in volatile_spans):
                add(pos, "volatile")
    if any(k in kinds for k in WARN) and not library:
        seen = set()
        for pos, kind in _warn_sites(code):
            if kind in kinds and (_line(code, pos), kind) not in seen:
                seen.add((_line(code, pos), kind))
                add(pos, kind)
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


def compiler_sites(path, text=None) -> list[Site]:
    """The dead-code and proved-zero sites on PATH's own lines (a unit under src/), from the
    instrumented compiler. Empty for headers and when the compiler isn't built."""
    import deadcode
    rel = str(path).replace("\\", "/")
    if not rel.endswith(".c") or is_library(rel) or not deadcode.available():
        return []
    if text is None:
        text = open(os.path.join(deadcode.ROOT, rel), errors="replace").read()
    ok = allows(text)
    seen = {(line, kind) for kind, file, line, _, _ in deadcode.unit(rel) if file == rel}
    return [Site(line, kind, kind in ok.get(line, {})) for line, kind in sorted(seen)]


def warnings(found) -> list[Site]:
    """The unexcused warning sites."""
    return [s for s in found if s.kind in WARN and not s.excused]


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


def lint(text: str, path=None, *, single_use=frozenset(), compiler=None) -> list[str]:
    """Problems with TEXT's concession-ok comments: unknown kinds, kinds no comment can excuse,
    a missing reason, or a comment with nothing to excuse."""
    problems = []
    used: dict[int, set[str]] = {}
    for s in sites(text, path, single_use=single_use, kinds=EXCUSABLE + WARN):
        used.setdefault(s.line, set()).add(s.kind)
    c = cflags(text, path)
    if c:
        used.setdefault(c.line, set()).add("cflags")
    code = strip(text)
    for m in _ALIAS.finditer(code):
        used.setdefault(_line(code, m.start()), set()).add("alias")
    for s in compiler or ():
        used.setdefault(s.line, set()).add(s.kind)
    for m in _ALLOW.finditer(text):
        line = _line(text, m.start())
        where = f"{path or '<text>'}:{line}"
        kinds = [k.strip() for k in m.group(1).split(",") if k.strip()]
        if not kinds:
            problems.append(f"{where}: concession-ok() names no kind")
        for k in kinds:
            if k in HARD:
                problems.append(f"{where}: {k} is never the original's C, so no comment excuses it")
            elif k not in EXCUSABLE + WARN:
                problems.append(f"{where}: unknown concession kind {k!r} (one of {', '.join(EXCUSABLE + WARN)})")
            elif k in COMPILER and compiler is None:
                pass  # only a compile can say whether there is one here
            elif k not in used.get(line, set()) | used.get(line + 1, set()):
                problems.append(f"{where}: concession-ok({k}) has no {k} on this line or the next to excuse")
        if len(m.group(2)) < 10:
            problems.append(f"{where}: concession-ok needs a reason: why the original plausibly had this")
        elif any(k in WARN for k in kinds) and ADMISSION.search(m.group(2)):
            problems.append(f"{where}: the reason is an admission ({ADMISSION.search(m.group(2)).group(0)!r}), not a "
                            "justification: say why the original authors wrote it this way, or remove the form")
    return problems


def main(argv=None) -> int:
    """Lint PATHs (default: src and include): `file:line: error|warning [kind] text`, then totals."""
    import argparse
    import sys
    from pathlib import Path
    root = Path(__file__).resolve().parent.parent
    sys.path.insert(0, str(root / "tools"))
    ap = argparse.ArgumentParser(description=main.__doc__)
    ap.add_argument("paths", nargs="*")
    ap.add_argument("--errors-only", action="store_true")
    ap.add_argument("--kind", action="append", help="only these kinds")
    ap.add_argument("--summary", action="store_true", help="totals per kind only")
    ap.add_argument("--no-compiler", action="store_true", help="skip dead-code and proved-zero (no compiles)")
    args = ap.parse_args(argv)
    from statement_macros import single_use_names
    once = frozenset(single_use_names(root))
    files = []
    for p in args.paths or [root / "src", root / "include"]:
        p = Path(p)
        files += sorted([*p.rglob("*.c"), *p.rglob("*.h")]) if p.is_dir() else [p]
    import deadcode
    use_compiler = not args.no_compiler
    if use_compiler and not deadcode.available():
        sys.exit("dead-code and proved-zero need scope's compiler: sh tools/scope/build_agbcc.sh (or pass --no-compiler)")
    if use_compiler:  # compile in parallel up front; the loop below reads the cache
        from concurrent.futures import ThreadPoolExecutor
        units = [str(f.resolve().relative_to(root)) for f in files
                 if f.suffix == ".c" and f.resolve().is_relative_to(root) and not is_library(f.resolve().relative_to(root))]
        with ThreadPoolExecutor(8) as pool:
            list(pool.map(deadcode.unit, units))
    totals: dict[str, int] = {}
    for f in files:
        text = f.read_text(errors="replace")
        rel = f.resolve().relative_to(root) if f.resolve().is_relative_to(root) else f
        lines = text.splitlines()
        found = sites(text, rel, single_use=once)
        flags = cflags(text, rel)
        compiled = compiler_sites(rel, text) if use_compiler else None
        for s in sorted(found + (compiled or []), key=lambda s: (s.line, s.kind)) + ([flags] if flags else []):
            if s.excused or s.kind in TRACKED or (args.kind and s.kind not in args.kind):
                continue
            level = "warning" if s.kind in WARN else "error"
            if level == "warning" and args.errors_only:
                continue
            totals[s.kind] = totals.get(s.kind, 0) + 1
            if not args.summary:
                print(f"{rel}:{s.line}: {level} [{s.kind}] {lines[s.line - 1].strip()[:100] if 0 < s.line <= len(lines) else ''}")
        for problem in lint(text, rel, single_use=once, compiler=compiled if f.suffix == ".c" else None):
            totals["bad-ignore"] = totals.get("bad-ignore", 0) + 1
            if not args.summary:
                print(f"{problem.split(': ', 1)[0]}: error [bad-ignore] {problem.split(': ', 1)[1]}")
    errors = sum(n for k, n in totals.items() if k not in WARN)
    warns = sum(n for k, n in totals.items() if k in WARN)
    for k in [*SCORED, "bad-ignore", *WARN]:
        if k in totals:
            print(f"{'warning' if k in WARN else 'error':8} {k:18} {totals[k]:6,}", file=sys.stderr)
    print(f"{errors:,} errors, {warns:,} warnings in {len(files):,} files", file=sys.stderr)
    return 1 if errors else 0


if __name__ == "__main__":
    raise SystemExit(main())
