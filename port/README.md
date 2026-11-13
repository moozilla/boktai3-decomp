# Boktai 3 source port

## Hardware slice

`port/hw/` supplies portable BIOS operations and hardware services; its public
interface is `port/include/hw.h`. It builds without a libc, ROM, or GBA compiler.
Game source and the matching GBA build are unchanged.

Run from the repository root:

```sh
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
`hw_hblank` once per scanline and `hw_vblank` at line 160. DMA timing is
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
keypad input is active-low with OR/AND keypad interrupts.

Boktai 3 uses **8 KB EEPROM**, as confirmed by `docs/EEPROM.md`. The layer offers
512-byte/8 KB EEPROM serial transfers and SDK `Eeprom_*` block replacements,
plus 64/128 KB flash command handling for unlock, identification, banking,
programming and erase. Save bytes remain in `Hw`; load them with `hw_save_load`,
persist the range returned by `hw_save_data`, then call `hw_save_clean`. Storage
operations complete immediately. EEPROM file bytes preserve serial wire order.

RTC GPIO supports reset, status, date/time and time commands, with host time and
a writable offset. The default is deterministic 2000-01-01. Normal operation
uses 24-hour BCD. Solar GPIO counts clock pulses against a user-set threshold
(`hw_set_solar`, 0 bright through 255 dark); these values are not physical lux.

### Validation and remaining integration

The portable tests cover known decompression vectors, truncation, affine
cardinals, division corner cases, DMA modes, cascade/IRQ timing, FIFO refill,
keypad matching, flash, EEPROM wire reads, leap-day RTC rollover and solar
thresholds. Full game boot, register-store instrumentation, and audio playback
are integration work; this test module does not claim to run the game.
