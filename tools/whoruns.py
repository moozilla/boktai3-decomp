#!/usr/bin/env python3
"""Find checkpoint replays that executed a function or source unit.

Examples:
  python3 tools/whoruns.py BossSlot2_UpdateFrame
  python3 tools/whoruns.py src/bosses/boss_slot_2 --functions
  python3 tools/whoruns.py cannon_battery growth_hazard
  python3 tools/whoruns.py 08179B9C-08180488

Index a marked harness replay with --record, or use --scan-bank DIR with the
translation's saves directory to build an idle checkpoint index and screenshots.
An entry indicates an executed function entry point in a specific replay window.
"""

import argparse
import json
import os
from pathlib import Path
import re
import struct
import subprocess
from tempfile import TemporaryDirectory

from assets import saves
from romlib import ROOT
from source_layout import address_of, manifest, scan, symbol_names
from split import inventory_starts

ROOT = Path(ROOT)
ROM_BASE = 0x08000000
ROM_END = 0x09000000
EXEC_SIZE = (ROM_END - ROM_BASE) // 2
RANGE = re.compile(r"(?:0x)?([0-9a-f]{8})[-–](?:0x)?([0-9a-f]{8})", re.I)


def rom_entries():
    return [address for address in inventory_starts() if ROM_BASE <= address < ROM_END]


def resolve(query, entries):
    if span := RANGE.fullmatch(query):
        first, end = (int(value, 16) for value in span.groups())
        if first >= end:
            raise ValueError(f"invalid ROM range {query}")
        return {a for a in entries if first <= a < end}

    by_name = {name: a for a, name in symbol_names().items()}
    address = address_of(query, by_name)
    if address is not None:
        return {address} if address in entries else set()

    path = Path(query)
    if path.is_relative_to(ROOT):
        path = path.relative_to(ROOT)
    if path.parts and path.parts[0] != "src":
        path = Path("src") / path
    prefix = path.as_posix()

    # A path is unambiguous without a project-wide symbol or suffix lookup.
    if ("/" in query or path.suffix == ".c") and ".." not in path.parts:
        target = ROOT / path
        if target.is_relative_to(ROOT / "src"):
            if target.is_dir():
                addresses = {a for values in scan(ROOT, target).by_file.values() for a in values}
            elif target.is_file() and target.suffix == ".c":
                addresses = set(scan(ROOT, target.parent).by_file.get(prefix, ()))
            else:
                addresses = set()
            if addresses:
                return addresses

    source = manifest()
    address = source.names.get(query)
    if address is not None:
        return {address}

    units = set(source.by_file) | {p.as_posix() for file in source.by_file for p in Path(file).parents}
    if prefix not in units:
        matches = {unit for unit in units if unit.endswith("/" + query)}
        if not matches:
            matches = {unit for unit in units if unit.endswith("/" + query + ".c")}
        if len(matches) > 1:
            raise ValueError(f"ambiguous unit {query}: {', '.join(sorted(matches))}")
        if matches:
            prefix = matches.pop()
    return {a for file, addresses in source.by_file.items()
            if file == prefix or file.startswith(prefix + "/") for a in addresses}


def read_coverage(run, entries):
    segments = {}
    for path in sorted(run.glob("cov-*.exec")):
        bits = path.read_bytes()
        if len(bits) != EXEC_SIZE:
            raise ValueError(f"{path}: expected {EXEC_SIZE} bytes, got {len(bits)}")
        segments[path.stem.removeprefix("cov-")] = [
            f"{a:08X}" for a in entries if bits[(a - ROM_BASE) // 2] & 3
        ]
    if not segments:
        raise ValueError(f"{run}: no cov-*.exec files (use mark in the harness script)")
    return segments


def write_record(run, output, entries, checkpoint, scene, label, segment_scenes=None):
    record = {"save": checkpoint, "scene": scene, "label": label,
              "segments": read_coverage(run, entries)}
    if segment_scenes:
        unknown = segment_scenes.keys() - record["segments"].keys()
        if unknown:
            raise ValueError(f"unknown marked segments: {', '.join(sorted(unknown))}")
        record["segment_scenes"] = segment_scenes
    path = output / "coverage.json"
    temporary = path.with_suffix(".tmp")
    temporary.write_text(json.dumps(record) + "\n")
    temporary.replace(path)


def scan_bank(bank, output, entries):
    from PIL import Image

    prefix = (ROOT / "tests/save_menus_field.txt").read_text().split("\nsave ingame\n", 1)[0]
    output.mkdir(parents=True, exist_ok=True)
    for row in saves.bank(bank):
        if (ROOT / "build/STOP").exists():
            break
        if "error" in row:
            raise ValueError(f"{row['file']}: {row['error']}")
        checkpoint = row["file"].split(" ", 1)[0]
        destination = output / checkpoint
        destination.mkdir(exist_ok=True)
        if (destination / "coverage.json").is_file():
            continue
        with TemporaryDirectory(prefix="capture-", dir=destination) as scratch:
            run = Path(scratch)
            battery = run / "start.sav"
            saves.write_sav(saves.current(saves.read_sgm(str(bank / row["file"]))), battery)
            commands = destination / "commands.txt"
            commands.write_text(f"trace off\n{prefix}\ntrace on\nmark checkpoint\nwait 90\n"
                                "shot checkpoint\nsave checkpoint\ndump game 0203C400:778\n")
            with (destination / "harness.log").open("wb") as log:
                env = os.environ | {"BOKTAI3_SAV": str(battery), "BOKTAI3_RTC": "1128150000"}
                subprocess.run([ROOT / "tools/emu/harness", ROOT / "baserom.gba", commands, run],
                               cwd=ROOT, env=env, stdout=log, stderr=subprocess.STDOUT, check=True)
            scene = struct.unpack_from("<I", (run / "game.bin").read_bytes(), 0x5A4)[0]
            Image.open(run / "checkpoint.ppm").save(destination / "checkpoint.png")
            (run / "checkpoint.ss").replace(destination / "checkpoint.ss")
            write_record(run, destination, entries, checkpoint, scene, row["label"])
        print(f"{checkpoint}\t{scene}\t{row['label']}", flush=True)


def sightings(index, addresses, records):
    wanted = {f"{a:08X}" for a in addresses}
    for path in records:
        row = json.loads(path.read_text())
        for segment, hits in sorted(row["segments"].items()):
            matched = sorted(wanted.intersection(hits))
            if matched:
                scene = row.get("segment_scenes", {}).get(segment, row["scene"])
                yield row["save"], scene, segment, row["label"], path.parent.relative_to(index), matched


def scene_override(value):
    name, separator, scene = value.partition("=")
    if not name or not separator or not scene.isdecimal():
        raise argparse.ArgumentTypeError("expected SEGMENT=SCENE")
    return name, int(scene)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    target = parser.add_mutually_exclusive_group(required=True)
    target.add_argument("units", metavar="unit", nargs="*",
                        help="function names, unit paths, entry addresses, or half-open ROM ranges")
    target.add_argument("--record", type=Path, metavar="RUN", help="index a harness run in place")
    target.add_argument("--scan-bank", type=Path, metavar="DIR", help="capture idle coverage for a VBA checkpoint bank")
    parser.add_argument("--coverage", type=Path, default=ROOT / "build/identify", help="coverage index root")
    parser.add_argument("--save", help="checkpoint ID for --record")
    parser.add_argument("--scene", type=int, help="default scene procedure ID for --record")
    parser.add_argument("--segment-scene", type=scene_override, action="append", default=[],
                        metavar="SEGMENT=SCENE", help="override the scene for one marked segment")
    parser.add_argument("--label", help="replay label for --record")
    parser.add_argument("--functions", action="store_true", help="list matching function entries per segment")
    args = parser.parse_args()
    try:
        entries = rom_entries()
        if args.record:
            if args.save is None or args.scene is None or args.label is None:
                parser.error("--record requires --save, --scene and --label")
            write_record(args.record, args.record, entries, args.save, args.scene, args.label,
                         dict(args.segment_scene))
            return
        if args.scan_bank:
            scan_bank(args.scan_bank, args.coverage / "bank", entries)
            return
        queries = [(unit, resolve(unit, entries)) for unit in args.units]
        for unit, addresses in queries:
            if not addresses:
                parser.error(f"no function entries for {unit}")
        records = sorted(args.coverage.rglob("coverage.json"))
        if not records:
            parser.error(f"no coverage records under {args.coverage}")
        names = symbol_names()
        for index, (unit, addresses) in enumerate(queries):
            if len(queries) > 1:
                if index:
                    print()
                print(unit)
            print("save\tscene\tsegment\thits\tlabel\trun")
            matched_rows = list(sightings(args.coverage, addresses, records))
            for save, scene, segment, label, run, matched in matched_rows:
                print(f"{save}\t{scene}\t{segment}\t{len(matched)}\t{label}\t{run}")
                if args.functions:
                    for address in matched:
                        print(f"\t{address}\t{names.get(int(address, 16), 'sub_' + address)}")
            if not matched_rows:
                print(f"No hits in {len(records)} recorded replays.")
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as error:
        parser.exit(2, f"{parser.prog}: {error}\n")


if __name__ == "__main__":
    main()
