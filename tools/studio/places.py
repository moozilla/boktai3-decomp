"""The world's places: the areas (tools/assets/areas.py, scenes grouped by
their doors) grouped by what their labels say they belong to. An area labeled
"Ancient Tree (candle-switch room)" is in the place "Ancient Tree", and so is
"Ancient Tree optional challenge / ..."; "Dark Castle A" stays its own place.
The World view (/world) draws them and the search (tools/studio/search.py)
finds them.
"""
import re


def base_of(label):
    """The label before any "(...)" or " / ...": "Arena / Black Knight challenge" -> "Arena"."""
    return re.sub(r"\s*(\(| / ).*$", "", label or "").strip()


def split(labels):
    """{label: (place, detail)} for area labels: a label's place is the shortest
    other base it extends with a lowercase word ("Ancient Tree optional
    challenge" is in "Ancient Tree", "Dark Castle A" is its own place), and the
    detail is the rest ("candle-switch room", "route transition / Count event")."""
    bases = sorted({base_of(x) for x in labels} - {""}, key=len)

    def place_of(b):
        for m in bases:
            if b == m or (b.startswith(m + " ") and b[len(m) + 1:][:1].islower()):
                return m
        return b

    out = {}
    for label in labels:
        b = base_of(label)
        if not b:
            continue
        p = place_of(b)
        if label.startswith(f"{p} (") and label.endswith(")"):
            detail = label[len(p) + 2:-1]
        else:
            detail = re.sub(r"^[\s/–-]+", "", label[len(p):])
        out[label] = (p, detail)
    return out


def assign(w):
    """Add "place" and "detail" to each area of W (serve.world()) and return
    the places in story order (an area's "story_order", then the lowest scene id): [{name, areas,
    scenes, enemies, hue, shot}]. hue is None for a one-area place."""
    sp = split([a.get("label") or "" for a in w["areas"]])
    by = {}
    for a in w["areas"]:
        a["place"], a["detail"] = sp.get(a.get("label") or "", (None, ""))
        p = a["place"]
        if p is None:
            continue
        e = by.setdefault(p, {"name": p, "areas": [], "scenes": 0, "enemies": {}})
        e["areas"].append(a["area"])
        e["scenes"] += len(a["scenes"])
        for f, k in a.get("enemies", {}).items():
            e["enemies"][f] = e["enemies"].get(f, 0) + k
    area = {a["area"]: a for a in w["areas"]}
    # story order: data/labels/areas.csv's story_order (0 = unknown, last), then the lowest scene id
    def rank(p):
        orders = [area[i].get("story_order") or 0 for i in p["areas"]]
        return (min((o for o in orders if o), default=999), min(area[i]["scenes"][0] for i in p["areas"]))
    places = sorted(by.values(), key=rank)
    n = 0
    for p in places:
        if len(p["areas"]) > 1 or p["scenes"] >= 10:
            p["hue"] = round((20 + 137.5 * n) % 360, 1)
            n += 1
        else:
            p["hue"] = None
        biggest = sorted((area[i] for i in p["areas"]), key=lambda a: -len(a["scenes"]))
        p["shot"] = next((s for a in biggest for s in a["scenes"] if w["scenes"].get(s, w["scenes"].get(str(s), {})).get("shot")), None)
        for i in p["areas"]:
            area[i]["hue"] = p["hue"]
    return places
