# Golem_UpdateChargePursuit (080CCD7C): matched in ordinary C

Tags: #matching #golem #pre #stack-slots #gcse #integrate #cross-jump #reload

**Matched.** `Golem_UpdateChargePursuit` (4504 bytes) is byte-identical in
ordinary C (commit `3b21dc135`). It has no register pins, barriers, statement
macros, selector macros or compiler overrides. The old draft's asm barrier,
its `do { } while (0)` macros and the unit's register-pinning marker are gone.
The drafts are in `build/scope/drafts/charge/` (`f0.c` is the frontier as a
unit; `TA1.c` is the first match).

## What decided it

1. **The three turn-away copies are one inline, and the commit is another.**
   `Golem_TurnAway(body)` is called from the three places that turn more than
   31 off the charge heading. It ends both outcomes with
   `Golem_SetBehavior(body, 3 or 2)` (`behavior2AA`, `counter2AC = 0`,
   `field2B1 = 1`). The final code does not change, but gcse's input shrinks
   from 1902 to 1869 insns. The reason is in integrate.c: `try_constants`
   substitutes known-constant registers into every insn of an inline copy, so
   a field at a large offset is one `(plus reg const)` insn there. The same
   store in the function body loads the offset into a register first, which
   costs two insns. PRE's table goes from T 951 to T 935. At these pseudo
   numbers the ROM's slot order needs T 927 to 939 (N 1852 to 1879), and
   `scope.py FILE.c preorder` reported exactly that. Before the fix, one pair
   was out of order: `actor#66`+712 (bucket 841) before the flags pointer
   `actor`+724 (bucket 163, base r22, the parameter, which no renumbering
   moves).
2. **Earlier fixes, from 145/158 differing insns down to SP offsets only.** These
   were made before item 1:
   * the flags pointer comes from the shared `SceneActorChase_MotionFlagsOf`,
     which gives the ROM's PRE pseudo (frame 0x68);
   * the 0x80 test uses `SceneActorChase_MotionFlags`;
   * `SceneActorChase_SelectState` + `ResetStateEntry` put the zeros where the
     ROM has them;
   * one `distance` variable is reused;
   * the copies are written stop-first, which gives the ROM's reload rotation
     (0x2B2 in r7, then `subs r7, #2`);
   * `u32 mask = 0x10; actor->flags2CC |= mask;` loads the constant before the
     address.
3. **Cross-jumping fixes the copies' source shape.** In the ROM, copies 1 and 2
   keep separate stop blocks with identical registers, and all three share one
   resume block and one commit tail. That layout needs each stop to end in a
   `return`. jump2 then merges each stop's commit into copy 3's resume tail at
   a label it creates itself, and a new label gets no jump chain. If a stop
   ends in `goto commit`, the source label's chain lets the stop blocks match
   each other, and they merge (the h variants). The helper keeps the `return`s,
   so the layout is the same.

## Ruled out

* `goto commit` from the stops and resumes (the 31 combinations with at least
  one, `h/`). Each saves 9 insns, but every combination changed the code
  (4308 to 4720 bytes).
* `goto resume` into copy 3: from copy 2 alone (`g2`), copy 2's stop block
  changes registers; from copies 1 and 2 (`g3`), the two copies merge whole.
* One shared commit per copy with if/else (`g1`): 4596 bytes.
* `Golem_TurnGap(body)` reading the two fields itself (`TG1`-`TG3`): N 1878,
  but the allocation changes, and the near-miss `Golem_Recover` no longer
  cross-jumps into the `recover:` copy.
* The frontier's `do { } while (0)` macros. They are barriers, and each costs
  two insns at gcse.
* The 080C8774 Golem chase-step variants (`v1`, `v2`). They give the same code
  with N -3 and -1, which is not enough, and the match does not need them.
* Small rewrites: `Wrap` with returns, `IsDazzled` without the mask local, no
  `held` or `flag` locals, `u32` TurnGap parameters, no `target` locals. Either
  cse1 folds them and N is unchanged, or they change the code.
