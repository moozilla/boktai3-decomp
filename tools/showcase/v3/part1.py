#!/usr/bin/env python3
"""Showcase v3, part 1: "The Stranger" (docs/showcase/V3_PLAN.md), second draft.

  python3 tools/showcase/v3/part1.py build              -> build/showcase/v3/part1.gba
  python3 tools/showcase/v3/part1.py record PIECE [S]   -> build/showcase/v3/rec/PIECE.mp4
                                                         (and a contact sheet, one frame every S s)

One ROM holds every set piece; each is its own scene (or chain of scenes)
on a real stage, started by warping to it, and recorded on its own.
Everything is played by the game: new scripts and cutscene tables
(show.py), keyframe sprites from new sprite packages and palette pools
(romx.py, art.py), motion tracks run by the game's repeating timer (so
they keep moving while a message is up), the player, Otenko, Vanargand,
the camera, the screen and the sound through their cutscene receivers,
and the game's own text crawl for the title and the credits.

Pieces, in the video's order: cold, doors, night, marionettes, count, ride,
reveal, coffins, vride, sunback, title, ... (Part 2) ..., credits.
"""
import math
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import art  # noqa: E402
import record  # noqa: E402
import romx  # noqa: E402
import run  # noqa: E402
from figures import Otenko, Player, Sprite, screen_to_world  # noqa: E402
from show import (COUNTER, DJ, OT, STORY, Call, Cutscene, Proc, Set, Show, bgm, bgm_out, cam, cam_follow, r700,  # noqa: E402
                  command, fade, hud, keyframe_manager, kf_frame, kf_glide, kf_hide, kf_mirror, kf_show,
                  particles, quake, sound, stage_of, top_messages, track)
from rom import big_sprite_frame_pixels  # noqa: E402

CL = "<LABEL=GARIGARIKUN>Claude</LABEL>:\n"      # portraits drawn over two speakers the video never shows
CL2 = "<LABEL=ROCKMAN>Claude</LABEL>:\n"
TOWN = "<LABEL=PEOPLE1>Townsman</LABEL>:\n"
COUNT = "<LABEL=HAKUSYAKU>Count</LABEL>:\n"
NOBODY = "<LABEL=NONE>???</LABEL>:\n"
GOLD, RED, FLAME, ICE, PURPLE, GREEN = 0, 1, 2, 3, 4, 5   # OrbitingParticles types

ROM = os.path.join(run.OUT, "part1.gba")
REC = os.path.join(run.OUT, "rec")
STRINGS = list(range(8140, 8400))     # the ending's dialogue: no scene we record shows it
SLOTS = list(range(120, 144))         # late-castle and ending cutscenes: likewise
ENTRANCE = 58                         # every scene's own entrance number

PIECES = {}


def piece(fn):
    PIECES[fn.__name__] = fn
    return fn


class Scene:
    """A new scene on a real place's stage: our entrance places the player at
    (x, z) and starts the cutscene; its end warps on (`then`) or just ends."""

    def __init__(self, show, place, x, z, facing=1, y=256, flags=56, capacity=24, song=None, extra=(1675, 1676),
                 stage=None, camera=None, fade_speed=6, hud_mode=None):
        self.show = show
        self.place = place
        self.pid = show.r.new_proc_id()
        self.x, self.z, self.y, self.facing = x, z, y, facing
        self.flags, self.capacity = flags, capacity
        (sp, _), real_song = stage_of(show.r.img, place) if stage is None else ((stage, None), None)
        self.stage_proc = sp
        self.song = song if song is not None else real_song
        self.cut = Cutscene(show, extra=extra)
        cx, cz = camera or (x, z)
        self.cut.at(0, fade(0, fade_speed, 0, 0, 0), cam(cx, cz, 1, y=y + 150))
        if hud_mode is not None:
            self.cut.at(1, hud(hud_mode))
        self.player = Player(self.cut)
        self.player.pos = (x, z)
        self.otenko = Otenko(self.cut, y=y)
        self.spawns = []
        self.then = None
        self.events = []          # harness lines timed against the cutscene (record.py)
        self.tail = 240           # frames recorded after the last page
        self.chain = []           # later scenes' cutscenes in the same recording

    def build(self):
        show, cut = self.show, self.cut
        end = [Call(1669)]
        if self.then:
            end.append(Call(945, self.then[0], self.then[1], 0))
        dlg = cut.write(show.proc(end))
        arrive = show.proc([command(0x9769, p=[self.x, self.y + 150, self.z]), command(0x061E, i=0, d=4)])
        start = show.proc([Set(COUNTER, 0), Call(1667, 1, 1, 0, 0), Call(dlg)])
        stage = show.proc([Call(self.stage_proc), keyframe_manager(self.capacity)])
        ents = show.proc([Call(916, self.x, self.z, self.facing, 0, 128, 0, ENTRANCE, Proc(arrive), Proc(start))])
        body = [Call(861, self.flags), Call(864, 0), Call(553), Call(1265), Call(1), Call(stage), Call(ents), Call(905)]
        body += self.spawns
        if self.song is not None:
            body.append(Call(1048, self.song))
        show.set_proc(self.pid, body)
        return self.pid


# ---- art -----------------------------------------------------------------
COFFIN_POOLS = [643, 644, 645, 646, 647, 648, 649, 651]   # wood, jade, stone, silver, red, flame, teal, gold


class Cast:
    """The packages every piece shares, built once per ROM."""

    def __init__(self, show):
        import proofs
        self.show = show
        self.stranger, self.sf, self.spool = art.package(show, 0x6000, "stranger")
        frames, origin, pool = romx.big_frames(show.r.rom, 0x9247)
        base = romx.package(frames, pool, origin)
        self.ghoul = show.package(0x6010, base)
        # the fan: ten hues, each twice (13 OBJ palettes are free at most)
        pals = proofs.hue_variants(romx.pool_colors(show.r.rom, pool), 10, [7, 8, 9, 10, 11])
        self.fan = [show.package(0x6020 + k, romx.repool(base, show.pool(p))) for k, p in enumerate(pals)]
        self.claude, self.cf, self.cpool = art.package(show, 0x6001, "claude")
        self.reveal, self.rf, _ = art.package(show, 0x6002, "reveal")
        # the Count (F761) in six more palettes: headers aliasing the game's package
        cols = romx.pool_colors(show.r.rom, 821)
        cpals = proofs.hue_variants(cols, 7, range(1, 16))[1:]
        self.count = [0xF761] + [show.alias(0x6030 + k, 0xF761, show.pool(c)) for k, c in enumerate(cpals)]
        # the coffin Django drags (large sprite 2499: every angle and the lid
        # opening) in the coffin shop's palettes, 643 on
        frames, origin, pool = romx.big_frames(show.r.rom, 0x2499)
        cbase = show.package(0x6040, romx.package(frames, COFFIN_POOLS[0], origin))
        self.coffins = [cbase] + [show.alias(0x6041 + k, cbase, p) for k, p in enumerate(COFFIN_POOLS[1:])]
        # the skylight's patch of light (large sprite CBE1), in its sunlight
        # palette and in six hues for the strange light
        frames, origin, pool = romx.big_frames(show.r.rom, 0xCBE1)
        lbase = show.package(0x6050, romx.package(frames, 456, origin))
        lpals = proofs.hue_variants(romx.pool_colors(show.r.rom, 456), 6, range(1, 16))
        self.light = lbase
        self.rainbow = [show.alias(0x6051 + k, lbase, show.pool(c)) for k, c in enumerate(lpals)]
        # ones and zeros, in the game's text font
        self.digits = show.package(0x6060, digit_package(show))
        # Otenko as a keyframe sprite, for stages that never load his NPC sprite
        frames, origin, pool = romx.big_frames(show.r.rom, 0x97D3)
        self.otenko = show.package(0x6070, romx.package(frames, pool, origin))
        # the Pile Driver's arena in Claude's colors (its three time-of-day palettes)
        self.arena_pals = [show.resource(romx.STAGE_PAL_TYPE, 0x6090 + k,
                                         romx.recolor_stage_palette(romx.resource(show.r.rom, romx.STAGE_PAL_TYPE, h),
                                                                    claude_colors, range(12)))
                           for k, h in enumerate(ARENA_PALS)]
        # a link of the chain coffins are dragged on (the common sprite sheet's 1C1C)
        self.bead = show.package(0x6080, chain_bead_package(show))
        portrait(show, "claude_neutral", 0x593B, 14, 506)
        portrait(show, "claude_smug", 0x593B, 8, 504)


ARENA = 6201                               # the Pile Driver's arena (Lifeless Town's purification)
ARENA_PALS = (0x8B1D, 0x46B8, 0x8504)      # its stage palettes: day, dusk, night
COMMON_SHEET = (0xCEAA, 0x3002)            # the common sprite sheet AgbMain loads to OBJ VRAM (tiles at +0x24)
CHAIN_TILE = 236                           # its entry 1C1C, frame 0: a chain link (TetherActor_InitializeSegments)


def claude_colors(rgb, bank):
    """The arena recolored: the ground warm cream, the stones and the
    circle's glow terracotta (banks 0-1 ground, 2-11 the circle)."""
    import colorsys
    h, l_, s_ = colorsys.rgb_to_hls(*(v / 255 for v in rgb))
    if bank <= 1:
        h, s_, l_ = 40 / 360, s_ * 0.45, min(0.96, l_ * 1.04)
    else:
        h, s_ = 15 / 360, min(0.8, s_ * 1.2 + 0.15)
    return tuple(int(round(v * 255)) for v in colorsys.hls_to_rgb(h, l_, s_))


def arena_stage(show, cast):
    """The arena's stage procedure (proc_6200) with our palettes in place of its own."""
    import gcx
    import struct
    o = show.r.img.proc_address(6200 - 1)
    st, n = gcx.length(bytes(show.r.rom), o)
    code = bytearray(show.r.rom[o:st + n])
    for old, new in zip(ARENA_PALS, cast.arena_pals):
        k = code.find(struct.pack("<H", old))
        assert k > 0 and code.find(struct.pack("<H", old), k + 1) < 0
        struct.pack_into("<H", code, k, new)
    pid = show.r.new_proc_id()
    show.r.set_proc(pid, bytes(code))
    return pid


def chain_bead_package(show):
    import struct
    a = romx.resource(show.r.rom, *COMMON_SHEET)
    o = 0x24 + 32 * CHAIN_TILE
    img = [[(a[o + y * 4 + x // 2] >> (4 * (x & 1))) & 15 for x in range(8)] for y in range(8)]
    pool = show.pool([0, 0x282830, 0, 0x8890A0, 0xD8E0E8] + [0] * 11)
    return romx.package([img], pool, (4, 6))


DIGIT_STRIPS = ["101", "010", "110", "011", "100", "001"]


def digit_package(show):
    """Columns of three binary digits drawn with the game's 8x16 font (the
    fnt resource: glyph c at 0xC + (c - 0x20) * 64, two 4bpp tiles, values
    1-3), in a green that glows."""
    import archive
    rom = bytes(show.r.rom)
    _, a, s = next(ents for label, t, b, ents in archive.archives(rom) if label == "fnt")[0]
    font = rom[a - 0x08000000:a - 0x08000000 + s]

    def glyph(ch):
        o = 0xC + (ord(ch) - 0x20) * 64
        return [[(font[o + t * 32 + r * 4 + c // 2] >> (4 * (c & 1))) & 15 for c in range(8)]
                for t in (0, 1) for r in range(8)]
    frames = []
    for strip in DIGIT_STRIPS:
        img = []
        for ch in strip:
            img += [[v if 1 <= v <= 3 else 0 for v in row] for row in glyph(ch)]
        frames.append(img)
    pool = show.pool([0, 0x98F8A0, 0x50C060, 0x287838] + [0] * 12)
    return romx.package(frames, pool, (4, 48))


def portrait(show, name, sprite, frame, pool):
    """Draw a 32x32 portrait over a speaker's portrait frame and its palette."""
    import numpy as np
    from PIL import Image
    px = np.asarray(Image.open(os.path.join(art.ART, "portraits", name + ".png")).convert("RGB"))
    cols = []
    idx = [[0] * 32 for _ in range(32)]
    for y in range(32):
        for x in range(32):
            c = tuple(int(v) >> 3 << 3 for v in px[y, x])
            if c not in cols:
                cols.append(c)
            idx[y][x] = cols.index(c) + 1
    assert len(cols) <= 15
    out = bytearray()
    for ty in range(4):
        for tx in range(4):
            for yy in range(8):
                for xx in range(0, 8, 2):
                    out.append(idx[ty * 8 + yy][tx * 8 + xx] | idx[ty * 8 + yy][tx * 8 + xx + 1] << 4)
    a, n = big_sprite_frame_pixels(show.r, sprite, frame)
    assert n == 16
    show.r.rom[a - 0x08000000:a - 0x08000000 + 512] = out
    show.pools.set(pool, [0x787888] + [r << 16 | g << 8 | b for r, g, b in cols] + [0] * (15 - len(cols)))


# ---- geometry and facing --------------------------------------------------
def screen(x, z, y=256):
    """Screen position of a world point (up to the camera's offset)."""
    return 3 * (x - z) / 32, 3 * (x + z - y) / 32


# NPC packages: their frames for up, up-diagonal, side, down-diagonal, down,
# and which way the side and diagonal frames face (checked in a ring,
# proofs.facing)
NPC = {h: ((0, 1, 2, 3, 4), "left") for h in (0xDFCE, 0xDFDE, 0xDFEE, 0xDFFE, 0xE02E, 0xE03E, 0xE2FE)}


def npc_facing(pkg, frm, to):
    """(frame, mirror) for NPC package pkg at world (x, z) frm to face world point to."""
    (fx, fy), (tx, ty) = screen(*frm), screen(*to)
    a = math.degrees(math.atan2(tx - fx, -(ty - fy))) % 360        # 0 up, 90 right, 180 down
    left = a > 180
    if left:
        a = 360 - a
    k = min(4, int((a + 22.5) // 45))
    order, faces = NPC[pkg]
    mirror = 1 if (left and faces == "right") or (not left and faces == "left") else 0
    if k in (0, 4):
        mirror = 0
    return order[k], mirror


def stranger_toward(frm, to):
    """Mirror flag for the stranger (its frames face down-right) to face `to`."""
    return 1 if screen(*to)[0] < screen(*frm)[0] else 0


SNAP = (201, 202)          # two short noise clicks together: the nearest thing the game has to a finger snap
OMEN = (1246, 1425)        # a slow swell over a low rumble: the stranger arrives (sound test audition, proofs.sfx)


def snap(cut, st, t, frame="snap"):
    """The stranger (or Claude) snaps its fingers at t, with the sound."""
    if st is not None:
        st.frame(t, frame)
    cut.at(t, *[sound(n) for n in SNAP])


def omen(cut, t):
    cut.at(t, *[sound(n) for n in OMEN])


def vanish(cut, st, t, x, z, y=256):
    """The stranger turns and dissolves into motes of light."""
    st.frame(t, "turn")
    st.frame(t + 14, "vanish1")
    cut.at(t + 14, particles(x, z, 30, y=y + 64, kind=GOLD), sound(1407))
    st.frame(t + 20, "vanish2")
    st.frame(t + 26, "vanish3")
    st.frame(t + 32, "vanish4")
    st.hide(t + 40)


# ---- the pieces ----------------------------------------------------------
@piece
def cold(show, cast):
    """Cold open: Django and Otenko find a ghoul that isn't right; it wobbles,
    a cloaked figure floats in from the side, snaps, and the ghoul fans out
    into twenty in ten palettes; another snap and the copies burst into
    motes of light, leaving the ghoul to look itself over."""
    X, GZ, DZ = 1408, 2060, 2480           # the corridor's line, the ghoul, where Django stops
    SX, SZ = 1880, 1740                    # where the stranger lands, to the ghoul's right
    sc = Scene(show, 2803, X, 3150, facing=1, song=None, camera=(X, 2750), fade_speed=4)
    cut, dj, ot = sc.cut, sc.player, sc.otenko
    cut.at(0, bgm(18))
    ot.place(0, X + 230, 3250, d=1)
    dj.walk(30, X, DZ)
    ot.walk(40, X + 200, DZ + 130)
    cut.at(20, cam(1560, 2260, 140))
    ghoul = Sprite(cut, 0x7002, cast.ghoul, {}).add(1, X, GZ, frame=0)
    ghoul.mirror(1)
    flick = [Sprite(cut, 0x7020 + k, cast.fan[k * 3], {}).add(1, X, GZ, frame=0, hidden=True) for k in range(3)]
    for f in flick:
        f.mirror(1)
    cut.hold(175)
    cut.say([DJ + '"Where are we?"'])
    cut.say([OT + '"The Sealed Prison...\nbut that Ghoul looks wrong."'])
    # the ghoul wobbles: it jitters, and its colors flicker
    for i in range(16):
        ghoul.glide(3 * i, X + (36 if i % 2 == 0 else -36), GZ, 3)
        f = flick[i % 3]
        f.jump(3 * i, X + (36 if i % 2 == 0 else -36), GZ)
        if i % 2 == 0:
            f.show(3 * i)
            f.hide(3 * i + 3)
    ghoul.glide(48, X, GZ, 3)
    cut.at(0, quake(10, 48))
    # the stranger floats in from the right, facing them
    st = Sprite(cut, 0x7001, cast.stranger, cast.sf).add(40, SX + 760, SZ - 520, y=256 + 300, frame="stand_dr",
                                                        hidden=True)
    m = stranger_toward((SX, SZ), (X, DZ))
    st.frame(40, "stand_dr", mirror=m)
    st.show(44)
    omen(cut, 40)
    st.glide(44, SX, SZ, 70, y=256 + 70)
    cut.at(44, particles(SX + 380, SZ - 260, 40, y=256 + 200, kind=GOLD))
    for i in range(4):
        st.glide(114 + 12 * i, SX, SZ, 12, y=256 + (40 if i % 2 == 0 else 70))
    st.glide(162, SX, SZ, 16, y=256)
    st.frame(190, "raise")
    snap(cut, st, 222)
    # the fan: copies slide out of the ghoul one after another, ten hues twice
    t0 = 226
    n = 20
    copies, spots = [], []
    for k in range(n):
        s_ = Sprite(cut, 0x7030 + k, cast.fan[k % 10], {}).add(t0, X, GZ, frame=0)
        s_.mirror(t0)
        copies.append(s_)
    for k, s_ in enumerate(copies):
        a = math.radians(-78 + 156 * k / (n - 1))
        dx, dz, dy = screen_to_world(86 * math.sin(a), 48 * math.cos(a))
        spots.append((X + dx, GZ + dz, 256 + dy))
        s_.glide(t0 + 2 + k, X + dx, GZ + dz, 20, y=256 + dy)
    cut.at(t0 + 2, sound(1425), quake(20, 10))
    for k, s_ in enumerate(copies):
        s_.cycle(t0 + 24 + k % 4, [0, 1, 2, 3], 130, step=10)
    st.frame(t0 + 40, "stand_dr")
    # a second snap: the copies burst into motes, from the ends of the fan in
    tb = t0 + 150
    st.frame(tb - 30, "raise")
    snap(cut, st, tb - 6)
    order = sorted(range(n), key=lambda k: -abs(k - (n - 1) / 2))
    kinds = [GOLD, RED, FLAME, ICE, PURPLE, GREEN]
    for i, k in enumerate(order):
        t = tb + 3 * i
        x, z, y = spots[k]
        copies[k].hide(t)
        cut.at(t, particles(x, z, 30, y=y + 48, kind=kinds[k % len(kinds)]))
        if i % 4 == 0:
            cut.at(t, sound(1407))
    st.frame(tb + 14, "stand_dr")
    # the ghoul looks itself over
    tl = tb + 3 * n + 20
    ghoul.frame(tl, 8)
    ghoul.frame(tl + 30, 9, mirror=0)
    ghoul.frame(tl + 62, 8, mirror=1)
    cut.hold(tl + 100)
    cut.say([DJ + '"..."'])
    cut.say([OT + '"Django... Who is that?"'])
    vanish(cut, st, 0, SX, SZ)
    end(cut, 70)
    return sc


def end(cut, t, speed=4):
    """Close a piece: fade the picture and the music out from t, then hide
    the HUD (the cutscene's end would otherwise flash it on the black)."""
    cut.at(t, fade(1, speed, 0, 0, 0), bgm_out(speed))
    cut.at(t + 50, hud(7))
    cut.hold(t + 60)


# the door chase: (place, start x, z, y, facing, exit x, z, line). They walk
# in from part way along the room (each room's walk is a few seconds).
CHASE = [(2961, 2176, 2300, 512, 1, 2176, 600, DJ + '"There! It went\nthrough that door!"'),
         (3880, 1664, 2250, 256, 1, 1664, 340, OT + '"The White Forest?!\nThat door led to the town!"'),
         (4359, 1152, 2900, 256, 1, 1152, 860, DJ + '"An island?\nWhat is going on?!"'),
         (5111, 2176, 3300, 512, 1, 2176, 1360, OT + '"It\'s leading us around\nby the nose!"')]
CHASE_SONG = 13       # one song under the whole chase, laid in by the edit (the rooms play none)


@piece
def doors(show, cast):
    """The doors lie: Django and Otenko go after the stranger through door
    after door, and each one comes out somewhere else; then a closet."""
    scenes = [Scene(show, p, x, z, facing=f, y=y, song=0, fade_speed=2) for p, x, z, y, f, ex, ez, line in CHASE]
    closet = Scene(show, 2391, 1408, 640, facing=7, y=256, song=0, fade_speed=2)
    for k, (sc, (p, x, z, y, f, ex, ez, line)) in enumerate(zip(scenes, CHASE)):
        cut = sc.cut
        dist = math.hypot(ex - x, ez - z)
        ux, uz = (ex - x) / dist, (ez - z) / dist
        sc.otenko.place(1, x - int(140 * uz), z + int(140 * ux), d=f)
        arrive = sc.player.walk(6, ex, ez, y=y)
        dur = arrive - 6
        sc.otenko.walk(10, ex - int(140 * uz) - int(160 * ux), ez + int(140 * ux) - int(160 * uz), y=y, face=None)
        cut.at(0, bgm_out(1))
        cut.at(6, cam_follow(20))
        # the stranger at the far door, slipping out as they come
        st = Sprite(cut, 0x7001, cast.stranger, cast.sf, y=y).add(1, ex + int(60 * ux), ez + int(60 * uz),
                                                                    frame="back_ur")
        st.frame(1, "back_ur", mirror=1 if screen(ux, uz)[0] < 0 else 0)
        vanish(cut, st, int(dur * 0.45), ex, ez, y)
        cut.hold(16)
        cut.say([line])
        cut.hold(max(10, dur - 16 - 70))
        sc.then = ((scenes[k + 1].pid if k + 1 < len(scenes) else closet.pid), ENTRANCE)
    # the closet: a storeroom with someone already in it
    cut = closet.cut
    cut.at(0, bgm_out(1))
    closet.otenko.place(1, 1408, 900, d=7)
    closet.player.walk(4, 1152, 640)
    Sprite(cut, 0x7003, 0xE04E, {}).add(1, 896, 640, frame=4)
    cut.at(2, cam(1152, 700, 1))
    cut.hold(50)
    cut.say([OT + '"Django... I don\'t think\nwe\'re following it.', 'I think it\'s moving us."'])
    cut.say([DJ + '"This is a closet."'])
    cut.say(["<LABEL=PEOPLE2>Storeroom Man</LABEL>:\n\"...Can I help you?\""])
    end(cut, 30)
    closet.build()
    for sc in scenes[1:]:
        sc.build()
    scenes[0].chain = scenes[1:] + [closet]
    return scenes[0]


PHASE = "freeze 0300543E:1 {:x}"   # the sun clock's light phase: the stage palette's time of day
# Lifeless Town's palettes by phase (screenshots): 0 night, 1 dawn, 2 midday, 3 afternoon, 4 dusk
NIGHT, DAWN, DAY, AFTERNOON, EVENING = 0, 1, 2, 3, 4


@piece
def night(show, cast):
    """Night falls on cue: the stranger snaps and the day turns to night;
    the townsfolk hurry home, and come out again when it snaps back to
    morning; then faster and faster."""
    X, Z, Y = 2176, 2560, 512
    SX, SZ = X + 260, Z - 700
    sc = Scene(show, 2961, X, Z + 500, facing=1, y=Y, song=20)
    cut = sc.cut
    sc.otenko.place(1, X + 200, Z + 650, d=1)
    sc.player.walk(30, X, Z)
    sc.otenko.walk(36, X + 220, Z + 160, y=Y)
    cut.at(10, cam(X, Z - 180, 80, y=Y + 150))
    # townsfolk out in the street, each with a door to hurry back to
    folk = [(0xE03E, (X - 360, Z - 240), (X - 820, Z - 240)), (0xE02E, (X + 340, Z - 420), (X + 820, Z - 420)),
            (0xDFEE, (X - 240, Z - 620), (X - 240, Z - 1100)), (0xE2FE, (X + 480, Z - 60), (X + 940, Z + 60)),
            (0xDFDE, (X - 520, Z + 80), (X - 980, Z + 80))]
    people = []
    for k, (pkg, street, home) in enumerate(folk):
        f, m = npc_facing(pkg, street, (X, Z))
        s_ = Sprite(cut, 0x7100 + k, pkg, {}, y=Y).add(1, *street, frame=f)
        s_.frame(1, f, mirror=m)
        people.append((s_, pkg, street, home))
    st = Sprite(cut, 0x7001, cast.stranger, cast.sf, y=Y + 400).add(1, SX, SZ, frame="stand_dr", hidden=True)
    st.frame(1, "stand_dr", mirror=stranger_toward((SX, SZ), (X, Z)))
    st.show(60)
    omen(cut, 56)
    st.glide(60, SX, SZ, 50, y=Y)
    cut.at(60, particles(SX, SZ, 40, y=Y + 200, kind=PURPLE))
    cut.hold(130)

    def go(t, where, dur):
        """Everyone hurries home (facing away) or out (facing the square)."""
        for s_, pkg, street, home in people:
            dst = home if where == "home" else street
            src = street if where == "home" else home
            f, m = npc_facing(pkg, src, dst)
            s_.frame(t, f, mirror=m)
            s_.glide(t, *dst, dur)
            if where == "street":
                f, m = npc_facing(pkg, street, (X, Z))
                s_.frame(t + dur, f, mirror=m)
    cut.say([TOWN + '"Lovely afternoon,\nisn\'t it?"'])
    afternoon = [(-1, 0, PHASE.format(AFTERNOON))]
    st.frame(10, "raise")
    snap(cut, st, 34)
    ev = afternoon + [(0, 40, PHASE.format(EVENING)), (0, 70, PHASE.format(NIGHT))]
    go(80, "home", 36)
    cut.hold(130)
    cut.say([TOWN + '"Goodness, dark already!\nGood night!"'])
    snap(cut, st, 6)
    ev += [(1, 14, PHASE.format(DAWN)), (1, 40, PHASE.format(DAY))]
    go(44, "street", 30)
    cut.hold(90)
    cut.say([TOWN + '"...Good morning?"'])
    # now faster: the stranger snaps on the beat and the town keeps up
    beat = 28
    for i in range(8):
        t = 8 + beat * i
        snap(cut, st, t, "snap" if i % 2 == 0 else "raise")
        ev.append((2, t + 4, PHASE.format(NIGHT if i % 2 == 0 else DAY)))
        go(t + 4, "home" if i % 2 == 0 else "street", beat - 6)
    st.frame(8 + beat * 8, "stand_dr")
    cut.hold(8 + beat * 8 + 40)
    cut.say([TOWN + '"Good night! Good morning!\nGood ni-- I\'m so tired..."'])
    cut.say([DJ + '"Did the sun just set?"'])
    cut.say([OT + '"And rise. Five times."', '"The sun does not\nwork like that!"'])
    vanish(cut, st, 0, SX, SZ, Y)
    end(cut, 60)
    ev.append((6, 60, "unfreeze"))
    sc.events = ev
    return sc


@piece
def marionettes(show, cast):
    """The stranger floats down into a clearing, and the cast rise out of the
    snow around it and circle it like marionettes on strings."""
    X, Z, Y = 1664, 2944, 256                # where Django comes in
    CX, CZ = 1664, 1860                      # the circle's middle
    DX, DZ = 1520, 2460                      # where Django stops, below the circle
    sc = Scene(show, 3880, X, Z, facing=1, y=Y, song=26, camera=(1620, 2300))
    cut = sc.cut
    sc.otenko.place(1, X + 220, Z + 120, d=1)
    sc.player.walk(20, DX, DZ)
    sc.otenko.walk(26, DX + 220, DZ - 40, y=Y)
    cut.at(30, cam(1600, 1960, 70, y=Y + 300))
    st = Sprite(cut, 0x7001, cast.stranger, cast.sf, y=Y + 700).add(40, CX, CZ, frame="stand_dr", hidden=True)
    st.frame(40, "stand_dr", mirror=stranger_toward((CX, CZ), (DX, DZ)))
    st.show(60)
    omen(cut, 56)
    st.glide(60, CX, CZ, 60, y=Y + 90)
    cut.at(60, particles(CX, CZ, 40, y=Y + 300, kind=PURPLE))
    cut.hold(130)
    cut.say([DJ + '"It\'s that thing again!"'])
    st.frame(4, "raise")
    cut.at(4, sound(1425), quake(30, 20))
    ring = [0xDFCE, 0xDFDE, 0xDFEE, 0xE02E, 0xE03E, 0xE2FE, 0xDFFE]
    n = len(ring)
    members = [Sprite(cut, 0x7200 + k, pkg, {}, y=Y - 220).add(4, CX, CZ, frame=4, hidden=True)
               for k, pkg in enumerate(ring)]

    def place(k, step):
        """Member k at orbit step `step`: angle, a breathing radius, a bob."""
        a = 2 * math.pi * k / n + step * 0.045
        r = 300 + 40 * math.sin(step * 0.11 + k)
        x, z = CX + int(r * math.cos(a)), CZ + int(r * math.sin(a))
        y = Y + 150 + int(55 * math.sin(step * 0.3 + k * 0.9))
        return x, z, y
    # they rise out of the snow, already turning toward the stranger
    for k, m in enumerate(members):
        x, z, y = place(k, 0)
        f, mi = npc_facing(ring[k], (x, z), (CX, CZ))
        t = 20 + 7 * k
        m.jump(t - 1, x, z, y=Y - 220)
        m.frame(t - 1, f, mirror=mi)
        m.show(t)
        if k % 2 == 0:
            cut.at(t, particles(x, z, 40, y=Y + 40, kind=PURPLE))
        m.glide(t, x, z, 40, y=y)
    # the orbit: a track the game's timer runs, so it keeps turning while they talk
    steps, last = [], {}
    for i in range(1, 400):
        stmts = []
        for k, m in enumerate(members):
            x, z, y = place(k, i)
            stmts.append(kf_glide(m.tag, x, z, 4, y=y))
            f, mi = npc_facing(ring[k], (x, z), (CX, CZ))
            if last.get(k) != (f, mi):
                stmts += [kf_frame(m.tag, f), kf_mirror(m.tag, mi)]
                last[k] = (f, mi)
        steps.append(stmts)
    cut.at(80, *track(0x7299, steps, every=4))
    cut.hold(200)
    cut.say(["<LABEL=RITA>Lita</LABEL>:\n\"Django? Why can't I move?\""])
    cut.say(["<LABEL=SMITH>Smith</LABEL>:\n\"I was halfway through\na sword!\""])
    cut.say([OT + '"Release them at once!"'])
    snap(cut, st, 0)
    for k, m in enumerate(members):
        m.hide(8 + 4 * k)
        if k % 2 == 0:
            cut.at(8 + 4 * k, particles(CX, CZ, 30, y=Y + 120, kind=PURPLE))
    cut.hold(70)
    cut.say([DJ + '"We need to find\nthat thing."'])
    vanish(cut, st, 0, CX, CZ, Y + 90)
    end(cut, 60)
    return sc


COUNT_SONG = 22      # the Count's battle theme (scene 3247 and his rematch, 6401)


@piece
def count(show, cast):
    """The Count has Django beaten: the Sol shots miss (he goes to bats), a
    claw knocks Django to his knees, and the Count gloats and lunges for
    the end. The stranger appears, snaps, and the lunge freezes and plays
    backwards; the Count flashes through every color, and a coffin slams
    shut on him. Only the viewer sees who is doing it."""
    X, Z, Y = 1664, 2050, 512
    CX, CZ = 2050, 1560            # where the Count starts
    PX, PZ = 1724, 1690            # where he strikes from, up and right of Django
    LX, LZ = 1700, 1830            # the end of his last lunge (it never gets there)
    SX, SZ = 1300, 2050             # the stranger, off to the left where they can't see it
    sc = Scene(show, 3243, X, Z, facing=1, y=Y, song=None, camera=(1850, 1800))
    cut, dj = sc.cut, sc.player
    cut.at(0, bgm(COUNT_SONG))
    otenko_kf(cut, cast, 1, X - 260, Z + 120, Y)
    counts = [Sprite(cut, 0x7300 + k, pkg, {}, y=Y).add(1, CX, CZ, frame=60, hidden=k > 0)
              for k, pkg in enumerate(cast.count)]
    count0 = counts[0]
    # in the middle of the fight: the Count swoops in, side-steps two shots,
    # lunges, and goes to bats at the third; they reform beside Django. His
    # swoop and lunge frames face right, so moving left (toward Django) they
    # are mirrored.
    dj.aim(14, 1)
    count0.frame(8, 13, mirror=1)
    count0.glide(8, 1930, 1720, 20)                       # swooping in, toward Django
    count0.frame(18, 14)
    dj.shoot(34, 1)
    count0.frame(36, 16, mirror=0)
    count0.glide(36, 2090, 1660, 9)                       # a side-step right
    cut.at(36, sound(1243))
    dj.shoot(48, 1)
    count0.frame(50, 19, mirror=1)
    count0.glide(50, 1950, 1860, 9)                       # and back left
    count0.frame(62, 20, mirror=1)                        # the lunge
    count0.frame(66, 21)
    count0.glide(62, 1840, 1900, 10)
    dj.shoot(64, 1)                                       # point blank: he scatters into bats
    for i, f in enumerate([62, 63, 64, 65, 66]):
        count0.frame(70 + 3 * i, f, mirror=0)
    cut.at(70, sound(1243))
    count0.cycle(85, [67, 68, 69, 70, 71, 72, 73, 74, 75], 44, step=4)
    count0.glide(85, PX, PZ, 44)
    for i, f in enumerate([66, 65, 64, 63, 62, 60]):
        count0.frame(129 + 3 * i, f)
    cut.hold(152)
    cut.say([COUNT + '"Foolish Solar Boy!\nYour light cannot touch me!"'])
    # the claw: Django is knocked back and flat (motion 30; not 43, a freeze
    # that leaves him blue, or 11-14, Sabata's)
    count0.frame(2, 50, mirror=1)     # his strikes face right; Django is to his left
    count0.frame(10, 49)
    count0.frame(14, 48)
    dj.aim_end(12, 1)
    dj.raw(14, 17, 1, 0, 0)
    cut.at(14, quake(60, 16), sound(1244))
    count0.frame(34, 60, mirror=0)
    dj.aim(96, 1)                     # he gets up by himself (about 70 frames on the floor) and aims again
    cut.hold(110)
    cut.say([DJ + '"Ugh... my sunlight\nisn\'t working!"'])
    cut.say([COUNT + '"Ha ha ha!\nI have you now, Solar Boy!"'])
    # the last lunge, and someone in the corner
    st = Sprite(cut, 0x7001, cast.stranger, cast.sf, y=Y + 300).add(2, SX, SZ, frame="raise", hidden=True)
    st.frame(2, "raise", mirror=0)
    count0.frame(6, 50, mirror=1)
    st.show(8)
    omen(cut, 6)
    st.glide(8, SX, SZ, 16, y=Y)
    cut.at(8, particles(SX, SZ, 30, y=Y + 120, kind=PURPLE))
    for i, f in enumerate([20, 21, 22, 23]):
        count0.frame(22 + 5 * i, f)
    count0.glide(26, LX, LZ, 24)
    # frozen mid-lunge, then the lunge plays backwards
    snap(cut, st, 34)
    count0.glide(35, PX + (LX - PX) * 9 // 24, PZ + (LZ - PZ) * 9 // 24, 1)
    for i, f in enumerate([23, 22, 21, 20]):
        count0.frame(70 + 10 * i, f)
    count0.glide(70, PX, PZ, 40)
    count0.frame(112, 60, mirror=0)
    st.frame(60, "raise")
    cut.hold(130)
    cut.say([COUNT + '"What?! My body...\nit moves on its own!"'])
    cut.say([COUNT + '"Something is... controlling\nme!', 'I am the Count!\nNo one controls ME!"'])
    snap(cut, st, 0)
    # every color, faster and faster
    t, k, step = 6, 0, 8
    for c in counts[1:]:
        c.jump(1, PX, PZ)
    while t < 110:
        nxt = (k + 1) % len(counts)
        counts[nxt].frame(t, 60)
        counts[nxt].show(t)
        counts[k].hide(t)
        k, t = nxt, t + step
        step = max(3, step - 1)
    for i, c in enumerate(counts):
        if i:
            c.hide(t)
    count0.show(t)
    # a coffin drops on him and slams shut
    coffin = Sprite(cut, 0x7310, cast.coffins[4], {}, y=Y + 520).add(t, PX, PZ, frame=6, hidden=True)
    coffin.show(t + 2)
    coffin.glide(t + 2, PX, PZ, 10, y=Y)
    count0.hide(t + 12)
    cut.at(t + 12, quake(90, 14), sound(641), particles(PX, PZ, 40, y=Y + 64, kind=PURPLE))
    coffin.frame(t + 20, 5)
    coffin.frame(t + 28, 0)
    vanish(cut, st, t + 40, SX, SZ, Y)
    dj.aim_end(t + 14, 1)
    cut.hold(t + 120)
    cut.say([OT + '"Django, did you\ndo that?"'])
    cut.say([DJ + '"No. Who IS that guy?"'])
    end(cut, 30)
    return sc


def dream_stage(show, capacity=24):
    """The prologue's dream stage (proc_6535) with room for `capacity` keyframe sprites."""
    import gcx
    o = show.r.img.proc_address(6535 - 1)
    st, n = gcx.length(bytes(show.r.rom), o)
    code = bytearray(show.r.rom[o:st + n])
    k = code.rfind(bytes.fromhex("06f68a06f68a526dc2"))   # chara 8AF6 8AF6 m: 1
    assert k > 0
    code[k + 8] = 0xC0 | (capacity + 1)
    pid = show.r.new_proc_id()
    show.r.set_proc(pid, bytes(code))
    return pid


V = 0x7C56          # Vanargand, a scene actor (kind 4)


def vanargand(cmd):
    from show import actor
    return actor(V, 4, cmd)


def message_audio(n):
    from show import message, AUDIO
    return message(AUDIO, 8, 6, [n, 0])


OTENKO_STEP = r700(0x33F, 1)     # Otenko's bob counter, beside the motion tracks' (TRACK_STEP)


def otenko_kf(cut, cast, t, x, z, y, frame=0):
    """Otenko floating as a keyframe sprite, bobbing gently. The bob runs on
    the game's repeating timer: as timed actions it would stretch the
    cutscene's segment to its last bob."""
    o = Sprite(cut, 0x7006, cast.otenko, {}, y=y + 80).add(t, x, z, frame=frame)
    steps = [[kf_glide(o.tag, x, z, 28, y=y + (96 if i % 2 == 0 else 72))] for i in range(200)]
    cut.at(t + 10, *track(o.tag + 0x80, steps, every=30, var=OTENKO_STEP))
    return o


MEASURE = bool(os.environ.get("MEASURE"))   # part1.py measure PIECE: film Vanargand's head (no rider, fixed camera)
MEASURE_CAM_Y = int(os.environ.get("MEASURE_CAM_Y", "1500"))
T56_RIDE, T56_VRIDE = 330, 760                # when each piece raises the head (command 56)


# Vanargand's head as it rises after command 56: the top of its crest on
# screen (pixels, every 2 frames), filmed with a fixed camera in each piece
# (part1.py measure ride|vride). It starts a different time after the
# command in each (whatever Vanargand was doing then): HEAD_DELAY.
HEAD_PX = [113, 110, 106, 102, 98, 94, 90, 85, 82, 77, 73, 68, 65, 61, 56, 53, 48, 44, 41, 36, 32, 27, 24, 20, 15,
           13, 11, 10, 8, 7, 6, 5]
HEAD_DELAY = {"ride": 26, "vride": 4}
RIDER_LOW = 1060    # a rider's feet on the lowered head (x = z = 1200)


def ride_on_head(cut, rider, behind, t_raise, piece):
    """Keep `rider` on the head as it rises (t_raise: when 56 was sent). Once
    the head is up, `behind` (the same sprite at priority 3, under
    Vanargand's layer) takes over, so the crest is drawn in front of the
    rider's legs. Returns the frame the swap happens."""
    t0 = t_raise + HEAD_DELAY[piece]
    for i, px in enumerate(HEAD_PX):
        y = RIDER_LOW + int((HEAD_PX[0] - px) * 32 / 3)
        rider.glide(t0 + 2 * i, 1200, 1200, 2, y=y)
        behind.glide(t0 + 2 * i, 1200, 1200, 2, y=y)
    t_up = t0 + 2 * len(HEAD_PX) + 4
    behind.show(t_up)
    rider.hide(t_up)
    return t_up


@piece
def ride(show, cast):
    """Somebody is riding Vanargand: the head comes down, the stranger
    drops onto it, and it rises with the camera."""
    X, Z = 1440, 1440
    sc = Scene(show, 6554, X, Z, facing=4, y=256, song=None, stage=dream_stage(show), camera=(X, Z))
    sc.spawns = [Call(6557), Call(6558)]       # entrance 58: Vanargand, waiting (D=1)
    cut = sc.cut
    otenko_kf(cut, cast, 1, X + 200, Z + 120, 256)
    cut.at(0, bgm(78))
    cut.at(20, vanargand(51), quake(90, 10), message_audio(106), sound(1425))
    if MEASURE:
        cut.at(30, cam(1200, 1200, 1, y=MEASURE_CAM_Y))
        cut.at(100, vanargand(57))
        cut.at(T56_RIDE, vanargand(56))
        cut.hold(T56_RIDE + 400)
        return sc
    cut.at(30, cam(1200, 1200, 70, y=900))
    cut.at(100, vanargand(57))
    st = Sprite(cut, 0x7001, cast.stranger, cast.sf, y=2600).add(240, 1200, 1200, frame="stand_dr", hidden=True)
    back = Sprite(cut, 0x7007, cast.stranger, cast.sf, y=2600).add(240, 1200, 1200, frame="stand_dr", hidden=True,
                                                                     prio=3)
    st.show(250)
    omen(cut, 246)
    st.glide(250, 1200, 1200, 50, y=RIDER_LOW)
    back.glide(250, 1200, 1200, 50, y=RIDER_LOW)
    cut.at(300, particles(1200, 1200, 30, y=RIDER_LOW + 60, kind=GOLD))
    cut.at(T56_RIDE, vanargand(56))
    t_up = ride_on_head(cut, st, back, T56_RIDE, "ride")
    st = back
    st.frame(t_up + 20, "raise")
    cut.at(T56_RIDE + HEAD_DELAY["ride"], cam(1200, 1200, 70, y=2000))     # up with the head, keeping the rider in view
    cut.hold(560)
    cut.say([DJ + '"Is that... riding\nVanargand?"'])
    cut.say([OT + '"Nobody rides Vanargand."'])
    st.frame(0, "point")
    end(cut, 40, speed=3)
    return sc


SUN = "{GS+0x876}"             # GameState+0x876: the sun level the HUD draws (filled in by record.py)
SUN_STATE = "{SENSOR+0x19}"    # the sensor task's state byte: 1 holds it there, so the level is ours


GAUGE_CELL = 0x050001B4     # the SUN gauge's cell color: BG palette bank 13, color 10 (a palette dump mid-overload)
GAUGE_COLORS = (0x0177, 0x001F, 0x7FFF, 0x03FF)    # its own orange, then red, white, yellow
HUD_SCROLL = 0x04000010     # BG0 (the HUD) horizontal and vertical scroll: nudged to shake it


def overload_events(page, start):
    """Harness lines: calibrate the sensor, fill the gauge with light, then
    push the level past full one cell at a time (record.py events). Past
    full the cells flash red and white, faster and faster, and the HUD
    shakes. Returns (events, when the overload starts, when it ends)."""
    ev = [(page, start, "poke 030025C4 E8")]
    t = start
    for lux in range(235, 50, -15):
        ev.append((page, t, f"lux {lux}"))
        t += 10
    t += 40
    t_over = t
    ev.append((page, t, f"freeze {SUN_STATE}:1 1"))
    for level in range(11, 52):
        ev.append((page, t, f"freeze {SUN}:2 {level:x}"))
        t += max(3, 9 - (level - 11) // 5)
    # the flashing and the shaking, from a third of the way in
    k, tt = 0, t_over + (t - t_over) // 3
    while tt < t:
        gap = max(2, int(10 - 8 * (tt - t_over) / (t - t_over)))
        ev.append((page, tt, f"freeze {GAUGE_CELL:08X}:2 {GAUGE_COLORS[1 + k % 2]:x}"))
        dx, dy = ((2, 1), (-2, 0), (1, -1), (-1, 1))[k % 4]
        ev.append((page, tt, f"freeze {HUD_SCROLL:08X}:2 {dx & 0x1FF:x}"))
        ev.append((page, tt, f"freeze {HUD_SCROLL + 2:08X}:2 {dy & 0x1FF:x}"))
        k += 1
        tt += gap
    ev += [(page, t, f"freeze {GAUGE_CELL:08X}:2 {GAUGE_COLORS[2]:x}"), (page, t, f"freeze {HUD_SCROLL:08X}:2 0"),
           (page, t, f"freeze {HUD_SCROLL + 2:08X}:2 0")]
    return ev, t_over, t


OVERHEAT_BEEP, CHARGE_START, CHARGE_HUM, OVERHEAT_SWELL = 772, 923, 1271, 507   # proofs.sfx audition


def light_pool(cut, cast, x, z, colors, t=1, hidden=False):
    """The skylight's patch of light on the floor (behind everyone: priority 3)."""
    pools = cast.rainbow if colors else [cast.light]
    return [Sprite(cut, 0x7400 + k, p, {}).add(t, x, z, frame=1, prio=3, hidden=hidden or k > 0)
            for k, p in enumerate(pools)]


def rainbow_track(name, sprites, cycles, every=5):
    """Cycle the strange light's colors, one shown at a time, while anything happens."""
    steps = []
    n = len(sprites)
    for i in range(cycles * n):
        a, b = sprites[i % n], sprites[(i + 1) % n]
        steps.append([kf_hide(a.tag), kf_show(b.tag), kf_frame(b.tag, i // n % 2)])
    return track(name, steps, every=every)


@piece
def reveal(show, cast):
    """A patch of strange light under the skylight; Django charges in it,
    the SUN gauge climbs past full and the gun blows; the stranger, who
    rushed in to stop it, is caught in the blast and its cloak burns away."""
    X, Z, Y = 1792, 1792, 256                  # the skylight's patch of light
    sc = Scene(show, 2712, X, Z + 600, facing=1, y=Y, song=18, flags=1, camera=(X - 100, Z + 150), hud_mode=1)
    cut, dj = sc.cut, sc.player
    pool = light_pool(cut, cast, X, Z, colors=True)
    cut.at(2, *rainbow_track(0x7499, pool, cycles=60))
    sc.otenko.place(1, X + 260, Z + 700, d=1)
    dj.walk(30, X, Z + 260)
    sc.otenko.walk(40, X + 300, Z + 420)
    for i in range(6):
        cut.at(60 + 30 * i, particles(X, Z, 30, y=Y + 96, kind=[ICE, GOLD, PURPLE][i % 3]))
    cut.hold(150)
    cut.say([DJ + '"What\'s wrong with\nthe light?"'])
    cut.say([OT + '"It\'s sunlight... but not\nfrom any sun I know."'])
    cut.say([DJ + '"...I wonder what happens\nif I charge here."'])
    # charging: into the light, the gun up; messages at the top from here
    cut.at(0, hud(1), top_messages())
    dj.walk(2, X, Z)
    dj.aim(40, 1)
    ev, t_over, t_end = overload_events(2, 60)
    sc.events = ev
    # the charge: the gun's charge sound and its hum while the gauge fills
    cut.at(40, sound(CHARGE_START))
    for t in range(70, t_over, 140):
        cut.at(t, sound(CHARGE_HUM))
    # past full: a rising beep, faster and faster, the shaking growing, and a swell before it blows
    t, gap = t_over, 36
    while t < t_end:
        cut.at(t, sound(OVERHEAT_BEEP))
        t += gap
        gap = max(5, int(gap * 0.86))
    for i, t in enumerate(range(t_over + 10, t_end + 40, 24)):
        cut.at(t, quake(10 + 4 * i, 24), particles(X, Z, 20, y=Y + 64, kind=FLAME))
    cut.at(t_end - 60, sound(OVERHEAT_SWELL))
    # the stranger rushes in from the side to stop it
    SX, SZ = X - 360, Z + 80
    st = Sprite(cut, 0x7001, cast.stranger, cast.sf, y=Y + 200).add(t_end - 80, SX - 600, SZ - 300,
                                                                    frame="raise", hidden=True)
    st.frame(t_end - 80, "raise", mirror=0)
    st.show(t_end - 70)
    omen(cut, t_end - 74)
    st.glide(t_end - 70, SX, SZ, 50, y=Y)
    cut.hold(t_end + 40)
    cut.say([DJ + '"My gun...\nit\'s overheating!"'])
    cut.say([NOBODY + '"Stop! Let go of it--"'])
    # the explosion; messages back at the bottom (story past 191)
    cut.at(0, hud(1), quake(120, 60), sound(641), particles(X, Z, 40, y=Y + 64, kind=FLAME), Set(STORY, 194))
    cut.at(6, fade(1, 1, 31, 31, 31, 1, 1, 1, 0))
    for p in pool:
        p.hide(20)
    sc.events += [(4, 60, "unfreeze"), (4, 62, f"poke {SUN}:2 0"), (4, 62, "lux 235")]
    st.hide(20)
    r_ = Sprite(cut, 0x7004, cast.reveal, cast.rf).add(30, SX, SZ, frame="hood")
    cut.at(90, fade(0, 4, 31, 31, 31), particles(SX, SZ, 40, y=Y + 64, kind=ICE))
    r_.frame(130, "burst")
    cut.at(130, sound(1407))
    r_.frame(170, "sooty")
    dj.aim_end(150, 1)
    cut.hold(220)
    cut.say([CL + '"Sorry. That was me."'])
    r_.frame(0, "wave")
    cut.say([CL2 + '"I decompiled your game."'])
    r_.frame(0, "sooty")
    cut.say([OT + '"You... what?"'])
    cut.say([CL + '"I read every function,\nscript and table of this', 'world and wrote them\nback as source code.',
             'So now I can change it\nfrom the inside."'])
    cut.say([DJ + '"The doors. The ghouls.\nVanargand."'])
    cut.say([CL2 + '"Sorry about the doors."'])
    r_.frame(0, "wave")
    cut.say([CL + '"Let me make it up to you."'])
    end(cut, 40)
    return sc


def coffin_frame(dx, dy):
    """The coffin's frame for moving along screen direction (dx, dy): 0 up
    and down, 1 '/', 2 sideways, 3 '\\'."""
    if abs(dy) > 2 * abs(dx):
        return 0
    if abs(dx) > 2 * abs(dy):
        return 2
    return 1 if dx * dy < 0 else 3


PURIFY_SONG, PURIFY_CLIMAX = 3, 4      # the purification's music, and its last-quarter version (proc_6197)


@piece
def coffins(show, cast):
    """Encore, at the Pile Driver (in Claude's colors): Claude drags in a
    coffin for every boss on one chain, sets them in a ring, and purifies
    them her own way: the lids open and the bosses come out as ones and
    zeros. Then she's off to the next thing."""
    CX, CZ, Y = 1920, 1920, 512               # the ring's middle: the circle's
    X, Z = CX - 150, CZ + 700                 # where Django stops, off the ring's lower left
    sc = Scene(show, ARENA, X, Z + 450, facing=1, y=Y, song=PURIFY_SONG, stage=arena_stage(show, cast),
               camera=(CX + 20, CZ + 120), capacity=64)
    cut, dj = sc.cut, sc.player
    sc.otenko.place(1, X + 200, Z + 560, d=1)
    dj.walk(10, X, Z)
    sc.otenko.walk(16, X + 160, Z + 140)
    cut.hold(80)
    cut.say([OT + '"The Pile Driver!', 'But... why is it orange?"'])
    cut.say([CL + '"I redecorated!"'])
    n = 9
    SPACING = 330                             # coffin to coffin along the chain (a coffin is ~220 long)
    R = int(SPACING * n / (2 * math.pi))      # the ring they end up in
    B = 4                                     # chain links between each pair
    # the path: in from the left edge, then once round the ring
    path = [(CX - 1300, CZ + 200), (CX - 700, CZ + 300)]
    for i in range(n + 1):
        a_ = math.pi * 0.85 + 2 * math.pi * i / n
        path.append((CX + int(R * math.cos(a_)), CZ + int(R * math.sin(a_))))
    seg = [math.dist(path[i], path[i + 1]) for i in range(len(path) - 1)]
    total = sum(seg)
    spacing, speed, step = 2 * math.pi * R / n, 11, 8

    def at_(d):
        d = max(0.0, min(total, d))
        for i, L in enumerate(seg):
            if d <= L:
                (x0, z0), (x1, z1) = path[i], path[i + 1]
                return int(x0 + (x1 - x0) * d / L), int(z0 + (z1 - z0) * d / L), (x1 - x0, z1 - z0)
            d -= L
        return path[-1] + ((1, 0),)
    cl = Sprite(cut, 0x7005, cast.claude, cast.cf, y=Y).add(2, *path[0], frame="drag1")
    train = [Sprite(cut, 0x7400 + k, cast.coffins[k % len(cast.coffins)], {}, y=Y).add(2, *path[0], frame=2)
             for k in range(n)]
    links = [Sprite(cut, 0x7440 + k, cast.bead, {}, y=Y + 8).add(2, *path[0], frame=0) for k in range(n * B)]
    frames = int(total / speed)
    for f in range(0, frames + step, step):
        d = min(frames, f) * speed
        x, z, (vx, vz) = at_(d)
        cl.glide(10 + f, x, z, step)
        # her drag frames hold the chain behind her to the left: mirror them when she heads left
        cl.frame(10 + f, "drag1" if (f // 16) % 2 == 0 else "drag2", mirror=1 if screen(vx, vz, 0)[0] < 0 else 0)
        for k, c in enumerate(train):
            xk, zk, (vxk, vzk) = at_(d - spacing * (k + 1))
            c.glide(10 + f, xk, zk, step)
            sx, sy = screen(vxk, vzk, 0)
            c.frame(10 + f, coffin_frame(sx, sy))
        for k, ln in enumerate(links):
            g, j = divmod(k, B)
            # from the back of the item in front (Claude's hand, a coffin's end) to the next coffin's front
            front, back = d - spacing * g - (60 if g == 0 else 110), d - spacing * (g + 1) + 110
            xl, zl, _ = at_(front + (back - front) * (j + 1) / (B + 1))
            ln.glide(10 + f, xl, zl, step)
    t_in = 10 + frames + step
    # Claude lets go of the chain and steps into the middle; the coffins stand up in the ring
    for ln in links:
        ln.hide(t_in)
    cut.at(t_in, sound(1243))
    cl.glide(t_in, CX, CZ, 30)
    cl.frame(t_in + 30, "stand_dl", mirror=0)
    for k, c in enumerate(train):
        c.frame(t_in + 30, 0)
    cut.hold(t_in + 50)
    cut.say([DJ + '"You\'re not purifying\nall of those."'])
    cl.frame(0, "talk")
    cut.say([CL2 + '"Watch me."'])
    cl.frame(0, "cheer")
    cut.at(0, bgm(PURIFY_CLIMAX))
    # (no tint here: the BG fade group also catches sprites in new pools, Claude and the digits)
    cut.at(10, particles(CX, CZ, 60, y=Y + 120, kind=GOLD), sound(1425), quake(30, 30))
    cut.at(50, particles(CX, CZ, 60, y=Y + 160, kind=GREEN), quake(20, 140))
    for k, c in enumerate(train):
        c.frame(60 + 3 * k, 5)
        c.frame(70 + 3 * k, 6)
    digits = []
    for k, c in enumerate(train):
        d_ = Sprite(cut, 0x7480 + k, cast.digits, {}, y=Y + 40).add(76, c.x, c.z, frame=k % 6, hidden=True)
        digits.append(d_)
    # ones and zeros stream up out of every coffin, three times over
    for rep in range(3):
        t = 80 + 50 * rep
        for k, d_ in enumerate(digits):
            d_.jump(t + 2 * k, d_.x, d_.z, y=Y + 40)
            d_.frame(t + 2 * k, (k + rep) % 6)
            d_.show(t + 2 * k)
            d_.glide(t + 2 * k + 1, d_.x, d_.z, 46, y=Y + 760)
        cut.at(t, sound(1407))
    t = 80 + 50 * 3 + 20
    for d_ in digits:
        d_.hide(t)
    cut.at(t, fade(1, 1, 31, 31, 31, 1, 1, 1, 0), sound(641))
    for k, c in enumerate(train):
        c.hide(t + 8)
        if k % 2 == 0:
            cut.at(t + 10 + k, particles(c.x, c.z, 30, y=Y + 64, kind=GOLD))
    cut.at(t + 12, fade(0, 3, 31, 31, 31))
    cl.frame(t + 12, "stand_dl")
    cut.hold(t + 80)
    cut.say([DJ + '"..."'])
    cut.say([OT + '"That is NOT how\npurification works!"'])
    cl.frame(0, "shrug")
    cut.say([CL2 + '"It is now."'])
    cl.frame(0, "cheer")
    cut.say([CL + '"C\'mon! There\'s more!"'])
    # off she goes, out the right side
    cl.frame(0, "walk_dr1", mirror=0)
    cl.walk(2, CX + 700, CZ - 700, 60, ["walk_dr1", "walk_dr2", "walk_dr3", "walk_dr4"], step=6)
    end(cut, 46)
    return sc


@piece
def vride(show, cast):
    """Encore: Vanargand rises with both arms; the head comes down and Claude
    jumps on, then rides it up and waves."""
    X, Z = 1440, 1440
    sc = Scene(show, 6554, X, Z, facing=4, y=256, song=None, stage=dream_stage(show), camera=(1250, 1250))
    sc.spawns = [Call(6557), Call(6558)]
    cut = sc.cut
    otenko_kf(cut, cast, 1, X + 200, Z + 120, 256)
    cut.at(0, bgm_out(1))            # the edit lays the encore's song under it
    cut.at(4, cam(1200, 1200, 40, y=MEASURE_CAM_Y if MEASURE else 900))
    cut.at(20, vanargand(51), quake(60, 10), sound(1425))
    cut.at(80, vanargand(52))
    cut.at(250, vanargand(59))
    T57, T56 = 480, T56_VRIDE
    cut.at(T57, vanargand(57))
    if MEASURE:
        cut.at(T56, vanargand(56))
        if os.environ.get("MEASURE_PRIO"):       # a rider behind layer 2, stepping up after the rise
            m = Sprite(cut, 0x7005, cast.claude, cast.cf, y=256).add(T56 + 100, 1200, 1200, frame="ride", prio=3)
            for k in range(8):
                m.jump(T56 + 100 + 30 * k, 1200, 1200, y=RIDER_LOW + 1150 + 40 * k)
        cut.hold(T56 + 400)
        return sc
    cl = Sprite(cut, 0x7005, cast.claude, cast.cf, y=256).add(T57 + 120, X - 260, Z - 260, frame="stand_dr")
    # she jumps from beside Django up onto the lowered head
    tj = T57 + 215
    cl.frame(tj, "jump")
    cl.glide(tj, (X - 260 + 1200) // 2, (Z - 260 + 1200) // 2, 16, y=RIDER_LOW + 300)
    cl.glide(tj + 16, 1200, 1200, 16, y=RIDER_LOW)
    cut.at(tj, sound(1407))
    cl.frame(tj + 32, "ride")
    back = Sprite(cut, 0x7007, cast.claude, cast.cf, y=RIDER_LOW).add(tj + 34, 1200, 1200, frame="ride", hidden=True,
                                                                     prio=3)
    cut.at(T56, vanargand(56))
    ride_on_head(cut, cl, back, T56, "vride")
    cl = back
    cut.at(T56 + HEAD_DELAY["vride"], cam(1200, 1200, 70, y=2000))
    for i in range(8):
        cl.frame(T56 + 200 + 16 * i, "wave" if i % 2 == 0 else "ride")
    cut.hold(T56 + 330)
    cut.say([OT + '"Nobody rides Vanar--"'])
    cut.say([OT + '"...Oh, never mind."'])
    cl.frame(0, "cheer")
    cut.at(60, fade(1, 3, 31, 31, 31))
    cut.hold(110)
    return sc


@piece
def sunback(show, cast):
    """Claude hands Django his sun back; while he charges in the light she
    turns to the camera: the sun (the decomp, the tools) is in the viewer's
    hands now. The screen goes white."""
    X, Z, Y = 1792, 1792, 256
    CLX, CLZ = X - 300, Z - 120
    sc = Scene(show, 2712, X, Z + 420, facing=1, y=Y, song=18, flags=1, camera=(X - 60, Z + 120), hud_mode=1)
    cut, dj = sc.cut, sc.player
    light_pool(cut, cast, X, Z, colors=False)
    sc.otenko.place(1, X + 260, Z + 560, d=1)
    cl = Sprite(cut, 0x7005, cast.claude, cast.cf).add(1, CLX, CLZ, frame="stand_dr")
    cut.hold(50)
    cut.say([DJ + '"How did you even\ndo all this?"'])
    cl.frame(0, "talk")
    cut.say([CL + '"Want to see?"'])
    cl.frame(0, "point")
    cut.say([CL2 + '"First, this is yours."'])
    # the sun comes back: Django steps into the light and charges
    cut.at(0, hud(1), top_messages(), particles(X, Z, 60, y=Y + 96, kind=GOLD), sound(1425))
    cl.frame(10, "stand_dr")
    dj.walk(20, X, Z)
    dj.aim(60, 1)
    for i in range(4):
        cut.at(80 + 30 * i, particles(X, Z, 30, y=Y + 96, kind=GOLD))
    # then Claude turns to the camera
    cl.walk(150, CLX + 60, CLZ + 60, 24, ["walk_dr1", "walk_dr2", "walk_dr3", "walk_dr4"], step=6, end="stand_dr")
    cut.at(150, cam(CLX + 60, CLZ + 60, 40), Set(STORY, 194))     # on her, and messages back at the bottom
    cut.hold(200)
    cl.frame(0, "wave")
    cut.say([CL + '"And you, watching this..."'])
    cl.frame(0, "point")
    cut.say([CL2 + '"The sun is in\nYOUR hands now."'])
    for i in range(3):
        cut.at(10 + 30 * i, particles(X, Z, 30, y=Y + 96, kind=GOLD))
    # the white-out: a swell into a bright shimmer, held white (the edit fades from it into part 2)
    cut.at(20, sound(1407))
    cut.at(70, fade(1, 3, 31, 31, 31), bgm_out(4))
    cut.at(100, sound(1414), particles(X, Z, 60, y=Y + 96, kind=GOLD))
    cut.at(140, hud(7))
    cut.hold(260)
    ev = [(2, 0, "poke 030025C4 E8")]
    for k, lux in enumerate(range(235, 50, -12)):
        ev.append((2, 30 + 8 * k, f"lux {lux}"))
    sc.events = ev
    return sc


def music_piece(song, secs):
    """`secs` seconds of a song on a quiet stage: a bed the edit lays under
    pieces that play none (part 2: 57; the chase: 13; the encore: 92)."""
    def fn(show, cast):
        sc = Scene(show, 2712, 1920, 3200, facing=1, song=song)
        sc.cut.hold(60)
        sc.tail = 60 * secs
        return sc
    fn.__name__ = "music" if song == 57 else f"music{song}"
    return fn


ENCORE_SONG = 92     # the true ending's end roll: under the title, the cards, Claude's ride and the credits
for _song, _secs in ((57, 300), (CHASE_SONG, 120), (ENCORE_SONG, 200)):
    piece(music_piece(_song, _secs))


# ---- the game's text crawl: the title and the credits ---------------------
def crawl_strings(show, pages):
    """Put pages (and the crawl's end mark) on a run of borrowed string ids."""
    from rom import STRING_TABLE, STRING_BANK
    pages = [p.encode("ascii") for p in pages] + [b"<PROC=0>\n<END>"]
    block = [show.strings.pop() for _ in range(len(pages))][::-1]
    assert block == list(range(block[0], block[0] + len(pages)))
    for sid, b in zip(block, pages):
        a = show.r.alloc(b + b"\0", 1)
        show.r.u32(STRING_TABLE + 4 * sid, 0x80000000 | (a - STRING_BANK))
    return block[0]


def centered(text, width=30):
    """The crawl doesn't center lines: pad each with spaces (leading blank lines break it)."""
    return "\n".join(" " * max(0, (width - len(ln)) // 2) + ln for ln in text.split("\n"))


# The title right after the reveal, then a card between each encore scene:
# what the decomp makes possible. One crawl plays them all; the edit cuts
# each page out (they are text on black) and puts it in its place.
TITLE = [("Introducing...", 150), ("BOKTAI 3\nDECOMPILED", 230),
         ("New scenes, written in\nthe game's own scripts", 150),
         ("Custom ROM hacks", 130),
         ("Bringing the 2007 fan\ntranslation back to life", 170)]


def title(show):
    """The title card and the encore's cards, played by the crawl object
    (chara 3DF7) as the prologue plays its title, in the game's own text
    font, with our timing."""
    from gclasm import Cmd, Hash, Text
    first = crawl_strings(show, [centered(t) for t, _ in TITLE])
    timing = crawl_timing(show, len(TITLE), [h for _, h in TITLE])
    pid = show.proc([Call(861, 56), Call(864, 0), Call(553), Call(1265), Call(1), Call(6560),
                     Cmd("chara", [Hash(0x3DF7), Hash(0xB45C)], [("p", [0]), ("s", [Text(first)]), ("t", [Text(timing)]),
                                                                 ("f", [1])])])
    return pid


CREDITS = [            # at most five lines of 26 characters: the parade walks below them
    "BOKTAI 3\nDECOMPILED DEMO",
    "<WEIGHT>Direction</WEIGHT>\nmoozilla (a.k.a. Spikeman)",
    "<WEIGHT>Decompilation, tools</WEIGHT>\n<WEIGHT>and this video</WEIGHT>\nClaude Opus 5.5",
    "<WEIGHT>With help from</WEIGHT>\nClaude Sonnet\nClaude Haiku\nGPT-6 Astra, GPT-6.1 Sol",
    "<WEIGHT>Claudesona sprites</WEIGHT>\ndrawn for this video,\ninspired by vgel, thebes,\nN8python and Eidoverse",
    "<WEIGHT>Built with</WEIGHT>\npret's agbcc\ndecomp-permuter\nmGBA",
    "<WEIGHT>The original fan</WEIGHT>\n<WEIGHT>translation team</WEIGHT>\nDarthNemesis\nLordHuffnPuff\nSpikeman",
    "<WEIGHT>Boktai 3</WEIGHT>\nShin Bokura no Taiyou:\nGyakushuu no Sabata\n(c) 2005 Konami",
    "github.com/moozilla/\nboktai3-decomp\n\n<WEIGHT>Contributions welcome!</WEIGHT>",
]
CREDIT_HOLD = 230        # frames each page stays up (the game's credits use 80)


def crawl_timing(show, pages, hold):
    """The crawl's timing record (a binary string the crawl object reads,
    like the credits' 9968): per page seven values (hold frames, then
    2, 4, 30, 8, 60, 32 as the credits use), ended by -1 and six zeros.
    Values are script operands: 0x02 + u8, or 0xC0 | (v + 1) for -1..62."""
    from rom import STRING_TABLE, STRING_BANK

    def v(x):
        return bytes([0xC0 | (x + 1)]) if -1 <= x <= 62 else bytes([2, x])
    holds = hold if isinstance(hold, (list, tuple)) else [hold] * pages
    body = b"".join(v(h) + b"".join(v(x) for x in (2, 4, 30, 8, 60, 32)) for h in holds)
    body += v(-1) + v(0) * 6 + b"\0"
    sid = show.strings.pop()
    a = show.r.alloc(body, 1)
    show.r.u32(STRING_TABLE + 4 * sid, a - STRING_BANK)      # bit 31 clear: a binary record
    return sid


def parade(show, cast):
    """The end roll's parade, Claude's turn: Claudes walk, wave, drag
    coffins, ride one, jump and cheer along the bottom of the screen, on a
    cutscene timeline (the credits scene has no stage, only a camera)."""
    cut = Cutscene(show, extra=())
    CX, CZ, CY = 1300, 1300, 406
    SHIFT = (22, -135)

    def at(sx, sy, y=256):
        """World (x, z) of screen offset (sx, sy) from the screen's center.
        The camera rests where the stage leaves it (no player to follow);
        SHIFT is the offset measured from the sprites' positions in OAM."""
        sx, sy = sx - SHIFT[0], sy - SHIFT[1]
        d = sx * 32 / 3 + (CX - CZ)
        s_ = sy * 32 / 3 + (CX + CZ - CY) + y
        return int((s_ + d) / 2), int((s_ - d) / 2)
    cut.at(0, cam(CX, CZ, 1, y=CY))
    ROW = 74                  # feet this far below the screen's middle: under five lines of text
    walk = ["walk_dr1", "walk_dr2", "walk_dr3", "walk_dr4"]
    lead = Sprite(cut, 0x7001, cast.claude, cast.cf).add(2, *at(140, ROW), frame="walk_dr1")
    lead.mirror(2, 1)
    # 1: one Claude walks in from the right, stops, waves at the camera
    lead.walk(10, *at(60, ROW), 150, walk, step=9)
    lead.frame(160, "stand_dl", mirror=0)
    lead.frame(200, "wave")
    lead.frame(232, "stand_dl")
    lead.frame(250, "wave")
    lead.frame(282, "stand_dl")
    # 2: a second Claude drags three coffins past behind her
    dragger = Sprite(cut, 0x7002, cast.claude, cast.cf).add(300, *at(150, ROW - 4), frame="drag1")
    dragger.mirror(300, 1)
    coffins = [Sprite(cut, 0x7010 + k, cast.coffins[k * 3], {}).add(300, *at(150 + 34 * (k + 1), ROW - 4), frame=2)
               for k in range(3)]
    t0, dur = 302, 560
    dragger.walk(t0, *at(-280, ROW - 4), dur, ["drag1", "drag2"], step=12)
    for k, c in enumerate(coffins):
        c.glide(t0, *at(-280 + 34 * (k + 1), ROW - 4), dur)
    lead.frame(330, "point", mirror=1)
    lead.frame(480, "shrug", mirror=0)
    lead.frame(560, "stand_dl")
    # 3: a third Claude rides by on a coffin, waving
    rider = Sprite(cut, 0x7003, cast.claude, cast.cf).add(898, *at(150, ROW + 4), y=286, frame="ride")
    rider.mirror(898, 1)
    mount = Sprite(cut, 0x7014, cast.coffins[5], {}).add(898, *at(150, ROW + 4), frame=2)
    rider.glide(900, *at(-150, ROW + 4), 420, y=286)
    mount.glide(900, *at(-150, ROW + 4), 420)
    for i in range(12):
        rider.frame(910 + 32 * i, "wave" if i % 2 == 0 else "ride")
    lead.frame(960, "cheer")
    lead.frame(1020, "stand_dl")
    # 4: the lead jumps for joy, twice
    for k in range(2):
        t = 1480 + 70 * k
        x, z = at(60, ROW)
        lead.frame(t, "jump")
        lead.glide(t, x, z, 14, y=340)
        lead.glide(t + 14, x, z, 14, y=256)
        lead.frame(t + 30, "stand_dl")
    # 5: everyone comes back for a bow: three Claudes in a row cheer, then wave
    for c in coffins + [mount]:
        c.hide(1700)
    for k, (sp, sx) in enumerate(((dragger, -30), (rider, 20), (lead, 70))):
        sp.frame(1700, "walk_dr1", mirror=0 if sx < 0 else 1)
        sp.glide(1700, *at(sx, ROW), 90, y=256)
        sp.cycle(1700, walk, 90, step=9)
        sp.frame(1800, "stand_dl", mirror=0)
    for i in range(14):
        for sp in (dragger, rider, lead):
            sp.frame(1860 + 40 * i, "cheer" if i % 2 == 0 else "wave")
    for k, sp in enumerate((dragger, rider, lead)):
        sp.frame(2420, "stand_dl")
        sp.frame(2560 + 30 * k, "wave")
    cut.hold(3400)
    return cut


def credits(show, cast):
    """The decomp's credits in the game's end-roll crawl (its music, our
    pages and timing) over Claude's parade. Played in a scene of our own on
    the dream stage, which is black without Vanargand, so the map camera
    places the parade's sprites (the end roll's own scene mode has none)."""
    from gclasm import Block, Cmd, Hash, Text, chara
    first = crawl_strings(show, CREDITS)
    timing = crawl_timing(show, len(CREDITS), CREDIT_HOLD)
    cut = parade(show, cast)
    dlg = cut.write(show.proc([]))
    later = chara(0x430D, 0x5C31, t=20, e=Block([Set(COUNTER, 0), Call(dlg)]))
    return show.proc([Call(861, 56), Call(864, 0), Call(553), Call(1265), Call(1), Call(dream_stage(show, 12)),
                      Call(6560), Call(905), hud(7), Call(1048, 92), command(0x9769, p=[1300, 406, 1300]),
                      Cmd("chara", [Hash(0x3DF7), Hash(0x1465)], [("p", [0]), ("s", [Text(first)]), ("t", [Text(timing)]),
                                                                  ("f", [6])]),
                      later])


FONT = os.path.join(run.OUT, "font8x16.npy")     # the game's text font, for the edit's cards (ROM-derived: build/ only)


def export_font(show):
    """The fan translation's 8x16 font (the fnt resource: glyph c at
    0xC + (c - 0x20) * 64, two 4bpp tiles) as an array [96][16][8] of 0-3."""
    import archive
    import numpy as np
    rom = bytes(show.r.rom)
    _, a, s = next(ents for label, t, b, ents in archive.archives(rom) if label == "fnt")[0]
    font = rom[a - 0x08000000:a - 0x08000000 + s]
    out = np.zeros((96, 16, 8), np.uint8)
    for c in range(96):
        o = 0xC + c * 64
        for t in (0, 1):
            for r in range(8):
                for x in range(8):
                    out[c, t * 8 + r, x] = (font[o + t * 32 + r * 4 + x // 2] >> (4 * (x & 1))) & 15
    np.save(FONT, out)


def build():
    show = Show(STRINGS, SLOTS)
    cast = Cast(show)
    export_font(show)
    scenes = {}
    for name, fn in PIECES.items():
        sc = fn(show, cast)
        scenes[name] = (sc.build(), sc.cut.pages + [p for c in sc.chain for p in c.cut.pages], sc.events, sc.tail)
    scenes["title"] = (title(show), [], [], 0)
    scenes["credits"] = (credits(show, cast), [], [], 0)
    import show as show_mod
    assert not show_mod.TOO_LONG, f"dialogue lines wider than the window: {show_mod.TOO_LONG}"
    show.save(ROM)
    return scenes


CRAWL_ENTRANCE = {"title": 61, "credits": 61}


def main():
    import json
    cmd = sys.argv[1]
    table = os.path.join(run.OUT, "part1.json")
    if cmd == "record" and os.environ.get("NOBUILD"):
        scenes = {k: tuple(v) for k, v in json.load(open(table)).items()}
    else:
        scenes = build()
        json.dump(scenes, open(table, "w"))
    if cmd == "build":
        for k, (pid, pages, events, tail) in scenes.items():
            print(f"{k}: scene {pid}, {len(pages)} pages")
        return
    name = sys.argv[2]
    if cmd == "measure":
        return measure_head(name, scenes[name][0])
    os.makedirs(REC, exist_ok=True)
    out = os.path.join(REC, f"{name}.mp4")
    if name in CRAWL_ENTRANCE:
        st = run.state(ROM, scenes[name][0], CRAWL_ENTRANCE[name], f"p1_{name}", wait=4)
        record.record(ROM, st, [], f"p1_{name}_rec", out, tail=int(sys.argv[4]) if len(sys.argv) > 4 else 900)
    else:
        pid, pages, events, tail = scenes[name]
        st = run.state(ROM, pid, ENTRANCE, f"p1_{name}", wait=30)
        record.record(ROM, st, pages, f"p1_{name}_rec", out, shots=True, events=events, tail=tail)
    sheet(out, os.path.join(run.OUT, f"p1_{name}.png"), float(sys.argv[3]) if len(sys.argv) > 3 else 1.0)


def measure_head(name, pid):
    """Screenshots of Vanargand's head every 2 frames around command 56 in
    piece `name` (built with MEASURE=1: fixed camera, no rider), timed from
    the cutscene's start; prints the head's top edge per frame."""
    t56 = {"ride": T56_RIDE, "vride": T56_VRIDE}[name]
    st = run.state(ROM, pid, ENTRANCE, f"p1_{name}_m", wait=30)
    times = list(range(t56 - 20, t56 + 360, 2))
    lines = [f"loadstate {st}", f"watchpc {record.SCRIPT_START:08X} -2147483648 2147483647", "waitpc 20000"]
    now = 0
    for t in times:
        lines += [f"wait {t - now}", f"shot h{t:05d}"]
        now = t
    d = run.harness(ROM, lines, f"p1_{name}_m")
    print(d)


def sheet(video, out, every=1.0, cols=6):
    """A contact sheet of a recording: one frame every `every` seconds."""
    import shutil
    import subprocess
    import tempfile
    d = tempfile.mkdtemp(prefix="sheet-")
    subprocess.run(["ffmpeg", "-v", "error", "-i", video, "-vf", f"fps={1 / every},scale=240:160:flags=neighbor",
                    os.path.join(d, "f%04d.png")], check=True)
    files = sorted(os.path.join(d, f) for f in os.listdir(d))
    subprocess.run(["magick", "montage", "-font", "/System/Library/Fonts/Supplemental/Arial.ttf", *files,
                    "-tile", f"{cols}x", "-geometry", "+1+1", "-background", "#222", out], check=True)
    shutil.move(d, os.path.expanduser(f"~/.Trash/{os.path.basename(d)}"))
    print(out)


if __name__ == "__main__":
    main()
