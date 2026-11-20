# Bosses and Player warning run

The coordinator requested wind-down at 01:15:56 PST. All 142 commit requests have receipts. The final tracked worktree is clean. Every landed C unit passed its stock-compiler byte check. No concession-ok annotations were added.

The final scan covers 303 assigned files. The queue recorded 550 diagnostics. The corrected detector finds 22 additional pre-existing literal locals on the original source. On the same detector, the queue went from 572 diagnostics to 440, a reduction of 132 (23.1%). The ending count is 438 warnings and two inherited volatile errors. Run-scoped counts include priority errors in the queue totals.

| Unit | Original queue | Corrected baseline | Final | Status |
| --- | ---: | ---: | ---: | --- |
| Player | 103 | 103 | 16 | Pass complete |
| Player cue | 14 | 14 | 2 | Pass complete |
| Player contact | 3 | 3 | 0 | Clear |
| Target registry | 4 | 4 | 2 | Pass complete |
| Player orbit effect | 3 | 3 | 0 | Clear |
| Player burst | 2 | 2 | 0 | Clear |
| Player attack effect | 4 | 4 | 3 | Pass complete |
| Player barrier | 2 | 2 | 1 | Pass complete |
| Boss slot 2 | 11 | 13 | 13 | Pass complete |
| Nidhoggr | 20 | 20 | 17 | Pass complete |
| Hresvelgr | 119 | 121 | 103 | Pass complete |
| Vanargand | 149 | 161 | 161 | Interrupted |
| Vanargand projectile | 4 | 4 | 4 | Pending |
| Player particle cloud | 1 | 1 | 1 | Pending |
| Ratatosk | 105 | 111 | 111 | Priority investigated |
| Player clone | 6 | 6 | 6 | Pending |

## Unit results

- **Player**. Ascending array clears match the timer, slot and callback initialization. The recovered base-power argument, cue graphics and shared status operations match throughout the Player files.
- **Player cue**. Five wind and facing commands share one pose operation. Speeds 8 and 32 remain parked because the in-place multiplications compile as shifts.
- **Player contact**. The timed cue reset, ascending two-slot clear and contact status updates match both units.
- **Target registry**. The 68-byte ID lookup matches with two early returns. Both insertions retain their inherited volatile diagnostics.
- **Player orbit effect**. The orbit emitter and pause update match. The cursor and mask-first helper have limited provenance as recorded below.
- **Player burst**. Both handlers use the hidden flag on the task and its embedded sprite. The shared frame layout matches all 72 bytes.
- **Player attack effect**. The initializer and updater share sixteen slots. In-place full-turn angles and the initial stack-vector latch still change code generation.
- **Player barrier**. The marker uses the SDK eight-byte fill. Naming the barrier update frame moves the address load past the duration check and adds four bytes.
- **Boss slot 2**. Three handlers now use the shared action-entry record. The reported state 12 and distance 140 were in the original tree.
- **Nidhoggr**. The detached attack records its starting X in the shared slot. Its lateral speed uses the sample mask directly. The pool clear and three spawn gates remain open.
- **Hresvelgr**. Six state setters match with direct zeroes, six behavior handlers share vulnerability entry, and eight volley phases use the four offsets populated by the initializer. All twenty actor-header dependents and four polar-vector users matched.
- **Vanargand**. The initializer still matches 5,064 bytes and the paired-part unit matches 17,340. Four direct-literal function candidates matched in scratch before STOP and await full-unit landing checks.
- **Vanargand projectile**. The eight-slot occupancy scan, contact reset and coordinate fills await their ordinary cleanup pass.
- **Player particle cloud**. The source flag test in the forty-particle updater awaits its ordinary cleanup pass.
- **Ratatosk**. The particle-spawn dispatch was investigated. The ordinary pass over the 65 assigned files was pending at STOP.
- **Player clone**. The array access and five literal locals await their ordinary cleanup pass.

## Priority findings

- **Target registry.** The matched ID lookup preserves the original non-advancing mismatch behavior. The two insertions each need a second load of the list singleton across a node-link store. Two address-taken pointer copies reproduce both 72-byte functions. The unlink sibling uses ordinary pointer assignments and offers no evidence for introducing those copies.
- **Hresvelgr barrage.** Both phase flag masks are direct. With the proved-zero direction removed, the closest 732-byte draft differs by two bytes. The ROM saves and restores the random-index mask through IP and the draft uses SL.
- **Vanargand initializer.** Using a direct bank selector joins the pending-animation constant to the presentation store and changes stack placement. The closest ordinary initialization is 5,048 bytes against 5,064. The paired-part initializer independently confirms its position at part + 0x1C, or boss + 0x418. The one-word palette preset remains open.
- **Ratatosk particle spawn.** Expressing the weighted distribution as 0, 1, 2, 1 recovers the store paths without the proved-zero test. The 940-byte draft is missing the ROM's register copy between the halfword store and reload. The packed appearance and animation-field candidates left that instruction unresolved.

## RANDOM TABLE

Hresvelgr's defeat spray matches all 1,764 bytes after removing the first one-member table union. The separate case-two table union and unreachable spawn remain. Drawing through a shared cursor and building the complete effect vector or spawn record still leaves 22 paired instruction differences in the closest draft without the synthetic spawn. The three related burst emitters also change their table reload when expressed through a direct pointer. No general random-draw form was established.

## Other open Player decisions

The single-pass wrappers in 0813D834 and 08140F40 change indexed-store or return-path allocation. The movement and timeout paths at 08142CC0, 08142FC4, 0823F56C and 0823F928 need independent zero materialization around calls or input tests. Typed fields alone still change load order at 0814C3A4, 0814D9C4 and 08159030. Collider-mask tests at 0814D264 and 0823D810, callback traversal at 0823D7E0, animation speed at 0823E494 and throw radius at 0823E998 remain open.

## Tool changes

The literal-local detector now distinguishes writes to a local from writes to a same-named struct member. All 37 detector tests passed. Comparing both detector versions on the original source finds two additional literals in Boss slot 2, two in Hresvelgr, twelve in Vanargand and six in Ratatosk. The per-site baseline audit is in build/cx/closeout.json and the final compiler-backed linter output is in build/cx/closeout.lint.

## PARAMETER HELPERS

Distinct calling functions in the working tree.

- `GameState_TestWorldFlags` — 4 callers. Real game operation. World-flag queries across player logic use the same game-state field.
- `ParticleRandom_ReadCursor` — 1 caller. Needed for the match. The particle-pool sibling has the same by-address read, but only the orbit emitter calls this version.
- `Player_SetPositionResetDuration` — 4 callers. Real game operation. The reset and recovery paths set the same player duration.
- `Player_AddStatusFlags` — 23 callers. Real game operation. Player actions set the same status word with distinct status masks.
- `Player_SetTimedCue` — 2 callers. Needed for the match at two callers. Both store a cue kind and duration together; its two-caller signature has limited independent evidence.
- `WordFlags_Test` — 1 caller. Needed for the match. One frame-display test keeps the literal before the load.
- `WordFlags_Set` — 3 callers. Needed for the match. Generic word OR; mask-first was chosen to match the pause menu. The test uses the opposite argument order.
- `PlayerModifier_TestTargetFlags` — 6 callers. Real game operation. Six target-bonus functions test flags in the same target record.
- `PlayerCue_SetWindFacing` — 5 callers. Real game operation. Four wind commands and the facing command select the same pose, heading, and side.
- `SceneVec_SetPolarXZ` — 4 callers. Real game operation. Four volley phases form the same horizontal offset from heading and radius.
- `SceneVec_TruncateComponent` — 1 caller. Real operation. The X and Z components use signed fixed-point division toward zero. Its argument is a product.
- `Hresvelgr_EnterVulnerableBehavior` — 6 callers. Real game operation. Six behavior handlers set the vulnerability flag and consume the entry and completion latches.
- `StateActor_ConsumeActionEntry` — 7 callers. Real game operation. Both boss families consume the same action entry and clear the finished latch.
