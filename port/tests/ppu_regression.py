#!/usr/bin/env python3
"""Capture mGBA scanlines and compare the portable PPU, or replay saved dumps.

Run from the repository root with build/venv/bin/python. ROM data stays in build/.
A custom harness script can use every normal tools/emu/harness command.
"""
import argparse
import gzip
import json
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
BUILD = ROOT / "build/ppu"
LLVM = Path("/opt/homebrew/opt/llvm/bin/clang")
sys.path.insert(0, str(ROOT))
from port.build import build_ppu


def run(args, **kwargs):
    subprocess.run([str(x) for x in args], check=True, cwd=ROOT, **kwargs)


def compiler():
    return os.environ.get("CC", str(LLVM) if LLVM.exists() else "clang")


def build(capture=False, sanitize=False):
    BUILD.mkdir(parents=True, exist_ok=True)
    flags = ["-std=c99", "-O2", "-Wall", "-Wextra", "-Werror", "-Iport/include"]
    if sanitize:
        flags += ["-g", "-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
    run([compiler(), *flags, "port/tests/ppu_render.c", "port/ppu/ppu.c", "-o", BUILD / "ppu_render"])
    build_ppu(BUILD)
    run(["node", "port/tests/ppu_wasm.mjs", BUILD / "ppu.wasm"])
    if not capture:
        return
    source = (ROOT / "tools/emu/harness.c").read_text()
    replacements = {
        "int main(int argc, char** argv) {": '#include "ppu_capture.h"\nint main(int argc, char** argv) {',
        "core->reset(core);": "core->reset(core);\n\tppu_capture_install();",
        'else if (!strcmp(cmd, "shot")) shot(a1);':
            'else if (!strcmp(cmd, "shot")) { shot(a1); ppu_capture_shot(a1); }',
    }
    for before, after in replacements.items():
        if source.count(before) != 1:
            raise RuntimeError(f"harness adapter needs updating: {before}")
        source = source.replace(before, after)
    source = source.replace('write_cov("total", rom_reader, rom_exec);',
                            'if (!tracing) { core->deinit(core); return 0; }\n\twrite_cov("total", rom_reader, rom_exec);')
    adapter = BUILD / "capture_harness.c"
    adapter.write_text(source)
    # Ask the existing Makefile for the exact headers, ABI defines and library.
    # mGBA struct layout depends on the library's build flags.
    result = subprocess.check_output(["make", "-Bn", "-C", "tools/emu", "harness"], cwd=ROOT, text=True)
    commands = [line for line in result.splitlines() if " -o harness harness.c " in line]
    if len(commands) != 1:
        raise RuntimeError("could not obtain mGBA harness compile command")
    command = shlex.split(commands[0])
    command[command.index("harness.c")] = str(adapter)
    command[command.index("-o") + 1] = str(BUILD / "ppu_capture")
    command += ["-I" + str(ROOT / "port/include"), "-I" + str(ROOT / "port/tests"),
                str(ROOT / "port/ppu/ppu.c"), "-lz"]
    run(command)


def compare(directory):
    from PIL import Image, ImageChops
    rows = []
    names = {p.name.split(".ppu")[0] for p in directory.glob("*.ppu*")
             if p.name.endswith((".ppu", ".ppu.gz"))}
    for name in sorted(names):
        dump = directory / (name + ".ppu")
        if not dump.exists():
            dump = directory / (name + ".ppu.gz")
        reference = directory / f"{name}.ppm"
        output = directory / f"{name}.port.ppm"
        opener = gzip.open if dump.suffix == ".gz" else open
        with opener(dump, "rb") as data, output.open("wb") as out:
            run([BUILD / "ppu_render"], input=data.read(), stdout=out)
        expected = Image.open(reference).convert("RGB")
        actual = Image.open(output).convert("RGB")
        if expected.size != (240, 160) or actual.size != expected.size:
            raise RuntimeError(f"bad frame size: {name}")
        diff = ImageChops.difference(expected, actual)
        count = sum(any(p) for p in zip(*[iter(diff.tobytes())] * 3))
        if count:
            diff.point(lambda c: 255 if c else 0).save(directory / f"{name}.diff.png")
        rows.append(dict(scene=name, mismatches=count, pixels=38400))
        print(f"{name}: {count} / 38400 pixel mismatches", flush=True)
    if not rows:
        raise RuntimeError(f"no PPU dumps in {directory}")
    (directory / "results.json").write_text(json.dumps(rows, indent=2) + "\n")
    return sum(row["mismatches"] for row in rows)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--capture", type=Path, help="harness script (captures each shot)")
    parser.add_argument("--out", type=Path, default=Path("build/ppu/scenes"))
    parser.add_argument("--rom", type=Path, default=Path("baserom.gba"))
    parser.add_argument("--save", type=Path, help="battery save (copied by harness)")
    parser.add_argument("--sanitize", action="store_true")
    parser.add_argument("--wasm", action="store_true", help="also replay every capture in Node")
    parser.add_argument("--build-only", action="store_true")
    parser.add_argument("--unit", action="store_true", help="run RGB555/state contract tests")
    args = parser.parse_args()
    build(capture=bool(args.capture), sanitize=args.sanitize)
    if args.unit:
        flags = ["-std=c99", "-O1", "-g", "-Wall", "-Wextra", "-Werror", "-Iport/include"]
        if args.sanitize:
            flags += ["-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
        run([compiler(), *flags, "port/tests/ppu_unit.c", "port/ppu/ppu.c", "-o", BUILD / "ppu_unit"])
        run([BUILD / "ppu_unit"])
    if args.build_only:
        return
    directory = args.out.resolve()
    directory.mkdir(parents=True, exist_ok=True)
    if args.capture:
        env = os.environ.copy()
        if args.save:
            env["BOKTAI3_SAV"] = str(args.save.resolve())
        run([BUILD / "ppu_capture", args.rom.resolve(), args.capture.resolve(), directory], env=env,
            stdout=subprocess.DEVNULL)
        for path in directory.glob("*.ppu"):
            with path.open("rb") as src, gzip.open(str(path) + ".gz.tmp", "wb", compresslevel=1) as dst:
                shutil.copyfileobj(src, dst)
            Path(str(path) + ".gz.tmp").replace(str(path) + ".gz")
            path.unlink()
    mismatches = compare(directory)
    if args.wasm:
        run(["node", "port/tests/ppu_wasm.mjs", BUILD / "ppu.wasm", directory])
    sys.exit(bool(mismatches))


if __name__ == "__main__":
    main()
