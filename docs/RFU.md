# RFU wireless SDK source matches

Boktai 3 contains Nintendo's RFU wireless-adapter library version 1024. The
`RFU_V1024` string at `08602CF0` is supported by exact compiled-source matches
across the transport, link API, serial identification and ARM interrupt code.
This identifies the linked SDK implementation; it does not establish which
wireless features the game's own code exposes or which scenarios execute them.

The reference is the reconstructed SDK in
[pret/pokeemerald at 731ad5bfd6e6f265508d0efcca0ba42f9dcf5881](https://github.com/pret/pokeemerald/tree/731ad5bfd6e6f265508d0efcca0ba42f9dcf5881).
Files are `src/librfu_stwi.c`, `src/librfu_intr.c`, `src/librfu_rfu.c`,
`src/librfu_sio32id.c` and `include/librfu.h`. Select `LIBRFU_VERSION 1024`;
the upstream header's default is a later revision. This is reconstructed
source, not a claim to possess Konami's original source or project settings.
The pinned repository has no root license file. Adapted source retains its
upstream licensing status, separate from this project's MIT license.

| Unit | ROM range, end exclusive | C functions | Emitted bytes | Compiler |
|---|---|---:|---:|---|
| STWI command transport | `08243EFC–08244A38` | 48 | 2,876 | agbcc Thumb, O2, interworking |
| Serial interrupt | `08244A38–0824538C` | 7 | 2,388 | agbcc_arm, O2, interworking |
| RFU link API | `08245398–08248284` | 86 | 12,012 | agbcc Thumb, O2, interworking |
| Serial identification | `08248284–08248590` | 4 | 780 | agbcc Thumb, O2, interworking |

These four contiguous units contain **145 C functions and 18,056 emitted
bytes**. Fifty-four functions were already matched as individual files and
are consolidated here; **91 functions are newly matched**. Emitted sizes
include unit alignment. The progress report uses reviewed function spans,
which should not be confused with raw object symbol sizes.

## Evidence and adaptation

Relocation-masked signature searches anchored 124 Thumb functions and all
seven ARM functions uniquely. Contiguous object offsets supplied the remaining
short entries. Every adapted unit must then pass a resolved exact-byte check;
a masked signature by itself is insufficient, especially for tiny wrappers.
Function names in `symbols/proposed/rfu.csv` are backed by these source matches.
The compiled public labels remain `sub_XXXXXXXX`, using local source aliases.

Existing globals are declared, not allocated again. Literal relocations identify
`gSTWIStatus` at `03006A50`, four UNI slot pointers at `03006A60`, four NI slot
pointers at `03006A70`, link status at `03006A80`, static state at `03006A84`,
fixed state at `03006A88`, and serial-ID state at `03006A90`. SDK struct layouts,
volatile qualifiers, version branches and protocol constants are preserved in
`include/rfu_compat.h`. Static out-of-line helpers are externally visible to
retain their individual ROM boundaries. No machine instructions or register
bindings are used to force a match.

Existing read-only data is referenced by symbols instead of copied into new
objects: the two `LLSFStruct` records at `08602CD0`, version string at
`08602CF0`, `RFU-MBOOT` marker at `08602CFC`, four serial identification
halfwords at `08602D08`, and `Sio32ID_030820` at `08602D10`. The original source
constant blocks (54 and 23 bytes) were compared byte-for-byte before adapting
their declarations. Relocations preserve data shifting. `_call_via_r3` is an
alias of the existing `bx r3` helper at `08249244`.

The seven newly exposed Thumb starts are `082440C4`, `082441E4`, `08244250`,
`08244534`, `08244584`, `08245744` and `0824826C`. They are separately listed in
the progress inventory rather than credited only as part of a previous span.

## ARM code hidden in the disassembly

The disassembler left `08244A38–08245398` as a 2,400-byte binary blob after
`STWI_reset_ClockCounter`. Treating that blob as matched merely because its
preceding Thumb function is in C would overstate progress. It contains:

| Address | Function | Bytes |
|---|---|---:|
| `08244A38` | IntrSIO32 | 100 |
| `08244A9C` | sio32intr_clock_master | 656 |
| `08244D2C` | sio32intr_clock_slave | 1,036 |
| `08245138` | handshake_wait | 104 |
| `082451A0` | STWI_set_timer_in_RAM | 272 |
| `082452B0` | STWI_stop_timer_in_RAM | 80 |
| `08245300` | STWI_init_slave | 140 |
| `0824538C` | callback trampoline, bx r2 | 4 |
| `08245390` | callback trampoline, bx r1 | 4 |
| `08245394` | callback trampoline, bx r0 | 4 |

The first seven match ordinary C with the bundled `agbcc_arm`. The final three
remain assembly; upstream naked assembly definitions are deliberately omitted
from the C unit. The transport's interrupt pointer must retain the even ARM
address, unlike Thumb callbacks with bit 0 set.

`symbols/build_extra_functions.csv` records the reviewed entries and ISA.
The splitter applies these boundaries to binary blobs in memory, leaving shared
`gen/` untouched. It validates alignment and rejects duplicate or unlocated
boundaries. The progress inventory includes all ten, so the three retained
stubs receive no C credit. `// COMPILER: agbcc_arm` selects the bundled ARM
compiler and excludes the Thumb fork's unsupported `-fhex-asm` flag; the
compiler path, contents and flags remain part of the build cache identity.

## Validation and limits

Validation requires exact resolved unit bytes, a complete ROM SHA-1 match,
and the combined 64-KiB data-shift regression. Synthetic tests cover mixed-ISA
blob splitting, preservation of unmatched tails, invalid boundaries and compiler
flag selection. Existing emulator scenarios do not establish wireless-adapter
coverage: a future RFU-specific harness scenario would be useful behavioral
evidence, but it is not needed to establish these exact original bytes.
