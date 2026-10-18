#!/usr/bin/env python3
"""Redraw the Claudesona's face in every frame with the same features.

  python3 tools/showcase/v3/faces.py [--preview OUT.png]

The Codex frames drew the eyes a little differently each time (a dot here,
a dash or a wink there). This finds the white face disc in each frame,
clears what was drawn inside it, and draws one set of features: two 1x2
eyes and a small smile, an open mouth for talking, happy arcs for cheering
and jumping. Back views (no face) are left alone. Edits art/claude and
art/reveal in place; the originals stay in git history.
"""
import argparse
import os

import numpy as np
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
ART = os.path.join(HERE, "art")
INK = (24, 16, 16, 255)        # snapped to the frame set's own colors after drawing
MOUTH = {"talk": "open", "cheer": "arcs", "jump": "arcs"}
SKIP = {"back_ur", "back_ul", "walk_ur1", "walk_ur2", "walk_ur3", "walk_ur4", "cloak", "cracks", "burn1", "burn2", "hood"}


def face_box(img):
    """The face: the largest run of near-white pixels in the top half; its
    rows' white spans give the disc (holes inside it are features)."""
    a = img[..., 3] > 0
    w = a & (img[..., 0] > 215) & (img[..., 1] > 215) & (img[..., 2] > 200)
    h = img.shape[0]
    w[h // 2:] = False
    ys, xs = np.where(w)
    if len(ys) < 20:
        return None
    # keep the densest cluster: pixels near the median
    my, mx = np.median(ys), np.median(xs)
    keep = (abs(ys - my) < 7) & (abs(xs - mx) < 7)
    ys, xs = ys[keep], xs[keep]
    return ys.min(), ys.max(), xs.min(), xs.max(), w


def redraw(img, name):
    fb = face_box(img)
    if fb is None:
        return img, False
    y0, y1, x0, x1, white = fb
    out = img.copy()
    face_col = tuple(int(v) for v in np.median(img[white][:, :4], axis=0))
    for y in range(y0, y1 + 1):
        row = np.where(white[y, x0:x1 + 1])[0]
        if len(row) == 0:
            continue
        for x in range(x0 + row.min(), x0 + row.max() + 1):
            if not white[y, x]:
                out[y, x] = face_col
    # features drawn on the disc's edge (a big eye on the rim): dark pixels
    # next to the face, within a pixel of its box, become face too
    face = np.all(out == face_col, axis=2) | white
    for _ in range(2):
        for y in range(y0, y1 + 1):
            for x in range(max(0, x0 - 1), min(out.shape[1], x1 + 2)):
                px = out[y, x]
                if face[y, x] or px[3] == 0 or int(px[0]) + int(px[1]) + int(px[2]) > 240:
                    continue
                n = sum(face[yy, xx] for yy, xx in ((y - 1, x), (y + 1, x), (y, x - 1), (y, x + 1))
                        if 0 <= yy < out.shape[0] and 0 <= xx < out.shape[1])
                if n >= 2:
                    out[y, x] = face_col
                    face[y, x] = True
    cx = (x0 + x1) / 2 + (-0.5 if name.endswith("_dl") or name.endswith("_ul") else 0.5)
    cx = int(round(cx))
    hgt = y1 - y0 + 1
    ey = y0 + int(round(hgt * 0.36))
    my = y0 + int(round(hgt * 0.68))
    kind = MOUTH.get(name, "smile")
    if kind == "arcs":
        for ex in (cx - 2, cx + 2):
            out[ey + 1, ex - 1] = INK
            out[ey, ex] = INK
            out[ey + 1, ex + 1] = INK if ex + 1 != cx else out[ey + 1, ex + 1]
    else:
        for ex in (cx - 2, cx + 1):
            out[ey, ex] = INK
            out[ey + 1, ex] = INK
    if kind == "open":
        for dx in (-1, 0):
            out[my, cx + dx] = INK
            out[my + 1, cx + dx] = (160, 48, 40, 255)
    else:
        out[my, cx - 2] = INK
        out[my + 1, cx - 1] = INK
        out[my + 1, cx] = INK
        out[my, cx + 1] = INK
    return out, True


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--preview")
    ap.add_argument("--dry", action="store_true")
    a = ap.parse_args()
    shown = []
    for char in ("claude", "reveal"):
        d = os.path.join(ART, char)
        names = [ln.strip() for ln in open(os.path.join(d, "order.txt")) if ln.strip()]
        # the set's colors before the edit: new pixels snap to the nearest (15 at most)
        cols = set()
        for n in names:
            a_ = np.asarray(Image.open(os.path.join(d, n + ".png")).convert("RGBA"))
            cols |= {tuple(int(v) >> 3 << 3 for v in px[:3]) for px in a_[a_[..., 3] > 0]}
        pal = np.array(sorted(cols))
        for n in names:
            if n in SKIP:
                continue
            p = os.path.join(d, n + ".png")
            img = np.asarray(Image.open(p).convert("RGBA")).copy()
            out, ok = redraw(img, n)
            m = out[..., 3] > 0
            px = (out[m][:, :3].astype(int) >> 3) << 3
            out[m, :3] = pal[np.argmin(((px[:, None, :] - pal[None]) ** 2).sum(2), axis=1)]
            if ok:
                if not a.dry:
                    Image.fromarray(out, "RGBA").save(p)
                shown.append((img, out))
    if a.preview:
        W = sum(max(i.shape[1], o.shape[1]) + 4 for i, o in shown)
        H = max(i.shape[0] for i, _ in shown) * 2 + 8
        sheet = Image.new("RGBA", (W, H), (92, 84, 72, 255))
        x = 2
        for i, o in shown:
            sheet.alpha_composite(Image.fromarray(i, "RGBA"), (x, 2))
            sheet.alpha_composite(Image.fromarray(o, "RGBA"), (x, H // 2 + 2))
            x += max(i.shape[1], o.shape[1]) + 4
        sheet.resize((W * 5, H * 5), Image.NEAREST).save(a.preview)


if __name__ == "__main__":
    main()
