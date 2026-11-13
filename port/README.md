# Source port

## Picture processor

`ppu/ppu.c` is a portable C99 scanline renderer. `include/ppu.h` defines its public
interface. It has no mGBA, libc, ROM, host-pointer-width or platform dependency.
Native and freestanding wasm32 objects compile with Clang, and the suite links
a real WebAssembly module and runs it in Node. Set `WASM_LD` to override the
linker; the fallback is `~/git/boktai3-decomp/build/toolchain/wasm-ld`. All input arrays
use GBA little-endian byte order and may be unaligned. The output is 240 by 160
RGB555 pixels. The `*_rgb888` functions instead return `0x00BBGGRR`
pixels using mGBA software-renderer presentation arithmetic: colors expand
before blending, and darkening rounds the green/blue channels down. RGB555
retains 5-bit GBA color arithmetic. The regression suite explicitly uses RGB888
when comparing against mGBA 0.11 screenshots.

The renderer implements text backgrounds in every size and both depths, affine
backgrounds, bitmap modes 3/4/5, regular and affine sprites, sprite priorities,
OBJ windows, rectangular windows, alpha/brightness effects, mosaic and green swap.
The caller owns the renderer state and the four memory regions (IO, palette,
VRAM, OAM). Call `ppu_begin_frame` before visible line 0, then render each line
with `ppu_render_scanline`. Apply HBlank memory/register writes before the next
line. Notify `ppu_write_register` after writes to affine reference registers;
ordinary display registers are read from IO each line. `ppu_render_frame` is a
convenience for memory that stays constant throughout a frame.

### PPU validation

From the repository root:

```sh
build/venv/bin/python port/tests/ppu_regression.py --build-only
build/venv/bin/python port/tests/ppu_regression.py \
  --capture port/tests/ppu_boot.txt --save tests/saves/ShinBok2.sav \
  --out build/ppu/boot
build/venv/bin/python port/tests/ppu_regression.py --out build/ppu/boot --wasm
```

The capture command builds a private instrumented copy of the existing headless
harness, with the same mGBA headers, ABI defines and library as its Makefile. Each
`shot` records mGBA's screenshot and all 160 scanlines' input memory. An observer
of affine register writes maintains the portable PPU's reference state. It does
not use mGBA's background coordinates or pixel output as renderer input. Captures
are gzip compressed and remain under `build/`; no ROM-derived fixtures ship in
the repository. Replay needs only the portable renderer and Pillow. Reports
include per-scene pixel mismatch counts and a JSON result; failures produce a
visible difference image. Any mismatch makes the command fail.

Three startup captures and twelve menu/field captures match with zero pixel
differences. All twelve menu/field captures also match in Node WebAssembly.
The checkpoint runner also matches 40 frames across eight saves, covering dialogue,
dungeons, the sun gauge HUD and credits, in native and WebAssembly. Recreate them
with the user's save bank (an explicit directory avoids worktree-relative paths):

```sh
build/venv/bin/python port/tests/ppu_scenes.py --bank ../boktai3trans/saves --wasm
build/venv/bin/python port/tests/ppu_synthetic.py
```

The synthetic suite generates 150 cases without a ROM and compares directly with
mGBA's software renderer: all text sizes/depths/layers, affine and bitmap modes,
transforms, sprite shapes/sizes/depths/flips, clipping, affine/double-size OBJ,
windows, blending, brightness, mosaic, green swap and HBlank register changes.
All 150 currently match exactly. The implemented feature list and these finite
tests are not a claim of hardware-perfect behavior for every edge case. A
snapshot taken after a frame cannot reproduce HBlank effects, which is why the
suite captures the actual inputs of each scanline. Sub-scanline register effects
and exact memory-bus timing are outside this renderer's interface.
