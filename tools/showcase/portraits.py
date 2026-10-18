"""Dialogue portraits for Claude (the Claudesona) and Clawd, Boktai-style.

The message window draws a speaker's portrait from the large sprites 593A/
593B: frame = the speaker's sequence in animation 922E 4BBF, palette = the
pool sub_0803092C picks per speaker id (tools/studio/atlas.py
speaker_portraits). Each portrait frame is one 32x32 OBJ: 16 tiles, 512 bytes
of 4bpp pixels. The showcase draws over two speakers it does not use:
GARIGARIKUN (id 30, 593B frame 14, pool 506) becomes Claude and ROCKMAN
(id 25, 593B frame 8, pool 504) becomes Clawd; <LABEL=GARIGARIKUN>Claude
</LABEL> then shows Claude's face.

  python3 tools/showcase/portraits.py OUT.ppm      a preview
"""
import math

S = 32
CLAUDE_PAL = [0, 0x181014, 0x9C4A2C, 0xD97757, 0xEBA184, 0xF6CDB8, 0xD8D4D8, 0xFFFFFF,
              0x7462B0, 0xA392DC, 0xCDBEF2, 0, 0, 0, 0, 0]
CLAWD_PAL = [0, 0x4A1E10, 0x9C4A2C, 0xD97757, 0xEBA184, 0xF6CDB8, 0xFFF8F0, 0x181014,
             0, 0, 0, 0, 0, 0, 0, 0]
CLAWD_PAL[0] = CLAUDE_PAL[0] = 0x787888


def grid():
    return [[0] * S for _ in range(S)]


def mask(fn):
    return [[1 if fn(x + 0.5, y + 0.5) else 0 for x in range(S)] for y in range(S)]


def dist(m):
    d = [[99 if m[y][x] else 0 for x in range(S)] for y in range(S)]
    for _ in range(2):
        for y in range(S):
            for x in range(S):
                if d[y][x]:
                    for dy, dx in ((-1, 0), (0, -1), (1, 0), (0, 1)):
                        yy, xx = y + dy, x + dx
                        v = d[yy][xx] + 1 if 0 <= yy < S and 0 <= xx < S else 1
                        d[y][x] = min(d[y][x], v)
    return d


def shade(px, m, ramp, r=3.0, light=(-0.55, -0.75, 0.9)):
    d = dist(m)
    h = [[math.sqrt(min(d[y][x], r) / r) * r if m[y][x] else 0 for x in range(S)] for y in range(S)]
    n = math.sqrt(sum(c * c for c in light))
    for y in range(S):
        for x in range(S):
            if m[y][x]:
                gx = (h[y][min(x + 1, S - 1)] - h[y][max(x - 1, 0)]) / 2
                gy = (h[min(y + 1, S - 1)][x] - h[max(y - 1, 0)][x]) / 2
                i = (-gx * light[0] - gy * light[1] + light[2]) / (math.sqrt(gx * gx + gy * gy + 1) * n)
                px[y][x] = ramp[0 if i < 0.55 else 1 if i < 0.82 else 2 if i < 0.95 else 3]


def outline(px, ink):
    out = [r[:] for r in px]
    for y in range(S):
        for x in range(S):
            if px[y][x] == 0 and any(0 <= y + dy < S and 0 <= x + dx < S and px[y + dy][x + dx] not in (0, ink)
                                     for dy, dx in ((0, 1), (0, -1), (1, 0), (-1, 0))):
                out[y][x] = ink
    return out


def claude_portrait():
    """The Claudesona's head and shoulders: petals, white face, dot eyes, smile."""
    px = grid()
    cx, cy = 16, 14
    shade(px, mask(lambda x, y: abs(x - cx) < 11 and y > 25 or ((x - cx) / 12) ** 2 + ((y - 33) / 8) ** 2 < 1),
          [8, 9, 9, 10], 3)                                              # lavender shoulders
    for k in range(12):
        a = -math.pi / 2 + k * math.pi / 6
        ca, sa = math.cos(a), math.sin(a)

        def petal(x, y, ca=ca, sa=sa):
            dx, dy = x - cx, y - cy
            al, ac = dx * ca + dy * sa, -dx * sa + dy * ca
            if not 4 <= al <= 15.5:
                return False
            t = (al - 4) / 11.5
            half = 2.6 * (1 - max(0, t - 0.75) * 3)
            return abs(ac) <= half
        m = mask(petal)
        shade(px, m, [3, 4, 4, 5], 1.6)
        for y in range(S):
            for x in range(S):
                if m[y][x] and any(0 <= y + dy < S and 0 <= x + dx < S and not m[y + dy][x + dx] and px[y + dy][x + dx] in (3, 4, 5)
                                   for dy, dx in ((0, 1), (1, 0))):
                    px[y][x] = 1
    face = mask(lambda x, y: (x - cx) ** 2 + (y - cy) ** 2 <= 7.3 ** 2)
    shade(px, face, [6, 7, 7, 7], 4)
    for y in range(S):
        for x in range(S):
            if face[y][x] and any(not face[y + dy][x + dx] for dy, dx in ((0, 1), (0, -1), (1, 0), (-1, 0))):
                px[y][x] = 1
    for ex in (cx - 3, cx + 3):                                          # eyes with a glint
        for x, y in ((ex - 1, cy - 1), (ex, cy - 1), (ex - 1, cy), (ex, cy)):
            px[y][x] = 1
        px[cy - 1][ex - 1] = 7
    for x, y in ((cx - 2, cy + 2), (cx - 1, cy + 3), (cx, cy + 3), (cx + 1, cy + 2)):
        px[y][x] = 1                                                     # smile
    return outline(px, 1)


def clawd_portrait():
    """Clawd, close up: the body block, big shiny eyes, raised claws."""
    px = grid()
    for side in (-1, 1):
        cl = mask(lambda x, y, s=side: ((x - (16 + s * 12)) / 4.5) ** 2 + ((y - 9) / 4.5) ** 2 < 1
                  and not ((x - (16 + s * 13)) / 2) ** 2 + ((y - 5) / 2.6) ** 2 < 1)
        shade(px, cl, [2, 3, 4, 5], 2.5)
        arm = mask(lambda x, y, s=side: abs(x - (16 + s * 11)) < 2 and 11 < y < 22)
        shade(px, arm, [2, 3, 3, 4], 1.5)
    body = mask(lambda x, y: 4 <= x <= 28 and 12 <= y <= 32 and (min(x - 4, 28 - x) + min(y - 12, 99)) > 2)
    shade(px, body, [2, 3, 4, 5], 5)
    for ex in (11, 20):
        for y in range(17, 24):
            for x in range(ex, ex + 3):
                px[y][x] = 7
        px[18][ex] = px[19][ex] = 6
        px[22][ex + 2] = 6
    for x in range(14, 19):
        px[27][x] = 2
    return outline(px, 1)


def tiles(px):
    """512 bytes: a 32x32 OBJ's 16 tiles in 1D order (row-major tiles)."""
    out = bytearray()
    for ty in range(4):
        for tx in range(4):
            for y in range(8):
                for x in range(0, 8, 2):
                    out.append(px[ty * 8 + y][tx * 8 + x] | px[ty * 8 + y][tx * 8 + x + 1] << 4)
    return bytes(out)


def preview(path, scale=6):
    imgs = [(claude_portrait(), CLAUDE_PAL), (clawd_portrait(), CLAWD_PAL)]
    w, h = S * len(imgs) * scale + 8 * scale, S * scale
    rows = bytearray()
    for y in range(h):
        for img, pal in imgs:
            for x in range(S):
                c = img[y // scale][x]
                rgb = pal[c] if c else 0x506070
                rows += bytes([rgb >> 16 & 255, rgb >> 8 & 255, rgb & 255]) * scale
        rows += bytes([0x50, 0x60, 0x70]) * 8 * scale
    open(path, "wb").write(f"P6 {w} {h} 255\n".encode() + bytes(rows))


if __name__ == "__main__":
    import sys
    preview(sys.argv[1])
