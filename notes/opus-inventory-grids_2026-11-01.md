# SceneInventory_Populate{Equipment,Item,Consumable}Grid: all three grids matched pin-free

Tags: #loop-reduction #goto #reload-rotation #inventory #spilled-locals #giv #incremented-index #live-length

**PopulateItemGrid (0805FF38) matches with no pins** (commit 3f28b7dd2,
`src/fn/sub_0805FF38.c`). **PopulateEquipmentGrid (0805FD20) matches with no
pins and real loops** (commit 40eb582e0, `src/fn/sub_0805FD20.c`): the flags
are indexed with a variable that the column loop increments, so the outer loop
cannot reduce the per-row flag offset (equipment section below).
**PopulateConsumableGrid (080600DC) matches with no pins** (commit 8a8f207ee,
`src/fn/sub_080600DC.c`): real loops with `flagIndex = slot + 5`, and two
no-op `(s16)` narrowings of `kind` that lengthen flow's live ranges in the
column loop (consumable section below).

| Unit | Pin-free form | Differing (ROM / draft) | Shape |
|---|---|---|---|
| PopulateItemGrid 0805FF38 | **MATCH** (plain spilled locals, `slot` as the hide-loop counter, goto outer loop) | 0 | 0 |
| PopulateConsumableGrid 080600DC | **MATCH** (real loops, `flagIndex = slot + 5`, `(s16)kind` twice, goto hide loop) | 0 | 0 |
| PopulateEquipmentGrid 0805FD20 | **MATCH** (real loops, `flagsBase[flagIndex]` with `flagIndex++`, `u16 packed`) | 0 | 0 |

## What decided the item grid

* `nextRow`, `flagsBase` and `rowBytes` are plain locals that reload spills,
  declared in that order: spill slots follow pseudo order, so they land at
  sp+0, sp+4 and sp+8 as in the ROM. Any other declaration order swaps slots
  (6 to 10 differing). In memory (a local struct or `volatile`), CSE stores
  `row`'s register for `rowBytes = 0` (`str r7`) where the ROM loads a fresh
  zero. That was the pinned unit's `register u32 zero asm("r1")`.
* The closing hide loop counts with the function's `slot`
  (`for (slot = 0; slot < 8; slot++)`), not a counter of its own. A fresh
  `i` is set first from a constant, so update_equiv_regs doubles its live
  length (fact 20). It came out at priority 7777 and lost r1 to the entry
  pointer (27000). `slot`'s first set is `rowSlot + column`, so there is no
  doubling, and its refs from all three sections add up (17 refs over 16
  insns, 42500). This one change took the struct draft from 26 to 7
  differing, and with it the plain-locals form matches outright. The
  earlier plain-locals draft, with `i`, was 34 differing, its reload
  rotation one step off at `column = 0`; `scope.py spill` had already said
  that rotation alone did not explain it.
* The second section's loop can be `for (row = 0; row <= 1; row = nextRow)`
  and the first `if (++row <= 3) goto primaryRow;` with the label inside the
  section's block. Both are the same RTL.
* Still required: per-section column variables (one shared `column` is 15
  differing) and the base temporaries below.

## Why a goto outer loop: the first section's outer loop is invisible to loop.c

In all three ROMs, the first section recomputes the flag-entry offset
`(rowSlot + 5) * 0x60` (`(slotBase + 5) * 0x60` in the consumable grid) from
the row on every row. As C this is `entry = (void *)((rowSlot + 5) * 0x60 +
(u32)flagsBase)`.

* With a `do`/`while`/`for` outer loop, loop.c strength-reduces that multiply
  into a new induction variable: a stack slot that steps by 0x180, an extra
  frame word (`sub sp, #0x14` for 0x10). The only exception is when the giv
  chain is cut, which is what the pinned units do with `register s32 rowSlot
  asm("r9")` (a hard register is never a giv) or an `asm("" : "+r"(...))`.
* Writing the index in the inner loop (`flagsBase[slot + 5]`) does not help.
  The rerun of loop (-frerun-loop-opt) sees the init insns that the inner
  loop's reduction emitted as ordinary pseudos, and reduces them in the
  outer loop. You can see it in `u.i.loop`'s second pass.
* An index that the column loop increments does help, and it allows real
  loops: `flagIndex = rowSlot + 5;` per row, `flagsBase[flagIndex].flags` and
  `flagIndex++` per column. `flagIndex` is set twice inside the outer loop,
  so neither it nor the chain built on it is a giv there, in either pass.
  This is what matched the equipment grid (section below).
* A label plus `if (++row <= 3) goto primaryRow;` outer loop has no loop notes,
  so loop.c leaves it alone. The multiply then comes out per row as in the
  ROM. Item grid: 53 / 57 that way against 58 / 62 with `do`/`for`. (The
  consumable grid was 67 / 67 against 101 / 105, until real loops with
  `flagIndex = slot + 5` matched it; consumable section below.)
* `rowBytes` (+0x180 per row) has to be explicit for the same reason.
* In the consumable grid's goto form, `slotBase` (+4) and `saveByteOffset`
  (+8) have to be explicit too, and gcse then moves them (fact 50). With
  real loops, loop.c makes all three steps itself.

## Smaller properties found on the item grid

* Compute the member base before adding the row offset:
  `struct SceneInputWidget *base = panel->display.primary; widget = (void *)(rowBytes + (u32)base);`.
  Folded into one expression, it becomes `(rowBytes + 0x288) + panel`.
  The same holds for `display.secondary` in the second section.
* Both a per-section `column` and a per-section `rowSlot` give rowSlot sb and
  column sl, as in the ROM. With one shared pair, column comes first in
  global-alloc's priority order and takes sb.
* For the closing hide loop, `u32 mask = 1;` declared before `entry` puts
  `movs r3, #1` first, as in the ROM.

## Consumable grid (080600DC): matched pin-free with real loops and two no-op s16 narrowings

**Matches with no pins** (commit 8a8f207ee, `src/fn/sub_080600DC.c`). The register pins, the
asm barriers and the concession marker are gone. What it needed:

* Real `for` loops in the first section (`for (; row <= 3; row++)` around `for (column = 0;
  column <= 3; column++)`), with every per-column address derived from `slot = rowSlot +
  column`. loop.c (the outer loop's second pass) then makes the three row-end steps itself
  (+8 for the table byte offset, +4 for the slot base, +0x180 for the widget offset), in ROM
  order. gcse runs before loop.c, so it never sees them; in the goto form it moved two of them
  (fact 50).
* `flagIndex = slot + 5` with `flagsBase[flagIndex]`. flagIndex is a DEST_REG giv reduced in
  pass 1. The 0x60 chain on it is a giv only in pass 2, which emits the ROM's `(slotBase + 5)
  * 0x60 + flagsBase` after the widget and cursor inits.
* The table read is `((struct GameState *)((u8 *)gGameState + slot * 2))->table160[0]`
  (Inventory_GetConsumableId's address order). `row = 0` comes before `flagsBase`. The closing
  block loads `gGameState` into `state` first. The hidden-flags loop stays a goto loop: as a
  real loop its depth weight gives `slot` 26 refs and r6, where the ROM has the widget pointer
  in r6 and `slot` in r7.
* The deciding property: `(s16)kind >= 0` and `gUnk_08E60F80[(s16)kind]`. Both narrowings are
  no-ops (kind comes from a sign-extending halfword load). cse keeps them and combine folds
  them, so they are two extra insns in the column loop at flow time and none in the output.
  Each adds 2 to the live length of everything live there, which settles global's two close
  calls (priority = floor_log2(refs) * refs / live):
  * panel (21 refs) against the loop's table cursor (11 refs): 157 / 60 insns gives 5350
    against 5500, and the cursor took r8. 161 / 64 gives 5217 against 5156: panel r8, cursor
    sb, as in the ROM.
  * column (28 refs, live doubled for a constant first set) against row (18 refs): 262 / 168
    gives 4274 against 4285. 270 / 176 gives 4148 against 4090: column sl and row spilled, as
    in the ROM.
* One narrowing is not enough (panel 5283, cursor 5322). Any two work: an `s16` temp copied
  into kind (`s16 value = ...; kind = value;`) counts as one, and so does `s16 id = kind;`
  before the lookup. The unit uses the two casts, which add no variables.
* How it was found: the draft without casts matched under `AGBCC_PRI=22:+200,23:-100`, and
  `scope.py ... life panel` / `life PSEUDO` give the per-line live counts that the extra insns
  have to change.

Ruled out (drafts in `build/scope/drafts/cg/` and `build/scope/drafts/cv/`):

* With no extra insns, all 69 / 71 with the same priorities: either operand order for `slot`,
  no `rowSlot`, `< 4` bounds, `do`/`while` outer or inner loops, an `s32` cursor, `slot * 2 +
  (u32)gGameState`, the widget computed first, `rowSlot` set in the column loop, a raw `*(s16
  *)` read, `== 0` for `!`, `rowSlot` shared with the second section, widget addresses written
  inline, flagIndex computed before slot.
* `s16 kind` for the whole function: 69 / 73 (the second section changes).
* An `s16` temp or cast where cse or promotion absorbs it: `u16 frame = kind + 289`, `s16 base`
  for baseValue, `(s16)` on baseValue, `(s16)kind` inside the then-arm. No change.
* `GameState_IsConsumableSlotEquipped((u8)slot)`: the right priorities, but its truncation is
  real (1 / 3).
* Inline accessors: a `static inline s16` read gives one copy (the same as the temp). An
  inline definition lookup emits the shift before the table address (36 / 36).
* The hidden-flags loop as `for`, `do`/`while`, with a new counter or indexed: 23 / 23 or
  53 / 53.

## Equipment grid (0805FD20): matched pin-free with real loops

**Matches with no pins** (commit 40eb582e0). Both row sections are
`do`/`while` (first) and `for (row = 0; row <= 1; row = nextRow)` (second)
loops. Both index the flags as `flagsBase[flagIndex].flags` with `flagIndex =
rowSlot + 5` (or `+ 21`) per row and `flagIndex++` per column. The closing
loop's `packed` is a `u16` local. The unit's volatiles, register pins, asm
barriers and concession marker are all gone.

* The flag pointer that the ROM steps by 0x60 in the column loop is loop.c's
  reduction of `flagsBase[flagIndex]` (a DEST_ADDR giv of the biv
  `flagIndex`). In pass 1 the column loop goes first. Its setup goes in just
  before the column loop as `flagIndex * 0x60 + flagsBase`, which is
  emit_iv_add_mult's shift-add form, exactly the ROM's `(rowSlot + 5)`, `* 3`,
  `<< 5`, `+ flagsBase`. It uses new pseudos, which the outer loop cannot take
  as givs in pass 1. In pass 2, `flagIndex` is set twice in the outer loop (the
  per-row set and the column loop's `++`), so it is not a giv. So the chain on
  it is not a giv either, and nothing reduces it. Flow later deletes the
  `flagIndex++`, since nothing but the increment itself reads it.
* With an explicit `entry` pointer (`entry = (void *)((rowSlot + 5) * 0x60 +
  (u32)flagsBase)`, `entry++`), the outer loop reduces the chain. Its last
  giv has benefit 11, life 1 and threshold 15, which gives 165 against a
  60-insn loop. This was the e5 draft: 52 / 54 with no knob, 7 / 7 with
  `AGBCC_NOGIV=59,58,57,54`. No condition in loop.c skips it: rowSlot can
  derive (no label between it and the chain), and `row` is a plain biv.
* The real loops are what the allocation needs. Their depth weights give the
  first section's flag pointer r4, slot r7 and widget r6 as in the ROM. The
  goto outer loop was 84 / 82 with everything else equal (e2).
* Closing loop: `u16 packed = *materials;` in place of `u32 packed = *(const
  u16 *)materials;`. This adds two insns where materials is live and packed
  is not. The materials pointer's live length goes from 44 to 46 (11 refs),
  so its priority drops from 7500 to 7173, below packed's 7272 (4 refs over
  11 insns). packed then takes r3 and materials r4, as in the ROM. The pinned
  unit got the same two insns from a volatile flagsBase load plus an asm
  barrier.
* The second section matches with either the flagIndex form or an explicit
  `entry` pointer, because `row = nextRow` keeps `row` from being a biv
  there. The unit uses flagIndex in both sections. The closing loop needs
  the explicit pointer: `flagIndex = 41` there is 1 / 1.

Ruled out (drafts in `build/scope/drafts/eg/`):

* `&panel->display.primary[rowSlot]` without `rowBytes` (w2): loop.c folds
  0x288 into the reduced register, 6 / 4.
* `&panel->display.secondary[rowSlot]` in the second section (w4): 51 / 51.
* A goto closing loop with an explicit countdown (g1): 12 / 12. The depth-1
  weights drop packed further.
* With `u32 packed`: `(s16)packed < 0`, `% 4`, constant-first sums and a
  `(s16)` cast on y all stay at 7 / 7. `/ 4` for `>> 2` is 3 / 7.
* Earlier: `do`/`while` with goto inner exits 72 / 76; goto outer loop 99 /
  101; indexed `widget` and `entry` together (e8) 53 / 55;
  `flagsBase[slot + 5]` recomputed per column (e10) 54 / 58.
