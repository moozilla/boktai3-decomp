#!/usr/bin/env python3
"""Build the showcase ROM (v2): a new scene and cutscene about the decompilation.

  python3 tools/showcase/showcase.py           -> build/showcase/showcase.gba
  tools/showcase/video.sh                      -> build/showcase/showcase.mp4

Everything in the scene is new data played by the game's own engine, on the
fan translation (for its English font):

* script procedures in unused slots, written with tools/showcase/gclasm.py;
* a cutscene table (docs/CUTSCENES.md) in event slot SLOT;
* dialogue strings, on string ids borrowed from the ending;
* two new sprites in the sprite package archive (CB05): Clawd (clawd.py) and
  the Claudesona, vgel's Claude (vgel_claude.py), drawn by ScriptKeyframe and
  animated from the cutscene; they take their colors from palette pool 406,
  rewritten in Claude's colors (showcase_palette.py), which a ghoul loads
  first so the pool sits in a known OBJ palette slot;
* dialogue portraits for both (portraits.py), drawn over two speakers'
  portrait frames that the scene does not use;
* the prologue's dream stage with patrol paths added so ghouls stand where
  the cutscene wants them, and its keyframe manager made room for two sprites.

The first version is showcase_v1.py.
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
import clawd  # noqa: E402
import vgel_claude  # noqa: E402
from gclasm import Arg, Call, Expr, Hash, If, Proc, Set, Text, chara, command, proc, r700, switch  # noqa: E402
import portraits  # noqa: E402
from rom import Rom, add_paths, big_sprite_frame_pixels, message, package, replace_resource, set_pool  # noqa: E402
from showcase_palette import CLAUDE  # noqa: E402

OUT = os.path.join(ROOT, "build", "showcase", "showcase.gba")
DREAM_HP = 0xDA9F            # the dream stage's terrain (its stage procedure loads it)
DREAM_STAGE = 6535           # the dream stage procedure (copied, with room for two sprites)
BOK = 0x05A5                 # the ghoul family's registry key
POOL = 406                   # the palette pool rewritten in Claude's colors (C=4 ghouls)
# A sprite package's first header word is its palette pool entry (158A's 903
# is the pool its slot gets; checked against OBJ palette RAM): the new
# packages name pool 406, Claude's colors.
SLOT = 136                   # cutscene table slot (an ending cutscene)
STRINGS = 8140               # first borrowed string id (the ending's dialogue)
COUNTER = r700(0x119)        # the cutscene dialogue counter every scene uses
CLAWD_PKG, CLAUDE_PKG = 0x158A, 0xE349   # sprite packages replaced (the prologue's, unused here)
CLAWD_TAG, CLAUDE_TAG = 0x7A01, 0x7A02

# receivers (docs/CUTSCENES.md)
DIRECTOR, CAMERA, SCREEN, AUDIO = 0x4E69, 0xFF40, 0x730A, 0xD3A9
PLAYER, OTENKO, VANARGAND = 0x2B06, 0x9CFE, 0x7C56
# ids naming the scripts the cutscene runs (any distinct hashes)
K_DLG, K_MGR, K_OTENKO, K_GHOUL, K_CLAWD, K_CLAUDE, K_FRAME, K_MOVE = (0x5C0A, 0x9786, 0x4ACC, 0x5C0B, 0x5C0C,
                                                                 0x5C0D, 0x5C0E, 0x5C0F)

PX, PZ = 1440, 1440          # Django
OX, OZ = 1600, 1300          # Otenko
CLX, CLZ = 1660, 1020        # Clawd: up and to the right on screen
CDX, CDZ = 860, 1820         # Claude appears at the left edge and walks in
ROW = [(1200 + 150 * (i - 2), 1200 - 150 * (i - 2)) for i in range(5)]  # the ghoul line

OT = "<LABEL=OTENKO>Master Otenko</LABEL>:\n"
DJ = "<LABEL=PLAYER>Django</LABEL>:\n"
CW = "<LABEL=ROCKMAN>Clawd</LABEL>:\n"        # portraits drawn over two speakers (portraits.py)
CL = "<LABEL=GARIGARIKUN>Claude</LABEL>:\n"

# the messages, in order; each is a list of pages (a label line, up to two
# lines of 26 characters)
LINES = [
    [OT + '"Django! This scene isn\'t\nin the game...',
     'Something new is writing\nitself in!"'],
    [CW + '"Hi! I\'m Clawd, Claude\nCode\'s mascot!',
     'I drew myself into the\ngame\'s sprite archive!"'],
    [CL + '"And I\'m Claude. We\'re\ndecompiling Boktai 3:',
     '31% of it is byte-exact\nC so far."'],
    [DJ + '"Decompiling?"'],
    [CL + '"Turning the ROM back into\nsource we can read.',
     'Then we can make the game\ndo new things. Like..."'],
    [CL + '"One ghoul..."'],
    [CL + '"...one sprite, and a\nscript option picks',
     'its palette."'],
    [CW + '"Can I have one?"'],
    [CL + '"A palette we wrote:\nClaude colors!"'],
    [CL + '"Cutscenes are tables of\nsteps. The camera..."'],
    [CL + '"...the screen..."'],
    [CL + '"...and the sound test."'],
    [DJ + '"And that thing?"'],
    [CL + '"Vanargand is drawn as a\nbackground layer.',
     'It takes direction too!"'],
    [OT + '"240 cutscenes, 5,615\nscripts, 496 scenes...',
     '...and a fan with an AI,\nlearning every one."'],
    [CW + '"Want to see where we\ndo the work?"'],
    [OT + '"The Sun is on our side!"'],
]
MSG_IDS = []


def bok_spawner():
    """arg1 name, arg2 path, arg3 palette: a ghoul (Bok) standing on a patrol
    path; C picks the palette (pool 402 + C). n=0 keeps it still."""
    return proc([chara(BOK, Arg(1), N=Arg(1), r=[Arg(2), 0, 0], e=[512, 16], a=[10, 12, 2, 30, 15, 64, 2, 50],
                       P=[0, 0, 0], A=[100, 0, 0, 2, 1026, 1], l=[1, 200, 600], n=[0, 0], C=Arg(3),
                       I=Proc(1133), G=[0x5A0001, 540, 0], X=[20, 0], K=4, V=1, H=0)])


def sprite_in(tag, pkg, x, z):
    """ScriptKeyframe_Add (command EAB4): draw sprite package pkg at (x, z)."""
    return proc([command(0xEAB4, n=Hash(tag), d=Hash(pkg), i=0, m=0, p=[x, 256, z], R=2, A=0)])


def frame_setter():
    """arg1 0 Clawd / 1 Claude, arg2 frame: ScriptKeyframe_SetFrame (44CD)."""
    return proc([If(Expr(Arg(1), 0, "=="), [command(0x44CD, n=Hash(CLAWD_TAG), p=Arg(2))],
                    [command(0x44CD, n=Hash(CLAUDE_TAG), p=Arg(2))])])


def mover():
    """arg1 0 Clawd / 1 Claude, arg2 x, arg3 z, arg4 frames: glide there
    (ScriptKeyframe_SetTarget, 997B)."""
    def go(tag):
        return [command(0x997B, n=Hash(tag), p=[Arg(2), 256, Arg(3)], f=Arg(4))]
    return proc([If(Expr(Arg(1), 0, "=="), go(CLAWD_TAG), go(CLAUDE_TAG))])


def dialogue(self_id, starts):
    """The dialogue procedure as the game writes them: case 0 starts the
    cutscene; each message shows and message_thens itself, and the next case
    closes the window and sends the signal."""
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


def say(*extra):
    """Show the next message; the step ends when the player closes it."""
    return [[SIGNAL, run(K_DLG)] + list(extra)]


def cam(x, z, frames, ends=False):
    return message(CAMERA, 6, 2, [x, 406, z, frames], ends)


def fade(out, r, g, b, a=1, c=1, d=1, strength=0):
    return message(SCREEN, 7, 3, [out, 7, r, g, b, a, c, d, strength, 0])


def quake(strength, frames):
    return message(SCREEN, 7, 6, [strength, frames])


def sparkle(x, z, frames=40):
    return message(SCREEN, 7, 0, [x, 320, z, 128, frames, 24, 3, 0])


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


def move(who, x, z, frames):
    return run(K_MOVE, who, x, z, frames)


def clawd_frame(name):
    return run(K_FRAME, 0, clawd.ORDER.index(name))


def claude_frame(name):
    return run(K_FRAME, 1, vgel_claude.ORDER.index(name))


FIRST = 0


def steps():
    s = []
    # set up, fade in
    s += [[run(K_MGR), run(K_OTENKO, OX, OZ, 3, 1), bgm(26), cam(PX, PZ, 0), wait(1)]]
    s += [[fade(0, 0, 0, 0), wait(50)]]
    s += [[player(3, 1), otenko(3, 5), wait(20)]]
    s += say()                                                         # this scene isn't in the game
    s += [[sparkle(CLX, CLZ), sound(1407), wait(25)], [run(K_CLAWD), clawd_frame("cheer"), quake(20, 8), wait(20)],
          [clawd_frame("idle"), player(3, 2), wait(10)]]
    s += say(clawd_frame("wave"))                                      # Clawd
    s += [[clawd_frame("idle"), sparkle(CDX, CDZ), sound(1407), wait(25)],
          [run(K_CLAUDE), claude_frame("cheer"), quake(20, 8), wait(20)],
          [claude_frame("idle"), move(1, CDX + 160, CDZ - 160, 40), player(3, 0), wait(40)]]
    s += say(claude_frame("talk"))                                     # Claude: decompiling
    s += [[claude_frame("idle"), player(3, 1), wait(10)]]
    s += say()                                                         # Django: decompiling?
    s += say(claude_frame("talk"))                                     # turning the ROM back...
    s += [[claude_frame("idle"), cam(ROW[2][0] + 160, ROW[2][1] + 160, 30, True)]]
    s += say(claude_frame("talk"))                                     # one ghoul...
    s += [[claude_frame("idle"), run(K_GHOUL, 0x7103, FIRST + 2, 1), sound(1425), quake(30, 10), wait(40)]]
    s += say(claude_frame("talk"))                                     # ...one sprite, a palette option
    for i, c in ((1, 2), (3, 3), (0, 0)):
        s += [[claude_frame("idle"), run(K_GHOUL, 0x7101 + i, FIRST + i, c), sound(1407), wait(18)]]
    s += [[move(0, ROW[4][0] + 200, ROW[4][1] + 200, 36), clawd_frame("bob"), wait(36)]]
    s += say(clawd_frame("wave"))                                      # Clawd: can I have one?
    s += [[clawd_frame("cheer"), run(K_GHOUL, 0x7105, FIRST + 4, 4), sound(1425), quake(40, 10), wait(40)],
          [move(0, CLX, CLZ, 36), clawd_frame("bob"), wait(36)], [clawd_frame("idle"), wait(1)]]
    s += say(claude_frame("cheer"))                                    # Claude colors!
    s += [[clawd_frame("idle"), claude_frame("idle"), cam(PX, PZ, 30, True)]]
    s += say(claude_frame("talk"))                                     # the camera...
    s += [[claude_frame("idle"), cam(PX - 360, PZ + 360, 24, True)], [cam(PX + 360, PZ - 360, 24, True)],
          [quake(90, 10), sound(641), cam(PX, PZ, 24, True)]]
    s += say(claude_frame("talk"))                                     # the screen...
    s += [[claude_frame("idle"), fade(1, 31, 4, 4, 1, 0, 0, 64), wait(40)], [fade(1, 4, 8, 31, 1, 0, 0, 64), wait(40)],
          [fade(1, 31, 28, 4, 1, 0, 0, 64), wait(40)], [fade(0, 0, 0, 0), wait(30)]]
    s += say(claude_frame("talk"))                                     # the sound test
    for song in (18, 61, 76, 57):
        s += [[claude_frame("idle"), bgm(song), clawd_frame("bob"), wait(70)], [clawd_frame("idle"), wait(1)]]
    s += [[bgm_out(6), player(3, 0), otenko(3, 0), wait(20)]]
    s += say()                                                         # and that thing?
    s += [[cam(1114, 1114, 30, True)], [message(AUDIO, 8, 6, [106, 0]), sound(1425), wait(1)],
          [vanargand(51), quake(90, 10), wait(50)], [vanargand(52, True), quake(90, 20)], [vanargand(56), wait(30)]]
    s += say(claude_frame("talk"))                                     # Vanargand is a background layer
    s += [[claude_frame("idle"), vanargand(62), quake(90, 10), wait(70)],
          [message(AUDIO, 8, 9, [8, 0]), cam(PX, PZ, 40, True)], [bgm(57), player(3, 1), otenko(3, 5), wait(20)]]
    s += say()                                                         # 240 cutscenes...
    s += say(clawd_frame("wave"))                                      # where we do the work?
    s += [[clawd_frame("cheer"), claude_frame("cheer"), wait(10)]]
    s += say()                                                         # the Sun is on our side
    s += [[bgm_out(5), sound(641), fade(1, 31, 31, 31), wait(150)], [wait(1)]]
    return s


def stage_copy(r):
    """The dream stage procedure with its keyframe manager's capacity 1 -> 4."""
    import gcx
    o = r.img.proc_address(DREAM_STAGE - 1)
    st, n = gcx.length(bytes(r.rom), o)
    code = bytearray(r.rom[o:st + n])
    k = code.rfind(bytes.fromhex("06f68a06f68a526dc2"))   # chara 0x8af6 0x8af6 m: 1
    assert k > 0
    code[k + 8] = 0xC0 | (4 + 1)
    return bytes(code)


def build():
    global FIRST
    r = Rom()
    FIRST = add_paths(r, DREAM_HP, [(x, z, 160) for x, z in ROW])
    set_pool(r, POOL, CLAUDE)
    for spr, frame, art, pool, pal in ((0x593B, 14, portraits.claude_portrait(), 506, portraits.CLAUDE_PAL),
                                       (0x593B, 8, portraits.clawd_portrait(), 504, portraits.CLAWD_PAL)):
        a, ntiles = big_sprite_frame_pixels(r, spr, frame)
        assert ntiles == 16
        r.rom[a - 0x08000000:a - 0x08000000 + 512] = portraits.tiles(art)
        set_pool(r, pool, [c or 0x787888 for c in pal])
    for pkg, mod in ((CLAWD_PKG, clawd), (CLAUDE_PKG, vgel_claude)):
        imgs = [mod.FRAMES[k] for k in mod.ORDER]
        frames = [[(0x0C00, -32, -64, clawd.tile_of(i))] for i in range(len(imgs))]
        replace_resource(r, 0xCB05, pkg, package(frames, clawd.sheet(imgs), h0=POOL))
    sid = STRINGS
    for pages in LINES:
        MSG_IDS.append(sid)
        for p in pages + ["<PROC=0>\n<END>"]:
            r.set_string(sid, p)
            sid += 1
    ids = [r.new_proc_id() for _ in range(13)]
    entry, ents, arrive, start, cont, dlg, ghoul, done, stage, clawd_in, claude_in, frames_p, mover_p = ids
    r.set_proc(stage, stage_copy(r))
    r.set_proc(entry, proc([Call(861, 56), Call(864, 0), Call(553), Call(1265), Call(1), Call(stage),
                            Call(ents), Call(905), Call(6557), Call(6558)]))
    r.set_proc(ents, proc([Call(916, PX, PZ, 4, 0, 128, 0, 58, Proc(arrive), Proc(start)), Call(956, Proc(cont))]))
    r.set_proc(arrive, proc([command(0x9769, p=[PX, 406, PZ]), command(0x061E, i=0, d=4)]))
    r.set_proc(start, proc([Set(COUNTER, 0), Call(1667, 1, 1, 0, 0), Call(dlg)]))
    r.set_proc(cont, proc([]))
    r.set_proc(done, proc([Call(1669)]))
    r.set_proc(ghoul, bok_spawner())
    r.set_proc(clawd_in, sprite_in(CLAWD_TAG, CLAWD_PKG, CLX, CLZ))
    r.set_proc(claude_in, sprite_in(CLAUDE_TAG, CLAUDE_PKG, CDX, CDZ))
    r.set_proc(frames_p, frame_setter())
    r.set_proc(mover_p, mover())
    keys = [K_DLG, K_MGR, K_OTENKO, K_GHOUL, K_CLAWD, K_CLAUDE, K_FRAME, K_MOVE]
    procs = [dlg, 1675, 1676, ghoul, clawd_in, claude_in, frames_p, mover_p]
    starts = command(0x7348, d=SLOT, c=0, p=[Hash(k) for k in keys], P=[Proc(p) for p in procs], e=Proc(done))
    r.set_proc(dlg, dialogue(dlg, starts))
    r.set_event(SLOT, steps())
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    r.save(OUT)
    return {"scene": entry, "entrance": 58, "strings": (STRINGS, sid - 1), "dlg": dlg}


if __name__ == "__main__":
    info = build()
    print(f"{OUT}: scene {info['scene']} entrance {info['entrance']}, strings {info['strings']}, dialogue proc {info['dlg']}")
