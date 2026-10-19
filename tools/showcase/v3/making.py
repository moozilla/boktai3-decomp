#!/usr/bin/env python3
"""Showcase v3, part 2: "How I made this video", told by Claude.

  python3 tools/showcase/v3/making.py [--stills]   -> build/showcase/v3/rec/making.mp4

Screens of the real work behind part 1, each with Claude talking in the
game's message window (its colors, its font, her portrait), typing as the
game does: the Codex drawings and the sprite frames cut from them, the
arena's palettes recolored, a scene's cutscene table and script decoded
back out of the ROM, Django's motions tried one at a time, Vanargand's head
tracked frame by frame, the sound audition, the palette memory behind the
overload, and every take recorded at once.

Inputs (build/, ROM-derived, never committed): the font part1.py exports,
the recordings in rec/, proofs.py kneel (KGAP=90 KSEQ=20:1,22:1,28,
17:1:0:0,18:1:0:0,12:1,43) and sfx (copied to sfx_making.mp4, the list in
SOUNDS), part1.py measure ride (runs/p1_ride_m), the palette dump in
runs/reveal_pal, the arena proof (arena.png) and the Codex sheets under
build/showcase/refs. --stills writes one frame per screen instead.
"""
import argparse
import os
import subprocess
import sys

import numpy as np
from PIL import Image, ImageDraw, ImageFont

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(os.path.dirname(HERE)))
V3 = os.path.join(ROOT, "build", "showcase", "v3")
REC = os.path.join(V3, "rec")
REFS = os.path.join(ROOT, "build", "showcase", "refs", "codex", "out")
ART = os.path.join(HERE, "art")
W, H, FPS = 1920, 1080, 30
BG = (27, 25, 22)
INK, MUTED, ACCENT = (236, 230, 218), (154, 144, 131), (224, 116, 90)
# the game's message window (sampled from a recording): border, inner line, fill, text and its two shades
BOX_EDGE, BOX_LINE, BOX_FILL = (90, 33, 16), (247, 198, 165), (255, 255, 231)
TEXT = {1: (165, 107, 107), 2: (231, 156, 132), 3: (247, 198, 165)}
MONO = "/System/Library/Fonts/Menlo.ttc"
CPS, HOLD = 45, 1.9          # typing speed (characters a second) and the hold after a page
VIS = (120, 190, 1800, 700)  # the visual area: x0, y0, x1, y1

SOUNDS = [1407, 201, 202, 205, 1243, 1244, 1246, 1425, 641, 923, 1271, 772, 507, 1414, 868, 349, 1104, 1148, 1149,
          1277, 1295, 1296, 1029, 927, 1267, 1275, 1404, 504, 505, 506, 664, 807, 895, 769, 770, 429, 431, 1108]
SOUND_GAP = 100
PICKS = {201: "snap", 202: "snap", 1246: "arrival", 1425: "arrival", 923: "charge", 1271: "hum", 772: "alarm",
         507: "swell", 1414: "white-out"}

_font = None


def font():
    global _font
    if _font is None:
        _font = np.load(os.path.join(V3, "font8x16.npy"))
    return _font


def text_img(text, scale, colors):
    """RGBA of a line in the game's font; colors: {1: main, 2, 3: shades} or one color for all."""
    if isinstance(colors, tuple):
        colors = {1: colors, 2: tuple(int(c * .62) for c in colors), 3: tuple(int(c * .3) for c in colors)}
    f = font()
    img = np.zeros((16, 8 * len(text), 4), np.uint8)
    for i, ch in enumerate(text):
        g = f[max(0, min(95, ord(ch) - 0x20))]
        for v, col in colors.items():
            img[:, 8 * i:8 * i + 8][g == v] = col + (255,)
    return Image.fromarray(img.repeat(scale, 0).repeat(scale, 1), "RGBA")


def wrap(text, n):
    out, line = [], ""
    for w in text.split():
        if len(line) + len(w) + (1 if line else 0) > n:
            out.append(line)
            line = w
        else:
            line = (line + " " + w).strip()
    return out + ([line] if line else [])


def scaled(img, k):
    return img.resize((int(img.width * k), int(img.height * k)), Image.NEAREST)


def fit(img, w, h, nearest=True):
    k = min(w / img.width, h / img.height)
    return img.resize((max(1, int(img.width * k)), max(1, int(img.height * k))), Image.NEAREST if nearest else Image.LANCZOS)


def frame_at(video, t, crop=None):
    """One frame of a recording (its 4x size) as an image, optionally cropped (game pixels x, y, w, h)."""
    out = subprocess.run(["ffmpeg", "-v", "error", "-ss", f"{t:.3f}", "-i", video, "-frames:v", "1", "-f", "image2pipe",
                          "-vcodec", "png", "-"], capture_output=True, check=True).stdout
    import io
    img = Image.open(io.BytesIO(out)).convert("RGB")
    img = img.resize((240, 160), Image.NEAREST)
    if crop:
        x, y, w, h = crop
        img = img.crop((x, y, x + w, y + h))
    return img


def label(canvas, text, x, y, scale=2, color=MUTED, center=False):
    t = text_img(text, scale, color)
    canvas.alpha_composite(t, (int(x - t.width / 2) if center else int(x), int(y)))


def arrow(canvas, x, y, color=MUTED):
    d = ImageDraw.Draw(canvas)
    d.line([(x - 30, y), (x + 30, y)], fill=color + (255,), width=6)
    d.polygon([(x + 30, y - 16), (x + 52, y), (x + 30, y + 16)], fill=color + (255,))


def panel(canvas, box, title=None):
    d = ImageDraw.Draw(canvas)
    d.rounded_rectangle(box, 10, fill=(36, 33, 29, 255), outline=(58, 53, 46, 255), width=2)
    if title:
        label(canvas, title, box[0] + 18, box[1] + 14, 2, MUTED)


# ---- the screens' visuals: functions of (canvas, t) drawing into VIS -----------------------------
def v_hello(c, t):
    names = ["stand_dr", "wave", "stand_dr", "cheer"]
    img = Image.open(os.path.join(ART, "claude", names[int(t / 0.6) % len(names)] + ".png")).convert("RGBA")
    img = scaled(img, 9)
    cx, cy = (VIS[0] + VIS[2]) // 2, (VIS[1] + VIS[3]) // 2
    c.alpha_composite(img, (cx - img.width // 2, cy - img.height // 2))


_cache = {}


def cached(key, fn):
    if key not in _cache:
        _cache[key] = fn()
    return _cache[key]


def v_drawing(c, t):
    sheet = cached("sheet", lambda: Image.open(os.path.join(REFS, "claudesona_sheet_r3a.png")).convert("RGBA"))
    x0, y0, x1, y1 = VIS
    panel(c, (x0, y0, x0 + 560, y1), "Codex's drawing")
    s = fit(sheet, 520, 440, nearest=False)
    c.alpha_composite(s, (x0 + 20, y0 + 50))
    arrow(c, x0 + 600, (y0 + y1) // 2)
    panel(c, (x0 + 660, y0, x0 + 1180, y1), "pixel frames, true size")
    names = ["stand_dr", "walk_dr1", "walk_dr2", "talk", "cheer", "point", "wave", "drag1", "ride"]
    for i, n in enumerate(names):
        img = scaled(Image.open(os.path.join(ART, "claude", n + ".png")).convert("RGBA"), 4)
        cx, cy = x0 + 660 + 90 + (i % 3) * 170, y0 + 70 + (i // 3) * 150
        c.alpha_composite(img, (cx - img.width // 2 + 0, cy))
    arrow(c, x0 + 1220, (y0 + y1) // 2)
    panel(c, (x0 + 1280, y0, x1, y1), "in the game")
    g = cached("ingame", lambda: scaled(frame_at(os.path.join(REC, "coffins.mp4"), 19.5, (80, 30, 80, 90)), 4.6))
    c.alpha_composite(g.convert("RGBA"), (x0 + 1280 + (x1 - x0 - 1280 - g.width) // 2, y0 + 60))


def v_arena(c, t):
    x0, y0, x1, y1 = VIS
    # the proof's third shot (its montage: 240x160 tiles, 244 apart) and the piece before anything moves
    a = cached("arena0", lambda: Image.open(os.path.join(V3, "arena.png")).convert("RGB").crop((490, 2, 730, 162)))
    b = cached("arena1", lambda: frame_at(os.path.join(REC, "coffins.mp4"), 3.0))
    for k, (img, cap) in enumerate(((a, "the game's palettes"), (b, "my recolored copies"))):
        x = x0 + 40 + k * 840
        panel(c, (x, y0, x + 780, y1), cap)
        im = fit(img, 740, 440)
        c.alpha_composite(im.convert("RGBA"), (x + 20, y0 + 50))


def decoded_lines():
    sys.path[:0] = [os.path.join(ROOT, "tools"), os.path.join(ROOT, "tools", "assets")]
    import contextlib
    import io
    import cutscenes
    import gcx
    import gclview
    rom = open(os.path.join(V3, "part1.gba"), "rb").read()
    cutscenes._rom = rom
    gcx.load_rom = lambda *a, **k: rom
    out = []
    for mod, args in ((cutscenes, ["cutscenes.py", "127"]), (gclview, ["gclview.py", "16060"])):
        buf = io.StringIO()
        sys.argv = args
        with contextlib.redirect_stdout(buf):
            try:
                mod.main()
            except SystemExit:
                pass
        out.append(buf.getvalue().splitlines())
    return out


def v_scenes(c, t):
    table, script = cached("decoded", decoded_lines)
    x0, y0, x1, y1 = VIS
    f = ImageFont.truetype(MONO, 17)
    d = ImageDraw.Draw(c)
    for k, (lines, cap) in enumerate(((table[1:21], "the cutscene table, decoded from the ROM"),
                                      (script[1:21], "its actions, as a GCL script"))):
        x = x0 + k * 845
        panel(c, (x, y0, x + 815, y1), cap)
        shown = int(max(0, t - 0.3) * 30)          # the lines scroll in
        for i, ln in enumerate(lines[:min(len(lines), shown)]):
            ln = ln.replace("ScriptKeyframe_", "Keyframe.")
            d.text((x + 18, y0 + 48 + i * 22.5), ln[:80], font=f, fill=(206, 198, 186, 255))


def kneel_shots():
    d = os.path.join(V3, "runs", "kneel")
    picks = [(80, "20: a spin"), (245, "28: a transformation"), (485, "12: a kneel"), (575, "43: frozen blue"),
             (305, "17: knocked down")]
    return [(Image.open(os.path.join(d, f"k{t:05d}.png")).convert("RGB").crop((60, 30, 180, 130)), cap) for t, cap in picks]


def v_kneel(c, t):
    shots = cached("kneel", kneel_shots)
    x0, y0, x1, y1 = VIS
    d = ImageDraw.Draw(c)
    for i, (img, cap) in enumerate(shots):
        x = x0 + 20 + i * 336
        im = scaled(img, 2.6)
        on = i == len(shots) - 1 and t > 4.0
        c.alpha_composite(im.convert("RGBA"), (x, y0 + 60))
        d.rectangle((x - 4, y0 + 56, x + im.width + 4, y0 + 64 + im.height), outline=(ACCENT if on else (58, 53, 46)) + (255,),
                    width=6 if on else 2)
        label(c, cap, x + im.width // 2, y0 + 90 + im.height, 2, ACCENT if on else INK, center=True)


def head_track():
    d = os.path.join(V3, "runs", "p1_ride_m")
    ys = []
    for t in range(330, 440, 2):
        a = np.asarray(Image.open(os.path.join(d, f"h{t:05d}.png")).convert("RGB")).astype(int)[:, 110:130]
        m = (a[..., 0] >= 190) & (a[..., 1] >= 20) & (a[..., 2] < 10)
        rows = np.where(m.sum(1) >= 2)[0]
        ys.append((t, int(rows.min()) if len(rows) else None))
    frames = [Image.open(os.path.join(d, f"h{t:05d}.png")).convert("RGB") for t in (350, 366, 380, 400)]
    return frames, ys


def v_head(c, t):
    frames, ys = cached("head", head_track)
    x0, y0, x1, y1 = VIS
    d = ImageDraw.Draw(c)
    for i, img in enumerate(frames):
        x, y = x0 + (i % 2) * 400, y0 + (i // 2) * 260
        im = scaled(img, 1.6)
        c.alpha_composite(im.convert("RGBA"), (x, y))
        tt = (350, 366, 380, 400)[i]
        top = dict(ys).get(tt)
        if top is not None:
            d.line([(x, y + top * 1.6), (x + im.width, y + top * 1.6)], fill=ACCENT + (255,), width=3)
    gx0, gy0, gx1, gy1 = x0 + 860, y0 + 20, x1 - 20, y1 - 40
    panel(c, (gx0 - 20, y0, x1, y1), "the head's height, frame by frame")
    pts = [(tt, yy) for tt, yy in ys if yy is not None]
    tmin, tmax = pts[0][0], pts[-1][0]
    shown = int(len(pts) * min(1.0, max(0.0, (t - 0.5) / 2.5)))
    for k, (tt, yy) in enumerate(pts[:shown]):
        px = gx0 + 40 + (tt - tmin) / (tmax - tmin) * (gx1 - gx0 - 80)
        py = gy0 + 60 + yy / 115 * (gy1 - gy0 - 100)
        d.ellipse((px - 5, py - 5, px + 5, py + 5), fill=ACCENT + (255,))
    label(c, "frames after the command", (gx0 + gx1) / 2, gy1 - 6, 2, MUTED, center=True)


def sound_envs():
    raw = os.path.join(V3, "sfx_making.raw")
    subprocess.run(["ffmpeg", "-v", "error", "-y", "-i", os.path.join(V3, "sfx_making.mp4"), "-ac", "1", "-ar", "16000",
                    "-f", "s16le", raw], check=True)
    x = np.fromfile(raw, dtype=np.int16).astype(float) / 32768
    sr, hop = 16000, 160
    env = np.array([np.sqrt(np.mean(x[i:i + hop] ** 2)) for i in range(0, len(x) - hop, hop)])
    start = int(3.5 * sr / hop)
    first = (start + np.where(env[start:] > 0.005)[0][0]) * hop / sr
    gap = SOUND_GAP / 59.7275
    out = []
    for k, n in enumerate(SOUNDS):
        a, b = int((first + k * gap - 0.02) * sr), int((first + (k + 1) * gap - 0.1) * sr)
        out.append((n, x[a:b]))
    return out


def v_sounds(c, t):
    envs = cached("sounds", sound_envs)
    x0, y0, x1, y1 = VIS
    d = ImageDraw.Draw(c)
    cols, cw, ch = 7, 236, 82
    shown = int(len(envs) * min(1.0, t / 2.5))
    for i, (n, seg) in enumerate(envs[:shown]):
        x, y = x0 + (i % cols) * (cw + 4), y0 + (i // cols) * (ch + 4)
        pick = n in PICKS and t > 3.5
        d.rectangle((x, y, x + cw, y + ch), fill=((70, 40, 32) if pick else (36, 33, 29)) + (255,),
                    outline=((ACCENT if pick else (58, 53, 46)) + (255,)), width=2)
        bins = np.array_split(np.abs(seg), cw - 12)
        peak = max(1e-4, np.abs(seg).max())
        mid = y + ch // 2 + 8
        for j, bb in enumerate(bins):
            v = (bb.max() if len(bb) else 0) / max(peak, 0.15) * (ch / 2 - 14)
            d.line([(x + 6 + j, mid - v), (x + 6 + j, mid + v)], fill=(ACCENT if pick else (150, 140, 128)) + (255,))
        label(c, str(n) + (" " + PICKS[n] if pick else ""), x + 6, y + 4, 2, INK if pick else MUTED)


def v_overload(c, t):
    x0, y0, x1, y1 = VIS
    pal = np.frombuffer(open(os.path.join(V3, "runs", "reveal_pal", "pal.bin"), "rb").read()[:512], "<u2")
    panel(c, (x0, y0, x0 + 760, y1), "palette memory, mid-scene")
    d = ImageDraw.Draw(c)
    s = 26
    for i, v in enumerate(pal):
        b, k = divmod(i, 16)
        col = ((v & 31) << 3, (v >> 5 & 31) << 3, (v >> 10 & 31) << 3)
        px, py = x0 + 40 + k * (s + 2), y0 + 52 + b * (s + 2)
        d.rectangle((px, py, px + s, py + s), fill=col + (255,))
    if t > 1.5:
        px, py = x0 + 40 + 10 * (s + 2), y0 + 52 + 13 * (s + 2)
        d.ellipse((px - 14, py - 14, px + s + 14, py + s + 14), outline=ACCENT + (255,), width=5)
        label(c, "the gauge's color", x0 + 520, py + 6, 2, ACCENT)
    panel(c, (x0 + 800, y0, x1, y1), "the SUN gauge")
    a = cached("g0", lambda: frame_at(os.path.join(REC, "reveal.mp4"), 16.0, (40, 132, 200, 28)))
    flash = cached("gf", lambda: [frame_at(os.path.join(REC, "reveal.mp4"), 17.0 + 0.11 * k, (40, 132, 200, 28)) for k in range(12)])
    im = scaled(a, 4.2)
    c.alpha_composite(im.convert("RGBA"), (x0 + 820, y0 + 70))
    label(c, "full", x0 + 830, y0 + 190, 2, MUTED)
    f = scaled(flash[int(t * 9) % len(flash)], 4.2)
    c.alpha_composite(f.convert("RGBA"), (x0 + 820, y0 + 270))
    label(c, "past full: flashing, shaking", x0 + 830, y0 + 390, 2, ACCENT)


SCREENS = [
    ("How I made this video", v_hello,
     ["Hi, I'm Claude, the stranger! I made everything in Part 1 with the tools you just saw. Here's how."]),
    ("Drawing myself", v_drawing,
     ["I drew my sprites with an image model, using the game's own sprites as the style guide, then cut each "
      "drawing into true-size pixel frames.",
      "Then I packed them into the game's sprite format, with their colors as new palettes added to the game's "
      "palette table."]),
    ("Borrowing the game's art", v_arena,
     ["Wherever I could, I reused the game's own art: Django's coffin and the chain he drags it on, and its font "
      "for the ones and zeros.",
      "The Pile Driver arena is the real one. I copied its palettes, recolored the copies, and pointed a copy of "
      "its stage script at them."]),
    ("Writing the scenes", v_scenes,
     ["Each scene is a new cutscene table and new GCL scripts, assembled straight into the ROM: the camera, the "
      "music, the dialogue, every sprite's moves.",
      "The game's own cutscene engine plays them. None of the game's code was changed."]),
    ("Testing Django's moves", v_kneel,
     ["For the Count fight I needed a move where Django gets knocked down, so I tried his motion commands one at "
      "a time in a test scene.",
      "One spun him, one transformed him, one froze him blue. Command 17 was the one."]),
    ("Riding Vanargand", v_head,
     ["To ride Vanargand, I filmed his head rising with the camera held still, and tracked it pixel by pixel.",
      "The rider follows that curve frame by frame, drawn behind his layer so his head stays in front."]),
    ("Finding the sounds", v_sounds,
     ["The game's sounds have no names, so I recorded dozens of them and measured each one's length, pitch and "
      "noise.",
      "Then I picked the snap, the stranger's arrival, the charge, the alarm and the swell."]),
    ("The overload", v_overload,
     ["For the overload, I read the game's palette memory mid-scene to find the SUN gauge's color, then made it "
      "flash while the HUD shakes.",
      "The gauge runs past full because the code that draws it never stops at ten. I just kept raising the level."]),
    ("Recording every take", None,
     ["Then the game recorded every take itself, in our headless emulator harness, with the scenes running side "
      "by side.",
      "Every frame of Part 1 came out of the real game."]),
]
GRID = ["cold", "doors", "night", "marionettes", "count", "ride", "reveal", "coffins", "vride", "sunback", "title",
        "credits"]


def portrait():
    return scaled(Image.open(os.path.join(ART, "portraits", "claude_neutral.png")).convert("RGBA"), 6)


LINE = 42      # characters a line in the message window (three lines a page)


def split_pages(pages):
    """Pages longer than three lines become several, as the game would."""
    out = []
    for p in pages:
        lines = wrap(p, LINE)
        n = -(-len(lines) // 3)                    # pages needed, with the lines shared out evenly
        per = -(-len(lines) // n)
        out += [" ".join(lines[i:i + per]) for i in range(0, len(lines), per)]
    return out


def pages_timing(pages):
    out, t = [], 0.4
    for p in split_pages(pages):
        dur = len(p) / CPS + HOLD
        out.append((t, dur, p))
        t += dur
    return out, t + 0.3


def draw_chrome(c, k, title, pages, t):
    """The kicker, the title, the portrait and the message window, typing."""
    if k:
        label(c, f"How I made this video   {k}/{len(SCREENS) - 1}", 120, 44, 3, MUTED)
    label(c, title, 120, 92, 5, ACCENT if k == 0 else INK)
    p = cached("portrait", portrait)
    c.alpha_composite(p, (120, 780))
    d = ImageDraw.Draw(c)
    bx0, by0, bx1, by1 = 340, 740, 1800, 1050
    d.rectangle((bx0, by0, bx1, by1), fill=BOX_EDGE + (255,))
    d.rectangle((bx0 + 5, by0 + 5, bx1 - 5, by1 - 5), fill=BOX_LINE + (255,))
    d.rectangle((bx0 + 10, by0 + 10, bx1 - 10, by1 - 10), fill=BOX_FILL + (255,))
    label(c, "Claude:", bx0 + 34, by0 + 26, 4, TEXT)
    timing, _ = pages_timing(pages)
    for start, dur, text in timing:
        if start <= t < start + dur or (text is timing[-1][2] and t >= start):
            n = int((t - start) * CPS)
            lines = wrap(text, LINE)
            left = n
            for i, ln in enumerate(lines[:3]):
                shown = ln[:max(0, left)]
                left -= len(ln) + 1
                if shown:
                    label(c, shown, bx0 + 34, by0 + 26 + 64 * (i + 1), 4, TEXT)
            break


def render(out, stills=False):
    os.makedirs(os.path.dirname(out), exist_ok=True)
    tmp = os.path.join(V3, "making_tmp")
    os.makedirs(tmp, exist_ok=True)
    parts = []
    for k, (title, vis, pages) in enumerate(SCREENS):
        _, dur = pages_timing(pages)
        o = os.path.join(tmp, f"screen{k}.mp4")
        times = [dur * 0.85] if stills else [f / FPS for f in range(int(dur * FPS))]
        if stills:
            c = Image.new("RGBA", (W, H), BG + (255,))
            if vis:
                vis(c, times[0])
            draw_chrome(c, k, title, pages, times[0])
            c.convert("RGB").save(os.path.join(tmp, f"still{k}.png"))
            continue
        p = subprocess.Popen(["ffmpeg", "-v", "error", "-y", "-f", "rawvideo", "-pix_fmt", "rgb24", "-s", f"{W}x{H}",
                              "-r", str(FPS), "-i", "-", "-c:v", "libx264", "-crf", "16", "-preset", "fast",
                              "-pix_fmt", "yuv420p", o], stdin=subprocess.PIPE)
        for t in times:
            c = Image.new("RGBA", (W, H), BG + (255,))
            if vis:
                vis(c, t)
            draw_chrome(c, k, title, pages, t)
            a = np.asarray(c.convert("RGB")).astype(np.float32)
            f = min(1.0, t / 0.3, (dur - t) / 0.3)
            p.stdin.write((a * max(0.0, f) + np.array(BG) * (1 - max(0.0, f))).astype(np.uint8).tobytes())
        p.stdin.close()
        p.wait()
        if vis is None:
            o = grid_over(o, dur, tmp)
        parts.append(o)
    if stills:
        files = [os.path.join(tmp, f"still{k}.png") for k in range(len(SCREENS))]
        subprocess.run(["magick", "montage", "-font", "/System/Library/Fonts/Supplemental/Arial.ttf", *files,
                        "-tile", "3x", "-geometry", "640x360+2+2", out.replace(".mp4", "_stills.jpg")], check=True)
        print(out.replace(".mp4", "_stills.jpg"))
        return
    lst = os.path.join(tmp, "list.txt")
    with open(lst, "w") as f:
        for o in parts:
            f.write(f"file '{o}'\n")
    subprocess.run(["ffmpeg", "-v", "error", "-y", "-f", "concat", "-safe", "0", "-i", lst, "-f", "lavfi", "-i",
                    "anullsrc=r=48000:cl=stereo", "-map", "0:v", "-map", "1:a", "-shortest", "-c:v", "libx264",
                    "-crf", "16", "-preset", "fast", "-c:a", "aac", out], check=True)
    print(out)


def grid_over(chrome, dur, tmp):
    """The last screen: every part 1 recording playing at once, fanning out
    from one screen into a grid, laid into the visual area."""
    x0, y0, x1, y1 = VIS
    gw, gh = x1 - x0, y1 - y0
    cols, rows = 4, 3
    tw, th = gw // cols, gh // rows
    ins, filt = [], []
    for i, name in enumerate(GRID):
        src = os.path.join(REC, f"{name}.mp4")
        ins += ["-ss", "4", "-t", f"{dur:.2f}", "-i", src]
        filt.append(f"[{i}:v]scale={tw - 6}:{th - 6}:flags=neighbor,pad={tw}:{th}:3:3:color=0x24211d,setpts=PTS-STARTPTS,fps={FPS}[g{i}]")
    lay = "|".join(f"{(i % cols) * tw}_{(i // cols) * th}" for i in range(len(GRID)))
    filt.append("".join(f"[g{i}]" for i in range(len(GRID))) + f"xstack=inputs={len(GRID)}:layout={lay}[grid]")
    # the fan-out: from the first tile filling the area to the whole grid, over the first 1.6 s
    filt.append(f"[grid]zoompan=z='if(lte(on,{FPS * 0.4:.0f}),{cols},max(1,{cols}-({cols}-1)*(on-{FPS * 0.4:.0f})/{FPS * 1.6:.0f}))'"
                f":x='0':y='0':d=1:s={gw}x{gh}:fps={FPS}[fan]")
    filt.append(f"[{len(GRID)}:v][fan]overlay={x0}:{y0}:shortest=1[v]")
    o = os.path.join(tmp, "screen_grid.mp4")
    subprocess.run(["ffmpeg", "-v", "error", "-y", *ins, "-i", chrome, "-filter_complex", ";".join(filt), "-map", "[v]",
                    "-t", f"{dur:.2f}", "-c:v", "libx264", "-crf", "16", "-preset", "fast", "-pix_fmt", "yuv420p", o],
                   check=True)
    return o


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--stills", action="store_true")
    ap.add_argument("--out", default=os.path.join(REC, "making.mp4"))
    a = ap.parse_args()
    render(a.out, a.stills)


if __name__ == "__main__":
    main()
