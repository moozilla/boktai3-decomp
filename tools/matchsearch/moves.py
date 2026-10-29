"""Finite, replayable source moves for the matching search session.

The stock ``tools/rewrites.py`` generators are useful *hypotheses*, but several
do not preserve C semantics (copying across writes, reordering dependent stores,
changing integer widths).  This catalog uses the same pycparser representation
and supplies conservative guards for its default moves.  ``guarded`` means a
local syntactic argument for well-defined C, not a proof against the ROM.
Explicit ``legacy:rw_*`` moves remain available with grade ``hypothesis``.

An AST path is the site identity.  Coordinates are presentation metadata only.
Every move carries both the original translation unit with one definition
replaced and a preprocessed translation unit suitable for scope.fast.  Includes,
helpers, NONMATCHING guards, and surrounding comments stay in the original file.
The selected function is regenerated, so macros inside it are expanded.
"""

from __future__ import annotations

import copy
import hashlib
import importlib
import inspect
import json
import os
import re
import subprocess
import sys
from collections import Counter, deque
from dataclasses import asdict, dataclass, field
from pathlib import Path
from typing import Any, Iterable, Iterator


DEFAULT_FAMILIES = (
    "remove_pin", "declaration_order", "statement_order", "scope_narrow", "scope_widen",
    "temp_rvalue", "copy_value", "recompute_value", "commute", "compare_reverse",
    "if_swap", "split_and", "merge_and", "guard_clause", "loop_form",
    "return_to_goto", "goto_to_return",
)
LEGACY_FAMILIES = (
    "rw_reuse_stored", "rw_related_const", "rw_copy_var", "rw_temp_rvalue",
    "rw_scoped_const", "rw_goto_return", "rw_return_for_goto", "rw_guard_clause",
    "rw_split_and", "rw_temp_arg", "rw_hoist_args", "rw_share_const",
    "rw_inline_var", "rw_bitfield", "rw_explicit_narrow", "rw_void", "rw_width",
    "rw_commute", "rw_swap_stmts", "rw_if_swap", "rw_decl_order",
)
DEFAULT_MAX_CANDIDATES = 256
MAX_GENERATED_LOCALS = 8
_TOKEN = re.compile(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*|[A-Za-z_]\w*|[^\s]', re.S)
_DIRECTIVE = re.compile(r"^[ \t]*\#(?:[^\n\\]|\\[^\n]|\\\n)*", re.M)
_STANDARD_INTS = {"char", "short", "int", "long", "signed", "unsigned", "_Bool"}


@dataclass(frozen=True)
class Move:
    id: str
    family: str
    site: str
    description: str
    proof_grade: str
    preconditions: tuple[str, ...]
    inverse: str | None
    source: str
    function_source: str
    preprocessed_source: str
    metadata: dict[str, Any] = field(default_factory=dict)

    def to_dict(self, *, include_source: bool = False) -> dict[str, Any]:
        result = asdict(self)
        if not include_source:
            for key in ("source", "function_source", "preprocessed_source"):
                result.pop(key)
        return result


@dataclass(frozen=True)
class Region:
    """An AST subtree, optionally intersected with inclusive source lines."""

    path: str = ""
    first_line: int | None = None
    last_line: int | None = None

    def accepts(self, site: str, line: int | None) -> bool:
        if self.path and not (site == self.path or site.startswith(self.path.rstrip("/") + "/")):
            return False
        if self.first_line is not None and (line is None or line < self.first_line):
            return False
        if self.last_line is not None and (line is None or line > self.last_line):
            return False
        return True


class MoveError(ValueError):
    pass


def _load_parser(repo_root: Path) -> tuple[Any, Any, str]:
    candidates = [os.environ.get("PERMUTER"), repo_root / "build/tools/decomp-permuter",
                  repo_root.parent / "decomp-permuter"]
    for candidate in candidates:
        if candidate and (Path(candidate) / "perm_pycparser").is_dir():
            if str(candidate) not in sys.path:
                sys.path.insert(0, str(candidate))
    try:
        parser = importlib.import_module("perm_pycparser.c_parser")
        ca = importlib.import_module("perm_pycparser.c_ast")
        generator = importlib.import_module("perm_pycparser.c_generator")
        return ca, (parser.CParser, generator.CGenerator), "perm_pycparser"
    except ImportError:
        parser = importlib.import_module("pycparser.c_parser")
        ca = importlib.import_module("pycparser.c_ast")
        generator = importlib.import_module("pycparser.c_generator")
        return ca, (parser.CParser, generator.CGenerator), "pycparser"


def _blank(text: str) -> str:
    return "".join("\n" if c == "\n" else " " for c in text)


def _tokens(source: str) -> list[tuple[str, int, int]]:
    source = _DIRECTIVE.sub(lambda m: _blank(m.group()), source)
    return [(m.group(), m.start(), m.end()) for m in _TOKEN.finditer(source)
            if not m.group().startswith(("//", "/*"))]


def _function_span(source: str, names: set[str]) -> tuple[int, int, str]:
    """Find a top-level function; strings/comments/directives cannot add braces."""
    toks = _tokens(source)
    depth = 0
    boundary = 0
    found = []
    i = 0
    while i < len(toks):
        token, start, end = toks[i]
        if depth == 0 and token in names and i + 1 < len(toks) and toks[i + 1][0] == "(":
            j, parens = i + 1, 0
            while j < len(toks):
                parens += (toks[j][0] == "(") - (toks[j][0] == ")")
                j += 1
                if parens == 0:
                    break
            # GCC attributes after the declarator do not terminate it.
            while j < len(toks) and toks[j][0] in {"__attribute__", "__attribute"}:
                j += 1
                level = 0
                while j < len(toks):
                    level += (toks[j][0] == "(") - (toks[j][0] == ")")
                    j += 1
                    if level == 0:
                        break
            if j < len(toks) and toks[j][0] == "{":
                k, braces = j, 0
                while k < len(toks):
                    braces += (toks[k][0] == "{") - (toks[k][0] == "}")
                    k += 1
                    if braces == 0:
                        break
                if braces:
                    raise MoveError(f"unclosed function {token}")
                found.append((toks[boundary][1], toks[k - 1][2], token))
                boundary = k
                i = k
                continue
        depth += (token == "{") - (token == "}")
        if depth == 0 and token in {";", "}"}:
            boundary = i + 1
        i += 1
    if len(found) != 1:
        raise MoveError(f"expected one definition of {sorted(names)}, found {len(found)}")
    return found[0]


def _aliases(source: str, function: str) -> set[str]:
    pairs = re.findall(r"^[ \t]*#[ \t]*define[ \t]+(\w+)[ \t]+(\w+)[ \t]*(?://[^\n]*)?$", source, re.M)
    names = {function}
    while True:
        expanded = names | {a for a, b in pairs if b in names} | {b for a, b in pairs if a in names}
        if expanded == names:
            return names
        names = expanded


def _walk(node: Any, path: str = "") -> Iterator[tuple[str, Any]]:
    yield path, node
    for name, child in node.children():
        yield from _walk(child, f"{path}/{name}".lstrip("/"))


def _get(node: Any, path: str) -> Any:
    for part in filter(None, path.split("/")):
        match = re.fullmatch(r"(\w+)\[(\d+)\]", part)
        node = getattr(node, match[1])[int(match[2])] if match else getattr(node, part)
    return node


def _set(node: Any, path: str, replacement: Any) -> None:
    parent_path, _, slot = path.rpartition("/")
    parent = _get(node, parent_path)
    match = re.fullmatch(r"(\w+)\[(\d+)\]", slot)
    if match:
        getattr(parent, match[1])[int(match[2])] = replacement
    else:
        setattr(parent, slot, replacement)


def _new(cls: Any, **kwargs: Any) -> Any:
    """The fork adds gcc_attributes/asmlabel; upstream pycparser does not."""
    params = inspect.signature(cls).parameters
    return cls(**{key: value for key, value in kwargs.items() if key in params})


def _rename_type(tp: Any, name: str | None) -> None:
    """Rename the declarator itself, never parameters nested under FuncDecl."""
    while tp is not None:
        if type(tp).__name__ == "TypeDecl":
            tp.declname = name
            return
        tp = getattr(tp, "type", None)


class MoveCatalog:
    """One immutable source node's bounded neighborhood.

    ``from_source`` accepts raw source plus the evaluator's preprocessed text.
    Without preprocessed text it invokes the project's cpp flags once, or parses
    a directive-free standalone snippet without requiring the toolchain.
    ``moves`` is lazy and caps candidates; no candidate modifies the original AST.
    """

    @classmethod
    def from_source(cls, source: str, function: str, *, preprocessed: str | None = None,
                    repo_root: str | Path | None = None, max_candidates: int = DEFAULT_MAX_CANDIDATES) -> "MoveCatalog":
        return cls(source, function, preprocessed=preprocessed, repo_root=repo_root,
                   max_candidates=max_candidates)

    def __init__(self, source: str, function: str, *, preprocessed: str | None = None,
                 repo_root: str | Path | None = None, max_candidates: int = DEFAULT_MAX_CANDIDATES):
        if max_candidates < 1:
            raise MoveError("max_candidates must be positive")
        self.source = source
        self.function = function
        self.repo_root = Path(repo_root) if repo_root else Path(__file__).resolve().parents[2]
        self.max_candidates = max_candidates
        self.parent_hash = hashlib.sha256(source.encode()).hexdigest()
        self.names = _aliases(source, function)
        self.raw_span = _function_span(source, self.names)
        if preprocessed is None:
            preprocessed = self._preprocess(source)
        self.preprocessed = preprocessed
        self.pre_span = _function_span(preprocessed, self.names)
        self.ca, (parser_cls, generator_cls), self.parser_kind = _load_parser(self.repo_root)
        self.generator = generator_cls()
        # pycparser cannot parse the assembler's file-scope macro inclusion.
        # It is deliberately left in the exported preprocessed translation unit.
        parse_text = re.sub(r'^\s*(?:__asm__|asm)\s*\(\s*"\.include (?:\\.|[^"\\])*"\s*\)\s*;[ \t]*$',
                            lambda m: _blank(m.group()), preprocessed, flags=re.M)
        parse_text = _TOKEN.sub(lambda m: _blank(m.group()) if m.group().startswith(("//", "/*")) else m.group(), parse_text)
        try:
            self.ast = parser_cls().parse(parse_text, filename="candidate.c")
        except Exception as exc:
            hint = " Install the project's decomp-permuter for GNU C." if self.parser_kind == "pycparser" else ""
            raise MoveError(f"cannot parse target C: {exc}.{hint}") from exc
        funcs = [node for node in self.ast.ext if isinstance(node, self.ca.FuncDef)
                 and node.decl.name in self.names]
        if len(funcs) != 1:
            raise MoveError(f"parser found {len(funcs)} target functions")
        self.fn = funcs[0]
        self.nodes = list(_walk(self.fn))
        declarations = []
        declaration_paths = {}
        for path, node in self.nodes:
            if not isinstance(node, self.ca.Decl) or not node.name or node is self.fn.decl:
                continue
            parent = _get(self.fn, path.rpartition("/")[0])
            parameter = bool(re.fullmatch(r"decl/type/args/params\[\d+\]", path))
            if isinstance(parent, self.ca.Compound) or parameter:
                declarations.append(node)
                declaration_paths[node.name] = path
        counts = Counter(node.name for node in declarations)
        global_names = {node.name for node in self.ast.ext if isinstance(node, self.ca.Decl)}
        self.locals = {node.name: node for node in declarations if counts[node.name] == 1
                       and node.name not in global_names and self._scalar(node.type)}
        # Name equality is not binding equality. Disqualify a unique nested
        # local if any same-named ID occurs outside its lexical scope or before
        # its declaration. This also catches references to an unseen global.
        for name in list(self.locals):
            decl_path = declaration_paths[name]
            if decl_path.startswith("decl/type/args/"):
                continue
            scope_path, _, slot = decl_path.rpartition("/")
            index = int(re.search(r"\[(\d+)\]$", slot)[1])
            for path, node in self.nodes:
                if not isinstance(node, self.ca.ID) or node.name != name:
                    continue
                parent_path, _, part = path.rpartition("/")
                if isinstance(_get(self.fn, parent_path), self.ca.StructRef) and part == "field":
                    continue
                match = re.match(re.escape(scope_path) + r"/block_items\[(\d+)\]", path)
                if match is None or int(match[1]) < index:
                    self.locals.pop(name)
                    break
        self.typedefs = {node.name: node.type for node in self.ast.ext if isinstance(node, self.ca.Typedef)}
        self.local_typedefs = {node.name for _, node in self.nodes if isinstance(node, self.ca.Typedef)}
        # Only a unique file-scope definition can establish that direct members
        # are disjoint. Tags introduced in function/prototype scopes do not
        # belong to this table; local tag shadowing is rejected separately.
        self.structs = {}
        ambiguous_tags = set()
        pending = list(self.ast.ext)
        while pending:
            node = pending.pop()
            if isinstance(node, (self.ca.FuncDef, self.ca.FuncDecl)):
                continue
            if isinstance(node, self.ca.Struct) and node.name and node.decls is not None:
                if node.name in self.structs and self.structs[node.name] is not node:
                    ambiguous_tags.add(node.name)
                self.structs[node.name] = node
            pending.extend(child for _, child in node.children())
        for name in ambiguous_tags:
            self.structs.pop(name)
        self.local_struct_tags = set()
        for path, node in self.nodes:
            if isinstance(node, self.ca.Struct) and node.name:
                parent = _get(self.fn, path.rpartition("/")[0])
                if node.decls is not None or isinstance(parent, self.ca.Decl) and parent.name is None:
                    self.local_struct_tags.add(node.name)
        self.top_locals = {node.name for node in self.fn.body.block_items or [] if isinstance(node, self.ca.Decl)}
        ft = self.fn.decl.type
        if getattr(ft, "args", None):
            self.top_locals.update(node.name for node in ft.args.params if isinstance(node, self.ca.Decl))
        self.has_jump = any(isinstance(node, (self.ca.Label, self.ca.Goto)) for _, node in self.nodes)
        self.has_asm = any(type(node).__name__ in {"Asm", "AsmStatement", "GnuAsm"} for _, node in self.nodes)
        # The fork represents extended asm as an Asm node; unknown GNU statement
        # forms must not be used to justify dataflow transformations.
        self.has_asm |= bool(re.search(r"\b(?:asm|__asm__)\s*(?:volatile\s*)?\([^)]*:",
                                      source[self.raw_span[0]:self.raw_span[1]]))
        self.has_attributes = any(getattr(node, "gcc_attributes", None) for _, node in self.nodes)
        # A new forward jump must not enter a variably modified declaration's
        # scope. Conservatively reject epilogue rewrites around array types;
        # deciding which dimensions are integer constant expressions needs a
        # type-aware evaluator, not a textual constant test.
        self.has_array_declarator = any(isinstance(node, self.ca.ArrayDecl) for _, node in self.nodes)
        self.generated_locals = sum(name.startswith("_ms_") for name in self.locals)
        self._base_function = self._text(self.fn)
        self._moves: dict[str, Move] = {}
        self._summary_cache: dict[str, Any] | None = None
        self.warnings = ["Target function is regenerated from the AST; function-local comments and macro spellings are expanded."]
        if self.parser_kind != "perm_pycparser":
            self.warnings.append("Standard pycparser fallback; GNU C needs the project's decomp-permuter.")

    def _preprocess(self, source: str) -> str:
        if not re.search(r"^\s*#", source, re.M):
            return source
        tools = str(self.repo_root / "tools")
        if tools not in sys.path:
            sys.path.insert(0, tools)
        build = importlib.import_module("build")
        command = list(build.CPP) + ["-nostdinc", "-undef"] + list(build.DEFINES)
        command += ["-DNONMATCHING", "-I", str(self.repo_root / "include"), "-iquote", str(self.repo_root), "-x", "c", "-"]
        result = subprocess.run(command, input='#line 1 "candidate.c"\n' + source, text=True,
                                capture_output=True, cwd=self.repo_root)
        if result.returncode:
            raise MoveError(f"preprocessing failed: {result.stderr[-4000:]}")
        return result.stdout

    def _text(self, node: Any) -> str:
        return self.generator.visit(node)

    def _scalar(self, tp: Any) -> bool:
        return isinstance(tp, self.ca.PtrDecl) or (isinstance(tp, self.ca.TypeDecl)
            and isinstance(tp.type, (self.ca.IdentifierType, self.ca.Enum)))

    def _integer(self, tp: Any, seen: frozenset[str] = frozenset()) -> bool:
        if isinstance(tp, self.ca.TypeDecl):
            return self._integer(tp.type, seen)
        if isinstance(tp, self.ca.Enum):
            return True
        if not isinstance(tp, self.ca.IdentifierType):
            return False
        names = set(tp.names)
        if names & self.local_typedefs:
            return False
        if names <= _STANDARD_INTS:
            return True
        if len(names) == 1:
            name = next(iter(names))
            if name in self.typedefs and name not in seen:
                return self._integer(self.typedefs[name], seen | {name})
        return False

    def _plain(self, decl: Any, *, uninitialized: bool = False) -> bool:
        if not self._scalar(decl.type) or (uninitialized and decl.init is not None):
            return False
        if set(decl.storage or []) - {"auto", "register"} or getattr(decl, "asmlabel", None):
            return False
        if getattr(decl, "gcc_attributes", None):
            return False
        return self._plain_type(decl.type)

    def _plain_type(self, tp: Any, seen: frozenset[str] = frozenset(), *, writable: bool = False) -> bool:
        """Reject observable qualifiers even when hidden behind a typedef."""
        forbidden = {"volatile", "_Atomic"} | ({"const"} if writable else set())
        for _, node in _walk(tp):
            if forbidden & set(getattr(node, "quals", None) or []):
                return False
            if type(node).__name__ in {"ArrayDecl", "Typeof"} or getattr(node, "gcc_attributes", None):
                return False
            if isinstance(node, self.ca.IdentifierType):
                names = set(node.names)
                if names & self.local_typedefs:
                    return False
                if len(names) == 1:
                    name = next(iter(names))
                    if name in self.typedefs and (name in seen or not self._plain_type(
                            self.typedefs[name], seen | {name}, writable=writable)):
                        return False
        return True

    def _independent_field_stores(self, a: Any, b: Any) -> bool:
        """A small proved subset: constant stores to distinct direct members.

        This deliberately does not infer aliases between pointers, follow a
        pointer/array access chain, or reorder any memory read. A C struct's
        distinct non-bit-field members provide the disjointness argument.
        """
        ca = self.ca
        fields = []
        bases = []
        for stmt in (a, b):
            if stmt.op != "=" or self._pure(stmt.rvalue, integral=True) != set():
                return False
            lv = stmt.lvalue
            if not (isinstance(lv, ca.StructRef) and lv.type == "->" and
                    isinstance(lv.name, ca.ID) and isinstance(lv.field, ca.ID)):
                return False
            bases.append(lv.name.name)
            fields.append(lv.field.name)
        if bases[0] != bases[1] or fields[0] == fields[1]:
            return False
        decl = self.locals.get(bases[0])
        if decl is None or not isinstance(decl.type, ca.PtrDecl) or not self._plain(decl):
            return False
        pointee = decl.type.type
        if not isinstance(pointee, ca.TypeDecl) or not self._plain_type(pointee, writable=True):
            return False
        tag = pointee.type
        if not isinstance(tag, ca.Struct) or not tag.name or tag.name in self.local_struct_tags:
            return False
        definition = self.structs.get(tag.name)
        if definition is None or getattr(definition, "gcc_attributes", None):
            return False
        members = {member.name: member for member in definition.decls or [] if member.name}
        for name in fields:
            member = members.get(name)
            if (member is None or member.bitsize is not None or not self._plain(member)
                    or not self._integer(member.type) or not self._plain_type(member.type, writable=True)):
                return False
        return True

    def _pure(self, node: Any, *, integral: bool = False) -> set[str] | None:
        """Reads of a pure scalar-local expression, or None for unknown effects."""
        ca = self.ca
        if isinstance(node, ca.Constant):
            return set() if not integral or set(node.type.split()) <= _STANDARD_INTS else None
        if isinstance(node, ca.ID):
            decl = self.locals.get(node.name)
            if decl is not None and self._plain(decl) and (not integral or self._integer(decl.type)):
                return {node.name}
            return None
        if isinstance(node, ca.UnaryOp) and node.op in {"+", "-", "!", "~"}:
            return self._pure(node.expr, integral=integral)
        if isinstance(node, ca.BinaryOp) and node.op in {"+", "-", "*", "/", "%", "&", "|", "^", "<<", ">>", "==", "!=", "<", ">", "<=", ">=", "&&", "||"}:
            left, right = self._pure(node.left, integral=integral), self._pure(node.right, integral=integral)
            return left | right if left is not None and right is not None else None
        if isinstance(node, ca.Cast) and (not integral or self._integer(node.to_type.type)):
            return self._pure(node.expr, integral=integral)
        if isinstance(node, ca.TernaryOp):
            values = [self._pure(part, integral=integral) for part in (node.cond, node.iftrue, node.iffalse)]
            return set().union(*values) if all(value is not None for value in values) else None
        return None

    def _effects(self, stmt: Any) -> tuple[set[str], set[str]] | None:
        ca = self.ca
        if isinstance(stmt, ca.Assignment) and stmt.op == "=" and isinstance(stmt.lvalue, ca.ID):
            dest = stmt.lvalue.name
            if dest in self.locals and self._plain(self.locals[dest]):
                reads = self._pure(stmt.rvalue)
                if reads is not None:
                    return reads, {dest}
        if isinstance(stmt, ca.Return):
            reads = self._pure(stmt.expr) if stmt.expr is not None else set()
            if reads is not None:
                return reads, set()
        return None

    def _name(self, base: str) -> str:
        used = {node.name for _, node in self.nodes if getattr(node, "name", None)}
        for number in range(1, 10000):
            name = f"_ms_{base}{number}"
            if name not in used:
                return name
        raise MoveError("generated-name space exhausted")

    def _decl(self, old: Any, name: str, init: Any = None) -> Any:
        result = copy.deepcopy(old)
        result.name = name
        result.init = init
        result.storage = []
        if hasattr(result, "asmlabel"):
            result.asmlabel = None
        _rename_type(result.type, name)
        return result

    def _cast(self, decl: Any, expr: Any) -> Any:
        tp = copy.deepcopy(decl.type)
        _rename_type(tp, None)
        typename = _new(self.ca.Typename, name=None, quals=[], align=[], gcc_attributes=[], type=tp)
        return self.ca.Cast(typename, copy.deepcopy(expr))

    def _replace_reads(self, stmt: Any, name: str, replacement: Any) -> None:
        for path, node in list(_walk(stmt)):
            if not isinstance(node, self.ca.ID) or node.name != name or not path:
                continue
            parent = _get(stmt, path.rpartition("/")[0])
            slot = path.rpartition("/")[2]
            if isinstance(parent, self.ca.Assignment) and slot == "lvalue":
                continue
            if isinstance(parent, self.ca.StructRef) and slot == "field":
                continue
            _set(stmt, path, copy.deepcopy(replacement))

    def _emit(self, family: str, site: str, desc: str, changed: Any,
              conditions: tuple[str, ...], inverse: str | None = None,
              grade: str = "guarded", touched_sites: Iterable[str] = ()) -> Move | None:
        body = self._text(changed)
        if body == self._base_function:
            return None
        raw_start, raw_end, raw_name = self.raw_span
        pre_start, pre_end, _ = self.pre_span
        original_body = body
        if raw_name != changed.decl.name:
            raw_fn = copy.deepcopy(changed)
            raw_fn.decl.name = raw_name
            _rename_type(raw_fn.decl.type, raw_name)
            original_body = self._text(raw_fn)
        source = self.source[:raw_start] + original_body + self.source[raw_end:]
        preprocessed = self.preprocessed[:pre_start] + body + self.preprocessed[pre_end:]
        key = json.dumps((self.parent_hash, family, site, body), separators=(",", ":"))
        identity = hashlib.sha256(key.encode()).hexdigest()[:20]
        coord = getattr(_get(self.fn, site), "coord", None) if not site.startswith("legacy:") else None
        return Move(identity, family, site, desc, grade, conditions, inverse, source, original_body,
                    preprocessed, {"parent_source_sha256": self.parent_hash,
                                   "function": self.function,
                                   "source_line": getattr(coord, "line", None),
                                   "source_file": getattr(coord, "file", None),
                                   "touched_sites": [site, *touched_sites],
                                   "inverse_parent_sha256": self.parent_hash,
                                   "parser": self.parser_kind})

    def moves(self, families: Iterable[str] | None = None, *, region: Region | str | tuple[int, int] | list[int] | dict[str, Any] | None = None,
              limit: int | None = None, max_candidates: int | None = None) -> Iterator[Move]:
        """Enumerate distinct candidates. Legacy families require explicit selection.

        A region filters the primary AST site, not every touched declaration:
        temporary moves may also insert a declaration in the enclosing block.
        ``metadata.touched_sites`` exposes that wider edit to the session.
        """
        chosen = tuple(DEFAULT_FAMILIES if families is None else families)
        allowed = set(DEFAULT_FAMILIES) | {"statement_order"} | {f"legacy:{name}" for name in LEGACY_FAMILIES}
        unknown = set(chosen) - allowed
        if unknown:
            raise MoveError(f"unknown move families: {sorted(unknown)}")
        if isinstance(region, str):
            region = Region(path=region)
        elif isinstance(region, (tuple, list)):
            if len(region) != 2:
                raise MoveError("line region needs [first_line, last_line]")
            region = Region(first_line=region[0], last_line=region[1])
        elif isinstance(region, dict):
            region = Region(**region)
        cap = min(self.max_candidates, limit if limit is not None else self.max_candidates)
        if max_candidates is not None:
            cap = min(cap, max_candidates)
        if cap <= 0:
            return
        seen = set()
        count = 0
        pending = deque(self._legacy(family[7:]) if family.startswith("legacy:") else self._family(family)
                        for family in chosen)
        # Round-robin families: hundreds of arithmetic sites must not conceal
        # the single loop/control-flow move at the tail of a large function.
        while pending:
            iterator = pending.popleft()
            for move in iterator:
                if move is None:
                    continue
                if region and not region.accepts(move.site, move.metadata["source_line"]):
                    continue
                source_hash = hashlib.sha256(move.function_source.encode()).digest()
                if source_hash in seen:
                    continue
                seen.add(source_hash)
                self._moves[move.id] = move
                yield move
                count += 1
                if count >= cap:
                    return
                pending.append(iterator)
                break

    def apply(self, move_id: str) -> str:
        if move_id not in self._moves:
            for move in self.moves():
                if move.id == move_id:
                    break
        if move_id not in self._moves:
            raise MoveError(f"unknown move {move_id}; enumerate its family first")
        return self._moves[move_id].source

    def summary(self) -> dict[str, Any]:
        if self._summary_cache is not None:
            return dict(self._summary_cache)
        counts = Counter(move.family for move in self.moves())
        self._summary_cache = {"function": self.function, "parent_source_sha256": self.parent_hash,
                "families": dict(counts), "candidate_count": sum(counts.values()),
                "max_candidates": self.max_candidates,
                "possibly_truncated": sum(counts.values()) == self.max_candidates,
                "warnings": list(self.warnings)}
        return dict(self._summary_cache)

    def _family(self, family: str) -> Iterator[Move | None]:
        ca = self.ca
        if family in {"declaration_order", "statement_order", "scope_narrow", "scope_widen",
                      "copy_value", "recompute_value"}:
            yield from self._block_moves(family)
            return
        if family == "remove_pin":
            if self.has_asm:
                return
            pins = [path for path, node in self.nodes if isinstance(node, ca.Decl)
                    and node is not self.fn.decl and getattr(node, "asmlabel", None)
                    and "register" in (node.storage or [])]
            for sites in [[path] for path in pins] + ([pins] if len(pins) > 1 else []):
                changed = copy.deepcopy(self.fn)
                names = []
                for path in sites:
                    decl = _get(changed, path)
                    names.append(decl.name)
                    decl.asmlabel = None
                    decl.storage = [item for item in decl.storage if item != "register"]
                yield self._emit(family, sites[0], "remove register pin: " + ", ".join(names), changed,
                                 ("Local register declarations only", "No extended asm uses in target"),
                                 touched_sites=sites[1:])
            return
        for path, node in self.nodes:
            changed = None
            inverse = None
            conditions: tuple[str, ...] = ()
            desc = ""
            if family in {"commute", "compare_reverse"} and isinstance(node, ca.BinaryOp):
                choices = {"+", "*", "&", "|", "^", "==", "!="} if family == "commute" else {"<", "<=", ">", ">="}
                if node.op not in choices or self._pure(node, integral=True) is None:
                    continue
                changed = copy.deepcopy(self.fn)
                target = _get(changed, path)
                target.left, target.right = target.right, target.left
                if family == "compare_reverse":
                    target.op = {"<": ">", "<=": ">=", ">": "<", ">=": "<="}[target.op]
                desc = f"reverse operands of {self._text(node)[:80]}"
                conditions = ("Both operands are pure integer expressions over unique nonvolatile locals",)
                inverse = family
            elif family == "if_swap" and isinstance(node, ca.If) and node.iffalse is not None:
                changed = copy.deepcopy(self.fn)
                target = _get(changed, path)
                target.cond = ca.UnaryOp("!", target.cond)
                target.iftrue, target.iffalse = target.iffalse, target.iftrue
                desc, inverse = "negate condition and exchange if/else arms", family
                conditions = ("Condition evaluated once; both original branch scopes retained",)
            elif family == "split_and" and isinstance(node, ca.If) and node.iffalse is None and isinstance(node.cond, ca.BinaryOp) and node.cond.op == "&&":
                changed = copy.deepcopy(self.fn)
                target = _get(changed, path)
                replacement = ca.If(target.cond.left, ca.Compound([ca.If(target.cond.right, target.iftrue, None)]), None)
                _set(changed, path, replacement)
                desc, inverse = "split && into nested if statements", "merge_and"
                conditions = ("Short-circuit order and evaluation counts retained",)
            elif family == "merge_and" and isinstance(node, ca.If) and node.iffalse is None:
                inside = node.iftrue
                if isinstance(inside, ca.Compound) and len(inside.block_items or []) == 1:
                    inside = inside.block_items[0]
                if not isinstance(inside, ca.If) or inside.iffalse is not None:
                    continue
                changed = copy.deepcopy(self.fn)
                target = _get(changed, path)
                inner = target.iftrue.block_items[0] if isinstance(target.iftrue, ca.Compound) else target.iftrue
                target.cond = ca.BinaryOp("&&", target.cond, inner.cond)
                target.iftrue = inner.iftrue
                desc, inverse = "merge nested tests into &&", "split_and"
                conditions = ("Neither test has an else arm; no declarations between tests",)
            elif family == "guard_clause" and isinstance(node, ca.If) and node.iffalse is None:
                parent_path, _, slot = path.rpartition("/")
                parent = _get(self.fn, parent_path)
                if not isinstance(parent, ca.Compound):
                    continue
                index = parent.block_items.index(node)
                if index + 1 >= len(parent.block_items) or not isinstance(parent.block_items[index + 1], ca.Return):
                    continue
                changed = copy.deepcopy(self.fn)
                block = _get(changed, parent_path)
                target = block.block_items[index]
                block.block_items[index:index + 1] = [ca.If(ca.UnaryOp("!", target.cond),
                    copy.deepcopy(block.block_items[index + 1]), None), target.iftrue]
                desc = "replace nested if before return with an early guard"
                conditions = ("Following statement is return", "Original body scope is retained, never flattened")
            elif family == "temp_rvalue" and self.generated_locals < MAX_GENERATED_LOCALS:
                if not isinstance(node, ca.Assignment) or node.op != "=" or not isinstance(node.lvalue, ca.ID):
                    continue
                name = node.lvalue.name
                decl = self.locals.get(name)
                parent = _get(self.fn, path.rpartition("/")[0])
                if decl is None or not self._plain(decl) or not isinstance(parent, ca.Compound) or name.startswith("_ms_"):
                    continue
                if isinstance(node.rvalue, (ca.ID, ca.Constant)):
                    continue
                changed = copy.deepcopy(self.fn)
                target = _get(changed, path)
                temporary = self._name("value")
                _set(changed, path, ca.Compound([self._decl(decl, temporary, target.rvalue),
                                               ca.Assignment("=", target.lvalue, ca.ID(temporary))]))
                desc = f"compute {name}'s value in same-type temporary {temporary}"
                conditions = ("Destination is a unique nonvolatile scalar local", "Temporary has destination's exact type", "RHS is evaluated once at its original statement")
            elif family == "loop_form" and isinstance(node, (ca.While, ca.For)):
                changed = copy.deepcopy(self.fn)
                target = _get(changed, path)
                if isinstance(target, ca.While):
                    replacement = ca.For(None, target.cond, None, target.stmt)
                    desc = "while to for with empty initialization and increment"
                    conditions = ("Identical condition, body, and continue target",)
                elif target.init is None and target.next is None:
                    replacement = ca.While(target.cond or ca.Constant("int", "1"), target.stmt)
                    desc = "for with empty init/increment to while"
                    conditions = ("Identical condition, body, and continue target",)
                else:
                    if any(isinstance(n, (ca.Continue, ca.Label, ca.Goto)) for _, n in _walk(target.stmt)) or self.has_jump:
                        continue
                    body = [target.stmt] + ([target.next] if target.next is not None else [])
                    prefix = target.init.decls if isinstance(target.init, ca.DeclList) else ([target.init] if target.init is not None else [])
                    replacement = ca.Compound(prefix + [ca.While(target.cond or ca.Constant("int", "1"), ca.Compound(body))])
                    desc = "for to scoped initialization and while"
                    conditions = ("No continue, labels, or goto", "Original body scope retained; increment executes after each completed iteration")
                _set(changed, path, replacement)
                inverse = family
            elif family == "return_to_goto" and isinstance(node, ca.Return):
                if not path or self.has_asm or self.has_attributes or self.has_array_declarator:
                    continue
                value = node.expr
                if value is not None and not (isinstance(value, ca.Constant) or isinstance(value, ca.ID) and value.name in self.top_locals and value.name in self.locals):
                    continue
                # The final return cannot improve layout by jumping to itself.
                if self.fn.body.block_items and self.fn.body.block_items[-1] is node:
                    continue
                changed = copy.deepcopy(self.fn)
                label = self._name("return")
                target = _get(changed, path)
                _set(changed, path, ca.Goto(label))
                tail = changed.body.block_items
                if not isinstance(tail[-1], ca.Return):
                    # Falling off a nonvoid function is permitted when its
                    # caller discards the value. It must not newly evaluate an
                    # uninitialized or volatile return local along that path.
                    end = self._name("fallthrough")
                    tail.extend([ca.Goto(end), ca.Label(label, target),
                                 ca.Label(end, ca.EmptyStatement())])
                else:
                    tail.append(ca.Label(label, target))
                desc, inverse = "route early return through a shared epilogue label", "goto_to_return"
                conditions = ("Return value is void, constant, or unique function-scope scalar", "Goto skips all intervening computation; existing fallthrough bypasses the new return", "No array declarators or attributes")
            elif family == "goto_to_return" and isinstance(node, ca.Goto):
                if self.has_attributes or self.has_array_declarator:
                    continue
                labels = {n.name: n.stmt for _, n in self.nodes if isinstance(n, ca.Label) and isinstance(n.stmt, ca.Return)}
                target_return = labels.get(node.name)
                if target_return is None:
                    continue
                value = target_return.expr
                if value is not None and not (isinstance(value, ca.Constant) or isinstance(value, ca.ID) and value.name in self.top_locals and value.name in self.locals):
                    continue
                changed = copy.deepcopy(self.fn)
                _set(changed, path, copy.deepcopy(target_return))
                desc, inverse = f"return directly instead of goto {node.name}", "return_to_goto"
                conditions = ("Label contains only a return", "Value has the same binding at both sites")
            if changed is not None:
                yield self._emit(family, path, desc, changed, conditions, inverse)

    def _block_moves(self, family: str) -> Iterator[Move | None]:
        ca = self.ca
        for block_path, block in self.nodes:
            if not isinstance(block, ca.Compound) or not block.block_items:
                continue
            items = block.block_items
            if family == "declaration_order":
                for i in range(len(items) - 1):
                    a, b = items[i:i + 2]
                    if not all(isinstance(d, ca.Decl) and d.name in self.locals and self._plain(d) for d in (a, b)):
                        continue
                    if a.init is not None and b.init is not None:
                        reads_a, reads_b = self._pure(a.init), self._pure(b.init)
                        if reads_a is None or reads_b is None or b.name in reads_a or a.name in reads_b:
                            continue
                    else:
                        initialized, other = (a, b) if a.init is not None else (b, a)
                        if initialized.init is not None and any(isinstance(n, ca.ID) and n.name == other.name for _, n in _walk(initialized.init)):
                            continue
                    changed = copy.deepcopy(self.fn)
                    target = _get(changed, block_path).block_items
                    target[i], target[i + 1] = target[i + 1], target[i]
                    site = f"{block_path}/block_items[{i}]"
                    yield self._emit(family, site, f"exchange declarations of {a.name} and {b.name}", changed,
                                     ("Adjacent unique scalar declarations; no binding dependence", "At most one initializer, or two independent pure initializers", "No VLAs, attributes, or volatile types"), family,
                                     touched_sites=[f"{block_path}/block_items[{i + 1}]"])
            elif family == "statement_order":
                for i in range(len(items) - 1):
                    a, b = items[i:i + 2]
                    if not isinstance(a, ca.Assignment) or not isinstance(b, ca.Assignment) or self.has_asm:
                        continue
                    ea, eb = self._effects(a), self._effects(b)
                    if ea is not None and eb is not None:
                        if ea[1] & (eb[0] | eb[1]) or eb[1] & (ea[0] | ea[1]):
                            continue
                        description = "exchange independent scalar assignments"
                        conditions = ("Disjoint write/write and read/write sets",
                                      "No calls, memory accesses, volatile, or address computations")
                    elif self._independent_field_stores(a, b):
                        description = "exchange independent struct field stores"
                        conditions = ("Distinct non-bit-field integer members of one resolved struct",
                                      "Same locally bound pointer; no union, volatile, or atomic access",
                                      "Integer constant RHSs; no calls, memory reads, or address computations")
                    else:
                        continue
                    changed = copy.deepcopy(self.fn)
                    target = _get(changed, block_path).block_items
                    target[i], target[i + 1] = target[i + 1], target[i]
                    yield self._emit(family, f"{block_path}/block_items[{i}]", description, changed,
                                     conditions, family,
                                     touched_sites=[f"{block_path}/block_items[{i + 1}]"])
            elif family in {"copy_value", "recompute_value"}:
                if self.has_asm or self.generated_locals >= MAX_GENERATED_LOCALS:
                    continue
                for i, assignment in enumerate(items):
                    if not isinstance(assignment, ca.Assignment) or assignment.op != "=" or not isinstance(assignment.lvalue, ca.ID):
                        continue
                    name = assignment.lvalue.name
                    decl = self.locals.get(name)
                    if decl is None or not self._plain(decl) or name.startswith("_ms_"):
                        continue
                    dependencies = self._pure(assignment.rvalue, integral=True) if family == "recompute_value" else set()
                    if dependencies is None or name in dependencies:
                        continue
                    uses = []
                    for j in range(i + 1, len(items)):
                        effects = self._effects(items[j])
                        if effects is None:
                            break
                        reads, writes = effects
                        if name in writes or writes & dependencies:
                            break
                        if name in reads:
                            uses.append(j)
                        if isinstance(items[j], ca.Return):
                            break
                    if not uses:
                        continue
                    for selected in [uses[:1]] + ([uses] if len(uses) > 1 else []):
                        changed = copy.deepcopy(self.fn)
                        target_block = _get(changed, block_path)
                        target_items = target_block.block_items
                        if family == "copy_value":
                            temporary = self._name("copy")
                            replacement = ca.ID(temporary)
                            description = f"copy {name} into {temporary} for {len(selected)} following use(s)"
                        else:
                            replacement = self._cast(decl, assignment.rvalue)
                            description = f"recompute {name} with its original assignment conversion at {len(selected)} use(s)"
                        for j in selected:
                            self._replace_reads(target_items[j], name, replacement)
                        if family == "copy_value":
                            target_items.insert(i + 1, ca.Assignment("=", ca.ID(temporary), ca.ID(name)))
                            first_stmt = next((k for k, item in enumerate(target_items) if not isinstance(item, ca.Decl)), len(target_items))
                            target_items.insert(first_stmt, self._decl(decl, temporary))
                        conditions = ("Unique nonvolatile scalar local", "Straight-line local expressions only; stop at calls, memory, control flow, or relevant writes",
                                      "Original assignment retained" if family == "recompute_value" else "Copied value used only before it changes")
                        yield self._emit(family, f"{block_path}/block_items[{i}]", description, changed, conditions,
                                         touched_sites=[f"{block_path}/block_items[{j}]" for j in selected])
            elif family in {"scope_narrow", "scope_widen"} and not self.has_jump and not self.has_asm:
                # Only direct blocks and if arms: moving an uninitialized local
                # into a loop can destroy a value carried across iterations.
                children = []
                for i, statement in enumerate(items):
                    if isinstance(statement, ca.Compound):
                        children.append((f"{block_path}/block_items[{i}]", statement))
                    elif isinstance(statement, ca.If):
                        for arm in ("iftrue", "iffalse"):
                            child = getattr(statement, arm)
                            if isinstance(child, ca.Compound):
                                children.append((f"{block_path}/block_items[{i}]/{arm}", child))
                for child_path, child in children:
                    declarations = items if family == "scope_narrow" else child.block_items or []
                    for decl in declarations:
                        if not isinstance(decl, ca.Decl) or decl.name not in self.locals or not self._plain(decl, uninitialized=True):
                            continue
                        name = decl.name
                        refs = [path for path, n in self.nodes if isinstance(n, ca.ID) and n.name == name]
                        if not refs or not all(path.startswith(child_path + "/") for path in refs):
                            continue
                        if any(isinstance(n, ca.UnaryOp) and n.op == "&" and any(isinstance(x, ca.ID) and x.name == name for _, x in _walk(n.expr)) for _, n in self.nodes):
                            continue
                        # Typedefs introduced inside the child may be unavailable
                        # outside it. Only TU-known scalar types can move outward.
                        if family == "scope_widen" and not self._integer(decl.type):
                            continue
                        changed = copy.deepcopy(self.fn)
                        parent_items = _get(changed, block_path).block_items
                        child_items = _get(changed, child_path).block_items
                        if child_items is None:
                            continue
                        src_items, dst_items = (parent_items, child_items) if family == "scope_narrow" else (child_items, parent_items)
                        index = next(i for i, n in enumerate(src_items) if isinstance(n, ca.Decl) and n.name == name)
                        moving = src_items.pop(index)
                        dst_items.insert(0, moving)
                        site = f"{block_path}/block_items[{declarations.index(decl)}]" if family == "scope_narrow" else f"{child_path}/block_items[{declarations.index(decl)}]"
                        yield self._emit(family, site, f"{'narrow' if family == 'scope_narrow' else 'widen'} declaration scope of {name}", changed,
                                         ("Unique uninitialized nonvolatile scalar; address never taken", "Every reference is within one nonloop child block", "No labels or goto"),
                                         "scope_widen" if family == "scope_narrow" else "scope_narrow", touched_sites=[child_path, block_path])

    def _legacy(self, name: str) -> Iterator[Move | None]:
        if self.parser_kind != "perm_pycparser":
            raise MoveError("legacy rewrites require the project's perm_pycparser")
        tools = str(self.repo_root / "tools")
        if tools not in sys.path:
            sys.path.insert(0, tools)
        rewrites = importlib.import_module("rewrites")
        generator = getattr(rewrites, name)
        for i, (description, changed) in enumerate(generator(self.fn)):
            yield self._emit(f"legacy:{name}", f"legacy:{name}[{i}]", description, changed,
                             ("Unproved legacy rewrite: caller must establish type, alias, dominance, and effect preconditions",),
                             grade="hypothesis")
