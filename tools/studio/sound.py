#!/usr/bin/env python3
"""The game's sound for the Studio (tools/studio/sound.html): every MP2K song
in the song table with its tracks, voicegroup and size, where it is used, and
the bytes a song needs to play in the browser (tools/studio/mp2k.js).

  python3 tools/studio/sound.py            summary
  python3 tools/studio/sound.py N          one song as JSON

Where a song is used:

* scripts: calls to play_bgm / play_bgm_with_fade / play_bgm_fade (procs
  1048-1051) with a constant id, and the song-start script commands 53BF and
  E7CA (sub_0822AE7C: start, remember for resuming) and 1870 (m4aSongNumStart);
* scenes: the scene scripts that make those calls, directly or through library
  procedures (as Browse places objects);
* code: functions that pass a constant song number to Sound_PlaySong,
  Sound_PlaySongChecked, the m4aSongNumStart family or the wrappers above;
* the sound test's three lists (gUnk_086136F0, 086137AE, 08613BC4).

Song groups are the game's music players (the song table's ms field): player
10 is the 12-track BGM player; the others are effect players with 1-8 tracks.

Built from tools/m4a.py, the GCL scripts and gen/code_sym.s; cached in
build/cache/sound.json until an input changes. Nothing rendered is stored:
the browser synthesizes the audio from ROM bytes served by song_data().
"""
import base64
import collections
import csv
import functools
import json
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
for p in (os.path.join(ROOT, "tools", "assets"), os.path.join(ROOT, "tools"), HERE):
    if p not in sys.path:
        sys.path.insert(0, p)

import m4a  # noqa: E402
from datafmt import load_manifest, sound_test as sound_test_format  # noqa: E402

CACHE = os.path.join(ROOT, "build", "cache", "sound.json")
INPUTS = ["tools/studio/sound.py", "tools/m4a.py", "gen/code_sym.s", "gcl/names/proc.csv", "tools/assets/gclroles.py",
          "data/labels/scenes.csv", "data/labels/areas.csv"]
ROM_BASE = 0x08000000
PLAYER_TABLE = 0x082643EC  # gMPlayTable: {MusicPlayerInfo*, MusicPlayerTrack*, u16 ntracks, u16 unk}
BGM_PROCS = {1048: ("play_bgm", 0), 1049: ("play_bgm_with_fade", 1), 1050: ("play_bgm_urgent", 0), 1051: ("play_bgm_fade", 1)}
START_CMDS = {0x53BF: "start song (53BF)", 0xE7CA: "start song (E7CA)", 0x1870: "start song, raw (1870)"}
CODE_PLAY = {"Sound_PlaySong", "Sound_PlaySongChecked", "m4aSongNumStart", "m4aSongNumStartOrChange",
             "m4aSongNumStartOrContinue", "sub_0822AE7C", "sub_0822B330"}
KINDS = {0: "DirectSound", 1: "Square 1", 2: "Square 2", 3: "Wave", 4: "Noise", 8: "DirectSound (fixed)",
         9: "Square 1", 10: "Square 2", 11: "Wave", 12: "Noise"}


@functools.lru_cache(None)
def rom():
    from romlib import load_rom
    return load_rom()


def u32(a):
    return struct.unpack_from("<I", rom(), a - ROM_BASE)[0]


def players():
    out = {}
    for p in range(32):
        info, _, n = struct.unpack_from("<III", rom(), PLAYER_TABLE + 12 * p - ROM_BASE)
        if info:
            out[p] = n & 0xFFFF
    return out


def sound_test():
    """All menu positions of each song, including songs listed more than once."""
    positions = collections.defaultdict(list)
    for entry in load_manifest():
        if entry.format != "sound_test":
            continue
        if not os.path.exists(entry.file):
            entry.extract()
        with open(entry.file, encoding="utf-8") as source:
            songs = sound_test_format.songs(source.read(), entry.start, entry.end)
        category = sound_test_format.LISTS[entry.start][0]
        for number, song in enumerate(songs, 1):
            positions[song].append({"list": category, "no": number})
    return positions


def walk_song(song, ntracks, vg):
    """(tracks [{addr, end, size, patterns, voices}], voicegroups, samples) of one song."""
    tracks = []
    w = m4a.Walker(rom())
    allvoices = set()
    for t in range(ntracks):
        tp = u32(song + 8 + 4 * t)
        one = m4a.Walker(rom())
        one.walk_track(tp)
        allvoices |= one.voices
        w.tracks |= one.tracks
        for k, v in one.track_extent.items():
            w.track_extent[k] = max(w.track_extent.get(k, 0), v)
        tracks.append({"addr": f"{tp:08X}", "end": f"{one.track_extent[tp]:08X}", "size": one.track_extent[tp] - tp,
                       "patterns": sorted(f"{a:08X}" for a in one.tracks if a != tp), "voices": sorted(one.voices)})
    if vg:
        w.walk_voicegroup(vg)
    return tracks, w, allvoices


def _sizes(w):
    """Bytes of track data a walk covers, overlaps counted once."""
    spans = sorted((a, e) for a, e in w.track_extent.items())
    total, end = 0, 0
    for a, e in spans:
        if e > end:
            total += e - max(a, end)
            end = e
    return total


# ---------- usage ----------
def script_uses():
    """{song: [{proc, via}]} from the GCL scripts: constant song ids passed to
    the play_bgm procedures, to procedures that hand a parameter on to them
    (found to a fixpoint), stored in a variable that is later passed to them,
    or given to the song-start commands."""
    import actors
    import gclview
    img = gclview._image()
    names = {int(p): r["name"] for p, r in gclview.names().procs.items()}
    bodies = {cid: gclview.procedure(img, cid) or [] for cid in actors.procs()}
    params = {p: {i} for p, (_, i) in BGM_PROCS.items()}  # proc -> argument positions that are a song
    svars = set()                                          # variables passed as a song

    def stmts_of(stmts):
        for s in stmts:
            yield s
            for x in s[1:]:
                yield from blocks(x)

    def blocks(x):
        if isinstance(x, tuple):
            if x and x[0] == "block":
                yield from stmts_of(x[1])
            else:
                for y in x:
                    yield from blocks(y)
        elif isinstance(x, list):
            for y in x:
                yield from blocks(y)

    def var(v):
        return (v[1], v[2]) if v[0] == "var" and v[5] is None else None

    def scan(record):
        changed = False
        for pid, body in bodies.items():
            for s in stmts_of(body):
                if s[0] == "call" and s[1] in params:
                    for i in params[s[1]]:
                        if len(s[2]) <= i:
                            continue
                        v = s[2][i]
                        if v[0] == "int" and v[1] > 0 and record is not None:
                            record[v[1]].append({"proc": pid, "via": BGM_PROCS.get(s[1], (names.get(s[1]) or f"proc {s[1]}",))[0]})
                        elif v[0] == "arg" and v[1] - 1 not in params.setdefault(pid, set()):
                            params[pid].add(v[1] - 1)
                            changed = True
                        elif var(v) and var(v) not in svars and var(v) != ("r708", 0x74):  # 0x74: the previous track
                            svars.add(var(v))
                            changed = True
                elif s[0] == "eval" and s[1][0] == "bin" and s[1][1] == 22 and var(s[1][2]) in svars:
                    v = s[1][3]
                    if v[0] == "int" and v[1] > 0 and record is not None:
                        record[v[1]].append({"proc": pid, "via": "sets {}[0x{:x}]".format(*var(s[1][2]))})
                    elif v[0] == "arg" and v[1] - 1 not in params.setdefault(pid, set()):
                        params[pid].add(v[1] - 1)
                        changed = True
                elif s[0] == "cmd" and s[2] in START_CMDS and record is not None:
                    for letter, vals in s[4]:
                        if letter == "i" and vals and vals[0][0] == "int":
                            record[vals[0][1]].append({"proc": pid, "via": START_CMDS[s[2]]})
        return changed
    while scan(None):
        pass
    out = collections.defaultdict(list)
    scan(out)
    return out


def code_uses():
    """{song: [{func, addr, at}]}: constant song numbers passed to the play
    functions, from gen/code_sym.s (r0 set after the last label)."""
    out = collections.defaultdict(list)
    path = os.path.join(ROOT, "gen", "code_sym.s")
    func = faddr = None
    r0 = None
    for ln in open(path):
        s = ln.strip()
        m = re.match(r"thumb_func_start (\S+)|arm_func_start (\S+)", s)
        if m:
            func, r0 = m.group(1) or m.group(2), None
            continue
        m = re.match(r"(\S+): @ 0x([0-9A-F]{8})", s)
        if m:
            if m.group(1) == func:
                faddr = m.group(2)
            r0 = None
            continue
        if s.endswith(":") or s.startswith("_") and ":" in s.split()[0]:
            r0 = None
            continue
        m = re.match(r"movs r0, #(0x[0-9a-f]+|\d+)$", s)
        if m:
            r0 = int(m.group(1), 0)
            continue
        m = re.match(r"ldr r0, \S+ @ =(0x[0-9A-Fa-f]+)$", s)
        if m:
            r0 = int(m.group(1), 16)
            continue
        m = re.match(r"lsls r0, r0, #(0x[0-9a-f]+|\d+)$", s)
        if m and r0 is not None:
            r0 = (r0 << int(m.group(1), 0)) & 0xFFFFFFFF
            continue
        m = re.match(r"adds r0, (?:r0, )?#(0x[0-9a-f]+|\d+)$", s)
        if m and r0 is not None:
            r0 += int(m.group(1), 0)
            continue
        m = re.match(r"bl (\S+)$", s)
        if m:
            if m.group(1) in CODE_PLAY and r0 is not None and 0 < r0 < 0x10000:
                out[r0].append({"func": func, "addr": faddr, "via": m.group(1)})
            r0 = None
            continue
        if re.match(r"\w+ r0\b", s) and not re.match(r"(cmp|str\w*|tst|cmn) ", s):
            r0 = None
    return out


@functools.lru_cache(None)
def _files():
    import gclfiles
    callers = collections.defaultdict(set)
    for c, (calls, _) in gclfiles.refs().items():
        for t in calls:
            callers[t].add(c)
    return gclfiles.file_of(), {f["id"]: f for f in gclfiles.files()}, callers


def scenes_of(procs):
    """The scene entries whose own scripts run these procedures, directly or
    through library procedures (up to three calls up), and shared files."""
    fof, fkind, callers = _files()
    scenes, via = set(), set()
    todo, seen = [(p, 0) for p in procs], set()
    while todo:
        p, d = todo.pop()
        if p in seen:
            continue
        seen.add(p)
        f = fkind.get(fof.get(p))
        if not f:
            continue
        if f["kind"] == "scene" and f.get("entry") is not None:
            scenes.add(f["entry"])
        elif f["kind"] == "library" and d < 3:
            todo += [(c, d + 1) for c in callers.get(p, ())]
        else:
            via.add(f["id"])
    return sorted(scenes), sorted(via)


# ---------- the catalog ----------
def build():
    import gclfiles
    import gclview
    w = m4a.walk(rom())
    pl = players()
    test = sound_test()
    su, cu = script_uses(), code_uses()
    procnames = {int(p): r["name"] for p, r in gclview.names().procs.items()}
    fof = gclfiles.file_of()
    import areas
    scene_area = {int(k): v for k, v in areas.build()["scene_area"].items()}
    slab = {int(r["scene"]): r.get("label") or "" for r in csv.DictReader(open(os.path.join(ROOT, "data", "labels", "scenes.csv"), encoding="utf-8"))}
    alab = {int(r["area"]): r.get("place") or "" for r in csv.DictReader(open(os.path.join(ROOT, "data", "labels", "areas.csv"), encoding="utf-8"))}

    hdr_count = collections.Counter(s for _, s, _, _ in w.songs)
    vg_users = collections.defaultdict(list)
    songs = []
    for n, song, nt, vg in w.songs:
        ms, me = struct.unpack_from("<HH", rom(), m4a.SONG_TABLE + 8 * n + 4 - ROM_BASE)
        nblocks, prio, reverb = rom()[song + 1 - ROM_BASE], rom()[song + 2 - ROM_BASE], rom()[song + 3 - ROM_BASE]
        t = test.get(n, [])
        if nt == 0:
            songs.append({"test": t[0] if t else None, "test_positions": t, "n": n, "addr": f"{song:08X}", "tracks": [], "empty": True, "shared": hdr_count[song], "player": ms})
            continue
        tracks, sw, voices = walk_song(song, nt, vg)
        vg_users[vg].append(n)
        uses = su.get(n, [])
        procs = sorted({u["proc"] for u in uses})
        sc, shared = scenes_of(procs)
        songs.append({
            "n": n, "addr": f"{song:08X}",
            "player": ms, "player_tracks": pl.get(ms, 0), "priority": prio, "reverb": reverb, "blocks": nblocks,
            "voicegroup": f"{vg:08X}", "voices": sorted(voices),
            "tracks": tracks, "size": 8 + 4 * nt + _sizes(sw), "start": min(int(x["addr"], 16) for x in tracks),
            "test": t[0] if t else None, "test_positions": t,
            "scripts": [dict(u, name=procnames.get(u["proc"], ""), file=fof.get(u["proc"])) for u in uses],
            "scenes": [{"scene": s, "title": slab.get(s, ""), "area": scene_area.get(s, -1),
                        "area_title": alab.get(scene_area.get(s, -1), "")} for s in sc],
            "shared_files": shared,
            "code": cu.get(n, []),
        })
    # voicegroups and samples, in ROM order
    vgs = []
    for vg in sorted(w.voicegroups):
        vgs.append({"addr": f"{vg:08X}", "songs": vg_users.get(vg, [])})
    samples = []
    for a, e in sorted(w.samples.items()):
        if e - a > 16 + 1:
            freq = u32(a + 4)
            samples.append({"addr": f"{a:08X}", "size": e - a, "rate": round(freq / 1024), "loop": bool(rom()[a + 3 - ROM_BASE] & 0x40)})
        else:
            samples.append({"addr": f"{a:08X}", "size": e - a, "wave": True})
    return {"songs": songs, "players": {str(k): v for k, v in pl.items()}, "voicegroups": vgs, "samples": samples,
            "table": {"addr": f"{m4a.SONG_TABLE:08X}", "size": 8 * len(w.songs)}}


def titles():
    """{song number: name} from data/notes.csv (kind "song"), which the Sound
    page edits; read on every request so a new name shows at once."""
    p = os.path.join(ROOT, "data", "notes.csv")
    if not os.path.exists(p):
        return {}
    return {int(r["key"]): r["label"] for r in csv.DictReader(open(p, encoding="utf-8"))
            if r["kind"] == "song" and r["key"].isdigit() and r.get("label")}


def load():
    inputs = [os.path.join(ROOT, p) for p in INPUTS + ["data/manifest.csv", "tools/datafmt/sound_test.py"]]
    inputs.extend(e.file for e in load_manifest() if e.format == "sound_test")
    newest = max(os.path.getmtime(p) for p in inputs if os.path.exists(p))
    if os.path.exists(CACHE) and os.path.getmtime(CACHE) >= newest:
        return json.load(open(CACHE))
    out = build()
    os.makedirs(os.path.dirname(CACHE), exist_ok=True)
    json.dump(out, open(CACHE + ".tmp", "w"), ensure_ascii=False)
    os.replace(CACHE + ".tmp", CACHE)
    return out


# ---------- bytes for the browser player ----------
def _voice_spans(vg, progs, samples, spans, depth=0):
    """Add the ROM ranges voices progs of voicegroup vg read: the entries,
    keysplit tables, sub-voicegroups and wave data."""
    r = rom()
    for i in progs:
        e = vg + 12 * i
        spans.append((e, e + 12))
        kind = r[e - ROM_BASE]
        p1 = u32(e + 4)
        if not m4a.is_rom(p1, len(r)):
            continue
        if kind & 0x40 and depth < 2:          # keysplit: table of 128 keys, sub-voicegroup
            tbl = u32(e + 8)
            spans.append((tbl, tbl + 128))
            _voice_spans(p1, sorted(set(r[tbl - ROM_BASE: tbl - ROM_BASE + 128])), samples, spans, depth + 1)
        elif kind & 0x80 and depth < 2:        # rhythm: one voice per key
            _voice_spans(p1, range(128), samples, spans, depth + 1)
        elif kind & 0x07 == 0 and p1 in samples:
            spans.append((p1, samples[p1]))
        elif kind & 0x07 == 3:
            spans.append((p1, p1 + 16))


def _chunks(spans):
    r = rom()
    merged = []
    for a, b in sorted(spans):
        if merged and a <= merged[-1][1] + 16:
            merged[-1][1] = max(merged[-1][1], b)
        else:
            merged.append([a, b])
    return [[f"{a:08X}", base64.b64encode(r[a - ROM_BASE:b - ROM_BASE]).decode()] for a, b in merged]


@functools.lru_cache(None)
def _walk_all():
    return m4a.walk(rom())


def song_data(n):
    """{"song", "n", "chunks": [[addr, base64]]}: every ROM range song n reads
    when it plays (header, track streams with their patterns, the voicegroup
    entries it selects with keysplit tables and sub-voicegroups, wave data)."""
    r = rom()
    if not 0 <= n < 0x10000:
        return None
    song = u32(m4a.SONG_TABLE + 8 * n)
    if not m4a.is_rom(song, len(r)):
        return None
    nt, vg = r[song - ROM_BASE], u32(song + 4)
    spans = [(song, song + 8 + 4 * nt)]
    _, w, voices = walk_song(song, nt, 0)
    spans += list(w.track_extent.items())
    if m4a.is_rom(vg, len(r)):
        _voice_spans(vg, sorted(voices | {0}), _walk_all().samples, spans)
    return {"song": f"{song:08X}", "n": n, "chunks": _chunks(spans)}


def voicegroup_data(vg):
    """All 128 voices of a voicegroup, with what they read."""
    if vg not in _walk_all().voicegroups:
        return None
    spans = []
    _voice_spans(vg, range(128), _walk_all().samples, spans)
    return {"voicegroup": f"{vg:08X}", "chunks": _chunks(spans)}


def sample_data(a):
    w = _walk_all()
    if a not in w.samples:
        return None
    return {"sample": f"{a:08X}", "chunks": _chunks([(a, w.samples[a])])}


def main():
    cat = load()
    if len(sys.argv) > 1:
        n = int(sys.argv[1], 0)
        print(json.dumps(next((s for s in cat["songs"] if s["n"] == n), None), ensure_ascii=False, indent=1))
        return
    real = [s for s in cat["songs"] if not s.get("empty")]
    print(f"{len(cat['songs'])} song table entries, {len(real)} songs, {len(cat['voicegroups'])} voicegroups, {len(cat['samples'])} samples")
    print(f"scripts start {sum(1 for s in real if s['scripts'])} songs, code {sum(1 for s in real if s['code'])}, "
          f"in scenes {sum(1 for s in real if s['scenes'])}, sound test {sum(1 for s in real if s['test'])}")
    by = collections.Counter(s["player"] for s in real)
    print("players:", dict(sorted(by.items())))


if __name__ == "__main__":
    main()
