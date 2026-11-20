#!/usr/bin/env python3
"""Build a playable MBC5 cartridge or convert an individual stage or actor."""
import argparse
import json
from pathlib import Path

from PIL import Image

from art import ROOT, Source, actor_pose, encode_tile, fit_tiles, pose_tiles, screen_image
from gallery import actor_gallery, room_gallery
from music import compile_song
from pack import SCENES, LAYOUT, objects, pack_room, enemy_constants
from rooms import convert, place_skylight, terrain_overlay
from screens import screen_bank
from sm83 import assemble, cartridge

HERE = Path(__file__).resolve().parent
OUT = ROOT / "build/demake"
STATE = """bank next_bank player_x player_z player_h camera_x camera_y health energy tick
direction buttons pressed menu facing mirror walking dx dz used_a enemy_phase enemy_x
enemy_z enemy_timer point_index goal_x goal_z route_wait rng death_flag grace projectile
shot_x shot_z shot_dx shot_dz oam_count pose attr music_lo music_hi music_wait sfx_pulse sfx_noise
world_x world_x_hi world_y world_y_hi camera_x_hi camera_y_hi projected_h projected_h_hi
camera_snap tile_x tile_y cached_x cached_y row_pending col_pending row_lo row_hi col_x mode sun_level sun_source shadow_choice pressed_dir need_meter""".split()


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
    poses, markers = objects(source)
    obj = b"".join(poses)
    directions = direction_table()
    symbols = {name: 0xC100 + i for i, name in enumerate(STATE)}
    symbols.update(LAYOUT, **enemy_constants(), marker_bytes=len(markers))
    symbols.update(pickups=0xC200, want_pose=0xC2A0, live_pose=0xC2A4, live_tile=0xC2A8, spare_tile=0xC2AC)
    symbols.update({name: 0xC2B0 + i for i, name in enumerate(
        "upload_left upload_slot upload_pose upload_src_lo upload_src_hi upload_dest_lo upload_dest_hi".split())})
    symbols["lunge_period_mask"] = symbols["lunge_period"] - 1
    symbols.update(sk_records=0xC320, sk_base=0xC300, sk_slot=0xC310)
    symbols.update({name: 0xC300 + i for i, name in enumerate(
        "sk_x sk_z sk_phase sk_timer sk_point sk_dx sk_dz sk_speed sk_frac sk_facing".split())})
    bgm = next(int(t.split()[1], 0) for _, t in source.walker.text(2712) if t.startswith("proc_1048 "))
    score, wave, music_info = compile_song(bgm)
    symbols.update(screen_bank=len(rooms) + 2, music_bank=len(rooms) + 1, music_loop=0x4000 + music_info["loop_offset"])
    asm = (HERE / "engine.asm").read_text() + (HERE / "audio.asm").read_text() + (HERE / "scroll.asm").read_text() + (HERE / "poses.asm").read_text() + (HERE / "skeleton.asm").read_text() + (HERE / "screens.asm").read_text() + (HERE / "printer.asm").read_text()
    symbols.update({name: 0xc700 + i for i, name in enumerate("""
        pr_state pr_command pr_compress pr_length_lo pr_length_hi pr_checksum_lo pr_checksum_hi
        pr_raw_lo pr_raw_hi pr_ink_lo pr_ink_hi pr_run pr_repeat pr_bad pr_status
        pr_size_lo pr_size_hi pr_total_lo pr_total_hi pr_packet_lo pr_packet_hi pr_busy
        photo_ready photo_level pr_byte pr_received_lo pr_palette""".split())})
    reloc = {"pose_ptrs": 0, "directions": 0, "wave_pattern": 0, "obj_markers": 0}
    code, _ = assemble(asm, {**symbols, **reloc})
    fixed_end = 0x150 + len(code)
    table_addr = fixed_end + len(directions)
    first_pose = table_addr + 2 * len(poses)
    import struct
    ptrs = b"".join(struct.pack("<H", first_pose + 192 * i) for i in range(len(poses)))
    reloc.update(directions=fixed_end, pose_ptrs=table_addr, obj_markers=first_pose + len(obj),
                 wave_pattern=first_pose + len(obj) + len(markers))
    code, symbols = assemble(asm, {**symbols, **reloc})
    info, banks = [], []
    for room, _ in rooms.values():
        data, meta, display = pack_room(source, room, rooms)
        info.append(meta)
        banks.append(data)
        screen_image(display).save(out / f"room-{meta['scene']}.png")
    rom = cartridge(code + directions + ptrs + obj + markers + wave, [*banks, score, screen_bank(source)])
    path = out / "boktai.gb"
    path.write_bytes(rom)
    manifest = dict(rooms=info, rom_bytes=len(rom), code_bytes=len(code), obj_pose_count=len(poses), obj_tiles_per_pose=12, obj_marker_tiles=len(markers) // 16,
                    music=music_info, state={name: symbols[name] for name in [*STATE, "pickups"]}, engine=symbols)
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
    music = cmds.add_parser("music", help="reduce an MP2K song to four DMG channels")
    music.add_argument("song", type=int)
    music.add_argument("--frames", type=int, default=14400, help="maximum source video frames")
    args = ap.parse_args()
    OUT.mkdir(parents=True, exist_ok=True)
    source = Source()
    if args.cmd == "music":
        if not 1 <= args.frames <= 36000:
            ap.error("--frames must be between 1 and 36000")
        data, wave, meta = compile_song(args.song, args.frames)
        dest = OUT / f"song-{args.song}"
        dest.mkdir(exist_ok=True)
        (dest / "score.bin").write_bytes(data)
        (dest / "wave.bin").write_bytes(wave)
        (dest / "score.json").write_text(json.dumps(meta, indent=2) + "\n")
        print(dest.relative_to(ROOT))
    elif args.cmd == "room":
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
            from verify import verify
            verify(OUT, meta)

if __name__ == "__main__":
    main()
