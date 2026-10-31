# Finish scene actors, part 3

Tags: #matching #scene-actors #compiler #scope-gap #register-pinning #structs #data #studio

Thirteen of the fifteen assigned targets now match in ordinary C: fourteen physical functions, including the recovered forward-floor neighbor. This converts **89.313% of the assigned code**, an **89.313 percentage-point gain** in this queue, or **0.5007 percentage points** of project code (12,068 bytes secondary). Two targets remain guarded drafts. No landed match from this queue uses pins, barriers, handwritten instructions or compiler overrides.

The continuation adds five exact targets, **0.2137 percentage points** of project code (5,152 bytes), and removes three existing pins plus three empty asm constraints from three neighboring exact units. These removals move 1,820 bytes from concessions to ordinary C without changing exact coverage.

Branch: `codex/finish-scene-actors-3`, based on `a8c44f25d`. Work stopped at the owner's request with 23% weekly quota remaining, preserving the requested 20% reserve. The owner explicitly overrode the original brief's 50% floor earlier. Temporary register concessions were explored only after ordinary source attempts and compiler proofs; neither unfinished pinned candidate was landed.

## Reserved targets, in ROM order

| Address | Reviewed bytes | Result | Deciding source property or remaining mechanism |
|---|---:|---|---|
| 080C09F0 | 2,300 | MATCH — `Skeleton_UpdateTargetApproach` | Typed chase ownership, direct saved-control reads, promoted map width and direct cell-height storage recover PRE identities and stack ordering. |
| 080F2F7C | 560 | MATCH — `PhaseObject_UpdateEntryScatter` | Negate entry offsets before addressing; use typed indices, the RAM random-table literal and origin-first sums. |
| 080F3420 | 168 | MATCH — `ChildGroup_LaunchNextInactiveMember` | Destructive member-address accumulation, the common typed stride and byte activation preserve both exit-path multiplies. |
| 0812B91C | 448 | MATCH — `PursuingActor_AdvanceChase` | Shared pursuing/vector records, timer-before-Mod, signed lifetime, typed captures and velocity-first addition. |
| 0812BADC | 516 | MATCH — `PursuingActor_UpdateRecoil` | Mask/pointer definition order, unsigned RNG domains, full terrain offset and velocity-first addition. |
| 0812E118 | 708 | MATCH — two physical functions | A 420-byte traversal routine plus the recovered 288-byte `SceneActorTerrain_IsBelowForwardFloor` at 0812E2BC. Preserve the ROM's slope read-before-fill and branch-local offsets. |
| 0812E7EC | 1,764 | MATCH — `SceneActorReaction_DispatchPendingCommands` | Typed signed attachment components; the real shared packet/offset workspace; receiver initialization before group declaration; guarded do-loop; actor-derived receiver consumption; actual u8 message category. |
| 0812F5D4 | 800 | MATCH — `SceneActorReaction_UpdateContactRoute` | Real u16 RNG cursor/sample and u8 direction domains change pre-GCSE identities to the required N297/T149 and preserve the direction address. Staged group ownership remains intentional. |
| 0812FA0C | 1,976 | MATCH — `SceneActorReaction_UpdateRouteReturn` | Shared delayed-response/chase operations from the matched dog twin, with wait threshold 60 rather than 20. |
| 081314E8 | 824 | MATCH — `SceneActorRoute_UpdateContactRecovery` | Invoke the genuine void actor callback directly through its compatible typed pointer. The old Callback_InvokeActor symbol is a libgcc call-via-r1 trampoline; the typed call prevents the unwanted merged tail with identical BL bytes. |
| 08135F70 | 1,156 | Guarded ordinary draft 96.4% — `SceneActorReaction_ResolveDamage` | Correct flags OR semantics; typed ownership flags, signed mask records, late display priority and delayed health ownership recover the complete frame/HUD/tail. Remaining stock scope: 20/20, shape 4. |
| 081363F4 | 1,492 | MATCH — `SceneActorReaction_ResolvePlacementDamage` | Separate status/motion owners, packed flags snapshot, fully initialized signed mask carrier, late priority decision, real HUD/collider helper boundaries and full-width final field query. |
| 0815AC04 | 240 | MATCH — `ScriptActor_TestFacingPointFromScript` | Reset the consumed actor cursor after dx/dz so CSE retains the independent player alias; dead-set elimination removes the reset. |
| 08181514 | 288 | Guarded ordinary draft 90.1% — `ActorMarker_LaunchFragments` | Original scalar source remains the best readable ordinary draft: 284 bytes, 11/9, shape 8. Complete compiler proof and a 288-byte pinned near-match are retained separately. |
| 081B8E44 | 272 | MATCH — `OrbitMotion_UpdateCrossedSines` | Late `(s16)screenX` in the final Z projection lengthens the second coordinate's pre-combine life to equal the first; combine removes the redundant conversion. |

The 0812E118 row already includes its hidden neighbor; do not count that 288-byte body twice. Descriptive names remain tentative. Every exact replacement was checked independently over its full reviewed extent.

## Remaining damage resolver

The checked-in ordinary draft is the durable 20/20, shape-4 frontier. It preserves existing hit flags while ORing 0x20; the inherited higher-scoring pinned implementation overwrote those flags and was semantically wrong.

A local health pointer is captured into a one-field owner after subtraction, then extracted into a separately declared persistent pointer after the pending query. HUD and tail queries share that actual pointer. This removes the unwanted health PRE cache and gives the exact sp+0x14 health / sp+0x18 damage-flags order. A visibility decision union initialized from the real predicate, then changed to signed16 priority 1 after reading HUD arguments, reproduces the late literal and its preservation across HUD/CpuSet/display.

Both ~0x201 clears use a signed16 mask component with the entire carrier first initialized from the actual input flags. That delays constant folding until combine, keeps mask-first AND order and avoids the entry-live padding/spill caused by a partially initialized carrier. Whole zero initialization folds too early.

The remaining three groups are five initial reload-register differences and fifteen instructions around marking/snapshotting flags and the pending mask. Narrow early snapshots preserve updated/four/mode allocation at r5/r4/sl but leave an OR-result copy after the store. Full SI snapshots eliminate that copy but move a local health pointer from r3 to r2, cascading the wrong global allocation. Forcing only that local health pointer to r3 diagnostically restores the entire later allocation and reduces the full-SI source to 10/8, shape 4. This is stronger evidence than forcing several globals together.

The best temporary pinned candidate reaches 5/5, shape zero, with five genuine local bindings and no empty asm: `build/scope/drafts/resume3_hud_08135F70/pinned-late-four-down-marked.c`. Only offset-720 reload r4 versus r5 and the following signed-health zero index r5 versus r1 differ. `AGBCC_SPILL=2:1` fixes those five rows on the ordinary frontier. A real pinned offset cursor reused for health keeps the first correct offset and zero register, but leaves extra health-address arithmetic. Bare one-use offset hints fold away; signed/narrow parameter, ownership, delta, component, DEBUG_NOP and volatility variants did not solve it. This near-match is not exact progress.

Evidence: `build/matchsearch/resume3_hud_08135F70/REPORT.md`, the `resume3_hud_08135F70` draft directory, and `build/scope/campaign/resume2_hitflag_tie/REPORT.md`.

## Remaining fragment launcher

The original scalar-counter source has a complete diagnostic 288-byte match under three narrowly proved decisions: prevent origin HI/SI quantity merging, leave origins reload-owned, and retain Y-offset pseudo 75 through combine. The private driver is `build/brief-work/diagnose_marker_origin.py`; proof knobs are `AGBCC_NOCOMBINE_HISI=1 AGBCC_NOREG_ORIGINS=1 AGBCC_COMBINE_KEEP_REG=75`. None is a final-source dependency.

A real fast/index/sum workspace and an opaque RNG owner reproduce the frame, full sp+8 Z-sum store and tail index reload. Their tradeoff is altered local quantity ownership. Separate wrap/sample masks restore high-register owners, but the original scalar base with the three-decision proof is the cleanest restart point. Typed origin views, explicit signed/unsigned domains, real owner/query APIs, DI initialization and DEBUG_NOP boundaries were measured.

The strongest retained temporary pinned source is `build/scope/drafts/sub_08181514-resume-pinned-frontier.c`: 288 bytes, 3/3, shape 2, five differing bytes, fuzzy 97.8873%. Six local bindings and one empty value constraint retain the X low-pointer copy; Y load folding/roles remain. The exact frame, sp+8 store, preheader, counter and RNG owners match. r7-bound trials suffered eliminable-frame-pointer miscompilation and were discarded; no such source was landed. Volatile local bindings add a stack home without solving the decision.

Evidence: `build/scope/campaign/resume_08181514/REPORT.md`, `build/scope/campaign/resume_08181514_index/REPORT.md`, `build/scope/campaign/resume2_08181514_rng/REPORT.md` and the marker matchsearch graph.

## Landed cleanup

- `SceneActorReaction_ApplyCategoryDamage` (696 bytes): remove its owner-mask pin using the genuine halfword query result.
- `SceneActorReaction_ApplyStaggerDamage` (836 bytes): remove the same pin and its hit-bit value constraint using the signed halfword setter.
- `SceneActorControl_UpdateCallbackTrigger` (288 bytes): greedy production checks remove one pin and two empty constraints; remove stale mismatch commentary and unused helpers, and use the shared death-control field.
- Four damage handlers now use shared actor, palette and collider fields. Duplicate damage-property and drawable records were removed from both area-damage handlers.
- `include/scene_actor_damage_visibility.h` owns the common link ownership/display predicate used by five exact handlers. The remaining resolver can migrate after its allocation decision is solved.
- `damage_number_display.h` now owns the source identity record and actual Show declaration; the incompatible default-scale wrapper declaration was removed. `scene_hud_display.h` owns the actual timed-state declaration. All 118 actual includers match over their full reviewed extents.
- The shared orbit header has an include guard; all ten source includers still match.
- Three marker parameter tables now round-trip from editable CSVs: 180 bytes of frame/animation indices and signed XYZ/unknown-halfword offsets. Reviewed nonpointer ranges remove a false code relocation at 0824E6D8. Legacy linker labels remain compatible with existing C.
- Studio exposes `/actors#tables` and exact table deep links, source/consumer provenance, signed values and exact decoded ROM intervals. The data/Studio implementation has 18 relevant passing tests, all 26 manifest roundtrips pass, and an isolated full symbolizer run confirms pointer removal without editing shared generated output.

The wider read-only inventory remains `build/brief-work/cleanup-inventory/REPORT.md`. A greedy removal pass checked all ten other hinted behavior/placement files: only the callback-trigger unit had free removals. The escalating-hit owner-mask transfer still has fourteen register-only differences, so its existing pin was retained. Marker/particle and phase/child record consolidation remains future cleanup; it was not partially applied while matching was active.

## Validation and integration

Exact matching commits use stock scope plus independent production reviewed-extent checks. Guarded drafts retain byte-exact default ASM fallbacks; the resolver draft was recorded with the venv as 96.4%, 30 differing bytes, and is not counted exact. New cleanup checks cover each changed source and every actual shared-header includer. No hand-written instruction bodies, ROM, generated assembly, creative assets or emulator dumps were committed.

The requested tooling cherry-pick `02f36efc6` landed as `f4514cbad`: current function names work in check.py, and matchsearch uses short temporary sockets in managed paths. Earlier private name/address wrappers are historical evidence, not required for normal checks now. Scope's jump-table decoding can overcount the renamed command dispatcher's listing; the full 1,764-byte production comparison is decisive. NOTIE can prevent a regmove tie that local allocation later recreates, so unchanged output alone is not evidence of an ineffective intervention.

No push, PR, combined ROM build, PROGRESS regeneration or regroup was performed. The integrator must review the batch, run the combined build requiring `build/boktai3.gba: OK`, run shifted-data validation for the numeric-table change, then regenerate and commit both PROGRESS files once. The checked-in note and guarded C are the durable checkpoint; detailed scratch frontiers remain in this worktree's ignored build directories.
