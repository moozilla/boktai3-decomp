# Large scene animation controller = Nidhoggr boss (081915E0-0819792C)

Despite the brief's working title, this whole range belongs to **Nidhoggr**, a 26-body-
segment dragon boss task (`Task_Create(8, 0x72FC)`). It continues the earlier
"Nidhoggr (auto, tentative)" subsystem at 0818ECAC-0818F678 (a different unit); that
earlier range was not touched here.

Unit: `src/large_scene_animation_controller.c`, header `include/large_scene_animation_controller.h`
(`struct Nidhoggr`, 0x72FC bytes). 130 functions in ROM order, 120 matching (praise be),
10 left as `INCLUDE_ASM` (see below).

## Struct highlights

- `struct Task task` (engine.h) + `struct SceneEventReceiver receiver` (scene_event_dispatch.h)
  for the standard task/message-queue header.
- 26 `rows`/`colliders`/`altColliders`/`partExtra` arrays: one per body segment. Only 9 of
  them (indices used by `hp`/`dead`/`shuffleOrder`) track independent hit points; the rest
  are purely cosmetic (tail segments etc).
- `mode` (0x7249) selects one of the boss's behaviors via the table `gUnk_0824F5C0`
  (entrance, two attack phases, retreat, several cutscene-only reactions, ...); `state`
  (0x724A) is the sub-state within whichever behavior is active.
- Two independent frame counters, `timer` (0x7244) and `timer2` (0x7246), used by
  different subsets of the per-behavior Update functions — confirmed by two independently
  already-matching functions disagreeing on which offset holds "the" timer; both are real,
  separate fields.
- `maxHp` is at 0x726A (option `-g`, default 100), *not* 0x726C (option `-Y`, default 1000,
  purpose unresolved, kept as `optionY`) — confirmed the same way, by Nidhoggr_Init,
  Nidhoggr_InitFirstPhaseParts and Nidhoggr_ResetAllPartHealth agreeing on 0x726A after a
  first pass had them conflicting.
- `posOffsetX`/`swayX`/`posOffsetZ` + matching `*Velocity` fields (0x72A8-0x72C0): a
  generic "coarse motion + small sway" integrator reused by nearly every movement/behavior
  function (UpdateEntrance, UpdateFirstPartPhase, UpdateEffectAttack, ...); `Nidhoggr_UpdateAnchoredSway`/
  `UpdateHorizontalSway`/`UpdateWideSway` are the shared step functions.
- `script` (0x72C0) doubles as the raw value of script option `-e` (ReadOptions) and, for
  cutscene-created instances, a `Script_Call` key run when `UpdateDepartureTransition`
  finishes (confidence=low on the dual meaning, but both reads are at the same offset).

## MATCHING_CONCESSION / register pins

None used.

## Gaps left as INCLUDE_ASM (confidence=low throughout)

- `sub_0819321C` (760B): per-frame part-position/state driver called from Update; a
  function-pointer table (`gUnk_0824F364`) keyed by a per-part byte, plus
  `Collider_SetPosition` for all 26 colliders from offset tables `gUnk_0824F2E4`/`gUnk_0824F324`/`gUnk_0824F344`.
- `sub_08193874` (1216B) / `sub_08193D34` (964B): per-part "hit reaction" callbacks
  (damage, `groupHpA`/`groupHpB`, sounds); assigned as callbacks by `sub_081940F8`.
- `sub_081940F8` (904B): `Nidhoggr_InitPartAnimations`-ish; loads the shared `animHdr`
  resource and wires up the two callbacks above across the 26 parts.
- `sub_0819465C` (508B): the suspended object's state machine (states 0-8, matching the
  Start*/Is*SuspendedObject helpers already decompiled). Fully decoded by hand (see the
  in-file comment) but a direct translation only reached 48.5% fuzzy — the compiler keeps
  the task pointer itself in a register and indexes a "jitter" field pair
  (`suspendedBasePosX`+0/+2) through it rather than via a separate pointer variable; worth
  a retry with that constraint in mind.
- `sub_08194858` (244B) / `sub_0819494C` (244B): companions to
  Nidhoggr_Start{First,Second}PartAction; first attempt (direct translation) was 33%
  fuzzy, blocked on the same register-caching issue as above plus an unclear shared-tail
  jump between the two branches.
- `sub_08194D70` (564B): per-frame death-flash effect (`Blend_SetAlpha` off a fade table,
  `altColliders[0]` refresh) gated by `flags[1]`, then calls sub_08194858/sub_0819494C.
- `sub_08195334` (1088B): called right after the behavior dispatch in UpdateState; almost
  certainly the per-row animation dispatcher that picks which
  SceneAnimation_UpdateSlotAnimationNN/Nidhoggr_Update*AnimationNN handler to run.
- `sub_08196A5C` (328B): no direct caller found in this unit, so it's presumably another
  behavior handler reached only through the `gUnk_0824F5C0` table.
- `sub_081958A8` (164B) **does** match — the second-phase counterpart of
  Nidhoggr_ActivateRandomPart, tentatively named `Nidhoggr_ActivateRandomParts2` via a
  local `#define`.

## Surprises

- The ~50 `SceneAnimation_UpdateSlotAnimationNN`/`Nidhoggr_Update{Loop,OneShot,Part}AnimationNN`
  functions are all one mechanical pattern (same struct, same body shape, only the anim-id
  constant and name differ) driving a 26-entry animation row array via `rowInitFlag`/`rowDoneFlag`
  latches.
- `altColliders[26]` at 0x5DFC mirrors `colliders[26]` at 0x66D8 — found via sub_08194858's
  disassembly writing the identical 8-byte blob into both a `colliders[n]` and an
  `altColliders[n]` at a stride-0x54-shifted offset.
