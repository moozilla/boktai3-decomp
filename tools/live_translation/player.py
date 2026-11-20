"""Present the emulated cartridge and handle the in-context editing controls."""
import ctypes as C
import subprocess
import time

from runtime import ROOT, OUT

BOOT = [(300, "START"), (60, "A"), (150, "START"), (120, "A"), (120, "START"),
        (300, "START"), (120, "START"), (120, "A"), (150, "A")]


def entrance(game, ui, start):
    def advance(frames):
        while frames:
            if ui.lt_input() < 0:
                return False
            n = min(frames, 30)
            game.step(n)
            ui.lt_present(game.core.lt_pixels())
            frames -= n
        return True

    for index, (delay, key) in enumerate(BOOT):
        if not advance(delay):
            return False
        if index == 6 and start == "title":
            return True
        game.tap(key)
    for _ in range(50):
        if not advance(150):
            return False
        if game.resolver.dialogue_id == 5599:
            return advance(150)
        game.tap("A")
    raise RuntimeError("The opening checkpoint did not reach Otenko's first question")


def play(game, *, start="title", limit=None):
    subprocess.run(["make", "-s", "-f", "tools/live_translation/host.mk", "build/live_translation/player.so"],
                   cwd=ROOT, check=True)
    ui = C.CDLL(str(OUT / "player.so"))
    ui.lt_window_error.restype = C.c_char_p
    if not ui.lt_window():
        raise RuntimeError("SDL video: " + ui.lt_window_error().decode(errors="replace"))
    audio = (C.c_int16 * 8192)()
    game.resolver.bookmarks = True
    try:
        print("Arrows move. X confirms, Z cancels, Enter opens Start. A/S are L/R.\n"
              "F1 replays the current line in Japanese or English. F2 edits it. Space pauses. Esc exits.", flush=True)
        if start != "boot" and not entrance(game, ui, start):
            return
        game.core.lt_sound(1)
        deadline = time.monotonic()
        count = 0
        last_title = None
        while limit is None or count < limit:
            keys = ui.lt_input()
            if keys < 0:
                return
            line = game.resolver.current_line(game)
            if line and keys & 2048:
                game.resolver.source_preview = None if game.resolver.source_preview is not None else line["id"]
                game.rewind_line()
            if line and keys & 4096:
                from editor import edit
                if not edit(ui, game):
                    return
                deadline = time.monotonic()
            title = "Boktai 3   |   Live English"
            if line:
                title += f'   |   {line["key"]}   ·   F1 compare   ·   F2 edit'
            if title != last_title:
                ui.lt_title(title.encode())
                last_title = title
            if keys & 1024 and not keys & (2048 | 4096):
                time.sleep(.025)
                deadline = time.monotonic()
                continue
            game.step(1, keys & 1023)
            ui.lt_present(game.core.lt_pixels())
            n = game.core.lt_take_audio(audio)
            if n:
                ui.lt_audio(audio, n, game.core.lt_rate())
            count += 1
            deadline = max(deadline + 1/59.7275, time.monotonic()-.04)
            time.sleep(max(0, deadline-time.monotonic()))
    finally:
        game.core.lt_sound(0)
        ui.lt_quit_window()
