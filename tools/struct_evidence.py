"""Typed source and ROM observations for private-record recovery."""
from __future__ import annotations

from collections import defaultdict
from pathlib import Path
import re

import layoutmerge as lm
from pycparser import c_ast, c_parser


class EvidenceParser(c_parser.CParser):
    """Parse GNU statement expressions wherever a primary expression can appear."""

    def _parse_assignment_expression(self):
        # pycparser recognizes a statement expression here and returns early,
        # leaving following operators such as << unconsumed.
        expr = self._parse_conditional_expression()
        if self._is_assignment_op():
            op = self._advance().value
            return c_ast.Assignment(op, expr, self._parse_assignment_expression(), expr.coord)
        return expr

    def _parse_primary_expression(self):
        if self._peek_type() == "LPAREN" and self._peek_type(2) == "LBRACE":
            self._advance()
            expr = self._parse_compound_statement()
            self._expect("RPAREN")
            return expr
        return super()._parse_primary_expression()


def record_key(node, spell=None):
    if not node.coord:
        return None
    path = Path(node.coord.file).as_posix()
    if path.startswith(("src/", "include/")):
        return f"{path}:{spell or f'{lm.kind_of(node)} {node.name}'}"
    return None


def record_of(tu, t):
    agg = tu.aggregate(t)
    if agg:
        spell, node = agg
        if node.name:
            spell = f"{lm.kind_of(node)} {node.name}"
        if spell and node.decls is not None:
            return record_key(node, spell)
    return None


def const_int(expr):
    if isinstance(expr, c_ast.Constant) and any(word in expr.type for word in ("int", "long")):
        value = re.sub(r"[uUlL]+$", "", expr.value)
        try:
            return int(value, 8 if re.fullmatch(r"0[0-7]+", value) else 0)
        except ValueError:
            return None
    if isinstance(expr, c_ast.UnaryOp):
        n = const_int(expr.expr)
        if n is not None:
            return {"+": n, "-": -n, "~": ~n}.get(expr.op)
    if isinstance(expr, c_ast.BinaryOp):
        a, b = const_int(expr.left), const_int(expr.right)
        if a is not None and b is not None:
            if expr.op == "+":
                return a + b
            if expr.op == "-":
                return a - b
            if expr.op == "*":
                return a * b
            if expr.op == "<<" and 0 <= b < 32:
                return a << b
            if expr.op == ">>" and 0 <= b < 32:
                return a >> b
    return None


class References(c_ast.NodeVisitor):
    """Resolve local names, simple pointer arguments, and member selections."""

    def __init__(self, ast, tu, path):
        self.tu, self.path = tu, path
        self.env = [{}]
        self.declarations = {}
        self.static = set()
        self.func = ""
        self.calls, self.fields, self.params, self.globals = [], [], [], []
        for ext in ast.ext:
            d = ext.decl if isinstance(ext, c_ast.FuncDef) else ext
            if isinstance(d, c_ast.Decl) and d.name:
                if isinstance(d.type, c_ast.FuncDecl):
                    self.declarations[d.name] = d.type
                    if "static" in d.storage:
                        self.static.add(d.name)
                else:
                    self.env[0][d.name] = d.type
                    if Path(d.coord.file).as_posix() == path:
                        self.globals.append((d.name, record_of(tu, d.type), self.pointee(d.type)))
        for ext in ast.ext:
            if ext.coord and Path(ext.coord.file).as_posix() == path:
                self.visit(ext)

    def resolve(self, t):
        return self.tu.resolve(t)

    def pointee(self, t):
        t = self.resolve(t)
        return record_of(self.tu, t.type) if isinstance(t, (c_ast.PtrDecl, c_ast.ArrayDecl)) else None

    def binding(self, name):
        return next((env[name] for env in reversed(self.env) if name in env), None)

    def member(self, expr):
        t = self.resolve(self.expr_type(expr.name))
        if expr.type == "->":
            t = t.type if isinstance(t, (c_ast.PtrDecl, c_ast.ArrayDecl)) else None
        agg = self.tu.aggregate(t)
        d = next((d for d in agg[1].decls or [] if d.name == expr.field.name), None) if agg else None
        return record_of(self.tu, t), d

    def expr_type(self, expr):
        if isinstance(expr, c_ast.ID):
            return self.binding(expr.name)
        if isinstance(expr, c_ast.Cast):
            return expr.to_type.type
        if isinstance(expr, c_ast.StructRef):
            _, member = self.member(expr)
            return member.type if member else None
        if isinstance(expr, c_ast.ArrayRef):
            t = self.resolve(self.expr_type(expr.name))
            return t.type if isinstance(t, (c_ast.PtrDecl, c_ast.ArrayDecl)) else None
        if isinstance(expr, c_ast.UnaryOp):
            t = self.expr_type(expr.expr)
            if expr.op == "&" and t:
                return c_ast.PtrDecl([], t)
            t = self.resolve(t)
            if expr.op == "*" and isinstance(t, c_ast.PtrDecl):
                return t.type
        if isinstance(expr, c_ast.FuncCall) and isinstance(expr.name, c_ast.ID):
            f = self.declarations.get(expr.name.name)
            if f:
                return f.type
        return None

    def address(self, expr):
        """An address relative to a typed object, before looking up its layout."""
        if isinstance(expr, c_ast.Cast):
            # Preserve the container through a cast to the callee's pointer type.
            return self.address(expr.expr)
        if isinstance(expr, c_ast.UnaryOp) and expr.op == "&":
            return self.lvalue(expr.expr)
        if isinstance(expr, c_ast.BinaryOp) and expr.op in {"+", "-"}:
            t = self.resolve(self.expr_type(expr.left))
            delta = const_int(expr.right)
            if delta is None and expr.op == "+" and const_int(expr.left) is not None:
                return self.address(c_ast.BinaryOp("+", expr.right, expr.left))
            if isinstance(t, (c_ast.ArrayDecl, c_ast.PtrDecl)) and delta is not None:
                w, _, base = self.tu.scalar(t.type)
                if w == 1 and base == "int":
                    origin = self.address(expr.left)
                    if origin:
                        rec, steps, offset = origin
                        return rec, steps, offset + (delta if expr.op == "+" else -delta)
            return None
        t = self.resolve(self.expr_type(expr))
        if isinstance(t, c_ast.ArrayDecl) and isinstance(expr, c_ast.StructRef):
            return self.lvalue(expr)
        rec = self.pointee(t)
        return (rec, (), 0) if rec else None

    def lvalue(self, expr):
        if isinstance(expr, c_ast.StructRef):
            parent = self.address(expr.name) if expr.type == "->" else self.lvalue(expr.name)
            if parent:
                rec, steps, offset = parent
                return rec, (*steps, expr.field.name), offset
        elif isinstance(expr, c_ast.ArrayRef):
            # Only arrays embedded inline contribute to their parent's offset.
            if not isinstance(self.resolve(self.expr_type(expr.name)), c_ast.ArrayDecl):
                return None
            index = const_int(expr.subscript)
            parent = self.lvalue(expr.name)
            if parent and index is not None and index >= 0:
                rec, steps, offset = parent
                return rec, (*steps, index), offset
        elif isinstance(expr, c_ast.UnaryOp) and expr.op == "*":
            return self.address(expr.expr)
        else:
            rec = record_of(self.tu, self.expr_type(expr))
            if rec:
                return rec, (), 0
        return None

    def visit_FuncDef(self, node):
        self.func = node.decl.name
        params = node.decl.type.args
        self.env.append({})
        abi = record_of(self.tu, node.decl.type.type) is None
        for i, p in enumerate(params.params if params else ()):
            if not isinstance(p, c_ast.Decl):
                break
            self.env[-1][p.name] = p.type
            rec = self.pointee(p.type)
            if rec:
                self.params.append((self.func, i, rec, abi and i < 4, self.func in self.static))
            t = self.resolve(p.type)
            w, _, _ = self.tu.scalar(t)
            abi = abi and record_of(self.tu, t) is None and (w in (1, 2, 4) or isinstance(t, c_ast.ArrayDecl))
        self.visit(node.body)
        self.env.pop()
        self.func = ""

    def visit_Compound(self, node):
        self.env.append({})
        self.generic_visit(node)
        self.env.pop()

    visit_For = visit_Compound

    def visit_Decl(self, node):
        if self.func and node.name:
            self.env[-1][node.name] = node.type
        self.generic_visit(node)

    def visit_FuncCall(self, node):
        if isinstance(node.name, c_ast.ID) and isinstance(node.args, c_ast.ExprList):
            name = node.name.name
            if self.binding(name) is None:
                declared = self.declarations.get(name)
                formals = declared.args.params if declared and declared.args else ()
                for i, arg in enumerate(node.args.exprs):
                    origin = self.address(arg)
                    if origin:
                        dest = self.pointee(formals[i].type) if i < len(formals) and isinstance(
                            formals[i], (c_ast.Decl, c_ast.Typename)) else None
                        cast = self.pointee(arg.to_type.type) if isinstance(arg, c_ast.Cast) else None
                        self.calls.append((self.func, node.coord.line, name, i, name in self.static,
                                           origin, dest, cast))
        self.generic_visit(node)

    def visit_StructRef(self, node):
        rec, _ = self.member(node)
        coord = node.field.coord
        if coord and Path(coord.file).as_posix() == self.path:
            self.fields.append((rec, node.field.name, coord.line, coord.column))
        self.generic_visit(node)


def parse(text, path, *, declarations_only=False):
    body = lm.strip_bodies(text)
    error = None
    try:
        ast = EvidenceParser().parse(lm.pycparser_text(lm.strip_gnu(body if declarations_only else text)), path)
    except c_parser.ParseError as exc:
        if declarations_only:
            raise
        error = str(exc)
        ast = EvidenceParser().parse(lm.pycparser_text(lm.strip_gnu(body)), path)
    # Prevent local tags from replacing the file-scope definitions in layoutmerge.TU.
    ext = [node.decl if isinstance(node, c_ast.FuncDef) else node for node in ast.ext]
    tu = lm.TU(c_ast.FileAST(ext))
    return body, ast, tu, error


def selected_records(tu, path):
    records = {record_key(node): (f"{kind} {tag}", node)
               for (kind, tag), node in tu.tags.items() if node.coord and Path(node.coord.file).as_posix() == path}
    for alias, t in tu.typedefs.items():
        agg = tu.aggregate(t)
        if agg and agg[1].name is None and agg[1].coord and Path(agg[1].coord.file).as_posix() == path:
            records.setdefault(record_key(agg[1], alias), (alias, agg[1]))
    return records


def member_dependencies(tu, node, parent_spell):
    headers, private = set(), set()
    self_ref = False

    class Deps(c_ast.NodeVisitor):
        def add(self, name, where):
            path = Path(where.file).as_posix() if where else ""
            if path.startswith("include/"):
                headers.add(path)
            else:
                private.add(name)

        def visit_IdentifierType(self, base):
            if len(base.names) == 1 and base.names[0] in tu.typedefs:
                name = base.names[0]
                self.add(name, tu.typedefs[name].coord)

        def visit_Struct(self, base):
            nonlocal self_ref
            kind = lm.kind_of(base)
            if base.decls is not None:
                self.generic_visit(base)
            else:
                spell = f"{kind} {base.name}"
                decl = tu.tags.get((kind, base.name))
                if spell == parent_spell:
                    self_ref = True
                else:
                    self.add(spell, decl.coord if decl else None)

        visit_Union = visit_Struct

        def visit_Enum(self, base):
            # A named enum reference also needs its complete declaration.
            if base.values is None:
                private.add(f"enum {base.name}")

    Deps().visit(node.type)
    return sorted(headers), sorted(private), self_ref


def rom_loads(params):
    """Yield (record, offset, width, signed, function, ROM PC) for proven entry bases."""
    from matchsearch import constraints
    from progress import load_functions
    from scope import scope

    funcs, sizes = load_functions()
    addresses = dict(funcs)
    rom = scope.rom_bytes()
    arm = {a for a, mode in scope.arm_starts() if mode == "arm"}
    for func, regs in sorted(params.items()):
        addr = addresses.get(func)
        if addr is None or addr in arm:
            continue
        code = rom[scope.off(addr):scope.off(addr) + sizes[func]]
        decoded = scope.decode(code, addr, scope.rom_data(func, addr, code))
        graph = constraints.analyze(decoded, address=addr)
        live = {graph.instructions[i].offset for b in graph.blocks if b.reachable for i in b.instructions
                if graph.instructions[i].opcode.startswith("ldr")}
        for access in graph.resolved_addresses:
            terms = access["terms"]
            if access["offset"] not in live or access["kind"] != "read" or len(terms) != 1:
                continue
            term = terms[0]
            rec = regs.get(term["value"])
            if rec and term["coefficient"] == 1 and access["displacement"] >= 0:
                width = access["width"] // 8
                yield rec, (access["displacement"], width, access["signed"] if width < 4 else None,
                            func, addr + access["offset"])
