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

# Codex medium worker: task 08216584 (2026-10-20)

Tags: #matching #background #palette #sprite-oam #nonmatching

Worked offline on the preclaimed cloud/08216584 branch, largest function first.
Exact assignment progress is 0.00% (+0.00 percentage points). Four guarded
NONMATCHING drafts retain the assembly default; they contribute no exact code
progress. Used only per-file check.py compilations, including those run by
variants.py; no make, full build, network, direct git mutations, shared docs or
progress regeneration. Commits went through build/commit.sh. Left the existing
tools/cloud_task.py modification untouched. Ghidra's decomp.c export is absent.

- **08216584 — stopped below draft threshold, BgGfx_LoadTilemap.** Reconstructed
  a complete candidate in ignored wip/sub_08216584.c from assembly and the
  neighboring Bg_SetLayerMap/LoadTilemapSimple/LoadTilemapAtPosition routines.
  A layer entry is 0x30 bytes: old tile origins at 0x10/0x12, source width at
  0x18, metatile dimensions at 0x1C/0x1E, pixel scroll at 0x20/0x22, metatile
  quadrants at 0x24, source map at 0x28, ring buffer at 0x2C. Source metatiles
  contain four halfwords. A destination column writes the selected top/bottom
  pair according to X parity. The bounded path substitutes zero outside the
  map, or tiles[map[0]] in mode 2; mode 1 wraps source coordinates. Large
  changes redraw 16 two-row strips; small interior/wrapping changes update
  exposed rows then columns. Small bounded changes update one column and/or
  one two-row strip. Read BIG_FUNCTIONS.md and used condense.py, then scored
  realigned differences. Initial wrapping normalization calls 08249350
  (signed remainder), whereas inner lookups call BIOS Mod; using Mod for both
  destroys call anchoring. Plain C % currently fails the isolated link with
  unresolved __modsi3, so the local candidate declares/calls 08249350 directly.
  Final candidate: fuzzy 40.9%, 951 realigned differences / 265 ignoring
  registers. Register hints improved one earlier fuzzy score to 55.0% but
  worsened structure to 462; that variant was rejected. Row/column inline
  parameter lifetimes, index-expression grouping and register allocation still
  differ substantially. No below-threshold source was committed.
- **08217430 — stopped below draft threshold, proposed
  Palette_ApplyObjectFade.** Called by InterruptsAndVideo_OnGraphicsUpdateTask
  after the background fade, unless the grayscale countdown is active. Copies
  cached object palette banks at full strength; otherwise scales RGB555 or
  blends toward 030051DC with level 030051D4 >> 1. Flag masks at 03005210
  exclude specific resource ids, and mode 030051C8 == 1 fills unprotected
  banks with 03005214. Full strength also requires mode zero and caches the
  previous level at 03004DAC. The two exclusion sequences intentionally have
  a repeated 0x3AA test. Best retained candidate in ignored wip is 68.2%,
  259 realigned differences / 102 ignoring registers. Pinning level to r10
  improved structure strongly; changing predicate return width, pinning the
  red/blue term or splitting loop locals did not improve it. Mask hoisting,
  materialized predicates and distinct register allocation across the three
  bank loops remain. No below-threshold source was committed.
- **08217960 — drafted 80.1%, Palette_ApplyBackgroundFade.** Gets the next
  background palette buffer and combines 03004DA0/030051D8 into a 0..64
  fade level. At full strength selects the original 03004FC0 buffer;
  at zero fills masked banks with 03005214 and copies unmasked banks.
  Intermediate values scale or blend selected banks in packed RGB555,
  optionally blending the backdrop override at 030051D0 as well. Final
  reviewed check: 82 realigned differences, 39 ignoring registers.
  Blended-bank constant hoisting, loop-counter setup and allocation remain;
  only the level r12 pin is retained as a register-pinning concession.
- **08218508 — drafted 89.1%, SpriteOam_QueueFlaggedProjectedSprites.** Used as
  a render callback by 08163398. Same queue construction as 08217F4C, with an
  additional required drawable flag 0x1000; its semantic meaning is unknown,
  marked confidence=low. Final reviewed check: 41 realigned differences,
  19 ignoring registers. Register-pinning concession.
- **08217F4C — drafted 88.9%, SpriteOam_QueueProjectedSprites.** Common renderer
  installed by scene/UI setup through Video_SetRenderCallbacks. Traverses
  030042D0[03004290], rejecting hidden/parity-selected sprites (0x401 or
  0x201). Projects world coordinates isometrically unless flag 16 supplies
  screen coordinates; applies drawable offsets and screen bounds. Queues
  affine matrices, flips, blend/window flags, and either direct OAM entries
  or clamped depth-sort entries. Final reviewed check: 41 realigned
  differences, 19 ignoring registers. Projection temporary address caching,
  matrix-counter increment shape, signed comparison selection and operand
  scheduling differ. Register-pinning concession.
- **08218254 — drafted 86.7%, SpriteOam_QueueCameraRelativeSprites.** Alternate
  renderer paired with 08214D20 by 08225CB8/08225DA0. Shares queue behavior,
  but uses (position-camera)/8 plus screen center for X/Y and subtracts
  camera Z directly. Final reviewed check: 45 realigned differences,
  14 ignoring registers. Camera/offset scheduling and the common matrix/OAM
  register differences remain. Register-pinning concession.

Useful techniques: preserve Vec's compiler-rounded eight-byte size when
laying out the 0x24 next pointer (a six-byte conceptual vector plus six bytes
of explicit padding incorrectly moves it to 0x28). Constrain only observed
register lifetimes: removing the sprite depth and matrix-count pins improved
77.9% to 86.8%; freeing projection-value pins improved it further. Computing
frame parity before declaring the skip mask reproduces the original constant
load order. variants.py made these isolated hypotheses cheap to compare.
Background palette level pinning alone reaches draft threshold; pinning source,
destination and buffer pointers as well regresses it. Pointer-to-pointer bank
helpers also worsened code generation and were discarded.

All four guarded scores were recorded with check.py --nonmatching --record
against the reviewed extent and rechecked after clang-format. Three sprite
functions and the background fade have separate draft commits; a follow-up
single-unit cleanup formats the background fade and removes a stale duplicate
score comment. No generated assembly or ROM-derived artifact was committed.

## 2026-10-20 — cloud task 08116E44

Tags: #matching #motion #glove #nonmatching

Exact gain: 0.000 percentage points; no matches and no qualifying guarded
NONMATCHING drafts. Worked solo on the preclaimed cloud/08116E44 branch.
Only per-file check.py acceptance checks were used; no full build or network
operation was run. The pre-existing tools/cloud_task.py modification was left
alone. Below-threshold C is retained only in ignored wip/ files in this worker
checkout, not in production src/ or this commit.

All six functions share the motion state at actor + 0x18 and the model at
actor + 0x610. The matched ActorMotion_UpdateFallFrame (081203A8) and
SceneActor_UpdateFallState (08120618) provide the damping, facing-angle and
collision-bounce core. These functions additionally hold computed addresses
across Div and model updates; copying the sibling body alone does not match
those lifetimes. Tentative names below use the neighbouring MotionActor prefix.

- **08117948 — stopped below threshold; MotionActor_UpdateStopping.**
  Glove_BeginStopping (08117294), Glove_StopFromScript (08119F44) and
  Glove_StopWithMessage (08119F9C) install it. Clears initialization byte
  0x868, sets halfwords 0x53C/0x53E to 0x88/zero, updates movement audio,
  clamps speeds, integrates damped motion, then handles action callbacks,
  animation resets and damage recovery. Candidate wip/sub_08117948.c:
  56.7% fuzzy, 403 realigned differences / 214 ignoring registers.
  Initial source was 54.9%, 422 / 220. Extracting repeated action-animation
  resets into inline C improved code slightly; pointer caching, zero-value
  lifetimes, bit-mask order and extensive allocation differences remain.
  The apparent sub_08249240 calls are bx-r2 thunks: actual indirect callbacks
  stored at actor + 0x2C8 and +0x32C, called with the address of their own
  callback field and zero. The C uses function pointers for those calls.
- **081182E4 — stopped below threshold; MotionActor_UpdateDefeat.**
  Installed by MotionActor_DispatchRequestedState when state flag 2 is
  consumed. Stops locally selected sounds, hides the five model animations,
  releases marker/action effects and sprite-pair resources, restores model
  palettes, zeros horizontal motion, and emits bursts every 12 then 6 frames.
  After timer >120 chooses 08117F6C for link/continuation modes or
  Glove_OnActionComplete otherwise. Candidate wip/sub_081182E4.c: 60.5%,
  344 / 180. Best intermediate structure was 333 / 171 at 60.6%; the retained
  version corrects the palette-data loop to reload the graphics base on every
  iteration, as the ROM does. Explicit address locals before the animation
  loop reduced initial structural differences from 219 to 171. Stack-slot
  order, the outer pointer's low/high register choice and palette-loop
  addressing still differ substantially.
- **08116E44 — stopped below threshold; MotionActor_UpdateDeparture.**
  Installed when MotionActor_DispatchRequestedState consumes state flag 4.
  Runs script at +0x908 once with a two-value argument record (result index
  and actor +0x900, or link slot/result data), clears that script, stops local
  sounds, and saves the 16-byte position at +0x8A4. Sets flags, clears pending
  damage, runs normal motion while horizontal displacement from that saved
  position is <=127 units, then switches to MotionActor_RunPendingScript or
  MotionActor_RunDelayedScript. Candidate wip/sub_08116E44.c: 59.4%,
  345 / 191. Argument construction and shared movement pointer lifetimes are
  reconstructed but allocation, mask setup and callback-transition ordering
  remain far from draft threshold.
- **08117F6C — stopped below threshold; MotionActor_UpdateRecovery.**
  Clears model animation flag 1 on entry, sets a three-frame blink countdown,
  integrates motion and alternates that flag when the countdown expires.
  Copies the resource/health target fields and raises current health toward
  its target by 20 when the gap exceeds 100, otherwise by 5. After timer >120,
  snaps to target, restores the marker/color effect and installs 08117948.
  Initial candidate: 59.0%, 198 / 75. Retained wip/sub_08117F6C.c uses signed
  health pointers: 57.1%, 197 / 63. That improves selection of ldrsh versus
  explicit sign extensions, though register differences worsen the fuzzy
  score. Duplicate signed/unsigned health loads, callback-address lifetime,
  flag-pointer caching and the shared motion core remain.
- **081172C4 — stopped below threshold; MotionActor_UpdateRisingCue.**
  Entry clears phase +0x918 and speed +0x914, creates StageIconDisplay with
  Glove_BeginStopping as callback, and sets actor flag 0x10. Phase zero adds
  0x20 acceleration to the horizontal offset until it exceeds 0x40000;
  phase three subtracts 0x100 from positive speed and advances to phase four
  when nonpositive. Runs shared motion and the locally selected looping
  sound. Best fuzzy candidate wip/sub_081172C4.c: 74.0%, 107 / 34; distinct
  outer/state struct views recover the required offset reload after setting
  the animation halfword. An independent 90-second permutation experiment
  produced a valid candidate with 198 differing bytes and 15 structural
  differences, but still only 69.8% fuzzy (118 realigned differences), in
  wip/permuted_081172C4.c. No uninitialized reads were retained. Most residue
  is self-pointer r5 versus ROM r7 and spill/copy lifetimes across damping.
- **081175F8 — stopped below threshold; MotionActor_UpdateRisingReturn.**
  Shares the rise/slowdown phases with 081172C4, omits StageIconDisplay and
  flag 0x10, and phase four installs 08117948 with active=1 and timer=0.
  Candidate wip/sub_081175F8.c: 69.3%, 137 / 48, improved from initial 65.7%,
  152 / 52 by using distinct outer and motion-state views. The same shared
  motion-core allocation issue remains, plus callback-transition ordering.

Useful techniques and unsuccessful experiments:

- asmat.py's count is a listing-line limit, not an instruction count; dump
  enough lines to include pools and the epilogue, then stop at the next
  thumb_func_start. condense.py confirms repeated Div and animation runs.
- Score large functions using realigned structural/total counts. Candidate
  byte differences alone were misleading: the permutation reduced 462 to
  198 differing bytes without approaching the 80% fuzzy draft threshold.
- A 127-variant pointer-cache sweep on 081172C4 topped out at 74.0% fuzzy.
  Caching more pointers early can improve structure yet sharply worsen
  allocation. Adjacent declaration, inline-boundary and alias experiments
  did not close the shared-core residue.
- Register-pinning experiments on the outer pointer regressed these sources
  and were discarded. No retained candidate has a pinning concession.
- The permuter initially failed to link sub_0811BD30; its promoted symbol is
  Actor_UpdateShadowSpriteParts. Replacing the obsolete local spelling in
  the isolated search input fixed that tooling issue. Final acceptance still
  used check.py; permutation scores were never treated as exact evidence.

## Task 080B1A98 — Vorn route/reaction cohort (2026-10-20)

Tags: #matching #vorn #scene-actor #terrain #route

Worked offline on the pre-claimed `cloud/080B1A98` branch. Exact task
completion is 0%; exact gain is +0.00 percentage points. Four guarded drafts
qualified; two candidates stopped below the 80% threshold. No full build,
network operation, direct git mutation, generated assembly commit, or shared
header change was performed. The pre-existing tools/cloud_task.py edit was
left alone. All commits were queued through build/commit.sh.

Final production candidates were formatted with the venv's clang-format and
checked with `build/venv/bin/python tools/check.py FILE --nonmatching --record`.
The default branch of each file remains INCLUDE_ASM. No retained production
candidate uses register pinning. Descriptive aliases are tentative, using the
neighbouring Vorn prefix. Ghidra's gen/ghidra/decomp.c was unavailable.

Per-function results (realigned counts are total / ignoring registers):

- **080B2858 — drafted 85.1%, Vorn_SelectRetreatRoute.** 134 / 48 differences.
  Context +0x6D0 is a 60-frame gate unless reset is requested; context +0x6C4
  increments, while +0x6C8 and +0x6CC bound actor distanceF0. The near case
  faces opposite fieldE9 and calls 080B2580. The other two cases independently
  jitter fieldE9 by a random value masked with 63, initialize a missing target
  from player zero +0x24, and attempt a target-cell route. Failure falls back
  to 080B2580. Success installs idle control 0x10 and behavior 0x15, setting
  actor flags2D0 bit 4. Context response flags are distinct from actor flags.
  Explicit promoted u32 halfword-update temporaries improved the candidate
  from 77.3% to 81.9%; separating the RNG load from its mask improved registers.
  Residue: inline actor/group copies and spills, success-tail sharing, and
  constant/address register allocation. Pinning the outer actor worsened it.
- **080B3464 — drafted 84.3%, Vorn_ResumeRouteAfterReaction.** 129 / 74.
  Rejects inactive/dead actors, flags2D4 mask 0xE33F, and actors without bit
  0x80. While group +0x44 is nonzero, action 0x25 with context flag 4 only
  ticks Vorn sound. Otherwise context flag 4 or 0x10 starts the same route
  recovery setup, restores animation entry zero and delayed restoration
  control 14, sets state/phase/behavior 3/1/0, and queues response kind 7
  for 60 frames with entryTarget228 as payload. It starts the configured
  group route and requests motion toward its point. The two paths clear
  context flags by ~6 and ~0x70 respectively, then send a Vorn message.
  Pointer-based SnapTarget removed aggregate scalarization (spurious word
  read/modify/write stores) and improved 77.1% to 84.3%. Mask-taking helpers
  were important for constant-before-load order. Residue: the actor copy
  and saved flag pointer that the ROM keeps across both duplicated paths,
  stack-slot lifetimes, and remaining route/control allocation.
- **080B2580 — drafted 81.6%, Vorn_SelectReachableEscapeCell.** 88 / 53.
  Tries cardinal sweeps at range 2, then range 1, rotating facing by 64 for
  up to four attempts. Each sweep calls 0812E118 with original cell, tested
  cell, original terrain height and original cell shape. Saves the last
  traversable cell and restores the supplied facing after each range pass.
  Builds the selected cell centre, tries the two route routines, clears
  motion when appropriate, and invokes the failure callback if needed.
  Explicit pre-check plus do/while loops reproduced the ROM's scan order;
  factoring Sweep with direct returns reproduced its case-tail placement.
  A signed shift helper retains the original rounding branches. Addressing
  the failure callback through group +0x224 crossed the draft threshold.
  Residue: retry-loop hoisting, terrain-pointer caching, switch exit order,
  and route setup allocation. Pinning the range did not improve it.
- **080B1A98 — drafted 87.3%, Vorn_BeginTargetPursuit.** 70 / 29.
  Clears context +0x6D4 bit 8, then requests 080B3110 when response bit 1
  is set or signed context +0x67C is nonnegative. Otherwise rejects actor
  flag 0x100, optionally dispatches context response flags (0x1D), and
  initializes delayed restoration plus target pursuit. Context response
  flag 4 can extend the capped group +0x44 timer by 300. It snaps the route
  destination to the configured point, sets destination pad to zero, then
  requests a route toward actor.fieldDC +12. Signed full-width bitfield
  access preserves the context-pointer reload after clearing the flag.
  Group-relative state5D4 access, typed response-command fields and
  mask-taking context predicates were useful. Residue: saved flags-pointer
  lifetime, phase/control constant evolution, and target-argument ordering.
  This selected candidate has slightly more structural differences than
  the 84.3% alternative (29 versus 28) but fewer total differences (70 vs 80).
- **080B1FA8 — stopped below threshold.** Retained locally at
  wip/vorn/sub_080B1FA8.c, 79.5%, 87 / 11. Tentative alias:
  Vorn_SelectEscapeRoutePoint. Computes a fixed-point direction toward the
  current target, then prefers route points whose horizontal dot product
  is negative, excluding the current point's first word. Notably, a negative
  dot product at the current point does not advance the point pointer;
  only the nonnegative branch advances it. If none qualifies, chooses a
  random route index and avoids the previous index where possible.
  Typed signed vector views kept the packed x/z values on the stack.
  Separate initialRoute (actor +0x448) and later group-relative route
  pointers reproduced lifetimes; deriving the destination inside TryRoute
  restored route-before-target loads. Diagnostic pins on group/initialRoute
  helped this uncommitted candidate. Remaining differences are mainly
  scratch-register allocation inside the packed-delta/dot loop, plus a few
  compare/load and route-mask sequences. Declaration swaps, dot-expression
  variants, bool widths, actor/zero/mask pins and loop extraction did not
  cross 80%; a reversed compare measured 79.7% but did not improve structure.
- **080B23A0 — stopped below threshold.** Retained locally at
  wip/vorn/sub_080B23A0.c, 42.1%, 174 / 45; experiments reached 45.7% fuzzy.
  Tentative alias: Vorn_BeginRecoveryRoute. Context +0x6AC gates entry;
  reset or response flags 4/2 permit it. Sets context +0x6AC and +0x6EC
  to 60, sets response flag 4 and clears 2. On reset, starts the context's
  route with its alternative selector. Calls 080B1FA8, selects behavior 15,
  restores savedTarget control, installs 080ADCC0 in word104 with kind 0x25,
  enables route/chase flags and copies the reload timer. C recovered these
  stores, but zero/flag-mask lifetimes, reset in ip, halfword-load reuse and
  allocation across the phase/control store sequence remain poor. Explicit
  offset evolution reduced structural differences, while pins and adjacent
  alternatives failed to produce a qualifying candidate. Nothing promoted.

Useful shared lessons: distinguish context-relative and actor-relative views;
express the +0x448 route as an aggregate to keep its point at +8; use group
+0x200 when the ROM reuses that offset as its route-flag mask; and take stack
position addresses before populating fields to avoid aggregate scalarization.
Positive-success layout and a u8 return in the inlined route helper preserve
materialized 0/1 checks. Judge large functions by realigned structural/total
counts: positionally differing byte counts are dominated by shifted branches
and pools. More structural similarity did not always improve fuzzy scores.

## Task 0812E7EC — 2026-10-20

Tags: #matching #scene-actors #nonmatching #register-pinning

No exact matches; exact code gain is 0.00 percentage points. Four guarded drafts
were retained on the assigned cloud/0812E7EC branch. All scores below are the
final `check.py --nonmatching --record` scores, against reviewed extents. Default
builds retain INCLUDE_ASM. No full build, network operation, or shared-doc update
was performed. The pre-existing tools/cloud_task.py modification was left alone.

- **0812FA0C — drafted 80.6%.** Tentative alias:
  SceneActorReaction_UpdateRouteReturn. On entry installs savedTarget (+0x22C),
  clears route/chase flags; after counter >60, starts RestoreAfterDelay, saves
  the group's reload time and entry control, then performs the common arrival
  decision. `Chase_Arrive` from scene_actor_chase.h supplies almost the entire
  large inline: route snapping, linked target/player-cell lookup, direct route
  rebuilding, and random turn packet setup. Retained local copies allow future
  tuning without changing consumers. Final realigned differences: 228 total,
  44 ignoring registers. A signed halfword mask at group +0x40, ordering the
  saved time load before the packet address, and retaining the callback's zero
  return at the phase-counter store improved the draft. Pinning actor/group
  worsened it. Remaining issues: PRE stack slots, random-loop constants, and
  control/phase setup allocation. Initial commit title says 80.4%, but the
  recorded full-function score is 80.6%.
- **0812E7EC — stopped below threshold.** Local candidate:
  wip/sub_0812E7EC.c, 58.8%. Tentative alias:
  SceneActorReaction_DispatchPendingCommands. Called by
  SceneActor_DispatchActivePhase, gated by flags2C0 &0x100. Reads the receiver
  at actor +0xC using scene-event double-buffer phase +0x24. Commands 0/1/2
  set movement/turn destinations; 4,8,9,10,11 select behaviors; 5/6/7 forward
  actor messages; command 3 dispatches attachment creation/update/release.
  Category 18 uses actor +0x2FC coordinates; others use route-object +0x1C.
  A typed command union avoids materializing a separate argument pointer;
  an explicit precheck plus do/while restores the loop entrance. A u8 phase
  parameter preserves distinct byte/word zeros. Still, the compiler merges
  large state/control tails that the ROM duplicates, leaving the candidate
  substantially shorter. Parameter-width variants and nested setter inlines
  did not resolve this. Nothing promoted.
- **0812F5D4 — drafted 80.8%.** Tentative alias:
  SceneActorReaction_UpdateContactRoute. Invokes callbacks +0x270 and +0x260,
  applies actor field2C2 as duration scale, consumes the route probe event,
  then requests a descriptor turn or acceleration. An enabled group contact
  can start a random turn with retry limit three, saving control/state in the
  group packet and setting group +0x206 to two. Final realigned differences:
  92 total, 35 ignoring registers. Using a u32 saved-direction argument avoids
  early byte narrowing; explicit RNG table/index/mask locals expose the loop;
  loading the existing duration through a temporary before the conditional
  store improved allocation substantially. Two scoped register hints remain
  on RNG prior direction and retry count, with the concession marker.
  Remaining issues: duration/control setup, PRE stack slots and RNG allocation.
  Correction commit restores these hints after depin.py mistakenly tested the
  guarded assembly fallback and removed them (77.4%). Running depin.py on the
  unguarded candidate correctly skips it as nonmatching. The final C itself
  was rechecked at 80.8%; depin.py needs NONMATCHING support before use on drafts.
- **0812EED8 — drafted 81.3%.** Tentative alias:
  SceneActorReaction_BeginTargetRecovery. Requires flags2CC bits 0x400000 and
  0x200000 plus motion +0xEB bit 1; installs delayed recovery, snaps the route
  position, resets response flags/timers and marks field5DA as 0xFF. Final
  realigned differences: 58 total, 22 ignoring registers. Pointer-based flag
  tests avoid combining the two tests into one, delaying the motion pointer
  assignment improves its lifetime, and reusing the flags pointer for the
  late test removes recomputation. Explicit signed masks reproduce the byte
  flag clear. Register assignment and PRE copies remain. Initial commit title
  says 80.8%; the recorded full-function score is 81.3%.
- **081301C4 — drafted 81.6%.** Tentative alias:
  SceneActorReaction_UpdateRouteRecovery. After the delay clears recovery bit
  0x100, requests category reaction 2, starts delayed restoration and probes a
  route to its saved point using the actor's current Y. The near-position test
  chooses a saved control with behavior seven or zero. Increments the counter
  after the recovery work as well as before the delay expires. Final realigned
  differences: 61 total, 24 ignoring registers. Reading the route-point pointer
  once and filling a ContactPosition through its address fixed many structural
  differences. Remaining issues: zero/flag lifetimes and route/packet register
  allocation. Initial commit title says 81.1%; final recorded score is 81.6%.
- **0812F3CC — stopped below threshold.** Local candidate:
  wip/sub_0812F3CC.c, 73.5%, realigned 49 total / 10 structural differences.
  Tentative alias: SceneActorReaction_BeginContactResponse. Numerous actor
  callers test its u8 result; it selects a contact, clears the response-control
  counters, records the selected contact at group +0x88, starts idle control
  and state (3,1,0), clears flags2CC bit 0x10, and sets flags2D6 bit 0x800.
  The failure block must precede the success block, using gotos to reproduce
  that layout. A u32-returning motion-flags inline gives the initial mask's
  operand order; a diagnostic r5 group pin reduces structural differences.
  The main obstacle is allocation and CSE retaining the zero result of the
  0x80 guard across the contact call, rather than materializing fresh zeros.
  Narrowing the *high-bit* flags getter to u16 is invalid (it erases the
  0x1000000 gate); that experimental variant was rejected immediately.
  Separate narrow/boolean low-bit getters, contact/actor/flags pins and mask
  order variants did not reach 80%. Nothing promoted.

Useful techniques: shared Chase helpers are strong structural seeds for this
range, but raw whole-listing fuzzy scores move with register allocation as well
as structural changes. Inspect realigned counts first. Inline argument widths
and whether a stack position is filled through a pointer materially change
front-end narrowing, CSE and PRE. A guarded draft must always be scored with
--nonmatching; testing its default assembly fallback proves nothing about its C.

## Task 081DEC2C — 2026-10-20

Tags: #matching #menu #animation #time #password

Worked solo on the already-checked-out cloud/081DEC2C branch, largest target
first. Used only per-file check.py acceptance checks; no full build or network.
All new matches/drafts have file-local descriptive aliases. Existing unrelated
changes in tools/cloud_task.py were left alone.

- **081EAB64 — drafted 88.5%, Params3D0_UpdateDisplay.** The creator at
  081EB7DC installs this as the display task callback. It selects seasonal
  music using month/light phase, handles the title/menu reveal and palette
  interpolation, animates option changes, and calls the configured script on
  confirmation or timeout. Ownership/name is tentative, following neighbouring
  Params3D0 names. State occupies +0x72C, selection +0x724, fade shift +0x738,
  idle timer +0x73C, state timer +0x748, palette step +0x75C, and palette shift
  +0x760. The distinct +0x734 counter increments only in two states; it is not
  the common state timer. Guarded C recorded with --nonmatching --record;
  final realigned differences: 157 total, 53 ignoring registers. Remaining
  differences include palette argument ordering, cached field addresses and
  selection-position arithmetic/narrowing. No exact credit. Condense exposed
  the repeated keyframe/palette blocks; asmat supplied the full switch tables.
- **081DFCB4 — matched, Password_UpdateEntryMenu (1,076 bytes).** Keyboard
  navigation repeats every five held frames after a sixteen-frame history;
  A appends/accepts, B erases/cancels, Start selects/accepts the end entry,
  shoulders move the text position. It joins text rows, decodes the password,
  pads the player name, fades and dispatches the configured result script.
  Capturing the loaded character before testing/storing it, and loading the
  player value before the destination pointer, reproduce original ordering.
  A do/while name loop avoids the extra entry guard produced with pinned
  induction registers. Register-pinned category; exact explicit-size check.
- **081E013C — matched, Password_InitializeEntryMenu (696 bytes).** Improved
  the existing 85.8% draft to exact. Initializes selection/repeat state,
  backgrounds, relocated resource headers and six animation instances; copies
  palettes into task-local storage and assigns their bank attributes. Retaining
  the first header pointer through its palette-member access fixes a lost
  pointer increment. Palette attribute 0xE002 must be a separate constant;
  deriving it from a live 0xE000 value changes instruction selection. Scoped
  register hints and one empty operand barrier preserve offset lifetimes.
  Register-pinned category; exact explicit-size check.
- **081DEE38 — matched, Time_InitializeDateMenu (564 bytes).** Initializes
  date menu backgrounds and animation resources, copies a 32-byte relocated
  header at +0x6B8, saves/copies a palette, builds date digits, and chooses the
  fade based on the script parameter. Assignment of the one-valued setup
  argument inside Bg_SetupLayers gives the original stack-store ordering.
  Cache the palette source before storing its saved pointer to avoid an extra
  reload through potentially aliasing raw pointers. Register-pinned category;
  exact explicit-size check.
- **081EC7D8 — matched, ActorDisplay_UpdateForegroundSprite (412 bytes).**
  Tentative foreground role follows ActorDisplay_RefreshScreenLayer's call and
  coordinate placement. Sets the drawable at +0x2C0 from screen/origin offsets,
  conditionally hides it, then advances packed-command animation unless its
  mode is one. The sequence begins at drawable +0x2C; +0x3C is an unknown word,
  animation-ended is +0x40, and mode is +0x44. Reused the known packed-command
  playback idiom from route_sprite_playback.h locally without changing shared
  types or consumers. Explicit coordinate temporaries and scoped pointer
  hints reproduce the address/load ordering. Register-pinned category; exact
  explicit-size check.
- **081DEC2C — matched, Time_UpdateDateMenu (456 bytes).** Saves prior date
  fields, wraps the selected field, delegates edits, applies date/time on
  Start, recalculates sunrise/sunset, then fades and dispatches the exit script.
  Typed consecutive s32 fields reproduce all address-increment patterns.
  Explicit empty case 2 is necessary for the original switch decision tree;
  the two visible nonempty cases alone compile differently. No register
  pinning. Exact explicit-size check.

Each accepted function/draft was committed separately through build/commit.sh.
Combined ROM validation is deferred to the integrator as instructed.

## Task 081F8CBC (2026-10-20)

Tags: #matching #ratatosk #animation #script

Five exact matches, checked individually with reviewed target sizes; no full build,
network operations, or register-pinning concessions. Commits were queued through
`build/commit.sh`. The pre-existing change to `tools/cloud_task.py` was left alone.

- **081F8CBC matched** — `Ratatosk_UpdateRetreatSequence` (tentative behavioral
  name). On activation, tests player positions offset by +/-512 on X and Z,
  keeps the farthest from its collider position, then advances animation states
  and selects a point farthest from the player. Explicit stack-vector pointers
  prevent agbcc scalar replacement from turning the vector into packed register
  operations. A duration pointer declared before the zero local reproduces the
  reset-store ordering. Reviewed extent: 488 bytes.
- **081F8EA4 matched** — `Ratatosk_UpdateRepeatedRetreat` (tentative behavioral
  name). Similar offset search after two completion notifications; a six-stage
  animation sequence eventually chooses a random movement point. Declaring the
  candidate pointer before the self-position pointer fixed the last two loop
  setup instruction reorderings. Reviewed extent: 672 bytes.
- **081F9F10 matched** — `Ratatosk_ApplyDamageReaction`. Damage callback selected
  by signed kind 0/1/2; kind 0 displays positive damage, and the cases install
  directional reaction/recovery callbacks. Packed signed 16-bit vector fields
  reproduce the damage-display size setup. Be careful that agbcc rounds small
  byte-only structs to four-byte sizes. Use the existing named symbols
  `Ratatosk_BeginDirectionalReaction` and `Ratatosk_UpdateDirectionalRecovery`:
  obsolete sub-address declarations resolved without their Thumb bit in this
  checkout's checker stubs. Explicit direction/destination/zero locals reproduce
  the final kind-1 store ordering. Reviewed extent: 352 bytes.
- **081FA418 matched** — `Ratatosk_UpdateSunOrbPlayerTransfer` (tentative).
  Checks sun-orb slot state, active target proximity, selected target and player
  state before starting a two-stage transfer with 90-frame timers. The target
  distance table has 24-byte entries at +0xE00. A typed array produces the proper
  base-before-index address calculation; a flag-mask inline puts the mask before
  the load. A separate local for the second target search fixes register reuse.
  Reviewed extent: 440 bytes.
- **081FA718 matched** — `Ratatosk_UpdateCollidersAndPalette`. Updates five
  colliders, actor/camera/shadow visibility, flash timers, palette transitions,
  and per-frame counters/force accumulators. Exact sprite flag access comes from
  small inlines taking the animation object at +0x34C, with flags at +8; camera
  flags similarly belong to the object at +0x3DC, +0x20. Duplicating the palette
  oscillation tail in cases 3 and 4 lets cross-jumping reproduce the shared tail;
  an explicit goto produced different control flow. A signed full-width 16-bit
  bitfield for the +0x18E flag insertion fixed the final operand-register tie.
  Reviewed extent: 1532 bytes.
- **081FAE0C stopped, not drafted** — `Ratatosk_InitializeFromScript` candidate
  remains only in ignored `wip/sub_081FAE0C.c`. Best latest check: fuzzy 66.2%,
  realigned 392 differing instructions / 122 ignoring registers, artifact
  `build/check/run-q_g_0_tt`. Below the 80% retention threshold. Reads script
  options e/l/p/r/m/A/c/B/S/s/R/G/b/D/d/X/t/i, initializes actor slot 4, movement
  points, collider parameters, three palette banks and child pools. Explicit
  actor-member pointer locals before the first option loop reproduce the early
  address setup and their declaration order controls spilled pointer slots.
  Remaining differences include constant/address reuse in default option stores,
  shared branch tails, loop-invariant palette height, source/destination register
  choices in palette copying, and generic actor setup. A register-pinning trial
  made the initial candidate worse and was discarded. The first p-option store
  should access position.x directly; introducing its pointer only for y/z avoids
  keeping that pointer live across all preceding calls. Original next-state is
  zero in both mode-dependent initialization paths, even when current state is 2.

The integrator still needs the normal combined full-ROM validation for this batch.

## Task 0820302C — 2026-10-20

Tags: #matching #animation #script #ooze #cannon

Two exact matches and four guarded drafts. Used only fast per-file checks; no
full build or network operations. Separate function commits went through the
commit driver. The pre-existing `tools/cloud_task.py` change remains untouched.

- **0820302C matched — ProximitySequence_EmitBurst.** Emits alternating-speed
  sprite effects every ten countdown ticks below 90, plays a variant-dependent
  sound at tick 80, and returns to proximity testing at expiry. The neighbouring
  `ProximitySequence_TestPlayerDistance` installs this callback with timer 180.
  Position is copied as two words and raised on Y; effect velocity uses signed
  sixteen-bit bitfields. Fetch the first random value, then compute one shared
  `base = spread - 8`, then use that base for both X and Z: duplicating the base
  inside expressions/inline helpers makes agbcc narrow the subtraction, introduce
  a literal, and allocate different registers. Small copy/raise inlines prevent
  packed stack-vector scalar replacement. Exact check after formatting: 468 bytes.
  No register-pinning concession required.
- **08203204 matched — CannonEmitter_UpdateAnimation.** Pause-gated callback
  dispatch, sprite visibility-bit clearing, temporary palette assignment while
  +0xF2 counts down, then packed sequence playback. Preserves the callback's
  return value in r0; treating it as a void callback changes the entire stepper's
  register assignment. The same object shape is constructed at 0820337C and used
  by 0820302C: sprite +0x20, flash palette +0xB0, callback +0xD0, sequence +0xD4.
  A shared `paused:` return reproduces the original branch layout while allowing
  the flip-mask constant to be initialized after the first pause test. Explicit
  flag-result and read/store pointer locals reproduce the second flip update.
  Register-pinning concession; `depin.py --write` reduced 29 hints to 21. Exact
  check after formatting: 364 bytes.
- **0820337C drafted 87.6% — CannonEmitter_InitializeFromScript.** Reads script
  options p/r/a/t/s/S, squares the trigger range (default 0x40000), loads sprite
  resource 0x7E7D, selects palette `variant + 0x1BA`, fills sixteen flash colours
  with 0x7FFF, starts sequence 6 and optionally registers a scene body. Installs
  `ProximitySequence_TestPlayerDistance`. Reused the recovered animation stepper
  and explicitly preserved the `Anim_SetSequence` / terrain-queue return value.
  Remaining: saved member-pointer lifetimes, palette-loop branch layout and
  animation allocation (43 realigned instructions, 16 ignoring registers).
- **08205B70 drafted 89.1% — Ooze_UpdateAttack (existing Ooze_BeginAttack).**
  Three animation phases: wait until +0x11E exceeds +0x13E; start the attack and
  play sound 0x523; repeat/hold until +0x13C reaches +0x13A; finish and restore
  `Ooze_UpdateAttackDelay`, resetting phase/timer/repeat counters. Collider +0x68
  is refreshed during phase 1. The stepper shares long runs with
  `ContactSequenceActor_UpdateAppearing` and the roaming-object playback helper.
  Passing sprite and sequence separately fixes their address-materialization
  order. Remaining: 77 realigned instructions, 17 ignoring registers, mainly
  transition-field address reuse and allocation. The draft's address alias also
  maps back to existing exported `Ooze_BeginAttack`, allowing `--record` to find
  the named assembly fallback; the initial zero score was corrected in a followup.
- **08206098 drafted 85.0% — Ooze_InitializeFromScript.** Reads p/a/i/t/r/w/h/S;
  defaults collider extents to 30/10/30, interval 60, attack delay 30 and repeat
  limit 3. The h option maps pairs of headings to two facings. Initializes sprite,
  variant-dependent sequence IDs, attack collider, optional delay list and optional
  scene body. For variant 1 the body's origin shifts +128 along X or Z according
  to facing. Installs `Ooze_UpdateAttackDelay`. Explicit return preserves the
  observed +0x14E member address on the no-body path and terrain-queue result on
  the body path, rather than falling off a non-void function. Remaining: option
  branch shape, animation allocation and body-setup pointer lifetimes (103
  realigned instructions, 47 ignoring registers).
- **08206C00 drafted 83.6% — VulcanCannon_UpdateTargeting.** Compares player
  distance with squared range; optional delay-table/fixed timer starts turning.
  Facing 0 tests a narrow Z band with player to the right; the alternate facing
  tests a narrow X band with player in front along Z. A clear shot resets attack
  counters and installs 08207010. At aim timer 119, atan2 can select turning
  callback 08207350. Final animation playback belongs to sprite +0x6C and sequence
  +0x10C. A stack-address lifetime hint retains the copied player position in
  memory; repeated member expressions avoid keeping a position pointer in a high
  register throughout the function. Remaining: facing-change branch sharing and
  address/allocation differences (103 realigned instructions, 20 structural).

All four drafts compile, retain INCLUDE_ASM fallbacks, carry register-pinning
markers, and have scores recorded with `check.py --nonmatching --record` against
reviewed function extents. Declaration-swap trials did not improve them;
`depin.py` skips non-exact candidates. No handwritten instruction bodies.
Combined full-ROM validation remains for the integrator.

## Task 081AA788 (2026-10-20)

Tags: #matching #vanargand #boss #paired-scene-actor

- **081ABA64 — stopped unmatched.** Read BIG_FUNCTIONS.md, assembly, all of the condensed listing, existing notes and relatives first. Eleven phases combine animation playback, action-sequence effects, player synthetic contact and track motion. The largest matched relative explains 37%, mostly repeated packed-command playback; no close full-function port emerged and no complete candidate was produced. Phase 0 is a debug-input path: nudge position, require held bit 0x100, save input bit 1 at animation+0xA4, then enter phase 2. Normal entry instead selects phase 1, limits target lateral distance to 512, chooses angle 0x60/0xE0 and creates a track around (896,768,896). Phases 6/7 repeatedly apply synthetic player contact; phase 7 advances the action effect at tick 60. Interior labels 081AC706 and 081AC91A are already recorded nonentries, and calls to them / 081AC942 are local far branches. A future full candidate needs the six shared playback expansions and the action-sequence state contract, rather than treating these labels as callees.
- **081AA788 — stopped below draft threshold.** Authored a full tentative Vanargand_UpdateSweepingProjectileAttack candidate, retained only in ignored wip/sub_081AA788.c. Initial check: 70.8%, 261 realigned differences / 90 structural. A typed auxiliary-part view, corrected part stride and register-pin trial did not improve the result (latest unpinned candidate 70.3%, 265 / 87). Seven phases initialize both halves, animate the auxiliary part, alternate backward/forward tracks and create randomized directed/free projectiles. Part stride is **0x21C**, not 0x43C: the assembly's multiply is ((17*i)*8-i)*4. Counter is pair+index*0x21C+0x20C; selected collider is pair+index*0x21C+0xB4. Shared projectile helpers leave different high-register lifetimes and stack spills, and shared phase-reset tails differ. No production draft committed.
- **081AB078 — matched as Vanargand_UpdatePaletteLeap.** Improved the existing guarded 93.0% draft to an exact 868-byte match (100% of this function). Frame in sl and animation in sb require marked register pinning; a distinct transition-owner value preserves r8. Materializing the second palette address from the documented fixed RAM address, independently from the first symbol reference, prevents GCSE from retaining the first palette pointer across all six interpolation calls. Scoped colour locals plus input constraints on packed colour r0 / palette r1 reproduce the final stores. Argument/owner setup order matters: preserve frame and owner before computing animation. Guard removed after exact verification.
- **081ACC24 — drafted 99.1% as Vanargand_ApplyPendingDamage.** Reconstructed ordinary damage, accumulated threshold tests and paired-part contact damage; lethal ordinary damage sets flag 2, threshold exhaustion with no positive pair counters sets flag 1, otherwise strong part contact installs palette leap mode 20 and flag 0x10000. Shares the state-3 palette restoration with 081AD0D0. Important correction: ordinary damage-number position is actor+0x1AC, not +0x2AC. A u32-returning accessor for the part halfword flags prevents narrowing and lets the constant-one mask be shared. A value barrier on the early accumulated-damage pointer prevents PRE from retaining it across calls; a typed threshold array fixes address order. Final reviewed-extent check: 3 realigned differences, all structural, from a long conditional branch and its padding; candidate 1200 bytes versus ROM extent 1196. Register pins are marked. INCLUDE_ASM fallback uses the raw address name because the macro stringifies its argument; a corrective commit fixes the initial descriptive fallback spelling. Default fallback checked exact.
- **081AD0D0 — matched as Vanargand_ApplyRequestedMode.** Byte AA values 1..32 set paired motion parameters 2/4 and install callbacks from gUnk_08612020. Leaving previous state 3 restores the variant palette, interpolates highlight colours and stops the action sequence. Exact reviewed-extent check: zero differing bytes, 752 bytes (100% of this function); no fixed-register pins. A separate owner value prevents merging it into the low-register installation pointer. Materialize the zero before state/callback stores; separate scoped colour temporaries and a packed-colour input constraint give exact palette pointer allocation. These fixes also improved the shared palette region in 081ACC24.
- **081AFD10 — drafted 85.8% as PairedSceneActor_UpdateHalfPresentation.** Adds origin+offset+256Y, applies random radial shake, updates body/attack colliders, restores flashed palette, toggles shadow and clears completed pair bits before returning the half to state 2. Layout: position 0x1C, origin pointer 0x90, offset 0x9C, colliders 0xB4/0x108, mode 0x15C, flags 0x160, state/previousState 0x16C/0x16D, flash 0x184, shake 0x19C. agbcc rounds the three-halfword vector struct to eight bytes, so explicit padding is needed when embedding it. Mask-taking flag accessors avoid a duplicate retained flags pointer (which otherwise spills the owner into r8); full-width signed collider bitfields preserve negative masks. Marked register pins steer RNG and vector temporaries. Final reviewed-extent check: 46 realigned differences / 5 structural, candidate 644 bytes versus ROM extent 648; remaining vector operand order, reset-constant reuse, offsets and register allocation. Source's recorded 85.8% supersedes the preliminary 85.5% commit title. Default fallback checked exact.

Two exact matches and two guarded drafts retained; both larger phase machines stopped as described. Each source was committed through build/commit.sh, with this notes update last. Exact sources were formatted and checked with explicit reviewed extents; draft scores were recorded with tools/check.py --nonmatching --record and fallbacks checked. No full builds, network operations, generated assembly, ROM or emulator outputs committed. Combined ROM validation and project percentage / percentage-point accounting remain for batch integration.

## Task 081ABA64 (2026-10-20)

Tags: #matching #vanargand #paired-scene-actor

- **081ABA64 — stopped unmatched.** Inspected the largest target first with
  asmat, condense, relatives and the prior task notes after reading
  BIG_FUNCTIONS.md. Existing evidence still points to an eleven-phase attack
  machine containing six packed-animation playback expansions, tracks and
  synthetic player contact. Matched relatives cover only about 37% of its
  instructions, chiefly playback. No sufficiently close full-function port or
  complete candidate emerged. The local far-branch labels must stay inside the
  parent function; they are not external callees. Left the assembly fallback.
- **081ACC24 — matched as Vanargand_ApplyPendingDamage.** Raised the existing
  99.1% draft to 100% exact (1196 bytes). The sole cause of its long branch and
  extra pool alignment was a redundant empty-asm input constraint on the
  constant-one mask immediately before setFlags. Removing that constraint
  preserves the same allocation while allowing the short branch. Removed the
  NONMATCHING guard and rechecked the entire previously reviewed extent before
  committing. The unit retains its register-pinning concession marker.
- **081B03C0 — drafted 86.2% as PairedSceneActor_UpdatePlayerCapture.** Entry
  selects animation 12 and starts phase 0. A track moves the half offset to
  (384,0,384); player requests use the displayed position minus 128 Y and
  side-specific request flags. Player flag 0x40000 aborts capture into phase 3.
  Track completion enters phase 1; after timer 120 it selects animation 9,
  enters phase 2, applies a position request and transitions to phase 3, which
  sets the completion byte. Phase and timer are +0x164/+0x168, with the entry
  byte at +0x16F. Reused the playback shape from Dog_ResetActionState. Vector
  stores through an inline s16 pointer prevent agbcc from packing local
  halfwords into word read/modify/writes; an inline u8 predicate reproduces the
  materialized track-completion boolean. Shared abort code and owner pinning
  improved the draft from 70.2%. Remaining: animation allocation, call argument
  setup, retained phase-entry pointer and transition tails (112 realigned
  differences, 55 ignoring registers). Guarded fallback checked exact. A small
  follow-up source commit formats the draft and uses MotionTrack_Update's
  canonical callee name; the recorded score is unchanged.
- **081AFD10 — improved draft to 91.8%, PairedSceneActor_UpdateHalfPresentation.**
  Swapping the source order of the origin/offset additions matches the ROM's
  load order (agbcc otherwise commutes them). A variants batch identified a
  removable direction-register pin and a separately materialized +0x168 reset
  offset; explicit +0x19C shake and +0x20C shadow-control offsets improved
  remaining temporary choices. Final candidate has the complete 648-byte
  extent, with 27 realigned differences / 2 ignoring registers. Remaining
  differences are mostly scratch-register choices for member offsets, plus
  mask/address setup order. Declaration swapping did not help further.
  rasolve could not directly compile the guarded alias-named function, so no
  solver result was adopted. Score recorded, fallback checked exact.
- **081B0018 — matched as PairedSceneActor_InitializeHalf.** Complete 100% exact
  match (388 bytes), with marked register pinning. Initializes sprite 0x89CA,
  side/mode and origin, offsets and extents, attack/body colliders, target
  registration and scaled shadow 0x1C1B; clears track/state/flash/shake fields.
  Typed zero temporaries distinguish byte-zero r6 from word-zero r4; scale
  1024 and active value 1 stay in sb/r8. Scoped member pointers reproduce
  address-before-constant setup and let 300 reuse offset 0x176 minus 0x4A.
  A source-pointer constraint reproduces the final CpuSet argument order.
  Use the canonical ActorStatus_BeginTimedState callback symbol: an undeclared
  raw-address alias resolves as an even address in the checker stub, whereas
  the canonical Thumb callback has bit 0 set. The collider callees are
  ActorStatus_InitializeCollider and ActorStatus_InitializeBodyCollider.
- **081B0248 — stopped below draft threshold.** Wrote several complete tentative
  PairedSceneActor_SpawnRadialEffects candidates. Four emissions use angle
  48+8*i for mode zero, otherwise 255-8*i, radius 640 about the origin plus
  256 Y, randomized angle increment 0..31 and speed 12..19, then call
  VanargandEffect_Create with side 1-mode. The RNG samples are shifted by 3
  before their low-bit remainder calculations. Best trial reached only 63.1%
  (91 realigned differences / 27 structural); register-pinning trials exposed
  stack-vector placement, inline polar allocation and RNG load/store ordering
  differences rather than producing a draft-quality result. Latest candidate
  remains only in ignored wip/sub_081B0248.c; no production C file committed.

Two new exact matches; one new guarded draft and one improved guarded draft.
All retained sources use descriptive file-local aliases. Both exact matches
were checked again, and both guarded sources' default assembly paths checked
exact. Draft scores were recorded with check.py --nonmatching --record against
reviewed extents. All commits use build/commit.sh. No full builds, network
operations, ROM, generated assembly or emulator dumps. Project percentage and
percentage-point accounting remain for batch integration.

## Task 080DE128 — 2026-10-20

Tags: #matching #dog #register-allocation

Worked solo on the preclaimed cloud/080DE128 branch; no network or full build.
Three of six assigned entries (50%) pass reviewed-extent exact checks; one is
retained as a 99.4% guarded draft and two remain below the draft threshold.
The three exact C bodies already existed in the starting checkout under
promoted descriptive names: these commits repair their missing local aliases
and reduce unnecessary hints, rather than claiming newly recovered body bytes.
Project percentage and percentage-point accounting belong to batch integration.

- **080DE128 — matched, Dog_SelectTargetAction.** Added the local alias required
  by the worker checkout's ELF, then checked all 772 body/pool bytes through
  080DE42C. The subsequent byte block is retained trailing assembly, not part
  of the accepted C body. Chooses target-range/heading responses 0x75, 0x82 or
  0x87, otherwise invokes the movement selection. depin.py reduced 14 hints
  to four; the register-pinning concession remains marked. Exact after format.
- **080DE57C — matched, Dog_ResetActionState.** Added local alias and validated
  the complete 336-byte extent ending at 080DE6CC. Clears context action gates,
  timers and slots, resolves movement and steps the drawable sequence. depin.py
  reduced 14 hints to 13. Exact after format; concession remains marked.
- **080DEA7C — drafted 99.4%, Dog_UpdateCollisionBounds.** Retained the existing
  guarded C with its required alias and recorded the score. Remaining difference:
  the availability test separately loads offset 0x2D0 instead of deriving it by
  subtracting four from 0x2D4 (three realigned differences including alignment).
  A constrained offset removed the structural difference but introduced nine
  to eleven register differences elsewhere; further signed-load and offset pins
  did not improve it. Those experiments were discarded. A follow-up fixes the
  fallback to spell sub_080DEA7C literally: INCLUDE_ASM stringifies its argument
  without expanding the alias. Default assembly path is exact; draft score
  recorded with --nonmatching --record against its 680-byte extent.
- **080E059C — matched, Dog_ResumeAfterReaction.** Added local alias; complete
  436-byte extent ending at 080E0750 matches after format. Waits for animation
  completion, then resumes chase control with route flags or restores idle
  movement. Both existing register hints remain necessary under depin.py.
- **080E0750 — stopped below threshold.** Tentative name
  Dog_ResumeRouteAfterKnockback. Read BIG_FUNCTIONS.md, the condensed listing,
  callers and matched relatives. Reconstructed the entry's velocity quartering,
  bounded offset smoothing (+/-200), animation-completion gates, chase versus
  route restoration, and repeated terrain/route/turn logic from scene_actor_chase.h.
  First candidate: 74.5%, 655 realigned differences / 234 ignoring registers.
  Moving the invalid-target callback to a common tail and retaining smoothing
  field pointers did not help: 73.8%, 656 / 238, 4184 candidate bytes. Current
  valid candidate remains in ignored wip/sub_080E0750.c; no production draft.
  Ghidra export is absent. Most remaining differences concern inlined control
  tails, RNG retry allocation, stack slots and shared callback placement.
- **080DE6E0 — stopped below threshold.** Tentative name Dog_EmitFootstepEffect.
  Frames 1, 5 and 7 at elapsed zero emit palette-mode 1/3 effects behind the
  actor, with two randomized X/Z offsets; returns a chase-dependent frame-table
  value from gUnk_086061E4. Effect mode 3 uses randomized lifetime 20..83 and
  mode 1 uses the fixed alternate effect parameters. Typed inline vector Add
  and Offset helpers improved the initial 42.2% trial to 56.0% against the
  reviewed 588-byte target (161 realigned / 57 structural; 616 candidate bytes).
  Some pinning experiments scored up to 76.1%, but inspection showed incorrect
  stack-address rematerialization around explicitly pinned inline pointers,
  plus a missing r7 save in another trial. Rejected those candidates rather
  than treating their scores as progress. Retained the unpinned, defined-value
  candidate in ignored wip/sub_080DE6E0.c. It explicitly initializes the Y
  component before the vector addition; the assembly's transient Y is overwritten
  before the effect call, so reproducing that otherwise dead stack read is not
  an acceptable way to improve a candidate. Remaining issues are register
  lifetimes, initial flag address sharing, vector addressing and temporary layout.

All commits were queued through build/commit.sh. No ROM, generated assembly,
emulator dumps, shared docs or symbol tables were changed. Only check.py was
used for acceptance; the integrator still needs the combined full-ROM build.

## Task 081B1E24 — Vanargand part callbacks (2026-10-20)

Tags: #matching #vanargand #animation #register-allocation

Worked solo on the preclaimed `cloud/081B1E24` branch. Exact code gain: 0.0
percentage points. All six functions are guarded drafts, with assembly retained
as the default path; 081B2608 was an existing 83.8% draft in the starting tree.
No full build or network operations were run. Each draft was formatted, checked
with `tools/check.py --nonmatching --record` against its full function extent,
and committed separately through `build/commit.sh`.

- **081B3410 — drafted 94.0%, Vanargand_UpdatePartSweepingStrike.** Seven-state
  callback: aim/clamp the player-facing angle, approach with distance-wave motion,
  wind back, prepare the player-relative target, strike, halve residual velocity,
  and return to the saved heading/distance. Remaining realigned differences:
  58 total / 28 ignoring registers. Most concern the packed animation setup,
  high-register enter pointer lifetimes, literal pools and copied counters.
- **081B28D8 — drafted 93.1%, Vanargand_UpdatePartGroundStrike.** Related
  seven-state attack using ordinary motion tracks rather than the distance-wave
  helper. The impact plays song 0x1BA; the linger phase emits bursts every 16
  ticks through tick 44, sets flag 0x80, and exits after tick 89. Remaining:
  66 total / 27 structural, chiefly animation register lifetimes, enter pointers,
  saved counter addressing and pool placement.
- **081B1F74 — drafted 92.5%, Vanargand_UpdatePartPairedStrike.** Reset computes
  player-relative targets with +/-112 offsets when both counters are positive,
  otherwise +/-256. Completion consults the opposite part's byte at boss+0x205
  or boss+0x421 before selecting the alternate motion/burst path. Cases 3–5 are
  retained even though the visible completion paths select state 6 directly.
  Remaining: 65 total / 9 structural. Mostly register allocation, enter-pointer
  copies, zero reuse and literal padding. This unit has no register pins.
- **081B2F38 — drafted 89.0%, Vanargand_UpdatePartOffsetStrike.** Five-state
  callback: wind away from the player, prepare a player target offset by +128
  on X or Z according to the variant, strike with trails, and return to rest.
  Remaining: 76 total / 38 structural. Stack frame/spills, retained motion/one
  registers, animation setup and enter-pointer allocation remain different.
- **081B2608 — improved draft to 88.0%, Vanargand_UpdatePartRetreat.** Preserved
  both identical-looking variant branches in the retreat-vector calculation.
  Typed inline state/flag writes and removal of the enter register pin improved
  the baseline. Added the required local alias so the current worker ELF can
  locate the address, and spelled sub_081B2608 literally in INCLUDE_ASM because
  that macro stringifies its argument. Remaining: 48 total / 11 structural,
  mostly saved-boss/enter allocation, zero and constant reuse, animation inlines.
- **081B1E24 — drafted 90.7%, Vanargand_UpdatePartTracking.** Retargets every
  16 ticks with opposite +/-512 X/Z offsets according to variant, ends after
  64 ticks, then marks completion. Remaining: 18 total / 13 structural, localized
  to constant-load/copy order and shared-tail placement in the offset addition.
  Scalar widths, inline scalar additions, unsigned parameters and constrained
  constants did not improve this residue. This unit has no register pins.

Names are tentative static interpretations, following the neighbouring
Vanargand family. No runtime coverage claims were made. Common observed layout:
sequence at +0x48, motion at +0x58, motion's base-position pointer at +0x90,
origin at +0x9C, target at +0xA4, heading/distance at +0xAC/+0xAE, variant at
+0x15C, flags/state/timer at +0x160/+0x164/+0x168, and reset/enter/finished bytes
at +0x16E/+0x16F/+0x170. Unknown fields remain explicit and local to each unit.

Useful techniques: BIG_FUNCTIONS.md's realigned counts and small hypothesis
batches with variants.py; typed base pointers fixed vector argument ordering;
address-taking inline vector operations prevented agbcc from turning direct
stack halfword updates into word read/modify/write sequences; explicitly
preloading the enter byte into r1 before deriving its pointer greatly improved
the three larger attack callbacks. Separate inline state/flag writes restored
constant-before-address order. depin.py --write was run on the hinted units;
it skips nonmatching candidates, so no claim is made that retained draft hints
are minimal. Ghidra export was unavailable. Further work should start from these
recorded candidates and focus on the remaining allocation/setup differences.

## Task 08208498 (2026-10-20)

Tags: #matching #cannon-turret #fan-cannon #codex-medium

Worked solo on the already checked-out cloud/08208498 branch. No network or
full builds. Exact checks used build/venv/bin/python tools/check.py; commits
went through build/commit.sh. New exact gain: **+0.0225 percentage points** of
code (544 bytes), in the Register-pinned category because its one empty
register barrier is necessary. No total progress report was regenerated.

- **08208498 matched**, CannonTurret_FireAimedVolley. Fires every ten ticks
  while the countdown exceeds 45, aims toward the effective player position,
  adds random angular spread, selects speed between half and full configured
  speed, and alternates effect variants on the twenty-tick cadence. Returns
  to sub_0820843C after the countdown. Static evidence: neighbouring
  CannonTurret_InitCollider/Create, the update callback comparison at
  08208740, Math_Atan2 and SpriteEffectController_Create calls. Exact check:
  544 bytes, fuzzy 100%, after formatting and depin.py. The sole retained
  concession is an empty barrier on the random-table pointer; no fixed
  register variables remain. An inline CopyPosition with separately assigned
  destination/source pointers restored argument order. RandomFrom(table)
  with a preloaded pointer restored the RNG register lifetime. Calling Polar
  with &velocity and assigning the caller's velocity pointer afterwards
  removed the final three differing instructions. A low r7 table pin was
  rejected: agbcc reused r7 and turned table accesses into stack accesses.
- **08208740 drafted 81.7%**, CannonTurret_UpdateCombat. New complete candidate
  handles pause flags, callback dispatch, collider visibility, damage/HUD,
  defeat debris and animation, periodic animation selection, palette flash,
  blink and packed sequence playback. Last full-size nonmatching check:
  119 realigned differing instructions, 38 ignoring registers. Remaining
  differences include pause layout, collider/vector setup and allocation.
  The callback's incidental ROM result is replaced with defined zero; a
  defined r0 allocation token keeps playback from freely using r0. Uses a
  local Subsystem_VerbNoun alias and the required concession marker.
- **08208BE4 drafted 93.3%**, CannonTurret_InitializeFromScript. New complete
  candidate parses p/r/a/l/s/S/z/g/D/G/X/V, loads graphics resource 7E7D,
  uses palette 1BC, looks up animation resource (922E,5184), starts live/dead
  animation, steps packed playback, and initializes collider/terrain overlay.
  Last full-size check: 37 realigned differing instructions, 6 ignoring
  registers. Remaining differences: resource-slot reload and register
  lifetimes, plus an explicit zero success result rather than incidental
  r0. Keeping Anim_SetSequence's defined return live in r0 through playback
  reduced the register differences substantially (72.0% to 93.3%). Health
  option writes must remain in ROM order: +136 then +132; test the narrowed
  local value to avoid an extra reload. Position +1C, sprite +24, display
  position +40, graphics +50, sequence +10C, resource pointer +164.
- **08209254 retained draft 80.7%**, FanCannon_ApplyHitReaction. Added the
  required file-local descriptive alias and re-recorded its unchanged score.
  Reviewed source/callees/assembly and tested removal of HUD/health pins and
  an explicit critical-hit mask lifetime. All worsened the existing draft
  (about 70.8%); restored its safe r10 health binding. Final check: 37
  realigned differing instructions, 7 ignoring registers. Stack-address
  selection and allocation remain. Do not restore the earlier unsafe r7
  health binding mentioned in notes/codex-low.md.
- **08209704 stopped below draft threshold**. Built a complete exploratory
  FanCannon_RampPowerAndEmitParticles candidate under wip/sub_08209704.c.
  Reviewed the full body and neighbouring inactive/toggle routines. Emits
  every 30 ticks, chooses axis/direction, computes signed sine products with
  magnitude truncation toward zero, samples a distance interval, randomizes
  graphics/flip flags, and increments power capped at +10C. Power at +120,
  distance +108, signed axis +11A and direction +11B. Candidate experiments
  stayed around 50–52%; random-table/address lifetimes, stack materialization,
  duplicated direction paths and allocation remain too far away. No src
  draft committed. Ghidra export gen/ghidra/decomp.c was unavailable.
- **08209AC4 stopped below draft threshold**. Read full asm and BIG_FUNCTIONS,
  ran condense and relatives, then built a complete exploratory
  FanCannon_UpdateCombatAndForce candidate under wip/sub_08209AC4.c. It covers
  scheduled toggling, callback dispatch, damage/defeat effects and terrain
  overrides, aligned-player range/path tests, global force outputs, palette
  flash/blink, sequence choice and speed scaling, and packed playback.
  Reviewed extent: 2052 bytes through 0820A2C8. Safe unpinned main-pointer
  candidate: fuzzy 58.7%, realigned 444 differing instructions / 169 ignoring
  registers. A diagnostic r7 pointer pin scored 63.8% but was not retained
  because of low-register lifetime concerns. Largest issues: main pointer
  selected into sl instead of r7, field-address PRE, vector/debris setup,
  and stack/register lifetimes. Source remains outside src; no draft credit.

Tool caveat: depin.py on a guarded draft can check the default INCLUDE_ASM
and incorrectly remove every NONMATCHING hint without checking its C. This
happened for the new 08208740/08208BE4 drafts; restored their committed sources
immediately using git show. Only use the tool on an extracted candidate for
such drafts, or teach it --nonmatching before applying it. The exact
08208498 depin run was on unguarded C and is valid.

## 2026-10-20 — cloud task 082154E4

Tags: #matching #background #palette #sprite-oam #nonmatching

Worked solo and offline on the preclaimed cloud/082154E4 branch. Exact gain:
0.000 percentage points (0% of this assignment matched). Reused this checkout's
previous family notes and candidates. All retained changes are guarded drafts;
the assembly remains the default. Used per-file check.py only, including the
checks beneath variants.py; no make, full build, network or direct git mutations.
Each changed unit was queued separately through build/commit.sh.

- **08216584 — stopped below draft threshold, BgGfx_LoadTilemap.** Rechecked
  the complete ignored candidate and read the condensed listing and big-function
  guidance. Baseline remains 40.9%, 951 realigned differences / 265 ignoring
  registers. The previously documented bounded/interior/wrapping paths and
  metatile quadrant expansion remain the useful reconstruction. No qualifying
  source was added. Ghidra export is still unavailable.
- **08217430 — stopped below draft threshold, Palette_ApplyObjectFade.**
  Baseline was 68.2%, 259 realigned differences / 102 ignoring registers.
  Tested scoped loop variables, per-path source/destination/counter pins,
  explicit full-strength copy arguments, declaration order and predicate
  materialization. Explicit copy arguments reduced structural differences to
  100 but scored only 67.4%; scoped pins and unpinned scoped variables regressed.
  No qualifying source was added. Mask/predicate code is substantially recovered,
  but distinct bank-loop allocation remains unresolved.
- **08217F4C — improved existing draft from 88.9% to 91.0%,
  SpriteOam_QueueProjectedSprites.** Final extent 776 bytes; 37 realigned
  differences / 16 ignoring registers. A local old matrix index pinned to r0,
  copied to the result before increment/store, reproduces the affine allocator's
  load/copy/increment sequence. Removing projection pins and changing the inline
  to accept the drawable did not improve structure. Projection-pointer caching,
  the scale comparisons' signed branch selection, and scheduling remain.
- **08218254 — improved existing draft from 86.7% to 91.0%,
  SpriteOam_QueueCameraRelativeSprites.** Final extent 692 bytes; 34 realigned
  differences / 12 ignoring registers. Retaining drawable flags from the initial
  skip test and applying the same matrix-counter update improved the candidate.
  Camera/offset scheduling, signed branch selection and allocation remain.
- **08217960 — retained existing 80.1% draft,
  Palette_ApplyBackgroundFade.** Rechecked 604-byte extent, 82 realigned
  differences / 39 ignoring registers. Added the required local alias and made
  the fallback spell the raw assembly symbol. Packed-color constant hoisting,
  loop-counter setup and allocation remain unchanged.
- **082154E4 — new 82.6% draft, SpriteOam_FinalizeQueue.** Final extent
  436 bytes; 41 realigned differences / 12 ignoring registers. The graphics
  update task calls it after render callbacks and before palette updates. It
  links queued sprites into 256 depth buckets through the low byte of OAM's
  fourth halfword, computes hardware-priority output offsets, emits descending
  depth buckets within the 128-entry limit, and disables unused OAM entries.
  It then writes up to 32 affine matrices from the queued angle and signed
  scales, using the sine table at 086149C4 and Div, and marks OAM dirty.
  A stack struct containing the 256-byte bucket array and fill word, followed by
  a separate priority array, reproduces the observed stack layout. Reusing the
  tested zero scale as the zero result avoids redundant movs. Scoped first-loop
  and affine-loop pins improved allocation. Keeping a separate priority sum
  from the emitted-entry count and retaining a shared OAM base improved the
  structure. Further count/base/index pin changes regressed; invariant-address
  hoisting and priority-loop allocation remain. Register-pinning concession.

All four changed units have file-local Subsystem_VerbNoun aliases. The fallback
INCLUDE_ASM uses sub_XXXXXXXX literally: the checker parses that spelling before
preprocessing when recording per-draft scores; using the descriptive macro there
left the old fuzzy comment unchanged. Re-recorded all scores successfully with
--nonmatching --record after formatting. depin.py --write was invoked on all four
units; it skips guarded drafts because their default build is not an exact C
match. No handwritten instruction bodies were introduced.


## 2026-10-20 — cloud task 080AD75C, Vorn reaction and route callbacks

Tags: #matching #vorn #scene-actor #terrain #nonmatching #register-pinning

Worked solo and offline on preclaimed cloud/080AD75C. No exact matches:
exact code progress gain is 0.000 percentage points (0% of the assignment).
Improved one existing draft, added three new guarded drafts, and retained the
existing defeat draft with its required file-local alias. Each unit was queued
separately through build/commit.sh. Used check.py for acceptance, never make or
a full ROM build, and made no network or direct git mutations. No shared headers,
generated assembly, ROM, shared documentation, or symbol tables were edited.

Family evidence: neighbours are named Vorn; the initializer near 080B4E18
installs 080AD75C, 080AE1C4, 080AE6F0, 080AEB74 and 080AEE08. Read their assembly,
the earlier codex-high task notes, and the relevant callees and exact relatives.
The Ghidra export is unavailable. Names remain tentative, with confidence=low.

- **080ADCC0 — drafted 98.9%, Vorn_UpdatePursuitDistance**, improved from
  85.7%. Full reviewed extent 1120 bytes, candidate 1120; six differing
  instructions/bytes and zero structural differences in anchordiff. Removing
  the context r7 pin, expanding centre construction at its two call sites,
  and removing the centre-pointer pin fixed the extra address materializations.
  A predicate returning the tested flags pointer through an output parameter
  reproduces the original load/and/high-register-copy ordering. Pinning the
  Div result z to sb swaps z and the saved flags pointer into their original
  high registers. Only this pin remains in the unit. The remaining six
  differences are three pairs of low-register choices in the middle-distance
  path: context flags offset at 080ADF56/58, timer offset at 080ADF88/8A,
  and map-global address at 080ADFE4/E6. Scoped offset/map pins, near/value
  widths, adjacent declaration swaps, and delayed centre pointer variants
  did not improve it. Function-scope variable variants need C89 declaration
  placement fixed before they can be usefully tested; those syntax failures
  were discarded. The permuter setup failed before a search because split.py
  cannot resolve unrelated Ghoul_ResumePendingRoute (0808CC48) against the
  checkout's generated function starts; no rebuild was attempted.
- **080AD75C — stopped below threshold, 77.2%**, tentative
  Vorn_UpdateStaggerRecovery in ignored wip/sub_080AD75C.c. Reviewed extent is
  1076 bytes, through the final literal at 080ADB8C, ending at 080ADB90;
  candidate 1080. Final comparison: 143 differing instructions / 38 ignoring
  registers. Entry selects animation three, plays 0x1CB and 0x12A for motion
  flag eight, clears the completion latch; subsequent frames mark flags,
  emit a spark every twelve frames, and converge saved X/Z motion by signed
  quarter differences clamped to +/-200. At rest and completion, it chooses
  a delayed stagger control, context recovery, or player pursuit and saved
  route control. Exact 08082D80 and 0808148C supplied the convergence and spark
  idioms. Explicit action-flags and motion-flags pointers improved the baseline
  from 65.0%; high-register pins and predicate/lifetime variants regressed or
  stalled. Remaining work is flags/route address reuse, stack-vector lifetimes,
  register choices, and late state/control scheduling. No src draft committed.
- **080AE1C4 — drafted 82.1%, Vorn_UpdateKnockbackRecovery**. Reviewed
  extent 1056, candidate 1052; 118 differing instructions / 40 structural.
  Entry rounds direction, selects animation six, requests attachment category
  25 once, resets reaction/collision flags and broadcasts a message. Each frame
  writes 65 at actor+0x1EA (not +0x2EA), uses a signed timer at +0xAA for blink
  scheduling, then applies the same quarter-step convergence. At rest with a
  nonpositive timer it installs group+0x20C control kind eight, restarts the
  route, adjusts collider flag 0x100, and optionally enables motion flag eight.
  Correcting the +0x1EA store and 0x02000000 flag constant, expressing heading
  division with the signed conditional idiom, caching the initial mask two,
  and passing the existing motion-flags pointer through ClearMotion brought
  this above threshold. Actor/prior-pointer pins and alternate control-address
  forms regressed; none retained. Reset-address lifetimes, collision scheduling,
  and saved registers remain unresolved. This new unit has no register pins.
- **080AE6F0 — drafted 83.9%, Vorn_ResumeReactionRoute**. Reviewed extent
  592, candidate 600; 55 differing instructions / 23 structural. On entry
  turns back and selects animation zero. Flags2D0 bit 0x10 gates route resume;
  it clears bit four, sends a message, clears context flags, then runs script
  0824E20C/0x39F or restores group+0x210 control kind 15 and state 3/phase 1.
  Computing flags2D0 after the prepared-entry animation, using full-width signed
  masks for halfword clears, passing the retained motion-flags pointer into
  reset helpers, and pinning the long-lived flags2D0 pointer to r8 improved
  the first 64.0% candidate. The r8 pin is safe across the external calls.
  Saved-zero, result-local and explicit control-store variants regressed.
  Context flag scheduling, saved zero and address lifetimes still differ.
- **080AEB74 — drafted 80.2%, Vorn_TryRestorePatrolRoute**. Reviewed
  extent 660, candidate 664; 68 differing instructions / 34 structural.
  Requires flags2CC bits 0x400000 and 0x200000 and motion flag one. Saves a
  30-frame command for savedTarget/kind nine, samples the saved patrol point's
  terrain height, marks the destination ready, resets collision/context timers,
  sends a message, and returns one. A narrow local route view with a points
  pointer at +8 reproduces the original +0x74/+8 accesses. Chase_Height's
  repeated terrain-byte expression matches better than a cached-byte helper.
  A volatile local savedMotion pointer retains the observed stack slot across
  calls; it is initialized on the successful guard path before every use.
  It is currently stored after that guard, whereas the original spills during
  the predicate. Low-register pointer pins across terrain calls were rejected
  as unsuitable; no pins remain in this unit. Destination spill, flags-address
  reuse and reset scheduling remain unresolved.
- **080AEE08 — retained 83.4%, Vorn_UpdateDefeat**. Reviewed extent and
  candidate both 984; 84 differing instructions / nine structural. Added the
  required file-local alias and raw sub_080AEE08 fallback spelling, then recorded
  the score again. Tested RNG table/index forms, removal of RNG-result/table
  pins, and a separately evaluated last spark count. None improved the best
  structural comparison, so retained the previous body unchanged. The remaining
  differences include low-register allocation, a copied first RNG table base,
  and late state/control constant reuse. The existing pinning marker remains.

All five retained units compile as NONMATCHING, keep INCLUDE_ASM as their default,
and have checker-recorded scores after direct clang-format. No handwritten
instruction bodies or instrumented-compiler settings were introduced. The
worker leaves combined ROM validation and any integration/name promotion to
the integrator. Helpers passing existing field pointers often preserve the
original address lifetimes better than helpers recomputing them from the actor.

## Task 081E013C — 2026-10-20

Tags: #matching #animation #menu #scene-actors #register-pinning

Worked solo and offline on the preclaimed task. Five of six targets finish
exact (83.3% by function count); three are new matches, while two already had
exact C from the earlier 081DEC2C task. New exact code gain is approximately
0.070 percentage points (1,692 bytes), all in the Register-pinned category.
The remaining target is a guarded draft. Only check.py was used for acceptance;
no full build, make, network, or direct git mutation. Separate commits were
queued through build/commit.sh and observed in git log.

- **081EAB64 — drafted 90.7%, Params3D0_UpdateDisplay**, improved from
  88.5%. Existing creator 081EB7DC and TimeDisplay callees establish the menu
  family. Seasonal table bases must be captured before the light-phase call,
  then offset afterwards; this preserves the unadjusted ROM label in the pool.
  Signed s16 position and preparing the palette input pointer before the output
  pointer improve the selection slides and palette calls. Final checker-recorded
  draft: 133 realigned differing instructions, 39 ignoring registers; candidate
  2,620 bytes, checker-reviewed original extent 2,612 bytes. Palette field/address reuse,
  selection-position arithmetic and allocation remain. Used BIG_FUNCTIONS,
  variants batches and rtlprobe perline; rejected full-width position and
  explicit keyframe-zero variants. Default retains INCLUDE_ASM.
- **081E013C — matched/revalidated, Password_InitializeEntryMenu**, 696-byte
  reviewed extent. This was already exact on arrival. Added its required local
  alias to resolve against the local ELF's address spelling, then depin removed
  ten of eleven hints. Only the first relocated header pointer's r5 pin remains.
  No new exact credit for this function.
- **081EC7D8 — matched/revalidated, ActorDisplay_UpdateForegroundSprite**,
  412-byte reviewed extent. Also already exact on arrival. Added the local alias,
  and depin reduced 21 hints to 13 while preserving exact bytes. No new credit.
- **081EC974 — matched, ActorDisplay_UpdateAccentSprite**, 360-byte reviewed
  extent. Called immediately after the foreground updater by
  ActorDisplay_RefreshScreenLayer at 081ECADC. Drawable +0x308 uses screen X,
  origin +0x5E0, a signed three-step offset at drawable +0x40, and a +149 Y
  position. +0x5EC controls visibility. Reused the foreground's packed sequence
  decoder. The +0x40 field doubles as a short positional cycle here and is
  reset on playback completion; do not generalize its name to animation-ended
  for every consumer. Explicit coordinate temporaries and a short r0 binding
  reproduce load/subtract order. Depin leaves 12 playback/coordinate hints.
- **081ECF18 — matched, SceneActors_UpdateIdleDisplay**, 652-byte reviewed
  extent ending 081ED1A4 (before the trailing undecoded next body). Known
  SceneActors_EnterIdleState installs it. Entry byte +0xB1 gates initialization:
  clears +0x298/+0x290, starts foreground sequence zero and accent sequence four,
  and steps both. Each frame advances the shared 1,024-entry random-table index,
  sets +0x288 to its low bit, then refreshes the three display pieces. Passing
  the already-established sequence pointer to the playback inline avoids extra
  pointer calculations; scoped flip masks stop constants being kept across
  the second initialization call. Loading the RNG sample before preparing its
  destination fixes final store ordering. Depin leaves ten short playback hints;
  entry zero/context/actor pins all proved unnecessary and were removed.
- **081ED5F4 — matched, SceneActors_UpdateMovingDisplay**, 680-byte reviewed
  extent ending 081ED89C (the separate return stub follows). Installed by
  SceneActors_EnterMovementState. Shares the idle initialization, then calls
  ActorMotion_AdvanceTowardScreenTarget with +0x294, refreshes display and emits
  periodic particles with its second argument. A temporary actor barrier and
  second-argument pin initially fixed prologue move ordering; depin removed
  both once the common decoder hints were minimized. Ten playback hints remain.

All five exact sources passed explicit reviewed-size checks after direct
clang-format. tools/depin.py --write was run on every exact hinted unit. Naming
is provisional where marked confidence=low. Existing shared types were reused;
no shared headers, symbols, generated files or shared docs changed. The local
ELF predates promoted target names, so file-local aliases also permit checker
symbol resolution. Combined ROM validation and name promotion remain with the
integrator.

# Task 08206C00 — Vulcan cannon states (2026-10-20)

Tags: #matching #vulcan-cannon #animation #register-pinning

Worked offline on the preclaimed cloud/08206C00 branch. Only fast checker
compilations were used, with reviewed extents for exact acceptance; no full
build, network operation or shared documentation edit. Commits went through
build/commit.sh. Ghidra's export is absent. Existing VulcanCannon_Create,
VulcanCannon_FireProjectile, VulcanCannon_InitCollider and the callback literals
establish this family's ownership and provisional field meanings.

- **08206C00 — matched, VulcanCannon_UpdateTargeting**, reviewed extent 1,040
  bytes. Improved the existing 83.6% draft. Tests squared player distance,
  directional firing alignment and clear-shot visibility; changes to firing
  or facing-turn states. A timed heading check also requests a turn. Inline
  distance and angle helpers taking whole position pointers reproduce the
  paired signed-coordinate accesses. Keeping the effective player pointer
  in r0, and using short r0/r3 and r2/r3 bindings inside those helpers, fixed
  the last dead copy and swapped pointers. A memory operand on the target
  vector preserves its stack copy without materializing its address. A short
  r5 facing binding keeps the zero-facing value live through the turn call,
  preventing the compiler from merging the two turn-call bodies. depin removed
  five unnecessary hints (21 to 16); exact checked again after formatting.
- **08207010 — matched, VulcanCannon_UpdateFiringBurst**, reviewed extent 448
  bytes. Fires at ten-tick intervals through tick 30, restarts the firing
  animation, and plays its shot sound every third projectile. After the burst
  delay, resets the timer and returns to targeting once the configured burst
  count is satisfied. The shared packed-command playback helper matches with
  its mask in r4 and mask source in r2.
- **082071D0 — matched, VulcanCannon_UpdateDefeatEffects**, reviewed extent
  384 bytes. Steps the defeated animation; non-inert cannons create particles
  at ticks 13 through 23 and record defeat at tick 1. Explicit inline position
  copy and raise helpers materialize the stack destination before the source
  loads, fixing the only four differing instructions in the first candidate.
- **08207350 — drafted 88.0%, VulcanCannon_UpdateFacingTurn**. Disallowed
  turning returns to targeting immediately. Otherwise animation completion
  toggles facing and restores the targeting animation. The local playback
  variant and owner/token constraints recover most of the original ip-owner
  layout, but address allocation, facing-store/reload ordering and return-path
  sharing remain: 32 differing instructions, 16 ignoring registers. This
  callback is declared void, consistent with its discarded result in the main
  update. No undefined fall-through C result is retained.
- **082074F0 — drafted 81.3%, VulcanCannon_UpdateDamageState**. Pause gates,
  invokes the current state, maintains collider activity, applies damage and
  HUD feedback, emits defeat debris/smoke, changes target attachments and
  palettes, blinks the sprite, and advances timers. Passing the project's
  RANDOM_TABLE_ADDR through the RNG inline lets agbcc defer intermediate
  random-index stores, unlike direct extern-array indexing. Initializing the
  collider pointer separately in the live/dead branches matches the original
  lifetimes better. Remaining: 121 differing instructions, 30 ignoring
  registers, mainly effect/vector setup, constant reuse and allocation. The
  draft defines its discarded result as zero through a short r0 constraint.
  An initial commit's message used the candidate-length score; the subsequent
  refinement and recorded source score are authoritative and clear the full
  80% retention threshold.
- **08207A00 — drafted 96.4%, VulcanCannon_InitializeFromScript**. Recovers
  options p/a/R/l/d/i/h/r/I/n/S/z/g/D/G/X/V, graphics 0x7E7D, resource
  0x922E/0x5184, initial active/inert states, optional terrain registration,
  scene-body setup and the delay list. Defaults and signed/narrow fields are
  represented in include/vulcan_cannon_worker.h, shared by this batch's new
  sources only. Uncertain fields retain offset names. Remaining: 23 differing
  instructions, six ignoring registers; switch bounds, position-pointer
  lifetime and explicit return-zero epilogue are the structural residue.
  A no-result candidate reached 97.3%; the retained draft returns zero so the
  factory's negative-result check consumes a defined C value. The initial
  commit message predates the final full-extent score; source records 96.4%.

All three exact matches retain the Register-pinned concession, including
constraints in the shared animation helper. depin was run on each exact unit;
it sees direct hints in the targeting source, but does not expand the shared
helper for the other two. Each draft keeps INCLUDE_ASM in its default branch;
all scores were recorded again after direct clang-format. Batch integration
must still perform the combined ROM validation and name promotion.

## Task 081AA178 — 2026-10-20 follow-up

Tags: #matching #vanargand #paired-scene-actor #register-pinning

Worked solo and offline on the preclaimed cloud/081AA178 branch. Read the
worker/cloud guidance and BIG_FUNCTIONS.md, looked up the existing attempts,
and inspected the largest target and its relatives before refining retained
candidates. Exact code gain in this pass: +0 percentage points; no new exact
matches. Three existing guarded drafts were retained in separate driver commits.

- **081ABA64 — stopped unmatched.** Re-inspected its opening assembly,
  condensed listing and matched relatives. The best relative covers 37%, mostly
  the six animation playback expansions, rather than the eleven-phase attack
  machine. The prior notes already identify local far-branch tails and the
  track/synthetic-contact behavior. No new complete candidate or productive
  structural hypothesis emerged; assembly remains in use.
- **081AA788 — stopped below threshold.** Resumed the complete scratch
  Vanargand_UpdateSweepingProjectileAttack candidate. The original check was
  70.3%, 265 realigned differences / 87 ignoring registers. Keeping the owner
  in sb, the spread in sl, and a separate auxiliary-part pointer across
  Anim_SetSequence improved the preliminary score to 75.4% (225 / 99), with
  candidate length equal to the 1672-byte body ending at 081AAE10. This is
  still below the retention threshold. The part pointer recovers resource
  +0x58/reset +0x5C accesses; randomized projectile setup, stack lifetimes,
  phase transitions and argument ordering remain. Latest source stays only
  in ignored wip/sub_081AA788.c.
- **081AA178 — improved guarded draft, 99.5%,
  Vanargand_UpdateRepeatedPairAttack.** Full reviewed extent is 440 bytes.
  A phase input constraint keeps the phase live through the phase-zero RNG
  path, giving the original timer-pointer allocation. A scoped r0 zero
  constraint gives the separate reset after the sixth repetition. Only one
  byte / one instruction differs: phase-zero timer reset uses r7 rather than
  r5 (both contain zero); structural differences are zero. Narrow/wide phase
  trials, fixed phase bindings and separate dispatch/result lifetimes did
  not close it. Retained readable C and marked the register-pinning concession.
- **081AFD10 — improved guarded draft, 95.3%,
  PairedSceneActor_UpdateHalfPresentation.** Full reviewed extent is 648
  bytes; 15 realigned differences, zero ignoring registers. Scoped shake/reset
  destination pointers, palette-timer offset and shadow offsets recover most
  remaining allocation. A mask constraint after shadow-control address setup
  recovers its instruction order. Explicit flag/owner-offset bindings broke
  useful CSE and made the code worse, so were discarded. Remaining offsets
  and pointer registers are listed by the checker; no behavioral omissions
  were found. Register-pinning concession remains marked.
- **081B0248 — stopped below threshold.** Resumed the radial-effects scratch
  candidate. Reversing stack-vector declaration order, consistently raising
  the same position-pointer view and preserving signed RNG samples before
  shifting improved the preliminary result from 63.1% to 65.2% (74 realigned
  differences / 27 ignoring registers, full 376-byte extent). Position/velocity
  stack placement, polar helper allocation, RNG update order and loop scratch
  registers remain. No production draft; latest source stays only in ignored
  wip/sub_081B0248.c.
- **081B03C0 — retained guarded draft, 86.2%,
  PairedSceneActor_UpdatePlayerCapture.** Full reviewed extent is 1272 bytes;
  candidate is 1308, with 112 realigned differences / 55 ignoring registers.
  Binding the playback owner to r6, with either input or input/output
  constraints, increased differences and was discarded. Remaining work is
  playback allocation, request arguments and shared phase-entry tails. Added
  the required file-local alias for its already established symbol name.

All three retained units now have exact file-local aliases, confidence comments
above those aliases, and raw sub-address INCLUDE_ASM fallbacks (the macro
stringifies its argument). This also lets the checker resolve names against
the older local ELF. Formatted directly with build/venv/bin/clang-format;
recorded each draft with check.py --nonmatching --record and checked each
default assembly fallback exactly. Commits used build/commit.sh. No make,
full build, network operation, shared documentation/index edit, ROM or generated
assembly commit. Combined validation and name promotion remain with integration.

# Task 081A2430 — Codex medium, 2026-10-20

Tags: #matching #hresvelgr #vanargand #effect-pool #register-pinning

Worked solo on the already claimed `cloud/081A2430` branch, offline. Exact
code gain: **+0.0000 percentage points**; no new exact C matches. Retained
five guarded drafts (four inherited, one newly authored). All scoring used
`build/venv/bin/python tools/check.py`; no make or full build was run.

- **081A5614 — retained draft 80.7%**,
  `Vanargand_UpdatePaletteTransition`. Read BIG_FUNCTIONS, the condensed
  listing and the matched restore/interpolation relative in 081ACC24.
  Baseline: 299 realigned differences / 82 ignoring registers, candidate
  2848 bytes against the 2800-byte symbol extent. A blend wrapper taking
  the owner and computing the output address inside reduced structural
  differences to 60, but worsened allocation and fuzzy score to 76.8%.
  Removing the selector pin, pinning RGB/frame temporaries, and changing
  interpolation to a shared result did not improve the retained draft.
  Restored baseline; added the required local alias and raw assembly
  fallback so it resolves against the cached ELF/generated assembly.
- **081A2858 — improved draft 86.3% to 91.3%**,
  `Hresvelgr_UpdateCollisionPresentation`. Fixed a real source defect:
  the first invulnerability path dereferenced `flags` before initialization.
  It now accesses the owner's flags directly. Preserve the second timer
  load before materializing the shared flags pointer. Split the disabled
  collider flag operations into two full-width signed bitfield updates;
  this reproduces the otherwise surprising zero OR. Reordered the camera
  branches to put the set-offset path before reset. Sprite/sequence inline
  accessors improve address and argument setup. Remaining: 49 realigned
  differences / 39 ignoring registers, including reset-pointer allocation,
  sprite field addressing and constant scheduling; candidate/target 868 bytes.
- **081A39E0 — improved draft 88.3% to 91.1%**,
  `EffectPool_UpdateEntry`. Caller 081A3D00 iterates entries; the target-angle
  helper and the body establish homing, animation/collider updates, trail
  emission and lifetime-dependent acceleration. Keeping the angle mask
  opaque in r3 preserves the final AND before the byte store. Pinning the
  animation timer to r0 improves its scheduling. Separate vector component
  temporaries and an input-liveness barrier reproduce more of the animation
  flag operations. Remaining: 37 realigned differences / 4 ignoring
  registers, mainly temporary registers, RNG/trail setup and timer mask
  scheduling. Candidate and target 800 bytes.
- **081A2430 — retained/revalidated draft 87.5%**,
  `Hresvelgr_DispatchSceneMessages`. Explicitly alias the renamed callback
  `Hresvelgr_UpdateRisingProjectile` to 081A12DC for the cached ELF.
  Ordinary multidimensional pending-message indexing regressed; alternative
  pointer arithmetic and sharing zero/one action parameters also regressed.
  Retained the original structure: 51 realigned differences / 25 ignoring
  registers, candidate and target 680 bytes. Remaining differences are
  pending-buffer indexing and mode/action constant lifetimes.
- **081A8F48 — new draft 88.7%**, tentative
  `Vanargand_UpdatePlayerDrain`. Positions player zero at the boss position
  plus (128, 0, 128); chooses animations 12–14 and copies player collision
  data according to player mode at 418. Later adds the position-request
  result to boss health and shows the change. Preserve the second position
  construction before testing the player flag. A mask-taking inline and a
  retained initial `one` recover the initial failure block and argument
  setup. Keep the signed start-frame bias explicit; otherwise CSE rewrites
  it onto the current frame and loses the original load order. Remaining:
  38 realigned differences / 19 ignoring registers, initial vector aliases,
  register choices and constant scheduling. Candidate and target 560 bytes.
- **081A6AEC — stopped below full-extent draft threshold; no production C.**
  Tentative `Vanargand_GetTargetLateralDistance`. The call site following
  081ABB6C clamps the result to 512. The visible body projects the actor position at 54 and
  target position via 110, keeps only projected X separation, unprojects
  with zero Y, and returns planar square-root distance. Independently
  authored candidate, using the matched projection idiom from 081A543C,
  improved from 65.8% to 85.6% against the visible 312-byte body. A local
  owner pin to ip and a shared-result signed-shift helper produced that
  gain. It remains only **51.0% against the 736-byte symbol span**: the
  existing symbol includes undecoded executable bytes beginning at 081A6C24
  through the next function at 081A6DCC. No shortened-extent match or draft
  credit is claimed. Boundary recovery is needed before resuming.

All five production drafts retain NONMATCHING guards and useful local names.
Formatted directly with `build/venv/bin/clang-format` (restoring the exact
single-space alias syntax afterward), recorded final draft scores, and checked
all default assembly fallbacks byte-identically. Follow-up per-unit commits
repair the inherited fallback names and retain formatting. Commits use only
`build/commit.sh`; no ROM/generated assembly is committed.

Tool caveat: `depin.py --write` does not enable NONMATCHING, so for drafts with
working assembly fallbacks it can report success and strip hints without
checking the C. Its removals in 081A39E0/081A8F48 were immediately restored;
the final C scores were verified again. Other drafts were skipped by depin
because their inherited named fallback files were absent; those fallback
identifiers are now fixed. Use unguarded scratch candidates for future depin
experiments on drafts. Combined ROM validation remains the integrator's job.

## Task 080B1FA8 — Vorn route follow-up (2026-10-20)

Tags: #matching #vorn #scene-actor #terrain #route

Worked solo, offline, on the already checked-out `cloud/080B1FA8` branch.
Exact completion remains **0%**, with **+0.0000 percentage points** of new
exact code. Improved two inherited guarded drafts and retained a third;
three new candidates stopped below the 80% threshold. No full build or make,
network operation, direct git mutation, shared header edit or generated
assembly commit. Per-file commits and this note use `build/commit.sh`.

- **080B2858 — retained draft 85.1%, Vorn_SelectRetreatRoute.** Read the
  callers, complete assembly and previous attempt. Baseline remains 134
  realigned differences / 48 ignoring registers. Giving SelectRoute a
  distinct opaque actor pointer worsened it to 82.6%, so restored the
  baseline. Actor/group lifetimes and spills, success-tail state/control
  addressing and RNG constant scheduling remain. Added the required local
  alias and an alias for the 080B2580 callee; raw INCLUDE_ASM fallback fixes
  resolution against the cached ELF. Candidate 1672, target 1684 bytes.
- **080B3464 — improved draft 84.3% to 87.8%,
  Vorn_ResumeRouteAfterReaction.** Read BIG_FUNCTIONS and steered with
  realigned counts: 129/74 to 113/70. Retain the actor flags2D0 pointer
  across both Resume expansions, pass it to the inline, materialize the
  group control before its flag condition, and express TryRoute as nested
  success tests with duplicated failure cleanup. An opaque inline actor
  pointer regressed to 69.8% and was discarded. Actor copies/spills,
  control offsets, halfword-mask registers and route argument allocation
  remain. Candidate 1636, target 1656 bytes. Required local alias added.
- **080B2580 — improved draft 81.6% to 84.0%,
  Vorn_SelectReachableEscapeCell.** Move the four scan-result expressions
  to labels following the cardinal switch. This reproduces the shared
  east/south/west/north tails and reduces structural differences from 53
  to 34. Explicit outer-loop found/break and a failure-first final route
  test improve further to 69/27 differences. Volatile found regresses
  to 77.7%; retain the ordinary found variable. Remaining differences:
  loop jump threading moves success and range tests before the retry loop,
  terrain pointer reloads/pool placement, and allocation. Candidate 724,
  target 728 bytes. Required local alias added.
- **080B1FA8 — stopped at 55.2%; no production draft.** Tentative name
  Vorn_SelectOpposingRoutePoint. Computes target-relative facing, makes a
  planar direction vector from the sine table scaled by 160, and scans
  eight-byte route points for a negative dot product excluding the current
  point. Falls back to a different random route index, snaps destination
  height and attempts route motion. Failure installs idle control/state;
  success marks destination pad=1. Packed-vector stores must retain
  word read/modify/write behavior while the dot product uses signed
  halfword loads through a ContactPosition view. That alias improves
  40.4% to 49.5%; extracting the scan and copying its origin, then retaining
  the group pointer, reaches 55.2%. Still 217/135 differences over the
  reviewed 812-byte extent; candidate 816. Stack-vector lifetimes,
  scan return layout and register allocation remain unresolved.
- **080B23A0 — stopped at 61.5%; no production draft.** Tentative name
  Vorn_BeginContextPursuit. A context +6AC timer gates entry. Reset=1 or
  response flag conditions allow a 60-frame pursuit; sets context flag 4,
  clears flag 2, initializes context +6EC to 60, optionally starts the
  alternate route, and calls 080B1FA8. Installs behavior 15 and callback
  080ADCC0/action 0x25, plus actor/group pursuit flags. Explicit unsigned
  halfword temporaries, signed masks, separate word/byte zeros and
  temporary register hints improved 50.0% to 61.5%. Still 94/28
  differences, chiefly constant/address scheduling and control-field
  allocation; candidate and reviewed target both 440 bytes. These
  experimental hints are confined to ignored artifacts.
- **080B3C1C — stopped at 69.9%; no production draft.** Tentative name
  Vorn_AcquireContactTarget. Checks actor flags and callback26C, requires
  motion flag 1 and a nonzero range, and selects contact kind 6. Releases
  an existing contact when flags2D4 bit 0x1000 is set, records the selected
  object and contact kind, and either refreshes an existing response or
  installs state/phase 3/1, a 180-frame timer and an animation update.
  u32 mask temporaries recover bics for 0x1000 and 0x100. Keeping the
  initial zero mask result on the stack and changing those masks improves
  66.8% to 69.9%, with 87/18 differences; candidate/target 572 bytes.
  Most residue is a persistent r6/r7 swap and motion/contact pointer
  copies. Broad or individual pointer pins and declaration swaps failed
  to improve it; no retained production source uses those hints.

Final draft scores were recorded with check.py --nonmatching --record after
venv clang-format. All three default INCLUDE_ASM fallbacks were checked
byte-identically; these assembly matches do not count as new exact C.
All retained aliases use the exact required single-space syntax. No retained
production candidate uses register pinning. The three below-threshold source
attempts are only in ignored build/attempts/080B1FA8/ for local follow-up;
final exploratory scores above use explicit reviewed target sizes. Ghidra's
gen/ghidra/decomp.c is unavailable. Combined ROM validation is left to the
integrator as required by the worker instructions.


## Task 08207350 — cannon callback follow-up (2026-10-20)

Tags: #matching #vulcan-cannon #cannon-turret #collision #register-pinning

Worked solo and offline. New exact completion is **7.30% of this task's
reviewed code extent**, a **+7.30 percentage-point gain** (416 / 5700 bytes).
The batch adds **+0.0172 percentage points** of project code, all in the
Register-pinned category. One inherited draft became exact, four guarded
drafts were refined, and the previously missing hit callback now has a
qualifying guarded draft. No full build, make, network operation, shared
header change, generated-assembly commit or direct git mutation was used.

- **08207A00 — retained draft 96.1%, VulcanCannon_InitializeFromScript.**
  Read the largest function, prior notes, BIG_FUNCTIONS and relatives first.
  A separate facing <= 3 guard with an empty value constraint preserves the
  ROM's redundant switch bounds check. Retaining the terrain position in r3
  removes the extra coordinate-pointer copy. Realigned differences improve
  from 23/6 to 26/2 (total / ignoring registers): fewer structural differences,
  but the high sprite/position registers exchange roles. The defined return
  zero also remains an extra instruction; keep it because the factory consumes
  the initializer result. Candidate and reviewed target are both 1384 bytes.
  The removed position barrier was tested to have no effect.
- **082074F0 — improved draft 81.3% to 81.9%,
  VulcanCannon_UpdateDamageState.** An inline CopyPosition(out, in) materializes
  each stack destination before its source loads, fixing the center and smoke
  setup order. Differences fall from 121/30 to 115/26. Separating the dead
  collider pointer, adding a random-value barrier, or simplifying the final
  r0 return constraint regressed and was rejected. Effect masks/constants,
  zero reuse across the smoke calls and allocation remain. Candidate 1296,
  reviewed target 1284 bytes.
- **08208740 — retained draft 81.7%, CannonTurret_UpdateCombat.** Initialize
  the collider separately in the live/dead branches and use the same position
  copy inline. Realigned differences improve from 119/38 to 115/30 even though
  the fuzzy score stays unchanged. A combined pause guard gave no improvement.
  Pause/return layout, animation register copies, effect constants and allocation
  remain. Candidate 1180, reviewed target 1176 bytes.
- **08208BE4 — improved draft 93.3% to 95.5%,
  CannonTurret_InitializeFromScript.** A memory constraint after setting the
  inert/dead field reproduces the resource reload before the inert animation
  setup. Differences fall from 37/6 to 23/5. A resource value constraint was
  weaker; forcing the body pointer lifetime was worse. Return-zero, constant
  reuse and selected register lifetimes remain. Candidate 1016, reviewed
  target 1012 bytes.
- **08207350 — matched, VulcanCannon_UpdateFacingTurn.** Start from the prior
  88.0% draft. A local signed facing value with a single final store avoids
  repeated field-address computation and gives the signed halfword-offset
  byte load. A shared first-flip store with r1 flags and r2 owner views matches
  the common store tail. Define the return as zero when turning is disabled,
  the incoming owner token when animation continues, and Anim_RestartSequence's
  actual u32 result on completion. The callee is a matched return-value wrapper
  at 08220D20, so this avoids undefined C fall-through. Keeping the turn-enable
  test in r1 removes the last five differing instructions. depin reduced 18
  direct hints to 12; the final unguarded source matches the entire reviewed
  416-byte extent exactly after clang-format. No shared helper was edited.
- **082081F4 — new draft 82.6%, CannonTurret_ApplyHitReaction (tentative).**
  Read its collider-registration caller at 082083A0, complete assembly, and
  matched Combat_ApplyHitReaction (0820FEA8). The callback clears blocked
  hits, records damage/type/reaction, updates damage display and HUD for flag
  0x40, otherwise adds contact stats, then emits three sparks when source
  flags intersect target +13A. Spark velocity is copied from gUnk_0824FE84;
  position comes from target +40 with random planar offsets [-64,63] and a
  byte-sized vertical offset. An inline using unsigned halfword views avoids
  scalarized word read/modify/write operations on the stack vector. Keeping
  the table in r3 and constraining each computed planar base together improve
  69.1% to 82.6%. Broad loop/index/mask pins regressed to 51–57% and were
  discarded. Still 49/29 differences: first RNG store, hoisted loop constants,
  target-field scheduling and registers. Candidate and target both 428 bytes.

Every retained function has the required file-local semantic alias, and named
fallbacks were changed back to raw sub_ addresses so the cached ELF resolves
them. All five draft scores were recorded with check.py --nonmatching --record
following direct venv clang-format. Their default INCLUDE_ASM branches also
check byte-identically; these assembly checks earn no exact C credit. depin
was run on unguarded copies of the drafts to avoid testing their asm fallbacks;
it correctly skipped nonmatching bodies. Driver commits are separate per
function, followed by this note. Scratch alternatives and diagnostic outputs
remain ignored under build/attempts/08207350/. Combined ROM validation and
name promotion remain the integrator's work.

## 2026-10-20 — cloud task 08214E3C

Tags: #matching #sprite #sprite-oam #background #palette #nonmatching

Worked solo, offline, on the preclaimed branch. Only per-file check.py checks
were used, including those beneath variants.py; no make, full build, network,
or direct git mutations. Commits went through build/commit.sh. Assignment
instruction coverage is 6.46% (+6.46 percentage points; 157 of 2431 instructions).
Global progress was not regenerated. Ghidra's decomp.c export is unavailable.

- **08216584 — stopped below draft threshold, BgGfx_LoadTilemap.** Read the
  full condensed listing, BIG_FUNCTIONS.md, callers and existing family notes.
  Reused the ignored candidate in this checkout. Replacing signed truncating
  division with arithmetic shifts only on coordinates already checked as
  nonnegative, and adding the metatile row offset after the column offset,
  reduced realigned differences from 951/265 structural to 594/228. Computing
  viewport bounds earlier improved structure further to 599/220 (50.6%).
  Widening the blank color to u32 regressed severely. Still below threshold:
  inline row/column lifetimes, outer-loop increment placement, expression
  ordering, spills and allocation differ. Updated ignored wip candidate only.
- **08217430 — stopped below draft threshold, Palette_ApplyObjectFade.** Read
  full assembly, graphics-update caller and existing reconstruction. Baseline
  remains 67.4%, 256 realigned differences / 100 ignoring registers. Tested
  explicit predicate result locals, unsigned level and volatile mask reads;
  none improved the baseline. The mask tests and three distinct bank loops
  still need their original constant hoisting and allocation. Kept in wip.
- **08214FBC — matched, Sprite_RenderUnflippedList (344 bytes, unpinned).**
  Reconstructed from assembly using the matched screen-projection inline in
  actor_animation_controller.c. Traverses the current 030042C0 sprite list,
  skips hidden/frame-parity-selected entries, requires flag 0x2000, projects
  world coordinates unless flag 16 supplies screen coordinates, rejects
  offscreen resource bounds unless flag 64 bypasses clipping, then submits
  through 08214600. Resource dimensions/offsets are at +8..+B; sprite resource
  at +C, position +1C, next +28. This rendering path ignores flip flags when
  computing bounds; the semantic meaning of its required flag is tentative.
  First candidate matched exactly; formatted final source also matches.
- **08214E3C — drafted 89.1%, Sprite_RenderFlipAwareList.** The sibling requires
  flag 0x1000 and adjusts resource bounds for flip flags 4 and 8. Pinning output
  coordinate lifetimes brought 55.8% to 85.3%; a width register, an explicit
  r0 pointer for reading the projected stack vector, and sharing the width
  load after the horizontal branch reduced structure further. Final reviewed
  check: 22 realigned differences / 2 ignoring registers, extent 388 bytes.
  Projection-source r8 versus ip and flipped-bounds register ties/scheduling
  remain. Guarded assembly-default draft, register-pinning concession marked.
  Its commit title's 88.5% was stale; the recorded reviewed score is 89.1%.
- **082154E4 — retained 82.6% draft, SpriteOam_FinalizeQueue.** Read the complete
  assembly and checked the existing draft. Tested output-buffer/counter pins,
  countdown priority loop, scoped disable pointer and matrix-count pointer;
  none improved the 41 realigned / 12 structural baseline. Priority-loop
  allocation and premature invariant-address hoisting remain. Added the
  required file-local alias, made the fallback use the raw assembly symbol,
  formatted, and rerecorded the reviewed 436-byte draft.
- **08217960 — retained 80.1% draft, Palette_ApplyBackgroundFade.** Verified the
  existing 604-byte draft: 82 realigned differences / 39 structural. A green
  term r10 pin reduced structure to 33 but added a concession with no useful
  fuzzy gain, so it was discarded. Explicit source-color caching and counter
  pinning also failed to improve it. Constant hoisting, loop-counter setup and
  packed-color operand allocation remain. Added the required local alias and
  raw-symbol fallback; formatted and rerecorded the existing draft.

Final checks: byte-exact acceptance for 08214FBC; --nonmatching --record for
all three guarded drafts. The follow-up style commit formats the exact sprite
source without changing its instructions. No ROM or generated artifacts added.

## 2026-10-20 — cloud task 082015D4

Tags: #matching #nonmatching #combat #projectile #ooze #tooling

Worked solo on the preclaimed branch, offline. Per-file checks and diagnostic
variant batches only; no make, full build, network or direct git mutations.
Exact assignment progress: 0% (+0 percentage points). Five guarded drafts are
retained; these earn no exact progress. Existing exported names were preserved
and all retained functions have file-local semantic aliases.

- **08205B70 — retained draft 89.1%, Ooze_BeginAttack.** Read BIG_FUNCTIONS,
  existing notes, the attack-state body and complete assembly. Baseline remains
  77 realigned differences / 17 ignoring registers. Timer-register variants and
  cached pointers to transition fields all regressed. The remaining differences
  are constant/address reuse across transitions and stepper allocation. The
  assembly file in this cache is named Ooze_BeginAttack.s, so its fallback must
  retain that exported spelling even though the C alias uses sub_08205B70.
- **08206098 — improved draft 85.0% to 87.8%, Ooze_InitializeFromScript.** Widen
  the heading-switch operand from u16 to u32 after the u16 narrowing: this keeps
  the two unsigned upper-bound checks instead of the old compiler's additional
  signed comparisons/reloads. Move the variant-1 body-origin adjustment into
  a small inline taking the stack SceneVec pointer; this avoids scalarizing
  coordinate stores into word masks. Final 79 realigned / 21 structural
  differences. Option-field pointer variants regressed. Sequence-ID address
  reuse, body setup and animation registers remain.
- **08202B30 — stopped below threshold, Projectile_SpawnFromConfig.** Read pool
  creation/free-slot routines, config setters and callers. Reconstructed config
  copy, trigonometric planar velocity, collider/resource/sprite setup, optional
  direction-dependent animation, homing target lookup, vertical-origin cache,
  sprite flag and occupied-slot bit. Initial 68.1%; exploratory hints reached
  76.3%, below the 80% draft threshold. Not added to src. Scratch is under
  wip/sub_08202B30.c and build/projectile-*.c. The C still differs in the velocity
  setup, saved config/slot/member pointer lifetimes, sequence argument ordering
  and sprite read/write views in the packed-command stepper. Fixed pointer
  pins introduced additional spills, so that experiment is not a trusted ABI
  reconstruction. No exact claim or durable draft for this address.
- **0820337C — improved draft 87.6% to 99.0%,
  CannonEmitter_InitializeFromScript.** Separate r4 pins for the two animation
  flip masks prevent their premature hoisting. Explicit frame-store r3 and
  first-flip-mask r2 views resolve the remaining stepper allocation differences.
  A descending signed palette index reproduces all code after the palette loop
  exactly. Only five realigned instructions differ (24 bytes): the ROM compares
  the descending palette pointer against its start, while this draft decrements
  an index. Direct signed-pointer, callback-end-pointer and inline-fill variants
  worsened address materialization/order; retain the 99.0% index candidate.
- **08201A78 — retained draft 80.2%,
  ScriptCombatActor_UpdateProjectileAttack.** Added semantic alias and a local
  address alias for the projectile-attack callback, which the cached ELF still
  calls sub_08201898. Explicit signed-angle liveness, a later position pointer
  and a volatile RNG-table parameter failed to improve 71 realigned / 33
  structural differences. RNG table load placement, stack-vector lifetime and
  allocation remain.
- **082015D4 — new draft 82.4%, ScriptCombatActor_ApplyHitReaction
  (tentative).** Ported the closely related CannonTurret_ApplyHitReaction after
  reading its registration at 08201784 and the target assembly. Records damage
  +180, type +1B8 and reaction +104; flag 0x40 starts the display/HUD response;
  selected flags emit three sparks using gUnk_0824FE44. Target-specific member
  offsets include +17C/+198/+19E/+1A2 and HUD pointer +1AC. RNG-counter, mask,
  position-pointer and countdown-register variants did not improve the retained
  score. Final 49 realigned / 29 structural differences, mostly RNG store
  timing, loop invariant hoisting and member-address allocation.

Final C checks used --nonmatching --record; all five assembly fallbacks also
passed per-file byte checks (these assembly matches earn no C credit).
clang-format was invoked directly from the venv. Driver commits contain the
individual sources and this note; no ROM or generated assembly was added.

Tool pitfall: depin.py on a guarded draft tests its default assembly fallback,
then incorrectly considers every hint removable. This briefly produced lower
scores in queued commits for 0820337C and 082015D4. Explicit nonmatching C checks
caught it immediately; follow-up commits restore the reviewed 99.0% and 82.4%
sources. Running depin on unguarded scratch copies correctly says the C does not
match and skips it. Future workers should strip the fallback before depinning
any draft. Batch integration/name promotion/full ROM validation remain with
the integrator.

## Task 0817F9C0 — 2026-10-20

Tags: #matching #boss-slot-2 #armor #actor-controller

Offline worker on the preselected cloud/0817F9C0 branch. Used per-file
`build/venv/bin/python tools/check.py` checks only, with driver commits;
no full build, network operation, shared-doc edit or generated-file commit.
Ghidra's `gen/ghidra/decomp.c` is absent, so reconstruction used assembly,
callers, named neighbors and matched relatives.

- **0818323C — matched, ActorController_UpdateMovingAttackCollider.**
  Called by ActorController_UpdateMovementVelocity and
  ActorController_UpdateDecayingVelocity. Starts the directional animation
  from 0824EC84, arms the attack collider on frame 9, emits the particle burst
  at the collider offset plus actor position, changes attack power on frame 11,
  disables the collider on frame 14, and ticks camera shake. The 08183078
  matched relative supplied most collider and layer idioms. Exact without
  register pinning, 580 bytes. The copied opening comment about half power
  belongs to the relative: this function uses full power on frame 9 and half
  power on frame 11. Important technique: write the stack-vector sum through
  s16 pointers, rather than local struct-field assignments (which generated
  word read/modify/write sequences). A zero local initialized after computing
  the destination of the facing store gives the final exact allocation and
  instruction ordering. Formatted source passed the exact checker.
- **081835D8 — drafted 94.0%, ActorController_UpdateHeavyAttackCollider
  (tentative).** Uses fixed animation records 0824ECC4/0824ECC8, transitions
  through three phases, and loads per-frame collider size, offset and facing
  from 0824ED0C/0824ECCC/0824ED4C. Attack scaling is 0xAAA in fixed point,
  followed by sign-aware truncation and force 80. The source's inherited
  half-power header/helper name is a seed artifact, not the recovered scale.
  Final guarded source uses no register hints. The default build retains
  INCLUDE_ASM; --nonmatching --record measured 10 realigned instructions,
  5 ignoring registers, against the reviewed extent. The commit subject's
  93.6% was the earlier candidate-length score; the recorded reviewed score
  is 94.0%. Remaining differences are the layer/collider base calculation,
  reuse of the 0x190 offset, and the multiply-result register. A pinning
  experiment reached 96.1%, but added an address copy and retained less useful
  constraints; kept the readable unpinned draft.
- **0817FDE0 — stopped below draft threshold (best exploratory 57.3%).**
  BossSlot2_CreateFromScript calls this initializer. Recovered script options
  t/B/i/P/p/s/a/e/c/A, actor registration, resource 0xCB05:0xDE23, packed
  position, health and collider configuration, frost emitter, shadow,
  proximity volume, camera, and palette buffer initialization. Needs the
  boss/common-actor pointer views recovered more accurately: current raw
  accesses and hard-register experiments alter address lifetimes, loop shape,
  spills and allocation. Initial structure score was 137 differing instructions
  ignoring registers; hints reduced that to 90 but did not produce a viable
  retained draft. Scratch: wip/sub_0817FDE0.c and build/fde0-*.c.
- **0817F9C0 — stopped below threshold (best 72.3%).** Tentative role:
  BossSlot2_UpdatePresentation. Called after the state callbacks by
  SceneActor_UpdateUnlessPaused. Updates effects and particles, collision
  visibility, frost emission, shadow visibility, palette countdown, shake and
  camera tracking; clears per-frame flags and increments the frame counter.
  A HasFlag inline reproduces constant-before-load ordering. Signed full-width
  collider flags and a duplicate pinned boss pointer did not solve the remaining
  address, halfword-update and register differences; rejected the latter.
  Scratch: wip/sub_0817F9C0.c; build/f9c0-current.c is the better unpinned form.
- **08186CC8 — stopped below threshold (unpinned 72.0%).** Tentative role:
  Armor_SelectAlignedAttack. Recovers player-relative rounded x/z displacement,
  absolute vertical separation, behavior/state gates, attack-origin proximity
  test and remembered-position update. An exploratory three-register pinning
  attempt scored 84.4%, but produced a suspicious stack-pointer comparison in
  place of the z test; rejected rather than committing misleading progress.
  Typed fields and declaration swaps did not improve the unpinned candidate.
  The compiler merges/reorders fallback SetBehaviorIfFinished calls and keeps
  a different phase-address lifetime. Scratch: wip/sub_08186CC8.c.
- **08186EB4 — stopped below threshold (best 75.7%).** Tentative role:
  Armor_SelectTimedAttack. Related alignment selector with timed follow-ups,
  directional facing checks and behavior 24. Explicit near/far branch labels
  reduced structural differences, but mode/one/phase-pointer allocation and
  shared zero-state exits remain unlike the ROM. Register hints did not help.
  Scratch: wip/sub_08186EB4.c; build/6eb4-base.c retains the 75.7% attempt.

Only the exact function and the >=80% guarded draft were promoted to src.
The four lower-scoring sources remain local scratch and earn no progress.
Full batch validation and name promotion remain with the integrator.

## Task 081B0F34 — 2026-10-20

Tags: #matching #actor-status #vanargand #register-allocation

Worked solo on the assigned offline checkout. All six functions already had
NONMATCHING drafts and promoted semantic names. Retained those names, restored
file-local aliases, and changed each assembly fallback to its sub_XXXXXXXX
identifier: INCLUDE_ASM stringifies its argument without macro expansion, and
this checkout's cached assembly/ELF still uses address names. This also lets
check.py --nonmatching --record associate each score with its fallback.
Exact C gain: +0.0 percentage points. No full build or network operation.

- **081B1F74 — retained/improved draft 94.5%,
  Vanargand_UpdatePartPairedStrike (was 92.5%).** Read the large-function
  workflow, assembly and matched relatives (notably the polar-vector and burst
  sequences). Changing addition operand order and sharing the entry zero with
  the post-MotionTrack_Initialize store improved the realigned count from
  65 to 46 differing instructions; 9 ignore-register differences remain,
  including jump-table data/padding. Entry-pointer copies, per-case state and
  timer address allocation, zero lifetime, and literal placement remain.
  A pinned entry-pointer experiment did not improve it and was discarded.
- **081B1334 — retained/improved draft 86.0%,
  ActorStatus_UpdateGuardedApproach (was 83.0%).** Isolated the guarded
  behavior-18 request as an inline taking the action argument. This better
  reproduces the argument/mask setup before the guards and reduces the
  realigned count from 149/55 to 119/34 (total/ignoring registers). The guard
  tests flag 0x100, previous actions 25/26 and completion before storing action,
  reset, completion, countdown, state and timer. Entry/state pointer lifetimes
  and packed-animation allocation remain. A volatile state read and raw
  vector-component stores did not improve the candidate and were reverted.
- **081B18C8 — retained draft 80.3%, ActorStatus_UpdateRisingStrike.**
  80/27 realigned differences. State-address reuse and entry-pointer lifetime,
  constant placement around Anim_SetSequence and stack-vector setup remain.
  Volatile state and raw s16-pointer vector writes produced no improvement;
  kept the original candidate with the required alias and working fallback.
- **081B1B90 — retained draft 84.5%, ActorStatus_UpdateFlankingStrike.**
  60/20 realigned differences. Assembly confirms the same +/-512 lateral
  offsets as the tracking callback, then orbit motion and sequence eight.
  Porting the improved pinned Offset inline and pinning entry pointers to r8
  changed spills/allocation and did not improve the retained candidate.
  Kept the original candidate with the required alias and working fallback.
- **081B0F34 — retained draft 81.9%, ActorStatus_UpdatePlayerCapture.**
  121/66 realigned differences. The ROM shares one player-position-request call
  between variants, with different capture and slot arguments. Rewriting to
  one C call reduced structural differences to 28 but scored only 72.7%, so
  retained the better existing guarded draft. Volatile state reads and raw
  component stores did not help. Request-branch merging, frame/entry pointer
  lifetime, and vector/constant ordering remain.
- **081B1E24 — retained/improved draft 98.1%,
  Vanargand_UpdatePartTracking (was 90.7%).** A temporary register-pinning
  concession in Offset fixes constant-before-load ordering and the shared z
  offset tail: r1 component pointer, r2 x offset, r3 z offset and r0 arithmetic
  value, with empty operand barriers only. No instruction bodies were added.
  Only three instructions differ: after MotionTrack_Update, the compiler
  materializes timer offset 0x168 in r1 instead of r2. Explicit timer/address
  pins, integer signedness, declaration order, alternate timer predicates and
  inline tick helpers failed to improve this residue. Retained the minimal
  improved Offset candidate; discarded the larger timer-pinning attempts.

All final C candidates were formatted directly with the venv clang-format and
checked using check.py --nonmatching --record. Every default assembly fallback
also passed check.py; those fallback matches earn no C credit. Six separate
draft commits and this note were submitted through build/commit.sh. Full batch
validation and name promotion remain with the integrator.

## Task 081B4040 — 2026-10-20

Tags: #matching #vanargand #animation #motion #tooling

- 081B52E8: matched, `Vanargand_UpdatePartBobbing`. On reset, selects
  sequence 9 and performs one packed animation step; every tick sets origin Y
  from the signed sine table at frame & 255, scaled by 128, and sets finished.
  Complete 364-byte extent ends at 081B5454. Register-pinning concession.
- 081B4C04: matched, `Vanargand_UpdatePartRecovery` (existing draft 91.8%).
  Sets flags 12 and 0x120, selects sequence 11 on reset, and after 180 ticks
  clears flag 0x100 and requests action 22, respecting the unfinished 25/26
  action guard. Complete 416-byte extent ends at 081B4DA4. Register-pinning
  concession. Setting the flag-offset register to r3, the flag pointer to r2,
  and assigning the r1 mask after pointer materialization recovered the last
  six-byte difference. depin.py reduced 18 hints to 14 before final acceptance.
- 081B431C: drafted 84.9%, `Vanargand_UpdatePartImpact`, improved from 80.6%.
  Sequence 8 precedes a short sideways motion and a travel to the impact
  coordinates. Paired counter/state checks select the strong burst and flash
  versus the ordinary burst; travel emits trails on even frames. Remaining:
  100 realigned differing instructions, 30 ignoring registers. Enter-value
  temporaries avoid premature address canonicalization; transition constants,
  vector argument order, pool placement and saved pointer registers remain.
- 081B48A0: drafted 87.6%, tentative `Vanargand_UpdatePartRetreat`. Starts
  sequence 11, moves 128 units along the angle byte at +187 while preserving
  origin height, then selects sequence 9 and sets finished. Remaining:
  63 realigned differences, 19 ignoring registers; vector pointer copies,
  transition ordering, saved owner/state and animation mask reuse differ.
- 081B4040: drafted 81.0%, tentative `Vanargand_EmitRadialPartEffects`.
  Selects sequence 10, emits six type-3 effects around the display position
  at radius 128 with delays i*2, waits for the externally updated flash timer,
  steps the animation, plays sound 0x58C, then deactivates the part. Remaining:
  92 realigned differences, 27 ignoring registers. Declaring reset-field
  pointer assignments after CpuSet reproduces more loop-hoisted addresses;
  stack spills, loop registers and transition instruction ordering remain.
- 081B4DA4: stopped; scratch candidate `wip/sub_081B4DA4.c` only. Tentative
  `Vanargand_UpdatePartLeap`: sequence 9, a variant-dependent planar leap with
  Y=768, then movement to terrain height and completion flag 0x80. The actual
  apparent body is 0x388 bytes, ending at 081B512C, including final padding.
  081B512C begins a separate apparent function (push {r4,r5,r6,lr}, own stack
  frame, reset and state dispatch) currently emitted as bytes under the same
  assembly block. The candidate scores 84.2% over the reviewed body, with
  82 realigned differences / 20 ignoring registers, but --record independently
  uses the inventory extent through 081B52E8 and records only 67.7%. A briefly
  queued draft was removed in a follow-up commit once this distinction was
  confirmed. No below-threshold production draft remains. Integrator should
  review the missing 081B512C entry/boundary before resuming. Vector stores,
  owner/enter lifetimes, terrain-height store width and transition ordering
  remain even over the shorter body. An unused vector initialization is
  observed in the assembly and retained through an inline vector setter.

All six callbacks occur in the part-action table at gUnk_086120A0, with named
Vanargand neighbors. Existing shared local layouts describe the sprite at +0,
sequence +48, motion +58, base position pointer +90, origin +9C, target +A4,
variant +15C, flags +160, state +164, timer +168 and enter +16F.
These are static observations; no runtime coverage claims were added.

Useful compiler details: the packed stepper's second sprite flag update must
use a distinct source expression (`((struct VanargandPart *)obj)->sprite`)
from the first update. This removes an unnecessary ip pointer alias and its
copies. Materializing the animation resource before the sequence address via
an empty operand barrier reproduces argument ordering. Final exact sources
were depinned, formatted and checked again. No instruction bodies were added.

Tool caveats: depin.py treats a raw-symbol NONMATCHING assembly fallback as
an exact default source and may strip every draft hint without checking the
NONMATCHING candidate. This happened on three drafts; their reviewed snapshots
were restored and their candidate scores rechecked. With a named fallback,
check.py --record can fail to update the fuzzy line when the defined symbol
uses a raw-address alias; changing the fallback to sub_081B431C fixed recording.
For 081B4DA4, --target-size changes the comparison but not draft_scores()'s
recorded score. Do not substitute the shorter score for the recorded one.

Final disposition: two exact matches (both register-pinned), three guarded
NONMATCHING drafts and one stopped scratch candidate. Each retained source
was checked with tools/check.py; draft scores were recorded on formatted C.
Only the commit driver was used. No make, full build, generated-file edits,
network operations, pushes or PRs. Batch validation and name promotion remain
with the integrator. Notes remain local to this worker; shared docs untouched.

## Task 080FDA44 — 2026-10-20

Tags: #matching #axe #wooden-crate #named-object #register-pinning

All six assigned functions matched: task completion 100% (+100 percentage
points), with no NONMATCHING drafts. Exact checks used reviewed full extents;
no make, tools/build.py, network, generated-file edits or direct git writes.
One source per matching commit, submitted through build/commit.sh. Names are
file-local aliases and tentative semantic interpretations, pending integration.

- 080FDA44: matched, `Axe_FollowDeceleratingRoute`, 1108 bytes. Consumes the
  pending-entry flag, follows nine context points at +E5C, then wraps to the
  last point and halves speed at +E5A. Speed ramps every 16 ticks up to 12.
  Computes random offsets around the player, using radius 512 on retarget.
  Calls 080FBA38 during movement. The actor motion is +64, distinct from the
  secondary motion at +2EC. Register-pinned: retains the observed unused
  0x0403B714 load in r4 through the first polar calculation, and pins the
  middle random-table base to r3. Empty operand barriers only; no handwritten
  instructions. depin.py retained all three necessary hints.
- 080FDE98: matched, `Axe_FollowAcceleratingRoute`, 1092 bytes. Same route
  skeleton as 080FDA44, but uses radius 256 on retarget, leaves speed intact
  at route wrap, and ramps to 18. Porting the matched sibling required only
  those changes. Same three register hints, verified with depin.py.
- 08100C18: matched, `NamedObject_EmitMovementTrail`, 784 bytes. Emits at
  alternating direction-dependent offsets from +10C, toggling +D2 and
  resetting +BC to four ticks. Kind +F3 selects default particles (0) or
  explicit resource/variant particles (3, resources F422/121B). Using a
  switch for kind and spelling additions as object coordinate plus offset
  reproduced the original addressing and branch order without hints.
- 080FF87C: matched, `WoodenCrate_CreateFromScript`, 636 bytes. Acquires the
  manager and a slot, loads BB68, initializes the sprite and parses p/R/r/n/
  C/D/s/k/t. Records initial flags at +CE, selects palette 1A1 or 1A0, chooses
  idle sequence 0 or 2, advances its packed command stream and creates the
  collider. Related named manager, flag and collider routines establish the
  WoodenCrate prefix. Register-pinned: separate sprite and animation views
  reproduce the saved sprite pointer and repeated flag accesses; explicit
  saved duration locals avoid alias-induced reloads. depin.py reduced 24
  hints to 16. Full extent ends at WoodenCrate_BeginToggleAnimation.
- 080FFC24: matched, `WoodenCrate_UpdateToggleAnimation`, 428 bytes. Updates
  palette timers, advances the packed animation and, on completion, selects
  idle sequence 0/2, sets crate flag 1/2, marks +D4, and calls script +B8 with
  the name and crate flags. The command reads are volatile halfwords, as in
  matched display-animation relatives. Register-pinned: first flag-test
  operand barriers and second flag mask/result pins preserve compiler operand
  ties. depin.py reduced five hints to four. Reviewed extent ends before the
  separate 080FFDD0 entry retained as bytes by the existing assembly inventory.
- 081010E0: matched, `NamedObject_UpdateVerticalTravel`, 524 bytes. Clears
  motion, consumes the sound-entry byte +BA, respects pause flag 4, and moves
  toward upper/lower limits at +D8/+D4 according to flag 2. On arrival, snaps
  an attached player, sets flag 100, state 4 and the entry byte, stops movement
  sounds and plays 159 unless sound flag 4 suppresses it. An inline delta
  helper preserves the otherwise-unused initial player-relative vector.
  Passing the player-global address first to that helper delays dereferencing
  it until the other argument addresses are materialized; this resolved the
  last load-order mismatch without pinning. A normal position-view cast on
  the copied player position preserves the observed halfword height store.

Useful technique across the task: model observed local layouts first, then
steer inline argument and commutative operand order. Literal-bearing RAM
random-table expressions must remain distinct on the first sample to avoid
GCSE keeping an extra table pointer live. On the Axe callbacks, returning a
masked u32 from the first random helper places the 255 mask before the byte
load and reproduces the original polar setup. All four hinted units were
run through depin.py; all six sources were clang-formatted and passed their
final tools/check.py checks. Combined ROM validation and name promotion are
left to the integrator, as required by the worker assignment.

## Task 08113738 — 2026-10-20

Tags: #matching #native-effect #particles #register-pinning

Worked offline on the already-claimed `cloud/08113738` branch. Each function
was checked using `build/venv/bin/python tools/check.py` and queued in its own
commit through `build/commit.sh`. No full builds, network operations or shared
generated-file changes. All six assigned functions now have C: three exact
matches and three guarded NONMATCHING drafts. Combined ROM validation and
name promotion remain the integrator's work.

- **08113F64 — matched, NativeEffect_UpdateTurningTrail (tentative).**
  Type-three profile update. Clears the entry flag, integrates fixed-point
  position and changes polar velocity over timer ranges 0–5, 6–19 and 20+.
  During the middle range the angle changes by twice the signed direction,
  and resets at tick 19; after tick 19 horizontal speed increases every four
  ticks. Emits randomized particles on odd ticks through tick 19. Projects
  the sprite, steps animation, updates the embedded collider, and switches
  to SpriteEffect_RemoveWithBurst on ground contact or lifetime expiry.
  Exact extent: 1036 bytes. Register-pinned category; depin retained 11 hints.
  The final six differences were position/height spill slots. A u16 angle
  parameter plus an explicit s16 angle local inside the inline polar helper
  fixed PRE ordering without adding instructions; actual callers supply the
  byte angle at +CA.
- **08113BBC — drafted 99.6%, NativeEffect_UpdateRotatingFlight (tentative).**
  Type-one update shares the same motion/animation/collider layout. Uses polar
  speed three before tick 20, then four; rotates every fourth tick initially,
  every second tick through tick 19, and every tick through tick 41. Ground
  test uses margin eight and schedules NativeEffect_DestroyEntry. Full-size
  checker leaves two register-only instruction differences: the final height
  address reload uses r3 instead of r0. Ten focused argument/pointer hint
  variants failed to improve it; preserving the simpler draft. The animation
  mask temporary must be r3 here, unlike the other family members.
- **08114928 — drafted 83.9%, NativeEffect_CreateFromProfile (tentative).**
  Confirms the 16-slot native-effect manager layout: entries begin at +18,
  stride D8, and the manager frame counter is +D98. Input profile byte +1
  selects five sprite initializers and corresponding motion states. Profile
  +2 supplies lifetime, +6 collider damage and +A the threshold selection.
  Copies two four-word vectors, sets height and sprite/activity fields,
  combines group with the low manager-frame byte, and returns the chosen
  index or -1. Types zero/two select 08113738; one selects 08113BBC; three
  selects 08113F64; four calls EffectActor_BeginHorizontalMotion, which selects
  08113920. Remaining 53 realigned differing instructions (23 with registers
  ignored): argument/spill allocation, group-id expression ordering, and
  initialization/call order. Narrowing the stored group id directly causes
  agbcc to select an undesired byte load; the retained explicit helper keeps
  the original word load but hoists an extra manager-counter address.
- **08113920 — drafted 90.5%, NativeEffect_UpdateAcceleratingTrail (tentative).**
  Called by EffectActor_BeginHorizontalMotion. Integrates position, projects
  and animates the sprite, updates the collider, then accelerates X velocity
  by direction * 512. Emits odd-tick particles through tick nine with each
  offset `(random & 7) - 3`. Ground test margin zero or lifetime expiry selects
  SpriteEffect_RemoveWithBurst. Remaining 32 realigned differing instructions,
  two with registers ignored: particle-offset operand order plus position and
  sprite-position spill order. Factoring the random load into a helper helped
  08113F64 but worsened this function; restored the best direct-load form.
- **08113738 — matched, NativeEffect_UpdateLinearFlight (tentative).**
  Shared type-zero/type-two fixed-velocity flight update; entry clears sprite
  bit zero, then integrates, projects, animates, places and inserts its collider.
  Ground margin eight or lifetime expiry selects NativeEffect_DestroyEntry.
  Exact extent: 488 bytes. Register-pinned category; depin retained 11 hints.
  The first packed flip comparison needs its result in r2 rather than the
  shared ActorAttachment_StepSprite helper's r1.
- **08114CB0 — matched, NativeParticle_UpdateAnimatedMotion.**
  Updates the small particle position using signed halfword velocities, copies
  its packed coordinates into a 40-byte sprite selected by signed index +13,
  and advances the parallel sequence at manager +520. Computes sprite tile
  from frame, width/height in tile units and resource tile base. Animation end
  sets state +12 to two; increments timer +14. Unlike the larger sprite
  stepper, it neither stores frameWeight nor clamps zero duration to one.
  Exact extent: 340 bytes. Register-pinned category; depin reduced 16 hints to
  13. Keeping the tile dimensions' product separate from the frame product
  fixed multiplication order; tail operand hints fixed pointer scratch regs.

Useful family technique: project through an inline output-pointer helper,
then copy the eight-byte packed result into sprite.position. Directly writing
fields of a local packed struct caused substantially different allocation;
the output-pointer form immediately brought the large updates to within a few
register differences. Existing actor_attachment_sprite_playback.h provided
the observed volatile packed-command decoder, specialized locally for each
function's register ties. No instruction bodies were handwritten.

All committed C was clang-formatted. Exact files passed final byte checks;
draft scores were recorded with `--nonmatching --record` against full extents.
Run depin on the unguarded candidate when inspecting a draft: depin has no
NONMATCHING option, so running it on a guarded source checks only its assembly
fallback and can incorrectly discard every hint. Guarded sources were restored
and their recorded C scores rechecked; unguarded draft candidates correctly
report that depin cannot minimize a non-exact base.
# Task 081F3C68 — Ratatosk attack and reaction callbacks (2026-10-20)

Tags: #matching #ratatosk #projectiles #sun-orb #nonmatching #register-pinning

Worked offline on the preclaimed cloud/081F3C68 branch. Exact code gain:
**+0.0417 percentage points** (1,008 reviewed bytes), comprising +0.0185 pp
without hints and +0.0232 pp in the Register-pinned category. Only fast
per-file checks were run; no make, complete build, network or shared-doc edits.
Commits were queued individually through build/commit.sh. The Ghidra export
is absent. Neighboring Ratatosk callbacks, the state tables and
Ratatosk_ApplyDamageReaction establish the family and reaction ownership.

- **081F48AC — matched, Ratatosk_UpdateSunOrbSummon**, reviewed extent 448
  bytes, no hints. Plays unmirrored animation phases at ticks 6 and 86,
  selects a point via Ratatosk_FindPointNearestReference, creates a SunOrb
  and raises its slot. Records point/target occupancy and the selected slot.
  The decisive edit was modeling the tables at +0xD18 and +0xDF0 as arrays
  of 24-byte records (active word, position, twelve unknown bytes). Raw
  pointer arithmetic scored 66.2%; the typed arrays matched immediately.
  Kept these observed layouts local to the translation unit.
- **081F467C — matched, Ratatosk_UpdateHomingShot** (tentative), reviewed
  extent 560 bytes. At tick 28, prepares a player-homing projectile, chooses
  a launch angle and packed spawn offset from cardinal facing, increments
  +0xEFA and plays song 0x56A. Finishes after tick 30. Explicit case zero
  preserves the first switch's range branches; operand order in the vector
  sum fixes the three component loads. Five final differing instructions
  were compare/callback-address registers. Minimal r1/r2/r0 bindings and
  two empty barriers close these; depin reduced eight hints to five.
  Register-pinning concession is marked. Cardinal facing is initialized
  by the normal state-entry path before the offset switch is reached.
- **081F4378 — drafted 91.7%, Ratatosk_UpdateProjectileVolley** (tentative).
  Sequences directional animations 17–20, clamps aim to a 32-unit cone,
  and launches three projectiles separated by 16 angle units. A hit event
  starts the +0xE74 cooldown; tick 120 forces completion. The event consumer
  returns s32, avoiding an extra byte truncation. Declaring the spawn pointer
  after the sound call improves allocation; writing the constant-first OR
  removes one copy. Remaining full-extent residue: 36 realigned differing
  instructions, 13 ignoring registers. The compiler duplicates the phase
  increment for the first three cases; explicit shared labels reduced that
  structure but worsened pointer allocation. Retained without register hints.
- **081F4CF8 — drafted 86.1%, Ratatosk_UpdateDefeatSequence** (tentative).
  Runs the first event script and kills a task for the ordinary defeat,
  clears animation fade, stops sounds/orbs, steers away from flagged terrain,
  changes animation at tick 140 and emits pairs of particles through tick
  150. At tick 440, runs the second event script, sets event flag 0x51 and
  updates four bounded persistent counters. Typed animation flags and
  separate home/current vector pointers fixed address selection. Declaring
  the spawn pointer inside the particle iteration and using u16 particle
  samples (shifted values fit their widths) raised 76.8% to 86.1%.
  Remaining: 84 differing instructions, 22 ignoring registers, chiefly RNG
  constant/register lifetimes, angle spill, stack pointer placement and the
  final flag mask. Pointer register pins were counterproductive and were
  discarded; no hints remain in the retained draft.
- **081F3C68 — stopped below draft threshold**, best ordinary candidate
  69.7%, Ratatosk_UpdateRadialSummonAttack (tentative). Above tick 56,
  state three creates homing projectiles with randomized angle/variant and
  one of four angle-step patterns; other states create radial player clones.
  Both clamp spawn positions to arena bounds and set completion. Candidate
  preserves the calls, counts and collider parameters, but actor-pointer
  spills, high-register vector pointers, polar constants and collider-load
  allocation remain (218 differing instructions, 37 ignoring registers).
  Tried separate actor aliases, a volatile owner, register constraints,
  branch-local/global spawn pointers, polar pointers and parameter array
  widths. No useful improvement; restored the best ordinary candidate to
  untracked wip/sub_081F3C68.c rather than adding a below-threshold draft.
- **081F5290 — stopped below draft threshold**, best experiment 67.4%,
  Ratatosk_UpdateKnockbackRecovery (tentative). Initializes animation 29,
  decays directional motion for sixteen ticks and scales by 24 per tick,
  emits alternating-tick particles and chooses defeat or idle after tick
  29 according to health/locked state. The starting candidate was 59.5%.
  Sprite-view typing, explicit scale pointers, expansion of the polar math,
  tick and particle register bindings did not reach the retention threshold.
  Main differences are the speed/tick register choices, scale-pointer reuse,
  RNG/table lifetime through particle initialization and callback-state zero
  sharing. A 64.9% tick-constrained candidate remains untracked at
  wip/sub_081F5290.c; the broader particle constraints were not retained.

Both guarded drafts were formatted directly with clang-format, then recorded
using check.py --nonmatching --record against full extents. Both exact matches
passed explicit --target-size checks after formatting; depin was run on the
hinted exact source. Each function has a local Ratatosk_VerbNoun alias.
Original INCLUDE_ASM remains the default for drafts and the two stopped
functions. Combined ROM validation and name promotion remain with integration.
The pre-existing local tools/cloud_task.py trial-runner edit was left alone.

# Task 081A14FC — Codex medium, 2026-10-20

Tags: #matching #hresvelgr #vanargand #effect-pool #nonmatching #register-pinning

Worked solo, offline on the preclaimed cloud/081A14FC branch. Exact code gain:
**+0.0000 percentage points**. Four inherited guarded drafts retained, two
improved substantially. Only per-function check.py validation; no make, full
build, network, shared documentation/index edits or generated-assembly commits.

- **081A5614 — retained draft 80.7%, Vanargand_UpdatePaletteTransition.**
  Read BIG_FUNCTIONS, condensed assembly and the matched palette-restoration
  relative 081ACC24. Baseline remains 299 realigned differences / 82 ignoring
  registers, candidate 2848 versus reviewed 2800-byte target. A shared-result
  interpolation form with frame-first multiplication regressed to 67.2%
  (545 / 249); restored the inherited interpolation. Blend argument address
  scheduling, interpolation register/spill placement and shared tails remain.
  Added the exact local alias for the already established symbol name.
- **081A14FC — stopped unmatched after complete static assembly/relative
  review; no production draft.** Tentative Hresvelgr_UpdateDefeatSequence.
  Five states gate the first event script on animation phase/player action,
  accelerate eight movement stages, emit randomized effects, bounce against
  terrain, set event flags 0/3/0x4E, run the second event script and complete.
  The matched movement/terrain relative covers only 7% of the body. Repeated
  precomputation of damage/flag field addresses across switch edges remains
  the known source-shape obstacle; no new concrete solution emerged. Did not
  recreate the earlier below-threshold candidate or claim draft credit.
- **081A1EFC — stopped below draft threshold, best complete candidate 47.0%.**
  Tentative Hresvelgr_ApplyPendingDamage. Consumes damage and effect requests,
  selects the active collider hit flags, suppresses optional effects for bit
  0x40, applies periodic environmental damage from player +0x876 times five,
  flashes collision presentation and emits damage numbers. Action 16 uses
  accumulated damage/flags to choose reaction or action 33; action 33 permits
  three repeated reactions. Other actions test the signed threshold at
  +0x374, optionally emit sparks/two randomized motion effects, then request
  reaction or play the hit sound. Candidate 1092 versus reviewed 1148 bytes,
  388 realigned differences / 178 ignoring registers. Corrected the threshold
  layout and signed modulo type while drafting. Packed stack-vector writes,
  owner aliases, shared flag tails and RNG allocation remain. Explicit owner
  constraints and narrower bitfield variants regressed (46.2–46.3%); restored
  the ordinary 47.0% candidate at ignored wip/sub_081A1EFC.c. No production C.
- **081A39E0 — improved draft to 96.2%, EffectPool_UpdateEntry.** Revalidated
  inherited C at 91.1% despite its older 88.3% comment. Keeping the homing
  interval's loaded value separate from the r1 store temporary, comparing
  the loaded value, constraining animation/trail loads, making the collider
  zero explicit and returning the frame through a u32 accessor reduced
  37 / 4 realigned differences to **15 / 0**. Candidate and target 800 bytes.
  Remaining register choices: commutative sine-table address addition, the
  second animation mask tie, the timer narrowing literal and trail-count/
  frame/RNG pointer setup. Mask-copy and pointer-constraint variants regressed.
  A bounded permuter attempt could not start because its split dependency
  rejects the unrelated Ghoul_ResumePendingRoute start; no search result used.
- **081A2858 — improved draft 91.3% to 98.3%,
  Hresvelgr_UpdateCollisionPresentation.** Compute reset-field pointers in
  each collider branch, derive sequence-data pointers inside the inline from
  an offset, and type the render-position vector. Crucially corrected both
  sway writes from the inherited +0x376 field to render-position Y at +0x1B6,
  as observed in assembly. This is a behavioral correction as well as an
  addressing improvement. Candidate and reviewed target remain 868 bytes;
  residue is **10 realigned differences / 8 ignoring registers**, comprising
  loop-index initialization scheduling and camera flag mask/load allocation.
  Four additional mask/index/accessor variants regressed; retained best.
- **081A2430 — retained draft 87.5%, Hresvelgr_DispatchSceneMessages.**
  Reviewed the pending-trigger loop and action/animation consumers. Reordered
  pending-array arithmetic reduced structural count from 25 to 24 but worsened
  total from 51 to 54 and score to 86.6%; restored original. Candidate/target
  680 bytes, 51 / 25 remaining differences in trigger indexing and shared
  zero/one action constants. Added local aliases, including the renamed
  rising-projectile callback needed by the cached ELF.

All four retained units have required file-local aliases and were formatted
with build/venv/bin/clang-format. Scores recorded after formatting using
check.py --nonmatching --record against reviewed extents. All four default
assembly fallbacks checked byte-identically; EffectPool_UpdateEntry retains
its existing named INCLUDE_ASM because only that generated filename exists.
The other three use the existing numeric assembly filenames. depin was run
on unguarded scratch copies and correctly declined to minimize non-exact C;
concession markers remain. One driver commit queued per draft, with this
notes commit last. Combined ROM validation and name promotion are deferred
to integration.


## Task 081A9910 — 2026-10-20

Tags: #matching #vanargand #paired-scene-actor #register-pinning

Worked solo and offline on the preclaimed cloud/081A9910 branch. Inspected the
largest targets first, read BIG_FUNCTIONS.md and prior attempts, and used only
per-file checks. Exact task instruction coverage gained **13.9 percentage
points** (465 of the assigned 3335 instructions); project-wide accounting is
left to batch integration. Exact sources total 1172 bytes, both in the
Register-pinned category.

- **081ABA64 — stopped unmatched.** Re-read opening assembly, the condensed
  eleven-phase listing and relatives. The 37% relative coverage is chiefly
  packed-animation expansion; it does not supply the track/contact phase
  machine. Existing notes identify the local far-branch labels correctly.
  No complete close candidate or new structural hypothesis emerged; left
  assembly in use.
- **081AA788 — stopped below threshold.** Resumed the scratch sweeping-projectile
  attack at 75.4% (225 realigned differences, 99 ignoring registers). Pinning
  the scale helper's second input in r8 and independently forming each
  auxiliary-animation address from owner+0x444 reduced structural differences
  to 88, total still 225; latest score 75.6%. Explicit projectile RNG table/mask
  bindings and a barrier separating angle subtraction regressed, so discarded.
  Remaining: stack slots, timer-pointer lifetime, randomized-projectile argument
  allocation and phase-reset tails. Latest full candidate remains only in
  ignored wip/sub_081AA788.c; no production draft below 80%.
- **081A9910 — matched, Vanargand_UpdateSynchronizedPairAttack.** New readable
  C matches the full reviewed 676-byte extent. Commands 3/4/5 run both halves
  through repeated synchronized attacks, with positive-counter checks selecting
  motion parameters at +0x588 or +0x586. Two halves in state 5 set +0xF3C to 60;
  after three repetitions, selected halves receive recovery command 6, and
  motion parameter +0x584 is restored. Local mask variables reproduce constant
  load order. Explicit next/zero locals recover shared transition tails. Fixed
  r4/r5 status locals solve the pair-status register swap. depin removed the
  unnecessary owner constraint; two bindings remain. Name is tentative.
- **081A9BB4 — drafted 98.5%, Vanargand_UpdateFacingPairAttack.** New full
  552-byte candidate starts pair command 7, randomly chooses command 11 (one
  in four) or 8, and uses facing plus the active-half mask to select a half.
  Stores its index at +0x5B6; completion restores parameter +0x584. A u8 facing
  predicate reproduces the materialized boolean. Compute random bits before
  setting the default command to reproduce the conditional layout. Four
  differing instructions, zero ignoring registers: RNG table literal uses r1
  instead of r2; pair-slot offset uses r2 instead of r0. Scoped constraints,
  address-expression variants and declaration swaps did not improve the clean
  candidate. Recorded score and verified default INCLUDE_ASM fallback exact.
- **081A9F88 — matched, Vanargand_UpdateChargedPairAttack.** Improved the
  retained 92.8% draft to exact across its reviewed 496-byte extent. An owner
  constraint after the halves-finished call prevents PRE retaining pair-slot
  address in r4 across that call; scoped r4/r5 status bindings recover both
  loads and comparisons. A zero constraint gives the separate phase-zero reset.
  depin reduced four hints to three. Removed guard, formatted and rechecked.
- **081AA178 — retained draft 99.5%, Vanargand_UpdateRepeatedPairAttack.**
  Added the required file-local alias and raw-address fallback so the fast
  checker resolves it against this checkout's ELF. One byte/instruction still
  differs: phase-zero timer reset uses r7 rather than r5, both zero. Phase
  output constraints, fixed phase/zero bindings and a fixed timer pointer
  regressed allocation; restored the best source. Score re-recorded against
  the full 440-byte extent; default fallback checked exact.

Per-function sources were committed through build/commit.sh. No full build,
network action, generated assembly, ROM or emulator output was committed.
Combined ROM verification and name promotion remain for the integrator.


## Task 081DB794 — 2026-10-20

Tags: #matching #options #menu #scene-actors #register-pinning

Worked solo and offline on the preclaimed branch. Exact task instruction
coverage gained **33.0 percentage points** (1030 of the assigned 3122 instructions);
project totals remain for integration. One new exact function, 2612 bytes,
in the Register-pinned category. Resumed existing guarded candidates, largest
first, using BIG_FUNCTIONS, anchored comparisons and hypothesis-driven variant
batches. No full build, make, network or direct git mutations.

- **081EAB64 — matched, Params3D0_UpdateDisplay**, from inherited 91.0%.
  Creator 081EB7DC installs this menu callback and TimeDisplay callees establish
  the display family. Expanding the two palette-helper call sites into their
  case bodies preserves the shift-field address across interpolation and its
  following threshold check; 125 / 31 realigned differences became 61 / 19.
  Constraining palette sources to r2 and temporarily constraining slide Y and
  reset values recovered the remaining allocation. Two inherited field errors
  were corrected against assembly: case 6 increments idleTimer at +0x73C, and
  case 13 compares counter at +0x734 against the fade duration, rather than
  timer at +0x754. Case 13 still increments counter. depin reduced twelve hints
  to two source-pointer bindings, removing all empty barriers and the temporary
  slide/reset pins. Formatted C passes the explicit full **2612-byte** check.
- **081EE518 — drafted 91.4%, SceneActors_UpdateHitSensitiveDeparture**,
  from 90.5%. Installer 081EF29C and hit consumer 081F029C establish a departure
  action whose hit callback halves its speed. Replaced the ordinal name with
  this tentative behavior name, through a file-local alias only. Computing
  the second drawable after Anim_SetSequence, using the low context for its
  state pointer, and sharing entry zero reduce 75 / 20 differences to 66 / 11.
  Residue includes the pinned r7 base's missing prologue save, second decoder
  command/mask allocation and phase-transition address derivation. Unpinning
  the outer locals fixes save structure but drops to 86.1% (99 / 9); retained
  the stronger guarded candidate. Empty r7 clobber and late base barriers did
  not fix its save. This draft is not ABI-validated executable C.
- **081EE130 — drafted 99.4%, SceneActors_UpdatePeriodicEffectAction**,
  from inherited 90.4%. Moving RNG masking outside the table-reader inline,
  constructing the initial mask after its destination, and retaining the entry
  pointer remove scheduling differences. Keeping the first decoder mask in r9
  avoids an extra copy; simultaneous fixed flip operands recover both reverse
  flag updates. Remaining **3 realigned differences / 0 ignoring registers**:
  the +0x598 reset's address constant is in r3 instead of r1. Offset/pointer
  bindings, a clobber and natural decoder temporaries did not improve it.
  Candidate matches the reviewed 1000-byte length; assembly stays active.
- **081EEAD8 — drafted 94.4%, SceneActors_UpdatePaletteDispersal**, from
  87.0%. Sharing the entry zero in r3 and setting the palette-loop index before
  its source/destination pointers reduce 63 / 27 differences to 29 / 18.
  Remaining palette destination setup is too early, and the particle-loop
  counter/stack-vector setup order differs. Source-pointer barriers and inline
  argument reordering regress substantially; retained the stronger candidate.
- **081EDB74 — drafted 96.1%, SceneActors_UpdateLeftDeparture**, from
  inherited 85.0% in this checkout. Reused the matched 081ED5F4 drawable layout,
  initialization and playback decoder, removing separate duplicated decoders
  and the hard r7 base binding. Preparing the destroy callback before its field
  address improves the departure tail. Remaining **16 / 8** differences include
  time spilled to the stack, context allocation, decoder literal scheduling and
  callback setup. A time pin and context constraints did not improve it.
- **081DB794 — drafted 85.1%, Options_InitScreen**, from 82.4%. Binding the
  relocated graphics pointer to r4 reduces 53 / 22 differences to 43 / 22.
  Sprite/resource lifetimes, text-rectangle setup and a redundant palette-source
  reload remain. Distinct sprite variables, resource-barrier removal, bitfield
  rectangles and explicit palette-source caching did not improve the retained
  candidate.

All five drafts retain NONMATCHING guards, raw-address INCLUDE_ASM fallbacks,
required descriptive aliases and concession markers. Formatted and recorded
with check.py --nonmatching --record; default assembly alternatives also check
exact. depin was run on unguarded scratch copies and correctly skipped the
non-exact candidates, so no draft hint-minimality claim is made. Per-function
commits and this notes commit use build/commit.sh. Combined ROM validation and
name promotion remain with the integrator; assembly fallback checks earn no
exact coverage.

# Codex medium: task 081B03C0 (2026-10-20)

Tags: #matching #actor-status #vanargand #register-pinning

Worked solo on the already checked-out `cloud/081B03C0`, offline. All six
functions had committed NONMATCHING starting points. Retained their names from
symbols/functions.csv and added the required file-local aliases. The caller
PairedSceneActor_UpdateHalves (081B5D8C) dispatches these through the part-state
callback table at 086120A0. Names remain tentative; these are paired scene-part
motion states with player-position requests, not evidence of a universal actor
layout. No shared headers or symbols were changed.

Exact code progress: **0.0% of this task (+0.0 percentage points)**. All six
results remain guarded drafts. Reviewed extents include each function's body,
literal pools and alignment: 081B03C0 1272, 081B08B8 1660, 081B0F34 600,
081B1334 1428, 081B18C8 712, 081B1B90 660 bytes. No new exact or Register-pinned
exact bytes are claimed.

- **081B08B8 drafted 99.8%**, up from 91.3%. Realigned 2 differing instructions,
  0 ignoring registers; candidate 1660 bytes, just two bytes differ. ROM uses
  r1 for the zero-offset scratch of the signed player X load at 081B0B20;
  candidate uses r3. One function-scope enter pointer recovers the shared
  completion tail. Set transition values before forming destination pointers;
  retain the state-one zero for the player-position call. Widen SetVector's
  scalar arguments to s32 to avoid opaque-zero narrowing. Reversing X/Z add
  operands matches the inlined vector sum. Explicit resource-before-options
  and timer-offset constraints recover instruction ordering. Declaration
  swaps, player/distance pins, opaque indexing and extra zero lifetimes did not
  solve the final scratch register. The guarded version is retained.
- **081B1334 drafted 96.1%**, up from 86.0%. Realigned 32 / 3 ignoring registers;
  candidate 1424 bytes. Ported the related approach's entry/value lifetimes.
  Forming the switch state address with an opaque offset avoids retaining the
  state pointer in an extra high register, reducing 60 differences to 32.
  Remaining differences concern reset/transition pointer registers, guarded
  action-transition address reuse, and final entry/finished stores. Making
  only the state value opaque or volatile did not fix the hoisting.
- **081B03C0 drafted 88.6%**, up from 86.2%. Realigned 97 / 55 ignoring registers;
  candidate 1308 bytes. Pinning frameWeight's loaded value to r1 improves the
  existing packed playback without changing behavior. Replacing the playback
  wholesale with the matched release sibling or pinning its owner base made
  allocation worse; those attempts were discarded. Request-call setup,
  phase-entry tails and packed-playback allocation remain unresolved.
- **081B18C8 drafted 87.0%**, up from 80.3%. Realigned 50 / 17 ignoring registers;
  candidate 724 bytes. Explicit entry reads before deriving the motion pointer,
  shared zero values, function-scope entry pointer, and an isolated switch-state
  address improve the shape. Case one reuses currentState (one) in its entry
  store. The ROM derives 512 from the live 0x16F offset; our vector initialization
  still materializes it independently. Entry-pointer allocation, vector-copy
  ordering and transition constants remain.
- **081B1B90 drafted 90.4%**, up from 84.5%. Realigned 35 / 11 ignoring registers;
  candidate 656 bytes. A shared entry pointer in r8 preserves the state in r7;
  the playback helper must use its dynamic `one` parameter directly rather
  than first copying it to r3. Share the entry-clear zero with animation
  options. Remaining differences: flank-offset operands, entry-pointer copies
  and transition pointer/value allocation.
- **081B0F34 drafted 92.8%**, up from 81.9%. Realigned 62 / 50 ignoring registers;
  candidate 612 bytes. An isolated switch state in r4 shares its zero across
  the request's stack arguments and its one with the finished store. Materialize
  the entry load before saving its pointer; share transition one. The ROM
  cross-jumps the two player request calls into one tail, while the retained
  draft duplicates them. An explicit common call reduced structural differences
  but worsened the total (68) and fuzzy score (84.0%), so it was discarded.

All sources were clang-formatted and checked using only tools/check.py.
Scores were refreshed with --nonmatching --record and the reviewed target sizes.
The initial named INCLUDE_ASM fallbacks could not assemble against this
checkout's address-named generated files. Changed each fallback to its raw
sub_XXXXXXXX identifier; all six default assembly paths then checked exactly.
Those assembly fallback matches grant no C progress. Per-function draft and
fallback-fix commits, followed by this note, use build/commit.sh. No make,
full build, generated-file edits, network actions, or direct Git mutation.
