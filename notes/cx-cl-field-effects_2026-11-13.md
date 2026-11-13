# Field objects and presentation cleanup

Tags: #matching #animation #particles #scene #assets #tooling

Phase 4 worker branch `codex/cx-cl-field-effects`. Work follows the assigned concession-free ranges in ROM order; the coordinator performs the combined ROM build and report regeneration. Every consolidated extent below passes the stock per-unit byte comparison. Code matching coverage is unchanged by consolidation.

## Consolidated ranges

| Range (end exclusive) | Unit | Former files | Main recovery |
|---|---|---:|---|
| 080133CC–080136B4 | terrain_shadow | 13 | Intrusive links, tile sprite, manager, terrain access, ordinary callbacks |
| 080136B4–08013C14 | scaled_shadow | 21 | Sprite/resource ownership and height-dependent scale bounds; four provisional names |
| 08013C14–08013F98 | shadow_registry | 17 | 84-byte registry entries embedding the terrain shadow and position |
| 08014F64–08015160 | fragment_group | 6 | Four groups of six fragments, shared scene animation/sprite types |
| 0801689C–08017270 | floating_item_effect | 15 | Eight widget slots, player targets and common projection helpers |
| 08017270–08017428 | scanline_wave | 4 | Phase/timing and 288 contiguous samples; six trailing allocation bytes remain unknown |
| 08018438–080187BC | palette_cycle | 7 | Sixteen transition records, endpoint/hold fields and saved BG palette |
| 08018D94–08018F38 | blend_transition | 5 | Signed coefficient pairs, target masks and unsigned transition time |
| 08019518–080195DC | palette_frames | 5 | Seven palette frames advanced every four ticks |
| 08020F6C–080213EC | scene_presentation_commands | 14 | Message operands, receiver ownership, shake state and orbiting effect |

Range boundaries include a few singleton resets belonging to the following owner. They remain in ROM order and now have names in the symbol CSV. Their old compiler annotations were unnecessary: they match with the default compiler. Existing named functions are preserved except two demonstrated mistakes: 080211B0 and 080211C0 are SceneCommands_SetShake and SceneCommands_StopShake, based on the camera-shake consumer at 08021360.

## Compiler decisions

- FragmentManager_Initialize (080150D0): typed array indexing initially recomputed the enabled-byte address. `scope probe` with `AGBCC_GIV=54` matched, establishing strength reduction as the difference. A backwards byte cursor, initialized after the active/next stores, gives the original code with ordinary C.
- FloatingEffect_RiseAndSettle (08016AA0): the closest structured conditional inverted one branch. Preserve the explicit terminal control flow for now. Three separate projection copies were successfully consolidated into a common scalar helper and a common vector helper.
- BlendTransition_InitializeFromScript (08018E00): converting the pointer read loop to `do/while` hoisted its end address and changed allocation. `hoist` identifies UID 78; `AGBCC_NOMOVE=78` confirms a diagnostic match. The landed source retains the pointer loop, without a knob or concession.

## Shared header verification

`SceneInputWidget` now names the affine scale bytes at 0x34/0x35. It remains 0x60 bytes with all existing offsets preserved. All 345 transitive source dependents passed `tools/check.py`. No unrelated source was changed.

## Tooling

`merge_unit.py --write --no-git` writes the merged file and unlinks input files without staging. This supports the external sandbox committer. Tests verify that an output also listed as an input is preserved, git is never invoked in this mode, and an output-creation failure preserves inputs. The existing parser tests also pass (five tests total).

No full ROM build, disassembly regeneration, regrouping or progress-report regeneration was run in the worker.

## Second cleanup checkpoint

Tags: #matching #animation #scene #structs #compiler

| Range (end exclusive) | Unit | Former files | Main recovery |
|---|---|---:|---|
| 0803EE58–0803F5C4 | scene_arc | 11 | Shared sprite/sequence/graphics, common playback, structured flight ending |
| 0803F5C4–0803F804 | ground_effect | 12 | Typed lifecycle and shared tile resource |
| 08042320–080427FC | display_animation | 16 | Real sequence/keyframe union, shared widget and graphics |
| 08045A20–08045D18 | projected_display | 11 | Shared projection helper, widget/resource/task records |
| 08047BB4–08047DBC | temporary_animation_pool | 6 | Sixteen widget slots, lifetimes, shared resource and task |
| 0804D778–0804DF40 | staged_effect | 17 | Task/message receiver/sprite/graphics ownership; typed saved position |
| 08054294–08054B80 | hit_switch | 17 | Shared sprites, typed entry indexing, named player terrain-cell field |
| 08057678–08057960 | proximity_icon | 5 | Shared widget/resource/task records |
| 080638BC–080645D4 | roaming_object | 28 | Both variants share SceneActorBody, sprite, graphics, terrain and listener types |
| 08103AD0–081041DC | named_actor_commands | 15 | Canonical named-object, manager, route and player types |
| 08104F08–08105808 | callback_pool_feedback | 9 | Sprite/sequence/graphics/collider/shadow, health thresholds and distances |
| 08105808–08106DAC | region_actor | 16 | One actor and pool layout, terrain override, structured damage path |
| 08106DAC–081084E4 | tile_trigger | 24 | Body, sequence, script payload and twelve-slot pool; two terminal gotos removed |

The proximity and region ranges straddle an object boundary at 08105808. Split there and combine the contiguous region prelude with its pool. Likewise, the tile-trigger handlers and adjacent Pool1B8 range form one object and one contiguous unit.

The display-wrapper header audit covered 2,252 units. Its sole compatibility failure was an older HUD caller using the widget's `unknown4` array; retain that public array and document its registration byte. The affected HUD and pool units match after correction. The unchanged SceneAnimation and ScaledShadow layouts live in small type headers so consumers can reuse them without gaining conflicting legacy API declarations. Other affected headers received per-unit checks (69 for the projected display; four final shadow/health feedback consumers).

Several typed changes clarify compiler-sensitive behavior without concessions. A signed initialization result preserves the scene-arc constructor's negative check. The hit-switch initializer is void, while its wrapper forwards an unused scalar result through a legacy function-pointer call adaptation. Unsigned views of the named object's signed flag field preserve halfword loads. Keeping relative pointers to a nested resource or target-list entry preserves the original base-plus-field addressing.

Shared playback or random-shake inlining was tried in staged effects and the tile constructor. It changes register allocation or table-load placement, so matching local sequences remain. The region contact-damage early exits and tile activation/release terminal paths accept structured conditionals with unchanged bytes.

Two ranges are set aside for a second pass. Cell gates: 13 of 14 typed functions match, but the canonical player-pointer array lets the compiler remove the ROM's reload after a coordinate store in ClampPlayerEdge. Scheduling manager: typed ID lookup reorders a cursor copy and bit constant; eight loop variants retain that two-instruction difference. Original sources were restored; drafts and evidence remain under `build/cx/`.

The coordinator shortened this worker's queue to stop before Scene status indicator at 08126650. Work after that boundary belongs to the cloud worker. Code matching coverage remains unchanged by this cleanup; the combined ROM check remains with integration.

## Final cleanup checkpoint

Tags: #matching #animation #particles #scene #structs #compiler

The final seven completed units are animated tiles (14 former files), scene debris (13), native effects (29 files / 31 functions), native particles (13), actor palette fade (one file / two functions), actor-stat configuration (two), and scheduled effects (13). Every complete unit matches with the stock compiler. This brings the run to thirty consolidated units; matching coverage is unchanged.

Animated tiles reuse the shared playback helper and a structured name lookup. Scene debris uses a union of the existing sequence/keyframe visuals; its callback loop needs a signed byte stride to retain the original induction-variable choices. Native effects separate the old collider/graphics byte block into a 0x40-byte spatial collider and a 0x1C-byte graphics record, and use an embedded slot-key cursor to preserve registry addressing. The unsequenced sprite initializer retains its unused direction argument because the profile caller passes it in the ROM. Native particles use the canonical night flag and a structured pause dispatcher; the allocator's common exit remains because early return changes block ordering.

Actor palette fade now lives in the shared ActorStats record at 0x348 and uses CharacterPreviewModel renderers. All five affected units match. Its data header deliberately omits API prototypes because an older external caller has a different legacy signedness declaration. Scheduled effects now have one complete record shared with the earlier scheduling manager; all nine dependent units match. Its slot index is unsigned for the initializing halfword OR, with a signed cast for the scheduler's negative sentinel test. The destructor retains separate typed cursors for terrain removal and body unregistering.

Two additional ranges were set aside with original matching sources restored. Palette fade at 081111E8: sixteen of seventeen typed routines match, but AdvanceColors schedules the flat palette-buffer address differently from the old two-dimensional declaration. Challenge records at 0811D524–0811D6C8: canonical GameState arrays change index/base addition order, and the reset's halfword assignment changes to a read/OR sequence. Typed experiments and logs remain under `build/cx/palette_typed_draft` and `build/cx/challenge_typed_draft`. The earlier cell-gate and scheduling-manager blockers also remain; the scheduled effect type itself is now recovered and landed.

At the user's run-stop request, the current spread-shot item had only been inventoried. Its source remains untouched. A durable [layout and boundary draft](spread_shot_cleanup_draft.md) records the recovery so far. Scene moving particles were not started. No work at or after the reassigned Scene status indicator boundary was performed. All code commits through request 0034 were confirmed by the external runner. Final notes are the last commit request. No full ROM build or progress regeneration was run; combined validation remains with the coordinator.
