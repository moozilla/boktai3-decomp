#!/usr/bin/env python3
"""Throwaway scenes that prove each v3 mechanism before the story uses it.

  python3 tools/showcase/v3/proofs.py NAME     build, run, screenshot montage

Each proof builds its own ROM under build/showcase/v3/ and writes a montage
of screenshots there. Names: fan (the ghoul fanning out in many palettes),
cast (NPC packages summoned in a ring), chain (three real places joined by
door transitions), moves (the player and Otenko through their receivers),
talk (dialogue with adaptive timing), ride (a sprite on Vanargand's rise),
ptypes (the screen's particle types), campan (camera pans in the dream).
"""
import colorsys
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from show import (COUNTER, Call, Cutscene, Proc, Set, Show, cam, command, fade, kf_add, kf_glide, kf_hide, particles,  # noqa: E402
                  keyframe_manager, scene_entry, sound)
import romx  # noqa: E402
import run  # noqa: E402

STRINGS = range(8140, 8400)       # the ending's dialogue (shown by no scene we visit)
SLOTS = [136, 137, 138, 139]      # ending cutscenes
OUT = run.OUT


def hue_variants(colors, n, ramp):
    """n palettes: the colors at indices `ramp` hue-rotated around the wheel."""
    out = []
    for k in range(n):
        pal = list(colors)
        for i in ramp:
            c = colors[i]
            r, g, b = (c >> 16 & 255) / 255, (c >> 8 & 255) / 255, (c & 255) / 255
            h, l_, s = colorsys.rgb_to_hls(r, g, b)
            h = (h + k / n) % 1.0
            s = min(1.0, s * 1.1)
            r, g, b = colorsys.hls_to_rgb(h, l_, s)
            pal[i] = int(r * 255) << 16 | int(g * 255) << 8 | int(b * 255)
        out.append(pal)
    return out


def ghoul_packages(show, n=20, first=0x6100):
    """n packages of the ghoul's frames (9247), each in its own new pool."""
    frames, origin, pool = romx.big_frames(show.r.rom, 0x9247)
    base = romx.package(frames, 0, origin)
    pals = hue_variants(romx.pool_colors(show.r.rom, pool), n, [7, 8, 9, 10, 11])
    return [show.package(first + k, romx.repool(base, show.pool(p))) for k, p in enumerate(pals)]


def fan():
    show = Show(STRINGS, SLOTS)
    pk = ghoul_packages(show, 20)
    cut = Cutscene(show, extra=[1675])
    GX, GZ = 1152, 1152
    tags = [0x7100 + k for k in range(20)]
    cut.at(0, fade(0, 7, 0, 0, 0), cam(GX, GZ))
    cut.at(1, *[kf_add(tags[k], pk[k], GX, GZ) for k in range(20)])
    import math
    for k in range(20):
        a = math.pi * (k / 19.0)          # a half circle of positions
        x = GX + int(520 * math.cos(a) * 0.7) - int(520 * math.sin(a) * 0.3)
        z = GZ - int(520 * math.sin(a) * 0.7) - int(520 * math.cos(a) * 0.3)
        cut.at(60, kf_glide(tags[k], x, z, 40))
    cut.at(60, sound(1407))
    for k in range(20):
        cut.at(140, kf_glide(tags[k], GX, GZ, 20))
    cut.at(170, *[kf_hide(tags[k]) for k in range(1, 20)])
    cut.hold(240)
    done = show.proc([Call(1669)])
    dlg = cut.write(done)
    arrive = show.proc([command(0x9769, p=[GX, 406, GZ + 512]), command(0x061E, i=0, d=4)])
    start = show.proc([Set(COUNTER, 0), Call(1667, 1, 1, 0, 0), Call(dlg)])
    stage = show.proc([Call(2718), keyframe_manager(24)])
    scene = scene_entry(show, stage, 58, GX + 256, GZ + 512, 6, arrive, start)
    path = os.path.join(OUT, "fan.gba")
    show.save(path)
    st = run.state(path, scene, 58, "fan")
    print(run.shots(path, st, [5, 30, 70, 85, 100, 130, 150, 175], "fan"))



def simple_scene(show, cut, stage_proc, x, z, facing=6, capacity=24, flags=56, extra=(), bgm_id=None, cam_xz=None):
    """A scene on a real stage whose arrival starts `cut`; returns the scene id."""
    done = show.proc([Call(1669)])
    dlg = cut.write(done)
    cx, cz = cam_xz or (x, z)
    arrive = show.proc([command(0x9769, p=[cx, 406, cz]), command(0x061E, i=0, d=4)])
    start = show.proc([Set(COUNTER, 0), Call(1667, 1, 1, 0, 0), Call(dlg)])
    stage = show.proc([Call(stage_proc), keyframe_manager(capacity)])
    return scene_entry(show, stage, 58, x, z, facing, arrive, start, extra=extra, flags=flags, bgm_id=bgm_id)


CAST = [0xDFCE, 0xDFDE, 0xDFEE, 0xDFFE, 0xE01E, 0xE02E, 0xE03E, 0xE04E, 0xE2DE, 0xE2EE, 0xE2FE, 0xE349]


def cast():
    import math
    show = Show(STRINGS, SLOTS)
    cut = Cutscene(show)
    X, Z = 1152, 1152
    cut.at(0, fade(0, 7, 0, 0, 0), cam(X, Z))
    for k, h in enumerate(CAST):
        a = 2 * math.pi * k / len(CAST)
        x, z = X + int(420 * math.cos(a)), Z + int(420 * math.sin(a))
        t = 20 + 10 * k
        cut.at(t, particles(x, z, 30), kf_add(0x7200 + k, h, x, z))
        cut.at(t, sound(1407)) if k % 3 == 0 else None
    cut.hold(260)
    scene = simple_scene(show, cut, 2718, X, Z + 300)
    path = os.path.join(OUT, "cast.gba")
    show.save(path)
    st = run.state(path, scene, 58, "cast", wait=240)
    print(run.shots(path, st, [10, 60, 120, 200], "cast"))



def chain():
    """Three real places in a row: each cutscene ends in a door transition
    (proc_945) to the next scene, whose arrival starts its own cutscene."""
    from show import actor
    show = Show(STRINGS, SLOTS)
    stops = [(2718, 384, 2432, 256, 1), (2850, 2176, 1792, 512, 1), (4281, 1280, 2560, 256, 1)]
    scenes = [show.r.new_proc_id() for _ in stops]
    for i, (stage, x, z, y, f) in enumerate(stops):
        cut = Cutscene(show)
        cut.at(0, fade(0, 7, 0, 0, 0), cam(x, z, y=y + 150))
        cut.at(20, actor(0x2B06, 2, 33, x - 600, y, z - 600))      # run_to
        cut.at(20, cam(x - 600, z - 600, 50, y=y + 150))
        cut.hold(90)
        nxt = scenes[(i + 1) % len(scenes)]
        done = show.proc([Call(1669), Call(945, nxt, 58, 0)])
        dlg = cut.write(done)
        arrive = show.proc([command(0x9769, p=[x, y + 150, z]), command(0x061E, i=0, d=4)])
        start = show.proc([Set(COUNTER, 0), Call(1667, 1, 1, 0, 0), Call(dlg)])
        stg = show.proc([Call(stage), keyframe_manager(8)])
        ents = show.proc([Call(916, x, z, f, 0, 128, 0, 58, Proc(arrive), Proc(start))])
        show.set_proc(scenes[i], [Call(861, 56), Call(864, 0), Call(553), Call(1265), Call(1), Call(stg), Call(ents), Call(905)])
    path = os.path.join(OUT, "chain.gba")
    show.save(path)
    st = run.state(path, scenes[0], 58, "chain", wait=60)
    print(run.shots(path, st, list(range(0, 1200, 75)), "chain", scale=1))



def moves():
    """Django walks and runs (player 2B06 commands 7 and 33), Otenko is placed
    (proc_1675 then proc_1676) and walks (NPC 9CFE command 4), the camera follows."""
    from show import actor
    show = Show(STRINGS, SLOTS)
    cut = Cutscene(show, extra=[1675, 1676])
    P, O = 0x2B06, 0x9CFE
    cut.at(0, fade(0, 7, 0, 0, 0), cam(1408, 1664), cut.run(1675))
    cut.at(1, cut.run(1676, 1664, 1664, 3, 1))
    cut.at(30, actor(P, 2, 7, 896, 256, 1664, 0))
    cut.at(30, actor(O, 5, 4, 1152, 256, 1408))
    cut.at(30, cam(1152, 1536, 60))
    cut.at(120, actor(P, 2, 33, 896, 256, 896, 0), cam(1024, 1024, 40))
    cut.at(180, actor(O, 5, 5, 1152, 256, 896))
    cut.at(240, actor(P, 2, 3, 2), actor(O, 5, 3, 6))
    cut.hold(300)
    scene = simple_scene(show, cut, 2718, 1408, 1664)
    path = os.path.join(OUT, "moves.gba")
    show.save(path)
    st = run.state(path, scene, 58, "moves", wait=40)
    print(run.shots(path, st, list(range(0, 480, 40)), "moves", scale=1))



def talk():
    """Dialogue with adaptive timing: three messages, one of two pages."""
    from show import DJ, OT, actor
    import record
    show = Show(STRINGS, SLOTS)
    cut = Cutscene(show, extra=[1675, 1676])
    cut.at(0, fade(0, 7, 0, 0, 0), cam(1408, 1664), cut.run(1675))
    cut.at(1, cut.run(1676, 1664, 1664, 3, 1))
    cut.hold(40)
    cut.say([OT + '"Django. Who is that?"'])
    cut.say([DJ + '"..."'], actor(0x2B06, 2, 3, 2))
    cut.say([OT + '"Something is wrong with\nthis place.', 'I can feel it."'])
    cut.at(0, actor(0x9CFE, 5, 4, 1408, 256, 1408))
    cut.hold(60)
    scene = simple_scene(show, cut, 2718, 1408, 1664)
    path = os.path.join(OUT, "talk.gba")
    show.save(path)
    st = run.state(path, scene, 58, "talk", wait=40)
    out = os.path.join(OUT, "talk.mp4")
    d = record.record(path, st, show.pages, "talk", out, shots=True)
    print(d, show.pages)



def dream_stage(show, capacity=8):
    """The prologue's dream stage (proc_6535) with room for `capacity` keyframe sprites."""
    import gcx
    o = show.r.img.proc_address(6535 - 1)
    st, n = gcx.length(bytes(show.r.rom), o)
    code = bytearray(show.r.rom[o:st + n])
    k = code.rfind(bytes.fromhex("06f68a06f68a526dc2"))
    assert k > 0
    code[k + 8] = 0xC0 | (capacity + 1)
    pid = show.r.new_proc_id()
    show.r.set_proc(pid, bytes(code))
    return pid


def ride():
    """A silhouette on Vanargand's head as it rises (scene actor 7C56, commands 51/52/56/62)."""
    import art
    from show import actor, quake, sound
    show = Show(STRINGS, SLOTS)
    pk, fr, pool = art.package(show, 0x6200, "stranger")
    shadow, _, _ = art.package(show, 0x6201, "stranger", recolor=None)
    cut = Cutscene(show)
    V = 0x7C56
    positions = [(1000, 700, 1000), (1100, 900, 1100), (900, 1100, 900), (1114, 1200, 1114)]
    cut.at(0, fade(0, 7, 0, 0, 0), cam(1114, 1114))
    for k, (x, y, z) in enumerate(positions):
        cut.at(1, kf_add(0x7300 + k, pk, x, z, y=y, frame=fr["stand_dr"]))
    cut.at(30, actor(V, 4, 51), quake(90, 10), sound(1425))
    cut.at(90, actor(V, 4, 52), quake(90, 20))
    cut.at(200, actor(V, 4, 56))
    cut.at(260, actor(V, 4, 62), quake(90, 10))
    cut.hold(400)
    done = show.proc([Call(1669)])
    dlg = cut.write(done)
    arrive = show.proc([command(0x9769, p=[1440, 406, 1440]), command(0x061E, i=0, d=4)])
    start = show.proc([Set(COUNTER, 0), Call(1667, 1, 1, 0, 0), Call(dlg)])
    stage = dream_stage(show)
    ents = show.proc([Call(916, 1440, 1440, 4, 0, 128, 0, 58, Proc(arrive), Proc(start))])
    scene = show.proc([Call(861, 56), Call(864, 0), Call(553), Call(1265), Call(1), Call(stage), Call(ents), Call(905),
                       Call(6557), Call(6558)])
    path = os.path.join(OUT, "ride.gba")
    show.save(path)
    st = run.state(path, scene, 58, "ride", wait=40)
    print(run.shots(path, st, list(range(0, 480, 40)), "ride", scale=1))



def ptypes():
    """Particle types 0-7 (OrbitingParticles_Create's last argument), 50 frames each."""
    from show import message, SCREEN
    show = Show(STRINGS, SLOTS)
    cut = Cutscene(show)
    X, Z = 1152, 1152
    cut.at(0, fade(0, 1, 0, 0, 0), cam(X, Z, 1))
    for k in range(8):
        cut.at(20 + 50 * k, message(SCREEN, 7, 0, [X, 320, Z, 128, 40, 24, k, 0]))
    cut.hold(440)
    scene = simple_scene(show, cut, 2718, X, Z + 300)
    path = os.path.join(OUT, "ptypes.gba")
    show.save(path)
    st = run.state(path, scene, 58, "ptypes", wait=240)
    print(run.shots(path, st, [10 + 50 * k for k in range(8)] + [30 + 50 * k for k in range(8)], "ptypes", scale=1))



def campan():
    """Camera pans in the dream stage (does anything clamp it?)."""
    from show import message, CAMERA
    show = Show(STRINGS, SLOTS)
    cut = Cutscene(show)
    cut.at(0, fade(0, 1, 0, 0, 0))
    cut.at(30, message(CAMERA, 6, 2, [1144, 406, 1144, 60]))
    cut.at(120, message(CAMERA, 6, 2, [1300, 900, 1300, 60]))
    cut.at(210, message(CAMERA, 6, 1, [1000, 406, 1000]))
    cut.hold(300)
    done = show.proc([Call(1669)])
    dlg = cut.write(done)
    arrive = show.proc([command(0x9769, p=[1440, 406, 1440]), command(0x061E, i=0, d=4)])
    start = show.proc([Set(COUNTER, 0), Call(1667, 1, 1, 0, 0), Call(dlg)])
    stage = dream_stage(show)
    ents = show.proc([Call(916, 1440, 1440, 4, 0, 128, 0, 58, Proc(arrive), Proc(start))])
    scene = show.proc([Call(861, 56), Call(864, 0), Call(553), Call(1265), Call(1), Call(stage), Call(ents), Call(905),
                       Call(6557), Call(6558)])
    path = os.path.join(OUT, "campan.gba")
    show.save(path)
    st = run.state(path, scene, 58, "campan", wait=40)
    print(run.shots(path, st, [20, 100, 190, 280], "campan", scale=1))



def vcmds():
    """Vanargand's scene-actor commands 51-62, one every 90 frames, camera looking up."""
    from show import actor
    show = Show(STRINGS, SLOTS)
    cut = Cutscene(show)
    cut.at(0, fade(0, 1, 0, 0, 0), cam(1200, 1200, 1, y=900))
    order = [51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62]
    for k, c in enumerate(order):
        cut.at(20 + 90 * k, actor(0x7C56, 4, c), cam(1200, 1200, 1, y=900))
    cut.hold(20 + 90 * len(order) + 60)
    done = show.proc([Call(1669)])
    dlg = cut.write(done)
    arrive = show.proc([command(0x9769, p=[1440, 406, 1440]), command(0x061E, i=0, d=4)])
    start = show.proc([Set(COUNTER, 0), Call(1667, 1, 1, 0, 0), Call(dlg)])
    stage = dream_stage(show)
    ents = show.proc([Call(916, 1440, 1440, 4, 0, 128, 0, 58, Proc(arrive), Proc(start))])
    scene = show.proc([Call(861, 56), Call(864, 0), Call(553), Call(1265), Call(1), Call(stage), Call(ents), Call(905),
                       Call(6557), Call(6558)])
    path = os.path.join(OUT, "vcmds.gba")
    show.save(path)
    st = run.state(path, scene, 58, "vcmds", wait=40)
    print(run.shots(path, st, [10 + 90 * k + d for k in range(12) for d in (45, 85)], "vcmds", scale=1))



def vseq():
    """Record Vanargand through a command sequence (VSEQ="cmd@frame,..."), D from VD (default 1)."""
    from show import actor
    import record
    show = Show(STRINGS, SLOTS)
    cut = Cutscene(show)
    cut.at(0, fade(0, 1, 0, 0, 0), cam(1200, 1200, 1, y=int(os.environ.get("VY", "900"))))
    seq = [tuple(int(v) for v in x.split("@")) for x in os.environ.get("VSEQ", "51@20,52@80").split(",")]
    for c, t in seq:
        cut.at(t, actor(0x7C56, 4, c))
    markers = os.environ.get("VMARK")      # "y1,y2": stranger sprites at (1200, y, 1200) for the first second
    if markers:
        import art
        pk, fr, _ = art.package(show, 0x6000, "stranger")
        from show import kf_add, kf_hide
        for k, y in enumerate(int(v) for v in markers.split(",")):
            cut.at(1, kf_add(0x7300 + k, pk, 1200, 1200, y=y, prio=int(os.environ.get("VPRIO", "2"))))
            if not os.environ.get("VKEEP"):
                cut.at(60, kf_hide(0x7300 + k))
    cut.hold(max(t for _, t in seq) + 200)
    done = show.proc([Call(1669)])
    dlg = cut.write(done)
    arrive = show.proc([command(0x9769, p=[1440, 406, 1440]), command(0x061E, i=0, d=4)])
    start = show.proc([Set(COUNTER, 0), Call(1667, 1, 1, 0, 0), Call(dlg)])
    stage = dream_stage(show, 8)
    ents = show.proc([Call(916, 1440, 1440, 4, 0, 128, 0, 58, Proc(arrive), Proc(start))])
    from gclasm import chara as chara_
    vd = int(os.environ.get("VD", "1"))
    spawn = [chara_(0x7C56, 0x7C56, D=vd, m=0)] if vd != 1 else [Call(6557)]
    scene = show.proc([Call(861, 56), Call(864, 0), Call(553), Call(1265), Call(1), Call(stage), Call(ents), Call(905)]
                      + spawn + [Call(6558)])
    path = os.path.join(OUT, "vseq.gba")
    show.save(path)
    st = run.state(path, scene, 58, "vseq", wait=40)
    out = os.path.join(OUT, "vseq.mp4")
    record.record(path, st, [], "vseq_rec", out, tail=int(os.environ.get("VLEN", max(t for _, t in seq) + 260)))



def timer():
    """A repeating timer (chara 430D) circles a sprite while a dialogue window blocks the cutscene."""
    import math
    import art
    import record
    from show import OT, kf_glide, track
    show = Show(STRINGS, SLOTS)
    pk, fr, _ = art.package(show, 0x6000, "stranger")
    cut = Cutscene(show)
    X, Z = 1152, 1152
    cut.at(0, fade(0, 1, 0, 0, 0), cam(X, Z))
    cut.at(1, kf_add(0x7300, pk, X + 300, Z))
    steps = []
    for k in range(120):
        a = 2 * math.pi * k / 40
        steps.append([kf_glide(0x7300, X + int(300 * math.cos(a)), Z + int(300 * math.sin(a)), 4)])
    cut.at(2, *track(0x7301, steps, every=4))
    cut.hold(30)
    cut.say([OT + '"Is it still moving?"'])
    cut.say([OT + '"It is!"'])
    cut.hold(120)
    scene = simple_scene(show, cut, 2718, X, Z + 300)
    path = os.path.join(OUT, "timer.gba")
    show.save(path)
    st = run.state(path, scene, 58, "timer", wait=30)
    record.record(path, st, cut.pages, "timer_rec", os.path.join(OUT, "timer.mp4"), shots=True)



def facing():
    """Each NPC package at 8 points around a center, facing it (part1.npc_facing)."""
    import math
    import part1
    from figures import Sprite
    show = Show(STRINGS, SLOTS)
    cut = Cutscene(show)
    CX, CZ = 1664, 1860
    cut.at(0, fade(0, 1, 0, 0, 0), cam(CX, CZ, 1, y=406))
    pkgs = [int(x, 16) for x in os.environ.get("PKGS", "DFFE").split(",")]
    k = 0
    for pkg in pkgs:
        for j in range(8):
            a = 2 * math.pi * j / 8
            x, z = CX + int(380 * math.cos(a)), CZ + int(380 * math.sin(a))
            f, m = part1.npc_facing(pkg, (x, z), (CX, CZ))
            sp = Sprite(cut, 0x7200 + k, pkg, {}).add(2, x, z, frame=f)
            sp.frame(3, f, mirror=m)
            k += 1
    import art
    pk, fr, _ = art.package(show, 0x6000, "stranger")
    Sprite(cut, 0x7300, pk, fr).add(2, CX, CZ, frame="stand_dr")
    cut.hold(120)
    scene = simple_scene(show, cut, 3878, 1664, 2944)
    path = os.path.join(OUT, "facing.gba")
    show.save(path)
    st = run.state(path, scene, 58, "facing", wait=30)
    print(run.shots(path, st, [200], "facing", scale=2))



def fades():
    """Which fade groups (bg, obj, other) tint which sprites: Claude and the
    digits (new pools), a coffin (a game pool), Django."""
    import part1
    from figures import Sprite
    show = Show(STRINGS, SLOTS)
    cast = part1.Cast(show)
    cut = Cutscene(show)
    X, Z, Y = 2176, 2400, 512
    cut.at(0, fade(0, 1, 0, 0, 0), cam(X, Z - 300, 1, y=Y + 150))
    Sprite(cut, 0x7005, cast.claude, cast.cf, y=Y).add(2, X - 200, Z - 400, frame="stand_dr")
    Sprite(cut, 0x7400, cast.coffins[0], {}, y=Y).add(2, X + 100, Z - 400, frame=0)
    Sprite(cut, 0x7480, cast.digits, {}, y=Y).add(2, X, Z - 600, frame=0)
    for k, (bg, obj, oth) in enumerate([(1, 0, 0), (0, 1, 0), (0, 0, 1)]):
        cut.at(40 + 60 * k, fade(1, 1, 31, 0, 0, bg, obj, oth, 0))
        cut.at(80 + 60 * k, fade(0, 1, 31, 0, 0, 1, 1, 1, 0))
    cut.hold(260)
    scene = simple_scene(show, cut, 2850, X, Z, flags=56)
    path = os.path.join(OUT, "fades.gba")
    show.save(path)
    st = run.state(path, scene, 58, "fades", wait=30)
    print(run.shots(path, st, list(range(60, 420, 30)), "fades", scale=1))


SFX = [220, 221, 222, 263, 266, 402, 540, 628, 893, 205, 199, 200, 201, 202, 641, 1243, 208, 1414, 1268, 1244, 853,
       219, 1425, 1261, 1140, 894, 854, 747, 606, 1402, 1342, 1262, 1242, 1132, 103, 899, 868, 861, 811, 742, 740,
       531, 488, 349, 244, 234, 231, 230, 1480, 1464, 1427, 1408, 1365, 1338, 1299, 1298, 1251, 1247, 1246, 1241,
       1136, 1131, 1407]
SFX_GAP = 60


def sfx():
    """An audition: each sound in SFX, SFX_GAP frames apart, recorded with
    sound on a quiet stage (the edit picks a snap and an ominous arrival)."""
    import record
    show = Show(STRINGS, SLOTS)
    cut = Cutscene(show)
    X, Z = 1152, 1152
    from show import bgm_out
    cut.at(0, fade(0, 1, 0, 0, 0), cam(X, Z, 1), bgm_out(1))
    for k, n in enumerate(SFX):
        cut.at(120 + SFX_GAP * k, sound(n))
    cut.hold(120 + SFX_GAP * len(SFX) + 120)
    scene = simple_scene(show, cut, 2718, X, Z + 300)
    path = os.path.join(OUT, "sfx.gba")
    show.save(path)
    st = run.state(path, scene, 58, "sfx", wait=2)
    out = os.path.join(OUT, "sfx.mp4")
    record.record(path, st, [], "sfx_rec", out, tail=120 + SFX_GAP * len(SFX) + 200)


def arena():
    """The Pile Driver's arena (scene 6201's stage), with and without the
    Pile Driver object (proc_6182), for the coffin piece (ARENA_PD=1)."""
    import part1
    show = Show(STRINGS, SLOTS)
    X, Z, Y = 1920, 1920, 512
    sc = part1.Scene(show, 6201, X, Z + 700, facing=1, y=Y, song=0, camera=(X, Z))
    if os.environ.get("ARENA_PD"):
        sc.spawns = [Call(6182, 0)]
    sc.cut.hold(400)
    pid = sc.build()
    path = os.path.join(OUT, "arena.gba")
    show.save(path)
    st = run.state(path, pid, 58, "arena", wait=60)
    print(run.shots(path, st, [10, 120, 300], "arena", scale=1))


def kneel():
    """Django hit and down on one knee: the player commands one at a time
    (43 shake, 12 kneel_down, 13 kneel_gasp, 14 get_up, 11 kneel)."""
    import part1
    show = Show(STRINGS, SLOTS)
    X, Z, Y = 1664, 2050, 512
    sc = part1.Scene(show, 3243, X, Z, facing=1, y=Y, song=0, camera=(X, Z))
    dj = sc.player
    seq = [[int(x) for x in v.split(":")] for v in os.environ.get("KSEQ", "43,12:1,13:1,14:1,11:1").split(",")]
    gap = int(os.environ.get("KGAP", "80"))
    for k, c in enumerate(seq):
        dj.raw(30 + gap * k, *c)
    sc.cut.hold(30 + gap * len(seq) + 60)
    pid = sc.build()
    path = os.path.join(OUT, "kneel.gba")
    show.save(path)
    st = run.state(path, pid, 58, "kneel", wait=30)
    import record
    gap = int(os.environ.get("KGAP", "80"))
    times = [20] + [30 + gap * k + d for k in range(len(seq)) for d in range(5, gap, gap // 6)]
    lines = [f"loadstate {st}", f"watchpc {record.SCRIPT_START:08X} -2147483648 2147483647", "waitpc 20000"]
    now = 0
    for t in times:
        lines += [f"wait {t - now}", f"shot k{t:05d}"]
        now = t
    d = run.harness(path, lines, "kneel")
    import subprocess
    files = [os.path.join(d, f"k{t:05d}.png") for t in times]
    subprocess.run(["magick", "montage", "-font", "/System/Library/Fonts/Supplemental/Arial.ttf", *files, "-tile", "6x",
                    "-geometry", "240x160+2+2", os.path.join(OUT, "kneel.png")], check=True)
    print(os.path.join(OUT, "kneel.png"))


if __name__ == "__main__":
    globals()[sys.argv[1]]()
