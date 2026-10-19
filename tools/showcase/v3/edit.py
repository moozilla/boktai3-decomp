#!/usr/bin/env python3
"""Cut showcase v3 from its recordings.

  python3 tools/showcase/v3/edit.py [--out ~/Movies/boktai3-showcase/boktai3-showcase-v3.mp4]

Inputs (build/showcase/v3/): rec/PIECE.mp4 from part1.py, rec/studio_*.mp4
and rec/studio_live.json from tour.py, live/*.png from live_demo.lua.
Output: a 1920x1080 video with sound and NAME.chapters.txt next to it (the
chapters are embedded in the MP4 too).

Part 1 is played by the game: each recording starts with the warp into its
scene, which is cut (the first black frames end it); the game is scaled from
the recording's 4x to 6x (whole pixels) on a dark frame. Part 2 is the Studio
tour with the game's side of the live edit laid in beside it and a music
bed recorded from the game (rec/music.mp4) under it.
"""
import argparse
import json
import os
import re
import subprocess

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(os.path.dirname(HERE)))
V3 = os.path.join(ROOT, "build", "showcase", "v3")
REC = os.path.join(V3, "rec")
BG = "0x0e0c0a"
FPS = 30
FONT = "/System/Library/Fonts/Supplemental/Arial.ttf"
BOLD = "/System/Library/Fonts/Supplemental/Arial Bold.ttf"

# Part 1: (recording, chapter title or None, trims, options) in order. trims:
# None = from the scene's start to the end of its closing fade; "cuts" = skip
# the dark stretches (the chase's warps); ("abs", [(start, end, speed)]) in
# recording seconds; ("pages", [i, j]) = the crawl's pages i..j (text on
# black, found by brightness). options: bed = a song recorded on its own
# (part1.py musicN) laid under the clip, continuing where its last use
# stopped (bed_end: the stretch that ends with the song); only_bed = the bed
# replaces the clip's sound (otherwise mixed in).
PART1 = [
    ("cold", "Cold open: something is wrong with that ghoul", None, {}),
    ("doors", "The doors lie", "cuts", {"bed": 13, "fade_on_last_line": True, "lively": True}),
    ("night", "Night falls on cue", None, {}),
    ("marionettes", "The marionettes", None, {}),
    ("count", "The Count loses control", None, {"fade_on_last_line": True}),
    ("ride", "Somebody rides Vanargand", None, {}),
    ("reveal", "The overload: who the stranger was", None, {"fade_on_last_line": True}),
    ("card:title", "Introducing... Boktai 3, decompiled", None, {"bed": 92, "only_bed": True}),
    ("card:engine", None, None, {"bed": 92, "only_bed": True}),
    ("coffins", "Encore: a purification at the Pile Driver", None, {}),
    ("card:hope", None, None, {"bed": 92, "only_bed": True}),
    ("vride", "Claude rides Vanargand", None, {"bed": 92}),
    ("sunback", "The sun is in your hands", None, {"white_out": True, "fade_on_last_line": True, "fade_color": "white"}),
    ("part2", None, None, {}),
    ("credits", "Credits", ("abs", [(0.9, 58.0, 1.0)]), {"bed": 92, "only_bed": True, "bed_end": True}),
]

# The cards between the scenes, drawn in the game's own font (part1.py
# exports it): screens of text lines, each (text, scale, color, y, fly-in
# time or None), with a fade in and out.
CREAM, ACCENT = (236, 230, 218), (224, 116, 90)
ITEMS = [("Custom ROM hacks", 5, CREAM, 450), ("The game's unused content, rediscovered", 5, CREAM, 570),
         ("...and maybe even a new edition", 5, CREAM, 700), ("of the 2007 fan translation", 5, CREAM, 790)]
HOPE = ("We hope this work brings:", 6, ACCENT, 290)
CARDS = {
    "title": [(2.6, [("Introducing...", 6, CREAM, 492, None)]),
              (3.6, [("BOKTAI 3", 10, ACCENT, 380, None), ("DECOMPILED", 10, CREAM, 560, None)])],
    "engine": [(4.4, [("Every scene in this demo is running", 5, CREAM, 420, None),
                      ("in the game's actual cutscene engine.", 5, CREAM, 540, None)])],
    "hope": [(6.4, [HOPE + (None,), ITEMS[0] + (0.8,), ITEMS[1] + (1.7,), ITEMS[2] + (2.8,), ITEMS[3] + (2.8,)])],
}
TMP = os.path.join(V3, "edit_tmp")      # reused: every run overwrites it (moving it to the Trash filled the disk)


def sh(*cmd, capture=False):
    r = subprocess.run(cmd, check=True, capture_output=capture, text=True)
    return r.stdout if capture else None


def duration(path):
    return float(sh("ffprobe", "-v", "error", "-show_entries", "format=duration", "-of", "csv=p=0", path, capture=True))


def scene_start(path, default=2.3):
    """Where the recording's warp ends: the end of its first black run."""
    out = subprocess.run(["ffmpeg", "-i", path, "-t", "5", "-vf", "blackdetect=d=0.05:pix_th=0.08", "-an", "-f", "null",
                          "-"], capture_output=True, text=True).stderr
    m = re.search(r"black_end:([0-9.]+)", out)
    return float(m.group(1)) if m else default


GAME = (f"scale=1440:960:flags=neighbor,pad=1920:1080:240:60:color={BG},fps={FPS},format=yuv420p")
# the crawl's cards: text centered (the crawl prints near the top) on black (it draws on dark gray)
CARD = ("crop=960:512:0:0,pad=960:640:0:128:color=0x212121,colorlevels=rimin=0.15:gimin=0.15:bimin=0.15,"
        f"scale=1440:960:flags=neighbor,pad=1920:1080:240:60:color=black,fps={FPS},format=yuv420p")


def black_runs(path, th=0.10, min_len=0.05):
    out = subprocess.run(["ffmpeg", "-i", path, "-vf", f"blackdetect=d={min_len}:pix_th={th}", "-an", "-f", "null", "-"],
                         capture_output=True, text=True).stderr
    return [(float(a), float(b)) for a, b in re.findall(r"black_start:([0-9.]+) black_end:([0-9.]+)", out)]


def dark_runs(path, below=58, fps=30, min_len=0.1):
    """(start, end) of the stretches whose mean luma is under `below` (the
    warps between scenes fade through dark gray, not black)."""
    out = subprocess.run(["ffmpeg", "-v", "error", "-i", path, "-vf",
                          f"fps={fps},signalstats,metadata=print:key=lavfi.signalstats.YAVG:file=-", "-f", "null", "-"],
                         capture_output=True, text=True).stdout
    ys = [float(v) for v in re.findall(r"YAVG=([0-9.]+)", out)]
    runs, a = [], None
    for i, y in enumerate(ys + [255]):
        if y < below and a is None:
            a = i
        elif y >= below and a is not None:
            if (i - a) / fps >= min_len:
                runs.append((a / fps, i / fps))
            a = None
    return runs


def luma(path, key="YAVG", fps=30):
    """Per-frame luma statistic (signalstats) sampled at fps."""
    out = subprocess.run(["ffmpeg", "-v", "error", "-i", path, "-vf",
                          f"fps={fps},signalstats,metadata=print:key=lavfi.signalstats.{key}:file=-", "-f", "null", "-"],
                         capture_output=True, text=True).stdout
    return [float(v) for v in re.findall(key + r"=([0-9.]+)", out)]


def content_end(path, fps=30, keep=0.1):
    """Seconds into the recording where the closing fade (to black or white)
    is complete, plus `keep`: the recording runs on in black after it."""
    ys = luma(path, fps=fps)
    live = [i for i, y in enumerate(ys) if 24 < y < 228]
    return (live[-1] + 1) / fps + keep if live else len(ys) / fps


def last_line_end(path, after=0.0, until=None, fps=30):
    """Seconds into the recording when the last message window closes before
    `until`: its cream box fills the bottom of the screen (bright and
    yellowish; a white fade is neutral)."""
    out = subprocess.run(["ffmpeg", "-v", "error", "-ss", f"{after:.3f}", *(["-t", f"{until - after:.3f}"] if until else []),
                          "-i", path, "-vf", f"fps={fps},crop=880:150:40:460,signalstats,metadata=print:file=-",
                          "-f", "null", "-"], capture_output=True, text=True).stdout
    ys = [float(v) for v in re.findall(r"signalstats.YAVG=([0-9.]+)", out)]
    us = [float(v) for v in re.findall(r"signalstats.UAVG=([0-9.]+)", out)]
    up = [i for i, (y, u) in enumerate(zip(ys, us)) if y > 190 and u < 123]
    return after + (up[-1] + 1) / fps if up else None


def pages(path, fps=30, th=60):
    """(start, end) seconds of each crawl page: runs of frames with bright text."""
    ys = luma(path, "YMAX", fps)
    runs, a = [], None
    for i, y in enumerate(ys + [0]):
        if y > th and a is None:
            a = i
        elif y <= th and a is not None:
            if i - a > fps:          # not the warp's last frames before the crawl
                runs.append((a / fps, i / fps))
            a = None
    return runs


def motion(path, fps=30):
    """Per-frame change from the previous frame (mean of the difference)."""
    out = subprocess.run(["ffmpeg", "-v", "error", "-i", path, "-vf",
                          f"fps={fps},scale=240:160,tblend=all_mode=difference,signalstats,"
                          "metadata=print:key=lavfi.signalstats.YAVG:file=-", "-f", "null", "-"],
                         capture_output=True, text=True).stdout
    return [float(v) for v in re.findall(r"YAVG=([0-9.]+)", out)]


def lively(src, parts, s0, fps=30):
    """Each part started where its fade-in has finished (85% of the part's
    median brightness) and the picture has started moving: the chase's
    rooms open on a slow fade with Django standing still."""
    import statistics
    ys, ms = luma(src, fps=fps), motion(src, fps)
    out = []
    for a, b, sp in parts:
        i, j = int((s0 + a) * fps), min(len(ys), int((s0 + b) * fps))
        if j - i < fps:
            out.append((a, b, sp))
            continue
        med = statistics.median(ys[i:j])
        k = i
        while k < j - fps and ys[k] < 0.85 * med:
            k += 1
        while k < j - fps and k < len(ms) and ms[k] < 0.3:
            k += 1
        out.append((k / fps - s0, b, sp))
    return out


def without_black(src, s0, keep=0.1):
    """Trims that skip every dark stretch after s0 (the warps between scenes),
    keeping `keep` seconds of each side of it."""
    end = duration(src)
    parts, t = [], s0
    for a, b in dark_runs(src):
        if b <= s0:
            continue
        if a - t > 0.2:
            parts.append((t - s0, a + keep - s0, 1.0))
        t = max(t, b - keep)
    if end - t > 0.2:
        parts.append((t - s0, end - s0, 1.0))
    return parts


def text_layer(text, scale, color, font):
    """RGBA image of one line in the game's 8x16 font: color 1 is the line's
    color, 2 and 3 its shading (the font's own), 4 is clear."""
    import numpy as np
    w = 8 * len(text)
    img = np.zeros((16, w, 4), np.float32)
    shade = {1: 1.0, 2: 0.62, 3: 0.28}
    for i, ch in enumerate(text):
        g = font[max(0, min(95, ord(ch) - 0x20))]
        for v, k in shade.items():
            m = g == v
            img[:, 8 * i:8 * i + 8][m] = [color[0] * k, color[1] * k, color[2] * k, 255]
    return img.repeat(scale, 0).repeat(scale, 1)


def card_clip(name, tmp, tag):
    """A card (CARDS[name]) as a 1920x1080 clip: each screen fades in, its
    flying lines come in from the right with an ease-out, and it fades out."""
    import numpy as np
    font = np.load(os.path.join(V3, "font8x16.npy"))
    o = os.path.join(tmp, f"{tag}.mp4")
    p = subprocess.Popen(["ffmpeg", "-v", "error", "-y", "-f", "rawvideo", "-pix_fmt", "rgb24", "-s", "1920x1080",
                          "-r", str(FPS), "-i", "-", "-c:v", "libx264", "-crf", "14", "-preset", "fast",
                          "-pix_fmt", "yuv420p", o], stdin=subprocess.PIPE)
    for dur, lines in CARDS[name]:
        layers = [(text_layer(t, sc, col, font), y, fly) for t, sc, col, y, fly in lines]
        n = int(dur * FPS)
        for f in range(n):
            t = f / FPS
            frame = np.zeros((1080, 1920, 3), np.float32)
            for img, y, fly in layers:
                h, w = img.shape[:2]
                x = (1920 - w) // 2
                a = 1.0
                if fly is not None:
                    u = min(1.0, max(0.0, (t - fly) / 0.45))
                    if u <= 0:
                        continue
                    x += int((1920 - x) * (1 - u) ** 3)
                    a = min(1.0, u * 2)
                x0, x1 = max(0, x), min(1920, x + w)
                if x1 <= x0:
                    continue
                part = img[:, x0 - x:x1 - x]
                al = part[..., 3:4] / 255 * a
                frame[y:y + h, x0:x1] = frame[y:y + h, x0:x1] * (1 - al) + part[..., :3] * al
            k = min(1.0, t / 0.35, (dur - t) / 0.45)
            p.stdin.write((frame * max(0.0, k)).clip(0, 255).astype(np.uint8).tobytes())
    p.stdin.close()
    p.wait()
    return o


def game_clip(name, tmp, trims=None, tag=None, opt=None):
    """A Part 1 recording, cut and scaled; returns the clip's path. trims:
    see PART1 ("cuts": the sound is cut with the picture); sped-up parts
    keep the recording's sound running at its own speed."""
    tag = tag or name
    opt = opt or {}
    src = os.path.join(REC, f"{name}.mp4")
    s0 = scene_start(src)
    if isinstance(trims, tuple) and trims[0] == "abs":    # times from the recording's start (the crawl scenes)
        s0, trims = 0.0, trims[1]
    is_card = isinstance(trims, tuple) and trims[0] == "pages"
    if is_card:
        runs = pages(src)
        i, j = trims[1][0], trims[1][-1]
        s0, trims = 0.0, [(max(0.0, runs[i][0] - 0.3), runs[j][1] + 0.3, 1.0)]
    end = duration(src)
    if trims is None:
        end = min(end, content_end(src))
    fade_last = 0.0
    if trims is None and opt.get("fade_on_last_line"):
        # the game shifts the camera while a message is open: end on the last line, fading out over it
        e = last_line_end(src, max(s0, end - 8), end)
        if e:
            end, fade_last = e, 0.8
    cut_sound = trims == "cuts"
    parts = without_black(src, s0) if cut_sound else (trims or [(0, end - s0, 1.0)])
    if cut_sound and opt.get("lively"):
        parts = lively(src, parts, s0)
    if cut_sound and opt.get("fade_on_last_line"):
        e = last_line_end(src, s0 + parts[-1][0], s0 + parts[-1][1])
        if e:
            parts[-1] = (parts[-1][0], e - s0, 1.0)
            fade_last = 0.8
    outs = []
    for k, (a, b, speed) in enumerate(parts):
        a, b = s0 + a, min(end, s0 + b)
        o = os.path.join(tmp, f"{tag}_{k}.mp4")
        vf = f"setpts=(PTS-STARTPTS)/{speed},{CARD if is_card else GAME}"
        if name == "credits" and k == len(parts) - 1:     # the video's end: fade to black
            vf += f",fade=out:st={(b - a) / speed - 2.5:.3f}:d=2.5"
        if fade_last and k == len(parts) - 1:
            vf += f",fade=out:st={(b - a) / speed - fade_last:.3f}:d={fade_last}:color={opt.get('fade_color', 'black')}"
        sh("ffmpeg", "-v", "error", "-y", "-ss", f"{a:.3f}", "-to", f"{b:.3f}", "-i", src, "-vf", vf,
           *(["-af", "aresample=48000,afade=t=in:d=0.03" + (f",afade=t=out:st={(b - a) - fade_last:.3f}:d={fade_last}"
                                                             if fade_last and k == len(parts) - 1 else "")]
             if cut_sound else ["-an"]),
           "-c:v", "libx264", "-crf", "16", "-preset", "fast", *(["-c:a", "aac", "-ac", "2"] if cut_sound else []), o)
        outs.append(o)
    if cut_sound:
        o = os.path.join(tmp, f"{tag}.mp4")
        concat(outs, o)
        return o
    v = os.path.join(tmp, f"{tag}_v.mp4")
    concat(outs, v, audio=False)
    fo = 2.5 if name == "credits" else (fade_last or 0.25)
    # sound: the recording's own, unsped, from the clip's start, as long as the picture
    dv = duration(v)
    o = os.path.join(tmp, f"{tag}.mp4")
    sh("ffmpeg", "-v", "error", "-y", "-i", v, "-ss", f"{s0 + parts[0][0]:.3f}", "-i", src, "-map", "0:v", "-map", "1:a",
       "-af", f"aresample=48000,afade=t=in:d=0.08,afade=t=out:st={max(0, dv - fo):.3f}:d={fo},apad",
       "-t", f"{dv:.3f}", "-c:v", "copy", "-c:a", "aac", "-b:a", "192k", "-ac", "2", o)
    return o


def song_length(song):
    """Seconds of song `song` in rec/musicN.mp4 from the bed's start until it
    falls silent (the end roll's song ends; it doesn't loop)."""
    music = os.path.join(REC, f"music{song}.mp4")
    out = subprocess.run(["ffmpeg", "-i", music, "-af", "silencedetect=n=-55dB:d=2", "-f", "null", "-"],
                         capture_output=True, text=True).stderr
    ms = scene_start(music) + 0.5
    starts = [float(v) for v in re.findall(r"silence_start: ([0-9.]+)", out) if float(v) > ms + 5]
    return (starts[0] if starts else duration(music)) - ms


def white_in(nxt, tmp, src, at, hold=1.0, fade=2.0, lead=1.6):
    """A held white, then a slow fade from white into the first frame of
    `nxt`, with the sound of `src` (the scene that whited out) from `at`
    seconds on, fading away, and part 2's music (rec/music.mp4) rising under
    the last `lead` seconds (part 2 picks it up from there)."""
    first = os.path.join(tmp, "white_next.png")
    sh("ffmpeg", "-v", "error", "-y", "-i", nxt, "-frames:v", "1", first)
    d = hold + fade
    music = os.path.join(REC, "music.mp4")
    ms = scene_start(music) + 1
    o = os.path.join(tmp, "white_in.mp4")
    sh("ffmpeg", "-v", "error", "-y", "-loop", "1", "-t", f"{d}", "-i", first, "-ss", f"{at:.3f}", "-i", src,
       "-ss", f"{ms - lead:.3f}", "-i", music, "-map", "0:v",
       "-vf", f"fps={FPS},format=yuv420p,fade=in:st={hold}:d={fade}:color=white",
       "-filter_complex", f"[1:a]aresample=48000,afade=t=out:st={hold + 0.8:.2f}:d={d - hold - 0.8:.2f},apad[g];"
                         f"[2:a]aresample=48000,volume=0.85,afade=t=in:d={lead},adelay={int((d - lead) * 1000)}:all=1,apad[m];"
                         f"[g][m]amix=inputs=2:normalize=0:duration=first[a]",
       "-map", "[a]", "-t", f"{d}", "-c:v", "libx264", "-crf", "16", "-c:a", "aac", "-ac", "2", o)
    return o


def with_bed(clip, song, at, tmp, tag, only=False, fade_in=0.3, fade_out=0.5):
    """`clip` with song `song` (rec/musicN.mp4) from `at` seconds into it laid
    under (or, only=True, in place of) its sound (at < 0: so that the song
    ends with the clip). Returns (path, seconds used)."""
    music = os.path.join(REC, f"music{song}.mp4")
    ms = scene_start(music) + 0.5
    d = duration(clip)
    if at < 0:
        at = max(0.0, song_length(song) - d)
    o = os.path.join(tmp, f"{tag}_bed.mp4")
    fade = (f"afade=t=in:d={fade_in}," if fade_in else "") + (
        f"afade=t=out:st={max(0, d - fade_out):.3f}:d={fade_out}" if fade_out else "anull")
    if only:
        af = ["-map", "0:v", "-map", "1:a", "-af", f"aresample=48000,{fade},apad"]
    else:
        af = ["-filter_complex", f"[1:a]aresample=48000,{fade},apad[b];[0:a]aresample=48000[g];"
                                 f"[g][b]amix=inputs=2:normalize=0:duration=first[a]", "-map", "0:v", "-map", "[a]"]
    sh("ffmpeg", "-v", "error", "-y", "-i", clip, "-ss", f"{ms + at:.3f}", "-i", music, *af, "-t", f"{d:.3f}",
       "-c:v", "copy", "-c:a", "aac", "-b:a", "192k", "-ac", "2", o)
    return o, d


def card(tmp, name, big, small="", secs=2.5, white=False, music=None):
    """A title card (still), silent or with a few seconds of `music` (a recording)."""
    png = os.path.join(tmp, f"{name}.png")
    bg, fg, acc = ("#f6f3ec", "#1b1916", "#b5462f") if white else ("#0e0c0a", "#ece6dc", "#e0745a")
    cmd = ["magick", "-size", "1920x1080", f"xc:{bg}", "-gravity", "center", "-font", BOLD, "-pointsize", "110",
           "-fill", fg, "-annotate", "+0-30", big]
    if small:
        cmd += ["-font", FONT, "-pointsize", "40", "-fill", acc, "-annotate", "+0+80", small]
    sh(*cmd, png)
    o = os.path.join(tmp, f"{name}.mp4")
    audio = (["-ss", f"{scene_start(music) + 0.5:.3f}", "-i", music] if music else
             ["-f", "lavfi", "-t", str(secs), "-i", "anullsrc=r=48000:cl=stereo"])
    sh("ffmpeg", "-v", "error", "-y", "-loop", "1", "-t", str(secs), "-i", png, *audio, "-map", "0:v", "-map", "1:a",
       "-vf", f"fps={FPS},format=yuv420p,fade=in:0:6,fade=out:st={secs - 0.25}:d=0.25",
       "-af", f"aresample=48000,afade=t=in:d=0.3,afade=t=out:st={secs - 0.6:.2f}:d=0.6", "-t", str(secs),
       "-c:v", "libx264", "-crf", "16", "-c:a", "aac", "-ac", "2", o)
    return o


def concat(paths, out, audio=True):
    lst = out + ".txt"
    with open(lst, "w") as f:
        for p in paths:
            f.write(f"file '{p}'\n")
    args = ["ffmpeg", "-v", "error", "-y", "-f", "concat", "-safe", "0", "-i", lst]
    args += ["-c:v", "libx264", "-crf", "16", "-preset", "fast"] + (["-c:a", "aac", "-b:a", "192k", "-ar", "48000"] if audio
                                                                    else ["-an"])
    sh(*args, out)
    os.remove(lst)


def live_track(tmp, meta, total):
    """The game's side of the live edit as a 480x320 video lined up with the
    Studio's recording: reading the panel before the save, after it, then a
    second after each palette edit (the last frame held between)."""
    live = os.path.join(V3, "live")
    seq = []                # (png, seconds)

    def frames(prefix):
        return sorted(f for f in os.listdir(live) if f.startswith(prefix + "_") and f.endswith(".png"))

    def run(prefix, t0, t1):
        fs = frames(prefix)
        n = min(len(fs), int((t1 - t0) * 60))
        for f in fs[:n:2]:
            seq.append((os.path.join(live, f), 2 / 60))
        if n < (t1 - t0) * 60 and fs:
            seq.append((os.path.join(live, fs[n - 1]), (t1 - t0) - n / 60))
    marks = [meta["live_start"], meta["live_save"] + 0.3] + meta["live_pool"] + [total]
    run("a", marks[0], marks[1])
    run("c", marks[1], marks[2])
    for k in range(len(meta["live_pool"])):
        run(f"d{k + 1}", marks[2 + k], marks[3 + k])
    lst = os.path.join(tmp, "live.txt")
    with open(lst, "w") as f:
        for p, d in seq:
            f.write(f"file '{p}'\nduration {d:.4f}\n")
        f.write(f"file '{seq[-1][0]}'\n")
    o = os.path.join(tmp, "live_game.mp4")
    sh("ffmpeg", "-v", "error", "-y", "-f", "concat", "-safe", "0", "-i", lst,
       "-vf", f"scale=480:320:flags=neighbor,fps={FPS},format=yuv420p", "-c:v", "libx264", "-crf", "14", o)
    return o


def centaur_track(tmp, meta, total):
    """The game's side of the Centaur stop: live_tour.lua's frames (saved at
    the GBA's rate, in real time) from when the Studio linked, lined up with
    the Studio's recording, every second frame (30 fps)."""
    d = os.path.join(V3, "livetour")
    fs = sorted(f for f in os.listdir(d) if f.startswith("g_") and f.endswith(".png"))
    start = meta.get("game_start", 0.0)
    seq = [(os.path.join(d, fs[0]), max(0.04, start))]
    t = start
    for f in fs[::2]:
        if t >= total:
            break
        seq.append((os.path.join(d, f), 2 / 59.7275))
        t += 2 / 59.7275
    if t < total:
        seq.append((seq[-1][0], total - t))
    lst = os.path.join(tmp, "centaur.txt")
    with open(lst, "w") as f:
        for p, dd in seq:
            f.write(f"file '{p}'\nduration {dd:.4f}\n")
        f.write(f"file '{seq[-1][0]}'\n")
    o = os.path.join(tmp, "centaur_game.mp4")
    sh("ffmpeg", "-v", "error", "-y", "-f", "concat", "-safe", "0", "-i", lst,
       "-vf", f"scale=480:320:flags=neighbor,fps={FPS},format=yuv420p", "-c:v", "libx264", "-crf", "14", o)
    return o


def beside(tmp, studio, game, box, name):
    """The Studio's recording with the game laid into the box its page leaves for it."""
    x, y, w, h = box
    o = os.path.join(tmp, name)
    sh("ffmpeg", "-v", "error", "-y", "-i", studio, "-i", game, "-filter_complex",
       f"[1:v]pad={w + 8}:{h + 8}:4:4:color=0x3a352e[g];[0:v][g]overlay={x - 4}:{y - 4}:shortest=0,fps={FPS},format=yuv420p",
       "-t", f"{duration(studio):.3f}", "-c:v", "libx264", "-crf", "16", "-preset", "fast", o)
    return o


def part2(tmp, chapters, t0):
    """The Studio tour (its pages; Centaur and the live edit, each with the
    running game beside the Studio), Claude's "how I made this video"
    (making.py), and the closing cards, under a bed of song 57."""
    import tour_stops
    pages = os.path.join(REC, "studio_pages.mp4")
    cards = os.path.join(REC, "studio_cards.mp4")
    making = os.path.join(REC, "making.mp4")
    cen = os.path.join(REC, "studio_centaur.mp4")
    cmeta = json.load(open(os.path.join(REC, "studio_centaur.json")))
    cen2 = beside(tmp, cen, centaur_track(tmp, cmeta, duration(cen)), cmeta.get("game_box") or tour_stops.GAME_BOX,
                  "studio_centaur_game.mp4")
    live = os.path.join(REC, "studio_live.mp4")
    meta = json.load(open(os.path.join(REC, "studio_live.json")))
    live2 = beside(tmp, live, live_track(tmp, meta, duration(live)), meta.get("game_box") or tour_stops.GAME_BOX,
                   "studio_live_game.mp4")
    parts = [(pages, "How it was done: the Studio"), (cen2, "Centaur: the Studio, linked to a running game"),
             (live2, "Live edit: the Studio changes the running game"), (making, "How Claude made this video"),
             (cards, "The tools, and where to find them")]
    vids, t = [], t0
    for p, title in parts:
        o = os.path.join(tmp, os.path.basename(p).replace(".mp4", "_n.mp4"))
        sh("ffmpeg", "-v", "error", "-y", "-i", p, "-vf", f"scale=1920:1080,fps={FPS},format=yuv420p", "-an",
           "-c:v", "libx264", "-crf", "16", "-preset", "fast", o)
        vids.append(o)
        chapters.append((t, title))
        t += duration(o)
    v = os.path.join(tmp, "part2_v.mp4")
    concat(vids, v, audio=False)
    dv = duration(v)
    music = os.path.join(REC, "music.mp4")
    ms = scene_start(music)
    assert duration(music) - ms - 1 > dv, "rec/music.mp4 is shorter than part 2: record a longer bed"
    o = os.path.join(tmp, "part2.mp4")
    sh("ffmpeg", "-v", "error", "-y", "-i", v, "-ss", f"{ms + 1:.3f}", "-i", music, "-map", "0:v", "-map", "1:a",
       "-af", f"aresample=48000,volume=0.85,afade=t=in:d=0.05,afade=t=out:st={dv - 3:.3f}:d=3,apad", "-t", f"{dv:.3f}",
       "-c:v", "copy", "-c:a", "aac", "-b:a", "192k", "-ac", "2", o)
    return o


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", default=os.path.expanduser("~/Movies/boktai3-showcase/boktai3-showcase-v3.mp4"))
    ap.add_argument("--part", choices=["1", "2", "all"], default="all")
    a = ap.parse_args()
    import sys
    sys.path.insert(0, HERE)
    tmp = TMP
    os.makedirs(tmp, exist_ok=True)
    clips, chapters, t = [], [], 0.0
    beds = {}                  # song -> seconds of it used so far
    if a.part in ("1", "all"):
        for k, (name, chapter, trims, opt) in enumerate(PART1):
            if name == "part2":
                if a.part == "1":
                    continue
                if PART1[k - 1][3].get("white_out"):         # from the last scene's white into part 2
                    w = white_in(os.path.join(REC, "studio_pages.mp4"), tmp, os.path.join(REC, f"{PART1[k - 1][0]}.mp4"),
                                 white_from)
                    clips.append(w)
                    t += duration(w)
                c = part2(tmp, chapters, t)
                clips.append(c)
                t += duration(c)
                continue
            tag = f"{k:02d}_{name.replace(':', '_')}"
            c = card_clip(name[5:], tmp, tag) if name.startswith("card:") else game_clip(name, tmp, trims, tag, opt)
            if opt.get("bed"):
                song = opt["bed"]
                prev, nxt = (PART1[k - 1][3] if k else {}), (PART1[k + 1][3] if k + 1 < len(PART1) else {})
                joined_in, joined_out = prev.get("bed") == song, nxt.get("bed") == song     # one stretch of the song
                at = -1 if opt.get("bed_end") else beds.get(song, 0.0)
                c, used = with_bed(c, song, at, tmp, tag, only=opt.get("only_bed", False),
                                   fade_in=0 if joined_in else 0.3,
                                   fade_out=2.5 if name == "credits" else (0 if joined_out else 0.5))
                beds[song] = beds.get(song, 0.0) + used
            if opt.get("white_out"):     # where its recording goes on after the clip (for the white-in's sound)
                src = os.path.join(REC, f"{name}.mp4")
                white_from = scene_start(src) + duration(c)
            if chapter:
                chapters.append((t, chapter))
            clips.append(c)
            t += duration(c)
            print(f"{tag}: {duration(c):.1f} s (at {t:.1f})", flush=True)
    if a.part == "2":
        c = part2(tmp, chapters, t)
        clips.append(c)
        t += duration(c)
    out = a.out
    os.makedirs(os.path.dirname(out), exist_ok=True)
    body = os.path.join(tmp, "body.mp4")
    concat(clips, body)
    meta = os.path.join(tmp, "chapters.txt")
    with open(meta, "w") as f:
        f.write(";FFMETADATA1\ntitle=Boktai 3, decompiled: The Stranger\n")
        for k, (s_, title) in enumerate(chapters):
            e = chapters[k + 1][0] if k + 1 < len(chapters) else t
            f.write(f"[CHAPTER]\nTIMEBASE=1/1000\nSTART={int(s_ * 1000)}\nEND={int(e * 1000)}\ntitle={title}\n")
    # loudness to -16 LUFS (the game's mix is quiet), chapters from the metadata file
    sh("ffmpeg", "-v", "error", "-y", "-i", body, "-i", meta, "-map", "0:v", "-map", "0:a", "-map_metadata", "1",
       "-map_chapters", "1", "-c:v", "copy", "-af", "loudnorm=I=-16:TP=-1.5:LRA=11", "-ar", "48000", "-c:a", "aac",
       "-b:a", "192k", "-movflags", "+faststart", out)
    with open(out.replace(".mp4", ".chapters.txt"), "w") as f:
        for s_, title in chapters:
            f.write(f"{int(s_ // 60)}:{int(s_ % 60):02d} {title}\n")
    print(f"{out}: {duration(out):.1f} s")


if __name__ == "__main__":
    main()
