# Round 14: discard zero proofs from rejected combine attempts

Tags: round14, tooling, concessions, proved-zero

Date: 2026-11-16. Source snapshot: `f10b8f9eb`.

## Defect and three independent false positives

`AGBCC_DEAD` reported zero-valued expressions immediately from `simplify_rtx`.
That function also runs while `try_combine` explores transformations that it
later rejects. Narrow volatile loads provided three concrete counterexamples:

| Source | Operation | Why the report was false |
|---|---|---|
| `src/link/serial/sub_082227D8.c:14` | Serial control register masked with `0x40` | The selected hardware bit can be set. |
| `src/lib/eeprom/Eeprom_WriteBlock.c:78,96` | EEPROM input masked with `1` | The sampled input bit can be either value. |
| `src/lib/rtc/Rtc_ReadDataByte.c:17` | RTC input masked with `2`, then shifted right once | This extracts a real input bit. |

In each case the diagnostic observed an intermediate zero in a proposed
narrow-load transformation. The transformation was rejected; the source
operation was not proved zero. No source-pattern or hardware exemption is
needed to correct this.

## Correction

Buffer zero events for the current `try_combine` attempt. Emit them only at its
two successful exits, and clear the buffer when the next attempt starts. The
existing source-operator filter and unreachable-effect diagnostic are unchanged.
No new warning kind or suppression is introduced. The change is confined to
the `combine.c` section of `tools/scope/agbcc.patch`.

This corrects rejected-attempt reports. It does not claim that every surviving
compiler intermediate necessarily describes an artificial source expression;
the remaining reports still need source review.

## Counts and validation

On the same source tree, the complete linter changes from **3 errors / 2,052
warnings** to **3 errors / 2,043 warnings**. Only `proved-zero` changes, **44 to
35**; `dead-code` remains 13. These nine corrections are diagnostic changes,
not nine source cleanups. No new compiler sites appear.

The removed sites are:

- `src/link/wireless/connection/WirelessController_BuildPeerRosterPacket.c:47`
- `src/objects/floor_switch/FloorSwitch_SetOccupant.c:32`
- `src/bosses/hresvelgr/Hresvelgr_UpdateCollisionPresentation.c:137`
- `src/link/serial/sub_082227D8.c:14`
- `src/lib/eeprom/Eeprom_WriteBlock.c:78,96`
- `src/lib/rtc/Rtc_ReadDataByte.c:17`
- `src/monsters/ivy/ivy.c:1197`
- `src/monsters/vorn/vorn_main.c:2680`

All 32 concession tests pass. Added regressions cover all three hardware
counterexamples and two genuinely zero expressions involving volatile reads;
the latter remain reported. Existing impossible-guard and known-zero-pointer
tests also remain positive.

With all diagnostic environment variables absent, **all 5,927 source units
using normal agbcc emit identical assembly before and after this change**, with
their real per-unit flags. The other 114 units select the unchanged old/ARM
compiler variants. No stock compiler source or production flags changed.

Ignored reproduction artifacts are under `build/round14/`: the before/after
summaries, fixture results, whole-tree site difference, and
`zero-no-env-parity.json`. The full combined ROM check is recorded in the batch
delivery report.
