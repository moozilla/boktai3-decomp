"""Index the input corpus. The optional audit touches every actor and stage triple."""
import csv
import json
import struct

from art import ROOT, actor_pose, encode_tile, fit_tiles, pose_tiles, tilesets
from rooms import convert_resources


def catalog(source, out, check):
    triples = tilesets.script_stages()
    with (ROOT / "data/stage_palettes.csv").open() as f:
        palettes = {r["map"]: r["palette"] for r in csv.DictReader(f)}
    fallback = next(iter(source.archives["t92B3"]))
    actors = []
    errors = []
    records = [("CB05", key, None) for key in sorted(source.archives["tCB05"])]
    records += [("98F5", row[0], i) for i, row in enumerate(source.big.records)]
    for group, key, record in records:
        count, render = source.actor(key, group=group, record=record)
        actor = dict(hash=f"{key:04X}", archive=group, frames=count, record=record)
        actors.append(actor)
        if check:
            for frame in range(count):
                try:
                    converted = pose_tiles(actor_pose(render(frame)))
                    if len(converted) != 192:
                        raise ValueError("24x32 OBJ size")
                except (IndexError, KeyError, ValueError, struct.error) as exc:
                    errors.append(f"{group}:{key:04X} record {record} frame {frame}: {exc}")
                    break
    for stage in triples:
        key = stage["map"]
        stage["captured_palette"] = palettes.get(key)
        if check:
            # A test palette exercises geometry for stages with no captured
            # palette. The runtime exporter requires an evidenced or supplied one.
            try:
                room = convert_resources(source, key, stage["tileset"], stage["hp"],
                                         palettes.get(key, f"{fallback:04X}"))
                if room.background.shape != (512, 512) or room.background.max() > 3:
                    raise ValueError("BG ink or plane size")
                tiles, ids, _, _ = fit_tiles(room.background, 256)
                patterns = b"".join(encode_tile(t) for t in tiles)
                if len(patterns) > 4096 or len(ids) != 4096:
                    raise ValueError("hardware patterns or tilemap size")
            except (IndexError, KeyError, ValueError, struct.error) as exc:
                errors.append(f"{key}-{stage['tileset']}-{stage['hp']} {exc}")
    result = dict(stages=triples, actors=actors, palettes=[f"{k:04X}" for k in sorted(source.archives["t92B3"])],
                  verified_all_frames_and_stages=check, errors=errors)
    (out / "catalog.json").write_text(json.dumps(result, indent=2) + "\n")
    print(f"{len(triples)} scripted stage triples, {len(actors)} actor packages, {sum(a['frames'] for a in actors)} frames, {len(errors)} conversion errors")
    if errors:
        raise ValueError("see build/demake/catalog.json for the resource IDs")
