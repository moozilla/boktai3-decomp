"""Script text bank: the u32 entry table and the bank of strings and blobs it indexes.

The range starts with the script image's four section offsets (gcx, loaded by
sub_0821AE04), relative to their own address: the body (+0), the entry table
(+0x14), the bank and a fourth word before the body; then a zero word. The body and fourth word
lie past the bank, so the encoder adds the bank's size change to them.

Layout (Text_LookupString, 0821ADD4): N u32 words, then the bank. Each word is
an offset from the bank start; bit 31 marks a NUL-terminated string, clear
marks a binary blob (tables read by script code). Entries sit in the bank in
table order, back to back; a blob keeps its original offset modulo 4, so the
strings before it are followed by zero padding.

Source file, one line per entry (tab-separated, UTF-8):

    ID  T  TEXT          string; escapes \\\\ \\n \\t \\{ \\xNN, {XXXX} glyph code
    ID  B  ALIGN  HEX    blob; ALIGN is the original offset modulo 4

Glyphs (lead byte 0x80-0x85 + second byte) print as the character from
data/charmap.tsv, or {XXXX} when the map has none. The bank is ROM-derived, so
its source lives under build/text/ and is extracted on demand (see docs/DATA.md).
Strings may change length; the bank grows or shrinks (zero-padded to a multiple
of 4 bytes) and later data shifts.
"""
import os
import re
import struct

RESIZABLE = True
# how the source holds the bytes (tools/progress.py ROM data measure, docs/DATA_TRACKING.md)
SOURCE = "decoded"
TABLE = 0x08D6B1F8  # entry table, after the four section words
ANCHORS = (TABLE, 0x08D74DDC)  # loaded by Text_LookupString
ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
ROM_BASE = 0x08000000
BANK = ANCHORS[1]  # end of the table, start of the bank


def charmap():
    m = {}
    with open(os.path.join(ROOT, "data", "charmap.tsv"), encoding="utf-8") as f:
        for ln in f:
            if ln[:1] in "#i" or not ln.strip():
                continue
            i, c, _ = ln.rstrip("\n").split("\t")
            m[int(i, 16)] = c
    return m


def decode_string(raw, cmap):
    out, i = [], 0
    while i < len(raw):
        b = raw[i]
        if 0x80 <= b <= 0x85 and i + 1 < len(raw):
            g = (b - 0x80) << 8 | raw[i + 1]
            out.append(cmap.get(g, f"{{{g:04X}}}"))
            i += 2
            continue
        c = chr(b)
        if c == "\\":
            out.append("\\\\")
        elif c == "\n":
            out.append("\\n")
        elif c == "\t":
            out.append("\\t")
        elif c == "{":
            out.append("\\{")
        elif 0x20 <= b < 0x7F:
            out.append(c)
        else:
            out.append(f"\\x{b:02x}")
        i += 1
    return "".join(out)


TOKEN = re.compile(r"\\x([0-9a-fA-F]{2})|\\(.)|(\{[^}]*\})|(.)", re.S)


def encode_string(s, inverse):
    out = bytearray()
    for m in TOKEN.finditer(s):
        hx, esc, brace, ch = m.groups()
        if hx:
            out.append(int(hx, 16))
        elif esc:
            out += {"n": b"\n", "t": b"\t"}.get(esc, esc.encode("latin-1"))
        elif brace:
            if re.fullmatch(r"\{[0-9A-F]{4}\}", brace):
                g = int(brace[1:-1], 16)
            elif brace in inverse:
                g = inverse[brace]
            else:
                raise ValueError(f"unknown glyph name {brace}")
            out += bytes([0x80 + (g >> 8), g & 0xFF])
        elif ord(ch) < 0x80:
            out.append(ord(ch))
        elif ch in inverse:
            g = inverse[ch]
            out += bytes([0x80 + (g >> 8), g & 0xFF])
        else:
            raise ValueError(f"character {ch!r} is not in data/charmap.tsv")
    return bytes(out)


def decode(rom, start, end, names=None):
    sections = struct.unpack_from("<5I", rom, start - ROM_BASE)
    if sections[1:3] != (TABLE - start, BANK - start) or start + 20 != TABLE or sections[4]:
        raise ValueError("text bank: unexpected section words")
    if not end - start <= sections[3] < sections[0]:
        raise ValueError("text bank: body sections must follow the bank")
    start = TABLE
    first, = struct.unpack_from("<I", rom, start - ROM_BASE)
    assert first & 0x7FFFFFFF == 0, "first entry must start the bank"
    n = (BANK - start) // 4
    words = list(struct.unpack_from(f"<{n}I", rom, start - ROM_BASE))
    offs = [w & 0x7FFFFFFF for w in words]
    if any(x >= y for x, y in zip(offs, offs[1:])):
        raise ValueError("text bank: table offsets must increase")
    bank = rom[BANK - ROM_BASE:end - ROM_BASE]
    cmap = charmap()
    offs.append(len(bank))
    lines = []
    for i, w in enumerate(words):
        a, b = offs[i], offs[i + 1]
        chunk = bank[a:b]
        if w & 0x80000000:
            n = chunk.index(b"\0")
            lines.append(f"{i}\tT\t{decode_string(chunk[:n], cmap)}")
            pad = chunk[n + 1:]
            nxt_align = offs[i + 1] % 4 if i + 1 < len(words) and not words[i + 1] & 0x80000000 else None
            want = padding(a + n + 1, nxt_align)
            if pad != b"\0" * want:
                lines.append(f"{i}\tP\t{pad.hex()}")
        else:
            lines.append(f"{i}\tB\t{a % 4}\t{chunk.hex()}")
    head = (f"# script text bank; see tools/datafmt/text_bank.py for the format\n"
            f"# sections {sections[0]:#x} {sections[3]:#x} (body, fourth; relative, before resizing)\n")
    return head + "\n".join(lines) + "\n"


def padding(pos, align):
    if align is None:
        return 0
    return (align - pos) % 4


def encode(text, start, end):
    inverse = {c: g for g, c in charmap().items()}
    entries = []
    sections = None
    for ln in text.split("\n"):
        if ln.startswith("# sections "):
            sections = [int(x, 16) for x in ln.split()[2:4]]
        if not ln or ln.startswith("#"):
            continue
        f = ln.split("\t")
        i = int(f[0])
        if f[1] == "P":
            entries[-1][3] = bytes.fromhex(f[2])
            continue
        if i != len(entries):
            raise ValueError(f"text bank: entry {i} out of order")
        if f[1] == "T":
            entries.append(["T", encode_string("\t".join(f[2:]), inverse), None, None])
        else:
            entries.append(["B", bytes.fromhex(f[3]), int(f[2]), None])
    if len(entries) * 4 != BANK - TABLE:
        raise ValueError(f"text bank: {len(entries)} entries, table needs {(BANK - TABLE) // 4}")
    if sections is None:
        raise ValueError("text bank: missing '# sections' line")
    bank = bytearray()
    words = []
    for k, (kind, data, align, pad) in enumerate(entries):
        if kind == "T":
            words.append(0x80000000 | len(bank))
            bank += data + b"\0"
            if pad is not None:
                bank += pad
            else:
                nxt = entries[k + 1] if k + 1 < len(entries) else None
                bank += b"\0" * padding(len(bank), nxt[2] if nxt and nxt[0] == "B" else None)
        else:
            if len(bank) % 4 != align:
                raise ValueError(f"text bank: blob {k} misaligned (edit a P line before it)")
            words.append(len(bank))
            bank += data
    # Keep the size change a multiple of 4 so word-aligned data after the bank
    # (event bytecode pointer tables) stays aligned.
    size = TABLE - start + len(words) * 4 + len(bank)
    bank += b"\0" * ((end - start - size) % 4)
    delta = TABLE - start + len(words) * 4 + len(bank) - (end - start)
    head = [sections[0] + delta, TABLE - start, BANK - start, sections[1] + delta, 0]
    return [(4, w, False) for w in head + words] + [(1, b, False) for b in bank]
