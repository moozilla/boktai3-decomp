#!/usr/bin/env python3
"""Verify which ROM data the project's tools can round-trip, and record it.

  build/venv/bin/python tools/data_roundtrip.py          check everything, write the manifest
  build/venv/bin/python tools/data_roundtrip.py --dry    check and summarize only

Reads the ROM (local only). For every block a codec supports it decodes the
bytes to an editable, meaningful form (PNG for pixels, JSON with named fields,
WAV for samples, CSV, text), serializes that form, parses it back, re-encodes
it and compares with the ROM. Blocks that come back byte-exact are written to
symbols/data_roundtrip.csv (ranges and tool names only, no ROM content), which
tools/romdata.py and tools/progress.py read without the ROM
(docs/DATA_TRACKING.md).

Levels:
  exact       uncompressed: the editable form re-encodes to the stored bytes
  lz-exact    LZ77: the decompressed content round-trips, and tools/lz77.py
              recompresses it to the stored bytes
  lz-content  LZ77: the decompressed content round-trips; recompression differs

Codecs (tool column): datafmt (text bank, actor registry, archive containers),
palette (92B3 palettes, the 9B1B pool), font (A635), tileset (the CEEF library),
map (C091 MP maps), sprite (CB05 packages), m4a (PCM and wave samples,
voicegroups, the song table). Unknown fields stay as raw named numbers in the
JSON, as in data/ (docs/DATA.md); a block whose bytes fall outside every known
section (nonzero gaps) is not counted.
"""
import argparse
import csv
import io
import json
import os
import struct
import sys
import tempfile
import wave

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
for p in ("tools", "tools/assets", "tools/studio"):
    sys.path.insert(0, os.path.join(ROOT, p))

import numpy as np  # noqa: E402
from PIL import Image  # noqa: E402

import archive  # noqa: E402
import lz77  # noqa: E402
import m4a  # noqa: E402
from romlib import load_rom  # noqa: E402

ROM_BASE = 0x08000000
OUT = os.path.join(ROOT, "symbols", "data_roundtrip.csv")
GRAY = [v for i in range(16) for v in (i * 17,) * 3]


# ---------- editable forms ----------

def via_json(obj):
    return json.loads(json.dumps(obj))


def png_roundtrip(pixels):
    """2-D array of palette indices -> indexed PNG -> back."""
    img = Image.fromarray(np.asarray(pixels, dtype=np.uint8), "P")
    img.putpalette(GRAY + [0] * (768 - len(GRAY)))
    f = io.BytesIO()
    img.save(f, "PNG")
    f.seek(0)
    return np.array(Image.open(f))


def tiles_to_pixels(data, n, tw=1, th=1, cols=16):
    """4bpp tiles (low nibble first) -> a sheet of n glyphs of tw x th tiles, cols per row."""
    a = np.frombuffer(data[:n * tw * th * 32], dtype=np.uint8)
    px = np.empty(a.size * 2, dtype=np.uint8)
    px[0::2], px[1::2] = a & 15, a >> 4
    g = px.reshape(n, th, tw, 8, 8).transpose(0, 1, 3, 2, 4).reshape(n, th * 8, tw * 8)
    rows = -(-n // cols)
    g = np.concatenate([g, np.zeros((rows * cols - n, th * 8, tw * 8), np.uint8)])
    return g.reshape(rows, cols, th * 8, tw * 8).transpose(0, 2, 1, 3).reshape(rows * th * 8, cols * tw * 8)


def pixels_to_tiles(sheet, n, tw=1, th=1, cols=16):
    rows = sheet.shape[0] // (th * 8)
    g = sheet.reshape(rows, th * 8, cols, tw * 8).transpose(0, 2, 1, 3).reshape(-1, th * 8, tw * 8)[:n]
    px = g.reshape(n, th, 8, tw, 8).transpose(0, 1, 3, 2, 4).reshape(-1)
    return (px[0::2] | (px[1::2] << 4)).astype(np.uint8).tobytes()


def tiles_roundtrip(data, n, tw=1, th=1, cols=16):
    return pixels_to_tiles(png_roundtrip(tiles_to_pixels(data, n, tw, th, cols)), n, tw, th, cols)


def color(v):
    c = [v & 31, v >> 5 & 31, v >> 10 & 31]
    return c + [1] if v & 0x8000 else c


def uncolor(c):
    return c[0] | c[1] << 5 | c[2] << 10 | (0x8000 if len(c) > 3 and c[3] else 0)


def colors(b):
    return [color(v) for v in struct.unpack(f"<{len(b) // 2}H", b)]


def uncolors(cs):
    return struct.pack(f"<{len(cs)}H", *[uncolor(c) for c in cs])


def padded(content, size):
    """content then zero padding up to size, or None if content is longer."""
    return content + bytes(size - len(content)) if len(content) <= size else None


class Layout:
    """A byte buffer filled section by section; bytes no section wrote must be zero."""

    def __init__(self, size):
        self.b = bytearray(size)

    def put(self, at, data):
        self.b[at:at + len(data)] = data


# ---------- codecs: each returns the re-encoded bytes (or raises) ----------

def palette_92B3(raw):
    d = via_json({"header": list(struct.unpack_from("<10H", raw)), "colors": colors(raw[20:532])})
    return padded(struct.pack("<10H", *d["header"]) + uncolors(d["colors"]), len(raw))


def palette_pool(raw):
    count, unk = struct.unpack_from("<HH", raw)
    d = via_json({"unknown": unk, "palettes": [colors(raw[4 + 32 * i:36 + 32 * i]) for i in range(count)]})
    out = struct.pack("<HH", len(d["palettes"]), d["unknown"]) + b"".join(uncolors(p) for p in d["palettes"])
    return padded(out, len(raw))


def font(raw):
    count, b2, b3, o8, o16 = struct.unpack_from("<HBBII", raw)
    n16 = (len(raw) - o16) // 128
    d = via_json({"glyphs_8x16": count, "b2": b2, "b3": b3, "offset_8x16": o8, "offset_16x16": o16,
                  "glyphs_16x16": n16})
    small = tiles_roundtrip(raw[o8:o8 + count * 64], count, 1, 2)
    big = tiles_roundtrip(raw[o16:o16 + n16 * 128], n16, 2, 2)
    L = Layout(len(raw))
    L.put(0, struct.pack("<HBBII", d["glyphs_8x16"], d["b2"], d["b3"], d["offset_8x16"], d["offset_16x16"]))
    L.put(d["offset_8x16"], small)
    L.put(d["offset_16x16"], big)
    return bytes(L.b)


def tileset_library(raw):
    n, = struct.unpack_from("<H", raw)
    head = list(struct.unpack_from("<5H", raw, 2))
    T, I, D = struct.unpack_from("<III", raw, 0xC)
    sets = []
    for i in range(n):
        sid, k, o = struct.unpack_from("<HHI", raw, T + 8 * i)
        sets.append({"id": sid, "index_offset": o, "tiles": list(struct.unpack_from(f"<{k}H", raw, I + o))})
    ntiles = (len(raw) - D) // 32
    d = via_json({"header": head, "table": T, "index": I, "pool": D, "pool_tiles": ntiles, "tilesets": sets})
    L = Layout(len(raw))
    L.put(0, struct.pack("<H5HIII", len(d["tilesets"]), *d["header"], d["table"], d["index"], d["pool"]))
    for i, s in enumerate(d["tilesets"]):
        L.put(d["table"] + 8 * i, struct.pack("<HHI", s["id"], len(s["tiles"]), s["index_offset"]))
        L.put(d["index"] + s["index_offset"], struct.pack(f"<{len(s['tiles'])}H", *s["tiles"]))
    L.put(d["pool"], tiles_roundtrip(raw[D:D + 32 * ntiles], ntiles, cols=32))
    return bytes(L.b)


def grid_rows(b, at, w, h):
    g = np.frombuffer(b[at:at + 2 * w * h], dtype="<u2")
    if w % 16 == 0 and h % 16 == 0 and w and h:
        g = g.reshape(h // 16, w // 16, 16, 16).transpose(0, 2, 1, 3)
    return g.reshape(h, w).tolist()


def grid_bytes(rows, w, h):
    g = np.asarray(rows, dtype="<u2").reshape(h, w)
    if w % 16 == 0 and h % 16 == 0 and w and h:
        g = g.reshape(h // 16, 16, w // 16, 16).transpose(0, 2, 1, 3)
    return g.astype("<u2").tobytes()


def mp_map(b):
    """An MP map (decompressed): JSON with layers, metatiles and own tiles as PNG."""
    if b[:4] != b"MP\0\0":
        raise ValueError("not MP")
    nl, nt, nm, h0 = struct.unpack_from("<4H", b, 4)
    tab, tiles, defs = struct.unpack_from("<III", b, 0xC)
    layers = []
    for i in range(nl):
        w, h, name, off = struct.unpack_from("<HHII", b, tab + 12 * i)
        layers.append({"width": w, "height": h, "name": name, "offset": off, "grid": grid_rows(b, off, w, h)})
    metas = [[{"tile": e & 0x3FF, "hflip": e >> 10 & 1, "vflip": e >> 11 & 1, "bank": e >> 12}
              for e in struct.unpack_from("<4H", b, defs + 8 * i)] for i in range(nm)]
    d = via_json({"size": len(b), "tiles": nt, "h0": h0, "layer_table": tab, "tile_data": tiles,
                  "metatile_table": defs, "layers": layers, "metatiles": metas})
    L = Layout(d["size"])
    L.put(0, b"MP\0\0" + struct.pack("<4HIII", len(d["layers"]), d["tiles"], len(d["metatiles"]), d["h0"],
                                       d["layer_table"], d["tile_data"], d["metatile_table"]))
    for i, ly in enumerate(d["layers"]):
        L.put(d["layer_table"] + 12 * i, struct.pack("<HHII", ly["width"], ly["height"], ly["name"], ly["offset"]))
        L.put(ly["offset"], grid_bytes(ly["grid"], ly["width"], ly["height"]))
    if d["tile_data"]:
        L.put(d["tile_data"], tiles_roundtrip(b[tiles:tiles + 32 * nt], nt))
    L.put(d["metatile_table"], b"".join(struct.pack("<4H", *[e["tile"] | e["hflip"] << 10 | e["vflip"] << 11 | e["bank"] << 12
                                                              for e in m]) for m in d["metatiles"]))
    return bytes(L.b)


def sprite_package(raw):
    pal, nf, na, np_, nk, nt = struct.unpack_from("<6H", raw)
    of, oa, op, ok, ot = struct.unpack_from("<5I", raw, 0xC)
    frames = [dict(zip(("id", "parts", "unknown", "x", "y", "w", "h", "part_offset"),
                       struct.unpack_from("<HHIhhHHI", raw, of + 20 * i))) for i in range(nf)]
    anims = [dict(zip(("id", "keys", "key_offset"), struct.unpack_from("<HHI", raw, oa + 8 * i))) for i in range(na)]
    parts = []
    for i in range(np_):
        attr, x, y, t = struct.unpack_from("<HhhH", raw, op + 8 * i)
        parts.append({"attr": attr, "x": x, "y": y, "tile": t & 0xFFF, "bank": t >> 12})
    keys = [dict(zip(("frame", "duration"), struct.unpack_from("<HH", raw, ok + 4 * i))) for i in range(nk)]
    d = via_json({"palette": pal, "offsets": [of, oa, op, ok, ot], "frames": frames, "animations": anims,
                  "parts": parts, "keys": keys, "tiles": nt})
    of, oa, op, ok, ot = d["offsets"]
    L = Layout(len(raw))
    L.put(0, struct.pack("<6H5I", d["palette"], len(d["frames"]), len(d["animations"]), len(d["parts"]),
                         len(d["keys"]), d["tiles"], of, oa, op, ok, ot))
    for i, f in enumerate(d["frames"]):
        L.put(of + 20 * i, struct.pack("<HHIhhHHI", *[f[k] for k in ("id", "parts", "unknown", "x", "y", "w", "h", "part_offset")]))
    for i, a in enumerate(d["animations"]):
        L.put(oa + 8 * i, struct.pack("<HHI", a["id"], a["keys"], a["key_offset"]))
    for i, p in enumerate(d["parts"]):
        L.put(op + 8 * i, struct.pack("<HhhH", p["attr"], p["x"], p["y"], p["tile"] | p["bank"] << 12))
    for i, k in enumerate(d["keys"]):
        L.put(ok + 4 * i, struct.pack("<HH", k["frame"], k["duration"]))
    L.put(ot, tiles_roundtrip(raw[ot:ot + 32 * nt], nt))
    return bytes(L.b)


def hp_terrain(b):
    """HP terrain (decompressed, from "HP"): grid, boxes, paths, nav meshes (tools/assets/hp.py)."""
    if b[:4] != b"HP\0\0":
        raise ValueError("not HP")
    go, bo, po, no = struct.unpack_from("<4I", b, 4)
    g0, w, h, ox, oy = struct.unpack_from("<IHHhh", b, go)
    cells = [dict(zip(("height", "tile", "flags"), struct.unpack_from("<BBH", b, go + 12 + 4 * i))) for i in range(w * h)]
    boxes = [dict(zip(("x0", "z0", "x1", "z1", "y0", "y1", "name"), struct.unpack_from("<4HBBH", b, bo + 4 + 12 * i)))
             for i in range(struct.unpack_from("<I", b, bo)[0])]
    paths = []
    for i in range(struct.unpack_from("<I", b, po)[0]):
        n, off = struct.unpack_from("<HH", b, po + 4 + 4 * i)
        paths.append({"offset": off, "points": [dict(zip(("x", "z", "wait", "dir", "flags"),
                                                          struct.unpack_from("<HHBBH", b, po + off + 8 * k))) for k in range(n)]})
    meshes = []
    if no:
        for i in range(struct.unpack_from("<I", b, no)[0]):
            mo, = struct.unpack_from("<I", b, no + 4 + 4 * i)
            o = no + mo
            nr, npo, nx, ro, pto, xo = struct.unpack_from("<HHIIII", b, o)
            meshes.append({"offset": mo, "rect_offset": ro, "portal_offset": pto, "extra_offset": xo,
                           "rects": [{"x0": r[0], "z0": r[1], "x1": r[2], "z1": r[3], "portals": list(r[4:])}
                                     for r in (b[o + ro + 16 * k:o + ro + 16 * k + 16] for k in range(nr))],
                           "portals": [dict(zip(("x", "z", "u", "rect_a", "rect_b", "unknown"),
                                                struct.unpack_from("<HHHBBI", b, o + pto + 12 * k))) for k in range(npo)],
                           "extra": list(struct.unpack_from(f"<{nx}H", b, o + xo))})
    d = via_json({"size": len(b), "offsets": [go, bo, po, no], "grid_word": g0, "width": w, "height": h,
                  "origin": [ox, oy], "cells": cells, "boxes": boxes, "paths": paths, "nav": meshes})
    go, bo, po, no = d["offsets"]
    L = Layout(d["size"])
    L.put(0, b"HP\0\0" + struct.pack("<4I", go, bo, po, no))
    L.put(go, struct.pack("<IHHhh", d["grid_word"], d["width"], d["height"], *d["origin"]))
    L.put(go + 12, b"".join(struct.pack("<BBH", c["height"], c["tile"], c["flags"]) for c in d["cells"]))
    L.put(bo, struct.pack("<I", len(d["boxes"])) + b"".join(
        struct.pack("<4HBBH", *[x[k] for k in ("x0", "z0", "x1", "z1", "y0", "y1", "name")]) for x in d["boxes"]))
    L.put(po, struct.pack("<I", len(d["paths"])))
    for i, pa in enumerate(d["paths"]):
        L.put(po + 4 + 4 * i, struct.pack("<HH", len(pa["points"]), pa["offset"]))
        L.put(po + pa["offset"], b"".join(struct.pack("<HHBBH", *[q[k] for k in ("x", "z", "wait", "dir", "flags")])
                                          for q in pa["points"]))
    if no:
        L.put(no, struct.pack("<I", len(d["nav"])))
        for i, m in enumerate(d["nav"]):
            o = no + m["offset"]
            L.put(no + 4 + 4 * i, struct.pack("<I", m["offset"]))
            L.put(o, struct.pack("<HHIIII", len(m["rects"]), len(m["portals"]), len(m["extra"]),
                                 m["rect_offset"], m["portal_offset"], m["extra_offset"]))
            L.put(o + m["rect_offset"], b"".join(bytes([r["x0"], r["z0"], r["x1"], r["z1"], *r["portals"]]) for r in m["rects"]))
            L.put(o + m["portal_offset"], b"".join(struct.pack("<HHHBBI", *[q[k] for k in ("x", "z", "u", "rect_a", "rect_b", "unknown")])
                                                   for q in m["portals"]))
            L.put(o + m["extra_offset"], struct.pack(f"<{len(m['extra'])}H", *m["extra"]))
    return bytes(L.b)


def pcm_sample(raw):
    typ, status, freq, loop, size = struct.unpack_from("<HHIII", raw)
    data = raw[16:]
    f = io.BytesIO()
    with wave.open(f, "wb") as w:
        w.setnchannels(1)
        w.setsampwidth(1)
        w.setframerate(max(1, round(freq / 1024)))
        w.writeframes(bytes(x ^ 0x80 for x in data))   # signed 8-bit -> WAV's unsigned
    meta = via_json({"type": typ, "status": status, "freq": freq, "loop_start": loop, "size": size})
    f.seek(0)
    with wave.open(f, "rb") as w:
        pcm = bytes(x ^ 0x80 for x in w.readframes(w.getnframes()))
    return struct.pack("<HHIII", meta["type"], meta["status"], meta["freq"], meta["loop_start"], meta["size"]) + pcm


def wave_sample(raw):
    d = via_json({"samples": [v for x in raw for v in (x >> 4, x & 15)]})
    s = d["samples"]
    return bytes(s[i] << 4 | s[i + 1] for i in range(0, len(s), 2))


VOICE = ("type", "key", "length", "pan_sweep", "pointer", "attack", "decay", "sustain", "release")


def voicegroup(raw):
    d = via_json([dict(zip(VOICE, struct.unpack_from("<BBBBIBBBB", raw, 12 * i))) for i in range(len(raw) // 12)])
    return b"".join(struct.pack("<BBBBIBBBB", *[v[k] for k in VOICE]) for v in d)


def song_table(raw):
    rows = [f"song_{p:08X},{ms},{me}" for p, ms, me in struct.iter_unpack("<IHH", raw)]
    text = "song,player,unused\n" + "\n".join(rows) + "\n"
    out = b""
    for r in csv.DictReader(io.StringIO(text)):
        out += struct.pack("<IHH", int(r["song"][5:], 16), int(r["player"]), int(r["unused"]))
    return out


def lz_greedy(data, min_disp):
    """tools/lz77.py's greedy compressor (longest match, nearest first) with
    hash chains, for any minimum displacement (lz77.compress uses 2)."""
    out = bytearray([0x10, len(data) & 0xFF, len(data) >> 8 & 0xFF, len(data) >> 16 & 0xFF])
    chains, i, n = {}, 0, len(data)

    def index(k):
        if k + 3 <= n:
            chains.setdefault(data[k:k + 3], []).append(k)
    pos = 0  # positions below pos are indexed
    while i < n:
        flag_pos = len(out)
        out.append(0)
        flags = 0
        for bit in range(8):
            if i >= n:
                break
            while pos <= i - min_disp:
                index(pos)
                pos += 1
            best_len = best_disp = 0
            for j in reversed(chains.get(data[i:i + 3], ())):
                if i - j > 0x1000:
                    break
                ln = 3
                while ln < 18 and i + ln < n and data[j + ln] == data[i + ln]:
                    ln += 1
                if ln > best_len:
                    best_len, best_disp = ln, i - j
                    if ln == 18:
                        break
            if best_len >= 3:
                x = (best_len - 3) << 12 | (best_disp - 1)
                out += bytes([x >> 8, x & 0xFF])
                flags |= 0x80 >> bit
                i += best_len
            else:
                out.append(data[i])
                i += 1
        out[flag_pos] = flags
    return bytes(out)


# ---------- the checks ----------

class Results:
    def __init__(self):
        self.rows, self.failed = [], {}

    def check(self, start, raw, codec, what, tool, level="exact"):
        why = "differs"
        try:
            ok = codec(raw) == raw
        except Exception as e:  # a codec that cannot read a block does not count it
            ok, why = False, f"{type(e).__name__}: {e}"
        if ok:
            self.rows.append([start, start + len(raw), what, tool, level])
        else:
            self.failed.setdefault((what, tool), []).append(f"{start:08X} ({why})")
        return ok

    def add(self, start, end, what, tool, level):
        self.rows.append([start, end, what, tool, level])


def check_datafmt(rom, R):
    import datafmt
    from symbolize import load_func_names
    names = load_func_names()
    for e in datafmt.load_manifest():
        mod = e.module()
        raw = rom[e.start - ROM_BASE:e.end - ROM_BASE]
        if e.format == "resource_archive":
            continue  # the containers are checked in check_archives, the entries per kind
        if hasattr(mod, "decode") and hasattr(mod, "encode"):
            got = datafmt.to_bytes(mod.encode(mod.decode(rom, e.start, e.end, names), e.start, e.end))
        else:
            with tempfile.TemporaryDirectory() as tmp:
                path = os.path.join(tmp, "src")
                mod.extract(rom, e.start, e.end, path)
                got = datafmt.to_bytes(mod.encode_path(path, e.start, e.end))
        R.check(e.start, raw, lambda _r, got=got: got, e.name, f"datafmt/{e.format}")


def check_archives(rom, R):
    """Container headers from the entry list, then each entry by its kind."""
    entry_codecs = {"t92B3": (palette_92B3, "palette"), "t9B1B": (palette_pool, "palette"),
                    "fnt": (font, "font"), "ptn": (tileset_library, "tileset"), "tCB05": (sprite_package, "sprite")}
    for label, _, base, ents in archive.archives(rom):
        o = base - ROM_BASE
        count, ho, oo, do = struct.unpack_from("<4I", rom, o)
        hashes = [h for h, _, _ in ents]
        offs = [a - base for _, a, _ in ents]

        def container(raw, count=count, oo=oo, do=do, hashes=hashes, offs=offs):
            d = via_json({"entries": [{"name": h, "offset": x} for h, x in zip(hashes, offs)],
                          "hash_table": 0x10, "offset_table": oo, "data": do})
            L = Layout(d["data"])
            L.put(0, struct.pack("<4I", len(d["entries"]), d["hash_table"], d["offset_table"], d["data"]))
            L.put(d["hash_table"], struct.pack(f"<{count}H", *[e["name"] for e in d["entries"]]))
            L.put(d["offset_table"], struct.pack(f"<{count}I", *[e["offset"] for e in d["entries"]]))
            return bytes(L.b)
        R.check(base, rom[o:o + do], container, f"{label} container", "datafmt/resource_archive")
        for h, a, size in ents:
            raw = rom[a - ROM_BASE:a - ROM_BASE + size]
            if label in entry_codecs:
                codec, tool = entry_codecs[label]
                R.check(a, raw, codec, label, tool)
            elif label == "map":
                check_lz(a, raw, R, "map", mp_map, b"MP\0\0", struct_size)
            elif label == "tAE6C":
                check_lz(a, raw, R, "terrain", hp_terrain, b"HP\0\0")


def check_lz(a, raw, R, what, codec, magic, raw_size=None):
    """An archive entry that is LZ77 (content round-trips; recompression graded)
    or stored raw (bytes round-trip, the rest zero padding)."""
    fail = lambda r: None  # noqa: E731
    if raw[:1] == b"\x10":
        res = lz77.decompress(raw, 0, limit=0x800000)
        if not res:
            return R.check(a, raw, fail, what, what)
        content, clen = res
        i = content.find(magic)
        if i < 0 or i > 8:
            return R.check(a, raw, fail, what, what)
        prefix, body = content[:i], content[i:]  # the prefix stays as raw bytes in the JSON
        try:
            ok = codec(body) == body and not any(raw[clen:])
        except Exception:
            ok = False
        if not ok:
            return R.check(a, raw, fail, what, what)
        level = "lz-exact" if any(lz_greedy(prefix + body, d) == raw[:clen] for d in (2, 1)) else "lz-content"
        R.add(a, a + len(raw), what, what, level)
    elif raw_size:
        R.check(a, raw, lambda r: padded(codec(r[:raw_size(r)]), len(r)), f"{what} (raw)", what)
    else:
        R.check(a, raw, fail, f"{what} (raw)", what)


def struct_size(b):
    """End of the last section of a raw MP map (the rest is padding)."""
    nl, nt, nm, _ = struct.unpack_from("<4H", b, 4)
    tab, tiles, defs = struct.unpack_from("<III", b, 0xC)
    ends = [tab + 12 * nl, defs + 8 * nm, tiles + 32 * nt if tiles else 0]
    for i in range(nl):
        w, h, _, off = struct.unpack_from("<HHII", b, tab + 12 * i)
        ends.append(off + 2 * w * h)
    return max(ends)


def check_sound(rom, R):
    w = m4a.walk(rom)
    for a, e in sorted(w.samples.items()):
        raw = rom[a - ROM_BASE:e - ROM_BASE]
        if e - a > 16:
            R.check(a, raw, pcm_sample, "sample", "m4a")
        else:
            R.check(a, raw, wave_sample, "wave sample", "m4a")
    vgs = sorted(w.voicegroups)
    for i, a in enumerate(vgs):
        end = min(vgs[i + 1] if i + 1 < len(vgs) else a + 128 * 12, a + 128 * 12)
        n = (end - a) // 12 * 12
        R.check(a, rom[a - ROM_BASE:a - ROM_BASE + n], voicegroup, "voicegroup", "m4a")
    t = m4a.SONG_TABLE
    R.check(t, rom[t - ROM_BASE:t - ROM_BASE + 8 * len(w.songs)], song_table, "song table", "m4a")


def merged(rows):
    out = []
    for s, e, what, tool, level in sorted(rows):
        if out and out[-1][2:] == [what, tool, level] and s <= out[-1][1]:
            out[-1][1] = max(out[-1][1], e)
        else:
            out.append([s, e, what, tool, level])
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--dry", action="store_true")
    a = ap.parse_args()
    rom = load_rom()
    R = Results()
    check_datafmt(rom, R)
    check_archives(rom, R)
    check_sound(rom, R)
    rows = merged(R.rows)
    by = {}
    for s, e, what, tool, level in rows:
        by[(what, tool, level)] = by.get((what, tool, level), 0) + e - s
    for k, v in sorted(by.items(), key=lambda x: -x[1]):
        print(f"{v:>10,}  {k[0]:24} {k[1]:28} {k[2]}")
    for (what, tool), bad in sorted(R.failed.items()):
        print(f"not round-tripped: {what} ({tool}): {len(bad)} blocks, e.g. {', '.join(bad[:3])}")
    if not a.dry:
        with open(OUT, "w", newline="") as f:
            f.write("# generated by tools/data_roundtrip.py from a local ROM: ranges whose content a project\n"
                    "# tool decodes to an editable form and re-encodes byte-exact (docs/DATA_TRACKING.md)\n")
            wr = csv.writer(f, lineterminator="\n")
            wr.writerow(["start", "end", "what", "tool", "level"])
            for s, e, what, tool, level in rows:
                wr.writerow([f"{s:08X}", f"{e:08X}", what, tool, level])
        print(f"wrote {os.path.relpath(OUT, ROOT)}: {len(rows)} ranges")


if __name__ == "__main__":
    main()
