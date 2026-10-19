# Scene panel control and lifecycle unit

Tags: #scene #panel #matching #types

Range: `0805B28C–0805CB88` (end exclusive).

Consolidated the 48 existing implementations in ROM order into
`src/scene_panel_control_and_lifecycle.c`. All 48 were already matched; this
is type recovery and consolidation, with no newly matched functions, drafts,
or gap functions. The range remains 100% matching, a gain of 0 percentage
points (6,396 retained exact bytes).

## Shared types

Reused `include/scene_panel.h`, because these are the same parent, rig and
subjects already described by that header. It now contains the combined
`ScenePanel` layout, the existing complete subject layout as
`ScenePanelSubject`, the embedded `ScenePanelRig` and its sprite type, palette
and auxiliary sprites, and command/receiver records. Removed the overlapping
interaction, configuration, command and lifecycle parent structs, and the
partial interaction/rig subject structs. Neighboring subject consumers use the
same subject tag; their code is otherwise unchanged.

The parent observations include actor ID/initial mode at 0x18/0x1A, subject
and active counts at 0x1E/0x1F, presentation callback at 0x250, palette sprite
at 0x254, saved player/tether pointers at 0x4F4/0x4F8, effect state/frame at
0x4FC/0x4FE, positions at 0x500/0x508/0x510, auxiliary sprite at 0x518, subject
slots at 0xBE8, palette mode at 0xC10, acquisition variant at 0xC16, rig at
0xC6C, notification/ramp fields at 0x1336–0x1338, warning sounds at 0x1344,
script property at 0x17AC, and receiver at 0x17B0. Parent allocation remains
0x17E4 bytes. The previous transitionSound is warningSounds[3], the same
halfword at 0x134A. Unknown regions and tentative semantics remain explicit.

## Matching adaptations and observations

- `ScenePanel_UpdateActiveSubjects` retains its indexed slot calculation using
  a byte offset from the typed subject-pointer array. Direct array indexing
  changes agbcc's induction loop and register allocation. The slot reload
  after callbacks is retained.
- `ScenePanel_CreateSubjects` retains the packed unsigned bitfield position
  temporary: its component additions truncate to 16 bits. Other packed
  position temporaries also retain their observed widths and signedness.
- Existing presentation consumer `08058700` needs signed packed-vector copies
  of the shared center position. Two explicit packed reinterpretations retain
  its original operations; other position users use `ContactPosition` fields.
- The palette initializer now uses direct typed member addresses; its previous
  manually computed member offsets were unnecessary.
- `tools/depin.py --write` retained all five existing hints across B34C, B40C
  and B8B4 (one, three and one respectively). No new pins/barriers were added.
- C754's old_agbcc override was unnecessary: ordinary agbcc produces the exact
  function, allowing one compiler for the merged unit. Retained the local
  `ScenePanel_ClearGlobal` alias and the empty-state
  `ScenePanel_HoldInteraction` alias; removed duplicate alias definitions.
- agbcc does not support anonymous union members here. Shared fields use one
  canonical name instead: effectState/effectFrame for interaction state,
  paletteMode for the mode byte, and state for the presentation callback.
- No engine prototype mismatch was encountered.

## Validation

Per-function checks passed for all 48 after initial type unification. The
merged-unit check reports zero differing bytes and a 6,396-byte exact match.
`make format` was run. `tools/progress.py` retained exactly the baseline
matched function and byte counts, including the register-pinned category.

The merge used `tools.merge_unit.merge` directly, then wrote the result and
removed the original inputs with filesystem operations: the CLI's `--write`
uses `git rm`, while this worker must leave git metadata untouched.

Rough iteration count: three layout passes, two direct-access experiments,
one merged-unit check, and the requested depinning checks. No unmatched
functions needed drafting. Full-ROM build result is recorded below.

The clean build needed local resource extraction: this worktree initially had
no `build/res/` archives. Existing `datafmt` extractors recovered them from the
ROM. The shared `gen/data.s` also referenced 578 indexed filenames absent from
a fresh extraction; these were materialized from the corresponding extracted
entry after checking each filename's hash and archive index. Only ignored
local build outputs were written; shared `gen/` and tools were untouched.
The adjacent overlay renderer was updated to the canonical origin/count/state
names; its exact check and the packed-center presentation consumer's check
both passed (1,056 and 1,012 bytes respectively).

The first completed ROM comparison exposed one remaining old-view name in the
neighboring subject update: `SCENE_PANEL_GLOBAL->mode` meant the byte at 0xC10,
not the existing core's halfword at 0x1328. Changed that access to
`paletteMode`. The candidate returned to the original size; the fast check
against the failed build's ELF differed only in the address of the relocated
indirect-call helper. Final validation uses the corrected complete build.

Final validation: `build/venv/bin/python tools/build.py` printed
`build/boktai3.gba: OK`. The corrected subject updater also passes its exact
check against the final ELF. The before/after progress reports are identical.
All changes are left uncommitted; no shared docs or git metadata were written.
