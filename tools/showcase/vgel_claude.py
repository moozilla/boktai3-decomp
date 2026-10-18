"""The Claudesona (vgel's Claude, drawn by thebes and others: a 12-petal
rust-orange starburst mane around a round white face with simple line
features, a lavender long-sleeve top, light-blue jeans, brown shoes, orange
paws and a curled orange lion's tail; reference: the CC0 sprites in
github.com/N8python/claudesona) as a Boktai-style sprite: chibi proportions,
an ink outline, pillow shading lit from the top left (clawd.py's height-field
shading), one palette shared with Clawd (showcase_palette.CLAUDE).

  python3 tools/showcase/vgel_claude.py OUT.ppm     a preview of the frames
"""
import math

from clawd import H, W, blank, distance, ellipse, rounded, shape

ORANGE = [2, 3, 4, 5]        # palette ramps, dark to light
MANE = [3, 4, 4, 5]          # the petals: lighter, salmon
LAVENDER = [7, 8, 9, 9]
BLUE = [10, 11, 12, 12]
FACE = [13, 13, 15, 15]
SHOE = [1, 2, 2, 3]
INK = 14
CX = 32


def tiers(m, roundness=4.0, light=(-0.55, -0.75, 0.9)):
    d = distance(m)
    h = [[math.sqrt(min(d[y][x], roundness) / roundness) * roundness if m[y][x] else 0 for x in range(W)] for y in range(H)]
    lx, ly, lz = light
    n = math.sqrt(lx * lx + ly * ly + lz * lz)
    out = blank()
    for y in range(H):
        for x in range(W):
            if m[y][x]:
                gx = (h[y][min(x + 1, W - 1)] - h[y][max(x - 1, 0)]) / 2
                gy = (h[min(y + 1, H - 1)][x] - h[max(y - 1, 0)][x]) / 2
                k = math.sqrt(gx * gx + gy * gy + 1)
                i = (-gx * lx - gy * ly + lz) / (k * n)
                out[y][x] = 0 if i < 0.55 else 1 if i < 0.82 else 2 if i < 0.95 else 3
    return out


def put(px, m, ramp, roundness=3.0):
    t = tiers(m, roundness)
    for y in range(H):
        for x in range(W):
            if m[y][x]:
                px[y][x] = ramp[t[y][x]]


def ink_outline(px):
    out = [row[:] for row in px]
    for y in range(H):
        for x in range(W):
            if px[y][x] == 0 and any(0 <= y + dy < H and 0 <= x + dx < W and px[y + dy][x + dx] not in (0, INK)
                                     for dy, dx in ((0, 1), (0, -1), (1, 0), (-1, 0))):
                out[y][x] = INK
    return out


def petal(cx, cy, angle, r0, r1, width):
    """One long petal from radius r0 to r1, rounded at the tip."""
    ca, sa = math.cos(angle), math.sin(angle)

    def f(x, y):
        dx, dy = x - cx, y - cy
        along = dx * ca + dy * sa
        across = -dx * sa + dy * ca
        if along < r0 or along > r1:
            return False
        t = (along - r0) / (r1 - r0)
        half = width * (0.75 + 0.25 * math.sin(math.pi * min(t * 1.4, 1)))  # widest a little past the middle
        if t > 0.8:                                                            # round the tip
            half *= math.sqrt(max(0.0, 1 - ((t - 0.8) / 0.2) ** 2))
        return abs(across) <= half
    return f


def frame(arms="out", face="smile", step=0, bob=0):
    """arms: out, up (cheer), down; face: smile, talk, happy (big grin), blink."""
    px = blank()
    hy = 18 - bob
    # tail: a curve from behind the hips out to the left, ending in a curl
    tail = shape(lambda x, y: any(math.hypot(x - (CX - 8 - 6 * t), y - (45 - 6 * math.sin(t * 2.6) - bob)) < 1.6
                                  for t in [i / 24 for i in range(25)]))
    put(px, tail, ORANGE, 1)
    # legs, shoes
    for side, lx in ((-1, CX - 5), (1, CX + 1)):
        lift = step if side == 1 else 0
        leg = shape(rounded(lx, 42 - bob, lx + 4, 57 - lift, 2))
        put(px, leg, BLUE, 2)
        shoe = shape(ellipse(lx + 2 + side, 58 - lift, 3.6, 2))
        put(px, shoe, SHOE, 1.5)
    # body: a lavender top
    body = shape(rounded(CX - 6, 31 - bob, CX + 6, 44 - bob, 3))
    put(px, body, LAVENDER, 3)
    # arms and orange paws
    for side in (-1, 1):
        if arms == "up":
            arm = shape(lambda x, y, s=side: any(math.hypot(x - (CX + s * (5 + 7 * t)), y - (32 - 12 * t - bob)) < 2.2
                                                 for t in [i / 12 for i in range(13)]))
            paw = (CX + side * 12, 19 - bob)
        elif arms == "out":
            arm = shape(lambda x, y, s=side: any(math.hypot(x - (CX + s * (5 + 9 * t)), y - (33 + 3 * t - bob)) < 2.2
                                                 for t in [i / 12 for i in range(13)]))
            paw = (CX + side * 15, 37 - bob)
        else:
            arm = shape(rounded(CX + side * 7 - 2, 32 - bob, CX + side * 7 + 2, 41 - bob, 2))
            paw = (CX + side * 7, 43 - bob)
        put(px, arm, LAVENDER, 2)
        put(px, shape(ellipse(paw[0], paw[1], 2.6, 2.6)), ORANGE, 1.5)
    # the mane: twelve long petals around the face, each outlined on its own
    for k in range(12):
        a = -math.pi / 2 + k * math.pi / 6
        p = shape(petal(CX, hy, a, 5, 15.5, 3.2))
        put(px, p, MANE, 2)
        for y in range(H):              # ink between petals
            for x in range(W):
                if p[y][x] and any(0 <= y + dy < H and 0 <= x + dx < W and not p[y + dy][x + dx] and px[y + dy][x + dx] in MANE + ORANGE
                                   for dy, dx in ((0, 1), (0, -1), (1, 0), (-1, 0))):
                    px[y][x] = INK
    # face: a white disc with an ink rim and line features
    face_m = shape(ellipse(CX, hy, 7.2, 7.2))
    put(px, face_m, FACE, 5)
    for y in range(H):
        for x in range(W):
            if face_m[y][x] and any(not face_m[y + dy][x + dx] for dy, dx in ((0, 1), (0, -1), (1, 0), (-1, 0))):
                px[y][x] = INK
    ink = lambda pts: [px[y].__setitem__(x, INK) for x, y in pts]  # noqa: E731
    if face == "blink":
        ink([(CX - 4, hy - 1), (CX - 3, hy - 1), (CX + 3, hy - 1), (CX + 4, hy - 1)])
    elif face == "happy":  # closed happy eyes: little arches
        ink([(CX - 4, hy - 1), (CX - 3, hy - 2), (CX - 2, hy - 1), (CX + 2, hy - 1), (CX + 3, hy - 2), (CX + 4, hy - 1)])
    else:  # open eyes: dark ovals with a glint
        for ex in (CX - 3, CX + 3):
            ink([(ex - 1, hy - 2), (ex, hy - 2), (ex - 1, hy - 1), (ex, hy - 1), (ex - 1, hy), (ex, hy)])
            px[hy - 2][ex - 1] = 15
    if face == "talk":
        ink([(CX - 1, hy + 3), (CX, hy + 3), (CX - 1, hy + 4), (CX, hy + 4)])
    elif face == "happy":
        ink([(CX - 2, hy + 2), (CX - 1, hy + 2), (CX, hy + 2), (CX + 1, hy + 2),
             (CX - 1, hy + 3), (CX, hy + 3)])
    else:  # a small smile
        ink([(CX - 2, hy + 2), (CX - 1, hy + 3), (CX, hy + 3), (CX + 1, hy + 2)])
    return ink_outline(px)


FRAMES = {"idle": frame(), "talk": frame(face="talk"), "cheer": frame(arms="up", face="happy", bob=1),
          "down": frame(arms="down"), "blink": frame(face="blink")}
ORDER = ["idle", "talk", "cheer", "down", "blink"]


def preview(path, palette, scale=4):
    imgs = [FRAMES[k] for k in ORDER]
    w, h = W * len(imgs) * scale, H * scale
    out = bytearray()
    for y in range(h):
        for i in range(len(imgs)):
            for x in range(W):
                c = imgs[i][y // scale][x]
                rgb = palette[c] if c else 0x506070
                out += bytes([rgb >> 16 & 255, rgb >> 8 & 255, rgb & 255]) * scale
    open(path, "wb").write(f"P6 {w} {h} 255\n".encode() + bytes(out))


if __name__ == "__main__":
    import sys
    from showcase_palette import CLAUDE
    preview(sys.argv[1], CLAUDE)
