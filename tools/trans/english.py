#!/usr/bin/env python3
"""Build an English ROM from the fan translation (github.com/moozilla/boktai3trans).

  python3 tools/trans/english.py [--trans ../boktai3trans] [--out build/english.gba]

1. Imports scriptEd/script.sjs (English per string ID, the Japanese kept as
   // comments) into a copy of the extracted text bank, build/text/english.tsv.
   Strings without a translation keep their Japanese.
2. Builds the ROM through the normal pipeline with that bank (text may grow;
   later data moves).
3. Applies the graphics/menu/code records of ips/latest_translation.ips, but not
   its text-bank records (step 1 replaces those). Code and the archives sit
   before the text bank, so their offsets are unchanged; the patch's new code
   and data live in the ROM's trailing zero padding, which is checked to still
   be free.
Everything produced is ROM-derived and stays under build/.
"""
import argparse
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.join(ROOT, "tools"))
from datafmt import load_manifest, text_bank  # noqa: E402

TEXT_START, TEXT_END = 0x08D6B1E4 - 0x08000000, 0x08DD6B96 - 0x08000000


def fan_table(path):
    """Glyph index for each {name} or character in the fan table."""
    raw = open(path, "rb").read().decode("shift_jis", errors="replace").replace("\r", "")
    out = {}
    for ln in raw.split("\n"):
        m = re.match(r"^([0-9A-Fa-f]{4})=(.+)$", ln)
        if m and 0x8000 <= int(m.group(1), 16) < 0x8600:
            out.setdefault(m.group(2), int(m.group(1), 16) - 0x8000)
    return out


def fan_script(path):
    raw = open(path, "rb").read().decode("shift_jis", errors="replace").replace("\r", "")
    blocks = {}
    for m in re.finditer(r"\$E=====(\d+)=====\n(.*?)(?=\n\$E=====|\Z)", raw, re.S):
        lines = m.group(2).split("\n")
        jp = [ln[2:] for ln in lines if ln.startswith("//")]
        en = [ln for ln in lines if not ln.startswith("//")]
        while en and en[-1] == "":
            en.pop()
        blocks[int(m.group(1))] = ("\n".join(jp), "\n".join(en))
    return blocks


def to_source(text, table, inverse):
    """Fan notation -> text_bank source notation."""
    out = []
    for m in re.finditer(r"(\{[^}]*\})|(.)", text, re.S):
        brace, ch = m.groups()
        if brace:
            g = table.get(brace)
            out.append(f"{{{g:04X}}}" if g is not None else brace.replace("{", "\\{"))
        elif ch == "\n":
            out.append("\\n")
        elif ch == "\\":
            out.append("\\\\")
        elif ord(ch) < 0x80 or ch in inverse:
            out.append(ch)
        elif ch in table:
            out.append(f"{{{table[ch]:04X}}}")
        else:
            raise ValueError(f"character {ch!r} not in the game font")
    return "".join(out)


def import_script(trans, src, dst):
    table = fan_table(os.path.join(trans, "scriptEd", "chartable.sjs.tbl"))
    blocks = fan_script(os.path.join(trans, "scriptEd", "script.sjs"))
    inverse = {c: g for g, c in text_bank.charmap().items()}
    lines, n, skipped = [], 0, []
    for ln in open(src, encoding="utf-8").read().split("\n"):
        p = ln.split("\t")
        if len(p) > 2 and p[1] == "T":
            sid = int(p[0])
            jp, en = blocks.get(sid, ("", ""))
            if jp and en and en != jp:
                try:
                    new = to_source(en, table, inverse)
                    text_bank.encode_string(new, inverse)
                    ln = f"{sid}\tT\t{new}"
                    n += 1
                except ValueError as e:
                    skipped.append((sid, str(e)))
        lines.append(ln)
    open(dst, "w", encoding="utf-8").write("\n".join(lines))
    return n, skipped


def ips_records(path):
    d = open(path, "rb").read()
    assert d[:5] == b"PATCH"
    i, out = 5, []
    while d[i:i + 3] != b"EOF":
        o = int.from_bytes(d[i:i + 3], "big")
        n = int.from_bytes(d[i + 3:i + 5], "big")
        i += 5
        if n == 0:
            out.append((o, d[i + 2:i + 3] * int.from_bytes(d[i:i + 2], "big")))
            i += 3
        else:
            out.append((o, d[i:i + n]))
            i += n
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--trans", default=os.path.join(os.path.dirname(ROOT), "boktai3trans"))
    ap.add_argument("--out", default=os.path.join(ROOT, "build", "english.gba"))
    ap.add_argument("--patch", default="latest_translation.ips")
    a = ap.parse_args()
    entry = next(e for e in load_manifest() if e.name == "text_bank")
    if not os.path.exists(entry.file):
        entry.extract()
    bank = entry.file
    english = os.path.join(ROOT, "build", "text", "english.tsv")
    n, skipped = import_script(a.trans, bank, english)
    print(f"imported {n} translated strings; {len(skipped)} skipped")
    for sid, why in skipped[:20]:
        print(f"  #{sid}: {why}")
    # Build with the English bank in place of the extracted one, then restore it.
    saved = bank + ".japanese"
    shutil.copy2(bank, saved)
    try:
        shutil.copy(english, bank)
        subprocess.run(["make", "gen/data.s"], cwd=ROOT, check=True, stdout=subprocess.DEVNULL)
        subprocess.run([sys.executable, "tools/build.py", "--out", os.path.relpath(a.out, ROOT), "--no-compare"],
                       cwd=ROOT, check=True)
    finally:
        shutil.copy2(saved, bank)
        os.remove(saved)
        os.utime(bank)  # regenerate gen/data.s from the Japanese bank next time
    rom = bytearray(open(a.out, "rb").read())
    base = open(os.path.join(ROOT, "baserom.gba"), "rb").read()
    used = len(bytes(rom).rstrip(b"\0"))  # end of the built data
    applied = 0
    for o, data in ips_records(os.path.join(a.trans, "ips", a.patch)):
        if TEXT_START <= o < TEXT_END:
            continue  # text: replaced by the import
        if TEXT_END <= o < len(base.rstrip(b"\0")):
            raise SystemExit(f"{a.patch}: record at {o:07X} is in data that moved")
        if o >= TEXT_END and o < used:
            raise SystemExit(f"{a.patch}: record at {o:07X} overlaps grown data (ends {used:07X})")
        rom[o:o + len(data)] = data
        applied += 1
    open(a.out, "wb").write(rom)
    print(f"applied {applied} records of {a.patch}; data ends at {used:07X}; wrote {os.path.relpath(a.out, ROOT)}")


if __name__ == "__main__":
    main()
