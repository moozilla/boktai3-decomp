# MGS/GCL matching round 1

Worker: Sol 6.1, isolated checkout `match-luna-pilot`, branch
`codex/mgs-gcl-round1`, base `68348386352893ad51ac2fec1d02fd95b77cbaaa`.
Assigned starts: `08219A00–0821B9FF`, `08225000–082258FF`.
Existing previous worker branch/WIP was preserved; no push or PR requested.

Detailed evidence and all new findings: `docs/MGS_GCL_MATCHING.md`.
No semantic symbol-map edits or copied external source.

| Target | Checks to MATCH | Bytes | Main matching issue resolved |
|---|---:|---:|---|
| `0821AF2C` | 2 | 136 | Explicit normal-stop assignment matches null-path branch |
| `0821B738` | 1 | 92 | Condition/block goto structure reproduces B3 selector |
| `0821B7B0` | 1 | 92 | Sequential option checks and shared execution |
| `0821A66C` | 4 | 84 | Signed switch and separately evaluated high byte |
| `08225560` | 4 | 48 | Ordinary callback pointer, explicit result branches |
| `0821B8AC` | 1 | 96 | 16-bit count bitfield preserves adjacent high half |
| `0821AD2C` | 2 | 108 | High-byte temporary and increment before count init |
| `0821B858` | 1 | 84 | Retain string scan and 512-byte stack buffer |
| `082257E4` | 2 | 40 | Return registration result for correct epilogue |
| `08225844` | 3 | 64 | Equality-positive return branch order |
| `0821B6C8` | 2 | 112 | Explicit source local before destination local |
| `0821A6F0` | 2 | 420 | Distinct OR operand order prevents merging cases 6 and 8 |
| `0821B2B0` | 1 | 156 | Big-endian descriptor and two auxiliary operands |
| `0821B478` | 1 | 80 | Descriptor copy and low-half auxiliary stores |

Total: 14 new functions, 1,612 native bytes.

Deferred candidates in untracked `wip/`: `0821AA1C` (three checks),
`0821AA50` (four checks). Initial drafts differed only in the order of
stack-address setup / vector-register assignment / counter initialization.
An explicit pointer local changed stack allocation and worsened the draft.
No permuter run was needed for successful targets.
The `0821B20C` typed variable-load helper remains WIP after five source
variants; boolean lowering and extra saved registers in shared-result variants
remain off. One check attempted while a full build rewrote its symbol ELF
failed to read that file; the check was rerun after the build completed.
Checks and full builds must be sequential within this checkout.
No other failed matching candidate was copied to `src/`.

Validation: every added function passed `tools/check.py` against the ROM.
The complete fourteen-unit build printed `build/boktai3.gba: OK`.
`PATH="$PWD/build/venv/bin:$PATH" tools/shift_test.sh
tests/newgame_intro.txt 0x10000` passed all 16 screenshot comparisons.
The initial shift attempt found the worker checkout missing its harness binary;
a read-only symlink to the existing root built harness fixed this.
Per parent instruction, saved-game scenario shifts are left to combined
batch integration validation; no additional verification was started.
