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

## Further pass

| Class | Report lines before and after | Steering uses before and after |
|---|---:|---:|
| Skeleton | 251 → 251 | 310 → 310 |
| Vorn | 226 → 226 | 236 → 235 |
| Mummy | 174 → 170 | 176 → 172 |

- Skeleton's route reset, pending contact, script counter, and range
  initializer use their native class fields. Skeleton_UpdateDefeat
  (080BD1F0) shares its three script payloads and passes the native
  collider to the kill counters. The spill still differs when the
  unreachable exits are removed. The ordinary tracking OR loads the
  byte before building the mask. The ROM performs those steps in the
  reverse order.
- Vorn_UpdateActionCompletion now takes the native chase actor and
  uses its tracking byte. With that signature the direct OR is one
  load-order difference from the ROM. The class uses native action
  table fields, a 120-frame pursuit wait timer, the sprite elapsed
  time, and the shared drawable hide in Vorn_UpdateDefeat.
- Mummy's straight-path check (080D6DF8), nearby target tracking,
  collider bounds, and profile use the shared layouts. Burn damage
  (080D812C) counts defeat reactions through EnemyDefeatStatistics.
  The revival and defeat callbacks share the drawable show operation.
  Testing the dying status as a literal (080D203C) changes reload
  registers for the prepared latch and signed chase-group timer.

All six units edited in this pass match their recorded ROM extents.
Both dependents of the Vorn header also match. The scoped linter
reports zero errors. Across all three passes, report lines fell from
743 to 647 and steering uses from 844 to 717.

## Visibility and route follow-up

| Class | Report lines before and after | Steering uses before and after |
|---|---:|---:|
| Skeleton | 251 → 244 | 310 → 300 |
| Vorn | 226 → 221 | 235 → 229 |
| Mummy | 170 → 170 | 172 → 172 |

- Skeleton's visibility flicker and hidden-state tick use the shared
  two-layout drawable operations. The return-to-entry callback
  (080BBBC0) uses the typed probe destination and shared state
  transitions. The approach callback reads the class distance bands
  and the chase fallback. Skeleton_UpdateDefeat (080BD1F0) indexes
  the random samples directly. Removing the remaining false paths
  still changes the spark-position spill and late table allocation.
- Vorn_UpdateHiddenRecovery (080AD1C0) takes the native actor and frame
  and uses the shared drawable and control transitions. The snapshot
  restore shares the revival health operation. The probe approach
  (080B04F4) uses the real chase route, Vorn class flags, and animation
  components. The direct animation callback expression still changes
  the rate-selection allocation and reload order. Direct sample
  indexing in Vorn_UpdateDefeat (080AEE08) also changes the spark
  registers.
- Mummy_UpdatePlayerRoute (080D3D60) uses the turn state, saved control,
  tracked target, and route fields. The shared saved-control helper
  changes a spill from sp+0x14 to sp+0x38 in this caller. The volley
  (080D62CC) reads class parameters and passes native positions using
  the projectile's declared interface. The particle pattern
  (080DA810) updates the same position type directly.

All five edited units matched their recorded extents. The scoped
linter reported zero errors. The pass removed 12 report lines and
16 steering uses. The original 844 steering uses are now 701.

## Target and class-field follow-up

| Class | Report lines before and after | Steering uses before and after |
|---|---:|---:|
| Skeleton | 244 → 243 | 300 → 299 |
| Vorn | 221 → 220 | 229 → 228 |
| Mummy | 170 → 170 | 172 → 172 |

- Skeleton reads the real tracked target and uses the shared vector
  difference when selecting a behavior at 080B6BA8. Merging the
  angle and distance fallbacks removes one of the ROM calls to
  Skeleton_SelectDistanceReaction. The reaction gate (080B6CE4)
  uses the native tracking and class context. The approach capture
  (080C0900) stores the target position at its class field at 0x710.
  The hidden defeat outcome uses a direct branch, and the bestiary
  declarations agree with their definitions.
- Vorn uses the chase route and actual player position in its
  callbacks. The class records side-step duration at 0x6BA and retreat
  requests at 0x6C4. The retreat selector (080B2858) reads the class
  limits. Patrol restore shares the delayed response operation, and
  the delayed script uses the existing class bytes.
- Mummy uses the shared random turn, typed player cell, and native
  health and collider in the burn handler (080D812C). The ordinary
  attack-bounds assignment still changes the generated stores. A
  separate palette-timer assignment and literal motion clear
  change two ROM instructions to three.

All six edited units matched their recorded extents. All four
Skeleton header dependents and both Vorn header dependents matched.
The scoped linter reported zero errors. This pass removed two report
lines and two steering uses, bringing the original 844 uses to 699.
