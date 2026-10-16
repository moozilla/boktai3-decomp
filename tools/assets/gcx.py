#!/usr/bin/env python3
"""Disassemble the compiled script image (the `gcx` resource, 08D5A85C).

  gcx.py info                 image layout
  gcx.py dis [PROC...]        disassemble procedures (default: all), with
                              recovered names (gcl/names) and English comments
  gcx.py chara                every `chara` command: procedure, key, callback
                              (from data/actor_registry.csv)

Layout (loaded by sub_0821AE04): a u32 timestamp, a procedure table of u32
words ending in 0xFFFFFFFF (index = procedure id; low 24 bits = block offset
from the body, top byte = a flag; 0 = unused), then a section header of four
u32 offsets: body, string offsets (Text_LookupString's table), string data,
and a fourth. The body starts with its u32 size.

Instructions and operands follow Script_DecodeOperand (0821A6F0) and the block
dispatcher (0821AF2C): the high nibble of the first byte selects the kind and
the low nibble the payload length (13/14/15: a 1/2/3-byte little-endian length
follows, sub_0821A66C). Command ids are hashes of command names
(gcl/names/command.csv; tools/assets/gclnames.py). Inline strings are EUC-JP. Output is ROM-derived; keep dumps under build/.
"""
import csv
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gclnames  # noqa: E402
from romlib import ROOT, load_rom  # noqa: E402

ROM_BASE = 0x08000000
IMAGE = 0x08D5A85C
COMMANDS = {k: r["name"] for k, r in gclnames.best("command").items()}
KEYED = ("chara", "command")  # the first operand is a registry key (gcl/names/chara.csv)
_keys = None  # {key: name row} while disassembling with names
_prints = {}  # {print text: English} while disassembling with names


class Image:
    def __init__(self, rom):
        self.rom = rom
        o = IMAGE - ROM_BASE
        self.timestamp, = struct.unpack_from("<I", rom, o)
        o += 4
        self.procs = []
        while True:
            w, = struct.unpack_from("<I", rom, o)
            o += 4
            if w == 0xFFFFFFFF:
                break
            self.procs.append(w)
        self.base = o
        self.sections = struct.unpack_from("<4I", rom, o)
        body = o + self.sections[0]
        self.body_size, = struct.unpack_from("<I", rom, body)
        self.body = body + 4

    def proc_address(self, pid):
        w = self.procs[pid]
        return self.body + (w & 0xFFFFFF) if w else None


def length(b, p):
    """(payload start, payload length) of the item at p (sub_0821A66C)."""
    n = b[p] & 15
    if n == 13:
        return p + 2, b[p + 1]
    if n == 14:
        return p + 3, b[p + 1] | b[p + 2] << 8
    if n == 15:
        return p + 4, b[p + 1] | b[p + 2] << 8 | b[p + 3] << 16
    return p + 1, n


def operand(b, p):
    """(text, next position) for one operand (Script_DecodeOperand)."""
    op = b[p]
    fam = op & 0xF0
    if (fam & 0xC0) == 0xC0:
        return str((op & 0x3F) - 1), p + 1
    if fam == 0:
        if op == 0:
            return "<end>", p + 1
        if op in (8, 6):
            return f"{b[p + 1] | b[p + 2] << 8:#x}", p + 3
        if op == 1:
            v = b[p + 1] | b[p + 2] << 8
            return str(v - 0x10000 if v & 0x8000 else v), p + 3
        if op in (9, 10, 13):
            return f"{struct.unpack_from('<I', b, p + 1)[0]:#x}", p + 5
        if op in (2, 3, 4):
            return str(b[p + 1]), p + 2
        if op == 7:
            n = b[p + 1]
            s = bytes(b[p + 2:p + 2 + n]).split(b"\0")[0].decode("euc_jp", "backslashreplace")
            return "'" + s.replace("\\", "\\\\").replace("'", "\\'").replace("\n", "\\n") + "'", p + 2 + n
        if op == 14:
            return f"text#{b[p + 1] | b[p + 2] << 8}", p + 3
        return f"?{op:02x}", p + 1
    if fam in (0x10, 0x20):
        d = b[p] << 24 | b[p + 1] << 16 | b[p + 2] << 8 | b[p + 3]
        region = {0x800000: "gs", 0x100000: "r700"}.get(d & 0xF00000, "r708")
        text = f"{region}[{d & 0xFFFF:#x}]:t{(d >> 24) & 15}"
        q = p + 4
        if fam == 0x20:
            a, q = operand(b, q)
            i, q = operand(b, q)
            text += f"({a},{i})"
        return text, q
    if fam == 0x40:
        if op & 15 == 15:
            return f"arg{b[p + 1] + 15}", p + 2
        return f"arg{op & 15}", p + 1
    if fam == 0x90:
        return f"local{op & 15}", p + 1
    start, n = length(b, p)
    if fam == 0x50:
        inner = operands(b, start + 1, start + n)
        return f"{chr(b[start])}:{' '.join(inner)}", start + n
    if fam == 0x30:
        return f"({expression(b, start, start + n)})", start + n
    if fam == 0x80:  # a nested block used as an operand (switch cases, keyword values)
        inner = [t.split("  # ", 1) for _, t in instructions(b, start, start + n)]
        return "{ " + "; ".join(t[0] + (f" /* {t[1]} */" if len(t) > 1 else "") for t in inner) + " }", start + n
    return f"?{op:02x}", start + n


OPS = {4: "+", 5: "-", 6: "*", 7: "/", 8: "%", 9: "<<", 10: ">>", 11: "==", 12: "!=",
       13: "<", 14: "<=", 15: ">", 16: ">=", 17: "|", 18: "&", 19: "^", 20: "||", 21: "&&",
       22: "="}
UNARY = {1: "-", 2: "!", 3: "~"}
KEYWORDS = {"e": "else", "i": "elif", "c": "case", "d": "default"}


def expression(b, p, end):
    """Infix text of an expression (sub_0821BA5C): operands are pushed, a byte
    0xA0 | op pops two and pushes the result (sub_0821B938), 0xA0 ends it.
    Unary operators (1-3) use only the top of the stack; 22 assigns."""
    stack = []
    while p < end:
        op = b[p]
        if op & 0xE0 == 0xA0:
            o = op & 0x1F
            if o == 0:
                break
            r = stack.pop() if stack else "?"
            if o in UNARY:
                stack.append(f"{UNARY[o]}{r}")
            else:
                left = stack.pop() if stack else "?"
                text = f"{left} {OPS.get(o, f'op{o}')} {r}"
                stack.append(text if o == 22 else f"({text})")
            p += 1
            continue
        t, p = operand(b, p)
        stack.append(t)
    text = " ".join(stack)
    return text[1:-1] if text.startswith("(") and text.endswith(")") and text.count("(") == 1 else text


def arglist(b, p):
    """(start, length) of a native command's argument list (sub_0821A6C0): one
    byte below 0x80, else two big-endian bytes masked with 0x7FFF."""
    if b[p] & 0x80:
        return p + 2, ((b[p] << 8) | b[p + 1]) & 0x7FFF
    return p + 1, b[p]


def operands(b, p, end):
    out = []
    while p is not None and p < end:
        t, p = operand(b, p)
        out.append(t)
        if t == "<end>":
            break
    return out


def instructions(b, p, end, depth=0, out=None):
    """Decode a block body into (depth, text) lines; nested blocks are expanded."""
    out = [] if out is None else out
    while p < end:
        op = b[p]
        kind = op >> 4
        if op == 0:
            break
        start, n = length(b, p)
        if kind == 6:
            cid = b[start] | b[start + 1] << 8
            name = COMMANDS.get(cid, f"cmd_{cid:04X}")
            q, alen = arglist(b, start + 2)  # the argument list has its own length
            args, aend, note, first, line0 = [], q + alen, "", True, len(out)
            while q < aend:
                if b[q] & 0xF0 == 0x80:
                    s2, n2 = length(b, q)
                    args.append("{")
                    out.append((depth, f"{name} {' '.join(args)}"))
                    args = []
                    instructions(b, s2, s2 + n2, depth + 1, out)
                    out.append((depth, "}"))
                    q = s2 + n2
                    continue
                t, q = operand(b, q)
                if q is None:
                    break
                if first and name in KEYED and _keys is not None and t.startswith("0x"):
                    r = _keys.get(int(t, 16))
                    if r:
                        note = f"  # {int(t, 16):04X}" + (f" {r['english']}" if r["english"] else "")
                        t = r["name"]
                elif first and name == "print" and t[:1] == "'" and _prints.get(t[1:-1]):
                    note = "  # " + _prints[t[1:-1]]
                first = False
                args.append(t)
            if args or not out or out[-1] != (depth, "}"):
                out.append((depth, f"{name} {' '.join(args)}".rstrip()))
            # Keyword items after the argument list: `if` takes `e` (else) and
            # `i` (else if: condition, block); `switch` takes `c` (case: value,
            # block) and `d` (default).
            while q < start + n and b[q] & 0xF0 == 0x50:
                s2, n2 = length(b, q)
                k = chr(b[s2])
                label, r = [KEYWORDS[k] if name in ("if", "switch") and k in KEYWORDS else k + ":"], s2 + 1
                while r < s2 + n2:
                    if b[r] & 0xF0 == 0x80:
                        s3, n3 = length(b, r)
                        out.append((depth, " ".join(label + ["{"])))
                        label = []
                        instructions(b, s3, s3 + n3, depth + 1, out)
                        out.append((depth, "}"))
                        r = s3 + n3
                        continue
                    t, r = operand(b, r)
                    label.append(t)
                if label and out[-1][0] == depth and out[-1][1] != "}":
                    out[-1] = (depth, out[-1][1] + " " + " ".join(label))
                elif label:
                    out.append((depth, " ".join(label)))
                q = s2 + n2
            if note:
                out[line0] = (out[line0][0], out[line0][1] + note)
        elif kind == 7:
            pid = b[start] | b[start + 1] << 8
            out.append((depth, f"proc_{pid} {' '.join(operands(b, start + 2, start + n))}".rstrip()))
        elif kind == 3:
            out.append((depth, expression(b, start, start + n)))
        else:
            out.append((depth, f".byte {bytes(b[p:start + n]).hex()}"))
        p = start + n
    return out


def disassemble(img, pid, names=False):
    """(depth, text) lines of procedure slot pid. With names, registry keys
    after chara/command print as their GCL names (gcl/names/chara.csv), the
    key and English in a trailing comment, and Japanese print strings get
    their translation (gcl/prints.csv); without, keys stay hex (for tools
    that parse the text)."""
    global _keys, _prints
    a = img.proc_address(pid)
    if a is None:
        return []
    b = img.rom
    start, n = length(b, a)
    _keys = gclnames.best("chara") if names else None
    _prints = gclnames.prints() if names else {}
    try:
        return instructions(b, start, start + n)
    finally:
        _keys, _prints = None, {}


def main():
    rom = load_rom()
    img = Image(rom)
    cmd = sys.argv[1] if len(sys.argv) > 1 else "info"
    if cmd == "info":
        used = sum(1 for w in img.procs if w)
        print(f"timestamp {img.timestamp:#x}; {len(img.procs)} procedure slots, {used} used; "
              f"body {img.body + ROM_BASE:08X} ({img.body_size:#x} bytes); sections "
              + ", ".join(f"{img.base + s + ROM_BASE:08X}" for s in img.sections))
    elif cmd == "dis":
        pids = [int(x, 0) for x in sys.argv[2:]] or range(len(img.procs))
        for pid in pids:
            lines = disassemble(img, pid, names=True)
            if lines:
                print(f"proc {pid} (flag {img.procs[pid] >> 24}):")
                for d, t in lines:
                    print("    " * (d + 1) + t)
    elif cmd == "chara":
        import re
        reg = {}
        with open(os.path.join(ROOT, "data", "actor_registry.csv")) as f:
            for r in csv.DictReader(f):
                reg[int(r["key"], 16)] = (r["callback"], r["name"])
        for pid in range(len(img.procs)):
            for d, t in disassemble(img, pid):
                for m in re.finditer(r"\bchara (\S+)( [^;}]*)?", t):
                    k = m.group(1)
                    key = int(k, 0) if k.startswith("0x") or k.isdigit() else None
                    cb = reg.get(key, ("?", "?")) if key is not None else ("?", "?")
                    print(f"{pid}\t{m.group(0).strip()}\t{cb[0]}\t{cb[1]}")


if __name__ == "__main__":
    main()
