# Boktai 3 source port

Tags: #port #runtime #build

The first target is wasm32: pointers and integers remain four bytes, and linear
memory can hold the GBA regions at their original addresses. The user supplies a
ROM at runtime. No ROM bytes or generated assets belong in the port source.
The original agbcc build does not include anything under `port/`.

## Build the C
## Hardware slice

`port/hw/` supplies portable BIOS operations and hardware services; its public
interface is `port/include/hw.h`. It builds without a libc, ROM, or GBA compiler.
Game source and the matching GBA build are unchanged.

Run from the repository root:

```sh
build/venv/bin/python port/build.py --target wasm32
build/venv/bin/python port/build.py --target host
```

LLVM clang is selected from Homebrew when available; override with `--cc` or
`PORT_CC`. Four compiler processes run by default (`-j`). Each source retains its
own object under `build/port/TARGET/src/`, and `compile.json` records every unit,
its result and diagnostic path. Unchanged inputs are cached by content, compiler
identity and flags. `--source src/fn/AgbMain.c` limits a diagnostic run.

The wasm32 compile sweep currently passes all **10,169 translation units**,
including `src/lib/`. This proves compilation, not a linked or playable game.
The link and runtime work described below is separate.

## Compiler compatibility

Compatibility changes apply only to generated inputs beneath `build/port/`.
The original C and headers remain inputs to the byte-identical GBA build.

| Failure or difference | Port treatment |
| --- | --- |
| ARM register pins and empty matching barriers | Forced include erases `asm(...)` and `register`; source preparation normalizes the `asm volatile` spelling without changing volatile MMIO accesses. |
| Global assembler includes / `INCLUDE_ASM` | Suppressed in portable objects; these are assembly boundaries that need named C replacements before linking. |
| SDK `SystemCall(n)` | Generated SDK header calls `port_system_call(n)`; device/BIOS layer supplies it. |
| Declarations used before their later prototypes | Add the existing prototype before the actor animation and golem units in generated source. |
| `PlayerCueTimers` size assertion | Give that record four-byte alignment in a generated header, preserving agbcc's word-rounded size and every member offset on wasm32. |
| EEPROM cast lvalues | Express the two byte-pointer increments as ordinary pointer assignments in generated source. |
| Implicit declarations and legacy cross-unit pointer signatures | GNU89 compatibility diagnostics permit compilation; link/runtime signature reconciliation remains necessary. |
| Host pointer-width assertions | Omit named size/layout assertion typedefs only for host objects. Host objects are suitable for selected subsystem tests, not full GBA-ABI execution. wasm assertions remain enabled. |

The compatibility layer never supplies empty bodies for missing executable code.
A successfully compiled object can still contain unresolved dependencies.

## Memory and platform interface

[`include/platform.h`](include/platform.h) is the small shared core/device/PPU
interface. `core/memory.c` validates addresses against EWRAM, IWRAM, I/O, palette,
VRAM, OAM and ROM, and loads runtime-supplied ROM bytes at `08000000`. wasm memory
must span at least `0A000000` bytes (160 MiB); ordinary module data and its stack
must live below `02000000`, outside the emulated hardware ranges. Reset clears
RAM/devices and preserves ROM. Hardware address mirrors are not implemented.

The native mapper requests each region without `MAP_FIXED` and rejects any
mapping at the wrong address, so it never overwrites an existing host mapping.
On Apple Silicon, use an x86_64 test binary with `-Wl,-no_pie` and
`-Wl,-pagezero_size,0x10000`: native arm64 executables reserve the first 4 GiB.
Native 64-bit pointers still prevent full-game struct compatibility.

## Frames and interrupts

`core/timing.c` advances 228 scanlines of 1,232 cycles, with 960 drawing cycles per
line and VBlank starting at line 160. Device hooks receive elapsed cycles; the
PPU hook receives visible scanlines before HBlank. DISPSTAT and VCOUNT update at
these boundaries, and enabled VBlank/HBlank/VCount IRQs dispatch through the
game's table at `03003A10`, using the order and nesting mask in `IntrMain`
(`080001F0`). Devices can raise timer/serial/DMA/keypad IRQs with `port_irq_raise`.
This is cooperative event timing, not CPU instruction timing.

The resolver hook translates an ARM ROM/IWRAM code address into a portable
function. ROM data pointers retain their addresses; ROM **code** pointers require
this translation because wasm function pointers are table indices. A missing
handler returns an error. There is no attempt to execute ARM bytes.

`port_wait_vblank` advances devices and IRQs instead of spinning on a latch.
Original game code that busy-waits on DMA/VBlank needs explicit platform
boundaries or generated MMIO instrumentation before it can run cooperatively.

## Current execution limits

The full game has not linked or booted. Absolute RAM/ROM data binding and volatile scalar MMIO instrumentation are
implemented. Remaining work includes relocation of ROM function-pointer tables,
strict wasm function signature mismatches, remaining intentional assembly
replacements, and validation of device behavior beyond the boot slice. The portable runtime
and its tests are a foundation for that integration, not an emulator.

## Linked boot slice and repeatable tests

The early boot slice now links with LLD and runs in Node with **zero imports**.
It executes the original `Rtc_Initialize`, `Rtc_EnableGpioReads`,
`Clock_ClearState` and `Memory_Clear`, using port-hw's `CpuSet`. It preserves the
two EWRAM boot words and the IWRAM stack tail. A separate check copies and verifies
the complete 0x110C-byte IWRAM code payload. The clock RAM at `03005430:24` matches
mGBA's first video-frame snapshot on both the native host and wasm.

```sh
build/venv/bin/python port/tests/run_core.py
build/venv/bin/python port/tests/run_boot.py
build/venv/bin/python port/build.py --slice
```

Before the port-hw branch is integrated, append
`--hardware-root ../cx-port-hw/port` to the boot test/build commands. Set `WASM_LD`
to select a linker. Discovery also checks `~/git/boktai3-decomp/build/toolchain/wasm-ld`
and then `PATH`. The wasm output is `build/port/slice-wasm32/boot.wasm`.
`run_boot.py` runs the native executable, Node, then the mGBA harness and compares
the 36-byte dump. `--no-reference` skips mGBA when its harness is unavailable.
All dumps, probe logs and result JSON stay under `build/port/`.

`core/addresses.py` binds external data declarations to the existing GBA symbol
addresses before compiling LLVM IR to objects. wasm does not support the GBA
linker's absolute data symbols. This changes address relocation only: C function
bodies and callable function symbols remain compiled by stock clang. Function
addresses declared as byte arrays remain ROM-byte references for copy operations.
The build records every bound and unresolved external in `bindings.json`.

`--link-game` attempts the complete game link with these bindings and the
cooperative entry in `core/entry.c`; detailed linker diagnostics go in
`build/port/wasm32-bound/link.log`. This broader path remains under development.

The complete host compile sweep now also passes all 10,169 game units. Generated
host inputs omit ABI assertion typedefs and shrink three padding arrays whose
GBA gaps are consumed by wider host pointers. These objects still use the host's
wider layouts; the full game remains a wasm32 target.

The ARM soft-double library swaps its two 32-bit words around IEEE operations;
wasm and current hosts use little-endian double words. Generated source removes
those swaps, and the generated fdlibm header orders `lsw` before `msw`.
`build/venv/bin/python port/tests/run_math.py` checks eight arithmetic, conversion,
square-root and sine cases using the original library C on host and wasm.

With `--slice --mmio`, volatile scalar game accesses go through typed C bus
functions. This preserves immediate DMA transfers, GPIO and write-one-to-clear
IRQ semantics. The test runs the original `InterruptsAndVideo_Initialize` and
checks VRAM/palette clears, every OAM word, and both interrupt-table/code copies.
Device mapping also accepts the wasm module's data/stack below EWRAM, because
DMA sources can point at C locals. The plain address-validation API continues to
accept only hardware regions. The pass removes optimizer side-effect assumptions
that no longer hold after an access can trigger DMA or callbacks.

The first complete link audit exposes hundreds of cross-unit ABI declarations
that agbcc/ARM tolerated but wasm rejects or replaces with traps. The build
preserves warnings in `link.log` and summarizes them in `link-audit.json`.
`build/venv/bin/python port/core/signatures.py` compares compiled declarations to
actual C definitions and separates return-type, argument-count and argument-type
conflicts. This is an inventory, not automatic argument repair: inventing missing
arguments or return values would change the game's behavior.

The core supplies C equivalents of the SDK call-via and IWRAM interworking
veneers. The remaining full-link dependencies are MP2K's handwritten sequence
routines (including `MPlayMain`, `TrackStop` and several event handlers), alongside
the call-signature cleanup and ROM function-pointer translation still required
for full execution. No fallback bodies are supplied for those routines.

With `--ppu-root ../cx-port-ppu/port` and `--mmio`, the slice routes visible
scanlines to the sibling C PPU. `run_boot.py --ppu-root ../cx-port-ppu/port`
checks the first forced-blank frame against mGBA in RGB555. This is the initial
white frame, not a gameplay boot. CPU MMIO writes notify the affine renderer;
DMA writes to affine registers still need a device notification interface.

## See the port

[`demo/README.md`](demo/README.md) describes the offline browser viewer. Build it
with `python port/demo/build_demo.py` (pass sibling hardware/PPU roots before
integration), then open `build/demo/index.html`. The generated page embeds the
compiled C module and accepts a ROM or PPU captures through local file pickers.
No server is required. The source page also supports choosing the wasm file.

Thirty moving checkpoint frames replayed through the demo module match all
38,400 mGBA pixels per frame. Local PNG comparisons, a GIF and a lossless APNG
are listed in `build/demo/README.md`; none of those ROM-derived artifacts are
committed. This is captured-state rendering, not full gameplay execution.
The Node page-handler test covers loading the module, decompressing captures,
comparing a reference, and running original early boot C. Browser visual testing
was unavailable in this worker environment.

The MMIO slice also includes the original `Solar_InitTimer` and
`Solar_OnTimerInterrupt`. `node port/tests/solar_irq.cjs
build/port/slice-wasm32/boot.wasm` checks that timer 3 invokes the game C 13 times
during its first frame, updates the measurement counter, acknowledges IF and
restores IME. The resolver accepts both the portable pointer installed by C and
the original ROM address. This focused resolver is not a complete game callback
registry. The slice keeps original RTC C and excludes port-hw's optional RTC SDK
replacement, because generated MMIO instrumentation already routes its edges.

The `--slice --rtc` extension runs original `Clock_CheckRtcDate`, including the
GPIO protocol, date validation and sun-clock update. Run
`python port/tests/run_rtc.py --hardware-root ../cx-port-hw/port`: all 44 bytes of
clock and RTC RAM match native, wasm and mGBA frame 3. The harness uses `TZ=UTC`
because mGBA converts its fixed epoch through local time. The slice links without
warnings. Generated port source makes the incoming date arguments explicit at
`SunClock_SetDateTime`'s `Clock_PackDate` call (the GBA prologue leaves r0/r1/r2
unchanged) and corrects two discarded return declarations. The original source
and GBA build are untouched. The demo now includes this RTC extension.
build/venv/bin/python port/hw/build.py
```

This runs native tests with AddressSanitizer/UBSan, compiles wasm32 objects,
links `build/port-hw/hw-test.wasm`, and runs the same tests in Node. `CLANG`,
`WASM_LD`, and `NODE` override tool discovery. The linker fallback is
`~/git/boktai3-decomp/build/toolchain/wasm-ld`. `--native-only` skips wasm;
`--no-test` builds both artifacts without running them. The shared port builder
can use `port.hw.build.find_wasm_ld()` for the same linker discovery.

### Host and frame-loop integration

Allocate an `Hw`, pass callbacks to `hw_init`, then call `hw_bind` for the SDK
entry points. `HwHost.map` maps a GBA address range to stable host storage. A
wasm host can return the address itself; native tests use separate arrays.
The host owns persistence and supplies UTC Unix time, input, interrupts,
wait/yield, and reset handling. Missing BIOS operations return `HW_UNSUPPORTED`;
the optional `bios_error` callback reports wrapper failures.

Use `hw_write8/16/32` for hardware writes and `hw_read8/16/32` for reads. These
implement DMA enable edges, FIFO writes, timer reloads, interrupt flag
acknowledgment, and cartridge serial edges. `hw_sync_io` imports direct stores
from a mapped I/O page, but cannot observe repeated identical writes, writes
overwritten before synchronization, or a busy-wait between stores. Core must
route those accesses through the bus or replace the relevant SDK functions.
Mapping linear memory alone does not implement hardware side effects.

The frame loop calls `hw_step_cycles` to advance timers and sound FIFOs,
`hw_scanline_start` at each line start, `hw_hblank` at the start of HBlank,
and `hw_vblank` once at line 160. Line-start updates clear HBlank and generate
VCOUNT-match interrupts; HBlank does not repeat that match interrupt. DMA timing is
functional, not cycle-accurate. HBlank DMA runs only on visible lines. Interrupt
callbacks receive pending enabled bits when IME is set; the host dispatches the
game's handlers and acknowledges IF through a bus write. The BIOS wait callback
must yield to this scheduler; it must not recursively run a second game frame.

### BIOS

The SDK names CpuSet, CpuFastSet, Div/Mod, Sqrt, ArcTan/ArcTan2, affine transforms,
LZ77, RL, Huffman, delta filters, RAM reset, VBlank wait, reset, and sound bias
have portable implementations. `hw_bios_call` dispatches supported SWIs with
explicit arguments. `hw_bios_decompress` validates input/output bounds and
compressed backreferences; prefer it for runtime ROM assets. Original SDK
wrappers retain their trusted-pointer API. VRAM output uses ordinary bytes in
the port's backing memory, so decompression is not routed through GBA byte-store
replication. No BIOS image or ROM-derived table is included.

Force-include `port/include/hw_bios_shim.h` after the core compatibility shim to
replace SDK `SystemCall(n)` with the portable dispatcher. The game uses Stop
(3). The sound library also has a literal SWI 0x2A in `sub_0823025C`; it needs
the sound jump-table replacement when that library is integrated.

### Cartridge and devices

DMA supports all four channels, immediate/VBlank/HBlank triggers, increment,
decrement, fixed/reload addresses, repeat, completion interrupts, and sound FIFO
requests on channels 1/2. Timers support prescalers, reload, cascade and IRQ;
keypad input is active-low with OR/AND keypad interrupts. Byte writes preserve
timer reload values and PSG length counters; display/sound status bits remain
read-only.

Boktai 3 uses **8 KB EEPROM**, as confirmed by `docs/EEPROM.md`. The layer offers
512-byte/8 KB EEPROM serial transfers and SDK `Eeprom_*` block replacements,
plus 64/128 KB flash command handling for unlock, identification, banking,
programming and erase. Save bytes remain in `Hw`; load them with `hw_save_load`,
persist the range returned by `hw_save_data`, then call `hw_save_clean`. Storage
operations complete immediately. EEPROM file bytes preserve serial wire order.

RTC GPIO supports reset, status, date/time and time commands, with host time and
a writable offset. The default is deterministic 2000-01-01. Normal operation
uses 24-hour BCD. `port/include/hw_rtc.h` declares portable SIIRTC SDK
entry points, including protection, status translation, date/time reads and
writes, reset and alarm storage. They preserve the game record layout and
software-lock byte at `030035C6`, and drive the GPIO device through bus calls.
Alarm interrupts and 12-hour wire conversion are not implemented. Solar GPIO counts clock pulses against a user-set threshold
(`hw_set_solar`, 0 bright through 255 dark); these values are not physical lux.

### Validation and remaining integration

The portable tests cover known decompression vectors, truncation, affine
cardinals, division corner cases, DMA modes, cascade/IRQ timing, FIFO refill,
keypad matching, flash, EEPROM wire reads, leap-day RTC rollover and solar
thresholds. Full game boot, register-store instrumentation, and audio playback
are integration work; this test module does not claim to run the game.

Optional runtime-ROM validation (system libmgba headers/library required for
the oracle; override its prefix with `MGBA_PREFIX`):

```sh
build/venv/bin/python port/hw/build.py --mgba-rom baserom.gba
build/venv/bin/python port/hw/build.py --song-rom baserom.gba --song 1 --seconds 10
```

The oracle also checks 4,491 Div/Sqrt/copy/decompression vectors against mGBA,
executing its replacement BIOS for copy SWIs. It checks signed ArcTan across its full input range, ArcTan2 across
quadrants, all 256 affine angle steps, and 128 bounded LZ77 candidates read
from the runtime ROM. Integer arithmetic agrees exactly; affine coefficients
allow one unit because mGBA HLE rounds negative products differently. It also
executes the ROM's original SoundMain/IWRAM mixer on a synthetic channel and
compares PCM (within one 8-bit unit), fractional phase and remaining sample
count (exact). Another 96 original-mixer calls cover fixed/interpolated playback,
looping/sample end, ADSR and pseudo echo. The sample-end oracle supplies the
explicit interpolation guard byte that the original reads beyond sample size;
the port substitutes zero (or the loop start) to keep accesses bounded.
It runs the original EEPROM write/read routines and checks both
readback and backing-file byte order. All generated bytes remain under ignored
`build/`.

### Sound

`hw_mp2k_mix` replaces the IWRAM mixer using the existing SDK `SoundInfo`,
`SoundChannel`, and `WaveData` declarations. It advances attack/decay/sustain/
release and pseudo-echo envelopes once per call, applies channel/master volume,
supports fixed and interpolated PCM playback and loops, and writes both a
signed 16-bit stereo stream and the SDK's signed 8-bit DMA ring. It clamps the
final stereo mix; original packed-byte overflow behavior is not reproduced.
Reverb uses feedback from adjacent DMA-ring blocks. It is functional audio,
not a claim of bit-identical GBA audio.

The SDK `SoundMain` replacement finds the sound state through the mapped
`03007FF0` pointer, or `hw_m4a_attach` for a native host. It calls the existing
sequencer and CGB callbacks, imports register writes, renders PCM and PSG, then
delivers `HwHost.audio(user, stereo, frames, rate)`. The callback must copy or
consume the buffer before the next call. Sample-rate conversion to the host's
audio device belongs in the host. Do not also feed the same MP2K DMA FIFO to
the output, which would duplicate Direct Sound.

PSG supports both pulse channels with duty/length/envelope and channel-1
frequency sweep, 32-sample wave playback with volume control, and 7/15-bit
noise. The 512 Hz frame sequencer advances from the CPU clock. Explicit bus
writes observe retrigger strobes and DAC power; wave RAM currently represents
one audible 32-sample bank. 64-sample bank switching, analog filtering, exact
sweep/length edge quirks, FIFO output resampling and master bias ramp timing
remain work for hardware accuracy.

The WAV test has a small, bounded MP2K test sequencer that feeds real song
samples into this mixer. It exercises PCM notes, gates, tempo, patterns,
programs, key split/rhythm, pitch and stereo volume; it reports skipped PSG
notes. LFO, memory-access and extended sequence commands are omitted. This
utility is for mixer validation; the port should keep the game's C sequencer.
The ten-second song-1 run renders 133 PCM notes with no skipped PSG notes.
There is no committed ROM, sample table, decompressed asset, audio, or BIOS.

### Offline audio showcase

Open `port/demo/hardware.html` to hear the C/wasm PSG or choose a local ROM
for PCM music. It includes a live scope and WAV export. Build and reproduction
instructions are in `port/demo/README.md`; generated plots, animation, video
and runtime-ROM audio stay under `build/demo/`.
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

The synthetic suite generates 315 cases without a ROM and compares directly with
mGBA's software renderer: all text sizes/depths/layers, affine and bitmap modes,
transforms, sprite shapes/sizes/depths/flips, clipping, affine/double-size OBJ,
windows, blending, brightness, mosaic, green swap and HBlank register changes.
Boundary tests cover tile-bank wrapping, bitmap OBJ eligibility, invalid BG
characters, transparent OBJ priority propagation, vertical window latches and
sprite cycle budgets. All 315 match exactly under AddressSanitizer and
UndefinedBehaviorSanitizer; all 315 also match in linked WebAssembly. Separate C
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
build/venv/bin/python port/build_ppu.py --ppu
build/venv/bin/python port/demo/build.py
```

`WASM_LD` selects an executable linker and `WASM_CC` selects Clang. An invalid
explicit linker override fails instead of silently using another toolchain.
The platform port can reuse `find_wasm_ld()` and `build_ppu()` when combining
subsystem builds. None of these tools modifies `src/` or the agbcc build.
