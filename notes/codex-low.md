
## Task 080DE6E0 (2026-10-20)

Tags: #matching #dog

- 080E059C: matched as Dog_ResumeAfterReaction (tentative interpretation). Entry consumes field2B2, selects animation 11, clears field2B3; completion resumes group control (flags2D4 bit 0x80) or idle. Existing Chase_SetCtl/Chase_SetBehavior reproduce the reset sequences exactly. Two necessary register pins control the motion mask and group flag insertion; depin retained both. Formatted source passed check.py with zero differing bytes (436 bytes).
- 080E4834: drafted 82.3% as Dog_ResumeRoute. Takes arrival, invokes control, attempts route toward context+0x6D8, then selects group chase or saved route. Uses shared chase/terrain helpers. Reviewed full-extent check reports 76 realigned differing instructions, 23 ignoring registers. Candidate is 12 bytes longer; helper expansion and literal/register placement remain. The queued commit title uses the preliminary candidate-length score 81.7%; the source's recorded full-extent score is authoritative.
- 080E25FC: inspected largest first with asmat, relatives and condensed listing. 1976 instructions; matched relative 080E4468 covers only 38%, largely repeated terrain lookups. Entry plays song 0x211 or 0x13E and tests motion/action flags. No complete candidate produced; stopped without a promising close port.
- 080E0750: inspected asmat, relatives and condensed listing. 1875 instructions; best relative covers 32%. Entry consumes field2B2 and quarters saved motion components before selecting directional animation 3. Four repeated terrain-boundary/height runs identified. No complete candidate produced; stopped without a promising close port.
- 080DE6E0: inspected full assembly. Animation-frame sound lookup from gUnk_086061E4; frame kinds 1/3 and frames 1/5/7 can spawn randomized SpriteEffectController effects behind actor direction. Vector stack construction and effect argument layout need reconstruction; no candidate retained.
- 080DEA7C: inspected full assembly and caller Dog_FinishFrame. Updates body/auxiliary collider positions and sizes, rounded to cardinal direction; context+0x678 bit 2 chooses enlarged offset/extents. Uses packed vector word updates on stack; no candidate retained.

Ghidra pseudo-C unavailable: gen/ghidra/decomp.c is absent. No full build or network operations performed. Only per-file check.py acceptance used; integration must validate combined ROM.

## Task 08134398 (2026-10-20)

Tags: #matching #scene-actor #effects

- 08134398: retained and named the existing guarded draft as SceneActorReaction_RecordDefeatTransition, recorded 91.5%. Matched relative SceneActorReaction_DefeatRecordStats explains 93% of the assembly. Defeat switches to control kind 17 and records saturating category/weapon counters. Check reports 232 realigned differing instructions, only 5 ignoring registers. Pinning every STAT_ADD offset to r5 made the compiler share substantially more code and regressed to 53.1%; reverted. Remaining prologue stores and counter-offset allocation need work. Existing register concession retained.
- 08135F70: retained and named the existing guarded draft as SceneActorReaction_ApplyPendingHit, recorded 84.4%. Pending damage changes health, palette, HUD, damage display and hit flags, then calls 08134398. Existing draft reports 96 realigned differences, 20 ignoring registers. Pinning mode to r10 did not improve it (97/22); reverted. Added the required register-concession marker for existing local bindings.
- 081363F4: stopped without a retained draft. Inspected assembly, relatives and condensed listing; assembled an exploratory candidate from 08135F70's shared hit-flag decoder. This variant releases a contact node, calls destination-jump/escalating-hit logic, clears fields at actor+0x428/0x434/0x438, then updates collider flag 0x100. Best initial candidate scored 64.7%, 298 realigned differences / 154 ignoring registers; corrections and pointer/register variants did not improve it. Needs a dedicated +0x3D4/+0x420 view and closer reconstruction of the two cleanup paths. Exploratory source discarded below 80% threshold.
- 08138B18: matched as ParticleEmitter_UpdateScatterTrail (tentative emitter identity). Every fifth tick spawns three effects at position+random Z offsets (31/31/63 masks), plays song 0x16E, moves X by -0x4000, updates spatial collider and switches to ParticleEmitter_OnStop after tick 60. Ported argument layout and random sampling from SpriteEffect_HideWithScatterBurst. Explicit Mod call was essential (C % compiled to unresolved __umodsi3). Exact check: 452 bytes, zero differences.
- 08138778: new guarded draft PositionEffectDisplay_UpdateSources, recorded 90.6%. Copies source positions into four display slots, decodes packed frame/flip/timing commands and hides/deactivates slots configured to stop at sequence end. Seeded decoder from AnimatedParticleManager_Update; biased cursor at task+0x44, separate animation/sprite cursors, and pointer barriers improved candidate-length similarity from 75% to 90.1% (reviewed extent 90.6%). Remaining loop setup/order, constant-one hoisting, pointer spills and register placement: 25 realigned differences, 16 ignoring registers. Uses marked register pinning. Candidate is four bytes longer than the ROM extent.
- 08138CDC: matched as ParticleEmitter_UpdateFlight (tentative emitter identity). Initializes position/velocity relative to selected player and player offset; integrates motion, projects the sprite against camera X, steps animation and switches to 08138B18 on HeightProbe_IsBelowLimit. Existing ActorAttachment_StepSprite inline matched the animation region. Initial full candidate scored 98.7%; an inline TestMode(mask) reading the globals internally moved mask materialization before the global loads and closed the five-instruction difference. Exact check: 772 bytes, zero differences. Register-concession marker covers the shared playback inline's hints.

Two exact matches and three guarded drafts committed separately through build/commit.sh. Existing drafts were refreshed and named, not newly reconstructed. Ghidra export is absent. No full build, network operation or shared-doc edits performed; combined integration validation remains with the orchestrator.

## Task 080D1784 (2026-10-20)

Tags: #matching #mummy #scene-actor

- 080D1AFC: matched as Mummy_ResumeSavedTarget. Mummy_Init installs this callback; nearby named functions establish the Mummy prefix. Entry selects animation zero, waits past frame 42 and field2B3, snaps X/Z to the saved route point, and resumes chase control kind 0x25 (flag 0x80) or saved target kind 1. Clears the target pad on the ordinary path and sets field2C2 to Div(64, 3). Seeded from Bat_ResumeSavedTarget. An explicit shared rewind label reproduced `beq clear; b exit`; returning on the negated comparison emitted the opposite branch. Pinning the group flag mask to r0 fixed the remaining OR operand ordering. Exact check: zero differing bytes, 468 bytes. Marked register-pinning concession.
- 080D1CD0: guarded draft Mummy_UpdateHiddenRecovery, recorded 94.2% (22 realigned differences, 14 ignoring registers). Always hides the drawable, consumes field2B2, disables the shadow on entry, then after frame 26 inlines directional selection 8 / flags 4 and installs Mummy_UpdateRevival, kind 13. Seeded animation inline from SceneActorAnimation_SelectDirectionalEntry; its early returns must become exits from the inline only, so the revival transition still executes. Keeping the prepared-field pointer live through the inline captures the ROM's sl lifetime. An explicit u8-returning take helper reduced structural differences to 2 but swapped actor/flags registers and regressed fuzzy to 76.9%; an actor r4 pin also regressed. Retained the better manual take form. Existing animation register hints are marked. Default guarded fallback checked byte-identical.
- 080D3878: guarded draft Mummy_UpdateDefeat, recorded 86.2% (87 realigned differences, 34 ignoring registers). Mummy_Init installs this alongside 080D2584. Ported Bat_UpdateDefeat (73% instruction-run coverage), replacing drawable alpha changes with hide/show, collision/height updates and a motion-flag-dependent position-pattern/stimulus path. Calls terrain script after either effect path; then calls the actor script and records the event before returning to idle. Keeping the motion-flags pointer across the burst improved similarity from 82.2% to 86.2%. Remaining differences concern route-control address materialization, collider lifetime, stack spill and register placement. Burst table register pin inherited from the matched bat source and marked. Default guarded fallback checked byte-identical.
- 080D1784: exploratory candidate stopped at 61.4% (177 realigned differences, 82 ignoring registers), discarded below draft threshold. Consumes entry, clears route control and selects animation 7; holds mode/frame at zero through frame 59, then restores the interrupted route and delayed target. Shared Chase_SnapToRoute and Chase_TryRoute explain the terrain and route checks, but the two control-reset paths create many PRE address lifetimes/spills unlike the simple helper expansion. Needs closer reset ordering and explicit pointer lifetime reconstruction.
- 080D2194: exploratory candidate stopped at 60.8% (239 realigned differences, 75 ignoring registers), discarded below threshold. Sets flags2D6 bit 16; after animation completion branches on flags2D4 bit 2 (group+0x208 callback, kind 7), bit 0x80 (group+0x48, kind 0x25), or ordinary delayed route restoration. Ordinary route path saves entry target with delay 40, terrain-snaps the saved route point, and falls back to idle on route failure. Shared chase helpers reproduce much of the shape, but branch/reset order and PRE-held field pointers remain unresolved.
- 080D2584: inspected largest first using asmat, relatives and condense. 2139 instructions; best relative 080E4468 explains 30%, mostly four terrain-height runs. Entry consumes field2B2, quarters signed saved motion fields 0x2EC/0x2F0 toward zero, selects animation 3, and can play songs 0x12A/0xFD. Repeated paired route probes, chase transitions and two major repeated behavior paths dominate the body. Local `bl _080D385C` and `bl _080D385A` are far branches into the common epilogue/store tail, not separate callees. No complete candidate produced; stopped without a close whole-function port.

One exact match and two guarded drafts committed separately through build/commit.sh. Ghidra export is absent. Only per-file checks used; no full build or network operations. No generated assembly retained. Combined validation remains with the integrator.
# Task 080D812C — 2026-10-20

Tags: #matching #scene-actors #dog #register-pinning

- 080DDEC8 matched as `Dog_RestoreRoute` (608 bytes). Ported the 92%-covered Golem_RestoreRoute sibling. Dog uses a 20-frame saved chase delay rather than 30 and sets group+0x40 bit 4 after extending the chase timer. Addressing that flag through `group + 0x40`, rather than actor+0x414, made the remaining instructions exact. Depinning retained one necessary register hint. Formatted and checked exactly before committing.
- 080DE57C matched as `Dog_ResetActionState` (336 bytes; interpretation tentative). Clears context fields 0x678, 0x690, 0x6A0 (to FFFF), 0x6A4, 0x6A6, 0x6A7 and eight bytes at 0x6D8, resolves route position, selects directional animation zero, then steps its drawable sequence. Reused the matched 080EBD0C sequence inline. A typed local context view fixed all initial offset-selection differences. Depinning removed one of fourteen hints; thirteen remain. Formatted and checked exactly before committing.
- 080DE128 stopped with an untracked candidate in wip/sub_080DE128.c and a wrapped checkpoint in wip/sub_080DE128-reviewed.c. Target-facing angle, range and timer tests select response packets 0x75, 0x82 or 0x87, otherwise call 080DF400 unless context flag 8 is set. Candidate-length score was 85.4%, but the recorded reviewed-extent score was only 72.2%, so no production draft remains. A draft was mistakenly queued before checking that recorded score, then withdrawn in a follow-up commit. Important boundary finding: gen/code_sym.s labels 080DE42C inside this extent as .byte data although those bytes begin a separate push-r4-r7-lr routine and end before 080DE550. This needs boundary review before resuming; do not count the apparent main-body similarity as a full match. Main-body residuals include branch placement, mask-256 lifetime and state-reset allocation.
- 080D9B20 stopped after an initial C attempt (27.0% candidate-length fuzzy; realigned 662 / 546 structural differences). Four directions repeat same-height / attribute-mask-0x43 cell checks. Properties retrieval is shared with 08123194; the guessed adjacency inline lost the original materialized booleans and retained cell comparisons. Candidate remains untracked in wip/sub_080D9B20.c and needs the actual inline contract recovered before allocation work.
- 080D812C inspected first with asmat, relatives and condense; no useful complete candidate recovered. Shares 76% of instruction runs with Ivy_UpdateDefeat (080EE1B0), particularly the large statistics tail, but entry/route/reaction handling differs substantially. Condensed listing is under build/codex-low-080D812C.txt. Ghidra export is absent. Prior docindex evidence flags three far-branch tails, which must be included when resuming.
- 080DE6E0 inspected and stopped without a candidate. On sequence elapsed zero, sets actor flag 0x10000, emits effects for animation kinds 1/3 at frames 1/5/7, then returns a halfword from 086061E4 indexed by frame and chase flag. Random position construction partially updates packed stack words, and the observed y lane is read before the later explicit y assignment; recover the packed-vector initialization/contract before drafting. Helpers in SceneSteeringActor_EmitRandomParticle cover the trig/scatter portion, but only 17% overall.

Only per-file check.py checks were run; no full builds or network operations. Exact additions total 944 bytes, both in the register-pinned category. No retained production drafts.

## Task 0813214C (2026-10-20)

Tags: #matching #scene-actor #reaction #register-pinning

- 08134398: stopped at the existing 91.5% guarded SceneActorReaction_RecordDefeatTransition draft (232 realigned differences, 5 ignoring registers). Read the large-function workflow and reviewed condensed assembly and the prior worker notes first. New experiments pinning only the counter read offset to r5, leaving the write offset natural, changed cross-jumping substantially (759 structural differences); rejected both pinned and natural-offset/barrier variants. No source change retained.
- 081363F4: stopped without a retained candidate after reviewing assembly, condensed listing, relatives and the earlier 64.7% attempt. Entry rejects flags2D4 bit 0x4000; without a pending hit it calls 08132904, clears control timers and releases the contact node. Its +0x3D4/+0x420 control state and distinct cleanup paths still need a dedicated typed view. The earlier shared-hit-decoder port is not close enough to resume by allocation tuning alone.
- 08135F70: improved existing SceneActorReaction_ApplyPendingHit guarded draft from 84.4% to 86.9% by pinning the mode-test constant `four` to r4. Reviewed-extent check: 82 realigned differences, 20 ignoring registers (previously 96/20). Pinning flags2D4 to r8, removing the updated-value r5 pin and extending mode lifetime all regressed. Remaining issues include health/field2D8 pointer spills, mode lifetime and the constant-one lifetime across HUD/display calls. Separate draft update committed.
- 08132904: new guarded SceneActorReaction_ApplyAreaDamage draft, 83.5%, 100 realigned differences / 34 ignoring registers. Checks action eligibility, scales environmental gain at the actor position (link-mode variant when mode mask 0x4800 is active), generates two randomized probe components, applies health damage and display changes, then chooses defeat kind 17 or saved reaction kind 18. Seeded the palette/HUD/defeat sequence from the newly matched 08132490 and stagger sibling 0813214C. The damage halfword is collider+0x3E (actor+0x172), distinct from ContactCollider.responseValue at +0x44; explicit offset access corrects this layout mismatch. Branch-local position/max-health pointer creation preserves the ROM's duplicated argument setup. Removing the actor r6 pin improved structure significantly; a collider r8 binding helped. Remaining pointer spills, RNG temporaries and reset allocation prevent exactness. Separate draft committed.
- 0813214C: new guarded SceneActorReaction_ApplyStaggerHit draft, 99.8%, two realigned structural differences: `movs r7,#0` occurs after the field2B8 address computation rather than before the flags2D4 store. Applies pending damage and chooses defeat kind 17 or SceneActorReaction_PlayStaggerBurst kind 33, finally setting flags2CC bit 0x2000. Ported 08132490; explicit callback temporary and mask/result lifetime hints reproduce the other instructions. Attempts to force a separate early byte-zero value regressed. Separate draft committed.
- 08132490: exact match as SceneActorReaction_ApplyTerminalHit (tentative name). Applies pending damage, palette flash, timed HUD update and damage number; clears hit timers; at zero health hides the drawable and installs defeat control kind 17, otherwise plays song 0x13E. Reused ShouldShowDamage and Reaction_SetControl/Hide from the matched family. Loading HUD arguments into locals in assembly order, an inline CpuSet zero-fill wrapper, and computing the zero before the flags store closed the structure. Depin removed four of six hints; two empty value barriers remain under the register-pinning concession. Formatted final check: zero differing bytes, 696 bytes. Separate matching commit queued and verified in git log.

Only per-file check.py checks and tools that use that checker were run; no make, full build or network operations. New exact progress is 100% of 08132490's body (696 bytes); project-wide percentage reporting remains with batch integration. Ghidra export remains unavailable. Existing 08134398 draft was retained unchanged; 081363F4 remains assembly. Integration must validate the combined ROM.

## Task 080E25FC (2026-10-20)

Tags: #matching #dog

- 080E25FC: inspected largest first; earlier notes already identify the song-selection entry and repeated terrain lookups. Best matched relative 080E4468 covers 38%; no Ghidra export exists locally. No complete candidate retained; stopped without a close port.
- 080E4B34: inspected relatives and condensed assembly. Best relative 080E4468 covers 39%. After control/range callbacks and tick 20, clears group flags 6, installs RestoreAfterDelay kind 14, saves group control/delay and enters state 3/1/0. Then repeats route snapping, player/link-mode target selection, terrain lookups and rerouting. Shared Chase_Arrive logic is relevant, but substantial surrounding state logic remains; no complete candidate retained.
- 080E4834: existing Dog_ResumeRoute draft retained at 82.3%, 76 realigned differences / 23 ignoring registers. Changing the group flag insertion to the signed full-width bitfield helper produced identical differences; reverted. No new draft commit for this already-recorded source.
- 080E52EC: matched as Dog_UpdateProbeApproach (tentative). Ported SceneActorControl_UpdateProbeApproach from 080C2BE0: removes the range-ready early callbacks, selects SceneActorMotion_UpdateTargetTransition kind 42, sets context+0x678 bit 8 at entry and bit 4 on acceleration completion; a successful descriptor turn selects behavior 18. Action-timer expiry gates final control/range callbacks. Exact check: 0 differing bytes, 476 bytes, 100% of this function recovered. Inherited actor value barrier is marked as a register-pinning concession. Separate match committed.
- 080E6BE4: new Dog_UpdateShadowlessApproach guarded draft, recorded 84.0% (reviewed extent), 46 realigned differences / 18 ignoring registers. Starts shadowless control kind 36, requests a descriptor turn, clears actor+0x2EC/+0x2F0 on success, and compares context+0x658 against actor+0x2F4 when context+0x65C bit 1 is active. Otherwise 080E8444 selects idle control or behavior 15. A mask-taking inline plus signed full-width bitfield clear improved structure; group r8 binding retained. Pinning context r7 caused invalid inline substitution (stack-relative load) and was discarded. Remaining differences include context flag pointer lifetime, state-reset register allocation and code sharing. Separate draft committed (queue title used preliminary 83.5%; recorded source score 84.0% is authoritative).
- 080E6E08: exploratory C reconstructed entry cleanup (partner pointer, timers, motion flags, context marker), delayed route toward context+0x650 and position-window dependent saved control. First candidate 45.6%, 224 realigned differences / 108 ignoring registers; moving group creation and pinning actor/context regressed. Discarded below threshold; remains assembly. Future work should reproduce saved packet field stores and route helper lifetime before allocation tuning.

Only per-file check.py checks were used; no full build or network operations. Project-wide percentage and percentage-point gain remain for batch integration. Unrelated tools/cloud_task.py working-tree changes were left untouched.

## Task 081A9DDC (2026-10-20)

Tags: #matching #vanargand #boss

- 081ABA64: inspected largest first with asmat, relatives and condense after reading BIG_FUNCTIONS.md. Eleven-way phase dispatch, flags bit 8, pending cue 1, entry command 0x13 to both halves, phase initialized to 1. Repeated animation playback accounts for most of the best relative's 37% coverage (Actor_MoveAlongScriptedCurve). Multiple motion/animation phases and attachment state remain to reconstruct; no complete candidate retained. Local `bl _081AC942` is a far branch to the common epilogue, not a separate function call.
- 081AA788: inspected assembly, relatives and condensed listing. Seven-way phase dispatch starts with command 0x13, waits for the pair, applies command 2 and starts animation 8 plus the auxiliary part sequence. Phase 3 initializes a motion track toward position minus 0x100 in X/Z, starts animation 9, derives vector components with division by 3, then performs randomized paired attacks gated by positive counters. Best matched relative covers only 4%. No complete candidate retained; the motion/vector and paired command contracts need closer reconstruction before a useful full draft.
- 081AB078: existing guarded palette-fade leap draft retained and named Vanargand_UpdatePaletteLeap (tentative), refreshed to reviewed-extent 93.0%, 32 realigned differences / 11 ignoring registers. Existing 88.0% comment was stale under the current checker. Palette-address value barriers reduced structural differences but worsened total allocation to 88.9%; reverted. ROM holds frame in sl and animation in sb, while candidate spills frame and keeps the first palette address across interpolation. Default fallback checked exact. Separate draft refresh committed.
- 081A9DDC: matched as Vanargand_UpdateAlternatingPairAttack (tentative). Chooses an RNG half, applies preparation command 7 to both, issues command 8 to one half, waits 61 ticks, issues it to the other, then waits for pair completion and sets finished. A signed-returning RNG inline preserves the ROM's arithmetic shift and remainder expression; integer address arithmetic `*(u16 *)((index << 1) + (u32)table)` puts the table literal after the index shift. Explicit next/zero locals reproduce the shared phase-reset tail. Exact check: zero differing bytes, 428 bytes, 100% of this function recovered; no register-pinning concession. Separate match committed.
- 081AA178: new guarded Vanargand_UpdateRepeatedPairAttack draft, reviewed 96.2%, 9 realigned differences / 2 ignoring registers. Prepares both halves, randomizes the selected half, alternates command 9 for six intervals over 50 ticks, then waits for completion plus 16 ticks. Mode 1 sets flag 0x40000. Remaining zero reuse and case-zero timer pointer allocation differ. Explicit phase locals and register-pin/value-barrier trials did not improve the candidate; all concessions discarded. Default fallback checked exact. Separate draft committed.
- 081A9F88: new guarded Vanargand_UpdateChargedPairAttack draft (tentative), reviewed 92.8%, 19 realigned differences / 6 ignoring registers. Starts animation 4 and song 0x586, then pair commands 0x13, 0xC and 0x17. If both positive counters and both pair-state bytes are 0xC, stores 60 at actor+0xF3C. Finishes after 120 ticks in the final waiting phase. Remaining differences involve zero-store order, pair pointer PRE, byte-load ordering and allocation. A typed pair view did not change code. Default fallback checked exact. Separate draft committed.

Only fast per-file checks were run; no make, full build, network operations or generated-assembly commits. Exact function recovery and drafts are reported separately; project-wide percentage and percentage-point gain remain for batch integration. Unrelated tools/cloud_task.py changes were left untouched.


## Task 081B1F74 (2026-10-20)

Tags: #matching #vanargand #motion #animation

- 081B2608: drafted 83.8% as Vanargand_UpdatePartRetreat (tentative). Callback in gUnk_086120A0; adjacent callback table follows named Vanargand callbacks. Sets part flag 0x20. On reset constructs a heading-relative length-720 vector (both variant branches are duplicated in ROM), initializes motion mode 4, waits for motion byte+0x29 == 2, plays sequence 7 using boss+0x4D0 and part variant, then marks finished. Uses signed truncation toward zero for sine products. The signed-returning TruncateScale inline preserves arithmetic shifts; assigning the conditional directly to a halfword produced logical shifts. Reused playback from DriftingSprite_Update, with the state-one value also supplying the flip mask. Reviewed check: 71 realigned differing instructions, 22 ignoring registers; candidate 724 bytes versus 720-byte ROM extent. Remaining state-transition pointer reuse, prologue allocation, register copies and pool layout. r9 boss and r8 enter pins retained with the concession marker. Default assembly fallback checked exact. Separate draft committed.
- 081B3AAC: drafted 83.6% as Vanargand_UpdatePartLunge (tentative). Six states: initial angle-160/length-256 motion relative to origin; half the player-minus-base displacement after a 17-tick wait; complete that motion; wait 31 ticks; heading/distance motion; finished. Motion initializer modes are 4, 3, 4. Vector addition and difference must be pointer-based inline functions: direct local-field assignments generated packed word updates and scored 59.0%; inline pointer helpers reached 78.6%. Hoisting the enter pointer to function scope reached 83.6%. Explicit r7 enter / r12 radius pin experiments regressed and were discarded. Reviewed check: 74 realigned differing instructions, 33 ignoring registers; candidate 784 bytes versus 780-byte ROM extent. Remaining state-store ordering, pointer lifetimes and multiplication operand allocation. Default fallback checked exact. Separate draft committed.
- 081B3410: largest-first inspection with assembly, relatives and condensed listing. Seven-state callback, player-facing angle selection, inline sequence playback, motion, sounds, particle bursts and part trail emission. Best relatives explain only about one fifth, largely common playback. No complete candidate produced; stopped without a close port.
- 081B28D8: inspected assembly entry, relatives and condensed listing. Another seven-state callback with player-angle calculations, packed playback, motion, sound and trails. AnimatedParticles_Create explains 19%, mostly playback. No complete candidate produced; stopped without a close port.
- 081B1F74: inspected relatives and condensed assembly. Repeated polar vectors, offset additions and AnimatedParticles_EmitBurst calls; seven repeated phase-reset sequences. Best relative covers 20%; SceneBurstActor_UpdateTimedBurst explains shared burst construction but not the state machine. No complete candidate produced; stopped without a close port.
- 081B2F38: inspected relatives and condensed assembly. Player-angle tracking, motion initialization, packed playback, Sound_PlaySong, Vanargand_EmitPartTrail and AnimatedParticles_EmitBurst. Best relative covers 22%, primarily playback; remaining state transitions require reconstruction. No complete candidate produced; stopped without a close port.

Ghidra pseudo-C unavailable (gen/ghidra/decomp.c absent). No exact C matches: exact coverage gain 0.0 percentage points. Only fast check.py checks run; no make, full build, network operations or generated-assembly commits. Unrelated tools/cloud_task.py changes left untouched.

## Task 08149DF0 (2026-10-20)

Tags: #matching #player #attack

- 0814A128: matched, Player_UpdateStatARestoreAction (568 bytes). Consumes two currentStatB units, restores up to 40 currentStatA points, and shows the amount restored. Four animation phases use variant entries 8 through 11. Stat meanings remain tentative. Shared Player fields and the neighbouring Player_UpdateStatBEffectAction shape gave an exact first candidate.
- 0814DFDC: matched, Player_SpawnSelectedAttackProjectile (964 bytes). Configures the selection display, calculates cost and damage, creates a projectile (owner variant for actionMode six), and subtracts stat B only on success. Base damage is reduced by the missing-stat-A ratio, `(maximum-current)*16/maximum`. Initial 90.8% improved through switch layout, explicit kind selection, mask-before-load inline, and positive-first signed truncation. Final temporary separation made the arithmetic order exact; depin removed all five hints and the formatted source still matched. No register-pinning concession remains in this match.
- 08149DF0: drafted 99.3%, Player_UpdateSpiralTrailAction. Creates spiral particles, advances the packed auxiliary sprite animation, ramps its duration to 32, and exits after 32 frames. Ported the packed decoder from ContactSequenceActor_UpdateActive, whose 115-instruction run covers 47% of this target. Mask-before-load inline and resource-id lifetime reproduce the initial stores. Two address-selection instructions at 08149FB6/08149FB8 use r0 instead of r3; explicit offset pinning disturbed earlier allocation and constant reuse. Retained the best guarded draft after focused attempts stopped improving. Register-pinning concession marked; depin requires an exact base and therefore skipped this draft.
- 0814E3D8: drafted 90.0%, Player_UpdateSelectedAttackCharge. Eight phases implement charge tiers, held/released input, projectile creation, recoil decay and buffered re-entry. Related draft 0823F148 supplied the particle bitfield layout and recoil shape, then all target-specific phase logic was reconstructed from asm. Full-extent recorded check: 76 differing instructions, 36 ignoring registers. Remaining differences include a merged charge-tier transition tail, cost temporary lifetime and particle initialization order. Commit subject retains the preliminary candidate-length 88.4% score; the source's recorded full-extent score is 90.0%.
- 0814D264: stopped below draft threshold. Full geometry candidate retained locally at wip/sub_0814D264.c, 75.1%, 163 realigned differences / 59 ignoring registers. Three animation selectors (0x54, 0x81, 0x72) construct planar, vertical and radial contact sweeps using the sine table, then refresh/configure the collider. A coordinate-store inline improved 71.1% to 75.1% by delaying destination calculation until after the source value; moving the sine table to a function-scope local regressed to 69.2%, reverted. Remaining problems are table lifetime/high-register allocation, early-return constant stores and context-address lifetimes. No production draft committed.
- 0814C3A4: inspected largest first with asmat, relatives and condense after BIG_FUNCTIONS.md. Eight-phase apparent grab/motion routine snapshots previous position/resource fields, installs Player_OnGrabMotionExit, allocates a particle pool, and manipulates the object in gUnk_0200054C. Condense found only modest repetition (1209 instructions to 1165 listing lines); strongest relative covers 8%, mostly signed polar-vector math. Late phases calculate distance with Sqrt/Div and heading with Math_Atan2. No viable complete C candidate produced; stopped with assembly intact. The local `bl _0814CF20` targets the common epilogue, not an external call.

Only fast per-file check.py checks ran; no make, full build, network or generated-assembly commits. Ghidra pseudo-C was unavailable (gen/ghidra/decomp.c absent). Exact percentage and percentage-point gain are left to batch integration's report regeneration. Unrelated tools/cloud_task.py changes were left untouched. Each match/draft was committed separately through build/commit.sh.
# Task 081556E0 — 2026-10-20

Tags: #matching #player #animation

- 081556E0: drafted 87.8% (recorded NONMATCHING check); tentative name
  Player_UpdateStateMotionCue. PlayerCue_StateMotion installs it for state five.
  Chooses palette source by status flags on entry, plays song 490, runs animation
  154, steps the auxiliary sequence for the first ten ticks, and returns to stand
  after tick 39. Inline accessors returning masked flags preserve the repeated
  null checks; direct `p && flags` expressions merged those checks and scored
  only 71.9%. Remaining: animation-step selection/control flow, temporary
  liveness, and registers (57 realigned differences, 34 ignoring registers).
  The commit title retains the earlier 86.9% whole-candidate score; the source's
  recorded reviewed-extent score is 87.8%.
- 0815767C: drafted 88.2%; Player_ApplyControlPresentation synchronizes control
  flags to collider/drawable flags, shadow visibility and debris palette source;
  copies the render position with offsets and optional random jitter. A drawable
  struct preserves the base+8 flags access; explicit integer-addressed RNG table
  lookup preserves load order. Remaining 30 instructions differ only in registers
  (zero differences ignoring registers). No register pinning used.
- 081561EC: inspected assembly, condensed listing and relatives; left unmatched.
  PlayerCue_Motion38To installs it for state two / sequence 0x26 and writes the
  target coordinates at 0x590. Entry initializes a particle pool at 0xA88 and
  installs Player_OnTargetMotionExit. Five phase paths mix secondary animation,
  movement interpolation and particle emission. Relatives explain short trig/RNG
  fragments, not the complete state logic. No compilable candidate retained.
- 08156FE0: inspected assembly, condensed listing and relatives; left unmatched.
  Large sequence selector with two jump tables, gauge drain, barrier/interaction
  tests, status flags and animation event tests. Returns the current sequence in
  some early exits and zero after normal transitions. No close matched seed or
  compilable candidate retained.
- 081559A4 and 08155D58: inspected assembly and relatives; left unmatched. Related
  six-phase player action sequences, adding control flag 8 and dispatching on
  byte 0x458. Phase zero selects variant animation zero and advances to sequence
  7/phase 1 on action-config flag 1. Phase one initializes facing 4, heading 1,
  side zero and action origin. Existing relatives cover only short fragments;
  no compilable candidates retained.

Ghidra export `gen/ghidra/decomp.c` is absent. Only tools/check.py was used for
compilation; no full build, network operations or generated assets committed.

## Task 080F645C (2026-10-20)

Tags: #matching #sword #nonmatching

No exact matches: exact code gain 0.0 percentage points. One guarded draft was
committed; no full build, network operation, or direct git mutation was run.
The pre-existing tools/cloud_task.py modification was left alone.

- 080F645C: drafted 96.5%, Sword_UpdateDamage. Caller Sword_UpdateTask confirms
  the damage/death role: CB4 cooldown, palette restore, D78 pending damage,
  D7A damage-effect parameter, signed health at 7E. Death sets state 1/6 and
  callback sub_080F421C; increments total and Sword defeat counters (87C, 538,
  548), plus the first applicable category counter 58A–592 selected by CA8.
  CannonTurret_HandleDefeat (08207FAC) supplies most of the exact counter code.
  Final check: 15 differing bytes, 11 realigned instructions, 2 structural.
  Residue: the health load occurs before forming the damage pointer, one
  counter's register assignment, and the last mask's destination. Uses r8
  zero and a scoped r2 read concession; depin skipped because the candidate
  is nonmatching. Default INCLUDE_ASM path checked exactly. Draft fuzzy score
  recorded with check.py --nonmatching --record. Typed SetState helper from
  080F3848 reproduced state stores better than raw byte assignments.
- 080F67AC: inspected full condensed initializer and relatives; no candidate
  retained. Sword owner setup (02000580), main sprite at B68, animation at B58,
  graphics at B94, CC8/CF0 callbacks, copied palette at D30, optional A/T/p/K/V
  script parameters, several colliders, shadow and another sprite. Nearest
  matched relatives explain only the packed animation stepper (~23%). A
  direct sibling seed is absent, and the 846-instruction mixed initializer
  needs a local layout and shared stepper before useful allocation work.
- 080F79A8: inspected condensed phase body and relatives; no candidate retained.
  Entry initializes CAB flags, CA0 radius, CA4 angular step, CA5 sign and C98
  center. Trigonometric orbit/trail positions, timed RNG sign changes, player
  heading and expansion lead to later state transitions. Closest matched
  relative explains ~21%; several distinct timed phases need separate C
  recovery. A single generic steering helper would lose the branch layout.
- 080F6F90: inspected condensed body and relatives; no candidate retained.
  Parses c/C collision parameters and n (default 4), creates four child records
  at 4BC + i*194, graphics 5291, animation resource 922E/3F72 sequence 2,
  palette D56, owner pointer at child+1D0; attack/body colliders use 2001/5009.
  Stack vector initialization uses packed halfword insertion (not arbitrary
  uninitialized scalar reads). The inlined animation stepper is the only
  substantial relative (~20%); no reliable full-body seed was available.
- 080F8648: tentative Sword_UpdatePursuitReturn candidate in untracked
  wip/sub_080F8648.c, 67.0% (187 realigned, 47 structural differences). Entry
  initializes a trail at A24 with polar velocity A2C (speed 12); shrinks
  drawable byte 6, follows the player through tick 119, then returns to the
  actor at speed 8. Settles into state 29 within squared distance 1023 or
  after tick 600. Reused Polar/Delta/Scale12 from 080E1A28 and the existing
  SceneActorRegistryResetView. Explicit register hints improved 63.3% to
  67.0%, but source load/address order and lifetime differences remain broad;
  below the commit threshold, so stopped rather than retaining in src.
- 080F75C0: tentative Sword_ApplyContactRetreat candidate in untracked
  wip/sub_080F75C0.c, best 73.1% (53 realigned, 22 structural differences).
  Rejects statusFlags 40/status 20000/dead health, delegates common contact
  reaction, records hit fields, and if owner CAB bit 2 and EB0 > 2 disables
  three pairs of colliders (A96/B92, stride 54), selects reverse flicker phase
  2F, and clears EB0. A u8 inline predicate reproduces the materialized bool.
  Inline state pointer arguments hoist addresses across the collider loop;
  an input-only loop-index barrier prevents reverse countdown. Register-pinned
  loop indexes instead inhibit strength reduction and were worse. Experiments
  with pinning all state pointers regressed; best source retained separately.

Only the guarded damage draft is production source. Scratch candidates are
below the 80% draft threshold and are deliberately uncommitted. Resume with
those candidates and build/sword-*.txt condensed listings if this checkout is
retained; generated listings are not committed.

## Task 0821881C — 2026-10-20

Tags: #matching #terrain #sprites #scanline

- 082192F0: matched as `ScanlineEffect_StageTransfer` (commit 76695fe00).
  Stages 160 halfwords from the queued source into ScanlineEffect_GetBuffer:
  modes 0/1 add the selected BG horizontal/vertical scroll and mask to 511;
  mode 2 copies mosaic values; mode 3 sets blend registers and masks to 31.
  Publishes the transfer target/buffer and clears the source. Neighbouring
  ScanlineEffect functions and caller 082141C0 establish the subsystem.
  `buffer[i]` rather than `*out++` reproduces the exact loop allocation and
  initialization order. BackgroundLayer field accesses reproduce the ROM's
  base-address load and field offsets. Final check.py: 100%, zero differing bytes.
- 0821C8F8: stopped after assembly/condense/relative analysis, no draft.
  Largest target first inspected; no matched relative explains more than 3%.
  Six parameters: context, position, movement vector, two u16 extents, u8 kind.
  Direction flags use 0x10/0x20 for X and 1/2 for Z. Builds paired offset probes,
  calls 0821BF5C and 0821C254, and commits through 0821C754. Many duplicated
  direction-specific branches and ten stack position temporaries must be retained;
  a generic shared probe loop is unlikely to reproduce this layout.
- 0821C254: stopped after full assembly and relative analysis, no draft.
  No relative explains more than 5%. Uses context halfwords +4/+6 as cell indices,
  classifies an index delta into eight neighbouring directions with terrain width,
  and dispatches eight cases. Diagonal cases probe both orthogonal cell-edge
  positions through 0821BF5C before clamping the destination. Edge positions use
  `(column << 8) + 1` or `((column + 1) << 8) - 1`; keep explicit case bodies.
- 0821BF5C: stopped, best candidate 70.8% fuzzy, 167 differing instructions / 125
  ignoring registers; below the draft threshold. Candidate retained locally at
  wip/codex-low/sub_0821BF5C.c. Resolves terrain overrides (payload +4) or base
  cells (+12+4*index), rejects blocking flags and excessive 0821BE3C result,
  then checks adjacent flat/slope transitions. Kind 4 selects mask 2/limit 128;
  other kinds mask 1/limit 100. Game-state +868 bit 16 with destination flag 128
  can reject lateral slope movement. Explicit duplicated override lookups improve
  the score. Shared compare-pointer tails shrink the function incorrectly;
  register-pinning trials regressed and were discarded.
- 0821881C: stopped, no qualifying draft. Best fuzzy trial 62.8%; best structural
  trial 41 differing instructions ignoring registers (203 total), 56.9% fuzzy.
  Retained the latter locally in wip/codex-low/sub_0821881C.c. Tentative role:
  stages queued sprites into OAM lists, projects world coordinates with signed
  3/32 scaling, allocates affine inputs, applies display flags and clamps depth.
  Scale limit comparisons load unsigned bytes, but affine inputs use signed
  bytes. Projection inline with pointer parameters reduced structural differences;
  explicit register pinning worsened structural agreement. Heavy allocation and
  projection/control-flow differences remain.
- 0821D178: stopped, candidate 62.1% fuzzy, 193 differing instructions / 86
  ignoring registers; retained locally in wip/codex-low/sub_0821D178.c.
  Clips a segment to a 256-unit cell. Checks +Z, -Z, -X, +X intersections and
  returns 3, 2, 4, 5 respectively; unchanged destination returns 1, failure 0.
  The packed two-word position preserves unspecified padding halves in the ROM;
  candidate layout and register allocation differ. Pinning the union and dz
  regressed and was discarded. Candidate is unreviewed scratch, not an accepted draft.

Only per-file check.py checks were run; no full build or network operations.
No NONMATCHING drafts met the required 80% threshold. Existing tools/cloud_task.py
modification was left untouched. No shared docs or generated assembly committed.

## Task 08151EB8 (2026-10-20)

Tags: #matching #player #animation #cue

Exact progress: 0.0 percentage-point gain; five guarded drafts, no C matches.
All six callbacks belong to PlayerCue, as established by the named callers
0815B0A8 (WalkTo), 0815B9CC (Motion7Variant3), 0815B52C,
0815C428 (FormMotion), and 0815C4FC (StateMotion).

- 08152174: drafted 81.7% as PlayerCue_UpdateMotionFade (tentative).
  Largest target inspected first with asmat, relatives and condense. Six phases
  play motion 7 while changing sprite bytes 154/155, control flags and scale 4E8.
  ContactSequenceActor_UpdateActive supplies the repeated packed-command decoder.
  Final reviewed check: 217 realigned differences, 37 ignoring registers.
  Pinning the player pointer or command pointer worsened code sharing/allocation;
  reverted. Scale-store addressing and phase tails remain, plus decoder registers.
- 08153A00: drafted 94.2% as PlayerCue_UpdateSideWalkTarget (tentative).
  State-four walk snaps within squared distance 144, uses twelve-unit velocity,
  maps the atan2 angle to eight directions, and scales one velocity axis by
  40/64 or 52/64 depending on field28F and sign. Completion installs 081538DC.
  Reviewed check: 39 realigned differences, 18 ignoring registers. Conditional
  multiplication folds sign checks differently; direction-store sharing differs.
- 08153224: drafted 84.4% as PlayerCue_UpdateFormChange (tentative).
  Four phases toggle variant418 between zero and one, choose variant animation
  table entries 0/8/10/11, update spiral parameters AD6/AD8 and resume standing.
  Real observed typed fields raised the initial 73.5% candidate above threshold;
  reviewed check: 84 realigned differences, 26 ignoring registers. ROM preserves
  the high half of a stack particle word; candidate fully initializes both words
  instead of introducing an uninitialized C read. Other residue: constant copies,
  timer masks and phase branch/register layout. No pinning in this unit.
- 08151EB8: drafted 98.6% as PlayerCue_UpdateWalkTarget.
  WalkTo's state-three callback: nine-unit velocity, squared arrival radius 81,
  odd-facing animation directions, optional final direction at 59D. Completion
  installs 08151D90. Reviewed check: eight differing instructions, six structural.
  Main residue is the common byte-store tail for finalDir versus dir, and OR
  ordering before the direction store. Explicit shared output-pointer and inline
  facing-value variants regressed and were discarded.
- 08153610: drafted 87.1% as PlayerCue_UpdateSpiralMotion (tentative).
  State-three motion 13 grows AD8 by three up to 32, alternates facing according
  to timer bits, sets resource16F and eventually resumes standing. Struct-copy
  syntax reproduces the paired position copy at AC; the animation mask must use
  r6 instead of the sibling decoder's r5. Reviewed check: 52 realigned differences,
  20 ignoring registers. Resource offset derivation and constant/timer lifetimes
  remain different.
- 08152A5C: stopped with uncommitted scratch at
  wip/codex-low/sub_08152A5C.c, tentative PlayerCue_UpdateRecoil.
  Plays song E6 once; speed598 decays by 3/4 and drives opposite-facing velocity.
  The 480-byte body candidate differs in six register-selection instructions
  (97.5% candidate-length similarity), but the inventory extent reaches 08153224.
  There is another push/prologue at 08152C3C in the retained byte tail. The required
  full-extent recorded score is only 38.6%; do not count or commit it as a qualifying
  draft and do not mistake the body-only score for acceptance. Boundary repair is
  required before resuming. No shared generated files or split metadata changed.

Each retained draft has a file-local descriptive alias and guarded INCLUDE_ASM
fallback; nonmatching checks used --nonmatching --record. Formatting used the
venv clang-format directly. depin was attempted on unguarded candidates and
correctly skipped their mismatches; its default-mode handling of a guarded draft
would test the assembly fallback, so do not use that to minimize draft hints.
All five default assembly fallbacks passed check.py. Ghidra export was absent.
No full build or network operation performed. Existing tools/cloud_task.py edit
was left untouched. Five separate draft commits were made through build/commit.sh.

## Task 08208BE4 (2026-10-20)

Tags: #matching #fan-cannon #cannon-turret #screamer

Exact progress: 0 percentage-point gain; no matches.

- 08209254: drafted **80.7%**, FanCannon_ApplyHitReaction. Copies damage/type/reaction, handles blocked contact, applies critical-hit sound/display/HUD changes, updates contact stats, and emits three sparks for selected source flags. Target view: position 2C, damage 110, health 10E, flash 118, HUD parameters 128/12A/134, spark mask 12C, damage type 15C. Reviewed check: 37 realigned differing instructions, 7 ignoring registers. Register and stack-address selection remain. The preliminary 89.6% and 83.7% commits had an unsafe agbcc low-register binding: r7 health was overwritten while forming the HUD-duration argument. The final corrective commit binds health to r10 and assembly review confirms it reaches the call intact. Only the final 80.7% source is a valid candidate.
- 0820BF70: drafted **94.3%**, Screamer_ApplyHitReaction. Same contact behavior with Screamer layout, spark template 0824FE9C (FanCannon uses 0824FE8C). Target view: position 40, damage 1DC, health 1E4, flash 1DE, HUD parameters 1E6/1EC/1F4, spark mask 1EA, HUD owner 1F8, damage type 204. Reviewed check: 14 realigned differences, 3 ignoring registers. Mainly index-pointer copies, final random sample/mask allocation and stack-address materialization. HUD health binding r10 reviewed in emitted assembly; no low-register overwrite.
- 08209AC4: inspected largest first using asmat and condense; no candidate retained. FanCannon_Update combines automatic activation/toggle scheduling, callback dispatch, contact flags, damage/defeat effects, terrain modifier registration, player alignment/range/path checks, global fan force direction/strength and packed animation playback. Condense finds only two short repeated runs; no broad matched sibling port found. Stopped at reconstruction without a close C hypothesis.
- 0820A5CC: inspected full assembly and relatives; no candidate retained. FanCannon script creation (tentative FanCannon_CreateFromScript): manager allocation/free slot, keyword setup, sprite/animation, collider initialization, terrain modifier, scene body, toggle schedule, occupied-slot bit. Best matched relative covers only 19%, mostly playback. Direction keyword h normalizes (signed-byte value minus one)/2 to two axes. Radius R defaults to 1536, power a to 15, toggle interval i to 300. Needs a complete typed slot/setup reconstruction before allocation tuning.
- 08208BE4: inspected full assembly, caller CannonTurret_Create and relatives; no candidate retained. Tentative CannonTurret_InitializeFromScript. Position at 1C, squared radius at 11C (default 0x40000), three a values at 120/122/124 (defaults 30/0/30); sprite at 24, position copy to 40, graphics load, animation initialization, collider and scene-body/terrain-overlay setup. Closest relative only covers 24%, largely packed playback. No complete candidate produced.
- 08209704: inspected assembly; no candidate retained. FanCannon_UpdateActive emits randomized enemy particles every 30 ticks and increases power at 120 up to limit 10C. Axis 11A and direction 11B control angle; radius 108 controls randomized along-axis displacement. Signed products with sine table 086149C4 truncate magnitude toward zero after shifting 12. Stack particle parameters and duplicated direction branches need reconstruction; no close port available.

Techniques: u16 vector components reproduce halfword arithmetic without signed-store packing. A two-vector stack aggregate fixes position/velocity ordering. Raw RANDOM_TABLE_ADDR plus sample-before-position arithmetic reproduces the RNG loop more closely. Scoped high-register bindings improve the loop but low-register bindings can silently corrupt live values; inspect emitted assembly, not just fuzzy scores. depin on guarded drafts tests the default assembly fallback and therefore incorrectly removes every hint; restored hints and rechecked NONMATCHING. Both final sources have descriptive aliases, register-concession markers, guarded assembly fallbacks and recorded full-extent scores. Direct venv clang-format used. No make/full build/network operation. Existing tools/cloud_task.py edit left untouched.

## Task 08153E1C (2026-10-20)

Tags: #matching #player #animation

- 08153E1C: matched PlayerCue_UpdateSideDeparture (tentative). Four phases: control flag 4, camera target copied from player+30 with y raised by 150, screen command selected by byte4E0, side-facing sequence 2 and songs 1AD/1AE; then scale4E8 descends by 60 for 30 ticks before hidden/control completion. Exact check after formatting and depinning: zero differing bytes, 696-byte full function. All 11 animation hints remained necessary under depin after removing the NONMATCHING guard.
- 081540D4: drafted 91.5%, PlayerCue_UpdateSideArrival. Scale4E8 decreases by 30, then sequence 3 plays until an endpoint before returning to side stand callback 081538DC. Realigned 40 differences, 27 ignoring registers. Phase value is retained in r8 in ROM and reused as the first animation mask; draft uses a separate constant. Endpoint return placement also differs.
- 081543F0: drafted 86.4%, PlayerCue_UpdateSideFade. Six phases, with playback in phases 0/3/4/5 and scale/visibility changes in phases 1/2. Phase 0 and 3 initialize direction 4 and update facing at timer zero. Realigned 131 differences, 29 ignoring registers. Scale-store address order, scratch registers and last-phase sprite/state allocation remain. Separately pinning last-phase sprite/state and using inline width/height stores reduced structural count to 23 but worsened total to 179, so retained the better total candidate.
- 08154AF0: drafted 97.5%, PlayerCue_UpdateSideRecoil. Song E6 on entry; sequence 2; speed598 projected opposite direction+5 with sine table 086149C4, then multiplied by 3/4 each tick until zero. Realigned six differences, zero ignoring registers (seven differing bytes). Remaining constants 598 and 3A4 occupy different scratch registers. Reading table before speed fixed load order; pinned command r6 and temporary offset r0/table r1 fixed playback allocation. Explicit pins for the two remaining offsets worsened code and were rejected.
- 08154CD4: drafted 88.8%, PlayerCue_UpdateSpiralRecovery. Song37F and SpiralTrail_CreateWithParticles on entry; visibility threshold uses signed comparison of timer&7 against 7-(timer>>2), animation9A and resource16F, fadeAD8 rises by three to 32; at 32 ticks marks bytes464/465 and5A3 around standing transition. Realigned 33 differences, 13 ignoring registers. Resource pointer/constant reuse and tail mask allocation differ.
- 08155008: drafted 86.0%, PlayerCue_UpdateAlternateFade. Same six-phase scaling as 081543F0, using resource414 and animation selection byte46B. Only phase 3 initializes direction4/facing at timer zero. Realigned 131 differences, 28 ignoring registers. Same scale-store and final playback allocation issues as side fade.

Technique: existing 08152174 provides nearly the entire six-phase cue draft and reusable packed command playback. Player sequence is at 3FC, resource at 410 (there is a word at 40C after the 16-byte sequence), alternate resource at414, phase458 and timer45A. The side-facing callers 0815B404/0815B49C/0815B52C/0815B858/0815B9CC/0815C4FC establish PlayerCue naming context. Ghidra export was unavailable. Used asmat, relatives and anchordiff; only check.py for compilation. All drafts have file-local aliases, concession markers, NONMATCHING guards and recorded scores. Important tooling caveat: depin checks the default fallback on guarded drafts and can falsely remove every hint; restore before checking NONMATCHING, and remove the guard before depinning exact C. No full build/network operation. Existing tools/cloud_task.py edit was left untouched.

## Task 081B0248 (2026-10-20)

Tags: #matching #actor-status #vanargand

Exact progress: 0 percentage-point gain; no matches.

- 081B118C: drafted **92.9%**, ActorStatus_BeginPlayerRelease (tentative). Entry clears state164/timer168, sets phase16F, applies Player_ApplyPositionRequest at status position1C, selects sequence9 from owner resource4D0 and steps packed animation at48. Then advances state164 to1; phase1 sets done170. Full-extent NONMATCHING check: 18 realigned differing instructions, 4 ignoring registers. Entry pointer copies, resource addressing and final state-store order remain. Default fallback check matches the retained assembly. Commit message has preliminary 92.5%; recorded source is the full-extent 92.9% score.
- 081B08B8: attempted, best preliminary fuzzy 67.3% (359 realigned differences, 166 ignoring registers); below retention threshold. Five states: move offset toward -256 on x/z, seek player's position relative to anchor90 with duration3, proximity check squared <=0x63FF, contact animation12/song58A versus timeout30 then animation9, done170. Vanargand_GetComponentWithFlag10 can end seeking. Large inline sequence allocation and control-flow placement differ. Local scratch under build/codex-low-attempts only; INCLUDE_ASM remains authoritative.
- 081B1334: attempted, best preliminary fuzzy 51.8% (441 realigned differences, 178 ignoring registers); below retention threshold. Related approach with -1024 offset, duration4, proximity-triggered status18 subject to flags100 and previous-state25/26 gates; timeout120 then sequence9. Three packed sequence copies. Entry also plays song58A. Control-flow and inline allocation remain unresolved; scratch only.
- 081B03C0: attempted, best preliminary fuzzy 61.9% (308 realigned differences, 146 ignoring registers); below retention threshold. Sequence12, motion target384/0/384, player request guarded by player flags40000, wait120, sequence9, apply player position, sequence9 again, done170. Failure goes to final state3 rather than ordinary completed-motion state1. Three inline sequence steps dominate; scratch only.
- 081B0F34: attempted, best preliminary fuzzy 70.4% (166 realigned differences, 131 ignoring registers); below retention threshold. Sequence12, target640/528/640, player request and motion completion, then done170. flags160 gains12 and16. Motion branch shape, vector copies and packed-step allocation differ; scratch only.
- 081B0248: attempted, best preliminary fuzzy 67.4% (78 realigned differences, 37 ignoring registers); below retention threshold. Four VanargandEffect_Create calls around anchor90 with radial distance640, y+256; side15C chooses angle48+i*8 or255-i*8 and effect side1-side. Random samples shifted3 provide angle perturbation modulo32 and speed12..19. Signed truncation toward zero after trig multiply is explicit. Pinning angle/vector pointer made allocation worse; scratch only.

Technique: relatives identified Ivy_TickDrawable's packed animation step (113 instructions per copy). Read docs/BIG_FUNCTIONS.md and used condense for the three large functions; Ghidra export is missing. Borrowed inline pins did not transfer reliably to different caller lifetimes; tested pinned and unpinned forms. depin was invoked on the unguarded 081B118C candidate and skipped it because it was not exact. Only per-file check.py compilation was used. No full build or network operations. Existing tools/cloud_task.py modification was left untouched. Only the qualifying guarded draft is committed; below-threshold candidates remain in ignored build scratch.

## Task 080A9BD8 (2026-10-20)

Tags: #matching #tentacle #actor-trail

- 080AB7BC: updated the existing guarded draft to 93.1%, named TentacleActor_UpdateSegmentColliders. Corrected the attack-properties final argument offsets from 0x12/0x1 to 0xC6/0xC4 using the assembly's pointer increments. A sequential pointer-based SetAttack inline reproduces the argument load order. A local collider flags struct preserves the +6 halfword access, and tsth preserves mask-before-load ordering. Final recorded check: 15 realigned differing instructions, 13 ignoring registers. Remaining issues include the 0x2D0 constant derivation in chk, stack-vector address scheduling, and the first delta component's register choices. Register pinning of the chk offset regressed and was removed. No register pins retained in this unit.
- 080AB968: new guarded draft, recorded 85.9%, named TentacleActor_UpdateRetraction (tentative). Entry consumes 0x2B1, selects control sub_080A69F0/kind 0x23, and sets mode 8; advances segment targets with divisor 4 and threshold 0x6400, then oscillation with limit 0x1AE and scale 40. Completion restores pointer228 control and behavior 2. Shared scene_actor_reset.h fields and a SetControl inline closely reproduce the state reset. Two local register pins (phase r1, offset r2) improve the final reset. Final recorded check: 29 realigned differing instructions, 12 ignoring registers. The original retains two unused absolute-value calculations for motion components at 0x2EC/0x2F0; the candidate compiler deletes their arithmetic. State-reset addressing and allocation remain. The initial 83.7% candidate-length score became 85.9% when checked against the reviewed full extent.
- 080A9DB0: attempted, best preliminary fuzzy 52.4% (166 realigned differences, 86 ignoring registers). Enabled, visible segments approach their 0x4E8 targets when squared distance exceeds the supplied threshold, with divisor clamped to at least one. After sub_080A97AC, it fills runs of non-enabled segments through 080A9BD8; the first enabled segment approaches the drawable position at divisor four when squared distance exceeds 0x1A44. Changing indexed loops to pointer increments regressed to 29.9%; stopped and removed below-threshold source.
- 080AD530: attempted, preliminary fuzzy 58.5% (151 realigned differences, 105 ignoring registers). Events above 18 set completion state. Actor flag 0x2D4 bit 2 restores a group control; body+0x6D4 mask 0x1D invokes sub_080B3ADC. Otherwise targets player+0x24, attempts route calls 0821F204/0821F7A8, and enters behavior 7 with timer 32. Control-reset expansion and route failure layout differ; no qualifying draft retained.
- 080A9BD8: inspected assembly and relatives; best matched relative covers only 3%. Interpolates segment positions between copied endpoints using signed fixed-point square/cube calculations at scale 1024. The unusual retained zero products and stack-vector setup require more careful inline reconstruction. No complete candidate produced; left INCLUDE_ASM.
- 080ABDCC: inspected first as the largest target, read BIG_FUNCTIONS.md, generated condensed listing and queried relatives. Actor initialization allocates either a 0x5C sprite or 0x80 alternate drawable, loads resources D636/C102, and includes repeated script parameter parsing plus terrain/animation setup. Best matched relative explains only 9%; no close whole-function port and no complete candidate produced. Left INCLUDE_ASM.

Only per-file check.py checks used; no full build or network operations. Ghidra export gen/ghidra/decomp.c is unavailable. Existing tools/cloud_task.py edits were untouched. Scratch listings/check output are in ignored build/codex-low-080A9BD8. Exact progress gain is 0 percentage points; these two guarded drafts do not count as matching progress.

## Task 08146574 (2026-10-20)

Tags: #matching #player #animation

- 08146EAC: matched as Player_UpdateAlternateFormCue (tentative). Motion eleven toggles actionMode two, drives the secondary sprite at 0x1D0 using the sequence at 0x218, clamps its height relative to GameState position, and coordinates attachment/screen effects. Ported the packed-command stepper from 08153E1C and restored its completion result. Typed sprite/sequence fields fixed evaluation order; signed ContactPosition.pad at 0x1F2 reproduced separate halfword/byte constants. A conditional volatile pointer reload and two register pins in the height clamp reproduce the original reload path. depin reduced 14 hints to 12; retained register-pinning concession. Final formatted check: zero differing bytes, 2188-byte extent.
- 08147768: matched as Player_UpdateFormChangeCue (tentative). Motion ten toggles actionMode zero/one and refreshes form equipment. The assembly's shared phase-four tail is an explicit goto. An inline mask-taking TestModeFlags puts constant two before the global load. No pins. Final formatted check: zero differing bytes, 1416-byte extent.
- 0814692C: matched as Player_UpdateEnergyBurst (tentative). Energy check and shared latch gate ActorBurst_Create; four phases prepare the offset origin, play animations, raise the duration and restore facing. Existing Player fields and SceneVec copies matched on the first check, without pins. Final formatted check: zero differing bytes, 824-byte extent.
- 08146574: matched as Player_UpdateInputCadenceCue (tentative). Shifts 32 byte input-history samples at 0x500, counts occupied samples, and derives animation speed from occupancy before departure phases. Replacing a pointer-index loop with a typed containing-struct array prevented unwanted strength reduction and matched exactly. No pins. Final formatted check: zero differing bytes, 764-byte extent.
- 08146C64: matched as Player_UpdateAttachmentCue (tentative). Creates descriptor attachment 0x50, optionally faces the actor in gUnk_02000488, then sequences animations and a fading control cue. The facing mask needs a single r1 result pin; depin retained it. Final formatted check: zero differing bytes, 504-byte extent.
- 08147CF0: guarded draft as Player_UpdateSpiralRelease (tentative), recorded full-extent fuzzy 97.1%. Creates spiral particles, alternates folded facing/collider updates, advances the secondary animation and selects palette 0x2DD. Reused the packed-command stepper from 08153E1C. Removing an early duplicate overlay pointer eliminated an extra saved-register copy. Remaining realigned differences: 11 instructions, three ignoring registers, primarily tail address temporaries/constant derivation and pool extent. Explicit offset pins regressed to 95.9% and were reverted. The commit title's preliminary candidate-length 97.4% is superseded by the recorded reviewed-extent 97.1%.

Five exact functions and one guarded draft; exact functions cover 88.9% of this assignment's function extents (5696 bytes), with the remainder a draft. Register-pinned exact category: 08146C64 and 08146EAC; unpinned exact category: 08146574, 0814692C, 08147768. No project totals regenerated in this worker. Only per-file check.py checks used; no make, full build, network, or shared-doc edits. Ghidra export is absent. Pre-existing tools/cloud_task.py changes were left untouched. Separate function commits queued through build/commit.sh; formatting-only followups cover the first two accepted units.
# Task 081A2858 — Codex low, 2026-10-20

Tags: #matching #hresvelgr #vanargand #effect-pool

Exact progress: 0 percentage points; no exact matches. One guarded draft,
081A2858, recorded at 86.3%. No full build or network commands were run.
The existing tools/cloud_task.py modification was left untouched.

- **081A5614 — stopped after structural analysis, no C draft.** Inspected first
  as the largest target. It selects a palette bank (0x6F versus 0x7F according
  to pointer F24), dispatches six states through F20, and performs repeated
  clamped fixed-point color interpolation. State 1 blends palette buffers at
  actor offsets 718/918/B18 with the F18 counter and F22 exponent. The condensed
  listing identifies three copies of a 120-item color transition run and 22
  signed division/shift runs. `relatives.py` supplies the interpolation idiom
  from SpriteController_AnimateGrowing but covers only small individual runs;
  no Ghidra export exists in this checkout. Stopped without a complete,
  independently checkable C implementation. Condensed scratch listing:
  build/codex-low-5614.txt. A future attempt should factor the clamped
  interpolation before reproducing all six palette states.
- **081A2858 — drafted 86.3%.** Tentative name
  Hresvelgr_UpdateCollisionPresentation; called by ActorReaction_Tick after
  behavior dispatch. Synchronizes attack/body colliders, sprite visibility,
  shadow enablement, sound latch, camera offset and oscillation scale.
  Invulnerability halfwords are at 314 and 318 (316 is the oscillation scale).
  Full-width signed collider flag bitfields and mask-taking accessors improved
  instruction selection. Pinning the descending collider-loop counter to r5
  improved 78.6% to 85.7%; a scoped clear mask and explicit flags pointer brought
  the recorded full-extent result to 86.3%. Unit has the register-pinning
  concession marker. `depin.py --write` was invoked; it skips a nonexact base.
  Remaining realigned differences: 83 total, 61 ignoring registers, chiefly
  flag insertion, branch layout and address/zero lifetimes. Default build uses
  INCLUDE_ASM. Final command: tools/check.py src/fn/sub_081A2858.c
  --nonmatching --record. Candidate emits 860 bytes against an 868-byte extent;
  this is a guarded draft only, not exact progress.
- **081A39E0 — stopped, candidate 65.2%.** Existing canonical name is
  EffectPool_UpdateEntry; local tentative candidate name is
  EffectPool_UpdateHomingEntry. Its active-entry caller advances byte 158.
  Turns angle 163 toward the player with turn shift 162 and signed interval
  164; speed 159 produces the velocity at E4. Includes the packed animation
  stepper at 2C and periodic MotionEntry spawns into pointers at 168.
  Remaining differences are animation-step control flow, inline vector
  addressing and registers (65 structural/156 total). Register pin trials did
  not improve the structure. Candidate retained in wip/sub_081A39E0.c;
  below the committed-draft threshold.
- **081A6AEC — stopped: full-extent score 54.1%.** The 312-byte visible body
  projects actor position 54 and target pointer 110, isolates projected X
  separation, unprojects it with Y=0 and computes a planar square-root distance.
  Tentative name Vanargand_GetTargetLateralDistance. Debug_NudgeWorldPositionWithB
  supplies an exact projection idiom. Arrays plus inline projection/set/inverse
  routines improved the visible-body comparison from 55.4% to 90.8%, with only
  6 structural and 17 total realigned differences. However the symbol extends
  through undecoded executable bytes starting at 081A6C24, up to 081A6DCC.
  The required --nonmatching --record check correctly scores the full extent
  at 54.1%. An early queued draft commit was followed by a removal commit;
  no below-threshold production draft remains. Retained in wip/sub_081A6AEC.c.
  Do not treat the candidate-length 90.8% as a full-function score. Boundary
  recovery is required before this candidate can qualify.
- **081A8F48 — stopped, best candidate 65.8%.** Tentative name
  Vanargand_UpdatePlayerDrain: positions the player at actor position +
  (128,0,128), selects animations 12–14 and copies player collision data into
  palettes 7B8/7D8/7F8 by player mode 418, then adds a position-request result
  to boss health and displays the change. Array-based stack vectors eliminate
  unwanted merged word stores; three-argument vector addition improves the
  structure. Latest helper experiment is 65.7% (68 structural/125 total),
  retained in wip/sub_081A8F48.c; initial candidate also in build/8f48-base.c.
  Important control-flow detail: global flag 4 branches to finished, rather
  than returning without setting it. Remaining differences include shared
  animation/drawable addresses across switch arms and stack-vector temporaries.
- **081A9178 — stopped, candidate 54.7%.** Tentative name
  Vanargand_UpdateTargetDash. Callback at 104 can interrupt; state A2 starts
  animation 4, waits past frame 10, initializes a mode-3 motion track toward
  target 110 (or a point 1024 units from center 640,768,640), then spawns
  afterimages until track state 55D becomes 2. Candidate retained in
  wip/sub_081A9178.c. Remaining differences include stack vector construction,
  motion-track pointer spills, state-switch block shape and register lifetimes.

Only the qualifying draft and these notes are committed. Other scratch C is
kept outside production src/. No symbols, generated assembly, ROM or emulator
outputs were committed.

## Task 08175008 (2026-10-20)

Tags: #matching #hud #numeric-display

- 081793EC matched as FieldHud_UpdateTimeDigits. Caller FieldHud_UpdateWidgets supplies its widget base. Pointer at +0x3A8 supplies a time value: divide by 60, scale remainder by 169/10, render two groups of three glyphs. Values above 59999 select glyph 50 for all six. Repeated subtraction (100, then 10) and explicit u16 glyph casts matched immediately. No register pins.
- 081779E8 matched as Hud_LoadLayoutResources. Called by Hud_InitAlternateLayout and SceneHud initialization. Loads nine CB05 resource headers (32 bytes each), relocates each, then finds 1C1C and initializes four 40-byte effects at +0x4CE0 with origin (-4,-4). A separate origin local initialized before the descending loop moved constant initialization to the exact ROM order. No register pins.
- 081796D8 drafted 97.5% as FieldHud_UpdateNumberDisplay. Smooths source value by 10 when difference exceeds 50, otherwise 1, clamps rendering to 0..999, pulses number palette below threshold, and draws current/maximum numbers with a separator. Best candidate has 9 realigned differing instructions (all structural), involving 0xFFFFD000 tile attributes narrowed by the compiler to 0xD000. Explicit separate red/green/blue resets reproduce ROM store order; chained assignment reverses it. Tried signed 16-bit full-width bitfield stores without improvement. Inline OR/store wrappers retained the full constant but removed register copies and changed earlier offset registers (19 realigned differences); retained direct stores. No pins retained. Guarded and recorded with check.py --nonmatching --record.
- 08175008 stopped unmatched. Reconstructed allocation selection input routine in local wip/sub_08175008.c; first candidate 64.8%, 169 realigned differences / 37 ignoring registers. It scans three signed pending counts, handles up/down selection and left/right stat allocation, confirms or cancels, updates prompt icons and description. Pinning p/loop/input locals worsened structure (98 ignoring registers); reverted. Below draft threshold, so no production C or committed draft.
- 08176BA4 stopped unmatched after assembly/caller/relative review. Hud_SetUpdateCallback installs it during menu exit. Advances selection bounds for 17 ticks, restores display registers and persistent songs, then dispatches pending health burst, script arguments and three particle effects. Stack-packed position/extent and script argument structures need careful reconstruction; relatives explain only short common sequences, not a reusable candidate. No production changes.
- 08176F70 stopped unmatched after BIG_FUNCTIONS guidance, full/condensed listing and relatives review. Hud_InitAlternateLayout calls it after resource loading and panel refresh. Large widget layout initializer with repeated sub_0821983C calls, a packed animation stepper, and multiple loops. Largest relative covers only 9%; no Ghidra export exists locally. No candidate retained or production changes.

Validation: only tools/check.py per-file checks; both matches rechecked after clang-format. No full builds or network operations. Existing tools/cloud_task.py modification was present at entry and untouched.

## Task 08176F70 (2026-10-20)

Tags: #matching #hud #actor-decision #palette

This assignment overlaps retained work in the checkout. No new C bodies reached
exact matching; exact progress gain is 0 percentage points. Revalidated the
existing exact body at 0817BDB0 and retained the two existing qualifying drafts,
adding the required file-local aliases in separate driver commits. No full
build, network operations, shared-doc edits, generated files or ROM commits.

- **0817C3F4 — retained draft 83.3%.** BossSlot2_SelectCombatAction selects
  actions by mode 4EA, previous choice 4E9 and event index 4EB, with vitality
  thresholds 274/276 and summon pointers C60/C64. Reviewed existing C and the
  assembly entry under BIG_FUNCTIONS guidance. Final full-extent check records
  530 realigned differences, 407 ignoring registers. The callback has a shared
  tail reached through several long-branch islands. Removing the initial event
  pointer assignment gives 83.5% but leaves paths with an uninitialized pointer;
  rejected and restored the valid retained source. Added local address alias.
- **08176F70 — stopped unmatched.** Reviewed prior task notes, relatives and a
  fresh condensed listing (build/codex-low-6f70.txt). Hud_InitAlternateLayout's
  large layout initializer repeats widget creation and embeds an animation
  stepper; its best matched relative explains only 9%. Ghidra export remains
  unavailable. No new complete C candidate or production body.
- **081796D8 — retained draft 97.5%.** FieldHud_UpdateNumberDisplay remains nine
  realigned structural differences: four full 0xFFFFD000 literals narrowed to
  0xD000 materialization, plus pool effects. Tested a pinned full-width constant
  through an inline digit store and an empty compiler barrier: structure fell
  to six but total differences rose to 95 and score fell to 63.8%. Reverted the
  experiment; no pins/barriers retained. Added local address alias and reran
  check.py --nonmatching --record.
- **0817B0B4 — stopped, candidate 59.8%.** Tentative name
  SceneVolume_UpdateSequencePalette. Mode 506 selects a 24-tick fade from global
  palette bank 2E80 to 680, fades between 680 and 2DE0, or a repeating pulse.
  Counter 504 is read signed, shift 507 controls duration. Destination is C00.
  Fraction clamps to 0..24 and otherwise truncates signed (6144*t/limit)/256.
  Reset uses SceneVolume_ResetPendingSequence. Typed fields, unsigned storage
  with signed reads, and a parameterized interpolation fraction slightly
  improved the first 57.7% candidate. Compiler merges call tails differently;
  202 realigned differences, 166 ignoring registers. Kept below-threshold C in
  wip/sub_0817B0B4.c only.
- **0817AD7C — stopped, best candidate 74.1%.** Tentative name
  ActorDecision_SpawnBobbingFormation. Starts animation 301, selects initial
  direction 0 or 32 from RNG, emits four actors at quarter-turn offsets using
  radius 256 and adds 320 to Y. Each spawn uses target pointer 110 and signed
  parameter 500; steering direction is initial angle +16 then steps by 64.
  Typed actor position and a magnitude-taking sine inline improved 70.0% to
  74.1% (50 realigned differences, 31 structural). Vector-add inlines improved
  structure but worsened total; explicit pinning of base/mask/loop/position
  registers also worsened scores. Restored the best unpinned source in
  wip/sub_0817AD7C.c. Remaining issues include stack spills, sine multiplication
  register choice and vector temporary lifetime. No production draft.
- **0817BDB0 — existing exact body verified.** ActorDecision_UpdateHazardPath
  passed byte-exact check after adding its local alias. Entire 352-byte extent
  ends at 0817BF10. Consumes queued action, selects path/point with RNG, requests
  path movement, and emits proximity hazards on consumed path events. This is
  verification/naming of existing C, not newly recovered exact coverage.

Only per-file check.py acceptance checks were used. Commit messages identify
addresses and existing draft scores; integration must distinguish alias-only
changes from new code progress.

## Task 080D2584 (2026-10-20, follow-up)

Tags: #matching #mummy #scene-actor #register-pinning

- 080D2584: stopped unmatched after revisiting assembly and condensed runs. Prior attempt already identified four terrain-height copies and paired route probes. No complete candidate or new promising whole-function port; Ghidra export is still absent.
- 080D5778: retained existing guarded Mummy_ResumeDelayedRoute draft at 88.3%, 134 realigned differences / 39 ignoring registers. Changing PickTurn's for loop to do/while produced identical output; unsigned counter comparison regressed one instruction; pinning actor to r7 regressed substantially. Added required local address alias and local alias for callee Mummy_RequestStraightRoute (080D6FDC), absent under its descriptive name from the local ELF.
- 080D3D60: retained existing guarded Mummy_UpdatePlayerRoute draft at 82.4%, 194 realigned differences / 70 ignoring registers; added required local address alias. No new recovered C coverage.
- 080D3878: improved existing guarded Mummy_UpdateDefeat draft from 86.2% to 86.4% by pinning frame argument to r8, matching the entry argument move. Remaining 86 realigned differences / 34 ignoring registers. Pinning animation pointer to r9 regressed, rejected. Existing register-concession marker retained. Added required local address alias.
- 080D453C: stopped below draft threshold. Exploratory Mummy_ReactToNearbyStimulus candidate in wip/sub_080D453C.c scored 60.2% candidate-length, 218 realigned differences / 116 ignoring registers. Clears flags2D0 bit 24, gates on action/motion flags and cooldowns, finds overlapping stimulus, sets 180-tick timer and behavior 11 on first activation. Resolves tagged position, tries nearby-player tracking, restores saved route and attempts navigation toward stimulus. Success sets motion flag 0x200, context+0x6EA=4 and attachment target; return reports first activation or fallback. No production source retained. Needs field lifetimes and inline contract reconstruction before allocation work.
- 080D53F8: retained existing guarded Mummy_UpdateProbeNavigation draft at 80.0%, 95 realigned differences / 30 ignoring registers. Removing speed r6 pin worsened to 77.4%; narrowing saved turn packet argument to u8 worsened to 77.9%; widening scale temporary made no difference. Added required local address alias and straight-route callee alias.

Four draft updates queued separately through build/commit.sh. Scores recorded with check.py --nonmatching --record. INCLUDE_ASM fallbacks now use address names explicitly because the macro stringifies its argument; all four default fallbacks passed byte-exact per-file checks. No exact C matches and 0 percentage-point exact coverage gain. No full build, network operation or shared-doc edit; combined validation remains with integration.

## Task 080D62CC (2026-10-20, follow-up)

Tags: #matching #mummy #dog #scene-actor #register-pinning

All source bodies except 080D812C were already retained on entry. This pass adds no new exact coverage (0 percentage-point gain); the exact function below is verification and removal of an unnecessary hint.

- 080D812C: stopped unmatched. Read entry, caller Mummy_UpdateBehavior, fresh condensed assembly and prior codex-medium/low attempts. Timed burn damage includes the three known far-branch tails and extensive saturating defeat counters. Ivy_UpdateDefeat explains 76% of instruction runs, but the prior complete reconstruction scored only 46.5%. No new complete candidate or promising close port; Ghidra export is absent. Leave assembly intact.
- 080D6608: improved retained Mummy_TryDirectPursuit draft from 87.0% to 89.2%; 144 realigned differences, 26 ignoring registers (previously 179/28). Passing mask 3 into PickTurn makes its constant available before the RNG update and improves allocation. Unsigned/descending/do-while loop forms did not improve further; moving saved-control writes before RNG regressed strongly and was reverted. Added local address aliases for entry and Mummy_RequestStraightRoute. Remaining differences include scratch pointer spill, route vector lifetime, flag-mask load order and register placement.
- 080D9B20: retained SceneActor_FindDirectionalCell draft at 85.3%, 681 realigned differences / 574 ignoring registers. Raw halfword/word getter forms compile identically; widening boolean returns or changing the else-if chain regresses. The high fuzzy score conceals major missing repeated predicate structure: recover the original adjacency inline contract before allocation work. Added local address alias and explicit address fallback.
- 080D6FDC: retained Mummy_RequestStraightRoute draft at 80.6%, 105 realigned differences / 17 ignoring registers. Passing a map-global pointer through Width/Height/Cell/IsLevelOpen with r6 binding regressed to 61.6% (240/111), reverted. The original retains the global pointer address across the scan while this candidate spills it. Added local address alias and explicit address fallback.
- 080DDEC8: existing Dog_RestoreRoute C verified exactly over reviewed 608-byte extent, not a newly recovered match. Added local alias, ran depin --write (four hints to three), and rechecked zero differing bytes. Remaining register concession retained.
- 080D62CC: retained Mummy_FireAcceleratingVolley draft at 94.6%, 17 realigned differences / 10 ignoring registers. Separate r1 height-difference binding, r4 speed binding and pinned quotient temporary each regressed; reverted. Remaining quotient absolute-value copy, loop setup and projectile argument ordering affect pool placement. Added local address alias and explicit address fallback.

Formatted the four guarded drafts directly with the pinned clang-format, recorded each score with check.py --nonmatching --record, and queued separate commits through build/commit.sh. No make, full build or network operations. Combined ROM validation remains with the integrator.

## Task 080E21AC (2026-10-20, follow-up)

Tags: #matching #dog #scene-actor

All six bodies were already present on entry. This pass adds no new exact coverage (0 percentage-point gain).

- 080E25FC: retained Dog_ChooseReactionBehavior draft at 83.0%, 426 realigned differences / 162 ignoring registers. Direct motion-field test regressed to 82.4% (445/182); swapping context/group declaration order regressed to 82.3% (445/170). A turn-selection wrapper compiled identically. Restored original candidate, added required local address alias and explicit address fallback. Branch layout, motion-flag pointer lifetime and PRE stack slots remain unresolved.
- 080E4B34: retained Dog_BeginDelayedRouteRecovery draft at 82.1%, 195 realigned differences / 46 ignoring registers. Pinning outer group pointer to r8 regressed to 70.0% (329/136); rejected. Splitting group initialization and adding a turn-selection wrapper produced identical output. Added required local address alias and explicit address fallback. Shared recovery inline allocation and flag update ordering remain.
- 080E4468: existing Dog_ChasePlayer C verified exactly (972 bytes); added local address alias. Nearby Dog callbacks and chase-control transitions support existing provisional name. No new recovered code.
- 080E4834: improved Dog_ResumeRoute draft from 82.3% to 84.7% by declaring context before group; 64 realigned differences / 24 ignoring registers, versus 76/23. Candidate now 776 bytes against 768-byte target. Reversing OR operands had no effect; deriving context at use regressed to 78.8%, and pinning group to r8 regressed to 81.4%. Remaining control-reset address materialization, literals and allocation need further reconstruction. Added local alias and explicit address fallback.
- 080E21AC: existing SceneActorControl_BeginHiddenDefeat C verified exactly (636 bytes); added local entry alias and local CompleteBlinkOut callee alias, required because the descriptive callee symbol is absent from the local ELF. No new recovered code.
- 080E2428: existing SceneActorControl_CompleteBlinkOut C verified exactly (468 bytes); added local address alias. No new recovered code.

Formatted directly with build/venv/bin/clang-format. Recorded all guarded scores with check.py --nonmatching --record and verified all three default assembly fallbacks exactly. Separate per-function commits queued through build/commit.sh. No make, full build or network operations; combined validation remains with integration.

## Task 0814C3A4 (2026-10-20, follow-up)

Tags: #matching #player #attack #script-control

- 0814C3A4: stopped unmatched. Read entry assembly, regenerated condensed listing and relatives, and reviewed prior attempt. Largest relative covers only 9%, chiefly signed polar-vector calculations. The eight-phase grab/motion routine needs a complete reconstruction; no new viable whole-function candidate. Ghidra export remains absent. Assembly retained.
- 0814E3D8: retained Player_UpdateSelectedAttackCharge draft at 90.0%, 76 realigned differences / 36 ignoring registers. Replacing the first charged-tail goto with a direct phase-four call compiled identically; reverted. Added required entry alias and local projectile callee alias because descriptive symbols are absent from the local ELF. Explicit address INCLUDE_ASM fallback checked exactly.
- 0814D264: stopped at existing local candidate's 75.1%, 163 realigned differences / 59 ignoring registers. Adjacent declaration-order variants for sequence/first and radius/first compiled identically. Geometry, sine-table lifetime and context-address allocation remain unresolved. Candidate remains in wip/sub_0814D264.c; no production draft.
- 0814F328: new guarded draft at 87.8%, 77 realigned differences / 25 ignoring registers. Player_UpdateAttachedAttackCharge is installed by Player_BeginDescriptorAction for action kind 0x30. Runs animation entries 0x26–0x28, creates/updates an attached attack entry, queries its kind-seven value for charge thresholds, releases on input-up, and decays recoil before accepting buffered re-entry. Entry resets the attached handle and installs Player_ReleaseActionEffectOnExit. Started from the adjacent selected-attack charge draft, reconstructed all eight target phases from asm. Mask accessor with caller-side null tests improved initial 84.6% to 87.7% candidate-length. Recorded full extent is 87.8%. Remaining constant reuse, mask/address order, particle initialization and register placement. Callback uses its existing descriptive ELF symbol to retain the Thumb function-pointer bit; raw sub_ alias lost that bit in an early candidate and was removed.
- 0814DFDC: existing Player_SpawnSelectedAttackProjectile C verified exactly (964 bytes). Added required local alias. This is verification/naming, not newly recovered exact coverage.
- 0814FC80: new guarded draft at 89.7%, 44 realigned differences / 18 ignoring registers. Player_AdvanceScriptDestination is installed by PlayerCue_WalkTo for ordinary/action-mode-five scripted walking. Calculates planar destination angle and distance, applies movement slowdown and directional axis scaling, chooses facing/side and walking sounds, then snaps to destination and returns to stand when within one step. Initial 84.7% candidate improved through byte-store inlines that calculate destination addresses after their source values, and loading slowdown activation before initializing its boolean. Remaining flag/null-test merging, literal materialization, field lifetimes and allocation. Name remains tentative.

Four per-function changes queued through build/commit.sh. All three guarded drafts recorded with check.py --nonmatching --record; their default assembly fallbacks passed exact checks. Formatted directly with build/venv/bin/clang-format. No register pins, make, full build or network operations. Newly recovered exact coverage gain: 0 percentage points; existing exact progress unchanged. Combined ROM validation remains with the integrator.


## Task 0817C3F4 (2026-10-20, follow-up)

Tags: #matching #scene-volume #boss #actor-decision

Four exact C bodies and the guarded combat-decision draft were already present on entry. This pass adds no new exact coverage (0 percentage-point gain). All checks used check.py only; no full build or network operations.

- 0817C3F4: retained BossSlot2_SelectCombatAction draft at 83.3%, 530 realigned differences / 407 ignoring registers. Reviewed BIG_FUNCTIONS guidance, entry and prior notes. Initial event-pointer derivation from q instead of p compiled identically. A parameterized mask accessor regressed to 83.0% (537/409). Eliminating the event pointer in favor of direct q->index accesses retained 83.3% fuzzy but worsened structure to 485 differences; reverted. Keep the original valid initialized pointer and guarded draft unchanged. Long-branch shared tails and repeated decision blocks remain unresolved.
- 0817DA1C: existing SceneVolume_UpdatePursuitSpin C verified exactly over 940 bytes; added required local address alias. Destination pursuit, periodic particles, rotating effect angle and recovery phases support the tentative existing name. No new recovered code.
- 0817E6E0: stopped unmatched, exploratory SceneVolume_UpdateLungeApproach candidate retained in wip/sub_0817E6E0.c at 56.6% candidate-length (242 realigned differences / 79 ignoring registers). Entry translates bounds, clears reached and attack bit 2, optionally plays sound 540. Tracks player/path destination; a 16-tick window changes collision flags and palette settings. Computes a quantized destination angle, tests a 32-unit projected floor height against floorHeight at 4F2, then snaps within radius 50 or advances with animation F9. Completion resets the directional callback. Started from the exact knockback sibling's terrain and direction inlines. Pinning actor/angle/deltas worsened structural differences to 110; vector-add and flag/palette inlines worsened overall shape, reverted. Main issues: vector/address lifetimes, entry-reset store ordering, constant reuse and register allocation. Below draft threshold; no production source or register concession retained.
- 0817E040: existing SceneVolume_UpdateKnockbackLanding C verified exactly over 824 bytes ending at 0817E378; added required local address alias. Height decay, impact velocity and landing animation sequence support existing provisional name.
- 0817D7AC: existing SceneVolume_UpdateRepeatedShot C verified exactly over 624 bytes ending at 0817DA1C; added required local address alias. Repeats aim/fire phases with impact counter capped at three.
- 0817DDC8: existing SceneVolume_UpdateImpactSlide C verified exactly over 428 bytes ending at 0817DF74; added required local address alias. Decays impact speed and advances recovery animation.

Formatted the four alias changes with the pinned clang-format and queued one driver commit per verified exact function. Existing 83.3% draft unchanged. Combined ROM validation remains with integration.

## Task 08187230 (2026-10-20)

Tags: #matching #armor #decision

- 08187230: new guarded draft Armor_UpdateAxisAttackDecision, reviewed fuzzy 83.9% (83 realigned differing instructions, 12 ignoring registers). Reconstructed the axis/range/facing tests and two decision switches from assembly. The player/armor delta rounds X/Z to grid units, checks Y within 127, and selects attack, approach, recovery and collision responses. Pinning only absolute Z to r2 improved the initial 79.8% candidate; pinning all observed registers, Z delta alone, or decision pointers regressed. A timer-relative decision store also regressed. Remaining differences include merged decision stores, branch polarity, pointer offsets and r5/r7 allocation. Candidate is 960 bytes versus reviewed 956-byte extent; it is not an exact match.
- 081875EC: retained existing guarded Armor_UpdateAlignedAttackDecision draft at 83.9%, added required file-local address alias. Assembly is the same axis-decision implementation as 08187230. Checked reviewed extent: 83 realigned differences, 12 ignoring registers. No new recovered code for this address. Both drafts retain their necessary absolute-Z register hint; depin skipped nonexact inputs.
- 08187F44: stopped below threshold; best exploratory Armor_UpdateCloseCombatDecision candidate 65.9%, 308 realigned differences / 146 ignoring registers. Biggest function inspected first. Countdown and fast-mode toggle feed close-range decision states, cardinal alignment chooses randomized lateral attacks (RNG bit 4 selects behavior 36 versus 24), and ordinary/fast modes select different approach behaviors. Separating angle calculation from the facing comparison prevented a facing load from moving before Math_Atan2 and improved initial 60.8%. Actor pinning and explicit collider-pointer/delta-order experiments regressed. Shared branch tails, collider address form, position lifetimes and register allocation remain unresolved. Candidate remains in wip/sub_08187F44.c.
- 081879A8: stopped below threshold at 63.2%, 289 realigned differences / 131 ignoring registers. Close-attack sibling of 08187F44: four lateral choices use behavior 24 directly rather than random selection; close-entry timer assignment joins a later shared tail. Ported the independently reconstructed close-combat candidate using assembly comparison. Pointer lifetimes, branch sharing and register placement remain unresolved; wip/sub_081879A8.c only.
- 081885BC: stopped below threshold at 73.0%, 234 realigned differences / 167 ignoring registers. Primary behavior dispatcher calls this when actor flag 0x200 is clear. Three timer decrements, two six-state switches, cardinal-axis attacks and facing/range tests drive behavior selection. State 3 restores saved fields B26/B28, copies B28 to 274, and clears flags 0x200004 at 21C/1C8. Ordinary decision 1 can request behavior 31 after timer1130 exceeds 300 and Armor_IsSequenceIncomplete succeeds. Separating angle evaluation improved 70.8% to 73.0%; pinning observed actor/delta registers regressed substantially. Shared reset tails and switch layout still differ. Candidate remains in wip/sub_081885BC.c.
- 08188B7C: stopped below threshold at 71.9%, 239 realigned differences / 168 ignoring registers. Alternate behavior dispatcher selects this when flag 0x200 is clear. Near sibling of 081885BC: type 13 rather than 12 selects behavior 41; ordinary decision 1 uses distance greater than four to select behavior 9 rather than the timer1130 collision test; behavior 29 uses unconditional SetBehavior rather than SetBehaviorIfFinished. Reconstructed sibling candidate remains in wip/sub_08188B7C.c.

No exact progress (0.0 percentage-point gain). One new production draft and one existing draft alias correction. Both default assembly fallbacks checked byte-identical over 956 bytes after changing INCLUDE_ASM arguments to raw sub-address identifiers (the macro stringifies its argument). Corrective fallback commits follow the initially queued draft commits. Ghidra export is absent. Only fast per-file checks were run; no full build or network operation. No shared docs or generated assembly committed.

## Task 081559A4 (2026-10-20, follow-up)

Tags: #matching #player #action #register-pinning

- 0815767C: matched as Player_ApplyControlPresentation. Started from the existing 88.2% guarded draft; added its missing address alias and corrected its stringified fallback before checking. All 30 initial differences were allocation-only. Small offset helpers pin argument offsets to r1/r2 for shadow and drawable accesses; collider insertion pins pointer/value/mask to r2/r1/r0, and jitter pins its result/mask to r1/r0. Formatted final source checked exactly over 528 bytes. Marked register-pinning concession.
- 0815788C: matched as Player_UpdateActionModeMovement. Existing untracked candidate passed exact check immediately, then again after promotion and formatting (1136 bytes). Dispatches ordinary, controlled and scripted movement updates; updates camera/callback/presentation and tests projected terrain for action-four entry. Existing local alias and tentative interpretation retained. No register pins.
- 081559A4: new guarded Player_UpdateTimedChargeAction draft, reviewed fuzzy 87.3%, 65 realigned differences / 30 ignoring registers. Six phases select variant animation 0/8/9/10/11, hold/release song 216, emit near particles, accumulate charge using context+0x876, and call the saved script with success or timeout. Explicit frame/count pointers and a local narrowed elapsed value improved the initial 70.8% candidate; duplicating the initial charge transition and placing the shared pause label within phase two reproduced more of the ROM layout. Actor pinned to r4, marked concession. Remaining pointer lifetimes, zero/one reuse, script-call setup and allocation differences. Default assembly fallback checked exactly (948-byte extent); C candidate is 952 bytes. Committed separately through driver.
- 08155D58: stopped below draft threshold. Reconstructed pulsing-charge sibling from 081559A4: reverses held-bit polarity, uses far particles, songs 740/569, increments charge by four, stores phase five directly, and oscillates AD8 over ticks 0..64 with AD6=367. Initial candidate 73.9%; explicit pulse tick/frame locals reduced size but regressed to 72.9% (179 realigned differences / 76 ignoring registers). Removing actor pin regressed to 70.3%, reverted. Candidate remains untracked at wip/sub_08155D58.c; no production draft. Pulse-field address lifetimes and shared zero registers need reconstruction.
- 081561EC: stopped unmatched after revisiting largest-first entry assembly, relatives and condensed listing alongside prior notes. Best relative explains only 17%, largely RNG/trigonometry. Particle-pool lifecycle, target-motion phases, inline animation stepping and local far-branch tail remain substantial; no promising whole-function candidate. Condensed listing is in build/codex-low-081561EC.txt.
- 08156FE0: stopped unmatched after revisiting entry, relatives and condensed listing. Best relative explains 11%; two sequence-selection tables, signed status-byte fallback, input/action-mode gates and interaction transitions still need a dedicated reconstruction. No complete candidate retained. Condensed listing is in build/codex-low-08156FE0.txt.

Two exact matches and one new guarded draft committed separately through build/commit.sh. Notes committed last. Ghidra export remains absent. Only check.py per-file compilation used; no make, full build, network operation, shared-doc edit or generated assembly committed. Combined ROM validation remains with integration.

## Task 0821BF5C (2026-10-20, follow-up)

Tags: #matching #terrain #scene-indicator

- 0821C8F8: stopped unmatched after revisiting entry assembly, fresh condensed listing and relatives. Largest relative still explains only 3%; prior analysis of direction-specific paired probes and ten stack temporaries remains applicable. No complete viable candidate; Ghidra export is absent.
- 0821C254: stopped unmatched after revisiting entry/caller and prior full-assembly analysis. Eight adjacent-cell cases need explicit diagonal orthogonal probes and shared clamping tails; no new close candidate.
- 0821D178: stopped at retained scratch candidate's 62.1%, 193 realigned differences / 86 ignoring registers. Segment/cell clipping and packed-position layout still need reconstruction. No production draft.
- 0821D6F4: new guarded Terrain_HasHigherCorner draft, recorded reviewed score 97.7%, 8 realigned differences / 6 ignoring registers over the 800-byte target. Builds center/height values, selects positive-X/Z corner probes by subcell threshold, and returns whether any probed cell has greater base height. Initial candidate 92.3%; grouping cell offset arithmetic before adding the base and reversing the final comparison improved it. Remaining differences are Z halfword-load plus mask versus byte-load selection and pool alignment. LowZ accessor, mask argument and u16-return wrapper did not improve output. No pins. Default assembly fallback checked byte-identical. Driver commit title used the earlier candidate-length 97.2%; source records the authoritative reviewed 97.7%.
- 0821BF5C: stopped at retained scratch candidate's 70.8%, 167 realigned differences / 125 ignoring registers. Rechecked prior slope-transition reconstruction; no new structural hypothesis. No production draft.
- 0821DA14: matched Terrain_GetHigherNeighborMask, 936 bytes. SceneIndicator_CreateFromScript consumes its result with mask 14. Samples five offsets around the center of the current cell: (-X,+Z), (0,+Z), (+X,+Z), (+X,0), (+X,-Z), returning bits 1/2/4/8/16 for higher base heights. Unsigned 16-bit packed fields reproduce halfword loads and packed initial stores; later writes naturally simplify to strh. Using addition rather than OR for the Z-center offset reproduced the final separate constant load and yielded byte equality. No register hints. Formatted and checked the production file exactly before its separate driver commit.

One new exact match and one new guarded draft; only fast check.py checks, no full build or network operations. Notes committed last through build/commit.sh. Combined validation remains with integration.

## Task 0820C4D4 (2026-10-20)

Tags: #matching #sprite #screamer #pink-spine

Exact code gain: 0.0 percentage points. Five existing guarded drafts revised or made independently checkable; no exact matches. All six targets stopped after the hypotheses below. Only per-file check.py compilation used; no full build or network operations.

- 08214600: stopped below draft threshold at 64.6%, 278 realigned differing instructions / 73 ignoring registers, reviewed extent 1348 bytes (entry through final literal pool before 08214B44). New scratch candidate at wip/sub_08214600.c, tentative name Sprite_QueueFrameParts. Reads graphics+0x14 frame descriptors (12 bytes each), clips frame parts, computes affine transforms with signed magnitude shifts, caches tile/palette data, and submits to direct/depth OAM queues. Sprite+0x10 is an eight-byte frame/offset group; part descriptors are four bytes, with shape at +0 and signed XY at +2/+3. Width, height and tile count tables are 03004D90/03004D20/03004D30. Initial candidate was 56.9%; preserving the frame group and shape/flag lifetimes improved it to 64.6%. Forcing the part pointer to r6 spilled initial XY; volatile visible count and explicit output cursor made queue loops worse (61.0% / 53.3%), reverted. Remaining stack placement, frame/offset pointer lifetimes and transformed queue loops are substantial. No production draft or commit for this function. Ghidra export absent; matched-relative coverage was only 2%.
- 0820CD00: revised Screamer_InitFromScript draft, recorded 88.1% versus baseline 81.1%; 89 realigned differences / 16 ignoring registers. Preserve Script_GetValue's full result for the paired life stores, then narrow only for the sign test. Widening the orientation-switch selector to u32 and introducing a separate initial sprite lifetime reduced structural differences from 27 to 16. Hard-pinning sprite/initialSprite to r5 regressed badly; a non-pinned initialSprite with an empty constraint barrier after sequence lookup improved output. Duplicating switch default stores regressed; reverted. Remaining orientation range guard, animation pointer lifetimes, address/register allocation and explicit defined success return. Caller consumes the sign of the incidental ROM return, so retained the existing defined zero return rather than introducing an uninitialized result.
- 0820C4D4: revised Screamer_UpdateScream draft, recorded 94.9% versus 94.4%; 34 realigned differences / 7 ignoring registers. In phase one, the animation stepper's first mask can reuse the known phase value (one). Added a local specialized inline for this loop, preserving readable command playback and its r2 mask temporary. Remaining field-address allocation, other playback constant copies and phase liveness prevent equality.
- 08210654: revised PinkSpine_InitFromScript draft, recorded 93.5%; 25 realigned differences / 6 ignoring registers. Separate initialSprite from the post-lookup sprite lifetime using the same empty-constraint technique as Screamer initialization. Hard r5 pinning reduced similarity; reverted. Remaining high-register sprite copy timing, animation pointer recomputation, field-address constants and explicit defined success return. Tentative species prefix follows the baseline creator and palette interpretation.
- 0820C9AC: retained Screamer_UpdateDamageParticles draft at recorded 81.3%; 93 realigned differences / 60 ignoring registers. Splitting pause guards into explicit local goto paths generated identical code, so reverted. Random-table lifetimes, particle stack allocation, pause layout and callback return still differ. Added file-local aliases and address-form fallback so the stale-symbol local ELF can link and the recorder can actually find this draft.
- 08210398: revised PinkSpine_UpdateDamageAnimation draft, recorded 88.4%; 46 realigned differences / 21 ignoring registers, down from 29 structural differences in the fresh baseline check (89.3% fuzzy). Explicit typed collider pointers keep the base at +0x6C and flags at +6 rather than selecting a separate +0x72 pointer. This cuts differing bytes from 627 to 125 despite the lower fuzzy metric. Pause-goto experiment had no effect and was reverted. Remaining mask-register choice, pause block/pool layout, animation lifetimes and defined callback return.

All five drafts retain NONMATCHING guards and INCLUDE_ASM in the default path. Every draft now has its own file-local descriptive-name alias, with local aliases for renamed callees absent from the baseline ELF. The fallback must use sub_XXXXXXXX: check.py --nonmatching --record otherwise compiles correctly but silently fails to update a fuzzy comment whose named fallback differs from the emitted sub_ symbol. Scores above were recorded after correcting that form and formatting with the pinned clang-format. depin.py --write was invoked and skipped the nonexact candidates; it did not establish any removable hint. Five separate draft commits queued through build/commit.sh; notes queued last. Combined ROM validation remains with integration.

## Task 08152174 follow-up (2026-10-20)

Tags: #matching #player #animation #cue

Exact progress: 0.0 percentage-point gain; 08153E1C was already exact C.
The checked-out sources use promoted names but the cached ELF and assembly files
use address names. Restored the required file-local aliases and address-named
INCLUDE_ASM fallbacks; no full build or network operation was used.

- 08152174: retained guarded draft at 81.7%, 217 realigned differences, 37
  ignoring registers. Read largest first with asmat and relatives. Pinning the
  player to r6 worsened to 49.0%; removing decoder pins worsened to 80.1%.
  Both reverted. Remaining scale addressing, phase tails and decoder registers.
- 08153A00: improved guarded draft from 94.2% to 96.9%, 17 realigned
  differences, eight structural. Separate explicit negative/nonnegative velocity
  multiplier branches reproduce more of the ROM than a ternary multiplier.
  Unsigned multiplication and value barrier did not improve; memory barrier and
  volatile reload regressed. Retained plain C branch improvement. Sign-test
  folding and direction-store sharing remain.
- 08153224: retained guarded draft 84.4%, 84 realigned differences, 26
  structural. No safe new hypothesis for the partly initialized ROM particle
  word; retained fully initialized words. Constant and phase layout residue.
- 08153610: improved guarded draft from 87.1% to 89.9%, 42 realigned
  differences, 15 structural. Resource ID 0x16F is stored then shifted to derive
  its resource pointer; an empty value barrier retains that dependency instead
  of loading a separately folded offset. Existing register-pinning concession
  marker retained. Timer-mask, field addressing and register lifetimes remain.
- 08153E1C: existing exact PlayerCue_UpdateSideDeparture confirmed at 696
  bytes after local alias restoration. depin --write retained all 11 hints;
  final formatted per-file check reports zero differing bytes. No new exact
  coverage claimed.
- 08152A5C: stopped at existing scratch (38.6% full-extent score), not
  committed as a draft. asmat confirms recoil callback context. Recorded extent
  continues to 08153224 despite an additional prologue at 08152C3C. Boundary
  investigation is needed before the 480-byte candidate can be accepted.

All four guarded candidates were formatted, checked with --nonmatching --record,
and their default assembly fallbacks checked byte-identical. One-function
commits were queued through build/commit.sh; follow-up fallback fixes retain the
address name because INCLUDE_ASM stringifies its argument without macro expansion.

## Task 081540D4 follow-up (2026-10-20)

Tags: #matching #player #animation #cue

Exact progress: 0.0 percentage-point gain. All six targets already had guarded
C drafts. Restored file-local descriptive aliases and address-form INCLUDE_ASM
fallbacks for the cached ELF; all six default fallbacks pass check.py exactly.
Only per-file checks were used, with no full build or network operations.

- 081543F0: retained PlayerCue_UpdateSideFade draft, recorded 86.4%, 131
  realigned differences / 29 ignoring registers. Reviewed largest first using
  asmat and relatives; the exact SideDeparture sibling shares its playback.
  Removing sprite/state pins worsened to 79.0%; specializing final playback
  with swapped pointer/mask pins worsened to 81.6%. Reverted both. Scale field
  addressing, phase tails and playback allocation remain.
- 08155008: retained PlayerCue_UpdateAlternateFade draft, recorded 86.0%,
  131 realigned differences / 28 structural. Resource414/selection46B variant
  of the same phase machine. Removing pointer pins worsened to 79.4%; swapping
  final playback pins worsened to 82.2%. Reverted; same remaining issues.
- 081540D4: improved PlayerCue_UpdateSideArrival draft from 91.5% to 98.5%,
  nine realigned differences / seven structural. Reuse the phase value for
  Face's zero flip and StepEnd's first mask; assign animation completion after
  the index branches through shared labels. Removing the phase mask barrier
  cuts one more difference. Completion store sharing and mask copies remain.
- 08154AF0: retained PlayerCue_UpdateSideRecoil draft at 97.5%, six realigned
  differences / zero structural (seven differing bytes). ROM uses r2 for the
  recoil offset and r5 for direction offset; C uses r1/r2. Explicit offset pins
  with barriers and a cached recoil pointer changed CSE and regressed to 95.7%;
  keeping later field accesses independent regressed to 89.2%. Both reverted.
- 08154CD4: improved PlayerCue_UpdateSpiralRecovery from 88.8% to 93.8%,
  21 realigned differences / six structural. Keep resource 0x16F as a value
  stored before shifting to derive its pointer (empty value barrier), and
  preserve the widened increment before masking the cue timer. Remaining
  resource-load ordering, effect timer constant copies and address allocation.
- 081556E0: improved Player_UpdateStateMotionCue from 87.8% to 88.3%,
  54 realigned differences / 31 structural. Express event bit31 as a signed
  negative test, retaining the nullable-player guard. Mask argument barriers
  regressed structure and were removed. Event mask ordering, resource selection
  store sharing and playback still differ.

Formatted with the pinned clang-format and recorded every score with
check.py --nonmatching --record. depin.py --write tested default assembly
fallbacks and incorrectly removed every hint from guarded drafts; restored the
candidate hints and rechecked the recorded scores. Do not use its default path
as evidence that a NONMATCHING candidate can lose its pins. Six one-function
draft commits queued through build/commit.sh, followed by these notes.

## Task 081172C4 — 2026-10-20

Tags: #matching #glove #actor-motion

- **081188F4 — matched, Glove_UpdateRecoil.** Exact 664-byte match, with no
  register concessions. Initializes animation zero, updates recoil and clamped
  movement, eases the three offset velocities, updates facing/collision and
  accessory animation, and advances the particle emitter. The matched
  ActorPhysics_UpdateFallMotion (08120B44) supplies the shared movement layout.
  This actor embeds that movement object at +0x18; its model starts at +0x610.
- **08117948 — drafted 82.3%, Glove_UpdateStopping.** Full-size recorded
  NONMATCHING check: 144 realigned differing instructions, 55 ignoring registers.
  Splitting the action/status work into inline routines improved the first
  57.4% attempt substantially. Remaining differences include action flag masks,
  zero-store scheduling, pointer lifetime/spills, and register allocation. No
  register pins retained. Reads the actor's +0x8C4 input pointer and handles the
  action kind at +0x2CD, then recovery/cooldown fields relative to the motion
  object. The name follows Glove_BeginStopping and the script stop callback.
- **081182E4 — stopped below threshold (best 76.1%).** Tentative
  Glove_UpdateDefeat: clears action/sprite state, hides markers, restores model
  palettes and emits timed flashes before selecting a recovery callback after
  120 ticks. Explicit cleanup field pointers and an ascending model-part offset
  improved structure. Still differs in pointer setup/spill scheduling, loop
  addressing and the common motion block. Best scratch source remains in
  wip/sub_081182E4.c; its scalar height pin is experimental, not exact credit.
- **08117F6C — stopped below threshold (best 74.1%).** Tentative
  Glove_UpdateRecovery: blinks model flags, eases the motion object's +0x58
  halfword toward +0x5A, then restores its marker/color effect and switches to
  08117948 after 120 ticks. Explicit motion pointers plus a height register pin
  improved the attempt, but pointer spills, mask order and tail lifetimes remain.
  Scratch source: wip/sub_08117F6C.c.
- **081175F8 — stopped below threshold (best 70.0%).** Tentative
  Glove_UpdateLaunch: accelerates +0x914 by 0x20 until position exceeds 0x40000,
  decelerates by 0x100, then installs 08117948 in phase four. The common motion
  block retains allocation and pointer-lifetime differences. Inline damp/physics
  boundaries and explicit pointer experiments did not establish an exact path.
  Scratch source: wip/sub_081175F8.c.
- **081172C4 — stopped below threshold (best 71.3%).** Tentative
  Glove_UpdateLaunchDisplay: related launch phases, but creates the stage icon
  display with Glove_BeginStopping and sets flag 0x10. Same motion residue as
  081175F8; declaration swaps and register/pointer variants did not improve it
  enough to retain as a production draft. Scratch: wip/sub_081172C4.c.

Ghidra export gen/ghidra/decomp.c is absent. Used assembly, named callers and
matched relatives only; no network, full build, generated-file edits or other
workers' candidate sources. Fast checks verified the exact match again after
formatting. Final notes commit also formats that already-matched unit and removes
unused local prototypes without changing its bytes. The guarded stopping draft
uses INCLUDE_ASM in the default build. All other attempts remain outside src/.

## Task 080F52C0 — 2026-10-20

Tags: #matching #sword #nonmatching

Exact code gain: 0.0 percentage points. No full build or network operations.

- 080F52C0: drafted **95.5%**, tentative Sword_BeginTargetOrbit. Caller
  080F6170 installs this callback. Reads owner +160 and owner's target +60,
  computes heading to target, chooses random turn sign, saves start position,
  constructs an 800-unit trigonometric endpoint with target height +220,
  stores deltas, and installs ActorPhase_AccelerateTurn. A local typed endpoint
  layout improved initial 51.4% to 79.0%. Keeping the rotation subtraction
  full-width with an empty constraint and using a constrained r2 RNG mask,
  stored after direction, reached 95.5% (7 realigned, 4 structural differences).
  Remaining differences are RNG table load order/register, negative-one setup,
  and resulting alignment. More pointer/negative-value pins regressed. The
  translation unit is marked register-pinning; no instruction bodies used.
- 080F645C: improved existing Sword_UpdateDamage draft **96.5% to 97.2%**.
  A scoped r2 result for the final category mask fixes its operand destination.
  Remaining: health load scheduled before forming damage pointer, and +538
  counter registers (9 realigned instructions, 2 structural). Explicit counter
  pointer/value pins worsened allocation, so reverted those experiments.
  Added the required local alias. Both drafts have recorded fuzzy scores and
  raw sub-address INCLUDE_ASM fallbacks, which pass default exact checks.
- 080F75C0: stopped below threshold. New raw-pointer contact response candidate
  reached only 29.1%; removed it from src. Existing wip/sub_080F75C0.c and the
  earlier 73.1% notes remain the better starting point. Typed fields, the u8
  inline availability predicate, and pre-loop inline state-pointer arguments
  are needed to retain the ROM's address scheduling; a direct scalar loop
  does not reproduce it. No draft committed.
- 080F67AC: stopped after assembly/condensed listing and relatives inspection.
  Existing notes already describe the full initializer. Nearest matched seed
  covers 23%, chiefly the animation stepper. The mixed sprite, palette,
  script-option, and collider initialization still needs full typed recovery;
  no new candidate or progress beyond the prior attempt.
- 080F6F90: stopped after condensed listing and relatives inspection. Script
  collision options and four-child creation share only ~20% with matched code;
  the packed stack vectors and inline animation state need joint recovery.
  Prior detailed notes remain applicable; no candidate retained.
- 080F79A8: stopped after assembly/condensed phase and relatives inspection.
  Timed orbital movement has several distinct phases, repeated vector/trig
  operations, sounds, RNG and attack collider updates. No close full-body seed
  was found; prior notes apply. No candidate retained.

Ghidra export is absent. Used local assembly and fast check.py only. No generated
assembly, ROM, or emulator output is included in these commits.

## Task 08146EAC (2026-10-20)

Tags: #matching #player #animation

- 08146EAC: already matched in this checkout as Player_UpdateAlternateFormCue. Verified byte-identical with check.py (2188 bytes); used a scratch file-local alias because the cached ELF predates promoted names. No production change.
- 08147768: already matched as Player_UpdateFormChangeCue. Verified byte-identical with check.py (1416 bytes) through the same scratch alias technique. No production change.
- 08147CF0: existing Player_UpdateSpiralRelease guarded draft remains 97.1% over the reviewed extent, 11 realigned differences / three ignoring registers. Inspected the shared packed-animation stepper and tail; existing offset-pinning attempts were already unsuccessful. No production change.
- 08147FB8: new guarded draft, Player_UpdateSpiralAfterglow (tentative), recorded 96.6%. Player action dispatch table gUnk_08610B70 references it immediately after spiral release; repeats the timed particle/palette sequence without the facing changes, uses song 0x3A1, palette 0x16F, and completion latch value four. Animation resource is at 0x410, unlike 0x40C in the other two new targets. Reused the stepper and changed its elapsed mask pin to r5. Remaining nine realigned differences / three ignoring registers are tail address temporaries, constant derivation and the trailing literal pool. Additional pointer pins regressed; restored the best candidate.
- 08148318: new guarded draft, Player_UpdateWaveringMovement (tentative), recorded 82.7%. Player dispatch table gUnk_08610CA8 references it. Clears status bit zero, selects the smoothed direction or a neighboring direction with RNG, holds it for nine or three frames using byte 0x469, folds heading/side, and applies directional velocity nine. Delaying timer pointer construction until after state entry fixes the initial block. Remaining branch merging, RNG result allocation, an unused original 0x0403B714 literal and stepper allocation/pool layout differ. Signed RNG result pinning and explicit chosen-label tails regressed and were reverted. Realigned structural scoring is unstable around these divergently placed blocks; reviewed full-extent fuzzy is the recorded acceptance-for-draft measure.
- 08148608: new guarded draft, Player_UpdateDirectionRecovery (tentative), recorded 88.6%. Referenced by the Player dispatch table beginning at gUnk_08610CA8. Begins action 23, folds the first nonnegative directionHistory sample into heading/side, advances sequence one from resource 0x40C, and returns to action zero when movementAnimationState is negative. Remaining 27 realigned differences / six ignoring registers are primarily owner r6/r7 versus command-pointer r7/r6 allocation, facing store address scheduling and the last byte-field address temporary. Pinning owner r7 produced erroneous frame-relative accesses in agbcc; pinning the command pointer also regressed. Retained the unpinned owner form.

Exact gain: 0 percentage points; two pre-existing exact matches verified, one pre-existing draft retained, three new guarded drafts. New drafts retain register-pinning concessions in their shared packed-command steppers. Only check.py checks, no make/full build/network operations. Ran depin as required, but it tested guarded drafts' default INCLUDE_ASM branches and removed every hint while claiming success; restored best candidates and rechecked with --nonmatching --record. Future depin use on guarded drafts must test the actual candidate branch. Formatted directly with the pinned build/venv/bin/clang-format. Per-function commits queued through build/commit.sh.

## Task 081FA718 — 2026-10-20

Tags: #matching #ratatosk #orbit-effect #script-combat-actor

- 08201898 matched as `ScriptCombatActor_TryProjectileAttack` (480 bytes including pool). Checks with reviewed extent 480 are exact. It tests the player's horizontal squared distance, samples four points along the full 3D segment against terrain height, then starts the projectile attack sequence. The matched VulcanCannon_HasClearShot supplied cellIndex/cellData and packed sample types. Crucial technique: inline distanceSquared(a,b), rather than open-coded accesses to p->position, exactly reproduces the base-pointer lifetimes and register allocation. No pins.
- 081FDDC0 drafted 88.4% as `OrbitEffect_SpawnOrbitTrail`. Every fourth tick activates one of twelve trail sprites, picks radius 768..1279 and angle from the random table, and positions it around the actor using signed sine products truncated toward zero. Ported the layout and setup from the existing OrbitEffect_SpawnHomingTrail draft and the trigonometry from OrbitEffect_UpdateTrailSprite. Final realigned difference: 19 instructions, 4 ignoring registers (two missing register copies plus their alignment padding). Timer-mask copy and sine-table lifetime remain; pin experiments did not improve the retained unpinned source. Default path remains INCLUDE_ASM.
- 08201A78 drafted 80.2% as `ScriptCombatActor_UpdateProjectileAttack`. Emits randomized projectiles near the end of a timed attack, adjusts vertical launch velocity by player distance, jitters the actor around its stored origin, and returns to 08201898 when the timer expires. Final realigned difference: 71 instructions, 33 ignoring registers. Random-table/global pointer lifetimes, stack-vector addressing, and allocation remain. Inline distanceSquared also improved this candidate. Default path remains INCLUDE_ASM; no pins retained.
- 082015D4 stopped below draft threshold. Best measured initial candidate 69.1%, 88 realigned differences (56 structural). Callback clears rejected damage, records hit properties, displays damage/HUD feedback, applies player contact stats, and emits three spark particles on selected contact flags. Raw struct/cast accesses produced poor address reuse; stack-vector scalarization and random-loop register lifetimes remain. Scratch attempt retained under ignored wip/codex-low, not production src.
- 081FAE0C inspected first for payoff, condensed, and compared against relatives. Ratatosk initializer: script keywords, five collider initializers, actor/particle/projectile pools, paths, camera and shadow setup. No sufficiently covering matched sibling (largest shared run 29/932 instructions), and no gen/ghidra/decomp.c export. Stopped without a C candidate; assembly remains authoritative.
- 081FA718 inspected and condensed. Ratatosk per-frame collider, visibility/shadow, palette interpolation and camera synchronization. Repeated Collider_SetPosition runs do not cover enough of the 673-instruction body to supply a reliable port. Stopped without a C candidate; assembly remains authoritative.

Only tools/check.py per-file compilations were used for validation; no make, full build, network, or direct git mutation. Exact gain is one function; aggregate percentage reporting is deferred to batch integration. Commit requests went through build/commit.sh.

## Task 081ED8A0 — 2026-10-20

Tags: #matching #scene-actor #animation #particles #palette

No exact matches: exact code gain is 0.0 percentage points. Five guarded drafts were retained; all scores below are the recorded full reviewed extent from `tools/check.py --nonmatching --record`. Default builds retain INCLUDE_ASM.

- 081EE518: drafted 82.2% as `SceneActors_UpdateSequenceTwoCharge` (tentative). Worked largest first after reading BIG_FUNCTIONS.md. Sequence 2 initializes the action; sequence 3 begins after tick 90, acceleration starts after 150, and screen-X/time/sequence-hit tests select recovery. Calls Player_GetHorizontalVelocity, SceneMover_StepPastScreenX and the common scene-actor update trio. Final realigned difference: 152 instructions, 58 ignoring registers. Second packed animation stepper allocation, random-mask order and pointer lifetimes remain. A stack position query writes three 32-bit coordinates (the scratch reserve is four words), not the 16-bit SceneVec layout used for screen coordinates.
- 081EE130: drafted 89.2% as `SceneActors_UpdatePeriodicEffectAction` (tentative). Sequence 1 emits ActorEffect_SpawnOffsetEffect periodically, with interval/count adjusted for a small field2B4 value; recovery restarts the sequence in reverse and waits for completion. Final realigned difference: 53 instructions, 29 ignoring registers. Removing the long-lived r6 parent-pointer pin improved candidate-length similarity from 77.9% to 89.0%; multiply operand order was a smaller independent improvement. Timing-expression load order, random masks and initialization allocation remain.
- 081EDB74: drafted 85.3% as `SceneActors_UpdateLeftDeparture` (tentative). Initializes sprite sequences 0 and 4, sets horizontal velocity to -0x5000, and transitions to SceneActors_DestroyTask when screen X is negative. Final realigned difference: 58 instructions, 22 ignoring registers. Specialized second sprite stepper with a high-register sprite view improved the candidate past threshold; pointer allocation, reset ordering and constant derivation remain.
- 081ED8A0: drafted 91.6% as `SceneActors_UpdateTimedApproach` (tentative). Initializes sequences 0 and 4, waits until tick 121, then approaches field294 at speed 0x6000; arrival sets completion byte AF and field284 to 16. Final realigned difference: 34 instructions, 11 ignoring registers. Entry/reset allocation, second sprite pointer and random-mask order remain. A volatile local tick copy was retained as an allocation concession. Removing the second sprite's low-register pin avoided pathological stack-relative flag accesses.
- 081EEAD8: drafted 87.0% as `SceneActors_UpdatePaletteDispersal` (tentative). Interpolates two actor palettes and four palette banks; progressively flickers both sprites, waits 50 ticks, emits five particles per tick until count59C exceeds 23, then completes. Final realigned difference: 63 instructions, 27 ignoring registers. Removing long-lived actor-pointer pins and using do/while for both fixed-count loops raised candidate similarity from 69.3% to 87.0%. Remaining differences are initialization register selection and palette/loop setup order. Signed 16-bit packed fields reproduce the word-sized stack vector updates.
- 081EEECC: stopped below threshold; no production source or draft committed. Best candidate-length similarity 58.3%, 157 realigned differences (58 ignoring registers), in `wip/sub_081EEECC.c`. Shakes offsets5E6/5E8 for the first 60 ticks using RAM random table0203B400/index03005308, emits alternate-track particles on odd ticks after 60, installs a secondary effect at counter5A0==58, and blinks out after tick90. Tried ordinary and volatile tick locals, memory constraints, explicit batching of random-index updates, union-word XY construction and a grouped stack work record. No useful path to threshold remained: stack layout, random-index/table lifetimes and register allocation dominate. The tracked assembly remains authoritative.

Boundary evidence: `081EDB74`'s prior reporting extent incorrectly absorbed the independent callback at 081EDE4C (own saved r8-sl prologue after the departure return/pool). Added 081EDE4C to progress extra boundaries and inventory; full draft score is 85.3%, versus the misleading 51.2% against both callbacks. Likewise 081EEAD8 ends at 081EEDDC, where another callback starts with its own push r4/lr and return before 081EEE30; added that reporting boundary. These additions split only reporting extents and leave the independent bodies in assembly. A further independent short callback starts at 081EF12C after 081EEECC's pool; it was observed but not added because no draft was accepted for 081EEECC.

The common packed stepper came from matched PlayerCue_UpdateSideDeparture; empty compiler operand constraints contain no instruction bodies. All retained units carry the register-pinning concession marker and provisional file-local naming aliases. Ghidra export is absent. Only per-file check.py compilation was used; no make, full build, network, push, PR, or direct git mutation. Commit requests used build/commit.sh. Combined ROM validation remains the integrator's responsibility.

## Task 081731E8 — 2026-10-20

Tags: #matching #hud #menu-input #nonmatching

Exact gain: approximately 0.026 percentage points of total code (636 bytes),
without register pinning. Two new guarded drafts; two assigned functions were
already exact. Aggregate totals remain in PROGRESS.md and are not regenerated
by this worker.

- 08176BA4 matched as SceneHud_FinishPageExit. SceneHud_FadeOutPage installs
  this callback after restoring display state. Advances bounds for 17 ticks,
  restores display globals and persistent songs, then handles queued health
  burst, script calls and three particle emissions. The particle position and
  extent are signed 16-bit full-width bitfields packed into two words: position
  offset (-512,-256,-512), extent (1024,512,1024). Script arguments have a u16
  count at zero and value pointer at four. Explicit zero initialization between
  calculating the incremented timer and storing it resolves the last two
  instruction differences. An inline mask helper reproduces mask-before-load.
  Exact again after formatting; no pins.
- 08175008 drafted 94.1% as Presentation_UpdateAllocationSelection. Resumed
  retained 64.8% scratch candidate. Three pending signed counts gate preview;
  up/down select a stat, left/right refund or allocate points, A confirms and B
  resets. Typed allocation arrays, an inline description setter taking the
  already-computed description, and loading the owner's stat before forming
  the pending-count pointer greatly improve address scheduling. Retains r4
  scan/mask, r1 input and r6 selection pins with the required concession marker.
  Remaining 26 realigned instructions / nine ignoring registers concern
  constant ordering and array address reuse/allocation. Typed owner fields and
  reversed mask operands reduce structural differences to seven but lower
  fuzzy similarity; restored the 94.1% candidate. depin was run on the actual
  unguarded candidate; it skipped because this is not an exact match.
- 081731E8 drafted 97.3% as SceneHud_UpdateSecondaryActionSelection. Called by
  the equipment selection page's interaction callback. A saves the secondary
  grid selection and handles reset (39), main action grid (32), or action
  choice (38); navigation uses mask C0 and updates prompt/cursor. Evaluating
  selected-pointer assignment in the second cursor argument preserves address
  order. Loading the selected byte before initializing the prompt bool fixes
  its constant scheduling. Four realigned instructions / two ignoring registers
  remain: compiler reuses the grid address in r6 instead of recomputing it into
  r4 for choice 38, plus pool effects. No pins. Full reviewed score is higher
  than candidate-length score (96.7%) because the candidate omits four bytes.
- 081779E8 already matched as Hud_LoadLayoutResources; verified 536 exact bytes
  using a scratch file-local alias for the cached ELF's original name. No
  production changes or new exact credit.
- 081793EC already matched as FieldHud_UpdateTimeDigits; verified 356 exact
  bytes using the same scratch alias technique. No production changes or new
  exact credit.
- 08176F70 stopped unmatched. Read BIG_FUNCTIONS, regenerated the condensed
  assembly and reviewed relatives plus prior attempts. Hud_InitAlternateLayout
  calls this widget-layout initializer after resource loading. Repeated widget
  creation loops and an embedded packed animation stepper dominate it; the
  strongest matched relative covers only 9%. No Ghidra export is available.
  No new complete candidate or production body; prior detailed notes apply.

Only check.py per-file compilations were used, including --nonmatching --record
for both drafts and exact default INCLUDE_ASM checks. Formatted directly with
build/venv/bin/clang-format. No make, full build, network, generated-file edits,
ROM commits or direct git mutations. Per-function commits queued through
build/commit.sh; combined ROM validation remains for batch integration.

## Task 081B3DB8 — Vanargand part behaviors (2026-10-20)

Tags: #matching #vanargand #animation #nonmatching

Exact gain: 0.000 percentage points. Two guarded drafts; no exact matches.
Read the entry-point and worker docs, BIG_FUNCTIONS, callers/callees and matched
relatives. No Ghidra export exists in this worktree. Worked solo, without network
or full builds. Per-function checks used build/venv/bin/python tools/check.py.

Observed part layout follows Vanargand_UpdatePartRetreat / UpdatePartLunge:
sequence at 0x48, motion at 0x58, base-position pointer at 0x90, origin at 0x9C,
target at 0xA4, variant at 0x15C, behavior flags at 0x160, state/timer at
0x164/0x168, previous/current action bytes at 0x16D/0x16C, reset/enter/finished
at 0x16E/0x16F/0x170. Names below are tentative interpretations from static code.

- 081B431C drafted **80.6%**, Vanargand_UpdatePartImpact. Selects sequence 8,
  moves by a short directional offset, then moves toward one of two fixed impact
  points relative to basePosition. Emits a burst (and possibly a flash timer)
  when the motion completes; emits trails on alternate frames while moving.
  Reviewed-extent check: 140 realigned differing instructions, 38 ignoring
  registers. A typed Set/Difference inline recovered the three-halfword stores
  and eliminated repeated pointer reloads. Remaining differences include pointer
  lifetimes, transition constant scheduling, instruction order and pool layout.
  Saved boss/enter/frame register hints improved the initial candidate.
- 081B4DA4 stopped unmatched. Candidate raises the part toward y=0x300 along
  heading 48 or 16, then returns it to terrain height, setting flag 0x80 when done.
  Candidate-length check reached 79.0%; this is **not** a qualifying draft score.
  Reviewed extent also includes code-like bytes at unlabeled 081B512C through
  roughly 081B52E8: reviewed score only 64.1%. Review this boundary before a
  future complete attempt; do not count candidate-length equality as acceptance.
  Terrain lookup uses map rows, TerrainModifiers_FindCellOverride(cell,1), the
  high-nibble slope and low-nibble height. Retained scratch candidate under wip/.
- 081B48A0 stopped unmatched. Candidate sidesteps 128 units along byte 0x187,
  preserving origin y, then selects sequence 9 and marks finished. Initial reset
  selects sequence 11 and steps it once. Best reviewed score 76.4%, below the
  draft threshold; 113 realigned differences, 41 ignoring registers. Natural
  allocation and helper-owner/command pin experiments did not produce a useful
  next improvement. Retained scratch candidate under wip/.
- 081B4040 stopped unmatched. Creates six radial type-3 Vanargand effects,
  selects sequence 10, waits for flashTimer to reach zero, then advances the
  sequence, plays song 0x58C and calls sub_081B019C after completion. Candidate
  reached 67.2% candidate-length similarity; pointer-hoisting and owner-register
  experiments stalled. Retained scratch candidate under wip/.
- 081B3DB8 stopped unmatched. Creates six type-1 radial effects spaced by
  (i*255)/6, plays song 0x58C, advances sequence 10 and calls sub_081B019C after
  completion. Candidate reached 73.9% candidate-length similarity. Explicit
  state/timer/enter pointer lifetimes recovered some loop hoisting, but register
  allocation, loop constant scheduling and stack shape remain. Retained under wip/.
- 081B4C04 drafted **91.8%**, Vanargand_UpdatePartRecovery. Sets flags 0x12C,
  selects and steps sequence 11 on reset; after frame 180 clears flag 0x100 and
  requests action 22 unless action 25/26 is unfinished. Resets the action timer,
  state and state timer when that transition is accepted. Reviewed check leaves
  19 realigned differences, nine ignoring registers: an extra ip owner copy in
  the animation stepper and register/constant derivation in the action change.

The Ivy_TickDrawable void stepper was a useful source for the reset-only steps;
Vanargand_UpdatePartRetreat supplied the completion-returning stepper. Format
was run directly through build/venv/bin/clang-format, then draft scores recorded
again. tools/depin.py only supports exact sources: on a NONMATCHING wrapper it
checks the default INCLUDE_ASM and incorrectly strips all hints. Restored both
candidates and reran depin on their unguarded scratch files; it correctly skipped
nonmatching bases. Final recorded scores above were verified after restoration.
Each draft has a file-local naming alias and register-concession marker. Draft
commits queued separately through build/commit.sh. No shared docs regenerated.

## Task 080D0C30 — 2026-10-20

Tags: #matching #mummy #collision #nonmatching

Exact gain: 0.000 percentage points. No new matches or qualifying drafts.
The assignment overlaps the earlier Task 080D1784; retained sources were not
recommitted as new progress. No network, full build, or direct git mutation.

- 080D0FE8: inspected largest first with BIG_FUNCTIONS, asmat, relatives and
  condense. Turns toward route-facing byte 0x3CD, chooses transition animation
  by current cardinal quadrant and desired octant, adjusts direction by 21 per
  tick, applies duration scale, and resumes group control kind 27 or saved
  control kind 2. Six animation-selection copies account for much of the body.
  Strongest relative covers 35%, mainly small animation/reset runs; no complete
  reliable candidate recovered. Stopped unmatched.
- 080D2194: reviewed full assembly and relatives against previous 60.8% attempt.
  Prior notes remain applicable: ordinary path retains many state-field pointers
  across terrain and route calls, unlike simple chase helper expansion. No new
  candidate or qualifying draft; stopped unmatched.
- 080D1784: re-read entry and prior attempt. Clears group+0x4C timers, selects
  animation 7, freezes animation mode/frame through tick 59. Prior 61.4% attempt
  needs reset-pointer lifetimes reconstructed; no improved candidate retained.
- 080D0C30: new scratch candidate, tentatively Mummy_OnAttackCollision, using
  Ghoul_OnAttackCollision's unavailable predicate and contact recorder. Context
  at actor+0x3D0 has flag 0x800 at +0x6F2; contact kinds 1..8 clear that flag,
  resolve the contacted object and can queue a timed response with cooldown 30.
  Other kinds only record contact. Initial score 54.1%; explicit boolean helper,
  volatile contact-halfword reload and r3/r6 bindings reduced realigned
  differences from 138/61 structural to 127/53 but lowered fuzzy to 35.0%.
  No production draft: source stays under ignored wip/codex-low. Future work
  should recover the separate saved-owner and input-context pointer lifetimes,
  plus original contact record layout, before adding allocation hints.
- 080D1AFC: existing Mummy_ResumeSavedTarget verified exact over 468 bytes.
  Scratch file-local alias reconciles promoted source name with cached ELF's
  original sub_ name; production source unchanged, no new exact credit.
- 080D1CD0: existing Mummy_UpdateHiddenRecovery draft verified at 94.2%,
  22 realigned differences / 14 ignoring registers, reviewed extent 464 bytes.
  Adjacent declaration swaps, narrowing queued to u8 and narrowing mask to u16
  changed nothing. TakePrepared helper again reduces structural differences to
  two but regresses total to 54 and fuzzy to 76.9%. Retained existing draft
  unchanged; same cached-ELF alias workaround used only in scratch.

Validation used only tools/check.py (including variants' per-file checks).
Combined ROM validation remains for integration. Notes queued with build/commit.sh.
## Task 081ED5F4 — 2026-10-20

Tags: #matching #scene-actors #tooling

- 081ED5F4 matched as `SceneActors_UpdateMovement`: paired render animations (sequences zero and four), target movement and periodic particles. Exact fast check: 680 bytes. Register-pinning concession; depin reduced 20 hints to eight. Keeping the initialization zero through the sequence call, a full-width local sequence declaration, and the flags operand in r1 closed the match.
- 081ED8A0 matched as `SceneActors_UpdateDelayedMovement`: same animation setup, motion starts after tick 120; successful motion sets transition byte AF and field 284 to 16. Exact fast check: 724 bytes. Declaration order plus two scoped register temporaries closed the six-instruction residue. Depin reduced twelve hints to nine.
- 081EE518 drafted 90.5% as `SceneActors_UpdateSequenceTwoAction` (tentative phase semantics). Full-extent NONMATCHING check: 75 differing instructions, 20 ignoring registers. Five phases initialize sequence two, advance to sequence three, follow player-derived velocity, then reverse the sequence and request transition. Returning the raw random table value from the inline, and applying the mask at its caller, prevented GCC from merging the two random paths and raised the candidate from 60.6% to 90.5%. Kept guarded in src with default INCLUDE_ASM.
- 081EDB74 stopped: candidate for `SceneActors_UpdateDeparture` matches the visible body to 97.5% (ten differing instructions, two structural); full-extent draft check is only 65.2%, so not committed. Inventory includes the following raw assembly blob starting 081EDE4C through 081EE130. Investigate that separate prologue/boundary before resuming; do not treat body-only similarity as full-function acceptance. Candidate in wip/sub_081EDB74.c.
- 081EE130 stopped below draft threshold: randomized timed action; latest full reviewed 1000-byte extent scored 78.1%. Remaining pointer lifetimes, repeated arithmetic load order, and first animation's shared constant-one allocation. Candidate in wip/sub_081EE130.c.
- 081EEAD8 stopped below draft threshold: palette interpolation/flashing followed by five particle spawns per tick and eventual transition. Plain vector fields reached 70.1%; bitfields and base pins regressed to 65.9%. Candidate in wip/sub_081EEAD8.c, earlier version in build/ead8.old.c. No production C retained.

Tool caveat: `tools/depin.py` does not enable NONMATCHING. On a guarded draft its default assembly branch passes, so it removes all C hints without validating the draft. Restored 081EE518's hints immediately and reran `check.py --nonmatching --record` to confirm 90.5%; corrective driver commit preserves the validated result. Use depin on an extracted unguarded candidate for future drafts.

No full build, network operation or shared documentation edits. Existing tools/cloud_task.py modification left untouched. Exact code gain is the two matches only; draft earns no exact progress. Integration should validate the combined ROM and calculate percentage-point gain centrally.
# Task 0815788C — 2026-10-20

Tags: #matching #player #contact #register-allocation

- 0815788C matched: `Player_UpdateActionModeMovement` (tentative). Player callback using table 08610B70; action modes 0/2 run timed events, mode 1 also accumulates action and ticks mode particles. The mode-1 path requests nearby effect targets and clamps alternate terrain positions.
- 08157CFC matched: `Player_UpdateInteractionMovement` (tentative). Uses callback table 08610C0C. This supplied the common implementation for 0815788C and 08158848. Five qualifying updates probe 50 units behind facing; a floor at least 200 units above the current Y and no cell flag 4 starts action 4. The eligibility flag lives at player +0x284, not +0x464.
- 08158320 matched: `Player_UpdateInteractionHoverDisplay` (tentative). Clamps interaction velocity, refreshes positional effects, and updates hover height toward a random byte target every 16 ticks. Current hover height is +0x466, target +0x467, timer +0x468; collider Y includes terrain recovery height. Register-pinned concession; depin removed two of six initial hints, and the retained four hints passed exact check after formatting.
- 08158848 matched: `Player_UpdateLinkModeMovement` (tentative). Installed by Player_InitActionMode under global flag 0x1000; uses table 08610D88. Shares terrain/action-four logic with 08157CFC, omits periodic scene-feature events, and has distinct kind-two and kind-four handling.
- 0815E1F0 drafted 85.6%: `PlayerContact_ApplyResponse`. Applies contact power and reaction flags, scales the applied cost by player status/mode, and raises pending damage to the larger response. Full guarded draft recorded by check.py. Remaining differences include flag/load ordering, shift/damage register allocation, repeated special-state checks, and timed-reset call setup (ROM retains source power/duration in r2/r3 and reads global flags). No invented hidden-argument contract was added. The movement animation byte must be read signed despite the shared header's u8 field.
- 0815E8A4 drafted 89.4%: `PlayerContact_ApplyTerrainDamage` (tentative). Selects hazardous-terrain reactions, accumulates terrain/status damage, displays and subtracts damage, clears the pending vector, and consumes inventory item 0x26 to restore health. Remaining differences include early reaction call merging, boolean materialization, address lifetimes and register allocation. The commit subject used the pre-format 88.5% score; the recorded full-extent score is 89.4%.

Useful technique: typed SceneVec/bitfield union probes reproduce packed halfword updates and the inlined terrain tests. A local mask before the status load fixes ordering. Initializing special-state results after loading `active` fixes another load-order difference. The unsigned nonzero expression `(-value | value) >> 31` preserves the original rsbs/orrs/lsrs idiom; operand order selects the original registers. Inline helpers around the random result and hover-target subtraction preserve arithmetic shift and store ordering.

Validation: only tools/check.py per-file checks; no make, full build, network, push or PR. Exact sources were formatted with the pinned clang-format. Four exact functions out of six assigned (66.7% of assigned functions); drafts receive no exact credit. Shared docs/index and the pre-existing tools/cloud_task.py modification were left untouched, per cloud-worker instructions.

Score recording caveat: check.py --record keys scores by the compiled sub_ alias, but the already-named 0815E1F0 fallback uses PlayerContact_ApplyResponse. Recorded with the address token temporarily, then restored the correct named INCLUDE_ASM fallback. No generated assembly was edited.

## Task 081A0BC8 — 2026-10-20

Tags: #matching #hresvelgr #scene-commands #register-allocation

Exact progress: 0.0 percentage-point gain. Two guarded drafts, no exact matches.
The supplied branch was cloud/081A0BC8; the actual checkout reports
cloud/0815788C (with the task-claim commit in its history). No branch changes
were made; driver commits are on that existing branch. The pre-existing
modification to tools/cloud_task.py was left untouched.

- 081A14FC stopped unmatched after full assembly and relative review, following
  BIG_FUNCTIONS guidance. Five-state sequence tentatively handles defeat:
  starts animation 15, waits for frame >8 and the player action gate, runs the
  first event script, then animation 20; later falls to terrain, emits bursts,
  sets event flags 0/3/0x4E and runs the second script. State 1 derives movement
  in 30-tick sections, adjusts animation rate and emits randomized motion
  entries. No useful large relative: best covers only 7%. No qualifying C
  candidate; repeated terrain, RNG and stack-vector expansion remain work.
- 081A1EFC stopped unmatched after full condensed assembly and relative review.
  Tentatively applies received/environmental damage: consumes pending bytes
  +328/+329 and halfword +326, chooses the active contact pair at +31C/+320
  or +31E/+322, clears all four, and considers contact bit 0x40. Environmental
  damage is five times the signed global player field +876 and has a 60-tick
  cooldown at +31A. Subtracts actor health +94, displays damage, requests
  reactions through callback +100, and emits spark/motion effects. Packed
  stack vectors and many reaction branches prevented a useful close draft;
  relatives cover only short pieces (best 11%).
- 081A0BC8 stopped unmatched, scratch candidate 62.2% (166 realigned differing
  instructions, 98 ignoring registers). Tentative sweep attack: waits past
  128 ticks, positions beside target, moves toward arena edge, periodically
  spawns projectiles at randomized nearby terrain cells. Uses action state
  +2F0, facing +A8 and mirror +32B; stops beyond coordinate 5120. Shared terrain
  code from Actor_UpdateMovementAnimationAndEffects helped recover semantics,
  but RNG lifetime, control shape and actor offsets still differ. Scratch:
  wip/sub_081A0BC8.c; below retention threshold.
- 081A2430 stopped unmatched, valid scratch candidate 61.9% before allocation
  experiments. Hresvelgr scene-command dispatcher, based on the receiver at
  +118 and shared SceneEventReceiver layout. Kinds 0/1/19/26 request actions
  28/29/30/31; 26 also sets mirror +32B. Kind 9 configures an effect, 5 changes
  heading and restarts animation, 2 updates body/display positions. Inline
  function arguments hoisted action/callback constants before stores; a macro
  improved ordering. Correct prepared flag is +B1; transition-pending is +B0.
  Register pins worsened the candidate. Scratch wip/sub_081A2430.c only.
- 081A26D8 drafted 85.3% as Hresvelgr_UpdateMovementAndTargets. Runs command
  dispatch, body motion and animation; copies body position to display +1B4;
  applies a decaying sine/cosine shake from +310; updates target metrics and
  per-target angles/squared distances from pointers +110. Typed views fixed
  load/store order and avoided raw-pointer loop strength reduction. A pinned
  r9 actor alias plus empty identity barrier improved the draft. Remaining:
  shake pointer/angle registers, literal-load order, target-loop shape and
  pointer copies (32 realigned differences, 17 ignoring registers). Metrics
  use an overlapping union to avoid a one-element distance array indexed by
  the runtime count. Guarded fallback uses original assembly. Final full-extent
  recorded score differs from commit subject's initial 85.1%.
- 081A12DC drafted 81.7% as Hresvelgr_FireProjectileAndDescend. Faces target,
  starts animation 25, raises projectile origin by 512, launches with a
  four-unit planar vector and vertical speed 48, then descends toward Y=832
  in steps of four after the animation event. Inline vector construction and
  position-copy/raise helpers fixed stack halfword access patterns. Pinned
  state pointer r8 improves allocation. Remaining: heading-mask operand ties,
  actor aliases, vector pointer and counter registers (58 realigned differences,
  17 ignoring registers). Guarded fallback uses original assembly.

Validation: only check.py per-file comparisons, including --nonmatching --record
on both formatted retained sources. depin.py was invoked on both candidates;
its exact-match prerequisite means it skipped these nonmatching drafts. No
make/full build, network, push, PR, generated assembly or ROM changes. Ghidra
export gen/ghidra/decomp.c is absent. Only qualifying drafts and these notes
are committed; lower-scoring scratch sources stay outside production src/.

## Task 08141C40 — 2026-10-20

Tags: #matching #player #movement #resource

- 08141C40: drafted 99.9%, `Player_EmitMovementEffects`. One differing byte: the initial null-player branch targets 08141C90 rather than ROM 08141CFC. Remaining instructions are identical. Pinned the position pointer to r4; pinning the player pointer instead made code worse. Boolean global-flag accessor must return u8 and take its mask through an inline to preserve explicit boolean materialization and constant ordering. Called by normal movement and the timed resource dash.
- 08141FA0: matched, tentative `Player_UpdateResourceDash`. State entry spends shared resource or accepts actionFlags bit 0x20, derives capped movement speed and inverse duration, then updates directional movement. Tiny mask/store inlines recover load ordering and address/value evaluation order without pinning.
- 08142194: existing draft improved from 91.6% to 92.9%, tentative `Player_UpdateObjectPush`. Direction-three effect-position adjustment needs explicit `a.y += 0` and `a.z += 0`: packed-vector bitfields retain the ROM's word writes even though values do not change. Remaining realigned differences: 84 instructions, 33 ignoring registers. Extra high register, status-test ordering, terrain flag narrowing, and stack-vector register choices remain. Ghidra export unavailable locally.
- 08142AA0: matched, tentative `Player_UpdateObjectPushRecovery`. Three phases set diagonal retreat velocity, retry neighboring-object pushing, and return to the parent action. No register pinning.
- 08142CC0: matched, `Player_UpdateFlaggedResourceRecovery`. Five phases animate entry, sustain recovery, pause while ineligible, and exit when input is released. Calls flagged resource eligibility and near-particle emission. A pinned zero in r5 must be assigned after materializing the duration-field pointer and before writing 30.
- 08142FC4: matched, `Player_UpdateUnflaggedResourceRecovery`. Sibling uses unflagged eligibility and far particles, plus a triangular pulse duration across a 0..64 timer. Cache the pulse counter once and pass its value to an inline that computes the destination internally. Pinned zero in r5 at the timer increment/store boundary.

All acceptance checks used tools/check.py only; no full build or network operations. Register-pinning concessions are marked in affected units. Each match/draft is submitted separately through build/commit.sh. Drafts remain behind NONMATCHING with assembly defaults.

## Task 080D0FE8 (2026-10-20, retained-candidate follow-up)

Tags: #matching #mummy #scene-actor #register-pinning

Exact progress: 0 percentage-point gain. Both production candidates existed on entry; this pass improves one and repairs their local aliases, not new exact coverage.

- 080D2584: inspected largest first with asmat, relatives and fresh condensed listing (build/codex-low-mummy-motion.txt). Best relative still covers only 30%; four repeated terrain-height regions and paired route probes do not provide a close complete port. Prior far-branch-tail warning remains applicable. Stopped unmatched without a new candidate.
- 080D0FE8: revisited entry and relatives alongside prior Task 080D0C30 notes. Route-facing octant selection and six animation-selection copies dominate; best relative covers 35%. No new reliable complete candidate; stopped unmatched.
- 080D3878: retained existing Mummy_UpdateDefeat draft at recorded 86.4%, 86 realigned differences / 34 ignoring registers. Added the required file-local address alias and explicit address INCLUDE_ASM fallback, allowing checks against the local ELF. Remaining control-address materialization, collider lifetime and spills are unchanged; this is verification/alias repair only.
- 080D2194: re-read entry and prior failed reconstruction. flags2D6 bit 16 and animation completion gate three restoration paths; ordinary terrain/route path needs original field-pointer lifetimes. Prior 60.8% candidate was below threshold. No new useful hypothesis; stopped unmatched.
- 080D1784: re-read entry/reset and prior failed reconstruction. Clears control timers and selects animation 7; freezes mode/frame through tick 59. Prior 61.4% candidate needs reset ordering and PRE lifetimes reconstructed. No new candidate; stopped unmatched.
- 080D1CD0: improved existing Mummy_UpdateHiddenRecovery guarded draft 94.2% to 94.3%; realigned differences 22/14 structural to 15/4 structural. A u8-returning take helper materializes the consumed flag but initially swapped actor/flag allocation (76.9%). Pinning queued to r5 and passing its known-zero value into the animation inline through an empty operand barrier retains the mirror-zero lifetime and improves the whole candidate. Removing the queued pin regresses to 74.7%; direct barrier after manual take regresses to 75.9%; actor+queued pins regress to 78.4%. Remaining entry offset/load/store allocation and a few inline temporaries differ. Added required local address alias and explicit address fallback. Candidate remains 464 bytes versus reviewed target extent 464 bytes, but is not byte-identical C.

Formatted both sources directly with the pinned clang-format. Recorded scores with check.py --nonmatching --record and checked both default assembly fallbacks exactly. Separate draft updates queued through build/commit.sh. No full build, make, network operation or shared-doc edits. Existing register concessions remain confined to guarded drafts; integration must perform combined validation.

## Task 080A9DB0 (2026-10-20, retained-candidate follow-up)

Tags: #matching #tentacle #scene-actor #reaction

Exact progress: 0 percentage-point gain. Two existing guarded drafts retained;
080AB7BC improved from 93.1% to 97.1%. No new exact matches.

- 080ABDCC: reviewed largest first, read BIG_FUNCTIONS and queried relatives.
  Prior initializer analysis remains applicable: 0x5C/0x80 drawable allocations,
  D636/C102 resource setup, script parsing, terrain and packed animation code.
  Best relative covers only 9%; no close complete port. Stopped unmatched.
- 080AD75C: read complete target assembly and matched reaction/control relatives.
  Tentative generic stagger-motion recovery, rather than tentacle ownership:
  entry selects directional animation 3, flag 8 gates sounds 459/298 and periodic
  sparks (every 12 frames). Quarter differences between +310/+318 and +314/+31C
  are clamped to +/-200 and added to motion. Once settled and +2B3 is set,
  clears flags 0x120, checks actor flag 0x80000, body +6D4 mask 0x1D, and routes
  toward player+24 or restores control. Recovery timers are 64/32. Relatives
  SceneActorReaction_PlayStaggerBurst and SceneActorControl_ConvergeSavedMotion
  each explain about 25%; no complete candidate. Stopped unmatched.
- 080A9DB0: reread complete assembly and prior 52.4% failed reconstruction.
  Two loops interpolate segment targets and bridge inactive runs, ending in
  TentacleActor_UpdateSegmentHeadings. Stack-vector lifetimes and loop allocation
  remain unresolved; no new reliable hypothesis. Stopped unmatched.
- 080AD530: reread complete assembly and 58.5% prior attempt. Shared control
  resets, route success/failure layout and offset lifetimes remain unresolved.
  Stopped unmatched without retaining below-threshold C.
- 080AB7BC: improved guarded TentacleActor_UpdateSegmentColliders draft to 97.1%,
  five realigned differing instructions, all structural. Make the mask-taking
  accessor accept the actor object, deriving its field inside, and pass the
  existing byte-pointer actor alias cast to the view (passing pp instead adds
  a spill). An explicit u32 mask for the collider clear now emits bics using
  the same mask. Remaining: derived 2D0 offset and early stack-vector address
  scheduling. Explicit vector-pointer barrier reduced structural count to three
  but swapped the first delta registers, lowering fuzzy to 96.2%; offset pins
  and barriers regressed. Kept the cleaner unpinned 97.1% version.
- 080AB968: retained guarded TentacleActor_UpdateRetraction at 85.9%, 29 realigned
  differences / 12 ignoring registers. Empty argument constraints in Advance
  did not preserve the unused abs calculations; reverted. Existing reset pins
  remain. Added required file-local address aliases to both drafts and explicit
  address-based INCLUDE_ASM fallbacks; checks now resolve against local ELF.

Formatted with build/venv/bin/clang-format; recorded both full-extent draft
scores with check.py --nonmatching --record, and verified both default assembly
fallbacks. Default fallback MATCH output does not mean either C draft matched.
Only per-file checks; no make, full build, network or shared-doc changes. Ghidra
export is absent. Separate draft updates and these notes submitted through the
commit driver. Earlier notes under Task 080A9BD8 remain useful context.

## Task 0819DD44 — 2026-10-20

Tags: #matching #hresvelgr #behavior #contact-damage

Exact coverage of the six assigned reviewed body/pool extents: **19.03%
(+19.03 percentage points; 1,224 of 6,432 bytes)**. Both new exact matches
are unpinned. Guarded drafts add no exact coverage. Repository-wide totals
remain for batch integration. Worked solo and offline on the supplied
cloud/0819DD44 branch; no make, full build, network or direct Git mutations.

- **0819DD44 matched**, `Hresvelgr_UpdateAttackCycle`, 740 bytes. Fifteen-state
  behavior callback requests attacks through byte AA, waits on completion AF,
  counts repeated attacks in 02000220 and eventually sets readiness byte 383.
  Flags 04000000/20000000/40000000/80000000 select interruptions and branches.
  State 2 checks the interruption flag only while waiting, not after its entry
  request or completed-action transition. Inline request setters preserve the
  constant-before-address ordering. Splitting byte A1 bit updates into named
  temporaries recovered operand ties. Temporary pins closed the last four
  instructions; depin removed all five hints and the formatted source still
  passed the complete 740-byte exact check.
- **0819E408 matched**, `Hresvelgr_SelectNextBehavior`, 484 bytes. Consumes
  readiness 383, advances index 32E within behavior tables 08611D44/5C/7C/8C,
  selects phase 32C based on health below half or contact 318, resets behavior
  timer C4 and requests the selected behavior through AE/B0. Accumulation at
  324 resets when index is zero. A shared next-index local with one store
  after the range branch was essential; writing stores in both arms changes
  cross-jumping. Keeping both object pointers typed produced 97.1%, whereas
  byte-pointer casts reached only 79.9%. A scoped phase-zero next-index local
  recovered the final allocation; depin removed its two hints, retaining an
  exact unpinned match after formatting.
- **0819FE9C retained draft 86.1%**, `Hresvelgr_UpdatePlayerRelease`, 1,564
  reviewed bytes. Confirmed the existing guarded draft's score: 123 realigned
  differing instructions / 35 ignoring registers. No improvement claimed.
  Added the required local alias and adjacent confidence comment; changed its
  assembly alternative to the address symbol so the stale baseline ELF can
  check it. Default assembly alternative passes exactly.
- **081A0BC8 improved draft 93.9% -> 96.1%**,
  `Hresvelgr_UpdateBoundaryBarrage`, 732 reviewed bytes. Passing the live state
  value as animation duration argument, then consuming it with `state &= tick`,
  recovers the shared constant-one lifetime and eliminates all structural
  differences. Final residue: 14 register-only instructions / 15 bytes,
  principally RNG mask lifetime (ip versus sl), signed remainder temporaries,
  and terrain-height constant allocation. Explicit mask/scatter helper pins
  regressed and were discarded. Retains its register-pinning concession.
  Recorded the score, required local alias and confidence comment; default
  assembly alternative passes exactly.
- **081A14FC stopped unmatched** after largest-first assembly, condensed-run
  and relative review. Five-state defeat/event sequence: animation 15 and
  player-action gate; eight 30-tick movement stages toward 0B54; animation
  21/22 landing and particle bursts; event flags 0/3/4E and second script.
  The condensed listing exposes seven copies of the four contact-field
  address precomputations, not just duplicated RNG/terrain helpers. The best
  matched relative covers only 7%; no new useful hypothesis for recovering
  that repeated switch-edge address setup, so no new source was promoted.
- **081A1EFC stopped below draft threshold**, tentative
  `Hresvelgr_ApplyContactDamage`. New complete scratch reconstruction in
  ignored `wip/codex-low-0819DD44/sub_081A1EFC.c`; initial reviewed check was
  49.1% (369 realigned differences / 208 structural), subsequent pointer pins
  regressed. Correct body/pool extent is 1,148 bytes through 081A2378;
  the first exploratory check used 1,140 and should not be cited as acceptance.
  Consumes pending spark/reaction/motion at 328/329/326, selects contact pair
  31C/320 or 31E/322, applies environmental damage of five times the signed
  player field 876 with a 60-tick cooldown, subtracts health, then selects
  flags/reaction callback or emits effects. Packed bitfield vector operations,
  shared reaction tails and pointer lifetimes remain structurally different.
  No below-threshold candidate was put in production source.

Validation: per-file tools/check.py only, with reviewed extents for exact
matches and drafts. Both draft scores recorded with --nonmatching --record;
formatted with the pinned clang-format. All production changes queued as one
function per driver commit. Shared docs/index were left alone per CLOUD.md.
Combined ROM verification remains with the integrator.

## Task 08209AC4 revisit (2026-10-20)

Tags: #matching #fan-cannon #screamer

- 0820C120: matched unpinned as Screamer_InitColliders (532 bytes).
  Creates directional attack collider 6C (2001), body collider C0 (5009),
  and directional secondary body collider 114 (4009). Attack properties use
  signed fields 1C4/1C6/1C8; body properties use unsigned 1E8/1EA.
  Ooze_InitAttackCollider supplied the packed signed 16-bit vector bitfields
  required for the first setup; subsequent assignments use halfword views.
  A full-width unsigned 16-bit flags bitfield reproduced the final two OR
  operations exactly, eliminating four register-only differences without pins.
- 0820BF70: improved guarded Screamer_ApplyHitReaction draft 94.3% -> 95.2%.
  Declaration order source/hit/target fixes argument-copy ordering; passing
  &frame.position rather than the pinned position pointer fixes stack address
  materialization. Remaining 11 realigned instructions / 2 structural concern
  RNG index/mask/sample allocation. Recorded with --nonmatching --record.
- 0820C4D4: retained guarded Screamer_UpdateScream draft 94.9%, 34 realigned
  differing instructions / 7 ignoring registers. Added required address alias
  and callback alias so the stale-symbol baseline ELF can check the draft.
  Animation constant copies and switch-case field-address lifetimes remain.
- 0820C9AC: retained guarded Screamer_UpdateDamageParticles draft 81.3%,
  93 realigned differences / 60 ignoring registers. Added required alias.
  Pause branch shape, particle stack frame, RNG lifetimes and callback return
  remain unresolved. Recorded both retained draft scores again.
- 08209AC4: stopped unmatched after largest-first asmat/condense review.
  Condense exposes only short timer-check and effect-call runs, insufficient
  to port the mixed activation, contact, fan-force and animation update.
  The earlier reconstruction notes above remain applicable; no new candidate.
- 0820A5CC: stopped unmatched after asmat/relatives review. Best matched
  relative explains 19%, chiefly animation playback; script option setup,
  terrain/collider and manager slot layout need full reconstruction. No new
  source promoted. Ghidra export is absent in this checkout.

Validation used tools/check.py only. Exact initializer acceptance printed zero
bytes differing. All three guarded assembly alternatives also pass exactly;
this is fallback validation, not C matching credit. One driver commit per
match/draft; no full build or network operations. Combined ROM validation is
left to integration.

## Task 08218508 revisit (2026-10-20)

Tags: #matching #sprites #terrain

- 0821C8F8: stopped unmatched after largest-first entry assembly, fresh full
  condensed listing and relative review. The best current relative covers 7%,
  chiefly terrain lookup fragments. Direction-specific paired probes and ten
  separate stack position temporaries remain the obstacle; no new viable C.
- 0821C254: stopped unmatched after entry assembly and relative review. Best
  relative covers 5%; the eight adjacent-cell branches and duplicated orthogonal
  probes still require reconstruction. Earlier full-assembly notes apply.
- 0821D178: stopped below draft threshold. Rechecked retained segment-clipping
  scratch candidate: approximately 62% fuzzy, with packed-position stores and
  intersection/control-flow differences unresolved. No production draft.
- 08218508: improved guarded SpriteOam_QueueFlaggedProjectedSprites draft from
  89.1% to 90.9%. Added required local address alias and address-based assembly
  alternative for the stale baseline ELF. Full reviewed extent is 788 bytes.
- 0821881C: new guarded SpriteOam_QueueOverlayProjectedSprites draft, 90.9%.
  Tentative name: the observable distinction from 08218508 is selection flag
  0x2000 rather than 0x1000. Porting the existing sibling draft is much stronger
  than the previous independent scratch candidate. Both routines stage the same
  projected/absolute sprite positions, affine inputs and depth-sorted OAM lists.
  Both now have 33 realigned differences, 14 ignoring registers. Computing the
  culling mask before offset loads and passing the whole drawable to projection
  improve structure. Returning the unsigned scale byte through an s32 accessor
  recovers signed branch instructions. Destination scope, explicit affine-count
  increment and signed affine-index trials did not improve the result. Remaining
  differences include projection pointer hoisting/spill, affine result copies,
  priority load order and register lifetimes. Register-pinning concessions retained.
- 0821BF5C: stopped below draft threshold after full assembly and retained scratch
  candidate review. Reviewed 760-byte extent scores 73.7%, 175 realigned / 134
  ignoring registers; this is not directly comparable to the earlier
  candidate-length score. Override lookup branch merging and slope-transition
  tails remain structurally different. No production draft.

Validation: formatted both production drafts with build/venv/bin/clang-format;
recorded both scores using check.py --nonmatching --record (full 788-byte extents).
Both default assembly alternatives pass exactly; this earns no C matching credit.
One driver commit queued per draft. No full build or network operations. Shared
index/history left unchanged per CLOUD.md; combined build belongs to integration.

## 2026-10-20 — task 081DA810

Tags: #matching #projectiles #menus #scene-actors

- **081DA810 — matched, TimedEffect_SpawnProjectile.** Reads the projectile
  type's update callback, allocates a TimedEffect entry, initializes velocity,
  collision, graphics and category counters. Type 9 installs a manager-owned
  collision callback with special bounds. Config offsets follow the neighbouring
  motion-pool layout but add type/collision options. A separate saved collider
  pointer recovered the original lifetime. Temporaries for the collision flag,
  signed heading and priority value fixed the remaining load/copy order.
  `depin.py --write` removed all five hints; the final C has no concessions.
  Exact check: full 752-byte body.
- **081DB474 — matched, Options_UpdateScreen.** Handles three menu selections,
  a nine-value sub-selection, highlight rectangles, fade-out and script results.
  An explicit empty case 2 reproduces the switch tree. The text rectangle has
  a packed first word and two halfwords; expressing that first word as a field
  rather than a cast recovered the original allocation. One empty input barrier
  retains the early mask load; marked register-pinning concession. Depinning
  removed the explicit r2 pin. Exact check: full 744-byte body.
- **081DB794 — drafted 82.4%, Options_InitScreen.** Initializes the option-screen
  backgrounds, five animations, palette scratch, labels and initial selection.
  Separate background/palette resource locals avoid unnecessarily extending the
  sprite-resource lifetime. Constants use temporary register pins. A preliminary
  83.4% checkpoint was superseded: the final draft removes a fixed input-pointer
  pin that overlapped another pointer's allocation. Remaining 53 realigned
  differences, 22 ignoring registers, include sprite-pointer lifetimes, initial
  argument order and a redundant palette-source reload. Default retains asm.
- **081EAB64 — drafted 91.0%, Params3D0_UpdateDisplay**, up from 90.7%.
  Multiplying the light-phase index before advancing the seasonal song table
  recovers the original shift order. Remaining 125 realigned differences,
  31 ignoring registers: palette pointers, selection-animation temporaries and
  address lifetimes. Default retains asm; existing descriptive name retained.
- **081EE130 — drafted 90.4%, SceneActors_UpdatePeriodicEffectAction**, up from
  89.2%. Reversing the timing-product operands matches load order. Remaining
  47 realigned differences, 25 ignoring registers: initialization, random-mask
  placement and packed sequence-step allocation. Removing existing pins/barriers
  worsened the candidate and was reverted. Default retains asm.
- **081EDB74 — retained draft 85.3%, SceneActors_UpdateLeftDeparture.** Two
  animation sequences depart to the left before the task switches to destruction.
  Added the required file-local alias and checker mappings for renamed callees.
  Removing hints, changing the second sequence's command-pointer derivation and
  constraining the initial input did not improve the retained candidate.
  Remaining 58 realigned differences, 22 ignoring registers: frame saves,
  second-sprite addressing and initialization constant/order choices.

Validation used only per-function check.py; all draft scores were recorded with
--nonmatching --record. Two exact matches and four guarded drafts retained.
No full build, network operation or shared documentation/index change. Commits
queued through build/commit.sh; combined ROM verification belongs to integration.

## Task 0818911C — 2026-10-20

Tags: #matching #armor #worker

- **0818AFEC — matched, Armor_UpdateContactBoost.** Increments contact counters,
  moves through modes 0/1/3, temporarily scales the saved attack and defense to
  125%, and applies 0x200004 to the two attack colliders. The contact-power field
  is at 0x274. Naming is tentative because the exact gameplay meaning of the
  contact counters is not confirmed. Started from Armor_UpdateAccumulatedBehavior's
  observed layout and scaling idiom. An inline scaling helper left eight register
  differences; writing the two signed scaling blocks at their use sites matched
  exactly. No register pins. Formatted and checked twice before driver commit.
- **0818B2A0 — drafted 99.5%, Armor_UpdateBehaviorAfterContact**, improved from
  the inherited 97.8% draft. Clear-flags inline calls recovered the intended mask
  and collider registers. Explicit three-argument prototypes and forwarding the
  incoming flags to 0818AFEC and Armor_UpdateAccumulatedBehavior removed another
  register mismatch and made the call contracts defined. Three realigned
  differences remain (two ignoring registers): zero materialization precedes the
  mode/timer stores instead of following them in the modes 2/3 forced-transition
  path. Declaration swaps and a signed full-width bitfield store did not help;
  retained the simpler ordinary store. Required alias added; score recorded.
- **0818911C — stopped below threshold, Armor_UpdateAggressiveDecision.**
  Candidate remains locally in wip/sub_0818911C.c. Recovered timer expiry,
  boost removal, target Manhattan distance, facing quantization, and both flag-1
  decision switches. First draft 61.8%; separating coordinate differences from
  rounding reached 71.4% (242 realigned / 73 structural). Variant trials lowered
  structural differences to 50 and total to 173, but fuzzy fell to 69.9%.
  Remaining issues include cached target coordinates versus reloads, stack
  lifetimes and shared tail placement. Tried generic global pointer, narrow flag,
  signed flag, inline direction conversion, coordinate widths/declaration order,
  and separated rounding. No source committed below threshold.
- **081896D0 — drafted 81.5%, Armor_UpdateFacingDecision.** Ported the recovered
  decision routine after reading its full assembly. Unlike 0818911C, mode 1 first
  checks quantized facing, then retreats with behavior 9 when distance exceeds 5;
  its special type is 15 rather than 14. Explicit rounding steps produce the
  retained draft. Final recorded full-function check: 158 realigned differences,
  48 ignoring registers. Target-coordinate lifetimes/spills, branch tails and
  register allocation remain. The alternate local wip candidate has fewer
  realigned differences (152 / 44) but scores 77.4% fuzzy. No register pins.
- **0818B7AC — stopped after assembly/relative analysis; no qualifying draft.**
  Collision callback takes attacking collider, contacted collider and owner.
  Owner flags 0x80 and the signed 0xB2C cooldown reject contacts; collider +0x44
  supplies the new cooldown. It tests owner flag 2 to choose defense-health
  handling at 0xB20 versus regular reactions, temporarily changes contacted
  collider categories/properties by armor type, applies health changes at 0xB1C,
  forwards original damage plus attacker properties to 0818B2A0, updates the HUD,
  shows damage numbers and chooses impact sounds. Death sets flag 0x80; the
  reaction script uses 0xB50 and flag 0x100. Best matched relative covers only
  19/543 instructions. No sufficiently concrete compiler-shape hypothesis for
  a worthwhile candidate after this analysis; left assembly unchanged.
- **0818BE30 — stopped below threshold, Armor_InitCombatColliders.** Local
  candidate wip/sub_0818BE30.c initializes colliders at 0x190/0x1E4/0x238/0x28C,
  with contact callbacks 0818B7AC and Armor_TriggerBehavior35, quarter/half attack
  strength, and category masks selected by type. First candidate 50.9%; replacing
  inaccurate u16 callee argument declarations with u32 removes extra narrowing.
  Main unresolved shape is overmerged switch tails and pointer allocation.
  Named fields, scoped collider locals and diagnostic register pins failed to
  improve it sufficiently; all pins removed. Nothing below threshold committed.

Validation: only tools/check.py per file; draft scores recorded with
--nonmatching --record. One matching commit and two draft commits queued via
build/commit.sh, with no make, full build or network operation. Shared docs and
index were not edited. Integration must run the combined ROM verification.

## Task 081F6B90 (2026-10-20)

Tags: #matching #ratatosk

Exact code gain: 0.0 percentage points. Three guarded drafts retained; no exact C matches.

- 081FAE0C: inspected largest first with asmat, relatives and condense. Ratatosk_Create calls this initializer after allocating 0xF30 bytes and installing update/destruction callbacks. Registers actor slot 4, parses script keywords, initializes collider/animation and extensive attack parameters, then creates projectile, clone, sun-orb and particle managers. Allocation failures return -1. Best relative covers only 3%; condense leaves 758 unfolded instructions. No complete candidate produced; stopped at reconnaissance rather than retain an incomplete initializer. Ghidra export is absent.
- 081F6B90: inspected with asmat, relatives and condensed listing. SceneMessages command dispatcher for the Ratatosk actor; starts from actor+0x118, consumes pending triggers and dispatches command byte +6 through a 51-entry table. Most comparable matched relative (ScenePanel_DispatchTransitionCommands) covers 22%, largely common animation/state stores. Calls 081F6748, 081F6894 and 081F6A14 plus actor-resource and shadow routines. No close whole-function port established; left assembly.
- 081F7FD0: inspected with asmat and condense. Four-state attack selects animation 9, clears sequence/repeat/wait fields, sets duration 64 and starts song 0x558. Body combines player-relative midpoint calculations, interpolated player movement, random position changes, particle spawning and sound lifecycle; condense folds only three RNG runs. No matched relative output and no complete candidate produced; left assembly.
- 081F7DAC: drafted 88.6% as Ratatosk_UpdateCloneAttack (tentative attack label). Consumes activation, selects opening animation according to signed field E96, then clears sequence/repeat count. Cycles animation 4 and random recorded positions; after six repetitions chooses arena coordinates and animation 8, requests clone spawning while E6E is positive, then toggles player flag 740 according to active clone count. Retained unpinned draft has 35 realigned differing instructions, 22 ignoring registers; 532 emitted bytes versus 548-byte original extent. Differences concentrate in RNG constant/address scheduling, coordinate-store register lifetimes and associated pools/frame. Experimental r4 pins improved candidate-length similarity but were removed from the retained draft. Scoped animation constants and explicit integer-address table lookup improved selection.
- 081F8600: drafted 96.3% as Ratatosk_UpdateSunOrbPlacement (tentative). Consumes activation; if no orb manager status, chooses far player offset, resets counters and randomly chooses target source. Otherwise starts directly in orb animation state. Avoids arena-exterior positions, chooses a player-near point, initializes 0x20-byte orb config, requests a raised orb and marks point/orb slots active. Waits up to 600 frames and fades manager orbs. Remaining 16 realigned differing instructions, 10 ignoring registers, involve RNG assignment address scheduling and point-array address materialization. Scoped state constant fixes state-byte store ordering. Typed destination/sequence/orb slots are useful; changing F0E to a direct field unexpectedly added a literal and changed register allocation, so that experiment was reverted.
- 081F8988: drafted 97.7% as Ratatosk_UpdateSunOrbRelocation (tentative). Selects current sun orb, waits for slot state, raises a replacement orb at the far player offset, retires the prior slot, relocates actor to replacement position, then moves toward the near player offset. Failure paths finish immediately. Candidate matches original 740-byte extent and differs in eight instructions/eight bytes, zero differences ignoring registers: r0/r2 choices in sequence=2 and sequence=3 stores, plus the subsequent wait-counter address. Replacing raw sequence, unsigned selected-orb and orb-slot stores with actual fields removed all structural differences. Pointer locals, scoped constants and register pins did not close the final residue; pinning values moved instructions instead. Retained source is unpinned.

Formatted the three files directly with the pinned clang-format executable (no make). Recorded full-function draft scores with check.py --nonmatching --record and checked each default assembly fallback byte-identical. Queued one driver commit per draft. No full build, network operation or generated assembly commit. Batch integration validation remains with the integrator.

## Task 08131044 (2026-10-20)

Tags: #matching #scene-actor #reaction

- 08135F70: improved existing SceneActorReaction_ResolveDamage guarded draft from 86.8% to 88.2%. Pinning the mode-test constant `four` to r4 reduced realigned differences from 79 to 73 (18 ignoring registers). Adjacent declaration swaps did not improve it. Added address aliases for this function and its area-damage/defeat callees because the local ELF still uses sub-address symbols. Remaining health/+0x2D8 pointer spills and shared flag-expansion choices remain unresolved. Retained register-pinning concession.
- 08132904: retained existing SceneActorReaction_ApplyLinkedAreaDamage draft at 87.1%, 96 realigned differences / 24 ignoring registers. Read entry gating and link/local area-gain split; added required file-local address alias and corrected the guarded assembly fallback to the address label used by this checkout. No C improvement retained.
- 08131D5C: retained existing SceneActorReaction_ApplyAreaDamage draft at 87.5%, 91 realigned differences / 25 ignoring registers. Tested pins for the collider, timer, health, max-health, position and flag pointers against observed ROM high-register allocation. No structural improvement; reverted these variants. Added required alias and address fallback. Pointer spills and RNG scheduling remain.
- 081314E8: stopped without a new draft after reading assembly and relatives. Entry clears flags2CC bit 0x80, flags2D0 bit 0x1000000 and flags2D4 bit 0x200, selects control +0x22C. When flags2D4 bit 0x1000 is set, compares squared X/Z displacement to the nav+0x88 target against 0xFFFE and selects nav+0x238 control (kind 0x19) on proximity. At tick 30 emits sub_0807C72C; after tick 60 starts behavior 7 and tries the saved route. Dog_ChasePlayer covers 51%, mostly control resets; the target-specific route lifecycle needs reconstruction.
- 08131044: exploratory SceneActorReaction_AdvanceRouteMotion candidate reached 68.9%, 136 realigned differences / 58 ignoring registers; discarded below threshold. Selects directional entry 1, gets speed from +0x260 callback, consumes nav probe event and either requests turn (behavior 6) or accelerates. On arrival clears nav+0x54, selects behavior 2/5 from nav+0x200 descriptor +6, and restores +0x22C control. Shared reset/control helper expansion and event-result lifetimes still differ.
- 081312F4: exploratory SceneActorReaction_ResumeAfterMotion candidate reached 76.8%, 76 realigned differences / 22 ignoring registers; discarded below threshold. Entry selects +0x224 control kind 2. Completed motion with flag 0x10 chooses behavior 8 and nav+0x214 control kind 0x13; otherwise flags2CC bit 0x10 chooses behavior 7/2 and +0x228 control. Tried offset-based state resets and kind/callback register pins; none improved the baseline. Helpers change constant derivation and require an extra saved register.

All retained drafts checked with `tools/check.py --nonmatching --record`; 08135F70's named body required updating its fuzzy comment explicitly because record did not rewrite it. Default guarded assembly checks are byte-identical, which validates fallback preservation only and does not count as C matching progress. No exact C gain. No full build or network operation performed; commits queued through build/commit.sh.

## Task 08098CA0 (2026-10-20)

Tags: #matching #ghost #lun

Exact code gain: +0.0139 percentage points of total code (336 bytes), entirely
without register pinning. One exact match and four guarded drafts; no full build
or network operations. All commits submitted through build/commit.sh.

- 08098CA0: drafted 85.6% as Ghost_FollowPlayerLoop (tentative). Ghost action
  table gUnk_08605DE8 references this and 08099130. Starts a randomized path around
  the player's position, follows nine context positions beginning at +0x568,
  accumulates motion and rebuilds the path after reaching its final point. Speed
  at context +0x566 starts at one, increases periodically to about twelve, and
  halves on path renewal. Entry flag +0x2B1 also resets the control callback.
  Remaining 107 realigned differences / 45 ignoring registers: random-number
  address scheduling, position-address lifetimes, instruction order and registers.
  Recomputing waypoint coordinates directly for Math_Atan2 improved fuzzy from
  78.5% to 85.6%. Read BIG_FUNCTIONS.md; steered by realigned counts and variants.
- 08099130: drafted 84.9% as Ghost_PursuePlayerLoop (tentative). Same initial
  path and steering as 08098CA0, but renewal uses radius 256 instead of 512,
  retains speed and increases it every sixteen ticks while below eighteen.
  Remaining 103 realigned differences / 42 ignoring registers. Writing the
  renewal Z store through actor->position instead of the cached position pointer
  changed allocation and raised the score above threshold. Reviewed full extent
  gives 84.9%, versus the earlier candidate-length 84.7% in the commit title.
- 0809B94C: drafted 81.1% as Lun_ApproachMovementTarget (tentative). Installed by
  the Lun initializer alongside its turning callback. Eases X/Z accumulated
  motion toward targets at actor +0x310/+0x314, tracks the accumulated amounts at
  +0x318/+0x31C, divides the remaining displacement by four toward zero and clamps
  each step to +/-200. Emits particles every twelve ticks with flags2D4 bit 8.
  Completion selects a special callback, renewed player pursuit, or waiting,
  according to bit 8 and context delay +0x444. Remaining 110 realigned differences
  / 32 ignoring registers. Scoped collider pointers recover base+6 flag accesses;
  signed full-width bitfields reproduce the signed clear masks and raise fuzzy
  from 79.6% to 81.1%. Extra register pins and an alternate typed movement view
  worsened results and were discarded.
- 0809B3F8: drafted 82.8% as Lun_TurnTowardSavedDirection (tentative). Lun's
  initializer installs this callback. Compares eight-way direction buckets with
  the saved direction at actor +0x3CD; chooses a turn sense, advances direction by
  32 at animation events, and restores the saved callback when facing matches.
  Reuses the animation-selection inline reconstructed for 0809B6AC. Remaining
  64 realigned differences / 24 ignoring registers include duration-scale pointer
  lifetimes, mask/address scheduling and control reset constants. Reviewed full
  extent gives 82.8%, versus candidate-length 81.7% in the commit title.
- 0809B6AC: matched as Lun_UpdateWaitingAnimation (tentative). Called through
  control transitions including RouteActor_UpdatePlayerDestinationPhase
  (0809D948). Selects profile entry eleven and directional frame, updates either
  sequence or tile animation, sets flags2D6 bit 0x20 for ticks <=29, and marks
  field2B0 complete after tick 59. Porting the matching selection body from
  SceneActorAnimation_SelectExplicitVariant (080765F8) as a value-argument inline
  avoids spilling the address-taken directional outputs: 53.5% ->97.0%. A u16
  mask parameter in the actor flag setter recovers constant-before-address order
  and resolves the final six differing instructions. Formatted source checked
  byte-identical with tools/check.py. No pin/barrier concession required.
- 0809AE0C: stopped without a qualifying draft. ActorContact_InitializeBodyAndSecondary
  (0809CBB8) installs this secondary collision callback. Requires contactFlags=1,
  rejects inactive/dead owners, compares player-relative height to 60 or 260
  within tolerance 32, changes control/state, disables both colliders, and sets
  context +0x3D4 bit 2 while applying a player effect based on actor +0x1E6.
  Best exploratory candidate stayed below 50%; natural allocation keeps actor
  in ip rather than ROM r6, and dead bit-test computations, context reloads and
  flags/states differ. Pins and value-returning test helpers did not improve it
  sufficiently. Local scratch remains in wip/sub_0809AE0C.c, not production src.

All four draft scores recorded by tools/check.py --nonmatching --record; their
normal build retains INCLUDE_ASM. Ghidra export gen/ghidra/decomp.c was absent.
Only per-file checks were run. Combined ROM validation belongs to integration.


## Task 080D453C (2026-10-20, route/volley follow-up)

Tags: #matching #mummy #navigation

Exact gain: +0.0442 percentage points of total code (1068 bytes), without
register pinning. No full build or network operations.

- 080D5778: retained guarded Mummy_ResumeDelayedRoute draft, 88.3%, 134 realigned
  differences / 39 ignoring registers. Passing RNG mask as an inline argument
  regressed to 87.2%, 147/43; reverted. Added required address/callee aliases.
- 080D6608: retained guarded Mummy_TryDirectPursuit draft, 92.5%, 103/12.
  Removing the existing r7 RNG mask pin in favor of an inline argument regressed
  to 92.1%, 111/14; reverted. Added required address/callee aliases.
- 080D6FDC: matched Mummy_RequestStraightRoute. A u32-returning Attributes inline
  around the halfword load prevents HImode narrowing of the mask expression.
  This resolves all previous 105 realigned differences, including the map-address
  allocation/spill issue. Formatted and checked exact (1068 bytes). No pins.
- 080D62CC: improved Mummy_FireAcceleratingVolley from 94.6% to 98.5%, 4 realigned
  differences / 4 ignoring registers. Explicitly load projectile shot/category
  before target pointer; use a distinct unpinned heightDifference local rather
  than sharing difference with facing correction. Remaining differences are the
  quotient-to-absolute-value register copy, resulting pool padding, and delta
  pointer scheduling around the loop mask. Pinning heightDifference, speed or
  quotient (including a quotient barrier) regressed; reverted. Existing count
  and strength pins retained; no new pins retained.
- 080D53F8: retained Mummy_UpdateProbeNavigation draft, 80.0%, 95/30. Narrowing
  saved turn argument to u8 regressed to 77.9%, 126/34; reverted. Added aliases.
- 080D453C: rechecked retained exploratory Mummy_ReactToNearbyStimulus candidate:
  60.2% candidate-length, 218/116. Below draft threshold; no production candidate.
  The previously recorded control-reset and target-position lifetime issues remain.

Draft scores recorded with check.py --nonmatching --record. Explicit sub-address
INCLUDE_ASM labels are required because descriptive aliases do not expand inside
its stringified filename. Corrected fallback labels and checked all four default
assembly paths byte-identical; these checks do not count as C matching progress.
Commits submitted through build/commit.sh; fallback fixes are separate follow-ups
because the driver consumed draft commits before those fixes were written.

## 2026-10-20 task 080AE1C4: Vorn recovery and delayed routes

Tags: #matching #vorn #navigation

Exact gain: +0.0167 percentage points of total code (404 bytes), register-pinned.
Only per-file check.py validation; no full build or network operations.

- 080AF2BC: stopped at 72.9%, 347 realigned differences / 84 ignoring registers.
  Read the complete condensed body, callers/control context and matched chase
  relatives. A candidate using Chase_Arrive, delayed command storage, route snap
  and route planning started at 65.6%, 475/167. A volatile group pointer improved
  stack allocation to 72.9%, 349/87; passing the command pointer directly to the
  delay inline reduced it to 347/84. Spilling actor or flags regressed. Below
  the 80% draft threshold; removed the candidate, retained assembly.
- 080AE1C4: retained guarded Vorn_UpdateKnockbackRecovery draft at 81.2%,
  113/36 (previously 82.1%, 118/40). Delaying maskTwo initialization until after
  directional animation and spilling group improved realigned structure while
  lowering coarse fuzzy score. Control-pointer recomputation regressed.
- 080AEE08: retained guarded Vorn_UpdateDefeat draft at 83.4%, 84/9.
  Unpinning burst locals or RNG cursor regressed. Declaration swaps had no
  effect. Added a local address alias for SceneActor_CallContextScriptB because
  the base ELF retains sub_080B0D28. Remaining issues are address derivation,
  register allocation and constant reloads.
- 080AEB74: retained guarded Vorn_TryRestorePatrolRoute draft at 80.2%, 67/32
  (previously 68/34). Removing volatile from savedMotion improved structure;
  pinning destination, spilling group and deriving control from group regressed.
  Terrain destination spills, flag-address reuse and reset scheduling remain.
- 080AE6F0: retained guarded Vorn_ResumeReactionRoute draft at 82.8%, 57/21
  (previously 83.9%, 55/23). Explicit motion flag load/OR/store improves structure;
  caching flags2D0 before testing/clearing removes two register differences.
  Explicit sl zero pin, removing r8 pin and spilling group regressed.
- 080AFBF4: matched Vorn_UpdateAnimationRetries, 404 bytes. On entry it selects
  delayed animation, clears context retry count 0x696 and elapsed count 0x694,
  and chooses duration 10..41 at 0x697. Completion repeats while retries remain,
  otherwise clears context bits 0..1 at 0x6D4 and dispatches gUnk_0824E224.
  Specializing the RNG inline to return the duration, rather than passing a
  mask argument, improved 94.0% to 99.0%. A register-pinned r2 table pointer in
  the second RNG expansion resolves the final two register differences.
  Marked MATCHING_CONCESSION: register-pinning; no instruction bodies.

All four existing drafts now have required file-local naming aliases, and their
scores were recorded with check.py --nonmatching --record. Default fallback
INCLUDE_ASM arguments must use literal sub-address names because the macro
stringifies the filename. Corrected those and checked all four fallback paths
exactly; assembly fallback checks are not C progress. Function/draft commits and
fallback follow-ups submitted separately through build/commit.sh.


## Task 0814957C — 2026-10-20

Tags: #player #matching #drafts

- 0814957C: drafted 93.7%, tentative Player_UpdateOddDirectionPose.
  Ported the nearby direction-recovery pose and shared packed-command stepper.
  Pinning the command pointer to r6 improved 88.3% to 93.7%; 16 realigned
  differences, six ignoring registers. Explicit facing-address barriers worsened
  selection; pinning the whole player to r7 also worsened code generation.
- 08149720: drafted 92.5%, tentative Player_UpdatePoseRecovery. First phase
  forces an odd facing and decays halfword 0x4EC by 30, then plays sequence 3
  until playback ends. Stepper returns whether it wrapped or held at an end.
  35 realigned differences, 14 ignoring registers; return/control layout remains.
- 08149A14: drafted 97.3%, tentative Player_UpdateRecoilMotion. Uses three
  effect flags (0x479, 0x47B, 0x489) to approach saved position 0x47C, otherwise
  recoils along facing. Shake and speed halfword 0x270 decay by 3/4.
  Explicit abs/shift/negate truncation instead of C signed division by eight
  improved 76.4% to 97.3%. All 13 realigned differences are register-only.
- 08149DF0: retained existing 99.3% Player_UpdateSpiralTrailAction draft and
  added required local alias. Exactly two instructions differ: final duration
  address 0xAD8 uses r0 instead of r3. Tried duration-pointer/offset pins,
  scalar temporary forms, clobber barriers and declaration permutations;
  none improved the original. Restored best source. Fixed fallback's literal
  INCLUDE_ASM name after the alias exposed its stringified filename.
- 0814D264: stopped below retention threshold; full exploratory C reached
  73.9% (171 realigned differences, 69 ignoring registers). Removed candidate
  from src. Action 0x54 sweeps a horizontal radius-160 arc; 0x81 sweeps a
  radius-250 vertical arc projected along stored yaw; 0x72 uses a timed radial
  reach. Configures collider at player+0xB8C. A local typed arc layout improved
  70.9% to 73.9%; swapping sum operands did not help. Address evaluation and
  table-pointer liveness remain substantial differences.
- 0814C3A4: inspected first, with BIG_FUNCTIONS guidance, relatives and condensed
  listing. No retained candidate. Apparent grab-motion state machine uses
  Player_OnGrabMotionExit, saved auxiliary sprite state, particle creation and
  trigonometric movement. Only 9% covered by closest matched relative;
  condense leaves 973 instructions unfolded (1165 lines). Needs a dedicated
  state-by-state reconstruction; no Ghidra export is present in this checkout.

All retained C remains guarded by NONMATCHING, scored with --record.
Per-file default assembly fallback checks pass after the 08149DF0 filename fix;
these checks do not count as C matches. No full build or network operations.


## Task 08153224 follow-up (2026-10-20)

Tags: #matching #player #animation #cue

Exact progress: 0.0 percentage-point gain. All six targets were existing guarded
C drafts; restored file-local aliases using their current symbols/functions.csv
names and address-form INCLUDE_ASM fallbacks. No network or full build used.
Formatted all six files and recorded full-extent scores with check.py
--nonmatching --record; each default assembly fallback checks byte-identically.

- 081543F0: improved PlayerCue_UpdateSideFade from 86.4% to drafted 91.3%.
  Typed sprite.unknown8[0/1] accesses instead of byte-pointer casts reduce
  realigned differences from 131/29 structural to 77/7. A scale setter inline
  reached 89.7%; value barriers 89.0%; specializing final playback with swapped
  state/sprite pins regressed to 85.6%. Retained typed accesses. Remaining
  differences concern scale offsets, phase tails and playback allocation.
- 08153A00: retained PlayerCue_UpdateSideWalkTarget, drafted 96.9%, 17 realigned
  differences / eight structural. Unsigned final-direction reset and unsigned
  scaling products did not change code. Volatile reset worsened to 94.8%;
  product barriers did not improve it. Direction-reset store and signed scaling
  branch layout remain; reverted experiments.
- 08153224: retained PlayerCue_UpdateFormChange, drafted 84.4%, 84 realigned
  differences / 26 structural. Explicit widened timer and opaque mask worsened
  to 82.3%; splitting effect-timer addition did not change code. Remaining
  constant lifetimes, phase layout and particle configuration. Preserved fully
  initialized particle words rather than introducing an uninitialized read.
- 081540D4: retained PlayerCue_UpdateSideArrival, drafted 98.5%, nine realigned
  differences / seven structural. Shared last-frame index store did not improve
  the instruction count; phase-mask barrier increased structural differences.
  Reverted both. Completion-store sharing and first-mask materialization remain.
- 08153610: retained PlayerCue_UpdateSpiralMotion, drafted 89.9%, 42 realigned
  differences / 15 structural. Widened timer plus mask reduced structural count
  to 13 but raised total to 56 and lowered fuzzy to 88.3%; rejected. Effect
  timer temporary alone made no change. Constants and field-address lifetimes
  remain.
- 08154AF0: improved PlayerCue_UpdateSideRecoil from 97.5% to drafted 98.7%,
  three realigned differences / zero structural (three bytes). Pin the direction
  offset to r5 and its derived pointer to r0 with empty operand barriers.
  Remaining recoil-offset constant uses r1 instead of ROM r2. Pinning that
  offset, with input/output/no barrier and local pointer variants, disrupts
  earlier allocation and worsens the candidate. Retained only direction hints.

Tool caution: depin.py tests the default fallback of guarded drafts, so its
reported hint removal does not validate NONMATCHING code. Its changes were
reverted and the guarded candidate rechecked explicitly; use it on extracted
unguarded C for draft minimization.

## Task 08150DB0 (2026-10-20)

Tags: #matching #player #cue #animation

Task exact coverage: 19.8% (+19.8 percentage points; 1,072 of 5,424 reviewed
body bytes). Normal C: 712 bytes; Register-pinned: 360 bytes. Drafts do not
count toward exact progress. Used per-file check.py only, with no full build,
network operation, or generated assembly changes.

- 08150DB0: matched PlayerCue_UpdateKnockdownRecovery, 712 bytes. Caller
  PlayerCue_Motion30 selects motion 0x1E; callback applies decaying recoil,
  reduces terrainRecoveryCounter, waits, and either stays down or plays the
  recovery animation. Porting velocity and scaling expressions from exact
  Player_UpdateRecoilAction matched on the first check. Reviewed extent ends
  at 08151078. No register hints.
- 08151078: matched PlayerCue_UpdateRecoilRecovery, 360 bytes. Caller
  PlayerCue_Motion29B selects motion 0x1D; callback decays recoil then plays
  recovery, advancing to phase two and acknowledging completion. An explicit
  empty case two preserves the original switch dispatch tree. Pinning the
  initial frame value to r3 removes a redundant copy and its padding. Marked
  MATCHING_CONCESSION: register-pinning. Final formatted check exact through
  081511E0.
- 08151A14: new guarded draft PlayerCue_UpdateTurningPalette, recorded 82.2%,
  95 realigned differences / 48 ignoring registers. Caller
  PlayerCue_BeginModeSeven sets facing and phase. Callback turns toward the
  requested direction, selects side-specific animations, and interpolates
  palette entries at 36A, 36C and 37A over a sixteen-tick triangle wave. Read
  the full body through its return/pool at 08151D90. Calculating red before
  green improved the first candidate from 65.6% to 82.2%. Pinning the player
  and weight registers regressed; a color inline and value barriers did not
  help. Remaining direction-branch, field-address, color temporary and global
  fade-counter allocation differences. Ghidra export is absent.
- 08151EB8: improved existing PlayerCue_UpdateWalkTarget draft from 98.6% to
  recorded 99.7%, two realigned differences / two ignoring registers. A
  separate arrival direction value with an empty output constraint lets the
  final-direction and computed-direction stores share their tail. Only the
  moving path's OR versus direction-address scheduling remains. Input-only
  constraints and register clobbers caused constant reuse and worsened
  allocation; reverted. Restored required local aliases and address fallback.
- 08152174: retained PlayerCue_UpdateMotionFade draft, recorded 81.7%, 217
  realigned differences / 37 ignoring registers. Read largest first and used
  relatives plus BIG_FUNCTIONS workflow. Existing packed animation stepper
  covers most of the six phases. Full-width timer arithmetic emitted identical
  code; removing the weight pin worsened total differences to 244. Remaining
  scale-store addressing, phase tails and decoder register allocation. Restored
  local aliases and address fallback; no exact coverage claimed.
- 08152A5C: new guarded PlayerCue_UpdateRecoilSlide draft, recorded 97.5%, six
  realigned differences / zero ignoring registers. Caller Motion29 selects
  this callback for actionMode three. One-time sound, packed animation playback,
  decaying slide, then standing callback and completion. Two address constants
  use different registers. Pinning address constants changed earlier sequence
  setup and worsened the result; reverted. Verified separate function prologue
  bytes F0 B5 83 B0 at 08152C3C after the callback's own return and literal
  pool. Added this evidence-backed boundary to progress_extra_functions.csv
  and progress_functions.csv: the previous 38.6% score erroneously included
  the independent following function/data. Actual callback extent is 480 bytes;
  default INCLUDE_ASM still retains the entire original 1,992-byte span.

Each exact function and each draft queued separately with build/commit.sh.
All four default assembly fallbacks check byte-identical. Combined validation
is deferred to the integrator as instructed. Names remain tentative where
marked; no shared documentation or generated outputs were committed.

## Task 0814E3D8 follow-up (2026-10-20)

Tags: #matching #player #cue #charge

Task exact coverage gained 8.7 percentage points (504 of 5,800 reviewed body
bytes). Normal C: 504 bytes; no register-pinned exact bytes. Used only fast
per-file checks; no full build or network operation.

- 0814E3D8: improved guarded Player_UpdateSelectedAttackCharge from 90.0% to
  recorded 90.6%, 71 realigned differences / 34 ignoring registers. Pinning
  the buffered-input temporary to r0 removes redundant copies (unit marked
  MATCHING_CONCESSION). Duplicating the charged phase calls regressed after
  preserving the original early exit, so reverted. Shared tier tail, particle
  initialization scheduling, descriptor argument and constant lifetimes remain.
- 0814F328: improved guarded Player_UpdateAttachedAttackCharge from 87.8% to
  recorded 88.5%, 72 realigned differences / 23 ignoring registers, using the
  same buffered-input r0 concession. Read the descriptor-action caller and
  existing charge analysis. Restored required local name alias and raw-address
  fallback. Local callee alias for Player_SpawnSelectedAttackProjectile is
  needed because this checkout's ELF still exposes sub_0814DFDC.
- 0814FC80: retained guarded Player_AdvanceScriptDestination at recorded 89.7%,
  44 realigned differences / 18 ignoring registers. A shared arrival direction
  temporary/store regressed to 87.0%; reverted. Restored the local alias and
  raw-address fallback. Existing movement flags/null tests and register
  lifetimes remain unresolved.
- 081503EC: new guarded PlayerCue_UpdateScaleTransition at recorded 84.0%,
  93 realigned differences / 28 ignoring registers. Motion7Turn,
  Motion7Variant3 and Motion7Variant7 install it for the ordinary player
  variants. Nine phases animate departure/arrival, alter scale and control
  flags, and select hidden/standing callbacks. Reconstructed the entire body
  through its return and literal pool at 081507F4. Store inlines delaying
  destination address formation improved the initial 73.6% candidate to
  82.9% candidate-length / 84.0% reviewed extent. Typed player accesses made
  no further change. Remaining constant reuse, address allocation, scheduling
  and shared tails. No register hints in this unit.
- 0814FFA4: new guarded PlayerCue_UpdateMoveDestination at recorded 80.1%,
  84 realigned differences / 14 ignoring registers. PlayerCue_MoveTo installs
  this callback with target position, final facing, movement-effect enable,
  effect magnitude 150 and completion script. It approaches the target on
  cadence ticks, decays the effect magnitude, invokes the completion script
  once and plays animation entries 16/17. Read through return/pool at
  081502AC. Delayed byte stores reduced structural differences; pinning
  distance/x/z to r8/sl/sb raised fuzzy score past the draft threshold. Marked
  register-pinning concession. Remaining facing load/store sharing, constant
  scheduling and allocation; no handwritten instructions.
- 0815093C: matched PlayerCue_UpdateDeparture, 504 bytes, first check.
  PlayerCue_BeginMode27 installs it for non-side player modes. Ported the
  camera lift / screen transition / descent phases from matched
  PlayerCue_UpdateSideDeparture, replacing its packed animation stepper with
  Player_SetAnimation and adding mode-specific sound plus scale resets.
  Four phases hide the player and acknowledge completion. No register hints.

Each function queued separately through build/commit.sh. All five guarded
assembly fallbacks check byte-identical; all draft scores recorded with
--nonmatching --record. Ghidra export is absent. Combined validation remains
with the integrator. No shared documentation or generated assembly changed.

## Task 08092814 (2026-10-20)

Tags: #matching #bee #ghost #actor-animation

Task exact coverage gained 23.1 percentage points, reaching 23.1% of these
4,540 reviewed function bytes (1,048 exact bytes). All exact C is unpinned.
No full build, network operation, shared documentation edit or generated
assembly commit; combined ROM validation belongs to the integrator.

- 08093810: drafted 82.1% as Bee_FollowWanderRoute (tentative). Consumes entry,
  installs entryTarget228, chooses a random heading near actor+0xE9 (or a
  reverse heading when context+0x3D4 bit 8 is set), builds nine route points,
  follows them, then chooses behavior from context+0x3DC after its cooldown.
  Context+0x428 is waypoint index; +0x43A is remaining interpolation budget;
  +0x480 holds two movement words. Full-extent check: 130 realigned differing
  instructions, 71 ignoring registers. Stack-vector addresses, PRE address
  reuse and register allocation remain. Typed context/node experiments and
  actor/context pins regressed and were reverted. Explicit Control offsets,
  delayed CpuSet initialization and shift-before-table RNG addressing helped.
- 080941F4: stopped without a qualifying draft. Complete exploratory C in
  untracked wip/sub_080941F4.c. Follows the same route shape but aims at the
  object at context+0x440, uses radius +0x43E, starts budget at 1280, and
  restarts with +0x42C << 5. Its target delta is recomputed at entry, each
  update and route restart. Best tested structural count was 91 (body pinned
  r4; fuzzy 61.9%); the unpinned final exploration was 53.3%, 298/140 realigned
  differences. Actor/context pins, pointer locals, declaration changes,
  typed waypoint context and helper rewrites did not yield a close candidate.
- 08095740: matched Ghost_UpdateEnergyDrainAnimation, 604 bytes. Directional
  profile entry 1 is selected using Ghost_GetDirectionalFrame; modes 0/1
  periodically subtract player energy, mode 1 also calls 0813D834. Tracks
  player+0x4F4 wrapping progress relative to context+0x5BE; elapsed +0x5BD,
  limit +0x561 and interval +0x560. Matched Select inline from Lun animation
  code supplied the shared sprite/tile animation branches. WrapCounter inline
  preserves the subtraction order. A typed u16 flag-setting inline and
  initializing zero after taking the destination pointer fixed final ordering.
- 08095C00: drafted 89.1% as Ghost_TurnTowardHeading (tentative). Turns in
  32-angle increments on field2CA bit 4, choosing direction with flags2CC
  bit 0x10000; resumes savedTarget when the octant matches actor+0x3CD.
  Refreshes duration scale and profile entry 1 animation. Full-extent check:
  39 realigned differences, 4 ignoring registers. Main residue is motion /
  sequence pointer register allocation, mask lifetime and Control zero timing.
  Explicit flag-mask locals and a mask-taking read inline helped. Pointer
  pins and wholesale replacement of Control regressed and were reverted.
- 08093EE4: matched Bee_ApproachSavedPlayerPosition, 444 bytes. On entry saves
  the player's two position words at context+0x450, selects recovery animation,
  copies context+0x429 to +0x3DC, sets flag 0x100 and plays song 0x3C8. Approaches
  the saved X/Z at speed 24 when farther than 32 on either axis; animation
  completion selects behavior 4. Expressing the saved position as a field of
  a motion view at context+0x444 reproduced the original addressing and all
  registers. Delaying the context byte destination until after loading its
  value and a local mask in Control fixed the other structural differences.
- 08092814: drafted 93.1% as Bee_ReturnToPlayerWhenStopped (tentative).
  Initializes random movement components at actor+0x2EC/+0x2F0 using the sine
  table scaled by 1/64; when stationary, disables movement and begins moving
  toward player+0x30, then selects phase 2 behavior 0. Full-extent check:
  13 differing instructions, zero ignoring registers (15 differing bytes).
  Explicit Control offset lifetimes and an inline CpuSet zero initialization
  reduced the structural residue to zero; RNG pointer arithmetic puts its
  shift before the table literal as in ROM. Remaining setup register choices
  resisted small local/type/store-inline variants; retained without pins.

Each match and qualifying draft was committed separately through build/commit.sh.
Draft scores were recorded with check.py --nonmatching --record; all three
normal assembly fallbacks check byte-identical. Ghidra export is absent.

## Task 08140CC4 — 2026-10-20

Tags: #matching #player #movement #camera #nonmatching

Exact gain: approximately 0.018 percentage points of total code (448 bytes),
without register pinning. One exact match, three new guarded drafts and one
updated existing draft. Aggregate totals remain in PROGRESS.md. Worked solo;
no network, full build, or direct git mutation. All commits use build/commit.sh.

- 08142194: updated existing guarded draft to 91.5%, named
  Player_UpdatePushAction (tentative: neighbor pushes and contact effects).
  Read BIG_FUNCTIONS and inspected assembly, callers/callees and relatives.
  Explicitly sequencing the direction difference, doubled offset, addition of
  eight and facing addition reduces structural realigned differences from 47
  to 35; total differences are 105 versus the original 104. The slight fuzzy
  decrease from 91.6% accompanies this structural improvement. Remaining
  differences include extra saved high register, terrain helper mask/narrowing
  and address/register allocation. Existing body preserved under NONMATCHING.
- 08140CC4: stopped unmatched, no qualifying draft. Static caller
  Player update 0815788C invokes it to maintain locked movement. Byte 284 bit
  one gates direction refresh; bytes 48D/48E delay release, byte48F stores
  direction, halfwords490/494 store ten-unit direction-derived components.
  Modes 1/2 or mode6 with valid direction history start locking; mode3 phase5
  with contactMagnitude starts opposite facing. Typed local layout improved
  initial 62.2% candidate to 74.4% (106 realigned differences, 12 structural).
  Owner r5 pin regressed to 72.0%; no productive next hypothesis remained.
  Scratch candidates remain under ignored wip/codex-low and build/.
- 08140F40: drafted 99.7% as Player_GetForceDirection. Quantizes global angle
  0200053C, compares terrain heights in the directional neighbor; diagonal
  movement checks the corner and the adjacent cell selected by within-cell X/Z.
  Pinned angle/result to r0 and direction to r4 recover register allocation
  and tail. One differing instruction remains: ROM arithmetic shift versus
  compiler logical shift in the nonnegative masked-angle quantization.
  Recorded reviewed extent is 632 bytes. depin on the unguarded candidate
  skips because the base is nonmatching. Required concession marker retained.
- 08141C40: drafted 99.3% as Player_EmitMovementStepEffects. Called by
  Player_UpdateMovement and the resource dash. Animation cue counter/flags
  gate emission; action flags, repeated-effect presence, terrain/action status
  and mode5 select effects/songs. A global bit at GameState+868 enables an
  additional position effect. Passing that mask to an inline recovers its
  mask-before-global-load order. Four realigned instruction differences remain:
  initial null-path destination and flag load/mask scheduling. No pins.
- 08141FA0: matched as Player_UpdateResourceDash (tentative dash identity).
  Entry tests shared resource availability or actionFlags bit20, sets speed
  from GameState+876 (or 26 for free dash), caps at32 and uses 512/speed ticks.
  Phase0 sets directional velocity, movement effects and animation; failed
  entry uses phase1 for eight ticks. Using the existing Player padding bytes
  at4F0/4F1 avoids a second aliased owner pointer. The direction store is
  facing at3A4, not coordinateAxis at4A5. Exact after formatting: 448 bytes.
- 0814159C: drafted 83.7% as existing Player_UpdateCameraOffset. Starts from
  global camera position minus player position (Y minus150), steps horizontal
  offsets by thirty units from directionHistory, clamps, then derives facing
  and side from the offset angle when camera input is processed. Remaining
  28 realigned differences / 11 structural concern input-direction allocation,
  angle/mask scheduling and derived field constants. No pins. The default
  INCLUDE_ASM must use existing promoted Player_UpdateCameraOffset filename,
  while the draft retains a local alias to sub_0814159C; corrected after the
  draft commit when the default check exposed the unavailable old filename.

All four drafts were formatted and recorded with check.py --nonmatching
--record using reviewed extents. All default assembly fallback checks pass,
including the camera fallback correction. The dash passed the exact C check.
Ghidra export is absent. Combined ROM validation remains for the integrator.

## Task 080C66B0 — 2026-10-20

Tags: #matching #golem #nonmatching #register-pinning

Exact code gain this pass: 0 percentage points. The checkout already held three
exact C implementations and three guarded drafts under promoted names. Restored
file-local aliases required by this worker task and raw sub-address assembly
fallback names required by this checkout's generated assembly. Existing Golem
callers 080CC910, 080CDF14 and 080CEDF0 install route/windup callbacks; adjacent
Golem controls establish the provisional state names. No new exact coverage is
claimed for the inherited implementations.

- 080C6A68: drafted 86.5%, up from 85.9%. Read BIG_FUNCTIONS before work.
  Explicit packed X/Z word updates preserve the neighboring halfword; pinning
  variantPointer to r5 and mirrorPointer to r2 recovers selector allocation.
  Realigned differences improved from 96 total / 32 structural to 93 / 20.
  Remaining residue is stack-slot placement, packed-store scheduling and late
  context direction address scheduling. An explicit sum temporary reached
  17 structural differences but worsened total to 102 and fuzzy to 84.3%; rejected.
- 080C75B8: inherited match verified, Golem_UpdateRecoil, 1008 bytes.
  Decelerates recoil, approaches the saved position, signals completion.
- 080C725C: inherited guarded draft retained at 89.1%, Golem_UpdateSlideImpact.
  54 total / 23 structural differences: motion pointer copies, RNG allocation,
  control-reset offsets. Pinning body to r10 regressed to 83.9%; rejected.
- 080C6F58: inherited match verified, Golem_UpdateChargeWindup, 772 bytes.
- 080C66B0: inherited match verified, Golem_UpdateRouteMotion, 576 bytes.
- 080C68F0: inherited guarded draft retained at 81.6%, Golem_UpdateChargeStop.
  37 total / 9 structural differences. Pinning quadrant/selection to r3,
  mirror to r2 and keyframe mode to r7 regressed to 78.1%; rejected.

All six formatted using build/venv/bin/clang-format. Exact C checks and all three
assembly fallback checks pass. Draft scores recorded by check.py --nonmatching
--record. depin retains 3/4/3 hints on route/windup/recoil. On guarded drafts,
depin tested the assembly alternative and incorrectly removed hints; restored
all draft sources before final checks, so it gives no draft minimality evidence.
relatives.py failed while generating its progress cache; no network or full build
was attempted. Per-function commits completed through build/commit.sh. Combined
ROM validation and name promotion remain for integration.

## Task 080B77D0 — 2026-10-20

Tags: #matching #skeleton #nonmatching

Exact code gain: 0 percentage points. Improved two inherited guarded drafts and
restored required local aliases/fallback spellings on a third. Worked solo;
no network, make, full build, or direct git mutations. All commits use the driver.
Read entry-point/worker/cloud guidance and BIG_FUNCTIONS; inspected assigned
assembly, condensed large bodies and prior committed notes before changes.

- 080BBDD0: drafted **88.9%**, Skeleton_ConvergeDisplacedPosition, up from
  87.1%. Narrowing PickDirection's selected direction to u8 reduces realigned
  differences from 316 total / 44 structural to 256 / 46 and makes candidate
  length equal the reviewed 4116-byte extent. It represents a byte direction
  throughout. Mask-first short/settle updates, a signed group flag helper,
  deriving saved state directly from actor, narrowing prior direction and
  pinning the outer actor to r8 did not improve the result. Remaining recovery
  pointer/flag lifetimes and RNG allocation require further work. No pins.
- 080B7C10: retained inherited **92.8%** draft, Skeleton_SelectDistanceReaction.
  Restored file-local own-name and retreat-callee aliases plus raw-address
  INCLUDE_ASM spelling for this checkout's generated assembly. Adjacent local
  declaration swaps, byte direction result, removing current-direction r5 pin
  and direct RNG table access did not improve 146 total / 22 structural
  differences. Existing concession marker retained. depin on the actual
  unguarded scratch candidate skips because the base is not exact.
- 080B9158: stopped unmatched. Inspected entry, condensed repeated terrain
  recovery and installer 080B5D84 (timed behavior 56, category-dependent timer).
  Entry samples RNG, probes direction through 080B713C, then uses animation 15
  and route/chase recovery on failed direction or landing. No complete improved
  candidate recovered beyond prior notes' below-threshold attempt. relatives.py
  fails generating its progress cache (progress.py subprocess failure); no
  tooling change made to work around that failure.
- 080BF0E4: stopped below threshold; existing production draft unchanged.
  Scratch PlayerSlot index-first loading changes reviewed realigned residue
  from 473 / 164 to 476 / 162, scoring 72.4% over 3040 bytes. Narrowing the
  random-direction helper's return did not improve structure. Directional
  rise animation expansion and recovery allocation remain the blockers.
- 080BA268: new scratch candidate, tentative Skeleton_ReleaseBoneProjectiles,
  stopped at **64.5%** over reviewed 784 bytes (candidate 772), 148 total / 50
  structural differences. Caller 080BA13C installs it following queued contact;
  animation 22 releases two randomized D45E/1DC3 bone effects at heights +260
  then +220 with upward velocity 30, reinitializes category 5 and collider.
  Signed shift helper preserves the quantization branch; packed position copy
  and typed addition recover much of the launch sequence. Remaining polar
  argument scheduling, redundant angle masks, effect literal scheduling and
  register allocation prevent a qualifying draft. Scratch only, no src body.
- 080B77D0: drafted **94.3%**, Skeleton_TrySelectRetreatCell, up from 87.8%.
  Deriving group at its late callback use instead of retaining a function-wide
  group pointer removes its extra prologue setup; 52 / 30 differences become
  27 / 27. A byte retry counter regresses. Moving north/south tail computations
  to shared goto labels reduces structure to 25 but increases total to 43 and
  lowers fuzzy to 93.1%; restored the stronger 94.3% candidate. Remaining
  residue is north/south map reload tail layout and pool placement. No pins.

Three changed guarded files formatted directly with build/venv/bin/clang-format;
reviewed scores recorded with check.py --nonmatching --record, and all three
assembly fallback checks pass. Below-threshold candidates remain in ignored
wip/codex-low. No exact coverage claimed for assembly fallbacks. Combined ROM
validation remains for integration. Shared docs and totals were not regenerated.

## Task 080C68F0 (2026-10-20)

Tags: #matching #golem #register-pinning

All six assigned functions already contained guarded drafts. Retained existing names from symbols/functions.csv and added the required file-local aliases. Exact C progress: 0 percentage-point gain; all remain drafts.

- 080CCD7C: drafted 80.4%, Golem_UpdateChargePursuit. Inspected largest first with asmat, condense and relatives. Reviewed-extent check: 539 realigned differing instructions, 141 ignoring registers. Best matched relative covers only 29%; charge/turn resets and route pointer lifetimes remain substantial. Added local aliases for charge-stop/recovery callback references because the cached ELF still uses address names. No body changes retained.
- 080C8774: drafted 95.8%, Golem_ResumeBehaviorAfterDisplacement. Baseline 73 realigned differences / 12 ignoring registers. RNG wrap-mask r8 pin regressed to 77.7% (524/174); retry-loop goto form regressed to 91.9% (165/16). Restored baseline. RNG table/wrap allocation and PRE stack slots remain.
- 080CE188: drafted 90.3%, Golem_ResumeRouteAfterControl. Baseline 114/34 realigned differences. Narrowing RandomDirectionExcept's return to u8 regressed to 89.3% (128/41); reverted. Shared RNG loop and outer group/control-reset lifetimes remain.
- 080C6A68: drafted 86.5%, Golem_UpdateChargeRecovery. Baseline 93/20 realigned differences. Swapping the step/candidate stack declarations left output identical; reverted. Packed halfword scheduling and stack sample placement remain.
- 080C725C: improved draft from 89.1% to 91.3%, Golem_UpdateSlideImpact. Binding the RNG table to r6 with an empty value barrier reduced realigned differences from 54/23 to 40/19. Removing the barrier restores the baseline. Replaced collider offset cast with its existing typed flags field (same generated code). Candidate is 12 bytes longer than the 860-byte target extent. Remaining seed allocation, repeated table reloads and collider/reset scheduling prevent exactness.
- 080C68F0: improved draft from 81.6% to 91.9%, Golem_UpdateChargeStop. Ported mirror-pointer/value bindings and quadrant/address barrier from the matched SceneActorAnimation_SelectDirectionalEntry sibling; removed the unnecessary u8 flags cast at Anim_SetSequence. Realigned differences fell from 37/9 to 18/9. Explicit flags/selection pins regressed to 79.0%; a mirror-load barrier regressed to 84.3%. Remaining pointer copies, flag load order and selector scheduling prevent exactness.

Formatted all six files directly with the project's clang-format (no make). Ran check.py --nonmatching --record for every draft. The score recorder does not resolve INCLUDE_ASM aliases; changed the two improved drafts' fallback operands to sub_ names and reran recording so their fuzzy lines contain the measured scores. This required separate corrective driver commits after the initial draft commits. Both improved default assembly fallbacks checked byte-identical. depin --write skips nonmatching candidates; attempted it on all three hinted units. No full build or network operations performed. Commits queued through build/commit.sh only.

## Task 080B5F38 (2026-10-20)

Tags: #matching #skeleton #register-pinning

Exact code gain: **+0.0376 percentage points** (908 bytes); Register-pinned
category +0.0151 points (364 bytes), unpinned +0.0225 points (544 bytes).
Worked solo on the already checked-out task branch, without network or full build.

- 080B5F38: **matched**, tentative Skeleton_ResetRouteReaction. Caller
  Actor_RestoreSnapshot restores the live snapshot before this reset. The context
  has timer byte +6D0, saved flags word +6DC and counter halfword +704. A local
  typed context reproduced the compiler's +34 address reuse and zero lifetimes;
  raw casts had scored only 76.1%. Mask-first updates preserve instruction order.
  Sharing a final ready-byte pointer across the route-result branches closed the
  last five realigned instructions. depin --write removed seven of nine hints;
  only control pointer r3 and state-byte offset r2 pins remain. Formatted source
  passed exact check.py (364 bytes), then committed through the driver.
- 080B6210: **matched**, tentative Skeleton_UpdateFacingColliders. Read caller
  Skeleton_UpdateActionFlags and matched forward-collider relative 080B1038.
  Skips unavailable actors, flag 800 and control kinds 39/3D; context timer +6D0
  gates bit 4 in actor/context body colliders. Facing shifts the position by 128;
  category 6 uses 180/84 horizontal bounds versus 160/64 for other categories,
  swapping by facing quadrant, with vertical bound 100. Typed body +624 and attack
  +678 colliders establish the local context view. Porting the mask/offset helper
  from 080B9030 and passing the attack collider through a bounds inline changed
  93.4% to exact. No pins. Formatted source passed check.py (544 bytes); driver commit.
- 080B7C10: retained **92.8% draft**, Skeleton_SelectDistanceReaction. Read large
  function guidance, assembly and inherited source. Returning signed halfword
  height plus explicit shape mask increased realigned differences from 146/22
  to 167/22; reverted. Restored required local own-name and retreat-callee aliases
  and raw sub_ assembly fallback. Recorded score and driver commit. Existing
  recovery-stack/address-lifetime residue remains; existing concession retained.
- 080B77D0: retained **94.3% draft**, Skeleton_TrySelectRetreatCell. Width locals
  left 27/27 realigned differences unchanged. Keeping a late group pointer across
  TryRoute regressed to 75.1% (106/64); reverted. Remaining north/south width-tail
  placement and late group calculation require a different control-flow shape.
  Restored required local alias and raw sub_ fallback, recorded score and driver
  commit. Both guarded functions' default assembly fallback checks passed;
  those checks do not count as C matches.
- 080B9158: stopped unmatched. Read entry, installer 080B5D84, condensed repeated
  terrain recovery and matched relatives (best Dog_ResumeRoute covers only 35%).
  Duplicated route-height/saved-control recovery remains substantially larger
  than a straightforward relative port. No complete candidate and no new useful
  matching hypothesis beyond the earlier recorded attempt. Ghidra export absent.
- 080BA268: stopped below draft threshold. Revisited scratch
  Skeleton_ReleaseBoneProjectiles against assembly and existing notes. Packed
  position union plus source-first vector addition improved current baseline
  63.4% (154/56 realigned) to 66.7% (141/47), 780 candidate bytes versus reviewed
  784-byte extent. Pinning Polar's pointer/angle/speed and actor/context regressed
  structural differences to 54; restored unpinned packed candidate in ignored
  wip/codex-low. Remaining argument scheduling, redundant masks and collider
  field loads prevent a qualifying draft. No production source added.

All accepted C and guarded draft files were formatted directly with the pinned
clang-format. Per-file exact checks and --nonmatching --record only; no make,
tools/build.py, full ROM check, shared-doc regeneration, direct git mutation,
or network operation. Integration must perform combined ROM validation.

## Task 081879A8 (2026-10-20, follow-up)

Tags: #matching #armor #decision

Exact C gain: **0.0 percentage points**. Worked solo on the assigned branch,
without network or full builds. Existing scratch candidates from the previous
Armor task supplied the starting points. Ghidra export remains unavailable.

- 08187F44: stopped below threshold, baseline 65.9%, 308 realigned differences /
  146 ignoring registers. Read largest first, BIG_FUNCTIONS, matched relatives
  (best covers only 9%) and assembly entry. Halfword coordinate locals raised
  fuzzy to 66.5% but worsened structural residue to 164; rejected. Shared lateral
  attack tails and coordinate lifetimes remain. No production draft added.
- 081879A8: stopped below threshold, baseline 63.2%, 289/131 realigned residue.
  Halfword coordinate locals scored 64.6% but worsened structure to 154; rejected.
  Same shared-tail and coordinate-lifetime blockers as the close-combat sibling.
- 081885BC: stopped below threshold at 73.0%, 234/167 realigned residue. Reviewed
  primary flag dispatcher and assembly's shared timer/reset tails. Unsigned
  absolute coordinates regressed to 72.7%; explicit inverse axis tests and
  absolute-coordinate pins compiled identically. No production source added.
- 08188B7C: stopped below threshold at 71.9%, 239/168 baseline residue. Absolute
  coordinate pins slightly improved scratch output to 72.4%, 235/165, still below
  threshold. Unsigned absolutes regressed; inverse axis tests were identical.
  Experimental pins remain only in generated variant artifacts, not source.
- 0818911C: stopped below threshold at 69.9%, 173/50 realigned residue. Existing
  aggressive-decision scratch uses halfword coordinates; changing them to words
  regressed to 68.4%, 305/60. Signed flag type compiled identically. Remaining
  shared tails, coordinate spills and registers block a qualifying draft.
- 081896D0: retained **81.5% guarded draft**, Armor_UpdateFacingDecision. Added
  required file-local address alias and changed stringified INCLUDE_ASM operand
  to raw sub_081896D0. Recorded score: 158 realigned differences / 48 ignoring
  registers. Halfword coordinates, unsigned absolutes and volatile/byte flag
  types regressed. Full coordinate-register bindings reduced structural residue
  to 24 but increased total differences to 199 and dropped fuzzy to 72.4%; partial
  bindings also failed to improve a qualifying candidate. No pins retained.
  Default assembly fallback checked byte-identical; this is not an exact C match.

Only check.py and variant checks were used. Existing accepted source remains
unpinned and its body unchanged. Queued the guarded alias/fallback correction
and these notes through build/commit.sh; combined ROM validation stays with the
integrator. No shared documentation or totals regenerated.

## Task 080ABDCC (2026-10-20, pursuit register closure)

Tags: #matching #vorn #tentacle #reaction

- 080ABDCC: stopped unmatched after largest-first review of BIG_FUNCTIONS,
  initializer entry, condensed listing and relatives. Best relative explains
  only 9%; mixed drawable setup, script options, terrain placement and playback
  still lack a complete close C candidate. Ghidra export is unavailable.
- 080ADCC0: **matched**, Vorn_UpdatePursuitDistance, 1120 bytes in the
  Register-pinned category. Prior 98.9% candidate had six register-only
  differences. A separate middle-distance flag accessor with offset bound to
  r3 first changed downstream allocation, resolving the other two pairs.
  Binding its field to r1 and mask to r2 left only instruction order; consuming
  the mask before assigning the offset reproduced movs-before-ldr exactly.
  Empty operand constraints contain no instructions. Removed NONMATCHING guard
  and verified default C with check.py: zero differing bytes.
- 080AD75C: stopped unmatched. Revisited full assembly and relatives; stagger
  burst and saved-motion convergence each explain about 25%, while route and
  control-reset tails require additional reconstruction. Prior notes on sound,
  clamped quarter-motion and recovery timers remain applicable.
- 080AD530: stopped unmatched. Revisited complete assembly, caller/control
  context and prior 58.5% attempt. Completion after event 18, context mask
  0x1D dispatch and route-failure/control-reset layout remain the main blockers.
- 080AE1C4: retained guarded **81.2% draft**, Vorn_UpdateKnockbackRecovery.
  Recorded current score: 113 realigned differences, 36 ignoring registers.
  Adjacent declarations and reversed OR operands compiled identically. Removing
  volatile group raised fuzzy to 82.3% but worsened structure to 38; rejected.
  Added required local address alias and explicit raw-address assembly fallback.
- 080AE6F0: retained guarded **82.8% draft**, Vorn_ResumeReactionRoute.
  Recorded current score: 57 realigned differences, 21 ignoring registers.
  Declaration swaps were identical; volatile group and removing the flags pin
  worsened structure to 35 and 30 respectively. Added local address alias and
  explicit raw-address assembly fallback. Existing register concession retained.

Solo work, per-file check.py and variants only. Draft scores were recorded and
both assembly fallbacks verified; those fallback checks are not C matches.
Separate driver commits for the exact match and each guarded draft. No full
build, network access, direct git mutation or shared-doc regeneration. Combined
ROM validation and global percentage-point accounting remain with integration.

## Task 080D9B20 (2026-10-20, directional-cell and dog reactions)

Tags: #matching #scene-actor #dog #terrain

No exact matches; exact code gain 0.0 percentage points. Retained four guarded
production drafts and their existing descriptive names. Added required local
address aliases and raw sub-address INCLUDE_ASM fallbacks so the checker resolves
them against this worktree's ELF. Used per-file checks only, no full build.

- 080E25FC: retained Dog_ChooseReactionBehavior at **83.5%**. Largest-first
  assembly/relative review; 386 realigned differences, 148 ignoring registers.
  Widening the motion accessor, reading flags directly and replacing the signed
  group bitfield with a u16 store all worsened structure; widening the saved
  direction parameter was identical. No candidate changes retained.
- 080E0750: stopped unmatched after entry assembly, condensed listing and
  relatives review. Entry consumes +0x2B2, quarters signed motion components,
  selects animation 3 and updates motion flags. Shared chase terrain lookups
  explain 32%; saved-motion convergence explains a separate entry region.
  Complete reaction/control-flow reconstruction remains necessary. Ghidra
  export gen/ghidra/decomp.c is unavailable.
- 080E4B34: retained Dog_BeginDelayedRouteRecovery at **87.7%**; 131 realigned
  differences, 40 ignoring registers. The same motion-width/direct-load and
  group-bitfield variants worsened structure; saved-direction widening was
  identical. Twenty-update delay and saved group control packet remain visible.
- 080D9B20: retained SceneActor_FindDirectionalCell at **82.3%**. Changing the
  Adjacent inline return from u8 to s32 reduces realigned differences from
  681/574 structural to 269/250 structural, despite fuzzy dropping from 85.3%.
  Widening Open instead worsened structure to 668. Kept the realigned improvement
  as BIG_FUNCTIONS recommends. Repeated passability branch merging still differs;
  removed duplicate fuzzy annotation and recorded current score.
- 080DEA7C: retained Dog_UpdateCollisionBounds at **99.4%**, three structural
  instructions differ: compiler rematerializes 0x2D0 rather than subtracting four
  from the earlier 0x2D4 offset. Explicit offset lifetime constraints reproduce
  that subtraction, reaching zero structural differences, but introduce nine
  register differences downstream (table signed-load zero index, heading store,
  collider address). Declaration swaps do not improve them. A pinned pointer
  resolves two more register instructions in another variant; a forced zero
  table index adds instructions. Rejected these experiments and kept unpinned
  99.4% source. No new register concession committed.
- 080DE6E0: stopped unmatched. Built a complete initial candidate in
  wip/sub_080DE6E0.c (not production or committed): animation tick zero gates
  effect spawning at frames 1/5/7 for kinds 1/3, random position jitter and
  rearward 80-unit offset; returns gUnk_086061E4 frame/chase entry. Candidate
  scores only 40.7% over reviewed 588-byte extent, with 246 realigned differences
  (177 structural) and oversized output. Moving CpuSet's zero initialization
  to the use improves the first draft but is far below draft acceptance.

Formatted the four production units directly with the pinned clang-format and
recorded scores with check.py --nonmatching --record. Queued each guarded draft
separately through build/commit.sh. No network, direct git mutation, shared docs,
ROM-derived artifacts or full-build commands. Combined validation remains with
the integrator.

## Task 08154CD4 — 2026-10-20, resumed worker pass

Tags: #matching #player #animation

- 081561EC: stopped unmatched after largest-first assembly and relatives review. Best relative still covers only 17%; no whole-function hypothesis beyond the prior particle-pool, target-motion and inline animation observations. No new source retained.
- 08155008: retained guarded PlayerCue_UpdateAlternateFade draft, recorded 86.1%, 130 realigned differences / 27 structural. Corrected the completion callback to sub_08154EE4 (alternate resource replay), as proven by the ROM literal, rather than the similarly named sub_081538DC. Restored local aliases for the cached ELF. Phase scale addressing and playback allocation remain.
- 08155D58: stopped below draft threshold at 74.9%, 153 realigned differences / 55 structural, versus the previous 72.9% candidate. Changed the script parameter count to a 16-bit bitfield and removed the cached frame pointer from pulse stores/increments. Pulse source remains in untracked wip/sub_08155D58.c; not promoted. Shared zero/one lifetimes, script pointer addresses and pulse field addressing remain.
- 081559A4: retained guarded Player_UpdateTimedChargeAction draft at 87.3%, 65 realigned differences / 30 structural. Script count bitfield expresses the observed word mask/store but gives the same score. Restored the required local alias. Pointer lifetimes and shared constants remain.
- 081556E0: retained guarded Player_UpdateStateMotionCue draft at 88.3%, 54 realigned differences / 31 structural. Signed Event helper regressed to 56 differences; early event mask barrier regressed to 62. Reverted both and restored local alias.
- 08154CD4: improved guarded PlayerCue_UpdateSpiralRecovery from 93.8% to 98.1% (+4.3 fuzzy percentage points), five differing bytes / five realigned instructions / zero structural. Compute the resource destination before its literal, load the effect timer mask after the increment store, keep seven and its low-bit copy distinct, and share the sequence pointer through playback. Explicit resource and final byte pointer lifetimes fix remaining extra copies. Sprite offset and effect timer offset still use different registers. Further offset pin variants regressed and were discarded.

Formatted using build/venv/bin/clang-format; recorded all four draft scores with check.py --nonmatching --record. Default assembly fallbacks pass exact checks after spelling INCLUDE_ASM entries with sub addresses, since the cached build lacks the promoted assembly filenames. The documented depin guarded-draft limitation still applies; no fallback-only result was used to remove hints. Four separate draft commits queued through build/commit.sh. No full build or network operations. Exact code progress gain: 0 percentage points.

## Task 081EAB64 — 2026-10-20, resumed display and actor actions

Tags: #matching #scene-actor #display #animation

No exact matches; exact code progress gain 0 percentage points. Five guarded
production drafts retained. Restored file-local address aliases and raw-address
assembly fallbacks for the cached ELF; added renamed sprite-callee mappings.

- 081EAB64: retained Params3D0_UpdateDisplay at 91.0%, 125 realigned
  differences / 31 structural. Reviewed entry, creator relationship, prior notes
  and relatives. Signed position arithmetic and declaration swaps compile
  identically; widening position and changing palette pointer lifetimes worsen
  structure. Palette pointer reuse and selection-animation ordering remain.
- 081EE518: retained draft at 89.5%, SceneActors_UpdateScreenCrossingAction
  (replaces the ordinal SequenceTwo name). Computing the second sprite pointer
  after Anim_SetSequence and through the shared base reduces structural
  differences from 20 to 13; total differences increase from 75 to 84.
  Kept the structural improvement per BIG_FUNCTIONS guidance despite lower fuzzy
  score. Volatile command loads and a second base constraint do not improve it.
- 081EE130: improved SceneActors_UpdatePeriodicEffectAction to 91.9%, 43
  realigned differences / 20 structural, from this checkout's effective 90.4%,
  47 / 25. Reuse the entry-byte pointer, materialize the random destination
  before the mask, and address completion through the original input.
  Remaining initialization, packed animation-step and random-mask ordering.
- 081EEAD8: improved SceneActors_UpdatePaletteDispersal from 87.0% to 89.4%,
  52 realigned differences / 21 structural (was 63 / 27). Initialize the loop
  index, destination and source in that order. Explicit palette-source lifetime
  barriers worsen structure; reversing vector declarations compiles identically.
- 081EDB74: retained SceneActors_UpdateLeftDeparture at 85.3%, 58 realigned
  differences / 22 structural. Reused entry byte is identical; deriving the
  second sequence directly from input or constraining zero-store offsets worsens
  the result. Two sprite/sequence pointer lifetimes remain the main obstacle.
- 081EEECC: stopped below acceptance at 66.0%, 111 realigned differences / 59
  structural. New full candidate remains untracked in wip/sub_081EEECC.c.
  Hides two sprites, shakes offsets for sixty ticks, emits particles every odd
  tick after sixty, clamps a departure counter and blinks to completion.
  Retaining the RNG table across particle setters improves the initial 60.5%
  candidate; pinning the actor/base worsens it. The checked extent includes a
  separate-looking entry-byte reset routine at 081EF12C (push/pop and return),
  currently emitted as raw bytes before SceneActors_TickStateCallback; boundary
  review is needed before interpreting a whole-extent exact result.

Formatted the five production units directly with build/venv/bin/clang-format,
recorded scores with check.py --nonmatching --record, and verified all default
assembly fallbacks. Fallback matches are not C progress. Queued each guarded
production draft separately through build/commit.sh. No full build, network,
direct git mutation, shared-doc regeneration or generated ROM artifact commits.

## Task 0814FFA4 follow-up (2026-10-20)

Tags: #matching #player #animation #cue

Exact progress: 0.0 percentage-point gain. Six existing guarded drafts retained, with required file-local aliases restored for this cached address-named ELF. Only per-file check.py checks; no network or full build. Named callers and existing symbols establish the PlayerCue prefix; callback interpretations remain tentative.

- 08152174: drafted 81.6%, 218 realigned differences / 37 ignoring registers. Largest first, reviewed asmat and exact relative 08151D90. Sprite-byte setter inline reduced structure to 34 but worsened total to 243 and fuzzy to 79.7%; reverted. Remaining decoder allocation, phase tails and scale addressing.
- 081503EC: drafted 84.4%, 90 realigned differences / 25 structural. Explicitly duplicate the shrink tail instead of entering a shared C label. Baseline after alias repair was 83.5%, 95 / 28. Phase-two flag stores, scale arithmetic and byte-address lifetimes remain.
- 08151A14: drafted 83.1%, 87 / 48. Explicit case-four and case-six animation calls improve total from 95 at baseline; structure unchanged. Palette arithmetic, phase branching and field-address allocation remain.
- 0814FFA4: drafted 80.0%, 82 / 12, versus baseline 80.1%, 84 / 14. Cache the facing direction inside the inline to avoid redundant loads; differing bytes fall from 453 to 335. Direction stores, zero lifetime, multiplication selection and allocation remain.
- 08151EB8: retained 99.7%, two differing instructions / two structural: movement-direction OR scheduled after field-address calculation. Read/write and input-only barriers on the second direction calculation regress to 96.5% and 96.8%; reverted both.
- 08152A5C: retained 97.5%, six differing instructions / zero structural. Speed-offset constant wants r2 and direction-offset constant r5. Pinning both constants with input or read/write constraints regressed to 81.9% by losing address reuse; reverted. Reviewed body is 480 bytes through 08152C3C; default fallback still includes the following assembly extent, so default-path matches are never counted as C progress.

All six recorded with check.py --nonmatching --record; all default assembly fallbacks also checked. depin.py --write incorrectly removed hints by checking the guarded assembly path; all such removals were rejected and the original hints restored before final recording. No claim of removable hints. Formatting applied with pinned clang-format, then alias whitespace restored to the exact naming contract. Separate draft commits queued through build/commit.sh, notes last.


## Task 081175F8 follow-up — 2026-10-20

Tags: #matching #glove #motion #recoil

Two new unpinned exact matches: 0.0583 percentage-point code gain (1,408 bytes).
All six task functions reviewed; only per-file check.py checks, no full build or
network. Restored required file-local aliases for the cached address-named ELF.

- 08117948: retained guarded Glove_UpdateStopping at 86.4%, 99 realigned
  differing instructions / 39 ignoring registers. Baseline was 90.0%, 85 / 42.
  Passing the motion object into the secondary action inline reduces structural
  differences, though allocation and the fuzzy score worsen; retained the
  structural improvement per BIG_FUNCTIONS guidance. Separate byte and halfword
  initialization zeros remove four further differences. Constant-first damage
  flag and declaration swaps do not improve it. Action pointer lifetimes remain.
- 081182E4: improved guarded Glove_UpdateDefeatEffects from 80.7% to 81.3%,
  138 / 63 (baseline 140 / 66). Delay actionSelf initialization until after
  sound calls; calculate the palette destination before indexing; swap style
  and motionBias declarations. Cleanup pointer setup/spills and motion
  allocation remain. Existing scalar-height register concession retained.
- 08117F6C: stopped below acceptance, best new scratch 74.9%, 120 / 49,
  Glove_UpdateRecovery. Reusing an explicit flags pointer, delaying the vertical
  position pointer until after the height clamp, and placing the link mask
  before global loads improve structure. Transplanting the exact recoil motion
  block regresses to 63.8%; unpinning height regresses too. Pointer spills and
  lifetimes remain. Source stays in untracked wip/sub_08117F6C.c.
- 081175F8: stopped below acceptance; retained previous 70.0% launch scratch.
  Inlining the physics body directly regresses to 66.2%; explicit motion
  pointers reach 37 structural differences but only 63.6% overall. No new
  declaration-swap improvement. Useful alternate in ignored
  build/launch-explicit-pointers.c, original in wip/sub_081175F8.c.
- 08119378: matched Glove_UpdateRecoilReturn, reviewed extent 704 bytes.
  Script command case 2 configures recoil and selects this callback; animation
  zero runs until ActorRecoil_UpdateMotion returns nonzero, then consumes the
  trigger and selects 081198F8. Adapted exact Glove_UpdateImpactRecoil motion,
  removing speed clamps. Declaring completion before height establishes stack
  order; callback/activation pointer/zero locals establish the final stores.
- 08119638: matched Glove_UpdateStaggerReturn, reviewed extent 704 bytes.
  Script command case 6 selects this counterpart; animation 0x21 completes
  into Glove_UpdateStaggerRecoil (08118E30). Same exact body as 08119378 except
  the animation and completion callback.

Formatted production units with pinned clang-format. Both matches pass explicit
704-byte check.py acceptance. Draft scores recorded with --nonmatching --record;
assembly fallbacks also pass and are not counted as C progress. Queued one
commit per match/draft through build/commit.sh, notes last. Batch integration
validation remains with the orchestrator.


## Task 081B28D8 follow-up — 2026-10-20

Tags: #matching #vanargand #motion #animation

Exact progress: 0.0 percentage-point gain. Reviewed all six existing guarded
Vanargand drafts, largest first. Restored required file-local address aliases;
assembly fallbacks use address symbols because INCLUDE_ASM stringifies its
argument. Existing functions.csv names and neighbouring named callbacks retain
the Vanargand prefix; interpretations remain tentative.

- 081B3410: retained drafted 94.5%, 52 realigned differences / 18 ignoring
  registers. Read BIG_FUNCTIONS, asmat, relatives and condensed listing. Byte
  variant reads, duration barrier, declaration swaps and per-case local entry
  variables do not change output. Initial animation setup order, variant
  address reuse and packed playback allocation remain.
- 081B28D8: improved drafted 98.2% to 99.9%, 15 / 4 differences reduced to
  2 / 2. State four needs an ordinary local entry value rather than the
  function-scope r1-pinned value. Only the order of two register copies at
  081B2DB6 and 081B2DB8 remains. Explicit flags pointers, entry-register pins,
  entry-input barriers, swapped entry setup and a scoped entry pointer regress
  or leave the two copies unchanged; stopped after focused scheduling trials.
- 081B2F38: retained drafted 93.2%, 53 / 28. Inspected assembly; declaration
  swaps and ordinary local entry values in cases zero, one and three do not
  improve it. Motion/state setup and packed playback lifetimes remain.
- 081B3AAC: improved drafted 88.6% to 90.3%, 49 / 23 reduced to 38 / 13.
  Removing SetState's r1 pin and value barrier improves state transitions.
  Byte-returning motion completion and typed base-position field produce the
  same baseline. Remaining entry/state pointer lifetimes and allocation.
- 081B3DB8: retained drafted 87.4%, 56 / 14. Removing the r10 state pointer
  pin or swapping vector-add operands improves fuzzy similarity but worsens
  structural differences, so both reverted. Radial-loop and decoder lifetimes
  remain.
- 081B4040: drafted 80.1%, 93 / 26, versus baseline 81.0%, 92 / 27.
  Explicit phase accumulator incremented by 255 improves structure by one
  instruction; retained per BIG_FUNCTIONS structural-first guidance. Combining
  flags worsens both scores; reverted. Radial-loop and decoder allocation remain.

Formatted using pinned clang-format, restored exact alias spacing, and recorded
all six scores with check.py --nonmatching --record (explicit reviewed extents
1632, 1240, 1692, 780, 648 and 732 bytes respectively). Default assembly paths
also pass; those are not C matches. Ran depin.py --write on unguarded scratch
copies to avoid testing fallback assembly: all six correctly skipped because
they are not exact. No claim that remaining hints are minimal. Only fast local
checks; no make, full build, network or generated-artifact commits. Queued one
commit per draft through build/commit.sh, notes last.

# Task 08214600 (2026-10-20)

Tags: #matching #sprites #video #palette

Offline worker pass on the preclaimed branch. Exact gain: +0.0197 percentage
points of code (476 bytes), in the Register-pinned category. No full build or
network operations; acceptance used tools/check.py only.

- **08214B44 — matched, Sprite_RenderScaledList.** Started from the checked-in
  Sprite_RenderUnflippedList sibling (08214FBC), removed its resource-list flag
  guard, and added signed scale-byte multiplication and flip-aware bounds.
  The resource width/height are unsigned bytes; origin offsets and the sprite's
  scale bytes are signed. Scaling uses arithmetic >> 6, while projection uses
  sign-symmetric truncation. Pinning only the resource pointer to r6 resolved
  the pointer allocation swap. Moving z to function scope before the skip-mask
  initialization fixed the final six differences (z/skip stack slots exchanged).
  depin.py confirmed the single pin is required. clang-format followed by
  check.py reported MATCH, 0 differing bytes.
- **08214E3C — improved existing draft, 95.3% (was 89.1%).** Added the required
  local Sprite_RenderFlipAwareList alias. Pinning height to r1 removed the extra
  move; pinning top to r4 and an intermediate horizontal edge to r0 improved
  operand selection. Final check: 9 differing instructions, 0 ignoring registers,
  10 differing bytes. Remaining differences are projection source r8 versus ip,
  vertical-bound operands and the last bounds-sum result register. Pointer pinning,
  local ordering, explicit flags, and vertical edge temporaries did not improve
  the retained candidate. Kept the existing guarded draft with recorded score.
- **082154E4 — retained existing draft, 82.6%.** Added the required local
  SpriteOam_FinalizeQueue alias. Read its depth-bucket chaining, per-priority
  output offsets, unused-entry clearing and affine matrix emission. Tried a
  descending four-element priority-pointer loop, pinning the outer index/buffer,
  and declaration swaps. None improved structural alignment; retained the original
  candidate: 41 differing instructions, 12 ignoring registers, 88 differing bytes.
- **08217430 — stopped below draft threshold, 21.0%.** Scratch candidate is
  wip/sub_08217430.c (not production or committed). Reconstructed palette-ID skip
  groups, full-brightness bank copies, black fades, colored fades and mode-1
  solid-color replacement. The RGB555 arithmetic separates 0x7c1f and 0x3e0,
  weights by level >> 1 and 32 minus that level, masks products then shifts 5.
  The helper-based reconstruction differs substantially in branch layout and
  allocation: 858 differing instructions, 753 ignoring registers. Needs direct
  per-path skip tests matching the ROM's repeated loads before allocation work.
- **08216584 — inspected, no completed candidate.** BgGfx_LoadTilemap uses
  0x30-byte layer records, stores current tile/scroll coordinates, and returns
  early on unchanged coordinates. It has separate map formats plus full-window
  and small-delta paths; metatile expansion writes paired rows into a 32-column
  wrapping destination. The blocked-source lookup and incremental edge paths
  remain to be reconstructed. Ghidra export is absent and relatives.py did not
  resolve the address by numeric name; no source was promoted.
- **08214600 — inspected, no completed candidate.** The renderer called by
  08214B44 clamps half-depth to [-128,127], prepares OAM flags, allocates affine
  matrix records, transforms/culls resource pieces into two 32-word stack arrays,
  then allocates tiles/palette and emits either immediate or depth-bucketed OAM.
  Mixed flip/scale/rotation paths and their stack lifetimes remain unresolved.
  condense.py found no repeated runs to fold. No incomplete C was promoted.

Tool caveat: do not run depin.py directly on guarded NONMATCHING source: its
checks see the default INCLUDE_ASM branch and falsely accept pin deletions.
This pass caught that after queued draft commits, restored all required pins,
reran --nonmatching --record (95.3% / 82.6%), and committed the corrections.
The exact 08214B44 check and depin pass were on unguarded C and are unaffected.


## Task 081314E8 follow-up — 2026-10-20

Tags: #matching #scene-actor #reaction #position-effects #register-pinning

Exact progress: 0.0 percentage-point gain. Restored required file-local aliases
in the four existing guarded drafts, using current functions.csv names. Explicit
address symbols in INCLUDE_ASM preserve the default fallback; local aliases for
08131D5C and 08134398 also resolve the older ELF names during 08135F70 checks.

- 081363F4: inspected largest first with asmat, relatives and condense, and read
  BIG_FUNCTIONS plus previous attempts. No complete close candidate retained.
  Linked-area-damage path resets group+0x4C timers and releases actor+0x45C;
  pending-hit path calls destination jump and escalating-hit logic, then clears
  timers and updates collider bit 0x100. The distinct cleanup paths and shared
  hit decoder still require dedicated reconstruction; prior 64.7% attempt is
  unsuitable for allocation-only tuning.
- 08135F70: retained draft 88.2%, 73 realigned differences / 18 ignoring
  registers. Adjacent declaration swaps did not change output. Health/flags
  pointer spills and shared hit decoding remain. Restored ResolveDamage name
  from symbols/functions.csv and address aliases for unresolved local callees.
- 08132904: retained draft 87.1%, 96 realigned differences / 24 ignoring
  registers. Collider r8 pin and uncached damage-display pointer regressed;
  unpinning position/max-health and declaration swaps did nothing. Replacing
  the halfword damage reload with gain removed one instruction difference but
  bypasses narrowing, so rejected. No speculative semantic change retained.
- 08131D5C: retained draft 87.5%, 91 realigned differences / 25 ignoring
  registers. Tested the same collider, damage and pointer variants; no useful
  improvement. Existing RNG scheduling, pointer spills and branch layout remain.
- 081314E8: read full assembly and matched relatives (Dog_ChasePlayer explains
  51%). Entry clears arrived, restores saved target, clears flags and sets
  duration 64. A nearby linked target within squared XZ distance 65534 triggers
  control kind 25; at tick 30 invokes a group callback, after tick 60 attempts
  the saved nav route and invokes fallback callback on failure. No complete
  close port produced; stopped without a draft.
- 08138778: improved draft 90.6% to 96.8%, seven realigned differences / four
  ignoring registers. variants.py isolated initialization order, base-before-
  offset increment order, a pinned stop-output address and pinned first flag
  write address. Remaining sprite-pointer lifetime lets agbcc hoist constant 8
  into r7, changing first clear and later constant uses. Further pointer-barrier
  variants did not improve. Best candidate retained and formatted; default
  assembly fallback is 380 bytes and passes exact check.

Four separate draft commits through build/commit.sh; all scores refreshed with
check.py --nonmatching --record, and all four default fallbacks checked exact.
No full build or network operations. Ghidra export is absent. Integration must
validate the combined ROM; retained drafts contribute no exact progress.

## Task 0821C8F8 follow-up — 2026-10-20

Tags: #matching #terrain #path

- 0821C8F8: stopped unmatched after largest-first entry, relatives and condensed
  listing review. Best relative covers 7%; direction-specific paired probes and
  ten stack positions remain unreconstructed. No new complete candidate.
- 0821F2EC: stopped unmatched after entry, relatives and condensed listing review.
  Caches destination X/Z at context+0xC/+0x10 and source X/Z at +0x14/+0x18;
  identical endpoints return 2. Scans navmesh groups/regions with
  NavMesh_IsPointInsideRegion, rejects group changes, and handles region transitions.
  Best matched relative covers only 1%; no close complete candidate.
- 0821D178: retained scratch candidate, 62.8% over reviewed 888-byte extent
  (0821D178–0821D4F0), 189 realigned differences / 82 ignoring registers.
  Packed clipping intermediates and intersection/control flow remain unresolved.
- 0821D6F4: retained 97.7% guarded Terrain_HasHigherCorner draft, restored required
  file-local address alias and explicit-address assembly fallback. Eight realigned
  differences / six ignoring registers. Bitfield source, wider load, pinned mask
  and shift extraction all failed to recover ldrh plus mask for low Z.
- 0821EC84: matched Path_FindNearestDistantPoint, reviewed 348 bytes. Selects the
  nearest point whose squared XZ distance is at least the supplied minimum,
  defaulting output XZ to point zero; obtains output Y from terrain slope data.
  Assign best=chosen after initial point lookup to recover lifetime/spills;
  separate ordered zero stores recover failure paths. Read terrain byte twice
  through one data pointer to recover mask operand allocation. A temporary sl
  pin led to equality, then depin removed it; final formatted C matches without
  register concessions. Separate driver commit confirmed.
- 0821EF30: stopped with scratch candidate at wip/sub_0821EF30.c. Reviewed
  404-byte body scores 91.8%, 16 realigned differences / zero ignoring registers.
  Scans path points from a random starting index and retains the LAST point
  inside XZ bounds, then computes terrain Y. Positive bounds predicate and
  explicit random-table address arithmetic fix structure; pointer pins regress.
  Recorder includes the unrecognized dispatch routine at 0821F0C4–0821F15C,
  yielding 78.5%. A queued production draft was committed before this discrepancy
  was caught; a subsequent withdrawal removes it to honor the >=80% requirement.
  Do not count this as a retained draft or exact progress.

Only check.py per-file checks; no full build or network operations. Exact match
and existing draft update committed separately through driver; notes last.

## Task 081493E4 follow-up (2026-10-20)

Tags: #matching #player #animation

- 081493E4: matched Player_UpdateMovementPose, 408 bytes, register-pinned category. Consumes entry, derives heading/side, clears status bit one, steps auxiliary pose sequence two, then returns to standing when byte 0x5BB clears. Ported Step from 0814957C; removing the command r7 pin eliminated five pointer copies and a missing saved-register problem. Explicit r2 offset/r0 latch address pins closed the final two instructions. depin removed one unnecessary hint (15 to 14); formatted source passes exact check.
- 0814957C: improved existing draft 93.7% to 97.6%. Pinned index offset r0 and table r1 before forming command r6, at both decoder sites. Eight realigned differing instructions / six ignoring registers remain: facing destination calculation order and final movement-state address. Explicit facing destination pins caused reloads and regressed; reverted. Restored required local alias and address-form assembly fallback.
- 08149720: retained drafted 92.5%, 35 realigned differences / 14 ignoring registers. Existing phase recovery/decoder candidate still has selection/control and allocation differences. Restored required alias and address-form fallback; no new complete hypothesis.
- 08149A14: retained drafted 97.3%, 13 realigned differences / zero ignoring registers. Residue is strength/approach address and register allocation. Pinning strength offset r2 and pointer r3 changed earlier allocation and introduced reloads (90.1%); reverted. Restored alias/fallback.
- 08149DF0: retained drafted 99.3%, two realigned differences / zero ignoring registers. Duration offset needs r3 instead of r0. Explicit offset r3 pin regressed to 93.2% by disturbing earlier allocation and constant reuse; reverted. Restored alias/fallback.
- 0814C3A4: stopped unmatched after largest-first assembly/relatives/condense inspection and prior-note review. Eight-phase grab-motion routine; strongest relative covers only 9%, mostly trigonometry. Condense leaves 973 instructions unfolded. No Ghidra export available. No complete candidate produced; assembly intact.

All four guarded drafts were formatted, rescored with --nonmatching --record and their default assembly fallbacks checked exact. One exact match; percentage-point gain deferred to integration report (408 exact bytes). No full build, make, network or generated-file commits. Function commits and this note use build/commit.sh.

## 2026-10-20 — task 080F421C, sword return and initialization

Tags: #matching #sword #register-allocation

- **080F5170 matched — Sword_DecelerateReturn.** Reads the owner at component
  offset 0x160, advances rotation and horizontal position, and waits for the
  owner's 0xD80 low bits to clear before decelerating. When stopped, records
  the return displacement and chooses 080F4FA8 or 080F508C by the larger absolute
  horizontal delta. A typed local component layout plus a small signed-halving
  inline produced the exact 336-byte function. Direct assignments to the s16
  fields selected ldrh/shift sequences; the s32-returning inline produced the
  original ldrsh/branch shape. No register pins are needed.
- **080F645C drafted 97.6% — Sword_UpdateDamage.** Retained and improved the
  existing draft. Passing both health and pending-damage addresses into an
  inline subtraction fixes load scheduling. Seven realigned instruction
  differences remain, zero ignoring registers, all around the saturated
  defeat counter at global offset 0x538. Explicit counter pointer/value and
  offset pins changed selection or worsened allocation; restored the best
  candidate. Added the required local alias for the existing symbol name.
- **080F52C0 drafted 95.5% — Sword_BeginTargetOrbit.** Retained the existing
  draft and added its local alias. Writing RNG lookup address arithmetic with
  the shifted index first reduces structural differences from four to two:
  six realigned instruction differences remain. A forced negative-direction
  temporary and direction register did not improve it. Remaining differences
  concern RNG table register choice and the -1 temporary/copy.
- **080F6F90 drafted 84.8% — Sword_InitAttackComponents.** Retained and improved
  the existing initializer draft, adding its local alias and tentative name
  evidence. An index-first command address and a volatile empty mask barrier
  reduce structural differences from 26 to 15 (115 total realigned differences).
  The remaining problems include animation mask/copy lifetime, two hoisted
  collider offset-vector addresses, and register allocation. Changing the
  command base back to the passed pointer slightly worsened structure.
- **080F421C stopped at 70.4%, no production draft.** New candidate retained
  in wip/sub_080F421C.c. Tentative Sword_UpdateDefeat: disables child colliders,
  alternates display visibility, emits rotating five-position death particles,
  disables the shadow, and invokes an optional defeat script with eight
  arguments. Typed child fields and an inline script-argument writer improved
  the first 69.9% candidate only slightly. Still differs in loop strength
  reduction, field-address lifetimes, stack layout, and allocation; below the
  80% production-draft threshold.
- **080F67AC stopped, no candidate.** Inspected entry, condensed listing and
  matched relatives before the smaller functions. The 846-instruction sword
  initializer combines repeated packed animation playback with script-option
  parsing and multiple collider setups. Closest relatives explain only about
  24%, primarily animation playback, without a coherent whole-function port.
  Ghidra export is absent (gen/ghidra/decomp.c), so no pseudo-C starting point.

Validation used only tools/check.py; no full build, network or shared generated
file writes. Existing guarded drafts remain guarded. tools/depin.py --write
on guarded NONMATCHING sources incorrectly accepted removals against the default
INCLUDE_ASM branches; restored all candidates from local checkpoints and reran
--nonmatching --record. Depin needs an explicit nonmatching mode before use on
these files. One exact function (336 bytes) is new coverage; drafts earn none.

## 2026-10-20 — task 080F79A8, axe attack phases

Tags: #matching #axe #animation

- 080F79A8: drafted 90.1%, Axe_UpdateOrbitAttack. Existing 87.6% draft
  improved by making the spin direction signed and reversing the Y-add operand
  order in the vector inline. Final realigned count 96 / 63 ignoring registers.
  Orbit initialization, RNG comparison, address reuse, and collider-loop
  scheduling still differ. Restored required local alias and address fallback.
- 080F8648: retained drafted 99.2%, Axe_UpdateReturningTrail. Six realigned
  differences / four ignoring registers remain: player-global scheduling and
  stack-vector address reuse in the return-distance test. A distance inline
  regressed to 97.5%; moving both vectors into one function-scope local regressed
  to 95.8%. Both reverted. Added required local alias and address fallback.
- 080F8FFC: drafted 84.5%, tentative Axe_UpdateTargetLunge. Entry computes
  player displacement, divides XZ by 44 or 64 according to health, sets the
  vertical duration field, configures auxiliary rendering, and starts sound.
  Rotation accelerates on ten-frame intervals then brakes according to angle.
  Landing flag selects FinishRotation and clears the context attack flag.
  72 realigned differences / 34 ignoring registers remain, including stack
  vector address lifetimes, flag scheduling and callback setup allocation.
- 080F93E0: drafted 83.0%, tentative Axe_UpdateTurnAnimation. Compares octants,
  restores the saved callback when aligned, selects turn polarity, maintains
  duration scale, and rotates one octant on animation event bit four. Shared
  animation Select inline ported from Lun_UpdateWaitingAnimation. 66 realigned
  differences / 28 ignoring registers remain: octant arithmetic expression,
  flag/address scheduling, constant reuse and allocation.
- 080F9694: matched Axe_UpdateWaitingAnimation. relatives.py found that
  Lun_UpdateWaitingAnimation covers 150 of 151 instructions. Porting its C and
  substituting Axe_GetDirectionalFrame gives an exact unpinned match (336 bytes).
  Wait sets flag 0x20 through frame 29 and marks completion after frame 59.
- 080F97E4: drafted 81.0%, tentative Axe_ConvergeRecoveryMotion. Reused the
  quarter-delta/clamp/rest idioms from SceneActorControl_ConvergeSavedMotion.
  Approaches saved XZ motion with steps clamped to +/-200, then restores flags,
  copies the movement limit and requests behavior nine. 70 realigned differences
  / 30 ignoring registers remain. Explicit mask locals reduced structural
  differences but regressed fuzzy to 74.2%; reverted. Corrected profile-entry
  callee from the directional-entry routine during review.

All five drafts are guarded and scored with --nonmatching --record; their
assembly fallbacks pass exact checks. One match and five separate draft commits
queued through build/commit.sh. No full build or network operations. Exact
percentage-point gain is deferred to batch integration; drafts earn none.

## 2026-10-20 — task 08142194, player interactions and tether movement

Tags: #matching #player #tether #terrain

- 08142194: retained guarded Player_UpdateObjectPush draft at 92.9%.
  Added the required local alias and address-named assembly fallback, allowing
  checks against this worktree's ELF. Reassociated the direction reflection as
  ((target - direction) * 2 + 8) + direction: structural differences fell from
  33 to 29; total remains 84 realigned differences. Sine-table pointer lifetimes,
  cell-address reuse, prologue allocation and flag narrowing remain off. Existing
  one-argument ActorVariant_GetTableValue call corresponds to a ROM call with no
  explicit r1 setup; its ABI interpretation still needs investigation.
- 08143ED0: drafted 93.9%, tentative Player_UpdateTetherMovement. Selects idle
  or moving tether animation, applies signed-truncated directional velocity and
  axis scaling, emits terrain/action-dependent step cues, then constrains the
  tether target. Reused velocity idioms from Player_SetDirectionalVelocity and
  cue interpretation from Player_PlayActionCueSound. 38 realigned differences,
  27 ignoring registers: repeated action-flag address/load materialization,
  boolean conversion of global bit zero, effect-call zero scheduling, allocation.
- 08143B54: drafted 89.0%, tentative Player_ConstrainTetherTarget. Limits target
  radius around a 70-unit directional offset, blends target 3:1 toward the
  constrained point, tests terrain, clips to cell edges, then sets target facing.
  Pins actor/offsets to r6/r7/r8 (marked concession) improved initial 86.5% to
  89.0%; still 47 realigned differences, 12 ignoring registers. Field-address
  materialization, offset lifetimes and allocation remain. Duplicated height
  paths preserve the ROM's shared tail shape.
- 081432DC: drafted 88.6%, tentative Player_UpdateHeldInteraction. Approaches
  saved position and facing, then starts/holds/releases an action while checking
  capacity and the global resource at 0x77C. An initial 79.1% candidate improved
  by assigning the shared zero after the arrival-distance comparison and using
  explicit cue/holding labels. 57 realigned differences, 33 ignoring registers:
  facing-address scheduling, shared cue block layout and allocation remain.
- 08143604: drafted 92.1%, tentative Player_UpdateEnergyBurst. Entry tests
  energy 100 and the global burst latch, prepares one of two effect origins,
  grows an effect parameter to 32, and advances the held/recovery animations.
  32 realigned differences, six ignoring registers remain, primarily constant
  copies for the effect fields and register scheduling. No pins.
- 081439D8: matched Player_TestTetherTarget, unpinned. Terrain properties are
  read through the override lookup twice; rejects blocked attributes and height
  differences above 255. Flag 0x800 snaps the candidate to the tether actor and
  cancels the sequence when actor separation exceeds 255 on either horizontal
  axis. Adapted the terrain lookup idiom from TetherActor_TestStepPosition.
  First candidate differed by one subtraction operand order; height minus actor
  Y matches exactly (380 bytes).

All five drafts are guarded, recorded with --nonmatching --record, and have
exact assembly fallbacks checked separately. One exact match and five drafts
queued individually through build/commit.sh. No full build or network used.
Drafts contribute no exact progress; percentage-point gain is deferred to batch
integration. Ghidra export is absent in this checkout.


## Task 0812DA5C — 2026-10-20

Tags: #matching #scene-actors #terrain

- **0812DA5C — matched.** `SceneActorTerrain_CheckNeighborHeight` checks the selected cardinal neighbor against `position->y + 255`, with unsigned height comparison. Reused `scene_actor_terrain.h` center and height inlines from the matched GetNeighborCell relative. Duplicate center calls within the switch are needed for the ROM layout. An explicit `if (height > ceiling) return 0; return 1;` reproduces the final branch order; the boolean expression and opposite polarity do not. Fast exact check: 100%. Register-pinning concession marked because the shared height inline pins its attributes operand. Commit ed7a04379.
- **0812FA0C — retained existing draft 84.8%, no score gain.** Added the required file-local alias for the existing symbol name so the checker resolves it against the local ELF. Baseline realigned count: 165 total / 40 structural. Tested prior-direction unpinning, random result narrowing, signed height result, and widening motion flag result. None improved structural count; retained original C. Widening motion flags improved fuzzy to 87.2% but worsened structural count to 41, so rejected. Re-recorded 84.8%.
- **0812E7EC — inspected, left assembly.** Condensed listing has repeated state/control transitions interspersed with attachment descriptor operations, resource creation/release, pending-trigger consumption and scene-message trigger delivery. Largest matched relative explains only 26%; it is not a simple route-state clone. No complete C candidate produced. Ghidra export is unavailable (`gen/ghidra/decomp.c` missing).
- **0812F5D4 — retained existing draft 81.1%, no score gain.** Added required local alias; reproduced 80 total / 23 structural differences. Tested unpinning retry counter, wrap, mask and prior direction, plus widening state and control arguments. No improvement; retained original C and re-recorded score.
- **0812F3CC — unsuccessful draft, left assembly.** Reads motion flags, requires collision flags/contact availability, selects contact via SceneActorContacts_SelectForActor, clears reaction fields, installs idle control and state 3/phase 1, and sets flags2D6 bit 0x800. Two C revisions plus pinned/local-offset initialization reached only 54.7% fuzzy (95 total / 42 structural differences). Rejected below the draft threshold and removed scratch source. Main obstacles are inline control setup, address materialization and register allocation; a shared fail label before success reproduces the early return layout better than direct returns.
- **0812E118 — inspected, left assembly.** Terrain transition check compares slope kinds, adjacency and height, rejecting cell flags 0x43. Notable obstacle: the ROM computes slope-adjusted height through a stack position before GetCenter populates that position; slope cases read stack bytes before those stores. Do not silently reorder this into an initialized-center implementation and call it equivalent. No accepted candidate produced.

Only per-file check.py checks were run; no full build or network operations. Exact gain is one function (460 bytes); aggregate percentage reporting remains with batch integration. The two route draft commits only repair required aliases and preserve existing scores.

## Task 081F2A08 — 2026-10-20

Tags: #matching #ratatosk #sprite-trail #register-pinning

Worked offline on the preclaimed branch. Only per-file `tools/check.py` checks were run; no full build or network operations.

- **081F2A08 — matched**, `SpriteTrail_CreateMotionEffect`. Allocates a motion-effect slot, copies start/end positions, sets velocity to `(end - start) >> 4`, initializes four history/head sprites and three tail sprites, selects palette 485, initializes both colliders and marks the pool slot occupied. Local layouts establish the 0x258-byte entry's position at 4, history at 0x14, velocity at 0x54, target at 0x64, head sprites at 0x108, tail sprites at 0x1B8 and animation at 0x23C. Descending counters decrement before advancing sprite pointers. Explicit y/z load temporaries preserve load order while adding z + y. Depinning reduced three hints to two: position in r5 and its saved copy in sl. Marked Register-pinned. Formatted source rechecked exact (360 bytes).
- **081F5910 — matched**, `Ratatosk_UpdateReactionAnimation` (tentative). Reaction mode at 0xE94 chooses animations 0x21–0x23 and completion timings. When locked, restores facing from signed angle at 0x114. Separate mode checks and the shared `other` label reproduce the original control flow directly. Local typed fields and the existing facing idiom matched on the first check. Formatted source rechecked exact (340 bytes), no register pins.
- **081F4378 — drafted 80.8%**, existing name `Ratatosk_UpdateProjectileVolley`. Five-state attack waits at frames 10, 34, 37 and 43; aims toward the player with a facing-relative angular clamp; spawns three projectiles at 16-angle increments using resource 0x8E5A and procedure 0x4363. State is at 0x17C, cooldown at 0xE74. A pointer-based vector-sum inline removes large stack halfword merge sequences. Pinning the spawn-position pointer in r8 raised the ordinary candidate from 79.9% to 80.4%; the reviewed-extent NONMATCHING recording is 80.8%, 73 realigned differences / 20 ignoring registers. Remaining issues are duplicate actor-pointer allocation, polar temporaries and instruction order. `depin.py --write` was invoked; it skips nonexact inputs. One retained register pin, concession marked. Default source retains INCLUDE_ASM.
- **081F3C68 — stopped below draft threshold**, left assembly. Reconstructed both branches: ring of homing projectiles (mode 3) versus player clones, centered on the player's effective position with a 512-unit polar offset. Positions clamp x to 0x424..0xCDA and z to 0x528..0xBD6. Ring counts at 0xEDA/0xEDC, collider parameters from 0xEB4 onward. Separate branch-local angle/index lifetimes and the typed facing idiom lowered structural differences from 75 to 49; a typed boss parameter view reached 41, but total/register differences remained substantial (best fuzzy 67.9%). Scratch reconstruction in `wip/sub_081F3C68.c`; not committed as a draft.
- **081F4CF8 — stopped below draft threshold**, left assembly. Defeat callback kills an auxiliary task and starts event script, fades sun orbs, checks terrain flags and steers toward the stored position at 0x3CC while its timer remains active. Emits particles through frame 150; at frame 440 runs completion script, sets event flag 81 and updates saturating game counters. Initial reconstruction 67.8%, 193 realigned / 92 structural differences. Pinning the actor in r8 raised fuzzy to 71.0% but worsened structure to 104; restored the ordinary candidate. Remaining work: pointer lifetimes/address order and particle-loop allocation. Scratch in `wip/sub_081F4CF8.c`; not committed.
- **081F5290 — stopped below draft threshold**, left assembly. Hit recoil sets animation 29, decays motion halfwords by 24, emits particles on odd frames, and selects defeat or directional idle after frame 29 according to health and lock state. Initial reconstruction 45.5%; preserving the signed saved-angle addition until polar lookup and expressing the sprite-relative velocity improved it to 58.4%, 208 realigned / 72 structural differences. Remaining issues include merged polar tails, persistent sprite pointer, state-transition store order and particle register allocation. Scratch in `wip/sub_081F5290.c`; not committed.

Accepted exact gain: two functions, 700 bytes; 360 Register-pinned and 340 unpinned. Aggregate progress regeneration and complete ROM validation remain with batch integration. No other worker's candidate sources were read.

## Task 081D7C68 — 2026-10-20

Tags: #matching #projectiles #terrain #register-pinning

- 081D9470 matched: Task4E01_StartImpactAnimation. Looks up resource 0x922E/0x32D7, selects sequence 0 or 5 from flag 0x10, advances playback, sets size state to 8 and installs 081D95F0. Exact checker accepted 384 bytes. Register-pinned category.
- 081D95F0 drafted 82.3%: Task4E01_UpdateExpandingImpact. Clamps a decremented timer to 4, updates visibility flags, advances animation, selects sequence 4 once, expands collision every fifteenth tick through tick 44, then switches callback or removes the projectile. Registers, flag branch arrangement and collision-field copies remain different. The collision-size setter consumes four arguments; both horizontal dimensions use the expanding size.
- 081D7C68 drafted 82.2%: Task4E01_StartTerminalAnimation. Sets flag 0x1000, clears timers and selects terminal callbacks by projectile type; type 3 selects sequence 1 or 6 and advances playback. Switch layout and register allocation remain different.
- 081D7F8C drafted 84.8%: Task4E01_ResolveTerrainDirection. Tests adjacent terrain heights to resolve even direction values 0/2/4/6. Height comparisons are unsigned despite signed halfword loads. Cell-index register reuse, boolean placement and branch layout remain different.
- 081D8244 drafted 85.5%: Task4E01_SelectTerrainTurn. Odd directions reverse directly. Even directions use angle from the current cell center, adjacency to the other point, and prospective terrain height to choose a turn. Center-vector stack packing, branch sharing and register choices remain different.
- 081D9904 drafted 89.0%: Task4E01_SpawnImpactChildren. Unless flag 0x10 is set, creates three child projectiles with randomized directions and speed offsets, propagates damage/type/sprite settings, then advances animation and removes the owner. Parameter flag operations and saved-pointer lifetimes remain different. Child parameter storage is 0x58 bytes; owner-config pointer is at offset 0x54.

All descriptive interpretations are provisional. Neighbouring Task4E01 callback names establish the subsystem prefix; Projectile_* callees establish the parameter API. No shared layout/header changes were made.

Technique: reuse the packed-animation stepper from 081ED5F4. Pinning duration to r2, flags to r1 and the 0xFFFF mask to r5, with an empty constraint preserving the mask copy, made 081D9470 exact. Separate stepper variants prevent the compiler merging branches in 081D7C68. Volatile halfword random-table reads preserve the loads in 081D9904; explicit signed divide-by-two branches match its truncation idiom better than C division. Ghidra export is absent. Retained drafts have useful structure but further manual changes lacked a clear exact-match hypothesis.

Validation used only tools/check.py; all five guarded draft scores were recorded with --nonmatching --record. No full build or network operation was run. One exact function and five drafts; drafts receive no exact credit.

## Task 0809628C — 2026-10-20

Tags: #matching #ghost #nonmatching

Exact progress: +0 percentage points. No exact matches. Two guarded drafts checked with `tools/check.py --nonmatching --record`; neither counts as exact progress. No full build or network operations.

- 0809679C — drafted 97.6%, `Ghost_DecelerateMovement` (tentative). Four differing instructions, zero ignoring registers, five differing bytes in the reviewed 340-byte extent. Adds context velocity at 0x5FC/0x600 to actor horizontal motion, halves both signed components every four ticks, then chooses idle or flight state when control completes. Remaining differences are registers for the 0x5FC address and existing motion.x load. Register-pinning concession marked; `depin.py --write` declines a candidate that is not exact. Explicit context component pointer locals preserve address reuse across Mod; returning the flags mask through a u16 inline reproduces the narrowing. File-local alias retained.
- 08096AC8 — drafted 98.0%, `Ghost_RecoilTowardEndpoint` (tentative). Nine differing instructions, two ignoring registers, ten differing bytes in the reviewed 624-byte extent. Random angle drives two recoil components (0x2EC/0x2F0); after action completion it clears recoil and resumes movement toward a terrain-adjusted path endpoint. Remaining differences are initial zero placement and registers for RNG-table / velocity-destination addresses. A `Clear(destination, &zero)` inline lets destination computation precede the zero store while sharing one stack slot across both CpuSet sites. Moving the path advance call outside Endpoint and retaining its path pointer removes premature stack-address materialization. No register hints in this draft.
- 08096D38 — stopped below threshold, best 76.7% fuzzy, 195 realigned differences / 96 ignoring registers. Reconstructed accelerating flight along nine points, with context flag 4 selecting a retreat destination. Remaining differences include PRE pointer lifetimes, RNG-table register reuse, control setup and vector inline ordering. Fixed-register experiments worsened the realigned count; best unpinned attempt retained locally in wip/sub_08096D38.c. Follow docs/BIG_FUNCTIONS.md and use realigned count; the closest matched relative is Axe_FollowAcceleratingRoute (40% shared instruction coverage).
- 080964C4 — stopped below threshold, 69.6% fuzzy, 142 realigned differences / 102 ignoring registers. Reconstructed destination approach, duration scaling, sway, and next-path-point terrain height. Target destination is context + 0x5D4 (context + 0x5C8, then position + 0xC). Main residue is approach-vector evaluation and endpoint/address ordering. Local candidate only.
- 080968F0 — stopped below threshold, best 74.2% fuzzy, 68 realigned differences / 24 ignoring registers. Waits more than 60 signed counter ticks, restores path endpoint, sets facing from the delta, and clears stored velocity. Signed comparison matters. Endpoint helper call/pointer lifetimes and stack ordering remain unresolved; register-pinning experiment worsened the result. Local candidate only.
- 0809628C — stopped below threshold, 61.7% latest fuzzy, 124 realigned differences / 50 ignoring registers. Defeat animation/effects, terrain script, death script, event flag and idle transition recovered. Important ABI detail: sub_0800AFE8 has seven arguments here: count, zero, context byte 0x5BC, actor position, three-position array, 300, 120. Initial six-argument interpretation was corrected. Stack declaration order, control inlines and register allocation remain unresolved. Local candidate only.

These functions belong to the Ghost neighborhood (Ghost_* named neighbors/callees, including Ghost_SetMovementEndpoints and Ghost_StartMovementToPosition). Ghidra export is absent (`gen/ghidra/decomp.c`). All tentative interpretations remain provisional. Per-file checks only; shared docs and generated assets untouched.

## 2026-10-20 — task 0818F6A4

Tags: #matching #nidhoggr #scene-effects #animation #register-pinning

Worked offline on the preclaimed branch; per-file checks only, no full build or network operations. One exact function, one guarded draft, four stopped below the draft threshold. Ghidra export is unavailable. All matches/drafts have file-local descriptive aliases.

- **0818FC00 — matched**, `Nidhoggr_UpdateDetachedAttackEffect`. Steps the sprite animation, then detaches its anchor on animation completion, sets horizontal velocity to 12 and enables its attack collider. After moving more than 0x1000 from the saved x coordinate, invokes its destructor and disables the collider. Reused the matched Screamer animation-step idiom with the observed effect layout. The initial check was 98.5% with three register-only differences; a scoped 0x180 offset in r2 plus an empty constraint reproduced the state-byte address calculation exactly. `depin.py --write` retained all 12 hints. Formatted source rechecked byte-identical (412 bytes); Register-pinned category. Commit 12bc72ace.
- **0818F6A4 — drafted 87.1%**, `Nidhoggr_UpdateFallingAttackEffect` (tentative). Emits palette-426 effects every fourth age tick, accelerates upward, retargets x/z to the player after rising 0x1000, samples terrain height, enables the shadow and attack collider, then emits an impact effect during descent. State 2 falls through to the impact-height check even when the sound threshold is not met. State 255 creates an impact immediately. Animation prefix matched exactly during iteration; remaining differences concern state/age address allocation, coordinate lifetimes, terrain setup and dispatch layout. Recorded reviewed extent: 72 realigned differences, 35 ignoring registers. `depin.py --write` was invoked and skipped this nonexact candidate. Default compilation retains INCLUDE_ASM; no exact credit. Commit 22aa70356.
- **0818FEE4 — stopped**, assembly retained. Player-seeking attack effect, sharing the animation prefix and most motion code with 08190798. State zero increases velocity.x to 8 and waits past age 30. Other active states compute horizontal angle toward the player, choose sine-derived motion using the 1024-entry random table, and add a second random direction. At age >88 in state >1, creates an offset impact and removes the body collider; nearby player bounds instead select state 255. Packed local vector fields reproduce word merges better than ordinary halfword locals, but branch placement and allocation still differ substantially. Best explored candidate about 54.9% fuzzy; scratch source remains in wip/sub_0818FEE4.c, not a production draft.
- **08190798 — stopped**, assembly retained. Near sibling of 0818FEE4; emits `ScenePanelEffect_CreatePositionEffect`, which reads its damage field at boss+0x53C8, and omits the collider-removal call. Shared source reconstruction stayed below threshold (about 54.3% in the packed-vector experiment); subsequent explicit guard/goto layout did not improve it. Scratch source in wip/sub_08190798.c.
- **0818FA54 — stopped**, assembly retained. Constructor for 0818F6A4, called from `SceneAnimation_UpdateSlotSequenceNineEffect`. Uses resource C7ED, palette 1A6, sequence 3, anchor and copied offset; installs Nidhoggr collision callbacks, two collider bounds/offset pairs, and a disabled small shadow. Unpinned retained reconstruction checks at 70.9%, 66 realigned differences / 21 ignoring registers. Register pin experiments did not yield a usable improvement and were discarded. Scratch source in wip/sub_0818FA54.c.
- **08190274 — stopped**, assembly retained. Constructor for 0818FEE4, called from `SceneAnimation_UpdateSlotSequenceSevenEffect`; sequence 0, palette 1A6, no retained anchor, distinct attack/body bounds, `SceneAnimation_OnEffectContact` on body collider, small shadow and sound 1B9. Retained ordinary candidate checks at 56.5%, 94 realigned differences / 27 ignoring registers. Pinning experiments produced suspicious stack-addressed effect stores and were rejected, not promoted. Scratch source in wip/sub_08190274.c.

Observed effect layout: sprite at +0x24, position +0x40, sequence +0x50, shadow +0x60, attack collider +0xCC, body collider +0x120, anchor +0x174, velocity/offset +0x178, state +0x180, age +0x181, saved coordinate/ground +0x182 and boss pointer +0x184. Do not shorten the sprite's trailing padding: sequence must begin at +0x50. Constructor body-property calls reuse r2=0x120 from calculating the body-collider address; the retained candidates pass that value explicitly. Shared playback was kept file-local in committed units, leaving no new shared header dependency.


## 2026-10-20 — task 0823AF44

Tags: #matching #player #actor-motion #animation #terrain #register-pinning

Offline worker task: two exact matches and four guarded drafts. Only tools/check.py per-file validation was run; no full build or network operation. Existing drafts were used where present. The cached ELF predates several names in symbols/functions.csv, so file-local aliases are needed for the checker, including the local Player_EmitSweepSpark callee alias.

- **0823AF44 — matched**, `KeyframeActor_InitAuxiliarySprite` (tentative ownership). Caller KeyframeActor_InitBodyFromScript initializes this display after the body. Loads animation data D495, sequence resource 922E/AF44, initializes the sprite at +14C, copies actor position into +168, clears +4E8 and advances the packed animation at +3FC. Reused SceneActors_UpdateDelayedMovement's animation step. Explicit unsigned source temporaries reproduce the halfword position copy; a scoped r0 y temporary closes its register swap. Sequence in r8 and render in r6 preserve the setup allocation. Exact 400 bytes; Register-pinned category.
- **0823EF58 — matched**, `Player_UpdateTetherRecall`. Existing state-machine reconstruction was sound. A register-pinned input-byte read in r0 prevents retaining the zero-valued test in a different register; a scoped r0 zero reproduces the fresh clear before transition to phase 6. Exact 496 bytes; Register-pinned category.
- **0823C930 — drafted 86.0%**, `Player_UpdateTerrainSidestep` (tentative). Reconstructed the three phases, coordinate-axis facing, adjacent-heading movement, repeated terrain-height probes and directional effect position. Adapted packed vectors and ground-height calculation from Player_TryStepForward. The first candidate was 85.1% under candidate-length scoring; recorded reviewed-extent score is 86.0%, with 154 realigned differing instructions / 104 ignoring registers. Substantial probe/instruction-order and allocation work remains; the ROM also contains unused trigonometric calculations in the effect tail that ordinary C eliminates. No Ghidra export is present. No exact credit.
- **0823F148 — drafted 98.8%**, `Player_UpdateChargedTetherStrike` (tentative). Existing draft improved from 96.0% by pinning the charge-duration value, input-byte read and fresh zero clear. Remaining 14 realigned differences / 8 ignoring registers involve the duration store address order and masks in the particle bitfield setup. Retained guarded; register-pinning concession marked. No exact credit.
- **0823DAB4 — drafted 99.0%**, `Player_UpdateSweepAction`. Existing draft improved from 97.4% by scoping the positive-branch sum and pinning its shared 0xFF mask in r0. Three realigned differences / two ignoring registers remain: second-branch sum lacks the ROM copy, causing alignment padding. An empty constraint fixes the structure but swaps actor and angle registers (18 register-only differences); pinning the angle changes earlier expression allocation. Kept the higher-scoring simpler draft. No exact credit.
- **0823B47C — drafted 99.4%**, `ActorMotion_ApplyHeadingDrift`. Existing draft retained with the required local alias and freshly recorded score. Still two realigned differences: facing store and register copy are in opposite order. Pinning the copied byte and inserting a constraint worsened selection and was discarded. No exact credit.

Useful technique: explicitly type copied halfwords unsigned when a widened signed temporary otherwise introduces ldrsh; use separate scoped temporaries to reproduce alternating load/store register choices. A fixed-register zero can prevent CSE from reusing a branch-proven zero without adding handwritten instructions. All six task functions are committed individually through build/commit.sh. Full combined ROM validation remains with the integrator.

## 2026-10-20 — task 080A71A0

Tags: #matching #tentacle-body #actor-motion #animation #defeat

Offline solo worker pass: two exact matches, three guarded drafts, one stopped attempt. Exact coverage of this assigned range increased from 0% to 37.8% (+37.8 percentage points; 1,512 bytes). No register-pinning concession is present in the accepted sources. Validation used tools/check.py only; no full build or network operations. The integrator must run the combined build.

- **080A73B4 — matched**, `TentacleBody_UpdateDefeat`. Uses TentacleBody_GetDirectionFrame and profile entry +44 for the entry animation, configures the body at +3D0, emits particles every ten frames, updates segment motion, waits for blinking, removes trail segments, runs terrain/death scripts and resets control/shadow state. Caller references occur in the actor setup around 080AC870. Ported Select from Ghost_UpdateEnergyDrainAnimation and the script/control tail from SceneActorControl_CompleteScriptedResponse. A scoped mask before the flags read and explicit table-address arithmetic/operand order close the final random-number differences. Exact 844 bytes.
- **080A7700 — drafted 93.9%**, `TentacleBody_ApproachWaypoint` (tentative). Initializes context +8D4/+8D8, invokes callbacks, approaches the position at context +904, computes a next heading, requests turning when its wrapped heading difference exceeds 32, then advances motion and oscillation. Remaining 20 realigned differences / 8 ignoring registers: initialization ordering, state reset addressing and allocation. Candidate size 752 bytes versus target extent 744. Source records the reviewed score; commit subject used the earlier 93.4% candidate score.
- **080A79E8 — matched**, `TentacleBody_UpdateRetreat` (tentative role). Initializes the quadrant opposite the actor's saved facing, approaches saved waypoints, updates next heading and oscillation, and transitions after frame 80. Bee_ApproachSavedPlayerPosition provides the approach/rounded-shift idiom. Computing the quadrant into a temporary before storing it prevents premature destination-address setup and fixes most allocation differences; a specialized state reset with a local behavior constant closes the last misplaced zero load. Exact 668 bytes.
- **080A7C84 — drafted 90.7%**, `TentacleBody_UpdatePursuit` (tentative). Related to 080A79E8, with saved-facing quadrant rounded by 32, an additional callback exit and a transition after frame 800. Remaining 35 realigned differences / 15 ignoring registers: callback exit tail merging, state reset addressing and allocation. Candidate size 736 bytes versus target extent 732. Commit subject used the earlier 90.4% candidate score; source has the reviewed score.
- **080A7F60 — drafted 86.1%**, `TentacleBody_UpdateSpiral` (tentative). Emits sound every 48 ticks, clears the motion block using CpuSet, decreases spiral speed through three time ranges, advances facing and planar motion, and restores saved facing before the next behavior. The signed wrapped-angle test deliberately has only an upper bound, as in the assembly. Remaining 47 realigned differences / 17 ignoring registers concern angle-test ordering, control initialization and allocation. Candidate size 480 bytes versus target extent 484. Commit subject used the earlier 85.6% candidate score; source has the reviewed score.
- **080A71A0 — stopped, not drafted**. Best candidate 73.5% (untracked wip/sub_080A71A0.c), below the retention threshold. On entry it removes excess trail segments using health divided by context +8EB; it then retracts paired coordinates toward their targets with signed quarter-distance steps clamped to 200, finishing in idle/control behavior 5. Assembly hoists eight field pointers across the entry branch, with high-register and stack lifetimes the candidate does not reproduce. Moving pointers out of the helper worsened the score to 70.9%; pinning two pointers and reordering stack locals worsened it further to 63.5%, so both were discarded. Best unpinned candidate restored and rechecked. No source committed and no exact credit.

All five accepted sources were committed separately through build/commit.sh. Drafts are guarded by NONMATCHING with INCLUDE_ASM fallbacks and scores recorded using --nonmatching --record. No Ghidra export was available. A full build was intentionally left to batch integration.


## 2026-10-20 — task 0819CFEC

Tags: #matching #hresvelgr #actor-state #terrain

Offline solo worker pass: four unpinned exact matches and two retained guarded drafts. Exact coverage of the six assigned reviewed extents increased from 0% to 44.8% (+44.8 percentage points; 1,864 bytes). Checked only with tools/check.py; no full build or network operation. Each accepted source has its own driver commit and file-local descriptive alias. Semantic names remain tentative.

- **0819DAB4 — matched**, `Hresvelgr_UpdateOpeningAttackCycle`, 656 bytes. Adapted Hresvelgr_UpdateAttackCycle at 0819DD44, which explains 86% of the instruction stream. Removes the sibling's global cycle counter and retry states, adds the player movement-lock test before requesting action 17, and finishes when height reaches 832 or the departure animation ends. The player-flags helper taking a mask reproduces loading the mask before reading player+20; direct expressions place that load too early.
- **0819CFEC — matched**, `Hresvelgr_TryStartReaction`, 420 bytes. Called by 0819E664. Prioritizes health-triggered defeat, flag 1 recovery, flag 10000000 timed pose, and flag 100000 interrupted motion. Saves state at 388, callback at 384, animation at 389 for the last reaction. Reused the state/callback helpers from Ratatosk_ApplyDamageReaction. Request and flag-test inline helpers restore constant-before-address ordering. Use named callback symbols for Hresvelgr_UpdateTimedPose and Hresvelgr_UpdateMotionPhase: the stale baseline's raw address aliases lose the Thumb bit in pointer relocations.
- **0819D4D8 — matched**, `Hresvelgr_UpdateTerrainManeuver`, 392 bytes. Nine-state request sequence: actions 24, optional 25 when Hresvelgr_HasAdjacentOverride succeeds, then 3, 4, 5, 6, 7 and 12 through the observed state transitions. Sets readiness byte 383 at completion. Shared Start/Request helper shape from the attack cycle matches without pinning.
- **0819D928 — matched**, `Hresvelgr_UpdateRepeatedCharge`, 396 bytes. Clears attempt counter 330; requests action 24, then up to four action-26 attempts, choosing action 25 for a cell override and action 2 when heading leaves the boundary arc. Action 12 finishes the sequence and sets readiness 383. Signed wrapped-heading locals preserve signed comparisons. A shared outside-arc label inside the inline boolean helper reproduces the ROM's common zero-result tail; separate returns introduce extra moves/branches. Returning s32 avoids byte-narrowing instructions.
- **0819FE9C — retained draft 86.1%**, `Hresvelgr_UpdatePlayerRelease`, 1,564 reviewed bytes. Restored the required local alias and recorded score. Still 123 realigned differing instructions / 35 ignoring registers, concerning release-target vector/address lifetimes and RNG allocation. No improvement or exact credit claimed.
- **081A0BC8 — retained draft 96.1%**, `Hresvelgr_UpdateBoundaryBarrage`, 732 reviewed bytes. Restored the required local alias and recorded score. Still 14 realigned register-only differences, chiefly RNG mask and signed remainder temporaries and terrain constant allocation. Existing register-pinning concession retained. No improvement or exact credit claimed.

No Ghidra export is available. Largest-first review identified existing drafts; relatives then supplied the exact shared state-machine scaffolding for the remaining functions. Combined ROM validation remains with the integrator.

Final default-path verification found the cached assembly files still use address names. Both retained drafts now use address-named INCLUDE_ASM fallbacks; each default path passed the exact check. These fallback fixes were queued separately after the draft commits.

## Task 080A4798 — 2026-10-20

Tags: #matching #tentacle #scene-actors

Exact progress gain: 0 percentage points; no byte-identical matches.

- 080A6D18: drafted 89.0%, TentacleBody_BeginDirectedAttack. Gets profile entry 0, aims the attack arc at the player on entry, sets direction from secondary motion, enables the attack flag, clears the response flag, updates directional animation and trail angular offsets, and transitions to recovery when flags2D0 bit 0x10 appears. Uses entry byte 0x38 and song 0x3D6. Remaining differences: register assignment and transition-store ordering, 41 realigned instructions / 21 ignoring registers. Pinning the body pointer to r5 improved 77.7% to 89.0%; pinning actor r6 made the candidate worse. Retained one marked register-pinning concession.
- 080A6F50: drafted 92.4%, TentacleBody_UpdateAttackRecovery. Uses entry byte 0x44, invokes the segment animation update with 18 and another update with (20, 25600), then after frame 40 selects entry byte 0x38 and marks completion. Remaining differences: inline Select restart/selection pointer copies and allocation, 27 realigned instructions / 18 ignoring registers. Profile-entry pointer pinned to r10. Full reviewed score differs from candidate-length scoring. Checker includes trailing entry at 080A7188 (flag-clearing leaf), beyond the main body's return at 080A7186; do not mistake matching the parent body for matching this retained tail.
- 080A69F0: stopped at 56.2% (earlier raw-pointer candidate 60.1%); retained locally in wip/sub_080A69F0.c. Body[0x875] encodes signed sway direction and magnitude; motion is sine-scaled with truncation toward zero, and the byte advances by 12. Iterates 296-byte segments, requiring flags bit 2 and rejecting bits 1/4 at body+0x4D9+i*296. Sets segment motion to actor motion divided by i+1, and places the first two eligible segments 54 and 20 units behind the actor; remaining eligible segments use actor position. Transitions to 080A6D18 after frame 60. Struct conversion changed strength reduction and address lifetimes without improving the candidate; do not continue blind allocation changes.
- 080A65A8: stopped at 51.7%; retained locally in wip/sub_080A65A8.c. Same sway/segment loop as 080A69F0, but completion threshold is body u16[0x8E6], setting actor+0x2E8 to 2 and clearing body+0x8D0 bit 2. Otherwise calls the common update with (64, 65535). Loop structure and address recomputation still differ.
- 080A62DC: stopped at 70.1%; retained locally in wip/sub_080A62DC.c. Response windup sets flags2D0 bit 0x100000, starts segment animation 18, clears secondary motion with CpuSet, and grows field2EA by 10 or 40 depending on counter2AC. Threshold is 200 or 300 based on body[0x8EA] > 3, then transitions to 080A65A8. Otherwise moves backward with speed 1 or 2 and performs the shared segment positioning. Strength-reduced loop and register lifetimes still differ (132 realigned / 87 ignoring registers).
- 080A4798: stopped at 32.9%; retained locally in wip/sub_080A4798.c. Contact-route selection computes an angle toward fieldDC's position, forms a 160-scaled direction vector, scans route points for a negative dot product while excluding the current point, and otherwise calls Spider_SelectRandomEntry. Packed-vector bitfields and inline output lifetimes need further reconstruction; current candidate does not reproduce the stack/register layout.

Useful seed: src/fn/sub_080A73B4.c has TentacleBody's inline Select implementation. Naming prefix follows TentacleBody_GetDirectionFrame and TentacleBody_UpdateDefeat. Ghidra export gen/ghidra/decomp.c is absent. Only fast per-file checks were run; no full build or network actions.

Tool caveat: depin.py --write on guarded NONMATCHING drafts checked the default assembly branch and removed both pins as apparent matches. Restored the pins and explicitly rechecked with check.py --nonmatching --record. Integrators should strip the guard or fix depin's draft checking before using it on these files.

## Task 080E7134 — 2026-10-20

Tags: #matching #mimic #ivy #nonmatching

- 080E7134: drafted 93.4% (`SceneActor_LeapTowardTarget`, tentative). Starts a leap toward the route target, or corrects lateral displacement from the terrain-cell center first; context +0x65E controls randomized speed. Remaining realigned differences: 30 instructions, 8 ignoring registers. Reused the terrain-center and polar-vector idioms from 080EDEA8 and 080E1A28. Loading the random table entry into a separate local before the speed expression fixed instruction ordering. The surrounding Mimic callbacks suggest a future Mimic prefix refinement.
- 080E9C20: stopped at candidate-length fuzzy 73.3%, 135 realigned differing instructions / 67 ignoring registers. Scratch candidate remains at wip/sub_080E9C20.c, outside production. Tentative grab transition: animation entry 5, frame 3 threshold 3/6 depending on paletteOffset, then entry 0x12 and target relocation via Player_BeginPositionRequest or sub_0807D2E0. Table 086062B0 supplies offsets. Facing outputs require addressable byte locals; the output position uses s32 16-bit bitfields, not ordinary s16 fields. Control/state constant lifetimes and block layout still differ substantially.
- 080E7908: stopped at candidate-length fuzzy 70.8%, 111 realigned differing instructions / 58 ignoring registers. Scratch candidate remains at wip/sub_080E7908.c. Tentative Mimic pursuit resumption: approaches offsets at actor +0x310/+0x314 from +0x318/+0x31C by quarter-distance, capped at 200, then selects a participant and restores pursuit flags. Separate scalar x/z pointers improved the attempt over array pointers. Stack-vector materialization and lifetime/allocation still differ.
- 080E76F0: matched (`Mimic_UpdateDefeatBlink`). On entry selects animation 10; after frame 19 emits alternating spark particles, blinks, then runs terrain/defeat script and sets the defeat event flag. Initializing packed range z before x/y reproduces the original combined word store and removes an extra saved register. An inline ResetElapsed(sequence) preserves the +0x48 sequence base and +0xE store; a direct field expression folded these into +0x56. Exact check after clang-format: MATCH, 536 bytes; no pins.
- 080E7B68: matched (`Mimic_ResetPursuitDrawable`). Clears context +0x65A/+0x65C, snapshots position/cell when global flags & 0x4800, selects idle animation and advances the drawable sequence. Reused StepSequence from 080F05E8; its temporary 0xFFFF must pass through r1 instead of r2. depin removed one unnecessary hint (14 to 13); exact check after clang-format: MATCH, 416 bytes. Register-pinned concession retained.
- 080E99F8: drafted 91.9% (`Ivy_ApplyHeldTargetDamage`, tentative). Kind 4 immediately returns to idle; player kinds spawn contact particles and call 08122338, then disable collider, with a release behavior after frame 60. Remaining differences concentrate on retaining the actor position across the two effect calls, argument-load order and the collider mask-load order (18 realigned instructions, 11 ignoring registers).

Checks used only tools/check.py; no full build, network operation or direct git mutation. Ghidra export gen/ghidra/decomp.c is absent. Draft scores above are the recorded full-function scores, distinct from scratch candidate-length scores. Combined ROM validation remains for the integrator.

## Task 081B90F0 — 2026-10-20

Tags: #matching #motion #dark-loan #registry2388

Three of six assigned functions matched (50.0% by function count, +50.0 percentage points within this task); one guarded draft. Exact matched body sizes total 2080 bytes. No register pins were needed by the exact matches. Only fast per-file checks were run; no full build or network operations.

- 081BC468 matched as `DarkLoan_UpdateAdjustmentDigits` (392 bytes). Displays selected step count and its product with amount per step. Ported the two four-digit conversion blocks from `PauseMenu_RefreshPriceDigits`; keep separate animation and flag cursors, and a saved `s + 0x60` base. Descending stack-digit clears and descending four-widget loops reproduce the original.
- 081B98D8 matched as `Registry2388_UpdateIndicators` (744 bytes). Selects indicator frames from bits at 0x528; hides optional indicators; positions two selection markers via 081BB178. The child flag test is at child + 0x1A4. A small inline bit test taking the child pointer and mask reproduces constant-load order. A typed flag field at widget + 8 prevents folding that offset into the widget base. Reusing the base variable for the later markers fixes allocation without pins.
- 081BCCD4 matched as `DarkLoan_UpdateAdjustment` (944 bytes). Handles confirm/cancel and left/right adjustment, updates reward balance and lifetime total, advances selection pulse, and shows the maximum-balance warning. Resource animation at 0x58 uses header at 0x18. To match the final hiding loop, declare the local mask before constructing q (0x600) then p (0x420); retain p-store then q-store and q-increment then p-increment. This resolved the last 22 differing bytes.
- 081B95E4 drafted at 83.5% as existing `MotionTrack_Update`. Modes select vector addition, scalar approaches, orbit updates, timed interpolation and stepped target approach. Mode 10 deliberately falls through into mode 5 after updating absolute sines. Remaining differences are saved-pointer placement, square/distance register allocation, and a short extent difference. Marked register-pinning concession; `depin.py --write` skips a nonmatching input, so it could not remove pins under its exact-match criterion. Draft was checked with `--nonmatching --record` and defaults to INCLUDE_ASM.
- 081B90F0 stopped at 70.0%, not drafted. Local candidate retained at `wip/sub_081B90F0.c`. Arc-center/radius geometry: normalize signed angle modulo 256, special-case zero and half-turn, rotate the start/end delta, derive radius from cosine, normalize the offset and add the start position. Pointer-based inline subtraction and XZ-length helpers force the delta to stack and improve the initial 46.0% to 69.5%; pinning radius yields 70.0% with candidate length equal to target. Still 96 realigned differing instructions, 8 ignoring registers. More allocation and helper-shape recovery needed.
- 081BBE88 stopped at 59.9%, not drafted. Local candidate retained at `wip/sub_081BBE88.c`. Loads CB05 resources 32BA and 530D, initializes loan-rate digits and widget rows, two A47C sprites and warning animations. `R` script option clamps to 0..13 and stores rate + 65; absent option randomizes an unset rate. Pinning object to r10 modestly improved the initial 56.0%, but stack frame/resource lifetimes, sprite-field addressing, and widget loops still differ (82 structural instructions). Needs typed sprite/widget layouts and more careful resource-pointer lifetimes.

Ghidra export unavailable (`gen/ghidra/decomp.c` missing); used assembly and matched relatives. No shared documentation/index edits, following cloud-worker instructions.

## Task 08115E00 — 2026-10-20

Tags: #matching #sprite-drop #glove #collision

Matched 25.2% of the assigned function-body bytes (+25.2 percentage points within this task; 980 bytes). Two of six functions matched; no guarded drafts reached acceptance. Register-pinned category: 08115E00 (372 bytes); unpinned: 081162D8 (608 bytes). Worker used only fast checks, with no full build or network operations.

- 08116E44 stopped below draft threshold: best scratch candidate-length fuzzy 59.8%, 337 realigned differing instructions / 189 ignoring registers. Tentative `Glove_UpdateDeparture`: calls a completion/departure script with participant and result values, stops locally selected participant sounds, snapshots position, advances motion until 128 horizontal units away, then selects pending/delayed script callback by link mode. Shared motion block from 08119378 was useful, but script argument construction, pointer lifetimes, and allocation across the movement calls still differ. Candidate in `wip/sub_08116E44.c`.
- 081172C4 stopped below threshold: best scratch candidate-length fuzzy 69.0%, 129 realigned differing instructions / 26 ignoring registers (`build/low172-best.c` checkpoint; later scratch alternatives in wip). Tentative `Glove_UpdateLaunchArc`: creates a stage icon on entry, accelerates the vertical offset in phase 0, then decays forward speed in phase 3; shares damped motion and terrain response with other Glove callbacks. Explicit pointer assignment inside both damping branches improved structure; pinning the motion pointer made allocation worse. Needs pointer-lifetime/allocation work.
- 081162D8 matched as `ActorRecoil_ReceiveBodyContact` (tentative semantic name). Body-volume callback registered by 0811668C; rejects unavailable control/type 7, applies directional knockback except special kinds 3/4, plays contact sound, and records converted special-contact values. Reused exact `Actor_ApplyKnockback` (0811FAB8). An inline boolean helper that captures the flags pointer through an output argument reproduces its lifetime and mask-before-load ordering. Declaring local msg then obj argument copies fixes the final prologue instruction-order pair. Formatted exact check: MATCH, 608 bytes, no register pins.
- 081169A8 stopped below threshold: initial scratch fuzzy 60.8%, 132 realigned differing instructions / 75 ignoring registers. Tentative `Glove_InitializeStyleModifiers`: imports link participant payload style bytes or initializes style from game state, then initializes modifier sources, scaled stats, action mode, drain and low-resource color. Link payload is +0x1C4 + participant*0x24. Style/drain/color pointer hoisting worsened the candidate; typed style fields and pointer lifetimes remain to recover. Candidate in `wip/sub_081169A8.c`.
- 081160CC stopped below threshold: best scratch fuzzy 45.0%, 133 realigned differing instructions / 81 ignoring registers. Tentative `SpriteDrop_CreateAtTile`: searches 16 slots at stride 0xB8, initializes tile coordinates and terrain clearance, installs update/expiry callbacks and collision volume, selects animation, returns slot or -1. Slot body starts at manager +0x18; collision size is (8,6,10), offset (0,-6,-2), lifetime 600 for kind 0 and 200 otherwise. Packed signed 16-bit bitfield vectors reproduce word accesses, but manager/slot/body base selection and stack lifetimes still differ. Candidate in `wip/sub_081160CC.c`.
- 08115E00 matched as `SpriteDrop_UpdateEntry`. Updates screen Y/Z, spatial-index position and animation when tick +0xB1 is set; always updates screen X and inserts nearby drops into the spatial index. Reused the packed sequence stepper from 0805AAC8. A typed render subobject at +0x1C preserves the frame +0x10 and second flip-bit base accesses. Temporary position argument pins/barriers preserve y-minus-96 before z load and subtraction. `depin.py --write` removed six unnecessary hints (17 -> 11); formatted exact check: MATCH, 372 bytes. Register-pinning concession retained.

No below-threshold source was promoted to src. Ghidra export was unavailable. Integration still needs the combined ROM build; no shared documentation/index edits were made under the cloud-worker instructions.


## Task 0809AE0C follow-up — 2026-10-20

Tags: #matching #lun #route #collision

Two of six assigned functions matched (33.3% by function count, +33.3 percentage points within this task); 1164 exact bytes, all unpinned. Two existing guarded drafts retained. Only fast per-file checks were run; no full build or network actions.

- 0809BE98 matched as `Lun_FollowRoute` (tentative, 732 bytes). Entry installs callback +0x228, clears saved motion at context +0x424, and sets context +0x3E3. Invokes interruption and speed callbacks, scales animation, updates sway, approaches context +0x3F4/+0x3F8 within 32, then saves polar motion and selects another random route destination. Relative 080A79E8 supplied the exact Approach/Scale/Polar scaffolding. Moving route pointer construction after Polar recovered natural register allocation (91.0% -> 98.6%). A separate saved target pointer assigned inside the cell-index helper after calculating x/z reproduces the late pointer copy. The Fill(dst, tmp, value) inline from 0809D0C8 fixes CpuSet argument-load order and reaches exact equality. No pins.
- 0809BCE8 matched as `Lun_UpdateDefeat` (tentative, 432 bytes). Entry selects profile animation 10, plays the category event sound, clears flags2D0 bit 0x100000, disables both colliders, sets flags2C0 bit 0x800 and consumes the lift penalty. Animation completion runs terrain and eight-argument defeat scripts, conditionally sets the defeat event flag, and selects idle control/state. Reused the script argument and control/state scaffolding from matched Mimic_UpdateDefeatBlink (080E76F0); first candidate checked exact. No pins.
- 0809B94C retained existing draft 81.1%, `Lun_ApproachMovementTarget`. Restored required file-local alias and address-named INCLUDE_ASM fallback; added address alias for cached Lun_UpdateWaitingAnimation callback. Recorded full reviewed score remains 81.1%; 110 realigned differing instructions / 32 ignoring registers. Default assembly path checked exact separately; this earns no exact C credit.
- 0809B3F8 retained existing draft 82.8%, `Lun_TurnTowardSavedDirection`. Restored required file-local alias and address-named INCLUDE_ASM fallback. Recorded full reviewed score remains 82.8%; 64 realigned differing instructions / 24 ignoring registers. Default assembly path checked exact separately; this earns no exact C credit.
- 0809C174 stopped below draft threshold: best scratch fuzzy 75.1%, 46 realigned differing instructions / 18 ignoring registers. Tentative `Lun_DecayRouteMotion`: installs callback +0x224 on entry, adds saved context +0x424/+0x428 motion, halves it arithmetically every four ticks, then selects pursuit or rest when control completes. Reusing a motion pointer and mask helper improved 63.2% to 75.1%; targeted zero/offset pins subsequently worsened the result. Best candidate restored at wip/sub_0809C174.c; state-store constant derivation, zero lifetime and allocation remain.
- 0809AE0C stopped below draft threshold. Rechecked retained contact candidate: 46.9%, 154 realigned differing instructions / 71 ignoring registers. Original contains discarded context-bit calculations, while C removes them; ownership-pointer lifetime and state-store allocation also differ. Existing capture/contact interpretation remains tentative; candidate stays in wip/sub_0809AE0C.c.

Ghidra export gen/ghidra/decomp.c is unavailable. Commits queued through build/commit.sh; combined ROM validation remains with the integrator. No shared documentation/index edits under cloud-worker instructions.
