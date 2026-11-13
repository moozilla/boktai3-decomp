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
`hw_scanline_start` at each line start, `hw_hblank` at the start of HBlank,
and `hw_vblank` once at line 160. Line-start updates clear HBlank and generate
VCOUNT-match interrupts; HBlank does not repeat that match interrupt.
DISPSTAT writes preserve the current match flag until a scanline update,
without raising a write-triggered IRQ, as checked against mGBA. DMA timing is
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
A raw reset clears status; the SDK reset/probe restores 24-hour mode and
reports recovery, matching the original SDK in mGBA. Alarm interrupts and
12-hour wire conversion are not implemented. Solar GPIO counts clock pulses against a user-set threshold
(`hw_set_solar`, 0 bright through 255 dark); these values are not physical lux.

### Validation and remaining integration

The portable tests cover known decompression vectors, truncation, affine
cardinals, division corner cases, DMA modes, cascade/IRQ timing, FIFO refill,
keypad matching, flash, EEPROM wire reads, leap-day RTC rollover and solar
thresholds. Decoder robustness tests include 405 generated round trips and
20,000 malformed/truncated inputs with output guards, on native and wasm.
The mGBA oracle compares 3,640 solar GPIO edges at seven thresholds.
Full game boot, register-store instrumentation, and audio playback
are integration work; this test module does not claim to run the game.

Optional runtime-ROM validation (system libmgba headers/library required for
the oracle; override its prefix with `MGBA_PREFIX`):

```sh
build/venv/bin/python port/hw/build.py --mgba-rom baserom.gba
build/venv/bin/python port/hw/build.py --song-rom baserom.gba --song 1 --seconds 10
```

The oracle also checks 4,568 Div/Sqrt/copy/decompression vectors against mGBA,
executing its replacement BIOS for copy SWIs, including 77 overlapping fast
copies. Native/wasm tests also check the maximum 21-bit CpuFastSet count after
rounding to eight words. Another 57 cases verify multi-record object-affine
strides and BitUnPack widths/offsets against complete mGBA output buffers.
It samples ArcTan across the signed 16-bit range, checks ArcTan2 across
quadrants, all 256 affine angle steps, and 128 bounded LZ77 candidates read
from the runtime ROM. Integer arithmetic agrees exactly; affine coefficients
allow one unit because mGBA HLE rounds negative products differently. It also
executes the ROM's original SoundMain/IWRAM mixer on a synthetic channel and
compares PCM (within one 8-bit unit), fractional phase and remaining sample
count (exact). Another 96 original-mixer calls cover fixed/interpolated playback,
looping/sample end, ADSR and pseudo echo. The sample-end oracle supplies the
explicit interpolation guard byte that the original reads beyond sample size;
the port substitutes zero (or the loop start) to keep accesses bounded.
It runs 576 immediate DMA cases across all four channels, widths, address modes and
repeat settings, comparing destination bytes and completion IRQs. It runs
the original RTC SDK across all 24 hours, status translation, protection and
probing with a fixed UTC source. The RTC 12-hour path remains unimplemented;
mGBA wraps hours modulo 12 without an AM/PM distinction and cannot independently
validate that distinction. It runs the original EEPROM write/read routines and checks both
readback and backing-file byte order. All generated bytes remain under ignored
`build/`.

### Sound

`hw_mp2k_mix` replaces the IWRAM mixer using the existing SDK `SoundInfo`,
`SoundChannel`, and `WaveData` declarations. It advances attack/decay/sustain/
release and pseudo-echo envelopes once per call, applies channel/master volume,
supports fixed and interpolated PCM playback and loops, and writes both a
signed 16-bit stereo stream and the SDK's signed 8-bit DMA ring. It clamps the
final stereo mix; original packed-byte overflow behavior is not reproduced.
Reverb wraps at the active DMA-ring boundary and preserves the original
feedback rounding; 16 ring/level cases agree with the original full PCM buffer. It is functional audio,
not a claim of bit-identical GBA audio.

The SDK `SoundMain` replacement finds the sound state through the mapped
`03007FF0` pointer, or `hw_m4a_attach` for a native host. It calls the existing
sequencer and CGB callbacks, imports pending initialization writes before them
and callback writes afterward, renders PCM, and applies Direct Sound master enable, FIFO A/B routing and half-volume controls
before adding PSG and delivering `HwHost.audio(user, stereo, frames, rate)`.
The callback must copy or consume the buffer before the next call. Sample-rate conversion to the host's
audio device belongs in the host. Do not also feed the same MP2K DMA FIFO to
the output, which would duplicate Direct Sound.

PSG supports both pulse channels with duty/length/envelope and channel-1
frequency sweep, 32/64-sample wave playback with volume control, and 7/15-bit
noise. The 512 Hz frame sequencer advances from the CPU clock. Explicit bus
writes observe retrigger strobes and DAC power. Wave RAM exposes the bank
opposite the playing bank to the CPU; 64-sample mode switches banks every
32 samples. Both CPU-visible banks agree with mGBA, and native/wasm tests
check the output across bank boundaries. Analog filtering, exact sweep/length
edge quirks, FIFO output resampling and master bias ramp timing remain work
for hardware accuracy.

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
