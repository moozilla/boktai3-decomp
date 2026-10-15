#!/usr/bin/env python3
"""Per-scene stage metadata from the scripts: doors, entrances, items, enemies.

  stages.py show SCENE...          readable summary of scenes
  stages.py json [OUT]             every scene (default build/assets/stages.json)

A scene is a script procedure (tools/mgba/scenes.py; call IDs, one more than
the headers `gcx.py dis` prints). Its setup calls scene-local procedures that
use library procedures, decoded here by argument position:

  stage     proc_884(map, tileset, hp) (also proc_885 and the loader they
            wrap, proc_876(map, tileset, hp, ...)), proc_888(palette, 0, 0)
  entrance  library procedures that start `if (r708[0x70] == argK)` (the
            entrance requested by the door that led here) and place the player
            with proc_932(x, z, facing) or at a named HP box with
            proc_931(box, facing) (proc_909/910/911/... and 914-920)
  door      proc_934/935/936/937/938/949/950(box, scene, entrance, ...): a trap
            on the HP box that loads `scene` at `entrance` (proc_953 sets
            r708[0x70] = entrance, proc_952 starts the transition actor);
            proc_940-945/951(scene, entrance, ...) do the same without a box
            and are found inside `trap box` event blocks; proc_946(..., box,
            scene, entrance) is a door behind a lock
  item      proc_1118(category, item, x, z, facing, flags, flag) on the floor
            and proc_1119(category, item, x, z, facing, chest, ?, flags, flag,
            proc); chest = 1 puts it in a chest (proc_1120 creates actor 0x430D
            whose event spawns the pickup, cmd 0xDDCF `I: category item`).
            The item sits at (x + 128, z + 128), the centre of its cell.
            Category 0 items are named by strings 988 + item (回復薬, 大地の実,
            ...; 太陽の実 = 13); other categories are not mapped yet.
  enemy     spawner procedures 1490-1700 that create an actor family by
            registry key (cmd 0xB745 KEY `N: name r: path ...`); the path is an
            index into the HP patrol paths (tentative)

Each record keeps the script conditions it sits under (`r708[0x3c]` is the
story progress, `r708[0x70]` the entrance used). Positions are world units
(see tools/assets/hp.py for the map projection); box records are resolved
against the stage's HP data. Output is ROM-derived; keep it under build/.
"""
import collections
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(os.path.dirname(HERE), "mgba"))
from romlib import ROOT, load_rom  # noqa: E402
import gcx  # noqa: E402
import hp as hpmod  # noqa: E402
import text  # noqa: E402

# door procedures: (box, scene, entrance) argument positions (1-based; None = no box)
DOORS = {934: (1, 2, 3), 935: (1, 2, 3), 936: (1, 2, 3), 937: (1, 2, 3), 938: (1, 2, 3), 949: (1, 2, 3),
         950: (1, 2, 3), 946: (4, 5, 6), 940: (None, 1, 2), 941: (None, 1, 2), 942: (None, 1, 2),
         943: (None, 1, 2), 944: (None, 2, 3), 945: (None, 1, 2), 951: (None, 1, 2)}
ENEMY_PROCS = range(1490, 1700)
# enemy families by registry key (docs/ASSETS.md, "Enemy families"; names tentative)
FAMILIES = {0xF8BE: "bat", 0x05A5: "ghoul", 0xC3BC: "slime", 0xE131: "bee", 0xCCD3: "hooded figure",
            0x680E: "flame", 0x9E8D: "crow", 0x87E4: "spider", 0x0AC7: "tentacle body", 0x98EC: "golem",
            0x6913: "mummy", 0xF836: "hound", 0xBEF5: "mimic", 0x3F99: "flame worm", 0x1744: "chloro worm",
            0x2D30: "living sword", 0xDFDC: "black wings", 0xAD03: "skeleton"}
CONSUMABLE_NAMES = 988
# readable names for condition variables (r708 is the script work area)
PRETTY = [(" <end>", ""), ("r708[0x3c]:t1", "progress"), ("r708[0x70]:t1", "entrance")]


def num(t):
    """int of a literal operand text, else None."""
    try:
        return int(t, 0)
    except (TypeError, ValueError):
        return None


class Walker:
    def __init__(self, rom):
        self.img = gcx.Image(rom)
        self.b = rom
        self.lines = {}
        self.callers = collections.defaultdict(set)
        for idx in range(len(self.img.procs)):
            for d, t in gcx.disassemble(self.img, idx):
                for m in re.finditer(r"\bproc_(\d+)", t):
                    self.callers[int(m.group(1))].add(idx + 1)
        self.entrance_cache = {}

    def body(self, pid):
        """(start, end) of a procedure's block (call ID, 1-based)."""
        a = self.img.proc_address(pid - 1) if 0 < pid <= len(self.img.procs) else None
        if a is None:
            return None
        s, n = gcx.length(self.b, a)
        return s, s + n

    def text(self, pid):
        if pid not in self.lines:
            self.lines[pid] = gcx.disassemble(self.img, pid - 1) if 0 < pid <= len(self.img.procs) else []
        return self.lines[pid]

    def local(self, pid):
        return len(self.callers.get(pid, ())) <= 3 and pid not in DOORS

    def entrance_proc(self, pid):
        """Argument roles of an entrance procedure, or None."""
        if pid in self.entrance_cache:
            return self.entrance_cache[pid]
        info = None
        lines = self.text(pid)
        for d, t in lines[:3]:
            m = re.match(r"if \(r708\[0x70\]:t1 == arg(\d+)\) \{", t)
            if m:
                info = {"id": int(m.group(1))}
                break
        if info:
            box961 = None
            for d, t in lines:
                f = t.split()
                if f[0] == "proc_961" and len(f) > 2 and f[2].startswith("arg"):
                    box961 = int(f[2][3:])
                if f[0] == "proc_932" and len(f) > 3:
                    if f[1].startswith("arg") and f[2].startswith("arg"):
                        info.update(x=int(f[1][3:]), z=int(f[2][3:]))
                    elif f[1].startswith("r700[0x72]") and box961:
                        info["box"] = box961
                    if f[3].startswith("arg"):
                        info["facing"] = int(f[3][3:])
                    break
                if f[0] == "proc_931" and len(f) > 2 and f[1].startswith("arg"):
                    info["box"] = int(f[1][3:])
                    if f[2].startswith("arg"):
                        info["facing"] = int(f[2][3:])
                    break
        self.entrance_cache[pid] = info
        return info

    # ---- the walk ------------------------------------------------------------
    def scene(self, pid):
        self.out = {"scene": pid, "stage": None, "palette": None, "entrances": [], "doors": [], "items": [],
                    "enemies": [], "calls": []}
        self.visited = set()
        self.call(pid, [], [], None, 0, None)
        return self.out

    def subst(self, t, args):
        if args is None:
            return t
        return re.sub(r"\barg(\d+)\b", lambda m: args[int(m.group(1)) - 1]
                      if 0 < int(m.group(1)) <= len(args) else m.group(0), t)

    def call(self, pid, ops, conds, trap, depth, spawner):
        o, a = self.out, lambda k: ops[k - 1] if k and k <= len(ops) else None
        rec = {"proc": pid, "args": ops, "cond": list(conds)}
        if pid in (876, 884, 885) and len(ops) >= 3 and o["stage"] is None:
            o["stage"] = {"map": a(1), "tileset": a(2), "hp": a(3)}
            return
        if pid == 888 and o["palette"] is None:
            o["palette"] = a(1)
            return
        if pid in DOORS:
            bx, sc, en = DOORS[pid]
            o["doors"].append(dict(rec, box=a(bx) if bx else trap, scene=num(a(sc)), entrance=num(a(en))))
            return
        if pid in (1118, 1119):
            chest = pid == 1119 and num(a(6)) == 1
            flags, flag = (a(6), a(7)) if pid == 1118 else (a(8), a(9))
            o["items"].append(dict(rec, category=num(a(1)), item=num(a(2)), x=num(a(3)), z=num(a(4)),
                                   chest=chest, flags=flags, flag=num(flag)))
            return
        ent = self.entrance_proc(pid)
        if ent:
            e = dict(rec, id=num(a(ent["id"])), facing=num(a(ent.get("facing"))))
            if "x" in ent:
                e.update(x=num(a(ent["x"])), z=num(a(ent["z"])))
            if "box" in ent:
                e["box"] = a(ent["box"])
            o["entrances"].append(e)
            return
        if pid in ENEMY_PROCS and spawner is None:
            spawner = dict(rec, families=[], name=None, path=None)
            o["enemies"].append(spawner)
        elif not self.local(pid) and spawner is None:
            o["calls"].append(rec)
            return
        if depth > 8 or (pid, tuple(ops)) in self.visited:
            return
        self.visited.add((pid, tuple(ops)))
        span = self.body(pid)
        if span:
            self.block(span[0], span[1], ops, conds, trap, depth + 1, spawner)

    def block(self, p, end, args, conds, trap, depth, spawner):
        b = self.b
        while p < end:
            op = b[p]
            if op == 0:
                break
            start, n = gcx.length(b, p)
            kind = op >> 4
            if kind == 7:
                pid = b[start] | b[start + 1] << 8
                ops = [self.subst(t, args) for t in gcx.operands(b, start + 2, start + n) if t != "<end>"]
                self.call(pid, ops, conds, trap, depth, spawner)
            elif kind == 6:
                self.command(start, n, args, conds, trap, depth, spawner)
            p = start + n

    def command(self, start, n, args, conds, trap, depth, spawner):
        b = self.b
        cid = b[start] | b[start + 1] << 8
        q, alen = gcx.arglist(b, start + 2)
        aend, ops, blocks = q + alen, [], []
        while q < aend:
            if b[q] & 0xF0 == 0x80:
                s2, n2 = gcx.length(b, q)
                blocks.append((s2, s2 + n2))
                q = s2 + n2
                continue
            t, q = gcx.operand(b, q)
            ops.append(self.subst(t, args))
        kw = []  # (key, operand texts, block span or None)
        while q < start + n and b[q] & 0xF0 == 0x50:
            s2, n2 = gcx.length(b, q)
            key, r, vals, blk = chr(b[s2]), s2 + 1, [], None
            while r < s2 + n2:
                if b[r] & 0xF0 == 0x80:
                    s3, n3 = gcx.length(b, r)
                    blk = (s3, s3 + n3)
                    r = s3 + n3
                    continue
                t, r = gcx.operand(b, r)
                vals.append(self.subst(t, args))
            kw.append((key, vals, blk))
            q = s2 + n2
        name = gcx.COMMANDS.get(cid)
        if name == "if":
            cond = ops[0] if ops else "?"
            for s, e in blocks:
                self.block(s, e, args, conds + [cond], trap, depth, spawner)
            prev = [cond]
            for key, vals, blk in kw:
                if not blk:
                    continue
                if key == "i":
                    c = vals[0] if vals else "?"
                    self.block(*blk, args, conds + [f"not {x}" for x in prev] + [c], trap, depth, spawner)
                    prev.append(c)
                elif key == "e":
                    self.block(*blk, args, conds + [f"not {x}" for x in prev], trap, depth, spawner)
            return
        if name == "switch":
            v = ops[0] if ops else "?"
            for key, vals, blk in kw:
                if blk:
                    c = f"{v} == {vals[0]}" if key == "c" and vals else f"{v} default"
                    self.block(*blk, args, conds + [c], trap, depth, spawner)
            return
        if name == "trap":
            box = ops[0] if ops else None
            for key, vals, blk in kw:
                if blk and key == "e":  # the event: its arguments come from the trap, not from here
                    self.block(*blk, None, conds, box, depth, spawner)
            return
        key = num(ops[0]) if ops and (cid == 0xB745 or name == "chara") else None
        if spawner is not None and key in FAMILIES:
            if FAMILIES[key] not in spawner["families"]:
                spawner["families"].append(FAMILIES[key])
            k = dict((x, v) for x, v, _ in kw)
            spawner["name"] = spawner["name"] or (k.get("N") or [None])[0]
            if spawner["path"] is None and k.get("r"):
                spawner["path"] = num(k["r"][0])
        for s, e in blocks:
            self.block(s, e, args, conds, trap, depth, spawner)


def resolve(scene, hps, names):
    """Attach stage data: box rectangles, item names, enemy path starts."""
    st = scene["stage"]
    hpd = None
    if st and num(st["hp"]) is not None:
        hpd = hps.get(num(st["hp"]))
        st.update({k: f"{num(st[k]):04X}" for k in ("map", "tileset", "hp") if num(st[k]) is not None})
    if scene["palette"] is not None and num(scene["palette"]) is not None:
        scene["palette"] = f"{num(scene['palette']):04X}"
    boxes = {int(x["name"], 16): x for x in hpd["boxes"]} if hpd else {}

    def box(rec):
        k = num(rec.get("box"))
        if k is None:
            return
        rec["box"] = f"{k:04X}"
        if k in boxes:
            bx = boxes[k]
            rec["rect"] = [bx["x0"], bx["z0"], bx["x1"], bx["z1"]]
            rec.setdefault("x", (bx["x0"] + bx["x1"]) // 2)
            rec.setdefault("z", (bx["z0"] + bx["z1"]) // 2)
            rec.setdefault("y", bx["y0"])
    for e in scene["entrances"] + scene["doors"]:
        box(e)
    for it in scene["items"]:
        if it["category"] == 0 and it["item"] is not None:
            it["name"] = names.get(CONSUMABLE_NAMES + it["item"])
        if it["x"] is not None:
            it["x"] += 128
            it["z"] += 128
    for en in scene["enemies"]:
        p = en.get("path")
        if hpd and p is not None and 0 <= p < len(hpd["paths"]):
            en["route"] = [[q["x"], q["z"]] for q in hpd["paths"][p]]
            en["x"], en["z"] = en["route"][0]
    scene["enemies"] = [en for en in scene["enemies"] if en["families"]]
    for rec in scene["entrances"] + scene["doors"] + scene["items"] + scene["enemies"]:
        conds = []
        for c in rec["cond"]:
            for raw, nice in PRETTY:
                c = c.replace(raw, nice)
            if c not in conds and c != "({ proc_872 } == 0)":
                conds.append(c)
        rec["cond"] = conds
        if hpd and rec.get("x") is not None and rec.get("z") is not None and "y" not in rec:
            cx, cz = rec["x"] // 256, rec["z"] // 256
            if 0 <= cx < hpd["width"] and 0 <= cz < hpd["height"]:
                rec["y"] = hpd["cells"][cz * hpd["width"] + cx][0] * 256
    del scene["calls"]
    return scene


def build(only=None):
    rom = load_rom()
    import scenes as scenes_mod
    sc = scenes_mod.scenes()
    w = Walker(rom)
    hps = {}
    for h, raw in hpmod.entries(rom).items():
        hps[h] = hpmod.decode(raw)
    names = dict(text.strings(rom))
    todo = [s["scene"] for s in sc if not only or s["scene"] in only]
    out, seen = [], set(todo)
    while todo:  # door destinations that scenes.py does not list are scenes too
        s = resolve(w.scene(todo.pop(0)), hps, names)
        out.append(s)
        for d in s["doors"]:
            if not only and d["scene"] and d["scene"] not in seen and w.body(d["scene"]):
                seen.add(d["scene"])
                todo.append(d["scene"])
    return sorted(out, key=lambda s: s["scene"])


def summary(s):
    st = s["stage"] or {}
    print(f"scene {s['scene']}: map {st.get('map')} tileset {st.get('tileset')} hp {st.get('hp')} "
          f"palette {s['palette']}")
    cond = lambda r: f"  [if {' and '.join(r['cond'])}]" if r["cond"] else ""  # noqa: E731
    for e in s["entrances"]:
        where = f"box {e['box']}" if e.get("box") else ""
        print(f"  entrance {e['id']}: ({e.get('x')}, {e.get('y')}, {e.get('z')}) {where} facing {e['facing']}"
              f" via proc_{e['proc']}{cond(e)}")
    for d in s["doors"]:
        print(f"  door box {d['box']} ({d.get('x')}, {d.get('z')}) -> scene {d['scene']} entrance {d['entrance']}"
              f" via proc_{d['proc']}{cond(d)}")
    for it in s["items"]:
        print(f"  {'chest' if it['chest'] else 'item'} {it['category']}:{it['item']} {it.get('name') or ''} at "
              f"({it['x']}, {it['z']}) flag {it['flags']}/{it['flag']}{cond(it)}")
    for en in s["enemies"]:
        print(f"  enemy {'/'.join(en['families']) or '?'} name {en['name']} path {en['path']} "
              f"at ({en.get('x')}, {en.get('z')}) via proc_{en['proc']} {' '.join(en['args'])}{cond(en)}")


def main():
    cmd = sys.argv[1] if len(sys.argv) > 1 else "json"
    if cmd == "show":
        for s in build({int(x) for x in sys.argv[2:]}):
            summary(s)
    elif cmd == "json":
        path = sys.argv[2] if len(sys.argv) > 2 else os.path.join(ROOT, "build", "assets", "stages.json")
        res = build()
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path, "w", encoding="utf-8") as f:
            json.dump(res, f, ensure_ascii=False)
        n = lambda k: sum(len(s[k]) for s in res)  # noqa: E731
        print(f"{len(res)} scenes: {n('doors')} doors, {n('entrances')} entrances, {n('items')} items, "
              f"{n('enemies')} enemy spawners -> {os.path.relpath(path, ROOT)}")
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main()
