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

The full game has not linked or booted. Remaining work includes symbol binding
for absolute RAM/ROM data, relocation of ROM function-pointer tables, strict wasm
function signature mismatches, platform replacements for intentional assembly,
and device side effects for direct register loads/stores. The portable runtime
and its tests are a foundation for that integration, not an emulator.
