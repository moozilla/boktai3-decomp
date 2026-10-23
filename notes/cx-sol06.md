# cx-sol06 campaign notes

Tags: #matching #hresvelgr

- 0819BB18: drafted 81.0%. Decoded retained tail directly; facing-relative polar offset, unused target position, sixteen randomized motion emissions. Remaining table-address lifetime and loop allocation differences. Reviewed extent 416 bytes.
- 0819BCB8: skipped after side-emitter reconstruction. Best 65.4%; signed random remainders, opaque dead literal and cursor lifetime differ. Reviewed body ends at 0819BE28 (368 bytes); the listed 636 bytes includes another retained tail. Local attempt retained under ignored build directory.

## Batch 1

Exact matches (reviewed body sizes):

- 0819E25C: ActorAction_FinishRecoveryAnimation, 120 bytes. Ported the matched adjacent recovery-state layout and added the cooldown store.
- 0819E3B4: ActorAction_BeginLinkedAnimation, 84 bytes. Entry-flag helper and a saved actor alias reproduce the linked callback and timer update.
- 0819F55C: DirectionBurst_EmitOffset, 88 bytes. An opaque offset local prevents arithmetic reassociation. Listed retained extent also contains a separate emission-loop tail.
- 0819F638: Hresvelgr_UpdateRandomVolleyPhase, 336 bytes. Structured vector, no redundant vector pointer, RNG operand order and constrained table/spacing locals.
- 081A09C4: Hresvelgr_PlayDescent, 196 bytes. Opaque actor alias retains the original second actor-pointer lifetime.
- 081A0EA4: Hresvelgr_EmitRandomMotionBurst, 332 bytes. Ascending source loop compiles to the observed countdown; constrained per-iteration RNG table keeps its load inside the loop.
- 081A0FF0: Hresvelgr_UpdateOscillatingMotionBurst, 328 bytes. Ported the same burst helper; scoped flag-mask local reproduces the flag-update order.
- 081A1D18: Hresvelgr_UpdateFlaggedMotionBurst, 324 bytes. Same burst helper, enabling flags before entry processing.
- 081A2858: Hresvelgr_UpdateCollisionPresentation, 868 bytes. Ascending collider loops, explicit camera fallback and mask, and reversed reset-pointer declaration order.
- 081A388C: EffectPool_AllocateEntry, 256 bytes. Correct five-argument Anim_SetSequence declaration is essential; its zero fifth argument accounts for the outgoing stack slot and allocation throughout the function.

Guarded drafts improved or added:

- 0819C5D4: 91.6%; target calculation is close; table load, argument-copy order and redundant angle mask remain.
- 0819C6AC: 89.7%; target-vector address and polar-helper lifetimes remain.
- 0819C870: 88.2%; sine-table pointer copy and scaled-speed register differ.
- 0819C8F8: 71.4%; cosine-angle common subexpressions and scaled-speed register lifetimes differ.
- 0819FE9C: 87.5% from 86.1%; removing the position pointer local reduced realigned differences from 123 to 114 and structural differences from 35 to 30.
- 081A3554: 95.1% from 91.5%; literal X coordinate and constrained coordinate copies leave four register differences around lifetime/collider arguments.
- 081A39E0: 97.4% from 96.9%; a shared trail-count pointer removes two register differences. Animation mask reload and trail-setup scratch choices remain.

Unchanged or deferred:

- 0819CD60: pointer-local deletion sweep regressed all candidates; retained 81.9% draft. Dot-product sharing and second normalization register lifetimes remain.
- 081A0BC8: retained 99.4% draft. Eight arithmetic variants and a bounded single-process permuter run did not improve the two remainder-register differences.
- 081A2378: retained 85.7% draft; changing radial-array address association regressed to 72.8%.
- 081A2430: retained 87.5% draft; typed trigger indexing regressed to 81.5%.
- 081A14FC, 081A1EFC, 081A2C80: inspected entry/control structure and existing notes; deferred large defeat/damage/script-initialization bodies for a later pass. No new draft claimed.

All accepted matches passed check.py with explicit reviewed target extents. Full batch build and integration result are recorded below when complete. Register constraints use only empty compiler barriers and are marked as matching concessions; no instruction bodies were added.

Batch 1 validation: the first combined build exposed an alias-related splitter bug that removed retained assembly after 0819F55C. The splitter now compares generated exact names; regression tests cover both alias directions and ensure the retained blob appears exactly once. Registered independent tail 0819F5B4 in build/progress inventories so its 132 bytes remain assembly credit. Focused splitter tests passed (nine test methods), and the corrected combined build printed `build/boktai3.gba: OK`.

Batch 1 exact gain: 0.1216 percentage points (2,932 bytes), including 0.1026 percentage points register-pinned (2,472 bytes) and 0.0191 percentage points without pins (460 bytes). Eight guarded drafts gained fuzzy coverage only.

## Batch 2, in progress

Batch 1 merged as PR 577 after the verified combined build.

- 081A3E48: matched EffectPool_CreateEntries, 188 bytes. Ascending three-entry loop; opaque entry pointer prevents unwanted loop-strength reduction of member addresses. Returns the defined final collider-initializer result. The field cleared at 159 is speed, not trail count.
- 081A4064: matched MotionEntry_UpdatePosition, 156 bytes. Reversing the commutative vector-add source operands closed the register differences without pins.
- 081A4100: drafted 95.1%, MotionEntry_Create. Defined null-origin local improves scratch allocation; nine register differences remain. Bounded permuter output with an uninitialized entry read was rejected; the defined null-origin alternative was reviewed and retained.
- 081A42AC: drafted 90.7%, MotionEntry_UpdateFalling (tentative), reviewed 372-byte body. Second flip-flag pointer lifetime and animation-mask allocation remain. Registered independent sibling tail 081A4420, also 372 bytes; it remains assembly.
- 081A4AB4: drafted 71.0%, Hresvelgr_EmitImpactExplosions (tentative). One centered and seven offset explosions. Observed Z jitter masks bit 9 rather than taking a 512 modulus. RNG register lifetimes and offset scheduling remain.
- 081A4DF0: matched Hresvelgr_InitializeProjectilePool (tentative), 232 bytes. Retains a defined animation-call result in r0. An inline clear helper taking both fill and destination pointers reproduces CpuSet argument scheduling.
- 081A4BF8: matched Hresvelgr_UpdateFallingProjectile (tentative), 404 bytes. Typed motion and terrain helpers match; loading phase before the resource-address local fixes the last four ordering differences.
- 080C47DC: unchanged 70.2%; pinning initial dx/dz to the diagnosed registers regressed to 63.2%.
- 080C5930: deferred after reviewing its entry/turn-state assembly and prior below-threshold reconstruction (37.1%, 443 structural differences). Repeated animation helper tails remain the obstacle; no new draft claimed.
- 080C725C: unchanged 91.3%; pointer-local sweep and RNG alias/table forms regressed. Removing RNG barriers trades extra seed stores for different allocation and instruction selection rather than a useful improvement.
- 080C6A68: unchanged 86.5%; three pointer-local deletions and eight declaration swaps did not improve its 93 realigned differences.
- 080CF3E4: unchanged 88.3%; no removable pointer locals in the target body; declaration swaps did not improve the 112 realigned differences.

The campaign wrap-up marker appeared before starting the Slime family. Stopped new matching work and began batch 2 integration immediately. Remaining assigned families were not attempted.

Batch 2 exact gain: 0.0407 percentage points (980 bytes), including 0.0174 percentage points register-pinned (420 bytes) and 0.0232 percentage points without pins (560 bytes). Three new guarded drafts add fuzzy coverage only.

Batch 2 validation: rebased onto current main, regenerated the documentation index after its merge conflict, and ran the combined ROM build. It printed `build/boktai3.gba: OK`. No code conflicts required resolution.

Campaign result at wrap-up: 14 matched functions, 11 new or improved guarded drafts, 13 unchanged/deferred functions; the remaining 52 assigned functions were not attempted after the stop marker. Exact gain is 0.1623 percentage points (3,912 bytes). Register-pinned gain is 0.1200 percentage points (2,892 bytes), and gain without pins is 0.0423 percentage points (1,020 bytes). Both batches received full ROM validation before submission.


## Batch 3: resumed Slime family

Tags: #matching #slime #register-pinning

The orchestrator removed the initial landing signal and explicitly resumed the campaign after batch 2 merged as PR 589. Branch reset to current main before continuing.

- 0808E2CC: improved guarded draft from 88.1% to 88.8%. Keeping the actor pointer alive through the health load restores one missing copy. Pointer-local sweeps and six vector-pointer constraints regressed; vector high-register lifetimes still differ.
- 0808EC80: matched Slime_ConvergeReactionMotion, 728 bytes, without concessions. A named completion-check helper and removing the old ready-field clobber fix all four register differences. depin.py removed every temporary hint; plain C remained exact after cleanup.
- 0808FA40: deferred after reviewing the full 624-byte movement-destination body and prior 71.9% reconstruction. Nested cardinal probes, rotation retries and context destination copy are understood; the earlier reconstruction is unavailable in this checkout and its structural residue remains substantial.
- 08090160: matched Slime_EmitDeathDebris, 628 bytes, register-pinned. Plain late mask initialization removes early mask loads; one separate sum local per particle fixes register lifetimes. Keeping the first palette pointer live before the initial sum fixes the high-register swap. depin.py retains two hints.
- 080903D4: unchanged 88.9%. Seven pointer-local deletions, declaration swaps, position constraints, and two inline emitter shapes did not improve the 24 structural differences. Repeated pointer spills remain unexplained.
- 08090724: unchanged 80.4%. Pointer-local deletion and declaration sweeps did not improve; shares the targeted burst pointer-spill residue and additional random-vector allocation differences.
- 08090B4C: improved guarded draft from 91.5% to 96.6%. Delaying the retained motion pointer until after masked-damage calculation fixes allocation and copy order. A signed RNG temporary with an empty constraint reproduces the signed comparison. Independent field addressing, dynamic offsets, and timer-pointer experiments did not close the remaining six structural differences.

New hint tooling required a local instrumented compiler build. Its diagnostics accurately identified reload rotation and the delayed motion-pointer assignment; single-pass flips did not improve these targets.


### Scene actor reaction continuation

Tags: #matching #scene-actors #register-pinning

- 08132904: improved guarded draft from 87.1% to 88.1%. A table-first RNG helper with a local seed index merges two seed stores and restores load ordering. Pointer and declaration sweeps did not improve its retained 76 realigned differences, 19 structural.
- 08135CC4: matched SceneActorReaction_ResetFlaggedState (tentative), 176 bytes. New full typed draft. Signed initial-field widening restores the hoisted zero and reset offsets; a shared bit local and unknown combined value reproduce the OR operand copies. depin.py reduced seven hints to one. The false branch intentionally falls through the non-void function, reproducing the observed epilogue without inventing a defined return.
- 08135F70: unchanged 88.2%. Completed pointer-local sweep, separate initial flags pointer, delayed motion pointer, shortened health-pointer lifetime, mode copy, declaration swaps and compiler-pass flips. No structural reduction; the small fuzzy-only variation increased realigned register differences and was not retained. The causes probe cannot preprocess the guarded draft directly; an unguarded scratch was used for the pass diagnostics.
- 081363F4: new complete guarded draft, 81.6%, SceneActorReaction_ResolvePlacementDamage (tentative), reviewed 1,492 bytes. Includes linked-area fallback, route resets, contact-node release, both hit-flag expansions, pending damage and escalating hit, HUD, defeat recording and final collider gating. Started at 198 realigned differences / 116 structural; explicit early-fail label, saved pending pointer helper, group-relative control reset, collider lifetime, HUD one local and hit-state memory barrier reduced this to 172 / 67. Pointer/declaration sweeps and further constrained reloads stopped improving. Raw fuzzy scoring is unstable across equivalent allocations; retained the readable full 81.6% candidate and verified its default assembly fallback exactly. The causes database is absent locally, so --causes could not run; --flips identified CSE skip-block behavior but no exact source fix.
- 08136D40: matched SceneActorPlacement_ResumeResetControl (tentative), 188 bytes, no concessions. Resumes drawable/shadow, clears collider bit four, resets route/state and restores resetControl220 with kind 12. The initial draft was 96.7%; explicitly reading control into a local before SetControl reproduces its load-before-kind order.


### Monster book

Tags: #matching #monster-book #register-pinning

- 081D5CE0: matched MonsterBook_RefreshPage (tentative), 1,204 bytes, no concessions. Full initial draft assembled the matched 081D6C4C, 081D6D20, 081D6E0C, 081D6EE4 and 081D6F98 helper bodies. Direct flag field accesses reproduce the initial show/hide addressing. Explicit BookSprite-array casts at the A8C and BAC group-digit bases stop premature loop-invariant address motion; that change closed the remaining 104 realigned / 40 structural differences exactly. Pointer sweeps, declaration sweeps and constrained/raw alternatives were tested before the cast-array fix.
- 081D61C0: matched MonsterBook_UpdateNavigation (tentative), 1,752 bytes, register-pinned. Complete reconstruction of all three modes, category/entry/tab wraparound, inline redraws, script close and delayed return to list. Initial 92.0% draft reused the exact page helpers. A switch around the pending return preserves the ROM branch target; an empty barrier after that switch prevents body reordering. The remaining 66 instruction differences were entirely stack slots: rtlprobe.py recovered PRE table size 365 and the wanted expression order requires 367. Compiler-only entry barriers reproduce that ordering. depin.py reduced nine hints to six (five entry barriers plus one layout barrier). All instructions and all resolved branch bytes are exact; the earlier single-byte intermediate-branch-target candidate was not accepted.


### Registry F3E4 / contact effects

Tags: #matching #registry-f3e4 #effects #register-pinning

- 081C3024: complete new guarded draft, initially 57.1%, retained 75.4%, ContactImpactEffect_BeginMotion (tentative), 676 bytes. Converts optional screen origins and world targets, projects the initial position, initializes the distance-wave track and independent scale ramp. A missing origin gets the observed 512 conversion offset. Coordinate modes other than zero/one leave the target unchanged. Full pointer-lifetime, returned-pointer helper, typed/unsigned projection, register and declaration experiments reached 89 realigned / 37 structural differences. Reusing the timed sibling's ordered argument aliases and callback-store helper improved 73.9% to 75.4%.
- 081C32C8: complete new SceneTarget_BeginMove guarded draft, retained 51.8%, 804 bytes. Adds a required target, uses target height when unprojecting a missing world-mode origin, initializes the arc and invokes the callback immediately. Tested returned/independent origin pointers, shortened pointer lifetimes, declaration swaps, unsigned projection, destination/ramp parameter stores and register constraints. One pinned alternative reduces structural distance substantially but worsens byte similarity; retained the readable unpinned full draft. Timed-sibling argument aliases and callback helper improved the initial 50.5% to 51.8%.
- 081C35EC: matched ContactImpactEffect_BeginTimedPosition (tentative), 276 bytes, no concessions. Screen-to-world helper is reused in both optional-origin paths. Ordered aliases for source, duration and effect reproduce the prologue, and SetCallback reproduces literal-before-slot ordering. Those source changes close the initial 93.4% candidate exactly.
- 081C371C: new complete guarded draft 93.9%, ContactImpactEffect_BeginInterpolatedPosition (tentative), 564 bytes. Coordinate modes zero and one initialize the world endpoints and project their initial position; larger modes skip endpoint writes. Projection must delay Y/Z stores until camera subtraction. Independent conversion-output and retained-origin locals reduce 32 realigned / 10 structural differences to 19 / 6. Destination pinning, declarations and compiler flips did not close the remaining copies.
- 081C3950: matched RegistryF3E4_UpdateBlinkFrame (tentative), 108 bytes, no concessions. Signed local widening preserves the state reload before writing the animation frame. A frame view based at flags offset 34 reproduces the store with offset 10. Both timer phases match exactly.
- 081C3B04: new complete guarded draft 75.9%, ContactImpactEffect_UpdateShakingPosition (tentative), 280 bytes. Projects fixed world position, marks completion at the threshold, and applies two random remainders with range timer/4 before incrementing the timer. Shared seed index preserves the first increment across the remainder call. Table constraints improve 48 realigned / 23 structural differences to 36 / 13; stronger register constraints, declaration and pointer sweeps do not close it. Retained the defined table-lifetime candidate, with its matching-concession marker.

Batch 3 has eight exact matches, adding 0.2099 percentage points (5,060 bytes). Register-pinned gain is 0.1060 percentage points (2,556 bytes), and gain without pins is 0.1039 percentage points (2,504 bytes). Eight distinct guarded drafts are new or improved. All listed exact functions passed their reviewed-size checks; combined ROM validation follows before integration.

Batch 3 validation: rebased onto current main, kept both sides of the appended history conflict, regenerated the documentation index, and ran the complete combined ROM build. It printed `build/boktai3.gba: OK`. No code conflicts required resolution.


## Batch 4 continuation

Tags: #matching #sprite-effects #wireless #map-menu #register-pinning

- 0806AB1C: matched SpriteEffectController_UpdateBouncing (tentative), 1,112 bytes. Reused the exact playback sibling and reconstructed terrain bounce, damping and intermittent gravity. Removing the long-lived velocity alias restored actor allocation; returning that pointer only through the cell helper restored its late retention. An explicit two-square sum fixes multiply order, and a narrow flags-test constraint fixes the last reload rotation. depin.py removed one unnecessary hint; final cleanup and reviewed-size check remain exact.
- 0806AF74: complete new guarded draft, 90.8%, SpriteEffectController_UpdateTileBounce (tentative), 848 bytes. Reuses bounce motion but reads base cell height without slopes, then runs tile animation, resets animation index/elapsed under the lifetime condition, and performs completion cleanup. Signed/widened mask setup, direct drawable height access and a local playback layout barrier reduced 123 realigned / 45 structural differences to 42 / 8. Tested explicit frame-pointer lifetimes, declaration swaps, entry barriers, constrained reloads and compiler-pass flips; remaining frame-address hoisting and reverse-playback pointer copies did not improve. Default assembly fallback checked exactly.
- 0803C114: improved ScenePool_UpdateOccupiedActor (tentative) guarded draft from 57.2% to 63.1%, 1,752 bytes. Recovered actor fields from the exact initialization sibling; independent anchor-array bases, delayed impulse pointer retention, explicit signed height tests and declaration order reduce 409 realigned / 151 structural differences to 372 / 129. Full sibling motion replacement, argument/actor constraints, pointer sweeps and compiler flips did not improve structure. Retained the readable unpinned typed candidate; motion-pointer lifetimes and initial control block layout remain open.
- 0816B958: matched MapMenu_DrawActiveMarkers, 312 bytes, register-pinned. Signed full-width attribute values, constrained branch constants and the returned attribute preserve both literal-to-temporary copies. A marker-array helper constrains its base and byte offset independently, fixing the final two register differences. depin.py reduced ten hints to nine; reviewed-size check remains exact after formatting.
- 0816BC54: new complete guarded draft, 72.0%, MapMenu_RefreshNavigationArrows (tentative), 1,360 bytes, unpinned. Covers revealed normal links, fallback to the current page by shared map identity, and first-available alternate links with solid/dashed arrows. Initial full draft was 53.5% with 298 realigned / 176 structural differences. Passing the page-array address, computing normal output pointers after the reveal call, unsigned bit testing, direct selected-page access and declaration order reach 217 / 111. Explicit lifetime constraints, store barriers, pointer sweeps and compiler flips did not improve the retained plain candidate. Default assembly fallback checked exactly.

Batch 4 adds 0.0591 percentage points (1,424 exact bytes), both matches in the Register-pinned category, and three new or improved guarded drafts. The real wrap-up signal appeared while finishing the navigation draft; no further function was started. Combined validation follows before integration.

Campaign checkpoint: 24 matched, 22 distinct new or improved drafts, 17 unchanged/deferred, and 27 assigned functions not yet attempted. Resume with PinkSpine_UpdateDamageAnimation (08210398), then PinkSpine_InitFromScript (08210654), followed by the original remaining effect-part, child-group, link-menu, health-restore, numeric-overlay, native-particle, controller, slot-menu, scheduled-effect, scene-script, selection-panel and HUD order. Before completing the pass, revisit all prior review-only and unchanged attempts, highest fuzzy first, as instructed. Full reconstructions remain required for the large review-only items.
