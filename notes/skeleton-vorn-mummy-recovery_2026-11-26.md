# Skeleton, Vorn, and Mummy class recovery

Tags: #matching #types #phase3 #compiler

The worker pass covers Vorn (080ACBE0–080B551C), Skeleton
(080B551C–080C47B8), and Mummy (080D0C30–080DDD28).
All 13 translation units match their recorded ROM extents. All six
dependents of the two changed headers match.

| Class | Linter report lines before and after | Steering uses before and after |
|---|---:|---:|
| Skeleton | 311 → 268 | 402 → 341 |
| Vorn | 245 → 229 | 255 → 239 |
| Mummy | 187 → 177 | 187 → 179 |

The pass removed 69 report lines (9.3%) and 85 steering uses (10.1%).
The three directories have zero linter errors. Exact code coverage in
these units is 100%, a change of 0 percentage points.

## Recovered class fields and operations

- Skeleton's pursuit and revival code uses the existing class fields
  and chase group. Skeleton_BeginStationaryPursuit (080B57C0) reads the
  gate, destination, and cooldown from their owners.
  Skeleton_UpdateVisibilityFlicker (080B5BEC) advances the class's frame
  counter at 0x748. The action cooldown occupies the halfword at 0x704.
  The approach attack setup reads the actor's four parallel attack
  parameter arrays and updates the external attack collider. This makes
  Skeleton_SetAttackColliderProperties a tentative name for the current
  Vorn_SpawnHazard symbol.
- Vorn's forward attack uses a ContactCollider at 0x628, initialized by
  SceneActor_InitContextColliders (080B1D8C). The random-reaction timer
  and period at 0x690 and 0x692 are set together by Vorn_InitActionCooldown
  (080B1158). The saved-state restore (080AD270) reads the chase actor's
  saved control and the two drawable layouts. Item launch and the
  position script share the class's queued response fields.
- Mummy_UpdateScriptedAction (080D48F4) loads the gate duration and writes
  the gate counter. Mummy_UpdateMovementTimers (080D80E8) decrements the
  named timers through the Mummy class. Stimulus pursuit uses the shared
  chase gate, control, home route, and target position.
- Shared operations replace repeated control installation, availability
  checks, and typed tracking predicates. Skeleton uses the combined
  engine/scene query from engine_pause.h. Plain direct bit operations
  were retained at the sites where stock agbcc matched the full unit.

## Remaining matching decisions

- Skeleton_UpdateDefeat (080BD1F0) still uses two dead branches, two
  local unions, and a proved-zero song expression. Removing both dead
  branches kept the length but changed 28 instructions on each side.
  CSE merged the equivalent spark-position addresses across the
  branches. The draft used sl for the merged address where the ROM
  loads a stack slot at sp+0x98.
- SceneActorTerrain_FindOpenNeighborDirection (080B713C) still initializes
  its mask in a single-pass loop. A plain assignment changed the global
  allocation of the actor from r8 to r7 and moved the mask to a high
  register that needs reloading.
- Vorn_UpdateDefeat (080AEE08) still has three proved-zero expressions
  among the spark arguments. With plain arguments, the origin evicts
  the spark kind from r5 and the work value moves from r4 to r6.
  Adjusting reload rotation in scope reduced the diff from 115/118 to
  84/85 instructions. The underlying allocation still needs recovery.
- Mummy_UpdateDying (080D203C) matched with a direct status-field test.
  The fully typed timer read folded away the separate group address.
  Scope interpreted the register comparison as a choice for the actor.
  The ROM actually holds the actor in r4 and the group base in r7.
  The status-field-only variant preserves both and clears the warning.

## Type scanner

The stock scanner reported parser failures in eleven of twelve
candidate units before the fixes. A macro-expanded GNU statement
expression followed by a shift was handled at the assignment-expression
entry point, so the parser returned before consuming the shift. Reading
the compound expression as a primary operand also handles its
appearance on the right of a binary operator. The scanner now parses
all twelve candidates and reports 39 embedded-type observations in the
final source. The focused type-family suite passes 25 tests, including
the compound-expression regression.
