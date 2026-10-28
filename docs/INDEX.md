# Documentation index

Tags: #process

Where to look, by topic. Read the topic map, then open only what you need;
sizes in the table tell you what a file costs. Two lookups beat reading:

* `python3 tools/docindex.py 08081A20`: every doc and note that names that
  address or covers it with a range, plus its symbol and whether C exists.
* `python3 tools/docindex.py --tag tether`: files and history entries tagged `#tether`.

## Topic map

**Start here.** [README](../README.md) (build, layout) · [CLAUDE.md](../CLAUDE.md)
and [AGENTS.md](../AGENTS.md) (rules) · [HANDOFF](HANDOFF.md) (current state) ·
[WORKER](WORKER.md) (matching playbook) · [scope](../tools/scope/README.md) (the matching
workbench; [loop](../tools/scope/WORKER.md)) · [subsystem runs](../tools/scope/subsystem/README.md)
(Codex workers per subsystem) · [UNIT_WORKER](UNIT_WORKER.md) (unit consolidation brief).

**Matching craft.** [scope facts](../tools/scope/README.md) ("agbcc facts established with scope") · [AGBCC_TIPS](AGBCC_TIPS.md) (pre-scope patterns) · [AGBCC_RULES](AGBCC_RULES.md) (why agbcc chose it; `tools/why.py`) · [REGALLOC](REGALLOC.md) ·
[FOUNDRY](FOUNDRY.md) (asm-to-match pipeline and funnel numbers) · [BIG_FUNCTIONS](BIG_FUNCTIONS.md) (anchordiff, variants, rtlprobe: 500+ instruction functions) · [PERMUTER](PERMUTER.md) (permuter fork, regsolve, litsweep, chunks) ·
[MODEL_BENCHMARK](MODEL_BENCHMARK.md) (explain.py/idioms.py vs check.py with Haiku/Sonnet) ·
[COMPILER_REFERENCES](COMPILER_REFERENCES.md) · [EDS wiki](external/eds/README.md)
(Yu-Gi-Oh! EDS matching-tricks catalog, vendored, MIT) · [SCALAR_APPROACH](SCALAR_APPROACH.md)
(angle/scalar helpers) · [ASM_OUTPUT_QC](ASM_OUTPUT_QC.md) (ABI and credit audits) ·
[LAYOUTS](LAYOUTS.md) (one offset map per RAM object from every file's structs, casts and the ROM; `tools/layoutmerge.py`) ·
[RAM](RAM.md) (the RAM inventory `symbols/ram.csv`, shared declarations, the data measures; `tools/ramlayout.py`) ·
[CANDIDATES](CANDIDATES.md) · [PERMUTER_HANDOFF](PERMUTER_HANDOFF_2026-10-15.md) ·
[DRAFT_INVENTORY](DRAFT_INVENTORY_2026-10-09.md) · libraries:
[LIBGCC](LIBGCC.md), [LIBM](LIBM.md), [RTC](RTC.md), [EEPROM](EEPROM.md), [RFU](RFU.md),
[tasks/m4a](tasks/m4a.md).

**How the engine works.** [ENGINE](ENGINE.md) (tasks, script VM, scenes; the
big one) · [GAME_LAYOUT](GAME_LAYOUT.md) (ROM to gameplay) · [ROM_MAP](ROM_MAP.md) ·
[FINDINGS](FINDINGS.md) · [SOLAR_SENSOR](SOLAR_SENSOR.md) · runtime evidence:
[SCRIPT_TRACING](SCRIPT_TRACING.md), [RUNTIME_CONTEXT](RUNTIME_CONTEXT.md),
[MENU_CONTEXT](MENU_CONTEXT.md).

**Scripts (GCL).** [gcl/README](../gcl/README.md) (names, views, Konami's
naming) · [gcl/examples/scale_quiz](../gcl/examples/scale_quiz.md) (a puzzle read
end to end) · [gcl/examples/prologue](../gcl/examples/prologue.md) (New Game to the
Sealed Prison) · [gcl/examples/sealed_prison](../gcl/examples/sealed_prison.md) (the
first dungeon, room by room; story progress) · [CUTSCENES](CUTSCENES.md) (the event tables SceneMessages plays,
receivers and commands) · [MGS_GCL_MATCHING](MGS_GCL_MATCHING.md) (the matched VM) ·
[PROCESS_REVIEW](PROCESS_REVIEW.md) · [tasks/script_targets](tasks/script_targets.md)
(native routines scripts call, unmatched first) · [tasks/relatives](tasks/relatives.md)
(unmatched functions a matched relative mostly explains; `tools/relatives.py`).

**Subsystems** (one doc per matched family; the title carries the address
range). Scene actors: `SCENE_ACTOR_*`, [ACTOR_ATTACHMENT](ACTOR_ATTACHMENT.md),
[ACTOR_ATTACHMENT_MANAGER](ACTOR_ATTACHMENT_MANAGER.md),
[STACKED_ACTOR](STACKED_ACTOR.md), [ROUTE_SPRITE_ACTOR](ROUTE_SPRITE_ACTOR.md),
[HIT_SWITCH](HIT_SWITCH.md), [BOUNCING_OBJECTS](BOUNCING_OBJECTS.md). Scene
panel: `SCENE_PANEL_*`. Inventory: `SCENE_INVENTORY_*`. Input and HUD:
`SCENE_INPUT_*`, `SCENE_HUD_*`, [LEVEL_ADVANCE_DISPLAY](LEVEL_ADVANCE_DISPLAY.md).
Effects: [SPRITE_EFFECT_*](SPRITE_EFFECT_CONTROLLER.md),
[SPARK_PARTICLE](SPARK_PARTICLE.md), [BURST_EMITTER](BURST_EMITTER.md),
[SCENE_EFFECT_SCHEDULE](SCENE_EFFECT_SCHEDULE.md),
[TERRAIN_EDGE_MARKER](TERRAIN_EDGE_MARKER.md). Motion and animation:
[MOTION_0818](MOTION_0818.md), [ANIMATION_081E](ANIMATION_081E.md),
[BACKGROUND_CONTROLLERS](BACKGROUND_CONTROLLERS.md). Report grouping of all of
these: [REPORT_SUBSYSTEMS](REPORT_SUBSYSTEMS.md).

**Assets and data.** [ASSETS](ASSETS.md) (resource archives, sprites, stages,
enemies, areas and the Studio World page) · [DATA](DATA.md) (decoded data
formats) · `data/` (registry, charmap, items, notes).

**Tools and infrastructure.** [CENTAUR](CENTAUR.md) (mGBA script, Studio link,
stage survey) · [PROGRESS_CI](PROGRESS_CI.md) · `tools/studio/` · `tools/` docstrings
(`python3 tools/X.py -h`).

**History and process.** [history/](history/README.md) (dated handoff entries,
tagged) · [DEVLOG_2026-10-08](DEVLOG_2026-10-08.md) ·
[RETROSPECTIVE_2026-10-08](RETROSPECTIVE_2026-10-08.md) ·
[MODEL_BENCHMARK](MODEL_BENCHMARK.md) · [KNIDL_PROCESS_REVIEW](KNIDL_PROCESS_REVIEW_2026-10-09.md) ·
[PUSH20](PUSH20_2026-10-08.md) · [RESEARCH_2026-10-12](RESEARCH_2026-10-12.md) ·
[BACKLOG](BACKLOG.md) · `notes/` (per-batch worker notes: accepted sources,
skips with reasons, near-matches).

## Tags

Put `Tags: #a #b` in the first lines of a doc or note, and under the heading of
a history entry. Use these where they fit (add a new one only for a topic that
recurs): `#scripts` `#actors` `#scene` `#inventory` `#panel` `#hud` `#menu`
`#wireless` `#tether` `#particles` `#animation` `#sound` `#hardware`
`#library` `#assets` `#text` `#matching` `#tooling` `#progress` `#process`.
Files without a line get tags guessed from their title (shown in italics).
Addresses are their own tags: write them as 8-digit hex, ranges as `A–B`.

<!-- generated by tools/docindex.py: everything below this line -->

| File | Title | Tags | KB |
|---|---|---|---:|
| [docs/ACTOR_ATTACHMENT.md](../docs/ACTOR_ATTACHMENT.md) | Actor attachment state and offsets | *#actors* | 9 |
| [docs/ACTOR_ATTACHMENT_MANAGER.md](../docs/ACTOR_ATTACHMENT_MANAGER.md) | Attachment manager lifecycle and scene actor pool prefix | *#actors #scene #assets* | 8 |
| [docs/AGBCC_RULES.md](../docs/AGBCC_RULES.md) | agbcc rules: why the compiler wrote it that way | #matching #regalloc #tooling | 4 |
| [docs/AGBCC_TIPS.md](../docs/AGBCC_TIPS.md) | agbcc matching tips (pre-scope playbook) | #matching | 8 |
| [docs/ANIMATION_081E.md](../docs/ANIMATION_081E.md) | Animation controller matching batch | *#scripts #animation #assets #matching* | 5 |
| [docs/ASM_OUTPUT_QC.md](../docs/ASM_OUTPUT_QC.md) | Empty-asm outputs and return-value QC |  | 9 |
| [docs/ASSETS.md](../docs/ASSETS.md) | Resources and asset formats | *#scripts #assets #text #tooling* | 33 |
| [docs/BACKGROUND_CONTROLLERS.md](../docs/BACKGROUND_CONTROLLERS.md) | Background controllers and scheduler report groups | *#assets #progress* | 3 |
| [docs/BACKLOG.md](../docs/BACKLOG.md) | Research and development backlog |  | 16 |
| [docs/BIG_FUNCTIONS.md](../docs/BIG_FUNCTIONS.md) | Matching big functions: measure, hypothesise, read the compiler | #matching #tooling | 14 |
| [docs/BOKTAI12_COMPARISON.md](../docs/BOKTAI12_COMPARISON.md) | Boktai 1 and 2 against Boktai 3: preliminary engine comparison | #research #comparison | 21 |
| [docs/BOUNCING_OBJECTS.md](../docs/BOUNCING_OBJECTS.md) | Twelve-slot bouncing objects (080688D0–08069964) | *#particles* | 4 |
| [docs/BURST_EMITTER.md](../docs/BURST_EMITTER.md) | Five-slot burst emitter (tentative identity) | *#particles* | 2 |
| [docs/CANDIDATES.md](../docs/CANDIDATES.md) | Compact checking and candidate checkpoints | *#tooling* | 3 |
| [docs/CENTAUR.md](../docs/CENTAUR.md) | Centaur: playing with the decomp | *#scene #tooling* | 20 |
| [docs/CLOUD.md](../docs/CLOUD.md) | Cloud workers | #tooling #matching | 6 |
| [docs/COMPILER_REFERENCES.md](../docs/COMPILER_REFERENCES.md) | Community references and checked agbcc patterns | *#assets #matching* | 4 |
| [docs/CUTSCENES.md](../docs/CUTSCENES.md) | Cutscenes: the SceneMessages event tables | #scripts #scene #actors | 11 |
| [docs/DATA.md](../docs/DATA.md) | Decoded data |  | 7 |
| [docs/DEVLOG_2026-10-08.md](../docs/DEVLOG_2026-10-08.md) | Boktai 3 devlog for October 8 2026 | *#process* | 21 |
| [docs/DRAFT_INVENTORY_2026-10-09.md](../docs/DRAFT_INVENTORY_2026-10-09.md) | Existing-draft inventory (October 9, 2026) | *#inventory #matching* | 12 |
| [docs/EEPROM.md](../docs/EEPROM.md) | EEPROM SDK source matches | *#hardware #library* | 2 |
| [docs/ENGINE.md](../docs/ENGINE.md) | How the Boktai 3 engine works |  | 46 |
| [docs/ENTRY_AND_MENUS.md](../docs/ENTRY_AND_MENUS.md) | Entry point and contiguous subsystem batch |  | 5 |
| [docs/FINDINGS.md](../docs/FINDINGS.md) | Engine findings |  | 5 |
| [docs/FOUNDRY.md](../docs/FOUNDRY.md) | The foundry: asm to a committed match, measured as a funnel | #matching #tooling | 12 |
| [docs/GAME_LAYOUT.md](../docs/GAME_LAYOUT.md) | Boktai 3 game layout: from ROM to gameplay | *#assets* | 34 |
| [docs/HANDOFF.md](../docs/HANDOFF.md) | Handoff: current state and how to continue | #process | 10 |
| [docs/HIT_SWITCH.md](../docs/HIT_SWITCH.md) | Hit-counted switch pool (tentative identity) |  | 5 |
| [docs/KNIDL_PROCESS_REVIEW_2026-10-09.md](../docs/KNIDL_PROCESS_REVIEW_2026-10-09.md) | KNIDL workflow review and implications for Boktai 3 | *#process* | 13 |
| [docs/LAYOUTS.md](../docs/LAYOUTS.md) | RAM object layouts merged across files | #tooling #matching | 12 |
| [docs/LEVEL_ADVANCE_DISPLAY.md](../docs/LEVEL_ADVANCE_DISPLAY.md) | Level advancement display (tentative presentation identity) |  | 4 |
| [docs/LIBGCC.md](../docs/LIBGCC.md) | Floating-point runtime identification | *#library* | 4 |
| [docs/LIBM.md](../docs/LIBM.md) | Math-library source matches | *#library #text* | 5 |
| [docs/MENU_CONTEXT.md](../docs/MENU_CONTEXT.md) | Pause-menu config, sleep, and save context | *#menu* | 3 |
| [docs/MGS_GCL_MATCHING.md](../docs/MGS_GCL_MATCHING.md) | Boktai 3 / MGS GCL matching evidence | *#scripts #matching* | 66 |
| [docs/MODEL_BENCHMARK.md](../docs/MODEL_BENCHMARK.md) | Small-model matching benchmark: explain.py vs check.py | #matching #tooling | 13 |
| [docs/MOTION_0818.md](../docs/MOTION_0818.md) | Motion callback family at 0818D754 | *#tooling* | 7 |
| [docs/PERMUTER.md](../docs/PERMUTER.md) | Solving register allocation in the compiler, then in C | #matching #tooling | 22 |
| [docs/PERMUTER_HANDOFF_2026-10-15.md](../docs/PERMUTER_HANDOFF_2026-10-15.md) | October 15 matching residue and frozen searches | *#matching* | 5 |
| [docs/PROCESS_REVIEW.md](../docs/PROCESS_REVIEW.md) | External research review and next experiments | *#scripts #process* | 13 |
| [docs/PROGRESS_CI.md](../docs/PROGRESS_CI.md) | ROM-free progress reports | *#progress* | 13 |
| [docs/PUSH20_2026-10-08.md](../docs/PUSH20_2026-10-08.md) | Matching push toward 20% code | *#matching* | 11 |
| [docs/RAM.md](../docs/RAM.md) | RAM objects: the inventory and shared declarations | #tooling #matching #progress | 5 |
| [docs/REGALLOC.md](../docs/REGALLOC.md) | Register allocation in agbcc, and how to steer it from C | *#matching #tooling* | 20 |
| [docs/REPORT_SUBSYSTEMS.md](../docs/REPORT_SUBSYSTEMS.md) | Subsystem report organization | *#tooling #progress* | 10 |
| [docs/RESEARCH_2026-10-12.md](../docs/RESEARCH_2026-10-12.md) | RESEARCH_2026-10-12.md |  | 28 |
| [docs/RETROSPECTIVE_2026-10-08.md](../docs/RETROSPECTIVE_2026-10-08.md) | Boktai 3 matching run retrospective | *#matching #process* | 26 |
| [docs/RFU.md](../docs/RFU.md) | RFU wireless SDK source matches | *#wireless #library* | 6 |
| [docs/ROM_MAP.md](../docs/ROM_MAP.md) | ROM map — Boktai 3 (U33J) | *#assets #text* | 5 |
| [docs/ROUTE_SPRITE_ACTOR.md](../docs/ROUTE_SPRITE_ACTOR.md) | Route sprite actor pool (tentative) | *#actors #assets* | 9 |
| [docs/RTC.md](../docs/RTC.md) | Cartridge RTC matching evidence | *#hardware #matching* | 4 |
| [docs/RUNTIME_CONTEXT.md](../docs/RUNTIME_CONTEXT.md) | Runtime function context | *#scripts #tooling* | 4 |
| [docs/SCALAR_APPROACH.md](../docs/SCALAR_APPROACH.md) | Scalar approach and period-256 angle helpers | *#inventory* | 9 |
| [docs/SCENE_ACTOR_ANIMATION.md](../docs/SCENE_ACTOR_ANIMATION.md) | Scene actor profile animation selection | *#actors #scene #animation* | 4 |
| [docs/SCENE_ACTOR_ATTACHMENT_RESPONSES.md](../docs/SCENE_ACTOR_ATTACHMENT_RESPONSES.md) | Actor attachment selection and responses | *#actors #animation* | 15 |
| [docs/SCENE_ACTOR_BEHAVIOR.md](../docs/SCENE_ACTOR_BEHAVIOR.md) | Actor behavior and collision setup (0807462C–08076034) | *#actors #animation* | 18 |
| [docs/SCENE_ACTOR_BEHAVIOR_FRONTIER.md](../docs/SCENE_ACTOR_BEHAVIOR_FRONTIER.md) | Scene actor behavior frontier | *#actors #scene #inventory #animation* | 13 |
| [docs/SCENE_ACTOR_CONTACTS.md](../docs/SCENE_ACTOR_CONTACTS.md) | Scene actor contacts and profiles (tentative) | *#actors #scene #assets* | 5 |
| [docs/SCENE_ACTOR_CONTROL_HELPERS.md](../docs/SCENE_ACTOR_CONTROL_HELPERS.md) | Scene actor control helpers, 08083AEC–08084280 | *#actors #scene* | 5 |
| [docs/SCENE_ACTOR_CONTROL_MOTION.md](../docs/SCENE_ACTOR_CONTROL_MOTION.md) | Actor control motion phase, 08083550–08083AEC | *#actors* | 3 |
| [docs/SCENE_ACTOR_CONTROL_PARTICLES.md](../docs/SCENE_ACTOR_CONTROL_PARTICLES.md) | Scene actor control and particle pools (08067148–08068500) | *#actors #scene #inventory #particles* | 10 |
| [docs/SCENE_ACTOR_CONTROL_PHASE.md](../docs/SCENE_ACTOR_CONTROL_PHASE.md) | Actor control phase and animation transitions | *#actors #animation* | 7 |
| [docs/SCENE_ACTOR_EFFECT_LIFECYCLE.md](../docs/SCENE_ACTOR_EFFECT_LIFECYCLE.md) | Actor terrain targets and effect cleanup | *#actors #assets* | 6 |
| [docs/SCENE_ACTOR_MOTION.md](../docs/SCENE_ACTOR_MOTION.md) | Registered actor motion phase (tentative) | *#actors* | 8 |
| [docs/SCENE_ACTOR_PALETTE_SCRIPT_CONTACTS.md](../docs/SCENE_ACTOR_PALETTE_SCRIPT_CONTACTS.md) | Scene actor palette, script properties, and contact lists | *#scripts #actors #scene #assets* | 10 |
| [docs/SCENE_ACTOR_PLACEMENT.md](../docs/SCENE_ACTOR_PLACEMENT.md) | Scene actor placement and event feedback | *#actors #scene #sound #assets* | 4 |
| [docs/SCENE_ACTOR_REACTION_STATES.md](../docs/SCENE_ACTOR_REACTION_STATES.md) | Scene actor reaction callbacks | *#actors #scene #animation* | 8 |
| [docs/SCENE_ACTOR_REACTION_TRANSITIONS.md](../docs/SCENE_ACTOR_REACTION_TRANSITIONS.md) | Actor reaction transitions, 08081290–08082648 | *#actors* | 13 |
| [docs/SCENE_ACTOR_REGISTRY.md](../docs/SCENE_ACTOR_REGISTRY.md) | Scene actor registry (tentative) | *#actors #scene #assets* | 7 |
| [docs/SCENE_ACTOR_REGISTRY_LIFECYCLE.md](../docs/SCENE_ACTOR_REGISTRY_LIFECYCLE.md) | Registry list, message dispatch and update cadence (tentative) | *#actors* | 4 |
| [docs/SCENE_ACTOR_REGISTRY_QUERIES.md](../docs/SCENE_ACTOR_REGISTRY_QUERIES.md) | Actor-registry queries (08070070–08070FA8) | *#actors* | 12 |
| [docs/SCENE_ACTOR_TARGET_CONTROL.md](../docs/SCENE_ACTOR_TARGET_CONTROL.md) | Scene actor target and contact control | *#actors #scene #animation* | 2 |
| [docs/SCENE_ACTOR_TERRAIN.md](../docs/SCENE_ACTOR_TERRAIN.md) | Scene actor terrain transitions | *#scripts #actors #scene* | 2 |
| [docs/SCENE_ACTOR_UPDATE.md](../docs/SCENE_ACTOR_UPDATE.md) | Actor timer and pending-flag update helpers | *#actors #animation* | 3 |
| [docs/SCENE_ACTOR_VISIBILITY.md](../docs/SCENE_ACTOR_VISIBILITY.md) | Scene actor visibility and stored motion (tentative) | *#actors #scene* | 6 |
| [docs/SCENE_EFFECT_SCHEDULE.md](../docs/SCENE_EFFECT_SCHEDULE.md) | Scene effect schedule (tentative) | *#scene* | 2 |
| [docs/SCENE_GRID.md](../docs/SCENE_GRID.md) | Scene-grid position helpers (0806E34C onward) | *#scene #assets* | 4 |
| [docs/SCENE_HUD_DISPLAY.md](../docs/SCENE_HUD_DISPLAY.md) | HUD clock, numeric labels and keyframe digits | *#hud #assets* | 3 |
| [docs/SCENE_HUD_MAP.md](../docs/SCENE_HUD_MAP.md) | Scene HUD tilemap helpers | *#scene #hud* | 2 |
| [docs/SCENE_INPUT.md](../docs/SCENE_INPUT.md) | Scene input widgets | *#scene* | 3 |
| [docs/SCENE_INPUT_GRID.md](../docs/SCENE_INPUT_GRID.md) | Scene input grid and pair transition | *#scene* | 4 |
| [docs/SCENE_INPUT_NAVIGATION.md](../docs/SCENE_INPUT_NAVIGATION.md) | Scene input navigation | *#scene* | 2 |
| [docs/SCENE_INPUT_PANEL.md](../docs/SCENE_INPUT_PANEL.md) | Scene input panel display and comparison | *#scene #inventory #panel* | 5 |
| [docs/SCENE_INVENTORY_ACTIONS.md](../docs/SCENE_INVENTORY_ACTIONS.md) | Scene inventory action display and transition control | *#scene #inventory* | 2 |
| [docs/SCENE_INVENTORY_CONTROL.md](../docs/SCENE_INVENTORY_CONTROL.md) | Inventory action controller | *#inventory* | 7 |
| [docs/SCENE_INVENTORY_LIFECYCLE.md](../docs/SCENE_INVENTORY_LIFECYCLE.md) | Inventory lifecycle and neighboring controllers | *#scene #inventory* | 9 |
| [docs/SCENE_INVENTORY_PANEL.md](../docs/SCENE_INVENTORY_PANEL.md) | Scene inventory panel: values, descriptions and grids | *#scene #inventory #panel* | 6 |
| [docs/SCENE_INVENTORY_STATES.md](../docs/SCENE_INVENTORY_STATES.md) | Inventory transitions and confirmation states | *#inventory* | 4 |
| [docs/SCENE_INVENTORY_TAIL.md](../docs/SCENE_INVENTORY_TAIL.md) | Inventory controller tail, catalogs and completion | *#inventory #sound #tooling* | 8 |
| [docs/SCENE_PANEL_COMMANDS.md](../docs/SCENE_PANEL_COMMANDS.md) | ScenePanel message and display helpers |  | 3 |
| [docs/SCENE_PANEL_CONFIGURATION.md](../docs/SCENE_PANEL_CONFIGURATION.md) | Scene panel script configuration and subject creation | *#scripts #scene #panel* | 3 |
| [docs/SCENE_PANEL_EFFECTS.md](../docs/SCENE_PANEL_EFFECTS.md) | Scene panel particles and time palettes | *#scene #panel #particles #assets* | 10 |
| [docs/SCENE_PANEL_INTERACTION.md](../docs/SCENE_PANEL_INTERACTION.md) | Scene panel interaction states | *#scene #panel #assets* | 7 |
| [docs/SCENE_PANEL_LIFECYCLE.md](../docs/SCENE_PANEL_LIFECYCLE.md) | Scene panel lifecycle | *#scene #panel* | 2 |
| [docs/SCENE_PANEL_ORCHESTRATION.md](../docs/SCENE_PANEL_ORCHESTRATION.md) | Scene panel initialization and script controls | *#scripts #scene #panel #assets* | 3 |
| [docs/SCENE_PANEL_RIG_SETUP.md](../docs/SCENE_PANEL_RIG_SETUP.md) | Scene panel rig setup | *#scene #panel #assets* | 4 |
| [docs/SCENE_PANEL_SUBJECT.md](../docs/SCENE_PANEL_SUBJECT.md) | ScenePanel constructor and subject helpers |  | 4 |
| [docs/SCENE_PANEL_SUBJECT_INITIALIZER.md](../docs/SCENE_PANEL_SUBJECT_INITIALIZER.md) | ScenePanel subject initializer |  | 3 |
| [docs/SCENE_PANEL_SUBJECT_STATES.md](../docs/SCENE_PANEL_SUBJECT_STATES.md) | Scene panel subject animation states | *#scene #panel #animation* | 3 |
| [docs/SCENE_PANEL_SUBJECT_TAIL.md](../docs/SCENE_PANEL_SUBJECT_TAIL.md) | ScenePanel subject final state and driver |  | 4 |
| [docs/SCENE_RANGE_WATCH.md](../docs/SCENE_RANGE_WATCH.md) | Scene range watcher (tentative) | *#scripts #scene* | 2 |
| [docs/SCENE_STATE_QUERIES.md](../docs/SCENE_STATE_QUERIES.md) | Scene state, spatial queries and pair records (tentative) | *#actors #scene* | 4 |
| [docs/SCRIPT_TRACING.md](../docs/SCRIPT_TRACING.md) | Script calls in a running game | *#scripts #actors #tooling* | 7 |
| [docs/SHIFTABILITY.md](../docs/SHIFTABILITY.md) | Shiftable data: how it was proven | #tooling #assets | 1 |
| [docs/SOLAR_SENSOR.md](../docs/SOLAR_SENSOR.md) | Cartridge solar sensor: code and runtime evidence | *#hardware* | 5 |
| [docs/SPARK_PARTICLE.md](../docs/SPARK_PARTICLE.md) | Moving tile particles (tentative identity) | *#particles #assets* | 3 |
| [docs/SPRITE_EFFECT_CONTROLLER.md](../docs/SPRITE_EFFECT_CONTROLLER.md) | Sprite effect controllers (tentative) | *#assets* | 2 |
| [docs/SPRITE_EFFECT_STEERING_ACTORS.md](../docs/SPRITE_EFFECT_STEERING_ACTORS.md) | Sprite/effect controllers and steering actors (0806B2C4–0806CA3C) | *#actors #assets* | 13 |
| [docs/STACKED_ACTOR.md](../docs/STACKED_ACTOR.md) | Stacked display actor (tentative identity) | *#actors #particles #assets* | 5 |
| [docs/TERRAIN_EDGE_MARKER.md](../docs/TERRAIN_EDGE_MARKER.md) | Terrain edge marker manager (tentative identity) | *#scripts* | 4 |
| [docs/UNIT_WORKER.md](../docs/UNIT_WORKER.md) | Subsystem unit worker | *#text* | 4 |
| [docs/WORKER.md](../docs/WORKER.md) | Worker playbook: matching with scope | #matching #process | 7 |
| [docs/tasks/m4a.md](../docs/tasks/m4a.md) | Task: lift the MP2K (m4a) sound library from pokeemerald | *#sound #library* | 3 |
| [docs/tasks/relatives.md](../docs/tasks/relatives.md) | Unmatched functions with matched relatives | *#tooling* | 35 |
| [docs/tasks/script_targets.md](../docs/tasks/script_targets.md) | Decomp targets: the code the scripts call | *#scripts #assets #tooling* | 2 |
| [docs/history/2026-10-12.md](../docs/history/2026-10-12.md) | Handoff history: 2026-10-12 | #actors #animation #assets #hardware #inventory #library #menu #panel #particles #scene #scripts #sound #text #tooling #wireless | 49 |
| [docs/history/2026-10-13.md](../docs/history/2026-10-13.md) | Handoff history: 2026-10-13 | #actors #assets #inventory #menu #particles #scene #scripts #text #tooling #wireless | 13 |
| [docs/history/2026-10-14.md](../docs/history/2026-10-14.md) | Handoff history: 2026-10-14 | #actors #animation #assets #hud #inventory #library #menu #panel #particles #scene #scripts #sound #tether #text #tooling #wireless | 83 |
| [docs/history/2026-10-15.md](../docs/history/2026-10-15.md) | Handoff history: 2026-10-15 | #actors #animation #assets #inventory #particles #scene #scripts #sound #text #tooling | 41 |
| [docs/history/2026-10-16.md](../docs/history/2026-10-16.md) | Handoff history: 2026-10-16 | #actors #scripts #text #tooling | 5 |
| [docs/history/2026-10-17.md](../docs/history/2026-10-17.md) | Handoff history: 2026-10-17 | #actors #scripts | 0 |
| [docs/history/2026-10-18.md](../docs/history/2026-10-18.md) | 2026-10-18 | #actor-stats #assets #collision #foundry #history #input #inventory #matching #player #route #scene-actor #scene-hud #scripts #state-actor #stats | 5 |
| [docs/history/2026-10-19.md](../docs/history/2026-10-19.md) | 2026-10-19.md | #history #scripts #tooling | 1 |
| [docs/history/2026-10-23-cx-sol08.md](../docs/history/2026-10-23-cx-sol08.md) | 2026-10-23 — cx-sol08 actor campaign batch | #actors #compiler-debugger #matching #mimic #particles #register-pinning #weather | 2 |
| [docs/history/2026-10-23.md](../docs/history/2026-10-23.md) | Campaign sol05 session 2 wrap-up | #actors #animation #axe #bosses #camera #campaign #charge #compiler #effect-pool #effects #hresvelgr #inventory #ivy #linked-effect #map-menu #matching #monster-book #monsters #mummy #nidhoggr #nonmatching #palette #particles #player #ratatosk #reactions #register-pinning #registry-f3e4 #scene-actor #scene-actors #scene-npc #screamer #script-combat-actor #singleton-slot7 #slime #spider #sprite-effects #tentacle #terrain #vanargand #wireless | 19 |
| [docs/history/2026-10-27.md](../docs/history/2026-10-27.md) | Work history: 2026-10-27 | #matching #ram | 0 |
| [docs/history/2026-10-early-state.md](../docs/history/2026-10-early-state.md) | Handoff history: state as of October 9-12 | #actors #assets #hardware #inventory #library #menu #process #scripts #sound #tooling #wireless | 22 |
| [docs/history/HANDOFF_2026-10-23.md](../docs/history/HANDOFF_2026-10-23.md) | Handoff: current state and how to continue | #process | 14 |
| [docs/history/MANAGER_HANDOFF_2026-10-19.md](../docs/history/MANAGER_HANDOFF_2026-10-19.md) | Manager handoff (2026-10-19) | #process #tooling | 6 |
| [docs/history/README.md](../docs/history/README.md) | Handoff history |  | 17 |
| [docs/history/WORKER_2026-10-24.md](../docs/history/WORKER_2026-10-24.md) | Worker brief: family-oriented matching |  | 27 |
| [docs/history/WORKER_ROUND_2026-10-12.md](../docs/history/WORKER_ROUND_2026-10-12.md) | Standard round brief (only when workers are authorized) |  | 3 |
| [gcl/README.md](../gcl/README.md) | GCL: the game's scripts | *#scripts #scene #assets #tooling* | 17 |
| [gcl/examples/prologue.md](../gcl/examples/prologue.md) | Worked example: the prologue, from New Game to the Sealed Prison | *#scripts #assets #tooling* | 9 |
| [gcl/examples/scale_quiz.md](../gcl/examples/scale_quiz.md) | Worked example: the Blackbeard Brothers' scale quiz | *#scripts #assets #tooling* | 10 |
| [gcl/examples/sealed_prison.md](../gcl/examples/sealed_prison.md) | Worked example: the Sealed Prison, the first dungeon | *#scripts #assets #tooling* | 6 |
| [notes/actor_animation_controller.md](../notes/actor_animation_controller.md) | Actor animation controller (081E34D0-081EAB64) | *#actors #animation #assets #tooling* | 8 |
| [notes/actor_graphics_and_basic_states.md](../notes/actor_graphics_and_basic_states.md) | Actor graphics and basic states (08021EA8-08024F2C) | #actors #animation | 8 |
| [notes/animation-contact-burst_2026-10-14.md](../notes/animation-contact-burst_2026-10-14.md) | Temporary animations and contact-burst effect, October 14 | *#particles #animation* | 7 |
| [notes/area-effect-link_2026-10-14.md](../notes/area-effect-link_2026-10-14.md) | Link variant of area effects (Sol, 2026-10-14) |  | 2 |
| [notes/area-effect_2026-10-14.md](../notes/area-effect_2026-10-14.md) | Rectangular area effect manager (Sol, 2026-10-14) | *#tooling* | 4 |
| [notes/arm-08000430.md](../notes/arm-08000430.md) | sub_08000430: unpinned exact match, and how it works | #matching #compiler #register-pinning #arm | 6 |
| [notes/bk.md](../notes/bk.md) | bk notes (backlog retry, 45 matches) |  | 5 |
| [notes/breakable-prop_2026-10-14.md](../notes/breakable-prop_2026-10-14.md) | Breakable prop, 080515C0–08051A3C | *#particles* | 2 |
| [notes/cell-gates_2026-10-14.md](../notes/cell-gates_2026-10-14.md) | Cell-based traversal gates and the preceding script entry | *#scripts* | 4 |
| [notes/cgb.md](../notes/cgb.md) | CgbSound (0x08230AC8) - matched (register-pinning concession) | *#sound* | 3 |
| [notes/cloud-08084F44.md](../notes/cloud-08084F44.md) | Cloud task 08084F44 (Bat, unassigned functions) | #matching #cloud | 1 |
| [notes/cloud-080B7C10.md](../notes/cloud-080B7C10.md) | Cloud task 080B7C10 (Skeleton states) |  | 1 |
| [notes/cloud-080BFCC4.md](../notes/cloud-080BFCC4.md) | Cloud task 080BFCC4 | *#actors* | 1 |
| [notes/cloud-080C8774.md](../notes/cloud-080C8774.md) | Cloud task 080C8774 (Golem states) |  | 2 |
| [notes/cloud-080D2194.md](../notes/cloud-080D2194.md) | Cloud task 080D2194 |  | 1 |
| [notes/cloud-080D5778.md](../notes/cloud-080D5778.md) | Cloud task 080D5778 |  | 1 |
| [notes/cloud-080E0750.md](../notes/cloud-080E0750.md) | Cloud task 080E0750 (Dog states, scene-actor defeat) | *#actors #scene* | 2 |
| [notes/cloud-08122DF4.md](../notes/cloud-08122DF4.md) | Cloud task 08122DF4 | *#particles #animation #assets* | 15 |
| [notes/cloud-08132904.md](../notes/cloud-08132904.md) | Cloud task 08132904 |  | 1 |
| [notes/cloud-08132D2C.md](../notes/cloud-08132D2C.md) | Cloud task 08132D2C |  | 1 |
| [notes/cloud-0819E7F0_2026-10-19.md](../notes/cloud-0819E7F0_2026-10-19.md) | Cloud task 0819E7F0: Hresvelgr burst phases (2026-10-19) | #matching #hresvelgr | 2 |
| [notes/cloud-081AA438.md](../notes/cloud-081AA438.md) | Cloud task 081AA438 (Vanargand phase scripts) | *#scripts* | 3 |
| [notes/cloud-08228DB8.md](../notes/cloud-08228DB8.md) | Cloud task 08228DB8: SunClock_GetPhase (and Camera_SetPosition) |  | 3 |
| [notes/cloud-08231A0C.md](../notes/cloud-08231A0C.md) | Cloud task 08231A0C | *#animation #assets* | 1 |
| [notes/codex-high.md](../notes/codex-high.md) | Codex high: task 0808AF38 | #matching #ghoul #slime #register-pinning | 244 |
| [notes/codex-low.md](../notes/codex-low.md) | Task 080D812C — 2026-10-20 | #matching #dog | 376 |
| [notes/codex-medium.md](../notes/codex-medium.md) | Codex medium worker: task 080D6FDC | #matching #scene-actor #terrain #mummy #dog | 476 |
| [notes/compiler-debugger-2026-10-23.md](../notes/compiler-debugger-2026-10-23.md) | Compiler debugger: first session (2026-10-23) | #matching #tooling #regalloc | 12 |
| [notes/contact-actor-pool_2026-10-14.md](../notes/contact-actor-pool_2026-10-14.md) | Contact actor pool setup and display (tentative) | *#actors* | 7 |
| [notes/contact-actor-states_2026-10-14.md](../notes/contact-actor-states_2026-10-14.md) | Contact actor states and lifecycle, 08042E18–08043F64 | *#actors #tooling* | 5 |
| [notes/contact-sequence-actor_2026-10-14.md](../notes/contact-sequence-actor_2026-10-14.md) | Sequence contact actor pool, October 14 | *#actors* | 6 |
| [notes/contact-swarm_2026-10-14.md](../notes/contact-swarm_2026-10-14.md) | Contact swarm, 08043F64–08044C80 | *#assets* | 4 |
| [notes/countdown-timer_2026-10-14.md](../notes/countdown-timer_2026-10-14.md) | Singleton countdown timer |  | 2 |
| [notes/cx-astra1.md](../notes/cx-astra1.md) | Campaign worker astra1 | #matching #monsters #register-pinning | 7 |
| [notes/cx-astra2.md](../notes/cx-astra2.md) | Campaign worker astra2 | #matching #monsters #actors | 4 |
| [notes/cx-astra3.md](../notes/cx-astra3.md) | Campaign worker astra3 | #matching #nonmatching #golem #scene-actors | 3 |
| [notes/cx-sol01.md](../notes/cx-sol01.md) | cx-sol01 campaign | #player #matching | 19 |
| [notes/cx-sol02.md](../notes/cx-sol02.md) | Campaign worker sol02 | #matching #vanargand | 23 |
| [notes/cx-sol03.md](../notes/cx-sol03.md) | cx-sol03 campaign | #matching #armor #campaign | 19 |
| [notes/cx-sol04.md](../notes/cx-sol04.md) | cx-sol04 campaign | #matching #player #cue #animation #campaign | 23 |
| [notes/cx-sol05.md](../notes/cx-sol05.md) | Campaign worker sol05 | #matching #mummy #campaign | 23 |
| [notes/cx-sol06.md](../notes/cx-sol06.md) | cx-sol06 campaign notes | #matching #hresvelgr | 22 |
| [notes/cx-sol07.md](../notes/cx-sol07.md) | Campaign worker sol07 | #matching #ratatosk #particles #nonmatching #register-pinning | 15 |
| [notes/cx-sol08.md](../notes/cx-sol08.md) | cx-sol08 campaign notes | #matching #actors #particles | 18 |
| [notes/cx-sol09.md](../notes/cx-sol09.md) | Campaign worker sol09 | #matching #axe #scene-actor | 16 |
| [notes/cx-sol10.md](../notes/cx-sol10.md) | cx-sol10 campaign notes | #matching #scene-actors #register-pinning | 14 |
| [notes/cx-sol11.md](../notes/cx-sol11.md) | Campaign worker sol11 | #matching #actors | 13 |
| [notes/cx-xhigh.md](../notes/cx-xhigh.md) | Campaign worker xhigh | #matching #monsters #compiler #register-pinning | 9 |
| [notes/defeat-stats-template.md](../notes/defeat-stats-template.md) | Defeat-statistics handlers: how sub_08132D2C (5740 bytes) was matched | #matching | 7 |
| [notes/depin-manual-2_2026-10-21.md](../notes/depin-manual-2_2026-10-21.md) | depin-manual-2 worklog (2026-10-21) | *#tooling* | 8 |
| [notes/depin-manual-3_2026-10-21.md](../notes/depin-manual-3_2026-10-21.md) | depin-manual-3 worklog (2026-10-21) | *#actors #tooling* | 7 |
| [notes/depin-manual-4a_2026-10-21.md](../notes/depin-manual-4a_2026-10-21.md) | depin-manual-4a worklog (2026-10-21) | *#tooling* | 6 |
| [notes/depin-manual-4b_2026-10-21.md](../notes/depin-manual-4b_2026-10-21.md) | depin-manual-4b worklog (2026-10-21) | *#scripts* | 3 |
| [notes/depin-manual-5a_2026-10-21.md](../notes/depin-manual-5a_2026-10-21.md) | depin-manual-5a worklog (2026-10-21) |  | 1 |
| [notes/depin-manual-5c_2026-10-21.md](../notes/depin-manual-5c_2026-10-21.md) | depin-manual-5c worklog (2026-10-21) |  | 1 |
| [notes/depin-manual_2026-10-21.md](../notes/depin-manual_2026-10-21.md) | depin-manual worklog (2026-10-21) | *#tooling* | 6 |
| [notes/depin-search-2026-10-23.md](../notes/depin-search-2026-10-23.md) | Faster depin search: measurements and ideas (2026-10-23) | *#tooling* | 10 |
| [notes/dialogue-vm_2026-10-12.md](../notes/dialogue-vm_2026-10-12.md) | Dialogue interpreter and scrolling controller |  | 2 |
| [notes/directional-force_2026-10-14.md](../notes/directional-force_2026-10-14.md) | Directional force controller, 080510B8–080515C0 |  | 2 |
| [notes/display-track_2026-10-14.md](../notes/display-track_2026-10-14.md) | Script-controlled display tracks | *#scripts* | 4 |
| [notes/effects-background-flash_2026-10-14.md](../notes/effects-background-flash_2026-10-14.md) | Transient sprite effects and BG1 flash controller, October 14 | *#assets* | 7 |
| [notes/encoded-records_2026-10-14.md](../notes/encoded-records_2026-10-14.md) | Encoded records and password callers, 08044C80–080454C0 | *#menu #tooling* | 5 |
| [notes/far-branch-tails.md](../notes/far-branch-tails.md) | Far-branch tails: big functions that were split in pieces | #matching #tooling | 3 |
| [notes/foundry-actor-route-behavior_2026-10-18.md](../notes/foundry-actor-route-behavior_2026-10-18.md) | Actor route and behavior foundry pass | #matching #foundry #scene-actor #route | 2 |
| [notes/foundry-actor-stats-style_2026-10-18.md](../notes/foundry-actor-stats-style_2026-10-18.md) | Actor stats, preview palette and style foundry pass | #matching #foundry #actor-stats #scene-actor | 2 |
| [notes/foundry-display-resources_2026-10-18.md](../notes/foundry-display-resources_2026-10-18.md) | Foundry display resource functions, 2026-10-18 | #matching #assets | 2 |
| [notes/foundry-hud-page-input_2026-10-18.md](../notes/foundry-hud-page-input_2026-10-18.md) | HUD page input and script configuration foundry pass | #foundry #matching #scene-hud #input #scripts | 1 |
| [notes/foundry-inventory-stat-previews_2026-10-18.md](../notes/foundry-inventory-stat-previews_2026-10-18.md) | Inventory stat-preview foundry pass | #foundry #matching #scene-hud #inventory | 2 |
| [notes/foundry-player-action-phases_2026-10-18.md](../notes/foundry-player-action-phases_2026-10-18.md) | Player action phases foundry run (2026-10-18) | #player #matching #foundry | 2 |
| [notes/foundry-player-action-stats_2026-10-18.md](../notes/foundry-player-action-stats_2026-10-18.md) | Foundry player action and stat candidates | #player #matching #foundry #stats | 2 |
| [notes/foundry-player-contact_2026-10-18.md](../notes/foundry-player-contact_2026-10-18.md) | Foundry player/contact worker, 2026-10-18 | #player #collision #foundry | 2 |
| [notes/foundry-player-terrain-camera_2026-10-18.md](../notes/foundry-player-terrain-camera_2026-10-18.md) | Player terrain, force, particles and camera foundry pass | #player #foundry #matching | 2 |
| [notes/foundry-scene-actor-phases_2026-10-18.md](../notes/foundry-scene-actor-phases_2026-10-18.md) | Foundry scene-actor phase run, 2026-10-18 | #matching #scene-actor #foundry | 2 |
| [notes/foundry-scene-hud-action-grid_2026-10-18.md](../notes/foundry-scene-hud-action-grid_2026-10-18.md) | Scene HUD action grid foundry candidates | #foundry #matching #scene-hud #input | 3 |
| [notes/foundry-scene-npc-callbacks_2026-10-18.md](../notes/foundry-scene-npc-callbacks_2026-10-18.md) | Foundry: scene NPC callbacks, 2026-10-18 | #matching #foundry #state-actor | 2 |
| [notes/grid_attack_actor.md](../notes/grid_attack_actor.md) | Grid attack actor unit consolidation | #grid-attack-actor #unit #matching #080297E8 #0802C678 | 5 |
| [notes/interaction-marker_2026-10-14.md](../notes/interaction-marker_2026-10-14.md) | Interaction marker, 080537D0–08053D8C |  | 3 |
| [notes/ivy-matching.md](../notes/ivy-matching.md) | Ivy (080ED914..080F0B8C): matching notes | #matching | 2 |
| [notes/iwram-layouts_2026-10-27.md](../notes/iwram-layouts_2026-10-27.md) | Shared IWRAM object layouts (2026-10-27) | #matching #tooling #ram | 7 |
| [notes/large_scene_animation_controller.md](../notes/large_scene_animation_controller.md) | Large scene animation controller = Nidhoggr boss (081915E0-0819792C) | *#scene #animation* | 5 |
| [notes/link-field2.md](../notes/link-field2.md) | Field objects and hazards: open near misses (wip/field2) | *#particles #animation #assets* | 3 |
| [notes/link-menus_2026-10-12.md](../notes/link-menus_2026-10-12.md) | Wireless menus and result panels | *#panel #menu #wireless* | 4 |
| [notes/matching-idioms-batch_2026-10-19.md](../notes/matching-idioms-batch_2026-10-19.md) | Matching idioms from the 2026-10-19 cloud batch | #matching #agbcc #techniques | 3 |
| [notes/mgs-gcl-round1.md](../notes/mgs-gcl-round1.md) | MGS/GCL matching round 1 | *#scripts #matching* | 3 |
| [notes/mgs-gcl-round2.md](../notes/mgs-gcl-round2.md) | MGS/GCL matching round 2 | *#scripts #matching* | 2 |
| [notes/mgs-gcl-round3.md](../notes/mgs-gcl-round3.md) | MGS/GCL matching round 3 | *#scripts #matching* | 2 |
| [notes/mgs-gcl-round4.md](../notes/mgs-gcl-round4.md) | MGS/GCL matching round 4 | *#scripts #matching* | 2 |
| [notes/mgs-gcl-round5.md](../notes/mgs-gcl-round5.md) | MGS/GCL matching round 5 | *#scripts #matching* | 1 |
| [notes/mgs-gcl-round6.md](../notes/mgs-gcl-round6.md) | MGS/GCL matching round 6 | *#scripts #matching* | 2 |
| [notes/mgs-gcl-round7.md](../notes/mgs-gcl-round7.md) | MGS/GCL matching round 7 | *#scripts #matching* | 1 |
| [notes/modifier-display_2026-10-14.md](../notes/modifier-display_2026-10-14.md) | Modifier menu graphics and input (tentative) | *#menu* | 4 |
| [notes/modifier-menu_2026-10-14.md](../notes/modifier-menu_2026-10-14.md) | Modifier selection and stat previews (tentative) | *#menu #process* | 6 |
| [notes/modifier-preview_2026-10-14.md](../notes/modifier-preview_2026-10-14.md) | Modifier menu preview and lifecycle (tentative) | *#actors #menu #process* | 6 |
| [notes/modifier_selection_and_stat_previews.md](../notes/modifier_selection_and_stat_previews.md) | Modifier selection and stat previews (tentative) | #modifiers #menu #matching #unit-consolidation | 4 |
| [notes/orbit-background_2026-10-14.md](../notes/orbit-background_2026-10-14.md) | Orbit-positioned background controller |  | 4 |
| [notes/orbit-number-display_2026-10-14.md](../notes/orbit-number-display_2026-10-14.md) | Rotating contact actor and floating numbers, October 14 | *#actors* | 7 |
| [notes/orbiting_particles_2026-10-18.md](../notes/orbiting_particles_2026-10-18.md) | Orbiting particles unit consolidation (2026-10-18) | #particles #types #matching | 3 |
| [notes/palette-level-transitions_2026-10-18.md](../notes/palette-level-transitions_2026-10-18.md) | Palette level transitions unit (2026-10-18) | #palette #mosaic #unit-consolidation | 3 |
| [notes/parameterized-motion-pool_2026-10-14.md](../notes/parameterized-motion-pool_2026-10-14.md) | Parameterized motion pool, 080454C0–08045A20 |  | 5 |
| [notes/pilot-haiku.md](../notes/pilot-haiku.md) | pilot-haiku notes |  | 1 |
| [notes/pilot-sonnet.md](../notes/pilot-sonnet.md) | pilot-sonnet.md |  | 0 |
| [notes/player_3.md](../notes/player_3.md) | Player 3 unit consolidation | #player #matching #types | 7 |
| [notes/projected-display_2026-10-14.md](../notes/projected-display_2026-10-14.md) | Five-entry projected display, 08045A20–08045D18 | *#animation #assets* | 4 |
| [notes/proximity-emitter_2026-10-14.md](../notes/proximity-emitter_2026-10-14.md) | Proximity emitter, 08051A3C–08052130 | *#particles* | 3 |
| [notes/proximity-icon_2026-10-14.md](../notes/proximity-icon_2026-10-14.md) | Player-proximity keyframe icon |  | 2 |
| [notes/push20-astra-e013c.md](../notes/push20-astra-e013c.md) | Push20 Astra 081E013C escalation |  | 2 |
| [notes/push20-high-last.md](../notes/push20-high-last.md) | Final high short pass |  | 1 |
| [notes/push20-high.md](../notes/push20-high.md) | Push20 high worker |  | 7 |
| [notes/push20-low.md](../notes/push20-low.md) | Push20 low range |  | 14 |
| [notes/push20-mid.md](../notes/push20-mid.md) | Push20 mid-range worker | *#text* | 10 |
| [notes/push20-root.md](../notes/push20-root.md) | Push toward 20%: parent matches |  | 8 |
| [notes/repeated-effect_2026-10-14.md](../notes/repeated-effect_2026-10-14.md) | Contiguous repeated effect cohort (Sol, 2026-10-14) |  | 5 |
| [notes/research-handoff-2026-10-23.md](../notes/research-handoff-2026-10-23.md) | Research handoff: making matching cheaper (2026-10-23) | *#matching #process* | 7 |
| [notes/resource-station_2026-10-14.md](../notes/resource-station_2026-10-14.md) | Stored-resource station (Sol, 2026-10-14) | *#assets #tooling* | 3 |
| [notes/root-batch6.md](../notes/root-batch6.md) | Main-thread batch 6 evidence | *#scene #hardware #tooling* | 2 |
| [notes/round1-luna-a.md](../notes/round1-luna-a.md) | Luna A production round |  | 2 |
| [notes/round1-luna-b.md](../notes/round1-luna-b.md) | Production matching round 1 — Luna B | *#matching* | 1 |
| [notes/round1-sol.md](../notes/round1-sol.md) | Production matching round 1 — Sol | *#matching* | 4 |
| [notes/round2-luna-a.md](../notes/round2-luna-a.md) | Luna A production round 2 |  | 2 |
| [notes/round2-luna-b.md](../notes/round2-luna-b.md) | Luna B production round 2 |  | 1 |
| [notes/round2-sol.md](../notes/round2-sol.md) | Sol production round 2 |  | 5 |
| [notes/round3-luna-a.md](../notes/round3-luna-a.md) | Luna A round 3 notes |  | 1 |
| [notes/round3-luna-b.md](../notes/round3-luna-b.md) | Luna B round 3 |  | 1 |
| [notes/round3-sol.md](../notes/round3-sol.md) | Sol production round 3 |  | 5 |
| [notes/round4-luna-b.md](../notes/round4-luna-b.md) | Round 4 — Luna B |  | 2 |
| [notes/round4-sol.md](../notes/round4-sol.md) | Sol production round 4 |  | 3 |
| [notes/round5-sol.md](../notes/round5-sol.md) | Sol production round 5 |  | 3 |
| [notes/round6-sol.md](../notes/round6-sol.md) | Production round 6 — Sol |  | 3 |
| [notes/s0.md](../notes/s0.md) | s0 notes | *#tooling* | 11 |
| [notes/s1.md](../notes/s1.md) | s1 notes | *#tooling* | 12 |
| [notes/s2.md](../notes/s2.md) | s2 notes (range 080C9844-0813B134) |  | 11 |
| [notes/s3.md](../notes/s3.md) | s3 notes |  | 18 |
| [notes/s4.md](../notes/s4.md) | s4 notes |  | 21 |
| [notes/s5.md](../notes/s5.md) | s5 notes | *#assets* | 13 |
| [notes/saved-position-marker_2026-10-14.md](../notes/saved-position-marker_2026-10-14.md) | Saved-position marker, 08053D8C–08054294 |  | 2 |
| [notes/scene-actor-twenty_2026-10-12.md](../notes/scene-actor-twenty_2026-10-12.md) | Scene actors and dialogue: reaching 20% | *#actors #scene* | 3 |
| [notes/scene-arc-effects_2026-10-13.md](../notes/scene-arc-effects_2026-10-13.md) | Scene arc slots and projected ground effects (tentative) | *#scene* | 4 |
| [notes/scene-panel-background_2026-10-14.md](../notes/scene-panel-background_2026-10-14.md) | Scene panel background effects and overlay setup | *#scene #panel* | 2 |
| [notes/scene-panel-rig-states_2026-10-14.md](../notes/scene-panel-rig-states_2026-10-14.md) | Scene panel rig states and animation (tentative identities) | *#scene #panel #animation* | 6 |
| [notes/scene-panel-rig_2026-10-14.md](../notes/scene-panel-rig_2026-10-14.md) | Scene panel rig and contact/movement contracts (tentative) | *#scene #panel #assets* | 4 |
| [notes/scene-panel-schedule_2026-10-14.md](../notes/scene-panel-schedule_2026-10-14.md) | Scene panel scheduled actions (tentative) | *#scene #panel* | 2 |
| [notes/scene-panel-states_2026-10-14.md](../notes/scene-panel-states_2026-10-14.md) | Scene panel state and background controls (tentative) | *#scene #panel* | 2 |
| [notes/scene-pool-states_2026-10-13.md](../notes/scene-pool-states_2026-10-13.md) | Scene pool state family: second contiguous batch | *#scene* | 6 |
| [notes/scene-pool_2026-10-13.md](../notes/scene-pool_2026-10-13.md) | Scene pool and actor controls, 0803CDF0–0803EE58 | *#actors #scene #tooling* | 5 |
| [notes/scene_panel_control_and_lifecycle.md](../notes/scene_panel_control_and_lifecycle.md) | Scene panel control and lifecycle unit | #scene #panel #matching #types | 5 |
| [notes/script-entry-points_2026-10-16.md](../notes/script-entry-points_2026-10-16.md) | Script entry points, batches 1-9 (2026-10-16) | #scripts #text #actors #tooling | 23 |
| [notes/script-keyframe-pool_2026-10-14.md](../notes/script-keyframe-pool_2026-10-14.md) | Script keyframe pool | *#scripts* | 2 |
| [notes/script-selector-display_2026-10-14.md](../notes/script-selector-display_2026-10-14.md) | Script selector and shared display animation | *#scripts #animation* | 7 |
| [notes/script-utility_2026-10-14.md](../notes/script-utility_2026-10-14.md) | Script utility commands and reference ABI | *#scripts #inventory* | 3 |
| [notes/script_virtual_machine.md](../notes/script_virtual_machine.md) | Script virtual machine, merged unit | *#scripts* | 8 |
| [notes/selection_menu.md](../notes/selection_menu.md) | Selection menu unit (tentative) | #selection-menu #translation-unit #matching | 5 |
| [notes/shake-icon-display_2026-10-14.md](../notes/shake-icon-display_2026-10-14.md) | Short-lived shaking keyframe icon |  | 2 |
| [notes/sol-family-push-2.md](../notes/sol-family-push-2.md) | Sol family push 2 |  | 3 |
| [notes/sol-family-push-3.md](../notes/sol-family-push-3.md) | Sol family push 3 |  | 8 |
| [notes/sol-family-push-4.md](../notes/sol-family-push-4.md) | Sol family push 4 |  | 2 |
| [notes/sol-family-push-5.md](../notes/sol-family-push-5.md) | Sol family push 5 | *#wireless #library* | 7 |
| [notes/sol-family-push-6.md](../notes/sol-family-push-6.md) | Sol family push 6 | *#wireless #library* | 3 |
| [notes/sol-family-push.md](../notes/sol-family-push.md) | Sol family push |  | 7 |
| [notes/solo-family_2026-10-09.md](../notes/solo-family_2026-10-09.md) | Solo family tranche, October 9 |  | 5 |
| [notes/sonnet-081EEECC-task.md](../notes/sonnet-081EEECC-task.md) | Task 081EEECC worker notes |  | 1 |
| [notes/sonnet-from-end-08241504.md](../notes/sonnet-from-end-08241504.md) | sub_0824261C (Player update by kind) — unfinished |  | 1 |
| [notes/stage-icon-display_2026-10-14.md](../notes/stage-icon-display_2026-10-14.md) | Staged keyframe icon display |  | 3 |
| [notes/staged-effect_2026-10-14.md](../notes/staged-effect_2026-10-14.md) | Contiguous staged effect subsection (Sol, 2026-10-14) | *#tooling* | 4 |
| [notes/sub_082392E8.md](../notes/sub_082392E8.md) | sub_082392E8.md | *#particles* | 1 |
| [notes/subsystem-run_2026-10-25.md](../notes/subsystem-run_2026-10-25.md) | Subsystem scope run, 2026-10-25/26 | #process #tooling #matching | 4 |
| [notes/tether-emission-palette_2026-10-14.md](../notes/tether-emission-palette_2026-10-14.md) | Tether actor emissions, palette and movement effects, October 14 | *#actors #tether #assets* | 8 |
| [notes/tether-initialization_2026-10-14.md](../notes/tether-initialization_2026-10-14.md) | Tether initialization, charge and flagged terrain | *#actors #tether* | 7 |
| [notes/tether-interaction_2026-10-14.md](../notes/tether-interaction_2026-10-14.md) | Tether actor entry, interaction and script controls | *#scripts #actors #tether* | 5 |
| [notes/tether-roaming-states_2026-10-14.md](../notes/tether-roaming-states_2026-10-14.md) | Tether roaming, recovery and lifecycle states | *#actors #tether* | 6 |
| [notes/tether-world-states_2026-10-14.md](../notes/tether-world-states_2026-10-14.md) | Tether world position, player boundary and state callbacks | *#actors #tether* | 6 |
| [notes/tether_actor.md](../notes/tether_actor.md) | Tether actor unit consolidation | #tether-actor #unit #matching #types | 5 |
| [notes/w0.md](../notes/w0.md) | w0 notes | *#tooling* | 5 |
| [notes/w1.md](../notes/w1.md) | w1 notes (range 0803CC90-0806FC64) |  | 2 |
| [notes/w2.md](../notes/w2.md) | w2.md | *#tooling* | 2 |
| [notes/w3.md](../notes/w3.md) | w3 notes | *#text #tooling* | 3 |
| [notes/w4.md](../notes/w4.md) | w4 notes | *#tooling* | 3 |
| [notes/w5.md](../notes/w5.md) | w5 notes |  | 2 |
| [notes/w6.md](../notes/w6.md) | w6 notes | *#tooling* | 2 |
| [notes/w7.md](../notes/w7.md) | w7.md | *#tooling* | 1 |
| [notes/windows-wireless_2026-10-12.md](../notes/windows-wireless_2026-10-12.md) | Text windows, dialogue transitions and wireless controller | *#wireless #text* | 2 |
| [notes/wireless-controller_2026-10-12.md](../notes/wireless-controller_2026-10-12.md) | Wireless connection controller | *#inventory #wireless* | 4 |
| [notes/wireless-tether_2026-10-14.md](../notes/wireless-tether_2026-10-14.md) | Wireless link indicator and tether actor, October 14 | *#actors #wireless #tether #assets* | 7 |
| [notes/wireless_game_items_and_display.md](../notes/wireless_game_items_and_display.md) | Wireless game items and display (tentative) | #wireless #matching #types | 6 |
