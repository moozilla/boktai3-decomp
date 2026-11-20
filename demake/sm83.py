"""Two-pass assembler for the SM83 instructions used by the cartridge.

Labels are absolute; .local labels belong to the previous global label.
Numbers accept decimal and Python-style hex. No macro or host code evaluation.
"""
import re
import struct

R = "b c d e h l [hl] a".split()
RR = "bc de hl sp".split()
COND = ["nz", "z", "nc", "c"]
ALU = "add adc sub sbc and xor or cp".split()
BITS = "rlc rrc rl rr sla sra swap srl".split()
SIMPLE = {"nop": 0, "rlca": 7, "rrca": 15, "rla": 23, "rra": 31,
          "cpl": 47, "scf": 55, "ccf": 63, "halt": 118, "ret": 201,
          "reti": 217, "di": 243, "ei": 251, "jp hl": 233}


def assemble(source, constants=None, origin=0x150):
    symbols = {k.lower(): v for k, v in (constants or {}).items()}
    lines = []
    scope = ""
    for number, raw in enumerate(source.splitlines(), 1):
        line = raw.partition(";")[0].strip().lower()
        if not line:
            continue
        if ":" in line:
            label, line = (s.strip() for s in line.split(":", 1))
            if not label.startswith("."):
                scope = label
            else:
                label = scope + label
            lines.append((number, label, None))
        if line:
            line = re.sub(r"(?<!\w)\.(\w+)", lambda m: scope + "." + m[1], line)
            lines.append((number, None, line))

    for final in (False, True):
        out = bytearray()

        def value(expr):
            match = re.fullmatch(r"([\w.]+)(?:\s*([+-])\s*(0x[\da-f]+|\d+))?", expr)
            if not match:
                raise ValueError(f"invalid expression {expr}")
            name, sign, delta = match.groups()
            if name[0].isdigit():
                n = int(name, 0)
            else:
                n = symbols[name] if final else symbols.get(name, 0)
            return n + (int(delta, 0) * (-1 if sign == "-" else 1) if delta else 0)

        for number, label, line in lines:
            pc = origin + len(out)
            if label is not None:
                if not final:
                    if label in symbols:
                        raise ValueError(f"duplicate label {label}")
                    symbols[label] = pc
                continue
            try:
                out.extend(encode(line, value, pc, final))
            except (KeyError, ValueError, IndexError, struct.error) as exc:
                raise ValueError(f"line {number}: {line}: {exc}") from exc
    return bytes(out), symbols


def encode(line, val, pc, final):
    if line in SIMPLE:
        return [SIMPLE[line]]
    op, _, args = line.partition(" ")
    args = [s.strip() for s in args.split(",")]
    a, b = args[0], args[-1]

    def word(code, x):
        return bytes([code]) + struct.pack("<H", val(x))

    if op == "db":
        return [val(s) for s in args]
    if op == "dw":
        return b"".join(struct.pack("<H", val(s)) for s in args)
    if op == "ld":
        if a in R and b in R:
            if a == b == "[hl]":
                raise ValueError("memory-to-memory load")
            return [0x40 + R.index(a) * 8 + R.index(b)]
        indirect = ["[bc]", "[de]", "[hl+]", "[hl-]"]
        if a in indirect and b == "a":
            return [2 + indirect.index(a) * 16]
        if a == "a" and b in indirect:
            return [10 + indirect.index(b) * 16]
        if a in RR:
            return word(1 + RR.index(a) * 16, b)
        if a == "a" and b.startswith("["):
            return word(0xFA, b[1:-1])
        if a.startswith("[") and a != "[hl]" and b == "a":
            return word(0xEA, a[1:-1])
        if a in R:
            return [6 + R.index(a) * 8, val(b)]
    if op == "ldh":
        if a == "a":
            return [0xF0, val(b[1:-1])]
        if b == "a":
            return [0xE0, val(a[1:-1])]
    if op in ["inc", "dec"]:
        if a in R:
            return [4 + (op == "dec") + 8 * R.index(a)]
        return [3 + 8 * (op == "dec") + 16 * RR.index(a)]
    if op == "add" and a == "hl" and b in RR:
        return [9 + RR.index(b) * 16]
    if op in ALU:
        base = 8 * ALU.index(op)
        return [0x80 + base + R.index(b)] if b in R else [0xC6 + base, val(b)]
    if op in BITS:
        return [0xCB, 8 * BITS.index(op) + R.index(b)]
    if op in ["bit", "res", "set"]:
        return [0xCB, 64 * (1 + ["bit", "res", "set"].index(op)) + 8 * val(a) + R.index(b)]
    if op in ["push", "pop"]:
        return [0xC1 + 4 * (op == "push") + 16 * ["bc", "de", "hl", "af"].index(a)]
    if op == "ret":
        return [0xC0 + 8 * COND.index(a)]
    if op in ["jr", "jp", "call"]:
        base, direct = {"jr": (0x20, 0x18), "jp": (0xC2, 0xC3), "call": (0xC4, 0xCD)}[op]
        code = base + 8 * COND.index(a) if len(args) == 2 else direct
        if op == "jr":
            delta = val(b) - pc - 2
            if final and not -128 <= delta <= 127:
                raise ValueError(f"branch out of range ({delta})")
            return [code, delta & 255]
        return word(code, b)
    raise ValueError("unsupported instruction")


# DMG boot ROM authentication field, common to cartridges on this platform.
BOOT_FIELD = bytes.fromhex("ce ed 66 66 cc 0d 00 0b 03 73 00 83 00 0c 00 0d "
                           "00 08 11 1f 88 89 00 0e dc cc 6e e6 dd dd d9 99 "
                           "bb bb 67 63 6e 0e ec cc dd dc 99 9f bb b9 33 3e")


def cartridge(code, banks):
    count = 1 << max(1, len(banks).bit_length())
    if count > 256 or len(code) > 0x4000 - 0x150 or any(len(b) > 0x4000 for b in banks):
        raise ValueError("ROM bank overflow")
    rom = bytearray([0xFF]) * (count * 0x4000)
    rom[0x100:0x150] = bytes(0x50)
    rom[0x100:0x104] = bytes.fromhex("00 c3 50 01")
    rom[0x104:0x134] = BOOT_FIELD
    rom[0x134:0x13E] = b"BOKTAI DMG"
    rom[0x147] = 0x1B  # MBC5 with battery-backed SRAM
    rom[0x149] = 2     # one 8 KiB RAM bank
    rom[0x148] = count.bit_length() - 2
    rom[0x14A] = 1
    rom[0x14D] = (-sum(rom[0x134:0x14D]) - 25) & 255
    rom[0x150:0x150 + len(code)] = code
    for i, data in enumerate(banks, 1):
        rom[i * 0x4000:i * 0x4000 + len(data)] = data
    rom[0x14E:0x150] = struct.pack(">H", sum(rom) & 65535)
    return rom
