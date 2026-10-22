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

## 2026-10-20 — task 0807C72C, scene actor attachment responses

Tags: #matching #scene-actors #attachments #nonmatching

No exact matches; exact code progress gain is 0 percentage points. Three guarded
NONMATCHING drafts were retained; default builds still use INCLUDE_ASM. No full
build, network operation, name promotion, or shared-header edit was performed.

- **0807C72C — drafted 86.9%, SceneActorAttachment_ReplaceResponseSlot.**
  Replaces attachment kinds 0–3, or releases the requested kind for event 127.
  Kind 0 creates the sprite slot, plays a category sound and may add a tagged
  stimulus; kind 1 creates the value sprite if the manager/slot query allows it;
  kind 2 creates a single part; kind 3 creates counted parts. Read constructors,
  existing callers and SCENE_ACTOR_ATTACHMENT_RESPONSES.md. The final release
  result is the argument to ActorAttachment_IsUnavailable; its actual s32 return
  is narrowed to u8 at this call. Corrected that ABI explicitly in a follow-up
  commit (the instructions and score are unchanged). The remaining reviewed
  residue is 49 realigned instructions, 21 ignoring registers. A signed r5
  kind local helps; pinning all arguments or factoring the entire switch hurts.
  The register-pinning concession is marked. Non-release callers require
  supported category/kind combinations because SelectOffsets intentionally
  leaves unsupported outputs untouched; the earlier caller-proof gap documented
  in the subsystem notes is still open. No invented fallback offsets were added.
- **0807CA44 — drafted 85.5%, SceneActorAttachment_BeginReturnResponse.**
  Sets destination5D4 from route450 and terrain height, tests the contact-node
  object's position, and on failure tries the stored route point before
  selecting callback228/state0/behavior2. First-target success starts response14
  and the embedded behavior7 command with duration30. The contact-node position
  needs two pointer dereferences, not one. A pointer-taking SetCoordinates inline
  emits halfword stack stores; direct local assignments emitted word merges.
  ArmCommand and constant-first flag inlines improved the draft. Group pointer
  pinning to sb is marked as a concession. Remaining reviewed residue: 63
  realigned instructions, 45 ignoring registers; pointer lifetimes, address and
  zero-constant sharing still differ. No shared layout changes were needed.
- **0807CDC4 — drafted 81.3%, SceneActorAttachment_FindReachableNeighbor.**
  Accepts the input if terrain attributes maskC3 is clear and F204 accepts it.
  Otherwise ranks right/down/left/up cell centers by signed, halfword-truncated
  X/Z squared distance, stably sorts them, and returns the first accepted center.
  Exhaustion writes X/Z=30000 and Y=0 without overwriting the fourth halfword.
  Reused scene_actor_terrain.h inlines; neighbor centers use the low height
  nibble without slope adjustment. The sum uses unsigned arithmetic to preserve
  wrapping. Pinning the delta pointer to r8 helped; pinning the center to sl
  hurt. Concession marked. Remaining reviewed residue: 104 realigned
  instructions, 42 ignoring registers, chiefly vector address copies, cell
  spilling and final loop pointer comparison. Requires a valid map stride and
  backing storage for the queried neighbor cells.
- **0807D2E0 — stopped below threshold, 70.4%.** Scratch candidate is
  wip/codex-medium/sub_0807D2E0.c (untracked). Five consumed arguments, not six:
  the positive terminal selector is the fifth stack operand. Modes0/1 position
  the actor and mirror its drawable; mode2 either removes all health or subtracts
  half, with multiplayer HUD/damage-display gating. Prefix emits three packed
  effects when flags2D4 bit2000 is set. The candidate uses the exact sibling
  MayCount/ReportDrain helpers from Ivy_UpdateHold. Remaining: 392 realigned
  instructions, 222 structural. Attempts at reset factoring, mask inlines,
  random-value inlines, and actor/group bindings did not improve the best pass.
  Main issues are the random cursor's deferred stores, duplicated actor pointer,
  reset-block addresses, and six particle calls' shared constants/table spills.
  The original callback234 (not savedTarget22C) is used by the terminal path.
- **0807DB34 — stopped below threshold, 62.0%.** Scratch candidate is
  wip/codex-medium/sub_0807DB34.c (untracked). Releases an owned contact node,
  starts response14, then resolves a route/target cell or directly tries the
  stored destination. The substantial same-cell/reroute sequence is already
  represented by Chase_Arrive in scene_actor_chase.h; this is a better starting
  point than reproducing it from raw casts. Reusing it improved the initial
  45.7% pass. Remaining: 420 realigned instructions, 189 structural. Actor/flags
  and group bindings regressed. The fallback callback currently lays out early
  rather than at the ROM's common tail; state constants and nested turn-response
  pointer lifetimes also differ. Leave its assembly path intact.

All retained drafts passed compilation and full reviewed-extent scoring with
`build/venv/bin/python tools/check.py FILE --nonmatching --record`. Their local
SceneActorAttachment_VerbNoun aliases are provisional descriptions of the
observed effects. Raw candidate-length scores were lower than full-extent scores
when the candidate was shorter; use the reviewed figures above when resuming.
`git diff --check` was clean. Commits were queued through build/commit.sh.
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

Tooling detail: --record initially left the old fuzzy comments unchanged because
it keys scores by the #else INCLUDE_ASM identifier, which was the descriptive
alias while the checked function resolved to sub_XXXXXXXX. After changing the
fallbacks to raw identifiers, reran --nonmatching --record on all six and verified
the source comments contain the new scores. Score-only follow-up commits retain
one translation unit each.

## Task 081F7FD0 — 2026-10-20

Tags: #matching #ratatosk #orbit-effect #combat

Two exact matches and two guarded drafts. Worked on the already checked-out
cloud/081F7FD0 branch; only fast per-file checks, no full build or network.
Individual source commits and this note use build/commit.sh.

- **081FAE0C stopped, not drafted — Ratatosk_InitializeFromScript.** Inspected
  the largest function first with asmat, condense, relatives and the caller
  Ratatosk_Create at 081FB788. The existing medium-worker candidate in
  wip/sub_081FAE0C.c still scores 66.2%, realigned 392 instructions / 122
  ignoring registers (build/check/run-0x2knyxo). Script options, collider setup,
  palette banks and child pools are recovered, but address reuse, stack-pointer
  lifetimes and branch tails remain below the draft threshold. Ghidra export is
  absent in this checkout. No production source added for this target.
- **081F7FD0 stopped, not drafted — Ratatosk_UpdatePlayerCapture (tentative).**
  New candidate in wip/task081F7FD0/sub_081F7FD0.c: 75.2%, realigned 173 / 30
  structural differences (build/check/run-dkbci2q1). Captures the player at the
  midpoint between actor and player, chooses a cardinal-offset destination,
  watches the player's angular change, emits particles while held, then applies
  interpolated player movement. Uses animation 9/5/7 and fallback repositioning
  with animation 4. Explicit stack aggregate, parameter/view register keywords,
  particle-pointer locals, returning the polar helper's output pointer, and a
  pinned actor pointer did not improve the baseline. The actor pointer remains
  r6 instead of r7; vector/config stack slots are displaced by four bytes, and
  address-constant reuse and PRE pointer copies remain. Best candidate restored.
- **081F8600 drafted 83.3% — Ratatosk_UpdateSunOrbPlacement.** Chooses a point
  nearest the player, builds a 32-byte rising-actor config, sets source mode 2
  and motion parameters 360/4/0/2, creates and raises the orb, and updates both
  the point occupancy and orb occupancy arrays (24-byte stride). Reuse mode
  fades all orbs; the final wait has a 600-tick timeout. Small halfword-store
  inlines and explicit animation locals improved the initial 77.3% candidate.
  Final reviewed check: realigned 70 / 24 structural differences,
  build/check/run-yznfzmof. Constant-offset reuse, slot-array address ordering
  and registers remain. No register pinning in this draft.
- **081F8988 matched — Ratatosk_UpdateSunOrbRelocation.** Coordinates a selected
  orb, waits for slot state >60, creates a replacement orb at the farthest
  player-offset point, releases the old slot through the manager, copies the
  replacement's position to the actor, then moves toward the nearest player
  offset. A shared explicit failure label in case 2 reproduces cross-jumping
  from case 0; computing the scaled occupancy offset before its base eliminates
  another ordering difference. Two narrowly scoped sequence-store views fixed
  the final eight register-only instructions. depin.py reduced eight hints to
  three; formatted source rechecked exact, 740 bytes,
  build/check/run-0j6vxpxj. Marked register-pinning concession.
- **081FDDC0 matched — OrbitEffect_SpawnOrbitTrail.** Every fourth update,
  initializes one of twelve 56-byte trail sprites, chooses radius 768..1279
  and an 8-bit angle, selects kind-dependent tile frame/palette, and positions
  it around the actor using signed sine-table products truncated toward zero.
  The neighbouring OrbitEffect_SpawnHomingTrail provides the object layout;
  OrbitEffect_UpdateTrailSprite confirms angle/radius and target-vector fields.
  Preserving the full timer mask's copy, the next-trail field-address/value
  views, and the sine-table pointer transfer reproduced the remaining register
  allocation. depin.py reduced sixteen hints to eleven. Formatted source
  rechecked exact, 356 bytes, build/check/run-34bsumsj. Marked register-pinning
  concession. No handwritten instruction bodies.
- **082015D4 drafted 82.4% — ScriptCombatActor_ApplyHitReaction (tentative).**
  Resumed the existing medium-worker scratch candidate after reviewing assembly
  and Combat_ApplyHitReaction. Stores damage/type/reaction, drives the HUD and
  damage display for flag 0x40, and emits three randomized sparks for selected
  flags. The useful candidate has barriers on pre-narrowing X/Z bases and a
  pinned RNG table pointer; the unhinted depin scratch was only 70.0% here.
  Explicitly delaying the first two global RNG cursor stores reduced structural
  differences but worsened total similarity, so restored the better candidate.
  Final reviewed check: realigned 49 / 29 structural differences,
  build/check/run-ffzj4ehe. depin.py correctly skipped the unguarded nonmatch.

Both drafts were checked with --nonmatching --record, remain above 80%, and
have byte-identical assembly fallbacks checked separately. Fallback matches
are not C progress. No ROM, generated assembly or emulator output committed.
Batch integration, name promotion and complete ROM validation remain with the
integrator.

## Task 080AEB74 — Vorn route callbacks (2026-10-20)

Tags: #matching #vorn #navigation #register-allocation

- **080AFD88 matched — Vorn_UpdateDescriptorPursuit (tentative).** Calls the
  actor's update270 and movement callback at 0x260, sets animation duration
  scale, consumes the navigation descriptor's probe event, and either accelerates
  toward it or requests a turn. Acceleration completion enters state 3 and sets
  context flag 2 at 0x6D4; turn completion selects behavior 16. Exact extent is
  360 bytes. A u32-returning motion flag helper avoids narrowing-related register
  choices. Naming follows neighbouring Vorn callbacks and the named navigation
  callees. Scoped copies and explicit state/zero locals reproduce the code.
  depin.py reduced four hints to one empty read/write constraint on the context
  flag mask; no fixed registers remain. Marked matching concession. Formatted
  final exact check: build/check/run-gq8z7r9g.
- **080AFEF0 matched — Vorn_SelectPursuitAfterAction (tentative).** On entry,
  installs saved entry control 0x224 with kind 2. On action completion, selects
  behavior 15 and uses context flag 1 at 0x6D4 to choose callback 080ADB90/mode
  0x23 or Vorn_UpdatePursuitDistance/mode 0x25. Exact extent is 328 bytes, without
  hints. Initial candidate was 92.0%; declaring callback, zero, and mode locals
  in that order in the small inlined selector reproduced the two branches'
  differing zero-register lifetimes. Formatted exact check:
  build/check/run-ny_rkhv1.
- **080AEE08 retained draft 83.4% — Vorn_UpdateDefeat.** Reviewed the existing
  source against the full 984-byte extent. Remaining differences are 84
  realigned instructions, 9 ignoring registers: flags offset reuse, the first
  RNG table pointer copy, late control/state constant reloads, and register
  choices in the script argument and final spark blocks. Moving table scope,
  changing the first RNG helper, and pinning initial field offsets did not
  improve it; restored the existing body. Added the required local address alias
  and a compatibility alias for SceneActor_CallContextScriptB (080B0D28), which
  is absent under its descriptive name in this worktree's ELF. Re-recorded
  score: build/check/run-addo587z. Default assembly fallback checked exact.
- **080AEB74 retained draft 80.2% — Vorn_TryRestorePatrolRoute.** Reviewed the
  existing source against the 660-byte ROM extent. Remaining differences are
  67 realigned instructions, 32 ignoring registers: destination pointer spill
  across terrain lookup, early saved-motion spill, flag pointer reuse, and reset
  scheduling. Moving the saved-motion assignment into the boolean guard, with
  and without volatile, worsened the result; restored the original body. Added
  the required local alias, re-recorded score (build/check/run-lrp2lu9l), and
  verified the default assembly fallback exactly. INCLUDE_ASM must use the raw
  sub_ address token here: global.h stringifies it without macro expansion, and
  this checkout's generated assembly filenames still use raw names.
- **080AF2BC stopped below draft threshold — tentative delayed route recovery.**
  Started largest first. Recovered the delayed action queue and the shared
  Chase_Arrive/Chase_TryRoute paths from checked-in scene_actor_chase.h, confirmed
  terrain height sampling, player-cell comparison, random facing selection, and
  patrol fallback against the full assembly. Reviewed extent includes interior
  label 080AFBD0 and ends at 080AFBE4 (2344 bytes). Best retained scratch check
  was 66.1%, 472 realigned differences / 167 ignoring registers
  (build/check/run-sjepbc7p). Main actor/group stack residency, derived address
  reuse, and RNG loop hoisting differ. Correcting the signed timer comparison,
  declaring sub_0812F3CC as u8, passing group to the queue inline, and a volatile
  root-pointer experiment did not bring it to draft quality. Scratch remains
  uncommitted in wip/sub_080AF2BC.c.
- **080B0038 stopped below draft threshold — tentative route movement restore.**
  Recovered context route cleanup, movement resolution, clearing pursuit flags,
  descriptor clearing, and restoration of default control and callback state.
  First clean candidate reached 73.9%, 63 realigned differences / 26 ignoring
  registers (build/check/run-kgozlpt7). Keeping the zero-valued behavior result
  alive, forcing early group computation, changing state-store addressing, and
  pinning a flags pointer failed to improve a valid candidate to 80%; discarded
  the pinning experiments. Best clean source remains uncommitted in
  wip/sub_080B0038.c. Main residues are group-address lifetime, flag address
  reuse, and control/state reset constant derivation.

All checks used tools/check.py through build/venv/bin/python. No full build,
network operations, ROM/generated assembly commits, or integration changes.
The two refreshed drafts retain their existing C and fuzzy scores; they are
not newly matched progress. Both exact callbacks and each draft were committed
through build/commit.sh. Separate cleanup commits minimize the first exact
callback's hints/formatting and correct draft fallback names for the local
assembly snapshot. Combined ROM validation remains with the integrator.

## Task 0817FDE0 — 2026-10-20

Tags: #matching #boss-slot-2 #armor #actor-controller #nonmatching

Worked solo on the assigned offline cloud/0817FDE0 branch. Exact C progress:
0% of this assignment, +0.0 percentage points. Retained four guarded drafts
(one newly promoted, two improved, one refreshed); the other two targets
stopped below the draft threshold. Only per-file check.py checks and the
commit driver were used; no full build, network operation or shared-doc edit.
Read the entry-point/worker/cloud/large-function instructions, earlier notes,
assembly, initializer caller, Armor proximity/position callees and matched
relatives. The Ghidra export is absent.

- **0817FDE0 — stopped below threshold.** BossSlot2_InitializeFromScript
  (tentative); called by BossSlot2_CreateFromScript. Revisited the retained
  initializer, separating the early actor fields and writing the player-list
  initialization as incrementing source/destination pointers. This reduced
  this session's realigned residue from 388/125 to 336/90 differing instructions
  (total/ignoring registers), but the candidate-length fuzzy was only 53.2%.
  It does not supersede the earlier exploratory 57.3% result. Major remaining
  issues: common actor versus boss pointer lifetimes, palette-copy loop
  allocation, hoisted component addresses and stack slots. Scratch remains
  in wip/sub_0817FDE0.c; no source draft was promoted.
- **081875EC — improved draft 85.8%, Armor_UpdateAlignedAttackDecision.**
  Previously 83.9%. The flag-selected decision switches have the same
  instruction shape as 08187230. Keeping the flag-bit result in r6 with an
  empty register constraint, and reusing its zero value in the normal-path
  reset stores, reduced total realigned differences from 83 to 69. Structural
  differences changed from 12 to 14; this is an allocation improvement, not
  a structural match. Preserved the original descriptive local layout.
  Remaining: duplicated/merged decision stores, branch polarity/long-branch
  selection, cooldown-address derivation and scratch registers.
- **08187230 — improved draft 85.8%, Armor_UpdateAxisAttackDecision.**
  Same change and 69/14 realigned residue as 081875EC (earlier recorded 83.7%;
  the unchanged reviewed baseline reproduced as 83.9%). Marked register
  pinning; the absolute-Z r2 hint remains. A dz r5 pin worsened the result
  and was discarded. Both decision drafts still use the original assembly
  by default and earn no exact credit.
- **08186EB4 — stopped below threshold, Armor_SelectTimedAttack
  (tentative).** Rechecked the prior candidate and tried absolute-Z, fixed
  one/mode registers and separated constant initialization. No useful gain;
  stronger delta pins increased structural differences. Retained the earlier
  unpinned build/6eb4-base.c candidate in wip/sub_08186EB4.c: 75.9%
  against the reviewed 892-byte extent, but 195/122 realigned differences,
  worse structurally than this session's initial 149/53 form preserved in
  build/6eb4-start-task.c. Branch/store sharing, the mode/one lifetimes and
  state-pointer reuse remain blockers.
- **08186CC8 — new draft 80.6%, Armor_SelectAlignedAttack (tentative).**
  Actor/player X/Z alignment and vertical-distance test select attack or
  recovery, with Armor_IsNearSavedPosition / Armor_SaveCurrentPosition
  remembering the attack origin. The prior unpinned baseline is 72.0%.
  Pinning only absolute X to r5 yields 80.6% against the reviewed 492-byte
  body/pool extent (candidate 500 bytes); no stack-pointer comparisons were
  present in this accepted candidate. Remaining differences include an extra
  saved register, decision-pointer lifetime, merged fallback behavior calls
  and flags allocation. Repeated fallback calls move across the function,
  so call-anchored alignment reports a large 184/166 residue despite the
  higher whole-function fuzzy score. An ax+az pinned candidate scored 89.3%
  but generated `mov r0, sp` for the Z sign test: rejected, as in the earlier
  pass. Do not use that candidate as evidence of source correctness.
- **081835D8 — retained draft 94.0%,
  ActorController_UpdateHeavyAttackCollider.** Replaced the inherited
  half-power comment/helper name: the observed scale is 0xAAA / 4096 with
  sign-aware truncation. Typed layer accesses reduced structural residue
  from five to two but expanded allocation differences (79.6%); collider and
  multiply pins, barriers, operand order and inline variants did not close
  it. Kept the readable unpinned 94.0% candidate (10/5 realigned residue).

Restored required file-local aliases for all three pre-existing drafts and
used address-named INCLUDE_ASM fallbacks for compatibility with the cached
ELF. Each guarded source was formatted directly with the venv clang-format,
then checked with --nonmatching --record; all four default assembly fallbacks
passed exactly. depin.py was run on unguarded copies of the three hinted
candidates and skipped them because they are nonmatching; no fallback match
was mistaken for an exact C result. No generated assembly, ROM, dump or
scratch candidate is committed. Name promotion and combined ROM validation
remain integration work.

## Task 080980FC (2026-10-20)

Tags: #matching #ghost #register-pinning

Exact code gain: +0.0941 percentage points of total code (2,272 bytes), in the
Register-pinned category. Two existing guarded drafts became exact matches;
two new guarded drafts were retained. No network operations or full builds.
All commits submitted separately through build/commit.sh.

- 08098CA0: matched as Ghost_FollowPlayerLoop, retaining the existing name.
  tools/relatives.py found Axe_FollowDeceleratingRoute (080FDA44), explaining
  86% of the body. Porting its typed route and random/polar helpers, relocating
  route fields to +0x566/+0x568/+0x5B0/+0x5FC, adding the two route-builder
  calls, and restoring the mode-dependent speed increment yielded only two
  register differences. Removing RandomMotion's table pin resolved those.
  tools/depin.py reduced three hints to two: the inherited r4 observed-address
  pin and empty input barrier remain necessary. Formatted exact check: 1,168
  bytes. This callback rebuilds nine waypoints around the player and halves
  speed when the route renews.
- 08099130: matched as Ghost_PursuePlayerLoop, retaining the existing name.
  Ported the newly matched Ghost_FollowPlayerLoop; renewal radius is 256,
  speed is retained, and acceleration is every sixteen ticks up to eighteen.
  First check matched, and depin retained the same two hints. Formatted exact
  check: 1,104 bytes. Both route callbacks call 08098344 and 080980FC.
- 08098788: drafted 84.4% as Ghost_UpdateVisibility (tentative). Its caller
  Ghost_OnBehaviorUpdate runs this after state dispatch. Sets collider B bit 4,
  handles the special 0x4000 mode, and coordinates reveal/conceal transitions
  using context flags +0x558, timer +0x5B2, player mode and save state. The
  visibility tests share code with SceneActorControl_RequestVisibility.
  Retained candidate has 55 realigned differences, 18 ignoring registers:
  availability/hidden-test expansion, temporary lifetimes, scheduling and
  allocation. u8 result helpers and mask-before-load accessors improved the
  initial 75.1% candidate to the retained score. Pinning the context and
  changing Hidden's return width worsened the result and were discarded.
  tools/check.py --nonmatching --record measured the full reviewed extent.
- 080980FC: drafted 88.8% as Ghost_EmitAfterimage (tentative). Called by the
  matched route callbacks. Every context interval +0x556 ticks, refreshes one
  of four sprites (stride 0x60 at +0x3D4), selects the actor's directional
  animation frame, runs the packed animation stepper, copies its position,
  unhides it and sets lifetime eight. Index +0x554 wraps after three. The
  middle animation-stepper run is shared with Actor_InitPaletteEffectPose.
  Retained candidate has 28 realigned differences, six ignoring registers:
  sprite/animation address reuse, argument scheduling and allocation.
  Raw separate animation-base addressing improved 81.1% to 88.8%; forcing
  sprite/animation pointer registers worsened it. Recorded full reviewed
  extent is 488 bytes.
- 08098A74: stopped below draft threshold. Best exploratory candidate 61.3%,
  113 realigned differences / 34 ignoring registers. Tracks the player's
  position delta into actor motion using context +0x5EC, sets direction and
  animation scaling, then returns to idle when the player's +0x428 action
  field clears or control completes. Shared control/state reset expansions,
  player-pointer aliasing and allocation differ. Typed/raw player views,
  cached flags pointers and targeted lifetime pins did not improve it.
  Best candidate retained locally at wip/codex-medium/sub_08098A74.c.
- 08098344: stopped below draft threshold. Exploratory cubic-route candidate
  reached 40.4%; 160 realigned differences / 82 ignoring registers. Copies
  start/target positions with zero Y, samples eight intermediate points at
  Div(1024,8) intervals and copies the endpoint as point nine. Uses context
  motion X/Z scaled by 128 in the observed polynomial. Exact interpretation
  remains tentative: the polynomial includes terms that look unusual, so
  the scratch version follows observed arithmetic rather than correcting it.
  An explicit stack-frame struct fixed local layout, but expression grouping,
  repeated weights, stack-pointer lifetimes and allocation remain unresolved.
  Candidate retained locally at wip/codex-medium/sub_08098344.c.

## Task 081B48A0 — 2026-10-20

Tags: #matching #vanargand #animation #palette #register-pinning #nonmatching

Worked solo on the preclaimed offline cloud/081B48A0 branch. Read the entry,
worker, cloud and big-function guidance, existing task notes, callers/callees,
assembly and matched relatives. The Ghidra export is absent. Exact gain:
+0.0429 percentage points of total code (1,036 bytes), all Register-pinned.
Two functions matched and one existing guarded draft improved. Three targets
stopped below the full-extent draft threshold. No network or full build;
per-file check.py checks and build/commit.sh only. Shared docs were not edited.

- **081B5454 — matched, Vanargand_UpdatePartRadialTravel (tentative).**
  On reset, computes a planar heading/distance target, starts motion from
  origin with mode four, selects sequence nine and steps packed playback.
  Updates motion each tick; completion sets finished, otherwise requests
  flags 12. Full reviewed extent 472 bytes, ending at 081B562C. Porting the
  matched Vanargand_UpdatePartBobbing stepper and the existing sidestep vector
  idiom left five register differences. Moving the stepper's 0xFFFF source
  to r2 (mask held in r5), plus a short finished-offset/pointer constraint,
  closed them. Direct clang-format and the final full-extent check pass.
- **081B5A98 — matched, Vanargand_UpdatePartRevealFade (tentative).**
  On reset, places origin at the radial heading/distance target, unhides
  the sprite, copies palette source +194, and selects/steps sequence nine.
  Interpolates from +194 to +190 with frame and six banks, advances after
  frame 63, then marks finished. Full reviewed extent 564 bytes, ending at
  081B5CCC. The same stepper needed its 0xFFFF source in r3. Explicit short
  offset/address constraints reproduce the reset stores and prevent the
  switch-state address being held across Palette_InterpolateBanks. Reusing
  the state offset plus eleven gives the observed enter-address derivation;
  loading the destination palette before the source-offset constraint closes
  the last argument-order differences. Direct clang-format and final exact
  check pass. These hints contain no handwritten instruction bodies.
- **081B48A0 — improved draft 93.9%, Vanargand_UpdatePartSidestep.**
  Retained the existing symbols/functions.csv name. Previous draft 87.6%.
  Moves 128 units along byte +187, preserves origin height, then restores
  sequence nine and marks finished. Passing the saved state into the second
  stepper's first flag-mask calculation reproduces its reuse of the value
  one; the initial stepper gets an explicit first-bit input. A planar-add
  inline, r6 enter pointer, and transition-value constraints reduce residue
  from 63/19 to 27/11 realigned differing instructions (total/ignoring
  registers). Remaining: owner-copy scheduling, entry/motion address reuse,
  vector load registers, origin-height addressing and pool placement. More
  explicit planar-pointer constraints reduced structure to eight but worsened
  total differences to 58; discarded in favor of this 93.9% candidate.
  Recorded with --nonmatching --record; default INCLUDE_ASM fallback checked
  exactly. Restored the required local alias and raw-address fallback token
  for compatibility with the cached assembly/ELF naming.
- **081B562C — stopped below threshold, tentative part emergence.**
  Recovered owner display flag selection, sequence nine, palette +190 copy,
  two owner keyframe phases with song 0x58D, outward radial motion from radius
  640 to the configured distance, contact initialization and song 0x58E.
  The final owner display position projects basePosition with sign-aware
  division by 32 and the screen offset at 03005418. Read condensed listing
  and relatives; used the shared part/stepper layout. Best retained scratch
  is 74.5% over the full 1,132-byte extent, 177/56 realigned differences,
  candidate 1,156 bytes. An output-pointer projection helper fixes the initial
  word read/modify/write expansion of stack halfwords. State-address lifetime,
  enter-pointer copies, vector constant/address hoisting, extra saved register,
  owner flag scheduling and transition ordering remain. Typed owner fields,
  state-load address constraints, keyframe pointer constraints and a radius
  hint did not reach draft quality. Scratch remains uncommitted at
  wip/codex-medium/sub_081B562C.c.
- **081B5EA4 — stopped below threshold, tentative
  Vanargand_InitializePartController.** Called by
  Vanargand_CreatePartController. Initializes owner keyframes and two part
  records at +98 with stride +21C, assigns palette/tile sources and optional
  external palette destinations, and initializes visible mode with radial
  positioning and guarded action two. First source 66.4%, 133/52 realigned
  differences. Multiple advancing cursors for palette pointers, destination,
  tile and part improve fuzzy to 70.7% over the full 732-byte extent, but
  leave 114/75 differences (candidate 720 bytes). The second loop's canonical
  part/flags addresses, loop-stride hoisting, mode spill, action-store
  addressing and signed end comparison differ. High-register mode/palette
  hints and stride barriers did not improve the result. Best retained cursor
  candidate is uncommitted at wip/codex-medium/sub_081B5EA4.c.
- **081B4DA4 — stopped; existing boundary blocker confirmed.** Rechecked the
  earlier wip/sub_081B4DA4.c leap candidate against assembly and both extents.
  It is 84.2% only over the reviewed first body/pools through 081B512C
  (0x388 bytes): 82/20 realigned differences, candidate 912 bytes. Inventory
  extends through 081B52E8, including another apparent function at 081B512C
  with its own push, stack frame and reset/state dispatch. Full-extent check
  is 67.7%, 299/237 differences. No production draft was added. Integrator
  still needs to review/add the missing boundary before a shorter-body score
  can count. Existing candidate and evidence were preserved.

All accepted callbacks are in the statically observed Vanargand part-action
family around gUnk_086120A0; no runtime coverage claims were added. Each source
change was committed separately through the driver. Combined ROM validation
and name promotion remain with the integrator.

## 2026-10-20 — task 08217960 revisit

Tags: #matching #sprites #palette #terrain

Assigned-function exact coverage rose from 0% to 50% (+50 percentage points,
three of six functions). Three register-pinned exact matches; two improved
guarded drafts. Only fast check.py checks were run, with no full build or
network operations. Separate driver commits preserve each translation unit.

- **08218508 — matched, SpriteOam_QueueFlaggedProjectedSprites.** Reviewed
  788-byte extent. Read its render-callback caller 08163398 and the common
  OAM staging code. Kept the existing tentative name: flag 0x1000 selects
  this pass, but its meaning is still uncertain. Inlining the screen
  projection body directly into the renderer removes the helper argument
  copy and the hoisted/spilled stack-vector pointer. Preserve initial flags
  in a separate unpinned local. A local loaded-flags value pinned to r1
  before the culling mask recovers the later r6 copy. The shifted X temporary
  pinned to r0 and the affine counter's old value pinned to r0 recover the
  remaining copies. Load tile before priority, then combine with a left-
  associative OR expression: changing the grouping to tile |= (a | b)
  produces different code. Final formatted production C passes check.py
  byte-identically, with no handwritten instruction bodies.
- **0821881C — matched, SpriteOam_QueueOverlayProjectedSprites.** Reviewed
  788-byte extent. Ported the exact 08218508 body, changing only the required
  drawable flag from 0x1000 to 0x2000 and preserving the existing tentative
  name. Exact check passes; separate match commit.
- **08217F4C — matched, SpriteOam_QueueProjectedSprites.** Reviewed 776-byte
  extent. Ported the exact flagged renderer and removed its required-flag
  test. Existing callers install it with Video_SetRenderCallbacks. Exact
  check passes; separate match commit. These three matches total 2352 bytes.
- **08218254 — drafted 99.1%, SpriteOam_QueueCameraRelativeSprites.** Improved
  from the retained 91.0% draft by porting the exact common OAM/affine tail,
  computing the culling mask before offset loads and pinning the Z difference
  result to r0. Final reviewed score: four realigned differing instructions,
  two ignoring registers, over 692 bytes. The entry currently loads flags
  into r0 and copies to r6 before loading/masking the parity skip value; ROM
  loads r1, copies to r0, masks, then copies r1 to r6. The missing copy becomes
  pool padding, so total length already agrees. Explicit masks, an inline
  mask accessor, late flag assignment, and a small initial/mask/flags pin
  sweep did not resolve this. Kept the guarded draft and recorded its score.
- **08217960 — drafted 86.7%, Palette_ApplyBackgroundFade.** Improved from
  80.1%. Pin the blend color's green component to r10, introduce per-bank
  green/red-blue contributions, retain the red-blue mask in r9, and express
  the blended inner loop with separate red/green arithmetic temporaries.
  Freeing the fetched pixel local improves output more than pinning it.
  Final reviewed score: 55 realigned differences, 24 ignoring registers,
  against the 604-byte target. Loop-counter setup, mask hoisting, backdrop
  blend temporaries and allocation remain. Explicit outer-index increments
  regress; do/while inner loops produce identical output. Kept the guarded
  draft and recorded its score after formatting.
- **0821BF5C — stopped below draft threshold, tentative
  Terrain_CanCrossSlopeBoundary.** Read the full 760-byte target, its movement
  caller 0821C254 and height-query callee 0821BE3C. Fresh reconstruction in
  ignored wip scores 73.6% reviewed, with 121 realigned differences / 73
  ignoring registers. Tests destination collision bits (mask 1 or 2 by mode),
  height thresholds 100/128, source/destination slope orientations and the
  matching flat neighbor's low-nibble height and collision bits. Expanding
  neighbor override lookups in each direction branch preserves more of the
  duplicated call/control structure than the inline resolver; caching the
  selected index causes the compiler to merge those calls again. Signed
  threshold locals recover BLE. Still below 80%, so no production draft or
  source commit for this function.

All production functions have file-local descriptive aliases. Both guarded
sources use address-form INCLUDE_ASM alternatives, so the recorder resolves
against this checkout's baseline ELF; both default assembly alternatives
were checked exactly. Three exact sources retain their register-pinning
concession markers. Notes committed last; shared index/history left unchanged
under CLOUD.md. Combined ROM validation remains with the integrator.

## Task 08104350 — 2026-10-20

Tags: #matching #actor-group #proximity-actor #register-pinning

Assigned target-byte exact coverage increased from 0% to 31.5% (+31.5 percentage
points; 1300 of 4124 reviewed bytes including pools/alignment). Two exact
functions and four NONMATCHING drafts, each in its own driver commit on the
already-claimed cloud/08104350 branch. No network operations or full builds.

- **08105AB4 — drafted 81.2%, ProximityActor_UpdateDamageAnimation.** Reads the
  context at 020001B4, health at FC, maximum at FE, damage step at 100 and flags
  at 108. Starts a terminal animation/state when health reaches zero; otherwise
  selects a damage frame or mirrored recovery frame, restores health in steps
  of ten and clears the hit byte EB. The packed-command stepper repeats twice.
  Started from the matched MessageWindow/SceneActors stepper relatives. Explicit
  offset-before-table evaluation, scoped flag masks, a distinct duration copy,
  absolute-value expression and temporary quotient improved the candidate.
  Pointer lifetimes, instruction ordering and allocation remain: 102 realigned
  differing instructions, 29 ignoring registers. Recorded against all 952 bytes;
  the candidate is 940 bytes. Guarded draft retains register hints.
- **08104350 — drafted 84.0%, ActorGroup_CreateFromScript.** Creates/uses the
  020001B0 Pool184 manager, allocates a member, loads sprite resource 9385 and
  reads m/p/r/n/a/b/A/P/I/G options. The A option supplies health, debris count,
  a byte and three arguments passed to 08105194; default health is 100 and the
  default count is three. Origin at 118 copies the eight-byte position at 1C;
  a supplies the squared activation radius at 138. Recovered a local member
  view, shadow initialization and initial packed-animation stepping. Passing the
  tested status result to the alternate shadow call keeps its distinct call
  path. Typed fields and removing forced outer register/pointer lifetimes were
  more useful than additional pins. Rejected an intermediate with overlapping
  fixed-register lifetimes; retained the version with ordinary outer locals.
  Remaining option/default ordering, argument-loop form, pointers and allocation
  give 120 realigned differences / 70 ignoring registers. Candidate 1008 bytes,
  reviewed target 1016. Only the guarded stepper retains matching concessions.
- **0810523C — matched, ActorGroup_EmitRemovalDebris.** Five emissions through
  08127590 with resource hashes 9385/74C9, radius ten, lifetime 30 and parameter
  490. The first vertical velocity is 64, the others 34; the last three select
  (0,1), (1,0), (2,0) variants. Ends by calling 081050BC. Keeping random-angle
  evaluation before the vector argument in the inline Polar helper fixes load
  ordering in all five copies. A read-only empty constraint on the first angle
  and vector settles the first allocation without destroying the angle's known
  byte range. depin removed both explicit register bindings and retained only
  that constraint. Exact after formatting with --target-size 900; marked as a
  register-pinning concession because the empty constraint remains.
- **0810480C — drafted 94.5%, ActorGroup_UpdateMemberProximity.** On entry clears
  the frame counter, exposes the sprite and initializes a rotated offset with
  radii 41/148. On odd frames tests origin range and player eligibility, starts
  turn step four when the distance changes, and decrements the health/countdown.
  Expiration selects state three; other activations play song 3C9. Then calls
  081049F0 and increments the frame. A scoped clear helper restores CpuSet
  ordering; constraining the first scaled X value and the intermediate Z value
  improves allocation. Fourteen realigned differences, four ignoring registers:
  angle/Z register choices, a few address temporaries and flag-test/load order.
  Candidate and reviewed target both 484 bytes. Guarded draft with hints.
- **081049F0 — drafted 82.2%, ActorGroup_UpdateMemberSway.** Wraps the byte angle,
  rotates the two radii around the saved origin and adjusts/damps the turn step.
  Preserve -(radius * sine), rather than (-radius) * sine: agbcc selects a
  different negate/multiply order. Signed magnitude shifts reproduce the
  truncation-toward-zero fixed-point scaling and half-step calculation. A scoped
  r0 result for two scaled components recovers the original result copies;
  another attempted table pin regressed and was discarded. Forty-five realigned
  differences, four ignoring registers: primarily sine-table/scratch allocation,
  with redundant turn-step reads and two bytes of body length still off.
  Candidate 368 bytes, reviewed target 372. Guarded draft with hints.
- **08105808 — matched, ProximityActor_ApplyContactDamage.** Collider callback
  installed by ProximityActor_InitializeCollider (08105998). Flags at actor 108
  gate contact response; reaction bit eight emits variant-dependent sparkles and
  song 15C on a cooldown. Variant one loses 200 health per accepted contact;
  the other uses incoming collider damage. Both clamp health at zero. Variant
  one also forwards flag-10 contacts through the callback/data pair at collider
  4C/50. Use one blocked local across both variant branches to share the
  materialized status test; an inline mask accessor loads the mask before flags.
  `health -= damage; health = health < 0 ? 0 : health` reproduces both stores and
  the signed truncation test. Exact after formatting with --target-size 400,
  with no register hints.

All names are provisional and carry confidence=low comments above exact alias
lines. Four draft scores were recorded after formatting using check.py
--nonmatching --record; their default INCLUDE_ASM branches also pass exact fast
checks. depin was run on hinted sources (nonmatching candidates are skipped by
that tool). Some draft commit titles retain their pre-record rounded scores;
source fuzzy lines and the scores above are authoritative. Shared headers,
symbol names, generated output, index and history were left unchanged. Combined
ROM validation remains the integrator's responsibility.

## Task 08129C64 — 2026-10-20

Tags: #matching #effects #register-allocation

- **0812A7B8 matched** as `LinkedEffect_FinishRemoval`. Entry selects sequence 24; animation completion releases the pool bit, removes a visible sprite and the collider at 0xAC, invalidates the signed slot index, and clears the halfword at 0x174. Exact reviewed extent: 368 bytes. `tools/depin.py --write` reduced 14 hints to 12; the formatted source also passed the exact check. Local register pinning concession applies.
- **0812B424 drafted 94.8%** as tentative `TwinSpriteEffect_EmitTrail`. Periodically initializes sequence 1 on alternating 0x5C-byte trail records, steps it, copies the parent's packed position, sets display flags/palette and a four-tick lifetime, and wraps the two-entry index. Remaining differences: shared address calculation versus duplicate multiplication, animation mask register r6 versus r5, and late position-copy address ordering. Realigned difference: 16 instructions, 9 ignoring registers. Explicit sprite/animation pointers greatly improved the initial 80.3% candidate.
- **0812AA0C drafted 83.8%** as tentative `LinkedEffect_BeginRisingTravel`. Sets the hidden/transition flag, constructs a random-distance destination along its heading with Y elevated by 512, grows both sprite scales toward 64 over 15 ticks, emits the linked variant every eight frames, then enters state 2 once animation and scaling finish. Remaining differences: entry vector arithmetic, literal load ordering, register allocation, and reused field-offset constants. Realigned difference: 58 instructions, 11 ignoring registers.
- **08129FF0 drafted 86.5%** as tentative `SunDiscPool_UpdateFallingDisc`. Applies downward acceleration of four units, integrates packed position, samples terrain height and slope, removes slots below zero, and stops at the terrain before state 3 and sound 242. Remaining differences: pointer lifetimes/saved registers, first command load, and terrain-height truncation. Realigned difference: 44 instructions, 13 ignoring registers. Parenthesizing the cell offset and keeping animation and vector pointers explicit improved the candidate substantially.
- **0812ACB0 stopped below draft threshold**, best retained candidate 79.1% in `wip/sub_0812ACB0.c`. Tentative `LinkedEffect_FollowTarget`: randomized animation selection, delayed turning toward a target, randomized one/two-unit heading changes, acceleration every ten frames, rising Y interpolation, collider updates and expiry. Remaining differences: vector/address lifetimes, extra saved register, block layout, signed truncation arithmetic and allocation. Realigned difference: 148 instructions, 65 ignoring registers. Hard-register pointer experiments regressed and were discarded. Preserve only the ordinary-pointer candidate; no production C added.
- **08129C64 stopped below draft threshold**, best useful candidate 65.9% in `wip/sub_08129C64.c`. Tentative `SunDiscPool_UpdateTravellingDisc`: advances its animation/position, projects a 32-unit point ahead of velocity, starts gravity within squared XZ distance 0xBD0F of its target, samples terrain ahead, refreshes the collider, then rolls back movement and enters the impact state. Remaining differences: many pointer/register lifetimes, stack vector materialization, duplicate gravity branches and terrain-height handling. Register-pointer experiments regressed and were discarded. No production C added.

Shared observations: the linked-effect record has animation at 0x48, target pointer at 0x100, packed velocity/destination at 0x16C, state/slot at 0x176/0x177, age at 0x178, entry/speed/animation-complete/heading at 0x17A–0x17D, and countdowns at 0x180–0x188. These are local observed layouts, not a promoted universal actor type. The SunDisc record instead has animation at 0x2C, projected position at 0x48 and velocity at 0x8C. Existing matched packed-animation code supplied the volatile command reads and timing/loop semantics. Ghidra export was unavailable. All validation used `tools/check.py`; drafts were recorded with `--nonmatching --record`. No full build or network operations were run. Final notes commit also formats the already-matched 0812A7B8 source without changing its output.

Closeout tooling correction: `tools/depin.py` does not pass `--nonmatching`; on a guarded draft it checks the assembly fallback and can incorrectly remove every hint while retaining a stale fuzzy line. This occurred during closeout. All three reviewed C candidates were restored, formatted, and explicitly rechecked with `--nonmatching --record`, confirming 0812B424 = 94.8%, 0812AA0C = 83.8%, and 08129FF0 = 86.5%. Separate corrective commits restore each draft's hints. For future drafts, run depin on the unguarded scratch candidate, never on the guarded production source. The exact 0812A7B8 depin run used unguarded C and was unaffected.

## Task 08144408 — 2026-10-20

Tags: #matching #player #actions #recoil

All six assigned functions matched with ordinary C and file-local descriptive aliases. Only `tools/check.py` was used for compilation/validation; each formatted source passed with an explicit reviewed full extent. No register pinning, full build, network operation, or shared documentation change was needed. Each match was queued separately through `build/commit.sh`.

- **08145DA0 matched — Player_UpdateDefeatRecoil**, tentative name, 852 bytes. State entry folds facing, plays recoil sounds, notifies wireless defeat under global flag 0x1000, and clears the descriptor attachment. Recoil decays by three quarters; later phases set global flag 0x400, player byte 0xA6 and fading control flags. Matched recoil siblings supplied signed truncation and the existing Player layout; first candidate matched.
- **081454E0 matched — Player_UpdateTerrainRecoil**, tentative name, 840 bytes. Three terrain/contact conditions steer toward terrainTarget with signed truncation by eight; the first two emit distinct particles. Otherwise applies heading-based recoil. Requests camera shake and decays directionFrame. Completion selects motion 0x16, 0x15 or idle. Initial 98.5% candidate used contactMagnitude incorrectly: the assembly reads positionResetTimer at 0x4D8. Correcting that field produced an exact match.
- **08145828 matched — Player_UpdateContactKnockdown**, tentative name, 752 bytes. Sets status bit 0x40000, applies recoil and a saturating recovery counter, waits, then clears contactSpecial and returns to idle. Started from PlayerCue_UpdateKnockdownRecovery (08150DB0), replacing cue sequencing and recoilSpeed with this action's phase API and directionFrame. A scoped mask local before the status read fixed the sole instruction-order difference.
- **08145B18 matched — Player_UpdateDefeatSlide**, tentative name, 648 bytes. Shares defeat notification, attachment clearing and delayed fade with 08145DA0, but slides along facing plus one and advances when the speed decays to zero; animation 0x2E then enters the defeat wait. The shared typed recoil expressions matched without register hints.
- **08144408 matched — Player_UpdateChargedBurst**, tentative name, 540 bytes. Creates ChargedBurst 150 units along the facing-derived angle, with power shared-resource-level times 60 plus 120 and lifetime 90, then runs success/failure animation phases. Packed 16-bit coordinate bitfields reproduce the two word copies and masked updates. The assembly retains the angle in argument r3 at ChargedBurst_Create: explicitly passing it and computing power before the player-slot mask fixed the initial 87.1% candidate.
- **08144624 matched — Player_UpdateSpiralBurst**, tentative name, 512 bytes. Requires two shared resource units, projects the same packed burst position and calls SpiralBurst_Create. Uses motion 0x11 with effect parameter 0x173. Derived from the charged-burst state machine and matched on the first check.

Ghidra export was absent. Neighbouring matched functions and assembly were sufficient; all six final checks reported zero differing bytes over the complete body and trailing alignment/pool extent. Combined ROM validation is left to integration as instructed.

## Task 080F9974 — 2026-10-20

Tags: #matching #axe #actor-phase #animation #register-allocation

Two of six assigned functions matched (33.3% of the task); four were retained as guarded drafts. Exact code gain is approximately 0.047 percentage points (1,136 bytes), all without register pinning. Only per-file checks were run; no full build or network operation was performed. Each match and draft was committed separately through the driver. The Ghidra export was unavailable.

- **080F9BE4 drafted 86.9% — Axe_UpdateDefeat.** Entry selects profile 10, disables both colliders, sets actor flag 0x800, resets the spark-pattern index and releases the auxiliary sprite. Every other frame emits a spark at a rotated diagonal offset cycling through -120, -75, -30, 15 and 60. Blink completion calls the terrain/death scripts, records category event flags (including context byte ED7), and returns to idle. The 08082F70 cleanup and other defeat relatives supplied the common tail. A result local in signed fixed-point scaling fixed branch layout and reduced the realigned structural difference substantially. A struct containing the CpuSet zero source followed by eight script values reproduces the stack layout. Remaining: 84 realigned instructions, nine ignoring registers, chiefly register allocation, CpuSet setup order, script-value loads and final control setup. Reviewed target extent: 980 bytes. No pins.
- **080F9FB8 matched — Axe_UpdatePursuit.** Initializes control from entryTarget228, enables sprite flag 2, and sets context E56 to 512. Within that distance it chooses a tangential heading relative to the player; farther away it turns toward the player with an angle-dependent step. Writes a 32-unit heading velocity and sprite rotation 159 minus heading. The typed local context halfword removed a redundant constant copy. A Polar inline helper prevents local vector stores from being combined into masked word writes. Separate FromPlayer/ToPlayer inline helpers reproduce global-pointer and actor-position address evaluation order; computing the actor position inside FromPlayer was the final fix. Formatted C passed check.py with reviewed --target-size 604, zero differing bytes. No pins.
- **080FA32C matched — Axe_UpdateHealthPacedMotion**, tentative name. Restores saved control, checks callback completion, then selects speed 8/6/4 and phase duration 400/500/600 from health thresholds at context E48 and EB4. Lower health bands enter sprite-collapse control before animation state 4; the highest band switches only the animation state. Otherwise writes heading E9 to motion and advances the state counter. Explicitly narrowing Callback_InvokeActor's result to u8 recovers the observed byte test. Polar captures the heading once. Changing SetState's arguments from u32 to u8 fixed the final constant-load ordering difference. Formatted C passed check.py with reviewed --target-size 532, zero differing bytes. No pins.
- **080FBA38 drafted 81.8% — Axe_EmitMotionTrail.** If the parent sprite is visible and actor field 2CA is divisible by the context interval at E46, initializes and advances the next trail sprite, copies actor position/angle/scales, sets lifetime eight and flags, then wraps the four-entry cursor. Context trail records begin at 708, stride 60; sequence at slot+48, resource at +58 and lifetime halfword at +5C. Cursor is E44 and animation variant ED7. Existing packed animation code supplied the flip and frame-timing logic. Remaining: 62 realigned instructions, 31 ignoring registers; sequence argument/address setup, pointer lifetimes, field access order and registers. Reviewed target extent: 588 bytes; candidate is 584. Uses register hints; depin on the unguarded candidate correctly skipped because it is not exact.
- **080FBC84 drafted 85.2% — ActorPhase_EmitTrailSprite** (existing name retained). Same trail ring and stepper as 080FBA38, but takes explicit position and angle arguments; scales still come from the parent drawable. Remaining: 52 realigned instructions, 22 ignoring registers, including sequence/address setup, stack frame and field order. Reviewed target extent: 600 bytes; candidate is 596. Uses register hints; unguarded depin skipped as nonmatching. The default named INCLUDE_ASM fallback passed the per-file check over 600 bytes.

All fuzzy scores above are the recorded full-target scores from check.py --nonmatching --record. The trail commit titles carry the earlier candidate-length scores (81.4% and 84.8%); the recorded source scores are authoritative. check.py's recording helper keys scores by compiled name and does not follow the existing ActorPhase_EmitTrailSprite alias in the fallback. For 080FBC84, temporarily using sub_080FBC84 in the fallback allowed --record to write 85.2%; the canonical fallback was then restored and checked. The final notes commit includes that score-line correction. No shared headers, symbols, generated output or shared documentation were changed. Combined ROM validation remains for the integrator.

## 2026-10-20 — task 08156FE0

Tags: #matching #player #ui

- **0815FB80 matched**, `PlayerOrbitEffect_UpdateStatTransfer` (tentative). The
  orbiting transition effect follows five timed arcs around the scene position,
  returns to its owner and reduces current stat A toward the initialized target.
  Separate offset statements prevent reassociation of the frame shift and negative
  constant. Creating the scene-position pointer before those statements fixes
  literal order; reading the frame before the animation-pointer assignments fixes
  the final scheduling difference. No register hints needed.
- **0815F2C8 matched**, `Player_InitializeFromScript`. Initializes display resources,
  advances the embedded packed sequence, runs script initialization routines,
  registers the slot and creates barrier/attack effects. Reused the packed decoder
  from matched ContactSequenceActor code. The sprite pointer must occupy r6;
  pinning the player base itself caused bad allocation and was discarded. Retained
  12 register/barrier hints after `tools/depin.py --write`; unit is marked as a
  register-pinning concession. Exact check passed after formatting.
- **0816349C matched**, `TransitionPanel_UpdateCostSelection`. Accept/cancel and
  left/right choice handling for the cost prompt. Confirm invokes the configured
  script with the selected cost; cancel advances the player sequence and panel
  close state. Loading the choice into a local before setting the decision to one
  reproduces the original conditional initialization. No hints needed.
- **08163FFC matched**, existing name `StatDisplay_DrawRatio`. Converts numerator
  and denominator into digit arrays and draws both sides plus style-dependent
  padding. Two arrays with a local dimension of five reproduce the original
  separate dynamic stack allocations and frame pointer. First candidate exact;
  no hints needed.
- **08156FE0 retained existing draft 98.0%**, `Player_SelectActionSequence`.
  Independently reconstructed candidate reached 93.4% (55 realigned differences,
  four structural); flag tests need an explicit field pointer before mask/value
  statements. The scene-five input-field address is otherwise reused across a
  call. Final review found an existing descriptive-name draft that the initial
  address-only search missed. Restored that stronger source after an intermediate
  lower-score draft commit, added the required local alias, and recorded 98.0%
  again: 16 register-only instructions, zero structural differences. Default build
  continues to use INCLUDE_ASM.
- **08160108 stopped unmatched**, tentative `PlayerOrbitEffect_InitializeParticles`.
  Recovered the sixteen-particle setup, random angle/speed/radius/lifetime fields,
  trigonometric offsets and tile selection. Best whole-listing similarity was
  68.6%; a different variant had 16 structural differences but 90 total. Main
  residue is long-lived RNG/table/resource allocation, stack-slot order and the
  two sprite/interior-state pointers. Pinning the RNG pointer or interior pointer
  caused additional reloads after byte stores. No candidate met the 80% draft
  threshold; exploratory sources remain under ignored wip/ and build/.

Validation used per-file tools/check.py only; no full build or network operations.
Each exact match was committed separately through build/commit.sh.

## 2026-10-20 — task 080A8750

Tags: #matching #tentacle #scene-actor #animation #terrain

Worked offline on the already-claimed `cloud/080A8750` branch, using per-file
`tools/check.py` only. No make, complete build, network operation or direct git
mutation was run. Each match/draft was queued separately through build/commit.sh.

- **080A8750 matched — TentacleActor_UpdateKnockback.** On state entry,
  installs actor+0x22C control, derives horizontal velocity from facing and the
  sine table with scale four, clears vertical velocity and sets movement mode
  two. Advances movement with arguments six and 0x1000; flag 0x10 requests a new
  wander route and restores entry control/state. A pointer to direction created
  before the velocity pointer reproduces the original address scheduling.
  Explicitly retaining the state offset through the final flag store avoids
  a different address register. The final counter load needs r0 offset and r1
  pointer hints. Exact 380-byte body including pool/padding; depin reduced nine
  hints to six, and the formatted production file passed the exact check.
  Register-pinning concession retained.
- **080A8904 drafted 87.6% — TentacleActor_SelectWanderRoute.** Reads terrain
  surface and the context's route-choice count, enumerates adjacent cells while
  rejecting opposite headings, prefers an unvisited side route, then a random
  side route, then unvisited forward travel, then reverse travel. Writes the
  chosen cell center to context+0x904 and arms timer 0x8E4 for 40. Context+0x8E8
  counts choices; +0x8E9 stores a heading exclusion. If no reverse route exists,
  rotates that exclusion. Uses existing terrain helpers and visited-route calls
  at 080AB51C/080AB54C. A signed pointer do/while initializer and a nested terrain
  surface test improved the initial draft. Remaining: 64 realigned instructions,
  37 ignoring registers; facing/count/forward lifetimes, first destination
  pointer lifetime, and store/address order. No register hints retained.
- **080A8F38 drafted 85.0% — TentacleBody_RefreshDirectionalAnimations.** Iterates
  0x128-byte segments, skips status bit one, selects tail sequence one for status
  bit four and reverses its direction, otherwise uses the actor-profile sequence
  and preserves the current frame/timer around the sequence update. Advances the
  packed animation and updates sprite frame/flips. Remaining: 41 realigned
  instructions, eight ignoring registers; stack-slot order, sprite/command
  pointer lifetimes and register choices. Register/barrier hints retained.
- **080A915C drafted 87.0% — TentacleBody_ResetDirectionalAnimations.** Same
  directional animation update, without preserving the frame/timer. A local
  frame record reproduces the stack layout more closely. Remaining: 36 realigned
  instructions, ten ignoring registers; first frame/flip accesses, branch/store
  layout, and loop-tail registers. Register/barrier hints retained.
- **080A93F4 drafted 87.0% — TentacleBody_RestartSegmentAnimations.** Takes
  explicit row/flags rather than deriving direction, calls Anim_RestartSequence,
  applies speed to non-tail segments, then advances the same packed animation.
  Remaining: 38 realigned instructions, 16 ignoring registers; parameter/frame
  placement, sprite pointer copies, store layout and loop registers. Hints kept.
- **080A8D50 stopped unmatched — tentative TentacleBody_InitializeSegments.**
  Recovered setup of four segments, count/health-share fields 0x8DA/0x8EA/0x8EB,
  resource and sprite setup, direction mapping and tail status bit four. The
  other segments begin at staggered frame indices zero through three. Each
  segment stores its body owner at animation+0x34. Best attempted listing reached
  68.6% (62 realigned instructions, 19 ignoring registers), below draft threshold.
  The compiler hoists stack-output pointers and argument transforms out of the
  loop, merges the two initial constant-four loads, and changes local-counter
  and register lifetimes. The assembly's stack counter is initialized to zero,
  tested each iteration and incremented only in its nonzero branch; straightforward
  scalar C eliminates that branch. A local frame aggregate retains it but still
  misses allocation/scheduling. Scratch sources remain under ignored wip/build;
  no subthreshold production source was added.

All four draft scores were recorded after formatting with
`tools/check.py FILE --nonmatching --record`; each default INCLUDE_ASM fallback
also passed its per-file exact check. Depin on unguarded animation drafts skipped
because they are not exact. Their default assembly checks are not C progress.
The integrator must perform the complete combined ROM build.

Exact code gain for this task: **+0.0157 percentage points** (380 bytes), all in
the Register-pinned category. Drafts contribute no exact gain.

## Task 08148318 — 2026-10-20

Tags: #player #matching #register-pinning

Four matches and two guarded drafts. Exact coverage within this task is 56.2%
(+56.2 percentage points; 1,920 of 3,416 assigned bytes). The batch contributes
+0.0795 percentage points of project code, all Register-pinned. Drafts add no
exact coverage. Only per-file checks were run; combined ROM validation remains
with the integrator.

- 08148318: improved the existing Player_UpdateWaveringMovement draft from
  82.7% to **87.6%**. Writing odd-direction hold durations through the observed
  Player byte array (`unk466[3]`, offset 0x469) instead of the timer pointer
  restores the duplicated branch-specific stores. The same change must not be
  applied indiscriminately to the even-direction path, which retains its timer
  pointer. Remaining differences include RNG result allocation, even-direction
  address scheduling, branch/pool layout and an apparently unused original
  0x0403B714 literal. Barriers around RNG offset/table construction regressed
  to 82.2% and were reverted. Added the required local descriptive alias; the
  fallback uses the address symbol so `--nonmatching --record` finds it.
- 08148B64: new **99.4% draft**, Player_UpdateSpiralDisorientation (tentative).
  Player dispatch table gUnk_08610CA8 references it. Entry reconstructs facing
  from heading/side, emits queued effects and a spiral trail, then alternates
  folded facing using tetherFrame. It sets resource 0x16F, advances the packed
  animation, grows the effect duration to 32, and returns to action zero after
  frame 31. Typed spriteResource/actionVariantResource fields reproduce the
  resource-index shift instead of emitting another literal. Two instructions
  remain: the duration-field offset is loaded into r0 instead of r3. Explicit
  duration-pointer/offset pins disrupted shared 0xFFFF masking or earlier
  scratch-register choices; retained the best compact candidate.
- 08148914: **matched**, Player_UpdateDecayingRecoil (tentative), 592 bytes.
  Consumes an optional contactActionTag sound, folds facing, advances animation,
  applies opposite-facing sine/cosine impulses, and decays magnitude at 0x270
  by three quarters. At zero it clears contactSpecial and selects action 22
  only when byte 0x5B9 is three and 0x5BB is nonzero. Multiplying table sample
  by magnitude, in that source order, fixed most allocation/load-order misses
  (87.9% to 97.9%). Pins on the magnitude and facing address temporaries closed
  the remainder. Sign-preserving shifts reproduce the original rounding.
- 081487B4: **matched**, Player_UpdateTimedMovementRecovery (tentative),
  352 bytes. Subtracts 60 from byte 0x466 while it exceeds 59; otherwise clears
  status bit zero and the byte. Advances sequence one and returns to action
  zero when the movement gate at 0x5BB clears. The shared stepper matched
  immediately; paired offset/address pins fixed the final two instructions.
- 08148608: **matched** the previous direction-recovery draft, 428 bytes.
  Pinning the command pointer to r6 requires separately materializing its
  offset in r0 and table in r1, avoiding an unwanted in-place pointer shift.
  A local facing pointer with an input-only barrier schedules its address
  before the history mask while preserving the typed store and branch merge.
  The final signed movementAnimationState gate is at 0x4F3, not 0x418.
- 08148FC0: **matched** the previous walking draft, now named
  Player_UpdateWalkFootsteps (tentative), 548 bytes. Uses the matched packed
  stepper, alternates nearby positional effects 0x162/0x163 or sounds 0xC8/0xC7
  on animation frame two, and emits a secondary effect for GameState flag one
  at 0x868. Pins retain zero in r5 and position in r4; the saved zero must be
  u32, because an opaque u16 pin introduces extra truncation. Pinning the frame
  address then fixes all remaining scratch-register differences.

Every exact candidate passed a reviewed full-extent `tools/check.py` after
formatting. `tools/depin.py --write` reduced the retained hints to 14, 17, 17,
and 15 for 081487B4, 08148914, 08148608, and 08148FC0 respectively. Run depin
on unguarded C candidates: a guarded draft's default INCLUDE_ASM fallback can
make it strip hints without validating the candidate. The spiral candidate was
restored and re-recorded after encountering that behavior. Both draft scores
were recorded with `--nonmatching --record` after formatting. No make, full
build, network operation or shared documentation edit was performed.

## Task 080EB298 — 2026-10-20

Tags: #matching #scene-actor #ivy #worker

- **080EC0E8 matched** as `SceneActorReaction_FindAdjacentTarget`. The matched
  `Ivy_FindAdjacentTarget` (080F0910) explains all but one call: substitute
  `sub_080EC024` for `SceneActor_FindNearbyPlayerTarget`. Finds the nearer same-level
  player/category actor within the squared-distance limit, checks adjacency in
  terrain cells, optionally turns toward it, and retains its reference at +0xDC.
  Exact check: 536 bytes. Starting with `relatives.py` made this a first-check match.
- **080EB298 matched** as tentative
  `SceneActorReaction_ReacquireAdjacentTarget`. Starts reaction animation control
  (kind 41), checks for an adjacent target after completion or while control kind
  34 is active, shows the drawable and switches to `sub_080E9C20` when found.
  Kind 34 times out after 180 ticks into idle control. A separate no-argument
  idle-state inline helper, rather than passing constant zero to the generic
  state setter, puts the zero initialization after the field-address calculation
  and resolves the last two instruction-order differences. Exact check: 440 bytes.
- **080EB58C matched** as `SceneActorReaction_WaitForAdjacentTarget`. Context
  +0x634 is a signed consecutive-miss counter, +0x636 its signed limit. Entry
  clears the miss counter; exceeding the limit resets the actor state. After
  30 ticks with a current adjacent target it faces the saved target reference,
  resets its reaction state and begins `sub_080E9C20`. The target reference is
  loaded before the search call, as in the ROM. Reusing the combined state/control
  inline shape from 080EFC58, rather than two independent generic setters,
  reproduced offset derivation and register lifetimes exactly. Exact: 344 bytes.
- **080F3230 drafted 83.7%** as tentative `PhaseObject_ConvergeEntries`.
  Four entries (stride 0x2C, starting +0xA8) interpolate x/z from saved points
  (+0x468, stride 8) toward a shared target (+0x488), using delay/15. Their signed
  opacity bytes (+0xB0, stride 0x2C) rise by three and clamp to 64. When unpaused
  (+0xD80 bit 0 clear) and delay reaches zero, it resets entry coordinates,
  enables flags, clears collider bits and installs the next callbacks.
  `--nonmatching --record` recorded the score; default compilation keeps INCLUDE_ASM.
  Remaining: 32 realigned instructions, only two ignoring registers; principally
  r3/r4/r6/r7 allocation in the reset loop and constant ordering for mode stores.
  Keeping separate opacity and flag pointers plus a byte offset reproduced the
  loop structure. Explicit locals for opacity 64 and flag bit 1 improved ordering.
  Whole-object shifted views reproduce base-plus-index followed by +0xC4/+0xC8;
  pointers directly to entries generate different Thumb memory addressing.
  Use canonical `ChildGroup_UpdateAlternatingCallbacks` for 080F40C8: its old
  sub_ name resolved without the Thumb bit in this check environment.
- **080ED4E0 stopped below draft threshold** (best ordinary candidate 68.5%).
  Tentative `Ivy_UpdateCaptureReach`: selects directional animation 5, plays
  cue 0x1DD at sequence index 3/elapsed 0, enables capture collision after elapsed
  6, finds an adjacent target and switches to Ivy hold. Mirrored offset table
  `gUnk_08606384` positions players or actors; context +0x62C bit 1 marks a held
  target. Animation completion without capture switches to 080ED838.
  Best candidate retained at `wip/codex-medium/sub_080ED4E0.c`. Mismatches include
  extra high-register pressure, state/control constant derivation, packed-vector
  construction and branch/field-address ordering. Multiple helper/type variants
  did not get it above the threshold.
- **080F2F7C stopped below draft threshold** (best ordinary candidate 70.1%).
  Tentative `PhaseObject_RelocateEntryOrigin`: mirrors x/z offsets over four
  entries, reduces opacity to a minimum of 1, expands offsets while unpaused,
  then chooses a random origin around the anchor at radius 200 and y+350.
  Saves entry positions, marks entry flags/phases, installs
  `PhaseObject_BeginEntryMotion` and a 30-tick delay. Candidate at
  `wip/codex-medium/sub_080F2F7C.c`. Remaining: 13 structural differences in the
  best ordinary candidate, many allocation/stack-slot differences. The ROM
  keeps the 255 angle mask in a stack slot across Div; the candidate rematerializes
  it, shifting later spill offsets. Register-pinning experiments did not help
  enough and were not retained. Neither below-threshold source was added to src.

No network or full builds used. Per-file byte checks accepted the three matches.
The bounded permuter attempt for 080F3230 failed before searching: export's
`tools/split.py` rejected unrelated `Ghoul_ResumePendingRoute` in
`src/fn/sub_0808CC48.c` as not a known function start. The Ghidra export is absent
(`gen/ghidra/decomp.c`), so these candidates were recovered from assembly and
matched siblings. No register-pinning concession is present in accepted sources.

## Task 081BDE94 — 2026-10-20

Tags: #matching #storage #inventory #worker #register-pinning

Exact code gain: **+0.0729 percentage points** (1,760 bytes). Register-pinned
category: 1,212 bytes; ordinary C: 548 bytes. Three of the six assigned functions
matched; no NONMATCHING drafts remain from this task.

- **081BF67C matched**, named `Storage_InitWidgets`. Called by `Storage_Init`
  (081BFF18), it copies and relocates the 0xCB05/0x5D04 and 0xCB05/0x530D
  resources into +0x1538/+0x1518, initializes Storage's widgets (including the
  two sixteen-slot grids and eight equipped markers), then initializes the input
  marker/glyph pairs at +0x1558/+0x164C. Uses the shared `ContactPackedVector`
  and `SceneInputResource` types. Taking the clear-fill value's address early
  (`zeroPtr`) gives the original stack layout; otherwise the position starts
  four bytes too early and creates extensive allocation differences. Initially
  committed as a recorded 95.9% draft, then superseded by an exact match.
  Four hints survive `depin.py`: the first call's r1 zero plus an empty compiler
  barrier, and r0/r1 pointers for CpuSet. These resolve the final argument-order
  differences without handwritten instructions. Reviewed extent: 840 bytes.
- **081BE16C matched**, `Storage_RefreshItemSlots`. Refreshes two sixteen-entry
  grids at +0x318/+0x918 using mapping bytes +0x1790 and the resource at +0x1538;
  records the first empty slot in +0x1756/+0x1757, applies palette 0x373 to the
  carried grid and hides all eight equipped markers at +0x10F8. First empty
  field is written as unsigned 0xFF but tested through s8; the second starts
  with signed -1. A distinct row-byte offset and widget-base local reproduce
  the first grid's address addition order. The second grid needs an explicit
  advancing slot index, separate column counter and next-row value; otherwise
  agbcc replaces the counter with a descending loop or recomputes the index.
  A named mask initialized before the final flags pointer reproduces the hide
  loop. Two hints remain after depinning: column r9 and second-grid row slot
  r8. Reviewed extent: 372 bytes.
- **081BF1AC matched**, `SceneInventory_UpdateCategoryBrowse`. Select+A cycles
  clock mode; ordinary A saves the grid selection and either starts swap
  selection (mode 1) or checks whether the selected item can be removed (mode
  2), opens its confirmation and updates the palette/state. B returns to mode
  selection; grid navigation updates the cursor and description. A named
  pointer to the saved cursor gives a +0x14B8 base with flags at +8; a direct
  field expression instead synthesizes +0x14C0. Positive eligibility branches
  and a shared `allow` label reproduce the branch order. Set the consumed
  eligibility flag after the consumable-equipped call to avoid an extra saved
  register. Use canonical `Storage_UpdateSwapSelection` for callback 081BF04C:
  the old sub_ name resolved without its Thumb bit. No hints remain after
  depinning. Reviewed extent: 548 bytes.
- **081BE504 stopped below draft threshold**, best 49.6% (candidate-length
  score). Equipment refresher also highlights equipped/unassignable carried
  slots with palette 0x374, places equipped markers via 081BD934, and hides the
  unused markers. Ordinary flat/2D typed-widget and raw-pointer variants retain
  different row-base, flag-pointer and marker-offset inductions and stack
  allocation. Best source retained in `wip/sub_081BE504.c`; not added to src.
- **081BE2E0 stopped below draft threshold**, best 70.1% (candidate-length
  score). Consumable counterpart uses id +0x121, checks equipped slots through
  `GameState_IsConsumableSlotEquipped`, highlights those entries and positions
  equipped markers. Explicit row/marker offsets reduced some structural
  differences, but agbcc turns the inner slot pointer into an induction
  variable and the counters into descending loops. Volatile counter, aggregate
  frame and register-placement probes did not improve the retained score.
  Best source retained in `wip/sub_081BE2E0.c`; not added to src.
- **081BDE94 stopped below draft threshold**, best 50.2% (candidate-length
  score). Builds two transition entries for saved/current selections, hides
  their grid widgets and appropriate equipped markers, determines frames and
  enabled flags by category, reverses source positions into destinations,
  begins a twelve-frame pair transition and plays sound 0x38A. Output-array
  indexing produces different pointer induction/stack spilling; explicit
  output pointers and flag views worsened the candidate. Best source retained
  in `wip/sub_081BDE94.c`; not added to src.

All accepted functions were formatted with the venv's clang-format, minimized
with `depin.py`, and checked byte-identically with explicit reviewed extents
using `tools/check.py`. No full build or network operations. Ghidra's export
is absent, so recovery used assembly, callers/callees and matched neighbours.


## Task 0813FFE4 — 2026-10-20

Tags: #matching #player #movement #camera #nonmatching

Five of six assigned functions matched (83.3% by count), adding approximately
0.109 percentage points of total code (2632 bytes). Of that gain, approximately
0.043 percentage points (1044 bytes) belongs to Register-pinned; approximately
0.066 percentage points (1588 bytes) needs no matching concessions. One inherited
99.9% guarded draft retained with the required file-local alias and raw fallback.
Aggregate totals remain in PROGRESS.md. Worked solo on the preclaimed branch;
no network, make, full build or direct git mutations. Each function was committed
separately through build/commit.sh.

- **08140CC4 matched**, Player_UpdateLockedMovement (636 bytes). Relative
  Player_UpdateModifier08Drift at 0823BE50 provides most of the body and its
  local typed layout. Adapted the status predicate and sound call; added the
  mode-three/phase-five/contactMagnitude path, which starts movement opposite
  facing. Bytes 48C-48F hold active/played/timer/direction; signed halfwords
  490/494 hold ten-unit directional components. Three inline signed scaling
  expansions share the sine table; preserving its pointer/result lifetime
  reproduces the register allocation. No hints needed.
- **08140F40 matched**, Player_GetForceDirection (632 bytes), improving the
  inherited 99.7% draft. An empty compiler barrier on the masked signed angle
  retains the original arithmetic shift; assigning offset zero after that shift
  restores instruction order. depin retains angle r0, direction r4 and the empty
  barrier (3 of 3 hints). Compares terrain heights in a direction-dependent
  neighbor, with additional within-cell X/Z corner selection for even directions.
- **08140AC8 matched**, Player_SelectLockedTurnDirection (508 bytes).
  Actor_ProbeDiagonalTurnDir at 0823BC54 ports exactly after replacing its
  probe callee with 081409BC. Called by the locked-movement updater. Probes
  three offset positions for cardinal direction values 0/2/4/6 and selects
  the adjacent odd direction; unsupported input returns -1. No hints needed.
- **0813FFE4 matched**, Player_InitBodyFromScript (444 bytes). Uses packed
  signed sixteen-bit vector fields, reads script keyword p or saved player
  position, samples terrain height including slope adjustment, initializes
  SceneBody, display, movement probe, offset and contact registration. Combines
  the position sampler from 08064338 with the setup sequence of 0823B0D4.
  A switch on modes 0/1/2/6 reproduces the apparently redundant signed lower
  bound check; an ordinary range predicate folds it into an unsigned test.
  Separate early/late body locals reproduce the r4-to-r5 copy once that switch
  is present. Explicitly returns the defined contact-registration result while
  recording body IDs. Correcting the SceneBody_Init entry argument from u16
  to full width removes two unwanted truncation instructions. depin removes
  both temporary body pins, leaving no concessions.
- **0814159C matched**, Player_UpdateCameraOffset (412 bytes), improving the
  inherited 83.7% draft. Split input history from its quantized direction;
  use a signed full-width local for the narrowed atan2 result, separate the
  facing mask from direction computation, and preserve action-mode lifetime.
  depin reduces ten exploratory hints to three: history pointer r2, an empty
  barrier between input copying and adding one, and action-mode r0. These
  retain the original copy/add pair, load allocation, mask scheduling and
  derived field-address literals. Both angle sums remain readable signed C.
- **08141C40 drafted 99.9%**, Player_EmitMovementEffects (576 bytes).
  One resolved branch byte differs at 08141C64: ROM null-player branch skips
  directly to 08141CFC, while C branches to the redundant null test at
  08141C90. Realigned instruction comparison reports zero differences but
  byte comparison correctly rejects the candidate. Explicit goto/nested
  guards, combined suppression inline, and pointer barriers either preserve
  this difference or worsen register allocation and pool placement. Retained
  the strongest original body and existing position r4 pin. depin on the
  unguarded scratch source skips because its base is not exact; it provides
  no hint-minimality claim. Recorded with --nonmatching --record and verified
  the default assembly fallback separately. The comment identifies the residue.

All five exact functions were formatted with build/venv/bin/clang-format and
passed check.py with explicit reviewed body/pool extents (444, 508, 636, 632,
412 bytes respectively in address order). No handwritten instruction bodies.
Combined ROM validation and alias promotion remain for batch integration.
Shared docs/index and progress totals were not regenerated by this worker.

## Task 080FD6D0 — named object terrain constraints and actor-group motion

Tags: #matching #named-object #terrain #actor-group

- 08103F44 matched: NamedObject_PlayAdjacentExitSound. The caller is
  081012EC, after NamedObject_StepTowardTarget reports arrival and the object
  changes to state 3.
  Checks attachment ownership/flags, then four adjacent cells for equal height
  and absence of terrain attributes 0xC3; plays song 0x151 on the first exit.
  Reused the matched SceneActorTerrain cell accessor. Two nested u8 inline
  predicates preserve materialized booleans. Reading x/z through one u16
  position pointer and loading attributes through a u32-returning inline
  removed the final selection/register differences. No pinning.
- 0810271C matched: NamedObject_ClampTetherToPlatform. Ported the matched
  NamedObject_ConstrainTether (081023CC): sample tether cell instead of object
  cell, accept same terrain height unless attribute 0x40 is set, and use bounds
  at 086064B4. Installed for kind 1 by NamedObject_CreateFromScript. No pinning.
- 08102570 matched: NamedObject_ClampTetherAtBarrier. Same family, but its guard
  is terrain attribute 0x80. Installed for kinds 0/3. The rest of the clamp and
  attachment flag 0x200 update is shared with 0810271C. No pinning.
- 080FD6D0 matched: ActorGroup_BeginExtendedMotion. Based on matched
  ActorGroup_BeginMotionPhase (080FD334); changes control to 080F85B0/type 0x27,
  sets group flag 0x1000, copies duration EC4 to timer EBC, sets auxiliary
  sprite position from context+8A8 with mode 0x13D, marks ED0 active, and plays
  song 0x16F. Completion selects behavior 7. Correct context padding was the
  last mismatch. Name follows neighboring ActorGroup routines. No pinning.
- 081021DC retained inherited draft at 86.5%: NamedObject_ConstrainPlayer.
  Independent sibling port converged on the same 33 register-only differences:
  delta pointer r5/r6 and y-difference/reload allocation. Signed state access
  is required. Pointer pin experiments worsened structure and were discarded.
  Kept the inherited readable source, added the required local alias, and
  corrected its default INCLUDE_ASM label to sub_081021DC (the named assembly
  filename is absent in this checkout). Default fallback passes exact check.
- 08101340 drafted 84.5%: NamedObject_UpdateMovementSound, called by
  NamedObject_UpdatePool. Selects songs 0x28A/0x288/0x289 by object kind, using
  manager byte 1378 as the playing latch. A switch rather than if/else prevents
  constant-delta folding and recovers the instruction structure. The 448-byte
  executable body has 17 register-only differences (91.9% candidate-length
  score). Reviewed extent is 528 bytes and includes an additional apparent
  function at 08101500, so the recorded whole-extent score is 84.5%; do not
  mistake the body score for full coverage. Default assembly fallback passes.

Useful layout warning: sizeof(struct NamedObject) from named_object.h is
0x198 in this checkout, while pool stride is 0x19C. Explicit integer stride
arithmetic, as in matched NamedObject_UpdatePool, prevents incorrect indexing
and loop strength reduction. No shared header was changed.

Bounded permuter attempts for the two remaining drafts stopped before search:
tools/split.py rejects unrelated Ghoul_ResumePendingRoute in sub_0808CC48.c as
an unknown function start. Declaration swaps and pinning did not improve the
retained candidates. All four matches were clang-formatted and rechecked with
check.py; both drafts were checked with --nonmatching --record and their
assembly fallbacks checked separately. No full build or network operations.

## Task 0821FF8C — collision, digit sprites, serial and RFU (2026-10-20)

Tags: #matching #collision #serial #rfu #animation

Assigned instruction coverage: 15.7% (+15.7 percentage points; 304 of 1935
instructions). One exact function, five guarded drafts; drafts contribute no
exact progress. The exact function emits 632 bytes, without register pins.
All checks used tools/check.py; no full build or network commands were run.

- **0821FF8C — matched, Collision_CalculateDamageResponse.** Reads source attack
  flags/damage and destination affinity masks; sets response amount and flags,
  applies the nearby-layer bonus, signed scale factors, and a game-state
  percentage adjustment when source kind is 16. The response-zero guard prevents
  recalculation. Useful technique: an inline masked flag accessor puts the mask
  before the first attack load; caching the full attack flags after the damage
  clamp removes the extra canonical register copy. Repeated Nearby/ApplyScales
  inline bodies reproduce the two paths. The diagnostic damage register pin was
  removed by depin, and the formatted source still checks MATCH.
- **08223E28 — drafted 80.9%, RfuManager_ApplyRequestResult.** Callback installed
  by RfuManager_Init through rfu_setREQCallback. Decodes RFU request/result pairs
  into manager state transitions, notifications and disconnect/recovery work.
  Guarded draft: 188 realigned differing instructions, 84 ignoring registers.
  A persistent manager-pointer local initially produced only 62.9%; constant
  manager accesses improved rematerialization and block layout. Mixing persistent
  and constant accesses helped some structural regions but lowered overall
  similarity. Remaining issues include address lifetimes, stack-byte reloads,
  recovery-slot loop allocation, and literal/jump-table displacement differences.
- **08222D60 — drafted 83.3%, existing name Serial_ServiceInterrupt.** Reads all
  four multiplayer words, excludes bit-15 words from the combined readiness mask,
  updates latest/delivered packet double buffers, and stages packets through the
  four-entry ring. Error status -2 is selected by SIOCNT conditions; the tail
  writes the next send word, role and interrupt acknowledgement. Guarded draft:
  57 realigned differing instructions, seven ignoring registers. Writing array
  addresses as index + &global[offset] preserves the original separate offset
  bases; local-array-pointer expressions folded them into immediate offsets.
  Retains one first-word r8 pin and the concession marker. A volatile count local
  preserves the temporary halfword while the queue count is decremented/restored.
  Remaining differences are chiefly allocation and address/pool scheduling.
  Guarded depin mistakenly checked the assembly fallback and removed the pin;
  restored the pin in a corrective commit and rechecked the C score. Unguarded
  depin correctly declines to minimize a non-exact base.
- **082245A8 — drafted 82.1%, RfuManager_UpdateChildAdmissions.** Identifies newly
  connected child slots, starts admission timeouts, checks received game-name
  records against the terminated serial-number list at manager +0x20, accepts
  compatible slots and disconnects rejected/expired ones. No register hints.
  Guarded draft: 52 realigned differing instructions, 18 ignoring registers.
  Separate persistent/present-state pointers and an explicit pending-mask pointer
  recover the spill/hoisting pattern. A final state-pointer reload before testing
  pending admissions improved allocation from 79.1% to 82.1%. The XOR/AND
  expression still becomes BIC; timer addresses and several live ranges differ.
- **082239E0 — drafted 80.7%, RfuManager_UpdateLinkRecovery.** Wraps watchLink,
  records loss/reason masks, arms per-slot recovery timeouts, clears recovered
  slots and disconnects expired ones; returns whether any disconnect occurred.
  Guarded draft: 49 realigned differing instructions, 23 ignoring registers.
  Remaining differences involve pointer lifetimes and loop base hoisting.
  Explicit timeout pointers and an sb hint failed to improve the retained draft.
- **082213B8 — drafted 94.9%, Anim_DrawDecimalDigits.** Callers in menu panels
  pass widget arrays, digit resource/base, width, position and spacing. Repeated
  subtraction emits eight decimal glyph indices, then the requested suffix is
  drawn with optional leading-zero suppression. Guarded draft: 18 differing
  bytes, 12 realigned differing instructions, six ignoring registers; same
  400-byte extent. Explicit cursor plus index reproduces the separate zero-test
  cursor and glyph lookup, and a prechecked do-loop places the cursor after the
  width guard. Four register hints remain; unguarded depin skips this non-exact
  base. Volatile suppression parameter retains both observed stack reads and
  avoids a pinned intermediate incorrectly coalescing the original flag with its
  negation. Residue is initial load/negation scheduling and low-register choices.

Scores above are the final reviewed-extent --nonmatching --record results;
some queue commit subjects used the preceding candidate-length estimate. Every
source has its required local alias and every draft retains INCLUDE_ASM in the
#else branch. Draft experiments remain under ignored wip/ and build/ paths.

## Lun and Bee assignment — 2026-10-22

Tags: #matching #monster #lun #bee #register-pinning

Final result: four exact matches and one guarded draft. This batch adds
0.071 percentage points of exact code coverage (1,712 bytes secondary):
0.040 points unpinned (964 bytes), 0.031 points in Register-pinned (748 bytes).
Only per-file check.py checks were used. No make, full build, network operation,
shared documentation change, ROM commit or generated assembly commit occurred.
Every accepted source has a file-local descriptive alias. Existing symbols had
no names for these ten targets; neighbouring prefixes and static callers guided
the tentative names. Ghidra export was unavailable in this worker checkout.

- **0809AE0C — stopped below draft threshold.** Fresh tentative
  Lun_ApplyCaptureContact candidate scored 40.3%, 186 realigned differences /
  114 ignoring registers. The attack-collider callback requires contact kind 1,
  rejects disabled actors, checks the player's vertical reach, sets a lift state,
  disables both colliders and applies a speed bonus or status effect according
  to actor+0x1E6. The original retains discarded context+0x3D4 bit calculations;
  a volatile read preserves the read but loses the discarded AND. State-store
  constant derivation and actor/player pointer lifetimes also remain different.
  Read the caller Lun_InitColliders (0809CBB8) and prior notes before drafting;
  no below-threshold C was promoted.
- **0809B7FC — drafted 82.6%, Lun_ResumeAfterLift.** Consumes the animation
  entry flag, invokes the lift penalty, marks response flags, and after tick 30
  either restores the pursuit timer from context+0x442 to +0x444 and starts
  movement toward the player, or selects Lun_UpdateWaitingAnimation and resets
  direction/scale. Reviewed-extent --nonmatching --record result: 42 realigned
  differences / 19 ignoring registers. The control and state stores' constants
  and offset derivation differ; simple mask-pointer rewrites regressed and were
  reverted. INCLUDE_ASM remains the default branch.
- **0809C174 — matched, Lun_DecayRouteMotion (Register-pinned).** Applies
  context+0x424/+0x428 to actor motion at +0x64/+0x68, halves the signed drift
  every four ticks, and after control completion chooses pursuit or rest from
  flag 0x100. An explicit pointer to motion.z recovers its address increment;
  a mask helper taking the actor (rather than the field address) recovers mask
  setup order. The resulting 97.7% draft had only four register differences.
  Pinning mask to r1 and loaded flags to r0 with empty operand barriers closes
  those differences. No handwritten instruction body. Exact check: 356 bytes.
- **0809CCF4 — stopped below draft threshold.** Tentative
  Lun_AdvanceRouteCurve copies endpoints at context+0x414/+0x41C, clears their
  vertical component, evaluates a cubic using +0x3E0 progress and signed +0x3E4
  tangent, writes +0x3F4 and advances progress by 128 until complete. Best
  retained scratch: 63.9%, 98 realigned differences / 51 ignoring registers.
  A caller aggregate containing zero/start/end reproduces the initial stack
  offsets, and a shared drift term improves the initial 33.2% attempt. Coefficient
  common-subexpression lifetimes, arithmetic ordering and spills remain. An
  explicit doubled/tripled-coefficient experiment lowered structural differences
  to 37 but worsened total allocation; the better fuzzy candidate was restored.
- **0809D574 — matched, Lun_PursuePlayerPosition (unpinned).** Adapted
  Lun_FollowRoute (0809BE98): its initial body and approach calculation are the
  same, but callback +0x270's return is ignored and arrival resets endpoints to
  the player's current position before facing it. Reusing a State inline with
  an explicit behavior constant recovers the ROM's adjacent state-offset
  derivation. Exact check: 592 bytes.
- **0809D7C4 — matched, Lun_FollowPlayerDisplacement (unpinned).** Saves the
  player's position at context+0x414 on entry, invokes the participant action,
  adds the player's frame displacement to actor motion, and refreshes the saved
  position. Completion switches behavior and idle control. The first scalar
  draft optimized away the stack delta; an inline Delta restores it. Declaring
  its destination pointer before the saved/player pointers improves 86.5% to
  99.5%; declaring a global-pointer address local before that destination fixes
  the final two load-order differences. Exact check: 372 bytes. Earlier guarded
  draft is superseded by the exact matching commit.
- **0809DA14 — matched, Lun_RenewRouteDestination (Register-pinned category).**
  Ported the route-selection tail of Lun_ChooseMovementDestination (0809C2D8),
  removing its entry-control and height checks. The timer advances only while
  waiting (the else branch), rather than after selecting the new route. Carries
  the seed's r7 clobber and its MATCHING_CONCESSION marker. Exact check: 392 bytes.
- **08093138 — stopped below draft threshold.** Tentative Bee_AdvanceRouteCurve
  shares the Lun cubic core, using endpoint offsets +0x470/+0x478 and output
  +0x450. Progress at +0x430 advances by a random 256..511, clamps at 1024,
  then resets and returns completion on the next call. Scratch: 65.6%,
  99 realigned differences / 48 ignoring registers. The caller
  Bee_BeginMoveToPosition (08093370) establishes the endpoints/trajectory role.
  Same coefficient/spill problem as 0809CCF4; no qualifying draft.
- **0809345C — stopped below draft threshold.** Tentative Bee_BuildRouteCurve
  samples eight curve positions at context+0x3E0 with parameter increments
  Div(1024,8), using tangents +0x480/+0x484 shifted left four, then stores the
  destination at +0x420. Built from the same cubic helper with the additional
  initial-tangent factor and t-cubed minus t-squared term. Scratch: 37.2%,
  232 realigned differences / 37 ignoring registers. Most remaining differences
  are allocation, loop address lifetimes and spills, but extent also differs;
  not suitable for promotion.
- **080941F4 — stopped below draft threshold.** Tentative Bee_FollowTargetRoute
  starts from the committed Bee_FollowWanderRoute draft (08093810). It recomputes
  facing to the object at context+0x440 on entry, each update and route restart;
  randomizes within 63 heading units, uses radius +0x43E, starts the movement
  budget at 1280 and restarts with +0x42C << 5. Scratch: 53.0%, 303 realigned
  differences / 145 ignoring registers. Read BIG_FUNCTIONS.md and used realigned
  counts; stack-vector/pointer lifetimes remain substantially different.

Scratch attempts remain ignored under wip/; below-threshold functions have no
new production source. All four exact files passed byte-resolved check.py;
combined ROM validation remains the integrator's batch step. Intermediate
97.7%/86.5% draft commits for 0809C174/0809D7C4 are superseded by exact commits.

## Bee, Crow, Sword round 3 — 2026-10-22

Tags: #matching #monster #bee #crow #sword #register-pinning

Six exact matches, two new guarded drafts, and two improved existing guarded
 drafts. Exact contribution: 0.0715% of total code (+0.0715 percentage points;
1,724 bytes secondary). Of that, 0.0196 points are unpinned (472 bytes), and
0.0519 points are Register-pinned (1,252 bytes). Every accepted source has its
file-local descriptive alias; existing symbol names were reused. Only per-file
check.py validation was used, with reviewed target extents. No make, full build,
network operation or generated-assembly/ROM commit. Batch ROM validation remains
the integrator's step. Ghidra export is unavailable. Scratch variants remain
ignored under wip/ and build/.

### Exact matches

- **080F6370 — matched, Sword_UpdateComponentControl (Register-pinned).**
  Component update skips disabled components, decrements their countdown, runs
  the pre-collision callback, refreshes part colliders, and runs the second
  callback. The adjacent Sword_UpdateComponents dispatch and the collider
  updater establish the component role. A mask-taking inline gets the initial
  mask-before-load shape; r1 mask/r0 flags pins resolve three register choices.
  Exact reviewed extent: 84 bytes.
- **080F4FA8 — matched, Sword_UpdateHorizontalWaveReturn (Register-pinned).**
  Starts from Sword_UpdateHorizontalWaveFlight, uses sine index
  (256 - timer) & 255, decrements the timer, and marks completion at zero.
  Sword_DecelerateReturn selects this callback by trajectory direction.
  Pinning the completion value to r0 after the rotation store preserves the
  existing offset derivations; accumulating flags into that value fixes the
  final OR destination. Exact reviewed extent: 228 bytes.
- **080F508C — matched, Sword_UpdateDepthWaveReturn (Register-pinned).**
  Depth-wave sibling of 080F4FA8, with the sine displacement on Z. Same completion
  concession, independently checked. Exact reviewed extent: 228 bytes.
- **080F5C9C — matched, Sword_DampTargetReturn (unpinned).**
  Adapted SpinningApproach_DampReturnMovement (0818DC50). Every five ticks it
  halves horizontal drift; once stopped it rebuilds the segment from the target
  at component+0x15C, snapshots that target, reverses turn direction, and selects
  TargetMotion_AdvanceFollowSegment. Local layout has callback+0x17C,
  timer+0x184 and turn direction+0x18A; the first port omitted the +0x180 word,
  leaving exactly four wrong offset constants. Correcting the layout matched.
  Exact reviewed extent: 472 bytes.
- **080F5F24 — matched, Sword_BeginRestOrbit (Register-pinned).**
  Adapted Armor_InitPartStretch (0818DFF8). Initializes angular spacing from the
  parent component count/base angle, positions each component 200 units around
  the parent's position, sets the tracked target pointer, and dispatches its
  pulse callback. Resting-state interpretation is explicitly tentative above
  the alias. Pinning the RNG table, the shared one-to-minus-one value, callback
  store and dispatch addresses resolves the remaining register/derivation
  choices. Exact reviewed extent: 404 bytes.
- **080F5B68 — matched, TargetMotion_AdvanceFollowSegment (Register-pinned).**
  Improved the existing 87.3% draft. The interpolation and target correction
  already matched; all residue was the completion tail. Keeping a shared r0
  completion value, separately materializing +0x11F, and pinning the field and
  callback addresses reproduces the exact stores without instruction bodies.
  All values are initialized. Exact reviewed extent: 308 bytes.

### New and improved guarded drafts

- **080A0E00 — drafted 99.2%, Crow_ApproachPlayerThenRecoil.** Tentative name.
  Starts from Lun_PursuePlayerPosition's control, scale and approach inlines.
  Entry snapshots the player at context+0x3FC, computes approach durations in
  +0x42C/+0x430 and vertical correction in +0x42E, and plays sound 247.
  Arrival/terrain contact starts recoil at twice the movement speed. Replacing
  a velocity-vector pointer with individual observed offsets improves 71.2%
  to 98.6%; a mask-taking context helper and r1 depth-result pin reach 99.2%.
  Four realigned differences remain: scheduling of zero before the depth store
  and loading approach speed relative to the actor motion-address calculation.
  Explicit pointer/zero barriers and revised Approach arguments regressed and
  were discarded. Reviewed extent: 1,012 bytes, candidate also 1,012.
- **080F5620 — drafted 94.6%, Sword_AccelerateReturnTurn.** The thrust-retraction
  callback (080F558C) selects this after its return finishes. Adapted
  ActorPhase_AccelerateTurn (080F542C), replacing angle target and completion
  actions. Six differing instructions remain in the hoisted callback/state
  address derivation order. Typed callback fields, u8/u32 inline parameters and
  explicit pointer locals did not improve it. Reviewed extent: 184 bytes,
  candidate also 184. No register pinning in this draft.
- **080A11F4 — improved draft 86.6% to 89.0%,
  SceneActorControl_UpdateHeightReturn.** Reusing the existing symbol name.
  Reading ground height before forming the signed vertical difference and
  writing the explicit phase field reduces realigned differences from 30/14
  to 22/10 (total/ignoring registers). Ground-height load scheduling and final
  state/phase offset derivation remain; candidate 332 vs reviewed 340 bytes.
- **080F6F90 — improved draft 84.8% to 85.0%,
  Sword_InitAttackComponents.** localsweep's removal of the `state = &sequence`
  local reduces realigned differences from 115 to 114. Removing the component
  sprite local instead regresses to about 74%, so it stays. Animation mask
  lifetimes, collider argument addressing and register allocation remain.
  Reviewed extent: 1,224 bytes. Preserves its existing register concession.

### Stopped attempts; existing production sources retained

- **08093138 — below threshold, 66.2%.** Retested the prior Bee_AdvanceRouteCurve
  scratch against 476 bytes: 94 realigned differences / 43 ignoring registers;
  candidate is 488 bytes. Cubic coefficient lifetimes/spills remain. No draft.
- **0809345C — below threshold, 37.2%.** Prior Bee_BuildRouteCurve scratch still
  has 232/37 realigned differences at the reviewed 488-byte extent. The sampler
  loop and cubic coefficient allocation remain unsuitable for promotion.
- **080941F4 — below threshold, 53.3%.** Prior Bee_FollowTargetRoute scratch has
  298/140 realigned differences against 1,128 bytes, candidate 1,140. Stack
  vectors and waypoint-address lifetimes remain; no new production source.
- **080924EC — existing draft retained at 86.5%.** Typed context flags did not
  change code; localsweep's removal of the flags pointer regressed. The exit
  duplicates the context flag address, with additional low-register differences
  and zero-store scheduling. Best remains 27/6 realigned differences.
- **080929A4 — existing draft retained at 86.7%.** Removing the r8 context pin
  regressed to 71.1% (89/39 realigned differences). Stack vectors, control setup
  and state-store derivations remain. Keep the original pinned candidate.
- **08093810 — existing draft retained at 82.1%.** Replacing byte-offset route
  addressing with typed ContactPosition array indexing was code-identical:
  130/71 realigned differences. Original guarded candidate retained.
- **080A06E8 — below threshold, Crow_StopAtRaisedTerrain.** Adapted
  RouteActor_ReflectAtRaisedTerrain (0809D2A8): sums position, secondary motion
  and motion, evaluates terrain height, and clears movement on a raised cell.
  Unpinned best was 76.4%, 49/19 realigned differences, candidate 404 vs reviewed
  396 bytes. An r7 context pin produced an incorrect SP-relative derivation
  (r7/frame-pointer interaction); rejected despite a higher coarse score.
  A target pointer pin lowered structural residue but worsened total allocation.
  No qualifying draft was committed.
- **0809FC74 — existing draft retained at 98.7%.** Two differing instructions
  select r2 instead of r3 for the +0x3ED timer-store offset. Explicit address
  pins hoist the RNG mask and perturb the prologue; extending the RNG table's
  lifetime also regresses. Original guarded candidate retained.
- **0809EF50 — existing draft retained at 96.1%.** Body matches; prologue and
  epilogue omit r7 preservation even though the existing r7 flag hint uses it.
  An r7 clobber does not repair that compiler behavior; unpinning flags regresses
  to 88.2%. No exact claim or source change.
- **0809F510 — existing draft retained at 95.6%.** localsweep removing the
  entry-flag address local worsens realigned differences (23 to 30 in its
  legacy-length comparison). Entry-flag spill and control/reset ordering remain.
- **0809F84C — existing draft retained at 91.5%.** Rewriting the Minimum ternary
  as an explicit branch was code-identical (50/14 realigned differences).
  Retained the defined vector padding rather than introducing uninitialized
  scratch bytes to mimic the original paired-word copy.
- **080A09DC — existing draft retained at 96.1%.** Mask accumulator and pinned
  height-store offset experiments regressed to 93.4%. Ground-height temporary
  destinations, derived +0x2EA constant and literal-pool extent remain.
- **080F4CFC — existing draft retained at 97.4%.** Explicit pinned signed-load
  zero indices grew the function to 324 bytes and regressed to 86.7%.
  Original has only four differing register choices around the X/Z magnitude
  loads. Existing ActorPhase_BeginLunge name remains appropriate.
- **080F52C0 — existing draft retained at 95.5%.** Pinning the RNG table and
  minus-one value regressed to 82.0%; original table register, minus-one copy
  and four-byte extent deficit remain. No replacement retained.
- **080F7458 — existing draft retained at 91.4%.** Pinning the clamped kind to
  r1 regressed to 84.9%. Initial kind reload/store destination, pointer
  rematerialization and associated palette/animation constants remain.

### Large fresh initializers: inspected, no retained C candidate

Read BIG_FUNCTIONS.md, generated condensed listings, inspected initialization,
script-option and callback-registration sections, and queried matched relatives.
These were stopped at static triage rather than presenting partial initializers
as useful drafts; no byte-exact or fuzzy progress is claimed for them.

- **08094764 — Bee_Init, 1,559 instructions / 3,548 bytes.** Best matched
  relative covers only 11% (Bee_ResetRoute, principally the animation stepper
  and terrain query). Sprite/tile allocation branches, extensive script options,
  shadow/collider initialization, behavior-table installation and registry
  snapshot/registration still need a complete candidate.
- **080A1440 — Crow_Init, 1,334 instructions / 3,024 bytes.** Best relative
  covers 11% (Mimic_ResetPursuitDrawable, mostly animation playback). Its final
  setup installs Crow action/behavior tables and command/target/destroy
  callbacks, then snapshots registry state and registers the actor. No complete
  initializer candidate was constructed.
- **080F67AC — tentative Sword_InitializeBodyAndColliders, 846 instructions /
  2,020 bytes.** Best relative covers 25% (SingletonSlot7_InitDisplaySprites,
  shared animation playback). The body sets up parent-target state, animation
  and palette buffers, shadow, A/T/p/K/V script options, four child collider
  pairs, and optional TargetRegistry event registration. The already-drafted
  Sword_InitAttackComponents provides a stepper idiom but does not cover this
  initializer's distinct body/script/collider setup. No partial C promoted.
