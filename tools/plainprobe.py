#!/usr/bin/env python3
"""Measure plain literal forms one site at a time, using stock check.py linking.

  build/venv/bin/python tools/plainprobe.py [PATH ...] [--sample-only] [--resume]
  build/venv/bin/python tools/plainprobe.py --apply-plain PATH ...

The default sweep visits each linter binding or argument once. A fixed random sample
of up to 40 sites per src directory runs first. Weighted linter counts are retained
in the CSV. Header probes compare scratch include copies through their dependents.
"""
from __future__ import annotations

import argparse
import ast
import bisect
import csv
import fcntl
import hashlib
import json
import operator
import os
import random
import re
import sys
import tempfile
from collections import Counter, defaultdict
from concurrent.futures import ThreadPoolExecutor
from dataclasses import asdict, dataclass, field as dataclass_field
from functools import lru_cache
from pathlib import Path

import build
import check
import concessions
import fold_literals as folds
import keepcheck
import rawfield

ROOT = Path(keepcheck.ROOT)
OUT = ROOT / "build/plainprobe"
NOTE = ROOT / "notes/plainprobe_2026-11-27.md"
REPORT = ROOT / "build/REPORT.md"
STOP = ROOT / "build/STOP"
LOCAL_KINDS = ("literal-local", "const-local")
CLASSES = ("PLAIN", "BOUND-SINGLE", "BOUND-SHARED", "SKIP")
REVIEW_MARK = "<!-- Worker interpretation below. Preserved on resume. -->"
ID = r"[A-Za-z_]\w*"
MEMBER = rf"{ID}(?:(?:->|\.){ID}|\[(?:{ID}|\d+)\])*"
FIELD_EXPR = rf"(?:\*\(?{ID}\)?|{MEMBER})"
NUM = r"(?:0[xX][0-9a-fA-F]+|[0-9]+)[uUlL]*"
TOKEN = re.compile(rf"{ID}|{NUM}")
CONST_ATOM = rf"(?:{NUM}|[A-Z][A-Z0-9_]*)"
CONST_EXPR = re.compile(rf"(?<!\w){CONST_ATOM}(?:\s*(?:<<|>>|[+*|^&-])\s*{CONST_ATOM})+(?!\w)")
DEFINE = re.compile(rf"(?m)^[ \t]*#[ \t]*define[ \t]+({ID})[ \t]+([^\n]+)")
PARAM = re.compile(rf"(?P<type>.*?)\s*(?P<name>{ID})\s*$")
BINOPS = {ast.Add: operator.add, ast.Sub: operator.sub, ast.Mult: operator.mul,
          ast.LShift: operator.lshift, ast.RShift: operator.rshift, ast.BitAnd: operator.and_,
          ast.BitOr: operator.or_, ast.BitXor: operator.xor}
UNOPS = {ast.UAdd: operator.pos, ast.USub: operator.neg, ast.Invert: operator.invert}


def compact(text):
    return re.sub(r"\s+", "", text)


def unparen(text):
    while text.startswith("("):
        args, end = concessions._arguments(text, 1)
        if args is None or len(args) != 1 or end != len(text):
            break
        text = args[0].strip()
    return text


def constant(text, definitions):
    """Evaluate the linter's simple integer forms for sharing and conversion checks."""
    def evaluate(node, trail):
        if isinstance(node, ast.Constant) and type(node.value) is int:
            return node.value
        if isinstance(node, ast.Name) and node.id not in trail:
            value = definitions[node.id]
            if isinstance(value, int):
                return value
            return evaluate(tree(value), trail | {node.id})
        if isinstance(node, ast.UnaryOp):
            return UNOPS[type(node.op)](evaluate(node.operand, trail))
        if isinstance(node, ast.BinOp):
            a, b = evaluate(node.left, trail), evaluate(node.right, trail)
            if isinstance(node.op, (ast.LShift, ast.RShift)) and not 0 <= b <= 31:
                raise ValueError
            return BINOPS[type(node.op)](a, b)
        raise ValueError

    def tree(value):
        value = re.sub(rf"\b{NUM}\b", lambda m: str(folds.value(m[0])), value)
        return ast.parse(value.strip(), mode="eval").body

    try:
        return evaluate(tree(text), set())
    except (SyntaxError, ValueError, KeyError, RecursionError):
        return None


@dataclass(frozen=True)
class Helper:
    params: tuple
    types: tuple
    target: str
    mask: str
    operation: str
    result: str
    alias: tuple = ()
    complement_type: str = ""


@dataclass(frozen=True)
class InlineBody:
    prefix: str
    params: tuple
    body: str


def operation(body, mask):
    """Recognize one mask expression, including its simple assignment/boolean spellings."""
    body = re.sub(rf"\(({FIELD_EXPR})\)", r"\1", body) if "if(" not in body else body
    expr = rf"(?P<target>{FIELD_EXPR})"
    mask = re.escape(mask)
    if m := re.fullmatch(rf"{expr}(?P<op>\|=|&=)(?P<neg>~?){mask};", body):
        op = {("|=", ""): "set", ("&=", ""): "and-store", ("&=", "~"): "clear"}.get((m["op"], m["neg"]))
        return (m["target"], op) if op else None
    if m := re.fullmatch(rf"{expr}=(?P<rhs>[^;]+);", body):
        target, rhs = m["target"], unparen(m["rhs"])
        for bitop, op in (("|", "set"), ("&", "and-store"), ("&", "clear")):
            operand = "~" + mask if op == "clear" else mask
            if rhs in (target + bitop + operand, operand + bitop + target):
                return target, op
        return None
    if m := re.fullmatch(r"if\((.+)\)(?:\{return(?:1|TRUE);\}|return(?:1|TRUE);)return(?:0|FALSE);", body):
        test, op = unparen(m[1]), "nonzero"
    elif m := re.fullmatch(r"return(.+);", body):
        test, op = unparen(m[1]), "and"
        if test.endswith("!=0"):
            test, op = unparen(test[:-3]), "nonzero"
    else:
        return None
    m = re.fullmatch(rf"(?:{expr}&{mask}|{mask}&(?P<reverse>{FIELD_EXPR}))", test)
    return (m["target"] or m["reverse"], op) if m else None


def parse_helper(code, match, getters):
    params = [PARAM.fullmatch(p.strip()) for p in match["params"].split(",")]
    if not all(p and p["type"] for p in params):
        return {}
    names = tuple(p["name"] for p in params)
    types = tuple(p["type"].strip() for p in params)
    raw = code[match.end():concessions._scope_end(code, match.end())].strip()
    alias = ()
    # Some helpers name a typed view of an opaque argument before the operation.
    if view := re.match(rf"((?:struct|union)\s+{ID})\s*\*\s*({ID})\s*=\s*"
                        rf"(?:\(\s*(?:void|(?:struct|union)\s+{ID})\s*\*\s*\)\s*)?({ID})\s*;", raw):
        if view[3] in names:
            alias = (view[2], view[3], view[1])
            raw = raw[view.end():]
    body = compact(raw)
    # Replace a same-file, one-return field getter. Narrowing getters stay
    # out of this grammar, since their conversions can change the flag test.
    for getter, member in getters.items():
        body = re.sub(rf"\b{re.escape(getter)}\(({ID})\)", lambda m: m[1] + member, body)
    result = re.sub(r"\b(?:static|inline|__inline__)\b", "", code[match.start():match.start("name")]).strip()
    supported = {}
    for index, mask in enumerate(names):
        found = operation(body, mask)
        if found:
            target, op = found
            supported[index] = Helper(names, types, target, mask, op, result, alias)
        # A complemented keep mask is the same one clear operation.
        keep = re.match(rf"\s*((?:{concessions._SCALAR_TYPE}))\s+({ID})\s*=\s*~\s*{mask}\s*;", raw)
        if keep and (found := operation(compact(raw[keep.end():]), keep[2])) and found[1] == "and-store":
            supported[index] = Helper(names, types, found[0], mask, "clear", result, alias, keep[1])
    return supported


def file_facts(text):
    code = keepcheck.blank_comments(text)
    definitions = list(concessions._INLINE_DEF.finditer(code))
    getters = {}
    for m in definitions:
        param = PARAM.fullmatch(m["params"].strip())
        result = code[m.start():m.start("name")]
        if param and "," not in param["type"] and "*" in param["type"] and re.search(r"\bu32\s*$", result):
            body = compact(code[m.end():concessions._scope_end(code, m.end())])
            getter = re.fullmatch(rf"return{param['name']}((?:->|\.){MEMBER});", body)
            if getter:
                getters[m["name"]] = getter[1]
    helpers = {m["name"]: parse_helper(code, m, getters) for m in definitions}
    bodies = {
        m["name"]: InlineBody(code[m.start():m.start("name")], tuple(m["params"].split(",")),
                              text[m.end() - 1:concessions._scope_end(code, m.end()) + 1])
        for m in definitions
    }
    for m in concessions._MACRO_DEF.finditer(code):
        macro = concessions._FUNCTION_DEFINE.fullmatch(m[0].replace("\\\n", " "))
        if not macro:
            continue
        helpers[macro[1]] = {}
        bodies.pop(macro[1], None)
        args = tuple(arg.strip() for arg in macro[2].split(","))
        # Only a statement-expression with a single scalar binding and a single operation.
        if (tmp := concessions._MASK_TEMP.search(macro[3])) and tmp[2] in args:
            prefix, tail = compact(macro[3][:tmp.start()]), compact(macro[3][tmp.end():])
            if prefix == "({" and tail.endswith("})"):
                body = tail[:-2]
                found = operation(body, tmp[1]) or operation("return" + body, tmp[1])
                if found:
                    target, op = found
                    index = args.index(tmp[2])
                    dtype = tmp[0][:tmp.start(1) - tmp.start()].strip()
                    types = tuple(dtype if i == index else "" for i in range(len(args)))
                    helpers[macro[1]][index] = Helper(args, types, target, tmp[2], op, "")
    constants = dict(DEFINE.findall(code))
    for enum in re.finditer(r"\benum(?:\s+\w+)?\s*\{([^{}]*)\}", code):
        previous = "-1"
        for item in enum[1].split(","):
            m = re.fullmatch(rf"\s*({ID})\s*(?:=\s*(.+?))?\s*", item, re.S)
            if m:
                constants[m[1]] = m[2] or f"({previous}) + 1"
                previous = m[1]
    return helpers, constants, concessions._INCLUDE.findall(text), bodies


@lru_cache(maxsize=None)
def header_facts(path):
    return file_facts(path.read_text(errors="replace"))


@lru_cache(maxsize=None)
def header_path(parent, name):
    for path in (parent / name, ROOT / "include" / name):
        if path.is_file():
            return path.resolve()
    return None


def unit_facts(path, text):
    helpers, constants, bodies = {}, {}, {}
    seen = {path.resolve()}
    local, values, names, local_bodies = file_facts(text)

    def visit(parent, includes):
        for name in includes:
            target = header_path(parent, name)
            if target is None or target in seen:
                continue
            seen.add(target)
            defs, values, nested, definitions = header_facts(target)
            visit(target.parent, nested)
            helpers.update(defs)
            constants.update(values)
            for name in defs.keys() - definitions.keys():
                bodies.pop(name, None)
            bodies.update(definitions)

    visit(path.parent, names)
    helpers.update(local)
    constants.update(values)
    for name in local.keys() - local_bodies.keys():
        bodies.pop(name, None)
    bodies.update(local_bodies)
    return helpers, constants, bodies


def substitute_target(helper, args, code, function, offset):
    target = helper.target
    root = re.search(ID, target)[0]
    rest = target[target.index(root) + len(root):]
    if helper.alias and root == helper.alias[0]:
        _, source, dtype = helper.alias
        arg = args[helper.params.index(source)].strip()
        return f"(({dtype} *)({arg})){rest}"
    if root not in helper.params:
        for index in re.findall(rf"\[({ID})\]", rest):
            if index in helper.params:
                target = target.replace(f"[{index}]", f"[{args[helper.params.index(index)].strip()}]")
        return target
    index = helper.params.index(root)
    arg = unparen(args[index].strip())
    owner = re.fullmatch(r"(?:const\s+)?((?:struct|union)\s+\w+)\s*\*\s*(?:const)?", helper.types[index])
    arg_name = re.match(ID, arg)
    actual = rawfield.declared_type(code, function, offset, arg_name[0]) if arg_name else None
    opaque = re.match(r"\(\s*void\s*\*\s*\)", arg) or (
        arg_name and actual is None and re.search(
            rf"\bvoid\s*\*\s*(?:const\s+)?{arg_name[0]}\b", code[function:offset]))
    if target.startswith("*"):
        if arg.startswith("&"):
            return unparen(arg[1:].strip())
        if opaque and helper.types[index]:
            arg = f"({helper.types[index]})({arg})"
        return f"*({arg})"
    if rest.startswith("->"):
        if arg.startswith("&"):
            item = unparen(arg[1:].strip())
            address_type = rawfield.declared_type(code, function, offset, item) if re.fullmatch(ID, item) else None
            if owner and (not address_type or address_type != (owner[1], False)):
                return f"(({helper.types[index]})({arg})){rest}"
            return f"({item}).{rest[2:]}"
        if owner and opaque:
            return f"(({helper.types[index]})({arg})){rest}"
        return arg + rest if re.fullmatch(MEMBER, compact(arg)) else f"({arg}){rest}"
    for subscript in re.findall(rf"\[({ID})\]", rest):
        if subscript in helper.params:
            rest = rest.replace(f"[{subscript}]", f"[{args[helper.params.index(subscript)].strip()}]")
    return f"({arg}){rest}" if rest or not re.fullmatch(MEMBER, compact(arg)) else arg


def plain_call(helper, args, argument, values, code, function, offset):
    """Return the replacement, field expression and the mask spelling, or a skip reason."""
    if len(args) != len(helper.params) or helper.params[argument] != helper.mask:
        return "helper has another mask or uses additional arguments"
    root = re.search(ID, helper.target)[0]
    needed = {helper.mask} | ({root} if root in helper.params else set())
    if helper.alias and root == helper.alias[0]:
        needed.add(helper.alias[1])
    needed.update(re.findall(rf"\[({ID})\]", helper.target))
    if set(helper.params) != needed:
        return "helper has other arguments"
    literal = args[argument].strip()
    value = constant(literal, values)
    if value is None:
        return "mask value could not be resolved"
    mask_type = helper.types[argument].removesuffix(" const")
    limits = folds.type_range(mask_type) if mask_type else None
    if mask_type and limits is None:
        return "unsupported mask parameter type"
    if limits and not limits[0] <= value <= limits[1]:
        if limits != (0, 0xFFFFFFFF) or not -(1 << 31) <= value < 0:
            return "plain mask would change parameter conversion"
    if not re.fullmatch(folds.LIT, literal):
        literal = f"({literal})"
    target = substitute_target(helper, args, code, function, offset)
    op = helper.operation
    if op in ("and", "nonzero"):
        expr = f"({target} & {literal})"
        if op == "nonzero":
            expr = f"({expr} != 0)"
        elif helper.result and helper.result not in ("int", "s32"):
            # A call's return type remains relevant when its value is consumed.
            expr = f"(({helper.result}){expr})"
    else:
        sign = "~" if op == "clear" else ""
        operand = sign + literal
        if helper.complement_type and folds.type_range(helper.complement_type) != (0, 0xFFFFFFFF):
            operand = f"({helper.complement_type})({operand})"
        expr = f"({target} {'|=' if op == 'set' else '&='} {operand})"
    return expr, target, args[argument].strip(), value


def normalize_field(expr):
    expr = compact(expr)
    while m := re.match(rf"^\(({MEMBER})\)(?=\.|->)", expr):
        expr = m[1] + expr[m.end():]
    return unparen(expr)


def field_key(expr, code, function, offset, helper=None):
    expr = normalize_field(expr)
    if pointer := re.fullmatch(rf"\*\(?({ID})\)?", expr):
        name = pointer[1]
        writes = list(re.finditer(rf"(?<![\w.>]){name}\s*=(?!=)\s*([^;]+);", code[function:offset]))
        if len(writes) == 1:
            assigned = writes[0]
            target = re.fullmatch(rf"&\s*({MEMBER})", compact(assigned[1]))
            if target and offset < concessions._scope_end(code, function + assigned.end()):
                expr = target[1]
    if cast := re.fullmatch(rf"\(\((?:const)?(struct|union)({ID})\*\)\(.+\)\)->({MEMBER})", expr):
        return f"{cast[1]} {cast[2]}", cast[3].replace("->", ".")
    m = re.fullmatch(rf"({ID})((?:(?:->|\.){ID})+)", expr)
    if m:
        root, chain = m.groups()
        declared = rawfield.declared_type(code, function, offset, root)
        if declared is None:
            declared = rawfield.global_type(code[:function], root)
        if declared is None and helper and "->" in helper.target:
            owner = helper.target.split("->", 1)[0]
            if owner in helper.params:
                dtype = helper.types[helper.params.index(owner)]
                if dtype.startswith(("struct ", "const struct ")):
                    declared = (dtype.removeprefix("const ").split("*")[0].strip(), True)
        return (declared[0] if declared else "(unresolved type)", chain.lstrip(".->").replace("->", "."))
    if re.fullmatch(r"g[A-Z]\w*", expr):
        return "(global)", expr
    return "", ""


def local_operation(code, use, name):
    before, after = code[:use], code[use + len(name):]
    lhs = re.search(rf"(?P<field>{FIELD_EXPR})\s*(?P<op>\|=|&=|(?<![=&])&(?!&))\s*(?P<neg>~?)\s*$", before)
    if lhs:
        op = {("|=", ""): "set", ("&=", ""): "and-store", ("&=", "~"): "clear",
              ("&", ""): "and"}.get((lhs["op"], lhs["neg"]))
        if op and op == "and" and re.match(r"\s*\)*\s*!=\s*0\b", after):
            op = "nonzero"
        if op == "and":
            stem = before[:lhs.start()]
            if re.search(rf"(?<!\w){re.escape(lhs['field'])}\s*=\s*$", stem):
                op = "and-store"
            elif re.search(r"\b(?:if|while)\s*\(+\s*$", stem) and re.match(r"\s*\)", after):
                op = "nonzero"
        return (lhs["field"], op) if op else ("", "")
    rhs = re.match(rf"\s*&\s*(?P<field>{FIELD_EXPR})(?![\w.>])", after)
    if rhs:
        op = "and-store" if re.search(rf"(?<!\w){re.escape(rhs['field'])}\s*=\s*~?\s*$", before) else "and"
        return rhs["field"], "clear" if op == "and-store" and before.rstrip().endswith("~") else op
    return "", ""


def helper_reads(code, helpers):
    """Map exact local argument offsets to the operation performed by a flag helper."""
    for call in concessions._CALLED.finditer(code):
        forms = helpers.get(call[1])
        if not forms:
            continue
        args, end = concessions._arguments(code, call.end())
        if args is None or concessions._BODY_START.match(code, end):
            continue
        for index, helper in forms.items():
            if len(args) != len(helper.params):
                continue
            actual = args[index]
            if re.fullmatch(rf"\s*{ID}\s*", actual):
                offset = call.end() + sum(len(a) + 1 for a in args[:index]) + len(actual) - len(actual.lstrip())
                yield offset, (helper, args)


@dataclass
class Probe:
    path: str
    offset: int
    line: int
    column: int
    kind: str
    weight: int
    argument: int
    directory: str
    function: str = ""
    helper: str = ""
    spelling: str = ""
    literal: str = ""
    value: int | None = None
    owner: str = ""
    field: str = ""
    operation: str = ""
    field_keys: tuple = ()
    other_uses: str = ""
    rom_matches: tuple = ()
    edits: tuple = dataclass_field(default=(), repr=False)
    control_edits: tuple = dataclass_field(default=(), repr=False)
    reason: str = ""
    sample: bool = False

    @property
    def id(self):
        return f"{self.path}:{self.offset}:{self.kind}:{self.argument}"


def other_uses(code, span, candidate, excluded, local_reads, constants):
    if candidate.value is None:
        return ""
    start, end, _ = span
    wanted = candidate.value & 0xFFFFFFFF
    found = []
    for pos in local_reads[1:]:
        line = concessions._line(code, pos)
        found.append(f"{line}:{pos - code.rfind(chr(10), 0, pos)} {candidate.spelling} (same binding)")
    for token in TOKEN.finditer(code, start, end):
        if any(a <= token.start() < b for a, b in excluded):
            continue
        word = token[0]
        if word[0].isdigit():
            prefix = code[start:token.start()].rstrip()
            if prefix.endswith("~") or (prefix.endswith("-") and
                                        re.search(r"(?:^|[(=,;&|^?:{]|\breturn)\s*-$", prefix)):
                word = prefix[-1] + word
            try:
                value = folds.value(word)
            except ValueError:
                continue
        elif word in constants and not re.search(r"(?:->|\.)\s*$", code[max(start, token.start() - 12):token.start()]):
            value = constant(word, constants)
        else:
            continue
        if value is not None and value & 0xFFFFFFFF == wanted:
            line = concessions._line(code, token.start())
            found.append(f"{line}:{token.start() - code.rfind(chr(10), 0, token.start())} {word}")
    for match in CONST_EXPR.finditer(code, start, end):
        if any(a < match.end() and match.start() < b for a, b in excluded):
            continue
        value = constant(match[0], constants)
        if value is not None and value & 0xFFFFFFFF == wanted:
            pos = match.start()
            found.append(f"{concessions._line(code, pos)}:{pos - code.rfind(chr(10), 0, pos)} {match[0].strip()}")
    return "; ".join(found)


def specialize_helper(p, definition, args, end, call, code, span, values):
    """Make one copy of the helper and compare its staged and literal forms."""
    if definition is None or len(args) != len(definition.params) or p.helper == span[2]:
        return
    declaration = PARAM.fullmatch(definition.params[p.argument].strip())
    if declaration is None or not concessions._SCALAR_PARAM.fullmatch(definition.params[p.argument]):
        return
    mask = declaration["name"]
    body = definition.body
    clean = keepcheck.blank_comments(body)
    uses = folds.reads(clean, mask, 0, len(clean))
    if not uses or re.search(r"(?m)^\s*#|\b" + re.escape(p.helper) + r"\s*\(", clean):
        return
    if any(name == mask for group in concessions._local_declarations(clean) for name, *_ in group):
        return
    value = constant(p.literal, values)
    if value is None:
        return
    name = f"PlainProbe_{p.helper}_{p.offset}"
    if re.search(rf"\b{name}\b", code):
        return

    def edits(params, body_text, arguments):
        signature = ", ".join(item.strip() for item in params) or "void"
        clone = f"{definition.prefix}{name}({signature})\n{body_text}\n"
        return ((span[0], span[0], clone), (p.offset, end, f"{name}({', '.join(arguments)})"))

    dtype = declaration["type"].strip().removeprefix("const ").removesuffix(" const")
    literal = f"(({dtype})({p.literal}))"
    rewritten = folds.edit_text(body, [(use, use + len(mask), literal) for use in uses])
    i = p.argument
    p.control_edits = edits(definition.params, body, args)
    p.edits = edits(definition.params[:i] + definition.params[i + 1:], rewritten, args[:i] + args[i + 1:])
    p.value = value
    start = call.end() + sum(len(a) + 1 for a in args[:i])
    shared = other_uses(code, span, p, [(start, start + len(args[i]))], (), values)
    p.other_uses = "; ".join(filter(None, [shared, *(f"{p.helper}: mask read {j}"
                                                    for j in range(2, len(uses) + 1))]))


def plan(path, text, header_cache):
    rel = os.path.relpath(path, ROOT)
    parts = Path(rel).parts
    directory = "/".join(parts[:2]) if len(parts) > 2 and parts[0] == "src" else (
        parts[0] if parts[0] in ("src", "include") else "(scratch)")
    locals_ = concessions.sites(text, rel, kinds=LOCAL_KINDS, split_locals=True)
    calls = concessions.literal_argument_sites(rel, text, ROOT, headers=header_cache)
    sites = [s for s in (*locals_, *calls) if not s.excused]
    if not sites:
        return []
    code = keepcheck.blank_comments(text)
    spans = keepcheck.function_spans(text)
    starts = [span[0] for span in spans]
    drafts = keepcheck.draft_ranges(text)
    macro_ranges = [m.span() for m in concessions._MACRO_DEF.finditer(code)]
    disabled = [(m.start(), m.end()) for m in re.finditer(
        r"(?ms)^\s*#\s*ifdef\s+UBFIX\b.*?(?=^\s*#\s*(?:else|endif)\b)", code)]
    local_folds = folds.folds(text, rel, every=True, sites=locals_) if locals_ else {}
    remaining = {s.offset for s in locals_ if not s.excused} - local_folds.keys()
    if remaining:
        local_folds.update(folds.grouped_folds(text, rel, remaining))
    helpers, values, bodies = unit_facts(path, text)
    helper_args = dict(helper_reads(code, helpers)) if local_folds else {}
    out = []
    for site in sites:
        pos = site.offset
        p = Probe(rel, pos, site.line, pos - code.rfind("\n", 0, pos),
                  site.kind, site.weight, site.argument, directory)
        out.append(p)
        if site.kind == "literal-param":
            p.helper = site.note.split()[0]
        else:
            token = re.match(ID, code[pos:])
            p.spelling = token[0] if token else ""
        index = bisect.bisect_right(starts, pos) - 1
        span = spans[index] if index >= 0 and pos < spans[index][1] else None
        if span:
            p.function = span[2]
        if span is None or keepcheck.in_ranges(pos, macro_ranges):
            p.reason = "outside an ordinary C function body"
            continue
        if keepcheck.in_ranges(pos, drafts + disabled):
            p.reason = "inactive in the matching build"
            continue
        if site.kind in LOCAL_KINDS:
            fold = local_folds.get(pos)
            if not fold:
                p.reason = "no safe fold_literals --every-read proposal"
                continue
            p.spelling, p.literal, p.edits = fold.name, fold.literal, fold.edits
            p.value = folds.value(fold.literal)
            keys = set()
            for use in fold.uses:
                helper = None
                if use in helper_args:
                    helper, args = helper_args[use]
                    expr = substitute_target(helper, args, code, span[0], use)
                    op = helper.operation
                else:
                    expr, op = local_operation(code, use, fold.name)
                owner, member = field_key(expr, code, span[0], use, helper)
                if owner:
                    keys.add((owner, member, op))
            p.field_keys = tuple(sorted(keys))
            if len(keys) == 1:
                p.owner, p.field, p.operation = keys.pop()
            p.other_uses = other_uses(code, span, p, [(a, b) for a, b, _ in p.edits], fold.uses, values)
        else:
            helper = helpers.get(p.helper, {}).get(site.argument)
            call = concessions._CALLED.match(code, pos)
            args, end = concessions._arguments(code, call.end())
            p.literal = args[site.argument].strip()
            if helper is None:
                specialize_helper(p, bodies.get(p.helper), args, end, call, code, span, values)
                if not p.edits:
                    p.reason = "helper has no safe inline specialization"
                continue
            form = plain_call(helper, args, site.argument, values, code, span[0], pos)
            if isinstance(form, str):
                p.reason = form
                continue
            expr, target, p.literal, p.value = form
            p.owner, p.field = field_key(target, code, span[0], pos, helper)
            p.operation = helper.operation
            if p.owner:
                p.field_keys = ((p.owner, p.field, p.operation),)
            p.edits = ((pos, end, expr),)
            # Exclude only the mask argument, not identical values in the field address.
            arg_start = call.end() + sum(len(a) + 1 for a in args[:site.argument])
            p.other_uses = other_uses(code, span, p, [(arg_start, arg_start + len(args[site.argument]))], (), values)
    return out


def relocate_includes(text, path):
    def replace(m):
        neighbor = path.parent / m[1]
        if neighbor.is_file():
            return m[0].replace(f'"{m[1]}"', f'"{neighbor.resolve()}"')
        return m[0]
    return concessions._INCLUDE.sub(replace, text)


@lru_cache(maxsize=1)
def header_graph():
    import header_dependents

    parents = defaultdict(set)
    for parent, children in header_dependents.tree().items():
        for child in children:
            parents[ROOT / child].add(ROOT / parent)
    return parents


@lru_cache(maxsize=None)
def header_ancestors(target):
    parents = header_graph()
    affected = {target}
    pending = [target]
    while pending:
        for parent in parents.get(pending.pop(), set()) - affected:
            affected.add(parent)
            pending.append(parent)
    return frozenset(affected)


def override_header(text, path, scratch, target, replacement, affected):
    """Copy the include paths from this unit to the edited header into scratch."""
    copied = {}

    def rewrite(source, origin):
        def replace(m):
            spelling = m[1]
            candidates = (origin.parent / spelling, ROOT / spelling, ROOT / "include" / spelling)
            child = next((p.resolve() for p in candidates if p.is_file()), None)
            if child is None:
                return m[0]
            if child in affected:
                if child not in copied:
                    copy = scratch / f"header-{len(copied)}-{child.name}"
                    copied[child] = copy
                    content = replacement if child == target else child.read_text(errors="replace")
                    copy.write_text(rewrite(content, child))
                child = copied[child]
            return m[0].replace(f'"{spelling}"', f'"{child}"')

        return concessions._INCLUDE.sub(replace, source)

    return rewrite(text, path)


class Checker:
    def __init__(self, out):
        self.work = out / "work"
        self.work.mkdir(parents=True, exist_ok=True)
        self.rom = check.load_rom(verify=False)
        self.syms = check.elf_symbols()
        self.ram = build.ram_symbols()
        build.headers_hash()

    def compile(self, path, text, header=None):
        with tempfile.TemporaryDirectory(prefix="unit-", dir=self.work) as scratch:
            work = os.path.relpath(scratch, ROOT)
            source = Path(scratch) / path.name
            content = relocate_includes(text, path) if header is None else override_header(
                text, path, Path(scratch), header[0], header[1], header_ancestors(header[0]))
            source.write_text(content)
            try:
                _, address, names, binary = check.linked_bytes(
                    os.path.relpath(source, ROOT), work, syms=self.syms, ram=self.ram)
            except SystemExit as error:
                return str(error)
            return address, names, binary

    @lru_cache(maxsize=None)
    def baseline(self, path, original):
        result = self.compile(path, original)
        if isinstance(result, str):
            return "baseline compile/link failed: " + result
        address, names, binary = result
        offset = check.off(address)
        if offset < 0 or offset + len(binary) > len(self.rom) or self.rom[offset:offset + len(binary)] != binary:
            return "unchanged scratch unit differs from ROM"
        return address, names, binary

    def compare(self, path, original, variant, header=None):
        expected = self.baseline(path, original)
        if isinstance(expected, str):
            return "SKIP", expected
        result = self.compile(path, variant, header) if header else self.compile(path, variant)
        if isinstance(result, str):
            return "SKIP", "variant compile/link failed: " + result
        address, names, binary = result
        if (address, names) != expected[:2]:
            return "SKIP", "variant changed the unit's linked function inventory"
        if binary == expected[2]:
            return "PLAIN", f"MATCH ({len(binary)} bytes at 0x{address:08X})"
        count = sum(a != b for a, b in zip(binary, expected[2])) + abs(len(binary) - len(expected[2]))
        return "DIFF", f"{count} byte positions differ ({len(expected[2])} → {len(binary)} bytes at 0x{address:08X})"


def probe_header(p, text, checker):
    path = ROOT / p.path
    deps = sorted(unit for unit in header_ancestors(path) if unit.suffix == ".c")
    if not deps:
        return "SKIP", "header has no source dependents in the include graph"
    call = re.compile(rf"\b{re.escape(p.function)}\s*\(")
    originals = {unit: unit.read_text(errors="replace") for unit in deps}
    deps.sort(key=lambda unit: (not call.search(originals[unit]), len(originals[unit]), unit))
    variant = folds.edit_text(text, p.edits)
    control = folds.edit_text(text, p.control_edits) if p.control_edits else None
    failures = []
    for unit in deps:
        if STOP.exists():
            return None
        original = originals[unit]
        if control is not None:
            verdict, detail = checker.compare(unit, original, original, (path, control))
            if verdict != "PLAIN":
                failures.append(f"specialization control {verdict}: {detail}")
                continue
        verdict, detail = checker.compare(unit, original, original, (path, variant))
        if verdict == "DIFF":
            return ("BOUND-SHARED" if p.other_uses else "BOUND-SINGLE",
                    f"{unit.relative_to(ROOT)}: {detail}")
        if verdict == "SKIP":
            failures.append(detail)
    if failures:
        return "SKIP", f"header dependent checks incomplete ({len(failures)}/{len(deps)}): {failures[0]}"
    return "PLAIN", f"MATCH ({len(deps)} header dependent units)"


def probe(p, sources, checker):
    if p.reason:
        return "SKIP", p.reason
    text = sources[p.path]
    if Path(p.path).suffix == ".h":
        return probe_header(p, text, checker)
    if p.control_edits:
        control, detail = checker.compare(ROOT / p.path, text, folds.edit_text(text, p.control_edits))
        if control != "PLAIN":
            return "SKIP", f"helper specialization control {control}: {detail}"
    verdict, detail = checker.compare(ROOT / p.path, text, folds.edit_text(text, p.edits))
    if verdict == "DIFF":
        verdict = "BOUND-SHARED" if p.other_uses else "BOUND-SINGLE"
    return verdict, detail


def sample_order(sites):
    rng = random.Random(20261127)
    groups = defaultdict(list)
    for p in sites:
        groups[p.directory].append(p)
    for group in groups.values():
        for p in rng.sample(group, min(40, len(group))):
            p.sample = True
    return sorted(sites, key=lambda p: not p.sample)


COLUMNS = ("id", "path", "directory", "line", "column", "offset", "kind", "argument", "weight",
           "sample", "function", "helper", "spelling", "literal", "value", "owner", "field", "operation",
           "other_uses", "field_keys", "rom_matches", "probe_version", "class", "detail")


def atomic(path, write):
    temporary = path.with_name(path.name + ".tmp")
    with temporary.open("w", newline="") as stream:
        write(stream)
    temporary.replace(path)


def write_csv(path, sites, results):
    def write(stream):
        writer = csv.writer(stream)
        writer.writerow(COLUMNS)
        for p in sites:
            verdict, detail = results.get(p.id, ("PENDING", ""))
            writer.writerow([getattr(p, key) for key in COLUMNS[:-5]] +
                            [json.dumps(p.field_keys), json.dumps(p.rom_matches), probe_version(p), verdict, detail])
    atomic(path, write)


def counts(sites, results):
    return Counter(results.get(p.id, ("PENDING", ""))[0] for p in sites)


def table(sites, results, group, label, additional=None, *, multiple=False):
    groups = defaultdict(list)
    for p in sites:
        keys = group(p) if multiple else [group(p)]
        for key in keys:
            if key:
                groups[key].append(p)
    columns = [label, *CLASSES, "Pending"]
    if additional:
        columns.extend(("Helper and local spellings", "ROM mask / load / interleaved"))
    yield "| " + " | ".join(columns) + " |"
    yield "| " + " | ".join(["---"] * len(columns)) + " |"
    for key, members in sorted(groups.items()):
        c = counts(members, results)
        name = " / ".join(key) if isinstance(key, tuple) else key
        cells = [name, *(str(c[k]) for k in (*CLASSES, "PENDING"))]
        if additional:
            cells.extend(additional(key, members))
        yield "| " + " | ".join(cell.replace("|", "\\|") for cell in cells) + " |"


def summary(sites, results, rom_orders, aliases):
    sample = [p for p in sites if p.sample]
    for name, members in (("Sample", sample), ("Overall", sites)):
        yield f"## {name}"
        yield ""
        yield from table(members, results, lambda p: "All sites", "Scope")
        c = counts(members, results)
        tested = sum(c[k] for k in CLASSES[:-1])
        yield ""
        if tested:
            yield f"{c['PLAIN']:,} of {tested:,} compiler-tested sites matched ({c['PLAIN'] / tested:.1%})."
            yield ""
    yield "## Directory sample"
    yield ""
    yield from table(sample, results, lambda p: p.directory, "Directory")
    yield ""
    for title, key in (("Warning kind", lambda p: p.kind), ("Directory", lambda p: p.directory),
                       ("Helper", lambda p: p.helper)):
        yield f"## {title}"
        yield ""
        yield from table(sites, results, key, title)
        yield ""
    yield "## Typed field and operation"
    yield ""
    yield "The ROM share is mask-first among classified mask-first and load-first operations. Interleaved observations are shown separately. A dash means the ROM scan has no classified observations for that row."
    yield ""

    def extra(key, members):
        names = sorted({p.helper or p.spelling for p in members})
        owner, member, op = key
        rom_field = member if owner == "(global)" else f"{owner.removeprefix('struct ')}.{member}"
        rom_op = {"and": "test", "nonzero": "test", "clear": "and-update", "and-store": "and-update"}.get(op, op)
        order = rom_orders.get((rom_field, rom_op))
        evidence = "—"
        if order:
            mask, load, interleaved = (order[k] for k in ("mask", "word", "interleaved"))
            share = f" ({mask / (mask + load):.1%})" if mask + load else ""
            evidence = f"{mask} / {load} / {interleaved}{share}"
        return ", ".join(f"`{name}`" for name in names), evidence

    def groups(p):
        return {(*aliases.get((owner, member), (owner, member)), op) for owner, member, op in p.field_keys}

    yield from table(sites, results, groups, "Type / field / operation", extra, multiple=True)


def read_rom(path):
    rom_orders = defaultdict(Counter)
    matches = defaultdict(set)
    aliases = {}
    alias_path = path.with_suffix(".aliases.json")
    if alias_path.is_file():
        aliases = {(owner, member): (canonical, field)
                   for owner, member, canonical, field in json.loads(alias_path.read_text())}
    if path.is_file():
        with path.open() as stream:
            for row in csv.DictReader(stream, delimiter="\t"):
                op = "and-update" if row["op"] in ("clear", "and-store") else row["op"]
                rom_orders[row["field"], op][row["order"]] += 1
                key = (row["function"], row["field"], row["op"], int(row["mask"], 16))
                matches[key].add((row["site"], row["order"]))
    return rom_orders, aliases, matches


def link_rom(sites, aliases, matches):
    for p in sites:
        if p.value is None:
            continue
        found = set()
        mask = p.value & 0xFFFFFFFF
        inverted = mask ^ 0xFFFFFFFF
        for owner, member, op in p.field_keys:
            owner, member = aliases.get((owner, member), (owner, member))
            field = member if owner == "(global)" else f"{owner.removeprefix('struct ')}.{member}"
            keys = {
                "set": (("set", mask),),
                "clear": (("clear", mask), ("and-store", inverted)),
                "and-store": (("and-store", mask), ("clear", inverted)),
                "and": (("test", mask),), "nonzero": (("test", mask),),
            }
            for key, value in keys.get(op, ()):
                found.update(matches.get((p.function, field, key, value), ()))
        p.rom_matches = tuple(sorted(found))


def write_reports(sites, results, args, rom_orders, aliases):
    corroborated = Counter(p.rom_matches[0][1] for p in sites if len(p.rom_matches) == 1
                           and results.get(p.id, ("", ""))[0] == "BOUND-SINGLE")
    rom_summary = (
        f"The ROM census found {sum(sum(order.values()) for order in rom_orders.values()):,} classifiable operations. "
        f"Among BOUND-SINGLE sites with one ROM observation on the same function, field, operation and value, "
        f"{corroborated['mask']:,} are mask-first, {corroborated['word']:,} load-first, "
        f"and {corroborated['interleaved']:,} interleaved. The CSV includes their ROM addresses "
        "and retains multiple observations when the correspondence is ambiguous."
    )
    note = [
        "# Plain flag and local-constant probes", "", "Tags: #compiler #phase3 #tooling", "",
        "Each row represents one linter binding or constant argument. A local probe uses the "
        "`fold_literals.py --every-read` edit for that binding. The CSV keeps the linter weight, "
        "including multiple uses charged to the same local. The sample uses a fixed seed and "
        "takes up to 40 sites per directory before the rest of the sweep.", "",
        "PLAIN is a byte-identical scratch rewrite. BOUND-SINGLE is a mismatch with one use "
        "found in the containing source function. BOUND-SHARED is a mismatch with other uses "
        "listed in the CSV. SKIP records an unsupported form, inactive code or check failure. "
        "A mismatch identifies a dependency in the current reconstruction. The other-use scan "
        "looks for reads of the same binding and equal literals or named constants in the "
        "containing function. It cannot see every value produced by inlining or optimization. "
        "Confirm mask order and alternative matching C before inferring an original accessor or macro.", "",
        "The ROM-side scan follows recognized register provenance and omits cases it cannot "
        "attribute. Its branch-consumed tests are reported alongside both raw AND and boolean "
        "source forms. AND and BIC stores share a ROM denominator for clear and keep-mask forms. "
        "The compiler probes also cover non-flag locals. A local used on several fields appears "
        "once in each applicable field row.", "",
        rom_summary, "",
        "To reproduce the sweep, run `build/venv/bin/python tools/plainprobe.py`. Use "
        "`--resume` to continue from the CSV. Run "
        "`build/venv/bin/python tools/plainprobe_rom.py` to refresh the typed ROM census, "
        "then run plainprobe with `--resume` to import it.", "",
        *summary(sites, results, rom_orders, aliases), "",
    ]
    weights = Counter()
    skips = Counter()
    for p in sites:
        verdict, detail = results.get(p.id, ("PENDING", ""))
        weights[verdict] += p.weight
        if verdict == "SKIP":
            skips[detail.split(":", 1)[0]] += 1
    note += ["## Linter-use weights", "", "| Verdict | Weighted uses |", "| --- | ---: |"]
    note += [f"| {key} | {weights[key]:,} |" for key in (*CLASSES, "PENDING")]
    note += ["", "## Skip reasons", "", "| Reason | Sites |", "| --- | ---: |"]
    note += [f"| {reason} | {count:,} |" for reason, count in sorted(skips.items())]
    if args.out == OUT:
        note += [
            "", "## Scratch application control", "",
            "The control rewrites one argument at 08081C98 and leaves the full 1,260-byte "
            "unit on MATCH. The command checks the independent form, then keeps each "
            "cumulative edit only if it matches and adds no linter or compiler warning.", "",
            "```sh",
            "mkdir -p build/plainprobe/apply-control",
            "cp src/scene/actor/reaction/SceneActorReaction_UpdateTimedBurst.c build/plainprobe/apply-control/",
            "build/venv/bin/python tools/plainprobe.py --apply-plain \\",
            "  build/plainprobe/apply-control/SceneActorReaction_UpdateTimedBurst.c --out build/plainprobe/apply-control",
            "build/venv/bin/python tools/check.py build/plainprobe/apply-control/SceneActorReaction_UpdateTimedBurst.c --match-only",
            "```", "",
        ]
    if args.note.is_file():
        _, marker, review = args.note.read_text().partition(REVIEW_MARK)
        if marker:
            note += ["", marker + review.rstrip(), ""]
    atomic(args.note, lambda f: f.write("\n".join(note)))
    sc = counts([p for p in sites if p.sample], results)
    total = counts(sites, results)
    weighted = sum(p.weight for p in sites)
    done = len(results)
    report = [
        "# Plain constant probe", "",
        f"Recorded {done:,} of {len(sites):,} bindings or arguments ({weighted:,} linter uses). "
        "The game source retains the baseline steering-debt count. This measurement makes "
        "zero percentage-point change to exact code progress.", "",
    ]
    for name, c in (("Directory sample", sc), ("Full inventory", total)):
        report += [f"## {name}", "", ", ".join(f"{c[key]:,} {key}" for key in (*CLASSES, "PENDING")) + ".", ""]
    report += [rom_summary, ""]
    report += [
        "## Resume here", "",
        "From the worktree root run `build/venv/bin/python tools/plainprobe.py --resume`. "
        "The CSV is the checkpoint. The note contains the directory, helper and field breakdowns. "
        "Use `build/venv/bin/python tools/plainprobe_rom.py` to rebuild the ROM census.", "",
        "Open question for each mismatching flag form is whether the ROM's field and mask loads "
        "show the order expected from source binding. A byte difference alone can also reflect "
        "allocation, conversions or other code structure.", "",
    ]
    if args.report:
        atomic(args.report, lambda f: f.write("\n".join(report)))


def inventory(paths):
    if paths:
        selected = []
        for name in paths:
            path = (ROOT / name).resolve()
            if path.is_dir():
                selected.extend(path.rglob("*.c"))
                selected.extend(path.rglob("*.h"))
            elif path.is_file():
                selected.append(path)
            else:
                raise ValueError(f"path does not exist: {name}")
    else:
        selected = [*ROOT.glob("src/**/*.c"), *ROOT.glob("src/**/*.h"), *ROOT.glob("include/**/*.h")]
    return [p for p in sorted(set(selected)) if not concessions.is_library(os.path.relpath(p, ROOT))]


def snapshot(sources):
    digest = hashlib.sha256()
    inputs = [ROOT / "tools/check.py", ROOT / "tools/build.py",
              ROOT / "symbols/functions.csv", ROOT / "symbols/ram.csv",
              ROOT / "build/boktai3.elf", ROOT / "baserom.gba"]
    for path in inputs:
        with path.open("rb") as stream:
            digest.update(hashlib.file_digest(stream, "sha256").digest())
    digest.update(build.headers_hash().encode())
    for name, text in sources.items():
        digest.update(name.encode() + b"\0" + hashlib.sha256(text.encode()).digest())
    return digest.hexdigest()


def probe_version(p):
    """Invalidate just the rows whose rewrite or classification inputs have changed."""
    evidence = (p.edits, p.other_uses, p.reason)
    if p.control_edits:
        evidence += (p.control_edits,)
    return hashlib.sha256(repr(evidence).encode()).hexdigest()[:24]


def plan_sites(sources):
    headers = {}
    return sample_order([site for name, text in sources.items() for site in plan(ROOT / name, text, headers)])


def cached_sites(path, digest, sources):
    """Reuse the full-tree plan while its source snapshot and planner match."""
    version = hashlib.sha256(digest.encode())
    for module in (__file__, concessions.__file__, folds.__file__, keepcheck.__file__, rawfield.__file__):
        version.update(hashlib.sha256(Path(module).read_bytes()).digest())
    key = version.hexdigest()
    if path.is_file():
        try:
            saved = json.loads(path.read_text())
            if saved["version"] == key:
                sites = []
                for row in saved["sites"]:
                    row["edits"] = tuple(map(tuple, row["edits"]))
                    row["control_edits"] = tuple(map(tuple, row.get("control_edits", ())))
                    row["field_keys"] = tuple(map(tuple, row["field_keys"]))
                    row["rom_matches"] = ()
                    sites.append(Probe(**row))
                return sites
        except (ValueError, KeyError, TypeError):
            pass  # A damaged cache can be rebuilt from the source snapshot.
    sites = plan_sites(sources)
    atomic(path, lambda f: json.dump({"version": key, "sites": [asdict(p) for p in sites]}, f))
    return sites


def lint_counts(path, source):
    sites = concessions.sites(source, path) + concessions.literal_argument_sites(path, source, ROOT)
    counts = Counter()
    for site in sites:
        if not site.excused:
            counts[site.kind] += site.weight
    return counts


def apply_plain(paths, out):
    """Retest sites independently, then gate each cumulative edit before keeping it."""
    checker = Checker(out)
    applied = 0
    for path in paths:
        if STOP.exists():
            break
        rel = os.path.relpath(path, ROOT)
        unit = keepcheck.Unit(rel)
        before = unit.text
        accepted = []
        for p in plan(path, before, {}):
            if STOP.exists():
                break
            if p.control_edits:
                continue  # This is an evidence-only comparison of helper bodies.
            verdict, _ = probe(p, {rel: before}, checker)
            if verdict != "PLAIN":
                continue
            if any(a < end and start < b for start, end, _ in p.edits for a, b, _ in accepted):
                print(f"{rel}:{p.line}:{p.column}: overlapping accepted edit")
                continue
            translated = []
            for start, end, replacement in p.edits:
                delta = sum(len(new) - (b - a) for a, b, new in accepted if b <= start)
                translated.append((start + delta, end + delta, replacement))
            variant = folds.edit_text(unit.text, translated)
            if lint_counts(rel, variant) - lint_counts(rel, unit.text):
                print(f"{rel}:{p.line}:{p.column}: would add a linter finding")
                continue
            verdict, detail = checker.compare(path, unit.text, variant)
            if verdict == "PLAIN" and unit.attempt(variant):
                accepted.extend(p.edits)
                applied += 1
                print(f"{rel}:{p.line}:{p.column}: {detail}")
            else:
                print(f"{rel}:{p.line}:{p.column}: cumulative edit held ({unit.last or detail})")
    print(f"Applied {applied:,} PLAIN sites")
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("paths", nargs="*")
    parser.add_argument("--out", type=Path)
    parser.add_argument("--note", type=Path)
    parser.add_argument("--report", type=Path)
    parser.add_argument("--rom-tsv", type=Path)
    parser.add_argument("--resume", action="store_true")
    parser.add_argument("--sample-only", action="store_true")
    parser.add_argument("--apply-plain", action="store_true")
    parser.add_argument("-j", "--jobs", type=int, default=4)
    args = parser.parse_args(argv)
    if args.jobs < 1:
        parser.error("jobs must be positive")
    if args.apply_plain and (not args.paths or args.resume or args.sample_only):
        parser.error("--apply-plain requires explicit paths and cannot resume a sweep")
    try:
        paths = inventory(args.paths)
    except ValueError as error:
        parser.error(str(error))
    args.out = (args.out or (OUT / "focus" if args.paths else OUT)).resolve()
    args.out.mkdir(parents=True, exist_ok=True)
    args.note = args.note or (NOTE if args.out == OUT else args.out / "SUMMARY.md")
    args.rom_tsv = args.rom_tsv or args.out / "flag_sites.tsv"
    if args.apply_plain:
        return apply_plain([p for p in paths if p.suffix == ".c"], args.out)
    with (args.out / "sweep.lock").open("a") as lock:
        try:
            fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except BlockingIOError:
            parser.error("another sweep is using this --out")
        return run_sweep(args, paths, parser)


def run_sweep(args, paths, parser):
    if args.report is None and args.out == OUT:
        args.report = REPORT
    csv_path = args.out / "sites.csv"
    state = args.out / "snapshot.json"
    if csv_path.exists() and not args.resume:
        parser.error("sites.csv exists; use --resume or choose a new --out")
    sources = {os.path.relpath(path, ROOT): path.read_text(errors="replace") for path in paths}
    digest = snapshot(sources)
    previous = {}
    results = {}
    if args.resume and state.exists():
        if json.loads(state.read_text())["digest"] != digest:
            parser.error("source, tool or ROM inputs changed; choose a new --out")
        if csv_path.exists():
            with csv_path.open() as stream:
                previous = {r["id"]: r for r in csv.DictReader(stream) if r["class"] in CLASSES}
    elif csv_path.exists():
        parser.error("sites.csv has no snapshot; choose a new --out")
    # The full inventory covers private src headers too. Focused paths can
    # include files outside the snapshot, so plan them from source each time.
    sites = plan_sites(sources) if args.paths else cached_sites(args.out / "plan.json", digest, sources)
    rom_orders, aliases, matches = read_rom(args.rom_tsv)
    link_rom(sites, aliases, matches)
    if not set(previous) <= {p.id for p in sites}:
        parser.error("checkpoint contains unknown sites")
    for p in sites:
        row = previous.get(p.id)
        if row and row.get("probe_version") == probe_version(p):
            results[p.id] = row["class"], row["detail"]
    atomic(state, lambda f: json.dump({"digest": digest}, f))
    print(f"Inventory: {len(sites):,} sites, {sum(p.weight for p in sites):,} linter uses, "
          f"{sum(p.sample for p in sites):,} sampled, {len(results):,} recorded", flush=True)
    write_csv(csv_path, sites, results)
    if len(results) == len(sites) or STOP.exists():
        write_reports(sites, results, args, rom_orders, aliases)
        if STOP.exists():
            print("STOP detected; checkpoint written", flush=True)
        return 0
    checker = Checker(args.out)
    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        for phase in (True, False):
            if not phase and args.sample_only:
                break
            pending = [p for p in sites if p.sample == phase and p.id not in results]
            batch_size = 4 if any(p.path.endswith(".h") for p in pending) else 32
            for start in range(0, len(pending), batch_size):
                if STOP.exists():
                    break
                batch = pending[start:start + batch_size]
                verdicts = pool.map(lambda p: probe(p, sources, checker), batch)
                results.update((p.id, verdict) for p, verdict in zip(batch, verdicts) if verdict is not None)
                write_csv(csv_path, sites, results)
                if start % 256 == 224:
                    write_reports(sites, results, args, rom_orders, aliases)
                print(f"{len(results):,}/{len(sites):,}: {dict(counts(sites, results))}", flush=True)
            write_reports(sites, results, args, rom_orders, aliases)
            if STOP.exists():
                print("STOP detected; checkpoint written", flush=True)
                break
    return 0


if __name__ == "__main__":
    sys.exit(main())
