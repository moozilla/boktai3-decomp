"""Patch a copy of the ROM: new script procedures, strings and cutscene tables.

The base is baserom.gba with the fan translation's IPS applied (its English
font; ../boktai3trans/ips/latest_translation.ips). New data goes in the free
space at the end of the ROM (zeros from 0x08F00020; the translation uses the
start of it), so nothing existing moves:

* procedures: unused slots of the script image's procedure table (11,377 of
  16,992 are 0); a slot holds the offset of its block from the script body,
  and the block can lie anywhere after the body;
* strings: an existing string id is pointed at new text (the entry table
  holds offsets from the bank, bit 31 marking a string), so pick ids the
  showcase never otherwise shows;
* cutscenes: an entry of gEventBytecodePtrs (08E8755C) is pointed at a new
  table of steps and messages (docs/CUTSCENES.md).

Output is ROM-derived: write it under build/ only.
"""
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.join(ROOT, "tools", "assets"))
import gcx  # noqa: E402

TRANSLATION = os.path.join(os.path.dirname(ROOT), "boktai3trans", "ips", "latest_translation.ips")
BASE = 0x08000000
FREE = 0x08F10000
EVENTS = 0x08E8755C
STRING_TABLE = 0x08D6B1F8
STRING_BANK = 0x08D74DDC


def ips(rom, patch):
    rom = bytearray(rom)
    p = open(patch, "rb").read()
    assert p[:5] == b"PATCH"
    i = 5
    while p[i:i + 3] != b"EOF":
        off, n = int.from_bytes(p[i:i + 3], "big"), int.from_bytes(p[i + 3:i + 5], "big")
        i += 5
        if n == 0:
            rl, v = int.from_bytes(p[i:i + 2], "big"), p[i + 2]
            i += 3
            data = bytes([v]) * rl
        else:
            data = p[i:i + n]
            i += n
        if off + len(data) > len(rom):
            rom.extend(b"\0" * (off + len(data) - len(rom)))
        rom[off:off + len(data)] = data
    return rom


class Rom:
    def __init__(self, translated=True):
        rom = open(os.path.join(ROOT, "baserom.gba"), "rb").read()
        self.rom = ips(rom, TRANSLATION) if translated and os.path.exists(TRANSLATION) else bytearray(rom)
        self.img = gcx.Image(bytes(self.rom))
        self.free = FREE
        self.slots = [i for i, w in enumerate(self.img.procs) if not w and i > 16000]

    def alloc(self, data, align=4):
        self.free = (self.free + align - 1) & ~(align - 1)
        a = self.free
        o = a - BASE
        assert all(b == 0 for b in self.rom[o:o + len(data)]), f"{a:08X} is not free"
        self.rom[o:o + len(data)] = data
        self.free += len(data)
        return a

    def u32(self, a, v=None):
        o = a - BASE
        if v is None:
            return struct.unpack_from("<I", self.rom, o)[0]
        struct.pack_into("<I", self.rom, o, v)

    def new_proc_id(self):
        """A free slot; the call id (proc_N in scripts) is slot + 1."""
        return self.slots.pop(0) + 1

    def set_proc(self, pid, code):
        a = self.alloc(code, 1)
        off = (a - BASE) - self.img.body
        assert 0 < off < 0x1000000
        table = gcx.IMAGE + 4  # the procedure table follows the timestamp
        self.u32(table + 4 * (pid - 1), off)
        return a

    def set_string(self, sid, text):
        """Point string sid at new text (ASCII with the bank's <TAGS>, NUL-terminated)."""
        a = self.alloc(text.encode("ascii") + b"\0", 1)
        self.u32(STRING_TABLE + 4 * sid, 0x80000000 | (a - STRING_BANK))

    def set_event(self, n, steps):
        """Point cutscene n at a table: steps = [[message bytes, ...], ...]."""
        step_ptrs = []
        for msgs in steps:
            ptrs = [self.alloc(m, 4) for m in msgs]
            step_ptrs.append(self.alloc(b"".join(struct.pack("<I", p) for p in ptrs) + b"\0\0\0\0"))
        table = self.alloc(b"".join(struct.pack("<I", p) for p in step_ptrs) + b"\0\0\0\0")
        self.u32(EVENTS + 4 * n, table)

    def save(self, path):
        open(path, "wb").write(bytes(self.rom))


def message(to, kind, cmd, args=(), ends=False):
    args = list(args)
    return struct.pack("<HBBHBB", to, kind, 1 if ends else 0, 0, cmd, len(args)) + \
        b"".join(struct.pack("<h", a if a < 0x8000 else a - 0x10000) for a in args)


def _hp_mods():
    sys.path.insert(0, os.path.join(ROOT, "tools"))
    import archive
    import lz77
    return archive, lz77


def add_paths(r, hp_hash, points):
    """Give terrain resource hp_hash extra one-point patrol paths (a spawner's
    path index puts an enemy at its path's first point; docs/ASSETS.md, HP).
    points: [(x, z, direction)]. Returns the index of the first new path."""
    archive, lz77 = _hp_mods()
    for label, t, base, ents in archive.archives(bytes(r.rom)):
        if t != 0xAE6C:
            continue
        for i, (h, a, s) in enumerate(ents):
            if h != hp_hash:
                continue
            raw = bytes(r.rom[a - BASE:a - BASE + s])
            data = lz77.decompress(raw, 0)[0] if raw[:1] == b"\x10" else raw
            k = data.find(b"HP\0\0")
            pre, b = data[:k], bytearray(data[k:])
            grid_o, box_o, path_o, nav_o = struct.unpack_from("<4I", b, 4)
            n, = struct.unpack_from("<I", b, path_o)
            old = [struct.unpack_from("<HH", b, path_o + 4 + j * 4) for j in range(n)]
            paths = [bytes(b[path_o + off:path_o + off + 8 * pts]) for pts, off in old]
            paths += [struct.pack("<HHBBH", x, z, 255, d, 0) for x, z, d in points]
            head = 4 + 4 * len(paths)
            sec, body = bytearray(struct.pack("<I", len(paths))), bytearray()
            for p in paths:
                sec += struct.pack("<HH", len(p) // 8, head + len(body))
                body += p
            new_o = (len(b) + 3) & ~3
            b += b"\0" * (new_o - len(b)) + sec + body
            struct.pack_into("<I", b, 12, new_o)
            blob = lz77.compress(bytes(pre + b)) if raw[:1] == b"\x10" else bytes(pre + b)
            na = r.alloc(blob, 4)
            o = base - BASE
            count, ho, oo, do = struct.unpack_from("<IIII", r.rom, o)
            struct.pack_into("<I", r.rom, o + oo + 4 * i, na - base)
            return n
    raise KeyError(f"no HP resource {hp_hash:04X}")
