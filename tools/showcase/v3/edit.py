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
import shutil
import subprocess
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(os.path.dirname(HERE)))
V3 = os.path.join(ROOT, "build", "showcase", "v3")
REC = os.path.join(V3, "rec")
BG = "0x0e0c0a"
FPS = 30
FONT = "/System/Library/Fonts/Supplemental/Arial.ttf"
BOLD = "/System/Library/Fonts/Supplemental/Arial Bold.ttf"

# Part 1: (recording, chapter title or None, trims) in order. trims: list of
# (start, end, speed) in recording seconds; None = from the scene's start to the end.
PART1 = [
    ("cold", "Cold open: one ghoul, twenty palettes", None),
    ("title", None, None),
    ("doors", "The doors lie", "cuts"),
    ("night", "Night falls on cue", None),
    ("marionettes", "The marionettes", None),
    ("fight", "The boss who forgets", None),
    ("count", None, None),
    ("ride", "Somebody rides Vanargand", None),
    ("reveal", "The overload: who the stranger was", None),
    ("coffins", "Encore: a coffin for every boss", None),
    ("vride", None, None),
    ("sunback", "The sun, handed back", None),
    ("credits", "Credits", [(0, 32, 2.0), (32, 58, 1.0), (58, 74, 2.0), (74, 82, 1.0)]),
    ("finale", None, [(0, 5.2, 1.0)]),
]


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


def game_clip(name, tmp, trims=None):
    """A Part 1 recording, cut and scaled; returns the clip's path. trims:
    [(start, end, speed)] from the scene's start ("cuts": skip the dark
    stretches, the sound cut with the picture); sped-up parts keep the
    recording's sound running at its own speed (music under a fast crawl)."""
    src = os.path.join(REC, f"{name}.mp4")
    s0 = scene_start(src)
    end = duration(src)
    cut_sound = trims == "cuts"
    parts = without_black(src, s0) if cut_sound else (trims or [(0, end - s0, 1.0)])
    outs = []
    for k, (a, b, speed) in enumerate(parts):
        a, b = s0 + a, min(end, s0 + b)
        o = os.path.join(tmp, f"{name}_{k}.mp4")
        vf = f"setpts=(PTS-STARTPTS)/{speed},{GAME}"
        sh("ffmpeg", "-v", "error", "-y", "-ss", f"{a:.3f}", "-to", f"{b:.3f}", "-i", src, "-vf", vf,
           *(["-af", "aresample=48000,afade=t=in:d=0.03"] if cut_sound else ["-an"]),
           "-c:v", "libx264", "-crf", "16", "-preset", "fast", *(["-c:a", "aac", "-ac", "2"] if cut_sound else []), o)
        outs.append(o)
    if cut_sound:
        o = os.path.join(tmp, f"{name}.mp4")
        concat(outs, o)
        return o
    v = os.path.join(tmp, f"{name}_v.mp4")
    concat(outs, v, audio=False)
    # sound: the recording's own, unsped, from the scene's start, as long as the picture
    dv = duration(v)
    o = os.path.join(tmp, f"{name}.mp4")
    sh("ffmpeg", "-v", "error", "-y", "-i", v, "-ss", f"{s0:.3f}", "-i", src, "-map", "0:v", "-map", "1:a",
       "-af", f"aresample=48000,afade=t=in:d=0.08,afade=t=out:st={max(0, dv - 0.25):.3f}:d=0.25,apad",
       "-t", f"{dv:.3f}", "-c:v", "copy", "-c:a", "aac", "-b:a", "192k", "-ac", "2", o)
    return o


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


def part2(tmp, chapters, t0):
    pages = os.path.join(REC, "studio_pages.mp4")
    live = os.path.join(REC, "studio_live.mp4")
    cards = os.path.join(REC, "studio_cards.mp4")
    meta = json.load(open(os.path.join(REC, "studio_live.json")))
    dl = duration(live)
    game = live_track(tmp, meta, dl)
    import tour_stops
    x, y, w, h = meta.get("game_box") or tour_stops.GAME_BOX
    live2 = os.path.join(tmp, "studio_live_game.mp4")
    sh("ffmpeg", "-v", "error", "-y", "-i", live, "-i", game, "-filter_complex",
       f"[1:v]pad={w + 8}:{h + 8}:4:4:color=0x3a352e[g];[0:v][g]overlay={x - 4}:{y - 4}:shortest=0,fps={FPS},format=yuv420p",
       "-t", f"{dl:.3f}", "-c:v", "libx264", "-crf", "16", "-preset", "fast", live2)
    vids = []
    for p in (pages, live2, cards):
        o = os.path.join(tmp, os.path.basename(p).replace(".mp4", "_n.mp4"))
        sh("ffmpeg", "-v", "error", "-y", "-i", p, "-vf", f"scale=1920:1080,fps={FPS},format=yuv420p", "-an",
           "-c:v", "libx264", "-crf", "16", "-preset", "fast", o)
        vids.append(o)
    v = os.path.join(tmp, "part2_v.mp4")
    concat(vids, v, audio=False)
    dv = duration(v)
    music = os.path.join(REC, "music.mp4")
    ms = scene_start(music)
    o = os.path.join(tmp, "part2.mp4")
    sh("ffmpeg", "-v", "error", "-y", "-i", v, "-ss", f"{ms + 1:.3f}", "-i", music, "-map", "0:v", "-map", "1:a",
       "-af", f"aresample=48000,volume=0.55,afade=t=in:d=2,afade=t=out:st={dv - 3:.3f}:d=3,apad", "-t", f"{dv:.3f}",
       "-c:v", "copy", "-c:a", "aac", "-b:a", "192k", "-ac", "2", o)
    dp = duration(pages)
    chapters += [(t0, "Part 2: how it was done (the Studio)"), (t0 + dp, "Live edit: the Studio changes the running game"),
                 (t0 + dp + dl, "The tools, and thanks")]
    return o


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", default=os.path.expanduser("~/Movies/boktai3-showcase/boktai3-showcase-v3.mp4"))
    ap.add_argument("--part", choices=["1", "2", "all"], default="all")
    a = ap.parse_args()
    import sys
    sys.path.insert(0, HERE)
    tmp = tempfile.mkdtemp(prefix="v3edit-")
    clips, chapters, t = [], [], 0.0
    try:
        if a.part in ("1", "all"):
            for name, chapter, trims in PART1:
                if name == "title":
                    c = card(tmp, "title", "BOKTAI 3, DECOMPILED", secs=2.6, music=os.path.join(REC, "music.mp4"))
                else:
                    c = game_clip(name, tmp, trims)
                if chapter:
                    chapters.append((t, chapter))
                clips.append(c)
                t += duration(c)
                print(f"{name}: {duration(c):.1f} s (at {t:.1f})", flush=True)
        if a.part in ("2", "all"):
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
            for k, (s, title) in enumerate(chapters):
                e = chapters[k + 1][0] if k + 1 < len(chapters) else t
                f.write(f"[CHAPTER]\nTIMEBASE=1/1000\nSTART={int(s * 1000)}\nEND={int(e * 1000)}\ntitle={title}\n")
        # loudness to -16 LUFS (the game's mix is quiet), chapters from the metadata file
        sh("ffmpeg", "-v", "error", "-y", "-i", body, "-i", meta, "-map", "0:v", "-map", "0:a", "-map_metadata", "1",
           "-map_chapters", "1", "-c:v", "copy", "-af", "loudnorm=I=-16:TP=-1.5:LRA=11", "-ar", "48000", "-c:a", "aac",
           "-b:a", "192k", "-movflags", "+faststart", out)
        with open(out.replace(".mp4", ".chapters.txt"), "w") as f:
            for s, title in chapters:
                f.write(f"{int(s // 60)}:{int(s % 60):02d} {title}\n")
        print(f"{out}: {duration(out):.1f} s")
    finally:
        shutil.move(tmp, os.path.expanduser(f"~/.Trash/{os.path.basename(tmp)}"))


if __name__ == "__main__":
    main()
