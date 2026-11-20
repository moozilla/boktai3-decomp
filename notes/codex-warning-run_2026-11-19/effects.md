# Effects and presentation

## Wind-down at 01:15:56 PST

Reviewed units 1–29 and both later priority units (54 and 62), covering 31 of 67. All 64 commit requests have receipts. A final run of tools/check.py matched all 56 changed C files. The two singleton declarations passed all 2,680 RAM-header dependents. The worktree is clean at 454db23d0.

The corrected detector counts 287 warnings on the starting sources and 201 now across the 148 queued files. The original queue recorded 274 findings under the previous detector and included the baseline volatile error. The 31 reviewed units went from 130 warnings to 44 under the corrected detector. The volatile error in SpriteTrail_UpdatePositions remains in both counts as a separate error. The source baseline for the comparison is 6d9c47c04; the nine starting compiler-proved zeros are recorded in the queue.

All five priority functions were reviewed. Five of nine proved-zero expressions were removed. Three remain in FragmentGroup_SpawnRadial and one in SpiralBurst_InitParticles. The random-table evidence and the sprite-trail camera-load decision are recorded below. The parameter-helper audit identifies which earlier warning reductions still depend on a helper parameter.

Unit 30, Feather effect pool (08128B48), was in scratch trials when STOP arrived. Its five original files were rechecked individually and all match. Direct placement of the three pause masks and the destructor zero changes instructions. No source changes landed in this unit. Resume at unit 30. Units 31–53, 55–61 and 63–67 remain unreviewed.

## Finished

- Background controllers (080015AC), 3 → 0 warnings, 0 errors. Background_FindMapId now indexes entries by the returned index; strength reduction emits the ROM cursor walk. Background_LoadTilesFromScript and the scrolling slot reset use one typed descending fill. Both match the descending-store pattern also seen in 08001590 and in the nonzero TaskSystem_Initialize fill. The whole unit matches (5,372 bytes). Commits 0001–0003.

## In progress

- Background cleanup uses the MP decoder's layer count and layer name hash in the shared map layout. All 260 changed-header dependents match. The legacy view remains for SceneTransition's tileset field; its resource comes from the CEEF tileset library and needs its own type when that unit is reached. Commits 0004–0006 also promote both background helper names.
- Palette controllers (08002AA8), 6 → 1 warning, whole unit MATCH (3,152 bytes). One forward color loop, one shared script slot counter, direct idle resets and explicit fade callback scheduling remove five warnings. The retained 0x7C1F red/blue mask is used twice. Writing it in place leaves five reload differences; no exemption. Commit requests 0007 and 0009.

## Exemptions and tool changes

The only new exemption is the two-view velocity vector in Scene particle pools (08068500), documented below. Scope misidentified the moved loop count as unread. Commit request 0008 corrects the operand handling, reproduces the actual countdown reader at 08002B84 and passes 233 scope tests.

## Homing and orbiting effects

11 → 0 warnings, 0 errors. Both spawners use DisplaySequence_MaskCommand. Four updates separate particle state 4 from player-slot bit 4 and read gPlayerSlotStatus[0]. The typed setter handles both manager registration and destruction. Commit request 0010. Two pre-existing unused random-table assignments still affect loop-invariant motion. Removing them changes 40/48 and 57/63 instructions in the two spawners, including with direct symbolic draws. Keep open for the random-table investigation.

## Effect emitter

1 → 0 warnings. The default damage checks bit six through the same masked-word helper as scene actor availability. ContactCollider supplies both reactionFlags and calculated damage at the documented offsets. The whole unit matches (856 bytes), as do all 15 dependents of the shared flags and emitter headers. Commits 0011–0013.

## Particle groups

7 → 4 warnings, 0 errors. The bounce update shares the terrain shadow test. Group clearing and destruction use native slots, and the radial spawner uses named graphics fields and a struct position copy. Its 0x3FF wrap mask is used five times. Passing the literal directly changes 21 instructions. Commits 0014–0019.

## RANDOM TABLE

FragmentGroup_SpawnRadial (08014CA4) still has three proved-zero reports. Replacing only the table expression with either a symbol or a numeric address changes 104 ROM / 100 draft instructions. For the two state arrays, direct stores individually change 53/51 or 66/68. Native owner indexing, whole-group initialization, range draws and three global cursor forms still miss. The table equivalence and two independently hoisted array bases remain distinct decisions. No exemption.

## Animated particles

5 → 0 warnings. The slot holds a manager pointer and every one of its 2673 header dependents matches. Both assigned files use the shared particle, sprite and sequence types. AnimatedParticles_Create now retains all four ROM sequence calls with direct state arguments. Passing the particle animation to both DisplaySequence_StepSprite and the later duration setter eliminates the extra join copy. No unions, proved-zero terms or branch labels remain. Stock MATCH for 52 and 1176 bytes. Commits 0020–0022.

## Fading particle groups

3 → 1 warning. Reading the vertical speed as a 16-bit displacement for gravity leaves the position integration intact, eliminates the motion-pointer adjustment, and matches all 540 bytes. A separate pre-existing lifetime guard remains. Removing it drops two register copies after the byte load. Destruction uses the shared task slot. The spawner now shares the native layout, but replacing its group/table union with a direct table pointer changes 26 instructions, including the loop's table and cursor registers. No exemption. Commits 0023–0025.

## Linter coverage

Commit 0026 detects immutable integer initializers in comma-separated declarations. The regression test covers calls, character literals, aggregates and variables changed or passed by address. All 37 concession tests pass. This adds one reported site to each of the previously recorded palette controller and particle group units (packed red/blue masks and the pair of inactive/hidden flags). Their new totals are 2 and 5, respectively; the earlier counts used the original detector.

## Palette level transitions and floating item effects

- Palette level transitions, 3 → 0, 996 bytes MATCH. All three absent script arguments use the descending fill from the background controllers (0027).
- Floating item effects, 2 → 0 on the original detector, 5 → 0 on the corrected detector, 2516 bytes MATCH. The seven-entry ROM dispatch table identifies both wait states. Five handlers use the same state/entered/timer transition. A typed setter registers and releases the task (0029).
- Commit 0028 fixes another linter omission. Assigning effect->state previously counted as changing a local named state. The full concession suite passes (38 tests).

## Hardware blend transitions

1 → 0 warnings. The absent initial and delta coefficient pairs both use the same descending fill as palette level transitions. This removes the before-array return pointer and the zero local. Commit 0030.

## Transient sprite effects and temporary animation pool

Transient sprites, 2 → 0. Direct ContactPosition script fields match, and ZeroFill8 handles an absent velocity. The script handler is named in the symbol table. Temporary animation pool, 1 → 0. Its SDK fill takes an ordinary position local and no longer needs the scratch structure or a zero argument. Commits 0031–0033.

## Staged sprite effect

11 → 1 on the original detector, 14 → 1 on the corrected detector. The ROM table identifies all eight states. A shared state transition resets frame, signed fields handle spread and travel, and three phases share the shake sample conversion. WaitForScript retains its frame-zero local: moving the reset before the state store exchanges the memory writes; a direct post-store reset changes the frame and register choices. All 1992 bytes match, no exemption (0035). Commit 0034 separately excludes record members from local warnings and passes all 39 linter tests.

## Repeated effects

2 → 0 warnings, 160 and 84 bytes MATCH. Contact reads reuse the shared masked-word operation. The upper state checks the same world flags as the monster updates and increments its signed balance field directly. The existing monster spelling remains a compatibility alias; all 29 dependents match. Commits 0036–0038.

## Orbit background and scripted display track

Orbit background, 2 → 0 reported warnings. The turn clamps in the 256-step angle space. The display-disable helper is match-dependent (see below). Scripted display track, 1 → 0. Passing the embedded sequence state directly to playback removes the single-pass loop. All three files match. Commits 0039–0041.

## PARAMETER HELPERS

Counts are distinct direct callers, followed by call sites when different.

| Helper | Callers | Assessment |
| --- | ---: | --- |
| Array_FillDescending | 4 (7 sites) | Real descending range fill across background, palette and blend script defaults. Capturing the value before traversing the range matters to the match. |
| EffectManager_SetGlobal | 2 | Needed for the match. Registration and teardown are single stores. |
| PlayerSlot_HasStatus | 5 (9 sites) | Needed for the match. All homing callers pass bit 4. One caller is the existing pausedMode accessor. |
| PlayerHasStatus | 2 | Needed for the match. Both callers pass 128 to the renamed actor-mode accessor. |
| Flags_MaskedWord | 3 (4 sites) | Needed for the match in Effects. The mask-first order comes from the existing scene-actor helper, and does not establish a general convention. |
| Task_SetManager | 3 | Needed for the match. All callers clear a slot. Putting the manager before the slot mattered. |
| AnimatedParticle_SetFrame | 1 (3 sites) | Needed for the match. This renames an existing file-local frame setter. |
| FloatingEffect_SetState | 4 (8 sites) | Real transition. Four functions change state and reset entry and elapsed time together. Passing state as a parameter matters in the rise handler. |
| FloatingEffect_SetManager | 2 | Needed for the match. Registration and destruction each store one pointer. |
| StagedEffect_SetState | 7 (8 sites) | Real transition shared by seven handlers. All reset frame on entering a state. The width of the state parameter affects initialization. |
| StagedEffect_AddOuterSpriteFlags | 2 | Needed for the match. Both calls perform the same masked flag update. |
| StagedEffect_ShakeOffset | 3 (6 sites) | Real operation. Three shake phases convert varying samples into signed displacements. |
| GameState_TestWorldFlags | 7 (8 sites) | Real shared test. Six existing monster functions use the original spelling. The shared definition preserves its argument and result types. |
| Bg_DisableDisplay in sub_08050E9C.c | 1 (2 sites) | Needed for the match. A different file already contains a separate version. |
| DisplaySequence_MaskCommand | 37 (43 sites) across the tree | Predates the run. Its two new uses in homing spawners are needed for the match and do not establish that those values are sequence commands. |

The particle CellIndex, Direction, ReadOrigin and SpawnInline helpers predate this run. Their signatures changed with the position type. No new exemption.

## Sprite trail (priority review)

12 warnings plus one volatile error → 10 warnings plus one volatile error. Six checked files now use native trail, spatial contact or drawable records. The impact age and callback frame reset directly. The two collider masks, four flag constants, free-slot shift and two constructor loops still change code in place. The constructors exchange the pool and origin spill when the loop becomes an assignment; extracting either or both sprite setup loops did not match. Commit 0042.

SpriteTrail_UpdatePositions still hoists the scene-camera pointer. Native tilemap scroll, a shared projection, packed coordinate writes and an indexed history all retain the hoist. A fixed RAM-address diagnostic keeps both loads inside the loop but loses the screen-X spill and produces 296 of the 304 bytes. The canonical singleton declaration stays in source; no exemption.

## RANDOM TABLE

SpiralBurst_InitParticles (08234540), 3 proved-zero + 1 unrelated union → 1 proved-zero + 1 union. Five uses of the existing ParticleRandom_ReadDirectSample(table) eliminate **both in-loop table reassignments**. The first phase still advances gRandomIndex directly and reads the low byte. The loop now holds one unmodified table pointer. All 428 bytes MATCH, commit 0043.

Using plain gRandomTable for the initial table expression still exchanges the wrap mask and table between r6 and r9 (21 ROM / 15 draft instructions). Capturing both table and cursor in a stream object, and testing byte/halfword views of the same table, did not restore that allocation. For the sine pointer/sample union, an ordinary wave cursor differs only in the destination of the address add and the base of the signed load. Both items remain open.

The SpiralBurst direct-draw change was tested against FragmentGroup_SpawnRadial with the three expressions removed separately and together. It did not recover either array-base computation or the initial table address. The closest individual flag-store test still differs in 61 ROM / 59 draft instructions.

## Units 17–29, in ROM order

- Scene particle pools (08067C10), 6 → 5. The anchored velocity union overlays one vector, as SpriteEffectVector does. Added concession-ok(local-union) at 08068500 with that evidence. Its dual-purpose Z operand remains open. The update guard and pause mask change the loop preheader. Both direct singleton clears change the epilogue. The unsigned slot bit changes two instructions; slot 31 requires the unsigned shift.

- Five-slot burst emitter (08069964), 2 → 1. Scripted initialization now matches the direct constructor. Two emission phases share the halfword draw. The final kind literal in place changes two instructions.

- Moving tile particles (08069FA4), 2 → 1. Indexing the 48 particles lets teardown clear its singleton directly. Separating the spawn angle from the random table still changes 39 ROM / 41 draft instructions. A native record cursor and the shared draw also miss.

- Sprite effect controllers (0806A60C), 4 → 2. The controller and script setup use shared drawables and ContactPosition. Indexing the 24 slots lets teardown clear the singleton directly. The engine and player pause literals still change the instruction order. All 29 header dependents match. An additional shadowed outer pause-mask local is unreported.

0049 fixes a linter omission discovered in SpriteEffectController_UpdatePool. An inner mask declaration used to count as a write to the outer local. Both pause masks are now reported. Sprite effect controllers are 5 → 3 under the corrected detector. All 40 concession tests pass.

- Scheduled scene effects (0806F3A8), 4 → 3. One timing operation serves both scheduling and respawning. All four values come from signed halfword fields. Three pause masks remain open after direct-literal tests.

- Pal fade (0811D524), 3 → 0. The rank, score and timer handlers use literal missing-value defaults. The minimum-record and timer calls now pass the values their callees read in r1 and r0.

- Scene scheduled effect (08122824), 7 → 5. The animation owner and typed resource remove the setup zero. Indexed teardown clears the notice singleton directly. Four engine/player masks and one animation setup zero remain. Both full units match under both singleton declarations.

- Scene moving particle (08125B68), 1 → 0. The indexed teardown removes the native sprite and clears the manager directly.

Moving particles also use Terrain_GetCellIndex directly for current and proposed signed positions. The 1,500-byte unit still matches after removing the duplicate CellIndex and packed-position casts.

- Scripted particle (08126140), 3 → 0. The constructor and drift update share the native slot. Direct state stores, three shared random draws and direct playback remove the constants and one-member union. Both header dependents match.

- Burst spark (08126918), 3 → 0. Direct initialization and indexed traversal clear all three reported sites. Release, update, creation and teardown now share a single burst-spark slot. The existing unreported teardown zero remains.

- Sprite projectile (08126D38), 7 → 4. The three units share the ten native slots. Rebound uses the common terrain and animation code. Its first sound ID and both dispatcher pause masks still change instructions in place. Separating the constructor union exchanges r4/r5 between the initial flag and the random origin (five instructions).

- Timed sprite (08127994), 2 → 0. Both warnings clear. The blink callback toggles bit 0 directly and teardown indexes its twelve native sprites.

- Fragment burst (081281B4), 1 → 1. Native sprite/collider fields and shared projection scale match. Inlining the first flip bit swaps eight registers; using common full playback changes seven instructions.
