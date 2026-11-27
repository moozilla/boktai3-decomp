#!/usr/bin/env python3
"""Sample constant flag operations in the ROM, using the linked ELF for instruction boundaries.

Run struct_views.py first (see notes/flag-accessors_2026-11-25.md). Print one TSV row
per proven test or same-field update. Stop tracking values at incoming branch labels.
The uses column counts classified flag operations that share the mask definition
and register. For example, an earlier byte store that also uses the same constant
does not contribute to this count.
"""
import argparse
import csv
import re
import struct
import subprocess
import sys
import json
from collections import Counter
from dataclasses import dataclass
from pathlib import Path

from elftools.elf.elffile import ELFFile
from concessions import strip

BASE = 0x08000000
PAUSE = {"gEngineFlags", "gScenePauseFlags"}
WIDTH = {"": 4, "h": 2, "b": 1}
LABEL = re.compile(r"^(\w+): @ 0x([0-9A-Fa-f]{8})")
SIGNATURE = re.compile(r"(?m)^\w[\w \t*]*?\b(\w+)\s*\(([^();{}]*)\)\s*\{")
MEMORY = re.compile(r"(\w+), \[(\w+)(?:, (#?-?\w+))?\]")


@dataclass(frozen=True)
class Constant:
    value: int
    first: int
    ready: int


@dataclass(frozen=True)
class Address:
    owner: str
    arg: str
    offset: int


@dataclass(frozen=True)
class Word:
    field: str
    address: object
    loaded: int
    last: int


@dataclass(frozen=True)
class Result:
    word: Word
    mask: Constant
    pc: int
    opcode: str
    reg: str


def signatures():
    out = {}
    for path in Path("src").rglob("*.c"):
        if "lib" in path.parts:
            continue
        for name, params in SIGNATURE.findall(strip(path.read_text())):
            regs = {}
            for i, param in enumerate(params.split(",")[:4]):
                m = re.fullmatch(r"\s*(?:const\s+)?struct\s+(\w+)\s*\*\s*\w+\s*", param)
                if m:
                    reg = f"r{i}"
                    regs[reg] = Address(m[1], reg, 0)
                elif not re.fullmatch(r"\s*(?:const\s+)?(?:void|[us](?:8|16|32)|int|unsigned)\s*\*?\s*\w*\s*", param):
                    break  # the following argument may not occupy the next register
            out[name] = (str(path), regs)
    return out


def layout_fields(layout):
    if "targets" in layout:
        return {(row["owner"], row["offset"], row["width"]): row["field"] for row in layout["targets"]}
    fields = {}
    for row in layout["rows"]:
        for owner, member in row["members"].items():
            width = {"u32": 4, "u16": 2, "u8": 1}.get(member["ctype"])
            if width and "flags" in member["name"].lower():
                fields[owner, row["off"], width] = f"{owner}.{member['name']}"
    # Only classes with a proven first-member base qualify (no offset-only guesses).
    parents = {}
    for path in Path("include").rglob("*.h"):
        parents.update(re.findall(r"struct (\w+)\s*\{\s*struct (RoomActor|ChaseActor)\s+\w+;",
                                  strip(path.read_text())))
    for child in parents:
        for (owner, off, width), field in list(fields.items()):
            if owner == "RoomActor":
                fields[child, off, width] = field
    return fields


def add(a, b, pc):
    if isinstance(b, Address):
        a, b = b, a
    if isinstance(a, Address) and isinstance(b, Constant):
        return Address(a.owner, a.arg, a.offset + b.value)
    if isinstance(a, Constant) and isinstance(b, Constant):
        return Constant((a.value + b.value) & 0xffffffff, min(a.first, b.first), pc)
    return None


def sample(lines, rom, funcs, joins, sources, fields, globals_):
    regs, rows = {}, {}
    name = path = ""
    cc = None
    for line in lines:
        m = re.match(r"\s*([0-9a-f]+):\s+(\S+)\s*(.*)", line)
        if not m:
            continue
        pc, op, operands = int(m[1], 16), m[2].split(".")[0], m[3].split("@")[0].strip()
        if pc in funcs:
            name = funcs[pc]
            path, args = sources.get(name, ("", {}))
            regs, cc = args.copy(), None
        elif pc in joins:
            regs, cc = {}, None
        if not path:
            continue
        args = operands.split(", ")
        dest = args[0]

        def operand(arg):
            return Constant(int(arg[1:], 0), pc, pc) if arg.startswith("#") else regs.get(arg)

        if op in ("beq", "bne"):
            if cc:
                rows[cc.pc] = (name, path, "test", cc)
            continue
        if op in ("bl", "blx"):
            for reg in ("r0", "r1", "r2", "r3", "ip", "lr"):
                regs.pop(reg, None)
            cc = None
            continue
        if op in ("b", "bx", "pop", "ldmia", "stmia") or dest == "pc" or op.startswith("."):
            regs, cc = {}, None
            continue
        if op in ("ldr", "ldrh", "ldrb", "str", "strh", "strb"):
            mem = MEMORY.fullmatch(operands)
            if not mem:
                regs.pop(dest, None)
                continue
            off = operand(mem[3] or "#0")
            width = WIDTH[op[3:]]
            if mem[2] == "pc" and isinstance(off, Constant) and op == "ldr":
                value, = struct.unpack_from("<I", rom, ((pc + 4) & ~3) + off.value - BASE)
                regs[dest] = Constant(value, pc, pc)
                continue
            base = regs.get(mem[2])
            loc = add(base, off, pc)
            if isinstance(loc, Constant):
                loc = loc.value
            if op.startswith("ldr"):
                field = globals_.get((loc, width))
                if isinstance(loc, Address):
                    field = fields.get((loc.owner, loc.offset, width))
                regs[dest] = Word(field, (loc, width), pc, pc) if field else None
            elif isinstance(result := regs.get(dest), Result) and result.word.address == (loc, width):
                kind = {"orrs": "set", "bics": "clear", "ands": "and-store"}.get(result.opcode)
                if kind:
                    rows[result.pc] = (name, path, kind, result)
            continue
        if op in ("cmp", "tst", "ands", "orrs", "bics"):
            a, b = map(operand, args[:2])
            if op == "cmp":
                cc = a if isinstance(a, Result) and isinstance(b, Constant) and b.value == 0 else None
                continue
            maskreg = args[1]
            if isinstance(a, Constant) and op != "bics":
                a, b, maskreg = b, a, dest
            cc = None
            if isinstance(a, Word) and isinstance(b, Constant):
                cc = Result(a, b, pc, op, maskreg)
            if op != "tst":
                if op == "orrs" and isinstance(a, Word) and isinstance(b, Word) and {a.field, b.field} == PAUSE:
                    regs[dest] = Word("engine|scene pause", None, min(a.loaded, b.loaded), max(a.last, b.last))
                else:
                    regs[dest] = cc
            continue
        if op.startswith("b") or op == "push":
            continue
        if op == "mov" or op.endswith("s"):
            if op.endswith("s"):
                cc = None
            result = None
            if op in ("mov", "movs"):
                result = operand(args[1])
            elif op == "adds":
                a, b = map(operand, args[-2:])
                result = a if args[-1] == "#0" else add(a, b, pc)
            elif op in ("negs", "mvns") and isinstance(a := operand(args[1]), Constant):
                value = -a.value if op == "negs" else ~a.value
                result = Constant(value & 0xffffffff, a.first, pc)
            elif op == "lsls" and len(args) == 3 and isinstance(a := operand(args[1]), Constant) and args[2].startswith("#"):
                result = Constant((a.value << int(args[2][1:], 0)) & 0xffffffff, a.first, pc)
            regs[dest] = result
        else:
            regs.pop(dest, None)
            cc = None
    return rows


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("layouts", type=Path)
    args = ap.parse_args()
    rom = Path("baserom.gba").read_bytes()
    with open("build/boktai3.elf", "rb") as f:
        if ELFFile(f).get_section_by_name(".text").data() != rom:
            sys.exit("ELF .text differs from the ROM")
    asm = Path("gen/code.s").read_text()
    funcs = {int(addr, 16): name for name, addr in re.findall(LABEL.pattern, asm, re.M)}
    joins = {int(addr, 16) for addr in re.findall(r"^_([0-9A-Fa-f]{8}):", asm, re.M)}
    layout = json.loads(args.layouts.read_text())
    globals_ = {(int(r["addr"], 16), r["width"]): r["field"] for r in layout.get("globals", [])}
    with open("symbols/ram.csv") as f:
        globals_.update({(int(r["addr"], 16), 4): r["name"] for r in csv.DictReader(f)
                         if r["name"] in PAUSE | {"gEnginePauseFlags"}})
    with subprocess.Popen(["arm-none-eabi-objdump", "-d", "--no-show-raw-insn", "--stop-address=0x0824dafc",
                           "build/boktai3.elf"], stdout=subprocess.PIPE, text=True) as process:
        rows = sample(process.stdout, rom, funcs, joins, signatures(), layout_fields(layout), globals_)
        if process.wait():
            sys.exit("objdump failed")
    shared = Counter((r.mask.ready, r.reg) for _, _, _, r in rows.values())
    writer = csv.writer(sys.stdout, delimiter="\t", lineterminator="\n")
    writer.writerow(("site", "function", "unit", "field", "op", "mask", "order", "load", "mask_ready", "uses"))
    for pc, (name, path, kind, r) in sorted(rows.items()):
        order = "mask" if r.mask.ready < r.word.loaded else "word" if r.word.last < r.mask.first else "interleaved"
        writer.writerow((f"{pc:08X}", name, path, r.word.field, kind, hex(r.mask.value), order,
                         f"{r.word.loaded:08X}", f"{r.mask.ready:08X}", shared[r.mask.ready, r.reg]))


if __name__ == "__main__":
    main()
