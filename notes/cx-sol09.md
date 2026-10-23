# Campaign worker sol09

Tags: #matching #axe #scene-actor

## Axe family

- 080F75C0: skipped after a fresh contact-callback draft and a pinned increasing-loop variant. Best fresh attempt 66.3%; materialized unavailable guard matches, but contact-store scheduling, hoisted phase pointers, and collider induction differ. Prior documented attempts also stalled.
- 080F79A8: drafted 93.3% (from 91.0%). Reversed X/Z addition operand order, widened direction before negation, and used pointer-first flag-clear helper. Collider-loop induction, polar offset reuse, and signed random comparison still differ.
- 080F8FFC: candidate 86.5% (from 85.9%) with reversed inline vector arguments; clear-mask helpers and removing the retained flag pointer regressed. No exact match.
- 080F93E0: retained 86.0% draft. Adjacent declaration swaps did not change output. Differences include directional arithmetic and inline flag/phase scheduling.
- 080FA214: retained 94.3% draft. Removing the actor barrier removes the extra saved constant but reuses the counter pointer; pinned offset and barrier variants regress. Final counter store must rematerialize the offset without saving it across Mod.
- 080FAA5C: skipped fresh cubic-vector draft at 24.7%. Scalarization and coefficient scheduling differ substantially.
- 080FB30C: skipped after extent review. The named routine ends before byte-encoded trailing routines within the listed 1080-byte extent; matching its first routine alone would not establish full-extent equality.
- 080FB744: skipped fresh typed palette transition draft at 51.5%; array induction and pointer allocation differ.
- 080FB984: retained 98.9% draft after index type/order and bounded permutation attempts. Only initial zero/count ordering differs; forcing the index materialization alters strength reduction and allocation.
- 080FBA38: retained 81.8% draft. Reviewed sequence stepping and emission layout; helper register pins and argument scheduling remain unresolved.
- 080FBC84: retained 85.2% draft. Same sequence-step residue as the preceding emitter; full-extent check rejects the four-byte-short candidate.
- 080FBF4C: retained 98.1% draft while investigating final byte load. Pinning its value reaches 98.7%, but also pinning the address changes stack placement. Explicit pointer constraints are unsuitable without further stack-allocation work.
- 080FC094: skipped after reviewing its defeat-effect relative and extent. The emitted routine is followed by byte-encoded bodies within the listed extent; no partial-prefix acceptance claimed.
- 080FC4F4: retained 85.3% draft. Reviewed full diff: frame is eight bytes short and collider-loop pointer spills differ.
- 080FCB64: skipped fresh sibling-based collider draft at 65.0%; rotation expressions are understood but loop/control shape differs.
- 080FCDEC: skipped fresh attack-property draft at 30.9%; compiler merges equivalent final arms, and caller widths and field-pointer lifetimes require more work.
- 080FE3B0: retained 86.1% draft. Adjacent local declaration permutations leave the target/copy/zero allocation unchanged.

## Actor status family

- 081AFAC0: contact initializer candidate improved to 97.1% with only actor pinned to r5. Pinning the status too regresses. Remaining: parameter-save ordering and one signed-load scratch.
- 081AFF98: fresh requested-state draft improved from 55.4% to 61.3% using pointer-first flags helper. Stored-state pointer reuse and hoisted active-field pointer differ.
- 081B08B8: matched, ActorStatus_UpdatePlayerApproach, 1660 bytes, Register-pinned. Reserving r3 only around the first signed player-position load lets agbcc use r1 for its zero-offset scratch. The opaque output is used only by empty constraints, never C computation. Required depinning removed many inherited pins and barriers while preserving exact bytes.
- 081B0248: skipped fresh radial-effect draft at 76.7%; origin/velocity stack allocation and RNG modulo scheduling differ.
- 081B03C0: retained 88.6% draft; full check is 36 bytes too long, with duplicate position-request setup and packed sequence stepping residue.
- 081B0F34: retained 93.1% draft; reusing the depinned playback helper leaves output unchanged and duplicated player-request blocks remain.
- 081B1334: retained 96.3% draft. Exhaustive single-hint removal found no improvement. Most differences are state/timer offset registers plus one request-state offset derivation.
- 081B18C8: retained 87.0% draft; depinned playback helper regresses allocation.
- 081B1B90: drafted 93.8% (from 90.4%) by reusing the depinned playback helper. Structural differences drop from eleven to six; the candidate remains four bytes too long.

## Task 4E01 family

- 081D75E8: skipped fresh shrinking-trail update at 36.1%; the straightforward struct-array loop scalarizes far more aggressively than the original.
- 081D7934: retained 87.7% draft; explicitly using the otherwise-unused pinned owner regresses to 83.1%.
- 081D7C68: retained 84.0% draft; same pinned-owner experiment regresses.
- 081D7F8C: retained 96.8% draft. Moving width acquisition after the second cell lookup reduces structural differences but adds allocation differences; reversing the direction-two test also regresses the combined code.
- 081D8244: retained 93.0% draft; reviewed center-vector frame and branch-sharing residue.
- 081D8CF0: drafted 83.1% from no retained draft. Reuses the existing timed-impact playback helper; grows collision bounds for twelve frames, then switches to animation eight once and expires at configured duration. Candidate is sixteen bytes short.
- 081D905C: skipped fresh forward-cone homing draft at 45.2%; repeated completion tails and nearest-target distance allocation differ.
- 081D95F0: retained 87.9% draft; reviewed full-extent mismatch, four-byte-short playback expansion.
- 081D9904: retained 90.4% draft; reviewed child-loop scheduling and twelve-byte frame/code surplus.

Search tooling: Ghidra export and the worktree's instrumented agbcc are absent, so Ghidra and translate/flip cannot currently run. Bounded permuter searches on the sprite initializer, waypoint selector, and player approach yielded no better candidates. A relatives port invocation unexpectedly scanned all functions; its one out-of-assignment generated source was removed immediately and no such work was committed.

## Spider family

- 080A21EC: skipped fresh cardinal-turn draft at 44.6%; full extent is correct, but four inline tile animation arms share branches differently. Prior attempts documented the same difficulty.
- 080A28A8: matched, Spider_UpdateContactWindup (tentative), 400 bytes, Register-pinned. Move the frame-register copy inside the flag guard; force behavior twelve and phase twenty-six to materialize before stores; retain the counter-offset lifetime; constrain callback and final group-field addresses. These reduce six structural differences to zero and then remove the remaining register differences. Required depinning follows before commit.
- 080A2AEC: retained 83.1% landing draft; declaration swaps do not improve the pointer lifetimes or eight-byte code surplus.
- 080A34BC: skipped fresh recovery-wander draft at 62.6%, adapted from exact recovery-route helpers. RNG branching and repeated phase packets differ.
- 080A3A30: retained 90.5% pursuit draft; declaration swaps have no effect on mesh and route-pointer scheduling.
- 080A4018: retained 80.3% wander-goal draft; RNG value scheduling and offset allocation remain substantially different.
- 080A4334: retained 97.2% player-approach draft. Removing the cached position, changing the player barrier, and forcing rematerialization regress; the cached-player lifetime crosses both inline vector differences.
- 080A4798: skipped fresh retreat-point selector at 28.0%; packed-vector stack layout and conditional point advancement differ.

## Target and contact control family

- 0807E9D0: skipped after assembly, condensed-run, and relative review. This large contact callback has 854 instructions; its closest exact relative explains only eleven percent. Player-target selection appears twice alongside contact reaction and emission loops. No complete new candidate was produced.
- 0807F130: skipped after full assembly and relative review. Target/stimulus selection, route response setup, cell-height lookup, and navigation results share only limited exact-relative runs; no complete candidate was produced.
- 0807F81C: skipped fresh contact-selection/target-loss response draft at 48.8%; callback, node, route, and reset-packet allocation differ.
- 0807FCA8: drafted 85.7% from no retained draft, SceneActorControl_UpdateContactRelease (tentative). Moving node-address construction after entry, reversing vector helper arguments, reading facing before atan2, and using a sequence view recover the correct 280-byte extent. Pinning only actor r6 improves the saved registers. Pinning frame too removes the original's otherwise-unused angle-normalization residue and regresses. Remaining: frame/node allocation, facing-load scheduling, and mask order.

## First batch integration

Two exact matches, both Register-pinned, add 0.0855 percentage points of code coverage (2060 bytes). Five guarded drafts improve retained candidates or add new starting points. The pending player-trail candidate has only a commutative multiply register difference and is preserved locally for the next batch.
