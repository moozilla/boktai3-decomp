"""Persistent index of artwork actually selected by the game and its lettering."""
from io import BytesIO
import json
import re
import tempfile
from pathlib import Path


def write_cache(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(dir=path.parent, delete=False) as f:
        f.write(data)
        tmp = Path(f.name)
    tmp.replace(path)


def png(image):
    out = BytesIO()
    image.save(out, format="PNG")
    return out.getvalue()


class ArtIndex:
    def __init__(self, memory):
        self.db = memory.db
        self.db.executescript("""
            CREATE TABLE IF NOT EXISTS artwork (
                asset TEXT PRIMARY KEY, context TEXT NOT NULL, preview TEXT NOT NULL,
                state TEXT NOT NULL DEFAULT 'unexamined', origin TEXT, note TEXT);
            CREATE TABLE IF NOT EXISTS regions (
                asset TEXT NOT NULL, id TEXT NOT NULL, box TEXT NOT NULL,
                source TEXT NOT NULL, english TEXT, origin TEXT NOT NULL,
                PRIMARY KEY(asset, id));
        """)

    def encounter(self, asset, context, preview):
        with self.db:
            self.db.execute("""INSERT INTO artwork(asset,context,preview) VALUES (?,?,?)
                            ON CONFLICT(asset) DO UPDATE SET context=excluded.context, preview=excluded.preview""",
                            (asset, json.dumps(context, ensure_ascii=False), preview))

    def status(self, asset):
        return self.db.execute("SELECT * FROM artwork WHERE asset=?", (asset,)).fetchone()

    def regions(self, asset):
        rows = self.db.execute("SELECT * FROM regions WHERE asset=? ORDER BY id", (asset,))
        return [dict(row, box=json.loads(row["box"])) for row in rows]

    def mark(self, asset, regions, origin):
        row = self.status(asset)
        if not row:
            raise ValueError("Play this asset first so its original dimensions are known")
        context = json.loads(row["context"])
        w, h = context["width_px"], context["height_px"]
        for r in regions:
            if not isinstance(r.get("id"), str) or not re.fullmatch(r"[A-Za-z0-9_-]{0,48}",r["id"]):
                raise ValueError("A graphic region needs a short identifier")
            box = r.get("box")
            if (not isinstance(box, list) or len(box) != 4 or any(type(n) is not int for n in box)
                    or not (0 <= box[0] < box[2] <= w and 0 <= box[1] < box[3] <= h)):
                raise ValueError(f"Invalid box in {w} by {h} graphic")
            if not isinstance(r.get("source"), str) or not r["source"].strip():
                raise ValueError("Each region needs an original Japanese reading")
            if r.get("english") is not None and (not isinstance(r["english"], str) or
                    not r["english"].strip() or not all(32 <= ord(c) < 127 or c == "\n" for c in r["english"])):
                raise ValueError("Artwork lettering must be printable English")
        with self.db:
            for r in regions:
                self.db.execute("""INSERT INTO regions VALUES (?,?,?,?,?,?)
                    ON CONFLICT(asset,id) DO UPDATE SET box=excluded.box,source=excluded.source,
                    english=excluded.english,origin=excluded.origin""",
                    (asset, r["id"], json.dumps(r["box"]), r["source"], r.get("english"), origin))
                if origin == "human correction":
                    key = asset + ("/"+r["id"] if r["id"] else "")
                    prev = self.db.execute("SELECT english FROM entries WHERE key=?", (key,)).fetchone()
                    if prev:
                        self.db.execute("INSERT INTO edits(key,previous,english) VALUES (?,?,?)",
                                        (key, prev["english"], r.get("english")))
                        self.db.execute("UPDATE entries SET source=?,english=?,origin=? WHERE key=?",
                                        (r["source"], r.get("english"), origin, key))
            self.db.execute("UPDATE artwork SET state=?,origin=?,note=NULL WHERE asset=?",
                            ("lettering" if regions else "no lettering", origin, asset))

    def translate(self, resolver, game, asset, image, ctx, preview, recipes, frame):
        from runtime import ROOT
        from translator import PROVIDER
        if not preview.exists():
            write_cache(preview, png(image))
        self.encounter(asset, ctx, str(preview.relative_to(ROOT)))
        resolver.record(asset, event="selected", frame=frame, **ctx)
        if not resolver.enabled:
            for r in recipes:
                key = asset + ("/"+r["id"] if r["id"] else "")
                resolver.record(key, event="japanese", source=r["source"], frame=frame, **ctx)
            return []
        state = self.status(asset)["state"]
        if state == "unexamined" or (state == "review" and not self.regions(asset)):
            if recipes:
                self.mark(asset, recipes, PROVIDER)
            elif resolver.memory.model:
                found = resolver.memory.model.locate(ctx, resolver.memory.glossary(), png(image))
                self.mark(asset, found, resolver.memory.model.model)
        authored = {r["id"]: r for r in recipes}
        choices = []
        for region in self.regions(asset):
            rid = region["id"]
            key = asset + ("/"+rid if rid else "")
            box = region["box"]
            crop = image.crop(box)
            region.update({k:v for k,v in authored.get(rid, {}).items() if k in ("font", "style")})
            layout = dict(ctx, region=box, width_px=crop.width, height_px=crop.height,
                          typography=region.get("style", "sampled"))
            if ctx.get("archive") == "C091":
                layout["font_px"] = region.get("font", min(15, crop.height))
            resolver.bookmark_choice(game, key, region["source"], layout, frame, preview)
            if resolver.original_key == key:
                resolver.record(key, source=region["source"], event="japanese", frame=frame, **layout)
                continue
            english, origin, event = resolver.memory.choose(key, region["source"], layout, png(crop),
                                                            region["english"], region["origin"])
            resolver.record(key, source=region["source"], english=english, provenance=origin,
                            event=event, frame=frame, **layout)
            if english:
                choices.append((region, english))
        return choices

    def error(self, asset, message):
        with self.db:
            self.db.execute("UPDATE artwork SET state='review', note=? WHERE asset=?", (message, asset))
