# SceneActorReaction_ResolveDamage (08135F70): exact match

Tags: #matching #scene-actors #damage #compiler #reload #jump2 #cross-jump

**Exact match in ordinary C.** `SceneActorReaction_ResolveDamage`, 1156 bytes,
compiles byte-identically (`tools/check.py src/fn/sub_08135F70.c`: MATCH, 0
differing bytes). It has no register pins, barriers, inline assembly or compiler
overrides. Commit `7174089b8` on `codex/opus-ratatosk-defeat`. The unit's
guarded draft was at 96.4%. GPT-6's round 8 frontier
(`build/handoff/frontiers/damage-and-orbit/sub_08135F70.c`) was 5 bytes away,
and `AGBCC_SPILL=2:1` already matched it as a diagnostic. About 40 variants were
compiled, kept in `build/scope/drafts/rd/` (worktree scratch, not committed).

## The deciding source property

The first branch (no pending hit, area damage applied) writes the `0x2000`
clear in both places it applies:

```c
u16 *borrowed = &actor->flags2D4;
u32 flags = *borrowed;
flags2D4 = borrowed;
if (!flags) {
    u32 *flags2CC = &actor->flags2CC;
    u32 mask = ~0x2000;
    *flags2CC &= mask;
} else {
    u32 down = IsDown(actor, flags);
    if (down) {
        ...                       /* *flags2D4 = 0x40 and the rest */
    } else {
        u32 mask = 0x803E;
        flags = *flags2D4;
        if (mask & flags) {
            ...                   /* the hit-flag updates */
        } else {
            u32 *flags2CC = &actor->flags2CC;
            u32 mask = ~0x2000;
            *flags2CC &= mask;
        }
    }
}
SceneActor_ClearReactionFlags(actor);
return 1;
```

The ROM has only one copy of the clear (at func+0x12C). The `if (!flags)`
copy is gone, and `beq func+0x12C` jumps to the other one.

## Mechanism

1. **Reload's spill rotation.** The spill registers are r0, r1, r2, r4 and r5.
   `allocate_reload_reg` takes the next free one after the last register it
   chose. AGBCC_DUMPPRI writes each choice to `names.txt` (`R` and `X`
   records). For the frontier:
   * #0, the 0x2E6 offset in HasPendingHit: r1;
   * #1, the 0x2D4 offset: r2;
   * #2, the 0x2D0 offset in IsDown: r4;
   * #3, the scratch of the signed health load: r5.

   The ROM has r1, r2, r5 and r1. The next choice is the r8 reload for
   `*flags2D4 = 0x40`. r0 and r1 are busy there, so both rotations give it r2.
   That is why shifting only choice #2 matches everything.
2. **The ROM made one choice that it doesn't show.** The early clear's 0x2CC
   offset is a reload. It comes after the 0x2D4 load and before IsDown, takes
   r4, and moves the 0x2D0 offset to r5 and the scratch to r1. The landed
   unit's log shows it as choice #2 (insn 68, `=716`, r4).
3. **jump2 removes the copy.** Cross-jumping runs only in the jump pass after
   reload (`toplev.c`, the one `jump_optimize` call with `JUMP_CROSS_JUMP`).
   After reload, both copies are `r4 = 0x2CC; r2 = r6 + r4; r1 = ~0x2000; ...`
   and both reach the ClearReactionFlags call. `find_cross_jump` replaces the
   earlier copy with a jump to the later one, and `bne; b` becomes the ROM's
   `beq func+0x12C`.
4. **Stream order matters.** Reload goes through the insns in order, so the
   extra copy must sit between the 0x2D4 load and the IsDown query. That
   requires `if (!flags) clear; else ...`. Inside, the hit block has to come
   before the second clear (`if (mask & flags) hit; else clear;`). With
   `if (!(mask & flags)) clear; else hit;`, that clear is emitted first and 38
   instructions differ (e1, e2).

The late instance (`UpdateHitFlagsShared`, after RecordDefeat) keeps flags2CC
in r7, so its clear needs no reload. Written once or twice, it makes the same
code.

## Ruled out

The README's list for this function and the round 8 reports in
`build/handoff/matchsearch/resume3_hud_08135F70/REPORT.md` cover the earlier
attempts. Added in this session:

* IsDown shapes at the first call, all still 5/5: u8, u16 or s32 `down`; an
  early `if (!actor) return 0`; const parameters; an s16 health local;
  `health < 1`. Worse: a u32 flags parameter (22 differ), the `||` form or
  plain ifs without mask locals (7 differ), and IsDownAtHealth with
  `&actor->health` (99 differ).
* The shared `UpdateHitFlagsShared` call in the first branch (36 differ).
* Plain `actor->health` reads in the helper or the HUD path (7 to 11 differ).
* An early copy that also calls ClearReactionFlags and returns (d2, 13
  differ).
* Plain stores for the clears or the down block (f2, f3: 5 differ each).
* The statement macros without `do`/`while (0)` (113 or 20 differ), and
  inline functions in place of `DAMAGE_MARK_HIT` (12 to 15 differ).

## Scope gaps

* `scope.py spill` names the span with the missing choice. Its only suggestion
  is a value moved to a high register, which adds a reload that the code shows.
  It could also point to reloads the final code doesn't show: code that jump2
  deletes after reload (a duplicate that cross-jumping merges, or an arm that
  combine made unreachable).
* The choice log in `names.txt` is the most direct view of the rotation, but
  scope doesn't print it, and `scope.py FILE.c AGBCC_DUMPPRI=1` crashes
  (`dict() got multiple values for keyword argument 'AGBCC_DUMPPRI'`), because
  scope sets that variable itself. A `spill --log` listing (choice, insn, C
  line, value, register, the occupancy at that point) would help.
