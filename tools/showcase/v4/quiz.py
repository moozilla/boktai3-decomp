#!/usr/bin/env python3
"""Showcase v4: "Claude's Quiz Show", a playable Boktai 3 hack.

  python3 tools/showcase/v4/quiz.py build     -> build/showcase/v4/quiz.gba
  python3 tools/showcase/v4/bps.py make baserom.gba build/showcase/v4/quiz.gba quiz.bps
  python3 tools/showcase/v4/record.py NAME     -> build/showcase/v4/NAME.mp4 (a scripted playthrough)

To play: the Sealed Prison's dungeon menu, "Explore the Dungeon".

Claude (the Claudesona from v3) hosts a quiz show on the Pile Driver's
arena, redecorated in her colors. She asks Django questions about the
game's own insides, things the decompilation found, through the game's own
choice menus. A right answer earns applause and a prize; a wrong one
summons ghouls in her colors, and the quiz goes on once Django has fought
them off. Everything runs on the game's engine: new scripts (gclasm), a
cutscene for the opening, keyframe sprites, appended palette pools, strings
and cutscene slots (play.py), enemy spawners with our own death callback.
"""
import math
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import play  # noqa: E402  (puts v3 and tools/showcase on the path)
import art  # noqa: E402
import romx  # noqa: E402
from figures import Sprite  # noqa: E402
from gclasm import Arg, Expr, If, Proc, Set, Var, command, r700, switch  # noqa: E402
from show import (COUNTER, DJ, OT, Call, Cutscene, bgm, cam, fade, hud, keyframe_manager, kf_add,  # noqa: E402
                  kf_frame, kf_mirror, particles, sound, stage_of)

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(HERE)))
OUT = os.path.join(ROOT, "build", "showcase", "v4")
ROM = os.path.join(OUT, "quiz.gba")

CL = "<LABEL=GARIGARIKUN>Claude</LABEL>:\n"      # portraits drawn over two speakers (v3)
CL2 = "<LABEL=ROCKMAN>Claude</LABEL>:\n"
ARENA = 6201
ARENA_PALS = (0x8B1D, 0x46B8, 0x8504)
CX, CZ, Y = 1920, 1920, 512                     # the circle's middle: Claude's stage
PX, PZ = CX - 150, CZ + 650                     # where Django stands for the questions
ENTRANCE = 58
ARENA_STAGE = 515                               # the arena's stage number (proc_943 0x1839 57 515)
EXPLORE = 2587                                  # the dungeon menu's "Explore" (switch on DUNGEON)
DUNGEON = Var(0x10000000 | 1 << 24 | 0x800000 | 0x5AC)   # gs[0x5ac]:t1, the dungeon the menu is for (0: Sealed Prison)
PRISON = (2823, 1, 26)                          # what Explore does for the Sealed Prison
PRISON_MENU = (0xA14, 2, 539)                   # the Sealed Prison's dungeon menu (the world map's destination)
BOK = 0x05A5
GHOUL_POOL = 403                                # the ghoul's own palette (C = 1)
GOLD = 0
GHOUL_HP = int(os.environ.get("GHOUL_HP", "60"))
GHOUL_H = int(os.environ.get("GHOUL_H", "0"))   # 0: they die for good (1: undead that get back up)

# r700 scratch variables past every offset the game's scripts use (0x339; v3 used 0x33E)
LEFT = r700(0x340)       # ghouls still standing
QNUM = r700(0x342)       # the question being asked
SCORE = r700(0x344)      # right answers
TAG = 0x7005             # Claude's keyframe sprite
SFX_RIGHT, SFX_WRONG = 662, 402   # the scale quiz's correct and incorrect jingles (procs 5763, 5762)
PRIZES = [(PX + 260, PZ - 260), (PX - 260, PZ - 260), (PX + 360, PZ), (PX - 360, PZ), (PX + 100, PZ - 420), (PX - 100, PZ + 300)]


def claude_colors(rgb, bank):
    import colorsys
    h, l_, s_ = colorsys.rgb_to_hls(*(v / 255 for v in rgb))
    if bank <= 1:
        h, s_, l_ = 40 / 360, s_ * 0.45, min(0.96, l_ * 1.04)
    else:
        h, s_ = 15 / 360, min(0.8, s_ * 1.2 + 0.15)
    return tuple(int(round(v * 255)) for v in colorsys.hls_to_rgb(h, l_, s_))


def arena_stage(show):
    """proc_6200 (the arena's stage) with recolored palettes, and room for keyframe sprites."""
    import gcx
    import struct
    pals = [show.resource(romx.STAGE_PAL_TYPE, 0x6090 + k,
                          romx.recolor_stage_palette(romx.resource(show.r.rom, romx.STAGE_PAL_TYPE, h),
                                                     claude_colors, range(12)))
            for k, h in enumerate(ARENA_PALS)]
    o = show.r.img.proc_address(6200 - 1)
    st, n = gcx.length(bytes(show.r.rom), o)
    code = bytearray(show.r.rom[o:st + n])
    for old, new in zip(ARENA_PALS, pals):
        k = code.find(struct.pack("<H", old))
        assert k > 0
        struct.pack_into("<H", code, k, new)
    pid = show.r.new_proc_id()
    show.r.set_proc(pid, bytes(code))
    return pid


def portraits(show):
    sys.path.insert(0, os.path.join(play.V3))
    from part1 import portrait
    portrait(show, "claude_neutral", 0x593B, 14, 506)
    portrait(show, "claude_smug", 0x593B, 8, 504)


# body ramps (ghoul pool 403 indices 2-6) and accents (7-9) in Claude's colors
GHOUL_RAMPS = [
    ([0x5A2414, 0x8E3F2A, 0xC15F3C, 0xD97757, 0xF2C2A8], [0x3C2A20, 0x8A6A50, 0xE8D8C0]),   # terracotta
    ([0x6A5A48, 0x9C8A70, 0xC8B898, 0xE8DCC0, 0xFAF6EE], [0x8E3F2A, 0xC15F3C, 0xD97757]),   # cream
    ([0x4A1810, 0x7A2818, 0xA83C24, 0xCC5A38, 0xEC9070], [0x2A2A2A, 0x6A6A6A, 0xD0D0D0]),   # rust
    ([0x5C3A10, 0x946018, 0xC88A28, 0xE8B048, 0xF8E0A0], [0x8E3F2A, 0xC15F3C, 0xF2C2A8]),   # amber
]


def ghoul_pools(show):
    """The ghoul's palette (pool 403) in Claude's colors: new pools; returns their C values (pool - 402)."""
    cols = romx.pool_colors(show.r.rom, GHOUL_POOL)
    out = []
    for body, accent in GHOUL_RAMPS:
        pal = list(cols)
        pal[2:7] = body
        pal[7:10] = accent
        out.append(show.pool(pal) - 402)
    return out


# ---- the questions -------------------------------------------------------
# Each: Claude's lead-in pages, the question page (one line, then options in
# <ALTER> tags; the window is 26 characters wide), the right option (None:
# every option is right), her line for a right answer and for a wrong one,
# how many ghouls a wrong answer summons, and the prize a right one drops
# (data/items.csv).
SOLAR_FRUIT, DELUXE_CHOCOLATE, GARIGARI_COLA, ELIXIR, DROP_OF_SUN, POWER_FRUIT = 0x0D, 0x07, 0x0B, 0x10, 0x11, 0x14
QUESTIONS = [
    ([CL + '"Question one! This whole\nworld runs on scripts."'],
     'They come from which game?\n <ALTER>Metal Gear Solid</ALTER>  <ALTER>Zelda</ALTER>\n <ALTER>Castlevania</ALTER>',
     0, [CL2 + '"Right! Konami\'s GCL, the\nlanguage of Metal Gear."'],
     [CL + '"Nope! Metal Gear\'s GCL.\nSay hi to my friends!"'], 3, SOLAR_FRUIT),
    ([CL + '"Question two!"'],
     'How many script procedures?\n <ALTER>404</ALTER>   <ALTER>5,615</ALTER>   <ALTER>9,001</ALTER>',
     1, [CL2 + '"5,615! Plus the few dozen\nI just wrote for this show."'],
     [CL + '"5,615! Count them while\nyou fight these."'], 3, DELUXE_CHOCOLATE),
    ([CL + '"Question three!\nListen closely..."'],
     'Who plays this music?\n <ALTER>MP2K, aka Sappy</ALTER>\n <ALTER>Otenko, humming</ALTER>',
     0, [CL2 + '"MP2K! Nintendo\'s sound\ndriver, same as Pokemon."'],
     [CL + '"Otenko can\'t hold a tune.\nIt\'s MP2K! Get \'em!"'], 4, GARIGARI_COLA),
    ([CL + '"Question four! The\ngame was written in C."'],
     'Which compiler built it?\n <ALTER>Rust</ALTER>  <ALTER>agbcc</ALTER>  <ALTER>Crayons</ALTER>',
     1, [CL2 + '"agbcc, from GCC 2.95! We\nrebuild it byte for byte."'],
     [CL + '"agbcc! Rust is from 2015.\nThis game is from 2005!"'], 4, POWER_FRUIT),
    ([CL + '"Question five! The game\nhas 11,491 functions."'],
     'How many are back in C?\n <ALTER>About 300</ALTER>  <ALTER>About 8,900</ALTER>\n <ALTER>All of them</ALTER>',
     1, [CL2 + '"8,883 and counting! Not\nall of them... yet."'],
     [CL + '"8,883, and counting!\nBut first, these guys."'], 5, ELIXIR),
    ([CL + '"Last one! The big one."'],
     'What does Elefan say?\n <ALTER>PAO!</ALTER>   <ALTER>PAO?</ALTER>',
     None, [CL2 + '"Trick question!\nIt\'s always PAO."'], [], 0, DROP_OF_SUN),
]
FINALE = {   # by score
    6: [CL2 + '"A perfect score! You\'re\na natural decompiler!"'],
    4: [CL + '"Not bad at all! You\'ve\nbeen reading the source."'],
    0: [CL + '"Hey, you survived!\nThat counts for something."'],
}


class Quiz:
    def __init__(self):
        self.show = show = play.PlayShow()
        self.claude, self.cf, _ = art.package(show, 0x6001, "claude")
        portraits(show)
        self.pools = ghoul_pools(show)
        self.hp = stage_of(show.r.img, ARENA)[0][1][2]
        R = int(os.environ.get("RING", "520"))
        ring = [(PX + int(R * math.cos(a)), PZ - 200 + int(R * math.sin(a)))
                for a in [math.pi * (0.1 + 0.35 * k) for k in range(6)]]
        from rom import add_paths
        self.first_path = add_paths(show.r, self.hp, [(x, z, 0) for x, z in ring])
        self.npaths = len(ring)
        self.ring = ring

    def spawn_points(self):
        """Where each question's ghouls start (world x, z), for the recorder."""
        return [[self.ring[(i * 2 + k) % self.npaths] for i in range(q[5])] for k, q in enumerate(QUESTIONS)]

    def frame(self, f):
        return kf_frame(TAG, self.cf[f])

    def build(self):
        show = self.show
        ids = {k: show.r.new_proc_id() for k in ("spawn", "died", "next", "finale")}
        qprocs = [(show.r.new_proc_id(), show.r.new_proc_id(), show.r.new_proc_id()) for _ in QUESTIONS]
        # the ghoul spawner: arg1 name, arg2 path, arg3 palette (C); level-10 numbers from proc_1500
        show.set_proc(ids["spawn"], [
            command(BOK, N=Arg(1), r=[Arg(2), 0, 0], e=[512, 16], a=[6, 12, 2, 30, 9, 64, 2, 50], P=[80, 180, 120],
                    A=[GHOUL_HP, 0, 0, 2, 1026, 1], l=[1, 200, 600], n=[768, 512], C=Arg(3), F=0, I=0x46D, D=ids["died"],
                    G=[0x5A0001, 540, 0], M=512, X=[33, 0], K=4, L=0, V=10, H=GHOUL_H),
            ])
        # a ghoul is down: when the last one falls, the next question
        show.set_proc(ids["died"], [Set(LEFT, LEFT, 1, "-"),
                                    If(Expr(LEFT, 0, "<="), [Call(ids["next"])])])
        cases = [(k, [Call(q[0])]) for k, q in enumerate(qprocs)] + [(len(qprocs), [Call(ids["finale"])])]
        show.set_proc(ids["next"], [switch(QNUM, cases)])
        for k, ((lead, question, right, yes, no, ghouls, prize), (qp, ap, jp)) in enumerate(zip(QUESTIONS, qprocs)):
            show.set_proc(qp, [Set(QNUM, k), self.frame("talk")] + show.say(lead, then=ap, start=True))
            show.set_proc(ap, [self.frame("point")] + show.ask(question, jp))
            wrong = show.proc()
            spawns = [Call(ids["spawn"], 0x7A00 + 16 * k + i, self.first_path + (i * 2 + k) % self.npaths,
                           self.pools[(i + k) % len(self.pools)]) for i in range(ghouls)]
            show.set_proc(wrong, [Call(play.END_TALK), Set(LEFT, ghouls), Set(QNUM, k + 1), self.frame("stand_dl")]
                          + spawns)
            x, z = PRIZES[k % len(PRIZES)]
            good = ([Set(SCORE, SCORE, 1, "+"), Set(QNUM, k + 1), self.frame("cheer"), command(0x1870, i=SFX_RIGHT),
                     Call(1119, 0, prize, x, z, 2, 1, 0, 0, 0, 0)] + show.say(yes, then=ids["next"]))
            if right is None:
                show.set_proc(jp, good)
            else:
                show.set_proc(jp, [If(Expr(Arg(1), right, "=="), good,
                                      [self.frame("shrug"), command(0x1870, i=SFX_WRONG)] + show.say(no, then=wrong))])
        home = show.proc([Call(play.END_TALK), Set(play.ASKING, 2), Call(951, *PRISON_MENU)])   # ASKING 2: the show is over
        bye = show.proc([self.frame("wave")] + show.say(
            [CL + '"That\'s the show, folks!\nThanks for playing, Django!"',
             CL2 + '"Now go purify something.\nThe sun\'s in your hands."'], then=home))
        verdict = [(n, show.say(FINALE[n], then=bye)) for n in sorted(FINALE, reverse=True)]
        tail = verdict[-1][1]
        for n, stmts in reversed(verdict[:-1]):
            tail = [If(Expr(SCORE, n, ">="), stmts, tail)]
        show.set_proc(ids["finale"], [Call(1016, 0, 1), self.frame("cheer"), command(0x1870, i=SFX_RIGHT)] + tail)
        print("procs:", ids, "first path", self.first_path)
        pid = self.scene(ids["next"])
        self.entry(pid)
        return pid

    def entry(self, pid):
        """The way in: the dungeon menu's "Explore" for the Sealed Prison (call
        2587, case gs[0x5AC] == 0: proc_951(2823, 1, 26)) now leads to Claude's
        stage; she offers the quiz or sends Django on to the prison. Every other
        dungeon runs the original procedure (copied to a new id)."""
        import gcx
        show = self.show
        o = show.r.img.proc_address(EXPLORE - 1)
        st, n = gcx.length(bytes(show.r.rom), o)
        orig = show.r.new_proc_id()
        show.r.set_proc(orig, bytes(show.r.rom[o:st + n]))
        show.set_proc(EXPLORE, [If(Expr(DUNGEON, 0, "=="), [Call(951, pid, ENTRANCE, ARENA_STAGE)], [Call(orig)])])

    def scene(self, first):
        """The scene: the arena, Claude on her stage, a short opening, then the quiz."""
        show = self.show
        cut = Cutscene(show, extra=(1675, 1676))
        cut.at(0, fade(0, 6, 0, 0, 0), cam(CX - 60, CZ + 300, 1, y=Y + 150), hud(7))
        cl = Sprite(cut, TAG, self.claude, self.cf, y=Y).add(1, CX, CZ, frame="stand_dl")
        cut.hold(40)
        cl.frame(0, "wave")
        cut.say([CL + '"Welcome, welcome!\nIt\'s time for...', CL2 + '"CLAUDE\'S QUIZ SHOW!"'])
        cl.frame(0, "talk")
        cut.say([DJ + '"...Who are you?"'])
        cut.say([CL + '"I\'m Claude. I took your\ngame apart, line by line.',
                 CL2 + '"Get one right: a prize!\nGet one wrong: well...'])
        go = show.proc([Call(play.END_TALK), Call(951, *PRISON)])
        play_ = show.proc([Set(QNUM, 0), Set(SCORE, 0), Call(first)])
        choose = show.proc([If(Expr(Arg(1), 0, "=="), [self.frame("cheer")] + show.say(
            [CL2 + '"That\'s the spirit!"'], then=play_),
            [self.frame("shrug")] + show.say([CL + '"Suit yourself! The prison\'s\nright through there."'], then=go))])
        end = show.proc([Call(1668), Call(1016, 0, 1), self.frame("point")] + show.ask(
            'Play Claude\'s Quiz Show?\n <ALTER>Let\'s play!</ALTER>  <ALTER>Not now</ALTER>', choose))
        dlg = cut.write(end)
        arrive = show.proc([command(0x9769, p=[PX, Y + 150, PZ]), command(0x061E, i=0, d=4)])
        start = show.proc([Set(COUNTER, 0), Call(1667, 1, 1, 0, 0), Call(dlg)])
        stage = show.proc([Call(arena_stage(show)), keyframe_manager(8)])
        ents = show.proc([Call(916, PX, PZ, 1, 0, 128, 0, ENTRANCE, Proc(arrive), Proc(start))])
        pid = show.proc([Call(861, 56), Call(864, 0), Call(553), Call(1265), Call(1), Call(stage), Call(ents),
                         Call(905), Call(1048, 3)])
        return pid


def build():
    q = Quiz()
    pid = q.build()
    import show as show_mod
    assert not show_mod.TOO_LONG, show_mod.TOO_LONG
    q.show.save(ROM)
    return pid


if __name__ == "__main__":
    pid = build()
    print(f"{ROM}: scene {pid} entrance {ENTRANCE}")
