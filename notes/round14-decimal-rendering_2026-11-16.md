# Round 14: decimal-rendering once-loop investigation

Tags: phase3, once-loop, decimal, hud, character-model, register-allocation

Base: `c5883ff7d72e754337895289628075d887ba37c2`, including the coordinator's
HUD include deduplication and prior character-model constant cleanup.

**No source change was adopted.** The three decimal-rendering once-loops remain
explicit holdouts. Direct deletion and eight ordinary formatting alternatives
were evaluated for each function; none reproduced the stock bytes.

| Function | Address | Function size | Complete unit independently checked |
|---|---|---:|---:|
| `PanelText_DrawDecimal` | `0810D60C` | 72 bytes | 72 bytes |
| `PanelText_DrawDigits` | `0811D270` | 72 bytes | 3,716 bytes |
| `Hud_DrawDecimalDigits` | `08165488` | 84 bytes | 5,840 bytes |

Each current source unit is byte-identical under stock agbcc. Removing only the
once-loop in each renderer keeps both the function and unit sizes, but changes
six bytes: the remaining numeric value and tile cursor exchange registers r5
and r6. This was verified independently with the stock compiler over each
complete unit, not inferred solely from scope's normalized score.

## Actual operation and source evidence

All three routines repeatedly divide the remaining number by the current decimal
place, emit the quotient as a tile, subtract `digit * divisor`, then divide the
place by ten. The two panel routines select glyph bases `0x11` or `0x21`; HUD
selects `0x21` or `0x31` and sets palette bits `0xF000`.

The clean `DrawDecimalDigits` inline helper in `src/ui/hud/hud_tilemaps.c`, used by
`SceneHud_DrawDecimal`, performs the same quotient/store/remainder recurrence
without a once-loop. `MenuDisplay_LoadDecimalDigits` in `src/ui/pause/menu_display.c`
uses the recurrence with indexed widget output, and `Hud_DrawStatRatio` in
`src/ui/hud/display/hud_stat_display.c` first collects the digits into arrays. Those siblings
motivated the structural candidates below.

The `Div` call targets the BIOS wrapper, distinct from the compiler's signed
division routine. Replacing the recurrence with a C `%` expression or another
remainder call would not preserve the existing pair of BIOS-division calls.
No SDK or existing shared macro was found that establishes the current
single-statement remainder wrapper as original source.

## Measured allocation cause

The canonical instrumented compiler reports these values after direct deletion:

| Renderer | Remaining value: weighted references / live instructions / priority | Tile cursor: weighted references / live instructions / priority | Diagnostic experiment |
|---|---|---|---|
| Both panel routines | 7 / 32 / 4,375 | 7 / 19 / 7,368 | `AGBCC_PRI=value:+2994` matches |
| HUD | 7 / 36 / 3,888 | 7 / 23 / 6,086 | `AGBCC_PRI=value:+2199` matches |

The cursor is allocated first and takes r5; the remaining value then gets r6.
The existing once-loop changes the reference weighting of the remaining value.
The diagnostic priority changes establish the allocation mechanism but are not
source repairs or stock acceptance. No compiler option or instrumentation change
was adopted.

## Ordinary candidates retained

All three graphs contain the same nine hypotheses plus the existing baseline:

| Candidate | Reason for testing | Observed limit |
|---|---|---|
| Direct recurrence | Remove the redundant loop | Six register-dependent bytes differ |
| Halfword glyph base | Use the tilemap entry's actual storage domain | Same-sized code, eight differing bytes |
| Indexed output cells | Follow the indexed numeric-display sibling | Register choices and cursor placement differ |
| Complete generic renderer | Factor the existing complete rendering operation, including palette attributes | Parameter copies increase code size |
| Complete font renderer | Omit the generic attribute parameter where the font fixes it | Parameter copies still increase code size |
| Read-only renderer base | Preserve the immutable base while the other formatting state advances | Same result as the font renderer |
| Combined quotient/remainder | Model the actual division result and remaining value together | The remaining-value arithmetic moves before tile output |
| Direct conditional glyph base | Select an immutable halfword base from style | Initialization and allocation differ |
| Cursor/place loop updates | Advance both real iteration quantities in the loop update | Cursor placement and register choices differ |

No alias-only local, new weighting loop, fabricated condition, or diagnostic
suppression was introduced. Moving the existing remainder statement into a
do-once macro would only relocate the warning without establishing the original
operation's source form, so it was not adopted.

## Durable evidence

[round14-decimal-candidates_2026-11-16.jsonl](evidence/round14-decimal-candidates_2026-11-16.jsonl)
contains 30 complete source states as independently applicable patches against
the base commit above. It records source hashes, hypotheses, and historical
function scores. All candidate patches were reconstructed and hash-verified.
It contains no generated assembly or RTL.

Full stock logs, complete candidate files, and the three portable matchsearch
histories remain under `build/round14/decimal/`. The live graphs are
`build/matchsearch/round14-decimal-*`; the allocation traces are under
`build/scope/PanelText_DrawDecimal`, `PanelText_DrawDigits`, and
`Hud_DrawDecimalDigits`. No shared header changed, and no full build or
whole-tree scan was run for this bounded investigation.
