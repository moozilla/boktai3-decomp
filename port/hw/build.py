#!/usr/bin/env python3
"""Build/test the hardware slice from the repository root; no ROM or packages needed."""
import argparse
import os
from pathlib import Path
import shutil
import subprocess


def run(command):
    subprocess.run([str(arg) for arg in command], check=True)


def find_wasm_ld():
    configured = os.environ.get("WASM_LD")
    if configured:
        return configured
    installed = shutil.which("wasm-ld")
    if installed:
        return installed
    fallback = Path.home() / "git/boktai3-decomp/build/toolchain/wasm-ld"
    if fallback.is_file():
        return str(fallback)
    raise SystemExit("Set WASM_LD to a wasm-ld executable")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--native-only", action="store_true")
    parser.add_argument("--no-test", action="store_true")
    parser.add_argument("--mgba-rom", type=Path, help="also compare BIOS results against system libmgba using this runtime ROM")
    parser.add_argument("--song-rom", type=Path, help="render a runtime ROM song to build/port-hw/song-N.wav")
    parser.add_argument("--song", type=int, default=1)
    parser.add_argument("--seconds", type=int, default=10)
    args = parser.parse_args()
    if not Path("port/hw/bios.c").is_file():
        raise SystemExit("Run from the repository root")
    out = Path("build/port-hw")
    out.mkdir(parents=True, exist_ok=True)
    clang = os.environ.get("CLANG", "/opt/homebrew/opt/llvm/bin/clang")
    if not Path(clang).is_file():
        clang = shutil.which(clang) or shutil.which("clang")
    if not clang:
        raise SystemExit("Set CLANG to a clang with the wasm32 target")
    sources = sorted(Path("port/hw").glob("*.c"))
    flags = ["-std=c11", "-O2", "-Wall", "-Wextra", "-Werror", "-ffreestanding", "-fno-builtin", "-Iport/include", "-Iinclude"]
    run([clang, *flags, "-g", "-fsanitize=address,undefined", "-fno-sanitize-recover=all", *sources, "port/tests/hw_test.c", "port/tests/hw_native.c", "-o", out / "hw-test"])
    if not args.no_test:
        run([out / "hw-test"])
    if args.mgba_rom:
        prefix = Path(os.environ.get("MGBA_PREFIX", "/opt/homebrew"))
        oracle = out / "hw-mgba"
        oracle_flags = [f for f in flags if f != "-ffreestanding"]
        run([clang, *oracle_flags, "-std=gnu11", "-I" + str(prefix / "include"), *sources, "port/tests/hw_mgba.c",
             "-L" + str(prefix / "lib"), "-lmgba", "-o", oracle])
        run([oracle, args.mgba_rom])
    if args.song_rom:
        player = out / "hw-song"
        song_flags = [f for f in flags if f != "-ffreestanding"]
        run([clang, *song_flags, "-g", "-fsanitize=address,undefined", "-fno-sanitize-recover=all", *sources,
             "port/tests/hw_song.c", "-lm", "-o", player])
        run([player, args.song_rom, args.song, out / ("song-" + str(args.song) + ".wav"), args.seconds])
    if args.native_only:
        return
    objects = []
    for source in [*sources, Path("port/tests/hw_test.c")]:
        obj = out / (source.stem + ".wasm.o")
        run([clang, "--target=wasm32", *flags, "-c", source, "-o", obj])
        objects.append(obj)
    wasm = out / "hw-test.wasm"
    run([clang, "--target=wasm32", "-nostdlib", "-fuse-ld=" + find_wasm_ld(),
         "-Wl,--no-entry", "-Wl,--export=hw_test_run", *objects, "-o", wasm])
    if not args.no_test:
        run([os.environ.get("NODE", "node"), "port/tests/hw_wasm.js", wasm])
    print("Linked:", wasm)


if __name__ == "__main__":
    main()
