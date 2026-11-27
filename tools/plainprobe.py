#!/usr/bin/env python3
"""Measure plain literal forms one site at a time, using stock check.py linking.

  build/venv/bin/python tools/plainprobe.py [PATH ...] [--sample-only] [--resume]
  build/venv/bin/python tools/plainprobe.py --apply-plain PATH ...

The default sweep visits each linter binding or argument once. A fixed random sample
of up to 40 sites per src directory runs first. Weighted linter counts are retained
in the CSV. Headers without a single owning unit receive explicit skips.
"""
from __future__ import annotations

import argparse
import ast
import bisect
import csv
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
from dataclasses import dataclass, field as dataclass_field
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
ID = r"[A-Za-z_]\w*"
MEMBER = rf"{ID}(?:(?:->|\.){ID})*"
FIELD_EXPR = rf"(?:\*{ID}|{MEMBER})"
NUM = r"(?:0[xX][0-9a-fA-F]+|[0-9]+)[uUlL]*"
TOKEN = re.compile(rf"{ID}|{NUM}")
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


def operation(body, mask):
    """Recognize one mask expression, including its simple assignment/boolean spellings."""
    expr = rf"(?P<target>{FIELD_EXPR})"
    mask = re.escape(mask)
    if m := re.fullmatch(rf"{expr}(?P<op>\|=|&=)(?P<neg>~?){mask};", body):
        op = {("|=", ""): "set", ("&=", ""): "and-store", ("&=", "~"): "clear"}.get((m["op"], m["neg"]))
        return (m["target"], op) if op else None
    if m := re.fullmatch(rf"{expr}=(?P<rhs>[^;]+);", body):
        target, rhs = m["target"], unparen(m["rhs"])
        for join, op in (("|", "set"), ("&", "and-store"), ("&~", "clear")):
            operand = "~" + mask if op == "clear" else mask
            bitop = "&" if op == "clear" else join
            if rhs in (target + bitop + operand, operand + bitop + target):
                return target, op
        return None
    if m := re.fullmatch(r"if\((.+)\)(?:\{return1;\}|return1;)return0;", body):
        test, op = unparen(m[1]), "nonzero"
    elif m := re.fullmatch(r"return(.+);", body):
        test, op = unparen(m[1]), "and"
        if test.endswith("!=0"):
            test, op = unparen(test[:-3]), "nonzero"
    else:
        return None
    m = re.fullmatch(rf"(?:{expr}&{mask}|{mask}&(?P<reverse>{FIELD_EXPR}))", test)
    return (m["target"] or m["reverse"], op) if m else None


def parse_helper(code, match):
    params = [PARAM.fullmatch(p.strip()) for p in match["params"].split(",")]
    if not all(p and p["type"] for p in params):
        return None
    names = tuple(p["name"] for p in params)
    types = tuple(p["type"].strip() for p in params)
    body = compact(code[match.end():concessions._scope_end(code, match.end())])
    result = re.sub(r"\b(?:static|inline|__inline__)\b", "", code[match.start():match.start("name")]).strip()
    for mask in names:
        found = operation(body, mask)
        if found:
            target, op = found
            return Helper(names, types, target, mask, op, result)
    return None


def file_facts(text):
    code = keepcheck.blank_comments(text)
    helpers = {m["name"]: parse_helper(code, m) for m in concessions._INLINE_DEF.finditer(code)}
    for m in concessions._MACRO_DEF.finditer(code):
        macro = concessions._FUNCTION_DEFINE.fullmatch(m[0].replace("\\\n", " "))
        if not macro:
            continue
        helpers[macro[1]] = None
        args = tuple(arg.strip() for arg in macro[2].split(","))
        body = compact(macro[3])
        # Only a statement-expression with a single scalar binding and a single operation.
        if (tmp := concessions._MASK_TEMP.search(macro[3])) and tmp[2] in args:
            prefix, tail = compact(macro[3][:tmp.start()]), compact(macro[3][tmp.end():])
            if prefix == "({" and tail.endswith("})"):
                body = tail[:-2]
                found = operation(body, tmp[1]) or operation("return" + body, tmp[1])
                if found:
                    target, op = found
                    helpers[macro[1]] = Helper(args, ("",) * len(args), target, tmp[2], op, "")
    constants = dict(DEFINE.findall(code))
    for enum in re.finditer(r"\benum(?:\s+\w+)?\s*\{([^{}]*)\}", code):
        previous = "-1"
        for item in enum[1].split(","):
            m = re.fullmatch(rf"\s*({ID})\s*(?:=\s*(.+?))?\s*", item, re.S)
            if m:
                constants[m[1]] = m[2] or f"({previous}) + 1"
                previous = m[1]
    return helpers, constants, concessions._INCLUDE.findall(text)


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
    helpers, constants = {}, {}
    seen = {path.resolve()}
    local, values, names = file_facts(text)

    def visit(parent, includes):
        for name in includes:
            target = header_path(parent, name)
            if target is None or target in seen:
                continue
            seen.add(target)
            defs, values, nested = header_facts(target)
            visit(target.parent, nested)
            helpers.update(defs)
            constants.update(values)

    visit(path.parent, names)
    helpers.update(local)
    constants.update(values)
    return helpers, constants


def substitute_target(helper, args):
    target = helper.target
    root = re.search(ID, target)[0]
    rest = target[target.index(root) + len(root):]
    if root not in helper.params:
        return target
    index = helper.params.index(root)
    arg = unparen(args[index].strip())
    owner = re.fullmatch(r"(?:const\s+)?((?:struct|union)\s+\w+)\s*\*\s*(?:const)?", helper.types[index])
    if target.startswith("*"):
        return unparen(arg[1:].strip()) if arg.startswith("&") else f"*({arg})"
    if rest.startswith("->"):
        if arg.startswith("&"):
            return f"({unparen(arg[1:].strip())}).{rest[2:]}"
        if owner and concessions._SCALAR_CAST.match(arg):
            return f"({arg}){rest}"
        return arg + rest if re.fullmatch(MEMBER, compact(arg)) else f"({arg}){rest}"
    return f"({arg}){rest}" if rest or not re.fullmatch(MEMBER, compact(arg)) else arg


def plain_call(helper, args, argument, values):
    """Return the replacement, field expression and the mask spelling, or a skip reason."""
    if len(args) != len(helper.params) or helper.params[argument] != helper.mask:
        return "helper has another mask or uses additional arguments"
    root = re.search(ID, helper.target)[0]
    needed = {helper.mask} | ({root} if root in helper.params else set())
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
    target = substitute_target(helper, args)
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
        expr = f"({target} {'|=' if op == 'set' else '&='} {sign}{literal})"
    return expr, target, args[argument].strip(), value


def normalize_field(expr):
    expr = compact(expr)
    while m := re.match(rf"^\(({MEMBER})\)(?=\.|->)", expr):
        expr = m[1] + expr[m.end():]
    return unparen(expr)


def field_key(expr, code, function, offset, helper=None):
    expr = normalize_field(expr)
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
        return (lhs["field"], op) if op else ("", "")
    rhs = re.match(rf"\s*&(?![&=])\s*(?P<field>{FIELD_EXPR})", after)
    if rhs:
        return rhs["field"], "and"
    return "", ""


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
    other_uses: str = ""
    edits: tuple = dataclass_field(default=(), repr=False)
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
    return "; ".join(found)


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
    helpers, values = unit_facts(path, text)
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
        if path.suffix != ".c":
            p.reason = "header site has no single owning translation unit"
            continue
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
                expr, op = local_operation(code, use, fold.name)
                owner, member = field_key(expr, code, span[0], use)
                if owner:
                    keys.add((owner, member, op))
            if len(keys) == 1:
                p.owner, p.field, p.operation = keys.pop()
            p.other_uses = other_uses(code, span, p, [(a, b) for a, b, _ in p.edits], fold.uses, values)
        else:
            helper = helpers.get(p.helper)
            call = concessions._CALLED.match(code, pos)
            args, end = concessions._arguments(code, call.end())
            p.literal = args[site.argument].strip()
            if helper is None:
                p.reason = "helper is outside the single flag expression grammar"
                continue
            form = plain_call(helper, args, site.argument, values)
            if isinstance(form, str):
                p.reason = form
                continue
            expr, target, p.literal, p.value = form
            p.owner, p.field = field_key(target, code, span[0], pos, helper)
            p.operation = helper.operation
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


class Checker:
    def __init__(self, out):
        self.work = out / "work"
        self.work.mkdir(parents=True, exist_ok=True)
        self.rom = check.load_rom(verify=False)
        self.syms = check.elf_symbols()
        self.ram = build.ram_symbols()
        build.headers_hash()

    def compile(self, path, text):
        with tempfile.TemporaryDirectory(prefix="unit-", dir=self.work) as scratch:
            work = os.path.relpath(scratch, ROOT)
            source = Path(scratch) / path.name
            source.write_text(relocate_includes(text, path))
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

    def compare(self, path, original, variant):
        expected = self.baseline(path, original)
        if isinstance(expected, str):
            return "SKIP", expected
        result = self.compile(path, variant)
        if isinstance(result, str):
            return "SKIP", "variant compile/link failed: " + result
        address, names, binary = result
        if (address, names) != expected[:2]:
            return "SKIP", "variant changed the unit's linked function inventory"
        if binary == expected[2]:
            return "PLAIN", f"MATCH ({len(binary)} bytes at 0x{address:08X})"
        count = sum(a != b for a, b in zip(binary, expected[2])) + abs(len(binary) - len(expected[2]))
        return "DIFF", f"{count} byte positions differ ({len(expected[2])} → {len(binary)} bytes at 0x{address:08X})"


def probe(p, sources, checker):
    if p.reason:
        return "SKIP", p.reason
    text = sources[p.path]
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
           "other_uses", "class", "detail")


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
            writer.writerow([getattr(p, key) for key in COLUMNS[:-2]] + [verdict, detail])
    atomic(path, write)


def counts(sites, results):
    return Counter(results.get(p.id, ("PENDING", ""))[0] for p in sites)


def table(sites, results, group, label, additional=None):
    groups = defaultdict(list)
    for p in sites:
        key = group(p)
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


def summary(sites, results, rom_orders):
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
    for title, key in (("Directory", lambda p: p.directory), ("Helper", lambda p: p.helper)):
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
        order = rom_orders.get((rom_field, {"and": "test", "nonzero": "test"}.get(op, op)))
        evidence = "—"
        if order:
            mask, load, interleaved = (order[k] for k in ("mask", "word", "interleaved"))
            share = f" ({mask / (mask + load):.1%})" if mask + load else ""
            evidence = f"{mask} / {load} / {interleaved}{share}"
        return ", ".join(f"`{name}`" for name in names), evidence

    yield from table(sites, results, lambda p: (p.owner, p.field, p.operation)
                     if p.owner and p.operation else None, "Type / field / operation", extra)


def write_reports(sites, results, args):
    rom_orders = defaultdict(Counter)
    if args.rom_tsv and args.rom_tsv.is_file():
        with args.rom_tsv.open() as stream:
            for row in csv.DictReader(stream, delimiter="\t"):
                rom_orders[row["field"], row["op"]][row["order"]] += 1
    note = [
        "# Plain flag and local-constant probes", "", "Tags: #compiler #phase3 #tooling", "",
        "Each row represents one linter binding or constant argument. A local probe uses the "
        "`fold_literals.py --every-read` edit for that binding. The CSV keeps the linter weight, "
        "including multiple uses charged to the same local. The sample uses a fixed seed and "
        "takes up to 40 sites per directory before the rest of the sweep.", "",
        "PLAIN is a byte-identical scratch rewrite. BOUND-SINGLE is a mismatch with one use "
        "found in the containing source function. BOUND-SHARED is a mismatch with other uses "
        "listed in the CSV. SKIP records an unsupported form, inactive code or check failure. "
        "A mismatch identifies a dependency in the current reconstruction. Confirm mask order "
        "and alternative matching C before inferring an original accessor or macro.", "",
        "The ROM-side scan follows recognized register provenance and omits cases it cannot "
        "attribute. Its branch-consumed tests are reported alongside both raw AND and boolean "
        "source forms. The compiler probes also cover non-flag locals.", "",
        *summary(sites, results, rom_orders), "",
    ]
    atomic(args.note, lambda f: f.write("\n".join(note)))
    sc = counts([p for p in sites if p.sample], results)
    total = counts(sites, results)
    weighted = sum(p.weight for p in sites)
    done = len(results)
    report = [
        "# Plain constant probe", "",
        f"Recorded {done:,} of {len(sites):,} bindings or arguments ({weighted:,} linter uses). "
        "The sweep edits scratch copies only. Game source, headers and progress files are untouched.", "",
    ]
    for name, c in (("Directory sample", sc), ("Full inventory", total)):
        report += [f"## {name}", "", ", ".join(f"{c[key]:,} {key}" for key in (*CLASSES, "PENDING")) + ".", ""]
    report += [
        "## Resume here", "",
        "From the worktree root run `build/venv/bin/python tools/plainprobe.py --resume`. "
        "The CSV is the checkpoint. The note contains the directory, helper and field breakdowns.", "",
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
    inputs = [Path(__file__), ROOT / "tools/concessions.py", ROOT / "tools/fold_literals.py",
              ROOT / "tools/check.py", ROOT / "symbols/functions.csv", ROOT / "symbols/ram.csv",
              ROOT / "build/boktai3.elf", ROOT / "baserom.gba"]
    for path in inputs:
        with path.open("rb") as stream:
            digest.update(hashlib.file_digest(stream, "sha256").digest())
    digest.update(build.headers_hash().encode())
    for name, text in sources.items():
        digest.update(name.encode() + b"\0" + hashlib.sha256(text.encode()).digest())
    return digest.hexdigest()


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("paths", nargs="*")
    parser.add_argument("--out", type=Path, default=OUT)
    parser.add_argument("--note", type=Path, default=NOTE)
    parser.add_argument("--report", type=Path)
    parser.add_argument("--rom-tsv", type=Path, default=OUT / "flag_sites.tsv")
    parser.add_argument("--resume", action="store_true")
    parser.add_argument("--sample-only", action="store_true")
    parser.add_argument("--apply-plain", action="store_true")
    parser.add_argument("-j", "--jobs", type=int, default=4)
    args = parser.parse_args(argv)
    if args.jobs < 1:
        parser.error("jobs must be positive")
    if args.apply_plain:
        parser.error("--apply-plain is not implemented yet")
    try:
        paths = inventory(args.paths)
    except ValueError as error:
        parser.error(str(error))
    args.out = args.out.resolve()
    args.out.mkdir(parents=True, exist_ok=True)
    if args.report is None and args.out == OUT:
        args.report = REPORT
    csv_path = args.out / "sites.csv"
    state = args.out / "snapshot.json"
    if csv_path.exists() and not args.resume:
        parser.error("sites.csv exists; use --resume or choose a new --out")
    sources = {os.path.relpath(path, ROOT): path.read_text(errors="replace") for path in paths}
    digest = snapshot(sources)
    results = {}
    if args.resume and state.exists():
        if json.loads(state.read_text())["digest"] != digest:
            parser.error("source, tool or ROM inputs changed; choose a new --out")
        if csv_path.exists():
            with csv_path.open() as stream:
                results = {r["id"]: (r["class"], r["detail"]) for r in csv.DictReader(stream)
                           if r["class"] in CLASSES}
    elif csv_path.exists():
        parser.error("sites.csv has no snapshot; choose a new --out")
    header_cache = {}
    sites = sample_order([site for name, text in sources.items() for site in plan(ROOT / name, text, header_cache)])
    if not set(results) <= {p.id for p in sites}:
        parser.error("checkpoint contains unknown sites")
    atomic(state, lambda f: json.dump({"digest": digest}, f))
    print(f"Inventory: {len(sites):,} sites, {sum(p.weight for p in sites):,} linter uses, "
          f"{sum(p.sample for p in sites):,} sampled, {len(results):,} recorded", flush=True)
    write_csv(csv_path, sites, results)
    checker = Checker(args.out)
    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        for phase in (True, False):
            if not phase and args.sample_only:
                break
            pending = [p for p in sites if p.sample == phase and p.id not in results]
            for start in range(0, len(pending), 32):
                if STOP.exists():
                    break
                batch = pending[start:start + 32]
                verdicts = pool.map(lambda p: probe(p, sources, checker), batch)
                results.update((p.id, verdict) for p, verdict in zip(batch, verdicts))
                write_csv(csv_path, sites, results)
                print(f"{len(results):,}/{len(sites):,}: {dict(counts(sites, results))}", flush=True)
            write_reports(sites, results, args)
            if STOP.exists():
                print("STOP detected; checkpoint written", flush=True)
                break
    return 0


if __name__ == "__main__":
    sys.exit(main())
