#!/usr/bin/env python3
"""Build a playable MBC5 cartridge or convert an individual stage or actor."""
import argparse
import json
import struct
import zlib
from pathlib import Path

from PIL import Image

from art import ROOT, Source, actor_pose, encode_tile, fit_tiles, pose_tiles, screen_image
from journal import Journal
from gallery import actor_gallery, room_gallery
from music import compile_song
from pack import LAYOUT, objects, pack_room, enemy_constants, ghoul_palette
from rooms import connected_scenes, convert, convert_resources
from screens import screen_bank
from sm83 import assemble, cartridge

HERE = Path(__file__).resolve().parent
OUT = ROOT / "build/demake"
STATE = """bank next_bank player_x player_z player_h camera_x camera_y health energy tick
direction buttons pressed menu facing mirror walking dx dz used_a enemy_phase enemy_x
enemy_z enemy_timer point_index goal_x goal_z route_wait rng death_flag grace projectile
shot_x shot_z shot_dx shot_dz oam_count pose attr music_lo music_hi music_wait sfx_pulse sfx_noise
world_x world_x_hi world_y world_y_hi camera_x_hi camera_y_hi projected_h projected_h_hi
camera_snap tile_x tile_y cached_x cached_y row_pending col_pending row_lo row_hi col_x mode sun_level sun_source shadow_choice pressed_dir need_meter note_index note_lang note_count save_slot save_generation anim_dir anim_key anim_wait shot_pose_timer hud_energy hud_health dma_count""".split()


def direction_table():
    data = bytearray()
    for keys in range(16):
        x = bool(keys & 1) - bool(keys & 2)
        y = bool(keys & 8) - bool(keys & 4)
        sign = lambda n: (n > 0) - (n < 0)
        facing = (0 if y < 0 else 4) if x == 0 else {1: 3, 0: 2, -1: 1}[y]
        data.extend([sign(x + y) & 255, sign(y - x) & 255, facing, 32 if x < 0 else 0])
    return data


def build(source, out, scenes=None, song=None):
    out.mkdir(parents=True, exist_ok=True)
    scenes = scenes if scenes is not None else connected_scenes(source, 2698)
    if len(scenes) > 128 or len(set(scenes)) != len(scenes):
        raise ValueError("use up to 128 distinct rooms per cartridge")
    rooms = {n: (convert(source, n), i) for i, n in enumerate(scenes, 1)}
    journal = Journal(source, len(rooms) + 3)
    notes = {n: journal.room(room, rooms) for n, (room, _) in rooms.items()}
    saved_offset, resume_offset = journal.system_pages()
    poses, markers, anim = objects(source)
    directions = direction_table()
    symbols = {name: 0xC100 + i for i, name in enumerate(STATE)}
    symbols.update(LAYOUT, **enemy_constants(), marker_bytes=len(markers))
    symbols.update(pickups=0xC200, want_pose=0xC2A0, live_pose=0xC2A4, live_tile=0xC2A8, spare_tile=0xC2AC)
    symbols.update({name: 0xC2B0 + i for i, name in enumerate(
        "upload_left upload_slot upload_pose upload_src_lo upload_src_hi upload_dest_lo upload_dest_hi upload_bank".split())})
    symbols.update(ghoul_pose=anim["ghoul"], skeleton_pose=anim["skeleton"])
    symbols["lunge_period_mask"] = symbols["lunge_period"] - 1
    symbols.update(sk_records=0xC320, sk_base=0xC300, sk_slot=0xC310)
    symbols.update({name: 0xC300 + i for i, name in enumerate(
        "sk_x sk_z sk_phase sk_timer sk_point sk_dx sk_dz sk_speed sk_frac sk_facing".split())})
    song_ids = rooms[scenes[0]][0].stage["songs"]
    bgm = song if song is not None else song_ids[0] if song_ids else 18
    score, wave, music_info = compile_song(bgm)
    symbols.update(screen_bank=len(rooms) + 2, music_bank=len(rooms) + 1, music_loop=0x4000 + music_info["loop_offset"])
    asm = (HERE / "engine.asm").read_text() + (HERE / "audio.asm").read_text() + (HERE / "scroll.asm").read_text() + (HERE / "poses.asm").read_text() + (HERE / "skeleton.asm").read_text() + (HERE / "screens.asm").read_text() + (HERE / "printer.asm").read_text() + (HERE / "journal.asm").read_text()
    symbols.update({name: 0xc700 + i for i, name in enumerate("""
        pr_state pr_command pr_compress pr_length_lo pr_length_hi pr_checksum_lo pr_checksum_hi
        pr_raw_lo pr_raw_hi pr_ink_lo pr_ink_hi pr_run pr_repeat pr_bad pr_status
        pr_size_lo pr_size_hi pr_total_lo pr_total_hi pr_packet_lo pr_packet_hi pr_busy
        photo_ready photo_level pr_byte pr_received_lo pr_palette""".split())})
    save_fields = "bank player_x player_z health energy sun_level sun_source shadow_choice note_lang".split()
    signature = zlib.crc32(b"DMG-CHECKPOINT-1" + b"".join(struct.pack("<H", n) for n in scenes))
    asm += (HERE / "animation.asm").read_text()
    asm += "\nidle_poses:\n db " + ",".join(map(str, anim["idle"])) + "\n"
    asm += "gun_poses:\n db " + ",".join(str(v) for group in anim["gun"] for v in group) + "\n"
    # A source sequence is (pose, duration) pairs and a 255 loop marker.
    asm += "walk_ptrs:\n dw " + ",".join(f"walk_dir_{i}" for i in range(5)) + "\n"
    for i, seq in enumerate(anim["walk"]):
        asm += f"walk_dir_{i}:\n db " + ",".join(str(v) for pair in seq for v in pair) + ",255\n"
    source_asm = (HERE / "save.asm").read_text()
    asm += source_asm + "\nsave_tag:\n db " + ",".join(str(v) for v in signature.to_bytes(4, "little"))
    asm += "\nsave_fields:\n db " + ",".join(str(symbols[k] & 255) for k in save_fields) + "\n"
    symbols.update(room_count=len(rooms), save_field_count=len(save_fields),
                   save_body_bytes=5+len(save_fields)+len(rooms))
    reloc = {"saved_page": 0, "continue_page": 0, "journal_ptrs": 0, "pose_ptrs": 0, "directions": 0, "wave_pattern": 0, "obj_markers": 0}
    code, _ = assemble(asm, {**symbols, **reloc})
    fixed_end = 0x150 + len(code)
    table_addr = fixed_end + len(directions)
    # Each 24x32 pose lives wholly within one switch bank. Its three-byte
    # directory entry is visible regardless of which room or score is mapped.
    pose_banks, pointers = [], bytearray()
    first_pose_bank = journal.first_bank + len(journal.banks)
    for pose in poses:
        if not pose_banks or len(pose_banks[-1]) + len(pose) > 0x4000:
            pose_banks.append(bytearray())
        data = pose_banks[-1]
        pointers.extend(struct.pack("<BH", first_pose_bank + len(pose_banks)-1, 0x4000 + len(data)))
        data.extend(pose)
    reloc.update(directions=fixed_end, pose_ptrs=table_addr, obj_markers=table_addr+len(pointers),
                 wave_pattern=table_addr+len(pointers)+len(markers))
    reloc["journal_ptrs"] = reloc["wave_pattern"] + len(wave)
    reloc.update(saved_page=reloc["journal_ptrs"]+saved_offset, continue_page=reloc["journal_ptrs"]+resume_offset)
    code, symbols = assemble(asm, {**symbols, **reloc})
    info, banks = [], []
    for room, _ in rooms.values():
        data, meta, display = pack_room(source, room, rooms, *notes[room.stage["scene"]])
        info.append(meta)
        banks.append(data)
        screen_image(display).save(out / f"room-{meta['scene']}.png")
    rom = cartridge(code + directions + pointers + markers + wave + journal.directory, [*banks, score, screen_bank(source), *journal.banks, *pose_banks])
    path = out / "boktai.gb"
    path.write_bytes(rom)
    manifest = dict(rooms=info, rom_bytes=len(rom), code_bytes=len(code), obj_pose_count=len(poses), obj_tiles_per_pose=12, obj_marker_tiles=len(markers) // 16,
                    actor_animation=anim, actor_banks=list(range(first_pose_bank, first_pose_bank+len(pose_banks))),
                    journal_pages=len(journal.directory) // 3, music=music_info, state={name: symbols[name] for name in [*STATE, "pickups"]}, engine=symbols)
    (out / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    print(f"{path.relative_to(ROOT)}  {len(rom)//1024} KiB MBC5  {len(banks)} rooms  {len(code)} bytes SM83")
    return rooms, manifest


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    cmds = ap.add_subparsers(dest="cmd")
    rom = cmds.add_parser("rom", help="build the cartridge without running the replays")
    group = rom.add_mutually_exclusive_group()
    group.add_argument("--scenes", type=int, nargs="+", help="pack these decoded scene IDs in order")
    group.add_argument("--area-start", type=int, help="follow nearby door links from this scene")
    rom.add_argument("--song", type=int, help="override the starting room's MP2K song")
    room = cmds.add_parser("room", help="convert a scene by script call ID")
    room.add_argument("scene", type=int)
    room.add_argument("--palette", help="BGR555 resource hash, overriding script or capture")
    stage = cmds.add_parser("stage", help="convert a decoded MP, PTN and HP triple directly")
    stage.add_argument("resources", nargs=3, type=lambda v: f"{int(v, 16):04X}", metavar="HASH")
    stage.add_argument("--palette", help="BGR555 resource hash if the map has no matched capture")
    catalog = cmds.add_parser("catalog", help="index decoded resources and verify their conversions")
    catalog.add_argument("--check", action="store_true", help="convert every stage and every frame of every actor")
    actor = cmds.add_parser("actor", help="convert a CB05 or 98F5 actor by hexadecimal resource hash")
    actor.add_argument("hash", type=lambda k: int(k, 16))
    actor.add_argument("--record", type=int, help="98F5 record index for duplicate names")
    actor.add_argument("--archive", choices=("CB05", "98F5"), help="choose an archive for a shared hash")
    actor.add_argument("--frames", default="0", help="comma-separated frame indices, or all")
    actor.add_argument("--palette", type=int, help="first OBJ pool bank, decimal")
    actor.add_argument("--size", type=int, nargs=2, default=(24, 32), metavar=("WIDTH", "HEIGHT"))
    actor.add_argument("--scale", type=float, default=.85, help="maximum conversion scale")
    actor.add_argument("--obp", type=int, choices=(0, 1), help="hardware OBJ palette")
    music = cmds.add_parser("music", help="reduce an MP2K song to four DMG channels")
    music.add_argument("song", type=int)
    music.add_argument("--frames", type=int, default=14400, help="maximum source video frames")
    args = ap.parse_args()
    OUT.mkdir(parents=True, exist_ok=True)
    source = Source()
    if args.cmd == "catalog":
        from catalog import catalog
        catalog(source, OUT, args.check)
    elif args.cmd == "music":
        if not 1 <= args.frames <= 36000:
            ap.error("--frames must be between 1 and 36000")
        data, wave, meta = compile_song(args.song, args.frames)
        dest = OUT / f"song-{args.song}"
        dest.mkdir(exist_ok=True)
        (dest / "score.bin").write_bytes(data)
        (dest / "wave.bin").write_bytes(wave)
        (dest / "score.json").write_text(json.dumps(meta, indent=2) + "\n")
        print(dest.relative_to(ROOT))
    elif args.cmd in ("room", "stage"):
        if args.cmd == "room":
            r = convert(source, args.scene, args.palette)
            dest = OUT / f"scene-{args.scene}"
        else:
            r = convert_resources(source, *args.resources, args.palette)
            dest = OUT / ("stage-" + "-".join(args.resources))
        dest.mkdir(exist_ok=True)
        tiles, ids, display, _ = fit_tiles(r.background, 256)
        (dest / "bg.2bpp").write_bytes(b"".join(encode_tile(t) for t in tiles))
        (dest / "bg.map").write_bytes((ids ^ 128).tobytes())
        (dest / "stage.json").write_text(json.dumps(dict(stage=r.stage, terrain=r.terrain,
            scale=r.scale, crop=[r.left, r.top], palette=r.palette), indent=2) + "\n")
        room_gallery([r], dest / "contact.png", [256])
        print(dest.relative_to(ROOT))
    elif args.cmd == "actor":
        count, render = source.actor(args.hash, args.palette, args.archive, args.record)
        seq = list(range(count)) if args.frames == "all" else [int(i) for i in args.frames.split(",")]
        if not seq or any(i < 0 or i >= count for i in seq):
            ap.error(f"select frames in 0..{count - 1}")
        dest = OUT / f"actor-{args.hash:04X}"
        dest.mkdir(exist_ok=True)
        w, h = args.size
        if w <= 0 or h <= 0 or w % 8 or h % 16 or w > 80 or w*h > 40*128 or not 0 < args.scale <= 4:
            ap.error("use 8x16 cells, at most 40 total and ten per row, and a scale in (0, 4]")
        large = args.record is not None or args.archive == "98F5" or (args.archive is None and args.hash not in source.archives["tCB05"])
        enemy = large if args.obp is None else bool(args.obp)
        (dest / "poses.2bpp").write_bytes(b"".join(pose_tiles(actor_pose(render(i), size=(w, h), scale=args.scale)) for i in seq))
        cells = [(x, y) for y in range(0, h, 16) for x in range(0, w, 8)]
        (dest / "poses.json").write_text(json.dumps(dict(frames=seq, width=w, height=h, bytes_per_pose=w*h//4,
            oam=[[x, y, i * 2] for i, (x, y) in enumerate(cells)], obp=int(enemy)), indent=2) + "\n")
        for page in range(0, len(seq), 16):
            path = dest / ("contact.png" if page == 0 else f"contact-{page//16+1}.png")
            actor_gallery(source, args.hash, seq[page:page+16], path, args.palette, (w, h), args.scale, enemy=enemy, group=args.archive, record=args.record)
        print(dest.relative_to(ROOT))
    else:
        scenes = None
        dest = OUT
        if args.cmd == "rom" and (args.scenes or args.area_start or args.song is not None):
            scenes = args.scenes or connected_scenes(source, args.area_start or 2698)
            dest = OUT / "custom"
        rooms, meta = build(source, dest, scenes, getattr(args, "song", None))
        if args.cmd is None:
            room_gallery([rooms[n][0] for n in (2698, 2705, 2712)], OUT / "rooms.png", [r["bg_tiles"] for r in meta["rooms"][:3]])
            floor = Image.open(OUT / "room-2698.png")
            actor_gallery(source, 0xDE23, [0, 2, 4, 23], OUT / "django.png", floor=floor)
            actor_gallery(source, 0x9247, [0, 2, 5, 7], OUT / "ghoul.png", ghoul_palette(source), floor=floor)
            actor_gallery(source, 0xAE52, [0, 3, 5, 7], OUT / "skeleton.png", floor=Image.open(OUT / "room-2712.png"))
            from verify import verify
            verify(OUT, meta)

if __name__ == "__main__":
    main()
