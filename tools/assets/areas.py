#!/usr/bin/env python3
"""Group scenes into areas by their doors (tools/assets/stages.py).

  areas.py list                    one line per area: scenes, maps, links, label
  areas.py show AREA...            an area's scenes and their doors
  areas.py json [OUT]              everything (default build/assets/areas.json)

The door graph has an edge for every door (stages.py: a scene's door loads
another scene at an entrance). Two scenes are in the same area when a door
connects them and their scene ids are less than GAP apart: rooms of one
dungeon or town sit next to each other in the script numbering, while doors
across larger gaps lead to the world map, another town or a story scene.
Areas are numbered by their lowest scene id. This is a working grouping for
browsing, not a game structure; with today's data it gives 76 areas, the same
numbering the labeling pass used (data/labels/areas.csv).

Labels: an area takes the labeling pass place (data/labels/areas.csv, a Codex
pass that read the walkthroughs and the translation project's guide) of the
labeled area that shares the most scenes with it (data/labels/scenes.csv);
scenes keep their data/notes.csv label (kind=script, key = scene).
Output is ROM-derived; keep it under build/.
"""
import collections
import csv
import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
sys.path.insert(0, HERE)
from romlib import ROOT  # noqa: E402

GAP = 60
STAGES = os.path.join(ROOT, "build", "assets", "stages.json")
LABELS = os.path.join(ROOT, "data", "labels")
NOTES = os.path.join(ROOT, "data", "notes.csv")


def load_stages():
    """stages.py output, from build/assets/stages.json when it is newer than the tools."""
    tools = [os.path.join(HERE, n) for n in ("stages.py", "gcx.py", "hp.py")]
    if os.path.exists(STAGES) and os.path.getmtime(STAGES) >= max(map(os.path.getmtime, tools)):
        return json.load(open(STAGES, encoding="utf-8"))
    import stages
    res = stages.build()
    if not res:
        sys.exit("stages.py found no scenes (needs gen/code.s and baserom.gba; run make first)")
    os.makedirs(os.path.dirname(STAGES), exist_ok=True)
    with open(STAGES, "w", encoding="utf-8") as f:
        json.dump(res, f, ensure_ascii=False)
    return res


def group(scenes, gap=GAP):
    """[[scene id, ...], ...] sorted by lowest id: union-find over short doors."""
    ids = {s["scene"] for s in scenes}
    parent = {i: i for i in ids}

    def find(x):
        while parent[x] != x:
            parent[x] = parent[parent[x]]
            x = parent[x]
        return x
    for s in scenes:
        for d in s["doors"]:
            t = d["scene"]
            if t in ids and abs(t - s["scene"]) < gap:
                a, b = find(s["scene"]), find(t)
                if a != b:
                    parent[max(a, b)] = min(a, b)
    out = collections.defaultdict(list)
    for i in ids:
        out[find(i)].append(i)
    return sorted((sorted(v) for v in out.values()), key=lambda v: v[0])


def vision_labels():
    """({labeled area: {label, confidence, evidence}}, {scene: labeled area}, {scene: row})."""
    areas, scene_area, scene_rows = {}, {}, {}
    f = os.path.join(LABELS, "areas.csv")
    if os.path.exists(f):
        for r in csv.DictReader(open(f, encoding="utf-8")):
            areas[int(r["area"])] = dict(r, label=r["place"])
    f = os.path.join(LABELS, "scenes.csv")
    if os.path.exists(f):
        for r in csv.DictReader(open(f, encoding="utf-8")):
            if r["area"].isdigit():
                scene_area[int(r["scene"])] = int(r["area"])
            scene_rows[int(r["scene"])] = r
    return areas, scene_area, scene_rows


def script_notes():
    """{scene: label} from data/notes.csv kind=script."""
    if not os.path.exists(NOTES):
        return {}
    out = {}
    for r in csv.DictReader(open(NOTES, encoding="utf-8")):
        if r["kind"] == "script" and r["key"].isdigit():
            out[int(r["key"])] = r["label"]
    return out


def build(scenes=None, gap=GAP):
    """{"areas": [...], "scene_area": {scene: area}, "links": [...], "scenes": {...}}."""
    scenes = scenes if scenes is not None else load_stages()
    by_id = {s["scene"]: s for s in scenes}
    groups = group(scenes, gap)
    area_of = {i: n for n, g in enumerate(groups) for i in g}
    v_areas, v_scene_area, v_scenes = vision_labels()
    notes = script_notes()

    links = collections.Counter()
    for s in scenes:
        for d in s["doors"]:
            a, b = area_of[s["scene"]], area_of.get(d["scene"])
            if b is not None and a != b:
                links[(a, b)] += 1
    pairs = collections.Counter()
    for (a, b), n in links.items():
        pairs[(min(a, b), max(a, b))] += n

    out_areas = []
    for n, g in enumerate(groups):
        votes = collections.Counter(v_scene_area[i] for i in g if i in v_scene_area)
        v = v_areas.get(votes.most_common(1)[0][0] if votes else n, {})
        doors = [d for i in g for d in by_id[i]["doors"]]
        families = collections.Counter(f for i in g for e in by_id[i]["enemies"] for f in e["families"])
        out_areas.append({
            "area": n, "scenes": g,
            "label": v.get("label") or None, "confidence": v.get("confidence") or None,
            "evidence": v.get("evidence") or None,
            "maps": sorted({by_id[i]["stage"]["map"] for i in g if by_id[i]["stage"]}),
            "doors": len(doors), "inner": sum(1 for d in doors if area_of.get(d["scene"]) == n),
            "items": sum(len(by_id[i]["items"]) for i in g),
            "enemies": dict(families.most_common()),
            "out": {b: k for (a, b), k in sorted(links.items()) if a == n},
            "in": {a: k for (a, b), k in sorted(links.items()) if b == n},
        })

    out_scenes = {}
    for s in scenes:
        i = s["scene"]
        vr = v_scenes.get(i, {})
        out_scenes[i] = {
            "scene": i, "area": area_of[i], "map": (s["stage"] or {}).get("map"), "stage": s["stage"],
            "label": notes.get(i) or vr.get("label") or None,
            "vision": {k: vr[k] for k in ("label", "confidence", "evidence")} if vr else None,
            "doors": [{"scene": d["scene"], "entrance": d["entrance"], "box": d.get("box"), "cond": d["cond"]}
                      for d in s["doors"]],
            "entrances": sorted({e["id"] for e in s["entrances"] if e.get("id") is not None}),
            "items": len(s["items"]), "enemies": sorted({f for e in s["enemies"] for f in e["families"]}),
        }
    return {"gap": gap, "areas": out_areas, "scene_area": area_of, "scenes": out_scenes,
            "links": [{"a": a, "b": b, "doors": k} for (a, b), k in sorted(pairs.items())]}


def main():
    cmd = sys.argv[1] if len(sys.argv) > 1 else "list"
    w = build()
    if cmd == "list":
        for a in w["areas"]:
            nb = sorted(set(a["out"]) | set(a["in"]))
            s = a["scenes"]
            rng = f"{s[0]}" if len(s) == 1 else f"{s[0]}-{s[-1]}"
            print(f"{a['area']:3d}  {len(s):3d} scenes {rng:<10} {len(a['maps']):2d} maps  "
                  f"links {','.join(map(str, nb)) or '-':<16} {a['label'] or '?'}"
                  f"{' (' + a['confidence'] + ')' if a['confidence'] else ''}")
        print(f"{len(w['areas'])} areas, {len(w['scenes'])} scenes, {len(w['links'])} area links (gap < {w['gap']})")
    elif cmd == "show":
        for n in map(int, sys.argv[2:]):
            a = w["areas"][n]
            print(f"area {n}: {a['label'] or '?'} ({a['confidence'] or 'no label'})")
            for i in a["scenes"]:
                s = w["scenes"][i]
                print(f"  scene {i} map {s['map'] or '-'}  {s['label'] or ''}")
                for d in s["doors"]:
                    other = w["scene_area"].get(d["scene"])
                    where = "" if other == n else f"  [area {other}]"
                    cond = f"  if {' and '.join(d['cond'])}" if d["cond"] else ""
                    print(f"    -> scene {d['scene']} entrance {d['entrance']}{where}{cond}")
    elif cmd == "json":
        path = sys.argv[2] if len(sys.argv) > 2 else os.path.join(ROOT, "build", "assets", "areas.json")
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path, "w", encoding="utf-8") as f:
            json.dump(w, f, ensure_ascii=False)
        print(f"{len(w['areas'])} areas, {len(w['links'])} links -> {os.path.relpath(path, ROOT)}")
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main()
