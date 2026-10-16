#!/usr/bin/env python3
"""The options each GCL command and registry routine reads, from its code.

  gcloptions.py KEY|COMMAND...      the options of registry keys or commands

GCL options are a letter and values (`-p 2688,512,2432 -d 128`; compiled as
keyword packets, family 50). A native routine asks for them by letter:
`Script_SeekToKeyword('p')` then one `Script_GetValue()` per value, or
`Script_GetKeywordValue('d', default)` for one value with a default. This scans
the generated disassembly (gen/code.s) of a registry key's callback, its loader
and the loader's nearby callees (as tools/assets/actors.py does for sprites),
or a command's handler, for those calls: the letter, the default and the number
of values. Names come from the matched C where one exists (`radius =
Script_GetKeywordValue('d', 256)` names `d` radius; `pos.x = Script_GetValue()`
after `Script_SeekToKeyword('p')` names `p` pos), and from
gcl/names/option.csv (owner, letter, name, english, evidence), which wins.

The letter is all the compiled script keeps, so letters are the source
spelling (MGS1's GCL used letters too); names are ours.
"""
import csv
import functools
import glob
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(ROOT, "tools"))

SEEK = {"Script_SeekToKeyword"}
GETKW = {"Script_GetKeywordValue", "sub_0821ABA8"}
GETV = {"Script_GetValue", "Script_GetValueSafe"}
OTHER = {"Script_GetPc": "block", "Script_ParseStringRef": "text"}
CMD_TABLES = ((0x0861418C, 6), (0x08E8791C, 8), (0x080F38C8, 3))


def scan(lines):
    """[{letter, kind, default, values}] read by one function's code. Constants
    in r0-r7 are tracked through movs/lsls/negs/mov/adds #0/ldr =; a call
    clobbers r0-r3 and a label forgets everything."""
    out, reg, cur = [], {}, None
    for ln in lines:
        if re.match(r"^\w+:", ln):
            reg = {}
            continue
        m = re.match(r"\s*bl (\w+)", ln)
        if m:
            f, r0, r1 = m.group(1), reg.get(0), reg.get(1)
            letter = chr(r0) if r0 is not None and 0x21 <= r0 <= 0x7E else None
            if f in SEEK and letter:
                cur = {"letter": letter, "kind": "values", "default": None, "values": 0}
                out.append(cur)
            elif f in GETKW and letter:
                out.append({"letter": letter, "kind": "value", "default": r1, "values": 1})
                cur = None
            elif f in GETV and cur is not None:
                cur["values"] += 1
            elif f in OTHER and cur is not None and not cur["values"]:
                cur["kind"] = OTHER[f]
            for r in range(4):
                reg.pop(r, None)
            continue
        m = re.match(r"\s*(\w+) r(\d), (.*?)\s*(?:@ =(0x[0-9A-Fa-f]+))?\s*$", ln)
        if not m:
            continue
        op, d, rest, lit = m.group(1), int(m.group(2)), m.group(3), m.group(4)
        v = None
        if op == "movs" and re.fullmatch(r"#(0x[0-9a-f]+|\d+)", rest):
            v = int(rest[1:], 0)
        elif op == "lsls" and re.fullmatch(rf"r{d}, #\d+", rest) and d in reg:
            v = (reg[d] << int(rest.split("#")[1])) & 0xFFFFFFFF
        elif op in ("negs", "rsbs") and re.fullmatch(rf"r{d}(, #0)?", rest) and d in reg:
            v = -reg[d]
        elif op in ("mov", "adds") and re.fullmatch(r"r(\d)(, #0)?", rest):
            v = reg.get(int(rest[1]))
        elif op == "ldr" and lit:
            v = int(lit, 16)
            v = v - (1 << 32) if v & 0x80000000 else v
        if v is None:
            reg.pop(d, None)
        else:
            reg[d] = v
    for o in out:
        if o["kind"] != "value" and o["values"]:
            o["kind"] = "values"
    return out


@functools.lru_cache(None)
def c_bodies():
    """{function name: body} of every function written in C under src/."""
    out = {}
    for f in glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True):
        s = open(f, encoding="utf-8", errors="replace").read()
        for m in re.finditer(r"^[A-Za-z_][\w \t\*]*?\b(\w+)\([^;{]*\)\s*\{(.*?)^\}", s, re.M | re.S):
            out[m.group(1)] = m.group(2)
    return out


LETTER = r"('.'|0x[0-9A-Fa-f]{2}|\d+)"
TARGET = r"(?:\w+->)*(\w+)(?:\[[^\]]*\])?(?:\.\w+)*"


def _letter(t):
    return t[1] if t.startswith("'") else chr(int(t, 0))


def c_names(fname):
    """{letter: variable or field name} from a matched function's C:
    `radius = Script_GetKeywordValue('d', 256)`, `p->scene = ...(0x64, -1)`,
    and for Script_SeekToKeyword the first `X = Script_GetValue()` after it."""
    body, out = c_bodies().get(fname, ""), {}
    for m in re.finditer(TARGET + r"\s*=\s*\w*KeywordValue\w*\(\s*" + LETTER, body):
        out.setdefault(_letter(m.group(2)), m.group(1))
    for m in re.finditer(r"SeekToKeyword\(\s*" + LETTER + r"\s*\)", body):
        v = re.search(TARGET + r"\s*=\s*Script_GetValue\w*\(\)", body[m.end():m.end() + 400])
        nxt = re.search(r"SeekToKeyword|KeywordValue", body[m.end():m.end() + 400])
        if v and (not nxt or v.start() < nxt.start()):
            out.setdefault(_letter(m.group(1)), v.group(1))
    return out


@functools.lru_cache(None)
def overrides():
    """{(owner, letter): row} from gcl/names/option.csv."""
    try:
        with open(os.path.join(ROOT, "gcl", "names", "option.csv"), encoding="utf-8") as f:
            return {(r["owner"].upper() if re.fullmatch(r"[0-9A-Fa-f]{4}", r["owner"]) else r["owner"], r["letter"]): r
                    for r in csv.DictReader(f)}
    except OSError:
        return {}


def schema(owner, funcs):
    """{letter: {name, english, kind, default, values, in}} for an owner (a
    registry key "99CA" or a command name) read by funcs (addresses)."""
    import actors
    code = actors.code()[0]
    out = {}
    for a in funcs:
        fn = code.get(a)
        if not fn:
            continue
        names = c_names(actors.fname(a))
        names = {**c_names(fn["name"]), **names}
        for o in scan(fn["lines"]):
            e = out.setdefault(o["letter"], dict(o, name=names.get(o["letter"], ""), english="", **{"in": actors.fname(a)}))
            if not e["name"] and names.get(o["letter"]):
                e["name"] = names[o["letter"]]
    for (own, letter), r in overrides().items():
        if own == owner:
            e = out.setdefault(letter, {"letter": letter, "kind": "values", "default": None, "values": 0, "in": ""})
            e["name"], e["english"] = r["name"] or e.get("name", ""), r.get("english", "")
    return out


@functools.lru_cache(None)
def registry_schemas():
    """{key int: {letter: option}} for every registry entry."""
    import actors
    names = actors.code()[1]
    out = {}
    for key, r in actors.registry().items():
        cb = int(r["callback"], 16) & ~1
        funcs = [cb]
        lda = actors.loader_of(cb)
        if lda:
            funcs.append(lda)
        for f in list(funcs):
            funcs += [names[c] for c in sorted(set(actors.calls(f))) if c in names and abs(names[c] - f) < actors.LOCAL
                      and names[c] not in funcs and names[c] not in actors.ALLOC]
        out[int(key, 16)] = schema(key.upper(), funcs)
    return out


@functools.lru_cache(None)
def command_schemas():
    """{command name: {letter: option}} for the native command handlers."""
    import gclnames
    from romlib import load_rom
    rom, cmds = load_rom(), {k: r["name"] for k, r in gclnames.best("command").items()}
    out = {}
    for table, n in CMD_TABLES:
        for i in range(n):
            k, fn = struct.unpack_from("<II", rom, table - 0x08000000 + 8 * i)
            name = cmds.get(k, f"cmd_{k:04X}")
            if name not in ("if", "switch"):  # their options are syntax (else, case)
                out[name] = schema(name, [fn & ~1])
    return out


def describe(opts, letters=None):
    """'-p pos, -d radius' for the letters given (default: all)."""
    parts = []
    for k in letters or sorted(opts):
        o = opts.get(k)
        if o and (o["name"] or o["english"]):
            parts.append(f"-{k} {o['english'] or o['name']}")
    return ", ".join(parts)


def main():
    for a in sys.argv[1:] or sys.exit(__doc__):
        opts = command_schemas().get(a) if not re.fullmatch(r"[0-9A-Fa-f]{1,4}", a) else \
            registry_schemas().get(int(a, 16))
        print(a)
        for k, o in sorted((opts or {}).items()):
            d = "" if o["default"] is None else f" (default {o['default']})"
            print(f"  -{k}  {o['name'] or '?'}  {o['kind']} x{o['values']}{d}  in {o['in']}")


if __name__ == "__main__":
    main()
