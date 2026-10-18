"""ROM additions for the v3 showcase, on top of tools/showcase/rom.py.

* new archive entries (resource name -> data) without replacing any of the
  game's: the archive's index (count, sorted hash table, offset table) is
  rebuilt in free space and its directory record pointed at it;
* new sprite palette pools: the pool resource (9B1B C5E9: a u16 count, then
  32-byte entries) is copied with entries appended, so the game's 955 stay
  as they are and new sprites name pools 955 and up;
* sprite packages built from indexed images: each frame is cut into GBA OBJ
  parts and its tiles laid out on the package's 16-tile-wide sheet.

Output is ROM-derived: write it under build/ only.
"""
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(HERE))), "tools", "assets"))
import archive  # noqa: E402
from rom import BASE, bgr555  # noqa: E402

POOL_TYPE, POOL_NAME = 0x9B1B, 0xC5E9
PKG_TYPE = 0xCB05


def entries(r, type_hash):
    for label, t, base, ents in archive.archives(bytes(r.rom)):
        if t == type_hash:
            return base, ents
    raise KeyError(f"no archive {type_hash:04X}")


def add_resources(r, type_hash, items):
    """Add {name hash: data} to the archive of type_hash (names must be new)."""
    base, ents = entries(r, type_hash)
    have = {h: a for h, a, _ in ents}
    for h in items:
        assert h not in have, f"{type_hash:04X} already has {h:04X}"
    addr = dict(have)
    for h, data in items.items():
        if isinstance(data, tuple):           # ("alias", source name, pool): a header pointing at another package
            _, src, pool = data
            sa = have[src] if src in have else addr[src]
            hdr = bytearray(r.rom[sa - BASE:sa - BASE + 32])
            a = r.alloc(b"\0" * 32, 4)
            struct.pack_into("<H", hdr, 0, pool)
            offs = struct.unpack_from("<5I", hdr, 0xC)
            struct.pack_into("<5I", hdr, 0xC, *[(o + sa - a) & 0xFFFFFFFF for o in offs])
            r.rom[a - BASE:a - BASE + 32] = hdr
            addr[h] = a
        else:
            addr[h] = r.alloc(data, 4)
    names = sorted(addr)
    n = len(names)
    head = 16
    ho = head
    oo = (ho + 2 * n + 3) & ~3
    size = oo + 4 * n
    new = r.alloc(b"\0" * size, 4)
    o = new - BASE
    struct.pack_into("<IIII", r.rom, o, n, ho, oo, 0)
    for i, h in enumerate(names):
        struct.pack_into("<H", r.rom, o + ho + 2 * i, h)
        struct.pack_into("<I", r.rom, o + oo + 4 * i, (addr[h] - new) & 0xFFFFFFFF)
    key = archive.TYPE_KEYS[type_hash]
    d = archive.DIRECTORY - BASE
    while True:
        k, p = struct.unpack_from("<II", r.rom, d)
        assert k, f"no directory record for {type_hash:04X}"
        if k == key:
            struct.pack_into("<I", r.rom, d + 4, new)
            return new
        d += 8


class Pools:
    """New palette pool entries, appended to the game's table at build()."""

    def __init__(self, r):
        self.r = r
        base, ents = entries(r, POOL_TYPE)
        a = next(a for h, a, _ in ents if h == POOL_NAME)
        self.count = struct.unpack_from("<H", r.rom, a - BASE)[0]
        self.old = bytes(r.rom[a - BASE + 4:a - BASE + 4 + 32 * self.count])
        self.head = bytes(r.rom[a - BASE:a - BASE + 4])
        self.new = []

    def set(self, pool, colors):
        """Rewrite one of the game's entries (e.g. a portrait's palette)."""
        o = 32 * pool
        self.old = self.old[:o] + struct.pack("<16H", *[bgr555(c) for c in colors]) + self.old[o + 32:]

    def add(self, colors):
        """colors: 16 0xRRGGBB values (entry 0 is transparent). Returns the pool index."""
        assert len(colors) == 16
        self.new.append(struct.pack("<16H", *[bgr555(c) for c in colors]))
        return self.count + len(self.new) - 1

    def build(self):
        from rom import replace_resource
        n = self.count + len(self.new)
        data = struct.pack("<H", n) + self.head[2:] + self.old + b"".join(self.new)
        replace_resource(self.r, POOL_TYPE, POOL_NAME, data)


# GBA OBJ sizes: (shape, size) -> (w, h)
OBJ = {(0, 0): (8, 8), (0, 1): (16, 16), (0, 2): (32, 32), (0, 3): (64, 64),
       (1, 0): (16, 8), (1, 1): (32, 8), (1, 2): (32, 16), (1, 3): (64, 32),
       (2, 0): (8, 16), (2, 1): (8, 32), (2, 2): (16, 32), (2, 3): (32, 64)}


def cover(img, x0, y0, w, h):
    """Cut the opaque area of img (rows of indices, 0 transparent) inside
    (x0, y0, w, h) into OBJ rectangles: [(shape, size, x, y)], greedy from the
    top-left, largest first, skipping empty rectangles."""
    H, W = len(img), len(img[0])

    def used(x, y, ww, hh):
        return any(img[j][i] for j in range(max(0, y), min(H, y + hh)) for i in range(max(0, x), min(W, x + ww)))
    out = []
    covered = [[False] * W for _ in range(H)]
    order = sorted(OBJ.items(), key=lambda kv: -kv[1][0] * kv[1][1])
    for y in range(y0, y0 + h):
        for x in range(x0, x0 + w):
            if not (0 <= y < H and 0 <= x < W) or not img[y][x] or covered[y][x]:
                continue
            # the largest OBJ starting here (aligned to 8) whose area is mostly new pixels
            bx, by = x - (x - x0) % 8, y - (y - y0) % 8
            best = None
            for (sh, sz), (ww, hh) in order:
                new = sum(1 for j in range(by, min(H, by + hh)) for i in range(bx, min(W, bx + ww))
                          if img[j][i] and not covered[j][i])
                if new == 0:
                    continue
                area = ww * hh
                cost = area - 3 * new  # favor fewer, fuller parts
                if best is None or cost < best[0]:
                    best = (cost, sh, sz, ww, hh)
            _, sh, sz, ww, hh = best
            for j in range(by, min(H, by + hh)):
                for i in range(bx, min(W, bx + ww)):
                    covered[j][i] = True
            out.append((sh, sz, bx, by))
    return [p for p in out if used(p[2], p[3], *OBJ[(p[0], p[1])])]


def tiles_of(img, x, y, w, h):
    """4bpp tiles (row-major) of the w x h area at (x, y)."""
    H, W = len(img), len(img[0])
    out = []
    for ty in range(0, h, 8):
        for tx in range(0, w, 8):
            t = bytearray(32)
            for r_ in range(8):
                for c in range(8):
                    yy, xx = y + ty + r_, x + tx + c
                    v = img[yy][xx] if 0 <= yy < H and 0 <= xx < W else 0
                    t[r_ * 4 + c // 2] |= (v & 15) << (4 * (c & 1))
            out.append(bytes(t))
    return out


class Sheet:
    """A 16-tile-wide tile sheet: blocks of w x h tiles placed in free space."""

    def __init__(self):
        self.rows = []      # each row: 16 slots (None or tile bytes)

    def place(self, tiles, tw, th):
        assert tw <= 16
        r0 = 0
        while True:
            for c0 in range(0, 17 - tw):
                if all(self._free(r0 + j, c0 + i) for j in range(th) for i in range(tw)):
                    for j in range(th):
                        while len(self.rows) <= r0 + j:
                            self.rows.append([None] * 16)
                        for i in range(tw):
                            self.rows[r0 + j][c0 + i] = tiles[j * tw + i]
                    return (r0 * 16 + c0)
            r0 += 1

    def _free(self, r_, c):
        return r_ >= len(self.rows) or self.rows[r_][c] is None

    def data(self):
        return b"".join(t or bytes(32) for row in self.rows for t in row)

    def count(self):
        return 16 * len(self.rows)


def package(frames, pool, origin):
    """A sprite package from indexed frames (lists of rows of 0-15). origin is
    (x, y) of the anchor in every frame (feet: bottom center), so part offsets
    are relative to it. Frame i is keyframe i (44CD p=i)."""
    sheet = Sheet()
    ftab, ptab = b"", b""
    nparts = 0
    for i, img in enumerate(frames):
        H, W = len(img), len(img[0])
        ox, oy = origin if not callable(origin) else origin(i, img)
        parts = cover(img, 0, 0, W, H)
        po = len(ptab)
        for sh, sz, x, y in parts:
            w, h = OBJ[(sh, sz)]
            t0 = sheet.place(tiles_of(img, x, y, w, h), w // 8, h // 8)
            attr = sz << 10 | sh << 8
            ptab += struct.pack("<HhhH", attr, x - ox, y - oy, t0)
        nparts += len(parts)
        ftab += struct.pack("<HHIhhHHI", i, len(parts), 0, -ox, -oy, W, H, po)
    atab = struct.pack("<HHI", 0, len(frames), 0)
    ktab = b"".join(struct.pack("<HH", i, 30) for i in range(len(frames)))
    head = 0x20
    o_f = head
    o_a = o_f + len(ftab)
    o_p = o_a + len(atab)
    o_k = o_p + len(ptab)
    o_t = (o_k + len(ktab) + 3) & ~3
    tiles = sheet.data()
    hdr = struct.pack("<6H5I", pool, len(frames), 1, nparts, len(frames), len(tiles) // 32, o_f, o_a, o_p, o_k, o_t)
    hdr += b"\0" * (head - len(hdr))
    body = hdr + ftab + atab + ptab + ktab
    return body + b"\0" * (o_t - len(body)) + tiles


def repool(pkg, pool):
    """The same package drawn with another palette pool (the first header word)."""
    return struct.pack("<H", pool) + pkg[2:]


def big_frames(rom, sprite):
    """The frames of large sprite `sprite` (98F5) as indexed images, all on
    one canvas so they share an origin: (frames, (ox, oy) of the sprite's
    origin in the canvas, default pool)."""
    import bigsprites
    import sprites as pk
    bs = bigsprites.BigSprites(bytes(rom))
    i = next(k for k, rec in enumerate(bs.records) if rec[0] == sprite)
    frs = list(bs.frames(i))
    allparts = []
    for nparts, ntiles, palette, pix, po in frs:
        parts, tile = [], 0
        for k in range(nparts):
            attr, fl, x, y = struct.unpack_from("<BBbb", bs.d, bs.parts_at + po + k * 4)
            w, h = pk.SIZES.get(attr & 3, pk.SIZES[0])[(attr >> 2) & 3]
            parts.append((x, y, w, h, tile, fl, pix))
            tile += (w // 8) * (h // 8)
        allparts.append(parts)
    x0 = min(p[0] for ps in allparts for p in ps)
    y0 = min(p[1] for ps in allparts for p in ps)
    x1 = max(p[0] + p[2] for ps in allparts for p in ps)
    y1 = max(p[1] + p[3] for ps in allparts for p in ps)
    out = []
    for ps in allparts:
        img = [[0] * (x1 - x0) for _ in range(y1 - y0)]
        for x, y, w, h, t0, fl, pix in reversed(ps):
            tw = w // 8
            for ty in range(h // 8):
                for tx in range(tw):
                    o = bs.pixels + pix + (t0 + ty * tw + tx) * 32
                    for r_ in range(8):
                        for c in range(8):
                            v = (bs.d[o + r_ * 4 + c // 2] >> (4 * (c & 1))) & 15
                            dx, dy = tx * 8 + c, ty * 8 + r_
                            if fl & 0x10:
                                dx = w - 1 - dx
                            if fl & 0x20:
                                dy = h - 1 - dy
                            if v:
                                img[y - y0 + dy][x - x0 + dx] = v
        out.append(img)
    return out, (-x0, -y0), frs[0][2]


def pool_colors(rom, pool):
    """16 0xRRGGBB colors of palette pool entry `pool` (the game's table)."""
    from rom import POOLS
    vals = struct.unpack_from("<16H", rom, POOLS - BASE + 32 * pool)
    return [((v & 31) << 3) << 16 | ((v >> 5 & 31) << 3) << 8 | ((v >> 10 & 31) << 3) for v in vals]
