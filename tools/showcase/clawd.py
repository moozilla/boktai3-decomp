"""Clawd, Claude Code's mascot, drawn as a Boktai-style sprite.

Boktai's sprites are small chibi figures in a 3/4 view: a dark outline,
pillow shading lit from the top left in a four-step ramp, a bright rim on
the lit side and shine in the eyes. Clawd is built the same way from simple
shapes: each shape becomes a height field (its distance from its own edge),
the light falls on that surface, the result is quantized to the ramp, and
the silhouette gets a one-pixel outline. Palette indices (the recolored pool,
showcase.CLAUDE): 0 clear, 1 outline, 2 shade, 3 base, 4 light, 5 highlight,
6 white.

  python3 tools/showcase/clawd.py OUT.ppm     a preview sheet of the frames
"""
import math

W = H = 64
CX = 32                      # the sprite is drawn bottom-centered at (32, 62)


def blank():
    return [[0] * W for _ in range(H)]


def shape(mask_fn):
    m = blank()
    for y in range(H):
        for x in range(W):
            if mask_fn(x + 0.5, y + 0.5):
                m[y][x] = 1
    return m


def ellipse(cx, cy, rx, ry):
    return lambda x, y: ((x - cx) / rx) ** 2 + ((y - cy) / ry) ** 2 <= 1


def rounded(x0, y0, x1, y1, r):
    def f(x, y):
        dx = max(x0 + r - x, 0, x - (x1 - r))
        dy = max(y0 + r - y, 0, y - (y1 - r))
        return x0 <= x <= x1 and y0 <= y <= y1 and dx * dx + dy * dy <= r * r
    return f


def distance(m):
    """Chamfer distance of each set pixel from the shape's edge."""
    inf = 99
    d = [[inf if m[y][x] else 0 for x in range(W)] for y in range(H)]
    for y in range(H):
        for x in range(W):
            if d[y][x]:
                d[y][x] = min(d[y][x], (d[y - 1][x] + 1) if y else 1, (d[y][x - 1] + 1) if x else 1,
                              (d[y - 1][x - 1] + 1.4) if x and y else 1.4, (d[y - 1][x + 1] + 1.4) if y and x < W - 1 else 1.4)
    for y in range(H - 1, -1, -1):
        for x in range(W - 1, -1, -1):
            if d[y][x]:
                d[y][x] = min(d[y][x], (d[y + 1][x] + 1) if y < H - 1 else 1, (d[y][x + 1] + 1) if x < W - 1 else 1,
                              (d[y + 1][x + 1] + 1.4) if x < W - 1 and y < H - 1 else 1.4,
                              (d[y + 1][x - 1] + 1.4) if y < H - 1 and x else 1.4)
    return d


def shade(m, roundness=4.0, light=(-0.55, -0.75, 0.9)):
    """Ramp index (2..5) per pixel of shape m: a pillow surface lit from the top left."""
    d = distance(m)
    h = [[math.sqrt(min(d[y][x], roundness) / roundness) * roundness if m[y][x] else 0 for x in range(W)] for y in range(H)]
    lx, ly, lz = light
    n = math.sqrt(lx * lx + ly * ly + lz * lz)
    lx, ly, lz = lx / n, ly / n, lz / n
    out = blank()
    for y in range(H):
        for x in range(W):
            if not m[y][x]:
                continue
            gx = (h[y][min(x + 1, W - 1)] - h[y][max(x - 1, 0)]) / 2
            gy = (h[min(y + 1, H - 1)][x] - h[max(y - 1, 0)][x]) / 2
            nx, ny, nz = -gx, -gy, 1.0
            k = math.sqrt(nx * nx + ny * ny + nz * nz)
            i = (nx * lx + ny * ly + nz * lz) / k
            out[y][x] = 2 if i < 0.55 else 3 if i < 0.82 else 4 if i < 0.95 else 5
    return out


def paint(px, m, colors):
    for y in range(H):
        for x in range(W):
            if m[y][x]:
                px[y][x] = colors[y][x] if isinstance(colors, list) else colors


def outline(px):
    out = [row[:] for row in px]
    for y in range(H):
        for x in range(W):
            if px[y][x] == 0 and any(0 <= y + dy < H and 0 <= x + dx < W and px[y + dy][x + dx] not in (0, 1)
                                     for dy, dx in ((0, 1), (0, -1), (1, 0), (-1, 0))):
                out[y][x] = 1
    return out


def frame(bob=0, wave=0, blink=False):
    """bob: body offset up (px); wave: 0 claws down, 1 right claw raised, 2 both raised."""
    px = blank()
    by = 38 - bob                                     # body center
    # stubby legs: three a side, under the body's front edge
    for lx in (16, 22, 28, 36, 42, 48):
        leg = shape(rounded(lx - 2, by + 6, lx + 2, 59, 2))
        paint(px, leg, shade(leg, 2))
    # claws: a short arm and a round pincer each side
    for side in (-1, 1):
        up = wave == 2 or (wave == 1 and side == 1)
        ax = CX + side * 24
        ay = by - 12 if up else by + 1
        arm = shape(rounded(min(CX + side * 17, ax) - 2, min(by - 1, ay) - 1,
                            max(CX + side * 17, ax) + 2, max(by - 1, ay) + 2, 2))
        paint(px, arm, shade(arm, 2))
        claw = shape(ellipse(ax, ay - 1, 6.5, 6))
        notch = ellipse(ax + side * 2, ay - 6, 2.4, 3.6)   # the gap between the pincer's jaws
        claw = [[claw[y][x] and not notch(x + 0.5, y + 0.5) for x in range(W)] for y in range(H)]
        paint(px, claw, shade(claw, 3.5))
    # body: a big soft block seen from slightly above
    body = shape(rounded(CX - 19, by - 15, CX + 19, by + 10, 9))
    paint(px, body, shade(body, 7))
    top = shape(ellipse(CX - 4, by - 10, 12, 3.5))       # the top catches more light
    for y in range(H):
        for x in range(W):
            if top[y][x] and body[y][x] and px[y][x] in (3, 4):
                px[y][x] += 1
    # eyes: big, dark, with two shines each
    for ex in (CX - 8, CX + 7):
        if blink:
            for x in range(ex - 3, ex + 3):
                px[by - 1][x] = 1
                px[by][x] = 1
        else:
            eye = shape(rounded(ex - 3, by - 7, ex + 3, by + 3, 2.5))
            paint(px, eye, 1)
            for y, x in ((by - 6, ex - 2), (by - 5, ex - 2), (by - 6, ex - 1), (by + 1, ex + 1)):
                px[y][x] = 6
    # cheeks
    for cx in (CX - 13, CX + 12):
        for x in range(cx - 1, cx + 2):
            if px[by + 3][x] in (2, 3, 4):
                px[by + 3][x] = 4
    return outline(px)


FRAMES = {"idle": frame(), "bob": frame(bob=1), "wave": frame(wave=1), "cheer": frame(bob=2, wave=2),
          "blink": frame(blink=True)}
ORDER = ["idle", "bob", "wave", "cheer", "blink"]


def sheet(frames, cols=16):
    """4bpp tiles in a sheet `cols` tiles wide: frame i at tile columns 8i..8i+7
    (rows of frames wrap at `cols`)."""
    per_row = cols // 8
    rows = (len(frames) + per_row - 1) // per_row * 8
    tiles = bytearray(32 * cols * rows)
    for i, px in enumerate(frames):
        fx, fy = (i % per_row) * 8, (i // per_row) * 8
        for ty in range(8):
            for tx in range(8):
                t = (fy + ty) * cols + fx + tx
                for y in range(8):
                    for x in range(0, 8, 2):
                        tiles[t * 32 + y * 4 + x // 2] = px[ty * 8 + y][tx * 8 + x] | px[ty * 8 + y][tx * 8 + x + 1] << 4
    return bytes(tiles)


def tile_of(i, cols=16):
    per_row = cols // 8
    return (i // per_row) * 8 * cols + (i % per_row) * 8


def preview(path, palette, scale=4):
    imgs = [FRAMES[k] for k in ORDER]
    w, h = W * len(imgs) * scale, H * scale
    out = bytearray()
    for y in range(h):
        for i in range(len(imgs)):
            for x in range(W):
                c = imgs[i][y // scale][x]
                rgb = palette[c] if c else 0x203040
                out += bytes([rgb >> 16 & 255, rgb >> 8 & 255, rgb & 255]) * scale
    open(path, "wb").write(f"P6 {w} {h} 255\n".encode() + bytes(out))


if __name__ == "__main__":
    import sys
    from showcase_palette import CLAUDE
    preview(sys.argv[1], CLAUDE)
