# Monsters worker report

Stopped on `build/STOP` during Tentacle actor. The assigned queue went from 604 to 337 warnings, a reduction of 267 (44.2%), with stock byte matches preserved for every changed unit. The runner acknowledged all 63 commits.

## Bat
- 14 → 1 warning, zero errors. `src/monsters/bat.c` and `src/monsters/bat_restore.c` match.
- `Bat_Init` (08084F44) uses SDK `ZeroFill8` for both position clears. Replacing either alone changes the stack layout.
- `Bat_RestoreSavedState` (08086C1C) uses the native actor and drawable types and the same revival setup as the matching ActorTrail candidate. All 79 header dependents match.
- `Bat_ResumeSavedTarget` (08086E2C) uses the native route probe, animation sequence and chase controls.
- The group-level gate clear matches Bat's landing and recoil and both shared arrival paths. All 46 header dependents match. Recoil uses the existing status setter.
- `Bat_UpdateProbeReaction` (08088C24) uses the Ghoul context and typed halfword flags. Defeat uses the real sprite and Bestiary types and the shared idle-control and state operations.
- Defeat reads the first motion flag through a typed halfword query that keeps the field address for the later test. Ghoul's route query provided evidence for the captured-address form.
- Open in defeat: the pointer-byte union. The natural shared sampler candidate is in `build/cx/work/burst_macro.c` and the pre-tool-change matchsearch store is `build/matchsearch/Bat_UpdateDefeat`.

## RANDOM TABLE
- Bat defeat with an ordinary table pointer matches under `AGBCC_NOEQUIV=table`. The shared `ParticleRandom_ReadSample` macro on the first draw gives the table enough references to reach sl in stock code, but it takes r5 first and changes the other local placements. Global allocation then evicts it. `AGBCC_FORCE=table:10` on the macro candidate matches exactly. Repeated direct/macro draws, a typed sampler carrying the index pointer, and alternate burst argument scopes did not clear the union in stock C.

- Both Spider wander priority reports remain open. Three draws advance one cursor across the navigation call. With a plain table and cursor, the stock compiler rematerializes the table and keeps the radius in sb. Forcing the observed input, body, table, cursor and mask placements with table/cursor equivalence disabled is diagnostic-exact. The shared sampler, paired cursor/table state, and native radius, mesh, and route-result forms did not reproduce that allocation in stock C. Candidates and result summaries are under `build/cx/work/spider_wander/`.

- `Centipede_SpawnSparkBurst` (080AB294) is diagnostic-exact with a plain table pointer and `AGBCC_NOEQUIV=tbl`. Stock preserves the wrap mask in sl and rematerializes the table, while the ROM keeps the table in r7 and reloads the mask. The 43 typed pointer, packed vector, and shared sampler trials did not remove the union. See `build/cx/work/tentacle_sparks/trials.result`.

## Tool changes
- Scope `why` reads actual K/E records to explain earlier ownership of a local register and the density comparison behind its eviction. On the Bat sampler candidate, the new diagnostic suggests the exact confirming intervention. The 230 scope tests pass.
- Scope reload hints now restrict goto suggestions to the compiled function. The neighboring-function regression and all 231 scope tests pass.

## Ghoul
- 20 → 0 warnings, zero errors. Both source files match.
- Death animation, hidden recovery, chase updates, and route recovery use the shared state, flag, and drawable operations.
- `Ghoul_UpdateProbeApproach` (0808A234) passes the actor directly after recovering the shared animation-scale preparation. The helper also matches both Spider probe callbacks in scratch. All 46 existing header dependents match.
- The timed script uses native context fields as in Vorn's matching callback. The pending-route query uses the actual chase route probe and preserves the address of the motion flags for the route retry.
- Tentative names `Ghoul_RefreshMotionRoute` (0808A90C) and `Ghoul_ReadDelayedScript` (0808B320) describe their actual fields. All three Ghoul header dependents match.
- `Ghoul_Init` (0808D1D8) uses the SDK clear for both temporary positions. Shared drawable allocation and the member query reproduce the PRE slot order. All nine pursuit-body header dependents match.

## Slime
- 25 → 3 warnings, zero errors. The hit-type switch and shared chase, motion, and defeat operations match. Both pending flags match through the same halfword query. Slime uses the native sequence drawable and signed affine palette bytes. Three callback names now reflect their Slime context.
- Open priority site `Slime_UpdateRandomWait` (0808E838). The word selector omits the unused mask copy to r8. The generic byte playback form restores it but changes the sibling wait and seven other header-dependent units. Reverted that header trial. The direct flag composition and no-options selector also omit the copy.
- Scope's moved-row hints now skip pool data without RTL IDs. All 232 scope tests pass.
- Open in `Slime_FollowPlayer` (0808FD9C). With the native tracking record and direct state literals, the OR moves just ahead of the zero that remains live for the later state reset. Native and legacy tracking forms, combined and split control helpers, and the reaction reset all leave that instruction order different.
- Open in `Slime_Init` (08090FEC). Both SDK fills emit the exact local destinations and entire instructions. The result has PRE table size 663, rotating four field-address spills to the front of the frame. Diagnostic table size 665 matches exactly. The typed graphics, origin scopes, and body-reader variants did not produce the required table size in stock C. Best draft `build/cx/work/slime_final_init_exclusive.c`.
- The single Slime header dependent matches after removing its duplicate drawable layout and renaming its callback declarations.

## Bee
- 33 → 0 warnings, zero errors. `src/monsters/bee.c` matches. Common chase controls, typed defeat and contact checks, native planar curves, SDK fills, and the native sequence drawable account for the reduction.
- `Bee_Init` (08094764) matches both SDK position fills with stock PRE. The route and movement constructors also match the SDK clears, and both curve builders zero the actual height fields.
- The remaining route reset matches with direct writes to its fields. The wander angle matches with the half-turn added to the heading before adding the random offset. The duplicate drawable header has one dependent and it matches.

## PARAMETER HELPERS

Counts are distinct direct callers in tracked source, including shared header callers.

| Helper | Callers | Assessment |
| --- | ---: | --- |
| `SceneActorChase_GroupClearFlags` | 4 | Needed for the match. Clears the group's actual gate field, but every caller passes 4 and the helper is a generic bit clear. |
| `SceneActorChase_PrepareAnimationScale` | 14 | Real operation. Checks the actor's motion mode and prepares the animation scale in one step. Two Ghost callers, Lun's route scale update, and two Crow callers also use it. Three Spider and four Tentacle callers now use it. |
| `SceneActorReaction_BeginRevival` | 1 | Real operation. Applies the health and immunity changes together. The same sequence was recovered in the ActorTrail candidate. |
| `Bat_TestProbeFlags` | 1 | Needed for the match. Renames and types the old one-off predicate, and still takes the constant bit as a parameter. |
| `Bat_ReadDefeatMotionFlags` | 1 | Needed for the match. The out pointer preserves the flag address for the later test, but this is a one-off typed mask query. |
| `Ghoul_EnterRouteBehavior` | 3 | Real operation. Changes route behavior and resets its entry state in three route transitions. |
| `Ghoul_SetContextFlags` | 2 | Needed for the match. The pre-existing mask-first setter was moved and reused for the second disc flag. |
| `Ghoul_ReadRouteMotionFlags` | 1 | Needed for the match. The typed query preserves the field address for the retry and still takes a fixed mask. |
| `Slime_EnableReactionMotion` | 1 | Needed for the match. Sets a fixed behavior flag and returns its address for the completion path. |
| `Slime_TestPendingFlag` | 2 | Needed for the match. The shared halfword predicate preserves the mask-first evaluation of both old locals. |
| `Bee_SetRouteFlags` | 2 | Needed for the match. Both callers set the same bit. The generic setter replaces the literal locals that did not match in place. |
| `Bee_HasRouteFlag` | 3 | Needed for the match. Replaces two local generic queries and one route predicate with one typed query. The direct tests miss the mask ordering or Boolean result. |

The SDK fill recovery also factored the existing pursuit initializer into a field preamble and a script reader, with two wrapper callers each. `MonsterInit_LoadPursuitBody` has one tracked caller and is a real constructor operation. `Ghoul_AllocateDrawable` has one caller with two call sites and is a real allocation and memory-clear operation used for both drawable kinds.

## Ghost
- 65 → 20 warnings, zero errors. The full unit matches with SDK position clears, native planar fields, common control installs and state transitions, the shared animation scale operation, direct widget playback bits, and native drain counters.
- Scope diagnostics mistook a call on a continued condition for a definition. The parser now balances parameter lists before locating the body. `Ghost_AdvanceMovementCurve` and all 233 scope tests pass.

- The contact and linked-player locals remain open. The direct constants and existing combined control/state operations change load order in the contact setup, and the direct link test folds the Boolean branch used by the other callers.
- The turn threshold has one instruction of disagreement when typed as an octant delta. Shared scale preparation changes two allocation decisions in the same turn callback. The other two Ghost animation scale paths match.
- The first defeat flag read still needs the field address for the second read. The linked-collider flags, retreat vector, and four motion or placement flag sites change materialization order when expressed directly. The visibility and reaction flag tests likewise need a different address or mask order. Scratch candidates and exact differences are in `build/cx/work/ghost_last_*.result` and `ghost_direct_*.result`.
- No parameter helpers were added after the coordinator review.

## Lun
- 53 → 21 warnings, zero errors. The new native fields cover the route motion, curve endpoints and time, participant counters, and delay. The only header dependent matches.
- `Lun_Init` matches both SDK fills in stock C. The position-curve entry, movement reset, and launch callback also match the SDK clears. The route scale uses the same shared preparation as Ghoul, Slime, and Ghost.

- The native sprite and body fields match in the turn callback, while its octant threshold and tightly grouped duration-scale preparation remain open. The route scale operation matches in the two route callers.
- The lift/resume, linked idle, and delay/collision-reset paths still differ when the state fields are written directly or the shared reset is used. The launch, hover, and collider constants still change operand setup when used directly. Matched source keeps those warnings visible. See `build/cx/work/lun_last_*.result` and `lun_direct_*.result`.

## Crow
- 41 → 4 warnings, zero errors. Crow's recovery, scripted action, and reaction match through the common control and state transitions. Its two hop clears, ground reset, and directed response match through the SDK. The common sequence drawable covers all six users.

- `Crow_Init` (080A1440) has the same stack-only effect as Slime when using two SDK clears. The draft has PRE size 519, and size 521 is diagnostic-exact. Native drawable, health and body script forms have not restored the stock table size. Best draft `build/cx/work/crow_last_init_base.c`.
- The tracking bit test, scripted context bit, and knockback terrain mask remain open. The direct forms move the mask relative to the field load or the terrain call. No parameter helpers added.
- The only Crow header dependent matches after removing the duplicate drawable.

## Spider
- 39 → 11 warnings, zero errors. The full unit matches through the common animation scale and control transitions, native landing positions and sequences, named defeat parameters, delayed recovery packet, shared world and drawable queries, and native idle/tether context.
- `Spider_UpdateWanderGoal` (080A4018) has two proved-zero reports. See RANDOM TABLE for the shared draw attempt. Its 60-frame delay direct store also changes the timer address and value setup.
- `Spider_Init` (080A4AFC) matches the SDK pair apart from the rotation of three PRE spill slots. With the current pseudo numbers the hash model cannot put the collider address between the other two. Testing the preamble with native contexts, sprite fields, and deferred control operations left that result unchanged. Best clean draft `build/cx/work/spider_last_init_sdk.c`.
- The two hidden bits, landing and defeat flags, route response pending clear, and tether and idle predicates still change operand or flag address materialization in the ordinary forms. No parameter helpers or ignores added.

## Tentacle body
- 18 → 2 warnings, zero errors. Both units match after the two SDK secondary-motion fills, shared attack and defeat control transitions, native sequence drawable, shared flag queries, and direct selection and random literals. Both `tentacle_body.h` dependents match.
- The hidden drawable branches need the mask before dispatch. Native flags and a shared pointer after dispatch change the code. The direct recovery health store moves the immediate after the field address. Both warnings remain open. No ignores or parameter helpers added.

## Tentacle actor
- Partial pass 63 → 42 warnings, zero errors in all 31 queued files. All 31 match after the stop check. The waypoint, retreat, pursuit, and cell return share the animation scale. Spiral uses the SDK motion clear, both knockback handlers use the shared control transitions, and the segment removal, terrain alignment, and impulse sites use native segment fields. Both header dependency sweeps passed (2 and 7 dependents).
- The state installs and entry resets still change the order of the behavior and address loads in the waypoint phases. The direct sprite predicates, divisor limit, knockback factor, and segment impulse masks also change operand setup. See `build/cx/work/ta_*.result` and `ta2_*.result`.
- Resume at 080AAA90. The native segment impulse view and shared action check match in `ta2_080AAA90_blocked_native.c`. At 080AB04C the native waypoint view and shared bit operations match separately. The combined scratch candidate still needs a full check and landing. The generic action check also matches at 080AB6F4. The radial velocity and hit reaction views, collider setup, sequence lookup, queued state reset, facing reaction, and constructor are pending. No parameter helpers or ignores added.

## Open queue
- Tentacle actor has 42 warnings left. Vorn (87) and Skeleton (146) are still pending. See each earlier unit for its visible holdouts. The three random-table cases are recorded under RANDOM TABLE.

No ignores added.
