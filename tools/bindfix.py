#!/usr/bin/env python3
"""Rewrite bound flag constants into typed accessors of the object's own struct and field.

  build/venv/bin/python tools/bindfix.py PATH... [--dry] [--csv OUT.csv] [--no-joint]

A parked mask (`u32 mask = 0x100; ... actor->actionFlags & mask`) or a constant passed to
a generic pointer helper (`WordFlags_Test(&actor->actionFlags, 0x100)`) compiles exactly
like a call to an inline accessor with a scalar mask parameter. Each such site becomes
`PairedSceneHalf_TestActionFlags(actor, 0x100)`; the accessor is a `static inline` placed
right after the struct's definition (its header, or the unit). An accessor that already
exists for the same struct, member chain and operation is reused under its own name.

Only set / clear / keep (`&=` a constant) / test operations on a member reached through a
typed struct pointer or struct object are in scope; everything else (zero/one locals, loop
bounds, angles, untyped pointers) is counted and left alone. A rewrite is kept only when
the unit still matches (keepcheck: MATCH and no new compiler warning). Sites are tried one
at a time; the sites of a function that failed alone are then tried together. Headers that
gained an accessor are checked on all their dependents at the end; if one breaks, the
header's new accessors move into the units that use them.

The CSV lists every site with its verdict: converted (single / joint), already-accessor,
out-of-scope (reason), failed (reason).
"""
from __future__ import annotations

import argparse
import csv
import os
import re
import subprocess
import sys
import time
from collections import Counter, defaultdict
from functools import lru_cache
from dataclasses import dataclass, field as dfield
from pathlib import Path

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import concessions  # noqa: E402
import fold_literals as folds  # noqa: E402
import header_dependents  # noqa: E402
import keepcheck  # noqa: E402
import plainprobe as pp  # noqa: E402

ROOT = Path(keepcheck.ROOT)
ID = pp.ID
OPS = {"set": "Set", "clear": "Clear", "and-store": "Keep", "and": "Test", "nonzero": "Test"}
BODY = {"Set": "{p}->{chain} |= {m};", "Clear": "{p}->{chain} &= ~{m};",
        "Keep": "{p}->{chain} &= {m};", "Test": "return {p}->{chain} & {m};"}
# Tokens after which `FIELD & MASK` can be replaced by a call without regrouping the expression.
SAFE_BEFORE = re.compile(r"(?:^|[({;}!,?:]|(?<![=!<>])=|&&|\|\||(?<![|])\|(?![|=])|\^|\breturn)\s*$")
SAFE_AFTER = re.compile(r"\s*(?:[),;?:\]]|!=|==|&&|\|\||\|(?![|=])|\^|&(?![&=]))")


@dataclass
class Use:
    start: int
    end: int
    pointer: str
    owner: str
    chain: str
    op: str             # Set / Clear / Keep / Test
    literal: str
    mask_type: str
    result: str = "u32"
    keep_literal: str = ""  # the original ~BITS of a keep rewritten as a clear, for the fallback


@dataclass
class Site:
    unit: str
    function: str
    kind: str
    name: str           # local name or helper name
    literal: str
    line: int
    weight: int
    uses: list = dfield(default_factory=list)
    cuts: list = dfield(default_factory=list)   # declaration edits, when every read is rewritten
    verdict: str = ""
    detail: str = ""

    text: str = ""
    occurrence: int = 0

    @property
    def key(self):
        return (self.unit, self.function, self.kind, self.name, self.literal, self.text, self.occurrence)


def cap(name):
    return name[:1].upper() + name[1:]


def tag(owner):
    return owner.split()[-1]


def accessor_name(owner, chain, op):
    return f"{tag(owner)}_{op}{cap(chain.split('.')[-1])}"


def split_target(target):
    """(pointer expression, member chain) of a member access on a struct pointer or object."""
    expr = pp.normalize_field(target)
    m = re.fullmatch(rf"({ID})(->|\.)({ID}(?:\.{ID})*)", expr)
    if not m:
        m2 = re.fullmatch(rf"(\(\((?:const)?(?:struct|union){ID}\*\)\(.+\)\))->({ID}(?:\.{ID})*)", expr)
        return (m2[1], m2[2]) if m2 else (None, None)
    return (m[1] if m[2] == "->" else "&" + m[1]), m[3]


def literal_text(lit):
    return re.sub(r"\s+", "", lit)


def expression_use(code, use, name):
    """(start, end, target, op) of the flag expression around one read of NAME, or None."""
    before, after = code[:use], code[use + len(name):]
    lhs = re.search(rf"(?P<field>{pp.FIELD_EXPR})\s*(?P<op>\|=|&=|(?<![=&])&(?!&))\s*(?P<neg>~?)\s*$", before)
    if lhs:
        start, target = lhs.start("field"), lhs["field"]
        op = {("|=", ""): "Set", ("&=", ""): "Keep", ("&=", "~"): "Clear", ("&", ""): "Test"}.get(
            (lhs["op"], lhs["neg"]))
        if op is None:
            return None
        if op == "Test":
            stem = before[:start]
            assign = re.search(rf"(?<![\w.>])({re.escape(target)})\s*=\s*\(?\s*$", stem)
            if assign:  # x = x & mask
                if stem.rstrip().endswith("("):
                    return None
                start, op = assign.start(1), "Keep"
                if not re.match(r"\s*;", after):
                    return None
            elif not SAFE_BEFORE.search(stem) or not SAFE_AFTER.match(after):
                return None
        elif not re.match(r"\s*[;,)]", after):
            return None
        return start, use + len(name), target, op
    rhs = re.match(rf"\s*&\s*(?P<field>{pp.FIELD_EXPR})(?![\w.>\[(])", after)
    if rhs and SAFE_BEFORE.search(before) and SAFE_AFTER.match(after[rhs.end():]):
        return use, use + len(name) + rhs.end(), rhs["field"], "Test"
    return None


def helper_calls(code, helpers):
    """{argument offset: (call start, call end, helper form, args)} for flag-helper calls."""
    out = {}
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
            offset = call.end() + sum(len(a) + 1 for a in args[:index]) + len(actual) - len(actual.lstrip())
            out[offset] = (call.start(), end, helper, args)
    return out


def typed_key(index, h):
    """(owner, chain, op) when helper form H is already a typed accessor of one struct member."""
    if len(h.params) != 2 or h.alias or index != h.params.index(h.mask) or h.operation not in OPS:
        return None
    other = 1 - index
    owner = re.fullmatch(r"(?:const\s+)?((?:struct|union)\s+\w+)\s*\*", h.types[other].strip())
    m = re.fullmatch(rf"{re.escape(h.params[other])}->({ID}(?:\.{ID})*)", pp.compact(h.target))
    return (owner[1], m[1], OPS[h.operation]) if owner and m else None


def typed_accessors(helpers):
    """{(owner, chain, op): [names]} for inline helpers that already are typed accessors."""
    found = defaultdict(list)
    for name, forms in helpers.items():
        for index, h in forms.items():
            if key := typed_key(index, h):
                found[key].append(name)
    return found


def mask_type_of(code, start, name):
    m = re.search(rf"((?:const\s+)?(?:{concessions._SCALAR_TYPE}))\s+(?:\w+\s*(?:=[^,;]*)?,\s*)*{re.escape(name)}\b",
                  code[start:])
    return m[1].removeprefix("const ").strip() if m else "u32"


def collect(rel, text):
    """The unit's sites, with each in-scope read resolved to an accessor use."""
    path = ROOT / rel
    code = keepcheck.blank_comments(text)
    probes = pp.plan(path, text, {})
    helpers, values, _ = pp.unit_facts(path, text)
    calls = helper_calls(code, helpers)
    spans = keepcheck.function_spans(text)
    out = []
    lines = text.splitlines()
    seen = Counter()
    for p in probes:
        site = Site(rel, p.function, p.kind, p.spelling or p.helper, p.literal, p.line, p.weight)
        site.text = pp.compact(lines[p.line - 1]) if p.line <= len(lines) else ""
        stem = site.key[:-1]
        site.occurrence = seen[stem]
        seen[stem] += 1
        out.append(site)
        if p.reason:
            site.verdict, site.detail = "out-of-scope", p.reason
            continue
        span = next((s for s in spans if s[0] <= p.offset < s[1]), None)
        if p.kind == "literal-param":
            helper = helpers.get(p.helper, {}).get(p.argument)
            if helper is None:
                site.verdict, site.detail = "out-of-scope", "helper is not a recognized flag operation"
                continue
            if typed_key(p.argument, helper):
                site.verdict, site.detail = "already-accessor", p.helper
                continue
            call = concessions._CALLED.match(code, p.offset)
            args, end = concessions._arguments(code, call.end())
            target = pp.substitute_target(helper, args, code, span[0], p.offset)
            use = make_use(code, span, p.offset, end, target, helper.operation, args[p.argument].strip(),
                           helper.types[p.argument].removeprefix("const ").strip() or "u32",
                           helper.result if helper.result not in ("", "void") else "u32")
            if isinstance(use, str):
                site.verdict, site.detail = "out-of-scope", use
                continue
            site.uses = [use]
            continue
        # A parked local: rewrite each read that is a flag operation on a typed member.
        reads = [(a, b) for a, b, r in p.edits if r and code[a:b] == p.spelling]
        site.cuts = [(a, b, r) for a, b, r in p.edits if (a, b) not in reads]
        literal = literal_text(p.literal)
        mtype = mask_type_of(code, span[0], p.spelling)
        reasons = []
        for a, _ in reads:
            if a in calls:
                start, end, helper, args = calls[a]
                target = pp.substitute_target(helper, args, code, span[0], a)
                use = make_use(code, span, start, end, target, helper.operation, literal, mtype,
                               helper.result if helper.result not in ("", "void") else "u32", helper=helper)
            else:
                found = expression_use(code, a, p.spelling)
                use = make_use(code, span, found[0], found[1], found[2], found[3], literal, mtype) \
                    if found else "read is not a flag operation on a member"
            if isinstance(use, str):
                reasons.append(use)
            else:
                site.uses.append(use)
        if not site.uses:
            site.verdict, site.detail = "out-of-scope", reasons[0] if reasons else "no reads"
            continue
        if reasons:  # the local stays for its other reads
            site.cuts = []
            site.detail = f"{len(reasons)} other read(s) keep the local: {reasons[0]}"
    return out


def make_use(code, span, start, end, target, op, literal, mask_type, result="u32", helper=None):
    op = OPS.get(op, op)
    if op not in BODY:
        return f"operation {op} is not set/clear/test"
    pointer, chain = split_target(target)
    if pointer is None:
        return f"target {pp.compact(target)} is not a member of a struct pointer or object"
    owner, member = pp.field_key(target, code, span[0], start, helper)
    if not owner.startswith(("struct ", "union ")) or member != chain:
        return f"owner of {pp.compact(target)} is not a known struct type ({owner or 'unresolved'})"
    if op == "Keep" and re.fullmatch(r"~\s*(?:0[xX][0-9a-fA-F]+|\d+)[uUlL]*", literal.strip()):
        keep_literal = literal.strip()
        op, literal = "Clear", keep_literal[1:].strip()  # a keep mask spelled ~BITS is a clear of BITS
    else:
        keep_literal = ""
    if cast := re.fullmatch(rf"\(\(((?:const)?(?:struct|union))({ID})\*\)\((.+)\)\)", pointer):
        inner = cast[3]
        void = re.fullmatch(ID, inner) and re.search(rf"\bvoid\s*\*\s*{inner}\b", code[span[0]:start])
        pointer = inner if void else f"({cast[1]} {cast[2]} *){inner if re.fullmatch(ID, inner) else f'({inner})'}"
    return Use(start, end, pointer, owner, chain, op, literal, mask_type, result if op == "Test" else "void",
               keep_literal)


def unparen_span(code, start, end):
    """Widen [start, end) over parentheses that only grouped the rewritten expression."""
    while True:
        a = start - 1
        while a >= 0 and code[a] in " \t":
            a -= 1
        b = end
        while b < len(code) and code[b] in " \t":
            b += 1
        if a < 0 or b >= len(code) or code[a] != "(" or code[b] != ")":
            return start, end
        word = re.search(r"(\w+)\s*$", code[:a])
        if word:
            return start, end  # a call's, a statement's or a return's own parentheses
        start, end = a, b + 1


BARE_BLOCK = re.compile(r"(?m)^(?P<ind>[ \t]*)\{[ \t]*\n(?:[ \t]*\n)*[ \t]*(?P<body>[^\n{};]*;)[ \t]*\n"
                        r"(?:[ \t]*\n)*(?P=ind)\}[ \t]*\n")


def flatten_blocks(text, names):
    """Drop a bare block left holding one accessor call after its declaration was removed."""
    def flat(m):
        before = text[:m.start()].rstrip()
        if not before.endswith((";", "{", "}")) or not any(f"{n}(" in m["body"] for n in names):
            return m[0]
        return m["ind"] + m["body"] + "\n"
    return BARE_BLOCK.sub(flat, text)


# ---------------------------------------------------------------- struct definitions

@lru_cache(maxsize=1)
def include_tree():
    return header_dependents.tree()


@lru_cache(maxsize=None)
def includes_of(rel):
    files = include_tree()
    seen, todo = set(), [rel]
    while todo:
        for h in files.get(todo.pop(), ()):
            if h not in seen:
                seen.add(h)
                todo.append(h)
    return seen


def definition(owner, rel, text):
    """(file, offset just after the definition's `;`) for OWNER, preferring the unit itself."""
    kw, name = owner.split()
    pat = re.compile(rf"\b{kw}\s+{name}\s*\{{")
    for path in [rel] + sorted(includes_of(rel)):
        if path.startswith("src/") and path != rel:
            continue
        body = text if path == rel else (ROOT / path).read_text(errors="replace")
        code = keepcheck.blank_comments(body)
        for m in pat.finditer(code):
            if code[:m.start()].count("{") != code[:m.start()].count("}"):
                continue  # nested in another definition
            close = concessions._scope_end(code, m.end())
            semi = code.find(";", close)
            if semi < 0:
                continue
            return path, semi + 1
    return None, None


def local_header(rel):
    """The accessor header for a unit's directory: src/X/Y/u.c -> include/X/Y/flag_accessors.h."""
    return "include/" + str(Path(rel).parent.relative_to("src")) + "/flag_accessors.h"


def header_text(path):
    if (ROOT / path).exists():
        return (ROOT / path).read_text()
    guard = "GUARD_" + re.sub(r"\W", "_", "_".join(Path(path).parts[-2:])).upper()
    return f"#ifndef {guard}\n#define {guard}\n\n#endif\n"


def accessor_text(name, use):
    params = f"{use.owner} *p, {use.mask_type} mask"
    body = BODY[use.op].format(p="p", chain=use.chain.replace(".", "."), m="mask")
    return f"\n\nstatic inline {use.result} {name}({params})\n{{\n    {body}\n}}"


# ---------------------------------------------------------------- applying and checking

class Run:
    def __init__(self, args):
        self.args = args
        roots = getattr(args, "paths", None) or []
        self.prefixes = tuple(p.rstrip("/") + "/" for p in roots) + tuple(
            "include/" + p.rstrip("/").removeprefix("src/") + "/" for p in roots if p.startswith("src/"))
        self.created = {}         # name -> (file, owner, chain, op)
        self.header_users = defaultdict(set)
        self.reused = Counter()
        self.variants = Counter()
        self.compile_seconds = 0.0
        self.checks = 0

    def allowed(self, path):
        return not self.prefixes or path.startswith(self.prefixes)

    def plan_edits(self, rel, text, sites, fresh=False, keep=False):
        """({file: [(a, b, text)]}, accessor names created) for SITES applied together."""
        unit_code = keepcheck.blank_comments(text)
        helpers, _, _ = pp.unit_facts(ROOT / rel, text)
        existing = typed_accessors(helpers)
        local_defs = {m["name"]: m.start() for m in concessions._INLINE_DEF.finditer(unit_code)}
        edits = defaultdict(list)
        new, reused, pre, includes_done = {}, set(), {}, set()
        for site in sites:
            for use in site.uses:
                if keep and use.keep_literal:
                    use = Use(use.start, use.end, use.pointer, use.owner, use.chain, "Keep", use.keep_literal,
                              use.mask_type, use.result)
                key = (use.owner, use.chain, use.op)
                visible = [n for n in existing.get(key, ()) if local_defs.get(n, -1) < use.start
                           and (not fresh or n in self.created or n == accessor_name(*key))]
                name = visible[0] if visible else None
                local = local_header(rel)
                if name is None and not fresh and re.search(rf"\b{accessor_name(*key)}\(", header_text(local)):
                    name = accessor_name(*key)  # made for a neighbor unit already
                    if f'#include "{local.removeprefix("include/")}"' not in text and rel not in includes_done:
                        includes_done.add(rel)
                        inc = list(re.finditer(r'(?m)^#include "[^"]+"[^\n]*\n', text))
                        at = inc[-1].end() if inc else 0
                        edits[rel].append((at, at, f'#include "{local.removeprefix("include/")}"\n'))
                elif name is None:
                    name = accessor_name(*key)
                    if name in new:
                        pass
                    elif re.search(rf"\b{name}\b", unit_code) or name in helpers:
                        return None, f"{name} already names something else"
                    else:
                        where, offset = definition(use.owner, rel, text)
                        if where is None:
                            return None, f"no definition of {use.owner} visible to the unit"
                        if where != rel and not self.allowed(where):
                            # The struct lives outside the directories this run may edit: the
                            # accessor goes in a small header next to its users instead.
                            body = header_text(local)
                            if f'#include "{where.removeprefix("include/")}"' not in body:
                                pre.setdefault(local, set()).add(where.removeprefix("include/"))
                            if f'#include "{local.removeprefix("include/")}"' not in text and \
                                    rel not in includes_done:
                                includes_done.add(rel)
                                inc = list(re.finditer(r'(?m)^#include "[^"]+"[^\n]*\n', text))
                                at = inc[-1].end() if inc else 0
                                edits[rel].append((at, at, f'#include "{local.removeprefix("include/")}"\n'))
                            where, offset = local, body.rindex("#endif")
                        new[name] = (where, offset, accessor_text(name, use), key)
                elif name not in self.created:
                    reused.add(name)
                start, end = unparen_span(unit_code, use.start, use.end)
                edits[rel].append((start, end, f"{name}({use.pointer}, {use.literal})"))
            edits[rel].extend(site.cuts)

        for name, (where, offset, body, _) in new.items():
            if where in pre or not (ROOT / where).exists():
                body = body.lstrip("\n") + "\n\n"
            elif where.endswith(".c"):
                body += "\n"  # keep a blank line before what followed the struct
            edits[where].append((offset, offset, body))
        for local, headers in pre.items():
            base = header_text(local)
            at = base.index("\n\n", base.index("#define")) + 1
            lead = "" if (ROOT / local).exists() else "\n"
            edits[local].append((at, at, lead + "".join(f'#include "{h}"\n' for h in sorted(headers))))
        spans = sorted((a, b) for a, b, _ in edits[rel] if a != b)
        if any(b1 > a2 for (_, b1), (a2, _) in zip(spans, spans[1:])):
            return None, "overlapping rewrites"
        return (edits, new, reused), ""

    def attempt(self, rel, sites):
        """Try the rewrite; then with fresh accessors (an existing one may take another mask type),
        then with keep masks spelled as they were."""
        first = None
        for fresh, keep in ((False, False), (True, False), (False, True), (True, True)):
            if (fresh or keep) and not self.args.dry:
                planned, _ = self.plan_edits(rel, (ROOT / rel).read_text(), sites, fresh, keep)
                base, _ = self.plan_edits(rel, (ROOT / rel).read_text(), sites)
                if planned is None or base is None or planned[0] == base[0]:
                    continue
            ok, why = self.try_variant(rel, sites, fresh, keep)
            if ok:
                if fresh or keep:
                    self.variants[("fresh" if fresh else "") + ("keep" if keep else "")] += 1
                return ok, why
            first = first or why
            if self.args.dry:
                break
        return False, first

    def try_variant(self, rel, sites, fresh, keep):
        unit = keepcheck.Unit(rel)
        planned, reason = self.plan_edits(rel, unit.text, sites, fresh, keep)
        if planned is None:
            return False, reason
        edits, new, reused = planned
        if self.args.dry:
            return True, "dry run"
        others = {}
        for path, items in edits.items():
            if path != rel:
                others[path] = (ROOT / path).read_text() if (ROOT / path).exists() else None
                (ROOT / path).write_text(folds.edit_text(header_text(path), items))
        t = time.time()
        names = {re.match(ID, r)[0] for _, _, r in edits[rel] if re.match(rf"{ID}\(", r)}
        ok = unit.attempt(flatten_blocks(folds.edit_text(unit.text, edits[rel]), names))
        self.compile_seconds += time.time() - t
        self.checks += 1
        if not ok:
            for path, old in others.items():
                if old is None:
                    os.remove(ROOT / path)
                else:
                    (ROOT / path).write_text(old)
            return False, unit.last
        for name, (where, _, _, key) in new.items():
            self.created[name] = (where, *key)
        for name in reused:
            self.reused[name] += 1
        for path in others:
            if path.endswith(".h"):
                self.header_users[path].add(rel)
        pp.header_facts.cache_clear()
        return True, ""

    def unit(self, rel, report):
        tried, joint_pool = set(), defaultdict(list)
        original = (ROOT / rel).read_text()
        clean = keepcheck.formatted(original) == original
        stale = True
        while True:
            if stale:
                pp.header_facts.cache_clear()
                sites = collect(rel, (ROOT / rel).read_text())
                stale = False
                for s in sites:
                    if s.verdict and s.key not in tried:
                        tried.add(s.key)
                        report.append(s)
            todo = [s for s in sites if not s.verdict and s.key not in tried]
            if not todo:
                break
            site = todo[0]
            tried.add(site.key)
            ok, why = self.attempt(rel, [site])
            site.verdict, site.detail = ("converted-single", site.detail) if ok else ("failed", why)
            report.append(site)
            if ok and not self.args.dry:
                stale = True
            elif not ok:
                joint_pool[site.function].append(site.key)
        if not self.args.no_joint:
            self.joint(rel, report, joint_pool)
        if clean and not self.args.dry:
            unit = keepcheck.Unit(rel)
            text = keepcheck.formatted(unit.text)
            if text and text != unit.text and not unit.attempt(text):
                print(f"  {rel}: clang-format result did not match; left unformatted")

    def joint(self, rel, report, joint_pool):
        for function, keys in joint_pool.items():
            if len(keys) < 2:
                continue
            text = (ROOT / rel).read_text()
            pp.header_facts.cache_clear()
            current = {s.key: s for s in collect(rel, text)}
            group = [current[k] for k in keys if k in current and not current[k].verdict]
            if len(group) < 2:
                continue
            ok, why = self.attempt(rel, group)
            if ok:
                for s in report:
                    if s.key in keys and s.verdict == "failed":
                        s.verdict, s.detail = "converted-joint", f"with {len(group) - 1} other site(s)"

    def check_headers(self):
        """Every dependent of a header that gained an accessor must still match."""
        failures = {}
        for header in sorted(self.header_users):
            r = subprocess.run([sys.executable, str(ROOT / "tools/header_dependents.py"), header, "--check"],
                               cwd=ROOT, capture_output=True, text=True)
            if r.returncode:
                failures[header] = (r.stdout + r.stderr).strip().splitlines()[-5:]
        return failures


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("paths", nargs="+")
    ap.add_argument("--dry", action="store_true", help="plan only; touch nothing")
    ap.add_argument("--no-joint", action="store_true", help="skip the per-function joint pass")
    ap.add_argument("--csv", help="write one row per site")
    args = ap.parse_args(argv)
    run = Run(args)
    report = []
    units = keepcheck.src_files(args.paths)
    start = time.time()
    for rel in units:
        run.unit(rel, report)
    header_failures = {} if args.dry else run.check_headers()
    counts = Counter(s.verdict for s in report)
    weights = Counter()
    for s in report:
        weights[s.verdict] += s.weight
    print(f"{len(units)} units, {len(report)} sites, {time.time() - start:.0f}s "
          f"({run.checks} checks, {run.compile_seconds:.0f}s in check)")
    for verdict, n in sorted(counts.items()):
        print(f"  {verdict:18} {n:4} sites, weight {weights[verdict]}")
    print(f"  accessors created: {len(run.created)}; existing reused: {sum(run.reused.values())} uses "
          f"of {len(run.reused)}")
    print(f"  kept only as a variant: {dict(run.variants)}")
    for name, (where, *_rest) in sorted(run.created.items()):
        print(f"    + {name} in {where}")
    for header, tail in header_failures.items():
        print(f"  HEADER DEPENDENTS FAIL {header}:", *tail, sep="\n    ")
    fails = Counter(re.sub(r"0x[0-9A-F]+|\d+", "N", s.detail)[:70] for s in report if s.verdict == "failed")
    for reason, n in fails.most_common(8):
        print(f"  failed {n:3}: {reason}")
    outs = Counter(re.sub(r"\(.*", "", s.detail)[:70] for s in report if s.verdict == "out-of-scope")
    for reason, n in outs.most_common(8):
        print(f"  out-of-scope {n:3}: {reason}")
    if args.csv:
        with open(args.csv, "w", newline="") as f:
            w = csv.writer(f)
            w.writerow(["unit", "line", "function", "kind", "name", "literal", "weight", "verdict", "detail",
                        "accessors"])
            for s in report:
                w.writerow([s.unit, s.line, s.function, s.kind, s.name, s.literal, s.weight, s.verdict,
                            s.detail, ";".join(sorted({accessor_name(u.owner, u.chain, u.op) for u in s.uses}))])
    return 1 if header_failures else 0


if __name__ == "__main__":
    sys.exit(main())
