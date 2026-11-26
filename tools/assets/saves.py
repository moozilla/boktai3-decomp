#!/usr/bin/env python3
"""Save files: read them, read savestates the same way, and write saves.

  saves.py info SAVE|STATE...        story progress, scene, entrance, level...
  saves.py bank [DIR]                list checkpoints in the savestate bank
  saves.py bank --make [OUT]         a battery save per checkpoint (default build/saves/NNNN.sav),
                                     for BOKTAI3_SAV=build/saves/0044.sav tools/emu/harness ...
                                     --bank DIR selects the input for either command
  saves.py make STATE OUT.sav        a battery save holding a savestate's game
  saves.py set SAVE OUT.sav story=N [scene=N entrance=N]

A save is 8 KB of EEPROM (docs/ENGINE.md, section 14). mGBA and VBA store it
as the EEPROM's 64-bit blocks, each byte-reversed; reversed back, block i is
bytes i*8..i*8+7 of what the game wrote. `sub_0822EF44` reads it into RAM
in three checksummed regions per copy, and there are two copies:

| Region | Copy A block | Copy B block | Bytes | RAM |
|---|---|---|---|---|
| script variables (`r708`) | 0x11 | 0x92 | 0x400 | `0203DA00` (`gUnk_02000708`) |
| game state | 0x113 | 0x254 | 0x778 | `0203C400` (`gUnk_02000710`, `struct GameState`) |
| game state, continued | 0x395 | 0x3B6 | 0xE0 | `0203CB78` |

Block 0 holds a 0x28-byte header read at boot. Each region is followed by a
footer block: XOR of its halfwords, their sum, its length, 0xFFFF
(`sub_0822EE68`, the writer). A VBA-M savestate (`.sgm`, gzip) holds all of
RAM; EWRAM starts at byte 34,271 (after a 479-byte header, IWRAM and palette
RAM), so a savestate gives the same three regions as they are in RAM.

Loading checks the game state's first word against `*0x0200060C`, the script
image's build timestamp (`0x42A05217`, the gcx header's), and saving writes it
there (`sub_0822BBD8`, `sub_0822BC6C`); a game that has not saved yet has 0,
so write_sav stamps it. It also sets bit 1 of `r708[0x6E]`: without it a
game continued at a door entrance (most rooms' entrances 7 and 8 sit on the
door's box) walks straight back through the door; saves the game wrote have
it set, savestates taken mid-room often do not (found by splicing the test
save's variables into a checkpoint's until the door stopped firing).

Fields read here: story progress `r708[0x3C]` (tools/assets/story.py), the
entrance `r708[0x70]`, and the scene `GameState+0x5A4`.
"""
import argparse
import gzip
import json
import os
import struct
import sys
from pathlib import Path

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))


def find_bank(root):
    """Find the translation checkout beside this checkout or an enclosing worktree directory."""
    parents = Path(root).resolve().parents
    for parent in parents:
        candidate = parent / "boktai3trans" / "saves"
        if candidate.is_dir():
            return str(candidate)
    return str(parents[0] / "boktai3trans" / "saves")


BANK = find_bank(ROOT)
TEMPLATE = os.path.join(ROOT, "tests", "saves", "ShinBok2.sav")

EWRAM_IN_SGM = 34271
REGIONS = {"vars": (0x400, 0x3DA00), "game": (0x778, 0x3C400), "game2": (0xE0, 0x3CB78)}
COPIES = {"A": {"vars": 0x11, "game": 0x113, "game2": 0x395}, "B": {"vars": 0x92, "game": 0x254, "game2": 0x3B6}}
HEADER = (0, 0x28)
STAMP = 0x42A05217  # *0x0200060C: the script image's timestamp (gcx.py prints it)


def unblock(raw):
    return b"".join(raw[i:i + 8][::-1] for i in range(0, len(raw), 8))


reblock = unblock  # the same reversal both ways


def footer(body):
    x = s = 0
    for (h,) in struct.iter_unpack("<H", body):
        x ^= h
        s = (s + h) & 0xFFFF
    return struct.pack("<4H", x, s, len(body), 0xFFFF)


def region_at(d, block, n):
    """(bytes, checksum ok) of the region of n bytes at an EEPROM block."""
    st = block * 8
    body = d[st:st + n]
    end = st + (n + 7) // 8 * 8
    return body, d[end:end + 8] == footer(body)


def read_sav(path):
    d = unblock(open(path, "rb").read())
    out = {"kind": "save", "header": d[:HEADER[1]], "copies": {}}
    for name, blocks in COPIES.items():
        regs, ok = {}, True
        for r, b in blocks.items():
            regs[r], good = region_at(d, b, REGIONS[r][0])
            ok &= good
        out["copies"][name] = dict(regs, ok=ok)
    return out


def read_sgm(path):
    d = gzip.open(path).read()
    ew = d[EWRAM_IN_SGM:EWRAM_IN_SGM + 0x40000]
    if struct.unpack_from("<I", ew, 0x710)[0] != 0x0203C400:
        raise ValueError(f"{path}: EWRAM not where expected (GameState pointer {struct.unpack_from('<I', ew, 0x710)[0]:08X})")
    regs = {r: ew[a:a + n] for r, (n, a) in REGIONS.items()}
    return {"kind": "state", "copies": {"RAM": dict(regs, ok=True)}, "ewram": ew}


def load(path):
    return read_sgm(path) if path.endswith(".sgm") else read_sav(path)


def fields(c):
    v, g = c["vars"], c["game"]
    return {"story": struct.unpack_from("<h", v, 0x3C)[0], "entrance": struct.unpack_from("<h", v, 0x70)[0],
            "scene": struct.unpack_from("<I", g, 0x5A4)[0], "life": struct.unpack_from("<h", g, 0x28)[0],
            "life_max": struct.unpack_from("<h", g, 0x2A)[0]}


def current(s):
    """The copy to use: RAM for a savestate; for a save, the checksummed copy
    (both when both are good: A first)."""
    for c in s["copies"].values():
        if c["ok"]:
            return c
    return None


def write_sav(regs, out, header=None, template=TEMPLATE):
    """A save with both copies set to regs (vars, game, game2)."""
    d = bytearray(unblock(open(template, "rb").read()))
    if header is not None:
        d[:len(header)] = header
    v = bytearray(regs["vars"])
    v[0x6E] |= 2
    regs = dict(regs, game=struct.pack("<I", STAMP) + regs["game"][4:], vars=bytes(v))
    for blocks in COPIES.values():
        for r, b in blocks.items():
            body = regs[r]
            st = b * 8
            d[st:st + len(body)] = body
            end = st + (len(body) + 7) // 8 * 8
            d[end:end + 8] = footer(body)
    open(out, "wb").write(reblock(bytes(d)))


def bank(directory=BANK):
    rows = []
    for f in sorted(os.listdir(directory)):
        if f.endswith(".sgm"):
            try:
                c = current(read_sgm(os.path.join(directory, f)))
                rows.append(dict(fields(c), file=f, label=os.path.splitext(f)[0].split(" ", 1)[1]))
            except (OSError, ValueError) as e:
                rows.append({"file": f, "error": str(e)})
    return rows


def run_bank(args):
    parser = argparse.ArgumentParser(prog="saves.py bank")
    parser.add_argument("--bank", help="savestate input directory")
    parser.add_argument("--make", action="store_true", help="write one battery save per checkpoint")
    parser.add_argument("path", nargs="?", help="input directory for listing, output directory with --make")
    options = parser.parse_args(args)
    if options.bank and options.path and not options.make:
        parser.error("give the input directory either positionally or with --bank")
    directory = os.path.expanduser(options.bank or (options.path if not options.make else None) or BANK)
    if not os.path.isdir(directory):
        parser.error(f"save bank not found at {directory}. Select it with --bank DIR.")
    if options.make:
        out = os.path.expanduser(options.path or os.path.join(ROOT, "build", "saves"))
        os.makedirs(out, exist_ok=True)
        states = sorted(f for f in os.listdir(directory) if f.endswith(".sgm"))
        for name in states:
            save = current(read_sgm(os.path.join(directory, name)))
            write_sav(save, os.path.join(out, name.split(" ", 1)[0] + ".sav"))
        print(f"{len(states)} saves -> {os.path.relpath(out, ROOT)}")
        return
    for row in bank(directory):
        if "error" in row:
            print(f"{row['file']}: {row['error']}")
        else:
            print(f"story {row['story']:4d}  scene {row['scene']:5d}  entrance {row['entrance']:4d}  {row['label']}")


def main():
    a = sys.argv[1:]
    if not a:
        sys.exit(__doc__)
    cmd = a[0]
    if cmd == "info":
        for p in a[1:]:
            s = load(p)
            for name, c in s["copies"].items():
                print(f"{os.path.basename(p)} {name}: {'ok' if c['ok'] else 'bad checksum'} {json.dumps(fields(c))}")
    elif cmd == "bank":
        run_bank(a[1:])
    elif cmd == "make":
        c = current(load(a[1]))
        write_sav(c, a[2])
        print(f"{a[2]}: {json.dumps(fields(c))}")
    elif cmd == "set":
        c = dict(current(load(a[1])))
        v, g = bytearray(c["vars"]), bytearray(c["game"])
        for kv in a[3:]:
            k, val = kv.split("=")
            val = int(val, 0)
            if k == "story":
                struct.pack_into("<h", v, 0x3C, val)
            elif k == "entrance":
                struct.pack_into("<h", v, 0x70, val)
            elif k == "scene":
                struct.pack_into("<I", g, 0x5A4, val)
            else:
                sys.exit(f"unknown field {k}")
        c["vars"], c["game"] = bytes(v), bytes(g)
        write_sav(c, a[2])
        print(f"{a[2]}: {json.dumps(fields(c))}")
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main()
