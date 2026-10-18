#!/usr/bin/env python3
"""Showcase v3, part 1: "The Stranger" (docs/showcase/V3_PLAN.md).

  python3 tools/showcase/v3/part1.py build              -> build/showcase/v3/part1.gba
  python3 tools/showcase/v3/part1.py shots PIECE [N]    screenshots every N frames
  python3 tools/showcase/v3/part1.py record PIECE...    -> build/showcase/v3/rec/PIECE.mp4

One ROM holds every set piece; each is its own scene (or chain of scenes)
on a real stage, started by warping to it, and recorded on its own.
Everything is played by the game: new scripts and cutscene tables
(show.py), keyframe sprites from new sprite packages and palette pools
(romx.py, art.py), the player, Otenko, Vanargand, the camera, the screen
and the sound through their cutscene receivers.
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
from show import (COUNTER, DJ, OT, STORY, Call, Cutscene, Proc, Set, Show, bgm, bgm_out, cam, command, fade, hud, top_messages,  # noqa: E402
                  keyframe_manager, particles, quake, sound, stage_of)
from rom import big_sprite_frame_pixels  # noqa: E402

CL = "<LABEL=GARIGARIKUN>Claude</LABEL>:\n"      # portraits drawn over two speakers the video never shows
CL2 = "<LABEL=ROCKMAN>Claude</LABEL>:\n"
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
        self.otenko = Otenko(self.cut)
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
class Cast:
    """The packages every piece shares, built once per ROM."""

    def __init__(self, show):
        self.show = show
        self.stranger, self.sf, self.spool = art.package(show, 0x6000, "stranger")
        frames, origin, pool = romx.big_frames(show.r.rom, 0x9247)
        self.ghoul_frames = frames
        base = romx.package(frames, pool, origin)
        self.ghoul = show.package(0x6010, base)
        # the fan: ten hues, each twice (13 OBJ palettes are free at most)
        import proofs
        pals = proofs.hue_variants(romx.pool_colors(show.r.rom, pool), 10, [7, 8, 9, 10, 11])
        self.fan = [show.package(0x6020 + k, romx.repool(base, show.pool(p))) for k, p in enumerate(pals)]
        self.claude, self.cf, self.cpool = art.package(show, 0x6001, "claude")
        self.reveal, self.rf, _ = art.package(show, 0x6002, "reveal")
        # the stranger as a silhouette (for Vanargand's back)
        black = {c: (24, 16, 32) for c in art.palette(art.load("stranger")[1])}
        self.shadow, _, _ = art.package(show, 0x6003, "stranger", recolor=black)
        # the Count (F761) in six more palettes: headers aliasing the game's package
        cols = romx.pool_colors(show.r.rom, 821)
        cpals = proofs.hue_variants(cols, 7, range(1, 16))[1:]
        self.count = [0xF761] + [show.alias(0x6030 + k, 0xF761, show.pool(c)) for k, c in enumerate(cpals)]
        # the coffin (big sprite E0C9) as a package, in a palette per boss
        frames, origin, pool = romx.big_frames(show.r.rom, 0xE0C9)
        cbase = romx.package(frames, pool, origin)
        cpals = proofs.hue_variants(romx.pool_colors(show.r.rom, pool), 8, range(1, 16))
        self.coffins = [show.package(0x6040 + k, romx.repool(cbase, show.pool(c))) for k, c in enumerate(cpals)]
        # Otenko as a keyframe sprite, for stages that never load his NPC sprite
        frames, origin, pool = romx.big_frames(show.r.rom, 0x97D3)
        self.otenko = show.package(0x6050, romx.package(frames, pool, origin))
        portrait(show, "claude_neutral", 0x593B, 14, 506)
        portrait(show, "claude_smug", 0x593B, 8, 504)


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


# ---- the pieces ----------------------------------------------------------
@piece
def cold(show, cast):
    """Cold open: a ghoul fans out into twenty in ten palettes, snaps back."""
    X, SZ, GZ, DZ = 1408, 1520, 2060, 2380     # the stranger, where the ghoul stops, Django
    sc = Scene(show, 2803, X, 3150, facing=1, song=None, camera=(X, 2650))
    cut, dj, ot = sc.cut, sc.player, sc.otenko
    cut.at(0, bgm(18))
    ot.place(2, X + 230, 3250, d=1)
    dj.walk(40, X, DZ)
    ot.walk(24, X + 200, DZ + 140)
    cut.at(20, cam(X, 2020, 140))
    stranger = Sprite(cut, 0x7001, cast.stranger, cast.sf).add(1, X, SZ, frame="stand_dr")
    ghoul = Sprite(cut, 0x7002, cast.ghoul, {}).add(1, X, 1720, frame=0)
    ghoul.mirror(1)
    t = ghoul.walk(40, X, GZ, 130, [0, 1, 2, 3], step=9, end=0)
    stranger.frame(t + 10, "raise")
    cut.at(t + 10, sound(1407))
    # the fan: copies slide out of the ghoul one after another, ten hues twice
    t0 = t + 34
    n = 20
    copies = []
    for k in range(n):
        s_ = Sprite(cut, 0x7010 + k, cast.fan[k % 10], {}).add(t0, X, GZ, frame=0)
        s_.mirror(t0)
        copies.append(s_)
    for k, s_ in enumerate(copies):
        a = math.radians(-78 + 156 * k / (n - 1))
        dx, dz, dy = screen_to_world(86 * math.sin(a), 48 * math.cos(a))
        s_.glide(t0 + 2 + k, X + dx, GZ + dz, 20, y=256 + dy)
    cut.at(t0 + 2, sound(1425), quake(20, 10))
    stranger.frame(t0 + 36, "snap")
    cut.at(t0 + 36, sound(1407))
    # the copies cycle through their walk frames while they hang there
    for k, s_ in enumerate(copies):
        s_.cycle(t0 + 24 + k % 4, [0, 1, 2, 3], 80, step=10)
    tb = t0 + 120
    for s_ in copies:
        s_.glide(tb, X, GZ, 8, y=256)
        s_.frame(tb, 0)
    cut.at(tb, sound(1407))
    for s_ in copies:
        s_.hide(tb + 9)
    stranger.frame(tb + 12, "stand_dr")
    # the ghoul looks itself over
    ghoul.frame(tb + 30, 8)
    ghoul.mirror(tb + 60, 0)
    ghoul.frame(tb + 60, 9)
    ghoul.mirror(tb + 92, 1)
    ghoul.frame(tb + 92, 8)
    cut.hold(tb + 120)
    cut.say([DJ + '"..."'])
    cut.say([OT + '"Django. Who is that?"'])
    stranger.frame(0, "turn")
    stranger.frame(24, "vanish1")
    cut.at(24, particles(X, SZ, 30, kind=GOLD), sound(1407))
    stranger.frame(30, "vanish2")
    stranger.frame(36, "vanish3")
    stranger.frame(42, "vanish4")
    stranger.hide(50)
    cut.hold(100)
    return sc


# facing frames of the NPC packages: back, back 3/4, side, front 3/4, front
STANDARD = (0, 1, 2, 3, 4)
NPC = {0xDFCE: STANDARD, 0xDFDE: STANDARD, 0xDFEE: STANDARD, 0xE02E: STANDARD, 0xE03E: STANDARD,
       0xE2FE: STANDARD, 0xDFFE: (4, 3, 2, 1, 0)}
FORWARD = {1: (0, -1), 3: (1, 0), 5: (0, 1), 7: (-1, 0)}     # entrance facing -> the way into the room


def facing(pkg, dx, dy):
    """(frame, mirror) for an NPC package to face screen direction (dx, dy)."""
    order = NPC[pkg]
    a = math.degrees(math.atan2(dx, -dy)) % 360       # 0 up, 90 right, 180 down
    left = a > 180
    if left:
        a = 360 - a
    side = 0 if left else 1        # the side frames face left; mirror them to face right
    k = min(4, int((a + 22.5) // 45))                    # 0 up .. 4 down
    return order[k], side


def screen(x, z, y=256):
    return 3 * (x - z) / 32, 3 * (x + z - y) / 32


@piece
def doors(show, cast):
    """The doors lie: every door after the stranger leads somewhere else,
    faster and faster, ending in a closet."""
    stops = [(2961, 2176, 3200, 512, 1, 20, 110), (3373, 2944, 4224, 512, 1, 23, 92),
             (3880, 1664, 2944, 256, 1, 26, 78), (4366, 2944, 2944, 256, 1, 29, 66),
             (5111, 2176, 4480, 512, 1, 33, 56), (6086, 5760, 5760, 512, 7, None, 48),
             (2712, 1920, 3456, 256, 1, 18, 42)]
    scenes = [Scene(show, place, x, z, facing=f, y=y, song=song, fade_speed=1) for place, x, z, y, f, song, dur in stops]
    closet = Scene(show, 2391, 1408, 640, facing=7, y=256, song=18, fade_speed=1)
    for k, (sc, (place, x, z, y, f, song, dur)) in enumerate(zip(scenes, stops)):
        cut = sc.cut
        fx, fz = FORWARD[f]
        sc.otenko.place(1, x - 150 * fz, z + 150 * fx, d=f)
        sc.player.run(4, x + 700 * fx, z + 700 * fz, y=y)
        sc.otenko.run(10, x + 600 * fx - 150 * fz, z + 600 * fz + 150 * fx, y=y)
        cut.at(4, cam(x + 450 * fx, z + 450 * fz, dur, y=y + 150))
        if k in (0, 2, 4):       # the stranger, glimpsed far ahead, slipping away
            st = Sprite(cut, 0x7001, cast.stranger, cast.sf, y=y).add(1, x + 750 * fx, z + 750 * fz, frame="back_ur")
            st.glide(2, x + 1000 * fx, z + 1000 * fz, dur)
            st.frame(dur - 12, "vanish2")
            st.frame(dur - 6, "vanish4")
            st.mirror(1, 1 if f == 1 else 0)
        cut.hold(dur)
        sc.then = ((scenes[k + 1].pid if k + 1 < len(scenes) else closet.pid), ENTRANCE)
    # the closet: a storeroom with someone already in it
    cut = closet.cut
    closet.otenko.place(1, 1408, 900, d=7)
    closet.player.run(4, 1152, 640)
    Sprite(cut, 0x7003, 0xE04E, {}).add(1, 896, 640, frame=4)
    cut.at(2, cam(1152, 700, 1))
    cut.hold(50)
    cut.say([OT + '"Django... I don\'t think\nwe\'re following it.', 'I think it\'s moving us."'])
    cut.say([DJ + '"This is a closet."'])
    cut.say(["<LABEL=PEOPLE2>Storeroom Man</LABEL>:\n\"...Can I help you?\""])
    cut.hold(40)
    closet.build()
    for sc in scenes[1:]:
        sc.build()
    scenes[0].chain = scenes[1:] + [closet]
    return scenes[0]


@piece
def night(show, cast):
    """Night falls on cue: a snap, and the time of day sweeps round in
    seconds (the cartridge clock) while the townsfolk walk like clockwork."""
    X, Z, Y = 2176, 2560, 512
    sc = Scene(show, 2961, X, Z + 500, facing=1, y=Y, song=20)
    cut = sc.cut
    sc.otenko.place(1, X + 200, Z + 650, d=1)
    sc.player.walk(30, X, Z)
    sc.otenko.walk(36, X + 220, Z + 160, y=Y)
    cut.at(10, cam(X, Z - 150, 80, y=Y + 150))
    folk = [(0xDFDE, X - 420, Z - 260), (0xE03E, X + 380, Z - 420), (0xDFEE, X - 260, Z - 640),
            (0xE2FE, X + 520, Z - 80), (0xE02E, X - 560, Z + 120), (0xDFCE, X + 120, Z - 760)]
    sprites = []
    for k, (pkg, x, z) in enumerate(folk):
        dx, dy = screen(x, z, Y)
        s_ = Sprite(cut, 0x7100 + k, pkg, {}, y=Y).add(1, x, z, frame=4)
        sprites.append((s_, pkg, x, z))
    st = Sprite(cut, 0x7001, cast.stranger, cast.sf, y=Y).add(1, X + 150, Z - 760, frame="stand_dr")
    cut.hold(100)
    st.frame(100, "raise")
    st.frame(118, "snap")
    cut.at(118, sound(1407))
    # clockwork: everyone shuffles left, right, left... in step, turning on the beat
    beat = 30
    for i in range(10):
        t = 130 + beat * i
        for k, (s_, pkg, x, z) in enumerate(sprites):
            d = 1 if i % 2 == 0 else -1
            s_.glide(t, x + 160 * d, z - 160 * d, beat - 2)
            s_.frame(t, NPC[pkg][2])
            s_.mirror(t, 1 if d < 0 else 0)
            sprites[k] = (s_, pkg, x + 160 * d, z - 160 * d)
        cut.at(t, sound(1407) if i % 2 == 0 else sound(1407))
    for k, (s_, pkg, x, z) in enumerate(sprites):
        s_.frame(130 + beat * 10, NPC[pkg][4])
        s_.mirror(130 + beat * 10, 0)
    st.frame(130 + beat * 10, "stand_dr")
    cut.hold(130 + beat * 10 + 40)
    cut.say([DJ + '"Did the sun just set?"'])
    cut.say([OT + '"And rise. Five times."', '"The sun does not\nwork like that!"'])
    st.frame(0, "turn")
    st.frame(16, "vanish1")
    st.frame(22, "vanish2")
    st.frame(28, "vanish3")
    st.frame(34, "vanish4")
    cut.at(16, particles(X + 150, Z - 760, 30, y=Y + 64, kind=GOLD))
    st.hide(42)
    cut.hold(80)
    # the light phase of the sun clock (gUnk_03005430 +0x0E picks the stage
    # palette): day, evening, night, dawn, morning... twice, 22 frames each,
    # then back to the clock's own (5 at the harness's fixed time)
    seq = [1, 5, 2, 0, 4] * 2 + [1, 5]
    sc.events = [(-1, 118 + 22 * i, f"freeze 0300543E:1 {ph:x}") for i, ph in enumerate(seq)]
    sc.events.append((-1, 118 + 22 * len(seq) + 30, "unfreeze"))
    return sc


@piece
def marionettes(show, cast):
    """The cast, summoned in a ring around Django, all facing him, bobbing
    like marionettes."""
    X, Z, Y = 1664, 2300, 256
    sc = Scene(show, 3880, X, Z + 600, facing=1, y=Y, song=26)
    cut = sc.cut
    sc.otenko.place(1, X + 220, Z + 700, d=1)
    sc.player.walk(20, X, Z)
    sc.otenko.walk(26, X + 260, Z + 260, y=Y)
    cut.at(10, cam(X - 70, Z - 70, 60, y=Y + 200))       # a little above Django: the ring rises
    st = Sprite(cut, 0x7001, cast.stranger, cast.sf, y=Y).add(1, X + 420, Z - 560, frame="stand_dr")
    ring = [0xDFCE, 0xDFDE, 0xDFEE, 0xE02E, 0xE03E, 0xE2FE, 0xDFFE]
    members = []
    R = 330
    for k, pkg in enumerate(ring):
        a = 2 * math.pi * k / len(ring) + 0.4
        x, z = X + int(R * math.cos(a)), Z + int(R * math.sin(a))
        sx, sy = screen(x, z, Y)
        cx, cy = screen(X, Z, Y)
        fr, mir = facing(pkg, cx - sx, cy - sy)
        m = Sprite(cut, 0x7200 + k, pkg, {}, y=Y - 200).add(90, x, z, frame=fr, hidden=True)
        m.mirror(90, mir)
        members.append((m, x, z))
    st.frame(80, "raise")
    cut.at(80, sound(1425), quake(30, 20))
    for k, (m, x, z) in enumerate(members):
        t = 100 + 8 * k
        m.show(t)
        if k % 2 == 0:
            cut.at(t, particles(x, z, 40, kind=PURPLE))
        m.glide(t, x, z, 40, y=Y + 140)
    # bob on their strings, out of step
    for k, (m, x, z) in enumerate(members):
        for i in range(10):
            t = 156 + 8 * k + 34 * i
            m.glide(t, x, z, 32, y=Y + (250 if i % 2 == 0 else 80))
    cut.hold(200)
    cut.say(["<LABEL=RITA>Lita</LABEL>:\n\"Django? Why can\'t I move?\""])
    cut.say(["<LABEL=SMITH>Smith</LABEL>:\n\"I was halfway through\na sword!\""])
    cut.say([OT + '"Release them at once!"'])
    st.frame(0, "stand_dr")
    for k, (m, x, z) in enumerate(members):
        m.glide(10 + 6 * k, x, z, 30, y=Y - 220)
        m.hide(42 + 6 * k)
    cut.at(10, sound(1407))
    cut.hold(90)
    cut.say([DJ + '"...We need to find\nthat thing."'])
    st.frame(0, "turn")
    st.frame(16, "vanish1")
    st.frame(22, "vanish2")
    st.frame(28, "vanish3")
    st.frame(34, "vanish4")
    cut.at(16, particles(X + 420, Z - 560, 30, kind=GOLD))
    st.hide(42)
    cut.hold(70)
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


def otenko_kf(cut, cast, t, x, z, y, frame=0):
    """Otenko floating as a keyframe sprite, bobbing gently."""
    o = Sprite(cut, 0x7006, cast.otenko, {}, y=y + 80).add(t, x, z, frame=frame)
    for i in range(12):
        o.glide(t + 10 + 30 * i, x, z, 28, y=y + (96 if i % 2 == 0 else 72))
    return o


def vanargand(cmd):
    from show import actor
    return actor(V, 4, cmd)


@piece
def count(show, cast):
    """The boss who forgets: the Count, mid-attack, cycles through his
    palettes, plays his attack backwards and turns into a ghoul."""
    X, Z, Y = 1664, 2050, 512
    CX, CZ = 2050, 1560
    sc = Scene(show, 3243, X, Z, facing=1, y=Y, song=None, camera=(1850, 1800))
    cut, dj = sc.cut, sc.player
    cut.at(0, bgm(45))
    otenko_kf(cut, cast, 1, X - 260, Z + 120, Y)
    counts = []
    for k, pkg in enumerate(cast.count):
        c = Sprite(cut, 0x7300 + k, pkg, {}, y=Y).add(1, CX, CZ, frame=60, hidden=k > 0)
        counts.append(c)
    count0 = counts[0]
    dj.aim(20, 1)
    # the attack: a lunge, Django fires
    count0.frame(40, 20)
    count0.frame(48, 21)
    count0.frame(56, 22)
    count0.glide(48, CX - 220, CZ + 220, 24)
    for c in counts[1:]:
        c.glide(48, CX - 220, CZ + 220, 24)
    count0.frame(64, 23)
    dj.shoot(60, 1)
    # the stranger, on the arena's edge
    SX, SZ = 1550, 1500
    st = Sprite(cut, 0x7001, cast.stranger, cast.sf, y=Y).add(84, SX, SZ, frame="raise", hidden=True)
    st.show(90)
    cut.at(90, particles(SX, SZ, 30, y=Y + 64, kind=GOLD), sound(1407))
    # palettes: show one copy at a time, faster and faster
    t, k, step = 110, 0, 8
    while t < 230:
        nxt = (k + 1) % len(counts)
        counts[nxt].frame(t, 23)
        counts[nxt].show(t)
        counts[k].hide(t)
        k, t = nxt, t + step
        step = max(3, step - 1)
    for i, c in enumerate(counts):
        if i != 0:
            c.hide(t)
    count0.show(t)
    # the attack, backwards
    for i, f in enumerate([23, 22, 21, 20]):
        count0.frame(t + 10 + 10 * i, f)
    count0.glide(t + 10, CX, CZ, 40)
    count0.frame(t + 60, 60)
    cut.hold(t + 80)
    cut.say(["<LABEL=HAKUSYAKU>Count</LABEL>:\n\"This... is not how\nthis goes.\""])
    count0.frame(0, 4)
    cut.at(20, particles(CX, CZ, 40, y=Y + 64, kind=PURPLE), sound(1425), quake(20, 12))
    count0.hide(34)
    g = Sprite(cut, 0x7002, cast.ghoul, {}, y=Y).add(34, CX, CZ, frame=8)
    g.mirror(60, 1)
    g.frame(60, 9)
    g.mirror(90, 0)
    g.frame(90, 8)
    g.mirror(120, 1)
    g.walk(130, CX + 500, CZ - 500, 110, [0, 1, 2, 3], step=9)
    dj.aim_end(140, 1)
    st.frame(150, "turn")
    st.hide(170)
    cut.at(160, particles(SX, SZ, 30, y=Y + 64, kind=GOLD))
    cut.hold(200)
    cut.say([OT + '"Was that... the Count?"'])
    cut.say([DJ + '"He forgot his own fight."'])
    cut.hold(40)
    return sc


@piece
def ride(show, cast):
    """Somebody is riding Vanargand."""
    X, Z = 1440, 1440
    sc = Scene(show, 6554, X, Z, facing=4, y=256, song=None, stage=dream_stage(show), camera=(X, Z))
    sc.spawns = [Call(6557), Call(6558)]       # entrance 58: Vanargand, waiting (D=1)
    cut = sc.cut
    otenko_kf(cut, cast, 1, X + 200, Z + 120, 256)
    cut.at(0, bgm(78))
    sh = Sprite(cut, 0x7001, cast.stranger, cast.sf, y=1040).add(1, 1114, 1114, frame="stand_dr", hidden=True)
    VIEW = (1180, 1180, 700)       # x, z, y: Django at the bottom, Vanargand above him
    cut.at(30, cam(VIEW[0], VIEW[1], 90, y=VIEW[2]))
    cut.at(60, message_audio(106), sound(1425))
    cut.at(62, vanargand(51), quake(90, 10))
    cut.at(64, cam(VIEW[0], VIEW[1], 40, y=VIEW[2]))
    cut.at(122, vanargand(52), quake(90, 20))
    cut.at(124, cam(VIEW[0], VIEW[1], 10, y=VIEW[2]))
    sh.show(150)
    cut.at(150, particles(1114, 1114, 30, y=1080, kind=GOLD), sound(1407))
    cut.at(232, vanargand(56))
    cut.at(234, cam(VIEW[0], VIEW[1], 10, y=VIEW[2]))
    sh.frame(232, "raise")
    sh.glide(240, 1114, 1114, 30, y=1100)
    sh.glide(270, 1114, 1114, 30, y=1040)
    cut.hold(300)
    cut.say([DJ + '"Is that... riding\nVanargand?"'])
    cut.say([OT + '"Nobody rides Vanargand."'])
    cut.at(0, vanargand(62), quake(90, 10))
    cut.at(2, cam(VIEW[0], VIEW[1], 10, y=VIEW[2]))
    sh.frame(10, "point")
    sh.glide(10, 1114, 1114, 40, y=1090)
    cut.at(70, fade(1, 3, 0, 0, 0))
    cut.hold(120)
    return sc


def message_audio(n):
    from show import message, AUDIO
    return message(AUDIO, 8, 6, [n, 0])


SUN = "{GS+0x876}"             # GameState+0x876: the sun level the HUD draws (filled in by record.py)
SUN_STATE = "{SENSOR+0x19}"    # the sensor task's state byte: 1 holds it there, so the level is ours


def overload_events(page, start):
    """Harness lines: calibrate the sensor, fill the gauge with light, then
    push the level past full one cell at a time (record.py events)."""
    ev = [(page, start, "poke 030025C4 E8")]
    t = start
    for lux in range(235, 50, -15):
        ev.append((page, t, f"lux {lux}"))
        t += 10
    t += 40
    ev.append((page, t, f"freeze {SUN_STATE}:1 1"))
    for level in range(11, 52):
        ev.append((page, t, f"freeze {SUN}:2 {level:x}"))
        t += max(3, 9 - (level - 11) // 5)
    return ev, t


@piece
def reveal(show, cast):
    """The skylight: strange light, Django charges, the SUN gauge climbs
    past full and the gun blows; the stranger's cloak burns away."""
    X, Z, Y = 1792, 1792, 256
    SX, SZ = 1600, 1500
    sc = Scene(show, 2712, X, Z + 600, facing=1, y=Y, song=18, flags=1, camera=(X - 100, Z - 50), hud_mode=1)
    cut, dj = sc.cut, sc.player
    sc.otenko.place(1, X + 260, Z + 700, d=1)
    dj.walk(30, X, Z)
    sc.otenko.walk(40, X + 300, Z + 200)
    st = Sprite(cut, 0x7001, cast.stranger, cast.sf).add(1, SX, SZ, frame="stand_dr")
    # the light goes strange: the room's palette tinted round the rainbow
    colors = [(31, 8, 8), (31, 24, 4), (8, 31, 8), (4, 24, 31), (20, 8, 31), (31, 8, 24)]
    for i in range(14):
        r, g, b = colors[i % len(colors)]
        cut.at(90 + 14 * i, fade(1, 7, r, g, b, 1, 0, 0, 40))
        if i % 3 == 0:
            cut.at(90 + 14 * i, particles(X, Z, 30, y=Y + 96, kind=[ICE, GOLD, PURPLE][i // 3 % 3]))
    r, g, b = colors[13 % len(colors)]
    cut.at(90 + 14 * 14, fade(0, 7, r, g, b, 1, 0, 0, 40))     # back from the last tint, not from black
    cut.hold(90 + 14 * 14 + 20)
    cut.say([DJ + '"What\'s wrong with the light?"'])
    cut.say([OT + '"It\'s sunlight... but not\nfrom any sun I know."'])
    cut.say([DJ + '"...I wonder what happens\nif I charge here."'])
    # charging: the HUD back, Django raises the gun; the harness pours light in.
    # Messages open at the top from here, so the SUN gauge stays in view
    cut.at(0, hud(1), top_messages())
    dj.aim(4, 1)
    ev, t_end = overload_events(2, 20)
    sc.events = ev
    for i, t in enumerate(range(200, t_end + 40, 24)):
        cut.at(t, quake(10 + 4 * i, 24), sound(1407) if i % 2 == 0 else particles(X, Z, 20, y=Y + 64, kind=FLAME))
    cut.hold(t_end + 40)
    cut.say([DJ + '"My gun... it\'s overheating!"'])
    cut.say([OT + '"Django! Let go of it!"'])
    # the explosion; messages back at the bottom (story past 191)
    cut.at(0, hud(1), quake(120, 60), sound(641), particles(X, Z, 40, y=Y + 64, kind=FLAME), Set(STORY, 194))
    cut.at(6, fade(1, 1, 31, 31, 31, 1, 1, 1, 0))
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
    cut.say([CL + '"I read every function,\nscript and table of this', 'world and wrote them\nback as source code.', 'So now I can change it\nfrom the inside."'])
    cut.say([DJ + '"The doors. The ghouls.\nVanargand."'])
    cut.say([CL2 + '"Sorry about the doors."'])
    cut.hold(60)
    return sc


@piece
def coffins(show, cast):
    """Encore: Claude drags a train of coffins, one for every boss."""
    X, Z, Y = 2176, 2400, 512
    sc = Scene(show, 2961, X, Z, facing=1, y=Y, song=20, camera=(X, Z - 200))
    cut, dj = sc.cut, sc.player
    sc.otenko.place(1, X + 200, Z + 120, d=1)
    dj.face(10, 7)
    # an S across the street, from the left edge of the screen to the right
    path = [(X - 900, Z - 300), (X - 450, Z - 650), (X, Z - 550), (X + 250, Z - 250), (X + 650, Z - 450),
            (X + 950, Z - 950)]
    cl = Sprite(cut, 0x7005, cast.claude, cast.cf, y=Y).add(20, *path[0], frame="drag1")
    trains = [Sprite(cut, 0x7400 + k, cast.coffins[k % len(cast.coffins)], {}, y=Y).add(20, *path[0], frame=0)
              for k in range(12)]
    spacing, speed = 150, 8          # world units between coffins, units a frame
    seg = [math.dist(path[i], path[i + 1]) for i in range(len(path) - 1)]
    total = sum(seg)

    def at_(d):
        d = max(0.0, min(total, d))
        for i, L in enumerate(seg):
            if d <= L:
                (x0, z0), (x1, z1) = path[i], path[i + 1]
                return int(x0 + (x1 - x0) * d / L), int(z0 + (z1 - z0) * d / L)
            d -= L
        return path[-1]

    def run_train(t0, d0, d1, step=8):
        """Claude goes from distance d0 to d1 along the path; every coffin
        follows `spacing` further back. Returns when the last one stops."""
        frames = int((d1 - d0) / speed)
        for f in range(0, frames + step, step):
            d = d0 + min(frames, f) * speed
            cl.glide(t0 + f, *at_(d), step)
            for k, c in enumerate(trains):
                c.glide(t0 + f, *at_(d - spacing * (k + 1)), step)
        cl.cycle(t0, ["drag1", "drag2"], frames, step=10)
        return t0 + frames + step
    stop = 0.45 * total              # beside Django
    end = run_train(30, 0, stop)
    cl.frame(end, "stand_dl")
    cut.hold(end + 10)
    cut.say([DJ + '"You\'re not purifying\nall of those."'])
    cl.frame(0, "talk")
    cut.say([CL2 + '"Already did."'])
    cl.frame(0, "wave")
    end = run_train(20, stop, total + spacing * 13)
    cut.hold(end + 10)
    sc.tail = end + 60
    return sc


@piece
def vride(show, cast):
    """Encore: Claude on Vanargand, waving."""
    X, Z = 1440, 1440
    sc = Scene(show, 6554, X, Z, facing=4, y=256, song=None, stage=dream_stage(show), camera=(1250, 1250))
    sc.spawns = [Call(6557), Call(6558)]
    cut = sc.cut
    otenko_kf(cut, cast, 1, X + 200, Z + 120, 256)
    cut.at(0, bgm(57))
    VIEW = (1180, 1180, 700)
    cut.at(4, cam(VIEW[0], VIEW[1], 30, y=VIEW[2]))
    cl = Sprite(cut, 0x7005, cast.claude, cast.cf, y=1040).add(1, 1114, 1114, frame="ride", hidden=True)
    cut.at(10, vanargand(51), quake(60, 10), sound(1425))
    cut.at(12, cam(VIEW[0], VIEW[1], 20, y=VIEW[2]))
    cut.at(70, vanargand(52))
    cut.at(72, cam(VIEW[0], VIEW[1], 10, y=VIEW[2]))
    cl.show(100)
    cut.at(100, particles(1114, 1114, 30, y=1080, kind=GOLD))
    cut.at(180, vanargand(56))
    cut.at(182, cam(VIEW[0], VIEW[1], 10, y=VIEW[2]))
    for i in range(6):
        cl.frame(190 + 16 * i, "wave" if i % 2 == 0 else "ride")
    cut.hold(300)
    cut.say([OT + '"Nobody rides Vanar..."'])
    cut.say([OT + '"...Oh, never mind."'])
    cut.at(0, vanargand(62), quake(90, 10))
    cut.at(2, cam(VIEW[0], VIEW[1], 10, y=VIEW[2]))
    cl.frame(0, "cheer")
    cut.at(60, fade(1, 3, 31, 31, 31))
    cut.hold(110)
    return sc


@piece
def sunback(show, cast):
    """Encore: Claude hands Django the sun: the gauge swings, day turns to
    night and back, and the Gun Del Sol fires."""
    X, Z, Y = 1920, 2400, 256
    sc = Scene(show, 2712, X, Z, facing=1, y=Y, song=18, flags=1, camera=(X, Z - 150), hud_mode=1)
    cut, dj = sc.cut, sc.player
    sc.otenko.place(1, X + 240, Z + 100, d=1)
    cl = Sprite(cut, 0x7005, cast.claude, cast.cf).add(1, X - 300, Z - 300, frame="stand_dr")
    cut.hold(40)
    cut.say([CL + '"Here. Your sun back."'])
    cl.frame(0, "point")
    cut.at(0, hud(1), particles(X, Z, 40, y=Y + 64, kind=GOLD), sound(1407))
    dj.aim(30, 1)
    dj.shoot(330, 1)
    dj.shoot(360, 1)
    dj.aim_end(420, 1)
    cl.frame(330, "cheer")
    cut.hold(460)
    cut.say([DJ + '"...Thanks."'])
    cut.hold(30)
    ev = [(0, 0, "poke 030025C4 E8")]
    t = 10
    for lux in list(range(235, 50, -20)) + list(range(60, 236, 25)) + list(range(235, 50, -20)):
        ev.append((0, t, f"lux {lux}"))
        t += 8
    for i, ph in enumerate([5, 2, 0, 4, 1, 5, 2, 0, 4, 1]):
        ev.append((0, 40 + 24 * i, f"freeze 0300543E:1 {ph:x}"))
    ev.append((0, 40 + 24 * 10, "unfreeze"))
    sc.events = ev
    return sc


@piece
def finale(show, cast):
    """The last shot: "How did you even do all this?" "Want to see?\""""
    X, Z, Y = 1792, 1792, 256
    sc = Scene(show, 2712, X, Z + 300, facing=1, y=Y, song=18, camera=(X - 100, Z))
    cut = sc.cut
    sc.otenko.place(1, X + 240, Z + 400, d=1)
    cl = Sprite(cut, 0x7005, cast.claude, cast.cf).add(1, X - 250, Z - 250, frame="stand_dr")
    cut.hold(50)
    cut.say([DJ + '"How did you even\ndo all this?"'])
    cl.frame(0, "talk")
    cut.say([CL2 + '"Want to see?"'])
    cl.frame(0, "wave")
    cut.at(30, fade(1, 4, 31, 31, 31), bgm_out(4))
    cut.hold(160)
    return sc


@piece
def music(show, cast):
    """Five minutes of song 57 (the bed under part 2)."""
    sc = Scene(show, 2712, 1920, 3200, facing=1, song=57)
    sc.cut.hold(60)
    sc.tail = 60 * 300
    return sc


CREDITS = [
    ("cast", None),
    ("<WEIGHT>The Stranger</WEIGHT>\n???", "after:2479"),
]


def credits(show, finale_pid):
    """The game's end roll (scene 6595) with the decomp's credits inserted:
    the crawl's pages copied onto borrowed string ids, ours between them."""
    from gclasm import Hash, Text, Cmd
    from rom import STRING_TABLE, STRING_BANK, BASE
    import struct as st_
    rom = show.r.rom

    def raw(i):
        w = st_.unpack_from("<I", rom, STRING_TABLE - BASE + 4 * i)[0]
        o = STRING_BANK - BASE + (w & 0x7FFFFFFF)
        return bytes(rom[o:rom.index(b"\0", o)])
    ours = {
        2479: ["<WEIGHT>The Stranger</WEIGHT>\n???"],
        2481: ["<WEIGHT>BOKTAI 3 DECOMPILATION</WEIGHT>\n\n<WEIGHT>Direction</WEIGHT>\nmoozilla",
               "<WEIGHT>Decompilation, Tools</WEIGHT>\nClaude\n\n<WEIGHT>With</WEIGHT>\nGPT-6 Astra, GPT-6.1 Sol",
               "<WEIGHT>The Claudesona</WEIGHT>\nvgel  thebes\nN8python's CC0 sprites\nEidoverse",
               "<WEIGHT>Compiler</WEIGHT>\npret's agbcc\n\n<WEIGHT>Permuter</WEIGHT>\ndecomp-permuter",
               "<WEIGHT>Emulator</WEIGHT>\nmGBA"],
        2506: ["<WEIGHT>Special Thanks</WEIGHT>\nEveryone who still\nplays in the sun"],
    }
    keep = list(range(2469, 2485)) + [2504, 2505, 2506, 2507]
    pages = []
    for i in keep:
        pages.append(raw(i))
        for t in ours.get(i, []):
            pages.append(t.encode("ascii"))
    pages.append(b"<PROC=0>\n<END>")
    block = [show.strings.pop() for _ in range(len(pages))][::-1]
    assert block == list(range(block[0], block[0] + len(pages)))
    for sid, b in zip(block, pages):
        a = show.r.alloc(b + b"\0", 1)
        show.r.u32(STRING_TABLE + 4 * sid, 0x80000000 | (a - STRING_BANK))
    end = show.proc([Call(945, finale_pid, ENTRANCE, 0)])
    roll = show.proc([Cmd("chara", [Hash(0x4DDF), Hash(0x79C0)], [("t", [Text(9968)]), ("c", [Text(9970)])]),
                      Call(1048, 92),
                      Cmd("chara", [Hash(0x3DF7), Hash(0x1465)], [("p", [0]), ("s", [Text(block[0])]), ("t", [Text(9968)]),
                                                                  ("f", [6]), ("e", [Proc(end)])])])
    pid = show.proc([Call(861, 41), Call(864, 2), Call(553), Call(1265), Call(1), Call(6594), Call(6596), Call(905),
                     Call(868), Call(roll)])
    return pid


def build():
    show = Show(STRINGS, SLOTS)
    cast = Cast(show)
    scenes = {}
    for name, fn in PIECES.items():
        sc = fn(show, cast)
        scenes[name] = (sc.build(), sc.cut.pages + [p for c in sc.chain for p in c.cut.pages], sc.events, sc.tail)
    scenes["credits"] = (credits(show, scenes["finale"][0]), [], [], 0)
    show.save(ROM)
    return scenes


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
    os.makedirs(REC, exist_ok=True)
    out = os.path.join(REC, f"{name}.mp4")
    if name == "fight":
        # the real fight with the Count (scene 3247, entrance 47), played with the buttons
        st = run.state(ROM, 3247, 47, "p1_fight", wait=30)
        keys = ["wait 60"] + ["hold UP+RIGHT 6", "tap A", "wait 14"] * 6 + ["hold LEFT 20", "tap A", "wait 20"] * 3
        record.record(ROM, st, [], "p1_fight_rec", out, pre=[], tail=60, post=keys)
    elif name == "credits":
        st = run.state(ROM, scenes["credits"][0], 57, "p1_credits", wait=30)
        record.record(ROM, st, [], "p1_credits_rec", out, tail=int(sys.argv[4]) if len(sys.argv) > 4 else 5400)
    else:
        pid, pages, events, tail = scenes[name]
        st = run.state(ROM, pid, ENTRANCE, f"p1_{name}", wait=30)
        record.record(ROM, st, pages, f"p1_{name}_rec", out, shots=True, events=events, tail=tail)
    sheet(out, os.path.join(run.OUT, f"p1_{name}.png"), float(sys.argv[3]) if len(sys.argv) > 3 else 1.0)


def sheet(video, out, every=1.0, cols=6):
    """A contact sheet of a recording: one frame every `every` seconds."""
    import subprocess
    import tempfile
    d = tempfile.mkdtemp(prefix="sheet-")
    subprocess.run(["ffmpeg", "-v", "error", "-i", video, "-vf", f"fps={1 / every},scale=240:160:flags=neighbor",
                    os.path.join(d, "f%04d.png")], check=True)
    files = sorted(os.path.join(d, f) for f in os.listdir(d))
    subprocess.run(["magick", "montage", "-font", "/System/Library/Fonts/Supplemental/Arial.ttf", *files,
                    "-tile", f"{cols}x", "-geometry", "+1+1", "-background", "#222", out], check=True)
    import shutil
    shutil.move(d, os.path.expanduser(f"~/.Trash/{os.path.basename(d)}"))
    print(out)


if __name__ == "__main__":
    main()
