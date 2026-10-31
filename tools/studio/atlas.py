#!/usr/bin/env python3
"""The Studio's game index: every kind of thing in the game, organized as a
tree, each with one page of everything known about it (tools/studio/atlas.html).

  python3 tools/studio/atlas.py            build and print a summary
  python3 tools/studio/atlas.py ID         print one entity (e.g. actor:05A5)

Entities have an id "kind:key": char (the game's character list), monster
(the game's monster list, bosses included), actor (a registry entry: an
object scripts create with `chara KEY`, or a script command), item, area,
scene, file (a reconstructed GCL source file), sprite (98F5 large sprite),
tileset (a CEEF tileset), bgtiles (a map's own tiles), song (an MP2K song)
and func (a code function). The tree groups them:

* Cast and Bestiary: the game's own name lists, in its order;
* Objects (registry entries whose callback or loader loads a sprite, or that
  allocate an object) and Script routines (the rest), by the code subsystem
  their callback is in (the translation units; unmatched code takes its ROM
  neighbors' subsystem), in ROM order;
* Places: areas in story order, their scenes;
* Items: data/items.csv by category;
* Scripts: the GCL source files by category (tools/assets/gclfiles.py);
* Sprites: large sprites and sprite packages in archive (ROM) order;
* Tiles: the CEEF tileset library's tilesets by the area that uses them,
  and the maps that carry their own tiles (tools/assets/tilesets.py);
* Sound: the MP2K songs by the music player the song table assigns them
  (player 10 is the BGM player), from tools/studio/sound.py;
* Code: subsystems in ROM order, with their functions and decomp status.

rom_tree() is the ROM as nested blocks for tools/studio/rom.html.

Built from the existing tools (actors.py, stages, areas.py, bigpalettes.py,
paltrace.py families, script_targets.progress) and cached in
build/cache/atlas.json until an input changes.
"""
import collections
import csv
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
for p in (os.path.join(ROOT, "tools", "assets"), os.path.join(ROOT, "tools"), HERE):
    if p not in sys.path:
        sys.path.insert(0, p)

CACHE = os.path.join(ROOT, "build", "cache", "atlas.json")
INPUTS = ["data/actor_registry.csv", "data/items.csv", "data/notes.csv",
          "data/sprite_palettes.csv", "data/labels/areas.csv", "data/labels/scenes.csv",
          "data/labels/sprites.csv", "data/labels/palettes.csv", "gcl/names/proc.csv", "tools/assets/gclroles.py", "gcl/names/chara.csv",
          "gcl/names/files.csv", "symbols/functions.csv", "tools/studio/atlas.py", "tools/assets/actors.py",
          "tools/assets/tilesets.py", "data/stage_palettes.csv", "tools/studio/sound.py", "tools/m4a.py",
          "tools/assets/cutscenes.py", "tools/mgba/scenes.py", "tools/assets/gclfiles.py",
          "tools/assets/areas.py", "tools/assets/stages.py", "tools/assets/story.py",
          "tools/assets/saves.py", "tools/assets/player_cue.py", "data/player_cue/animations.csv",
          "data/manifest.csv", "tools/datafmt/player_animations.py"]
# the monster list's bosses: 黒騎士 and ニーズホッグ (9902-9903), then 伯爵 on (9915-)
BOSSES = {9902, 9903} | set(range(9915, 9922))


def words(name):
    """FloorSwitch_Create -> Floor switch."""
    base = re.sub(r"_(Create|CreateFromScript|StartFromScript|ScriptCreate|Command|Start|Init)$", "", name)
    base = base.replace("_", " ")
    s = re.sub(r"(?<=[a-z0-9])(?=[A-Z])", " ", base)
    return s[:1].upper() + s[1:].lower() if s.isascii() else s


def nice(s):
    """GHOUL + -> Ghoul+ (the fan translation's names are upper case)."""
    if not s or not s.isupper():
        return s
    t = re.sub(r"\s+\+", "+", s.title())
    return re.sub(r"(?<=[a-z])'S\b", "'s", t)


def speaker_portraits():
    """{speaker label: {"sprite", "frame", "pool"}}: the tokenizer's ids
    (sub_0803232C: strncmp against the table, then an id), the portrait
    animation 922E 4BBF (sequence = id, frame = key >> 6, over 593A's 16
    frames then 593B's) and the palette sub_0803092C selects per id."""
    import archive
    import struct as st_
    from romlib import load_rom
    rom = load_rom()

    def cstr(a):
        o = a - 0x08000000
        return rom[o:rom.index(b"\0", o)].decode()
    lines = open(os.path.join(ROOT, "gen", "code.s")).read().split("\n")
    # gen/code.s labels the function by its current name (symbols/functions.csv)
    fname = next((r["name"] for r in csv.DictReader(open(os.path.join(ROOT, "symbols", "functions.csv"), encoding="utf-8"))
                  if r["addr"].upper() == "0803232C"), "sub_0803232C")
    i0 = next(i for i, l in enumerate(lines) if l.strip() == f"thumb_func_start {fname}")
    ids, cur = {"R_DJANGO": 0, "B_DJANGO": 1, "W_DJANGO": 2}, None
    for l in lines[i0:i0 + 1200]:
        m = re.search(r"ldr r1, \S+ @ =0x(0824D[0-9A-F]{3})", l)
        if m:
            cur = cstr(int(m.group(1), 16))
            continue
        m = re.match(r"\s*movs r0, #(0x[0-9a-f]+|\d+)$", l)
        if m and cur:
            ids.setdefault(cur, int(m.group(1), 0))
            cur = None
    d = None
    for label, _, _, ents in archive.archives(rom):
        if label == "anm":
            for h, a, sz in ents:
                if h == 0x4BBF:
                    d = rom[a - 0x08000000:a - 0x08000000 + sz]
    if d is None:
        return {}
    n = st_.unpack_from("<H", d, 0)[0]
    pools = {4: 505, 5: 505, 13: 505, 19: 507, 20: 504, 25: 504, 30: 506}
    out = {}
    for label, sid in ids.items():
        if 0 <= sid < n:
            _, _, off = st_.unpack_from("<BBH", d, 4 + 4 * sid)
            f = st_.unpack_from("<H", d, off)[0] >> 6
            out[label] = {"sprite": "593A" if f < 16 else "593B", "frame": f if f < 16 else f - 16, "pool": pools.get(sid, 503), "id": sid}
    return out


def clean(t):
    """A name without the text bank's control sequences ($X=..., {00})."""
    return re.split(r"\$[A-Z]=|\{[0-9A-F]{2}\}", t)[0].strip() or t


def english_names():
    import serve
    jp, en = serve.display_strings(os.path.join(os.path.dirname(ROOT), "boktai3trans"))
    return jp, {k: nice(v) for k, v in en.items()}



def notes():
    out = {}
    p = os.path.join(ROOT, "data", "notes.csv")
    if os.path.exists(p):
        for r in csv.DictReader(open(p, encoding="utf-8")):
            out[r["kind"], r["key"]] = r
    return out


def location(place):
    """("Ancient Tree", "Side room") from "Ancient Tree (side room)"."""
    m = re.match(r"(.+?)(?: \((.+)\)| / (.+)| (route transition.*))?$", place)
    loc, sub = m.group(1).strip(), next((g for g in m.groups()[1:] if g), "")
    return loc, (sub[:1].upper() + sub[1:]) if sub else "Main area"


node_rank = {}


def build():
    import actors
    import areas
    import bigpalettes
    import gclfiles
    import gclview
    import paltrace
    import script_targets

    jp, en = english_names()
    N = notes()
    E, tree_items = {}, collections.defaultdict(list)  # tree path -> [entity id]

    def add(eid, path, **fields):
        E[eid] = dict(fields, id=eid, path=path)
        if path:
            tree_items[path].append(eid)
        return E[eid]

    prog = script_targets.progress()
    an = paltrace.addr_names()
    byname = {v: k for k, v in an.items()}
    funcinfo = {}

    def func(name):
        a = byname.get(name)
        if a is None:
            return None
        size, matched, nm, unit = prog.get(a, (0, False, name, ""))
        funcinfo[name] = {"addr": f"{a:08X}", "name": nm, "size": size, "matched": matched, "unit": unit}
        return name

    # ---- code subsystems, from the translation units (source files are
    # contiguous in ROM): an unmatched function takes the subsystem of its
    # neighbors when both agree, else an address bucket ----
    order_a = sorted(a for a in prog if a in an)

    def unit_sub(u):
        top, _, sub = (u or "").partition("/")
        if top == "game":
            return re.sub(r"/register-pinned$", "", sub)
        if top == "libraries":
            return "Library: " + re.sub(r"/register-pinned$", "", sub)
        return None
    named = [unit_sub(prog[a][3]) for a in order_a]
    subsys = {}
    prev = [None] * len(order_a)
    nxt = [None] * len(order_a)
    last = None
    for i, n in enumerate(named):
        last = n or last
        prev[i] = last
    last = None
    for i in range(len(named) - 1, -1, -1):
        last = named[i] or last
        nxt[i] = last
    subsys_at = {}
    for i, a in enumerate(order_a):
        subsys_at[a] = named[i] or (prev[i] if prev[i] and prev[i] == nxt[i] else f"Unsorted {a >> 16:04X}xxxx")
        subsys[an[a]] = subsys_at[a]
    subsys_start = {}
    for a in order_a:
        subsys_start.setdefault(subsys_at[a], a)

    # ---- registry entries ----
    A = actors.actors()
    procnames = {int(p): r["name"] for p, r in gclview.names().procs.items()}
    fams = paltrace.families()
    sprite_owner = collections.defaultdict(list)
    names_of = collections.defaultdict(list)  # monster/character string id -> actor keys
    for a in A:
        for n in a["names"]:
            names_of[n["id"]].append(a["key"])
    monster_list = actors.name_list(*actors.ENEMY_NAMES)
    monster_ids = {n["id"] for n in monster_list}
    # a species whose English name is an object's code name (Dragon Puppy:
    # DragonPuppy_Create), or that name plus a suffix (Skeleton Archer: Skeleton)
    squash = lambda t: re.sub(r"[^a-z]", "", t.lower())
    for a in A:
        f = a["callback_name"]
        creator = a["loader"] or re.search(r"_(Create|CreateFromScript|StartFromScript|ScriptCreate)$", f)
        if f.startswith("sub_") or not creator or any(n["id"] in monster_ids for n in a["names"]):
            continue
        base = squash(re.sub(r"_(Create|CreateFromScript|StartFromScript|ScriptCreate|Command|Start|Init)$", "", f))
        if len(base) < 4:
            continue
        for n in monster_list:
            e = (en.get(n["id"]) or "").lower()
            if e and (squash(e) == base or (e.startswith(words(f).lower() + " ") and squash(e).startswith(base))):
                a["names"].append({"id": n["id"], "jp": n["jp"], "via": "name"})
                names_of[n["id"]].append(a["key"])
    stage = areas.build()
    scene_area = {int(k): v for k, v in stage["scene_area"].items()}
    # placement: the scenes whose own script creates the object, directly or
    # through library procedures it calls (not shared scripts many scenes run)
    fof = gclfiles.file_of()
    fkind = {f["id"]: f for f in gclfiles.files()}
    callers = collections.defaultdict(set)
    for c, (calls, _) in gclfiles.refs().items():
        for t in calls:
            callers[t].add(c)

    def placed_in(procs):
        scenes, via = set(), collections.Counter()
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
                via[f["id"]] += 1
        return sorted(scenes), sorted(via)

    for a in A:
        gname = a["gcl"][0]["english"] if a["gcl"] and a["gcl"][0].get("english") else ""
        fname = a["callback_name"]
        note = N.get(("actor", a["key"]), {}).get("label")
        mnames = [n for n in a["names"] if n["id"] in monster_ids]
        title = note or (en.get(mnames[0]["id"]) if mnames else "") or gname or (words(fname) if not fname.startswith("sub_") else "")
        if mnames:
            title = note or " · ".join(dict.fromkeys(en.get(n["id"]) or n["jp"] for n in mnames))
        # an object: the callback allocates one and hands it to a loader
        # (actors.loader_of); a script routine does its work and returns
        sub = subsys_at.get(int(a["callback"], 16) & ~1, "Unsorted")
        path = None  # Objects or Script routines, once its sprites are known
        fam = sorted(fams.get(fname, ()))
        code = [f for f in fam if func(f)]
        scenes, shared = placed_in(a["procs"])
        by_area = collections.defaultdict(list)
        for s in scenes:
            by_area[scene_area.get(s, -1)].append(s)
        sprites = []
        for s in a["sprites"]:
            if s["kind"] == "big":
                sprites.append(s["hash"])
                sprite_owner[s["hash"]].append(a["key"])
        add(f"actor:{a['key']}", path, kind="actor", key=a["key"], order=int(a["callback"], 16) & ~1,
            title=title or f"{'Object' if a['loader'] else 'Routine'} {a['key']}", fname=fname, gcl=a["gcl"][:1],
            subsystem=sub, allocates=bool(a["loader"]),
            names=[{"id": n["id"], "jp": n["jp"], "en": en.get(n["id"], "")} for n in a["names"]],
            callback=a["callback"], loader=a["loader"], loader_name=a["loader_name"],
            sprites=sprites, effects=[], small=[s["hash"] for s in a["sprites"] if s["kind"] != "big"],
            procs=[{"id": p, "name": procnames.get(p, "")} for p in a["procs"]], spawns=a["spawns"],
            areas={str(k): v for k, v in by_area.items()}, nscenes=len(scenes), shared=shared,
            reach=len(a["scenes"]), code=code)

    # ---- the game's monster and character lists ----
    for n in actors.name_list(*actors.ENEMY_NAMES):
        keys = names_of.get(n["id"], [])
        boss = n["id"] in BOSSES
        add(f"monster:{n['id']}", "Bestiary" + ("/Bosses" if boss else ""), kind="monster",
            key=str(n["id"]), title=clean(en.get(n["id"]) or n["jp"]), jp=n["jp"], order=n["id"] - actors.ENEMY_NAMES[0] + 1,
            actors=[f"actor:{k}" for k in keys])
    for n in actors.name_list(*actors.CHARACTER_NAMES):
        add(f"char:{n['id']}", "Cast", kind="char", key=str(n["id"]), title=clean(en.get(n["id"]) or n["jp"]),
            jp=n["jp"], order=n["id"] - actors.CHARACTER_NAMES[0], actors=[f"actor:{k}" for k in names_of.get(n["id"], [])], sprites=[])

    # ---- places ----
    area_rows = {int(r["area"]): r for r in csv.DictReader(open(os.path.join(ROOT, "data", "labels", "areas.csv"), encoding="utf-8"))}
    scene_rows = {int(r["scene"]): r for r in csv.DictReader(open(os.path.join(ROOT, "data", "labels", "scenes.csv"), encoding="utf-8"))}
    st = {s["scene"]: s for s in areas.load_stages()}
    for ar in stage["areas"]:
        i = ar["area"]
        row = area_rows.get(i, {})
        order = int(row.get("story_order") or 0)
        place = row.get("place") or ar.get("label") or f"Area {i}"
        loc, short = location(place)
        node_rank["Places/" + loc] = min(node_rank.get("Places/" + loc, 999), order or 900)
        add(f"area:{i}", "Places/" + loc, kind="area", key=str(i), title=place, short=short, location=loc, order=i,
            confidence=row.get("confidence", ""), scenes=[f"scene:{s}" for s in ar["scenes"]], maps=ar.get("maps", []),
            enemies=ar.get("enemies", {}))
        for s in ar["scenes"]:
            r = scene_rows.get(s, {})
            sd = st.get(s, {})
            add(f"scene:{s}", None, kind="scene", key=str(s), title=r.get("label") or f"Scene {s}", area=f"area:{i}",
                floor=r.get("floor", ""), stage=sd.get("stage"), palette=sd.get("palette"),
                doors=[{"scene": d.get("scene"), "entrance": d.get("entrance"), "cond": d.get("cond", [])} for d in sd.get("doors", [])],
                items=[{"category": x.get("category"), "item": x.get("item"), "name": x.get("name"), "chest": x.get("chest")} for x in sd.get("items", [])],
                enemies=[{"proc": x.get("proc"), "families": x.get("families", [])} for x in sd.get("enemies", [])])
    for k, e in list(E.items()):
        if e["kind"] == "actor":
            for ar in e["areas"]:
                if f"area:{ar}" in E:
                    E[f"area:{ar}"].setdefault("actors", []).append(k)

    # ---- items ----
    cats = {"item": "Items", "sword": "Swords", "accessory": "Accessories"}
    for r in csv.DictReader(open(os.path.join(ROOT, "data", "items.csv"), encoding="utf-8")):
        c = r["category"]
        found = [f"scene:{sid}" for sid, sd in st.items() if any(x.get("name") == r["jp"] for x in sd.get("items", []))]
        add(f"item:{c}/{r['id']}", "Items/" + cats.get(c, c.title() + "s"), kind="item", key=f"{c}/{r['id']}",
            title=nice(r["en"]) or r["jp"], jp=r["jp"], string=r["string"], icon=r["icon"], category=c, found=found,
            order=int(r["id"], 16))

    # ---- scripts ----
    for f in gclfiles.files():
        cat = f.get("category") or "Other"
        title = f["title"]
        if f["kind"] == "scene" and f.get("entry") is not None:
            sc = E.get(f"scene:{f['entry']}")
            ar = E.get(sc["area"]) if sc else None
            path = "Scripts/Scenes/" + (ar["location"] if ar else "No area")
            if sc and not sc["title"].startswith("Scene "):
                title = f"{f['entry']} · {sc['title']}"
            if sc:
                sc["file"] = f"file:{f['id']}"
        else:
            path = "Scripts/" + ("Shared" if f["kind"] == "shared" else "Library/" + cat)
        add(f"file:{f['id']}", path, kind="file", key=f["id"], title=title, description=f.get("description", ""),
            nprocs=len(f["procs"]), procs=f["procs"][:400], scenes=f.get("scenes"), fkind=f["kind"], entry=f.get("entry"))

    # ---- who speaks where: dialogue strings start with the speaker's name
    # (おてんこさま 「...), and each scene's script names the strings it shows ----
    img = gclview._image()
    texts_in = collections.defaultdict(set)  # text id -> scenes whose own script shows it

    def walk_text(x, out):
        if isinstance(x, tuple):
            if len(x) == 2 and x[0] == "text" and isinstance(x[1], int):
                out.append(x[1])
            for y in x:
                walk_text(y, out)
        elif isinstance(x, list):
            for y in x:
                walk_text(y, out)
    text_files = collections.defaultdict(set)  # text id -> every script file that shows it
    text_order = []                            # text ids in file and script order
    for f in gclfiles.files():
        ids = []
        for c in f["procs"]:
            walk_text(gclview.procedure(img, c) or [], ids)
        for i in ids:
            if not text_files[i]:
                text_order.append(i)
            text_files[i].add(f["id"])
            if f["kind"] == "scene" and f.get("entry") is not None:
                texts_in[i].add(f["entry"])
    # the speaker of a line is its <LABEL=ID> tag (the game's own speaker
    # table at 0824DCD4, tokenized by sub_0803232C); the message window shows
    # that speaker's portrait: Anim_SetSequence(portrait, 922E 4BBF, speaker)
    # over the frames of 593A then 593B, in the pool entry sub_0803092C picks
    raw = {}
    for ln in open(os.path.join(ROOT, "build", "text", "bank.tsv"), encoding="utf-8"):
        p_ = ln.rstrip("\n").split("\t", 2)
        if len(p_) == 3 and p_[1] == "T" and p_[0].isdigit():
            raw[int(p_[0])] = p_[2]
    portrait = speaker_portraits()
    cast_jp = {e["jp"]: e for e in E.values() if e["kind"] == "char"}
    monster_jp = {e["jp"]: e["id"] for e in E.values() if e["kind"] == "monster"}
    by_label = collections.defaultdict(list)
    shown = collections.defaultdict(collections.Counter)
    for i in text_order:
        m = re.match(r"<LABEL=(\w+)>(.*?)</LABEL>", raw.get(i, ""))
        if m and m.group(1) not in ("NONE", "PLAYER"):
            by_label[m.group(1)].append(i)
            shown[m.group(1)][m.group(2)] += 1
    # cast without lines in the scripts: the speaker label that contains their name (R_DJANGO)
    for c in cast_jp.values():
        lab = next((l for l in portrait if re.search(r"(^|_)" + re.escape(c["title"].upper()) + r"$", l)), None)
        if lab:
            c["portrait"] = portrait[lab]
            c["label"] = None
    rank = 0
    for label, ids in sorted(by_label.items(), key=lambda kv: -len(kv[1])):
        names = [n for n, _ in shown[label].most_common() if not n.startswith("?")]
        who = re.sub(r"\(.*?\)$", "", names[0]).strip() if names else label
        e = next((c for j, c in cast_jp.items() if who.startswith(j) or any(n.startswith(j) for n in names)), None)
        same = None
        if e is not None and e.get("label") and e["label"] != label:
            same, e = e, None  # a second speaker label with the same name (SABATA_B): its own entry
        if e is None:
            rank += 1
            ens = collections.Counter(en.get(i, "").split(":", 1)[0].strip() for i in ids if ":" in en.get(i, "")[:30] and not en.get(i, "").startswith("?"))
            title = f"{same['title']} ({label})" if same else clean(ens.most_common(1)[0][0]) if ens else who
            e = add(f"char:{label}", "Cast", kind="char", key=label, jp=who, order=100 + rank, actors=[], sprites=[], title=title)
        e["label"] = label
        e["portrait"] = portrait.get(label)
        scenes = sorted({s_ for i in ids for s_ in texts_in[i]})
        by_area = collections.defaultdict(list)
        for sc in scenes:
            by_area[str(scene_area.get(sc, -1))].append(sc)
        e["lines"] = len(ids)
        e["areas"] = dict(by_area)
        e["nscenes"] = len(scenes)
        e["files"] = list(dict.fromkeys(f"file:{x}" for i in ids for x in sorted(text_files[i])))
        # every line, in script order, under the script that shows it
        # a message is the labeled first page the script names plus the pages
        # after it, up to the string with <END> (each string is one page)
        def pages(i):
            out = [i]
            for k in range(i + 1, i + 60):
                t_ = raw.get(k)
                if t_ is None or t_.startswith("<LABEL="):
                    break
                if re.sub(r"<PROC=\d+>|<END>|\\n|\s", "", t_):
                    out.append(k)
                if "<END>" in t_:
                    break
            return out

        def joined(d, ks):
            # a page's wrapped lines run together; pages stay separate
            return "\n".join(re.sub(r"\s*\n\s*", " ", x).strip() for x in (d.get(k, "") for k in ks) if x)
        e["dialogue"] = []
        for i in ids:
            ks = pages(i)
            e["dialogue"].append({"id": i, "pages": len(ks), "file": f"file:{sorted(text_files[i])[0]}", "en": joined(en, ks), "jp": joined(jp, ks)})
        if who in monster_jp:
            e["monster"] = monster_jp[who]
        for sc in scenes:
            if f"scene:{sc}" in E and e["id"] not in E[f"scene:{sc}"].setdefault("speakers", []):
                E[f"scene:{sc}"]["speakers"].append(e["id"])

    # ---- sprites ----
    lab = {r["hash"]: r for r in csv.DictReader(open(os.path.join(ROOT, "data", "labels", "sprites.csv"), encoding="utf-8"))}
    plab = collections.defaultdict(dict)
    for r in csv.DictReader(open(os.path.join(ROOT, "data", "labels", "palettes.csv"), encoding="utf-8")):
        plab[r["hash"]][int(r["pool"])] = r["label"]
    pal = bigpalettes.table()
    effect_of = collections.defaultdict(list)
    # loads the palette trace resolves, per script entry point
    ents = paltrace.entries()
    for f, _, hexpr in paltrace.loads_in():
        for fam in sorted(paltrace.family_of().get(f, ())):
            kind, key = ents.get(fam, (None, None))
            if kind != "registry":
                continue
            try:
                hs = {v[0] & 0xFFFF for v, _ in paltrace.Tracer(fam).joint([hexpr], f)}
            except paltrace.Why:
                continue
            e = E.get(f"actor:{key}")
            if e is None or len(hs) > 8:
                continue
            # its own sprite: loaded by the callback or the loader itself;
            # anything else its code loads (effects, projectiles, things it spawns)
            own = f in (e["fname"], e["loader_name"])
            for h in hs:
                hx = f"{h:04X}"
                if own:
                    if key not in sprite_owner[hx]:
                        sprite_owner[hx].append(key)
                    if hx not in e["sprites"]:
                        e["sprites"].append(hx)
                    if hx in e["effects"]:
                        e["effects"].remove(hx)
                elif hx not in e["sprites"] and hx not in e["effects"]:
                    e["effects"].append(hx)
                    effect_of[hx].append(key)
    big_order = {h: i for i, h in enumerate(bigpalettes.defaults())}  # archive (ROM) order
    # every resource lookup (sub_0821A520(type, hash)) the family's code makes:
    # sprite packages, maps, tilesets, palettes, animations (bosses drawn as
    # background layers load maps and tilesets)
    RES = {0xCB05: "packages", 0xC091: "maps", 0xCEEF: "tilesets", 0x92B3: "palettes", 0x922E: "animations"}
    for f, rec in paltrace.program().items():
        fams_f = sorted(paltrace.family_of().get(f, ()))
        for t, st in rec["calls"]:
            if t != "sub_0821A520" or not st.get("r0") or st["r0"][0] != "c" or st["r0"][1] not in RES:
                continue
            kind = RES[st["r0"][1]]
            for fam in fams_f:
                k2, key = ents.get(fam, (None, None))
                e = E.get(f"actor:{key}") if k2 == "registry" else None
                if e is None:
                    continue
                try:
                    hs = {v[0] & 0xFFFF for v, _ in paltrace.Tracer(fam).joint([st.get("r1")], f)}
                except paltrace.Why:
                    continue
                if len(hs) > 12:
                    continue
                own = f in (e["fname"], e["loader_name"])
                for h in hs:
                    hx = f"{h:04X}"
                    if kind == "packages":
                        if own and hx not in e["small"]:
                            e["small"].append(hx)
                        elif not own and hx not in e["small"]:
                            e.setdefault("effect_pkgs", []).append(hx) if hx not in e.get("effect_pkgs", []) else None
                    else:
                        lst = e.setdefault("resources", {}).setdefault(kind, [])
                        if hx not in lst:
                            lst.append(hx)
    for h, d in sorted(bigpalettes.defaults().items()):
        hx = f"{h:04X}"
        owners = sprite_owner.get(hx, [])
        t = pal.get(h, {})
        def plabel(p):
            """This sprite's label for pool entry p: data/labels/palettes.csv,
            else the part of the pool's note about this sprite."""
            v = plab[hx].get(p, "")
            if v and v.lower() != "unknown":
                return v
            for part in (N.get(("pool", str(p)), {}).get("label") or "").split(";"):
                k, _, rest = part.strip().partition(":")
                if k.strip().upper() == hx:
                    return rest.strip()
            return ""
        vs = [{"pool": p, "source": s.split(" ")[0], "label": plabel(p)}
              for p, s in sorted(t.get("variants", {}).items()) if p != d]
        add(f"sprite:{hx}", "Sprites/Large sprites", kind="sprite", key=hx, order=big_order.get(h, 0),
            title=N.get(("big", hx), {}).get("label") or lab.get(hx, {}).get("label") or f"Sprite {hx}",
            default=d, default_label=plabel(d),
            variants=vs, owners=[f"actor:{k}" for k in owners], effect_of=[f"actor:{k}" for k in effect_of.get(hx, [])],
            frames=lab.get(hx, {}).get("frames", ""))

    # an object draws something itself: its callback or loader loads a sprite
    # (or allocates one and hands it to a loader); else a script routine
    for e in E.values():
        if e["kind"] == "actor":
            r = e.get("resources", {})
            e["creates"] = bool(e["allocates"] or e["sprites"] or e["small"] or e.get("effect_pkgs") or r.get("maps") or r.get("tilesets"))
            e["path"] = ("Objects/" if e["creates"] else "Script routines/") + e["subsystem"]
            node_rank[e["path"]] = subsys_start.get(e["subsystem"], 0x09000000)
            tree_items[e["path"]].append(e["id"])

    # ---- sprite packages (CB05) ----
    pkg_owner = collections.defaultdict(list)
    for e in E.values():
        if e["kind"] == "actor":
            for h in e["small"]:
                pkg_owner[h].append(e["id"])
    for h, kind in sorted(actors.sprite_hashes().items()):
        if kind != "spr":
            continue
        hx = f"{h:04X}" if isinstance(h, int) else h
        add(f"spr:{hx}", "Sprites/Sprite packages", kind="package", key=hx,
            title=(N.get(("spr", hx), {}).get("label") or f"Package {hx}").capitalize(), owners=pkg_owner.get(hx, []))

    # ---- tiles: CEEF tilesets by the area that uses them, maps' own tiles ----
    import tilesets
    T = tilesets.load()
    for t in T["tilesets"]:
        place = t["areas"][0]["place"] if t["areas"] else ""
        loc = location(place)[0] if place else ("Not loaded by a scene" if t["maps"] else "Not loaded by a script")
        node_rank.setdefault("Tiles/Tilesets/" + loc, node_rank.get("Places/" + loc, 900))
        add(f"tileset:{t['id']}", "Tiles/Tilesets/" + loc, kind="tileset", key=t["id"], order=t["order"],
            title=f"Tileset {t['id']}", place=place, tiles=t["tiles"], new_count=t["new_count"],
            pool_addr=f"{t['pool_addr']:08X}", index_addr=f"{t['index_addr']:08X}",
            palette=t["palette"], palette_source=t["palette_source"], maps=t["maps"], stages=t["stages"],
            scenes=[f"scene:{x['scene']}" for x in t["scenes"]], areas=[f"area:{a['area']}" for a in t["areas"]])
        for x in t["scenes"]:
            sc = E.get(f"scene:{x['scene']}")
            if sc is not None and f"tileset:{t['id']}" not in sc.setdefault("tilesets", []):
                sc["tilesets"].append(f"tileset:{t['id']}")
        for a in t["areas"]:
            ar = E.get(f"area:{a['area']}")
            if ar is not None:
                ar.setdefault("tilesets", []).append(f"tileset:{t['id']}")
    for b in T["bg"]:
        add(f"bgtiles:{b['map']}", "Tiles/Map tiles", kind="bgtiles", key=b["map"], order=b["addr"],
            title=b["note"] or f"Map {b['map']} tiles", tiles=b["tiles"], layers=b["layers"], palette=b.get("palette"),
            addr=f"{b['addr']:08X}", compressed=b["compressed"], maps=b["maps"], stages=b["stages"],
            scenes=[f"scene:{x['scene']}" for x in b["scenes"]])

    # ---- sound: songs by music player, linked to the scenes that start them ----
    import sound
    S = sound.load()
    test_word = {"music": "Music", "effects": "Effect", "voices": "Voice"}
    for sg in S["songs"]:
        if sg.get("empty"):
            continue
        n, p = sg["n"], sg["player"]
        group = "Sound/" + ("Music (player 10)" if p == 10 else f"Player {p}")
        node_rank[group] = -1 if p == 10 else p
        t = sg["test"]
        title = N.get(("song", str(n)), {}).get("label") or (f"{test_word[t['list']]} {t['no']}" if t else f"Song {n}")
        add(f"song:{n}", group, kind="song", key=str(n), order=n, title=title, player=p, addr=sg["addr"],
            ntracks=len(sg["tracks"]), size=sg["size"], voicegroup=sg["voicegroup"], test=t,
            scenes=[f"scene:{x['scene']}" for x in sg["scenes"]], scripts=sg["scripts"], shared=sg["shared_files"],
            code=[c["func"] for c in sg["code"]])
        for x in sg["scenes"]:
            sc = E.get(f"scene:{x['scene']}")
            if sc is not None and f"song:{n}" not in sc.setdefault("songs", []):
                sc["songs"].append(f"song:{n}")
                ar = E.get(sc["area"])
                if ar is not None and f"song:{n}" not in ar.setdefault("songs", []):
                    ar["songs"].append(f"song:{n}")

    # ---- cutscenes: the event tables SceneMessages plays (tools/assets/cutscenes.py) ----
    import cutscenes
    speaker_of = {}
    for e in E.values():
        if e["kind"] == "char" and e.get("label"):
            for i in by_label.get(e["label"], []):
                speaker_of[i] = e["id"]
    starts = collections.defaultdict(list)
    for s in cutscenes.scan()["starts"]:
        starts[s["event"]].append(s)
    cn = cutscenes.chara_names()
    for n, steps in enumerate(cutscenes.table()):
        if not steps:
            continue
        start = starts[n][0] if starts[n] else None
        _, rows = cutscenes.timeline(n, start)
        said = [m for _, row in rows for m in row if "text" in m]
        cast = sorted({m["to"] for _, row in rows for m in row if m["kind"] != 1} - {"audio", "screen", "camera"})
        bgm = [m["args"][0] for _, row in rows for m in row if m["kind"] == 8 and m["cmd"] in ("start_bgm", "start_song") and m["args"]]
        fid = fof.get(start["proc"]) if start else None
        f = fkind.get(fid) if fid else None
        sc = E.get(f"scene:{f['entry']}") if f and f.get("entry") is not None else None
        ar = E.get(sc["area"]) if sc else None
        first = re.sub(r'^[^:"「]{1,24}:\s*', "", en.get(said[0]["text"], "")) if said else ""
        first = clean(first).strip().strip('"').split("\n")[0]
        title = f"{n} · {first[:40] + ('…' if len(first) > 40 else '')}" if first else f"{n} · {sc['title'] if sc else 'no dialogue'}"
        path = "Cutscenes/" + (ar["location"] if ar else "No area")
        node_rank[path] = min(node_rank.get(path, 999), node_rank.get("Places/" + ar["location"], 900) if ar else 999)
        e = add(f"cut:{n}", path, kind="cut", key=str(n), order=n, title=title, steps=len(steps),
                messages=sum(len(s_) for s_ in steps), start=start, starters=[s_["proc"] for s_ in starts[n]],
                file=f"file:{fid}" if fid else None, scene=sc["id"] if sc else None, area=sc["area"] if sc else None,
                cast=cast, lines=len(said), speakers=sorted({speaker_of[m["text"]] for m in said if m["text"] in speaker_of}),
                speaker_of={str(m["text"]): speaker_of[m["text"]] for m in said if m["text"] in speaker_of},
                songs=[f"song:{b}" for b in dict.fromkeys(bgm) if f"song:{b}" in E])
        if sc is not None:
            sc.setdefault("cutscenes", []).append(e["id"])
        if fid and f"file:{fid}" in E:
            E[f"file:{fid}"].setdefault("cutscenes", []).append(e["id"])
        for c in e["speakers"]:
            E[c].setdefault("cutscenes", []).append(e["id"])

    # ---- the main quest: story progress values (tools/assets/story.py) ----
    import story
    stage_doors = collections.defaultdict(list)  # value -> doors whose condition tests it
    for sd in areas.load_stages():
        sid = sd["scene"]
        for d in sd.get("doors", []):
            for c in d.get("cond", []):
                for m in re.finditer(r"progress\)? ?(<|<=|==|>=|>|!=) ?(\d+)", c):
                    stage_doors[int(m.group(2))].append({"from": f"scene:{sid}", "to": f"scene:{d['scene']}", "entrance": d.get("entrance"), "cond": c})
    prev_loc = "Before the quest"
    for b in story.beats():
        v = b["value"]
        s0 = next((s_ for s_ in b["setters"] if s_["scene"] is not None), b["setters"][0] if b["setters"] else None)
        sc = E.get(f"scene:{s0['scene']}") if s0 and s0["scene"] is not None else None
        ar = E.get(sc["area"]) if sc else None
        # a one-room story scene with no label of its own (a boss room, a
        # story event) belongs to the chapter of the beat before it
        loc = ar["location"] if ar and not ar["location"].startswith("Area ") else prev_loc
        prev_loc = loc
        cut = E.get(f"cut:{s0['cutscene']}") if s0 and s0["cutscene"] is not None else None
        line = clean(re.sub(r'^[^:"「]{1,24}:\s*', "", en.get(s0["text"], ""))).strip().strip('"').split("\n")[0] if s0 and s0.get("text") is not None else ""
        what = line[:48] + ("…" if len(line) > 48 else "") if line else (sc["title"] if sc else (f"set by {s0['file']}" if s0 else "never set"))
        path = "Story/" + loc
        node_rank[path] = min(node_rank.get(path, 999), v)
        add(f"beat:{v}", path, kind="beat", key=str(v), order=v, title=f"{v} · {what}",
            setters=[{"proc": s_["proc"], "name": procnames.get(s_["proc"], ""), "file": f"file:{s_['file']}" if s_["file"] else None,
                      "scene": f"scene:{s_['scene']}" if s_["scene"] is not None and f"scene:{s_['scene']}" in E else None,
                      "cutscene": f"cut:{s_['cutscene']}" if s_["cutscene"] is not None else None, "text": s_["text"]} for s_ in b["setters"]],
            scene=sc["id"] if sc else None, area=sc["area"] if sc else None, cutscene=cut["id"] if cut else None,
            doors=stage_doors.get(v, []),
            tests=[{"proc": t["proc"], "name": procnames.get(t["proc"], ""), "file": f"file:{t['file']}" if t["file"] else None,
                    "scene": f"scene:{t['scene']}" if t["scene"] is not None and f"scene:{t['scene']}" in E else None, "test": t["test"]} for t in b["tests"]])
        if sc is not None:
            sc.setdefault("beats", []).append(f"beat:{v}")
        if cut is not None:
            cut.setdefault("beats", []).append(f"beat:{v}")

    # ---- savestate checkpoints (tools/assets/saves.py; ../boktai3trans/saves) ----
    import saves
    try:
        bank = saves.bank() if os.path.isdir(saves.BANK) else []
    except Exception:
        bank = []
    beat_vals = sorted(int(e["key"]) for e in E.values() if e["kind"] == "beat")
    for r in bank:
        if "error" in r:
            continue
        ck = {"file": r["file"], "label": r["label"], "save": "build/saves/" + r["file"][:4] + ".sav",
              "story": r["story"], "scene": r["scene"], "entrance": r["entrance"]}
        sc = E.get(f"scene:{r['scene']}")
        if sc is not None:
            sc.setdefault("checkpoints", []).append(ck)
        # the beat it is at: the highest story value reached
        at = max((v for v in beat_vals if v <= r["story"]), default=None)
        if at is not None:
            E[f"beat:{at}"].setdefault("checkpoints", []).append(dict(ck, scene_id=sc["id"] if sc else None))

    # ---- explicit links (data/atlas/links.csv) ----
    for r in csv.DictReader(l for l in open(os.path.join(ROOT, "data", "atlas", "links.csv"), encoding="utf-8") if not l.startswith("#")):
        a, b = r["from"], r["to"]
        if a in E and b in E:
            if E[a]["kind"] == "actor":  # an object's picture
                E[a]["pic"] = b
                continue
            field = {"actor": "actors", "sprite": "sprites", "spr": "sprites"}[b.split(":")[0]]
            if b not in E[a].setdefault(field, []):
                E[a][field].append(b)
            E[b].setdefault("chars", []).append(a)

    # ---- code ----
    for a, (size, matched, nm, unit) in prog.items():
        if a not in an:
            continue
        sub = unit.split("/", 1)[1] if "/" in unit else unit or "Other"
        top = unit.split("/", 1)[0] if "/" in unit else "game"
        funcinfo.setdefault(an[a], {"addr": f"{a:08X}", "name": nm, "size": size, "matched": matched, "unit": unit})
    # subsystems in ROM order (the source files' order)
    groups = collections.defaultdict(list)
    for name in funcinfo:
        groups[subsys.get(name, "Unsorted")].append(name)
    for sub, fs in groups.items():
        size = sum(funcinfo[f]["size"] for f in fs)
        done = sum(funcinfo[f]["size"] for f in fs if funcinfo[f]["matched"])
        objs = sorted(e["id"] for e in E.values() if e["kind"] == "actor" and e["subsystem"] == sub)
        node_rank[f"Code/{sub}"] = subsys_start.get(sub, 0)
        add(f"unit:{sub}", "Code", kind="unit", key=sub, title=sub.replace(" (tentative)", ""), order=subsys_start.get(sub, 0),
            tentative="(tentative)" in sub, funcs=sorted(fs, key=lambda f: funcinfo[f]["addr"]), size=size, matched=done, objects=objs)

    # a species drawn in a palette labeled with its name (data/labels/palettes.csv)
    for e in E.values():
        if e["kind"] != "monster" or not e["title"]:
            continue
        name = e["title"].lower()
        for aid in e["actors"]:
            for h in E[aid]["sprites"]:
                sp = E.get(f"sprite:{h}")
                if not sp:
                    continue
                for pool, lab in [(sp["default"], sp["default_label"])] + [(v["pool"], v["label"]) for v in sp["variants"]]:
                    if lab and re.search(r"(?<![\w+])" + re.escape(name) + r"(?![\w+])", lab.lower()):
                        e.setdefault("palette", {"sprite": h, "pool": pool, "label": lab})
    # objects specific to places: game objects, not engine ones found everywhere
    for e in E.values():
        if e["kind"] == "actor":
            top = e["path"].split("/")[0]
            e["placed"] = e["creates"] and 0 < e["nscenes"] <= 120

    # placeholder titles from the data: the procedures that create it
    # (spawn_lun_1, spawn_lun_2 -> Lun), else its own sprite's label
    for e in E.values():
        if e["kind"] != "actor" or not re.match(r"(Object|Routine) [0-9A-F]{4}$", e["title"]):
            continue
        stems = collections.Counter()
        for p in e["procs"]:
            if {sp_["key"] for sp_ in actors.procs()[p["id"]]["spawns"]} != {e["key"]}:
                continue  # a procedure that creates other things too names them, not this
            n = re.sub(r"^(spawn|place|create|make|add|start|set|show|init)_", "", p["name"] or "")
            n = re.sub(r"(_\d+|_by_\w+)$", "", n)
            if n and not n.startswith("proc_"):
                stems[n] += 1
        sp = E.get(f"sprite:{e['sprites'][0]}") if e["sprites"] else None
        if stems:
            e["title"] = stems.most_common(1)[0][0].replace("_", " ").capitalize()
            e["title_from"] = "the names of the procedures that create it"
        elif sp and not sp["title"].startswith("Sprite ") and sp["title"].lower() != "unknown":
            e["title"] = sp["title"]
            e["title_from"] = f"its sprite {sp['key']}"

    # titles shared within a group get the function's name to tell them apart
    seen = collections.defaultdict(list)
    for e in E.values():
        if e["kind"] == "actor":
            seen[e["path"], e["title"]].append(e)
    for (_, t), es in seen.items():
        if len(es) > 1:
            for e in es:
                e["title"] = f"{t} ({words(e['fname']) if not e['fname'].startswith('sub_') else e['key']})"

    # ---- tree ----
    node_rank.update({"Sprites/Large sprites": 0, "Sprites/Sprite packages": 1, "Tiles/Tilesets": 0, "Tiles/Map tiles": 1,
                      "Tiles/Tilesets/Not loaded by a scene": 1000, "Tiles/Tilesets/Not loaded by a script": 1001})
    order = ["Story", "Cast", "Bestiary", "Items", "Places", "Cutscenes", "Objects", "Script routines", "Scripts", "Sprites", "Tiles", "Sound", "Code"]
    root = {}
    for path, ids in tree_items.items():
        node = root
        for part in path.split("/"):
            node = node.setdefault(part, {})
        node.setdefault("", []).extend(ids)

    def sortkey(eid):
        e = E[eid]
        return (e.get("order", 0), e["title"].lower())

    def emit(name, node, depth=1, path=""):
        path = f"{path}/{name}" if path else name
        sub = sorted(((k, v) for k, v in node.items() if k), key=lambda kv: (node_rank.get(f"{path}/{kv[0]}", 999), kv[0]))
        kids = [emit(k, v, depth + 1, path) for k, v in sub]
        items = list(node.get("", []))
        if path.split("/")[0] in ("Objects", "Script routines"):
            # small code groups fold into their parent: everything stays in ROM order
            for k in [k for k in kids if k["count"] < 3 and not k["children"]]:
                items += k["items"]
                kids.remove(k)
        items = sorted(items, key=sortkey)
        n = len(items) + sum(k["count"] for k in kids)
        return {"title": name, "children": kids, "items": items, "count": n}

    tree = [emit(k, v, 1) for k, v in sorted(root.items(), key=lambda kv: order.index(kv[0]) if kv[0] in order else 99)]
    return {"tree": tree, "entities": E, "funcs": funcinfo}


def load():
    """The index, rebuilt when an input is newer than the cache."""
    newest = max(os.path.getmtime(os.path.join(ROOT, p)) for p in INPUTS if os.path.exists(os.path.join(ROOT, p)))
    if os.path.exists(CACHE) and os.path.getmtime(CACHE) >= newest:
        return json.load(open(CACHE))
    out = build()
    os.makedirs(os.path.dirname(CACHE), exist_ok=True)
    json.dump(out, open(CACHE + ".tmp", "w"), ensure_ascii=False)
    os.replace(CACHE + ".tmp", CACHE)
    return out


def main():
    ix = load()
    if len(sys.argv) > 1:
        print(json.dumps(ix["entities"].get(sys.argv[1]), ensure_ascii=False, indent=1))
        return

    def show(n, d=0):
        print("  " * d + f"{n['title']} ({n['count']})")
        if d < 2:
            for k in n["children"]:
                show(k, d + 1)
    for n in ix["tree"]:
        show(n)
    print(len(ix["entities"]), "entities")



# ---------- the ROM in order (tools/studio/rom.html) ----------
ROMCACHE = os.path.join(ROOT, "build", "cache", "rommap.json")
ARCHIVE_KINDS = {"anm": "animation", "t92B3": "palette", "t9B1B": "palette", "t98F5": "sprite", "fnt": "font",
                 "tAE6C": "collision", "map": "map", "tCB05": "sprite", "ptn": "tiles", "tCEAA": "data"}


def region_kind(text):
    t = text.lower()
    for k, words_ in (("free", ["free"]), ("sound", ["mp2k", "pcm", "voicegroup", "song", "track"]),
                      ("text", ["text"]), ("script", ["bytecode", "script"]), ("font", ["font"]),
                      ("graphics", ["graphics", "tiles", "tilemap", "portrait"]), ("data", ["tables", "pointer"]),
                      ("code", ["code", "crt0", "header"]),
                      ("palette", ["palette"])):
        if any(w in t for w in words_):
            return k
    return "data"


def rom_segments():
    """{"size", "regions": [[start, end, kind, label]], "items": [[start, size, kind, label, ref]]}:
    the ROM's regions (docs/ROM_MAP.md), every function and every archive
    resource, in address order. ref is a Browse id or a Studio link."""
    import archive
    import script_targets
    from romlib import load_rom
    rom = load_rom()
    regions = []
    for ln in open(os.path.join(ROOT, "docs", "ROM_MAP.md"), encoding="utf-8"):
        m = re.match(r"\| `(08[0-9A-Fx]{6})[–-](08[0-9A-Fx]{6})` \|[^|]*\| (.+?) \|", ln)
        if m:
            a, b = int(m.group(1).replace("x", "0"), 16), int(m.group(2).replace("x", "F"), 16)
            label = re.sub(r"\*\*|`", "", m.group(3)).strip()
            regions.append([a, b + 1, region_kind(label), label])
    items = []
    for a, (size, matched, name, unit) in script_targets.progress().items():
        if size:
            items.append([a, size, "code" if matched else "asm", name, f"/code#{a:08X}"])
    known = set()
    for label, _, _, ents in archive.archives(rom):
        kind = ARCHIVE_KINDS.get(label, "data")
        for h, addr, size in ents:
            ref = {"tCB05": f"spr:{h:04X}", "map": f"/gfx#maps/{h:04X}", "t92B3": f"/gfx#palettes/{h:04X}",
                   "t98F5": "node:Sprites", "t9B1B": "/gfx#pool"}.get(label, "")
            items.append([addr, size, kind, f"{label} {h:04X}", ref])
            known.add(addr)
    items.sort()
    end = len(rom.rstrip(b"\x00")) + 0x08000000
    if not any(r[2] == "free" for r in regions):
        regions.append([end, 0x09000000, "free", "free (zero padding)"])
    return {"size": len(rom), "end": end, "regions": regions, "items": items}


def rom_load():
    newest = max(os.path.getmtime(os.path.join(ROOT, p)) for p in ("docs/ROM_MAP.md", "symbols/functions.csv", "tools/studio/atlas.py", "tools/assets/tilesets.py"))
    if os.path.exists(ROMCACHE) and os.path.getmtime(ROMCACHE) >= newest:
        return json.load(open(ROMCACHE))
    out = rom_segments()
    os.makedirs(os.path.dirname(ROMCACHE), exist_ok=True)
    json.dump(out, open(ROMCACHE, "w"))
    return out



# understanding levels, best first (tools/studio/rom.html colors and totals)
LEVELS = ["named", "decoded", "decompiled", "located", "assembly", "unmapped", "free"]
DECODED = {"sprite", "map", "tiles", "palette", "font", "animation", "text", "script", "sound"}


def command_table_blocks():
    """Exact decoded extents, rather than treating their surrounding region
    as decoded. The Actors page shows the ordered handlers and references."""
    import actors
    return [{"s": int(t["addr"], 16), "n": t["size"], "k": "data",
             "t": t["description"] or t["name"], "u": "decoded",
             "r": f"/actors#commands:{t['addr']}"}
            for t in actors.command_tables().values()]


def player_animation_blocks():
    """Keep each mode's exact decoded interval and the shared table viewer."""
    import player_cue
    data = player_cue.animation_variants()
    return [{"s": int(table["addr"], 16), "n": table["size"], "k": "data",
             "t": f"Player animation variants: mode {table['mode']}", "u": "decoded",
             "r": "/actors#player-animations"} for table in data["tables"]] if data else []


def numeric_table_blocks():
    """Reviewed parameter tables keep their own decoded ROM intervals."""
    import actors
    return [{"s": int(t["addr"], 16), "n": t["size"], "k": "data",
             "t": t["description"] or t["name"], "u": "decoded",
             "r": f"/actors#tables:{t['addr']}"}
            for t in actors.numeric_tables().values()]


def rom_tree():
    """The ROM as nested blocks in address order: top-level regions (code by
    subsystem and function, each asset archive by resource, sound, text,
    scripts, other data, unmapped gaps, free space), each block with a kind
    and an understanding level."""
    seg = rom_load()
    N = notes()
    ix = load()
    unit_of = {}
    for e in ix["entities"].values():
        if e["kind"] == "unit":
            for f in e["funcs"]:
                unit_of[f] = e["key"]
    an_rev = {fi["addr"]: name for name, fi in ix["funcs"].items()}
    note_kind = {"map": "map", "tCB05": "spr", "t92B3": "pal"}

    def leaf(start, size, kind, name, ref, level):
        return {"s": start, "n": size, "k": kind, "t": name, "r": ref, "u": level}

    tops = []
    code = collections.OrderedDict()
    for a, size, kind, name, ref in seg["items"]:
        if kind in ("code", "asm"):
            sub = unit_of.get(an_rev.get(f"{a:08X}", ""), "Unsorted")
            code.setdefault(sub, []).append(leaf(a, size, kind, name, ref, "decompiled" if kind == "code" else "assembly"))
    code_children = [{"s": fs[0]["s"], "n": sum(f["n"] for f in fs), "k": "code", "t": sub, "r": f"unit:{sub}", "c": fs}
                     for sub, fs in code.items()]
    code_children.sort(key=lambda x: x["s"])
    if code_children:
        tops.append({"s": code_children[0]["s"], "n": sum(c["n"] for c in code_children), "k": "code", "t": "Code", "c": code_children})
    archives = collections.OrderedDict()
    for a, size, kind, name, ref in seg["items"]:
        if kind in ("code", "asm"):
            continue
        label, _, h = name.partition(" ")
        nk = note_kind.get(label)
        named = bool(nk and N.get((nk, h), {}).get("label"))
        level = ("named" if named else "decoded") if kind in DECODED else "located"
        archives.setdefault(label, []).append(leaf(a, size, kind, name, ref, level))
    titles = {"anm": "Animation scripts (922E)", "t92B3": "Palettes (92B3)", "t9B1B": "Sprite palette pool (9B1B)",
              "t98F5": "Large sprites (98F5)", "fnt": "Font (A635)", "tAE6C": "Collision / HP data (AE6C)", "map": "Maps (C091)",
              "tCB05": "Sprite packages (CB05)", "ptn": "Tileset library (CEEF)", "tCEAA": "Unknown blob (CEAA)"}
    # the large sprite archive is one resource: split it by each sprite's pixel data
    if "t98F5" in archives:
        import bigsprites
        from romlib import load_rom
        bs = bigsprites.BigSprites(load_rom())
        base = archives["t98F5"][0]["s"]
        lab = {r["hash"]: r["label"] for r in csv.DictReader(open(os.path.join(ROOT, "data", "labels", "sprites.csv"), encoding="utf-8"))}
        starts = []
        for i, r in enumerate(bs.records):
            fr = list(bs.frames(i))
            if fr:
                starts.append((min(f[3] for f in fr), r[0]))
        starts.sort()
        kids = []
        head = archives["t98F5"][0]
        for k, (pix, h) in enumerate(starts):
            end = starts[k + 1][0] if k + 1 < len(starts) else head["n"] - bs.pixels
            hx = f"{h:04X}"
            named = bool(N.get(("big", hx), {}).get("label") or (lab.get(hx) and lab[hx].lower() != "unknown"))
            kids.append(leaf(base + bs.pixels + pix, end - pix, "sprite", f"sprite {hx} {lab.get(hx, '')}".strip(), f"sprite:{hx}",
                             "named" if named else "decoded"))
        tables = kids[0]["s"] - base if kids else head["n"]
        archives["t98F5"] = [leaf(base, tables, "sprite", "98F5 frame and part tables", "node:Sprites", "decoded")] + kids
    # the tileset library is one resource: split it into its table, each
    # tileset's index list and the run of pool tiles each tileset added
    if "ptn" in archives:
        import tilesets
        T = tilesets.load()
        lib, head = T["library"], archives["ptn"][0]
        idx, pool = [], []
        for t in T["tilesets"]:
            ref, name = f"tileset:{t['id']}", f"tileset {t['id']} {t['title']}".strip()
            level = "named" if t["title"] else "decoded"
            idx.append(leaf(t["index_addr"], t["index_size"], "tiles", name + " (index list)", ref, level))
            if t["new_count"]:
                pool.append(leaf(t["pool_addr"], t["new_count"] * 32, "tiles", name, ref, level))
        archives["ptn"] = [
            leaf(head["s"], lib["index"] - head["s"], "tiles", "CEEF header and tileset table", "node:Tiles/Tilesets", "decoded"),
            {"s": lib["index"], "n": lib["pool"] - lib["index"], "k": "tiles", "t": "Tileset index lists", "c": idx},
            {"s": lib["pool"], "n": head["s"] + head["n"] - lib["pool"], "k": "tiles", "t": "Tile pool", "c": pool},
        ]
    # maps with their own tiles lead to them
    if "map" in archives:
        import tilesets
        own = {b["map"] for b in tilesets.load()["bg"]}
        for x in archives["map"]:
            h = x["t"].partition(" ")[2]
            if h in own:
                x["t"] += " (own tiles)"
                x["r"] = f"bgtiles:{h}"
    for label, ls in archives.items():
        ls.sort(key=lambda x: x["s"])
        tops.append({"s": ls[0]["s"], "n": sum(x["n"] for x in ls), "k": ls[0]["k"], "t": titles.get(label, label), "c": ls})
    # sound (tools/studio/sound.py): the song table, voicegroups, samples and songs
    import sound
    S = sound.load()
    kids = [leaf(int(S["table"]["addr"], 16), S["table"]["size"], "sound", "song table", "node:Sound", "decoded")]
    vgs = sorted(int(v["addr"], 16) for v in S["voicegroups"])
    users = {v["addr"]: v["songs"] for v in S["voicegroups"]}
    vg_leaves = []
    for i, a in enumerate(vgs):
        end = min(vgs[i + 1] if i + 1 < len(vgs) else a + 128 * 12, a + 128 * 12)
        u = users.get(f"{a:08X}", [])
        vg_leaves.append(leaf(a, end - a, "sound", f"voicegroup {a:08X}" + (f" ({len(u)} songs)" if u else " (inside another)"),
                              f"/sound#vg={a:08X}", "decoded"))
    sm_leaves = [leaf(int(x["addr"], 16), x["size"], "sound", f"sample {x['addr']}" + (f" · {x['rate']} Hz" if x.get("rate") else " · GB wave"),
                      f"/sound#sample={x['addr']}", "decoded") for x in S["samples"]]
    song_leaves, last = [], 0
    titles_ = sound.titles()
    for sg in sorted((x for x in S["songs"] if not x.get("empty")), key=lambda x: x["start"]):
        a, b = max(sg["start"], last), int(sg["addr"], 16) + 8 + 4 * len(sg["tracks"])
        if b <= a:
            continue
        last = b
        name = titles_.get(sg["n"])
        song_leaves.append(leaf(a, b - a, "sound", f"song {sg['n']}" + (f" {name}" if name else ""), f"song:{sg['n']}", "named" if name else "decoded"))
    # what the walk doesn't reach inside the documented sound regions
    # (keysplit tables, the player table, padding) stays in the Sound block;
    # alignment bytes after a block join it
    ordered = sorted(vg_leaves + sm_leaves + song_leaves + kids, key=lambda x: x["s"])
    other = []
    for a, b, kind, label in seg["regions"]:
        if kind != "sound" or "SoundMainRAM" in label:
            continue
        p, prev = a, None
        for x in ordered + [None]:
            x0 = b if x is None else x["s"]
            if x is not None and (x0 + x["n"] <= p or x0 >= b):
                continue
            x0 = min(x0, b)
            if x0 - p >= 16:
                name = "music player table (gMPlayTable)" if p == sound.PLAYER_TABLE else f"{label.split(' (')[0]}: not walked"
                other.append(leaf(p, x0 - p, "sound", name, "node:Sound", "decoded" if p == sound.PLAYER_TABLE else "located"))
            elif x0 > p and prev is not None and prev["s"] + prev["n"] == p:
                prev["n"] += x0 - p
            if x is None:
                break
            p, prev = max(p, x["s"] + x["n"]), x
    for title, ls in (("Voicegroups", vg_leaves), ("Samples", sm_leaves), ("Songs", song_leaves), ("Other sound data", other)):
        if ls:
            kids.append({"s": ls[0]["s"], "n": sum(x["n"] for x in ls), "k": "sound", "t": title, "c": ls})
    kids.sort(key=lambda x: x["s"])
    tops.append({"s": kids[0]["s"], "n": sum(x["n"] for x in kids), "k": "sound", "t": "Sound (MP2K)", "c": kids})
    # Tables are separated by other data, so each keeps its own ROM interval.
    tops.extend(command_table_blocks())
    tops.extend(player_animation_blocks())
    tops.extend(numeric_table_blocks())
    # the documented regions that items don't cover (sound, text, scripts, graphics, tables)
    covered = sorted((c["s"], c["s"] + c["n"]) for t in tops for c in (t.get("c") or [t]) for c in (c.get("c") or [c]))
    merged = []
    for a, b in covered:
        if merged and a <= merged[-1][1]:
            merged[-1][1] = max(merged[-1][1], b)
        else:
            merged.append([a, b])

    def uncovered(a, b):
        out, p = [], a
        for x, y in merged:
            if y <= p or x >= b:
                continue
            if x > p:
                out.append((p, x))
            p = max(p, y)
        if p < b:
            out.append((p, b))
        return out
    regs = sorted([r for r in seg["regions"] if r[2] != "code" or "tables" in r[3]], key=lambda r: (r[0], -(r[1] - r[0])))
    used = []
    for a, b, kind, label in regs:
        b = min(b, 0x09000000)
        parts = [(x, y) for x, y in uncovered(a, b) if not any(x < uy and y > ux for ux, uy in used)]
        size = sum(y - x for x, y in parts)
        if size < 256:
            continue
        used += parts
        level = "free" if kind == "free" else ("decoded" if kind in DECODED else "located")
        for x, y in parts:
            tops.append({"s": x, "n": y - x, "k": kind, "t": label.split(" (")[0][:60], "full": label, "u": level})
    # everything else: bytes nobody has mapped
    allspans = sorted([(c["s"], c["s"] + c["n"]) for c in tops if "c" not in c] + [tuple(m) for m in merged] + used)
    p, gaps = 0x08000000, []
    for a, b in sorted(allspans):
        if a > p and a - p >= 1024:
            gaps.append((p, a))
        p = max(p, b)
    for a, b in gaps:
        tops.append({"s": a, "n": b - a, "k": "unknown", "t": f"unmapped {a:08X}", "u": "unmapped"})
    tops.sort(key=lambda t: t["s"])
    return {"size": seg["size"], "children": tops, "levels": LEVELS}


if __name__ == "__main__":
    main()
