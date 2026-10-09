# Push20 Astra math escalation

Owns only `sub_0822A230` on `codex/push20-astra-math`, with inherited accepted
context through `2842afd`. Inherited Sol source is `wip/sub_0822A230.c` and its
prior comparison is `wip/math-prior.diff`.

Ten fresh manual source checks were performed after rebuilding the private
baseline ELF to obtain the new comparison aliases. The initial attempted
checks could not link `__ltdf2`; they were rerun after the build printed
`build/boktai3.gba: OK`.

Best new safe candidate: **`wip/math_direct_final.c`**, 1,400 emitted bytes,
20 differing instructions. It removes all redundant `rise=s->d[3]`
assignments and computes `x=(date+s->d[3])-0.671262` directly. This fixes the
mode-2 add/subtract branch merging, mode-3 reload paths, and all branch and
literal placement differences. The prior 1,396-byte candidate's branch and
frame concerns are now reduced to two allocation issues:

- 16 instructions swap the rise and transit stack slots: original rise uses
  `sp+0x68`, transit uses `sp+0x70`; this candidate has them reversed.
- Four instructions at `0822A6D4`, `0822A6D6`, `0822A6E8`, `0822A6EA` spill
  and reload the final `x` through `sp+0x38` instead of holding it in r4/r5.

The direct field read is safe: `s->d[3]` is assigned before all mode branches.
No new calls, altered pointer lifetimes, uninitialized values, asm, register
bindings, volatile accesses, artificial conditions, data or shared-header
changes were introduced.

Other tests covered removing only the final reload, explicit mode-2 reloads,
a mode-2 early goto to the common finish, nested mode-2 branches, a mode-3 sum
local, a mode-3 early goto, both early gotos, an early rise reload, and a
separate final-rise block local. All full sources and diffs are ignored in
`wip/`; `wip/math_direct_final.diff` records the best comparison.

A fresh bounded 60-second single-worker permuter search used the improved
candidate to explore stack/temporary lifetimes:
`build/permute/sub_0822A230/run-y_g7grx9`, base score 928.
Search ended at its limit with `no candidate output saved`. No exact match,
zero accepted bytes, no C source commit. The improved safe candidate is
retained for a later explicitly budgeted pass. Only this note is committed.
