#!/usr/bin/env python3
"""Recovered names for GCL hashes (gcl/names/*.csv), and a tool to find more.

  gclnames.py hash TEXT...          the 16-bit hash of each TEXT (EUC-JP bytes)
  gclnames.py check                 every row's name hashes to its key
  gclnames.py crack KIND [--all]    corpus words whose hash is a KIND key, with
                                    what the key maps to (default: unnamed keys)
  gclnames.py find HASH...          corpus words with that hash (any kind)
  gclnames.py labels [--write]      variable, define and parameter names from
                                    the scripts' debug prints; --write updates
                                    gcl/names/{var,define,param}.csv (English
                                    already in the files is kept)

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


# ---------- names from the scripts' debug prints ----------
# A print operand string that ends in a GCL label (`$f:NAME`, `$$b:NAME`,
# `$:P_NAME`, `d:GROUP:NAME`) names the operand right after it.
VAR = re.compile(r"(\$\$?)([bfw]?):([^\s　・！？!?…,、。\[\]']+)(?:\[([^\]]*)\])?")
DEFINE = re.compile(r"\bd:([^\s　:\[\]']+)(?::([^\s　\[\]']+))?")
PROSE = ("が不正な値になっている", "です")  # sentence tails after a label, not part of it
STORE = re.compile(r"^(gs|r700|r708)\[(0x[0-9a-f]+(?:\.\d+)?)\]:t(\d+)(?:\((\d+),(.*)\))?$")
LETTER = {"f": 4, "b": 2, "w": 1}
TAIL = re.compile(r"[\s　・！？!?…]*$")
VAR_COLS = ["storage", "name", "english", "confidence", "conflict", "evidence"]
DEFINE_COLS = ["define", "value", "english", "confidence", "conflict", "evidence"]
PARAM_COLS = ["proc", "storage", "name", "english", "confidence", "conflict", "evidence"]


def storage(op):
    """(key, index operand or None) for a variable operand, e.g.
    r708[0x145.0]:t4(16,1) -> ('r708:0x145.0:t4(16)', '1'); else None."""
    m = STORE.match(op)
    if not m:
        return None
    reg, off, t, n, i = m.groups()
    return f"{reg}:{off}:t{t}" + (f"({n})" if n else ""), i


def _sigil_ok(sigil, letter, key):
    reg, _, t = key.split(":")[:3]
    return (sigil == "$$") == (reg == "r700") and (not letter or int(t[1:].split("(")[0]) == LETTER[letter])


def _english(en, marker):
    """The part of a print's English that translates the label at marker."""
    if not en:
        return ""
    i = en.rfind(marker)
    if i < 0:
        return ""
    e = en[i + len(marker):].strip()
    e = re.sub(r"\[.*?\]", "", e)
    e = re.sub(r"^P_\s*", "", e)
    return TAIL.sub("", e).rstrip(".!?… ")


def labels(rom):
    """Every (label, operand) pairing in the scripts' prints, as dicts with
    kind (var, define, param), proc (call id), and the row fields."""
    import gcx
    img = gcx.Image(rom)
    en = prints()
    out = []
    for pid in range(len(img.procs)):
        cid = pid + 1
        for args in gcx.print_args(img, pid):
            for s, op in zip(args, args[1:] + [None]):
                if s[:1] != "'":
                    continue
                text = s[1:-1]
                ok = op is not None and op[:1] != "'"
                found = list(VAR.finditer(text))
                if found:
                    m = found[-1]
                    sigil, letter, name, index = m.groups()
                    if not ok or index == "" or TAIL.sub("", text[m.end():]):
                        continue  # no operand, an array description (`NAME[]`), or prose follows
                    tidy = True
                    for t in PROSE:
                        if name.endswith(t):
                            name, tidy = name[:-len(t)], False
                    label = f"{sigil}{letter}:{name}"
                    english = _english(en.get(text, ""), f"{sigil}{letter}:")
                    conf = "high" if tidy else "medium"
                    st = storage(op)
                    if st and letter:
                        key, iop = st
                        good = _sigil_ok(sigil, letter, key)
                        out.append(dict(kind="var", proc=cid, storage=key, name=label, english=english,
                                        confidence=conf if good else "low",
                                        note="" if good else "sigil/type disagree with storage"))
                        if index and iop is not None:
                            dm = re.fullmatch(r"\s*d:([^:]+):(.+?)\s*", index)
                            pm = re.fullmatch(r"\s*\$:(\S+?)\s*", index)
                            am = re.fullmatch(r"\(?(arg\d+|local\d+)\)?", iop)
                            if dm and iop.isdigit():
                                e = re.search(r"\[d:[^:\]]*:([^\]]*)\]", en.get(text, ""))
                                out.append(dict(kind="define", proc=cid, define=f"d:{dm[1]}:{dm[2]}", value=iop,
                                                english=e[1].strip() if e else "", confidence="high", array=key, note=""))
                            elif pm and am:
                                out.append(dict(kind="param", proc=cid, storage=am[1], name=f"$:{pm[1]}",
                                                english="", confidence="high", note="index of " + label))
                    elif not letter and re.fullmatch(r"arg\d+|local\d+", op):
                        out.append(dict(kind="param", proc=cid, storage=op, name=label, english=english,
                                        confidence=conf, note=""))
                    continue
                m = DEFINE.search(text)
                if m and ok and re.fullmatch(r"-?\d+", op) and not TAIL.sub("", text[m.end():]):
                    d = f"d:{m[1]}:{m[2]}" if m[2] else f"d:{m[1]}"
                    out.append(dict(kind="define", proc=cid, define=d, value=op,
                                    english=_english(en.get(text, ""), "d:").split(":")[-1].strip(), confidence="high", array="", note=""))
    return out


def _merge(found, kind, cols, keyf, slot, name, group=lambda r: ""):
    """CSV rows for one kind: one per distinct (slot, name), with the procs as
    evidence, conflicts flagged, and English kept from the existing file."""
    path = os.path.join(NAMES, kind + ".csv")
    old = {}
    if os.path.exists(path):
        with open(path, encoding="utf-8") as f:
            old = {keyf(r): r for r in csv.DictReader(f)}
    rows_ = {}
    for x in found:
        r = rows_.setdefault(keyf(x), dict(x, procs=[], notes=set()))
        r["procs"].append(x["proc"])
        if x["note"]:
            r["notes"].add(x["note"])
        if RANK[x["confidence"]] < RANK[r["confidence"]]:
            r["confidence"] = x["confidence"]
        r["english"] = r["english"] or x["english"]
    by_slot, by_name = {}, {}
    for r in rows_.values():
        by_slot.setdefault((group(r), slot(r)), set()).add(name(r))
        by_name.setdefault((group(r), name(r)), set()).add(slot(r))
    out = []
    for k, r in rows_.items():
        c, s_, n_ = [], by_slot[group(r), slot(r)], by_name[group(r), name(r)]
        if len(s_) > 1:
            c.append("slot also named " + ", ".join(sorted(s_ - {name(r)})))
        if len(n_) > 1:
            c.append("name also on " + ", ".join(sorted(n_ - {slot(r)})))
        procs = sorted(set(r["procs"]))
        ev = "print in " + ", ".join(f"proc_{p}" for p in procs[:6]) + (f" (+{len(procs) - 6})" if len(procs) > 6 else "")
        if r.get("array"):
            ev += f"; index of {r['array']}"
        ev = "; ".join([ev] + sorted(r["notes"]))
        row = dict(r, english=old.get(k, {}).get("english") or r["english"], conflict="; ".join(c), evidence=ev)
        out.append({c_: row.get(c_, "") for c_ in cols})
    return out


def write_labels(rom):
    found = labels(rom)
    sk = lambda s: (s.split(":")[0], int(s.split(":")[1].split(".")[0], 16), s)  # noqa: E731
    specs = [
        ("var", VAR_COLS, lambda r: (r["storage"], r["name"]), lambda r: r["storage"], lambda r: r["name"],
         lambda r: (sk(r["storage"]), r["name"])),
        ("define", DEFINE_COLS, lambda r: (r["define"], r["value"]), lambda r: r["define"], lambda r: r["value"],
         lambda r: (r["define"], int(r["value"])), lambda r: r["define"].rsplit(":", 1)[0]),
        ("param", PARAM_COLS, lambda r: (str(r["proc"]), r["storage"], r["name"]),
         lambda r: r["storage"], lambda r: r["name"],
         lambda r: (int(r["proc"]), r["storage"], r["name"]), lambda r: str(r["proc"])),
    ]
    for kind, cols, keyf, slot, name, order, *group in specs:
        rows_ = sorted(_merge([x for x in found if x["kind"] == kind], kind, cols, keyf, slot, name, *group), key=order)
        with open(os.path.join(NAMES, kind + ".csv"), "w", encoding="utf-8", newline="") as f:
            w = csv.DictWriter(f, cols, lineterminator="\n")
            w.writeheader()
            w.writerows(rows_)
        print(f"{kind}.csv: {len(rows_)} rows")


def _table(kind):
    try:
        with open(os.path.join(NAMES, kind + ".csv"), encoding="utf-8") as f:
            return list(csv.DictReader(f))
    except OSError:
        return []


@functools.lru_cache(None)
def var_names():
    """{storage key: preferred var row} (gcl/names/var.csv)."""
    out = {}
    for r in sorted(_table("var"), key=lambda r: RANK.get(r["confidence"], 3)):
        out.setdefault(r["storage"], r)
    return out


@functools.lru_cache(None)
def param_names():
    """{(proc call id, argN/localN): preferred row} (gcl/names/param.csv)."""
    out = {}
    for r in sorted(_table("param"), key=lambda r: RANK.get(r["confidence"], 3)):
        out.setdefault((int(r["proc"]), r["storage"]), r)
    return out


@functools.lru_cache(None)
def index_defines():
    """{(array storage key, value): [(define, {procs whose prints use it})]}
    from the index defines in gcl/names/define.csv."""
    out = {}
    for r in _table("define"):
        m = re.search(r"index of (\S+)", r["evidence"])
        if m:
            procs = {int(p) for p in re.findall(r"proc_(\d+)", r["evidence"])}
            out.setdefault((m[1], r["value"]), []).append((r["define"], procs))
    return out


def index_define(key, value, cid):
    """The define that names index value of array key in procedure cid: the
    one its prints use, else the only one; None when unknown or ambiguous."""
    ds = index_defines().get((key, value), [])
    here = [d for d, procs in ds if cid in procs]
    return here[0] if len(here) == 1 else ds[0][0] if len(ds) == 1 else None

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
    if cmd == "labels":
        if "--write" in args:
            write_labels(rom)
        else:
            for x in labels(rom):
                print("\t".join(str(x.get(k, "")) for k in ("kind", "proc", "storage", "define", "name", "value", "confidence", "english")))
    elif cmd == "find":
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
