"""Persistent index of artwork actually selected by the game and its lettering."""
import json
from pathlib import Path


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
            self.db.execute("INSERT OR IGNORE INTO artwork(asset,context,preview) VALUES (?,?,?)",
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
            self.db.execute("UPDATE artwork SET state=?,origin=?,note=NULL WHERE asset=?",
                            ("lettering" if regions else "no lettering", origin, asset))

    def error(self, asset, message):
        with self.db:
            self.db.execute("UPDATE artwork SET state='review', note=? WHERE asset=?", (message, asset))
