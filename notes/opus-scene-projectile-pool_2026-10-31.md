# SceneProjectile_UpdatePool (081FCA44): exact match

Tags: #matching #scene-projectile #ratatosk #compiler #loop #giv #inline #jump2 #cross-jump

**Exact match in ordinary C.** `SceneProjectile_UpdatePool`, 1268 bytes,
compiles byte-identically (`tools/check.py src/fn/sub_081FCA44.c`: MATCH, 0
differing bytes). It has no register pins, barriers, inline assembly or compiler
overrides. Commit `b35a08d2e` on `codex/opus-ratatosk-defeat`. The unit's
guarded draft was at 74.5% (158 ROM / 140 draft instructions differing in
scope). Round 8's best, `build/handoff/frontiers/ratatosk/branch-suffix/`, was
at 81.2% (118 / 102). 33 drafts were compiled. They are kept in
`build/scope/drafts/pj/` (worktree scratch, not committed).

## What the ROM shows

The loop latch (func+0x4BE):

```
ldr  r2, [sp, #4]      @ i++
adds r2, #1
str  r2, [sp, #4]
movs r3, #0xe0
lsls r3, r3, #1        @ 0x1C0, the slot size
add  sb, r3            @ four reduced givs
ldr  r0, [sp, #0x18]
adds r0, r0, r3
str  r0, [sp, #0x18]
ldr  r1, [sp, #0x1c]
adds r1, r1, r3
str  r1, [sp, #0x1c]
adds r7, r7, r3
add  sl, r3            @ the induction variable
cmp  r2, #9
```

loop.c inserts a reduced giv's increment right before the increment of its
BIV (`insert_before = tv->insn`; thumb has no auto-increment). So sl, the last
one, is the BIV, and the four givs belong to it. sl is the slot pointer (pool
+ 0x20). The givs are slot + 0 (sb, which serves the age and the
`control->update` read at +0x38), the launch (+0x170), the control direction
(+0x2E) and the sprite (r7, +0x9C).

The frame holds pool (sp+0), `i` (sp+4), then geometry, collider,
animationSprite and terrainPosition (sp+8 to sp+0x14), then the two spilled
givs. terrainPosition is computed from sl at the top of every iteration
(`mov r3, sl; adds r3, #0xb8; str r3, [sp, #0x14]`), not reduced.

## The deciding source properties

1. **One slot pointer.** Every slot access goes through `p`:
   `for (p = pool->slots, i = 0; i <= 9; i++, p++)`. The unit's draft stepped
   a second pointer `cursor` beside `p`, and later drafts used
   `&pool->slots[i]`. Neither gives the ROM's latch.
2. **The animation helper copies its object pointer.**
   `StepAnimation(p, animationSprite)` starts with
   `struct ProjectileSprite *sprite = obj;` and reads the flags through
   `state = &cursor->sequence`. That is how `DisplaySequence_Step`
   (`include/display_sequence_inline.h`) is written. The shared helper can't
   be used here: it expects the sequence and the frame fields in one object,
   and here the sequence is in the slot and the frame in its sprite.
3. **The first release is an if/else.**
   `if (result == 1) SceneProjectile_ReleaseSlot(pool, p, i); else { ... }`
   wraps the rest of the active slot. The homing release
   (`release: SceneProjectile_ReleaseSlot(pool, p, i); continue;`) has the same
   tail. jump2 cross-jumps the first copy into the second and keeps the later
   one: the ROM's `cmp r0, #1; bne; b func+0x358`.
4. **terrainPosition is set twice.** It is set at the top of the loop body
   and again at the end. The second store is dead, and flow deletes it after
   loop.c has run.
5. **Smaller spellings.**
   * Function-scope declarations, in the frame's order.
   * Compound assignments for the position update.
   * Mask locals (`u32 mask = ~2; p->flags &= mask;`).
   * An s32 temporary for the aimed direction:
     `aim = direction + ((random & 15) - 8);`.
   * `p->control.collisionMode` read through `p`.

## Mechanism

1. **An inline parameter that is never assigned aliases the caller's
   variable.** integrate.c marks such a parameter TREE_READONLY. When the
   argument is a user variable's register, it maps the parameter straight to
   that pseudo. Without the copy, all of StepAnimation's sprite uses count as
   refs of `animationSprite`. It then outranks `p` for sl in global allocation.
   With the copy, the helper uses its own pseudo, and `p` gets sl (f6 101 / 117
   to f7 52 / 54).
2. **Giv benefit decides the pass.** A giv addressed through a DEST_REG giv
   (`control = &p->control`, `state = &cursor->sequence`) has benefit 2. A
   plain p-relative address has benefit 0. `control->update` and `state->flags`
   combine into the slot + 0 giv. With both, its benefit is 4, and
   strength_reduce's test passes in loop pass 1: lifetime 18 × threshold 15 ×
   benefit 4 = 1080, against an insn count of 553 (`AGBCC_LOOP_TRACE=1`: insn
   1587, `benefit=4 lifetime=18 reduce=1`). With only one of them it is 540,
   which fails. The giv is then reduced in pass 2 (`-frerun-loop-opt`), and its
   init and increment come after pass 1's (f13a, 32 / 30, which still read
   `cursor->sequence.flags`).
3. **A register set twice is not a giv.** loop.c records an induction
   variable only for a register with one set in the loop (`n_times_set`). The
   trace shows terrainPosition with `may_not=1 set_in_loop=2`. Without the
   second store, terrainPosition is a DEST_REG giv. CellIndex's x and z loads
   through it become address givs, and they combine into the sprite giv (r7).
   `AGBCC_NOGIV=929,936` on those two loads took f9b from 88 / 86 to 58 / 58.
   With the second store, the loads stay on the spilled pointer, as in the
   ROM.
4. **The else label limits derivation.** "If this giv is conditionally set and
   we have passed a label, it cannot derive anything" (strength_reduce).
   `control` is set inside `if (active)`, so after the else label nothing
   derives from it. That changes which givs are combined and reduced. On its
   own the if/else was worse (f12a, 199 / 197), because the slot + 0 giv was
   no longer reduced. Forcing it with `AGBCC_GIV=1589` (diagnostic) gave
   32 / 30. `control->update` gave the same in ordinary C (f13a), and
   `state->flags` moved the reduction into pass 1 (point 2).
5. **Spill slots follow pseudo order.** Parameters come first, then locals in
   declaration order, then the gcse and loop pseudos. The four function-scope
   pointers give the frame's sp+8 to sp+0x14 (f9a).
6. **Operand order.** combine folds the first operand's single-use load into
   the add, and reload loads it right before the add. `+=` makes the field the
   first operand, as the ROM has it. A store of the sum straight into the s8 direction narrows the
   arithmetic to QImode (`0xF8` in place of -8; f17b). An s32 temporary keeps
   it in SImode.

## Ruled out (this session)

Numbers are scope's ROM / draft differing instructions. Each row changes the
row above it unless it says otherwise. Not every step that the match needed
was better on its own (f8, f12).

| Draft | Change | Differing |
|---|---|---|
| The unit's draft | two stepping pointers | 158 / 140 |
| f6 | `cursor = &pool->slots[i]` | 101 / 117 |
| f7 | StepAnimation copies its object pointer | 52 / 54 |
| f8a, f8b | all slot accesses through `p` | 93 / 91 |
| f9a | function-scope declarations | 90 / 88 |
| f9b | `p->control.collisionMode` | 88 / 86 |
| f10a | `for` loop, release label inside the homing block | 85 / 81 |
| f10b | three release copies | 207 / 201 |
| f11a, f11b | terrainPosition's second store | 49 / 45 |
| f12a | if/else around the first release | 199 / 197 |
| f12b | f12a without the second store | 206 / 200 |
| f13a | `control->update` | 32 / 30 |
| f14a | `state->flags` | 22 / 20 |
| f15a | compound assignments for x, y and z | 4 / 4 |
| f15b | a compound assignment for x only | 21 / 21 |
| f16a | the sprite copy first in StepAnimation, mask locals, the direction as its own statement | 5 / 5 |
| f17a | `direction += (random & 15) - 8` | 26 / 26 |
| f17b | the sum stored straight into the s8 field | 5 / 5 |
| f18a | an s32 `aim` temporary | MATCH |
| f18b | `random` reused for the sum | 3 / 3 |
| f19a | f18a without the second store | 41 / 35 |
| f19b | the second store at the end of the loop body (landed) | MATCH |
| f20b, f20c | `position` merged into terrainPosition, with or without the first store | 88 / 82 |

Earlier rounds: `notes/finish-effects_2026-10-30.md` (a private
`AGBCC_BIV_CONST=448` knob for the in-loop stride),
`build/handoff/frontiers/projectile-pool/README.md` (79.8%) and
`build/handoff/frontiers/ratatosk/branch-suffix/README.md` (81.2%). All of them
kept an i-based or second slot pointer.

## Scope gaps

* **Giv decisions are invisible without the trace.** `why` doesn't say that a
  ROM register is a reduced giv, or why a draft's giv was reduced in another
  pass. `AGBCC_LOOP_TRACE=1` prints benefit, lifetime and the result per giv.
  scope could also print the test (lifetime × threshold × benefit against the
  insn count) and the pass, and flag a giv whose product is just below the
  bar, as 540 against 553 was here.
* **The BIV can be read from the latch.** A reduced giv's increment sits
  right before its BIV's. `scope.py rom` could mark the latch's increments and
  name the BIV (the last one) and the givs that belong to it. Here that showed
  that the induction variable is the slot pointer, not a second cursor.
* **Readonly inline parameters merge refs.** The G records show
  `animationSprite`'s refs but not that most come from an inlined body through
  a readonly parameter. `why` could name the inline and suggest a local copy.
* **Cross-jumped tails.** A ROM branch into another path's identical tail
  (`b func+0x358`) suggests a duplicate that jump2 merged. `why` could suggest
  writing the copy (as in `notes/opus-resolve-damage_2026-10-31.md`).
* **`land` refuses helper changes.** This unit was landed by hand (draft
  copied, clang-format, check.py), because StepAnimation changed.
