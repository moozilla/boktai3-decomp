#!/usr/bin/env python3
"""Items, swords and accessories: names from the game's text bank, icon frames
in the sprite package 5D04 (the menu icons, drawn with sprite palette pool
entry 884).

  python3 tools/assets/items.py [--trans ../boktai3trans]   writes data/items.csv

Where it comes from (docs/ASSETS.md, Items):
  * Names are text strings at a fixed base plus the ID: items 988 + ID (the
    descriptions follow at 1043 + ID), swords 817 + ID, accessories 1107 + ID.
  * The bags in GameState hold IDs: items +0xA0 (u16 per slot), accessories
    +0x160 (u16), swords +0x1C0 (16-byte records, ID first), valuables +0x838
    (u16, item IDs 0x27 and up). Captures of the menus show which 5D04 frame
    each slot draws, which gives the frame runs below; `source` says whether a
    run was seen that way or only rests on matching DarthNemesis's guide icons.
English comes from the fan translation when it is present; it is not the
game's data.
"""
import argparse
import csv
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.join(ROOT, "tools"))
sys.path.insert(0, os.path.join(ROOT, "tools", "trans"))
sys.path.insert(0, os.path.join(ROOT, "tools", "mgba"))

SAVE = "frame run seen in menu captures (GameState bags, ShinBok2.sav)"
GUIDE = "icon match against the guide's images only"

# category: (first name string, count, [(first id, last id, frame of first id or None, same frame?, source)])
CATEGORIES = {
    "item": (988, 55, [
        (0x00, 0x1D, 337, False, SAVE),          # seen: 0x01, 0x0D, 0x0E, 0x15
        (0x1E, 0x1F, 368, False, GUIDE),         # loser/winner sticks
        (0x20, 0x26, 370, True, SAVE),           # tarot cards share one icon; seen: 0x24-0x26
        (0x27, 0x36, 372, False, SAVE),          # valuables; seen: 0x27, 0x2A, 0x2B
    ]),
    "sword": (817, 49, [(0, 48, 240, False, SAVE)]),        # seen: 40-45, 47
    "accessory": (1107, 48, [(0, 47, 289, False, SAVE)]),   # seen: 7-9, 24, 25, 28, 35, 36, 45, 46
}


def strings(trans):
    import gen_centaur_data as g
    from datafmt import load_manifest
    jp, en = {}, {}
    for e in load_manifest():
        if e.name == "text_bank":
            if not os.path.exists(e.file):
                e.extract()
            for ln in open(e.file, encoding="utf-8"):
                p = ln.rstrip("\n").split("\t")
                if len(p) > 2 and p[1] == "T":
                    jp[int(p[0])] = g.display(p[2])
    script = os.path.join(trans, "scriptEd", "script.sjs")
    if os.path.exists(script):
        import english
        for sid, (j, e) in english.fan_script(script).items():
            if j and e and e != j:
                en[sid] = g.display(e).strip()
    return jp, en


def rows(trans):
    jp, en = strings(trans)
    out = []
    for cat, (base, count, runs) in CATEGORIES.items():
        for i in range(count):
            frame, source = "", ""
            for a, b, f, same, src in runs:
                if a <= i <= b:
                    frame, source = (f if same else f + i - a), src
            sid = base + i
            out.append({"category": cat, "id": f"{i:02X}", "string": sid, "jp": jp.get(sid, ""),
                        "en": en.get(sid, ""), "icon": f"5D04/{frame}" if frame != "" else "", "source": source})
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--trans", default=os.path.join(os.path.dirname(ROOT), "boktai3trans"))
    a = ap.parse_args()
    rs = rows(a.trans)
    path = os.path.join(ROOT, "data", "items.csv")
    with open(path, "w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, ["category", "id", "string", "jp", "en", "icon", "source"])
        w.writeheader()
        w.writerows(rs)
    print(f"wrote {os.path.relpath(path, ROOT)}: {len(rs)} rows")


if __name__ == "__main__":
    main()
