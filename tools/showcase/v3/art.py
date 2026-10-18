"""The v3 characters' pixel art: frames (1x RGBA PNGs made from Codex
references with pixelate.py, then cleaned) turned into palette-indexed
frames and sprite packages.

A character is a directory of frames NAME.png plus order.txt (one frame name
per line, in package order). Every frame is anchored at the bottom row,
under the middle of its head.
"""
import os

import numpy as np
from PIL import Image

import romx

HERE = os.path.dirname(os.path.abspath(__file__))
ART = os.path.join(HERE, "art")


def load(char, names=None):
    d = os.path.join(ART, char)
    if names is None:
        names = [ln.strip() for ln in open(os.path.join(d, "order.txt")) if ln.strip() and not ln.startswith("#")]
    return names, [np.asarray(Image.open(os.path.join(d, f"{n}.png")).convert("RGBA")) for n in names]


def palette(frames, extra=()):
    """The colors of all frames (at most 15), most used first; index 0 is
    transparent."""
    from collections import Counter
    c = Counter()
    for f in frames:
        for px in f[f[..., 3] > 0][:, :3]:
            c[tuple(int(v) >> 3 << 3 for v in px)] += 1
    cols = [k for k, _ in c.most_common()] + [tuple(x) for x in extra]
    cols = list(dict.fromkeys(cols))
    assert len(cols) <= 15, f"{len(cols)} colors"
    return cols


def indexed(f, cols):
    out = np.zeros(f.shape[:2], np.uint8)
    a = f[..., 3] > 0
    rgb = (f[..., :3].astype(int) >> 3) << 3
    lut = {c: i + 1 for i, c in enumerate(cols)}
    for y, x in zip(*np.where(a)):
        out[y, x] = lut[tuple(int(v) for v in rgb[y, x])]
    return out


def feet(img, head_rows=10):
    """(x, y) of the anchor: the bottom of the frame, and the middle of the
    top rows (the head stays put through a walk cycle; the feet don't)."""
    ys, xs = np.where(img > 0)
    top = ys.min()
    hx = xs[ys < top + head_rows]
    return int(round((hx.min() + hx.max() + 1) / 2)), int(ys.max() + 1)


def pool_colors(cols):
    return [0x786860] + [r << 16 | g << 8 | b for r, g, b in cols] + [0] * (15 - len(cols))


def package(show, name, char, names=None, recolor=None):
    """Add the frames of `char` as package `name` in a new pool; returns
    (package, {frame name: index}, pool). recolor: {old rgb: new rgb}."""
    names, frames = load(char, names)
    cols = palette(frames)
    imgs = [indexed(f, cols).tolist() for f in frames]
    if recolor:
        cols = [recolor.get(c, c) for c in cols]
    pool = show.pool(pool_colors(cols))
    data = romx.package(imgs, pool, lambda i, img: feet(np.array(img)))
    return show.package(name, data), {n: i for i, n in enumerate(names)}, pool
