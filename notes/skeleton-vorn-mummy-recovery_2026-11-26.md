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

## Resumed pass

- Skeleton's idle, approach, target, and sink paths use the shared control
  and state transitions. The near and far distance checks and drawable
  writes use the class fields. The 31,956-byte unit matches. Testing
  native spark positions with earlier random-table assignments still
  left the stack-slot mismatch in Skeleton_UpdateDefeat (080BD1F0).
  The direct-index neighbor loop (080B713C) kept the extent and changed
  33 bytes, with the actor and mask lifetimes still unresolved.
- Vorn's saved-target callback uses the shared chase group and player
  tracking. The defeat spread is a typed position in the same packed
  storage. Vorn's main unit reads the native class timers and uses the
  shared phase transition. Vorn_UpdateDefeat (080AEE08) still differs
  in 17 bytes with the proved-zero spread adjustment removed.
  In Vorn_UpdateActionCompletion the flat tracking read changed three
  instructions by folding the group address at actor + 0xDC into the
  byte read at actor + 0xEB.
- Mummy_UpdateDying (080D203C) uses the prepared latch, named motion
  field, and display sequence type. With those edits in place, the
  signed timer read through the chase group also matches. Mummy's
  hidden recovery uses a literal default mirror value. Revival
  (080D1EA0) uses the shared health and collider reset. Burn damage
  (080D812C) uses the defeat callback and shares the two-drawable
  palette and flash timer update. The contact callback writes its
  response bit directly, and the route replay uses the typed body.
  The shared burn death-state reset expands its zero argument before
  the state field address. The ROM loads zero after forming that
  address and before storing the state byte. In Mummy_OnAttackCollision
  (080D0C30), CSE reuses the direct response address for both accesses,
  putting it in r7. The ROM uses r0 for the read and r1 for the store.
  SceneActor_EnterMotionPhase (080D7438) keeps the tracking base at
  actor + 0xDC for its final flag write. The direct access folds it with
  the byte offset and loads the flag before building the mask.

| Class | Linter report lines after the resumed pass | Steering uses after the resumed pass |
|---|---:|---:|
| Skeleton | 251 | 310 |
| Vorn | 226 | 236 |
| Mummy | 174 | 176 |

The resumed pass removed 23 report lines and 37 steering uses. Across
both passes, the three directories went from 743 to 651 report lines
(12.4% lower) and from 844 to 722 steering uses (14.5% lower).
The seven units touched on resumption matched their recorded extents.
The scoped linter reported zero errors.
