# Bosses, Player and Engine core cleanup, 2026-11-13

Tags: #matching #actors #tooling #process

Requests 0001–0019 are committed on the worker branch: 17 consolidated units, one merge-tool fix and one shared callback refinement. Request 0020 preserves the unfinished Player actions draft and this handoff. No code progress percentage is claimed; this is phase-4 cleanup of already matching code. The coordinator must run the combined ROM build and regenerate the project reports.

The draft is [a patch](drafts/player_actions_2026-11-13.patch) against the tree after request 0019. It is not compiled and must not be applied for integration without resolving the mismatch and finishing naming/validation. It includes the two new files, the tentative Player fields and deletion of the 86 old files.

## Finished
- 0001: merge_unit.py writes output before removing inputs without git operations. Six merge/parser regression tests pass.
- 0002 queued: Clock events (080192D0–08019518), five files consolidated into src/clock_events.c. Complete 0x30-byte ClockEvents task, script argument block, existing SunClock fields, shared engine APIs; all five functions MATCH (584 bytes). Existing names retained and interpretations expanded in the symbol notes.

## Decisions and adaptations
- ClockEvents_Update retains its existing nonempty do/while grouping around zero assignment. Removing it loses the saved zero across calls; narrower local types do not restore it. No new concession introduced. All pointer-offset accesses and duplicate task views removed.
- Callback return values remain as matched; explicit function-pointer casts adapt the task scheduler's void callback API.

## Current
- Stopped at the weekly-run end. Requests 0001–0019 are committed. Player actions (08156BBC–0815A400) is saved as an unlanded draft in request 0020; the original matching sources and header are restored. The remaining queue was not started.

## Validation / limits
- No full ROM build: coordinator owns integration validation.
- Shared header changes have full consumer checks recorded below.

## Tile animation finished
- 0003 queued: 11 files into src/tile_animation.c (080195DC–08019A14), complete 0x200-byte task and eight 0x3C-byte channels. Shared header replaces all partial views, pointer offsets and duplicate types. Source/destination tile indices, frame durations and playback flags named.
- All 11 individual baselines and merged unit MATCH (1080 bytes); formatted and rechecked. Correct engine Script_GetValue(void) declaration matches despite the old local prototype taking an unused argument. No shared-header edits.
- DMA volatile access remains the existing hardware-register access.
- Next: Sprite pool (08019A14–08019E58).

## Sprite pool finished
- 0004 queued: 16 files into src/sprite_pool.c (08019A14–08019E58); complete 0xB1C-byte pool and sixteen 0xB0-byte slots, shared sprite/graphics/sequence layouts, typed callback/context and per-callback user storage. All 16 individual baselines and complete merged unit MATCH (1092 bytes), including formatting recheck.
- Removed explicit sub_08249248 trampoline in favor of callback(pool, slot), matching naturally.
- Promoted SpritePool_SetAnimationFrame and SpritePool_Remove from duplicate local aliases into symbols/functions.csv. No external source references needed edits.
- Retained free-slot search goto: direct return and break/result loop forms change five instructions. The complete shared type change itself matches without adaptations. The sprite link byte remains the existing shared sprite header's unknown4[0], with no new view or header edits.
- Next: Scene message controller (08020964–08020F6C).

## Scene messages finished
- 0005 queued: 24 files into src/scene_messages.c (08020964–08020F6C). Full 0xAC controller and linked receivers; reused the unchanged-size 0x28 SceneEventReceiver prefix. Named receiver/message identifiers, kinds and triggers in scene_event_dispatch.h, preserving every offset. Shared engine declarations replace conflicting local prototypes, including Trigger_ConsumePending.
- Unit MATCH (1544 bytes). All 92 direct and transitive C users of the shared header MATCH; full output in build/scene_messages_header_checks.log. Promoted SceneMessages_IsActive to functions.csv.
- Removed a dead call-address local. Retained the existing one-iteration argument-copy grouping and script-search goto: simpler loop/return forms change code. Replaced casts over the argument block with a named packed header word; its upper half must be preserved to match, while bit-field assignment uses a different access width.
- Dispatch handlers keep a consistent three-argument callback signature, including the unused message parameter in SetSignal.
- Next: Script-entry selector (08042198–08042320).

## Script selector finished
- 0006 queued: seven files into src/script_selector.c (08042198–08042320), complete Task member and typed VM cursor. Removed duplicate local API declarations in favor of script_virtual_machine.h. Every unit using script_selector.h is included in this merged check.
- Replaced both initializer and creator goto paths with ordinary early-return/guard structure; complete unit MATCH (392 bytes) after formatting.
- Promoted the three adjacent SceneActorRegistry_IgnoreEntry callbacks to real symbol names. They are empty registry stubs, not selector operations, so retained their object prefix. data/actor_registry.csv display-name refresh is left to the coordinator's regeneration; addresses are unchanged.
- Next: Script keyframe pool (08055678–08055B74).

## Script keyframe pool finished
- 0007 queued: 13 files into src/script_keyframe_pool.c (08055678–08055B74). Reused the existing resource/animation/motion layouts, added real Task member and API declarations, documented offsets. All header users are inside this unit.
- Removed hardcoded manager-entry/tag offsets from lookup in favor of entries and tag fields. Complete unit MATCH (1276 bytes), including after formatting.
- A fully indexed lookup changes register choices/lifetimes; retained the existing lookup flow with typed field bases. Destructor retains an animation-address traversal stepping by sizeof(entry): traversing full entries instead adds an address instruction per iteration. No new concession or partial struct view.
- Next: Script utility references (08056A60–08056D70).

## Script utility finished
- 0008 queued: 12 files into src/script_utility.c (08056A60–08056D70), reusing existing shared argument/reference types and VM declarations. Promoted ScriptUtility_SetFlag800; replaced RANDOM_TABLE_ADDR with the actual gRandomTable symbol.
- Complete unit MATCH (784 bytes) after formatting. Array-base arithmetic must stay integer-shaped: direct typed indexing schedules the table-base load before the index shift; integer addition with the named symbol matches.
- Retained the existing nested nonempty do/while grouping in SetFlag800. Removing it makes the pointer local-allocated after its loaded value (why: pointer priority 3750; r1 occupied by the value), rotating r1/r2/r3. Ordinary pointer/mask/value locals also change order. No new concession introduced; no shared headers changed.
- Next: Scene audio events (08063634–080638BC).

## Scene audio events finished
- 0009 queued: 15 files into src/scene_audio_events.c (08063634–080638BC). Moved the old partial task definition out of scene_event_dispatch.h; scene_audio_events.h now owns the complete 0x50-byte Task + linked receiver object. Existing receiver/message field offsets remain unchanged.
- Correct shared engine Sound_PlaySong and Trigger_ConsumePending declarations match signed command arguments without local redeclarations. Named the dispatch count helper and loop indices.
- Complete unit MATCH (648 bytes), all individual baselines MATCH, all 79 remaining direct/transitive shared-header users MATCH after formatting (build/scene_audio_header_checks.log).
- Next: Scene range watcher (0806F7B0–0806F990).

## Scene range watcher finished
- 0010 queued: five files into src/scene_range_watch.c (0806F7B0–0806F990), real Task member, sizeof allocation, named helpers/locals, direct maximum field in place of a raw 0x2B address.
- Converted the backward argument store from integer-cast pointers to a typed u32 pointer walk; the termination comparison retains the original signed address relation. Complete unit MATCH (480 bytes), all baselines MATCH, all header users are in the unit.
- Next: Scene tilemap (08110A8C–081111E8).

## Scene tilemap finished
- 0011 queued: 20 files into src/scene_tilemap.c (08110A8C–081111E8), full shared panel + scroll-source layouts, typed map/tile-list loaders, field-based destroy/query/update helpers, meaningful BG-control parameter names.
- All baselines and merged unit MATCH (1884 bytes), stock agbcc throughout: removed the old_agbcc annotation from SaveMenu_ClearSingleton. All tilemap_panel.h users are inside this unit.
- Promoted SaveMenu_ClearSingleton, SaveMenu_Close, SceneTilemap_WaitCopyDelay and SceneTilemap_GetScrollLimit; provisional interpretations are in functions.csv. Kept SaveMenu tails under their true object prefix despite the coarse assigned range.
- Found background_controller.h Script_ReadValue(u8 *) conflicting with script_utility.h Script_ReadValue(const void *). Avoided combining those headers; scene_tilemap.h declares only needed cursor APIs missing from engine.h. No outside-assignment header changes.
- gSceneCamera's existing Camera pointer declaration also holds this alternate task kind; explicit pointer casts adapt that existing singleton API without changing the shared header.
- Next: Spatial index (0811B048–0811B498).

## Spatial index finished
- 0012 queued: 12 files into src/spatial_index.c (0811B094–0811B498). The first requested function, SpatialIndex_ScanBuckets at 0811B048, lives inside protected src/fn/sub_0811AE9C.c; left that file entirely untouched and began after it as instructed.
- Unified 0x40-byte SpatialEntry, linked nodes, active/passive buckets and index task. The scheduler's head/count view is the bucket's passive list/count. Setters use real position/extents/offset/kind/callback fields. Position update reuses TilemapPanel scroll.x/y in place of a partial camera view.
- All individual baselines and merged run MATCH (1028 bytes), including formatting recheck. No existing header edits.
- Scheduler retains its existing delay-assignment do/while grouping: narrowing to the actual s16 destination type without the loop changes code and adds four bytes. All existing names retained.
- Next: Registry E7BC (0811B498–0811BCBC).

## Registry E7BC finished
- 0013: refined spatial contact callback to (context, other), hit byte and hit timer using the adjacent dispatcher; kept the scheduler's unused bucket-index argument explicit. Spatial unit still MATCH. Separate shared-type commit.
- 0014 queued: 28 files into src/registry_e7bc.c (0811B498–0811BCBC), unified collision lists with spatial_index.h, complete 0x48 registry objects / 0x378 task, shared packed/signed coordinate union, typed callbacks and game-state field addresses. All individual baselines and complete merged unit MATCH (2084 bytes); spatial-index header dependent also rechecked MATCH.
- Removed old_agbcc annotations from SetFlags/ClearFlags and the existing volatile gGameState declaration from GameState_GetActionCount892. Every raw GameState offset is now a named shared field address.
- Direct component accesses suppress the ROM's reloads after writes. Scalar-address accesses via (s16/s32 *)(u32)&gGameState->field preserve generic memory alias behavior and MATCH without volatile or new offsets. A plain typed-address cast folds back to a component; separate pointer locals restore shape but change registers. This is the minimal local adaptation retained.
- Bounds overlap retains its common separated label: replacing it with early returns changes three branches. Registry ownership of the two bit helpers remains tentative in symbol notes.
- Next: Character model (0811C5F8–0811D47C).

## Character model finished
- 0015 queued: 44 files into src/character_model.c (0811C5F8–0811D47C). Shared preview model now owns its resource/data/palettes/appearance slots; layout-preserving ScenePoolActor fields cover input, animation selections, acceleration, airborne state, sound and steering. Removed all function-local partial structs, reused shared style, BG, camera and GameState fields. Named parameters and locals.
- All 44 functions and merged unit MATCH (3716 bytes). All 150 direct/transitive users of modifier_list.h and scene_pool.h MATCH (build/character_header_checks.log). Removed two old_agbcc annotations.
- Promoted alternate-HUD/runtime-flag/actor-appearance names. ScenePool_ApplyMotionInput is named in functions.csv, but its C definition retains sub_0811CD70 because src/fn/sub_08116870.c outside the assignment calls that symbol. split.py supports generated-name aliases; coordinator can finish the source rename across the caller.
- Palette initialization must initialize its part pointer after the palette calls to retain argument-copy order. The style initializer retains a typed field-address reassignment to preserve the ROM's literal-address calculation. No new raw struct offsets.
- Kept ValidIndex's existing branch form: a Boolean range test folds to unsigned comparison and shrinks eight bytes. Kept the existing decimal-digit subtraction grouping: removing it changes six register choices.
- Next: Script options (0811DAD4–0811E37C).

## Script options finished
- 0016 queued: 24 files into src/script_options.c (0811DAD4–0811E37C). Complete 0x23C task now combines all five typed tables, counts and script cursors. Reused GameState flag fields, typed script references, shared engine callbacks, and ScenePoolActor tween fields.
- All 24 individual functions and merged unit MATCH (2216 bytes). ScenePoolActor gains only a named s32 field at +0x28 inside existing padding; all 81 remaining direct/transitive header users MATCH after the merge (build/options_header_checks.log).
- Corrected missing wrapper arguments in GameState_SetChallengeCompletionFlagFromScript and Video_DrawClockAndValueOnBackground to explicit index/layer parameters; both still match without extra moves. Removed duplicate partial structs and private API declarations.
- StateFlags_TestIndexedFlag retains its common enabled label: early-return form reverses return-block placement and three branch choices. No new concessions.
- Next: Script record (0811E37C–0811EE3C).

## Script record finished
- 0017 queued: 38 files into src/script_record.c (0811E37C–0811EE3C). All five table readers use the complete shared ScriptOptions task. Shared StatModifierState embeds its ModifierRecord and owns enabled/owner fields; callback types are real function pointers. Extended existing ScenePoolActor padding with source ids/count, modifier slots, drain prefix, counters and amounts without shifting any existing fields.
- All 38 functions and merged unit MATCH (2752 bytes), and all 104 direct/transitive users of both shared headers MATCH (build/record_header_checks.log).
- Converted all five goto lookups to ordinary guarded search loops; six duplicated scale/add callbacks now call common inline helpers. Replaced every explicit callback trampoline in the unit with direct typed callback calls; byte identical.
- Named table option getters correctly (they read option words rather than table counts), ModifierStats_ApplyKind0/1/3 and their interpretation notes. Updated kind-0/1 references in the already consolidated character unit, which also MATCHes in the shared-header checks. Kind-3 retains its C address symbol for out-of-assignment src/fn/sub_081169A8.c; CSV name is available to coordinator.
- Kept three existing nonempty do/while groups: ordinary returns/assignments rotate pointer/value registers. Kept existing address-of/dereference expressions in callback loops: removing them hoists the count address, shortens loops and changes saved registers. No new concessions.
- Constructor follows the embedded modifier state by sizeof(slot), preserving the original parallel callback/record traversal; all formerly raw enabled/owner offsets are named fields.
- Next: Player core (0813ADC4–0813C040).

## Player core finished
- 0018 queued: 44 files into src/player_core.c (0813ADC4–0813C040), 4732 bytes MATCH. Complete shared Player fields replace local partial actors and effect layouts; typed effect sprites and signed/unsigned coordinate union retain the original access widths. Recovered pending callback, completion procedure, event flags and graphics resources. All five remaining address functions have descriptive CSV names; four C spellings remain for out-of-unit callers.
- Removed the callback trampoline, all contact-response gotos and the directional-effect one-iteration wrapper. Unified Q12 and flag-test helpers. Removed three stale old-compiler annotations and a single-use stat-offset macro.
- All 750 direct/transitive Player header consumers MATCH: 747 in build/player_core_header_checks_final.log, then three orbit-effect units after resolving a struct-tag collision. Final merged and formatted core matches. Anonymous unions were rejected by the stock compiler; named coordinate union and typed one-element drawable arrays preserve existing pointer users and offsets.
- Minimal compiler-sensitive adaptations remain: typed indexed GameState base addresses preserve address association/alias reloads; max-stat signed loads use field-derived displacement from the stat-state base. Shared SceneVec signedness needs an unsigned effect-coordinate view. No raw constant field offsets or new concessions.

## Player cue actions finished
- 0019 queued: 53 files into src/player_cue_actions.c (0814F804–08156BBC), all 29624 bytes MATCH. Shared Player fields replace six padding views (graphics, auxiliary drawable, motion, timers, messages and coordinates). Recovered auxiliary playback/resources, cue receiver, target, timer triplet, particle handle/speed and secondary sprite channels. Existing flattened renderer storage is accessed through complete engine sprite/coordinate types at named fields.
- Extracted unchanged SceneEventCommand/SceneEventReceiver layouts into scene_event_types.h. Player needs these data types without importing scene-event API/global declarations that conflict with legacy units. All 726 direct/transitive consumers of Player and scene-event headers MATCH (build/player_cue_header_checks.log).
- Structured the transition animation callback and both charge-action state machines, removing their gotos and the charge argument one-iteration wrapper. Consolidated duplicate Q12/Q6 helpers, expanded ANIM and removed unused function aliases. All local partial structs are gone. Existing names retained; seven symbol interpretations refined. Scored concessions scan is empty.
- Two other animation callbacks retain shared reset/dispatch labels: structured versions move branches despite matching size. Side-facing helpers retain literal-zero versus phase-parameter variants; sharing the parameterized version changes the store/branch arrangement in 08153E1C. Alternate position signed loads retain named-address casts; direct unsigned stores recover the original store-address scheduling.

## Player actions draft and end of run
- 0020: saved the full consolidation patch at notes/drafts/player_actions_2026-11-13.patch and detailed evidence in notes/cx-cl-bosses_cleanup_2026-11-13.md. Restored all 86 original sources and the committed Player header; no compiled-code changes remain from this item.
- All 86 transformed individual files matched before merging. The merged stock-compiler candidate matches except Player_ResetLinkState (08158C14): 34 differing bytes in a 14404-byte unit, fuzzy 99.8%. Moving global clears into an inline helper reduced the difference to 28 bytes; moving the entire implementation before public functions did not resolve it. The saved draft uses the straightforward direct body.
- The reset preloads seven global addresses around the four-slot clear loop. ROM assigns the first three to high registers and the next three to low registers; the merged compiler assigns the first three to low registers. No barriers, pins or compiler overrides were introduced. A standalone reset probe importing the consolidated header differs by 22 bytes; moving ram.h ahead of it does not fix it. Restoring the original source/header yields MATCH (196 bytes).
- The tentative Player header only names hoverTargetHeight/hoverHeightTimer at 0467/0468 and lockedVelocity at 0490. Its broad consumer check did not finish; no result is claimed. Those header edits are part of the saved patch, not the compiled tree.
- Remaining quality work in this draft: promote address names in functions.csv, improve local names, recover terrain-cell header/attribute fields, and evaluate shared helper/structured control-flow forms. None of those is claimed complete.
- The next worker should resume this item, isolate which header declarations change reset allocation, then validate every shared-header consumer. Afterwards continue the untouched queue from Player cue (0815AD24–0815C9EC).
- The run ended by explicit user instruction. No further items were started. Coordinator still owns combined ROM validation and report regeneration.

## Resumed investigation: 08158C14 literal-pool hashing

A renewed continuation resumed the queue after the weekly-end handoff. The
original reset still matches; scope shows a specific source of the merged gap.
The seven global-address pseudos have equal allocation priority (344), equal
reference counts (2), and equal live lengths (58). GCC GCSE assigns pseudo
numbers in expression-hash bucket order. Its constant expressions contain
literal-pool labels, and those labels change from LC7–LC13 in the standalone
function to LC24–LC30 in the merged function. This changes the hash order and
therefore the pseudo-number tie break in global allocation. The ROM gives the
first three global addresses high registers, but the merged form gives them
low registers. It is not an effect of changed Player field offsets.

Inlining the slot-clearing loop into the reset leaves 28 differing bytes.
Grouping the three related interaction-state clears in an inline helper leaves
39 differing bytes. Neither was retained. No artificial declarations, padding
functions, compiler options or register constraints were added to manipulate
literal numbering. The existing saved patch remains the direct-body draft.
The subsystem is skipped under the worker brief's reasonable-time rule; the
compiled tree retains the original matching files. Next is Player cue,
0815AD24–0815C9EC. A future attempt should seek a meaningful C representation
that makes allocation independent of the tied GCSE pseudo numbers.
