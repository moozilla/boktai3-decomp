#!/usr/bin/env python3
"""Build the offline browser hardware showcase from the repository root."""
import base64
import importlib.util
import os
from pathlib import Path
import subprocess

spec = importlib.util.spec_from_file_location("hardware_build", "port/hw/build.py")
builder = importlib.util.module_from_spec(spec)
spec.loader.exec_module(builder)
clang = os.environ.get("CLANG", "/opt/homebrew/opt/llvm/bin/clang")
exports = ["demo_rom_buffer", "demo_rom_capacity", "demo_pcm_buffer", "demo_load_rom",
           "demo_start", "demo_psg", "demo_render", "demo_notes", "demo_skipped", "demo_error"]
wasm = Path("port/demo/hardware.wasm")
subprocess.run([clang, "--target=wasm32", "-std=c11", "-O2", "-Wall", "-Wextra", "-Werror",
                "-ffreestanding", "-fno-builtin", "-Iport/include", "-Iinclude", "-nostdlib",
                "-fuse-ld=" + builder.find_wasm_ld(), "-Wl,--no-entry", "-Wl,--initial-memory=50331648",
                *["-Wl,--export=" + name for name in exports],
                *map(str, sorted(Path("port/hw").glob("*.c"))), "port/demo/hardware.c", "-o", str(wasm)], check=True)
Path("port/demo/hardware-wasm.js").write_text(
    "// Generated from portable C by port/demo/build_hardware.py; no ROM data.\n"
    "const HARDWARE_WASM = '" + base64.b64encode(wasm.read_bytes()).decode("ascii") + "';\n")
out = Path("build/demo")
out.mkdir(parents=True, exist_ok=True)
# A single-file copy is convenient for sharing or opening directly from disk.
html = Path("port/demo/hardware.html").read_text().replace(
    '<script src="hardware-wasm.js"></script>', '<script>' + Path("port/demo/hardware-wasm.js").read_text() + '</script>')
(out / "hardware.html").write_text(html)
print("Offline showcase: port/demo/hardware.html; single-file copy: build/demo/hardware.html")
