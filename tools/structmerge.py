#!/usr/bin/env python3
"""Preview or apply one family from structfamilies.py, gated by byte-exact checks."""

from __future__ import annotations

import argparse
from collections import Counter, defaultdict
import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile

import inline_twins
import keepcheck
import layoutmerge as lm
import ramlayout
import struct_evidence as evidence
import structfamilies as families
from pycparser.c_parser import ParseError


RULE = ("Choose the type whose width and extension have the most support from distinct ROM functions "
        "loading at that offset, then the type used by the most copies. Take the name from the copy "
        "with the most informative fields, then the most typed fields. Ties have stable spelling "
        "order and weak or disputed choices are marked confidence=low.")
VOLATILE_TYPE = re.compile(r"\b(?:volatile|v[us](?:8|16|32|64))\b")


class Holdout(ValueError):
    """A proposal needed a source or layout judgment beyond the automatic pass."""


def specificity(view):
    return sum(not lm.OPAQUE.match(lf["name"].rsplit(".", 1)[-1]) for lf in view["known"]), len(view["known"])


def type_key(member):
    return tuple(member.get(k) for k in ("ctype", "size", "align", "array", "esize", "bits", "bitpos"))


def bit_spans(view):
    return {lf["name"]: (lf["bitpos"], lf["bits"]) for lf in view["leaves"]
            if lf.get("bits") is not None and lf.get("bitpos") is not None}


def whole_byte_bits(member, span):
    return span == (member["off"] * 8, member["size"] * 8)


def related_width(member):
    if member["array"]:
        return member.get("esize")
    return member.get("width")


def forward_tag(member):
    """A direct pointer to a named record needs only its tag declaration."""
    match = re.fullmatch(r"(?:const )?((?:struct|union) [A-Za-z_]\w*) \*", member["ctype"])
    if match and member["private"] == [match[1]]:
        return match[1]
    return None


def load_vote(view, member, loads):
    """Count voters at measured leaf offsets, including leaves within aggregates."""
    if member["bits"]:
        width = member["size"]
        signatures = {(member["off"], width, member["signed"] if width < 4 else None)}
        return {(off, fn) for off, width, signed, fn, _ in loads if (off, width, signed) in signatures}
    root = member["name"]
    leaves = [lf for lf in view["known"] if lf["name"] == root or lf["name"].startswith((root + ".", root + "["))]
    signatures = {(lf["off"], lf.get("esize") if lf["array"] else lf["width"], lf["signed"])
                  for lf in leaves if lf["base"] == "int"}
    signatures |= {(lf["off"], lf.get("esize") if lf["array"] else lf["width"], None)
                   for lf in leaves if lf["base"] in {"ptr", "int"} and (lf.get("esize") or lf["width"]) == 4}
    if not signatures and member["base"] == "int":
        width = related_width(member)
        signatures.add((member["off"], width, member["signed"] if width != 4 else None))
    return {(off, fn) for off, width, signed, fn, _ in loads if (off, width, signed) in signatures}


def pick_candidates(views, loads):
    choices = defaultdict(list)
    for view in views:
        occurrences = Counter()
        spans = bit_spans(view)
        for m in view["members"]:
            if families.opaque(m):
                continue
            if not m.get("name") or m.get("off") is None or m.get("size") is None:
                raise Holdout(f"{view['id']} has an unmeasured member")
            if m["bits"] and not whole_byte_bits(m, spans.get(m["name"])):
                raise Holdout(f"{view['id']} contains bitfields that need an allocation-group review")
            slot = (m["off"],)
            if view["kind"] == "union":
                # Deliberate union overlays are separate alternatives. Compare names
                # across copies only within the same scalar/array/aggregate shape.
                shape = (m["off"], m["size"], m["base"], m["array"], m.get("esize"), m.get("signed"), m["bits"])
                occurrences[shape] += 1
                slot = (*shape, occurrences[shape])
            choices[slot].append((view, m))
    output = []
    for entries in choices.values():
        grouped = defaultdict(list)
        for view, m in entries:
            key = (m["base"], m["width"], m["signed"], m["size"], m["bitpos"]) if m["bits"] else type_key(m)
            grouped[key].append((view, m))
        proposals = []
        for supporters in grouped.values():
            eligible = [(v, m) for v, m in supporters
                        if (not m["private"] or forward_tag(m)) and not VOLATILE_TYPE.search(m["decl"])]
            if not eligible:
                continue
            qualified = sorted(eligible, key=lambda vm: (
                bool(lm.OPAQUE.match(vm[1]["name"])), tuple(-x for x in specificity(vm[0])),
                vm[1]["name"], vm[0]["id"]))
            view, member = qualified[0]
            voters = set().union(*(load_vote(v, m, loads) for v, m in supporters))
            score = (len(voters), len({v["id"] for v, _ in supporters}))
            proposals.append((score, view, member))
        if not proposals:
            view, member = entries[0]
            dependency = ", ".join(member["private"]) or "volatile storage"
            raise Holdout(f"{view['id']}.{member['name']} depends on {dependency}")
        proposals.sort(key=lambda p: (-p[0][0], -p[0][1], tuple(-x for x in specificity(p[1])),
                                      p[2]["ctype"], p[2]["name"], p[1]["id"]))
        score, view, member = proposals[0]
        names = {m["name"] for _, m in entries}
        guessed = len(grouped) > 1 and (not score[0] or len(proposals) > 1 and score[0] == proposals[1][0][0])
        guessed |= score == (0, 1) or len(names) > 1 or bool(lm.OPAQUE.match(member["name"]))
        output.append((score, view, member, guessed))
    return output


def plan_header(family, views, name, header):
    selected, skipped = [], []
    candidates = pick_candidates(views, family["loads"])
    for score, view, member, guess in sorted(candidates, key=lambda p: (
            -p[0][0], -p[0][1], tuple(-x for x in specificity(p[1])), p[2]["off"], -p[2]["size"], p[2]["name"])):
        start, end = member["off"], member["off"] + member["size"]
        collision = next((m for m, _, _, _ in selected if m["off"] < end and start < m["off"] + m["size"]), None)
        if collision and family["kind"] == "struct":
            skipped.append(f"{start:#x} {member['name']} overlaps selected {collision['name']}")
            continue
        selected.append((member, view, score, guess))

    kind = family["kind"]
    guard = "GUARD_" + re.sub(r"[^A-Za-z0-9]", "_", str(header)).upper()
    headers = {h for m, _, _, _ in selected for h in m["headers"]}
    forwards = {tag for m, _, _, _ in selected if (tag := forward_tag(m)) and tag != f"{kind} {name}"}
    buf = [f"#ifndef {guard}", f"#define {guard}", "", '#include "global.h"']
    buf.extend(f'#include "{ramlayout.inc_name(h)}"' for h in sorted(headers - {"include/global.h", "include/gba/types.h"}))
    if forwards:
        buf.append("")
        buf.extend(f"{tag};" for tag in sorted(forwards))
    buf.extend(("", f"{kind} {name} {{"))
    offset = 0
    names = {m["name"] for m, _, _, _ in selected}
    used = set()
    expected = {}
    decisions = []
    for original, view, (rom_votes, votes), guess in sorted(selected, key=lambda p: (p[0]["off"], p[0]["name"])):
        off, size = original["off"], original["size"]
        if kind == "struct" and off > offset:
            pad(buf, names | used, offset, off - offset)
        member = original["name"]
        if member in used:
            member = available_name(f"{member}{off:X}", names | used)
            guess = True
        used.add(member)
        expected[member] = (off, size, original["bitpos"], size * 8) if original["bits"] else (off, size)
        decl = original["decl"]
        if original["self_ref"]:
            decl = re.sub(rf"\b{re.escape(view['spell'])}\b", f"{kind} {name}", decl)
        if member != original["name"]:
            decl = re.sub(rf"\b{re.escape(original['name'])}\b", member, decl, count=1)
        comment = " /* confidence=low */" if guess else ""
        buf.append("    " + decl.replace("\n", "\n    ") + ";" + comment)
        decisions.append(f"{off:#x} {original['ctype']} {member} ({rom_votes} ROM votes, {votes} copies)"
                         + (" confidence=low" if guess else ""))
        offset = max(offset, off + size)
    if offset < family["size"]:
        base = offset if kind == "struct" else 0
        pad(buf, names | used, base, family["size"] - base)
    spell = f"{kind} {name}"
    buf.extend(("};", "", f"typedef char {name}SizeCheck[sizeof({spell}) == {family['size']:#x} ? 1 : -1];",
                "", "#endif", ""))
    return "\n".join(buf), expected, decisions, skipped


def available_name(name, used):
    while name in used:
        name += "_"
    return name


def pad(lines, used, off, size):
    name = available_name(f"pad{off:X}", used)
    lines.append(f"    u8 {name}[{size:#x}];")


def probe_header(text, spell, expected, size, align):
    """Validate the new layout under stock agbcc before adding it to include/."""
    with tempfile.NamedTemporaryFile(mode="w", suffix=".c", dir=families.ROOT / "build") as file:
        file.write(text)
        file.flush()
        pre = lm.preprocess(file.name, nonmatching=False)
        if pre is None:
            raise Holdout("generated header failed preprocessing")
        try:
            body, _, tu, _ = evidence.parse(pre, file.name)
        except ParseError as exc:
            raise Holdout(f"generated header failed parsing: {exc}") from exc
        kind, tag = spell.split()
        node = tu.tags.get((kind, tag))
        measured = lm.probe_records(body, tu, {spell: (spell, node)}) if node else {}
        view = measured.get(spell)
    if not view or view["size"] != size or view["align"] != align:
        actual = (view["size"], view["align"]) if view else None
        raise Holdout(f"generated header measured {actual}, expected size/alignment ({size}, {align})")
    spans = bit_spans(view)
    measured_members = {m["name"]: (m.get("off"), m.get("size"), *spans.get(m["name"], ()))
                        for m in view["members"]}
    for name, layout in expected.items():
        actual = measured_members.get(name)
        if actual != layout:
            raise Holdout(f"generated {name} measured {actual}, expected {layout}")
    view["known"] = families.inhabited(view)
    return view


def field_paths(old, new):
    """Storage-preserving replacements for direct members, including a flattened leaf."""
    fields = {}
    old_bits, new_bits = bit_spans(old), bit_spans(new)
    choices = new["members"] + [dict(lf, name=lf["name"], base=lf["base"])
                               for lf in new["known"] if "." in lf["name"]]
    for m in old["members"]:
        if m.get("off") is None or m.get("size") is None:
            continue
        whole_bytes = (m["off"] * 8, m["size"] * 8)
        occupied_bits = old_bits.get(m["name"], whole_bytes)
        hits = {n["name"] for n in choices if n.get("off") == m["off"] and n.get("size") == m["size"]
                and n.get("array") == m["array"] and n.get("esize") == m.get("esize")
                and (n.get("base") == m["base"] or {m["base"], n.get("base")} <= {"int", "enum"})
                and occupied_bits == new_bits.get(n["name"], whole_bytes)}
        if m["base"] == "aggregate" and not m["array"] and m["size"]:
            for n in new["members"]:
                # A complete embedded sprite in a private view can be one
                # element of the owning object's sprite array.
                if (n["array"] and n.get("off") is not None and n.get("esize") == m["size"]
                        and re.fullmatch(re.escape(m["ctype"]) + r" \[[^\[\]]+\]", n["ctype"])):
                    offset = m["off"] - n["off"]
                    if 0 <= offset <= n["size"] - m["size"] and offset % m["size"] == 0:
                        hits.add(f"{n['name']}[{offset // m['size']}]")
        if m["name"] in hits:
            fields[m["name"]] = m["name"]
        elif len(hits) == 1:
            fields[m["name"]] = next(iter(hits))
        else:
            fields[m["name"]] = None
    return fields


def member_edits(text, pre, refs, view, mapping, span):
    """Map typed member token ordinals back to raw text, skipping ambiguous macros."""
    changes = {name for name, new in mapping.items() if name != new}
    if not changes:
        return []
    field_token = re.compile(r"(?:->|\.)\s*([A-Za-z_]\w*)")
    code = keepcheck.blank_comments(text)
    source = code.splitlines(keepends=True)
    offsets = [0]
    for line in source:
        offsets.append(offsets[-1] + len(line))
    pre_lines = defaultdict(list)
    for (path, line), contents in zip(lm.line_files(pre), pre.split("\n")):
        if Path(path).as_posix() == view["file"] and not lm.LINEMARK.match(contents):
            pre_lines[line].append(contents)
    refs_at = {(line, col - 1, name): key for key, name, line, col in refs.fields}
    edits = []
    for ln, line in enumerate(source, 1):
        raw = list(field_token.finditer(line))
        if not any(m.group(1) in changes for m in raw):
            continue
        outside = [m for m in raw if not span.start <= offsets[ln - 1] + m.start(1) < span.end]
        if not any(m.group(1) in changes for m in outside):
            continue
        parsed = pre_lines.get(ln, [])
        if len(parsed) != 1:
            raise Holdout(f"line {ln} has a changed field in inactive C or an ambiguous macro")
        tokens = list(field_token.finditer(parsed[0]))
        if [m.group(1) for m in raw] != [m.group(1) for m in tokens]:
            raise Holdout(f"line {ln} has a changed field whose macro expansion changes the token sequence")
        for m, token in zip(raw, tokens):
            name = m.group(1)
            if name not in changes or m not in outside:
                continue
            site = ln, token.start(1), name
            if site not in refs_at or refs_at[site] is None:
                raise Holdout(f"line {ln} cannot bind field {name} to a record")
            if refs_at[site] != view["id"]:
                continue
            if mapping[name] is None:
                raise Holdout(f"line {ln} uses {name}, whose storage has no unambiguous shared member")
            start, end = m.span(1)
            edits.append((offsets[ln - 1] + start, offsets[ln - 1] + end, mapping[name]))
    # A changed use created by a macro, or a member split after '->', has no safe raw token.
    count = sum(key == view["id"] and name in changes for key, name, _, _ in refs.fields)
    if count != len(edits):
        raise Holdout("some changed member uses are only visible after preprocessing")
    return edits


def check_pointer_prefix(ast, view):
    """Restrict a reviewed short view to pointer parameters and their field uses."""
    from pycparser import c_ast

    tag = view["spell"].split()[1]
    names = set()

    def nodes(node, parents=()):
        yield node, parents
        for _, child in node.children():
            yield from nodes(child, (*parents[-3:], node))

    roots = [node for node in ast.ext if node.coord and Path(node.coord.file).as_posix() == view["file"]]
    for root in roots:
        for node, parents in nodes(root):
            if not isinstance(node, c_ast.Struct) or node.name != tag or node.decls is not None:
                continue
            if len(parents) < 4 or not all(isinstance(parent, cls) for parent, cls in zip(
                    parents[-4:], (c_ast.ParamList, (c_ast.Decl, c_ast.Typename), c_ast.PtrDecl, c_ast.TypeDecl))):
                raise Holdout("short view is used outside a direct pointer parameter")
            parameter = parents[-3]
            if parameter.name:
                names.add(parameter.name)

    for root in roots:
        for node, parents in nodes(root):
            if not isinstance(node, c_ast.ID) or node.name not in names or not parents:
                continue
            parent = parents[-1]
            if isinstance(parent, c_ast.StructRef) and (parent.field is node or parent.type == "->"):
                continue
            if isinstance(parent, c_ast.UnaryOp) and parent.op == "!":
                continue
            if (isinstance(parent, c_ast.ExprList) and len(parents) > 1
                    and isinstance(parents[-2], c_ast.FuncCall) and parents[-2].args is parent):
                continue
            raise Holdout(f"line {node.coord.line} uses a short-view pointer outside a member access, "
                          "null test, or call argument")


def rewrite_unit(text, old, new, header, spell, *, allow_prefix=False):
    """Replace one file-scope declaration and the member uses bound to it."""
    short_view = (allow_prefix and old["kind"] == new["kind"] == "struct"
                  and old["spell"].startswith("struct ")
                  and old["size"] < new["size"] and old["align"] <= new["align"])
    if not short_view and (old["size"], old["align"]) != (new["size"], new["align"]):
        raise Holdout(f"private size/alignment {old['size']:#x}/{old['align']} differs from shared "
                      f"{new['size']:#x}/{new['align']}")
    spans = [r for r in inline_twins.records(text) if r.kind == old["kind"]
             and (r.name if r.typedef else f"{r.kind} {r.name}") == old["spell"]]
    if len(spans) != 1:
        raise Holdout("declaration is not a unique editable file-scope record")
    span = spans[0]
    if keepcheck.in_ranges(span.start, keepcheck.draft_ranges(text)):
        raise Holdout("declaration belongs to a NONMATCHING draft")
    mask = keepcheck.blank_comments(text)
    tag = re.compile(rf"\b{span.kind}\s+{re.escape(span.name)}\b")
    if not span.typedef and len(re.findall(tag.pattern + r"\s*\{", mask)) != 1:
        raise Holdout("another declaration shadows the record tag")
    mapping = field_paths(old, new)
    same_sequence = [(m["name"], type_key(m)) for m in old["members"]] == [
        (m["name"], type_key(m)) for m in new["members"]]
    edits = []
    if short_view or not same_sequence or any(name != new_name for name, new_name in mapping.items()):
        pre = lm.preprocess(old["file"])
        if pre is None:
            raise Holdout("preprocessing failed")
        try:
            _, ast, tu, error = evidence.parse(pre, old["file"])
        except ParseError as exc:
            raise Holdout(f"source parse failed: {exc}") from exc
        if error:
            raise Holdout(f"body parse failed: {error}")
        if short_view:
            check_pointer_prefix(ast, old)
        if not same_sequence and has_initializers(ast, tu, old["id"], old["file"]):
            raise Holdout("aggregate initializer needs a positional layout review")
        refs = evidence.References(ast, tu, old["file"])
        edits = member_edits(text, pre, refs, old, mapping, span)
    replacement = f"typedef {spell} {span.name};\n" if span.typedef else ""
    edits.append((span.start, span.end, replacement))
    if not span.typedef and old["spell"] != spell:
        edits.extend((m.start(), m.end(), spell) for m in tag.finditer(mask)
                     if m.end() <= span.start or m.start() >= span.end)
    for start, end, replacement in sorted(edits, reverse=True):
        text = text[:start] + replacement + text[end:]
    return inline_twins.add_include(text, ramlayout.inc_name(str(header)))


def has_initializers(ast, tu, key, path):
    from pycparser import c_ast

    def contains(t, seen=frozenset()):
        t = tu.resolve(t)
        if isinstance(t, c_ast.ArrayDecl):
            return contains(t.type, seen)
        if isinstance(t, c_ast.PtrDecl):
            return False
        if evidence.record_of(tu, t) == key:
            return True
        agg = tu.aggregate(t)
        if agg and id(agg[1]) not in seen:
            return any(contains(d.type, seen | {id(agg[1])}) for d in agg[1].decls or ())
        return False

    class Initializers(c_ast.NodeVisitor):
        found = False

        def visit_Decl(self, node):
            if node.init is not None and contains(node.type):
                self.found = True
            self.generic_visit(node)

        def visit_CompoundLiteral(self, node):
            if contains(node.type.type):
                self.found = True
            self.generic_visit(node)

    visitor = Initializers()
    for node in ast.ext:
        if node.coord and Path(node.coord.file).as_posix() == path:
            visitor.visit(node)
    return visitor.found


def path_under(path, directory, suffix):
    if path.is_absolute() or ".." in path.parts or not path.is_relative_to(directory) or path.suffix != suffix:
        raise Holdout(f"expected a repository-relative {directory}/ path ending in {suffix}")
    if not (families.ROOT / path).resolve().is_relative_to(families.ROOT / directory):
        raise Holdout(f"path resolves outside {directory}/")
    return path


def shared_layout(view):
    members = [(m.get("name"), m.get("off"), m.get("base"), m.get("width"), m.get("signed"), type_key(m))
               for m in view["members"]]
    return view["kind"], view["size"], view["align"], members, view["leaves"], view["containers"]


def header_choice(catalog, family, views, args):
    existing = catalog["records"]
    if args.use_shared:
        keys = [key for key, v in existing.items() if v["file"].startswith("include/")
                and args.use_shared in {v["spell"], key}]
        if len(keys) != 1 or keys[0] not in {row[0] for row in family["shared"]}:
            raise Holdout("--use-shared needs a unique related shared spelling or catalog key")
        key = keys[0]
        view = existing[key]
        header = path_under(Path(view["file"]), "include", ".h")
        content = (families.ROOT / header).read_bytes()
        if families.digest(content) != catalog["files"][str(header)]:
            raise Holdout(f"stale catalog for {header}; rescan the tree")
        result = families.scan_file(str(header), content.decode(errors="replace"))
        fresh = result["records"].get(key)
        if not fresh:
            detail = result.get("error", f"no measured definition for {view['spell']}")
            raise Holdout(f"cannot remeasure {header}: {detail}")
        if shared_layout(view) != shared_layout(fresh):
            raise Holdout(f"shared layout changed for {view['spell']} in {header}; rescan the tree")
        fresh["known"] = families.inhabited(fresh)
        return header, view["spell"], None, fresh, [], []

    used_tags = {families.short(v) for v in existing.values() if v["file"].startswith("include/")}
    default_name = family["name"] if family["name"] not in used_tags else family["name"] + "Recovered"
    name = args.name or default_name
    if not re.fullmatch(r"[A-Za-z_]\w*", name, re.ASCII) or name in used_tags:
        raise Holdout("choose a C tag that is unused in shared headers")
    snake = re.sub(r"(?<=[a-z0-9])(?=[A-Z])", "_", name).lower()
    header = path_under(args.header or Path(f"include/recovered/{snake}.h"), "include", ".h")
    text, expected, decisions, skipped = plan_header(family, views, name, header)
    spell = f"{family['kind']} {name}"
    new = probe_header(text, spell, expected, family["size"], max(v["align"] for v in views))
    return header, spell, text, new, decisions, skipped


def convert(catalog, family, args):
    views = [catalog["records"][key] for key in family["records"]]
    header, spell, header_text, new, decisions, skipped = header_choice(catalog, family, views, args)
    print(RULE)
    print(f"{spell} in {header}, size {new['size']:#x}, alignment {new['align']}")
    for line in decisions + skipped:
        print(f"  {line}")
    if header_text and not args.apply:
        print(f"\n{header_text}", end="")
    create = header_text is not None and not header.exists()
    if header_text is not None and not create and header.read_text() != header_text:
        raise Holdout(f"{header} already has different contents")
    by_path = defaultdict(list)
    for view in views:
        by_path[view["file"]].append(view)
    proposals = {}
    for rel, items in sorted(by_path.items()):
        try:
            path = path_under(Path(rel), "src", ".c")
            text = path.read_text()
            if families.digest(text.encode()) != catalog["files"][rel]:
                raise Holdout("source changed since scan; rescan the tree")
            if len(items) != 1:
                raise Holdout("multiple family declarations in one unit need joint scope review")
            proposals[rel] = rewrite_unit(text, items[0], new, header, spell,
                                          allow_prefix=rel in args.allow_prefix)
            if not args.apply:
                print(f"PROPOSED {rel}")
        except (Holdout, OSError) as exc:
            print(f"SKIPPED {rel} · {exc}")
    if not args.apply:
        print(f"{len(proposals)} of {len(by_path)} units have a source proposal")
        return 0 if proposals else 1
    if not proposals:
        print(f"0 of {len(by_path)} units converted")
        return 1

    kept = {}
    complete = False
    created_here = False
    try:
        if create:
            header.parent.mkdir(parents=True, exist_ok=True)
            with header.open("x") as out:
                created_here = True
                out.write(header_text)
        for rel, text in proposals.items():
            unit = keepcheck.Unit(rel)
            old = unit.text
            if families.digest(old.encode()) != catalog["files"][rel]:
                print(f"SKIPPED {rel} · source changed while planning")
                continue
            try:
                if unit.attempt(text):
                    kept[rel] = old
                    unit.finish()
                    print(f"MATCH {rel}")
                else:
                    print(f"REJECTED {rel} · {unit.last}")
            except BaseException:
                Path(rel).write_text(old)
                raise
        if kept and created_here:
            result = subprocess.run([sys.executable, "tools/header_dependents.py", str(header), "--check", "-j", str(args.j)])
            if result.returncode:
                raise Holdout("header-dependent check failed; restored all candidates")
        complete = bool(kept)
    finally:
        if not complete:
            for rel, old in kept.items():
                Path(rel).write_text(old)
            if created_here:
                header.unlink()
    print(f"{len(kept)} of {len(by_path)} units converted")
    return 0 if kept else 1


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("family", help="catalog ID or unique dominant tag")
    ap.add_argument("--catalog", type=Path, default=families.OUT / "families.json")
    ap.add_argument("--apply", action="store_true", help="write the header and keep only exact unit matches")
    ap.add_argument("--name", help="new shared tag")
    ap.add_argument("--header", type=Path, help="new header path below include/")
    ap.add_argument("--use-shared", help="reuse a related existing type instead of generating a header")
    ap.add_argument("--allow-prefix", action="append", default=[], metavar="SRC",
                    help="allow this reviewed unit's short pointer view of the same allocation")
    ap.add_argument("-j", type=int, default=2, help="parallel header-dependent checks")
    args = ap.parse_args(argv)
    if args.j < 1 or args.use_shared and (args.name or args.header):
        ap.error("select positive jobs and either a new header or --use-shared")
    os.chdir(families.ROOT)
    try:
        catalog = families.load_catalog(args.catalog)
        family = families.select_family(catalog, args.family)
        return convert(catalog, family, args)
    except (OSError, ValueError) as exc:
        ap.exit(1, f"structmerge: {exc}\n")


if __name__ == "__main__":
    sys.exit(main())
