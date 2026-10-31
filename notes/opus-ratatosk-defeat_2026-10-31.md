# Ratatosk_UpdateDefeatSequence (081F4CF8): exact match

Tags: #matching #ratatosk #compiler #reload #addressing

**Exact match in ordinary C.** `Ratatosk_UpdateDefeatSequence`, 1,064 bytes, now
compiles byte-identically (`tools/check.py src/fn/sub_081F4CF8.c`: MATCH, 0
differing bytes). It has no register pins, barriers, inline assembly or
compiler overrides. Commit `3c752a8b9` on `codex/opus-ratatosk-defeat`. The
change to the earlier draft is three lines. About 21 source variants were
compiled, all aimed at one diagnosed mechanism.

## The deciding source property

The spawn call takes a pointer to the spawn record that is set **once, before
the loop**. A separate per-iteration pointer only builds the offset position:

```c
if (((Mod(tick, 16) == 0) || (tick > 136)) && (tick <= 150)) {
    struct ContactPosition *spawnAt = &spawn;
    for (i = 0; i < 2; ++i) {
        struct ContactPosition *position = &spawn;
        ...
        spawn = p->body.position;
        position->x -= 128;
        position->y += 64;
        ...
        sub_081FC26C(&params, spawnAt);
    }
}
```

Three other spellings with the same property also match: the in-loop pointer
declared just before the struct copy, the copy written as
`*position = p->body.position`, or Y written as `spawn.y += 64`.

## Mechanism, pass by pass

1. **Thumb frame addresses.** `GO_IF_LEGITIMATE_ADDRESS` (thumb.h) rejects any
   sub-word address that mentions the frame pointer before reload. So every
   HImode frame access goes through a pseudo. cse1 replaces X's `(mem (reg
   position))` with the bare ADDRESSOF, because its address cost is special.
   `purge_addressof` then computes it in a fresh in-loop pseudo
   (`add r1, sp, #0x28`). This part was already right.
2. **The ROM's Y form is a reload rematerialization, not an ADDRESSOF form.**
   `mov r3, sp` with `[r3, #0x2a]` is what `eliminate_regs` (reload1.c)
   produces for `(plus R 2)` when R got no hard register and has
   `reg_equiv_constant = (plus fp 28)`. It becomes `(plus sp 42)`, and
   HImode cannot use an SP base, so reload copies sp into a low register.
   So the ROM's Y base is a pseudo with REG_EQUIV `(plus fp 28)` and no hard
   register. It is distinct from the call pointer, which the ROM spills to
   `[sp, #0x40]` and which therefore has no equivalence.
3. **Where the equivalence comes from.** cse's `insert` marks
   `FIXED_BASE_PLUS_P` values, `(plus fp k)` and ADDRESSOF among them, as
   `is_const`. cse therefore puts REG_EQUAL `(plus fp 28)` on a single set
   that *copies* a register known to hold that value. A set whose source is
   the expression itself gets no note. `update_equiv_regs` turns the note into
   REG_EQUIV (`function_invariant_p`) and doubles the live length.
4. **Why the old draft lost it.** gcse PRE turned the in-loop
   `position = &spawn` into a copy of a reaching register that it inserted in
   the preheader. loop.c hoisted the copy, and cse2 attached the note. Then
   combine folded the reaching register's set into the copy, since that
   register died there. `distribute_notes` keeps REG_EQUAL only for constants,
   so the note was dropped. `position` was spilled, and Y reloaded it from
   `[sp, #0x40]`.
5. **Why the new form works.** The in-loop pointer's set becomes a copy of
   `spawnAt`, which is available on loop entry, and loop.c hoists it. cse2
   attaches the note. Combine cannot fold it because `spawnAt` is still live
   for the call. The in-loop pointer keeps REG_EQUIV and gets no hard register:
   it lives across the loop and calls with a doubled live length. Reload then
   rematerializes Y. The lreg dump shows
   `(set (reg/v 298 position) (reg/v 296 spawnAt))` with REG_EQUIV
   `(plus r7 28)`. Pseudo 296 is the spilled call pointer, with no note.

This refines the earlier analyses. ADDRESSOF does decide X (the Codex
session's finding). GPT-6 Pro's `PLUS(ADDRESSOF, 2)` validity point is true,
but it is not the deciding layer: the ROM's Y never takes an ADDRESSOF form at
all.

## Ruled out in this session (scope counts, ROM / draft)

* Pointer set before the loop and used for X, Y and the call: 67 / 67.
* No in-loop pointer, with `spawn.x`/`spawn.y` direct and a pre-loop pointer
  for the call: 4 / 4. This gives the ROM's Y, but X's own `(plus fp 28)` is
  now the first in-loop occurrence. PRE and loop hoist it as well, and it is
  rematerialized too (`mov r3, sp` with `#0x28`).
* Call pointer assigned inside the body after the offsets (`= &spawn` or
  `= position`): 3 / 3. cse1 merges it back into `position`.
* Pre-loop pointer at function scope, initialized at entry or assigned before
  the loop: 19 / 19 and 39 / 41.
* Struct copy written through the pre-loop pointer: 14 / 16.
* Y-field pointers (`&spawn.y`, `&position->y`, in the body or before the
  loop): 3 / 3 to 25 / 27.

## Follow-ups

* **Sibling 081F5290 (knockback recovery).** Its ROM copies the offset pointer
  into a separate call-pointer register (`adds r5, r2, #0`) after the X/Y
  offsets. That suggests the same two-pointer source shape there.
* **scope gaps.**
  * `why 1443` (the reload of the spilled pointer) printed only registers. It
    could say "spilled to a stack slot, not rematerialized: no REG_EQUIV" and
    name the pass that dropped the REG_EQUAL note (combine's
    `distribute_notes`).
  * `insn UID sub_081F4CF8` printed nothing. It needs the display name
    `Ratatosk_UpdateDefeatSequence`.
  * `scope.py land` refused because the draft's helpers live inside the
    NONMATCHING block, and matchsearch `land` hit a three-way conflict against
    the fork's stale baseline. I landed it by hand: `land.unwrap`, then
    clang-format, then `tools/check.py`.
