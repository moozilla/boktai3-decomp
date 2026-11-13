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
