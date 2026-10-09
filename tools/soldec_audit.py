#!/usr/bin/env python3
"""Read-only script research helpers; this is not a bytecode decompiler.

The compact-length envelope is verified against Boktai 3 sub_0821A66C;
the candidate-name hash against sub_08219BD8. See docs/PROCESS_REVIEW.md.
All offsets are FILE offsets, never GBA bus addresses. JSON output generated
from a ROM belongs in ignored build/. No symbols are changed by this tool.
"""
import argparse
import json
import re
import struct
from pathlib import Path


def strcode(word):
    """16-bit rotate-left-five/add-byte; a hash match is not a unique name."""
    value = 0
    raw = word.encode("ascii")
    if b"\0" in raw:
        raise ValueError("names must not contain NUL")
    for byte in raw:
        value = (((value << 5) | (value >> 11)) + byte) & 0xFFFF
    return value


def checked_slice(data, start, size, end=None):
    limit = len(data) if end is None else end
    if start < 0 or size < 0 or limit > len(data) or start + size > limit:
        raise ValueError(f"truncated/out-of-range input at file offset 0x{start:X}")
    return data[start:start + size]


def compact_length(data, offset, end=None):
    """Return (payload_start, payload_length), including extended D/E/F forms."""
    code = checked_slice(data, offset, 1, end)[0]
    length = code & 15
    width = length - 12 if length >= 13 else 0
    if width:
        length = int.from_bytes(checked_slice(data, offset + 1, width, end), "little")
    return offset + 1 + width, length


def packets(data, start, end, max_records=10000):
    """Walk only known length-delimited envelopes; do not interpret payloads.

    Caller must provide a known boundary. End bytes stop the walk. Nested
    payloads are opaque, so success proves bounds only, not valid semantics.
    """
    checked_slice(data, start, end - start)
    rows = []
    offset = start
    kinds = {0x30: "expression", 0x50: "keyword", 0x60: "native-command",
             0x70: "procedure-call", 0x80: "block"}
    while offset < end:
        if len(rows) >= max_records:
            raise ValueError("record limit reached")
        opcode = data[offset]
        if opcode == 0:
            return {"records": rows, "stop": "end-marker", "next_offset": offset + 1}
        kind = kinds.get(opcode & 0xF0)
        if kind is None:
            raise ValueError(f"unsupported envelope 0x{opcode:02X} at 0x{offset:X}")
        payload, length = compact_length(data, offset, end)
        checked_slice(data, payload, length, end)
        row = {"offset": offset, "kind": kind, "payload": payload,
               "payload_length": length, "end": payload + length}
        if opcode & 0xF0 in (0x60, 0x70):
            row["id"] = int.from_bytes(checked_slice(data, payload, 2, payload + length), "little")
        rows.append(row)
        offset = payload + length
    return {"records": rows, "stop": "supplied-boundary", "next_offset": offset}


def function_starts(assembly):
    declared = set(re.findall(
        r"^\s*(?:thumb_func_start|arm_func_start|non_word_aligned_thumb_func_start)\s+(\S+)",
        assembly, re.M))
    labels = re.findall(r"^([\w.$]+):\s*@\s*0x([0-9A-Fa-f]{8})\b", assembly, re.M)
    return {int(address, 16) for name, address in labels if name in declared}


def audit_table(data, offset, count, words=(), starts=None):
    """Audit an explicitly bounded table of LE {u32 ID, u32 Thumb target}."""
    raw = checked_slice(data, offset, count * 8)
    names = {}
    for word in sorted(set(words)):
        names.setdefault(strcode(word), []).append(word)
    rows = []
    for i, (identifier, pointer) in enumerate(struct.iter_unpack("<II", raw)):
        address = pointer & ~1
        rows.append({
            "entry_offset": offset + i * 8,
            "id": identifier,
            "target": address,
            "thumb_rom_target": bool(pointer & 1) and 0x08000000 <= address < 0x08000000 + len(data),
            "recognized_function_start": None if starts is None else address in starts,
            "candidate_names": names.get(identifier, []),
            "name_status": "hash candidates only; verify behavior and collisions",
        })
    return rows


def number(value):
    return int(value, 0)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    h = commands.add_parser("hash", help="hash candidate ASCII names without a ROM")
    h.add_argument("words", nargs="+")
    t = commands.add_parser("table", help="inspect an explicitly bounded command table")
    t.add_argument("file", type=Path)
    t.add_argument("--offset", type=number, required=True)
    t.add_argument("--count", type=number, required=True)
    t.add_argument("--word", action="append", default=[])
    t.add_argument("--names-file", type=Path, help="one candidate name per line")
    t.add_argument("--asm", type=Path, help="generated assembly, to flag absent starts")
    p = commands.add_parser("packets", help="skip opaque envelopes at known boundaries")
    p.add_argument("file", type=Path)
    p.add_argument("--start", type=number, required=True)
    p.add_argument("--end", type=number, required=True)
    p.add_argument("--max-records", type=int, default=10000)
    args = parser.parse_args()
    try:
        if args.command == "hash":
            result = [{"name": word, "id": strcode(word)} for word in args.words]
        elif args.command == "table":
            words = args.word
            if args.names_file:
                words += [x.strip() for x in args.names_file.read_text().splitlines() if x.strip()]
            starts = function_starts(args.asm.read_text()) if args.asm else None
            result = audit_table(args.file.read_bytes(), args.offset, args.count, words, starts)
        else:
            result = packets(args.file.read_bytes(), args.start, args.end, args.max_records)
    except (ValueError, OSError) as error:
        parser.exit(2, f"error: {error}\n")
    print(json.dumps(result, indent=2))


if __name__ == "__main__":
    main()
