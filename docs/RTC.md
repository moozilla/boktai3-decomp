# Cartridge RTC matching evidence

Fourteen functions in `08248970–08249238` match independently reconstructed C
with **agbcc `-O0 -mthumb-interwork`**, adding **1,784 emitted native bytes**.
This differs from the usual game-code `-O2` setting. The r7 stack frame, byte
locals reloaded for each operation, and literal pools inside loops suggested
this test; exact per-function bytes and the complete ROM establish the result.
No compiler-vendor claim follows from a stack frame alone.

## Source correspondence

After reconstructing the two serial writers, serial reader, and GPIO enable
wrappers, comparison with pret's
[`src/siirtc.c`](https://github.com/pret/pokeemerald/blob/731ad5bfd6e6f265508d0efcca0ba42f9dcf5881/src/siirtc.c)
and [`include/siirtc.h`](https://github.com/pret/pokeemerald/blob/731ad5bfd6e6f265508d0efcca0ba42f9dcf5881/include/siirtc.h)
identified the larger family. This is reconstructed SDK source, not original
Konami source. B3's compiled representation differs: eight-bit `u32` bitfields
reproduce its status stores, narrowing operations, and twelve-byte local RTC
record. Ordinary `u8` fields do not generate the observed read-mask-write.
The source addresses remain unchanged; proposed semantic names are recorded in
`symbols/proposed/rtc.csv`.

| B3 function | Bytes | Corresponding operation |
|---|---:|---|
| `08248970` | 24 | Unprotect: enable GPIO reads and clear software lock |
| `08248988` | 24 | Protect: disable GPIO reads and set software lock |
| `082489A0` | 216 | Probe status/time and reset on observed flags |
| `08248A78` | 132 | Reset command, then set 24-hour status |
| `08248AFC` | 204 | Read status and translate interrupt flag positions |
| `08248BC8` | 168 | Translate status and write it |
| `08248C70` | 176 | Read seven date/time bytes, mask hour bit 7 |
| `08248D20` | 156 | Write seven date/time bytes |
| `08248DBC` | 180 | Read three time bytes into record offsets 4–6 |
| `08249040` | 164 | Write command byte, most-significant bit first |
| `082490E4` | 160 | Write data byte, least-significant bit first |
| `08249184` | 140 | Read data byte, least-significant bit first |
| `08249210` | 20 | Enable GPIO port reads |
| `08249224` | 20 | Disable GPIO port reads |

## Hardware and behavior evidence

The routines use cartridge GPIO data/direction/read-enable registers at
`080000C4`, `080000C6`, and `080000C8`. These are hardware addresses, not
relocatable ROM data. Command bytes `60`, `62/63`, `64/65`, and `67` select
reset, write/read status, write/read date-time, and read time. Software lock
byte `030035C6` gates the public transactions. Command writes clock three
low phases and one high phase per bit; reads clock five low writes and one
high write. Those repeated volatile stores are required behavior.

Probe preserves the source's redundant 12-hour-mode status condition and the
second-byte test-mode flag check. No bugfix options were enabled. The byte
reader shifts an initially uninitialized local eight times, replacing every
bit before returning. The two writers retain non-void fallthrough, which
keeps the original return-register convention; known callers discard that
value. These inherited source shapes match this compiler, and are not a claim
of portable modern-C behavior. Adding initializers/returns changes the ROM.

Undecoded bytes after `08248DBC` contain apparent further entry points at
`08248E70` and `08248F0C`, corresponding to time-write and alarm operations.
The first has an exact 156-byte standalone C candidate and a reviewed progress
boundary, so its following 464-byte block is **not credited** to the preceding
matched function. It is still retained assembly in the ROM. The alarm candidate
emits 304 rather than 308 bytes; local bitfield-address evaluation order differs.
Neither candidate is counted as matched C. Investigate them in a coordinated
boundary pass without rewriting shared `gen/` during active matching rounds.

The first seven routines were reconstructed before consulting the comparison
source; larger transaction routines were reconstructed from B3 instructions
with that source as structural context. The preexisting ROM layout is retained,
with no new strings, constants, or global storage.
