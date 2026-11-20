#!/usr/bin/env python3
"""Build a playable MBC5 cartridge or convert an individual stage or actor."""
import argparse
import json
from pathlib import Path

from PIL import Image

from art import ROOT, Source, actor_pose, encode_tile, fit_tiles, pose_tiles, screen_image
from gallery import actor_gallery, room_gallery
from pack import SCENES, LAYOUT, objects, pack_room, enemy_constants
from rooms import convert, place_skylight, terrain_overlay
from sm83 import assemble, cartridge

HERE = Path(__file__).resolve().parent
OUT = ROOT / "build/demake"
STATE = """bank next_bank player_x player_z player_h camera_x camera_y health energy tick
direction buttons pressed menu facing mirror walking dx dz used_a enemy_phase enemy_x
enemy_z enemy_timer point_index goal_x goal_z route_wait rng death_flag grace projectile
shot_x shot_z shot_dx shot_dz oam_count pose attr pickups""".split()


def direction_table():
    data = bytearray()
    for keys in range(16):
        x = bool(keys & 1) - bool(keys & 2)
        y = bool(keys & 8) - bool(keys & 4)
        sign = lambda n: (n > 0) - (n < 0)
        facing = (0 if y < 0 else 4) if x == 0 else {1: 3, 0: 2, -1: 1}[y]
        data.extend([sign(x + y) & 255, sign(y - x) & 255, facing, 32 if x < 0 else 0])
    return data


def build(source, out):
    out.mkdir(parents=True, exist_ok=True)
    rooms = {n: (convert(source, n), i) for i, n in enumerate(SCENES, 1)}
    for room, _ in rooms.values():
        place_skylight(source, room)
    obj = objects(source)
    directions = direction_table()
    symbols = {name: 0xC100 + i for i, name in enumerate(STATE)}
    symbols.update(LAYOUT, **enemy_constants(), obj_bytes=len(obj))
    asm = (HERE / "engine.asm").read_text()
    code, _ = assemble(asm, {**symbols, "obj_patterns": 0, "directions": 0})
    fixed_end = 0x150 + len(code)
    code, symbols = assemble(asm, {**symbols, "directions": fixed_end, "obj_patterns": fixed_end + len(directions)})
    info, banks = [], []
    for room, _ in rooms.values():
        data, meta, display = pack_room(source, room, rooms)
        info.append(meta)
        banks.append(data)
        screen_image(display).save(out / f"room-{meta['scene']}.png")
    rom = cartridge(code + directions + obj, banks)
    path = out / "boktai.gb"
    path.write_bytes(rom)
    manifest = dict(rooms=info, rom_bytes=len(rom), code_bytes=len(code), obj_tiles=len(obj) // 16,
                    state={name: symbols[name] for name in STATE}, engine=symbols)
    (out / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    print(f"{path.relative_to(ROOT)}  {len(rom)//1024} KiB MBC5  {len(banks)} rooms  {len(code)} bytes SM83")
    return rooms, manifest


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    cmds = ap.add_subparsers(dest="cmd")
    cmds.add_parser("rom", help="build just the cartridge and its manifest")
    room = cmds.add_parser("room", help="convert a scene by script call ID")
    room.add_argument("scene", type=int)
    room.add_argument("--palette", help="BGR555 resource hash, overriding script or capture")
    actor = cmds.add_parser("actor", help="convert a CB05 or 98F5 actor by hexadecimal resource hash")
    actor.add_argument("hash", type=lambda k: int(k, 16))
    actor.add_argument("--frames", default="0", help="comma-separated frame indices, or all")
    actor.add_argument("--palette", type=int, help="first OBJ pool bank, decimal")
    args = ap.parse_args()
    OUT.mkdir(parents=True, exist_ok=True)
    source = Source()
    if args.cmd == "room":
        r = convert(source, args.scene, args.palette)
        dest = OUT / f"scene-{args.scene}"
        dest.mkdir(exist_ok=True)
        tiles, ids, display, _ = fit_tiles(r.background, 256)
        (dest / "bg.2bpp").write_bytes(b"".join(encode_tile(t) for t in tiles))
        (dest / "bg.map").write_bytes((ids ^ 128).tobytes())
        (dest / "stage.json").write_text(json.dumps(dict(stage=r.stage, terrain=r.terrain,
            scale=r.scale, crop=[r.left, r.top], palette=r.palette), indent=2) + "\n")
        room_gallery([r], dest / "contact.png", 256)
        print(dest.relative_to(ROOT))
    elif args.cmd == "actor":
        count, render = source.actor(args.hash, args.palette)
        seq = list(range(count)) if args.frames == "all" else [int(i) for i in args.frames.split(",")]
        if not seq or any(i < 0 or i >= count for i in seq):
            ap.error(f"select frames in 0..{count - 1}")
        dest = OUT / f"actor-{args.hash:04X}"
        dest.mkdir(exist_ok=True)
        (dest / "poses.2bpp").write_bytes(b"".join(pose_tiles(actor_pose(render(i))) for i in seq))
        (dest / "poses.json").write_text(json.dumps(dict(frames=seq, width=16, height=32, bytes_per_pose=128,
            oam=[[0, 0, 0], [8, 0, 2], [0, 16, 4], [8, 16, 6]], obp=1 if args.hash in source.big_ids else 0)))
        actor_gallery(source, args.hash, seq[:16], dest / "contact.png", args.palette)
        print(dest.relative_to(ROOT))
    else:
        rooms, meta = build(source, OUT)
        if args.cmd is None:
            room_gallery([r for r, _ in rooms.values()], OUT / "rooms.png")
            actor_gallery(source, 0xDE23, [0, 2, 4, 23], OUT / "django.png")
            actor_gallery(source, 0x9247, [0, 2, 5, 7], OUT / "ghoul.png")

if __name__ == "__main__":
    main()
