#!/usr/bin/env python3
"""What can break (or light, or put out) a thing: element knowledge read from the source.

    python3 tools/autopilot/breakers.py [OUT]     (default build/autopilot/breakers.json)

An attack's collider carries reaction flags (ContactCollider.reactionFlags, +0x38); a
target's hit callback tests them. This collects, from the decomp's own C and the ROM's
weapon table, which flags each source of attack carries:

* targets: LightFixture_OnHit (src/objects/light_fixtures.c): the flag each slot state needs
  (state 1 -> 4: a candlestick is lit; state 2 -> 8: put out) and the state it moves to.
* lens: Player_GetAttackAttributeFlags (src/player/attack/Player_GetAttackAttributeFlags.c): the gun's flags per lens
  mode; the lens id in the bag (GameState+0x828) is the mode, its name is string 970 + id.
* weapon modifiers: Player_BuildActionModifiers (src/player/appearance/player_visuals.c): modifier kind -> the
  Player_OnWeaponHitApply* callback -> the flags it returns.
* weapons: the ROM's weapon records (gUnk_08E6092C, 32 bytes each, modifier kinds at
  +12 and +16); a sword's name is string 817 + id.

The Autopilot asks `means(flag)`: which lens or weapon carries it, and whether the bags
hold it (tools/autopilot/autopilot.mjs, breakable hittables).
"""
import json
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


sys.path.insert(0, os.path.join(ROOT, "tools"))
from source_layout import resolve


def src(path):
    with open(os.path.join(ROOT, resolve(path))) as f:
        return f.read()


def body(text, name):
    m = re.search(r"\b" + name + r"\s*\([^;{]*\)\s*\{", text)
    if not m:
        raise SystemExit(f"{name} not found")
    i, depth = m.end(), 1
    while depth:
        depth += {"{": 1, "}": -1}.get(text[i], 0)
        i += 1
    return text[m.end():i - 1]


def targets():
    b = body(src("src/objects/light_fixtures.c"), "LightFixture_OnHit")
    out = {}
    for m in re.finditer(r"state == (\d+)\)\s*\{\s*if \(SceneCollider_ReadReactionFlags\(other, (\w+)\)\)\s*"
                         r"LightFixture_EnterState\(\w+, (\d+)\)", b):
        out[int(m.group(1))] = {"flag": int(m.group(2), 0), "next": int(m.group(3))}
    return out


def switch_cases(b):
    """case labels -> the value assigned to r (a simple `switch (x) { case N: r = V; break; }`)."""
    out, pending = {}, []
    for line in b.splitlines():
        line = line.strip()
        m = re.match(r"case (\w+):", line)
        if m:
            pending.append(int(m.group(1), 0))
        elif line.startswith("default:"):
            pending.append("default")
        m = re.match(r"r = (\w+);", line)
        if m:
            for k in pending:
                out[k] = int(m.group(1), 0)
            pending = []
    return out


def text(i):
    for line in open(os.path.join(ROOT, "build/text/english.tsv"), encoding="utf8"):
        p = line.rstrip("\n").split("\t")
        if p[0] == str(i):
            return p[2] if len(p) > 2 else ""
    return None


def main():
    out_path = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, "build/autopilot/breakers.json")
    lens = switch_cases(body(src("Player_GetAttackAttributeFlags"), "Player_GetAttackAttributeFlags").split("else", 1)[1])
    p3 = src("src/player/appearance/player_visuals.c")
    returns = {m.group(1): int(m.group(2), 0) for m in re.finditer(
        r"s32 (Player_OnWeaponHitApply\w+)\(struct Player \*p\)\s*\{[^}]*return (\w+);", p3)}
    mods = {}
    for m in re.finditer(r"case (\d+):\s*if \(p\)\s*p->flagCallbacks\[i\] = (Player_OnWeaponHitApply\w+);",
                         body(p3, "Player_BuildActionModifiers")):
        mods[int(m.group(1))] = returns[m.group(2)]
    rom = open(os.path.join(ROOT, "baserom.gba"), "rb").read()
    weapons = []
    for i in range(47):  # gUnk_08E6092C[47] is the unarmed record (Player_BeginDescriptorAction)
        r = rom[0xE6092C + i * 32:0xE6092C + (i + 1) * 32]
        flags = 0
        for k in (r[12], r[16]):
            flags |= mods.get(k, 0)
        if flags:
            weapons.append({"id": i, "type": r[1], "flags": flags, "name": text(817 + i)})
    res = {
        "targets": {"Breakable": targets()},
        "lens": [{"id": k, "flags": v, "name": text(970 + k)} for k, v in sorted(
            ((k, v) for k, v in lens.items() if k != "default"))],
        "weaponModifiers": mods,
        "weapons": weapons,
    }
    os.makedirs(os.path.dirname(out_path), exist_ok=True)
    with open(out_path, "w") as f:
        json.dump(res, f, indent=1)
    print(json.dumps(res)[:600])


if __name__ == "__main__":
    main()
