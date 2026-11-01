# Storage_RefreshEquipmentSlots (081BE504): matched without pins

Tags: #matching #loop-reversal #goto #debug-g #addressof #twin

**Matched.** `Storage_RefreshEquipmentSlots` (556 bytes) is byte-identical in
ordinary C under the project's build. It has no register pins, barriers,
selector macros or compiler overrides.

## What decided it

### The loops leave through a goto

In the ROM, two loops keep a counter that nothing else reads: the carried rows
(`row`, [sp+4]) and the stored columns (`col`, [sp]). Both count up
(`adds #1; cmp #3; ble`). In the draft, loop.c reversed both
(`subs #1; cmp #0; bge`), because nothing stopped `check_dbra_loop`.

`check_dbra_loop` and `loop_iterations` only work on a loop whose last insn
before `NOTE_INSN_LOOP_END` is the conditional jump. The loop form decides
what sits there:

* `do { ... } while (col <= 3);`: the test is the last insn, so the loop is
  reversed.
* `do { ...; if (col > 3) break; } while (1);`: a break is a jump to the
  loop's end label. agbcc's `expand_end_loop` (the Cygnus "loop test" code in
  stmt.c) finds it as an exit test and rotates the loop, moving everything
  from the start label through the break to the bottom. After jump
  optimization the inverted test sits right before `LOOP_END`, so the loop is
  reversed.
* `if (col > 3) goto done; } while (1); done:`: a goto to a user label is not
  an exit test to `expand_end_loop`, so the loop is not rotated. Jump
  optimization threads the test to the loop start, and the `NOTE_INSN_LOOP_CONT`
  note stays between the test and `LOOP_END`. loop.c reports "No final
  conditional branch found" and leaves the counter alone.

The goto form is the only spelling found that matches without `-g`.

### `-g` does the same for a break on its own line

`-g` changes this function's code. With `-g`, the break form matches too, when
`break;` is on its own line. The break statement's line note then sits between
the test and `LOOP_END`. With `if (col > 3) break;` on one line, `emit_note`
drops the repeated line and the loop is reversed again.

The original may have been built with `-g`, which would make the plain break
form the original spelling. Evidence:

* Compiling all 10,094 matched units with and without `-g` (plus
  `-ffix-debug-line`, which 35 of them need to assemble) gives identical
  `.text` for every one of them.
* The twin `Storage_RefreshConsumableSlots` (081BE2E0) matches with its two
  `asm("" : : "r"(...))` barriers removed under `-g`, or with goto exits under
  either build.
* With its pins removed, the twin `Storage_RefreshItemSlots` (081BE16C) under
  `-g` with break exits keeps its column loops unreversed. Register choices
  still differ there.

`tools/check.py` has no `-g` mode, and the project builds without `-g`, so the
unit uses the goto form. That form compiles to the same code either way.

### The other two properties

* **Slot offsets start through `Storage_ClearOffset(&x)`**, the helper of the
  matched twin 081BE2E0. Taking the address keeps `rowSlot` and `markerBytes`
  behind ADDRESSOF through cse1. With a plain `rowSlot = 0`, gcse's PRE
  hoists `rowSlot + 4` to the top of the row loop into a new stack temporary,
  and the frame grows to 0x24.
* **One function-scope `id`** serves both sections. Its single pseudo
  conflicts with r2 in the carried section and gets r3 everywhere. That gives
  the stored section's `adds r3, r0, #0` and the later `adds r2, r3, #0` for
  the `(u16)(id + 240)` argument.

The explicit offset locals (`hideFlags`, `cellBase`, `widgetOffset`,
`markerBase`, `markerOffset`, `rowBytes`, `markerBytes`) are carried over from
the earlier draft, which had already matched their registers and stack slots.

## Ruled out

Each variant is the match with one change. Numbers are scope's ROM / draft
differing instructions.

| Variant | Change | Differing |
|---|---|---|
| dowhile | `} while (col <= 3);` and `} while (row <= 3);` exits | 74 / 78 (loops reversed) |
| break | `if (col > 3)` / `break;` exits, project build | 74 / 78; MATCH with `-g` |
| oneline | break exits written `if (col > 3) break;` on one line, with `-g` | check.py: 91 insns (reversed) |
| rowmul | `rowSlot = row * 4` each row instead of `rowSlot += 4` | 19 / 19 |
| plaininit | `rowSlot = 0; markerBytes = 0;` | 8 / 12 (PRE temporary, frame 0x24) |
| plainrow | only `rowSlot = 0` plain | 8 / 12 |
| plainmarker | only `markerBytes = 0` plain | 9 / 9 |
| localid | `s32 id` declared in each loop | 3 / 3 (stored `id` in r2) |
| natural | `p->carried[row][col]` and `p->stored[row][col]`, no offset locals | 130 / 144 |

The `markerStart` local of the old draft (an offset reset at the end of every
column) is not needed: `ShowWidget(&markerBase->markers[0])` and the offset
written in place match too.
