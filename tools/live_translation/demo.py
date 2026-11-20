"""Record the cartridge localization film with mGBA and ffmpeg. Use --stills for proof frames."""
import argparse
import ctypes as C
from contextlib import closing
import io
import wave
from pathlib import Path
import shutil
import subprocess
import time

from PIL import Image, ImageDraw
from editor import face
from runtime import ROOT, OUT, KEYS, checkpoint, prison_save, saves
from player import BOOT
from proof import session
from translator import Memory

BUILD = ROOT / "build"
WORK = OUT / "film"
PROOF = OUT / "proof"
FFMPEG = "/opt/homebrew/bin/ffmpeg" if Path("/opt/homebrew/bin/ffmpeg").is_file() else shutil.which("ffmpeg")
SCALE = 5
SCREEN = (240 * SCALE, 160 * SCALE)
WIDTH = SCREEN[0] * 2
HEADER = 128
PAPER = "#182329"
MINT = "#9af1cf"
WHITE = "#f4f5ec"
SHOTS = [
    ("title", "01-title", "The game typesets its own English logo.", 4),
    ("file", "02-file", "The file menu reads your save in English.", 3),
    ("awake", "04-awake", "Otenko calls the player by name.", 5),
    ("config", "06-config", "Change the text speed in the original settings menu.", 3),
    ("items", "07-items", "Choco Banana’s description scrolls in English.", 5),
    ("card", "11-location", "The place name and sunrise clock change language together.", 4),
    ("before", "05-blood", "Otenko’s line, before the edit.", 4),
    ("compare", "05-blood", "F1   Read Otenko’s original Japanese.", 3),
    ("edit", "05-blood", "F2   Give Otenko a new line.", 4),
    ("after", "05-blood", "Enter   The cartridge types your correction.", 5),
]


def stop():
    if (BUILD / "STOP").exists():
        raise SystemExit("Stopped")


def encode(args):
    stop()
    with subprocess.Popen([FFMPEG, "-hide_banner", "-loglevel", "error", "-y", *args]) as proc:
        while proc.poll() is None:
            if (BUILD / "STOP").exists():
                proc.kill()
                raise SystemExit("Stopped")
            time.sleep(.12)
        if proc.returncode:
            raise RuntimeError(f"ffmpeg exited with status {proc.returncode}")


def header(caption, right="Live English"):
    im = Image.new("RGB", (WIDTH, HEADER), PAPER)
    d = ImageDraw.Draw(im)
    d.text((24, 11), "BOKTAI 3   /   LIVE TRANSLATION", font=face(22, bold=True), fill=MINT)
    d.text((WIDTH-24, 11), "Offline translation memory", anchor="rt", font=face(22), fill=MINT)
    d.text((24, 41), caption, font=face(36, bold=True), fill=WHITE)
    d.text((24, 96), "Japanese cartridge", font=face(23), fill=WHITE)
    d.text((SCREEN[0]+24, 96), right, font=face(23), fill=MINT)
    return im


def still(shot, key, caption):
    reference = PROOF / "japanese" / (key + ".png")
    english = PROOF / "english" / (key + ".png")
    folder = PROOF / "english/correction"
    if shot in ("compare", "edit", "after", "before"):
        reference = folder / "02-japanese.png"
        english = folder / {"compare": "02-japanese.png", "edit": "04-editor.png",
                            "after": "03-corrected.png", "before": "01-english.png"}[shot]
    right = {"compare": "F1  /  Japanese comparison", "edit": "F2  /  Translation editor"}.get(shot, "Live English")
    im = Image.new("RGB", (WIDTH, HEADER+SCREEN[1]), PAPER)
    im.paste(header(caption, right), (0,0))
    for i, path in enumerate([reference, english]):
        with Image.open(path) as frame:
            if frame.size == (240,160):
                frame = frame.resize(SCREEN, Image.Resampling.NEAREST)
            im.paste(frame, (i*SCREEN[0]+(SCREEN[0]-frame.width)//2, HEADER+(SCREEN[1]-frame.height)//2))
    return im


def first_cut():
    WORK.mkdir(parents=True, exist_ok=True)
    sequence = []
    for shot, key, caption, seconds in SHOTS:
        stop()
        im = still(shot, key, caption)
        path = WORK / (shot + ".png")
        im.save(path)
        if shot == "title":
            im.save(WORK / "poster.png")
            (WORK / "poster.png").replace(BUILD / "demo-poster.png")
        sequence.extend([f"file '{path.name}'", f"duration {seconds}"])
    sequence.append(f"file '{path.name}'")
    playlist = WORK / "stills.ffconcat"
    playlist.write_text("\n".join(sequence)+"\n")
    temp = WORK / "stills.mp4"
    encode(["-f", "concat", "-safe", "0", "-i", str(playlist), "-vf", "fps=24", "-c:v", "libx264",
            "-preset", "ultrafast", "-crf", "18", "-pix_fmt", "yuv420p", "-movflags", "+faststart", str(temp)])
    stop()
    temp.replace(BUILD / "demo.mp4")


FPS = 30
LENGTH = {shot: seconds for shot, _, _, seconds in SHOTS}
REPLACEMENT = "That vampire blood kept you alive."


def take(game, language, shot, buttons=None):
    """One output frame for two consecutive GBA frames, with native stereo samples."""
    stop()
    audio = (C.c_int16 * 8192)()
    with (WORK / f"{language}-{shot}.rgb").open("wb") as frames, wave.open(str(WORK/f"{language}-{shot}.wav"), "wb") as sound:
        sound.setparams((2, 2, game.core.lt_rate() or 32768, 0, "NONE", ""))
        game.core.lt_sound(1)
        try:
            for frame in range(FPS*LENGTH[shot]):
                stop()
                button = buttons.get(frame) if buttons else None
                game.step(2, KEYS[button] if button else 0)
                frames.write(game.image().tobytes())
                count = game.core.lt_take_audio(audio)
                if count:
                    sound.writeframesraw(C.string_at(audio, count*4))
        finally:
            game.core.lt_sound(0)


def dialogue(game, ident):
    for _ in range(60):
        stop()
        game.step(150)
        if game.resolver.dialogue_id == ident:
            game.rewind_line()
            return
        game.tap("A")
    raise RuntimeError(f"Missing dialogue {ident}")


class EditorRecording:
    """Supply text events to the player's own editor and capture its actual redraws."""
    def __init__(self, dest):
        self.dest = dest
        self.position = 0

    def lt_begin_edit(self):
        return True

    def lt_end_edit(self):
        pass

    def lt_draw_edit(self, rgba):
        im = Image.frombytes("RGBA", (900,600), rgba).convert("RGB")
        # Keep the editor's context and enlarge its actual input field for video.
        panel = Image.new("RGB", SCREEN, "#f7f2e7")
        panel.paste(im.crop((0,0,900,380)), (150,35))
        field = im.crop((15,380,610,549))
        panel.paste(field.resize((1190,338), Image.Resampling.NEAREST), (5,430))
        count = 18 if self.position == 0 else 2
        if self.position == len(REPLACEMENT):
            count = FPS*LENGTH["edit"] - 18 - 2*(len(REPLACEMENT)-1)
        pixels = panel.tobytes()
        for _ in range(count):
            stop()
            self.dest.write(pixels)

    def lt_edit_event(self, buffer, capacity):
        if self.position == len(REPLACEMENT):
            return 9  # Enter
        buffer.value = REPLACEMENT[self.position].encode()
        self.position += 1
        return 1  # SDL text input


def opening(game, language):
    game.resolver.bookmarks = True
    for i, (frames, key) in enumerate(BOOT):
        stop()
        if i == 5:
            game.step(20)
            take(game, language, "title")
            game.step(frames-20-2*FPS*LENGTH["title"])
        elif i == 7:
            game.step(68)
            take(game, language, "file")
        else:
            game.step(frames)
        game.tap(key)

    dialogue(game, 5599)
    take(game, language, "awake")
    game.tap("A")
    dialogue(game, 5601)
    take(game, language, "before")
    if language == "en":
        game.resolver.compare(game, game.resolver.current_choice(game))
    else:
        game.rewind_line()
    take(game, language, "compare")
    if language == "en":
        from editor import edit
        with (WORK/"en-edit.rgb").open("wb") as stream:
            if not edit(EditorRecording(stream), game):
                raise RuntimeError("The editor closed without replay")
        if game.resolver.memory.db.execute("SELECT english FROM entries WHERE key='text/5601'").fetchone()[0] != REPLACEMENT:
            raise RuntimeError("The correction was not saved")
    else:
        raw = game.image().tobytes()
        with (WORK/"ja-edit.rgb").open("wb") as stream:
            for _ in range(FPS*LENGTH["edit"]):
                stream.write(raw)
        game.rewind_line()
    take(game, language, "after")


def menus(game, language):
    for line in (ROOT/"tests/save_menus_field.txt").read_text().splitlines():
        stop()
        args = line.partition("#")[0].split()
        if not args:
            continue
        if args[0] == "tap":
            game.tap(args[1])
        elif args[0] == "wait":
            game.step(int(args[1]))
        elif args == ["shot", "06_config"]:
            take(game, language, "config", {10: "A", 27: "DOWN", 43: "RIGHT", 74: "B"})
        elif args == ["shot", "10_items"]:
            game.tap("A")
            take(game, language, "items")
            return
    raise RuntimeError("Inventory route did not reach items")


def card(game, language):
    for frames, key in BOOT[:-1]:
        stop()
        game.step(frames)
        game.tap(key)
    game.step(122)
    take(game, language, "card")


def record():
    for language in ("ja", "en"):
        for route, save in ((opening, prison_save()), (menus, saves.TEMPLATE), (card, checkpoint(9,2961,32,2))):
            stop()
            print("Recording", language, route.__name__, flush=True)
            memory = Memory(":memory:")
            with closing(memory.db), session(memory, save, language == "en", io.StringIO()) as game:
                route(game, language)


def raw(path, size):
    return ["-f", "rawvideo", "-pixel_format", "rgb24", "-video_size", size, "-framerate", str(FPS), "-i", str(path)]


def finish():
    clips = []
    for shot, _, caption, seconds in SHOTS:
        stop()
        right = {"compare": "F1  /  Japanese comparison", "edit": "F2  /  Translation editor"}.get(shot, "Live English")
        name = WORK / "header.png"
        header(caption,right).save(name)
        args = raw(WORK/f"ja-{shot}.rgb", "240x160")
        args += raw(WORK/f"en-{shot}.rgb", "1200x800" if shot=="edit" else "240x160")
        args += ["-i", str(name)]
        if shot == "edit":
            args += ["-f","lavfi","-i","anullsrc=r=48000:cl=stereo"]
            body = "[0:v]scale=1200:800:flags=neighbor[left];[left][1:v]hstack"
        else:
            args += ["-i",str(WORK/f"en-{shot}.wav")]
            body = "[0:v][1:v]hstack,scale=2400:800:flags=neighbor"
        graph = body + ",pad=2400:928:0:128[body];[body][2:v]overlay=0:0:format=rgb[v]"
        out = WORK / (shot+".mp4")
        encode([*args, "-filter_complex_threads", "2", "-filter_complex", graph, "-map", "[v]", "-map", "3:a",
                "-af", f"aresample=48000,apad,afade=t=in:d=0.035,afade=t=out:st={seconds-.06}:d=0.06",
                "-t", str(seconds), "-c:a", "aac", "-b:a", "160k", "-c:v", "libx264", "-threads", "5",
                "-preset", "veryfast", "-crf", "17", "-pix_fmt", "yuv420p", str(out)])
        clips.append(out)
    playlist = WORK / "live.ffconcat"
    playlist.write_text("\n".join(f"file '{clip.name}'" for clip in clips)+"\n")
    dest = WORK / "live.mp4"
    encode(["-f","concat","-safe","0","-i",str(playlist),"-c","copy","-movflags","+faststart",str(dest)])
    stop()
    poster = WORK / "poster.png"
    encode(["-ss", "2", "-i", str(dest), "-frames:v", "1", "-update", "1", str(poster)])
    stop()
    dest.replace(BUILD / "demo.mp4")
    poster.replace(BUILD / "demo-poster.png")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--stills", action="store_true", help="Generate only the quick first cut")
    parser.add_argument("--assemble", action="store_true", help="Reuse previously recorded raw film clips")
    args = parser.parse_args()
    stop()
    WORK.mkdir(parents=True, exist_ok=True)
    if args.stills:
        first_cut()
    else:
        if not args.assemble:
            if not (BUILD/"demo.mp4").exists() and (PROOF/"contact-sheet.png").exists():
                first_cut()
            record()
        finish()
