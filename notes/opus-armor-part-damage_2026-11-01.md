# Armor_OnPartDamageCollision (0818C6F8): matched in ordinary C

Tags: #matching #armor #twin #random-table #cross-jump

**Matched.** `Armor_OnPartDamageCollision` (636 bytes) is byte-identical in
ordinary C. It has no register pins, barriers, selector macros or compiler
overrides. The new C matched on its first compile.

## What decided it

The function is a twin of the matched Sword collider callback `sub_080F4708`
(`src/fn/sub_080F4708.c`; `scope.py like` coverage 0.55). The draft takes that
function's form:

* the same part layout as a struct, as `struct ArmorPart`. The Armor part has
  a single `callback` at 0x1D0 where the Sword component has three, and a u16
  `timer` at 0x1D6;
* the same inline helpers: the contact-flag mask, the broken-state
  `SetPartState(part, 1, 5)`, the displacement written as three plain s16
  differences, and `GetPhase`, which reads the phase before it takes the
  phase's address;
* a `direction` store in each branch. Cross-jumping merges the three stores
  into the ROM's single store;
* the random sample read through `((const u16 *)RANDOM_TABLE_ADDR)[*counter]`.

The Armor-specific parts:

* the recoil test is phase 2 with any callback other than
  Armor_DampPartWaveReturn, or one of five motion callbacks whatever the phase;
* a `switch` over the phase: the ROM's compare tree is 3, then >3, then 2;
* no PlayerStats call. The health test after the subtraction therefore reuses
  the stored value (`lsls r0, r0, #0x10; cmp r0, #0`).

## Ruled out

Each variant is the match with one change. Numbers are scope's ROM / draft
differing instructions.

| Variant | Change | Differing |
|---|---|---|
| v_y | the y difference through an `int` temporary, as the old draft had it | 46 / 57 (`ldrsh` loads) |
| v_angle | one `direction` store, after the branches, through a local | 23 / 21 (shape 4) |
| v_tbl | `gRandomTable[*counter]` | 10 / 16 |
| v_mode | `phase = &part->phase; snapshot = *phase;` without the helper | 29 / 29 (shape 2) |
| v_goto | the broken state jumps to the else branch's 0x13E sound | match, the same code |

The old draft (fuzzy 83.4%, 30 / 31) did its arithmetic through byte
pointers and had all four of the first four changes.
