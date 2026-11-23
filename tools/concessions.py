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
  tools/statement_macros.py), `volatile` (a volatile local or by-value parameter, including the
  `vu32`/`vs16` family of typedefs, a `T *volatile` or a
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
  One source check supplements the compiler: an assignment guarded by `(s32)x < 0` where `x` was
  just narrowed with `(u8)` or `(u16)`, which the compiler deletes as a register copy and so
  does not report.
* `proved-zero`: an operation an accepted compiler transformation proved is always zero
  (`(u16)x >> 16`, `(x & 31) >> 5`); rejected speculative combinations do not count.
  These two come from the compiler, not from spelling (tools/deadcode.py; they need scope's
  instrumented agbcc, tools/scope/build_agbcc.sh), so respelling the C does not hide them.
* `dead-local-update`: a block initializes a scalar and updates only that scalar using
  its own value and integer constants, then discards it.
* `once-loop`: a `do { ... } while (0)` with a body, outside a macro. Usually the expansion of a
  macro of the original's: give the macro a name in a header and the warning goes.
* `stmt-expr`: a `({ ... })` outside a macro, or an expression macro that wraps a single
  assignment in a one-pass loop before returning the assigned temporary.
* `const-local`: a local named for a number (`zero`, `one`, `two`, `zeroX`, `four`, ...), which
  only holds that constant, alone or in a grouped declaration (`u32 kind, zero;`), with or
  without `register`, or a parameter of a `static inline` named that way
  (`Restore(struct A *a, u32 kind, u32 zero)`). A `goto` label, a `return` operand or a struct
  member is not one.
* `literal-local`: any other local that is initialized to a literal (`0x17`, `-1`, `~8`) and never changed
  (`u32 mask = 0x17;` then `if (mask & flags)`), in any declarator of a declaration
  (`int i = 0, limit = 8;`). Also a local whose first use is a separate literal assignment in the
  declaration's own block (`u32 mask; ... mask = 0x17;`, reported at the assignment), and a local
  whose only use is a literal assignment inside an expression (`x & (mask = 0x7f)`). A person
  does name a constant now and then, so
  this is the weakest kind: write the literal where it is used when that matches, and leave
  the rest for the owner to rule on. A loop counter or anything reassigned is not one; a write to
  a member of the same name (`p->state = 1`) or to an inner local that shadows it is not a change.
* `addr-deref`: `*(&x)` in place of `x`.
* `subtract-negative`: `x - -y` or `x - (-y)` in place of an addition.
* `duplicate-branches`: the same statements in both arms of an `if`/`else`, or in every arm of a
  `switch` that has a default. The test decides nothing.
* `placeholder-name`: a local named `new_var`, `dummy...` or `fake...`.
* `volatile-view`: a pointer to volatile that isn't a hardware register (the `volatile` kind only
  covers objects and `&x` casts). A global an interrupt writes is volatile in its header instead.
* `local-union`: a union defined inside a function (not a union member of a file-scope struct).
  Two views of one value are ordinary C; two unrelated values sharing storage are not.
* `literal-param`: a `static inline` of at most two statements, or of any length when it calls
  nothing (`u32 v = *p & mask; *out = p; return v;`), with an integer parameter that
  is only an operand of bitwise operations (`&`, `|`, `^`, `~`, shifts and their compound
  assignments, as in `*flags & mask` or `*flags |= mask`) and that gets a literal or an ALL_CAPS
  constant at every call in the tree (`WordFlags_Test(&p->flags, 1)`). To the compiler the
  parameter is a local initialized to the literal, so this is `literal-local` moved into a
  helper, and as weak: a real accessor of the original's looks the same. Reported at the
  definition with the number of calls; a helper nobody calls, one that gets a variable
  somewhere, and a setter that only stores the parameter (`p->state = state`) are not
  reported. It needs every call, so it comes from a pass over src and include
  (`literal_params`, `literal_param_sites`), not from `sites`; the linter, tools/progress.py
  and land's added-warnings check all run it.

Warnings don't lower a score. The order of work is errors to zero, then warnings to zero, adding
none. A warning is silenced the way a linter's is, by a `concession-ok(kind): reason` comment, and
the reason has to argue that the original authors wrote it this way ("the SDK's DmaSet macro reads
the register back"). A reason that talks about the compiler (registers, allocation, reload,
matching) is an admission, not a justification, and `lint` rejects it.
"""
from __future__ import annotations

import os
import re
from dataclasses import dataclass, field

HARD = ("pin", "operand", "clobber", "barrier")
EXCUSABLE = ("debug-nop", "empty-loop", "empty-if", "single-use-macro", "volatile", "alias", "cflags", "raw-offset")
SCORED = HARD + EXCUSABLE
COMPILER = ("dead-code", "proved-zero")  # found by tools/deadcode.py, not by sites()
TREE = ("literal-param",)  # found by literal_params(), a pass over the whole tree, not by sites()
WARN = COMPILER + ("once-loop", "stmt-expr", "const-local", "literal-local", "addr-deref",
        "subtract-negative", "duplicate-branches", "placeholder-name", "volatile-view", "local-union",
        "dead-local-update") + TREE
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
        r"(?<![*\w])volatile\s+(?:const\s+)?(?:unsigned\s+|signed\s+|struct\s+)?\w+"
        r"\s+(?:const\s+)?(?!(?:const|volatile|restrict)\b)\w+\s*[=;,\[)]"
        r"|\*\s*volatile\b"
        r"|\(\s*(?:volatile\s+(?:const\s+)?\w+|v[us](?:8|16|32))\s*\*\s*\)\s*&\s*(?!REG_)"),
    "goto": re.compile(r"\bgoto\b"),
    "raw-offset": re.compile(r"\*\s*\(\s*(?:const\s+)?(?:volatile\s+)?(?:v?[us](?:8|16|32)|s?int|void)\s*\*\s*\)\s*"
                             r"\(\s*\(\s*(?:const\s+)?u8\s*\*\s*\)\s*[\w>.\-\[\]]+\s*\+\s*(?:0x[0-9A-Fa-f]+|\d+)\s*\)"),
}
# a local named for a number: zero, one, zeroX, two_, four2 (not zeroed, oneshot, tenth)
_NUMBER_NAME = (r"(?:zero|one|two|three|four|five|six|seven|eight|nine|ten|sixteen|minusOne|negOne|allOnes)"
                r"(?:[A-Z_0-9]\w*)?")
_LITERAL_DIGITS = r"(?:0[xX][0-9A-Fa-f]+|\d+)[uUlL]*"
_LITERAL = r"(?:-|~\s*)?" + _LITERAL_DIGITS  # `~0x141C0` is as much a literal as `-1`
_DECL_TYPE = (r"(?:(?:register|const)\s+){0,2}(?:unsigned\s+|signed\s+)?"
              r"(?!v[us]\d|volatile\b|goto\b|return\b|static\b|else\b|case\b|register\b|const\b)\w+\s+\*?")
# A local declaration: its first declarator follows the type; _local_declarations walks the rest.
_DECL_START = re.compile(r"(?m)^[ \t]+" + _DECL_TYPE + r"(?P<first>[A-Za-z_]\w*)(?=\s*[=,;\[])")
_DECL_PUNCT = re.compile(r"[][(){},;]")
_DECLARATOR = re.compile(r"\s*\*?\s*([A-Za-z_]\w*)\s*")
_LITERAL_INIT = re.compile(r"=\s*" + _LITERAL + r"\s*")
_LITERAL_ASSIGNMENT = re.compile(r"\s*=\s*" + _LITERAL + r"\s*;")
_LITERAL_ASSIGNED = re.compile(r"\b([A-Za-z_]\w*)\s*=\s*" + _LITERAL + r"\s*[;)]")
_LOCAL_CHANGE = (r"\b{0}\s*(?:[-+*/|&^%]|<<|>>)?=(?!=)|\b{0}\s*(?:\+\+|--)|(?:\+\+|--)\s*{0}\b"
                 r"|(?:[(,=?:]|\breturn)\s*&\s*{0}\b")
_MEMBER_ACCESS = re.compile(r"(?:->|\.)\s*[A-Za-z_]\w*")
_RECORD_HEAD = re.compile(r"\b(?:struct|union)\s*(?:[A-Za-z_]\w*\s*)?\{")
_QUOTED = re.compile(r'"(?:\\.|[^"\\\n])*"' r"|'(?:\\.|[^'\\\n])*'")
_INTEGER_SCALAR = r"[us](?:8|16|32|64)"
_DEAD_LOCAL_BLOCK = re.compile(
    rf"(?m)^[ \t]*\{{\s*{_INTEGER_SCALAR}\s+(?P<name>[A-Za-z_]\w*)\s*=\s*"
    rf"(?:\(\s*{_INTEGER_SCALAR}\s*\)\s*)?(?:[A-Za-z_]\w*|{_LITERAL})\s*;"
    r"(?P<updates>(?:\s*(?P=name)\s*(?:<<|>>|[-+*/%&^|])?=(?!=)[^;{}]+;)+)\s*\}"
)
_WARN_REGEX = {
    "stmt-expr": re.compile(r"\(\s*\{"),
    "addr-deref": re.compile(r"\*\s*\(\s*&\s*\w+\s*\)"),
    "subtract-negative": re.compile(r"(?:[)\]]|\b(?!(?:return|case|sizeof|_Alignof)\b)\w+)"
                                    r"\s*-(?:\s+|\s*\(\s*)-\s*(?=[A-Za-z_])"),
    "placeholder-name": re.compile(r"(?m)^[ \t]+[\w \t]*?\w[ \t]+\**\s*(?:new_var\w*|dummy\w*|fake\w*)\s*[=;,\[]"),
    "volatile-view": re.compile(
        r"\(\s*(?:const\s+)?volatile\s+(?:const\s+)?\w+\s*\*\s*\)\s*(?!&|REG_|0x0?4[0-9A-Fa-f]{6}\b)[\w(*]"
        r"|(?m:^[ \t]+(?:const\s+)?volatile\s+(?:const\s+)?\w+\s*\*\s*\w+\s*[=;])"),
}
_LOCAL_UNION = re.compile(r"(?m)^[ \t]*union\b[^;{}()]*\{")
_ONCE_END = re.compile(r"\}\s*while\s*\(\s*0\s*\)")
_IF_ELSE = re.compile(r"\bif\s*\([^{};]*?\)\s*\{(?P<yes>[^{}]*)\}"
                      r"\s*else\s*\{(?P<no>[^{}]*)\}")
_SWITCH = re.compile(r"\bswitch\s*\([^{};]*?\)\s*\{(?P<body>[^{}]*)\}")
_SWITCH_LABEL = re.compile(r"\b(?:case\s+[^:;\n]+|(?P<default>default))\s*:")
_SWITCH_EXIT = re.compile(r"(?:^|;)\s*(?:break|return\b[^;]*|goto\s+\w+)\s*;\s*$")
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
    r"\bv(?:[us](?:8|16|32|64)|bool(?:8|16|32))\s+(?:const\s+)?"
    r"(?!(?:const|volatile|restrict)\b)[A-Za-z_]\w*\s*[=;,\[)]")
_FUNCTION_BODY = re.compile(r"[A-Za-z_][\w\s\*]*?\b\w+\s*\([^;{]*\)\s*\{", re.S)
_FUNCTION_MACRO = re.compile(r"#\s*define\s+\w+\(")
# A value narrowed to unsigned just before `(s32)x < 0` can't be negative, so the guarded
# assignment is dead. Only the simple case: a change to the local or arithmetic after the cast
# could invalidate the range.
_NARROW_NEGATIVE_ASSIGNMENT = re.compile(
    r"(?m)^[ \t]+(?:const\s+)?[us]32\s+(?P<local>[A-Za-z_]\w*)\s*=\s*"
    r"\(\s*u(?:8|16)\s*\)\s*(?:\(\s*[us]32\s*\)\s*)?"
    r"[A-Za-z_]\w*(?:\s*(?:->|\.)\s*[A-Za-z_]\w*)*\s*;\s*"
    r"if\s*\(\s*\(\s*s32\s*\)\s*(?P=local)\s*<\s*0\s*\)\s*"
    r"(?:\{\s*)?(?P<assignment>[A-Za-z_]\w*\s*=(?!=)[^;{}]+;)"
)


@dataclass(frozen=True)
class Site:
    line: int  # 1-based, in the text scanned
    kind: str
    excused: bool = False
    note: str = field(default="", compare=False)  # detail for the report ("12 call sites, ...")


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


def _blank(m) -> str:
    return re.sub(r"[^\n]", " ", m.group(0))


def _function_items(code: str, *, include_macros=False):
    """(offset, text) of each function definition in CODE, and of each function-like macro on request."""
    from merge_unit import items

    offset = 0
    for item in items(code):
        start = code.find(item, offset)
        offset = start + len(item)
        declaration = item.lstrip()
        if _FUNCTION_BODY.match(declaration) or (include_macros and _FUNCTION_MACRO.match(declaration)):
            yield start, item


def _volatile_typedef_locals(code: str):
    """Offsets of scalar volatile-typedef declarations (locals and by-value parameters) in
    functions and macros.

    Top-level register layouts, globals and typedefs are not automatic locals.
    Pointer declarations are intentionally excluded, just as for volatile T *reg.
    """
    if not _VOLATILE_TYPEDEF.search(code):
        return
    for start, item in _function_items(code, include_macros=True):
        # String contents are data even when they spell a declaration.
        declarations = _COMMENTS.sub(_blank, item)
        for match in _VOLATILE_TYPEDEF.finditer(declarations):
            yield start + match.start()


def _scope_end(code: str, start: int) -> int:
    """Offset of the `}` that closes the block START is in (len(CODE) when there is none)."""
    depth = 0
    for token in _DECL_PUNCT.finditer(code, start):
        if token[0] == "{":
            depth += 1
        elif token[0] == "}":
            if not depth:
                return token.start()
            depth -= 1
    return len(code)


def _record_bodies(code: str):
    """(start, end) of each struct or union body: what is declared there is a member, not a local."""
    for head in _RECORD_HEAD.finditer(code):
        end = _scope_end(code, head.end())
        if end < len(code):
            yield head.end(), end


def _local_declarations(code: str):
    """Each indented declaration as [(name, offset, end, init), ...], one entry per declarator
    (`u32 a = 1, b, c = f(x, y);` has three). END is the offset after the declarator's `,` or `;`.
    INIT is None without an initializer, True for an integer literal, False for anything else.
    Commas inside calls, subscripts and braces do not start a declarator."""
    for declaration in _DECL_START.finditer(code):
        start = declaration.start("first")
        depth, found = 0, []
        for token in _DECL_PUNCT.finditer(code, start):
            char = token[0]
            if char in "([{":
                depth += 1
            elif char in ")]}":
                if not depth:
                    break  # a parameter list or an expression, not a declaration
                depth -= 1
            elif not depth:
                item = _DECLARATOR.match(code, start, token.start())
                if not item:
                    break
                init = None if item.end() == token.start() else \
                    bool(_LITERAL_INIT.fullmatch(code, item.end(), token.start()))
                found.append((item[1], item.start(1), token.end(), init))
                if char == ";":
                    yield found
                    break
                start = token.end()


def _constant_locals(code: str):
    """(offset, kind) for const-local and literal-local in CODE (macros and strings blanked)."""
    # A write to actor->state or sprite.state cannot modify a local named state.
    masked = _MEMBER_ACCESS.sub(lambda m: "@" * len(m[0]), code)
    declarators = [d for declaration in _local_declarations(code) for d in declaration]
    bindings: dict[str, list[int]] = {}
    for name, pos, _, _ in declarators:
        bindings.setdefault(name, []).append(pos)
    assigned = {m[1] for m in _LITERAL_ASSIGNED.finditer(masked)}
    records = None
    for name, pos, end, init in declarators:
        if init is False:
            continue
        if re.fullmatch(_NUMBER_NAME, name):
            if records is None:
                records = list(_record_bodies(code))
            if not any(start <= pos < stop for start, stop in records):
                yield pos, "const-local"
            continue
        if init is None and name not in assigned:
            continue
        # The rest of the declaration's block, less the blocks of inner locals of the same name:
        # a write before an inner binding or after its block still changes this one.
        limit = _scope_end(code, end)
        body = masked[end:limit]
        for shadow in bindings[name]:
            if end <= shadow < limit:
                stop = _scope_end(code, shadow)
                body = body[:shadow - end] + " " * (stop - shadow) + body[stop - end:]
        n = re.escape(name)
        if init is None:
            first = re.search(rf"\b{n}\b", body)
            if not first:
                continue
            before = body[:first.start()].rstrip()
            assignment = _LITERAL_ASSIGNMENT.match(body, first.end())
            # Only a statement of the declaration's own block counts as its first assignment: a
            # nested, conditional or labeled write, or one a goto can skip, depends on control flow.
            if assignment and before.count("{") == before.count("}") and before[-1:] in ("", ";", "}") \
                    and not re.search(r"\bgoto\b", before):
                pos = end + first.start()
                body = body[assignment.end():]
            else:
                # `sample & (mask = 0x7f)` where mask is never read: the local only carries the literal
                inside = re.search(rf"\(\s*({n})\s*=\s*{_LITERAL}\s*\)", body)
                if inside and len(re.findall(rf"\b{n}\b", body)) == 1:
                    yield end + inside.start(1), "literal-local"
                continue
        if not re.search(_LOCAL_CHANGE.format(n), body):
            yield pos, "literal-local"


def _statements(source: str, start: int, end: int) -> str:
    """SOURCE[start:end] with spacing normalized, except inside quotes, for comparing two arms."""
    out, last = [], start
    for quoted in _QUOTED.finditer(source, start, end):
        out += [re.sub(r"\s+", "", re.sub(r"\b\s+\b", "\0", source[last:quoted.start()])), quoted[0]]
        last = quoted.end()
    out.append(re.sub(r"\s+", "", re.sub(r"\b\s+\b", "\0", source[last:end])))
    return "".join(out).replace("\0", " ")


def _duplicate_branches(code: str, source: str):
    """Offsets of an if/else or a switch whose arms all hold the same statements. CODE is SOURCE
    with quoted text blanked: the structure comes from CODE, the comparison from SOURCE, so
    arms that differ only in a string are different."""
    for match in _IF_ELSE.finditer(code):
        yes = _statements(source, *match.span("yes"))
        if yes and yes == _statements(source, *match.span("no")):
            yield match.start()
    for match in _SWITCH.finditer(code):
        body_start, body_end = match.span("body")
        labels = list(_SWITCH_LABEL.finditer(code, body_start, body_end))
        if not any(label["default"] for label in labels):
            continue
        ends = [label.start() for label in labels[1:]] + [body_end]
        arms = [_statements(source, label.end(), end) for label, end in zip(labels, ends)]
        if not arms[-1]:
            continue
        # Consecutive labels without statements already share a single arm.
        arms = [arm for arm in arms if arm]
        if len(arms) > 1 and _SWITCH_EXIT.search(arms[0]) and all(arm == arms[0] for arm in arms[1:]):
            yield match.start()


def _warn_sites(code: str):
    """(offset, kind) for the warning kinds in CODE (comments already blanked)."""
    for macro in _MACRO_DEF.finditer(code):
        if _WRAPPED_EXPR_MACRO.fullmatch(macro.group(0).replace("\\\n", " ")):
            yield macro.start(), "stmt-expr"
    source = _MACRO_DEF.sub(_blank, code)  # otherwise a macro's body is its own business
    code = _QUOTED.sub(_blank, source)  # and quoted text is data
    for kind, rx in _WARN_REGEX.items():
        for m in rx.finditer(code):
            yield m.start(), kind
    if _LOCAL_UNION.search(code):
        for start, item in _function_items(code):
            for m in _LOCAL_UNION.finditer(item):
                yield start + m.start(), "local-union"
    for m in _DEAD_LOCAL_BLOCK.finditer(code):
        updates = m["updates"]
        name = re.escape(m["name"])
        operators = re.sub(rf"(?<!\w)(?:{name}|{_LITERAL})(?!\w)", "", updates)
        if re.fullmatch(r"[\s()+\-*/%&|^~<>=!;]+", operators):
            yield m.start("updates") + len(updates) - len(updates.lstrip()), "dead-local-update"
    yield from _constant_locals(code)
    for m in _INLINE_DEF.finditer(code):  # `u32 zero` as an inline's parameter is the same local
        params_start = m.start("params")
        for param in re.finditer(r"[^,]+", m["params"]):
            scalar = _SCALAR_PARAM.fullmatch(param[0])
            if scalar and re.fullmatch(_NUMBER_NAME, scalar[1]):
                yield params_start + param.start() + scalar.start(1), "const-local"
    for offset in _duplicate_branches(code, source):
        yield offset, "duplicate-branches"
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
    instrumented compiler. Empty for headers and when the compiler isn't built.

    The compiler's report leaves out register-only deletions because most are incidental. An
    assignment under a guard that an explicit narrowing makes impossible is recovered from the
    source (_NARROW_NEGATIVE_ASSIGNMENT)."""
    import deadcode
    rel = str(path).replace("\\", "/")
    if not rel.endswith(".c") or is_library(rel) or not deadcode.available():
        return []
    if text is None:
        text = open(os.path.join(deadcode.ROOT, rel), errors="replace").read()
    ok = allows(text)
    seen = {(line, kind) for kind, file, line, _, _ in deadcode.unit(rel) if file == rel}
    code = strip(text)
    for guard in _NARROW_NEGATIVE_ASSIGNMENT.finditer(code):
        seen.add((_line(code, guard.start("assignment")), "dead-code"))
    return [Site(line, kind, kind in ok.get(line, {})) for line, kind in sorted(seen)]


_SCALAR_TYPE = (r"(?:(?:unsigned|signed)\s+)?(?:[us](?:8|16|32|64)|bool(?:8|16|32)|int|short|long|char)"
                r"|unsigned|signed")
_SCALAR_CAST = re.compile(r"\(\s*(?:const\s+)?(?:" + _SCALAR_TYPE + r")\s*\)")
_SCALAR_PARAM = re.compile(r"\s*(?:const\s+)?(?:" + _SCALAR_TYPE + r")\s+(?:const\s+)?([A-Za-z_]\w*)\s*")
_INLINE_DEF = re.compile(
    r"(?m)^[ \t]*(?:static\s+(?:inline|__inline__)|inline\s+static)\b[^;{}()=]*?"
    r"\b(?P<name>[A-Za-z_]\w*)\s*\((?P<params>[^(){};]*)\)\s*\{")
_OPERATOR = r"<<=|>>=|[-+*/%&|^]=|&&|\|\||==|!=|<=|>=|<<|>>|\+\+|--|[-+*/%&|^~=<>!,?:\[;{}]"
_OPERATOR_BEFORE = re.compile(r"(?:" + _OPERATOR + r")$")
_OPERATOR_AFTER = re.compile(_OPERATOR)
_BITWISE = ("&", "|", "^", "~", "<<", ">>")
_BITWISE_STORE = ("&=", "|=", "^=", "<<=", ">>=")
# a call argument that is a constant: literals and ALL_CAPS names, joined by constant operators
_CONSTANT_ARG = re.compile(r"(?:\s|[-+~*()|&^]|<<|>>|(?<![\w.])(?:" + _LITERAL_DIGITS
                           + r"|[A-Z][A-Z0-9_]*)(?![\w.]))+")
_DECLARED_ARG = re.compile(r"\s*(?:void|(?:const\s+|struct\s+|union\s+|enum\s+)*[A-Za-z_]\w*[\s*]+\**\s*[A-Za-z_]\w*)\s*")
_INCLUDE = re.compile(r'(?m)^[ \t]*#[ \t]*include[ \t]+"([^"\n]+)"')
_literal_params_cache: dict[str, dict] = {}  # root -> _tree_index


def _operand_use(body: str, start: int, end: int) -> bool:
    """True when the operand at BODY[start:end] is an operand of a bitwise operation: `&`, `|`,
    `^`, `~`, a shift, or the right side of `&=`, `|=`, `^=`, `<<=`, `>>=`. A plain assignment of
    it (a setter) is not one. BODY has its member accesses (`->name`, `.name`) replaced by `@`s."""
    i, before = start, None
    while before is None:
        head = body[:i].rstrip()
        if head.endswith("("):  # look out through the parentheses around it
            outer = head[:-1].rstrip()
            word = re.search(r"\w+$", outer)
            if word and word[0] not in ("if", "while", "return", "switch"):
                return False  # an argument of a call
            if word:
                before = ""
            i = len(head) - 1
            continue
        m = _OPERATOR_BEFORE.search(head)
        before = m[0] if m else ""
        if before in ("&", "*", "-", "+") and not re.search(r"[\w)\]@]$", head[:m.start()].rstrip()):
            return False  # address-of, dereference or sign
    tail = body[end:].lstrip()
    closed = tail.startswith(")")
    while tail.startswith(")"):
        tail = tail[1:].lstrip()
    m = _OPERATOR_AFTER.match(tail)
    after = m[0] if m else ""
    # a plain `=` decides nothing: `p->flags = mask | p->flags` is a bitwise use, `p->state = value` a setter
    if before not in _BITWISE + _BITWISE_STORE + ("", "=", ";", "{", "}", "!", "?", ":", "&&", "||"):
        return False
    if not closed and after not in _BITWISE + ("", ";", "?", ":", "&&", "||"):
        return False
    return before in _BITWISE + _BITWISE_STORE or (not closed and after in _BITWISE)


def _inline_definitions(code: str):
    """(name, offset, {parameter index: name}) for each static inline in CODE (comments blanked)
    that literal-param could report: a short body and an integer parameter used only as an
    operand of bitwise operations."""
    for m in _INLINE_DEF.finditer(code):
        stop = _scope_end(code, m.end())
        body = _SCALAR_CAST.sub("", _QUOTED.sub(_blank, code[m.end():stop]))
        body = _MEMBER_ACCESS.sub(lambda access: "@" * len(access[0]), body)  # p->mask is not the parameter mask
        if body.count(";") > 2 and any(word not in _NOT_CALLS for word in _CALLED.findall(body)):
            continue  # a longer helper that does more than move bits (it calls something)
        params = {}
        for index, param in enumerate(m["params"].split(",")):
            scalar = _SCALAR_PARAM.fullmatch(param)
            if not scalar:
                continue
            uses = list(re.finditer(r"\b%s\b" % re.escape(scalar[1]), body))
            if uses and all(_operand_use(body, use.start(), use.end()) for use in uses):
                params[index] = scalar[1]
        if params:
            yield m["name"], m.start("name"), params


def _arguments(code: str, start: int):
    """(arguments, offset after the `)`) of the call whose `(` is at CODE[start - 1], or (None, start)."""
    depth, last, out = 0, start, []
    for token in _DECL_PUNCT.finditer(code, start):
        char = token[0]
        if char in "([{":
            depth += 1
        elif char in ")]}":
            if not depth:
                out.append(code[last:token.start()])
                return (out, token.end()) if char == ")" else (None, start)
            depth -= 1
        elif char == "," and not depth:
            out.append(code[last:token.start()])
            last = token.end()
        elif char == ";" and not depth:
            break
    return None, start


_CALLED = re.compile(r"\b([A-Za-z_]\w*)\s*\(")
_NOT_CALLS = frozenset(("if", "while", "for", "switch", "return", "sizeof"))


def _file_calls(text: str, names, called=None):
    """({name: [arguments or None, ...]}, {names TEXT defines a function for}) for the calls in
    TEXT to NAMES. CALLED is the set of names TEXT has before a `(`, when the caller kept it."""
    calls: dict[str, list] = {}
    defines = set()
    names = names & (set(_CALLED.findall(text)) if called is None else called)
    if names:
        code = _QUOTED.sub(_blank, strip(text))
        for m in re.finditer(r"\b(" + "|".join(sorted(names)) + r")\s*\(", code):
            arguments, end = _arguments(code, m.end())
            if arguments is not None and code[end:end + 200].lstrip().startswith("{"):
                defines.add(m[1])
            elif arguments is None or not all(_DECLARED_ARG.fullmatch(a) for a in arguments):  # not a prototype
                calls.setdefault(m[1], []).append(arguments)
    return calls, defines


def _file_definitions(rel: str, text: str) -> list:
    if "inline" not in text or is_library(rel):
        return []
    return list(_inline_definitions(strip(text)))


def _tree_index(root) -> dict:
    """The per-file facts literal_params works from, for the tree on disk (cached per ROOT)."""
    from pathlib import Path
    if str(root) not in _literal_params_cache:
        texts = {}
        for sub in ("src", "include"):
            for path in sorted(Path(root, sub).rglob("*.[ch]")):
                texts[path.relative_to(root).as_posix()] = path.read_text(errors="replace")
        definitions = {rel: found for rel, text in texts.items() if (found := _file_definitions(rel, text))}
        names = {name for found in definitions.values() for name, _, _ in found}
        called = {rel: set(_CALLED.findall(text)) for rel, text in texts.items()}
        _literal_params_cache[str(root)] = {
            "texts": texts, "definitions": definitions, "names": names, "called": called,
            "calls": {rel: _file_calls(text, names, called[rel]) for rel, text in texts.items()}}
    return _literal_params_cache[str(root)]


def literal_params(root=None, overlay=None) -> dict[str, list[Site]]:
    """{path relative to ROOT: [Site(line, "literal-param", note="N call sites, ..."), ...]} for src and
    include (excused is not set here; literal_param_sites does that for one file). OVERLAY is
    {path: text} to use in place of what is on disk (a candidate that is not written yet).

    A call belongs to the definition in its own file when there is one (per-object clones share
    names), otherwise to the headers of that name the file includes."""
    from pathlib import Path
    root = Path(root or Path(__file__).resolve().parent.parent)
    index = _tree_index(root)
    if not overlay and "sites" in index:
        return index["sites"]
    texts, per_file, file_calls = index["texts"], index["definitions"], index["calls"]
    if overlay:
        texts, per_file = {**texts, **overlay}, dict(per_file)
        for rel, text in overlay.items():
            per_file[rel] = _file_definitions(rel, text)
        names = {name for found in per_file.values() for name, _, _ in found}
        file_calls = {**file_calls, **{rel: _file_calls(text, names) for rel, text in overlay.items()}}
        new = names - index["names"]
        for rel, text in index["texts"].items() if new else ():  # a new helper: any file may call it
            if rel not in overlay and new & index["called"][rel]:
                more, defines = _file_calls(text, new, index["called"][rel])
                file_calls[rel] = ({**file_calls[rel][0], **more}, file_calls[rel][1] | defines)
    definitions: dict[str, dict[str, list]] = {}  # name -> file -> [(offset, params)]
    for rel, found in per_file.items():
        for name, offset, params in found:
            definitions.setdefault(name, {}).setdefault(rel, []).append((offset, params))
    calls: dict[str, dict[str, list]] = {}  # name -> file -> [arguments or None, ...]
    for rel, (found, _) in file_calls.items():
        for name, arguments in found.items():
            calls.setdefault(name, {})[rel] = arguments

    direct: dict[str, list[str]] = {}
    reached: dict[str, set] = {}

    def reach(rel):
        """REL and every project header it includes, directly or not."""
        if rel not in reached:
            found, queue = {rel}, [rel]
            while queue:
                current = queue.pop()
                if current not in direct:
                    direct[current] = [target for name in _INCLUDE.findall(texts[current]) for target in (
                        "include/" + name, os.path.normpath(os.path.join(os.path.dirname(current), name)))
                        if target in texts]
                for target in direct[current]:
                    if target not in found:
                        found.add(target)
                        queue.append(target)
            reached[rel] = found
        return reached[rel]

    out: dict[str, list[Site]] = {}
    for name, files in definitions.items():
        headers = [rel for rel in files if rel.endswith(".h")]
        received: dict[str, list] = {rel: [] for rel in files}
        for rel, found in calls.get(name, {}).items():
            if rel in files:
                owners = [rel]
            elif name in file_calls[rel][1]:
                continue  # another function of the same name
            else:
                owners = [h for h in headers if h in reach(rel)] or headers
            for owner in owners:
                received[owner] += found
        for rel, sites_ in files.items():
            code = None
            for offset, params in sites_:
                found = received[rel]
                constant = [p for i, p in params.items() if found and all(
                    a is not None and i < len(a) and re.search(r"\w", a[i])
                    and _CONSTANT_ARG.fullmatch(_SCALAR_CAST.sub("", a[i])) for a in found)]
                if constant:
                    code = code if code is not None else strip(texts[rel])
                    out.setdefault(rel, []).append(Site(
                        _line(code, offset), "literal-param",
                        note=f"{len(found)} call site{'s' if len(found) != 1 else ''}, each with a constant"
                             f" for `{'`, `'.join(constant)}`"))
    if not overlay:
        index["sites"] = out
    return out


def literal_param_calls(site: Site) -> int:
    """The number of calls a literal-param site's note counts."""
    return int(re.match(r"(\d+) call site", site.note)[1])


def literal_param_sites(path, text=None, root=None, *, candidate=False) -> list[Site]:
    """The literal-param sites of PATH (relative to the tree), with TEXT's concession-ok comments
    applied. The calls come from the tree on disk; with CANDIDATE, TEXT stands in for PATH's
    own contents there (what land is about to write)."""
    from pathlib import Path
    rel = str(path).replace("\\", "/")
    root = Path(root or Path(__file__).resolve().parent.parent)
    if rel.startswith("..") or os.path.isabs(rel) or not rel.endswith((".c", ".h")):
        return []
    found = literal_params(root, {rel: text} if candidate and text is not None else None).get(rel, [])
    if not found:
        return []
    if text is None:
        text = (root / rel).read_text(errors="replace")
    ok = allows(text)
    return [Site(s.line, s.kind, s.kind in ok.get(s.line, {}), s.note) for s in found]


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


def lint(text: str, path=None, *, single_use=frozenset(), compiler=None, tree=None) -> list[str]:
    """Problems with TEXT's concession-ok comments: unknown kinds, kinds no comment can excuse,
    a missing reason, or a comment with nothing to excuse. COMPILER and TREE are the file's
    compiler_sites and literal_param_sites; without them a comment for one of those kinds is
    not checked for something to excuse."""
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
    for s in [*(compiler or ()), *(tree or ())]:
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
            elif k in COMPILER and compiler is None or k in TREE and tree is None:
                pass  # only a compile, or a pass over the tree, can say whether there is one here
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
    use_tree = not args.errors_only and (not args.kind or any(k in TREE for k in args.kind))
    totals: dict[str, int] = {}
    for f in files:
        text = f.read_text(errors="replace")
        rel = f.resolve().relative_to(root) if f.resolve().is_relative_to(root) else f
        lines = text.splitlines()
        found = sites(text, rel, single_use=once)
        flags = cflags(text, rel)
        compiled = compiler_sites(rel, text) if use_compiler else None
        params = literal_param_sites(rel, text, root) if use_tree else None
        for s in sorted(found + (compiled or []) + (params or []), key=lambda s: (s.line, s.kind)) \
                + ([flags] if flags else []):
            if s.excused or s.kind in TRACKED or (args.kind and s.kind not in args.kind):
                continue
            level = "warning" if s.kind in WARN else "error"
            if level == "warning" and args.errors_only:
                continue
            totals[s.kind] = totals.get(s.kind, 0) + 1
            if not args.summary:
                print(f"{rel}:{s.line}: {level} [{s.kind}] "
                      f"{lines[s.line - 1].strip()[:100] if 0 < s.line <= len(lines) else ''}"
                      + (f"  ({s.note})" if s.note else ""))
        for problem in lint(text, rel, single_use=once, compiler=compiled if f.suffix == ".c" else None, tree=params):
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
