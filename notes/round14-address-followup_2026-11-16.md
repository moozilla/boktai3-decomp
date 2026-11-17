# Round 14: address-expression follow-up and open families

Worker source checkpoint: `cc6a8f492`, following the earlier checkpoint at
`640ef64cb` documented in
[round14-address-expressions_2026-11-16.md](round14-address-expressions_2026-11-16.md).

The assigned 59-site queue now has **29 address sites removed and 30 remaining**.
Only the first two removals were simple deletions; another 27 required ordinary
source structure. The worker also removed **20 literal-only locals**. Every
accepted change passed a complete translation-unit stock comparison and a focused
before/after concession scan. No shared header was changed in this pass.

The remaining `*(&local)` forms still deserve concession tracking. Their effects
are broader than register allocation: the measured differences include operand
order, induction variables, instruction placement, and branch joins. Failure of
one direct deletion is a measurement, not proof that a natural exact form does
not exist. The collision-manager case below demonstrates that distinction.

## Additional exact address removals

These seven source units remove eight more sites after the 21-site checkpoint.

| Unit | Ordinary source change | Sites | Complete exact extent | Worker commit |
|---|---|---:|---:|---|
| `scene_status_indicator.c` | Allocate through the typed entry array and the existing occupancy predicate | 1 | 1,768 bytes | `461b993c2` |
| `fn/sub_08069D0C.c` | Pass the literal bit value to the existing burst occupancy predicate | 1 | 68 bytes | `bdffaf8fd` |
| `fn/sub_0810AFB0.c` | Pass the actual packet-mode mask directly to its three predicates | 1 | 368 bytes | `d3c8c34af` |
| `fn/sub_08062414.c` | Form each catalog index from the selection and page, including the post-call page reload | 2 | 460 bytes | `85f155917` |
| `choice_menu.c` | Read the four optional script operands through an indexed argument array | 1 | 2,944 bytes | `960bac51b` |
| `fn/sub_08041128.c` | Fill unused modifier windows by row index, then refresh the capacity | 1 | 344 bytes | `8c553dd08` |
| `collision_controller.c` | Keep the two boolean predicates and their selected result in the byte domain used by `paused` | 1 | 2,512 bytes | `cc6a8f492` |

The first two rows also remove one constant-`one` local each. The status-indicator
change removes a local `volatile u32 *` declaration, but the existing shared
occupancy predicate still accepts a volatile pointer. Its disappearing local
`volatile-view` warning is **not a complete removal of that volatile access**.

`CollisionController_InitManager` is an instructive repair. The direct deletion
had identical normalized instructions but one different branch-destination byte:
it bypassed the common boolean normalization. Both source predicates return only
zero or one, and the destination is a byte field. Representing those predicates
and the selected value as `u8` preserves the original branch destination without
an address barrier. The complete unit check, rather than the normalized score,
establishes the match.

## Literal-only locals

The coordinator's conservative generator was reused only on this worker's owned
units. It proposed 38 removals in 16 units; 16 removals in eight units matched.
Every accepted diff was reviewed, then independently checked with the focused
linter and the complete original unit extent. The two earlier pool allocators
and two additional occupancy fixes bring this worker's total to 20 removals.

| Unit | Additional locals removed | Complete exact extent | Worker commit |
|---|---:|---:|---|
| `fn/sub_08036230.c` | 1 | 176 bytes | `10ad357f3` |
| `fn/sub_08056F88.c` | 1 | 196 bytes | `af07286b3` |
| `player_clone.c` | 2 | 4,452 bytes | `1d990b3de` |
| `slime.c` | 1 | 15,436 bytes | `fed6614e2` |
| `status_screen_fade.c` | 1 | 2,232 bytes | `cf8e4844f` |
| `timed_effect.c` | 3 | 1,992 bytes | `716818b94` |
| `vanargand_projectiles.c` | 2 | 3,368 bytes | `9099e2f67` |
| `wireless_game_items_and_display.c` | 5 | 5,832 bytes | `f5e35035c` |

The generator preserves signedness with unsigned literals for `u32`, excludes
address-taken fill storage, and does not rewrite mixed assignments or duplicate
same-named declarations. For example, a CpuSet fill object needs real storage;
an enemy local named `zero` may instead hold an incoming response value. A
`const-local` spelling warning alone does not establish an allocation concession.
The 22 nonmatching proposals and the excluded cases remain visible.

## Remaining operation families

The 30 remaining sites span 28 units and 29 functions. Each family was tested
with direct deletion and at least one ordinary structural alternative. No dummy
work, fake condition, weighting loop, pointer-only union, volatile substitute,
or diagnostic suppression was adopted.

| Operation family | Sites | Targets and observed limit |
|---|---:|---|
| Animation and homing arithmetic | 2 | `PlayerClone_SetAnimation` and `EffectPool_UpdateEntry` each differ by one commuted ADD after direct deletion. Clone's order changes during CSE1; the homing ADD order is already present in initial RTL. Natural sequence, step, heading, and angle-domain forms were preserved but do not match. These are operand-order differences, not interchangeable register allocations. |
| Optional script operands | 2 | `DirectionalController_ScriptCreate`: the ordinary four-element argument loop is down to three register choices when establishing the array base. The corresponding choice-menu loop was solved above, but its source shape alone does not solve this controller. |
| Random-ring sampling and coordinate offsets | 5 | `LevelAdvanceDisplay_StartParticle`, `ScriptCombatActor_UpdateProjectileAttack`, `CallbackGroup_EmitAtPosition`, `PlayerOrbitEffect_SpawnParticle`, and `SceneActors_SpawnAmbientParticle`: actual cursor expressions, halfword samples, or direct coordinate operations still alter allocation or scheduling. The successful staged-effect and roaming-choice sample-width repairs do not transfer automatically to these call patterns. |
| Quarter-facing animation | 2 | `RoamingObject_UpdateAnimation` and its B variant: a byte angle and a normal facing helper still change shift/add structure. The two unrelated random-choice sites in the same unit were fixed separately. |
| Callback, occupancy, and timer flow | 5 | Ghoul and Skeleton probe approach, `Hresvelgr_UpdateMotionPhase`, `SceneParticleBurst_Update`, and `BouncingObject_HandleTimerContact`: typed query dispatch, direct actor/pool parameters, indexed slots, or ordinary timer branches change the measured code. Skeleton's initializer installs the halfword `sequence` query; the earlier word-query hypothesis is explicitly rejected in its graph. |
| Arrays and bounded lookup | 6 | `Wireless_BeginChildTransfer`, `ScenePanel_InitTimePalette`, `SceneActorRecord_FindPair`, `StageResults_ShowTimeDigits`, `Player_CollectCallbackFlags`, and `PlayerPanel_RemoveEffects`: actual ID, palette, record, widget, callback, and particle arrays were tested. The RFU ID search is down to a global-address register/copy difference; other cases retain induction or placement differences. |
| Vector, palette, and collider fields | 5 | `PositionedEffect_InterpolateArcNodes`, `TetherActor_PositionRequestIndicator`, `StatusScreen_FadeAndExit`, `TimedEffect_OnContact`, and `VanargandProjectile_UpdateApproachLaunch`: direct components, palette-bank cursors, ordinary flag merges, and adjacent collider fields are not yet exact. Tether differs only in where the getter-result copy occurs relative to the projected-position address setup. |
| Ring-particle initialization | 1 | `SceneEffect_InitParticleRing`: direct zero stores produce an additional post-loop zero materialization; the ROM reuses the loop's zero value. No artificial dependency or store/readback was added to keep it live. |
| RFU and outgoing-item control | 2 | `RfuManager_StartChildConnect` and `WirelessGameItems_StartOutgoingItems`: SDK partner fields and typed item-array/index/cursor loops remain nonmatching. The existing once-loop forms were not replaced with new scaffolding. |

These are bounded findings, not claims that every site is irreducible. Close
instruction counts are useful triage evidence, but neither a low distance nor a
compiler-switch experiment authorizes changing the stock acceptance rule.

## Preserved evidence

Replayable source evidence is tracked in
[round14-address-candidates_2026-11-16.jsonl](evidence/round14-address-candidates_2026-11-16.jsonl):
138 complete candidate patches for all 29 open functions in 28 units, with
hypotheses, rejection reasons, and historical score summaries. Apply each patch
independently to its named source at integration commit `ea3b1c59d`; the manifest
records the full commit and baseline blob IDs. Each reconstructed source is
checked by its recorded SHA-256. Only the named function was historically scored;
some snapshots contain earlier states of unrelated functions. These are preserved
hypotheses, not proposed source changes, and carry no generated assembly or RTL.

The fuller tooling records below remain in the ignored
`build/round14/addr-deref/` directory of the address-expression worktree; accepted
source and rationale are in the commits.

- `remaining.json` records the exact 30-site, 28-unit remainder.
- `family-matches.json` records each accepted structural and literal change,
  its stock extent, focused diagnostics, commit, and source candidate.
- `constants/results.json` and its per-unit subdirectories preserve the 38
  literal proposals, baseline sources, attempts, and accepted candidates.
- `graphs/index.json` indexes **32 matchsearch graphs with 151 unique nodes**:
  all 29 open functions plus the solved choice, modifier, and collision examples.
  Every stored candidate compiles under the frozen context. The portable
  `*-history.json` exports contain complete sources, hypotheses, historical
  evaluations, and the rejected Skeleton return-domain hypothesis.
- The live graphs are under `build/matchsearch/round14-address-*`. They freeze
  the canonical diagnostic compiler and source environment. Historical scores
  must be recomputed after importing into a changed context.
- `clone-why-all.txt` and `effect-why-all.txt` preserve the operand-order traces.
  The separately saved experimental commutation-switch logs did not improve
  these two cases and are not matching or causal proof. No experimental compiler
  change was adopted.

This worker performed no whole-tree concession sweep or complete rebuild. The
coordinator owns the combined build after integration; per-unit acceptance here
uses the pinned stock compiler and original complete `.text` extents.
