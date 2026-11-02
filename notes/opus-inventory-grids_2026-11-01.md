# SceneInventory_Populate{Equipment,Item,Consumable}Grid: item grid matched pin-free; a goto outer loop is needed

Tags: #loop-reduction #goto #reload-rotation #inventory #spilled-locals

**PopulateItemGrid (0805FF38) matches with no pins** (commit 3f28b7dd2,
`src/fn/sub_0805FF38.c`). The consumable grid is a holdout: gcse moves two
row-end increments above the column loop (README fact 50). The equipment grid
is below. Best pin-free drafts:

| Unit | Best pin-free draft | Differing (ROM / draft) | Shape |
|---|---|---|---|
| PopulateItemGrid 0805FF38 | **MATCH** (plain spilled locals, `slot` as the hide-loop counter, goto outer loop) | 0 | 0 |
| PopulateConsumableGrid 080600DC | goto outer loop, plain locals (25 / 25 with `AGBCC_NOPRE=1,2`) | 67 / 67 | 68 |
| PopulateEquipmentGrid 0805FD20 | `do` / `while` loops with goto exits | 72 / 76 | 68 |

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
* A label plus `if (++row <= 3) goto primaryRow;` outer loop has no loop notes,
  so loop.c leaves it alone. The multiply then comes out per row as in the
  ROM. Consumable grid: 67 / 67 that way against 101 / 105 with `do`/`for`.
  Item grid: 53 / 57 against 58 / 62.
* `rowBytes` (+0x180 per row) has to be explicit for the same reason.
* In the consumable grid, `slotBase` (+4) and `saveByteOffset` (+8) have to
  be explicit too.

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

## Consumable grid (080600DC): holdout, gcse moves two row-end increments above the column loop

**Not depinned.** The best pin-free draft is still the goto outer loop with explicit row
counters (below): 67 / 67, shape 68. What stops it is now known.

* gcse's PRE moves `saveByteOffset += 8` and `slotBase += 4` from the end of each row to the
  end of the block before the column loop, into two new pseudos, and the row-end adds become
  copies. The frame grows to 0x1c (0x18 in the ROM). Why: README fact 50 (the old LCM stops
  delaying at a loop header). `scope.py pre` shows both as "moved above a loop".
* With both kept (`AGBCC_NOPRE=1,2`, a list since commit d29d55a9c) the draft is 25 / 25, and
  every remaining difference is one swap: `panel` in sb and `cursor` in r8, where the ROM has
  `panel` in r8 and `cursor` in sb. `AGBCC_NOPRE=2` alone gives 37 / 40.
* `rowBytes += 0x180` is not moved (0x180 is loaded into a register first, so the add is not
  anticipatable), and `row++` is not moved (the second section computes `row + 1` again, so the
  row-end computation is isolated).
* No C found that keeps the two adds at the row end in the goto form. Fact 50's conditions need
  an increment Thumb cannot encode inline, the same expression again on every later path, or
  the operand set in the column loop or in the block before it. The ROM shows none of these.
* The other reading: the ROM's three row-end adds are loop.c's own giv steps. They sit right
  before `row++`, where loop.c puts giv steps, and their slots come after `nextRow`'s, which
  fits compiler-made pseudos (it also fits the declaration order). That means a real outer loop with the counters derived from `row` (`slot = row * 4 +
  column`, `&panel->display.primary[row * 4]`, `gGameState->table160[slot]`). loop.c runs
  after gcse, so nothing moves them. But loop.c then also reduces the entry offset
  `(row * 4 + 5) * 0x60`, which the ROM recomputes each row from `slotBase`: adjusted benefit
  9, threshold 15 (`(3 + 12)`, a loop with a call), life 1 is 135 against the loop's 75 to 119
  insns. Drafts this way (`build/scope/drafts/cg/h*.c`): per-row pointers 116 / 112 to 125 /
  122; per-column indexing (`widget = &panel->display.primary[slot]`, `entry =
  &flagsBase[slot + 5]`) 62 / 59. There every per-column init is reduced again in the outer
  loop's second pass (x4, x8, x12, x384 givs).
* Earlier findings still hold. The function-start stores take r0, r1, r2 and r3, then r0 again
  for `rowBytes = 0` (reload reuses the register that still holds 0). So these are spilled
  pseudos, not memory: in memory, CSE would store one zero register everywhere, as it did in
  the item grid's struct draft. The
  hidden-flags loop counts 4..7 upward with a goto exit (fact 40).
* For a stronger model: find why the ROM's loop.c leaves the entry offset alone (or which C
  makes it a non-giv while `row * 4`, `row * 8` and `row * 0x180` are still reduced). Or find
  a goto-form spelling whose row-end adds gcse leaves in place. Then fix the panel / cursor
  priority order (in the item grid, panel got r8 once the rest matched).

Best pin-free draft (67 / 67; 25 / 25 under `AGBCC_NOPRE=1,2`):

```c
#include "scene_inventory_panel.h"
void SceneInventory_PopulateConsumableGrid(struct SceneInventoryPanel *panel)
{
    s32 row;
    struct SceneInventoryWidgetFlags *flagsBase;
    s32 nextRow;
    u32 saveByteOffset;
    u32 slotBase;
    u32 rowBytes;
    u32 cursor;
    s32 column;
    s32 slot;
    s32 kind;
    Anim_SelectWidgetGlyph(&panel->display.heading, &panel->display.widgetResource, 0x66);
    row = 0;
    flagsBase = ((struct SceneInventoryFlagsView *)(void *)panel)->widgets;
    saveByteOffset = 0;
    slotBase = 0;
    rowBytes = 0;
primaryRow: {
        struct SceneInputWidget *widget;
        struct SceneInventoryWidgetFlags *entry;
        column = 0;
        {
            struct SceneInputWidget *base = panel->display.primary;
            widget = (void *)(rowBytes + (u32)base);
        }
        cursor = saveByteOffset;
        entry = (void *)((slotBase + 5) * 0x60 + (u32)flagsBase);
        do {
            slot = slotBase + column;
            kind = ((struct GameState *)((u8 *)gGameState + cursor))->table160[0];
            if (kind >= 0) {
                entry->flags &= ~1;
                Anim_LoadKeyframe(widget, &panel->display.gridResource, (u16)(kind + 289), 1);
            } else
                entry->flags |= 1;
            if (!gUnk_08E60F80[kind].baseValue || GameState_IsConsumableSlotEquipped(slot))
                SceneHud_SetKeyframePaletteBank(widget, 0x374);
            else
                SceneHud_SetKeyframePaletteBank(widget, 0x373);
            widget++;
            cursor += 2;
            entry++;
            column++;
        } while (column <= 3);
        saveByteOffset += 8;
        slotBase += 4;
        rowBytes += 0x180;
        row++;
        if (row <= 3)
            goto primaryRow;
    }
    for (row = 0; row <= 1;) {
        struct SceneInputWidget *widget;
        struct SceneInventoryWidgetFlags *entry;
        u32 flagIndex;
        u32 offset;
        column = 0;
        cursor = row * 4;
        nextRow = row + 1;
        flagIndex = cursor + 21;
        offset = ((row << 3) + cursor) << 5;
        {
            struct SceneInputWidget *base = panel->display.secondary;
            widget = (void *)(offset + (u32)base);
        }
        entry = (void *)(flagIndex * 0x60 + (u32)flagsBase);
        do {
            slot = cursor + column;
            kind = panel->catalog[slot + panel->page * 8];
            if (kind >= 0) {
                entry->flags &= ~1;
                Anim_LoadKeyframe(widget, &panel->display.gridResource, (u16)(kind + 289), 1);
                if (gGameState->unk7B0 < gUnk_08E60F80[kind].baseValue)
                    SceneHud_SetKeyframePaletteBank(widget, 0x374);
                else
                    SceneHud_SetKeyframePaletteBank(widget, 0x373);
            } else
                entry->flags |= 1;
            entry++;
            widget++;
            column++;
        } while (column <= 3);
        row = nextRow;
    }
    {
        struct ContactPosition *position = &panel->display.digits[0].position;
        struct SceneInventoryWidgetFlags *entry = &flagsBase[41];
        const s16 *party = gGameState->party;
        for (slot = 0; slot < 4; slot++) {
            s32 packed = *party;
            if (packed < 0)
                entry->flags |= 1;
            else {
                entry->flags &= ~1;
                position->x = (packed & 3) * 24 + 112;
                position->y = (packed >> 2) * 24 + 32;
            }
            position = (void *)((u8 *)position + 0x60);
            entry++;
            party++;
        }
    }
    slot = 4;
    {
        u32 mask = 1;
        struct SceneInventoryWidgetFlags *hidden = &flagsBase[45];
        do {
            hidden->flags |= mask;
            hidden++;
            slot++;
            if (slot > 7)
                goto hiddenDone;
        } while (1);
    hiddenDone:;
    }
}
```

## Equipment grid notes

* The `do`/`while` form (goto inner exits) is 72 / 76. The outer loop
  reduces the entry offset, and column and rowSlot swap sl and the stack.
* The goto outer loop is 99 / 101: panel gets r8 and row gets sb, where the
  ROM has the reverse.
* Not tried there yet: the item grid's base temporaries and per-section
  variables.
