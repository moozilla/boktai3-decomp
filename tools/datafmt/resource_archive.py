"""Resource archive (found through the directory at 08614D6C by ScriptProc_Start).

Layout, offsets relative to the archive start:
  +0 u32 count, +4 u32 hash table, +8 u32 offset table, +C u32 data start;
  count sorted u16 name hashes, zero padding, count u32 entry offsets, zero
  padding, then the entries in hash order, each starting on a 4-byte boundary.
An entry spans to the next one; the last spans to the end of the range.

Source: an index file plus one binary file per entry beside it, all
ROM-derived and kept under build/res/ (extracted on demand):

    # archive comments
    header COUNT PAD1HEX PAD2HEX
    HASH FILE                  one line per entry, in table order

Entries may change size: offsets are recomputed and each entry is padded to a
4-byte boundary, so later entries and later data shift. Adding or removing
entries is not supported yet (the hash table would need re-sorting).
"""
import os
import struct

RESIZABLE = True
# the container is regenerated, but each entry is an opaque extracted .bin
# (informational; see datafmt/__init__.py)
SOURCE = "split"
ANCHORS = ()  # referenced only at its start (the directory record)
ROM_BASE = 0x08000000


def extract(rom, start, end, path):
    o = start - ROM_BASE
    count, ho, oo, do = struct.unpack_from("<4I", rom, o)
    if ho != 0x10:
        raise ValueError(f"archive {start:08X}: unexpected header")
    hashes = struct.unpack_from(f"<{count}H", rom, o + ho)
    offs = list(struct.unpack_from(f"<{count}I", rom, o + oo)) + [end - start]
    if offs[0] != do or any(a >= b or a % 4 for a, b in zip(offs, offs[1:])):
        raise ValueError(f"archive {start:08X}: offsets not increasing and aligned")
    pad1 = rom[o + ho + 2 * count:o + oo]
    pad2 = rom[o + oo + 4 * count:o + do]
    d = os.path.dirname(path)
    os.makedirs(d, exist_ok=True)
    lines = [f"# archive at {start:08X}; see tools/datafmt/resource_archive.py",
             f"header {count} {pad1.hex() or '-'} {pad2.hex() or '-'}"]
    for i, h in enumerate(hashes):
        name = f"{h:04X}.bin"
        if os.path.exists(os.path.join(d, name)):
            name = f"{h:04X}_{i}.bin"  # duplicate hash
        with open(os.path.join(d, name), "wb") as f:
            f.write(rom[o + offs[i]:o + offs[i + 1]])
        lines.append(f"{h:04X} {name}")
    with open(path, "w") as f:
        f.write("\n".join(lines) + "\n")


def encode_path(path, start, end):
    d = os.path.dirname(path)
    entries, header = [], None
    for ln in open(path):
        f = ln.split()
        if not f or f[0].startswith("#"):
            continue
        if f[0] == "header":
            header = (int(f[1]), bytes.fromhex(f[2].strip("-")), bytes.fromhex(f[3].strip("-")))
        else:
            entries.append((int(f[0], 16), os.path.join(d, f[1])))
    count, pad1, pad2 = header
    if count != len(entries):
        raise ValueError(f"{path}: header count {count} but {len(entries)} entries")
    hashes = [h for h, _ in entries]
    if hashes != sorted(hashes):
        raise ValueError(f"{path}: entry hashes must stay sorted")
    ho = 0x10
    oo = ho + 2 * count + len(pad1)
    do = oo + 4 * count + len(pad2)
    offs, pos, body = [], do, []
    for _, fn in entries:
        size = os.path.getsize(fn)
        offs.append(pos)
        body.append((fn, size, -size % 4))
        pos += size + -size % 4
    items = [(4, v, False) for v in (count, ho, oo, do)]
    items += [(2, h, False) for h in hashes] + [(1, b, False) for b in pad1]
    items += [(4, v, False) for v in offs] + [(1, b, False) for b in pad2]
    for fn, size, pad in body:
        items.append((size, fn, "incbin"))
        items += [(1, 0, False)] * pad
    return items
