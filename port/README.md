# Source port

## Picture processor

`ppu/ppu.c` is a portable C99 scanline renderer. `include/ppu.h` defines its public
interface. It has no mGBA, libc, ROM, host-pointer-width or platform dependency.
Both native and freestanding wasm32 objects compile with Clang. All input arrays
use GBA little-endian byte order and may be unaligned. The output is 240 by 160
RGB555 pixels, with an optional RGB888 conversion helper.

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
build/venv/bin/python port/tests/ppu_regression.py --out build/ppu/boot
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

The first vertical slice matches three startup captures (zero pixel differences).
Gameplay coverage and synthetic tests are being expanded; the implemented feature
list is not yet a claim of hardware-perfect behavior for every edge case. A
snapshot taken after a frame cannot reproduce HBlank effects, which is why the
suite captures the actual inputs of each scanline. Sub-scanline register effects
and exact memory-bus timing are outside this renderer's interface.
