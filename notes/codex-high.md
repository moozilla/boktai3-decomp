# Codex high: task 0808AF38

Tags: #matching #ghoul #slime #register-pinning

Branch: `cloud/0808AF38`. Worked solo, offline, using the worktree's Python
and per-file checker only. No full build, generated-file edits, network actions,
or direct Git mutations. Commits were queued through `build/commit.sh`.

## Results

- **0808E838 matched**, `Slime_UpdateRandomWait`, 360 bytes, register-pinned.
  Consumes `field2B2`, plays profile entry 15, chooses a duration from RNG bits
  0–5 (zero becomes 64), and selects 0808E9A0 with control kind 0x23 when
  `field2B3` is set. Both preceding 0808E660 and following 0808E9A0 reference
  this state; the adjacent setup functions identify the Slime family.
  The animation selector is an inlined specialization of the shared profile
  animation logic at 08076818. Keeping the selection-address load separate
  from the saved pointer reproduced the otherwise missing register copy.
  An empty constraint on the drawable and narrow register hints reproduced
  the surviving constant-mask branch. `tools/depin.py --write` reduced 14
  hints to five; formatting and a final `check.py --target-size 360` retained
  byte equality. No handwritten instruction bodies.
- **0808EC80 drafted 88.6%**, `Slime_ConvergeReactionMotion`, reviewed extent
  728 bytes. On entry quarters secondary X/Z motion and selects profile entry
  three. Each update converges the saved X/Z displacement by a clamped
  quarter-step; once settled and animation-ready, health/context flags select
  idle control or 0808E660. The matched shared relative 08082D80 explains the
  convergence arithmetic. Remaining realigned difference: 40 instructions,
  four ignoring registers. The principal allocation mismatch is the flags
  pointer versus saved-X pointer; one pointer copy and a boolean copy also
  differ. Explicit 32-bit mask temporaries avoid unwanted narrow constants;
  a typed sequence view preserves the halfword timer store at offset 14.
- **0808E2CC drafted 88.1%**, `Slime_ResolveBodyContact`, reviewed extent
  640 bytes, candidate 632 bytes. Registered by `Slime_InitColliders`
  (0808F840) as its body callback. Rejects unavailable actors; contact flag
  0x800 adjusts movement relative to the terrain-cell center and cardinal
  direction, arms the primary collider if the destination ground height
  differs, otherwise moves X/Z and clears their velocity. Other contacts
  forward to `SceneActorBehavior_ApplyContactReaction`. Remaining realigned
  difference: 45 instructions, four ignoring registers. These four are the
  health-address copy and three high-register motion-pointer copies in the
  initial vector addition. Reversing the source operands of the shared Add
  inline fixed load order; an inline signed angle-division helper removed
  early result copies. Pinning the whole vector pointers made allocation
  worse, so that variant was discarded.
- **0808CC48 drafted 86.1%**, `Ghoul_ResumePendingRoute`, reviewed extent
  508 bytes, candidate 504 bytes. Called first by `Ghoul_Update` (0808D16C).
  Context bit zero at 0x628 requests a restart after the 0x648 countdown.
  It resets the actor from the route origin, saves a 30-frame delayed control,
  starts the route, attempts movement to context position 0x62C, and consumes
  the pending bit. Remaining realigned difference: 36 instructions, twelve
  ignoring registers. Cached route-slot and motion-flag pointer lifetimes
  differ. Returning the pending pointer through an inline output parameter
  reproduces its stack slot better than computing it twice. Mask temporaries
  keep the intended 32-bit arithmetic before halfword stores.
- **0808AF38 stopped below draft threshold**. Best complete candidate was
  66.7% fuzzy (candidate 816 bytes versus body/pool extent 812), with 154
  realigned differing instructions, 60 ignoring registers. Tentative role:
  `Ghoul_SelectRetreatRoute`. Computes a target-facing vector using Atan2
  and the sine table, scans route points for a negative dot product, excludes
  the current point, otherwise chooses a random different point, snaps its
  terrain height and attempts movement. Important original quirk: a rejected
  point in the negative-dot-product branch advances the index without
  advancing the point pointer. Candidate preserves that behavior. Bitfield
  vector writes plus signed-halfword pointer reads in the dot-product inline
  recover stack storage, but origin-coordinate hoisting/spills and route
  failure/control-reset allocation remain substantially different. Explicit
  volatile origin locals and broader pointer pinning regressed. Scratch
  candidates remain only in ignored `wip/`; no below-threshold source added.
- **0808B3EC stopped after structural investigation**, no qualifying draft.
  This 2,738-instruction callback is installed during the Ghoul initializer.
  Read with `asmat.py`, `condense.py`, and `relatives.py`; the Ghidra export is
  absent in this checkout. Three repeated chase/arrival expansions dominate
  the body, interleaved with sound, reaction flags and Ghoul-specific paths.
  `include/scene_actor_chase.h` supplies useful `Chase_Arrive`, terrain height,
  route and state/control templates; 080E4468 explains 38% of the target's
  instruction runs, with further matches in 080CEDF0 and 080C4A9C. The repeated
  expansions start around the terrain snap sequences at 0808B4B0, 0808BD54
  and the later third path. No whole-function candidate was ready to check;
  assembly remains authoritative.

## Validation and retention

The three drafts are guarded by `NONMATCHING`, retain `INCLUDE_ASM` in their
`#else`, and have scores recorded with `check.py --nonmatching --record` using
the reviewed extents above. Their default assembly paths also passed per-file
checks. Depinning was attempted before wrapping; the minimizer only accepts
exact candidates and skipped the three drafts. One exact match and three
drafts were committed separately. Names and inferred behavioral roles are
provisional, with file-local aliases and low-confidence comments. Full-ROM
batch validation remains for the integrator.

# Codex high: task 080DEA7C (2026-10-20)

Tags: #matching #dog #terrain #register-pinning

Branch: `cloud/080DEA7C`. Worked solo and offline. Used the worktree Python,
`asmat.py`, `condense.py`, `relatives.py`, `check.py`, `variants.py`, and
`rtlprobe.py loop`; no full build or network operation. All commits went
through `build/commit.sh`. Ghidra pseudo-C is unavailable (`gen/ghidra/decomp.c`
is absent). An unrelated driver change in `tools/cloud_task.py` was left alone.

Exact coverage of this task's reviewed body/pool extents is **3.46%
(+3.46 percentage points; 436 of 12,592 bytes)**, all in the Register-pinned
category. Drafts count as zero exact progress.

- **080E059C matched**, `Dog_ResumeBehaviorAfterAnimation`, 436 bytes.
  Consumes `field2B2`, selects directional animation 11, clears `field2B3`,
  then waits for that byte. On completion it either restores chase control
  (kind 26, behavior 12, group flag four and a 32-frame delay), or installs
  idle control (kind 16, behavior seven, flags2CC bit four). The chase path
  snaps X/Z using the collider at actor offset 0x5D0. Initial draft was 95.8%.
  A named u16 local in the motion-mask accessor fixed its operand allocation;
  spelling collider accesses in the call arguments fixed argument evaluation
  order. One `r0` hint on the group-flag mask fixes the last two instructions.
  `depin.py --write` retained that single hint. Formatted and accepted by
  `check.py --target-size 436` with zero differing bytes.
- **080DEA7C drafted 99.4%**, `Dog_UpdateCollisionBounds`, reviewed extent
  680 bytes. Called by matched `Dog_FinishFrame` (080E41D4). Quantizes facing
  to a cardinal heading, offsets a context collider by a short or long sine
  displacement depending on context flag two, sets its direction, and updates
  actor/context collider bounds. The nearby matched 080B1038 is a useful
  forward-collider relative. A crucial source idiom is copying a packed union
  through `*(struct ContactPosition *)&bounds`, with a collider pointer local;
  a direct union-member copy scalarized the sizes into registers and scored
  only 63.5%. Returning the context flag address through an inline output
  parameter recovered the original pointer copy and reached 99.4%. Remaining
  structure: the availability test loads 0x2D0 separately instead of subtracting
  four from its 0x2D4 offset, shifting an early pool alignment (three realigned
  instructions). An empty offset constraint fixes structure but introduces
  nine register differences; pin/clobber/width variants and a bounded
  60-second permuter attempt did not close it. Retained the 99.4% unpinned
  draft rather than those alternatives.
- **080E25FC drafted 83.0%**, `Dog_ChooseReactionBehavior`, reviewed extent
  4,500 bytes, candidate 4,472 bytes. Plays reaction-dependent sound, rejects
  unavailable/timed actors, chooses special reaction control or chase/route
  recovery, and handles a timed direction change. It includes two expansions
  of terrain snapping, target-cell comparison, rerouting and movement tests.
  Context flag 0x678 bit three is cleared on the special reaction path;
  that path installs `reactionTarget230`, not `entryTarget224`. Started at
  78.2%. Shared templates come from `scene_actor_chase.h` and matched
  080E4468, with 080CEDF0/080C4A9C/080E1D48 explaining other runs. Moving the
  inline failure callback to the reroute tail and spelling each outer failed
  movement callback/return at its use fixed block order (79.6% to 81.8%).
  A u16 motion-mask temporary and compact local helper definitions reached
  83.0%, with 426 differing instructions, 162 ignoring registers. Remaining
  issues are flag-address lifetimes, RNG/turn-loop allocation, and control
  reset/address reuse. Broad register hints regressed and were discarded.
- **080E4B34 drafted 82.1%**, `Dog_BeginDelayedRouteRecovery`, reviewed extent
  1,976 bytes, candidate 1,964 bytes. On entry restores the saved control,
  clears chase/movement flags, runs the action callback and shared reaction
  gate, then waits until the signed behavior counter exceeds twenty. It saves
  a group recovery packet, installs `SceneActorReaction_RestoreAfterDelay`
  with kind 14/state three/phase one, and expands the same target reroute
  operation as 080E25FC. Group +0x46 supplies a halfword saved at packet +12;
  the group control at +0x48 goes to packet +16. Final realigned difference:
  195 instructions, 46 ignoring registers. Shared inline allocation and a
  few flag/address-update shapes remain different.
- **080E4834 drafted 80.5%**, `Dog_ResumeRouteAfterAction`, reviewed extent
  768 bytes, candidate 764 bytes. Starts control 080E04D0/kind ten on entry,
  waits for the action callback and shared reaction gate, then attempts motion
  to context position 0x6D8. Success sets motion flag 0x200 and either resumes
  group chase or snaps to the saved route and selects behavior seven. Final
  realigned difference: 90 instructions, nineteen ignoring registers. The
  original keeps distinct byte/word zero lifetimes and reuses field-address
  offsets differently. A parameter-width helper variant emitted identical
  code and was discarded. The commit message used the earlier candidate-
  length score 80.2%; the recorded reviewed-extent score is 80.5%.
- **080E0750 stopped below the draft threshold**, tentative
  `Dog_ResumeBehaviorAfterMotion`. Reviewed extent 4,232 bytes; best retained
  complete candidate is 4,212 bytes and 77.5% fuzzy, with 557 realigned
  differences, 170 ignoring registers. Entry quarters secondary X/Z motion
  and selects animation three; subsequent frames converge saved displacement
  using quarter-steps clamped to +/-200. Once settled and animation-ready,
  special reaction flags or the two chase/route recovery expansions decide
  the next behavior. Matched 08082D80 explains the convergence arithmetic.
  Loading each current coordinate before forming its saved-coordinate pointer
  improved structure. Explicit shared flag-pointer parameters improved the
  structural count to 165 but worsened allocation/alignment similarity;
  declaration swaps did not help. Specializing the two recovery copies with
  sb/sl group pins regressed substantially. The better unpinned candidate
  remains only in ignored `wip/sub_080E0750.c`; no below-threshold source was
  added to production.

All four drafts have file-local descriptive aliases, low-confidence comments,
`NONMATCHING` guards and original `INCLUDE_ASM` alternatives. Their fuzzy scores
were recorded with `--nonmatching --record` and reviewed full extents. The four
default assembly alternatives also passed full-extent per-file checks. The one
match and four drafts were committed separately; this note is committed last.
Complete combined-ROM validation remains with the batch integrator.
