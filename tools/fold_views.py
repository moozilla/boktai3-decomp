#!/usr/bin/env python3
"""Fold a unit's private view of a class into the class struct.

  fold_views.py include/vorn.h Vorn src/fn/sub_080AD40C.c:SceneActorReactionContext ...
  fold_views.py include/vorn.h Vorn --candidates      list the views in units of Vorn_ functions

Drafts often declare their own copy of a monster struct: `u8 unknown0[0x6D4]; u16 flags;`.
For each UNIT:VIEW this places every named member of the view at its offset in the class
(both laid out by the compiler, through struct_views), then:

* a member the class already has at that offset with the same width and signedness is
  renamed to the class's name in the unit;
* a member that falls inside one of the class's padding arrays is added to the class,
  splitting the array;
* anything else (a member across two class members, a different width or signedness, a
  member inside the embedded base) is reported and the view is left alone.

Then the view's definition goes, the unit names the class instead, the class header is
included, the renamed accesses are rewritten (tools/retype_fields.py) and the unit is
checked with tools/check.py. The header is checked again by layout: every member the class
had keeps its offset. Any failure restores the header and the unit.
"""
import argparse
import csv
import os
import re
import subprocess
import sys

TOOLS = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(TOOLS)
sys.path.insert(0, TOOLS)
import struct_views as sv  # noqa: E402  (re-execs into build/venv for pycparser)
import retype_fields as rf  # noqa: E402
import header_dependents as hd  # noqa: E402

PAD = re.compile(r"(unknown|unk|pad)[0-9A-Fa-f]*$")
# a class name that only records the offset (field6AC, frame69A) gives way to a reader's name
OFFSET_NAME = re.compile(r"[a-z]+[0-9A-F]{1,3}$")
WEAK = {"flags", "value", "data", "state", "field", "mode", "timer", "counter", "a", "b", "c", "x", "y", "z"}


def better(view_name, class_name):
    return (OFFSET_NAME.match(class_name) and not OFFSET_NAME.match(view_name)
            and not re.search(r"\d", view_name) and view_name not in WEAK and len(view_name) > 2)


def top(name):
    return re.split(r"[.\[]", name, maxsplit=1)[0]


def members(leaves):
    """Top-level members in offset order: {name, off, end, kinds}."""
    out = {}
    for l in leaves:
        t = top(l["name"])
        m = out.setdefault(t, {"name": t, "off": l["off"], "end": l["off"] + l["size"], "kinds": []})
        m["off"] = min(m["off"], l["off"])
        m["end"] = max(m["end"], l["off"] + l["size"])
        m["kinds"].append((l["off"], sv.leaf_kind(l)))
    return sorted(out.values(), key=lambda m: m["off"])


def struct_body(text, tag):
    m = re.search(r"^struct %s \{\n(.*?)^\};\n?" % re.escape(tag), text, re.M | re.S)
    return m


def declarations(body):
    """{member name: declaration text} from a struct body (one level, no unions)."""
    out = {}
    body = re.sub(r"/\*.*?\*/|//[^\n]*", "", body, flags=re.S)
    for stmt in body.split(";"):
        stmt = " ".join(stmt.split())
        if not stmt:
            continue
        m = re.match(r"((?:const |unsigned |signed )*(?:struct |union )?\w+)\s*(.*)$", stmt)
        if not m:
            continue
        base, rest = m.groups()
        for d in rest.split(","):
            d = d.strip()
            n = re.search(r"(\w+)\s*(\[[^\]]*\])*\s*(:\s*\d+)?$", d)
            if n:
                out[n.group(1)] = f"{base} {d}"
    return out


def probe(header, tag, unit, view):
    views = sv.probe([f"struct {tag}", f"struct {view}"], [header, unit])
    return views[f"struct {tag}"], views[f"struct {view}"]


def plan(cls, view, view_decls, base_end):
    """(renames {view name: class name}, adopted {class name: view name},
    inserts [(pad member, view member, decl)], problems)."""
    renames, adopted, inserts, problems = {}, {}, [], []
    cmembers = members(cls["leaves"])
    for m in members(view["leaves"]):
        if PAD.match(m["name"]):
            continue
        if m["off"] < base_end:
            problems.append(f"{m['name']} at {m['off']:#x} is inside the base")
            continue
        same = [c for c in cmembers if c["off"] == m["off"] and c["end"] == m["end"] and not PAD.match(c["name"])]
        if same:
            c = same[0]
            # same widths but another signedness or pointer-ness: the byte check decides
            if [(o, k[0], k[3]) for o, k in c["kinds"]] != [(o, k[0], k[3]) for o, k in m["kinds"]]:
                problems.append(f"{m['name']} at {m['off']:#x}: {c['name']} has another type")
            elif better(m["name"], c["name"]):
                adopted[c["name"]] = m["name"]
            elif c["name"] != m["name"]:
                renames[m["name"]] = c["name"]
            continue
        pad = [c for c in cmembers if PAD.match(c["name"]) and c["off"] <= m["off"] and m["end"] <= c["end"]]
        if not pad:
            problems.append(f"{m['name']} at {m['off']:#x}..{m['end']:#x} crosses class members")
            continue
        decl = view_decls.get(m["name"])
        if not decl:
            problems.append(f"{m['name']}: declaration not found")
            continue
        inserts.append((pad[0], m, decl))
    taken = {adopted.get(c["name"], c["name"]) for c in cmembers}
    for i, (p, m, decl) in enumerate(inserts):
        if m["name"] in WEAK or len(m["name"]) <= 2:
            # a placeholder name in a draft says less than the offset
            new = f"field{m['off']:X}"
            renames[m["name"]] = new
            inserts[i] = (p, m, re.sub(r"\b%s\b" % m["name"], new, decl))
        elif m["name"] in taken:
            new = f"{m['name']}{m['off']:X}"
            renames[m["name"]] = new
            decl = re.sub(r"\b%s\b" % m["name"], new, decl)
            inserts[i] = (p, m, decl)
        taken.add(renames.get(m["name"], m["name"]))
    return renames, adopted, inserts, problems


def size(n):
    return str(n) if n < 10 else f"0x{n:X}"


def adopt(text, tag, adopted):
    """Rename members of struct tag in its definition."""
    body = struct_body(text, tag)
    new_body = body.group(1)
    for old, new in adopted.items():
        new_body = re.sub(r"\b%s\b" % re.escape(old), new, new_body)
    return text[:body.start(1)] + new_body + text[body.end(1):]


def apply_inserts(text, tag, inserts):
    """Split padding arrays of struct tag around the inserted members."""
    by_pad = {}
    for p, m, decl in inserts:
        by_pad.setdefault(p["name"], (p, []))[1].append((m, decl))
    body = struct_body(text, tag)
    new_body = body.group(1)
    for name, (p, items) in by_pad.items():
        line = re.search(r"^([ \t]*)u8 %s\[[^\]]+\];[^\n]*\n" % re.escape(name), new_body, re.M)
        if not line:
            raise ValueError(f"padding {name} is not on a line of its own")
        indent = line.group(1)
        out, at = [], p["off"]
        for m, decl in sorted(items, key=lambda x: x[0]["off"]):
            if m["off"] > at:
                out.append(f"{indent}u8 unknown{at:X}[{size(m['off'] - at)}];")
            out.append(f"{indent}{decl};")
            at = m["end"]
        if p["end"] > at:
            out.append(f"{indent}u8 unknown{at:X}[{size(p['end'] - at)}];")
        new_body = new_body[:line.start()] + "\n".join(out) + "\n" + new_body[line.end():]
    return text[:body.start(1)] + new_body + text[body.end(1):]


def layout_kept(old, new, adopted=None):
    """Every non-padding leaf of the old class keeps its offset and kind (under its new name)."""
    adopted = adopted or {}

    def renamed(n):
        t = top(n)
        return adopted.get(t, t) + n[len(t):]
    keep = {(renamed(l["name"]), l["off"], sv.leaf_kind(l)) for l in old["leaves"] if not sv.is_pad(l["name"])}
    have = {(l["name"], l["off"], sv.leaf_kind(l)) for l in new["leaves"]}
    return keep <= have and new["size"] >= old["size"]


def check(unit):
    r = subprocess.run([sys.executable, os.path.join(TOOLS, "check.py"), unit, "--quiet"],
                       cwd=ROOT, capture_output=True, text=True)
    return r.returncode == 0


def edit_unit(text, view, tag, header):
    body = struct_body(text, view)
    text = text[:body.start()] + text[body.end():].lstrip("\n") if body else text
    text = re.sub(r"\bstruct %s\b" % re.escape(view), f"struct {tag}", text)
    inc = f'#include "{os.path.basename(header)}"'
    if inc not in text:
        text = re.sub(r'^(#include "global.h"\n)', r"\1" + inc + "\n", text, count=1, flags=re.M)
        if inc not in text:
            text = inc + "\n" + text
    return text


def base_end(cls):
    first = members(cls["leaves"])[0]
    return first["end"]


def fold(header, tag, unit, view, dry_run=False):
    hpath, upath = os.path.join(ROOT, header), os.path.join(ROOT, unit)
    htext, utext = open(hpath).read(), open(upath).read()
    body = struct_body(utext, view)
    if not body:
        return f"{unit}: no struct {view}"
    cls, v = probe(header, tag, unit, view)
    renames, adopted, inserts, problems = plan(cls, v, declarations(body.group(1)), base_end(cls))
    if problems:
        return f"{unit}:{view}: left alone: " + "; ".join(problems)
    if dry_run:
        return f"{unit}:{view}: renames {renames}, class renames {adopted}, adds {[d for _, _, d in inserts]}"
    saved = {}
    try:
        if adopted:
            # the class takes the reader's name; every other unit that uses the old one follows
            users = [u for u in hd.dependents([header]) if u != unit]
            saved = {u: open(os.path.join(ROOT, u)).read() for u in users}
            htext2 = adopt(htext, tag, adopted)
            with open(hpath, "w") as f:
                f.write(htext2)
            _, unsettled, failed = rf.run(users, adopted, os.cpu_count() or 4)
            if unsettled or failed:
                raise RuntimeError(f"class rename left {list(unsettled.values())[:2] or list(failed.values())[:1]}")
            touched = [u for u in users if open(os.path.join(ROOT, u)).read() != saved[u]]
            bad = [u for u in touched if not check(u)]
            if bad:
                raise RuntimeError(f"class rename: {bad} mismatch")
            htext_cur = htext2
        else:
            htext_cur = htext
        if inserts:
            with open(hpath, "w") as f:
                f.write(apply_inserts(htext_cur, tag, inserts))
            new = sv.probe([f"struct {tag}"], [header])[f"struct {tag}"]
            if not layout_kept(cls, new, adopted):
                raise RuntimeError("the class layout moved")
        with open(upath, "w") as f:
            f.write(edit_unit(utext, view, tag, header))
        if renames:
            _, unsettled, failed = rf.run([unit], renames, 1)
            if unsettled or failed:
                raise RuntimeError(f"rewrite left {list(unsettled.values())[:2] or list(failed.values())[:1]}")
        if not check(unit):
            raise RuntimeError("tools/check.py: mismatch or compile error")
    except (RuntimeError, ValueError) as e:
        with open(hpath, "w") as f:
            f.write(htext)
        with open(upath, "w") as f:
            f.write(utext)
        for u, t in saved.items():
            with open(os.path.join(ROOT, u), "w") as f:
                f.write(t)
        return f"{unit}:{view}: restored: {e}"
    return f"{unit}:{view}: folded (renames {renames}, class renames {adopted}, {len(inserts)} members added)"


def candidates(tag, header):
    """UNIT:VIEW pairs: structs padded past the base in units with Prefix_ functions."""
    units = set()
    for r in csv.DictReader(open(os.path.join(ROOT, "symbols", "functions.csv"))):
        if r["name"].startswith(tag + "_"):
            out = subprocess.run(["git", "grep", "-l", "-P", r"^\w[\w \*]*\b%s\(" % r["name"], "--", "src"],
                                 cwd=ROOT, capture_output=True, text=True).stdout.split()
            units.update(out)
    cls = sv.probe([f"struct {tag}"], [header])[f"struct {tag}"]
    end = base_end(cls)
    found = []
    for u in sorted(units):
        text = open(os.path.join(ROOT, u)).read()
        for m in re.finditer(r"^struct (\w+) \{\n\s*u8 (?:unknown|unk|pad)0\[(0x[0-9A-Fa-f]+|\d+)\];", text, re.M):
            if int(m.group(2), 0) >= end:
                found.append(f"{u}:{m.group(1)}")
    return found


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("header")
    ap.add_argument("cls", metavar="CLASS")
    ap.add_argument("views", nargs="*", metavar="UNIT:VIEW")
    ap.add_argument("--candidates", action="store_true", help="list the views of the class's units")
    ap.add_argument("--all", action="store_true", help="fold every candidate")
    ap.add_argument("--dry-run", action="store_true")
    a = ap.parse_args()
    views = a.views
    if a.candidates or a.all:
        found = candidates(a.cls, a.header)
        if a.candidates:
            print("\n".join(found))
            return
        views = found
    bad = 0
    for uv in views:
        unit, view = uv.rsplit(":", 1)
        msg = fold(a.header, a.cls, unit, view, a.dry_run)
        bad += "folded" not in msg and not a.dry_run
        print(msg)
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
