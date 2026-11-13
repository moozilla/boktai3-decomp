# Boktai 3 source port

Tags: #port #runtime #build

The first target is wasm32: pointers and integers remain four bytes, and linear
memory can hold the GBA regions at their original addresses. The user supplies a
ROM at runtime. No ROM bytes or generated assets belong in the port source.
The original agbcc build does not include anything under `port/`.

## Build the C

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
white frame, not a gameplay boot. CPU and DMA register publications notify the affine renderer, including
identical-value writes that reload internal reference latches.

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

The `--slice --rtc --video-irqs` extension resolves the original VBlank table
entry to `sub_08213FD8` compiled from C. Build it with the hardware-root override,
then run `node port/tests/video_irq.cjs
build/port/slice-wasm32-rtc-video/boot.wasm baserom.gba`. Three frames verify the
original handler's BG scroll uploads, dialogue latches, upload flag, clock ticks
and BIOS VBlank latch, plus IF acknowledgement and IME restoration. The test
uses valid inactive sound state and leaves wireless uncreated; active audio/RFU
callbacks are not covered. This focused resolver
is a tested integration boundary, not the full game's callback registry.

## Discarded return declarations

`core/returns.py` records the actual integer return types of 25 frequently called
functions. Generated sources and mirrored headers correct explicit `void`
declarations for those functions; arguments and call sites are unchanged. Each
build verifies the recorded type against its original C definition and fails
clearly if integration changes that evidence. Other return/argument conflicts
and indirect callback signatures remain unresolved.

`python port/tests/run_returns_slice.py` reproduces the original wasm link error
for `ParticlePool_Destroy` calling `Task_Kill`, then executes the original C
call chain on host and wasm after the repair. No ROM is needed for this test.
`python port/tests/run_returns.py --hardware-root ../cx-port-hw/port` recompiles
all affected units and audits an existing full-game object sweep: 1,894 game
units plus 14 core/device units compile; distinct linker conflicts fall from
823 to 804, with no new conflicts and the same seven missing MP2K routines.
Six corrected functions retain other ABI mismatches.

`--link-game --compile-only --source PATH` can rebuild selected bound/MMIO
objects without attempting a partial final link. The audit collects fatal
signature diagnostics separately from missing symbols because LLD stops early
on fatal warnings. Any diagnostic wasm output is deleted; the normal build
continues to reject signature warnings. Results live in `build/port/returns/`.

The same video-IRQ slice includes the original two-phase VCount handler
`sub_08214144`. Run `node port/tests/vcount_irq.cjs
build/port/slice-wasm32-rtc-video/boot.wasm baserom.gba` to check the comparison
change from scanline 64 to 144, the dialogue-scroll write, phase reset and IRQ
mask restoration. It checks exactly two VCount calls per frame, avoiding a
duplicate HBlank notification. Register setup uses the exported device bus,
just like generated game C. Sound state remains inactive in this test.

The generated port-hw input renames only `hw_publish_io`'s implementation; its
callers use a core C wrapper that observes the completed mapped write. This
keeps sibling hardware sources unchanged and delivers affine notifications for
CPU, immediate DMA, HBlank DMA and VBlank DMA publications. The wrapper handles
unchanged values and partial register writes, not just differences between
snapshots. A signature check rejects a changed hardware publication boundary.

`node port/tests/affine_dma.cjs build/port/slice-wasm32-rtc-video/boot.wasm`
requires a slice built with `--ppu-root`. Its synthetic mode-3 image proves that
repeated identical HBlank writes reset BG2Y each line, then normal affine row
stepping resumes after DMA is disabled. Native/wasm boot and RTC comparisons
and original VBlank/VCount/solar IRQ tests still pass.

Core timing owns DISPSTAT/VCOUNT and the interrupt masks/flags in mapped memory.
Before bus access, elapsed-device updates and blanking events, the bridge copies
those specific registers into port-hw's shadow without treating them as writes.
This prevents stale scanline reads and avoids losing pending flags or interpreting
them as write-one-to-clear acknowledgements.

`python port/tests/run_display_bus.py --hardware-root ../cx-port-hw/port` verifies
that a line-64 IRQ reads line 64 and the active comparison bit, a timer preserves
a core-raised interrupt, and selective IF acknowledgement preserves other flags.
The pre-fix evidence read line 63 and lost both kinds of pending flags. Core,
native/wasm RTC, original video/solar IRQ and affine DMA regressions pass.
