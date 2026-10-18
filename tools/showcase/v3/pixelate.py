#!/usr/bin/env python3
"""Turn an upscaled pixel-art sheet (a Codex image: sprites drawn at about 4x
on flat green) into true-resolution sprites with one shared palette.

  pixelate.py SHEET.png OUTDIR [--colors 15] [--pitch P] [--names a,b,...]

1. Key out the background (pure green and its anti-aliased fringe).
2. Find the sprites: bands of rows with content, then runs of columns.
3. Find the pixel pitch: the grid spacing that lines up the most color edges
   (or --pitch), and its phase per sprite.
4. Sample each grid cell at its center (the median of the inner half).
5. Quantize every sprite of the sheet to one palette (k-means, --colors
   entries plus transparency) and write OUTDIR/NAME.png (RGBA, 1x) and
   OUTDIR/palette.txt.

The output is a starting point for hand cleanup (cleanup.py), not final art.
"""
import argparse
import os

import numpy as np
from PIL import Image


def key(rgb):
    r, g, b = rgb[..., 0].astype(int), rgb[..., 1].astype(int), rgb[..., 2].astype(int)
    return (g > 140) & (g - r > 60) & (g - b > 60)


def runs(v, gap=3):
    """[(start, end)] of True runs in v, merging gaps shorter than `gap`."""
    out, start, last = [], None, None
    for i, x in enumerate(v):
        if x:
            if start is None:
                start = i
            elif i - last > gap:
                out.append((start, last + 1))
                start = i
            last = i
    if start is not None:
        out.append((start, last + 1))
    return out


def sprites(mask, min_size=24):
    boxes = []
    for y0, y1 in runs(mask.any(axis=1), gap=6):
        for x0, x1 in runs(mask[y0:y1].any(axis=0), gap=6):
            sub = mask[y0:y1, x0:x1]
            ys = np.where(sub.any(axis=1))[0]
            if x1 - x0 >= min_size and len(ys) and ys[-1] - ys[0] >= min_size:
                boxes.append((x0, y0 + ys[0], x1, y0 + ys[-1] + 1))
    return boxes


def edges(img, mask):
    a = img.astype(int).sum(axis=2) + (~mask) * 2000
    ex = np.abs(np.diff(a, axis=1)) > 60
    ey = np.abs(np.diff(a, axis=0)) > 60
    return np.where(ex)[1] + 1, np.where(ey)[0] + 1


def score(pos, p, phase):
    d = (pos - phase) / p
    return np.mean(np.abs(d - np.round(d)) < 0.18)


def pitch(img, mask, lo=3.5, hi=12.0):
    """Every fraction of the true pitch lines up the edges too, so take the
    largest pitch that scores nearly as well as the best."""
    xs, ys = edges(img, mask)
    pos = np.concatenate([xs, ys]).astype(float)
    if len(pos) > 20000:
        pos = np.random.default_rng(0).choice(pos, 20000, replace=False)
    scored = [(max(score(pos, p, ph) for ph in np.arange(0, p, p / 12)), p) for p in np.arange(lo, hi, 0.02)]
    best = max(s for s, _ in scored)
    return max(p for s, p in scored if s >= 0.9 * best)


def phase(img, mask, p):
    xs, ys = edges(img, mask)
    px = max(np.arange(0, p, p / 16), key=lambda ph: score(xs.astype(float), p, ph))
    py = max(np.arange(0, p, p / 16), key=lambda ph: score(ys.astype(float), p, ph))
    return px, py


def sample(img, mask, box, p, ph):
    x0, y0, x1, y1 = box
    phx, phy = ph
    gx0 = int(np.floor((x0 - phx) / p))
    gy0 = int(np.floor((y0 - phy) / p))
    gx1 = int(np.ceil((x1 - phx) / p))
    gy1 = int(np.ceil((y1 - phy) / p))
    w, h = gx1 - gx0, gy1 - gy0
    out = np.zeros((h, w, 4), np.uint8)
    q = max(1, int(p / 4))
    for j in range(h):
        for i in range(w):
            cx = phx + (gx0 + i + 0.5) * p
            cy = phy + (gy0 + j + 0.5) * p
            xa, xb = int(cx - q), int(cx + q) + 1
            ya, yb = int(cy - q), int(cy + q) + 1
            if xa < 0 or ya < 0 or xb > img.shape[1] or yb > img.shape[0]:
                continue
            m = mask[ya:yb, xa:xb]
            if m.mean() < 0.5:
                continue
            px = img[ya:yb, xa:xb][m]
            out[j, i, :3] = np.median(px, axis=0)
            out[j, i, 3] = 255
    # the green fringe: greenish cells become transparent
    r, g, b = (out[..., k].astype(int) for k in range(3))
    out[(g > r + 25) & (g > b + 25), 3] = 0
    # trim
    a = out[..., 3] > 0
    ys, xs = np.where(a)
    return out[ys.min():ys.max() + 1, xs.min():xs.max() + 1]


def kmeans(colors, k, iters=40, seed=1):
    rng = np.random.default_rng(seed)
    c = colors.astype(float)
    # farthest-point init so small accents (eyes, glow) keep a color
    cent = [c[rng.integers(len(c))]]
    for _ in range(k - 1):
        d = np.min([((c - x) ** 2).sum(1) for x in cent], axis=0)
        cent.append(c[np.argmax(d)])
    cent = np.array(cent)
    for _ in range(iters):
        lab = np.argmin(((c[:, None] - cent[None]) ** 2).sum(2), axis=1)
        for i in range(k):
            if (lab == i).any():
                cent[i] = c[lab == i].mean(0)
    return np.round(cent).astype(np.uint8)


def to_555(c):
    return (np.asarray(c).astype(int) >> 3) << 3


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("sheet")
    ap.add_argument("out")
    ap.add_argument("--colors", type=int, default=15)
    ap.add_argument("--pitch", type=float)
    ap.add_argument("--names")
    a = ap.parse_args()
    img = np.asarray(Image.open(a.sheet).convert("RGB"))
    mask = ~key(img)
    boxes = sprites(mask)
    p = a.pitch or pitch(img, mask)
    print(f"{len(boxes)} sprites, pitch {p:.2f}")
    outs = []
    for box in boxes:
        x0, y0, x1, y1 = box
        m = 6
        sub = (max(0, x0 - m), max(0, y0 - m), min(img.shape[1], x1 + m), min(img.shape[0], y1 + m))
        crop, cm = img[sub[1]:sub[3], sub[0]:sub[2]], mask[sub[1]:sub[3], sub[0]:sub[2]]
        ph = phase(crop, cm, p)
        outs.append(sample(crop, cm, (x0 - sub[0], y0 - sub[1], x1 - sub[0], y1 - sub[1]), p, ph))
    allc = np.concatenate([o[o[..., 3] > 0][:, :3] for o in outs])
    pal = to_555(kmeans(allc, a.colors))
    os.makedirs(a.out, exist_ok=True)
    names = a.names.split(",") if a.names else [f"s{i:02d}" for i in range(len(outs))]
    for name, o in zip(names, outs):
        q = o.copy()
        on = q[..., 3] > 0
        lab = np.argmin(((q[on][:, :3].astype(float)[:, None] - pal[None].astype(float)) ** 2).sum(2), axis=1)
        q[on, :3] = pal[lab]
        Image.fromarray(q, "RGBA").save(os.path.join(a.out, f"{name}.png"))
        print(name, q.shape[1], "x", q.shape[0])
    with open(os.path.join(a.out, "palette.txt"), "w") as f:
        for c in pal:
            f.write("%02X%02X%02X\n" % tuple(c))


if __name__ == "__main__":
    main()
