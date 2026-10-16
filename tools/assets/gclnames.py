#!/usr/bin/env python3
"""Recovered names for GCL hashes (gcl/names/*.csv), and a tool to find more.

  gclnames.py hash TEXT...          the 16-bit hash of each TEXT (EUC-JP bytes)
  gclnames.py check                 every row's name hashes to its key
  gclnames.py crack KIND [--all]    corpus words whose hash is a KIND key, with
                                    what the key maps to (default: unnamed keys)
  gclnames.py find HASH...          corpus words with that hash (any kind)

GCL names are hashed with sub_08219BD8 (h = rotl16(h, 5) + byte) over their
EUC-JP bytes, as GCL source was EUC-JP (KCEJ GCL reference; the actor keys
confirm it: katakana names hit 46 keys hashed as EUC-JP, 12 as Shift-JIS and 6
as UTF-8, against 13 expected by chance). ASCII is the same in all three.

Kinds (one CSV each, columns key, name, english, confidence, evidence):
  chara    keys of data/actor_registry.csv, used by `chara KEY ...` (create an
           actor) and `command KEY ...` (call the routine, keep its result)
  command  native command ids (the tables at 0861418C, 08E8791C, 080F38C8)

gcl/prints.csv translates the scripts' Japanese debug `print` strings (text
as gcx.py prints it, English); gcx.py shows them as comments. `print` itself
(0821B858) outputs nothing in this ROM: it decodes its operands and converts
strings to Shift-JIS into a stack buffer, then drops them.

A key can have several rows: alternatives that hash alike. The first row
with the highest confidence is the one shown; the others stay for review.
Confidence: high = the hash and independent evidence (the sprite the actor
loads, the handler's behavior) agree; medium = a coherent game word plus
weaker context; low = a hash match only. A random word hits one of the 722
chara keys about 1.1% of the time, so `crack` prints the chance rate: a hit
on its own is a lead, not a name.

The corpus is the game's text bank (text.py) and the scripts' debug `print`
strings (EUC-JP, often the developers' own variable and procedure names),
split into words. Output is ROM-derived; names in gcl/names are facts about
hashes and may be committed.
"""
import csv
import functools
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(ROOT, "tools"))
NAMES = os.path.join(ROOT, "gcl", "names")
KINDS = ("chara", "command")
RANK = {"high": 0, "medium": 1, "low": 2}


def strhash(text):
    h = 0
    for c in text.encode("euc_jp") if isinstance(text, str) else text:
        h = (((h << 5) | (h >> 11)) + c) & 0xFFFF
    return h


def rows(kind):
    with open(os.path.join(NAMES, kind + ".csv"), encoding="utf-8") as f:
        return [r for r in csv.DictReader(f)]


def load(kind):
    """{key: [rows]}, the preferred row first."""
    out = {}
    for r in rows(kind):
        out.setdefault(int(r["key"], 16), []).append(r)
    for v in out.values():
        v.sort(key=lambda r: RANK.get(r["confidence"], 3))
    return out


@functools.lru_cache(None)
def best(kind):
    """{key: preferred row}."""
    return {k: v[0] for k, v in load(kind).items()}


@functools.lru_cache(None)
def prints():
    """{print text as gcx.py shows it: English} from gcl/prints.csv."""
    try:
        with open(os.path.join(ROOT, "gcl", "prints.csv"), encoding="utf-8") as f:
            return {r["text"]: r["english"] for r in csv.DictReader(f)}
    except OSError:
        return {}


def targets(kind, rom):
    """{key: what it maps to} for every key of a kind."""
    if kind == "chara":
        with open(os.path.join(ROOT, "data", "actor_registry.csv")) as f:
            return {int(r["key"], 16): r["name"] for r in csv.DictReader(f)}
    out = {}
    for table, n in ((0x0861418C, 6), (0x08E8791C, 8), (0x080F38C8, 3)):
        for i in range(n):
            k, fn = struct.unpack_from("<II", rom, table - 0x08000000 + 8 * i)
            out[k] = f"{fn & ~1:08X}"
    return out


WORD = re.compile(r"[ぁ-ゖァ-ヺー一-龥々〆ヶA-Za-z0-9_]+")
KANA = re.compile(r"[ァ-ヺー]{2,}|[一-龥々]+[ぁ-ゖァ-ヺー一-龥々]*|[A-Za-z_][A-Za-z0-9_]+")


def words(text):
    """Candidate names in a string: whole words, their parts split at `_`,
    and script-class runs (katakana, kanji with following kana, ASCII)."""
    out = set()
    for w in WORD.findall(text):
        out.add(w)
        out.update(p for p in w.split("_") if p)
        out.update(KANA.findall(w))
    return {w for w in out if len(w) >= 2 or ord(w[0]) > 0x7F}


def corpus(rom):
    """{word: first source}: text bank strings and script print strings."""
    import gcx
    import text
    out = {}
    for n, s in text.strings(rom):
        for w in words(s):
            out.setdefault(w, f"text#{n}")
    img = gcx.Image(rom)
    for pid in range(len(img.procs)):
        for _, t in gcx.disassemble(img, pid):
            for s in re.findall(r"\bprint '((?:[^'\\]|\\.)*)'", t):
                for w in words(s):
                    out.setdefault(w, f"print in proc_{pid + 1}")
    return out


def main():
    cmd, args = (sys.argv[1], sys.argv[2:]) if len(sys.argv) > 1 else ("check", [])
    if cmd == "hash":
        for a in args:
            print(f"{strhash(a):04X}\t{a}")
        return
    if cmd == "check":
        bad = 0
        for kind in KINDS:
            for r in rows(kind):
                if strhash(r["name"]) != int(r["key"], 16):
                    print(f"{kind} {r['key']}: '{r['name']}' hashes to {strhash(r['name']):04X}")
                    bad += 1
        print("ok" if not bad else f"{bad} bad rows")
        sys.exit(1 if bad else 0)
    from romlib import load_rom
    rom = load_rom()
    words_ = corpus(rom)
    if cmd == "find":
        want = {int(a, 16) for a in args}
        for w, src in sorted(words_.items()):
            if strhash(w) in want:
                print(f"{strhash(w):04X}\t{w}\t{src}")
    elif cmd == "crack":
        kind = args[0] if args else "chara"
        tg, named = targets(kind, rom), load(kind)
        hits = sorted((strhash(w), w, src) for w, src in words_.items() if strhash(w) in tg
                      and ("--all" in args or strhash(w) not in named))
        pool = len(tg) if "--all" in args else len(set(tg) - set(named))
        for k, w, src in hits:
            print(f"{k:04X}\t{w}\t{tg[k]}\t{src}")
        print(f"# {len(words_)} words, {len(hits)} hits on {pool} keys; "
              f"{len(words_) * pool / 65536:.1f} expected by chance", file=sys.stderr)
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main()
