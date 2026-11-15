# TargetRegistry_FindById (0815F5B4): holdout

Two concession-free drafts in `TargetRegistry_FindById.c` (use the original's types and includes).

* Draft B (`goto actor` / `goto compare`, one shared compare): layout matches; id and
  sceneActor are r4/r3 instead of r3/r4. sceneActor (5 refs, its `movs #0x10` tied in by
  regmove) outranks id (3 refs): `AGBCC_PRI=id:+2484` -> MATCH. scope's flip: id needs 5 refs.
* Draft A (each arm compares the id and runs the found code; g1/g4 forms): id reaches 5
  refs and every register matches. The two arms' tails (cmp id; bne loop; cmp disabled;
  bne exit; return entry) are identical. jump2 redirects the collider arm (first in insn
  order) into the actor arm's copy; the ROM kept the collider's copy and the out-of-line
  actor arm is `ldr; ldr; b 0x24`.
* Tried: if/else arms in both orders, the found code behind a label with `goto found`
  from the actor arm (the branch is inverted to `beq found; b loop` because the loop test
  follows; the id and disabled registers swap), `for (;;)`/`while (1)` (the loop shape changes),
  and ternary / entryId forms (no invariant hoist).
* Expected construct: per-arm compares where the actor arm's tail reaches jump2 as
  `cmp; bne loop; b found` without being inverted, so that its jump is the one redirected.
