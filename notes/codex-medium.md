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
