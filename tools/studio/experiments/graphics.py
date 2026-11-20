"""Join a screenshot coordinate to hardware OBJ data, a DMA upload and gameside C."""
from bisect import bisect_left
import csv
from functools import lru_cache
import gzip
from pathlib import Path
import re
import struct
import sys

from . import fields, obj_pixels, recipe

MEMORY = {
    "oam": (0x07000000, 0x400),
    "vram": (0x06010000, 0x8000),
    "pal": (0x05000200, 0x200),
    "io": (0x04000000, 2),
}
PLAYER_ROOT = "gPlayers"
PLAYER_MEMBERS = ("animation", "secondaryAnimation", "auxiliarySprite[0]")
QUEUE_SYMBOLS = ("gOamCount", "gOamBuffer", "gPendingOamCount", "gPendingOamBuffer")
RAW_QUEUES = ("oam-queues.bin", "sprite-sources.tsv", "oam-uploads.tsv")
QUEUE_BYTES = MEMORY["oam"][1]
IW_SOURCE = fields.ROOT / "src/iwram_code.c"


@lru_cache(maxsize=4)
def layout(stamps):
    source = IW_SOURCE.read_text()
    private = re.search(r"struct IwVec \{.+?\n\};\n\nstruct IwSpr;\n\nstruct IwObj \{.+?\n\};", source, re.S)
    if not private:
        raise RuntimeError("The IWRAM sprite declaration changed")
    measures = ["(unsigned)&((struct IwObj *)0)->tiles", f"sizeof({PLAYER_ROOT})"]
    measures.extend(f"(unsigned)&((struct Player *)0)->{member}" for member in PLAYER_MEMBERS)
    text = '#include "global.h"\n#include "player.h"\n' + private[0]
    text += "\nconst unsigned experiment_layout[] = {" + ",\n".join(measures) + "};\n"
    offsets = fields.compile_layout(text, "sprite renderer")
    if len(offsets) != len(measures) or offsets[1] % 4:
        raise RuntimeError("The sprite observer could not measure its target layout")
    symbols = fields.ram_symbols(stamps[-1])
    addresses = [int(symbols[name]["addr"], 16) for name in (*QUEUE_SYMBOLS, PLAYER_ROOT)]
    return [*addresses[:4], offsets[0]], (addresses[4], offsets[1]), dict(zip(PLAYER_MEMBERS, offsets[2:]))


def measured_layout():
    paths = (IW_SOURCE, fields.ROOT / "include/player.h", fields.ROOT / "symbols/ram.csv")
    return layout(tuple(path.stat().st_mtime_ns for path in paths))


def trace_command():
    offsets, _, _ = measured_layout()
    return "drawtrace " + " ".join(f"{value:X}" for value in offsets)


def expand_commands(commands):
    _, players, _ = measured_layout()
    buffers = {**MEMORY, "players": players}
    first = next(index for index, line in enumerate(commands) if line.startswith("sample "))
    yield from commands[:first]
    yield trace_command()
    for line in commands[first:]:
        yield line
        if line.startswith("shot "):
            name = line.split()[1]
            yield from (f"dump {name}-{kind} {address:08X}:{size:X}" for kind, (address, size) in buffers.items())


def tsv(path):
    with path.open() as source:
        return list(csv.DictReader(source, delimiter="\t"))


def store_graphics(directory, snapshots):
    """Retain compact per-sample memory and original native renderer traces."""
    _, players, members = measured_layout()
    buffers = {**MEMORY, "players": players}
    queues = (directory / RAW_QUEUES[0]).read_bytes()
    record_bytes = QUEUE_BYTES + 4
    if len(queues) % record_bytes:
        raise RuntimeError("The native sprite queue trace is incomplete")
    uploads = tsv(directory / RAW_QUEUES[2])
    frames = [int(upload["frame"]) for upload in uploads]
    for snap in snapshots:
        name, frame = snap["label"], snap["frame"]
        content = {}
        for kind, (_, size) in buffers.items():
            path = directory / f"{name}-{kind}.bin"
            data = path.read_bytes()
            if len(data) != size:
                raise RuntimeError(f"The {kind} capture has an unexpected size")
            content[kind] = data
            path.unlink()
        # The screen has just finished at line 160. Its visible scanlines ran
        # before this frame counter increment and the next VBlank upload.
        upload_index = bisect_left(frames, frame) - 1
        evidence = None
        if snap["relative"] and upload_index >= 0:
            upload = uploads[upload_index]
            generation = int(upload["generation"])
            if 0 <= generation < len(queues) // record_bytes:
                offset = generation * record_bytes
                if content["oam"] == queues[offset + 4:offset + record_bytes]:
                    evidence = {"generation": generation, "queue_frame": struct.unpack_from("<I", queues, offset)[0],
                                "upload_frame": int(upload["frame"]), "upload_pc": upload["pc"]}
        (directory / f"{name}.gfx.gz").write_bytes(gzip.compress(b"".join(content.values()), compresslevel=3, mtime=0))
        snap["graphics"] = {"submission": evidence, "player_members": members, "player_bytes": players[1]}
    for filename in RAW_QUEUES:
        path = directory / filename
        path.with_suffix(path.suffix + ".gz").write_bytes(gzip.compress(path.read_bytes(), compresslevel=3, mtime=0))
        path.unlink()


def cpu_function(pc, index):
    address = int(pc, 16)
    if 0x03000000 <= address < 0x0300110C:
        address += 0x08000350 - 0x03000000
    return index.lookup(address)


@lru_cache(maxsize=2)
def asset_ranges(rom_path, stamp):
    assets = str(fields.ROOT / "tools/assets")
    if assets not in sys.path:
        sys.path.insert(0, assets)
    from archive import archives

    data = Path(rom_path).read_bytes()
    return tuple((label, f"{digest:04X}", start, start + size)
                 for label, _, _, entries in archives(data) for digest, start, size in entries)


def asset(tile_source, rom):
    if not 0x08000000 <= tile_source < 0x0A000000:
        return None
    for label, digest, start, end in asset_ranges(rom, rom.stat().st_mtime_ns):
        if start <= tile_source < end:
            return {"archive": label, "hash": digest, "start": f"{start:08X}",
                    "tile_source": f"{tile_source:08X}",
                    "url": f"/gfx#sprites/{digest}" if label == "tCB05" else f"/api/res/{label}/{digest}"}
    return None


def owner(drawable, players, member_offsets, result, snap):
    for slot, (address,) in enumerate(struct.iter_unpack("<I", players)):
        if not (0x02000000 <= address < 0x02040000 or 0x03000000 <= address < 0x03008000):
            continue
        for member, offset in member_offsets.items():
            if address + offset != drawable:
                continue
            root = f"{PLAYER_ROOT}[{slot}]->"
            watched = [{**row, "path": field["path"]} for field, row in zip(result["fields"], snap["fields"])
                       if field["path"].startswith(root)]
            return {"type": "struct Player", "address": f"{address:08X}", "slot": slot,
                    "member": root + member, "header": "include/player.h", "fields": watched}
    return None


def sources_for(rows, slot, attrs):
    matches = [row for row in rows if tuple(int(row[f"attr{i}"], 16) for i in range(3)) == attrs]
    direct = [row for row in matches if row["queue"] == "d" and int(row["index"]) == slot]
    return direct or [row for row in matches if row["queue"] == "p"]


def inspect(directory, result, trial, frame, x, y, rom):
    """Resolve a sampled pixel only against its captured game state and queues."""
    from PIL import Image
    if not isinstance(trial, str) or not recipe.TRIAL_ID.fullmatch(trial):
        raise ValueError("Choose a captured trial")
    x = recipe.bounded_int(x, 0, 239, "Pixel X")
    y = recipe.bounded_int(y, 0, 159, "Pixel Y")
    case = next((item for item in result["trials"] if item["id"] == trial), None)
    snap = next((item for item in case["snapshots"] if item["relative"] == frame), None) if case else None
    if not snap:
        raise FileNotFoundError("This frame was not sampled")
    graphics = snap.get("graphics")
    if not graphics:
        raise ValueError("Rerun the recipe with sprite evidence enabled")
    base = directory / trial / snap["label"]
    with Image.open(base.with_suffix(".png")) as image:
        rgb = image.convert("RGB").getpixel((x, y))
    data = gzip.decompress(base.with_suffix(".gfx.gz").read_bytes())
    memory = {}
    offset = 0
    for kind, size in {**{key: value[1] for key, value in MEMORY.items()}, "players": graphics["player_bytes"]}.items():
        memory[kind] = data[offset:offset + size]
        offset += size
    if offset != len(data) or len(memory["players"]) != graphics["player_bytes"]:
        raise RuntimeError("The sprite capture is incomplete")
    candidates = obj_pixels.matching_objects(memory["oam"], memory["vram"], memory["pal"],
                                             obj_pixels.word(memory["io"], 0), x, y, rgb)
    submission = graphics["submission"]
    native = []
    if candidates and submission:
        with gzip.open(base.parent / "sprite-sources.tsv.gz", "rt") as source:
            native = [row for row in csv.DictReader(source, delimiter="\t")
                      if int(row["generation"]) == submission["generation"]]
    index = fields.function_index()
    for entry in candidates:
        rows = sources_for(native, entry["slot"], entry["attributes"])
        entry["submissions"] = []
        for row in rows:
            address = int(row["drawable"], 16)
            entry["submissions"].append({
                "queue": row["queue"], "slot": int(row["index"]), "runtime_pc": row["pc"],
                "function": cpu_function(row["pc"], index), "return_pc": row["resume"],
                "caller": cpu_function(row["resume"], index), "drawable": row["drawable"] if address else None,
                "asset": asset(int(row["tiles"], 16), rom),
                "actor": owner(address, memory["players"], graphics["player_members"], result, snap) if address else None,
            })
    upload = {**submission, "function": cpu_function(submission["upload_pc"], index)} if submission else None
    return {"run": result["id"], "trial": trial, "frame": frame, "game_frame": snap["frame"],
            "pixel": [x, y], "rgb": rgb, "upload": upload, "objects": candidates}
