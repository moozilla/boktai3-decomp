"""One search over everything the Studio knows (the ⌘K palette on every page,
tools/studio/common.js): the game index (tools/studio/atlas.py: monsters,
characters, places, items, sprites, songs, scripts ...), code (functions, units
and RAM objects, tools/studio/codemap.py), named script procedures and the text
bank. Each hit links to the page that shows it.

  python3 tools/studio/search.py QUERY
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
for p in (os.path.join(ROOT, "tools", "assets"), os.path.join(ROOT, "tools"), HERE):
    if p not in sys.path:
        sys.path.insert(0, p)

ATLAS_KIND = {"package": "sprite", "actor": "object", "monster": "monster", "char": "character", "item": "item",
              "area": "area", "scene": "scene", "file": "script", "sprite": "sprite", "unit": "subsystem",
              "tileset": "tileset", "bgtiles": "map tiles", "song": "song", "cut": "cutscene", "beat": "story"}
# how a tie between equally good matches is broken: the things people look for first
RANK = {"monster": 9, "character": 8, "object": 8, "item": 7, "area": 6, "story": 5, "scene": 5, "cutscene": 4,
        "sprite": 4, "function": 4, "song": 3, "script": 3, "procedure": 3, "tileset": 2, "map tiles": 2,
        "RAM": 3, "unit": 3, "subsystem": 1, "text": 1}


def score(q, title, *others):
    """100 exact, 80 prefix, 70 word start, 60 inside the title; 40 inside another field."""
    t = (title or "").lower()
    if t == q:
        return 100
    if t.startswith(q):
        return 80
    if re.search(r"(?:^|[\s_(/-])" + re.escape(q), t):
        return 70
    if q in t:
        return 60
    if any(q in (o or "").lower() for o in others):
        return 40
    return 0


def _atlas():
    try:
        import atlas
        return atlas.load()["entities"]
    except Exception:
        return {}


def _code():
    try:
        import codemap
        return codemap.summary()
    except Exception:
        return None


def _procs():
    try:
        import gclview
        return {int(p): r["name"] for p, r in gclview.names().procs.items() if r.get("name")}
    except Exception:
        return {}


def search(q, strings=None, english=None, limit=50):
    """Hits for Q, best first: {kind, title, sub, url}. STRINGS is the text bank
    ([{id, text}]) and ENGLISH the fan translation by id, when the caller has them."""
    q = q.strip().lower()
    if not q:
        return []
    hits = []

    def add(s, kind, title, sub, url):
        if s:
            hits.append((s, RANK.get(kind, 0), kind, title, sub, url))

    english = english or {}
    for e in _atlas().values():
        kind = ATLAS_KIND.get(e["kind"], e["kind"])
        names = " ".join(f"{n.get('jp', '')} {n.get('en', '')}" for n in e.get("names") or [])
        title = e.get("title") or e["id"]
        if e["kind"] in ("monster", "char") and e.get("key", "").isdigit():
            en = english.get(int(e["key"])) or english.get(e["key"])  # their titles are string ids' Japanese
            if en:
                title, names = en, names + " " + title
        add(score(q, title, e.get("key"), e.get("jp"), e.get("fname"), names), kind, title,
            e.get("jp") if e.get("jp") and e.get("jp") != title else e.get("path") or "", "/atlas#" + e["id"])
    code = _code()
    if code:
        hexq = re.sub(r"^(0x|sub_)", "", q)
        for c in code["categories"]:
            for u in c["units"]:
                add(score(q, u["name"]), "unit", u["name"], c["name"], f"/code#unit:{c['name']}/{u['name']}")
                for name, addr, *_ in u["functions"]:
                    s = score(q, name)
                    if not s and len(hexq) >= 4 and addr.lower().startswith(hexq):
                        s = 75
                    add(s, "function", name, u["name"], "/code#" + addr)
        for r in code["ram"]:
            add(score(q, r["name"], r["addr"]), "RAM", r["name"], r["addr"], "/code#ram:" + r["name"])
    for pid, name in _procs().items():
        add(score(q, name, str(pid)), "procedure", name, f"proc {pid}", f"/scripts#{pid}")
    if strings and len(q) >= 2:
        n = 0
        for s in strings:
            en = english.get(s["id"]) or english.get(str(s["id"])) or ""
            if q in s["text"].lower() or q in en.lower() or q == str(s["id"]):
                hits.append((30, 0, "text", en or s["text"], f"#{s['id']}" + (f" · {s['text']}" if en else ""), f"/text#{s['id']}"))
                n += 1
                if n >= 15:
                    break
    hits.sort(key=lambda h: (-h[0], -h[1], h[3].lower()))
    return [{"kind": k, "title": t, "sub": sub, "url": url} for _, _, k, t, sub, url in hits[:limit]]


if __name__ == "__main__":
    for h in search(" ".join(sys.argv[1:])):
        print(f"{h['kind']:>10}  {h['title']}  ({h['sub']})  {h['url']}")
