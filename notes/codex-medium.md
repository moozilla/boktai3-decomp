# Codex medium worker: task 080D6FDC

Tags: #matching #scene-actor #terrain #mummy #dog

Worked offline on the already-claimed cloud/080D6FDC branch. Only fast
`tools/check.py` compilations were used; no full build or network operations.
Commits were queued through `build/commit.sh`.

- **080DE57C — matched, Dog_ResetActionState (tentative).** Clears context
  flags at 0x678, timer 0x690, selection 0x6A0, timer 0x6A4, bytes 0x6A6/7
  and the eight-byte region at 0x6D8. Resolves the route position, selects
  directional animation entry zero and steps the drawable sequence.
  `Ivy_TickDrawable` (080F05E8) supplied the exact sequence stepper. Replacing
  raw context-offset casts with a narrow typed context removed all remaining
  constant-address and register differences. Exact checker: 336 bytes.
  Register-pinning concession retained from the stepper.
- **080DDEC8 — matched, Dog_RestoreRoute (tentative).** A close relative of
  `Golem_RestoreRoute` (080C4A9C): chase restoration uses delay 20 rather than
  30, and sets group+0x40 bit 4 after updating the capped chase timer.
  Addressing that flag through the group pointer reproduced the ROM's
  `mov r0, sb; adds r0, #0x40`; local r0/r1/r2 hints closed six remaining
  register differences. Exact checker: 608 bytes. Register-pinning concession.
- **080DE128 — matched, Dog_SelectTargetAction (tentative).** Called from
  080E3790. Within target range and a heading difference of at most 23,
  selects response 0x75 or 0x82 depending on actor+0x1E6, subject to context
  range 0x67C and timer 0x698. A tighter heading threshold of 5, range 0x680
  and timer 0x69A select response 0x87. Otherwise falls back to 080DF400
  unless context flag 8 is set. Preserving the two separate fallback checks
  and the shared inner failure label fixed block order. Register constraints
  prevented the 0x100 flag constant from being held across the heading
  calculation and reproduced the absolute-value result and final flag
  addresses. Exact checker: 772 bytes. Register-pinning concession.
- **080D6FDC — retained draft 80.6%, Mummy_RequestStraightRoute (tentative).**
  Started from the existing draft, added the required descriptive local
  alias, and re-recorded the score. Scans in facing order for the last open
  same-level cell, requests a route, and sets context+0x6D2 to 240 on success.
  The existing candidate still has 105 realigned differing instructions,
  17 ignoring registers. Passing an explicitly pinned map-address pointer
  through the terrain helpers and pinning the actor/cell/level worsened the
  candidate; those experiments remain outside src. The map address spill
  and larger stack frame remain the principal known differences.
- **080D9B20 — drafted 85.3%, SceneActor_FindDirectionalCell (existing name).**
  Keeps the current cell unless a requested cardinal neighbour passes a
  same-height and attribute-mask 0x43 check. Assembly repeats an adjacency
  predicate with four explicit branches. The draft preserves those tests,
  but agbcc merges the passability bodies and removes redundant intermediate
  booleans/cell comparisons: 681 realigned differing instructions, 574
  ignoring registers, over the full reviewed extent. Attempts using loops,
  separate boolean branches, and register constraints did not improve the
  retained candidate. This is a guarded fuzzy draft, not exact progress.
- **080D812C — stopped below draft threshold (46.5%).** Timed burn-damage
  update (proposed Mummy_UpdateBurnDamage, low confidence): actor-group timer
  0x1F0, periodic effects and sound, damage every 40 ticks, HUD/palette
  feedback, then defeat control and saturating category/reaction statistics.
  The actor action 0x15 extinguishes its effects. Matched routines
  08132D2C and 080EE1B0 supply most statistics code; 0813599C supplies the
  local-player HUD eligibility predicate. Candidate compiles but still has
  3051 realigned differing instructions, 193 ignoring registers. Extensive
  repeated counter blocks, scratch-register/pool layout and far-branch tails
  dominate. The Ghidra export is absent. Candidate stays in untracked wip;
  no below-threshold draft was added to src.

The three exact functions are all in the Register-pinned category. Guarded
scores were recorded with `tools/check.py --nonmatching --record`; their
normal build paths retain INCLUDE_ASM. No shared documentation was changed.

# Task 08131D5C — scene actor damage and defeat

Tags: #matching #scene-actor #damage #defeat-stats

Worked offline on the preclaimed cloud/08131D5C branch. Used only fast
`tools/check.py` compilations for validation; no make, full build or network.
Each function was committed separately through `build/commit.sh`.

- **08134398 — matched, SceneActorReaction_RecordDefeat.** Started with the
  existing guarded draft and read its matched sibling 08132D2C plus
  `notes/defeat-stats-template.md`. Rebuilding the opening from the sibling,
  dropping the contact-node release/group-link flag work and keeping the
  target's own reset fields gave the right behavior. The decisive change was
  the full-width signed bitfield for flags2D6: it creates separate SI/HI zero
  values before the halfword store, which makes the spill/reload register set
  match throughout the repeated statistics blocks. Passing the clear mask to
  a small inline puts the literal load before the halfword load. Counter
  increments retain two explicit stores and macros use plain braces rather
  than do/while wrappers. Exact reviewed extent: 5636 bytes. **No register
  pins remain** in this unit.
- **08132490 — matched, SceneActorReaction_ApplyCategoryDamage.** Called for
  actor categories 11/12/13 by SceneActorBehavior_DispatchFlaggedCategory
  (08132898). Applies reactionSoundVariant damage, palette/HUD/damage-number
  feedback, clears collision timers, and hides/redirects a defeated actor;
  surviving actors play song 0x13E. Started from the matched 08132748 defeat
  setup and 08135F70's damage-display predicate. Loading the damage into a
  local before subtracting fixed initial load order; changing the predicate's
  mask register from r3 to r2 closed the remaining allocation differences.
  Exact reviewed extent: 696 bytes. Register-pinning concession.
- **0813214C — matched, SceneActorReaction_ApplyStaggerDamage.** Sibling of
  08132490 for the dispatcher's default category path. Surviving actors enter
  control kind 0x21 with SceneActorReaction_PlayStaggerBurst; the final
  flags2CC bit 0x2000 is set on either health path. A signed full-width
  flags2D4 bitfield plus an inline mask parameter reproduced the early byte
  zero and halfword zero. A short r0 bit-mask constraint fixes the stagger
  flag OR order. Broader zero/collider pins caused unsafe-looking register
  overlap and were discarded. Exact reviewed extent: 836 bytes.
  Register-pinning concession.
- **08131D5C — drafted 85.4%, SceneActorReaction_ApplyAreaDamage.** Called by
  08135F70 when no pending hit exists. Requires enabled, clear flags2D0 bit 8,
  clear flags2CC bit 2 and zero collider responseValue. Gets damage from
  AreaEffect_ScaleGainAtPosition using the position and unsigned max health,
  applies it, generates two random knockback components, displays feedback,
  sets a 38-tick response timer and control kind 0x12 (or defeat kind 17).
  Candidate has 96 realigned differing instructions, 42 ignoring registers.
  Pointer spill placement, RNG expansion and load/store scheduling remain;
  the candidate is 12 bytes shorter than the 1008-byte original extent.
  Tried shared failure labels, call/output-pointer helpers, high-register
  hints, typed collider access, paired RNG expansion and bitfield OR forms.
  The typed collider/shared-failure candidate was retained; other experiments
  remain untracked under wip/med. Register-pinning concession in the common
  display predicate and short flag-mask lifetime.
- **08132904 — drafted 81.1%, SceneActorReaction_ApplyLinkedAreaDamage.**
  Shares the preceding area's damage flow, selecting
  AreaEffect_ScaleLinkGainAtPosition in link mode, and additionally clears
  flags2D4 bit 0x2000 after starting knockback. Candidate has 126 realigned
  differing instructions, 58 ignoring registers, and is 16 bytes shorter
  than the 1064-byte original extent. The same pointer/RNG issues remain.
  Register-pinning concession.
- **08135F70 — improved draft 86.8%, SceneActorReaction_ResolveDamage.**
  Resolves pending hit damage and flags, or falls back to 08131D5C for area
  damage. Added the required name and concession marker. Keeping mode in sl,
  preserving the constant one in sb across the HUD call, and constraining
  short-lived hit masks improved the existing 84.4% draft. Removing its old
  updated-value r5 pin reduces structural differences further: retained
  candidate has 79 realigned differing instructions, 18 ignoring registers.
  Health/flags2D8 pointer spills and the two inlined hit-flag expansions still
  differ; candidate is 1152 bytes versus the original 1156-byte extent.
  Forced stack barriers and output-pointer helpers worsened the structure
  and were discarded. Register-pinning concession.

All guarded scores were refreshed with `check.py --nonmatching --record`;
normal builds retain INCLUDE_ASM for the three drafts. Exact checks used
explicit reviewed extents for all three matches. Batch ROM validation and
name promotion are left to the integrator as instructed.

## 2026-10-20 — task 080D3D60, Mummy route and attack callbacks

Tags: #matching #mummy #navigation #nonmatching

No exact matches; exact code progress gain is +0 percentage points. Five
NONMATCHING drafts retain the original INCLUDE_ASM in default builds. Names
are provisional file-local aliases using the neighboring Mummy prefix.
No network operations or full builds were run.

- **080D5778 — drafted 88.3%, Mummy_ResumeDelayedRoute.** Consumes behavior
  entry, restores saved control, clears route/movement flags and a group
  timer, then waits for counter2AC > 60. Schedules a delayed restoration;
  when context+6D2 is set and the target goal cell equals terrainCell,
  invokes 080D6FDC or chooses a different route entry. Otherwise runs the
  common route reacquisition logic. Final realigned residue: 134 instructions,
  39 ignoring registers. Inlined random-turn allocation, flag-load ordering
  and target-pointer spills remain. No register pins in the authored unit.
- **080D6608 — drafted 87.0%, Mummy_TryDirectPursuit.** Checks group+64 against
  gUnk_02000508 and tests the target's goal cell along straight grid rows and
  columns via 0812E118. On failure, retargets the first player and uses the
  common route reacquisition logic. The matched 080D6DF8 CanReach/FindPath
  code supplies the first phase. Final residue: 179 instructions, 28 ignoring
  registers; allocation after the straight-path search dominates. Pinning
  the goal/current cell/height/kind did not improve it; retained without pins.
- **080D3D60 — drafted 82.4%, Mummy_UpdatePlayerRoute.** This is a control
  callback: entry uses field2B2, selects animation zero and sets flags2D0
  bit four. 080DA33C selects behavior 16, and behaviors 16/18 select walking
  or idle animation. When context+6D6 is clear, assigns player zero's target
  descriptor, clears flags, and reacquires its route; successful rerouting
  selects group+48 control kind 0x25 and behavior 12. Final residue: 194
  instructions, 70 ignoring registers. Control-state store ordering and
  common turn/route pointer allocation remain. No authored register pins.
- **080D53F8 — drafted 80.0%, Mummy_UpdateProbeNavigation.** Calls the update
  callback and movement-rate callback (+260), updates drawable duration scale,
  consumes a navigation probe event, then requests a descriptor turn or
  acceleration. Probe turns save control/state and use group+206 value two
  (the common arrival turn uses six); alternate branches select behaviors
  13/14. Final residue: 95 instructions, 30 ignoring registers. A scoped
  context+6F2 clear and a wide saved-heading local improved structure.
  Retains one r6 speed pin, marked as a concession. depin skipped it because
  it is not exact; broader actor/body/group pins all worsened the candidate.
- **080D62CC — drafted 94.6%, Mummy_FireAcceleratingVolley.** Called at frame
  four by SceneActor_UpdateAnimationCheckpoint (080D0E20). Raises the source
  position by 200; target height selects a divisor for the greater horizontal
  displacement component. Clamps speed to [10, context+6CC], constrains aim
  near a cardinal heading, then invokes 080692C8 one to three times based on
  paletteOffset. Each iteration scales strength by 8/10 and adds speed*4/10;
  plays song 0x101. A ContactPackedVector source reproduces the packed-word
  Y update. Retains r6 count/r8 strength pins. Final residue: 17 instructions,
  10 ignoring registers; quotient copy, argument load order, loop pointer
  setup and initial height-difference allocation remain.
- **080D453C — stopped at 76.1%, Mummy_TryStimulusReaction (local candidate
  only, wip/sub_080D453C.c).** Clears the temporary activity flag, rejects
  blocked states, finds a stimulus, initializes a 180-tick timer/control kind
  nine/behavior 11, resolves its tagged position, tries player tracking and
  a route toward the stimulus, and retargets the nearest player. Named inline
  flag predicates improved 65.3% to 76.1%; local register pins and declaration
  swaps stopped improving it. Residue: 121 instructions, 53 ignoring registers,
  concentrated in entry guards, zero sharing/state stores and route branches.
  Below the authorized draft threshold, so production source remains assembly.

Useful technique: scene_actor_chase.h already describes almost all of the
large route bodies. Replacing its random-turn expansion with the semantically
same PickTurn/Turn form from 080C09F0 improved 080D6608 from 76.0% to 87.0%
and 080D5778 from 83.3% to 87.9%, before minor signed-shift/route-field cleanup.
For 080D53F8 the original helper form was better. Compare realigned counts,
not raw byte differences: large byte deltas here include shifted literals and
register changes. The Ghidra export was unavailable; reconstruction used asm,
matched relatives, neighboring callbacks and shared headers. All five draft
scores were recorded with check.py --nonmatching --record. Full-ROM validation
and any alias promotion remain integration work.


# Task 081875EC — Armor attack decisions

Tags: #matching #armor #actor-controller #nonmatching

Worked offline on the already checked-out cloud/081875EC assignment. Read
CLAUDE.md, AGENTS.md, CLOUD.md, WORKER.md, HANDOFF.md and BIG_FUNCTIONS.md.
No network, full builds or direct git mutations were used. The existing
unrelated tools/cloud_task.py modification was left alone. No exact progress
was added: percentage-point gain is 0. This task retains one guarded draft.

Read the Armor-named neighboring callbacks (including 08186618), callers
08189C54/08189C80/08189CAC/08189CD8, and the behavior-setting callees in the
assembly. The six routines are actor decision state machines. They compare
player coordinates at 0x30/0x32/0x34 to actor coordinates at 0x48/0x4A/0x4C,
round horizontal deltas with (delta + 128) >> 8, and dispatch through the
byte at actor+0x1120. Actor+0xB32 bit one selects alternate decision paths;
0xB39 is facing, 0xB3C behavior and 0xB3D sequence phase. Names remain tentative.

- **081875EC — drafted 83.9%, Armor_UpdateAlignedAttackDecision.** Selects
  attacks on a shared horizontal grid axis; checks vertical separation <=127
  and horizontal range <=1/3, with a separate flag-dependent path. The normal
  idle path increments 0x1126 and can select behavior 31 after 300 ticks when
  Armor_IsSequenceIncomplete succeeds. Decision 1 waits for cooldown 0x1122;
  decision 2 requests behavior 23 and clears two timers. Retained an r2 pin
  for the absolute Z delta, marked MATCHING_CONCESSION. It removed an extra
  saved high register and reduced realigned structural residue from 31 to 14
  on the scratch check. Final reviewed-extent check: **83 differing insns,
  12 ignoring registers**. Remaining: branch polarity/far branches, merged
  decision stores, address derivation and scratch register choices. Pinning
  more deltas or the cooldown pointer did not close these differences. The
  default INCLUDE_ASM path passed check.py exactly (956 bytes); this is not
  an exact C match. Score recorded with --nonmatching --record.
- **08187F44 — stopped below threshold; best fuzzy 75.9%.** Proposed
  Armor_UpdateCloseRangeDecision. Decrements attack and collision cooldowns
  twice in fast mode, gates collider flag four, and toggles fast mode every
  300 ticks outside the alternate flag path. The two decision switches share
  approach/recovery logic; side attacks randomly choose behavior 36 or 24.
  Full candidate stays in wip/sub_08187F44.c. The retained structural best
  has 213 realigned differences, 99 ignoring registers (69.5% fuzzy); the
  higher fuzzy variants had worse total alignment. agbcc merges repeated
  facing loads and cooldown-address formation that the ROM repeats. Pinning
  decisions, angle and coordinate values, pointer forms and inline expansion
  did not produce an eligible draft.
- **081879A8 — stopped at 75.3%, Armor_UpdateMeleeDecision.** Near sibling
  of 08187F44 without its four random side-attack choices; each chooses
  behavior 24 directly. Retained scratch candidate has 211 realigned
  differences, 93 ignoring registers. The same facing-load caching and
  cooldown-tail/address sharing remain. A narrower Math_Atan2 declaration
  improved two instructions but did not resolve the body. No source draft.
- **081885BC — stopped at 68.7%, Armor_UpdateDefensiveDecision.** Six-case
  defensive decision switches, attack/secondary/collision countdowns,
  sequence-dependent recovery, and restoration of saved fields 0x112C/0x112E
  to 0xB26/0xB28/0x274. Clears mask 0x00200004 in two control flags on
  restoration. Normal decision 1 can select behavior 31 from timer 0x1130.
  Scratch candidate: 248 realigned differences, 113 ignoring registers.
  Pinning deltas and absolute values improved the first pass from 55.8%,
  but shared jump-table tails, pointer lifetimes and flag access shapes remain.
- **08188B7C — stopped at 66.2%, Armor_UpdateRetreatDecision.** Close sibling
  of 081885BC: special type 13 instead of 12, direct behavior-29 requests,
  and normal decision 1 chooses behavior nine when facing the player at a
  Manhattan distance over four (instead of the timer/sequence check).
  Scratch candidate: 277 realigned differences, 106 ignoring registers.
  Same jump-table and shared-tail problems as the defensive variant.
- **0818911C — stopped below threshold, Armor_UpdateAggressiveDecision.**
  Its expiring 0x112A countdown resets decision/mode, restores 0xB26,
  clears 0x1128 and control bits 0x00400008. Selects behaviors 37/39 or
  38/2 from fast mode and secondary cooldown, with 360-tick attack cooldowns.
  Scratch baseline: 52.9%, 395 realigned differences, 223 ignoring registers.
  Delta/absolute-value pins raised coarse fuzzy to 65.2% while worsening
  realigned structure (344), so retained the baseline. Shared flag-dependent
  cases and restore/control accesses need structural work before allocation.

Useful techniques: condense.py exposed the four repeated RNG/side-attack
blocks; matched relatives supplied Armor subsystem naming and facing-angle
rounding. Keep raw and absolute deltas separate: an r2 absolute-Z pin was
useful only in 081875EC. Expanding repeated inline attack choices improved
large-function structure. Compare realigned counts before accepting a fuzzy
increase: several pinning variants increased fuzzy while worsening alignment.
Ghidra export gen/ghidra/decomp.c is absent. All five below-threshold candidates
remain local in ignored wip/, outside production source. Commits are queued
through build/commit.sh; no shared documentation/index was edited.

# Codex medium worker: task 0817BDB0 (2026-10-20)

Tags: #matching #scene-volume #actor-decision #boss-slot #nonmatching

Worked offline on the claimed cloud/0817BDB0 branch. Five exact matches recover
41.79% of this assignment's code extent (+41.79 percentage points from its
unmatched baseline; 3,168 bytes), equivalent to +0.1312 percentage points of
project code using the PROGRESS.md denominator. The sixth function is a guarded
draft and contributes no exact progress. No register pinning was retained.
Only check.py per-file compilation checks were used; no full build, network,
direct git mutation, shared documentation or progress regeneration. Commits
went through build/commit.sh. The pre-existing tools/cloud_task.py edit was
left alone.

Read the neighboring ActorDecision, BossSlot2 and ActorEffect implementations,
the task assembly and callback tables gUnk_08611C6C / gUnk_08611CB8 /
gUnk_08611CD0. The first table drives the decision choices; the second selects
the large decision routine; the third includes the movement/effect callbacks.
0817F444 also installs 0817E040 as an actor callback after an impact response.
Names are explicitly tentative: the actor's species is not established.

- **0817DA1C — matched, SceneVolume_UpdatePursuitSpin.** Starts animation 0x134,
  tests the path/player movement mode, pursues destination halfwords at
  0x2A4/0x2A8 with speed five, and rotates an effect by 16 angle units each tick.
  Collision status at the 0x32C effect gates sound 0x2E7. After elapsed time 96
  it requests phase two and restores the requested facing. Exact 940-byte check.
  Local masks declared before flag accesses reproduce the original instruction
  order; incrementing the initially zero impact flag avoids reusing phase one.
- **0817E040 — matched, SceneVolume_UpdateKnockbackLanding.** Reverses facing
  relative to impact angle 0x471, decays speed 0x472 by three quarters, adds
  height 0x468 to the terrain floor, and plays landing/recovery animations.
  The terrain lookup handles cell overrides and sloped shapes one/two.
  Exact 824-byte check. A temporary X result followed by a zero local before
  the velocity stores reproduces the zero load before the first store. A
  local mask for the terrain nibble fixes its operand tie; ordered phase/zero
  locals fix the phase-two transition stores.
- **0817D7AC — matched, SceneVolume_UpdateRepeatedShot.** Alternates aiming,
  conditional projectile creation through 0817D67C, and impact-count states;
  the byte at 0x473 is capped at three. Exact 624-byte check. The explicit
  timer-zero jump to the increment path retains the original separate zero
  and >10 tests; the equivalent combined expression was optimized differently.
- **0817DDC8 — matched, SceneVolume_UpdateImpactSlide.** Initializes effects
  through 0817AEC8 and the attack collider, then decays impact velocity by
  three quarters or three eighths according to byte 0x470. Once stopped,
  animation 0x116 completes before requesting the next phase. Exact 428-byte
  check on the first candidate.
- **0817BDB0 — matched, ActorDecision_UpdateHazardPath.** Chooses a random path
  and point, advances after pending events, spawns proximity hazards on the
  first three advances and requests a transition afterward. Exact 352-byte
  check. Materializing the RNG table pointer before advancing its index fixes
  the table literal's lifetime; duplicated movement/state stores in the two
  event branches reproduce the compiler's shared tail and literal-pool layout.
- **0817C3F4 — drafted 83.3%, BossSlot2_SelectCombatAction.** Complete three-mode
  selector with vitality thresholds at 0x274/0x276, summon tests at 0xC60/0xC64,
  prior choice 0x4E9, event index 0x4EB and distance threshold 0x23FFF. Chooses
  attacks in each explicit switch case, clears the latch at 0x428/retracts
  hazards at phase boundaries, and dispatches choices through gUnk_08611C6C.
  Early transition-byte absence jumps directly to callback dispatch. Read
  BIG_FUNCTIONS.md and inspected condense.py / relatives.py output first.
  Final --nonmatching --record check: 530 realigned differing instructions,
  407 ignoring registers, reviewed extent; candidate 4,520 bytes versus the
  4,412-byte target extent. The initial candidate was 63.3%; putting the zero
  argument before the choice in the inline selector and retaining an explicit
  event pointer raised it above draft threshold. Moving that pointer into
  individual branches or returning it from the inline changed cross-jumping
  and regressed alignment substantially, so those variants were rejected.
  Remaining: shared transition tails, repeated event-pointer materialization,
  block/literal-pool layout and register choices. No Ghidra export is installed.
  The initial draft commit title says 82.3% (candidate-length score); recording
  against the reviewed extent gives 83.3%, which is the score in the source.

All files were clang-formatted and checked again. Confidence comments were
moved immediately above the #define aliases because trailing comments caused
clang-format to wrap the alias over two lines, outside promote_names.py's
recognition pattern. Each alias correction was committed separately by unit.
No generated assembly or ROM-derived artifact was committed.
