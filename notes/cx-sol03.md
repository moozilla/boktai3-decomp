# cx-sol03 campaign

Tags: #matching #armor #campaign

- 08182454 ActorController_SetAnimation: matched, 232 bytes. Side-selection scratch lifetime and call argument scheduling required local register hints; depin reduced 23 hints to 9. Exact full-extent check passed.
- 08186CC8 Armor_SelectAlignedAttack: skipped; flag and magnitude allocation variants did not yield a reliable improvement. Existing 80.6% draft retained. Tail merging and decision-pointer lifetimes remain.
- 08186EB4 Armor_SelectTimedAttack: matched, 892 bytes, no register hints. Replaced a shared strike label with explicit calls and changed near/far branch polarity. Temporary barriers exposed the layout issue and were all removed before acceptance.
- 08187230 / 081875EC: skipped; five bounded magnitude, delta and mode-lifetime variants retained 85.8% drafts. Removing the mode pin reduced structural differences but worsened the full comparison.
- 081879A8: skipped after fresh full-body reconstruction and coordinate/magnitude/delta lifetime trials. Ordinary candidate reached 64.8%, with substantial shared-tail and stack-lifetime differences; no production draft.
- 08187F44: skipped after porting the melee reconstruction and adding four random strike choices. Initial candidate 70.9%; repair identified mask four instead of one. Body layout remains substantially different.
- 081885BC / 08188B7C / 0818911C: deferred after assembly, history and relative review. Six-way defensive switches and boosted attack transitions need separate reconstruction; matched relatives explain only small fragments. Prior below-threshold attempts remain documented in earlier notes.
- 081896D0: skipped, existing 81.5% retained. Declaration sweep offered no useful local swaps; repair did not compile this guarded named draft.
- 08189D04: skipped, existing 82.5% retained. Expanding the strike tail as in the timed sibling worsened allocation; removing pins also worsened it. Local-pointer sweep found no gain.
- 08189FF8 / 0818A820: deferred after assembly and prior-attempt review; the former shares timer setup but has a distinct pursuit switch, the latter begins with a five-way message dispatch. Neither has an eligible starting draft in this checkout.
- 0818A500: skipped, 82.9% retained. Direct loop-count addressing worsened layout.
- 0818C6F8: skipped, 83.2% retained; pointer sweep found no removable address local.
- 0818E5E4: skipped, 85.5% retained; removing the per-part pointer worsened matching substantially.
- 0818E9B8 SpriteColliderGroup_ActivateParts: matched, 100 bytes. Hoisted activation constant, retained active-count pointer, and explicit stride lifetime recover the original loop. Depin reduced four hints to two.
- 0818EA3C SpriteColliderGroup_ResetActiveParts: matched, 84 bytes. A do/while loop avoids the duplicate count test introduced by the initial-count register hint. One hint remains after depin.
- 08021F4C SceneActorGraphics_OnDamageContact: new guarded draft 84.5%, 652-byte extent. Reconstructed damage response, contact metadata, palette feedback and wireless item rewards. Callback argument lifetimes and source/address scheduling remain. Recorded the isolated guarded source with check.py; the containing unit's default assembly path checks exact. Whole-unit NONMATCHING checking is blocked by pre-existing conflicting declarations in the separate command-dispatch draft.
- 08022624: skipped after typed box-search reconstruction, index/base barriers, parameter pins and five inline-argument permutations. Best candidate 69.1%; table-base hoisting and min/max register allocation remain.
- 08022694: skipped after scatter reconstruction and RNG-expression/loop-lifetime variants, best 74.2%. Entry r4/r5 reads arise from packed local vector updates, not evidence for hidden arguments; all vector components are initialized before use.
- 08022790: skipped after position-copy, range and three-sample RNG reconstruction. Typed pointer to the position fixed scalarized coordinate updates, reaching 74.4%; RNG store timing and vector/register lifetimes remain.
- 08022878: skipped after full radial-scatter reconstruction, 43.1%. Vector normalization and packed temporary handling remain far from the original.
- 08022B18: deferred after full assembly review. Three reaction installers share action-controller resets, damage display and wireless reward behavior; needs a dedicated typed reconstruction.
- 08022DBC SceneActorState_SelectReactionPhase: matched, 232 bytes. Distinct phase-choice tails plus explicit phase-field lifetime reproduce the seven-way selector. Depin reduced fifteen hints to four.
- 08022EA4 SceneActorState_SnapReactionPosition: matched, 228 bytes. Three empty compiler barriers preserve otherwise merged repeated calls. Whole containing unit checks exact.
- 08022F88: skipped, existing 81.1% retained. Isolated the guarded dispatcher and removed conflicting declarations only in scratch to make it checkable. Pointer sweep and sb/r8 entry/index pins did not improve it.
- 08023FE8: deferred after assembly/history review; packed polar-vector and six-way reaction setup needs a separate reconstruction.
- 080243CC: skipped after typed recovery reconstruction and inline-entry/ip lifetime trial; best 48.2%, below the earlier documented attempt. No production edit.
- 0802483C SceneActorState_UpdateVariantRecovery: matched, 264 bytes. Corrected shared timer increment on both animation-selection paths. A small offset helper and scoped animation results establish the desired branch layout; depin reduced fourteen hints to two. Whole containing unit checks exact.
- 080924EC Bee_UpdateMovementStop: matched, 388 bytes. Explicit lifetimes for collision flag pointers, current/prior offsets and reset zeros recover the scratch registers and remove a duplicated context address. Depin reduced fifty-one hints to fifteen.
- 080929A4 Bee_StartMoveToBody: improved guarded draft 86.7% to 89.2%. A volatile local fill word recovers the original stack ordering; a value-before-address item-ID store reduces structural residue. Result-register/clobber and aggregate-fill trials did not improve further. Recorded guarded score and exact default assembly path.
- 08093138: skipped after full Hermite-style route reconstruction, 36.2%. Packed start/end/zero-vector scalarization and polynomial scheduling dominate. No eligible production draft.
- 0809345C / 080941F4: deferred after assembly/history review. Route construction and target-route control remain far from the retained wander-route seed in the prior documented attempts.
- 08093810: skipped, 82.1% retained. Declaration sweep did not improve it. Repair proposed deleting the route angle's 128 bias, yielding 82.5%, but this needs semantic review against the original random-heading path before production use.
- 081E42C4 Actor_UpdateIdleWait: matched, 292 bytes. Separate phase-load/store address lifetimes and scoped flag/reset values recover the original callback. Depin reduced twenty-seven isolated-unit hints to five, including shared helper hints. Whole unit checks exact.
- 081E728C: deferred after assembly and matched-relative review. The scripted-curve sibling explains 54%, including two copies of its packed animation update; the remaining movement setup and branches require a dedicated reconstruction.
- 081E9BA4: skipped after full visibility/resource-loop reconstruction, 40.4%. The target keeps distinct count, resource-array and child-flags induction lifetimes; the C coalesces these too aggressively. No production edit.
- 081E9E44: deferred after assembly/history review; script-keyword child configuration includes several long state-specific branches.
- 081EA4DC: skipped after controller-constructor reconstruction, 59.6%. Child-control reload and sprite/palette pointer lifetime variants did not improve it. No qualifying production draft.
- 08165D10: first-pass check retained existing 81.7% palette-blend draft. Frame/duration spilling remains the main issue; resume after integration.
- 0816D634 PauseMenu_ApplyItemSlot: matched, 452 bytes. One empty compiler barrier preserves the separate timer-reset tails. The final resolved-byte discrepancy exposed a real error in the prior draft: four item IDs double both effect values, not only the first. Corrected and checked the complete extent; the barrier remains necessary after depin.

## Batch 1

Ten exact matches add approximately 0.1313 percentage points of code coverage
(3,164 bytes); two guarded drafts were added or improved. One match uses no
register hints; the others retain only the hints required by depin. This batch
is gated by a combined ROM build before integration. Functions marked deferred
above received static review but not a completed fresh matching attempt; return
to them after the first pass through the remaining assignment.
