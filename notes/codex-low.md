
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
