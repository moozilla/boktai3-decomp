#!/usr/bin/env python3
"""Preview or apply one family from structfamilies.py, gated by byte-exact checks."""

from __future__ import annotations

import argparse
from collections import Counter, defaultdict
from dataclasses import dataclass
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
            if families.opaque(m) or m.get("bits") and m["name"] is None:
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
            eligible = [(v, m) for v, m in supporters if not m["private"] and not VOLATILE_TYPE.search(m["decl"])]
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


@dataclass
class RecordPlan:
    spell: str
    size: int
    align: int
    lines: list
    expected: dict
    decisions: list
    skipped: list
    headers: set
    forwards: set
    embedded: set


def plan_record(family, views, name):
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
    spell = f"{kind} {name}"
    headers = {h for m, _, _, _ in selected for h in m["headers"]}
    forwards = {tag for m, _, _, _ in selected for tag in m.get("forwards", ())}
    embedded = {m["embedded"] for m, _, _, _ in selected if "embedded" in m}
    buf = [f"{spell} {{"]
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
    align = max(v["align"] for v in views)
    member_align = max((m.get("align") or m.get("width") or 1 for m, _, _, _ in selected), default=1)
    close = f"}} __attribute__((aligned({align})));" if align > member_align else "};"
    buf.extend((close, "", f"typedef char {name}SizeCheck[sizeof({spell}) == {family['size']:#x} ? 1 : -1];"))
    return RecordPlan(spell, family["size"], align, buf, expected, decisions, skipped, headers, forwards, embedded)


def render_header(header, plans):
    guard = "GUARD_" + re.sub(r"[^A-Za-z0-9]", "_", str(header)).upper()
    headers = set().union(*(p.headers for p in plans)) - {"include/global.h", "include/gba/types.h"}
    forwards = set().union(*(p.forwards for p in plans))
    buf = [f"#ifndef {guard}", f"#define {guard}", "", '#include "global.h"']
    buf.extend(f'#include "{ramlayout.inc_name(h)}"' for h in sorted(headers))
    if forwards:
        buf.extend(("", *(tag + ";" for tag in sorted(forwards))))
    for plan in plans:
        buf.extend(("", *plan.lines))
    return "\n".join([*buf, "", "#endif", ""])


def plan_header(family, views, name, header):
    plan = plan_record(family, views, name)
    return render_header(header, [plan]), plan.expected, plan.decisions, plan.skipped


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
    """Find direct and qualified member paths at the same measured storage."""
    fields = {}
    old_bits, new_bits = bit_spans(old), bit_spans(new)
    choices = new["members"] + [dict(lf, name=lf["name"], base=lf["base"])
                               for lf in new["known"] if "." in lf["name"]]
    nested = [lf for lf in old["known"] if "." in lf["name"] and "[" not in lf["name"]]
    for m in old["members"] + nested:
        if m.get("off") is None or m.get("size") is None:
            continue
        whole_bytes = (m["off"] * 8, m["size"] * 8)
        occupied_bits = old_bits.get(m["name"], whole_bytes)
        hits = {n["name"] for n in choices if n.get("off") == m["off"] and n.get("size") == m["size"]
                and n.get("array") == m["array"] and n.get("esize") == m.get("esize")
                and (n.get("base") == m["base"] or {m["base"], n.get("base")} <= {"int", "enum"})
                and occupied_bits == new_bits.get(n["name"], whole_bytes)}
        if m["name"] in hits:
            fields[m["name"]] = m["name"]
        elif len(hits) == 1:
            fields[m["name"]] = next(iter(hits))
        elif "." not in m["name"]:
            fields[m["name"]] = None
    return fields


def member_edits(text, pre, refs, view, mapping, span):
    """Map typed member token ordinals back to raw text, skipping ambiguous macros."""
    changes = {name for name, new in mapping.items() if "." not in name and name != new}
    chains = {}
    for key, chain in refs.field_chains:
        if key != view["id"]:
            continue
        names = [name for name, _, _ in chain]
        target = mapping.get(".".join(names))
        if not target:
            continue
        root = mapping.get(names[0])
        if root is not None and target == ".".join([root, *names[1:]]):
            continue  # The ordinary direct-member edit is sufficient.
        site = chain[0]
        if site not in chains or len(chain) > len(chains[site][0]):
            chains[site] = (chain, target)
    if any(len({line for _, line, _ in chain}) != 1 for chain, _ in chains.values()):
        raise Holdout("a changed nested field crosses source lines")
    chain_names = {name for name, _, _ in chains}
    if not changes and not chains:
        return []
    field_token = re.compile(r"(->|\.)\s*([A-Za-z_]\w*)")
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
    consumed = set()
    for ln, line in enumerate(source, 1):
        raw = list(field_token.finditer(line))
        if not any(m.group(2) in changes | chain_names for m in raw):
            continue
        base = offsets[ln - 1]
        outside = [m for m in raw if not span.start <= base + m.start(2) < span.end]
        if not any(m.group(2) in changes | chain_names for m in outside):
            continue
        parsed = pre_lines.get(ln, [])
        if len(parsed) != 1:
            raise Holdout(f"line {ln} has a changed field in inactive C or an ambiguous macro")
        tokens = list(field_token.finditer(parsed[0]))
        if [m.groups() for m in raw] != [m.groups() for m in tokens]:
            raise Holdout(f"line {ln} has a changed field whose macro expansion changes the token sequence")
        at = {(token.group(2), ln, token.start(2) + 1): m for m, token in zip(raw, tokens)}
        for m, token in zip(raw, tokens):
            name = m.group(2)
            coord = name, ln, token.start(2) + 1
            if coord in consumed:
                continue
            if coord in chains:
                chain, target = chains[coord]
                last = at.get(chain[-1])
                if not all(site in at for site in chain) or last is None:
                    raise Holdout(f"line {ln} cannot locate a nested field in the unexpanded source")
                start, end = base + m.start(2), base + last.end(2)
                pattern = r"\s*\.\s*".join(re.escape(field) for field, _, _ in chain)
                if not re.fullmatch(pattern, text[start:end]):
                    raise Holdout(f"line {ln} has a nested field interrupted by other source tokens")
                edits.append((start, end, target))
                consumed.update(chain)
                continue
            if name not in changes or m not in outside:
                continue
            site = ln, token.start(2), name
            if site not in refs_at or refs_at[site] is None:
                raise Holdout(f"line {ln} cannot bind field {name} to a record")
            if refs_at[site] != view["id"]:
                continue
            if mapping[name] is None:
                raise Holdout(f"line {ln} uses {name}, whose storage has no unambiguous shared member")
            start, end = m.span(2)
            edits.append((base + start, base + end, mapping[name]))
            consumed.add(coord)
    # A changed use created by a macro, or a member split after '->', has no safe raw token.
    if not chains.keys() <= consumed or any(key == view["id"] and name in changes and (name, line, col) not in consumed
                                          for key, name, line, col in refs.fields):
        raise Holdout("some changed member uses are only visible after preprocessing")
    return edits


def rewrite_unit(text, old, new, header, spell):
    return rewrite_records(text, [(old, new, spell)], header)


def rewrite_records(text, targets, header, *, allow_prefix=False):
    """Plan each record's edits against the original translation unit."""
    spans = inline_twins.records(text)
    drafts = keepcheck.draft_ranges(text)
    mask = keepcheck.blank_comments(text)
    edits = []
    removed = []
    renames = []
    refs = None
    for old, new, spell in targets:
        prefix = old["size"] < new["size"] and old["kind"] == new["kind"] == "struct"
        if old["align"] != new["align"] or (old["size"] != new["size"] and not (allow_prefix and prefix)):
            raise Holdout(f"{old['spell']}: private size/alignment {old['size']:#x}/{old['align']} differs "
                          f"from shared {new['size']:#x}/{new['align']}")
        found = [r for r in spans if r.spell == old["spell"]]
        if len(found) != 1:
            raise Holdout(f"{old['spell']}: declaration is not a unique editable file-scope record")
        span = found[0]
        if keepcheck.in_ranges(span.start, drafts):
            raise Holdout(f"{old['spell']}: declaration belongs to a NONMATCHING draft")
        if span.tag:
            tag = re.compile(rf"\b{span.kind}\s+{re.escape(span.tag)}\b")
            if len(re.findall(tag.pattern + r"\s*\{", mask)) != 1:
                raise Holdout(f"another declaration shadows {old['spell']}")
            if old["spell"] != spell:
                renames.append((tag, spell))
        mapping = field_paths(old, new)
        same_sequence = [(m["name"], type_key(m)) for m in old["members"]] == [
            (m["name"], type_key(m)) for m in new["members"]]
        if prefix or not same_sequence or any(name != new_name for name, new_name in mapping.items()):
            if refs is None:
                pre = lm.preprocess(old["file"])
                if pre is None:
                    raise Holdout("preprocessing failed")
                try:
                    _, ast, tu, error = evidence.parse(pre, old["file"])
                except ParseError as exc:
                    raise Holdout(f"source parse failed: {exc}") from exc
                if error:
                    raise Holdout(f"body parse failed: {error}")
                refs = evidence.References(ast, tu, old["file"])
            if prefix:
                if re.search(r"^\s*#\s*(?:if|ifdef|ifndef)\b", mask, re.M):
                    raise Holdout(f"{old['spell']}: source conditionals require a pointer-view review")
                if old["id"] in refs.extent_uses:
                    line, operation = refs.extent_uses[old["id"]]
                    raise Holdout(f"{old['spell']}: line {line} uses the private extent in {operation}")
            if not same_sequence and has_initializers(ast, tu, old["id"], old["file"]):
                raise Holdout(f"{old['spell']}: aggregate initializer needs a positional layout review")
            edits.extend(member_edits(text, pre, refs, old, mapping, span))
        replacement = f"typedef {spell} {span.name};\n" if span.typedef else ""
        removed.append((span.start, span.end, replacement))

    for tag, spell in renames:
        edits.extend((m.start(), m.end(), spell) for m in tag.finditer(mask))
    edits = removed + [e for e in edits if all(e[1] <= start or end <= e[0] for start, end, _ in removed)]
    previous = len(text)
    for start, end, replacement in sorted(set(edits), reverse=True):
        if end > previous:
            raise Holdout("joint record changes need overlapping source edits")
        text = text[:start] + replacement + text[end:]
        previous = start
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


def embedded_members(views, records, used_tags, owner):
    """Lift measured, equivalent private children repeated at an embedded slot."""
    groups = defaultdict(list)
    for parent in views:
        for member in parent["members"]:
            child = records.get(member.get("record"))
            if (member.get("base") != "aggregate" or member.get("off") is None or not child
                    or child["file"] != parent["file"]):
                continue
            spell = child["spell"]
            if not spell.startswith(("struct ", "union ")) or member.get("private") != [spell]:
                continue
            if any(m.get("private") or not m.get("name") or m.get("off") is None or VOLATILE_TYPE.search(m["decl"])
                   for m in child["members"]):
                continue
            fields = tuple((m["name"], m["off"], type_key(m)) for m in child["members"])
            signature = spell, child["size"], child["align"], fields
            groups[signature].append((parent, member, child))

    replacements = {}
    plans = {}
    for signature, uses in sorted(groups.items(), key=lambda row: repr(row[0])):
        slots = defaultdict(set)
        for parent, member, _ in uses:
            slots[member["off"], member["name"], member["size"], member["array"]].add(parent["file"])
        if max(map(len, slots.values())) < 2:
            continue
        children = {child["id"]: child for _, _, child in uses}
        spell, size, _, _ = signature
        kind, tag = spell.split()
        name = available_name(tag if tag not in used_tags and len(tag) >= 4 else owner + tag, used_tags)
        child_family = {"kind": kind, "size": size, "loads": []}
        plan = plan_record(child_family, list(children.values()), name)
        plans[plan.spell] = plan, children
        used_tags.add(name)
        for parent, member, _ in uses:
            pattern = rf"\b{re.escape(kind)}\s+{re.escape(tag)}\b"
            replacements[parent["id"], member["name"]] = dict(
                member, decl=re.sub(pattern, plan.spell, member["decl"], count=1),
                ctype=re.sub(pattern, plan.spell, member["ctype"], count=1),
                private=[], embedded=plan.spell)
    return [dict(view, members=[replacements.get((view["id"], m["name"]), m) for m in view["members"]])
            for view in views], plans


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
    parents, embedded = embedded_members(views, existing, used_tags | {name}, name)
    parent = plan_record(family, parents, name)
    children = [embedded[child] for child in sorted(parent.embedded)]
    text = render_header(header, [*(plan for plan, _ in children), parent])
    new = probe_header(text, parent.spell, parent.expected, parent.size, parent.align)
    new["embedded"] = {}
    decisions = []
    for plan, originals in children:
        measured = probe_header(text, plan.spell, plan.expected, plan.size, plan.align)
        for key in originals:
            new["embedded"][key] = measured
        decisions.append(f"embedded {plan.spell}, size {plan.size:#x}, alignment {plan.align}")
    return header, parent.spell, text, new, decisions + parent.decisions, parent.skipped


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
            targets = [(item, new, spell) for item in items]
            targets.extend((catalog["records"][key], child, child["spell"])
                           for key, child in new.get("embedded", {}).items()
                           if catalog["records"][key]["file"] == rel)
            proposals[rel] = rewrite_records(text, targets, header, allow_prefix=getattr(args, "allow_prefix", False))
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
    ap.add_argument("--allow-prefix", action="store_true",
                    help="adopt a larger existing record for a pointer view with no size-dependent uses")
    ap.add_argument("-j", type=int, default=2, help="parallel header-dependent checks")
    args = ap.parse_args(argv)
    if args.j < 1 or args.use_shared and (args.name or args.header):
        ap.error("select positive jobs and either a new header or --use-shared")
    if args.allow_prefix and not args.use_shared:
        ap.error("--allow-prefix requires --use-shared")
    os.chdir(families.ROOT)
    try:
        catalog = families.load_catalog(args.catalog)
        family = families.select_family(catalog, args.family)
        return convert(catalog, family, args)
    except (OSError, ValueError) as exc:
        ap.exit(1, f"structmerge: {exc}\n")


if __name__ == "__main__":
    sys.exit(main())
