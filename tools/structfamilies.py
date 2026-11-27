#!/usr/bin/env python3
"""Find related private C records and embedded objects. Writes only the report and cache."""

from __future__ import annotations

import argparse
from bisect import bisect_right
from collections import Counter, defaultdict
from concurrent.futures import ProcessPoolExecutor, ThreadPoolExecutor
import csv
import hashlib
import inspect
from itertools import combinations
import json
import os
from pathlib import Path
import re
import sys
import tempfile

import concessions
import inline_twins
import keepcheck
import layoutmerge as lm
import progress
import ramlayout
import rawfield
import struct_evidence as evidence
import struct_views
from pycparser import c_ast

ROOT = Path(keepcheck.ROOT)
OUT = Path("build/structfamilies")
SCHEMA = 1
POINTER_CAST = r"\([^;{}()]*\*\s*(?:const\s*)?\)"
BYTE_CAST = r"\(\s*(?:const\s+)?(?:u8|s8|(?:unsigned\s+|signed\s+)?char)\s*\*\s*(?:const\s*)?\)"
ADDRESS_CANDIDATE = re.compile(
    rf"[(,]\s*(?:{POINTER_CAST}\s*){{0,2}}(?:\(\s*)*"
    rf"(?:&[^;,{{}}]{{0,200}}->|{BYTE_CAST}[^;,{{}}]{{0,150}}[+-])")
RECORD_RETURN = re.compile(r"\b(?:struct|union)\s+\w+\s*\*\s*(?:const\s+)?\w+\s*\(")


def digest(data):
    return hashlib.sha256(data).hexdigest()


def json_write(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(mode="w", dir=path.parent, delete=False) as file:
        json.dump(data, file, sort_keys=True, separators=(",", ":"))
    os.replace(file.name, path)


def scan_file(rel, text):
    """One probe per file, with declarations-only fallback after a body parse error."""
    path = rel
    if rel.startswith("include/"):
        with tempfile.NamedTemporaryFile(mode="w", suffix=".c", dir=ROOT / "build") as file:
            file.write(f'#include "global.h"\n#include "{rel}"\n')
            file.flush()
            pre = lm.preprocess(file.name, nonmatching=False)
    else:
        pre = lm.preprocess(path, nonmatching=False)
    result = {"file": rel, "digest": digest(text.encode()), "records": {}}
    if pre is None:
        return {**result, "error": "preprocess failed"}
    try:
        body, ast, tu, error = evidence.parse(pre, path, declarations_only=rel.startswith("include/"))
    except lm.pycparser.c_parser.ParseError as exc:
        # pycparser can reject GNU syntax even after bodies are removed.
        return {**result, "error": str(exc)[:250]}
    selected = evidence.selected_records(tu, path)
    measured = lm.probe_records(body, tu, selected)
    for key, view in measured.items():
        spell, node = selected[key]
        for member, decl in zip(view["members"], node.decls):
            t = tu.resolve(decl.type)
            elem = t.type if isinstance(t, c_ast.ArrayDecl) else decl.type
            member["record"] = evidence.record_of(tu, elem)
            headers, private, forwards, self_ref = evidence.member_dependencies(tu, decl, spell)
            member.update(headers=headers, private=private, forwards=forwards, self_ref=self_ref)
        view.update(file=rel, line=node.coord.line)
    result["records"] = measured
    result["unmeasured"] = sorted(set(selected) - set(measured))
    if error:
        result["error"] = error[:250]
    if rel.startswith("src/") and not error:
        refs = evidence.References(ast, tu, path)
        result.update(calls=refs.calls, params=refs.params, globals=refs.globals,
                      return_definitions=refs.return_definitions, return_calls=refs.return_calls)
    return result


def cache_path(rel, text, version):
    key = digest(rel.encode() + b"\0" + version + b"\0" + text.encode())
    return ROOT / OUT / "cache" / key[:2] / f"{key}.json"


def scan_cached(job):
    rel, text, version = job
    path = cache_path(rel, text, version)
    if path.is_file():
        return json.loads(path.read_text())
    result = scan_file(rel, text)
    json_write(path, result)
    return result


def scan_header_batch(jobs):
    """Probe compatible shared headers together. Retry individual headers when needed."""
    cached, missing = {}, []
    for rel, text, version in jobs:
        cache = cache_path(rel, text, version)
        if cache.exists():
            cached[rel] = json.loads(cache.read_text())
        else:
            missing.append((rel, text, version))
    if len(missing) < 2:
        return [cached.get(job[0]) or scan_cached(job) for job in jobs]

    with tempfile.NamedTemporaryFile(mode="w", suffix=".c", dir=ROOT / "build") as file:
        file.write('#include "global.h"\n' + "".join(f'#include "{rel}"\n' for rel, _, _ in missing))
        file.flush()
        pre = lm.preprocess(file.name, nonmatching=False)
    if pre:
        try:
            body, _, tu, _ = evidence.parse(pre, file.name, declarations_only=True)
        except lm.pycparser.c_parser.ParseError:
            pre = None
    if not pre:
        mid = len(missing) // 2
        rows = scan_header_batch(missing[:mid]) + scan_header_batch(missing[mid:])
        cached.update((row["file"], row) for row in rows)
        return [cached[job[0]] for job in jobs]

    selections = {rel: evidence.selected_records(tu, rel) for rel, _, _ in missing}
    combined = {key: node for selected in selections.values() for key, node in selected.items()}
    measured = lm.probe_records(body, tu, combined)
    if not measured:
        mid = len(missing) // 2
        rows = scan_header_batch(missing[:mid]) + scan_header_batch(missing[mid:])
        cached.update((row["file"], row) for row in rows)
        return [cached[job[0]] for job in jobs]
    for job in missing:
        rel, text, version = job
        selected = selections[rel]
        expected = {f"{rel}:{r.spell}" for r in inline_twins.records(text)}
        if not selected or not (selected.keys() | expected) <= measured.keys():
            cached[rel] = scan_cached(job)
            continue
        for key, (spell, node) in selected.items():
            view = measured[key]
            for member, decl in zip(view["members"], node.decls):
                t = tu.resolve(decl.type)
                member["record"] = evidence.record_of(tu, t.type if isinstance(t, c_ast.ArrayDecl) else decl.type)
            view.update(file=rel, line=node.coord.line)
        row = {"file": rel, "digest": digest(text.encode()), "records": {k: measured[k] for k in selected}}
        json_write(cache_path(rel, text, version), row)
        cached[rel] = row
    return [cached[job[0]] for job in jobs]


def shared_scan_inputs(headers, requested):
    if not requested:
        return [(rel, text) for rel, text in sorted(headers.items())
                if rel.startswith("include/") and inline_twins.RECORD.search(text)]
    paths = set()
    root = ROOT.resolve()
    for name in requested:
        path = (root / name).resolve()
        if not path.is_relative_to(root / "include") or path.suffix != ".h":
            raise ValueError(f"--shared-header must name a file below include/: {name}")
        rel = path.relative_to(root).as_posix()
        if rel not in headers:
            raise ValueError(f"shared header not found: {rel}")
        paths.add(rel)
    return [(rel, headers[rel]) for rel in sorted(paths)]


def scan_tree(paths, jobs, shared_headers=None):
    root = ROOT
    headers = {str(path.relative_to(root)): path.read_text(errors="replace")
               for area in ("include", "src") for path in (root / area).rglob("*.h")}
    shared = shared_scan_inputs(headers, shared_headers)
    candidates = []
    for rel in keepcheck.src_files(paths):
        src = (root / rel).read_text(errors="replace")
        code = keepcheck.blank_comments(src)
        if inline_twins.RECORD.search(code) or ADDRESS_CANDIDATE.search(code) or RECORD_RETURN.search(code):
            candidates.append((rel, src))
    units = "unit" if len(candidates) == 1 else "units"
    header = "header" if len(shared) == 1 else "headers"
    print(f"Scanning {len(candidates)} source {units} and {len(shared)} shared {header}", file=sys.stderr, flush=True)
    stat = Path(lm.AGBCC).stat()
    key = hashlib.sha256(f"{stat.st_mtime_ns}:{stat.st_size}".encode())
    key.update(inspect.getsource(scan_file).encode())
    for path in ("struct_evidence.py", "layoutmerge.py"):
        key.update((root / "tools" / path).read_bytes())
    for rel, text in sorted(headers.items()):
        key.update(rel.encode())
        key.update(text.encode())
    results = []
    header_version = key.copy()
    header_version.update(inspect.getsource(scan_header_batch).encode())
    with ProcessPoolExecutor(jobs) as executor:
        tasks = ((rel, text, key.digest()) for rel, text in candidates)
        for result in executor.map(scan_cached, tasks, chunksize=8):
            results.append(result)
            if len(results) % 500 == 0:
                print(f"Scanned {len(results)} of {len(candidates)} units", file=sys.stderr, flush=True)
        tasks = [(rel, text, header_version.digest()) for rel, text in shared]
        batches = (tasks[i:i + 12] for i in range(0, len(tasks), 12))
        for batch in executor.map(scan_header_batch, batches):
            results.extend(batch)
    return results


def opaque(member):
    name = member.get("name")
    return (name and member.get("array") and member.get("esize") == 1
            and (struct_views.is_pad(name) or rawfield.padded(name + "[0]") or lm.OPAQUE.match(name)))


def inhabited(view):
    """Flattened typed storage by offset, excluding opaque byte padding."""
    leaves = view["leaves"]
    pads = {m["name"] for m in view["members"] if opaque(m)}
    return [leaf for leaf in leaves + rawfield.element_leaves(leaves, view["containers"])
            if leaf.get("off") is not None and leaf.get("size")
            and re.split(r"[.\[]", leaf["name"], maxsplit=1)[0] not in pads
            and not rawfield.padded(leaf["name"] + ("[0]" if leaf["array"] and leaf.get("esize") == 1 else ""))]


def leaf_shape(leaf):
    return (leaf["size"], leaf.get("esize"), leaf["base"], leaf["bits"], leaf.get("bitpos"))


def layout_stats(a, b):
    """Comparable starts, spelling agreements, and disjoint storage boundaries."""
    small = min(a["size"], b["size"])
    ax, bx = (v["_offsets"] if v["size"] == small and "_offsets" in v else
              {leaf["off"]: leaf for leaf in v["known"] if leaf["off"] < small} for v in (a, b))
    common = ax.keys() & bx.keys()
    exact = {off for off in common if leaf_shape(ax[off]) == leaf_shape(bx[off])}
    names = {off for off in exact if ax[off]["name"].rsplit(".", 1)[-1] == bx[off]["name"].rsplit(".", 1)[-1]
             and not struct_views.is_pad(ax[off]["name"].rsplit(".", 1)[-1])}
    return exact, names, common, ax, bx


def compatible(a, b, *, strong=False):
    if a["kind"] != b["kind"]:
        return False
    exact, _, common, ax, bx = layout_stats(a, b)
    if exact:
        return len(common - exact) <= len(exact) * (3 if strong else 1)
    if strong and (not ax or not bx):
        return a["size"] == b["size"] or (not ax if a["size"] < b["size"] else not bx)
    if strong and a["size"] == b["size"] and ax and bx:
        return not common or any(leaf["off"] + leaf["size"] <= a["size"]
                                 for leaf in ax.values()) and len(common) == 1
    return False


def short(view):
    return view["spell"].split()[-1]


def meaningful(view):
    return len(short(view)) > 2 and not re.fullmatch(
        r"(?:Local|Private|Object|Actor|Entity|Task|Work|State|Data|Record|Obj|Context|Ctx|Temp|Main|T|S|A|B)[0-9]*",
        short(view), re.I)


def report_groups():
    with (ROOT / "symbols/progress_subsystems.csv").open(newline="") as f:
        spans = [(int(r["start"], 16), int(r["end"], 16), r["name"]) for r in csv.DictReader(f)]
    starts = [a for a, _, _ in spans]
    funcs, _ = progress.load_functions()
    out = {}
    for name, addr in funcs:
        i = bisect_right(starts, addr) - 1
        if i >= 0 and addr < spans[i][1]:
            out[name] = spans[i][2]
    return out


def address_offset(origin, records):
    """Translate a typed lvalue path to bytes. Pointer members are never traversed."""
    key, steps, delta = origin
    view = records.get(key)
    total = 0
    i = 0
    final = view["size"] if view else 0
    while view and i < len(steps):
        member = next((m for m in view["members"] if m["name"] == steps[i] and not m.get("bits")), None)
        if not member or member.get("off") is None or not member.get("size"):
            return None
        total += member["off"]
        final = member["size"]
        i += 1
        if i < len(steps) and isinstance(steps[i], int):
            elem = member.get("esize")
            if not elem or not 0 <= steps[i] < final // elem:
                return None
            total += steps[i] * elem
            final = elem
            i += 1
        if i < len(steps) and (member["base"] != "aggregate" or
                               member["array"] and not isinstance(steps[i - 1], int)):
            return None
        view = records.get(member["record"])
    if i != len(steps):
        return None
    return total + delta, view["id"] if view else None


def flow(scans, records, groups):
    private = {k for k, v in records.items() if v["file"].startswith("src/")}
    buckets = defaultdict(set)
    edges = defaultdict(set)
    embeds = defaultdict(set)
    params = defaultdict(set)
    rom_params = defaultdict(dict)
    ram_names = {n: r["addr"] for r in ramlayout.load() for n in ramlayout.names_of(r)}
    for scan in scans:
        path = scan["file"]
        for fn, i, rec, reg, static in scan.get("params", ()):
            params[(path if static else "", fn, i)].add(rec)
            if rec not in private:
                continue
            if fn in groups and not static:
                buckets[("unit", groups[fn])].add(rec)
            if reg and not static:
                rom_params[fn][f"entry:r{i}"] = rec
        for name, direct, pointee in scan.get("globals", ()):
            for space, rec in (("object", direct), ("pointee", pointee)):
                if rec in private and name in ram_names:
                    buckets[("RAM", ram_names[name], space)].add(rec)
    for scan in scans:
        for caller, line, fn, i, static, origin, declared, cast in scan.get("calls", ()):
            key, steps, delta = origin
            if key not in records:
                continue
            loc = scan["file"] + f":{line}"
            slot = (scan["file"] if static else "", fn, i)
            defined = params.get(slot, set())
            if len(defined) != 1:
                defined = set()
            offset_address = bool(steps or delta)
            if offset_address or cast and cast != key:
                position = address_offset(origin, records)
                targets = defined or {declared}
                if position:
                    off, native = position
                    for dest in targets:
                        # A concrete incompatible formal conflicts with the cast.
                        if dest in records and (not cast or cast == dest) and dest != key and off >= 0:
                            if off + records[dest]["size"] <= records[key]["size"]:
                                embeds[(key, off, dest, native == dest and delta == 0)].add((loc, caller, fn, i))
            if offset_address:
                continue
            if key in private:
                buckets[("parameter", *slot)].add(key)
                if declared and declared == (next(iter(defined)) if defined else declared):
                    buckets[("formal type", declared)].add(key)
            for dest in defined | ({declared} if declared else set()):
                if dest != key and key in private and dest in private:
                    pair = tuple(sorted((key, dest)))
                    edges[pair].add(("flow", f"{loc} {caller} → {fn} argument {i + 1}"))
                if dest in records and key in private and dest not in private:
                    buckets[("formal type", dest)].add(key)
    return private, buckets, edges, embeds, rom_params


def strong_pair(a, b, proof):
    return any(kind in {"RAM", "flow", "formal type"} for kind, _ in proof) or (
        short(a) == short(b) and meaningful(a))


def returned_views(scans, records):
    """Compare the declared view at a call with the callee's measured return type."""
    definitions = defaultdict(set)
    sites = defaultdict(set)
    for scan in scans:
        for fn, static, result in scan.get("return_definitions", ()):
            definitions[scan["file"] if static else "", fn].add(result)
    for scan in scans:
        for caller, line, fn, static, view in scan.get("return_calls", ()):
            if view not in records:
                continue
            targets = definitions[scan["file"] if static else "", fn]
            if len(targets) != 1:
                continue
            result = next(iter(targets))
            if result != view and result in records:
                sites[view, result, fn].add((f"{scan['file']}:{line}", caller))
    return [{"view": view, "result": result, "function": fn, "sites": sorted(calls)}
            for (view, result, fn), calls in sorted(sites.items())]


def add_member_edges(private, records, edges):
    """Use corresponding by-value members of a supported parent pair as evidence."""
    slots = {}

    def members(key):
        if key not in slots:
            slots[key] = {(m["name"], m["off"], m["size"], m["array"], m.get("esize")): m["record"]
                          for m in records[key].get("members", ())
                          if m.get("base") == "aggregate" and m.get("record") in private
                          and m.get("off") is not None and m.get("size")
                          and not lm.OPAQUE.match(m["name"])}
        return slots[key]

    pending = list(sorted(edges))
    seen = set(pending)
    while pending:
        a, b = pending.pop()
        left, right = members(a), members(b)
        if not left or not right or not compatible(records[a], records[b],
                                                   strong=strong_pair(records[a], records[b], edges[a, b])):
            continue
        pairs = [(slot, slot) for slot in sorted(left.keys() & right.keys())]
        if any(kind in {"flow", "RAM"} for kind, _ in edges[a, b]):
            by_span = defaultdict(list)
            for right_slot in right:
                by_span[right_slot[1:]].append(right_slot)
            for left_slot in sorted(left.keys() - right.keys()):
                alternatives = by_span[left_slot[1:]]
                if len(alternatives) != 1 or alternatives[0] in left:
                    continue
                right_slot = alternatives[0]
                exact, names, common, _, _ = layout_stats(records[left[left_slot]], records[right[right_slot]])
                if len(names) >= 2 and len(common - exact) <= 1:
                    pairs.append((left_slot, right_slot))
        for slot, other in pairs:
            x, y = sorted((left[slot], right[other]))
            if x == y or records[x]["file"] == records[y]["file"] or not compatible(records[x], records[y]):
                continue
            name, off, *_ = slot
            pair = (x, y)
            label = name if slot == other else f"{name} / {other[0]}"
            edges[pair].add(("member", f"{a} and {b} both embed {label} at {off:#x}"))
            if pair not in seen:
                seen.add(pair)
                pending.append(pair)


def family_components(private, records, buckets, edges):
    """Merge supported pairs, keeping every pair in a component layout-compatible."""
    for anchor, keys in buckets.items():
        kind, *context = anchor
        if kind == "parameter" and len(keys) > 64:
            continue  # ubiquitous allocators, bulk copies, and byte/word helpers
        for a, b in combinations(sorted(keys), 2):
            x, y = records[a], records[b]
            exact, names, _, _, _ = layout_stats(x, y)
            same_tag = short(x) == short(y) and meaningful(x)
            same_size = x["size"] == y["size"]
            distinctive = len(exact) >= 2 and bool(names) or len(exact) >= 4
            if kind in {"unit", "parameter"} and not (same_tag and (exact or same_size)
                                                        or same_size and distinctive):
                continue
            if kind == "formal type" and not (same_tag or distinctive):
                continue
            if compatible(x, y, strong=kind in {"RAM", "formal type"} or same_tag):
                label = " / ".join(s for s in context if isinstance(s, str) and s)
                if kind == "parameter":
                    label += f" argument {context[-1] + 1}"
                edges[a, b].add((kind, label))

    add_member_edges(private, records, edges)

    def weight(item):
        pair, proof = item
        priorities = {"flow": 16, "RAM": 12, "formal type": 6, "parameter": 3, "member": 2, "unit": 1}
        strength = sum(priorities[k] for k in {k for k, _ in proof})
        named = int(short(records[pair[0]]) == short(records[pair[1]]))
        return -strength, -named, pair

    owner = {k: k for k in private}
    components = {k: {k} for k in private}
    for (a, b), proof in sorted(edges.items(), key=weight):
        x, y = owner[a], owner[b]
        if x == y:
            continue
        viable = True
        for left in components[x]:
            for right in components[y]:
                pair = tuple(sorted((left, right)))
                if not compatible(records[left], records[right],
                                  strong=strong_pair(records[left], records[right], edges.get(pair, ()))):
                    viable = False
                    break
            if not viable:
                break
        if viable:
            for right in components.pop(y):
                owner[right] = x
                components[x].add(right)
    return sorted(sorted(c) for c in components.values() if len(c) > 1)


def direct_shape(member):
    kind = member["base"]
    if kind == "aggregate":
        tag = (member.get("record") or member["ctype"]).rsplit(":", 1)[-1].split(" ", 1)[0]
        if tag in {"struct", "union"}:
            kind = tag
    return kind, member["array"], member["size"], member.get("esize")


def layout_rows(views):
    """Count one vote per declaration for each name/type at each byte offset."""
    cells = defaultdict(dict)
    ends = defaultdict(list)
    direct = defaultdict(dict)
    for view in views:
        for leaf in view["known"]:
            cells[leaf["off"]].setdefault(view["id"], []).append(leaf)
            ends[leaf["off"] + leaf["size"]].append(leaf)
        for member in view["members"]:
            if member.get("off") is not None and member.get("size") and not opaque(member):
                direct[member["off"]].setdefault(view["id"], []).append(member)
    member_conflicts = {}
    for off, copies in direct.items():
        if len(copies) < 2:
            continue
        members = [m for declarations in copies.values() for m in declarations]
        if not any(m["base"] == "aggregate" for m in members):
            continue
        shapes = {frozenset(map(direct_shape, declarations)) for declarations in copies.values()}
        if len(shapes) > 1:
            votes = Counter()
            for declarations in copies.values():
                votes.update({(m["name"], m["ctype"]) for m in declarations})
            member_conflicts[off] = [(name, ctype, votes[name, ctype]) for name, ctype in sorted(votes)]
    active = Counter()
    flags = lambda lf: (bool(lf["array"]), "." in lf["name"])
    rows = []
    for off in sorted(cells.keys() | ends.keys() | member_conflicts.keys()):
        for lf in ends.get(off, ()):
            active[flags(lf)] -= 1
        if off not in cells and off not in member_conflicts:
            continue
        cells_at_offset = cells.get(off, {})
        votes = Counter()
        values = [lf for leaves in cells_at_offset.values() for lf in leaves]
        for leaves in cells_at_offset.values():
            votes.update({(lf["name"], lf["ctype"]) for lf in leaves})
        conflicts = []
        if len({lf["width"] for lf in values}) > 1 or len({lf["size"] for lf in values}) > 1:
            conflicts.append("width/span")
        if len({lf["signed"] for lf in values if lf["base"] == "int"}) > 1:
            conflicts.append("signedness")
        shapes = {flags(lf) for lf in values} | {pair for pair, count in active.items() if count}
        if len({array for array, _ in shapes}) > 1:
            conflicts.append("array/scalar")
        if len({nested for _, nested in shapes}) > 1:
            conflicts.append("nested/flat")
        if any(active.values()):
            conflicts.append("overlapping span")
        active.update(flags(lf) for lf in values)
        row = {"offset": off, "votes": [(n, t, v) for (n, t), v in sorted(votes.items())],
               "conflicts": conflicts}
        if off in member_conflicts:
            conflicts.append("aggregate layout")
            row["member_votes"] = member_conflicts[off]
        rows.append(row)
    return rows


def extension_of(extra, existing):
    """Every extra typed span occupies storage the existing view leaves opaque."""
    if not existing <= extra:
        return False
    ranges = [(off, off + shape[0]) for off, shape, _ in existing]
    return all(not any(off < end and start < off + shape[0] for start, end in ranges)
               for off, shape, _ in extra - existing)


class SharedLayouts:
    def __init__(self, records, buckets, returns=()):
        self.by_name, self.by_size, self.by_field, self.by_formal, self.by_return = (
            defaultdict(set) for _ in range(5))
        self.records = records
        for key, view in records.items():
            if view["file"].startswith("include/"):
                self.by_name[short(view)].add(key)
                self.by_size[view["kind"], view["size"]].add(key)
                for lf in view["known"]:
                    self.by_field[lf["off"], lf["name"]].add(key)
        for anchor, members in buckets.items():
            if anchor[0] == "formal type" and anchor[1] in records and records[anchor[1]]["file"].startswith("include/"):
                for member in members:
                    self.by_formal[member].add(anchor[1])
        for finding in returns:
            result = finding["result"]
            if records[result]["file"].startswith("include/"):
                self.by_return[finding["view"]].add(result)

    def relations(self, views):
        size = max(v["size"] for v in views)
        observed = {(lf["off"], leaf_shape(lf), lf["signed"]) for v in views for lf in v["known"]}
        candidates = set()
        field_hits = Counter()
        for view in views:
            candidates |= self.by_name[short(view)] | self.by_size[view["kind"], view["size"]]
            candidates |= self.by_formal[view["id"]] | self.by_return[view["id"]]
            field_hits.update(k for lf in view["known"] for k in self.by_field[lf["off"], lf["name"]])
        candidates |= {k for k, hits in field_hits.items() if hits >= 2}
        matches = []
        for key in sorted(candidates):
            shared = self.records[key]
            formal = any(key in self.by_formal[v["id"]] for v in views)
            returned = any(key in self.by_return[v["id"]] for v in views)
            same_tag = any(short(v) == short(shared) and meaningful(v) for v in views)
            if not any(compatible(v, shared, strong=formal or returned or same_tag) for v in views):
                continue
            known = {(lf["off"], leaf_shape(lf), lf["signed"]) for lf in shared["known"]}
            common = len(observed & known)
            named = set((lf["off"], lf["name"]) for v in views for lf in v["known"])
            name_hits = len(named & {(lf["off"], lf["name"]) for lf in shared["known"]})
            if not (formal or returned or same_tag or common >= 3 and (known == observed or name_hits >= 2)):
                continue
            if size == shared["size"] and observed == known:
                relation = "equals"
            elif shared["size"] <= size and extension_of(observed, known):
                relation = "extends"
            elif size <= shared["size"] and extension_of(known, observed):
                relation = "contained by"
            else:
                relation = "overlapping view"
            proof = ("typed call" if formal else "typed return" if returned else
                     "same tag" if same_tag else "layout only")
            matches.append((key, relation, proof, common))
        return sorted(matches, key=lambda row: (row[2] == "layout only", -row[3], row[0]))


def warning_counts(paths, compiler, jobs):
    from statement_macros import single_use_names

    texts = {rel: (ROOT / rel).read_text(errors="replace") for rel in sorted(paths)}
    if not texts:
        return {}
    once = frozenset(single_use_names(ROOT))
    params = {rel: concessions.literal_param_sites(rel, text, ROOT)
              for rel, text in texts.items() if concessions.literal_param_definitions(rel, text)}

    def count(rel):
        text = texts[rel]
        sites = concessions.sites(text, rel, single_use=once) + params.get(rel, [])
        if compiler:
            sites += concessions.compiler_sites(rel, text)
        return rel, len(concessions.warnings(sites))

    with ThreadPoolExecutor(jobs) as pool:
        return dict(pool.map(count, texts))


def make_catalog(scans, *, jobs=2, compiler=True, rom=True):
    records = {key: dict(v, id=key, known=inhabited(v))
               for s in scans for key, v in s["records"].items()}
    for v in records.values():
        v["_offsets"] = {lf["off"]: lf for lf in v["known"]}
    private, buckets, edges, embeds, rom_params = flow(scans, records, report_groups())
    groups = family_components(private, records, buckets, edges)
    participants = {k for group in groups for k in group}
    warnings = warning_counts({records[k]["file"] for k in participants}, compiler, jobs)
    rom_params = {fn: regs for fn, regs in rom_params.items() if participants.intersection(regs.values())}
    loads = defaultdict(list)
    if rom:
        for key, row in evidence.rom_loads(rom_params):
            if key in participants:
                loads[key].append(row)
    returns = returned_views(scans, records)
    shared = SharedLayouts(records, buckets, returns)
    families = []
    for keys in groups:
        views = [records[key] for key in keys]
        canonical = sorted(Counter(short(v) for v in views).items(), key=lambda pair: (-pair[1], pair[0]))[0][0]
        identity = canonical + "-" + digest("\n".join(keys).encode())[:8]
        paths = {v["file"] for v in views}
        wc = sum(warnings[p] for p in paths)
        keys_set = set(keys)
        reasons = sorted({reason for a, b in edges if a in keys_set and b in keys_set
                          for reason in edges[a, b]})
        families.append({"id": identity, "name": canonical, "records": keys, "kind": views[0]["kind"],
                         "size": max(v["size"] for v in views), "warnings": wc, "files": len(paths),
                         "score": len(views) * wc, "rows": layout_rows(views), "evidence": reasons,
                         "shared": shared.relations(views), "loads": [r for k in keys for r in loads[k]]})
    families.sort(key=lambda f: (-f["score"], -len(f["records"]), f["id"]))
    sites_by_object = defaultdict(set)
    declared = set()
    for (p, o, c, native), sites in embeds.items():
        sites_by_object[p, o, c].update(sites)
        if native:
            declared.add((p, o, c))
    findings = [{"parent": p, "offset": o, "child": c, "declared": (p, o, c) in declared, "sites": sorted(sites)}
                for (p, o, c), sites in sites_by_object.items()]
    findings.sort(key=lambda r: (r["declared"], -len(r["sites"]), r["parent"], r["offset"], r["child"]))
    for v in records.values():
        del v["_offsets"]
    return {"version": SCHEMA, "families": families, "embeds": findings, "returns": returns, "warnings": warnings,
            "records": records, "files": {s["file"]: s["digest"] for s in scans},
            "scan_errors": {s["file"]: s["error"] for s in scans if s.get("error")},
            "unmeasured": [key for s in scans for key in s.get("unmeasured", ())],
            "compiler_warnings": compiler, "rom_loads": rom}


def family_lines(family):
    f = family
    rows = [f"### {f['id']}", "",
            f"{len(f['records'])} copies in {f['files']} files, extent `{f['size']:#x}`, "
            f"{f['warnings']} warnings, priority {f['score']}.", "",
            *[f"- `{k}`" for k in f["records"]], ""]
    rows.extend(f"- {relation} `{key}` ({proof}, {count} agreeing fields)"
                for key, relation, proof, count in f["shared"])
    rows.extend(f"- {kind} · {where}" for kind, where in f["evidence"])
    loads = defaultdict(Counter)
    for off, width, signed, _, _ in f["loads"]:
        loads[off][width, signed] += 1
    rows.extend(["", "| Offset | Candidate name and type (copies) | Conflicts | ROM loads |",
                 "|---:|---|---|---|"])
    for row in f["rows"]:
        off = row["offset"]
        votes = ", ".join(f"`{n}` `{t}` ({v})" for n, t, v in row["votes"]).replace("|", r"\|")
        conflicts = ", ".join(row["conflicts"])
        if row.get("member_votes"):
            members = ", ".join(f"`{n}` `{t}` ({v})" for n, t, v in row["member_votes"]).replace("|", r"\|")
            conflicts += f" ({members})"
        counts = loads.get(off, {})
        observations = []
        for width, signed in sorted(counts, key=lambda k: (k[0], -1 if k[1] is None else k[1])):
            prefix = "word" if signed is None else "s" if signed else "u"
            observations.append(f"{prefix}{width * 8} × {counts[width, signed]}")
        rows.append(f"| `{off:#x}` | {votes} | {conflicts} | {', '.join(observations)} |")
    return rows + [""]


def report_lines(catalog, limit):
    records, families = catalog["records"], catalog["families"]
    embeds = catalog["embeds"]
    yield "# Private record families"
    yield ""
    yield (f"{len(families)} families, {len(records)} measured records, {len(embeds)} embedded-type findings. "
           f"{len(catalog['scan_errors'])} files had a parser or preprocessor failure and "
           f"{len(catalog['unmeasured'])} definitions lacked a compiler measurement.")
    yield ""
    yield ("Priority is copy count × unexcused warnings in the distinct source files. "
           + ("Includes compiler-reported warnings." if catalog["compiler_warnings"] else
              "Compiler-reported warnings were disabled for this scan."))
    yield ""
    if catalog.get("shared_headers"):
        yield "The scan uses the selected shared headers for layout comparisons and embedded-type findings."
        yield ""
        for header in catalog["shared_headers"]:
            yield f"- `{header}`"
        yield ""
    yield "## Top families"
    yield ""
    yield "| Family | Copies | Bytes | Warnings | Priority |"
    yield "|---|---:|---:|---:|---:|"
    for f in families[:limit]:
        yield f"| [{f['id']}](#{f['id'].lower()}) | {len(f['records'])} | `{f['size']:#x}` | {f['warnings']} | {f['score']} |"
    yield ""
    for family in families[:limit]:
        yield from family_lines(family)
    yield "## Embedded structs and unions"
    yield ""
    yield "| Parent | Offset | Child | Status | Call sites | First site |"
    yield "|---|---:|---|---|---:|---|"
    for r in embeds:
        where, caller, callee, arg = r["sites"][0]
        status = "declared" if r["declared"] else "candidate"
        yield (f"| `{r['parent']}` | `{r['offset']:#x}` | `{r['child']}` | {status} | {len(r['sites'])} | "
               f"`{where}` {caller} → {callee} argument {arg + 1} |")
    if catalog.get("returns"):
        yield ""
        yield "## Return types seen through alternate declarations"
        yield ""
        yield "| Caller view | Defined result | Callee | Call sites | First site |"
        yield "|---|---|---|---:|---|"
        for finding in catalog["returns"]:
            where, caller = finding["sites"][0]
            yield (f"| `{finding['view']}` | `{finding['result']}` | `{finding['function']}` | "
                   f"{len(finding['sites'])} | `{where}` {caller} |")


def load_catalog(path):
    catalog = json.loads(path.read_text())
    if catalog["version"] != SCHEMA:
        raise ValueError("catalog version changed; rerun structfamilies.py")
    return catalog


def select_family(catalog, query):
    hits = [f for f in catalog["families"] if f["id"] == query or f["name"] == query]
    if len(hits) != 1:
        raise ValueError(f"expected one family for {query!r}; found " + (", ".join(f["id"] for f in hits) or "none"))
    return hits[0]


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("paths", nargs="*", help="source paths or directories; default all game units")
    ap.add_argument("--shared-header", type=Path, action="append", help="limit shared probes to this header, repeatable")
    ap.add_argument("--out", type=Path, default=OUT)
    ap.add_argument("--limit", type=int, default=50)
    ap.add_argument("-j", type=int, default=2)
    ap.add_argument("--no-compiler", action="store_true", help="rank without compiler-detected warning kinds")
    ap.add_argument("--no-rom", action="store_true", help="omit evidence from ROM loads")
    ap.add_argument("--family", help="print one family from the saved catalog")
    args = ap.parse_args(argv)
    if args.j < 1 or args.limit < 0:
        ap.error("jobs must be positive and limit nonnegative")
    os.chdir(ROOT)
    if args.family:
        try:
            print("\n".join(family_lines(select_family(load_catalog(args.out / "families.json"), args.family))))
        except (OSError, ValueError) as exc:
            ap.error(str(exc))
        return
    try:
        scans = scan_tree(args.paths, args.j, args.shared_header)
    except ValueError as exc:
        ap.error(str(exc))
    catalog = make_catalog(scans, jobs=args.j, compiler=not args.no_compiler, rom=not args.no_rom)
    if args.shared_header:
        catalog["shared_headers"] = sorted({s["file"] for s in scans if s["file"].startswith("include/")})
    json_write(args.out / "families.json", catalog)
    report = args.out / "REPORT.md"
    report.write_text("\n".join(report_lines(catalog, args.limit)) + "\n")
    print(f"{report}: {len(catalog['families'])} families, {len(catalog['embeds'])} embedded-type findings")


if __name__ == "__main__":
    main()
