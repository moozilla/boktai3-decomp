#!/usr/bin/env python3
"""Build the portable PPU WebAssembly slice without an SDK or game ROM.

The platform port can reuse find_wasm_ld() and build_ppu() when combining slices.
WASM_LD selects the linker; WASM_CC (or --cc) selects Clang.
"""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parent.parent


def find_wasm_ld():
    configured = os.environ.get("WASM_LD")
    if configured:
        expanded = str(Path(configured).expanduser())
        resolved = shutil.which(expanded)
        if not resolved:
            raise FileNotFoundError("WASM_LD does not name an executable linker")
        return resolved
    fallback = Path.home() / "git/boktai3-decomp/build/toolchain/wasm-ld"
    if fallback.is_file() and os.access(fallback, os.X_OK):
        return str(fallback)
    resolved = shutil.which("wasm-ld")
    if not resolved:
        raise FileNotFoundError("wasm-ld not found; set WASM_LD to its executable path")
    return resolved


def find_clang():
    configured = os.environ.get("WASM_CC")
    if configured:
        return str(Path(configured).expanduser())
    llvm = Path("/opt/homebrew/opt/llvm/bin/clang")
    return str(llvm) if llvm.is_file() else "clang"


def build_ppu(directory=None, cc=None):
    directory = Path(directory or ROOT / "build/ppu").resolve()
    directory.mkdir(parents=True, exist_ok=True)
    cc = cc or find_clang()
    module = directory / "ppu.wasm"
    linker = find_wasm_ld()
    # Publish only completed outputs: an ongoing Node replay can keep using the
    # previous module while another test rebuilds this slice.
    with tempfile.TemporaryDirectory(prefix="ppu-link-", dir=directory) as scratch:
        obj = Path(scratch) / "ppu.wasm.o"
        linked = Path(scratch) / "ppu.wasm"
        subprocess.run([cc, "--target=wasm32", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror",
                        "-ffreestanding", "-Iport/include", "-c", "port/ppu/ppu.c", "-o", str(obj)],
                       cwd=ROOT, check=True)
        subprocess.run([cc, "--target=wasm32", "-nostdlib", "-fuse-ld=" + linker,
                        "-Wl,--no-entry", "-Wl,--export-all", str(obj), "-o", str(linked)],
                       cwd=ROOT, check=True)
        obj.replace(directory / "ppu.wasm.o")
        linked.replace(module)
    return module


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ppu", action="store_true", help="build the PPU slice (default)")
    parser.add_argument("--out-dir", type=Path)
    parser.add_argument("--cc")
    args = parser.parse_args()
    try:
        module = build_ppu(args.out_dir, args.cc)
    except FileNotFoundError as error:
        parser.exit(1, f"{error}\n")
    print(module.relative_to(ROOT) if module.is_relative_to(ROOT) else module)


if __name__ == "__main__":
    main()
