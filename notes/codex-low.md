
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
