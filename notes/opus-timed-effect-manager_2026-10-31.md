# TimedEffect_UpdateManager (081DA3D8): exact match

Tags: #matching #timed-effect #compiler #global-alloc #caller-save #cse #abi

**Exact match in ordinary C.** `TimedEffect_UpdateManager`, 580 bytes,
compiles byte-identically (`tools/check.py src/fn/sub_081DA3D8.c`: MATCH, 0
differing bytes). It has no register pins, barriers, inline assembly or compiler
overrides. Commit `aa41e52f8` on `codex/opus-ratatosk-defeat`. The unit's
guarded draft was at 86.5% (24 ROM / 22 draft instructions differing in scope).
About 25 variants were compiled. They are kept in `build/scope/drafts/te/` and
`build/scope/drafts/te3/` (worktree scratch, not committed).

## The deciding source property

`TimedEffect_UpdateTerrainTurn` takes three arguments, not four:

```c
savedVelocity = &velocity;
velocity = cursor->velocity;
status = TimedEffect_UpdateTerrainTurn(manager, entry, i);
if (status <= 0) {
    ...
    } else if (config->type != 9) {
        sprite->position.x -= savedVelocity->x;
        rollbackPosition->y -= savedVelocity->y;
        rollbackPosition->z -= savedVelocity->z;
    }
}
```

The ROM has `r3 = sp` (`&velocity`) at the call, so every earlier draft passed
the velocity pointer as a fourth argument. It isn't one. The callee (081DA1DC)
reads only r1: it overwrites r0, r2 and r3 before reading them. r3 holds
`savedVelocity`, a pseudo that is live across the call and is saved and
restored by caller-save:

```
mov  r3, sp            @ savedVelocity = &velocity
ldr  r0, [r4, #0x10]   @ velocity = cursor->velocity
...
ldr  r2, [sp, #0xc]    @ i, the third argument
str  r3, [sp, #0x14]   @ caller-save
bl   TimedEffect_UpdateTerrainTurn
adds r7, r0, #0
ldr  r3, [sp, #0x14]   @ restore at the end of the block
```

The callee's matched unit declares two parameters, `(void *ctx, struct TEntry
*e)`. The caller's file-local prototype now has three. The original likely had
an unused third parameter (the index). The local `TimedEffect_TurnSnapshot`
helper, which existed only to pass the pointer, is gone.

The always-true collider test (`movs r2, #0; cmp r2, #0; bne`) is a zero local
at the top of the `else` branch: `if (config->type != 9 && !zero)`. The same
`zero` is the last argument of `Collider_SetPosition`. A literal 0 there
matches too.

## Mechanism

1. **A fourth argument leaves the pointer without a low register.** The
   four-argument drafts failed in one of two ways:
   * In the unit's draft and z1, the pointer goes through the inline
     helper, and cse replaces it by the frame pointer in every use. The
     result has no pointer register and no caller-save slot. The frame is
     0x14 bytes, and the y/z rollback uses `mov rN, sp`.
   * When the pointer survives to the rollback (z2), cse2 replaces it only
     in the r3 move (`(set (reg:SI 3) (reg:SI 7))`), because the frame
     pointer heads its equivalence class in that block. The pointer is still
     live there. In global allocation, `global_conflicts` makes a hard
     register set by any insn conflict with every allocno live at that insn
     (`mark_reg_store` → `record_one_conflict`). The argument moves set r0 to
     r3, so the pointer conflicts with all four.
2. **Allocation order.** `savedVelocity` (6 refs over 27 insns, 1 call,
   priority 4444) is allocated before `tail` (15 refs over 210, 6 calls,
   2142). Its preferred class is LO_REGS, and r4 to r7 are taken (cursor,
   rollbackPosition, sprite, status). The first `find_reg` pass excludes
   call-clobbered registers. Caller-save is profitable (`4 * 1 < 6`,
   regs.h), so `find_reg` retries with them (global.c, `flag_caller_saves`).
   * In z2, r0 to r3 all conflict. The retry fails, the alternate class
     gives the pointer sl, and `tail` spills to the stack: `str r0, [sp,
     #0xc]` in place of `mov sl, r0`.
   * With three, r3 is free. The retry takes it (F record: used1 `1fff7`,
     best 3), and `caller_save_needed` adds the slot at sp+0x14. The frame
     is 0x18 bytes.
3. **The zero survives every pass that could fold it.** It is set at the
   top of the `else` branch. The test comes after the `status != 1` block,
   whose nested ifs and calls end cse1's path. gcse's constant propagation
   only rewrites an insn if the result is still valid, and `(set (cc0)
   (const_int 0))` isn't. Its cc0-jump case is not implemented ("Note this
   does not currently handle machines which use cc0"). The zero gets no hard
   register (priority 449 over 2 calls), so reload loads the constant at the
   test. reload_cse then drops the argument's `movs r2, #0`, because r2
   already holds 0. Declared right before the test, cse1 folds it (c5: 9 / 7
   differing).

## Ruled out (this session)

Numbers are scope's ROM / draft differing instructions.

| Variant | Differing |
|---|---|
| The unit's draft (four arguments) | 24 / 22 |
| Function-scope `zero` with four arguments (z1) | 15 / 15 |
| z1 with the pointer set explicitly (z2) | 33 / 37 |
| z2 with `AGBCC_PRI=28:+2200` (diagnostic) | 21 / 23 |
| z2 with `AGBCC_PRI=28:+2200 AGBCC_FORCE=148:3` (diagnostic) | 5 / 3 |

The 5 / 3 run had no caller-save, and `mov r3, sp` was the argument move.

The three-argument call matches with the copy written either way:
`*savedVelocity = cursor->velocity` (z3a) or `velocity = cursor->velocity`
(z3b). These differ:

* The copy before the pointer (z3c, 1 / 1: order only).
* The pointer as a declaration initializer (d1, 38 / 36).
* `rollbackPosition` as a declaration initializer (d3, 72 / 72).

For the rollback, `sprite->position.x` with all three components through
`savedVelocity` matches (landed). Other spellings:

| Rollback spelling | Differing |
|---|---|
| `rollbackPosition->x` (s1) | 50 / 50 |
| `velocity.x` through `rollbackPosition` (s3) | 50 / 50 |
| `position` in place of `rollbackPosition` (s4) | 72 / 72 |
| `position` for all three (s5) | 91 / 91 |
| `velocity.y/z` (s6) | 15 / 13 |

For the zero test, these all match:

* `zero == 0`;
* u32;
* function scope, before or after `i`;
* the top of the active-entry block;
* a `result` local returned at the end.

The earlier round's `u8` helper returning `type == 9` (g1) gives 16 / 16.

Earlier rounds: `notes/finish-effects_2026-10-30.md` and
`build/handoff/codex-evidence/effects/scope/campaign/sub_081DA3D8/REPORT.md`
(snapshot output helpers, priority knobs, the guard helper). All of them kept
four arguments.

## Scope gaps

* **Argument count.** Nothing checks a call's argument count against the
  callee. For each `bl`, `scope.py rom` could list the argument registers
  the callee reads before writing them. Here that is r1 alone, against four
  in the draft. It could also flag a caller-save pair: a `str rN, [sp, #K]`
  just before the `bl` (N < 4), and `ldr rN, [sp, #K]` after it. Such an rN
  holds a pseudo, not an argument.
* **Allocation chains.** `why all` on z2 has no causal summary for 9 of its
  17 mechanisms, among them `tail`'s spill (insn 778) and the pointer's set
  (424). The chain is in the F and G records:
  1. `tail` lost sl to the pointer (pointer priority 4285 against 2142).
  2. The pointer reached the alternate class because both of its `find_reg`
     calls found nothing (used1 `1ffff`).
  3. r0 to r3 were in its conflicts because the argument moves set them while
     it was live.

  `why` could follow a spilled pseudo to the allocno that took its
  register, and name the insns behind that allocno's hard-register
  conflicts.
