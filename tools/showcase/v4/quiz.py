#!/usr/bin/env python3
"""Showcase v4: "Claude's Quiz Show", a playable Boktai 3 hack.

  python3 tools/showcase/v4/quiz.py build     -> build/showcase/v4/quiz.gba (+ .ips)

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
from gclasm import Arg, Expr, If, Proc, Set, command, r700, switch  # noqa: E402
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
WORLD_SAN_MIGUEL = 2571                         # the world map's San Miguel destination: proc_951(TOWN, 115, 538)
TOWN, TOWN_ENTRANCE, TOWN_STAGE = 0x9F9, 115, 538
BOK = 0x05A5
GHOUL_POOL = 403                                # the ghoul's own palette (C = 1)
GOLD = 0
GHOUL_HP = int(os.environ.get("GHOUL_HP", "60"))
GHOUL_H = int(os.environ.get("GHOUL_H", "1"))

# r700 scratch variables past every offset the game's scripts use (0x339; v3 used 0x33E)
LEFT = r700(0x340)       # ghouls still standing
QNUM = r700(0x342)       # the question being asked
SCORE = r700(0x344)      # right answers
TAG = 0x7005             # Claude's keyframe sprite


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
# (Claude's lead-in pages, the question page with <ALTER> options, the right
# option's index, her line for a right answer, her line for a wrong one, ghouls)
QUESTIONS = [
    ([CL + '"Question one!\nThis world runs on scripts."'],
     'Its scripts are written in:\n <ALTER>Metal Gear Solid</ALTER>  <ALTER>Zelda</ALTER>\n <ALTER>Castlevania</ALTER>',
     0, [CL2 + '"Correct! Konami\'s GCL,\nhashes and all."'],
     [CL + '"Nope! Metal Gear\'s GCL.\nSay hi to my friends!"'], 3),
    ([CL + '"Question two!"'],
     'How many script procedures?\n <ALTER>404</ALTER>  <ALTER>5,615</ALTER>  <ALTER>9,001</ALTER>',
     1, [CL2 + '"5,615! Well, plus\nthe ones I just wrote."'],
     [CL + '"It\'s 5,615!\nGhouls, take it away!"'], 4),
]


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
        for k, ((lead, question, right, yes, no, ghouls), (qp, ap, jp)) in enumerate(zip(QUESTIONS, qprocs)):
            show.set_proc(qp, [Set(QNUM, k), self.frame("talk")] + show.say(lead, then=ap, start=True))
            show.set_proc(ap, [self.frame("point")] + show.ask(question, jp))
            wrong = show.proc()
            spawns = [Call(ids["spawn"], 0x7A00 + 16 * k + i, self.first_path + (i * 2 + k) % self.npaths,
                           self.pools[(i + k) % len(self.pools)]) for i in range(ghouls)]
            show.set_proc(wrong, [Call(play.END_TALK), Set(LEFT, ghouls), Set(QNUM, k + 1), self.frame("stand_dl")]
                          + spawns)
            show.set_proc(jp, [If(Expr(Arg(1), right, "=="),
                                  [Set(SCORE, SCORE, 1, "+"), Set(QNUM, k + 1), self.frame("cheer")]
                                  + show.say(yes, then=ids["next"]),
                                  [self.frame("shrug")] + show.say(no, then=wrong))])
        home = show.proc([Call(play.END_TALK), Call(951, TOWN, TOWN_ENTRANCE, TOWN_STAGE)])
        show.set_proc(ids["finale"], [self.frame("wave")] + show.say(
            [CL + '"That\'s the show!\nThanks for playing, Django!"'], then=home, start=True))
        print("procs:", ids, "first path", self.first_path)
        pid = self.scene(ids["next"])
        self.entry(pid)
        return pid

    def entry(self, pid):
        """The way in: choosing San Miguel on the world map (call 2571) now asks
        where to go, the town or Claude's Quiz Show."""
        show = self.show
        choose = show.proc([If(Expr(Arg(1), 1, "=="), [Call(951, pid, ENTRANCE, ARENA_STAGE)],
                               [Call(951, TOWN, TOWN_ENTRANCE, TOWN_STAGE)])])
        show.set_proc(WORLD_SAN_MIGUEL, [Call(951, pid, ENTRANCE, ARENA_STAGE)] if os.environ.get("DIRECT") else show.ask(
            'Where to?\n <ALTER>San Miguel</ALTER>\n <ALTER>Claude\'s Quiz Show</ALTER>', choose))

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
        end = show.proc([Call(1668), Set(QNUM, 0), Set(SCORE, 0), Call(first)])
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
