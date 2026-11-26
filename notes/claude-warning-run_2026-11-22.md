# Claude warning run, 2026-11-22 to 23

Tags: #process #phase3

An overnight run of Opus 5.5 workers by subsystem on the linter's warnings, with one Fable 5.2
attempt at the hard sites and two side projects, integrated in four batches (PRs 1056, 1058, 1059,
1060) plus the mechanical cleanup tools (PR 1057) and a last tree-wide tool pass (PR 1061). Every batch built the combined ROM
(`build/boktai3.gba: OK`).

## Numbers

| | Before (50126e814) | After (PR 1061) |
|---|---|---|
| Errors | 4 | 3 |
| Warnings, linter as it was before the run | 4,698 | not measured at the end |
| Warnings, linter as extended during the run | 5,359 | 3,607 |

The linter got stricter during the run (`~N` counts as a parked literal, number-named inline
parameters count as const-local, literal-param covers longer call-free helpers), which added 661
sites to the starting figure.

**Read the drop with care.** A large part of the fall in literal-local and literal-param is
constants that are now arguments of flag accessors, not constants that disappeared: in batch 1
alone, literal masks passed to pointer-plus-mask helpers rose by 221 call sites (524 counting struct
accessors), in batch 2 by about 156. An accessor on a recovered type that sibling code already used
for that field is a recovery; a mask moved into a generic word helper at an isolated site is steering
under another name. Known instances of the latter are the scene1 commits titled "staged flag masks
through the word-flag helpers" (12 files). `docs/WORKER.md` "Mechanical cleanups" states the rule.

## What worked

* **Type recovery before the warned line.** Every large drop came from it: `struct Ratatosk`
  (0xF30 bytes, from about 45 private copies), Hresvelgr's and Vanargand's body types, `struct
  Player`'s two embedded `struct SceneInputWidget` animation channels and its full `ContactCollider`,
  `SceneVolumeActor`, `struct Dog`, `struct Axe`, `struct LinkPanel`, raw `actor + 0x2D0` offsets
  moved onto `RoomActor`/`ChaseActor` fields. Parked constants then often match written plainly.
* **Plain loops.** Many parked `mask`/`zero`/`one` locals in the menus were steering for loops
  written as pointer walks; `for (i = 0; i < N; i++) a[i].flags |= 1;` matches in most cases.
* **Writing a single-use parked constant in place almost never matches** (10 of 509 tried by
  `tools/fold_literals.py`): the constant is not the problem, the type or helper around it is.
* **Runtime evidence.** The flight recorder's `infer` found Player's embedded animation struct from
  how other code accesses the same bytes; a worker confirmed it against the code and landed it.

## Compiler facts found

* Defining an extra `static inline` in a widely included header can change register allocation in
  units that include it without using it (`sub_082171C0.c` stopped matching when two pause helpers
  were added to `render.h`). Put new helpers in a small header only their users include, and run
  `tools/header_dependents.py ... --check` to the end.
* When the ROM loads a mask before the word it tests, the original used an inline accessor (its
  argument is set up before its body); the test written in place loads the word first.
* `*(table + index)` builds the address index-first where `table[index]` adds the array offset
  first (`RANDOM_NEXT()` in `include/link/wireless/random.h`; also game-state arrays).
* Hard sites: see [hard-sites_2026-11-23.md](hard-sites_2026-11-23.md). Two fell
  (`Text_FormatDecimal`, `SceneInput_TickChoice`); the random-table family is one instruction off
  and turns on `update_equiv_regs` in local-alloc.c.

## Source fixes from running the port against the ROM

The Autopilot's port-versus-ROM comparison found calls that passed fewer arguments than the callee
takes (97; 96 were arguments the decompiled C had dropped, 1 is under `#ifdef UBFIX`) and functions
that fall off the end and return a leftover register (28 fixed; `port/missing_returns.py` lists about
230 more, mostly with unused results). The `UBFIX` rule is in `CLAUDE.md`.

## Side projects (both in the repo)

* `tools/recorder/`: record a session on the source port with a complete typed write log, replay
  it frame-exactly, `why` (a backward slice printed as C statements with runtime values, validated
  by perturbing a cause in a replay), `diff`, `infer`, DuckDB export, a Studio `/recorder` panel.
* `tools/autopilot/`: the game plays itself on the port from typed state, terrain, scene objects
  and scripts, verified against the ROM: power-on through the first dungeon, San Miguel and the
  bike ride into the second dungeon (scene 3011, story 40). `breakers.py` answers "what can break
  this" from the weapon table. Not built: healing, game-over recovery; a port crash on the pause
  menu's swords page is open.

## Leads not taken

* Rename `struct SceneInputWidget`: it is a general animated-sprite type (213 files).
* `SceneVec` and `ContactPosition` have the same layout; 366 casts between them tree-wide.
* Player bytes 0x14C to 0x178 look like a `DisplaySequenceSprite`.
* Duplicate helpers among the headers (`Chase_SetCtl` / `SceneActorChase_SetControl`,
  `Chase_ClearBits` / `SceneActorChase_ClearBits`, `Engine_PauseBits` / `Scene_TestPauseFlags`,
  `Player_SetStatusBits` / `WordFlags_Set`); `tools/inline_twins.py` reports 252 private helpers it
  cannot place because of them, and 327 private structs with a shared layout under another name.
* `game_state.h` `unk50` is the signed selected-item row; `scene_pool.h` 0x56C is a second collider.
* The flip-bit pattern (`bit = 1` / `bit = 2`) in the inlined sequence step, about 30 files.
* `tests/test_statement_macros.py::test_progress_marks_its_users` depends on the live tree and fails.
