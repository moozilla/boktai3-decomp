"""The world's places: the areas (tools/assets/areas.py, scenes grouped by
their doors) grouped by what their labels say they belong to. An area labeled
"Ancient Tree (candle-switch room)" is in the place "Ancient Tree", and so is
"Ancient Tree optional challenge / ..."; "Dark Castle A" stays its own place.
The World view (/world) draws them and the search (tools/studio/search.py)
finds them.
"""
import re


def base_of(label):
    return re.sub(r"\s*\(.*$", "", label or "").strip()


def assign(w):
    """Add "place" and "detail" to each area of W (serve.world()) and return
    the places in story order (by their lowest scene id): [{name, areas,
    scenes, enemies, hue, shot}]. hue is None for a one-area place."""
    bases = sorted({base_of(a.get("label")) for a in w["areas"]} - {""}, key=len)

    def place_of(b):
        for m in bases:
            if b == m or (b.startswith(m + " ") and b[len(m) + 1:][:1].islower()):
                return m
        return b

    by = {}
    for a in w["areas"]:
        b = base_of(a.get("label"))
        a["place"] = place_of(b) if b else None
        label, p = a.get("label") or "", a["place"]
        if not label:
            a["detail"] = ""
        elif label.startswith(f"{p} ("):
            a["detail"] = label[len(p) + 2:].removesuffix(")")
        elif label == p:
            a["detail"] = ""
        else:
            a["detail"] = re.sub(r"^[\s/–-]+", "", label[len(p):])
        if p is None:
            continue
        e = by.setdefault(p, {"name": p, "areas": [], "scenes": 0, "enemies": {}})
        e["areas"].append(a["area"])
        e["scenes"] += len(a["scenes"])
        for f, k in a.get("enemies", {}).items():
            e["enemies"][f] = e["enemies"].get(f, 0) + k
    area = {a["area"]: a for a in w["areas"]}
    places = sorted(by.values(), key=lambda p: min(area[i]["scenes"][0] for i in p["areas"]))
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
