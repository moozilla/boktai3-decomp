#!/usr/bin/env python3
"""Build the showcase ROM: a new scene and cutscene about the decompilation.

  python3 tools/showcase/showcase.py           -> build/showcase/showcase.gba
  SCENARIO=tests/scenarios/showcase.txt tools/mgba/headless.sh \\
      tools/mgba/scenario.lua $PWD/build/showcase/showcase.gba
  tools/showcase/video.sh                      -> build/showcase/showcase.mp4

Everything in the scene is new data played by the game's own engine, on the
fan translation (for its English font):

* script procedures in unused slots, written with tools/showcase/gclasm.py:
  the scene entry, its entrance, the dialogue (the game's own counter idiom),
  a ghoul spawner;
* a cutscene table (docs/CUTSCENES.md) in event slot SLOT, moving the camera,
  the screen, the sound, Django, Otenko, Vanargand and the ghouls;
* dialogue strings, on string ids borrowed from the ending;
* the prologue's dream stage, with patrol paths added to its terrain so the
  ghouls stand where the cutscene wants them.
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
from gclasm import Arg, Call, Hash, Proc, Set, Text, chara, command, proc, r700, switch  # noqa: E402
from rom import Rom, add_paths, message  # noqa: E402

OUT = os.path.join(ROOT, "build", "showcase", "showcase.gba")
DREAM_HP = 0xDA9F            # the dream stage's terrain (proc_6535 loads it)
BOK = 0x05A5                 # the ghoul family's registry key
SLOT = 136                   # cutscene table slot (an ending cutscene)
STRINGS = 8140               # first borrowed string id (the ending's dialogue)
COUNTER = r700(0x119)        # the cutscene dialogue counter every scene uses

# receivers (docs/CUTSCENES.md)
DIRECTOR, CAMERA, SCREEN, AUDIO = 0x4E69, 0xFF40, 0x730A, 0xD3A9
PLAYER, OTENKO, VANARGAND = 0x2B06, 0x9CFE, 0x7C56
# ids naming the scripts the cutscene runs (any distinct hashes)
K_DLG, K_MGR, K_OTENKO, K_GHOUL = 0x5C0A, 0x9786, 0x4ACC, 0x5C0B

PX, PZ = 1440, 1440          # where Django stands
OX, OZ = 1600, 1300          # Otenko
ROW = [(1200 + 150 * (i - 2), 1200 - 150 * (i - 2)) for i in range(5)]  # the ghoul line

OT = "<LABEL=OTENKO>Master Otenko</LABEL>:\n"
DJ = "<LABEL=PLAYER>Django</LABEL>:\n"   # the save's own name may be Japanese

# the messages, in order; each is a list of pages (a label line, two short lines)
LINES = [
    [OT + '"Django! Welcome to a\nscene that isn\'t in the',
     'game. Konami didn\'t write\nit: it\'s a fan-made hack."'],
    [DJ + '"A hack? Made how?"',
     OT + '"By the Boktai 3 decomp:\nfans rebuilding the source',
     'code byte for byte. 31% of\nit is matching C now."'],
    [OT + '"Watch. Every ghoul in the\ngame shares one sprite..."'],
    [OT + '"...and one script option\npicks its palette."'],
    [OT + '"Ghoul, Zombie, Ghoul+...\nsame bones, new colors.',
     'The decomp traced every\nsprite\'s palettes."'],
    [OT + '"Cutscenes are tables of\nsteps. Camera, screen,',
     'sound and every actor take\ntheir orders from them."'],
    [OT + '"240 of them, 3,651 steps,\nall decoded.',
     'This one is number 241!"'],
    [DJ + '"Then even that thing\nfollows a script?"'],
    [OT + '"Vanargand is drawn as a\nbackground layer, and it',
     'takes direction like\nanyone else!"'],
    [OT + '"The scripts are GCL, the\nlanguage Konami made for',
     'Metal Gear Solid. All of\nits 5,615 procedures are',
     'read, and now we write\nnew ones."'],
    [OT + '"496 scenes, the quest as\n173 story beats, and 83',
     'checkpoints a test can\nboot straight into."'],
    [DJ + '"So what\'s left?"',
     OT + '"Most of the code. One\nfunction at a time.',
     'The Sun is on our side!"'],
]


def bok_spawner():
    """arg1 name, arg2 path, arg3 palette: a ghoul (Bok) standing on a patrol
    path; C picks the palette (pool 402 + C: Ghoul, Zombie, Ghoul+...). n=0
    keeps it from moving, so it never closes in."""
    return proc([chara(BOK, Arg(1), N=Arg(1), r=[Arg(2), 0, 0], e=[512, 16], a=[10, 12, 2, 30, 15, 64, 2, 50],
                       P=[0, 0, 0], A=[100, 0, 0, 2, 1026, 1], l=[1, 200, 600], n=[0, 0], C=Arg(3),
                       I=Proc(1133), G=[0x5A0001, 540, 0], X=[20, 0], K=4, V=1, H=0)])


MSG_IDS = []


def dialogue(self_id, starts):
    """The scene's dialogue procedure, as the game writes them: a counter
    match where case 0 starts the cutscene, then each message shows (show,
    then message_then itself) and closes (close the window, send the signal)."""
    cases = [(0, [Set(COUNTER, COUNTER, 1, "+"), starts])]
    for i, first in enumerate(MSG_IDS):
        cases.append((1 + 2 * i, [Set(COUNTER, COUNTER, 1, "+"), command(0x1C64, r=Text(first)),
                                  Call(1022, Proc(self_id))]))
        last = i == len(MSG_IDS) - 1
        cases.append((2 + 2 * i, [Set(COUNTER, 0) if last else Set(COUNTER, COUNTER, 1, "+"),
                                  Call(1025), command(0x2502)]))
    return proc([switch(COUNTER, cases)])


# ---- the cutscene: steps of messages -------------------------------------
def run(key, *args, ends=False):
    return message(DIRECTOR, 1, 0, [key] + (list(args) or [0]), ends)


def wait(n):
    return message(DIRECTOR, 1, 1, [n], True)


SIGNAL = message(DIRECTOR, 1, 2, [])


def say():
    """Show the next message; the step ends when the player closes it (its
    message_then runs the dialogue's close case, which sends the signal)."""
    return [[SIGNAL, run(K_DLG)]]


def cam(x, z, frames, ends=False):
    return message(CAMERA, 6, 2, [x, 406, z, frames], ends)


def fade(out, r, g, b):
    return message(SCREEN, 7, 3, [out, 7, r, g, b, 1, 1, 1, 0, 0])


def quake(strength, frames):
    return message(SCREEN, 7, 6, [strength, frames])


def bgm(song):
    return message(AUDIO, 8, 2, [song, 0])


def sound(n):
    return message(AUDIO, 8, 0, [n, 0])


def bgm_out(speed):
    return message(AUDIO, 8, 4, [speed, 0])


def player(cmd, *args):
    return message(PLAYER, 2, cmd, list(args))


def otenko(cmd, *args):
    return message(OTENKO, 5, cmd, list(args))


def vanargand(cmd, ends=False):
    return message(VANARGAND, 4, cmd, [], ends)


FIRST = 0


def steps():
    s = []
    s += [[run(K_MGR), run(K_OTENKO, OX, OZ, 3, 1), bgm(26), cam(PX, PZ, 0), wait(1)]]
    s += [[fade(0, 0, 0, 0), wait(60)]]
    s += [[player(3, 1), otenko(3, 5), wait(30)]]
    s += say()                                         # welcome
    s += [[otenko(3, 5), player(3, 1), wait(20)]]
    s += say()                                         # the decomp
    s += [[cam(ROW[2][0] + 120, ROW[2][1] + 120, 40, True)]]
    s += say()                                         # one sprite...
    s += [[run(K_GHOUL, 0x7103, FIRST + 2, 0), sound(1425), quake(30, 10), wait(50)]]
    s += say()                                         # ...one option picks the palette
    for i, c in ((1, 1), (3, 2), (0, 3), (4, 4)):
        s += [[run(K_GHOUL, 0x7101 + i, FIRST + i, c), sound(1407), wait(24)]]
    s += [[player(7, ROW[2][0] + 180, 256, ROW[2][1] + 180, 0), wait(70)]]
    s += [[player(3, 7), wait(20)]]
    s += say()                                         # same bones, new colors
    s += [[player(7, PX, 256, PZ, 0), wait(70)], [player(3, 1), wait(10)]]
    s += [[cam(PX, PZ, 40, True)]]
    s += say()                                         # cutscenes are tables
    s += [[quake(90, 10), sound(641), wait(40)], [cam(PX - 300, PZ + 300, 30, True)],
          [cam(PX + 300, PZ - 300, 30, True)], [cam(PX, PZ, 30, True)]]
    s += say()                                         # 240 of them
    s += [[player(3, 0), otenko(3, 0), bgm_out(6), wait(30)]]
    s += say()                                         # even that thing?
    s += [[cam(1114, 1114, 40, True)]]
    s += [[message(AUDIO, 8, 6, [106, 0]), sound(1425), wait(1)]]
    s += [[vanargand(51), quake(90, 10), wait(60)], [vanargand(52, True), quake(90, 20)],
          [vanargand(56), wait(40)]]
    s += say()                                         # Vanargand is a background layer
    s += [[vanargand(62), quake(90, 10), wait(90)], [message(AUDIO, 8, 9, [8, 0]), cam(PX, PZ, 60, True)]]
    s += [[bgm(57), player(3, 1), otenko(3, 5), wait(30)]]
    s += say()                                         # GCL
    s += say()                                         # scenes, story, checkpoints
    s += say()                                         # what's left
    s += [[bgm_out(5), sound(641), fade(1, 31, 31, 31), wait(180)], [wait(1)]]
    return s


def build():
    global FIRST
    r = Rom()
    FIRST = add_paths(r, DREAM_HP, [(x, z, 160) for x, z in ROW])
    sid = STRINGS
    for pages in LINES:
        MSG_IDS.append(sid)
        for p in pages + ["<PROC=0>\n<END>"]:
            r.set_string(sid, p)
            sid += 1
    entry, ents, arrive, start, cont, dlg, ghoul, done = (r.new_proc_id() for _ in range(8))
    r.set_proc(entry, proc([Call(861, 56), Call(864, 0), Call(553), Call(1265), Call(1), Call(6535),
                            Call(ents), Call(905), Call(6557), Call(6558)]))
    r.set_proc(ents, proc([Call(916, PX, PZ, 4, 0, 128, 0, 58, Proc(arrive), Proc(start)), Call(956, Proc(cont))]))
    r.set_proc(arrive, proc([command(0x9769, p=[PX, 406, PZ]), command(0x061E, i=0, d=4)]))
    r.set_proc(start, proc([Set(COUNTER, 0), Call(1667, 1, 1, 0, 0), Call(dlg)]))
    r.set_proc(cont, proc([]))
    r.set_proc(done, proc([Call(1669)]))
    r.set_proc(ghoul, bok_spawner())
    starts = command(0x7348, d=SLOT, c=0, p=[Hash(K_DLG), Hash(K_MGR), Hash(K_OTENKO), Hash(K_GHOUL)],
                     P=[Proc(dlg), Proc(1675), Proc(1676), Proc(ghoul)], e=Proc(done))
    r.set_proc(dlg, dialogue(dlg, starts))
    r.set_event(SLOT, steps())
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    r.save(OUT)
    return {"scene": entry, "entrance": 58, "strings": (STRINGS, sid - 1), "dlg": dlg}


if __name__ == "__main__":
    info = build()
    print(f"{OUT}: scene {info['scene']} entrance {info['entrance']}, strings {info['strings']}, dialogue proc {info['dlg']}")
