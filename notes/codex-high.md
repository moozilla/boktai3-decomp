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

# Codex high: task 080B8B84 (2026-10-20)

Tags: #matching #skeleton #terrain #register-pinning

Branch: `cloud/080B8B84`. Worked solo and offline, with per-file checks only;
no make, full build, network operation, or direct git mutation. All commits used
`build/commit.sh`. The pre-existing driver edit in `tools/cloud_task.py` was left
alone. Read the existing guarded candidates before drafting; inspected callers,
callees, condensed listings and matched relatives. Used `variants.py` to rank
structural changes and `rtlprobe.py` loop/perline/dump diagnostics for the large
inlines. The two accepted matches cover **6.52% of the task (+6.52 percentage
points; 816 of 12,524 reviewed bytes)**, all Register-pinned. This adds about
**0.034 percentage points** of repository code coverage. The draft adds no exact
coverage.

- **080B8B84 matched**, `SceneActorReaction_TrySink`, 400 bytes. Retained the
  existing semantic name: a live, enabled actor on a sinking terrain cell switches
  to its sink callback when its adjusted height is at or below ground. The prior
  draft checked at 98.5%, with three structural differences from rematerializing
  the flags2D0 offset. A local r2 offset derived as 0x2D4 minus four and an r0
  pointer reproduce the original instruction sequence. `depin.py --write`
  removed the initial mask hint and empty constraint, retaining only those two
  pins. Formatted and checked over the complete 400-byte extent: zero differing
  bytes. Committed separately.
- **080BB61C matched**, `Skeleton_BeginKnockedFlight`, 416 bytes. Entry disables
  body/context colliders, launches along the facing direction with speed 28..43
  and upward speed 24..55, selects animation 17 and enables the shadow. Landing
  stops horizontal secondary motion; flags2D0 bit 0x10 triggers the optional
  bone sound and actor reset. Existing draft started at 86.3%, with two structural
  and 29 total differences. A u8 angle formal recovers the argument-copy order.
  Keeping the random value in r1 and the speed in r4 prevents the compiler from
  folding their distinct lifetimes into one register. Actor r5 and the second
  RNG table r2 complete allocation. The pre-existing dead 0x0403B714 literal is
  retained with an empty constraint, without instruction text. A bounded
  60-second permuter search saved no candidate. `depin.py --write` reduced six
  hints to five. Formatted and checked over all 416 bytes: zero differing bytes.
  Committed separately.
- **080BBDD0 drafted 87.1%**, `Skeleton_ConvergeDisplacedPosition` (tentative),
  reviewed extent 4,116 bytes; candidate 4,124 bytes. Skeleton creation installs
  this callback alongside Skeleton_Update and its defeat/idle callbacks. It
  converges saved X/Z displacement toward the anchor in quarter steps clamped
  to +/-200, then resumes route/chase control. Existing draft checked at 80.0%,
  with 610 total/96 structural differences. A signed 16-bit bitfield for clearing
  group flag four was the main improvement (308 total/60 structural on its own).
  The explicit retry exit, reading player index before the -1 sentinel, narrow
  motion-mask accessor, signed short flag updates and a shared quarter-result
  variable brought the final realigned count to **316 total/44 structural**.
  Whole-header replacement with `scene_actor_chase.h` regressed and was rejected.
  Remaining differences are flag/address lifetimes, retry-loop allocation and
  shared recovery stack slots. No register hints remain in this draft. Added
  the file-local alias and mismatch comment, formatted, recorded with
  `check.py --nonmatching --record`, and verified the default INCLUDE_ASM path
  over the full 4,116 bytes. Committed separately.
- **080B9158 stopped below threshold**, tentative
  `Skeleton_UpdateKnockedLanding`. Complete new candidate remains only in
  ignored `wip/sub_080B9158.c`: 3,724 bytes versus the 3,768-byte reviewed extent,
  34.3% fuzzy, **717 total/126 structural** differences. Entry chooses a usable
  neighboring direction through 080B713C and starts a short upward launch. If no
  direction is available, or after the landing flag, it selects animation 15,
  installs chase control and expands route recovery. This includes two full
  terrain/target/turn expansions. Starting with the rise-state helpers gave
  806/185 differences; the displacement-state helpers reduced structure further.
  Direct actor group accesses, byte-index load ordering and narrow state formals
  helped. Shared-header replacement and forcing the outer actor to sb regressed.
  Loop diagnostics confirmed both retry loops' invariant motion; remaining
  differences need more work on inline pointer/control lifetimes and allocation.
  No below-80% draft was added to src.
- **080BA268 stopped below threshold**, tentative
  `Skeleton_ReleaseBoneProjectiles`. New candidate remains only in ignored
  `wip/sub_080BA268.c`: 764 versus 784 bytes, 68.8% fuzzy, **126 total/34
  structural** differences. Selects animation 22, constructs a forward vector
  and two randomized bone-effect launches (shared D45E/1DC3 resources), changes
  to category five, reinitializes variant motion, and sets the attack collider
  from the actor's BA/C2/D8/CA fields. Animation completion installs the threshold
  reaction. First candidate was 56.8% with 187/64 differences. A signed Scale6
  inline preserves the otherwise optimized-away signed division branches;
  collecting collider arguments before the call recovers load/evaluation order;
  separating RNG sampling from masking recovers the table-load order. Parameter
  and register experiments, declaration swaps, and RTL inspection did not resolve
  the remaining polar-copy, RNG-mask, literal-load and allocation differences.
  No below-80% draft was added to src.
- **080BF0E4 stopped below threshold**, tentative
  `Skeleton_ResumeChaseAfterRising`. Existing production draft was left unchanged;
  improved candidate remains only in ignored `wip/sub_080BF0E4.c`: 3,028 versus
  3,040 bytes, 73.8% fuzzy, **426 total/162 structural** differences. Category
  five/six/seven choose directional rise animations 0x63/0x65/0x67 or delegate
  to the animation-B completion callback; completion expands the shared recovery
  sequence. Initial production candidate was 72.7%, 473/164. Player-index load
  ordering and the signed group-flag bitfield helped. Replacing the directional
  macro with an inline function gave 452/170 and was discarded. Narrow selection,
  variant changes and actor/selection pins did not improve structure. The packed
  directional-animation expansion and its address/selection lifetimes remain the
  main blockers; no below-80% source was newly committed.

The two matches and improved guarded draft have separate commits. Full combined
ROM validation remains with the integrator. Scratch candidates and diagnostic
artifacts stay under ignored `wip/` and `build/` paths.

Final caller-label audit confirmed the installer of 080BBDD0 is `Skeleton_Init`;
corrected the tentative alias's evidence comment accordingly (comment-only).

# Codex high: task 080C6F58 (2026-10-20)

Tags: #matching #golem #terrain #register-pinning

Branch: `cloud/080C6F58`. Worked solo and offline, using the worktree Python
and fast per-file checks only. No make, full build, network actions, or direct
Git mutations. All commits used `build/commit.sh`. The existing driver edit in
`tools/cloud_task.py` was left untouched. Read existing guarded drafts, callers,
callees, Golem initialization and behavior tables before naming the functions.

The two exact matches cover **12.89% of this task (+12.89 percentage points;
1,780 of 13,812 reviewed body/pool bytes)**, all in the Register-pinned category.
The four drafts add no exact coverage. Shared totals were not regenerated.

- **080C6F58 matched**, `Golem_UpdateChargeWindup`, reviewed extent 772 bytes.
  Installed by Golem_ChargeOrApproach (080CDF14) and Golem_ChargeOrReturnHome
  (080CEDF0) as control kind 0x28. Resets the animation-completion byte and
  context halfword at 0x634, selects animation eight, emits randomized dust
  every fourth frame, and ramps stored speed by sixteen while it is at most
  383. The original uses the cosine term for both particle X and Z; this
  unusual behavior is preserved. Starting from the recoil animation expansion
  gave a complete 90.5% candidate. Integer RAM-table references via the existing
  RANDOM_TABLE_ADDR macro restored separate literal loads. Keeping the flags
  pointer in a short-lived low-register local before retaining it across the
  particle call recovered the original copy order. A low-register flags-load
  temporary also forces the original duration-scale offset reload. The last
  six differences disappeared when the original unused 0x0403B714 literal's
  lifetime extended through the velocity calculation; it is never dereferenced.
  `depin.py --write` reduced thirteen compiler hints to four. Formatted and
  accepted by `check.py --target-size 772` with zero differing bytes.
- **080C75B8 matched**, `Golem_UpdateRecoil`, reviewed extent 1,008 bytes.
  Installed by the neighboring Golem reaction behavior (080CECA4). Selects
  animation eight, decays stored recoil speed by a category-dependent amount,
  converges on the saved position along the selected axis, then waits eight
  frames and flags completion. Existing candidate checked at 90.1%. Explicit
  read-before-pointer-copy ordering for the duration-scale fallback recovers
  a missing pointer copy; an empty actor constraint preserves the ROM's scale
  reload. A short-lived initial flags pointer, a separate animation zero,
  and operand order on the flag OR recover the other lifetimes. Temporary
  pins and constraints reached equality, then `depin.py --write` left only
  three hints (two register declarations and the empty actor constraint).
  Formatted and accepted over all 1,008 bytes with zero differing bytes.
- **080C8774 drafted 95.8%**, `Golem_ResumeBehaviorAfterDisplacement`, reviewed
  extent 4,304 bytes; candidate 4,296 bytes. Golem initialization installs this
  displacement-recovery callback. Converges saved X/Z displacement in clamped
  quarter steps, then resumes a completion callback or chase/route behavior.
  The retained candidate already had the strongest shared arrival expansion.
  Added a descriptive alias, current score and mismatch comment. Remaining
  realigned differences: **73 total / 12 structural**, concentrated in the two
  random-direction loops and PRE stack slots. Tested RNG argument orders,
  explicit retry exits, mask widths, integer RAM references and scoped pins;
  none improved the structural count. `rtlprobe.py loop` confirmed four
  hoisted constants per loop. A diagnostic forced-GCSE-table sweep over
  701..901 never improved the twelve structural differences; compiler knobs
  are diagnostic only and are not part of the retained source.
- **080CCD7C drafted 80.4%**, `Golem_UpdateChargePursuit`, reviewed extent
  4,504 bytes; candidate 4,508 bytes. Entry in Golem's behavior table at
  gUnk_0860613C. Follows/retargets the charge heading, drains stamina when
  turning or recovering, and falls back to shared chase/route recovery.
  Existing candidate started at 79.3%, 574 total / 167 structural differences.
  Keeping separate charge stores in the single-player and multiplayer branches
  recovers the original branch layout; one outer shared store had deleted
  important duplicated code. A command helper taking payload, kind, then
  duration recovers the load order of both delayed-recovery packets. Final
  differences: **539 total / 141 structural**. Remaining work concerns the
  repeated turn/control-reset expansions and shared flag/pointer lifetimes.
  Slot-index, signed bitfield, success-first routing and broader pin variants
  did not improve the selected candidate. No register hints remain here.
- **080CE188 drafted 90.3%**, `Golem_ResumeRouteAfterControl`, reviewed extent
  2,364 bytes; candidate 2,368 bytes. Another gUnk_0860613C behavior entry.
  After control completion, sets a delayed recovery packet, translates route
  bounds, then chooses nearest-participant chase or the saved route point.
  Porting the compatible shared helpers from 080C8774 raised the original
  78.0% draft to 89.4%; ordering command-helper arguments as payload/kind/
  duration reached 90.3%. Final differences: **114 total / 34 structural**.
  Outer group-pointer allocation, the RNG loop and flag-reset lifetimes still
  differ. Pinning the whole group pointer regressed and was discarded.
- **080C725C drafted 89.1%**, `Golem_UpdateSlideImpact`, reviewed extent
  860 bytes; candidate 864 bytes. Golem_BeginSlideState (080CEB48) installs
  it as kind 0x29 with speed 0x200. Pushes along the stored heading, emits
  dust every fourth frame, and resolves an impact into completion or defeat.
  Existing draft started at 68.2%. Independent extent RNG calls, the existing
  RAM-table macro and the original unused 0x0403B714 literal reached 76.4%.
  A final **u16 to u32 return-width change in NextRandom** raised that to
  89.1%, reducing 109 total / 28 structural differences to **54 / 23**.
  The wider inline result removes narrowing that distorts register allocation
  around packed extent stores. Attempts to pin the extent table, hold its
  halfword-preservation mask, or cache all three RNG draws together regressed.
  Retained the wider-result candidate with the register-pinning concession
  marker; remaining differences concern motion-pointer copies, RNG allocation
  and completion/control-reset offsets.

Both matches and all four guarded drafts have file-local descriptive aliases
with tentative naming evidence. The drafts retain their INCLUDE_ASM alternatives
and scores recorded by `check.py --nonmatching --record`. Each of the four
normal assembly paths also passed a full-extent fast check. No handwritten
instruction bodies or generated assembly were committed. Each function has a
separate commit; full combined-ROM validation remains with the batch integrator.
Scratch candidates and diagnostic artifacts remain under ignored `wip/` and
`build/` directories. This notes update is committed last.

# Codex high: task 080C12FC (2026-10-20)

Tags: #matching #golem #register-pinning #nonmatching

Branch: `cloud/080C12FC`. Worked solo and offline. Exact recovery is **13.39% of
this task's reviewed code, +13.39 percentage points** (1,312 exact bytes), all
in the Register-pinned category. Against the checked-in project denominator,
this batch contributes **+0.0543 percentage points**. Drafts receive no exact
credit. Each accepted function has its own driver-created commit; this notes
update is queued last.

- **080C12FC drafted 80.2%**, `Golem_ReactToContact`, tentative name based on
  contact flags selecting chase, roar, or the reaction control. This was an
  existing guarded candidate, recorded at 51.1%; today's checker measures it
  at 80.2% without changing its body. Added its required descriptive alias,
  formatted it, and recorded the current score. Final realigned difference:
  **739 total / 315 structural**. Read BIG_FUNCTIONS, the condensed listing,
  matched relatives and the shared chase header. Most residue is in the three
  Chase_Arrive expansions: callback branch layout, retry-loop allocation and
  saved pointer/flag lifetimes. The far tail 080C23FC is already included in
  the reviewed function extent; no boundary changes were needed. Shared
  headers were left untouched.
- **080C6A68 drafted 85.9%**, `Golem_UpdateChargeRecovery`. Ported the matched
  Golem_UpdateRecoil (080C75B8) animation and three-phase recovery structure.
  Entry clears completion/restart, saves the motion position, and captures the
  heading quadrant. Phase zero reduces speed by 40 and checks whether the
  projected X/Z step stays in the current terrain cell. Phase one approaches
  the current cell center along the stored heading axis, clamped to +/-8.
  Phase two waits 16 ticks, restores the saved route destination, settles it
  through 080C47DC, restores facing and signals completion. Final realigned
  difference: **96 total / 32 structural**. The main mismatch is packed-vector
  position updates: the ROM preserves the adjacent halfword with word
  read/modify/write operations while this candidate emits halfword stores.
  Stack placement and animation pointer copies also remain. Changing bitfield
  signedness/width or aliasing the candidate through a short-vector pointer
  did not close it. Three temporary hints remain; depin declines an unguarded
  nonmatching base, so no exact-minimal hint claim is made for this draft.
- **080C63D0 matched**, `Golem_UpdateChargeBuildup`, **736 bytes**. Callers and
  neighboring Golem controls identify it as speed buildup with a randomized
  windup. Species 1/3/5 wait 30 + 4*(RNG&3), species zero waits 50 +
  16*(RNG&3), and other species wait zero. Context flag 0x800 at +0x628 is
  consumed to skip the wait. Speed ramps by 16 through 0x17F; animation scale
  uses speed>>7, and exceeding the selected duration marks completion and
  plays variant event sound four. A WaitTime inline containing both the flag
  check/clear and RNG selection recovers the original shared branch tail.
  Moving a temporary pointer copy before the materialized byte boolean was
  essential. Depin reduced six hints to **two**: a value in r1 at the motion
  flag insertion and an empty actor constraint before animation-scale reloads.
  Formatted and accepted by the byte-exact per-file checker.
- **080C66B0 matched**, `Golem_UpdateRouteMotion`, **576 bytes**. Return-home
  behaviors 080CC910, 080CDF14 and 080CEDF0 install this control for travel
  toward a cell center. It consumes entry/restart, inserts motion flag 0x100,
  selects the directional animation, derives duration scale from signed
  context speed, and emits opposing-motion dust every fourth frame. Porting
  the common animation from 080C75B8 and using a typed motion view recovered
  the addressing. An initial scratch candidate had an unassigned flags
  pointer; that was corrected before retention or acceptance. The final
  19 differences were register-only; fixing the initial flag-load allocation
  closed them. Depin reduced five hints to **three** (zero in r6, flag value
  in r1, empty actor constraint before scale handling). Formatted and checked
  byte-identical after cleanup.
- **080C68F0 drafted 81.6%**, `Golem_UpdateChargeStop`. Charge/slide callers
  install this completion control. Entry selects directional animation seven
  with flag four, clears restart, and later animation completion clears
  motion flag 0x100 and sets control completion. Final realigned difference:
  **37 total / 9 structural**. Distinct typed animation view, operand order
  for mirror flags and an explicit flag-clear temporary recovered the best
  valid layout. Remaining differences concern selector allocation, pointer
  copies and call argument ordering. The bounded permuter search produced
  candidates, but its 80.1% output was rejected: an actor alias was assigned
  only in the tile branch and then read on the sequence path. A different,
  initialized typed-view candidate was retained and improved to 81.6%.
  Register hints and one empty pointer constraint are marked as concessions;
  depin skips the unguarded base because it is nonmatching.
- **080C47DC stopped; existing draft left unchanged**. Today's check measures
  the inherited settle-route draft at **75.5%, 54 total / 13 structural**.
  It routes toward the destination, projects the lesser X/Z displacement onto
  the body's cell center, quantizes the resulting heading, and clears motion
  if the new quadrant disagrees with the destination delta. Allocation keeps
  dz in r7 instead of the ROM's r2 and the link pointer in r8 instead of r5.
  Splitting delta lifetimes, declaration/assignment order, scoped pins and a
  100-second two-process permuter search did not yield a valid >=80% draft.
  The search's 90.6% candidate was explicitly rejected for reading delta.x
  before it was initialized. Correcting that read dropped it to 73.6%.
  The best valid scratch baseline remains in wip/codex-high; no new production
  source or draft commit was made for this function.

Validation used only `build/venv/bin/python tools/check.py FILE.c`; guarded
scores were recorded with `--nonmatching --record`. All three retained draft
assembly alternatives also pass their default per-file check. Both exact
matches were formatted and checked after depinning. No make/full build,
network action, shared generated-output edit, handwritten instruction body,
or direct Git mutation was performed. The unrelated pre-existing change to
`tools/cloud_task.py` was left untouched. Full combined-ROM validation remains
with the batch integrator. Diagnostic artifacts are ignored under build/ and
wip/; the Ghidra export was unavailable.


# Task 080B6618 — Skeleton attack and route selection (2026-10-20)

Tags: #matching #skeleton #scene-actor #terrain #register-pinning

Branch `cloud/080B6618`, assigned six-function cohort. Worked solo and offline.
The four matches cover **38.21% of the task (+38.21 percentage points; 2,696 of
7,056 reviewed bytes)**. Register-pinned: **10.09% of the task** (712 bytes);
without register pinning: **28.12%** (1,984 bytes). Repository code-coverage gain
is approximately **0.112 percentage points**. The two guarded drafts add no exact
coverage. Each function was committed separately through `build/commit.sh`.

- **080B7C10 drafted 92.8%**, tentative `Skeleton_SelectDistanceReaction`, reviewed
  extent 3,656 bytes; candidate also 3,656 bytes. Resets response state at actor
  +0x460 using gUnk_0824E250/0x4F5, increments context attempts703, and compares
  distanceF0 against context +0x708/+0x70C. The near path temporarily reverses
  the heading and invokes 080B77D0 with the prior heading as its second argument.
  The remaining paths expand route-point recovery, player/target cell selection,
  terrain slope height, retry-turn setup and chase routing. Started from the
  retained 080B9158 recovery candidate, itself related to the 080BBDD0 draft.
  Initial 87.7%, 243 total / 73 structural differences, became **146 / 22**.
  Useful changes: inline mask arguments, narrow motion-mask accessor, loading
  player index before the -1 sentinel, mask-first route-flag clearing, retaining
  an RNG table local, and pinning the previous random heading to r5. The TU is
  marked with the register-pinning concession. Remaining differences are recovery
  stack slots, turn-state address lifetimes and group-flag instruction order.
  The duplicate distance paths mirror the ROM's two inline expansions. RTL loop
  dumps and batched variants were used; no handwritten instructions.
- **080B6884 matched**, tentative `Skeleton_TryBeginArmedAttack`, all 804 bytes,
  without register hints. Its caller 080B6BA8 selects this gate for category seven.
  Near targets use health718 or context flag 0x80, angle <=24, ranges6E0/6E4,
  signed timers6F0/6F2 and attempts703 to choose phase-two behavior zero or three.
  Far targets require angle <=20. The near ranged selection also clears context
  flag 0x80. First C was 98.2%; passing context and mask into the byte-bool test,
  and a pointer/mask into the clear-flags inline, recovered mask/load order and
  the original derived 0x6DC offset. A follow-up source-only correction moved the
  confidence comment above the one-line alias: split.py's inventory regex requires
  a bare `#define Name sub_XXXXXXXX`, without a trailing comment/continuation.
- **080B6E04 matched**, tentative `Skeleton_RestorePursuitRoute`, all 712 bytes,
  register-pinned. Installed as a callback by the nearby creation code. Unless
  flags2D4 bit 0x100 or context active6D0 blocks it, selects chase control for bit
  0x80, extends group timer418 by 300 with a field416 ceiling, or installs delayed
  restoration control (kind 14, 60 frames, command kind seven). Copies the route
  point to the group destination, computes slope-adjusted terrain height, clears
  its pad and routes toward fieldDC's position; invokes the failure callback if
  routing fails. Started at 59.4%, 151 / 39 differences. Evaluating command
  arguments before stores, reading target before route, and using a route-link
  local reached 83.6%. A single r8 route-mask/offset local with an empty constraint
  reproduces the shared 0x200 address and BIC lifetimes. Finally, a **u16 local**
  inside the u16-returning mask accessor eliminated a reload and all remaining
  differences. Marker retained; no instruction text appears in the empty asm.
- **080B7350 matched**, tentative `Skeleton_InitializeCategoryProfile`, all 560
  bytes, without register hints. Its Skeleton defeat consumers call it when
  switching category to seven. Looks up profile entry zero, then initializes
  the category-five/six/seven action/animation entry values and flags in a sparse
  order. The existing matched 080B7580 SetEntry idiom supplied the byte-pair form.
  The correct flagged slot is entry 11 (offset 0x2C), not entry nine. Passing low
  value, flags and destination as **three separate inline arguments** recovered
  constant evaluation order and walking-pointer reuse. Combining the two bytes
  into one argument was 94.0%, hoisting the high byte reached 98.2%, separate
  arguments matched exactly. Other entry state bytes remain untouched.
- **080B6618 matched**, tentative `Skeleton_TryBeginGuardedAttack`, all 620 bytes,
  without register hints. Its caller 080B6BA8 selects it for category six. Fails
  over to 080B7C10 when range readiness is false unless context flag 0x200 blocks
  selection. Guard range6E8/timer6F4, angle <=12 and health <=health718 choose
  phase-two behavior two; nearby/ranged ranges and timers choose behavior zero
  or one, with angle limits 24/12 respectively. Every accepted attack resets
  attempts703. Reused the independently verified 080B6884 context and helpers;
  the first candidate matched exactly, including shared close-attack tails.
- **080B77D0 drafted 87.8%**, tentative `Skeleton_TrySelectRetreatCell`, reviewed
  extent 704 bytes; candidate 696 bytes, **52 total / 30 structural** differences.
  Probes one neighboring cell in the facing quadrant via 0812E118; rotates the
  temporary heading by 64 up to four attempts when blocked, restores the supplied
  prior heading, builds the selected cell center, and tries route setup/update.
  Failure clears motion flag 0x200 and invokes the group failure callback.
  Initial candidate was 70.6%. A center-building inline avoids agbcc's word-sized
  read/modify/write of direct stack halfword assignments. Putting the step count
  in an inline formal retains `cmp #1; blt` rather than canonicalizing to zero.
  Restoring a real outer do/while loop hoists cell/position addresses and raised
  the candidate to the retained threshold. Group-local placement also mattered.
  Remaining differences: north/south scan tails, cached group/route addresses,
  terrain-load allocation and padding. Register experiments did not help and
  were not retained. The commit subject says provisional 87.1%; the recorded
  full-extent score in the source is **87.8%**.

Validation: formatted all six TUs with build/venv/bin/clang-format and checked
its dry-run/Werror mode. The four C matches passed check.py with explicit reviewed
sizes 620/804/712/560; the two guarded assembly alternatives passed over all
704/3,656 bytes. Draft scores were recorded with --nonmatching --record. All
names use file-local aliases and tentative interpretation comments. Follow-up
formatting/declaration cleanup of 080B7C10 retains its 92.8% score. No make,
full build, generated assembly edit, network access, push/PR, shared-doc edit,
or direct git add/commit/checkout/stash occurred. The unrelated pre-existing
change to tools/cloud_task.py remains untouched. Combined-ROM validation and
name promotion belong to the batch integrator. Scratch candidates and diagnostic
artifacts remain ignored under wip/ and build/. The local Ghidra export is absent.

## 2026-10-20 — Task 080ADCC0, Vorn reaction and routing callbacks

Tags: #matching #vorn #scene-actor #terrain #register-pinning #nonmatching

No byte-identical matches; two guarded drafts accepted for retention. Exact code
progress gain is 0.000 percentage points. The four other candidates remain below
the 80% draft threshold in ignored `wip/` files. No shared headers or generated
files were changed.

Family evidence: neighbouring named callbacks use Vorn, and the creation code
around 080B4E18 installs 080AEE08, 080AEB74, 080AE1C4 and 080AE6F0. The special
control table gUnk_08605F94 contains 080AF1E0 and 080AF2BC; its initializer is
Vorn_InitSpecialActionTable. Vorn_SendMessage (080B4134) consumes context6C1 and
sends message seven plus the actor id to category 0x19. Function names below
remain tentative where the precise gameplay role is uncertain.

- **080AF2BC stopped, 72.0%** on the reviewed extent; tentative
  `Vorn_UpdateDelayedRouteRecovery`, candidate in `wip/sub_080AF2BC.c`.
  After callback/update guards and counter2AC > 60, releases an attachment,
  installs RestoreAfterDelay (kind 14), saves a 30-frame command, then either
  runs pursuit arrival/rerouting, restores the saved route for flags2CC bit
  0x10, or saves behavior two and waits 64. Existing scene_actor_chase.h supplies
  almost the entire pursuit path, including the deliberate repeated destination
  height probe and RNG retry turn. Retained candidate: 386 total / 101 structural
  differences; body 2364 versus reviewed 2344 bytes. The counter comparison must
  be signed. Read BIG_FUNCTIONS.md; used condensed assembly, relatives, variants
  and rtlprobe. Seven inline-shape/lifetime variants did not improve the default
  396 / 111 candidate-length result. A diagnostic GCSE T sweep 401..501 changed
  registers/spills (best 358 total) but left 111 structural differences; no
  instrumented-compiler setting was retained. Main-latch rewriting to a direct
  if greatly worsened address lifetimes. Needs more work on cached outer state
  addresses, RNG constant hoisting and local stack-vector lifetimes.
- **080ADCC0 drafted 85.7%**, `Vorn_UpdatePursuitDistance`, in
  `src/fn/sub_080ADCC0.c`. Prepared entry returns immediately. Subsequent calls
  broadcast position and use distanceF0 thresholds 0x19000, 0x38400 and 0x90000,
  context flag 0x2 at 0x6D4, nearTimer6A8 and farTimer6AA. They choose animation,
  delayed behavior 15 or cell-centre routing toward fieldDC. The route context
  here is **actor->context + 0x3D4**, as the original assembly establishes; it
  is not actor->group. The two route tests clear motion flag 0x200 on failure
  and invoke that context group's failure callback. An explicit unsigned flags
  pointer and a full-width signed mask store raised the candidate above the
  threshold. Pins retain context in r7 and the temporary centre pointer in sl.
  Reviewed result: 95 total / 69 structural differences, candidate 1160 versus
  reviewed 1120 bytes. Remaining: centre-building temporaries, branch layout,
  saved registers and address allocation. The commit subject accidentally says
  84.6%; the authoritative recorded source/check score is **85.7%**.
- **080AE1C4 stopped, 67.2%** reviewed; tentative
  `Vorn_UpdateKnockbackRecovery`, `wip/sub_080AE1C4.c`. Entry rounds direction,
  selects animation six, requests category-25 attachment behavior and song
  0x1CC once, resets route/reaction flags, then broadcasts its message. Each
  frame applies quarter-distance X/Z corrections clamped to +/-200 to motion64
  and saved positions318/31C. Once motion settles and the signed timerAA is
  nonpositive, restores route/control kind eight and collider flags. Relatives
  08082D80 and 080E8490 provided motion-convergence and collision-reset C.
  Retained candidate: 205 total / 79 structural differences, 1072 versus 1056
  bytes. Explicit cached field pointers reduced code size but worsened score;
  that candidate was discarded. Adjacent declaration swaps did nothing. A
  diagnostic T sweep 201..321 did not change the result. Needs initial reset
  mask/control-address lifetimes and the late repeated animation reset aligned.
- **080AE6F0 stopped, 70.0%** retained reviewed candidate; tentative
  `Vorn_ResumeReactionRoute`, `wip/sub_080AE6F0.c`. On prepared entry turns back
  and selects animation zero. When flags2D0 bit 0x10 is present, clears bit four,
  sends a message, resumes SceneBody_StartRoute, clears context flags, then
  either runs script gUnk_0824E20C/0x39F or resets route and installs group+0x210
  control kind 15. Retained candidate: 98 total / 42 structural differences,
  596 versus 592 bytes. Context/group/flags pinning transiently reached 71.0%
  candidate-length score (94 / 44); no exact match. Signed mask temporaries
  preserve the negative immediate for byte flag clearing. Moving motion access
  into a byte-pointer helper instead materialized actor+0xEB, losing the
  original actor+0xDC with a +15 load. A full-width s16 bitfield experiment did
  not improve the retained source. Needs saved zero and flags-address lifetimes.
- **080AEB74 stopped, 71.8%** reviewed; tentative
  `Vorn_TryRestorePatrolRoute`, `wip/sub_080AEB74.c`. Requires flags2CC bits
  0x400000 and 0x200000 plus motion flag one, saves a 30-frame command for
  savedTarget/kind nine, samples the route terrain height, marks the destination
  ready, resets reaction flags and context timers6D4/69E/6AC, sends a message,
  then returns one. Retained candidate: 142 total / 47 structural differences,
  668 versus 660 bytes. Mask-parameter helpers improved instruction ordering,
  but explicit flags/motion pointer caching and group/context pins did not
  improve the score. Needs the original spilled motion pointer, transient r3
  destination and flags2CC lifetime around the terrain call.
- **080AEE08 drafted 83.4%**, `Vorn_UpdateDefeat`, in
  `src/fn/sub_080AEE08.c`. Reused Bat_UpdateDefeat's verified spark/script/event
  pattern; Vorn adds Vorn_SendMessage, calls 080B0D28 before the burst, and
  always transitions to idle control/state 1/behavior 2 after its completion
  latch. Entry releases attachments, advances effects, clears route counters,
  and marks the collider. Completion spawns six sparks unless flags2CC bit
  0x400 suppresses them, runs the eight-argument defeat script, conditionally
  sets the category event flag, hides the drawable and shadow. Pins cover
  burst pointers/constants and the first RNG result/cursor. The first RNG uses
  a distinct helper; assigning the position pointer in the first call prevents
  an early address computation. A small drawable-tail type preserves the
  original +0x20 base and +8 flags access. Reviewed result: 84 total / nine
  structural differences; candidate and target both 984 bytes. Remaining:
  low-register choices, address derivation and late constant reloads.

Both retained drafts have bare file-local semantic aliases, register-pinning
markers and `#ifdef NONMATCHING` / original INCLUDE_ASM alternatives. Scores
were written by `tools/check.py --nonmatching --record`. All acceptance and
comparison work used per-file check.py; no make, full build, network access,
push/PR, direct git add/commit/checkout/stash, generated assembly modification
or shared-doc edit occurred. The local Ghidra export is absent. The unrelated
pre-existing modification to tools/cloud_task.py remains untouched. Commit
requests go through build/commit.sh; combined ROM validation is left to the
batch integrator. Scratch candidates and diagnostic artifacts remain in ignored
wip/ and build/ directories.

## 2026-10-20 — Task 0820C334, Screamer and PinkSpine actor callbacks

Tags: #matching #screamer #pink-spine #scene-actor #register-pinning #nonmatching

Exact coverage of this task's 5,316 reviewed body/pool bytes is **7.83%
(+7.83 percentage points; 416 bytes)**, entirely Register-pinned. Five guarded
C drafts recover the remaining functions and count as zero exact progress.
Worked solo on the already-claimed `cloud/0820C334` branch. Read the worker,
cloud and large-function guidance; inspected callers, neighbouring named
functions, full assembly and matched relatives. The Ghidra export is absent.
No make, full build, network action or direct Git mutation was performed.
Each function was committed separately through `build/commit.sh`.

- **0820C334 matched**, `Screamer_WaitForPlayerApproach`, full 416 bytes.
  Clears the sprite hide bit, obtains player position zero, compares squared
  X/Z distance to actor+1FC, then starts sequence 0x1D and installs 0820C4D4
  when close. Resets the task-age word and phase1D6, updates the body collider,
  disables the other collider and advances packed animation. Caller 0820CD00
  installs this state, and Screamer_Create (0820D1F8) establishes the family.
  The first candidate was three instructions away. Pinning the 0xFFFF source
  to r1, separately from the live r5 mask, reproduces the missing register copy
  and pool alignment. Final formatted check reports zero differing bytes.
  `depin.py --write` found no hints in the consumer itself; allocation hints
  used by its packed-animation inline remain in screamer_playback.h.
- **0820C4D4 drafted 94.4%**, `Screamer_UpdateScream`, reviewed 1,200 bytes.
  Three signed phases: wait for timer1CA > delay1D4, start 0x1B at duration
  scale 0x20 and play 0x51A; count completed animations against limit1D0;
  start 0x1D, then restore approach state/sequence0x1A after recovery1CC.
  Phase one refreshes the attack collider at +6C; other phases position the
  body collider at +C0. The same packed animation expansion occurs three
  times. Shared matched playback C from ContactSequenceActor and
  RoamingObject supplied the template. Initial 80.7% improved by constraining
  the explicit flag copies and the duration/weight/mask registers. Final
  realigned residue is **38 instructions / nine ignoring registers**.
  Remaining issues are constant source registers across the three expansions,
  reuse of phase one as the flip mask, offset allocation and switch liveness.
- **0820CD00 drafted 81.1%**, `Screamer_InitFromScript`, reviewed 1,272 bytes;
  candidate 1,268. Parses p/a/l/i/r/w/h/S/z/g/D/G/X/V, stores the creator's
  resource at +1F8, loads graphics0x7E7D and palette0x1C4, and obtains the
  0x922E/0x5184 sequence resource. Negative l marks the initially inactive
  state; h maps directional pairs to horizontal/vertical orientation. Starts
  approach state or SceneDrawablePair_SetFlags, offsets sprite X or Z by -16,
  optionally builds a terrain body at +168, and squares positive radius1DA.
  Remaining **137 instructions / 27 ignoring registers**: keyword narrowing,
  field-address registers, sequence/sprite pointer lifetimes and defined
  success return. A signed local for the l value avoids alias-driven reloads.
  Making the temporary position pointer escape through an empty constraint
  prevents agbcc from turning halfword coordinate stores into word masks.
- **0820C9AC drafted 81.3%**, `Screamer_UpdateDamageParticles`, reviewed
  840 bytes; candidate 816. Checks the global pause flags, invokes the active
  state, subtracts damage1DC from health1E4, copies contact flags204 to200,
  displays damage and updates the HUD. Defeat spawns eight randomly directed
  particles (graphics0x7E7C, frames0x18..0x1B, palette0x1C4), selects the
  inactive callback, invokes Screamer_HandleDefeat and plays0x52E. Otherwise
  flashes palette0x192 for eight frames, consumes damage and manages the
  collider's blink countdown104. Remaining **93 instructions / 60 ignoring
  registers**: pause-return layout, random-table base reconstruction and
  cursor lifetimes, stack slots and palette-address caching. Passing an
  explicitly retained r10 random-table pointer to the first three RNG
  expansions raised similarity substantially; the final two draws retain
  their independent low-register address path. Widening the local graphics
  API declaration avoids an extra, unsupported frame truncation.
- **08210654 drafted 92.8%**, `PinkSpine_InitFromScript`, reviewed 900 bytes;
  candidate 904. Prefix follows neighbouring PinkSpine_Create (082109D8) and
  PinkSpine_OnTaskDestroy (08210648); identity remains tentative because
  neighbouring behavior states use ProjectileEmitter names. Parses position,
  two squared radii, l/a/S/D/G/X, initializes graphics0x7E7D/palette0x1BD,
  selects approach sequence0xB or inactive sequence0xD, optionally builds
  the body at +C0, then stores one to field136. Remaining **31 instructions /
  six ignoring registers**: option/animation allocation and explicit success
  return. Preserves the original surprising final health-field overwrite.
- **08210398 drafted 89.6%**, `PinkSpine_UpdateDamageAnimation`, reviewed
  688 bytes; candidate 692. Checks pause, invokes the current behavior,
  positions or disables the collider according to134, subtracts damage130
  from136 and copies148 to144. Defeat selects sequence0xD and0821025C,
  resets age and plays0x522; all damage arms a four-frame palette flash.
  Advances animation and plays0x521 when a cycle ends during
  ProjectileEmitter_UpdateAttack. Remaining **44 instructions / 29 ignoring
  registers**: pause layout, collider base addressing, animation pointer
  lifetimes and an explicit callback return.

The common sprite position/sequence layout is observed in both actor families;
unknown scalar fields retain their offset-based names. Sprite types are scoped
as ScreamerSprite to avoid a collision with the unrelated ActorSprite type.
The two family headers keep the playback source readable and independent of
handwritten instruction bodies. All affected consumers carry the authorized
register-pinning concession marker. The draft helpers use allocation-only r0
outputs solely between empty-asm operands; those tokens never participate in
C arithmetic, stores, branches or returned values. This preserves the ROM's
reserved-register lifetime while giving the C drafts explicit zero results.
The initializers' creators consume a result sign, while the original bodies
leave incidental R0 words intact; those ABI details remain nonmatching and
must not be promoted as exact from fuzzy scores.

All five final scores were recorded with `check.py --nonmatching --record`
against complete function extents. Their default INCLUDE_ASM branches also
passed separate per-file exact checks. Formatting used the venv clang-format
directly. Depinning was invoked on every hinted candidate; the minimizer skips
nonmatching candidates, so their residual hints were retained. No combined ROM
build was run; it belongs to batch integration. The pre-existing modification
to tools/cloud_task.py was left untouched. Scratch candidates and diagnostics
remain ignored under wip/codex-high/ and build/. Shared HANDOFF, INDEX and
history files were not edited, per the cloud-worker scope.

## Task 080D6608 — 2026-10-20

Tags: #matching #mummy #dog #scene-actor #register-pinning

Worked solo on the already-claimed cloud/080D6608 branch. No network operations,
full build, generated-file edits or shared documentation edits. Final outcomes:
two checked exact retained bodies and four guarded drafts. The dog C bodies
were already present and byte-identical before this pass; their commits add the
required file-local aliases (and remove one unnecessary hint for 080DDEC8).
Do not count those pre-existing bodies as newly recovered exact-code percentage
or percentage-point gain from this session. Draft improvements earn no exact
progress credit.

- **080D812C drafted 98.3%**, `Mummy_UpdateBurnDamage` (tentative).
  Reviewed extent 6504 bytes, candidate also 6504; final realigned difference
  **59 instructions, seven ignoring registers**. The large statistics tail is
  the saturating category/weapon defeat-counter code shared with
  SceneActorReaction_DefeatRecordStats (08132D2C) and Ivy_UpdateDefeat
  (080EE1B0). Reused those matched C idioms rather than reproducing each switch
  arm independently. Actor +3D4 owns four halfword effect durations at relative
  +1E4, count +1EC, active mask +1EE and damage timer +1F0; relative +AA supplies
  the damage exponent. Every 40 ticks it subtracts 1 << exponent, flashes the
  palette and can display damage. At zero health it changes control, records
  defeat statistics and clears both timers. Action 0x15 clears the effect.
  The status interpretation as burn remains tentative. Important correction:
  the action-0x15 flags clear is **~0x10**, because the ROM derives -17 by
  subtracting 0x31 from 0x20. Keeping a typed collider pointer for cost +3E,
  spelling the duration indexing at each use and using an inline CpuSet fill
  reduced structural differences substantially. A shared status-store label
  also matters. Remaining differences are the signed mask materialization,
  collider/flag pointer copies, category-pointer move order and registers.
  Pointer-output damage-helper variants regressed; signed/bitfield mask forms
  improved the local clear but worsened allocation elsewhere. Final diagnostic
  pins retain the damage flag pointer, HUD ownership operands and status value.
  An initial 96.9% checkpoint was committed before the later improvement.
- **080D6608 drafted 92.5%**, `Mummy_TryDirectPursuit` (tentative).
  Reviewed extent 2032 bytes, candidate 2028; final realigned difference
  **103 instructions, 12 ignoring registers**. Starts with a same-row/column
  reachability scan using sub_0812E118, then falls back to the player route
  reacquisition path. The retained draft began at 87.0%. Separate success
  checks, a signed group flag update, passing &target directly to TryRoute,
  an r7 random-mask local and the target-index pointer in r1 improved it.
  Remaining PRE stack-slot ordering dominates the register differences in
  the turn/save-control block; some caller pointer reloads also differ.
  Rewriting the retry loop with a chosen label and forcing the whole arrival
  group to sb did not improve the retained candidate.
- **080D9B20 drafted 82.3%**, `SceneActor_FindDirectionalCell` (tentative).
  Reviewed extent 1768 bytes, candidate 1764; final realigned difference
  **269 instructions, 250 ignoring registers**. Checks the requested neighbour
  against height and blocking bits 0x43, otherwise returns the actor's cell.
  The retained u8 Adjacent candidate scored 85.3% but generated only 1444 bytes
  and had 574 structural differences with call-based realignment. Widening
  Adjacent's result to u32 recovers the duplicated body structure and nearly
  the complete length, despite the lower coarse fuzzy score. Open result
  widths, narrowed index types, nested returns, independent conditions and
  explicit result variables did not recover the ROM's particular cross-jumps.
  The older shorter form remains available in the parent commit.
- **080D6FDC drafted 80.6%**, `Mummy_RequestStraightRoute` (tentative).
  Reviewed and candidate extents 1068 bytes; final realigned difference
  **105 instructions, 17 ignoring registers**. Retained the existing candidate
  and supplied its required alias. Scans in the facing direction to the last
  open cell, starts a route, and arms a 240-tick cooldown on success. Its map
  address spills across Mod instead of staying in r6, giving a 0x14 frame
  rather than 0x10. Tested accessor temporaries, animation argument widths,
  signed masks, map pointer outputs, success polarity and instrumented GCSE
  table sizes 401..801. None improved the retained source; table size alone
  does not explain this register choice.
- **080DDEC8 matched**, `Dog_RestoreRoute`, reviewed extent **608 bytes**.
  Existing body matched after adding its local alias. Compared its delayed
  follow-up control and route restoration with Golem_RestoreRoute (080C4A9C):
  dog uses a 20-frame chase delay and sets flag 4 after the timer extension.
  Depinning reduced four recognized hints to three; exact check still passes.
- **080DE128 matched**, `Dog_SelectTargetAction`, reviewed body **772 bytes**
  through 080DE42C; the following retained byte blob is not part of this C body.
  Existing body matched after adding its local alias. Selects responses 0x75,
  0x82 and 0x87 from target distance, heading error and cooldowns, or asks
  sub_080DF400 to move. Depinning retained all four recognized hints.

All task functions have local Subsystem_VerbNoun aliases. Both exact bodies
passed check.py with explicit reviewed extents. All four drafts were formatted
with the venv clang-format, scored using --nonmatching --record, and their
INCLUDE_ASM default branches passed separate checks. Concession markers remain
on hinted units. The integrator still owns the one combined ROM build.

Tool caveat discovered here: depin.py on a guarded draft checks the default
INCLUDE_ASM branch and consequently treats every hint as removable. Those
results were discarded. Run depinning on the unguarded candidate instead;
it correctly reports that these nonmatching candidates cannot be minimized
as exact matches. The final guarded drafts preserve their tested hints.
Scratch hypotheses and checks are ignored under wip/ and build/.

## Task 08131820 — route approach and area damage (2026-10-20)

Tags: #matching #scene-actor #route #damage #register-pinning

Worked solo on the preclaimed `cloud/08131820` branch, offline. Used the
worktree Python and per-file checker; no make, full build, generated assembly
edits or network. Each source commit was queued through `build/commit.sh`.

New exact code gain: **+0.0217 percentage points**, all register-pinned (524
bytes, using PROGRESS.md's code denominator). Three assigned functions already
had matching C in the starting checkout; their revalidation adds no new code
credit. The two guarded drafts remain excluded from exact progress.

- **08134398 — matched, existing C reverified.**
  `SceneActorReaction_RecordDefeat`, reviewed extent 5636 bytes. The largest
  assigned function was already implemented and named by the preceding medium
  worker. Added the required local alias because the supplied ELF still names
  the target `sub_08134398`; its full resolved-byte check passed unchanged.
  Saturating category/weapon defeat statistics and kind-17 control setup are
  documented in notes/codex-medium.md. No register hints in this unit.
- **0813214C — matched, existing C reverified.**
  `SceneActorReaction_ApplyStaggerDamage`, reviewed extent 836 bytes. Retained
  the existing damage/display/defeat logic and surviving actor's kind-33 stagger
  callback. Added the local alias. `depin.py --write` reduced four hints to two;
  formatted full-extent check remained exact. Register-pinning concession.
- **08132490 — matched, existing C reverified.**
  `SceneActorReaction_ApplyCategoryDamage`, reviewed extent 696 bytes. Retained
  category damage, defeat control setup and surviving actor's song 0x13E.
  Added the local alias. Depinning reduced two hints to one; formatted
  full-extent check remained exact. Register-pinning concession.
- **08131820 — newly matched.** Tentative alias
  `SceneActorControl_UpdateRouteApproach`, reviewed extent 524 bytes. Present
  in the repeated route callback tables (including gUnk_08605C9C); its closest
  matched relative is 080B04F4, explaining 60% of the instructions. Consumes
  field2B1, invokes callback +0x260 for movement speed, updates drawable duration
  scale, then consumes the navigation probe event. A successful descriptor turn
  sets behavior 6. A successful acceleration sets behavior 8 and control kind
  0x13 from actor+0x5E8. Outside mode mask 0x4800, it chooses a route point using
  RNG/Mod, avoids the previous index by advancing once modulo the point count,
  resets its phase, updates its data pointer and resolves the route position.
  Other paths increment counter2AC. Reused the relative's duration-scale and
  descriptor-call shapes. Explicit byte-index RNG expansion (multiply the index
  before adding the table base) recovered the arithmetic-shift/load order.
  Local offset/pointer bindings recovered the two reset sequences and final
  counter allocation. Depinning reduced 35 hints to 14; the final formatted
  checker reports zero differing bytes over the complete 524-byte extent.
- **08131D5C — drafted 87.5%**, improved from 85.4%.
  `SceneActorReaction_ApplyAreaDamage`. Full target extent 1008 bytes;
  candidate 1004 bytes, 91 realigned differing instructions / 25 ignoring
  registers (previously 96 / 42). Direct failure returns inside the inline
  owner/display predicate recover its materialized boolean branch shape.
  Scoped r4/r5 position/max-health values retained after the gain call move
  their spills to the correct side of that call. A dz/sl binding improves its
  later uses. Remaining: collision-flags/collider/health pointer allocation,
  stack slots, RNG first-index store and constant scheduling, hit-flag load
  order. Forcing the collision flags to sb or collider to r8, combining RNG
  index updates, and a SetHit wrapper regressed structure; discarded. The best
  structural candidate is retained, rather than the 87.9% candidate with 30
  structural differences.
- **08132904 — drafted 87.1%**, improved from 81.1%.
  `SceneActorReaction_ApplyLinkedAreaDamage`. Full target extent 1064 bytes;
  candidate 1060 bytes, 96 realigned differing instructions / 24 ignoring
  registers (previously 126 / 58). Same predicate improvement. Branch-local
  position/max-health setup and scoped r4/r5 values reproduce the duplicated
  gain-call setup and improve the spills. A maximum-health/sl pin and explicit
  paired RNG expansion regressed and were discarded. Remaining pointer/stack
  allocation, RNG scheduling and hit-flag ordering overlap the preceding draft.

Both drafts retain NONMATCHING guards and raw-address-name INCLUDE_ASM fallback
identifiers, so `check.py --nonmatching --record` can locate and update their
scores despite the semantic-name aliases. Ran depin on both: it only minimizes
exact matches, so it skipped these drafts. Formatting and `git diff --check`
passed. No new layout/global data claims or shared header changes. Batch ROM
validation remains for the integrator.

# Task 081A1EFC — Codex high, 2026-10-20

Tags: #matching #hresvelgr #vanargand #effect-pool #register-pinning

Branch: `cloud/081A1EFC`. Worked solo and offline. Used the worktree Python,
`asmat.py`, `relatives.py`, `condense.py`, `anchordiff.py`, and per-file
`check.py`; no make, full build, network operation, or direct Git mutation.
All commits went through `build/commit.sh`. Exact project code gain is
**+0.0159 percentage points**, all without register pinning (384 bytes).
Drafts contribute no exact progress.

- **081A5614 — drafted 80.7%**, tentative
  `Vanargand_UpdatePaletteTransition`. The adjacent reset/blend functions
  and the Vanargand update caller establish the family. Modes 1–5 blend
  palette banks or restore their source buffers; mode 6 interpolates two
  paired RGB entries between stages. Repeated clamped, signed fixed-point
  interpolation is expressed with inline helpers. The restoration paths in
  modes 4/5 intentionally call the reset routine twice, as the ROM does.
  Reviewed target 2800 bytes; candidate 2848 bytes; 299 realigned differing
  instructions / 82 ignoring registers. The main remaining differences are
  argument setup, interpolation temporary allocation and spills. Pinning
  the selector to r8 is marked as a concession. Broad pinning of the nested
  RGB temporaries regressed. The discarded alternative with a Blend wrapper
  also had 81 structural differences, but changed the owner allocation and
  scored below the draft threshold. This unit was formatted after its draft
  commit; formatting preserved its recorded score.
- **081A1EFC — stopped below the draft threshold**, tentative working name
  `Hresvelgr_ApplyContactDamage`; no source committed. The candidate preserves
  contact accumulator consumption, environmental damage, health/display
  updates, special action-16/action-33 reactions, sparks and debris, but the
  initial check is only 49.4% (119 structural / 346 total realigned differences).
  Original stack frame 0x54 versus candidate 0x50; the original retains the
  owner in sl and a low-register copy in r7, while the candidate gives both
  copies high registers. Number-display vector stores become packed word
  updates; particle RNG temporaries and reaction/flag shared tails also
  differ. Eight targeted alignment/owner-pinning variants gave no useful
  improvement. `wip/sub_081A1EFC.c` retains the original candidate locally;
  the ROM assembly remains the build implementation. Future work should
  recover the original vector/store helper and shared reaction/flag setters
  before trying allocation again.
- **081A2858 — drafted 83.3%**, existing semantic name
  `Hresvelgr_UpdateCollisionPresentation`. `ActorReaction_Tick` calls it after
  its behavior/callback updates. It gates the two contact colliders, updates
  the body collider, presentation flags, shadow, sound latch, camera offset
  and vertical display ramp, then clears transient flags at BC/C0. The second
  contact collider overlaps the palette field at 2CE; a named union preserves
  the observed offsets. Full-width signed halfword bitfields reproduce the
  disabled-collider flag operations. Mask-taking inline accessors recover
  the mask-before-load order; animation-specific helpers retain the 194+8
  addressing. Reviewed target 864 bytes; candidate 872 bytes; 77 realigned
  differences / 19 ignoring registers. Remaining differences are pointer
  lifetimes, palette constant setup and allocation. C0's transient pointer
  is pinned to r10, with the required concession marker.
- **081A39E0 — drafted 88.3%**, `EffectPool_UpdateEntry`. Its caller is
  `EffectPool_UpdateActiveEntries` (081A3D00); its target-angle helper is
  `Hresvelgr_GetHomingShotTargetAngle` (081A37BC). The entry homes toward
  player zero, advances its animation/collider, emits up to four motion
  trails, and increases speed/turning frequency as its lifetime expires.
  Reviewed target and candidate both 800 bytes; 50 realigned differences /
  12 ignoring registers. Explicit vector-load temporaries and masks tied to
  their constant registers improve matching. Signed local RNG samples keep
  the ROM's arithmetic shifts. Remaining differences include redundant angle
  masking, animation timer scheduling and register copies. Register hints
  in the animation flags blocks are marked as concessions.
  The score was recorded with the raw sub_081A39E0 fallback identifier, then
  the fallback was restored to the existing canonical `EffectPool_UpdateEntry`
  name, whose assembly file exists in this checkout. Both the 88.3% C check
  and byte-identical default assembly check passed afterward. Current
  `check.py` cannot rediscover the per-draft score when a canonical fallback
  name differs from the compiled file-local alias; the recorded score is
  unchanged and the C is still guarded, not exact progress.
- **081A2430 — drafted 86.9%**, tentative
  `Hresvelgr_DispatchSceneMessages`. Called by 081A26D8. It reads the active
  scene-message buffer, sets the pending trigger, handles action/mode changes
  for commands 0/1/19/26, effect configuration for 9, facing/animation for 5,
  and position for 2. An explicit shared action block for cases 0/1/19
  recovers the ROM's cross-jumping; case 26 remains separate. Passing mode
  constants through the inline setter improves their lifetimes. Reviewed
  target and candidate both 680 bytes; 53 realigned differences / 25 ignoring
  registers. Remaining differences are trigger indexing and mode/action
  constants. Action and callback register hints are marked as concessions.
- **081A26D8 — matched**, tentative
  `Hresvelgr_UpdateAnimationAndTargets`, 384 bytes, **without register pinning**.
  `ActorReaction_Tick` (081A2BD4) calls this before behavior dispatch. It calls
  the pending-message dispatcher, advances animation, copies the render
  position, adds decaying random shake, updates target metrics, then computes
  bearings and squared horizontal distances for tracked positions. A sprite
  pointer scoped inside the prechecked target loop prevents unwanted output
  pointer induction. Typed bearing/distance arrays recover base-before-index
  addressing. `RANDOM_TABLE_ADDR` reproduces the late table-literal load.
  The last 20 differing bytes were a pure r6/r7 swap; removing the provisional
  owner/r9 hint produced exact output. Final formatted source checked MATCH.

No shared headers, symbols, generated files, history, HANDOFF or INDEX were
edited. The four drafts have NONMATCHING guards and recorded scores; no draft
is counted as a match. The integrator still performs name promotion and the
single combined ROM build for the batch.

# Codex high: task 081B08B8 (2026-10-20)

Tags: #matching #vanargand #actor-status #register-pinning

Branch: `cloud/081B08B8`. Worked solo, offline, with the worktree Python and
per-file checks only. No make, full build, network action, generated-file edit,
or direct Git mutation. Each function was queued separately through
`build/commit.sh`; this note was queued last. Ghidra's export is absent.

Exact task coverage is **7.73% (+7.73 percentage points; 424 of 5,484 reviewed
body/pool bytes)**, all Register-pinned. This adds approximately **0.018
percentage points** of repository code coverage. The five drafts add no exact
coverage. Reviewed extents are the contiguous function bodies and literal pools:
081B08B8=1,660; 081B0F34=600; 081B118C=424; 081B1334=1,428;
081B18C8=712; 081B1B90=660 bytes.

The caller `PairedSceneActor_UpdateHalves` (081B5D8C) invokes these through
`gPairedSceneHalfStateCallbacks` (086120A0), indexed by the part's byte at
0x16D. The surrounding named functions establish the ActorStatus/Vanargand
family. MotionTrack_Initialize (081B9334), MotionTrack_Update (081B95E4),
OrbitMotion_BeginDistanceWave (081B939C), and the player position-request APIs
explain the motion paths. Names below remain provisional and carry file-local
aliases with low-confidence comments. The drafts keep a local observed part
view: position 0x1C, packed sequence 0x48, motion 0x58, anchor pointer 0x90,
offset 0x9C, variant 0x15C, flags 0x160, substate 0x164, timer 0x168,
action/previous-action/reset/enter/finished bytes 0x16C..0x170, and countdown
0x19C. No shared header was changed.

- **081B118C matched**, `ActorStatus_BeginPlayerRelease`, 424 bytes. Improved
  the existing guarded candidate from 92.9% to byte equality. On reset clears
  substate/timer and sets enter; state zero requests player positioning with
  arguments 80/32/128, starts sequence nine from owner resource 0x4D0, steps
  packed playback, and advances to state one. State one sets finished.
  The final residue was entirely pointer and zero-value lifetimes. A flags
  pointer in r1 removes an extra address copy; the reset pointer in r2 plus a
  mask in r2 and one shared zero in r1 reproduces entry allocation. Explicit
  destination-pointer constraints preserve the part base at final stores.
  Using r1 for the elapsed-mask source fixes the animation's last allocation
  difference. `depin.py --write` reduced 30 hints to 18; formatting followed
  by `check.py --target-size 424` retained zero differing bytes.
- **081B08B8 drafted 91.3%**, `ActorStatus_UpdatePlayerApproach`, reviewed
  1,660 bytes; candidate 1,676. Entry starts sequence eleven. State zero moves
  the offset by (-256, 0, -256); state one seeks the player relative to the
  anchor using motion argument three. A nonnegative
  Vanargand_GetComponentWithFlag10 result completes the approach; squared
  X/Z distance <= 0x63FF instead selects contact sequence twelve and song
  0x58A. Motion completion selects a thirty-frame wait with sequence twelve,
  then sequence nine. Final realigned difference: 92 instructions, 35 ignoring
  registers. Explicit shared completion/finished labels recovered the ROM's
  cross-jumped tails. Remaining issues: entry-pointer ordering, vector operands,
  initial state stores, and register allocation around the shared tails.
- **081B1334 drafted 83.0%**, `ActorStatus_UpdateGuardedApproach`, reviewed
  1,428 bytes; candidate 1,412. Related approach starts at (-1024, 0, -1024)
  and uses motion argument four. In range, it requests action eighteen only
  when flags 0x100 is clear and previous action is outside 25/26 or finished
  is set. It writes both action bytes, rearms reset, clears finished and
  countdown, and clears substate/timer. Otherwise completed motion selects
  sequence twelve/song 0x58A for 120 frames before sequence nine. Entry also
  plays 0x58A. Remaining realigned difference: 149 instructions, 55 ignoring
  registers, chiefly the guarded transition and pointer/animation allocation.
- **081B18C8 drafted 80.3%**, `ActorStatus_UpdateRisingStrike`, reviewed
  712 bytes; candidate 696. Starts sequence eight with playback flags four,
  moves the offset up 512, starts song 0x596 and an externally consumed
  sixty-frame countdown, then returns toward negative anchor Y with a
  (0, 64, 0) motion parameter and finishes. Remaining realigned difference:
  80 instructions, 27 ignoring registers. Reversing the vector-add source
  operands, materializing the switch state, and using r3 as the duration-mask
  source improved the score. State-address reuse, entry-pointer lifetimes,
  vector initialization and constant ordering remain different.
- **081B1B90 drafted 84.5%**, `ActorStatus_UpdateFlankingStrike`, reviewed
  660 bytes; candidate 652. Computes player-minus-anchor and offsets X/Z by
  (+512, -512) for variant one, otherwise (-512, +512). Starts a distance-wave
  motion at angle `(variant * 128 + 96) & 255`, then plays sequence eight
  until playback wraps and marks finished. Remaining realigned difference:
  60 instructions, 20 ignoring registers. A saved state in r7 and an explicit
  next-state value improve allocation; entry-address lifetime, reuse of the
  switch's one value in the animation mask, and transition constants remain.
- **081B0F34 drafted 81.9%**, `ActorStatus_UpdatePlayerCapture`, reviewed
  600 bytes; candidate 624. Starts sequence twelve, sets status bits 12/16,
  moves the part offset toward (640, 528, 640), and requests the player at
  the part position minus 128 Y. Variant zero uses request arguments one/one;
  the other variant uses zero/two. Player flag 0x40000 or motion completion
  advances to the finished state. Remaining realigned difference: 121
  instructions, 66 ignoring registers. Volatile halfword destination writes
  recover the original three coordinate stores; merging the two request call
  paths and ordering player-flag masks/pointer lifetimes remain unresolved.

Large functions were read with `condense.py` and matched relatives, including
ScenePanelSubject's packed playback template. Scored structural hypotheses in
batches with `variants.py`, using reviewed-extent `check.py`/anchordiff counts
for decisions. `rtlprobe.py perline` confirmed the release entry test as the
last source of surplus instructions. A statement-expression macro experiment
showed that a redundant local owner pointer caused extra copies; retaining a
normal inline that uses its owner parameter directly produced the same better
code and kept the source readable. Initializing a u8 result to zero before the
motion/proximity test reproduces the materialized boolean better than two
immediate returns. Initializing stack coordinates through a halfword pointer
avoids agbcc merging partial vector fields into word read/modify/write stores.

Each draft is guarded by NONMATCHING, retains INCLUDE_ASM in its else branch,
and has its reviewed-extent fuzzy score recorded with `--nonmatching --record`.
All five default assembly alternatives passed reviewed-extent per-file checks.
Depinning was attempted on the unguarded draft sources; the exact-only tool
correctly skipped them. Source review and `git diff --check` passed. No complete
ROM build was run; combined validation remains with the batch integrator.

## Task 0812E118 — 2026-10-20

Tags: #matching #scene-actors #nonmatching #register-pinning #boundaries

Branch: `cloud/0812E118`. Worked solo and offline. Exact code gain: **0.00
percentage points (0% of the six assigned functions matched)**. Improved three
existing guarded drafts; no new exact coverage is claimed. Used the worktree's
Python and fast per-file checks, never make, a full build, network operations,
or direct Git mutations. Commits were queued through `build/commit.sh`.

- **0812FA0C — drafted 83.2%**, up from 80.6%. Local alias
  `SceneActorReaction_UpdateRouteReturn` (tentative existing interpretation).
  Reviewed function span: 1,976 bytes through 081301C4; C emits 1,968 bytes.
  Final realigned differences: **191 total / 50 ignoring registers**, versus
  228 / 44 initially. Explicit RNG table/index/wrap/mask locals and pinning
  the prior direction to r5 reduced allocation differences and improved the
  full-function fuzzy score, despite a small structural-count regression.
  Packet constants, narrower setter parameters, target/group pointer removal,
  bitfield masks, and other RNG pins did not improve the retained candidate.
  Remaining differences include PRE stack slots, RNG constant placement,
  delayed-control/state setup and the reroute target's stack-pointer lifetime.
  The file carries the register-pinning concession. The checker could not
  record the score while INCLUDE_ASM used the promoted descriptive name;
  using sub_0812FA0C in the assembly alternative fixed the local build and
  allowed `--nonmatching --record` to record 83.2%.
- **0812E7EC — stopped below threshold**, retained locally at
  `wip/high/sub_0812E7EC.c`. Tentative alias
  `SceneActorReaction_DispatchPendingCommands`. The earlier worker's local
  candidate was absent, so reconstructed the receiver loop, behavior/movement
  cases and attachment sub-dispatch from assembly. The receiver is actor+0xC;
  the event bank is gUnk_0200047C+0x24. Category 18 writes destination XYZ at
  actor+0x2FC; other categories use the route at group+0x1FC. Commands 5–7
  forward small halfword packets. Attachment kind 5 calls the parts creator
  with part count from command+0xE; kinds 6/7 update a descriptor; kind 8
  releases resources in order 0,1,2,4,5,3. Reviewed span is **1,764 bytes**,
  ending at 0812EED0, before the two tiny neighboring functions. Final check:
  **76.6%, 292 total / 179 structural differences**, C 1,564 bytes.
  Candidate-length scoring misleadingly reports 71.0%; use the reviewed span.
  The main remaining problem is cross-jumping: agbcc merges state-setting
  sequences the ROM duplicates. State/control parameter-width variants,
  explicit initial count, actor-relative count and local attachment-offset
  scopes did not resolve the missing sequences. Nothing promoted.
- **0812F5D4 — drafted 81.1%**, up from 80.8%, alias
  `SceneActorReaction_UpdateContactRoute`. Final realigned differences:
  **80 total / 23 structural**, down from 92 / 35. Additional pins on RNG
  wrap (r7) and mask (r6), an inline behavior setter, and passing the +0x414
  flag mask as an inline argument improved the structure. The last change
  reproduces mask-before-load ordering. The original r5 prior-direction and
  r1 retry-count pins remain. A higher-fuzzy intermediate scored 83.9%, but
  the retained candidate has fewer structural and total differences.
  Remaining differences are PRE stack-slot ordering, constant placement in
  the RNG preheader/loop, and control/state allocation. Reviewed span 800
  bytes; C 804 bytes. Ran depin on an unguarded temporary copy: it correctly
  skipped the nonmatching C. Do not depin a guarded draft through its matching
  assembly fallback; that does not measure the hints in the candidate.
- **0812EED8 — drafted 86.1%**, up from 81.3%, alias
  `SceneActorReaction_BeginTargetRecovery`. Spelling the 0x400 motion-flag
  clear as a scoped pointer/mask operation instead of the Chase helper reduces
  realigned differences from **58 to 43 total**, with **22 structural** left.
  Remaining differences include early flags-pointer PRE, motion-pointer
  copies, mask/load order, and recomputing the final motion-flags address.
  Actor/motion/flags pins, altered pointer scopes, and actor-based accessors
  did not improve the candidate. Reviewed span 596 bytes; C 600 bytes.
- **0812F3CC — stopped below threshold**, retained locally at
  `wip/high/sub_0812F3CC.c`. Tentative alias
  `SceneActorReaction_BeginContactResponse`. Selects a contact, clears route
  counters, stores the selected node at group+0x88, installs idle control and
  state (3,1,0), clears flags2CC bit 0x10, sets flags2D6 bit 0x800.
  An explicit failure label before the success body restores the short
  conditional branches. Clearing the group-relative control via a typed
  pointer restores its +0x18/+0x14/+8 stores. Group pinning helps allocation.
  Reviewed-span score **72.3%, 52 total / 17 structural differences**, C 332
  versus 340 bytes. Zero-value reuse across the contact-selection call differs
  from the ROM's distinct word/byte zeros (including ip). Setter-width,
  selected-contact/actor pins, explicit zero parameters and separate pinned
  zeros did not reach the draft threshold. Nothing promoted.
- **0812E118 — stopped after assembly/caller and extent review.** Its callers
  in the SceneActorPath family use it to test a neighboring terrain cell
  against current height/shape. It rejects attributes 0x43, permits specific
  slope/flat transitions only along the matching axis, and compares heights
  exactly or within one height unit for those transitions. However, at
  0812E188/0812E18C it reads bytes at sp+4/sp before initializing the stack
  position after the subsequent Mod/Div calls. This is a real independent
  prologue, not an interior split. Reproducing it directly would introduce
  uninitialized C reads, so no candidate was promoted. Its actual return is
  at 0812E2BA (the body ends at 0812E2BC); the generated assembly then retains
  **0x120 bytes of separate raw Thumb code** through 0812E3DC. Any future
  exact-credit claim must review that boundary and must not absorb the raw
  body into a candidate-length check.

Read the large functions with asmat/condense and used hypothesis batches with
variants, scoring structural/total realigned differences before positional byte
counts. `relatives.py` was unavailable because its progress-report dependency
exited nonzero; the Ghidra export is absent in this checkout. No tooling or
shared documentation was changed. All three retained drafts have file-local
aliases and explicit sub-address INCLUDE_ASM alternatives. Their assembly
alternatives passed per-file checks; the two smaller alternatives used reviewed
extents (800 and 596 bytes). Final C scores were recorded with
`check.py --nonmatching --record`. Formatting and `git diff --check` passed.
Combined ROM validation remains with the batch integrator.

# Codex high: task 0819F788 (2026-10-20)

Tags: #matching #hresvelgr #terrain #player-positioning #register-pinning

Branch: `cloud/0819F788`. Worked solo and offline, largest functions first.
Used the worktree Python and fast per-file checks; no make, full build,
network operations, generated-file edits or direct Git mutations. Function
commits and this note were queued through `build/commit.sh`.

Exact coverage of the assigned functions' reviewed body/pool extents is
**28.52% (+28.52 percentage points; 1,620 of 5,680 bytes)**, all in the
Register-pinned category. The two guarded drafts add no exact coverage.
Reviewed extents: 0819F788=460, 0819FC34=616, 0819FE9C=1,564,
081A0BC8=732, 081A12DC=544, 081A14FC=1,764 bytes. Each ends at the
following independent function's prologue; included literal pools were
reviewed in the assembly listings.

The neighboring named routines and `Hresvelgr_ApplyRequestedAction`
(081A1EA4) establish the Hresvelgr action-callback family. The scene-message
dispatcher at 081A2430 specifically installs 081A12DC for command 26/action
31. Names below are tentative, with file-local aliases and confidence comments.
Local observed layouts remain in their own translation units.

- **0819F788 matched**, `Hresvelgr_UpdateAlternatingVolley`, 460 bytes.
  Entry plays a side-specific sequence from 0824F72C at duration scale eight,
  clears 02000234 and plays song 0x4D2. Each animation pulse adds the selected
  offset at 0x338 to the actor position, emits four DirectionBurst shots,
  alternates the sequence, then finishes after the counter exceeds fifteen.
  The three random samples must share one local RNG index across the signed
  remainder call at 08249350; independently reading the global after that
  call creates a reload. The remainder routine is declared const. Computing
  the offset inside an inline taking the actor and origin reproduces pointer
  evaluation order. A separate automatic sample plus an r0 remainder local
  preserves the ROM's arithmetic shift; pinning the sample itself changes
  it to a logical shift. Register/barrier hints also retain index, angle,
  speed and loop-counter lifetimes. `depin.py --write` reduced 26 hints to
  thirteen; formatted source passed the reviewed-extent exact check.
- **0819FC34 matched**, `Hresvelgr_UpdateTurningCharge`, 616 bytes.
  Resets a signed turn at 0x334, faces the target heading, and starts the
  charge animation. It corrects the transverse coordinate toward target
  0x110, finishes near the target or beyond the boundary, and enters a brief
  random +/-16-heading turn on contact flag 0x10. The materialized player
  availability and boundary booleans preserve the original branches. A mask
  local in r1 fixes heading quantization's operand tie. An inline flags
  setter with an opaque temporary actor address prevents an unwanted flags
  pointer from remaining live across Callback_InvokeActor; its mask is
  evaluated first. `depin.py --write` reduced seven hints to four; formatted
  source passed the reviewed-extent exact check.
- **081A12DC matched**, `Hresvelgr_UpdateRisingProjectile`, 544 bytes.
  Entry restores target heading, starts animation 25 at duration scale 112,
  raises the render position by 512 and launches a moving projectile with
  vertical delta 48. On the next pulse it approaches Y=832 in four-unit
  steps and finishes at that height. Returning the direction-field pointer
  from SetHeading gives it the required lifetime without computing it before
  the entry check. CopyVector followed by a halfword-pointer Y update avoids
  agbcc's word read/modify/write of the stack vector. The last two reordered
  instructions disappeared by masking the angle in the PolarVector call
  argument as well as inside the helper; a barrier or a narrower argument
  made the code worse. `depin.py --write` reduced four hints to one. Final
  formatted source passed the full 544-byte check.
- **0819FE9C drafted 86.1%**, `Hresvelgr_UpdatePlayerRelease`, reviewed
  1,564 bytes, candidate 1,560. Coordinates a player position request,
  animation stages, an upward movement toward a free terrain-cell center,
  landing, camera shake, and the final player position request. Terrain
  attributes 0x81 reject release cells; it checks four cardinal neighbors
  before falling back to (0xB54, 0, 0xB54). The landing path intentionally
  recomputes terrain height before storing it. Returning immediately from
  each branch of the signed fixed-point shift helper substantially improves
  its generated branch order. Passing the RNG table into the sample inline
  improves allocation. Final realigned differences: 123 total / 35 ignoring
  registers, chiefly release-target address/vector lifetimes and RNG
  allocation. No register hints were retained in this draft.
- **081A0BC8 drafted 93.9%**, `Hresvelgr_UpdateBoundaryBarrage`, reviewed
  732 bytes, candidate 736. Waits 129 frames, positions the actor on the
  selected boundary, configures collision mask 0xD000 and bounds at 0x704,
  traverses the arena, and emits falling projectiles over valid terrain
  every eighth tick. At the far edge it restores mask 0x2F0/bounds 0x424 and
  finishes. An inline returning the enabled-flags pointer recovers the
  original cached flag value. Loading raw RNG samples before loading the
  actor coordinate preserves the interleaved sample/position order. Final
  realigned differences: 24 total / three ignoring registers. Remaining
  issues are animation arguments/state-one lifetimes and RNG arithmetic
  allocation; broader wrap/state hints regressed. Register hints are marked
  as concessions. Depinning on the unguarded candidate correctly skipped it
  because the minimizer only accepts exact matches.
- **081A14FC stopped below draft threshold**, tentative role
  `Hresvelgr_UpdateDefeatSequence`. Reconstructed all five states: entry
  animation/event script, eight increasingly fast movement stages with
  randomized motion effects, landing/bounces with particles, event flags
  0/3/0x4E and the second event script, then completion. Best complete check
  reached 53.8% (candidate 1,540 versus reviewed 1,764 bytes), with 465
  realigned differences / 203 ignoring registers. The main discrepancy is
  the ROM's repeated precomputation of the four final motion-field addresses
  around switch edges; the candidate sinks them to the common tail.
  Scalar fields, inline argument forms, explicit pointer locals, volatile
  input/opaque-output barriers, pointer pins and pure div/mod declarations
  did not recover that pattern. Nothing below threshold was promoted;
  scratch sources and variants remain only under ignored wip/build paths.

Ghidra's export is absent. `relatives.py` fails in its progress-report
dependency, so used neighboring C and assembly directly. Large functions
were read with `asmat.py`/`condense.py`; hypothesis batches used variants
and structural/total anchordiff counts. No tool or shared header was changed.
Both drafts have NONMATCHING guards, raw sub-address INCLUDE_ASM alternatives,
recorded reviewed-extent scores, and byte-identical default assembly checks.
Source review, formatting and `git diff --check` passed. Combined ROM
validation remains with the batch integrator.

# Codex high: task 081C2570

Tags: #matching #control-panel #sparkle-emitter #sprite-controller #scene-npc #register-pinning

Branch: `cloud/081C2570`. Worked solo and offline. Used only per-file checks;
no make/full build, network operations, or direct Git mutations. Each source
was committed separately through `build/commit.sh`. Combined ROM validation
remains with the batch integrator.

- **081CD484 drafted 93.8%**, `SceneNpc_DispatchCutsceneCommands`.
  Reviewed extent 2,448 bytes; candidate 2,444. Reconstructed the 109-entry
  NPC cutscene command switch, including kind-dependent states, target
  movement, attachment effects, drops, palette changes and trigger consumption.
  Commands 0 and the common state-change tail set the changed byte at 0x08;
  command 103 changes the kind-29 variant and motion vector. Command 69
  copies the context position and adds 200 to signed Y before spawning its
  effect. Final realigned differences: 79 total / 13 ignoring registers.
  Remaining issues are facing-mask registers, motion setup in command 103,
  and copies/narrowing in the kind dispatch. Typed packed vectors and keeping
  argument loads before the stack position copy improved the first 89.5%
  candidate substantially. Read `docs/BIG_FUNCTIONS.md`, used condense and
  anchored comparisons, and stopped after register hints and branch-motion
  variants regressed. No hints retained. Its default INCLUDE_ASM uses the
  existing named symbol `SceneNpc_DispatchCommands_081CD484`; the new local
  alias targets `sub_081CD484` for integrator naming promotion.

- **081CF62C matched**, `SceneNpc_UpdateDescendingArrival`, 544 bytes.
  Tentative arrival-state interpretation: entry animation 9, raise Y by 160,
  initialize height offset 300, animation 10 after frame three, then lower
  toward terrain while damping unsigned horizontal speed to three quarters.
  At rest plays song 680 and animations 11/12, then sets done. The callback
  is present in the NPC state table; neighboring SceneNpc_PlayAnimation12
  uses the same flags, animation callback and tag height field. Scalar
  terrain-height lookup and a polar-vector inline matched on the first check.
  Final formatted source passed `check.py --target-size 544`. No register
  hints or handwritten instructions.

- **081C2570 matched**, `ControlPanel_InitializeFromScript`, 636 bytes.
  Called by ControlPanel_CreateFromScript (081C27EC). Initializes three
  C046 sprite/animation pairs, the icon rows and reward balance display,
  installs ControlPanel_UpdateIdle and reads options p/N/r/t/b/e/a. Option t
  defaults to 180/300/600; r defaults to 300. Starts song 0x23E and publishes
  the singleton. Explicit panel halfword fields removed redundant constant
  copies; signed halfword arithmetic preserved the -0x300 coordinate delta.
  Separate early/late zero lifetimes and computing the 0x590 field address
  before initializing the late zero recovered instruction order. Depinning
  reduced three hints to two (animation r5 and sprite r4). The unit carries
  MATCHING_CONCESSION: register-pinning. Final formatted source passed the
  reviewed 636-byte check.

- **081C29A4 drafted 91.6%**, `SparkleEmitter_EmitParticle`, 532 bytes.
  Called every other update by SparkleEmitter_UpdateEmitting (081C2BBC).
  Allocates a free pool entry when speed is greater than one, activates it,
  chooses a randomized screen-edge position of radius 150, converts it into
  world coordinates, then sets jittered angle/speed and clears particle age.
  Three RNG samples use angle masks 63/15 and speed mask three. Final
  realigned differences: 32 total / 14 ignoring registers, chiefly RNG table
  load/index-store order and allocation. Loading samples and base fields into
  separate locals, and separating angle subtraction from speed addition in
  the polar inline, recovered the original shape. A raw-address RNG variant
  regressed and was discarded. No hints retained. Reviewed score recorded;
  default assembly path checked byte-identical.

- **081C3C1C matched**, `SpriteController_UpdateInterpolatedPosition`,
  488 bytes. Installed by the F3E4 move/scale setup at 081C371C. Interpolates
  scale from 0x94/0x96 and X/Y/Z from 0xDC/0xE4, using signed timer 0xF8 and
  unsigned duration 0x9A; projects the result relative to camera 03005418,
  advances the timer, and sets result 0xD8 after the duration. Packed 16-bit
  vector fields reproduce stack word insertions. Projection needs a distinct
  pointer view, component/sum temporaries, and multiplication before the
  signed divide-by-32 inline. Register hints diagnosed the final 13 bytes;
  `depin.py --write` removed all seven after the source temporaries were
  established. Final formatted source passed the reviewed 488-byte check.
  No concession marker remains.

- **081CEFCC stopped below draft threshold**, tentative
  `SceneNpc_FollowSceneTarget`. Script behavior two (081D10D8) and the NPC
  creation paths select this update. Context 0x438 supplies a target with
  position at 0x30 and heading at 0x2D; context 0x434 supplies the comparison
  position. Squared distance above 0x90000 snaps the NPC 0x300 behind the
  target; distance above 0xFFFF sets speed-24 motion toward it. In the near
  band, vertical separation <=255 and two heading tests set display flag
  0x800. Entry/normal animation mode arguments are four/one. Best complete
  unhinted candidate: 64.0%, 656 versus reviewed 664 bytes, realigned 147 / 31
  ignoring registers. A later hinted candidate reached 68.3% but retained
  more structural differences and was discarded. Remaining difficulties are
  distinct motion/position aliases, the X-difference spill, PRE pointer
  copies before the distance branches, facing allocation, and the shared
  final vector store. Stopped after scalar-distance, inline-argument,
  pointer-lifetime and pinning variants stalled. Retained only the unhinted
  scratch source under ignored `wip/codex-high/sub_081CEFCC.c`; no below-80%
  source was promoted or committed.

Ghidra's export was absent. `relatives.py` failed in its progress-report
subprocess, so assembly, neighboring C, symbol names and data-table references
provided the evidence. Both guarded drafts compile and their default assembly
paths compare byte-identical; this is not exact-C progress. All three matches
were checked over their complete reviewed extents. Shared headers, generated
files and shared documentation were untouched. An unrelated modification to
`tools/cloud_task.py` appeared during the session and was left untouched.
