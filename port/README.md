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

Ten startup/title/save-select frames and twelve menu/field frames match mGBA with
zero pixel differences in native C and Node WebAssembly. The complete 83-save
checkpoint bank produces 415 more exact frames in both native and WebAssembly, covering dialogue, dungeons, field scenes, the sun gauge
HUD and credits. The menu/field sequence includes a live boss encounter. Recreate captures with the user's save bank (an explicit directory
avoids worktree-relative paths):

```sh
build/venv/bin/python port/tests/ppu_scenes.py --bank ../boktai3trans/saves --wasm
# Sweep the whole bank; replay completed captures after an interruption.
build/venv/bin/python port/tests/ppu_scenes.py \
  --bank ../boktai3trans/saves --checkpoints all --resume \
  --out build/ppu/full-bank --wasm
build/venv/bin/python port/tests/ppu_synthetic.py --sanitize --wasm
build/venv/bin/python port/tests/ppu_regression.py \
  --out build/ppu/startup-delta --sanitize --unit --wasm
```

The synthetic suite generates 351 cases without a ROM and compares directly with
mGBA's software renderer: all text sizes/depths/layers, affine and bitmap modes,
transforms, sprite shapes/sizes/depths/flips, clipping, affine/double-size OBJ,
windows, blending, brightness, mosaic, green swap and HBlank register changes.
Boundary tests cover tile-bank wrapping, bitmap OBJ eligibility, invalid BG
characters, transparent OBJ priority propagation, vertical window latches and
sprite cycle budgets and screen-map fetches across VRAM banks. All 351 match exactly under AddressSanitizer and
UndefinedBehaviorSanitizer; all 351 also match in linked WebAssembly. Separate C
contract tests cover RGB555 arithmetic,
unaligned input, affine reference updates and sign/wrap behavior.

`PPUDUMP2` stores an eight-byte magic, then 160 scanline records: four signed
little-endian 32-bit affine origins (BG2 X/Y, BG3 X/Y), a byte of bank-change
flags, and the changed banks in IO/palette/VRAM/OAM order. Bank lengths are
0x400, 0x400, 0x18000 and 0x400 bytes. The first record must include every bank;
subsequent records retain unchanged banks. Gzip compression produces roughly
14–30 KiB per typical game frame. Replay also accepts the older `PPUDUMP1`
format, which stores every bank on every line. Neither format includes pixels
from mGBA as renderer input; references are separate PPM files.

These finite tests establish agreement with mGBA 0.11 on the tested inputs,
not hardware-perfect behavior for every edge case. A snapshot taken after a
frame cannot reproduce HBlank effects, which is why the suite captures the
actual inputs of each scanline. Sub-scanline register effects and exact
memory-bus timing are outside this renderer's interface. Mid-VBlank window
boundary writes are not modeled separately from the next frame's registers.

### Try the browser demo

Open `demo/index.html` directly. It runs the actual C renderer in WebAssembly,
with a local capture picker, optional mGBA reference comparison and sequence
playback. The bundled 7.3 KiB module contains only renderer code. No server,
ROM, captured memory or game pixels are bundled. See `demo/README.md` for
commands that regenerate the module, six side-by-side PNGs and a 60-frame APNG.
Generated images and captures stay in `build/demo/`; its README lists each
artifact. The browser bridge is tested in Node against captured frames; browser
UI automation is a separate validation step.

Build the standalone wasm slice with:

```sh
build/venv/bin/python port/build.py --ppu
build/venv/bin/python port/demo/build.py
```

`WASM_LD` selects an executable linker and `WASM_CC` selects Clang. An invalid
explicit linker override fails instead of silently using another toolchain.
The platform port can reuse `find_wasm_ld()` and `build_ppu()` when combining
subsystem builds. None of these tools modifies `src/` or the agbcc build.
