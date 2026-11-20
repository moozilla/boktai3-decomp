"""Edit native cartridge replays into a captioned, integer-scaled demo."""
from dataclasses import dataclass
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

import numpy as np
from PIL import Image, ImageDraw, ImageFont

from verify import playback

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / "build"
ASSETS = BUILD / "demake"
FPS = 30
WIDTH = 960
BAND = 96


@dataclass(frozen=True)
class Shot:
    section: str
    caption: str
    still: str
    seconds: float
    replay: str = ""
    start: int = 0
    end: int = 0


SHOTS = [
    Shot("BOKTAI 3", "On the original Game Boy", "title", 2.8),
    Shot("SEALED PRISON", "Django leaves the opening cell.", "opening", 2.5, "prison", 3, 114),
    Shot("SOLAR GUN", "A solar shot hits the ghoul.", "shot", 2.6, "campaign", 19, 86),
    Shot("FIELD MAP", "The arrow marks Django’s position.", "floor-map", 2.3),
    Shot("FIELD NOTES", "Otenko’s message in English", "notes", 2.2),
    Shot("FIELD NOTES", "The same line in the original Japanese", "notes-jp", 2.4),
    Shot("SUN METER", "Match a thumb’s shadow on the LCD.", "sun-meter", 2.8),
    Shot("SEALED PRISON", "A Solar Fruit restores gun energy.", "fruit", 2.5, "campaign", 202, 323),
    Shot("THE CORRIDOR", "Through the door and up the stairs", "corridor", 3.0, "campaign", 343, 505),
    Shot("SKYLIGHT CHAMBER", "Two skeletons patrol the room.", "skylight-room", 2.8, "campaign", 507, 620),
    Shot("SUNLIGHT", "Hold B beneath the skylight to charge.", "skylight", 2.8, "campaign", 621, 667),
    Shot("SKELETONS", "A skeleton lunges at Django.", "lunge", 2.0, "campaign", 668, 720),
    Shot("SEALED PRISON", "Deeper into the prison", "prison-2724", 2.4, "prison", 610, 738),
    Shot("BATTERY SAVE", "Save a checkpoint from the floor plan.", "floor-map", 2.2, "checkpoint", 241, 241),
    Shot("BATTERY SAVE", "The cartridge confirms the save.", "continue", 2.0, "checkpoint", 242, 242),
    Shot("COLD BOOT", "Start resumes the saved game.", "continue", 2.2, "checkpoint", 245, 245),
    Shot("CHECKPOINT RESTORED", "Django continues from the saved corridor.", "restored", 2.6, "checkpoint", 247, 343),
]


def stop_check():
    if (BUILD / "STOP").exists():
        raise SystemExit("Coordinator stop signal")


def font(name, size):
    roots = [Path("/System/Library/Fonts/Supplemental"), Path("/usr/share/fonts/truetype/dejavu")]
    fallback = "DejaVuSans-Bold.ttf" if "Bold" in name else "DejaVuSans.ttf"
    path = next((r / f for r in roots for f in (name, fallback) if (r / f).is_file()), None)
    return ImageFont.truetype(str(path), size) if path else ImageFont.load_default(size=size)


def caption(section, line):
    im = Image.new("RGB", (WIDTH, BAND), (26, 29, 28))
    d = ImageDraw.Draw(im)
    d.rectangle((29, 19, 33, 75), fill=(239, 195, 92))
    d.text((51, 12), section, font=font("Arial Bold.ttf", 18), fill=(239, 195, 92))
    d.text((49, 38), line, font=font("Arial Bold.ttf", 32), fill=(247, 246, 238))
    return im


def sampled_ticks(shot):
    count = round(shot.seconds * FPS)
    return [round(shot.start + (shot.end-shot.start)*i / max(1, count-1)) for i in range(count)]


def replay_lines(name):
    lines = (ASSETS / (name + ".replay")).read_text().splitlines()
    if name == "checkpoint":
        # The verified replay ends at the restored position. Continue walking
        # with ordinary direction input so the last shot demonstrates control.
        pc = next(line.split()[1] for line in lines if line.startswith("tick "))
        lines.extend([f"tick {pc} 32 16", f"tick {pc} 22 64", f"tick {pc} 42 16"])
    return lines


def capture(host, replay, ticks, work):
    """Insert screenshots at game-loop boundaries in the verified JOYP replay."""
    stop_check()
    script = []
    now = 0
    for line in replay_lines(replay):
        fields = line.split()
        if fields[0] == "power":
            script.append(line)
        if fields[0] != "tick":
            continue
        _, pc, length, keys = fields
        end = now + int(length)
        pos = now
        for tick in sorted(t for t in ticks if now < t <= end):
            script.extend([f"tick {pc} {tick-pos} {keys}", f"frame {replay}-{tick}.ppm"])
            pos = tick
        if end > pos:
            script.append(f"tick {pc} {end-pos} {keys}")
        now = end
        if now >= max(ticks):
            break
    subprocess.run([host, ASSETS / "boktai.gb"], cwd=work,
                   input="\n".join(script)+"\n", text=True, check=True)


def capture_pcm(host, work, lines, name, frames):
    stop_check()
    query = "\n".join([*lines, f"audio {frames} {name}.pcm", ""])
    result = subprocess.run([host, ASSETS / "boktai.gb"], cwd=work, input=query,
                            text=True, capture_output=True, check=True)
    _, rate, count = result.stdout.strip().split()
    data = np.fromfile(work / f"{name}.pcm", dtype="<i2").reshape(-1, 2)
    if len(data) != int(count):
        raise ValueError(f"incomplete cartridge audio for {name}")
    if not np.any(data):
        raise ValueError(f"silent cartridge audio for {name}")
    return int(rate), data


def effect(host, work, tick, channels, frames):
    """Reach the cue through JOYP, then capture its pulse or noise channel."""
    lines = []
    now = 0
    for line in replay_lines("campaign"):
        f = line.split()
        if f[0] != "tick":
            continue
        end = now + int(f[2])
        if tick <= end:
            if tick - 1 > now:
                lines.append(f"tick {f[1]} {tick-1-now} {f[3]}")
            lines.extend([f"channels {channels}", f"step 0 {f[3]}"])
            return capture_pcm(host, work, lines, f"effect-{tick}", frames)
        lines.append(line)
        now = end
    raise ValueError(f"cue outside replay at tick {tick}")


def score(host, work, seconds):
    rate, audio = capture_pcm(host, work, ["step 16 0", "step 4 8", "step 1 0"],
                              "music", int(60*(seconds+1)))
    mix = audio.astype(np.float32)
    # These are the actual campaign ticks at which the cartridge triggers
    # pulse 2 (shot, fruit, charge) or noise (ghoul hit).
    cues = [(29, 2, 11), (35, 8, 11), (322, 2, 11), (595, 2, 11),
            (630, 2, 7), (638, 2, 11)]
    video_frame = 0
    for shot in SHOTS:
        count = round(shot.seconds * FPS)
        if shot.replay == "campaign":
            for tick, mask, length in cues:
                if not shot.start <= tick <= shot.end:
                    continue
                cue_rate, pcm = effect(host, work, tick, mask, length)
                if cue_rate != rate:
                    raise ValueError("inconsistent APU sample rates")
                local = next(i for i,t in enumerate(sampled_ticks(shot)) if t >= tick)
                start = round((video_frame + local) * rate / FPS)
                fade = min(rate // 100, len(pcm)//2)
                env = np.ones((len(pcm),1), dtype=np.float32)
                env[:fade,0] = np.linspace(0,1,fade)
                env[-fade:,0] = np.linspace(1,0,fade)
                span = mix[start:start+len(pcm)]
                span *= 1 - .4*env
                span += .88 * env * pcm
        video_frame += count
    mix *= min(1, 30000 / max(1, float(np.abs(mix).max())))
    path = work / "mix.pcm"
    mix.astype("<i2").tofile(path)
    return ["-f", "s16le", "-ar", str(rate), "-ac", "2", "-i", str(path)]


def main():
    stop_check()
    ffmpeg = shutil.which("ffmpeg") or "/opt/homebrew/bin/ffmpeg"
    with tempfile.TemporaryDirectory(dir=BUILD, prefix="demo-edit-") as tmp:
        work = Path(tmp)
        counts = [round(s.seconds * FPS) for s in SHOTS]
        seconds = sum(counts)/FPS
        atlas = Image.new("RGB", (WIDTH, BAND * len(SHOTS)))
        for i,s in enumerate(SHOTS):
            atlas.paste(caption(s.section,s.caption), (0,BAND*i))
        atlas.save(work / "captions.png")
        poster = Image.new("RGB", (WIDTH, 864+BAND))
        with Image.open(ASSETS / "lunge.png") as im:
            poster.paste(im.resize((WIDTH,864),Image.Resampling.NEAREST), (0,0))
        poster.paste(caption("BOKTAI 3", "The Sealed Prison on Game Boy"), (0,864))
        poster.save(work / "poster.png")
        host = playback(work)
        if not host:
            raise RuntimeError("native libmgba playback is required for the demo")
        for replay in sorted({s.replay for s in SHOTS if s.replay}):
            ticks = {n for s in SHOTS if s.replay == replay for n in sampled_ticks(s)}
            capture(host, replay, ticks, work)
        stop_check()
        audio = score(host, work, seconds)
        expr = "+".join(f"gte(n,{sum(counts[:i])})" for i in range(1,len(SHOTS)))
        filters = (f"[0:v]scale={WIDTH}:864:flags=neighbor,setsar=1[game];"
                   f"[1:v]crop={WIDTH}:{BAND}:0:'{BAND}*({expr})'[cap];[game][cap]vstack[v];"
                   f"[2:a]aresample=48000,afade=t=out:st={seconds-.75:.3f}:d=0.75[a]")
        cmd = [ffmpeg,"-nostdin","-v","error","-y","-f","rawvideo","-pixel_format","gray","-video_size","160x144",
               "-framerate",str(FPS),"-i","pipe:0","-loop","1","-framerate",str(FPS),"-i",str(work/"captions.png"),
               *audio,"-filter_complex",filters,"-map","[v]","-map","[a]","-c:v","libx264","-preset","veryfast",
               "-crf","16","-pix_fmt","yuv420p","-c:a","aac","-b:a","160k","-t",f"{seconds:.5f}",
               "-movflags","+faststart",str(work/"demo.mp4")]
        proc = subprocess.Popen(cmd,stdin=subprocess.PIPE)
        try:
            for s,count in zip(SHOTS,counts):
                stop_check()
                if s.replay:
                    frames = [work / f"{s.replay}-{t}.ppm" for t in sampled_ticks(s)]
                else:
                    frames = [ASSETS / (s.still+".png")]*count
                previous = None
                for path in frames:
                    if path != previous:
                        with Image.open(path) as im:
                            raw = im.convert("L").tobytes()
                        previous = path
                    proc.stdin.write(raw)
            proc.stdin.close()
            if proc.wait():
                raise RuntimeError("ffmpeg encode failed")
        finally:
            if proc.poll() is None:
                proc.kill()
                proc.wait()
        stop_check()
        os.replace(work/"demo.mp4",BUILD/"demo.mp4")
        os.replace(work/"poster.png",BUILD/"demo-poster.png")
        print(f"{seconds:.1f} seconds, {sum(counts)} frames")


if __name__ == "__main__":
    main()
