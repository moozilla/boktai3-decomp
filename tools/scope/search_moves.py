"""Deterministic, checked C moves for search sessions.

The AST is an authority for syntax, not for facts it cannot know. In particular,
unknown typedefs, global names, memory accesses and calls are never assumed pure.
Moves retain the original snapshot hash and a concrete edit, so stale moves cannot
silently apply at a different site. Reversibility is provided by parent snapshots.
Only the selected function is regenerated; its comments and formatting normalize
on the first move. Includes, helpers and NONMATCHING guards remain untouched.
"""
from __future__ import annotations

import copy
from dataclasses import asdict, dataclass, field
from functools import lru_cache
import hashlib
from pathlib import Path
import re
import sys
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
TOOLS = ROOT / "tools"
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))
import permute  # noqa: E402
if permute.PERMUTER not in sys.path:
    sys.path.insert(0, permute.PERMUTER)
from perm_pycparser import c_ast as ca, c_generator, c_parser  # noqa: E402

GEN = c_generator.CGenerator()
KNOWN = {"u8": "u8", "s8": "s8", "u16": "u16", "s16": "s16",
         "u32": "u32", "s32": "s32", "bool8": "u8", "bool32": "u32"}
PRELUDE = "\n".join("typedef %s %s;" % (base, name) for name, base in {
    "u8": "unsigned char", "s8": "signed char", "u16": "unsigned short",
    "s16": "signed short", "u32": "unsigned int", "s32": "signed int",
    "bool8": "unsigned char", "bool32": "unsigned int"}.items())


class MoveError(ValueError):
    """A move is stale, malformed, unavailable, or cannot be justified."""


@dataclass(frozen=True)
class Edit:
    start: int
    end: int
    before: str
    after: str


@dataclass(frozen=True)
class Move:
    id: str
    kind: str
    description: str
    parameters: dict[str, Any]
    edits: tuple[Edit, ...]
    preconditions: tuple[str, ...]
    evidence: dict[str, Any]
    source_hash: str
    line: int = 0
    semantic_status: str = "by-construction"

    def to_dict(self):
        return asdict(self)

    @classmethod
    def from_dict(cls, data):
        d = dict(data)
        d["edits"] = tuple(Edit(**e) if isinstance(e, dict) else e for e in d["edits"])
        d["preconditions"] = tuple(d["preconditions"])
        return cls(**d)


def source_hash(source):
    return hashlib.sha256(source.encode()).hexdigest()


def _mask(source):
    # Preserve offsets and newlines, including braces inside comments/literals.
    rx = r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\''
    return re.sub(rx, lambda m: re.sub(r"[^\n]", " ", m.group()), source, flags=re.S)


def function_span(source, func=None):
    """Locate an actual definition without counting comment/string braces."""
    masked = _mask(source)
    aliases = dict(re.findall(r"^\s*#define\s+(\w+)\s+(\w+)\s*$", source, re.M))
    wanted = {func} | {a for a, b in aliases.items() if b == func} if func else None
    pat = re.compile(r"^[ \t]*(?:[A-Za-z_]\w*[ \t*]+)+([A-Za-z_]\w*)\s*\([^;{}]*\)\s*\{", re.M)
    found = []
    for m in pat.finditer(masked):
        if wanted and m.group(1) not in wanted:
            continue
        depth, i = 1, m.end()
        while i < len(masked) and depth:
            depth += (masked[i] == "{") - (masked[i] == "}")
            i += 1
        if depth == 0:
            found.append((m.start(), i, m.group(1)))
    if len(found) != 1:
        raise MoveError("select one function definition (found %d)" % len(found))
    return found[0]


def walk(node, path=()):
    yield path, node
    for key, child in node.children():
        yield from walk(child, path + (key,))


def _at(node, path):
    for key in path:
        if "[" in key:
            attr, idx = key[:-1].split("[")
            node = getattr(node, attr)[int(idx)]
        else:
            node = getattr(node, key)
    return node


def _set(node, path, value):
    parent, key = _at(node, path[:-1]), path[-1]
    if "[" in key:
        attr, idx = key[:-1].split("[")
        getattr(parent, attr)[int(idx)] = value
    else:
        setattr(parent, key, value)


def _strip_comments(source):
    return re.sub(r'/\*.*?\*/|//[^\n]*', lambda m: "\n" * m.group().count("\n"), source, flags=re.S)


@lru_cache(maxsize=1)
def _textual_macros():
    bad = set()
    for path in (ROOT / "include").rglob("*.h"):
        text = path.read_text(errors="replace").replace("\\\n", " ")
        for m in re.finditer(r"^\s*#\s*define\s+(\w+)(.*)$", text, re.M):
            if "#" in m.group(2) or re.search(r"\b(__LINE__|__COUNTER__|__FILE__)\b", m.group(2)):
                bad.add(m.group(1))
    return frozenset(bad)


def _parse(source, func):
    start, end, name = function_span(source, func)
    body = source[start:end]
    if re.search(r"^\s*#|\b(__LINE__|__COUNTER__|__FILE__|__func__|__FUNCTION__)\b", body, re.M):
        raise MoveError("function contains preprocessing or source-location-sensitive constructs")
    if set(re.findall(r"\b\w+\b", body)) & _textual_macros():
        raise MoveError("function uses a textual or source-location-sensitive macro")
    if re.search(r"\b(__attribute__)\b", body):
        raise MoveError("function has attributes requiring a source-preserving edit")
    # Unknown names can be typedefs for parsing, but never acquire an inferred
    # integer type. C macro calls remain opaque FuncCall nodes.
    types = set(re.findall(r"\b([A-Za-z_]\w*)\s+\**[A-Za-z_]\w*\s*(?=[,;=)\[])" , _strip_comments(body)))
    reserved = {"return", "else", "case", "goto", "sizeof", "struct", "union", "enum", "unsigned", "signed",
                "const", "volatile", "static", "extern", "register", "auto", "inline", "int", "char", "short",
                "long", "float", "double", "void", "if", "while", "for", "switch", "do"}
    types -= reserved | KNOWN.keys()
    prelude = PRELUDE + "\n" + "\n".join("typedef int %s;" % n for n in sorted(types)) + "\n"
    try:
        tree = c_parser.CParser().parse(prelude + _strip_comments(body))
    except Exception as exc:
        raise MoveError("AST unavailable: " + str(exc).split("\n")[0]) from exc
    fn = next((n for n in tree.ext if isinstance(n, ca.FuncDef)), None)
    if fn is None or fn.decl.name != name:
        raise MoveError("selected function was not parsed")
    for _, node in walk(fn):
        if isinstance(node, ca.Asm) and (getattr(node.asm, "value", None) != '""' or node.gotos):
            raise MoveError("nonempty or goto assembly requires a source-preserving edit")
    return start, end, fn, len(prelude.splitlines())


def _decl_type(decl):
    quals = []
    t = decl.type
    while hasattr(t, "type"):
        quals.extend(getattr(t, "quals", []))
        if isinstance(t, (ca.PtrDecl, ca.ArrayDecl, ca.FuncDecl)):
            return None
        t = t.type
    if "volatile" in quals + list(getattr(decl, "quals", [])) or not isinstance(t, ca.IdentifierType):
        return None
    words = t.names
    if len(words) == 1 and words[0] in KNOWN:
        return KNOWN[words[0]]
    if set(words) <= {"unsigned", "signed", "int", "char", "short", "long"}:
        if words.count("long") > 1:
            return None
        width = 8 if "char" in words else 16 if "short" in words else 32
        return ("u" if "unsigned" in words else "s") + str(width)
    return None


class Facts:
    def __init__(self, fn):
        all_nodes = [n for _, n in walk(fn)]
        decls = [n for n in all_nodes if isinstance(n, ca.Decl) and n.name]
        self.counts = {d.name: sum(x.name == d.name for x in decls) for d in decls}
        self.addressed = {n.expr.name for n in all_nodes if isinstance(n, ca.UnaryOp)
                          and n.op == "&" and isinstance(n.expr, ca.ID)}
        self.types = {d.name: _decl_type(d) for d in decls
                      if self.counts[d.name] == 1 and not d.storage and d.name not in self.addressed}
        self.decls = {d.name: d for d in decls}
        self.names = {n.name for n in all_nodes if isinstance(n, (ca.ID, ca.Decl)) and n.name}

    def typ(self, n):
        if isinstance(n, ca.ID):
            return self.types.get(n.name)
        if isinstance(n, ca.Constant) and n.type in ("int", "unsigned int", "long int", "unsigned long int"):
            try:
                value = int(re.sub(r"[uUlL]+$", "", n.value), 0)
            except ValueError:
                return None
            if value > 0xFFFFFFFF or (value > 0x7FFFFFFF and "u" not in n.value.lower() and not n.value.lower().startswith("0x")):
                return None
            return "u32" if "u" in n.value.lower() or value > 0x7FFFFFFF else "s32"
        if isinstance(n, ca.Cast):
            t = _decl_type(n.to_type)
            return t if self.typ(n.expr) else None
        if isinstance(n, ca.UnaryOp) and n.op in ("+", "-", "~", "!"):
            t = self.typ(n.expr)
            return None if t is None else "s32" if n.op == "!" or t in ("u8", "u16", "s8", "s16") else t
        if isinstance(n, ca.BinaryOp):
            a, b = self.typ(n.left), self.typ(n.right)
            if not a or not b:
                return None
            if n.op in ("==", "!=", "<", ">", "<=", ">=", "&&", "||"):
                return "s32"
            if n.op in ("<<", ">>"):
                return a if a in ("u32", "s32") else "s32"
            if n.op in ("+", "-", "*", "/", "%", "&", "|", "^"):
                return "u32" if "u32" in (a, b) else "s32"
        return None

    def pure(self, n):
        return self.typ(n) is not None

    def reads(self, n):
        return {x.name for _, x in walk(n) if isinstance(x, ca.ID)}

    def local(self, n):
        return isinstance(n, ca.ID) and bool(self.types.get(n.name))

    def fresh(self):
        for i in range(1, 100000):
            name = "search_tmp%d" % i
            if name not in self.names:
                return name
        raise MoveError("could not allocate temporary name")


def _typename(name):
    return c_parser.CParser().parse(PRELUDE + "\nvoid f(void){%s x;}" % name).ext[-1].body.block_items[0]


def _new_decl(name, typ):
    d = copy.deepcopy(_typename(typ))
    d.name = name
    d.type.declname = name
    return d


def _neg(n):
    return copy.deepcopy(n.expr) if isinstance(n, ca.UnaryOp) and n.op == "!" else ca.UnaryOp("!", copy.deepcopy(n))


def _path_string(path):
    return "/".join(path)


def enumerate_moves(source: str, func: str | None = None, *, kinds=None, line_range=None) -> list[Move]:
    """Enumerate finite legal edges, in stable AST order. Parse failures raise MoveError.

    `kinds` filters move families; `line_range=(lo, hi)` focuses original lines.
    No move adds pins, assembly, volatile accesses, arithmetic UB, or assumed
    memory independence. Conditions may contain effects when evaluated once in
    the same place. Other expression moves use proven unaliased integer locals.
    """
    start, end, fn, prelude_lines = _parse(source, func)
    facts, result, digest = Facts(fn), [], source_hash(source)
    allowed = set(kinds) if kinds is not None else None
    base_line = source.count("\n", 0, start)
    seen = set()

    def emit(kind, path, description, transform, preconditions, params=None):
        if allowed is not None and kind not in allowed:
            return
        node = _at(fn, path)
        line = base_line + max(1, getattr(getattr(node, "coord", None), "line", prelude_lines + 1) - prelude_lines)
        if line_range and not line_range[0] <= line <= line_range[1]:
            return
        child = copy.deepcopy(fn)
        transform(child)
        text = GEN.visit(child).rstrip()
        if text == source[start:end] or text in seen:
            return
        seen.add(text)
        parameters = {"path": _path_string(path), **(params or {})}
        ident = kind + ":" + hashlib.sha256((digest + kind + repr(parameters)).encode()).hexdigest()[:12]
        before, after, a, b = source[start:end], text, start, end
        while before and after and before[0] == after[0]:
            before, after, a = before[1:], after[1:], a + 1
        while before and after and before[-1] == after[-1]:
            before, after, b = before[:-1], after[:-1], b - 1
        result.append(Move(ident, kind, description, parameters, (Edit(a, b, before, after),),
                           tuple(preconditions), {"ast": type(node).__name__, "function": fn.decl.name,
                           "normalizes_function_formatting": True, "address_taken_locals_excluded": sorted(facts.addressed)},
                           digest, line))

    pure = ("operands are nonvolatile unaliased integer locals or integer constants",
            "no calls, writes, memory reads, or unspecified type assumptions")
    for path, n in walk(fn):
        if isinstance(n, ca.BinaryOp) and facts.pure(n.left) and facts.pure(n.right):
            if n.op in ("+", "*", "&", "|", "^", "==", "!="):
                def swap(f, p=path):
                    b = _at(f, p)
                    b.left, b.right = b.right, b.left
                emit("commute", path, "Swap operands of %s" % GEN.visit(n), swap, pure, {"operator": n.op})
            if n.op in ("<", ">", "<=", ">="):
                def reverse(f, p=path):
                    b = _at(f, p)
                    b.left, b.right = b.right, b.left
                    b.op = {"<": ">", ">": "<", "<=": ">=", ">=": "<="}[b.op]
                emit("compare-direction", path, "Reverse comparison direction", reverse, pure)
            if n.op in ("+", "*", "&", "|", "^") and facts.typ(n) == "u32":
                for direction in ("left", "right"):
                    inner = getattr(n, direction)
                    if (not isinstance(inner, ca.BinaryOp) or inner.op != n.op or facts.typ(inner) != "u32"
                            or any(facts.typ(x) != "u32" for x in (inner.left, inner.right, getattr(n, "right" if direction == "left" else "left")))):
                        continue
                    def reassociate(f, p=path, d=direction):
                        b = _at(f, p)
                        if d == "left":
                            a = b.left
                            _set(f, p, ca.BinaryOp(b.op, a.left, ca.BinaryOp(b.op, a.right, b.right)))
                        else:
                            a = b.right
                            _set(f, p, ca.BinaryOp(b.op, ca.BinaryOp(b.op, b.left, a.left), a.right))
                    emit("reassociate-unsigned", path, "Reassociate unsigned %s" % n.op, reassociate,
                         pure + ("all three operands have 32-bit unsigned type",), {"direction": direction})
            if n.op in ("*", "/", "%") and facts.typ(n.left) == "u32" and isinstance(n.right, ca.Constant):
                try:
                    value = int(re.sub(r"[uUlL]+$", "", n.right.value), 0)
                except ValueError:
                    value = 0
                if 0 < value <= 0x80000000 and value & (value - 1) == 0:
                    op, val = {"*": ("<<", value.bit_length() - 1), "/": (">>", value.bit_length() - 1), "%": ("&", value - 1)}[n.op]
                    def power(f, p=path, op=op, val=val):
                        b = _at(f, p)
                        b.op, b.right = op, ca.Constant("unsigned int", str(val) + "U")
                    emit("unsigned-power-two", path, "Spell unsigned %s by power of two as %s" % (n.op, op), power,
                         pure + ("left operand is 32-bit unsigned; shift count is in 0..31",))
        if isinstance(n, ca.If):
            if n.iffalse is not None:
                def invert(f, p=path):
                    b = _at(f, p)
                    b.cond = _neg(b.cond)
                    b.iftrue, b.iffalse = b.iffalse, b.iftrue
                    if not isinstance(b.iftrue, ca.Compound):
                        b.iftrue = ca.Compound([b.iftrue])
                emit("invert-if", path, "Negate condition and swap if/else bodies", invert,
                     ("condition is evaluated exactly once; branch bodies and labels are retained",))
            elif isinstance(n.cond, ca.BinaryOp) and n.cond.op == "&&":
                def split_and(f, p=path):
                    b = _at(f, p)
                    b.cond, b.iftrue = b.cond.left, ca.Compound([ca.If(b.cond.right, b.iftrue, None)])
                emit("split-short-circuit", path, "Split && into nested if statements", split_and,
                     ("left-to-right short-circuit evaluation and body execution are identical",))
        if isinstance(n, ca.TernaryOp) and n.iftrue is not None:
            def ternary(f, p=path):
                b = _at(f, p)
                b.cond, b.iftrue, b.iffalse = _neg(b.cond), b.iffalse, b.iftrue
            emit("invert-conditional", path, "Negate condition and exchange conditional arms", ternary,
                 ("condition is evaluated once; selected expression and conditional result type are preserved",))
        if isinstance(n, ca.While):
            emit("while-to-for", path, "Spell while as for with empty init/step",
                 lambda f, p=path: _set(f, p, ca.For(None, _at(f, p).cond, None, _at(f, p).stmt)),
                 ("condition, continue target, break target, and scope are unchanged",))
        if isinstance(n, ca.For) and n.init is None and n.next is None:
            emit("for-to-while", path, "Spell for with empty init/step as while",
                 lambda f, p=path: _set(f, p, ca.While(_at(f, p).cond or ca.Constant("int", "1"), _at(f, p).stmt)),
                 ("condition, continue target, break target, and scope are unchanged",))
        if isinstance(n, ca.Assignment) and facts.local(n.lvalue) and facts.pure(n.rvalue):
            if n.op in ("+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>="):
                def expand(f, p=path):
                    b = _at(f, p)
                    b.rvalue, b.op = ca.BinaryOp(b.op[:-1], copy.deepcopy(b.lvalue), b.rvalue), "="
                emit("expand-compound", path, "Expand %s assignment" % n.op, expand,
                     pure + ("left operand is a nonvolatile unaliased scalar local",))
            if (n.op == "=" and isinstance(n.rvalue, ca.BinaryOp) and isinstance(n.rvalue.left, ca.ID)
                    and n.rvalue.left.name == n.lvalue.name):
                def compact(f, p=path):
                    b = _at(f, p)
                    b.op, b.rvalue = b.rvalue.op + "=", b.rvalue.right
                if n.rvalue.op in ("+", "-", "*", "/", "%", "&", "|", "^", "<<", ">>"):
                    emit("compact-compound", path, "Use compound assignment for " + n.lvalue.name, compact, pure)
        if not isinstance(n, ca.Compound):
            continue
        items = n.block_items or []
        for i, item in enumerate(items):
            if i + 1 < len(items):
                other = items[i + 1]
                if (isinstance(item, ca.Decl) and isinstance(other, ca.Decl) and item.init is None and other.init is None
                        and facts.types.get(item.name) and facts.types.get(other.name)):
                    def decl_swap(f, p=path, i=i):
                        xs = _at(f, p).block_items
                        xs[i], xs[i + 1] = xs[i + 1], xs[i]
                    emit("declaration-order", path + ("block_items[%d]" % i,), "Exchange declarations %s and %s" % (item.name, other.name),
                         decl_swap, ("both declarations are uninitialized scalar locals without storage or address escape",), {"index": i})
                if (isinstance(item, ca.Assignment) and item.op == "=" and facts.local(item.lvalue) and facts.pure(item.rvalue)
                        and isinstance(other, ca.Assignment) and other.op == "=" and facts.local(other.lvalue) and facts.pure(other.rvalue)
                        and item.lvalue.name != other.lvalue.name
                        and item.lvalue.name not in facts.reads(other.rvalue) and other.lvalue.name not in facts.reads(item.rvalue)):
                    def stmt_swap(f, p=path, i=i):
                        xs = _at(f, p).block_items
                        xs[i], xs[i + 1] = xs[i + 1], xs[i]
                    emit("independent-statements", path + ("block_items[%d]" % i,), "Exchange independent assignments", stmt_swap,
                         pure + ("write/read sets do not conflict; locals cannot alias",), {"index": i})
            if isinstance(item, ca.Assignment) and item.op == "=" and facts.local(item.lvalue) and facts.pure(item.rvalue):
                # Do not extract declarations into blocks reachable by goto: a
                # jump could bypass the newly introduced assignment's context.
                name, typ = facts.fresh(), facts.typ(item.rvalue)
                def temp(f, p=path, i=i, name=name, typ=typ):
                    block = _at(f, p)
                    a = block.block_items[i]
                    block.block_items[i:i + 1] = [ca.Assignment("=", ca.ID(name), a.rvalue), ca.Assignment("=", a.lvalue, ca.ID(name))]
                    j = 0
                    while j < len(block.block_items) and isinstance(block.block_items[j], ca.Decl):
                        j += 1
                    block.block_items.insert(j, _new_decl(name, typ))
                emit("introduce-temporary", path + ("block_items[%d]" % i,), "Evaluate RHS through fresh %s temporary" % typ, temp,
                     pure + ("temporary has exactly the RHS integer result type; original store remains in place",),
                     {"name": name, "type": typ, "index": i})
    return result


def apply_move(source: str, move: Move | dict | str, func: str | None = None) -> str:
    """Apply one enumerated edge only to the exact parent it was proved for."""
    if isinstance(move, str):
        moves = [m for m in enumerate_moves(source, func) if m.id == move]
        if not moves:
            raise MoveError("move ID is not available for this snapshot")
        move = moves[0]
    elif isinstance(move, dict):
        move = Move.from_dict(move)
    if source_hash(source) != move.source_hash:
        raise MoveError("stale move: source snapshot hash differs")
    out, boundary = source, len(source)
    for edit in sorted(move.edits, key=lambda e: e.start, reverse=True):
        if not 0 <= edit.start <= edit.end <= boundary or source[edit.start:edit.end] != edit.before:
            raise MoveError("edit span does not match parent snapshot")
        out = out[:edit.start] + edit.after + out[edit.end:]
        boundary = edit.start
    return out
