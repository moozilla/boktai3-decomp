# Player 3 unit consolidation

Tags: #player #matching #types

Range: `0813C8F0–0813FC38` (end exclusive). The 53 original files were
merged into `src/player_3.c` in ROM order using `tools/merge_unit.py`.
Its `--write` removal step was adapted locally to unlink the inputs instead
of invoking `git rm`, because the driver owns git metadata and commits.
No shared documentation, tool source, generated assembly, or function-name
registry was edited. The changes are uncommitted.

## Shared types

`include/player_3.h` includes `engine.h`, reuses `player.h`, and provides
`PlayerModifierInput` (equipment selector at +0xB0) and
`PlayerModifierTarget` (target flags at +0x38). These are callback inputs,
not additional partial views of the owning Player; their exact broader
ownership remains tentative.

Extended the existing `Player` with the observed visual object pointer and
its `PlayerVisualObject` resource/parameter fields; visual parameter block,
resource selectors and flash state; alternate/effect positions; embedded
effect contexts; and event procedure. `GameState` now exposes the signed
ability selector at +0x60 and five signed target bonuses at +0x58A.
Names and effect interpretations are provisional. Existing movement-oriented
names for bytes +0x4F4..+0x4F6 and +0x5B9..+0x5BB also serve visual-effect
code here; these are shared storage, not separate layouts.

## Exact additions

All have file-local aliases, awaiting integration promotion:

| Address | Tentative name |
|---|---|
| 0813E108 | Player_GetStateModifierBonus |
| 0813E160 | Player_GetBlockedActionBonus |
| 0813E174 | Player_GetStatAModifierBonus |
| 0813E184 | Player_GetStatBModifierBonus |
| 0813E194 | Player_GetEnergyRatioModifier |
| 0813E1B8 | Player_GetLifeRatioModifier |
| 0813E284 | Player_GetScaledTargetFlagBonus |
| 0813F828 | Player_SpendEnchantmentEnergy |
| 0813FB30 | Player_InitActionRequest |

Seven missing boundaries were registered in `build_extra_functions.csv` and
`progress_extra_functions.csv`: E160, E174, E184, E194, E1B8, E284 and F828.
The progress inventory was refreshed. E108 was already registered, but its
retained blob concealed five additional independent callbacks. E284 and F828
were previously charged to adjacent matched functions; separating them does
not itself earn new bytes. The scaled target bonus uses the first qualifying
bit among target flags 7..11, arithmetic-shifts the signed saved value by six,
and caps only the upper bound at ten.

## Matching adaptations

- Direction history uses unsigned halfword storage so clearing retains the
  original load/OR/store loop. Its three existing consumers (0814433C,
  081580EC, 08158588) explicitly read signed samples; all three pass exact
  checks after the change.
- The action request is a typed five-byte view of Player +0x33C, overlapping
  the established individually named fields. It introduces no padded actor
  view. Signed kind reads remain explicit where required.
- Two GameState indexed accesses retain byte-address formation with the
  index added before the field offset; direct array indexing changes agbcc's
  register/address order. Random-table reads similarly retain byte-scaled
  indexing of the shared engine declaration.
- Reconciled local caller contracts for descriptor entry arguments, unsigned
  mode-value returns and the availability hook's unused arguments. The blocked
  action bonus forwards its incoming Player pointer to Player_IsActionBlocked.
  No engine.h prototype needed changing.
- The former isolated E0F0 old_agbcc leaf matches with the merged unit's normal
  agbcc compiler. No compiler exception, new register pins, or asm barriers
  are needed.

## Retained assembly and attempts

Near-match drafts are guarded by NONMATCHING and recorded with check.py:
0813DB60 (Player_SelectFlashingVisualMode), 82.1%, has the actor/input-mode
registers swapped; 0813F8F8 (Player_SelectRequestedActionMode), 85.0%, has
owner/request registers swapped and one mode-exit reload difference.

The other retained entries and blockers:

- 0813C9B4: palette component adjustment loop; closest port 14.7%.
- 0813CB8C / 0813CE20: large visual/resource mode selection; unrelated port
  skeletons score 2.0% / 10.6%, not useful drafts.
- 0813D178: resource/palette initialization with many branches; port 0%.
- 0813D3B0: random effect emitter constructs packed vector bitfields, including
  the compiler's read/modify/write of partially initialized stack words; its
  closest relative does not provide a complete function to port. Retained
  assembly rather than introducing uninitialized C or a false match.
- 0813D548: vector/effect creation; closest PlayerCue_ShakeEffect port 74%,
  with different effect setup and arguments; below retained-draft threshold.
- 0813D834: drafted state-timer setter; shared-field conversion still changes
  switch-derived index lifetime, saved registers, and signed byte loading.
  Several structural variants did not match; discarded from production.
- 0813D924: state-to-visual-mode policy; closest port 33.3%.
- 0813DB98: multi-state visual flag and speed transitions; closest port 10.8%.
- 0813DF5C: broad embedded-effect initialization; closest port 5.5%.
- 0813E608: action-modifier dispatch builder; closest relative explains only
  7% and is now part of the merged unit, not a complete portable source.
- 0813E90C: action/collider configuration; closest port 23.4%.
- 0813EDDC: large equipment flag reconstruction; closest relative explains
  only 2%, no complete portable candidate.
- 0813F702: internal entry into the preceding 0813EDDC routine's epilogue, with
  r5 and stacked high registers already live. It is not an independent C
  function; retain its INCLUDE_ASM entry in order. An unrelated relative port
  scored 4.2% and was rejected.

The Ghidra export is absent in this checkout. Relative candidates and failed
variants were kept only in ignored `build/player_3/`. Approximately 45 exact
check invocations covered the merge, type changes, candidates, and signed
history consumers. The default merged unit is an exact 13,128-byte range,
including the retained assembly. Two guarded drafts are not exact progress.

## Validation

- `make format` and focused clang-format validation passed.
- Final `tools/check.py src/player_3.c`: zero differing bytes across 13,128 bytes.
- All three signed-history consumers passed their individual exact checks.
- `tools/check.py src/player_3.c --nonmatching --record` recorded the two drafts.
- `tools/build.py`: **build/boktai3.gba: OK**.
- `tools/progress.py`: no loss of matched code; net batch gain is +0.015
  percentage points (360 bytes), with nine newly defined exact callbacks.

The first full-build attempts lacked this checkout's extracted `build/res`
files. They were extracted locally from the ROM through datafmt. Shared
`gen/data.s` references indexed archive filenames; matching aliases were
populated by archive table index in the local build directory. No shared
generated files were changed. The final complete build passed.
