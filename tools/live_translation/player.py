import ctypes as C
import os
import subprocess
import time
from runtime import ROOT, OUT


def play(game, *, quick=True, limit=None):
    subprocess.run(["make","-s","-f","tools/live_translation/host.mk","build/live_translation/player.so"],
                   cwd=ROOT,check=True)
    ui=C.CDLL(str(OUT/"player.so"))
    ui.lt_window_error.restype=C.c_char_p
    if not ui.lt_window():
        raise RuntimeError("SDL video: "+ui.lt_window_error().decode(errors="replace"))
    audio=(C.c_int16 * 8192)()
    try:
        print("Arrows move. X = A, Z = B, Enter = Start, Backspace = Select, A/S = L/R. Space pauses; Esc exits.",flush=True)
        if quick:
            # Run the real startup and title entrance before giving control.
            # Poll events and present between batches so the window stays responsive.
            for frames,keys in [(300,0),(2,8),(68,0),(2,1),(158,0),(2,8),(128,0),
                                (2,1),(128,0),(2,8),(188,0)]:
                while frames:
                    if ui.lt_input()<0: return
                    n=min(frames,20)
                    game.step(n,keys)
                    ui.lt_present(game.core.lt_pixels())
                    frames-=n
        game.core.lt_sound(1)
        deadline=time.monotonic()
        count=0
        while limit is None or count<limit:
            keys=ui.lt_input()
            if keys<0: return
            if keys & 1024:
                time.sleep(.025);deadline=time.monotonic();continue
            game.step(1,keys)
            ui.lt_present(game.core.lt_pixels())
            n=game.core.lt_take_audio(audio)
            if n: ui.lt_audio(audio,n,game.core.lt_rate())
            count+=1
            deadline=max(deadline+1/59.7275,time.monotonic()-.04)
            time.sleep(max(0,deadline-time.monotonic()))
    finally:
        game.core.lt_sound(0)
        ui.lt_quit_window()
