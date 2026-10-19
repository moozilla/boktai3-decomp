Tags: #actors #animation

# Actor graphics and basic states (08021EA8-08024F2C)

Merged into `src/actor_graphics_and_basic_states.c` (82 functions: 70 matched,
12 `INCLUDE_ASM`). Reuses `struct StateActor` (`include/state_actor.h`), the
same object as `081D...`'s state-table actors (confirmed: `SceneNpc_*` and
`SceneCharacter_*`/`MessageActor_*` in `src/message_actor_08612344.c` still
match unchanged after the header edit). This range fills in bytes that header
previously left as opaque padding.

## Header changes

* `include/state_actor.h`: `unk70[0x168-0x70]` and `receiver[0x2CC-0x168]`
  split into named fields:
  - `0x90-0x167`: a second, nested command/recovery pipeline, structurally
    identical to the primary state machine (`enterCommand`/`done`/`nextState`
    at 0x08/0x07/0x05) but at different offsets: `actionAnimation` (0xAA),
    `actionState` (0xAB), `actionMode`/`actionIndex`/`actionPhase`
    (0xAC-0xAE, three separate dispatch-table indices feeding
    `SceneActorState_UpdateActionController` / `_UpdateActiveActions` /
    `_DispatchPrimaryAction` and siblings), `actionFinished`/`actionEnter`
    (0xAF/0xB0), `actionTimer` (0xC4), `actionTryUpdate`/`actionReset`
    function pointers (0x104/0x108), and a second message receiver
    `receiver2` (0x118, used by all the `AdvanceToState*`/`*Recover`
    handlers via `Trigger_ConsumePending`).
  - `0x168-0x2CC` (previously all `receiver`): `receiver` shrunk to a
    16-byte placeholder (still just a guess at its real size - nothing in
    this unit characterizes it further), followed by `bodyDrawable` (0x178,
    `SceneActorGraphics_InitializeBody`'s collider), `overlayDrawable`
    (0x1CC, `_InitializeOverlay`'s collider), `worldPosition` (0x260, the
    collider/render position - distinct from the walk-target `position` at
    0x68), and `palette` (0x288, the active 16-color bank pointer).
  - `variant` (0x343, the byte right before `groundEffect`): a 0/1 selector
    read by `SceneActorGraphics_UpdateVariant` and the two
    `SceneActorAnimation_Update*Callback` concessions, and compared against
    in `SceneActorState_SetValueClampedWhen{Inactive,Active}`.
* `include/actor_graphics_and_basic_states.h` (new): `struct SceneActorGraphics`
  names `groundEffect`'s contents as far as this unit's evidence goes (0x344
  through the three 0x200-byte palette banks at 0x3C4-0x9C4, to the contact
  pool / background-flash pointers at 0x9D8/0x9DC). Declares the
  engine-adjacent helpers this unit calls that weren't already in
  `engine.h`/`contact_actor.h`/`background_flash.h`/`spark_particle.h`
  (`SceneActors_Start{Directional,EightDirection}Animation`,
  `SceneActors_GetBySlot`, `SceneActors_ConfigureEffect`,
  `SceneMessages_SetPendingTrigger`, `BackgroundFlash_Begin*`,
  `AreaEffect_IsPositionCovered`).

## Local adaptations (and why)

Struct-field access through `SceneActorGraphics` (a nested nested cast off
`groundEffect`) reliably **breaks the match** whenever more than one field
from the same cast is read together (e.g. the three attack-property shorts
passed to `Collider_SetAttackProperties`, or the `actionMode`/`actionIndex`/
`actionPhase`/`actionTimer` quads the `*Recover`/`Resume`-style handlers
write). agbcc ends up CSE-ing the shared base pointer and addressing fields
as small register offsets, instead of recomputing each field's full absolute
offset the way the original does. Single, isolated field reads (one access,
no sibling field from the same base nearby) matched fine through the named
struct. Net effect:

* Top-level `StateActor` fields used in isolation (`health`, `vulnerable`,
  `locked`, `variant`, `target`, `worldPosition`, `bodyDrawable`,
  `overlayDrawable`, `palette`, `actionAnimation`, `actionPhase`,
  `actionIndex`, `actionMode` alone, `receiver2`) -> converted to the shared
  struct, all still match.
* Anywhere two or more `groundEffect`/action-cluster fields are read or
  written together -> kept as the original per-function local struct or raw
  `(u8 *)p + 0xNN` casts, with a one-line comment pointing at the header
  field names. `sub_080221DC`, `sub_08022330`, `sub_080225B4`,
  `sub_08022AF4`, `sub_080237C0` and the whole `*Recover`/`Resume`/`Advance`
  cluster family (`sub_08023948` through `sub_0802483C`'s callers) are this
  shape; each has a comment citing the lesson.
* `sub_08021EC0` (`SceneActorGraphics_SelectResource`,
  `MATCHING_CONCESSION: register-pinning`) and `sub_080224E4`
  (`SceneActors_StartDisappearEffect`), `sub_08024CE0`/`sub_08024E18`
  (the two `SceneActorAnimation_Update*Callback` concessions) were left
  completely untouched (still their original ad hoc local structs): these
  are register-pinned, and even the single-field changes above were enough
  to perturb them.
* `src/fn/sub_08022F88.c` (`SceneActors_DispatchCommands`, already
  `#ifdef NONMATCHING`, 81.1% fuzzy) was left as is; its local `struct
  S`/`struct T`/`Entry` are a third, independent, lower-confidence view of
  the same object and weren't reconciled with the new header.

Several files had the same local struct/helper names (`struct S`, `chk`,
`TakeFlag`, `State`, `Actor`, `enter`, `ActorAnimation`, `setFacing`) with
different bodies; `merge_unit.py` requires identical names to have identical
bodies, so each was suffixed with its address (`chk_08023638`, `S_080237C0`,
...) before merging.

## Gap functions (no C before this unit; 12 total)

Matched: none closely enough to keep as a draft. All 12 are
`INCLUDE_ASM("asm/nonmatching", ...)`:

* `sub_08022624` (55 insn): searches an array of position pointers at
  `base+0x110` for one inside a min/max box (x and z only). Best attempt hit
  66.7% fuzzy - very close logically, but the original advances a running
  pointer by 4 each iteration instead of recomputing `base+0x110+i*4` from
  the index, and swaps two registers for `min`/`max`; couldn't reproduce
  with plain C after ~6 tries.
* `sub_080243CC` (56 insn): the same actionPhase/actionTimer/actionEnter
  recovery pattern as the matched `*Recover` family, gated on a byte at
  `groundEffect+0x3F` (`0x383` absolute) that's also cleared by
  `SceneActorState_EnterPhase6AndResetAnimation`. Best attempt 64.3% fuzzy;
  register allocation diverges as soon as the `if`/`else` halves are given
  as separate statements rather than whatever shape the original used.
* `sub_08022694`, `sub_08022790`, `sub_08022878`, `sub_08022B18`,
  `sub_08022DBC`, `sub_08022EA4`, `sub_08021F4C`, `sub_08023FE8`,
  `sub_0802483C`: not attempted beyond reading the asm. `sub_08022694` is a
  packed 16.16-style particle-spawn helper with an unusual calling
  convention (takes extra arguments in `r4`/`r5`, i.e. register-pinned by
  its caller); `sub_08021F4C`/`sub_08023FE8` are large (250-300 insn)
  jump-tabled dispatchers. None of these were registered as
  `progress_extra_functions.csv` gaps - they already have normal
  `thumb_func_start` labels in `gen/code_sym.s`, just no name or C yet.

## Surprises

* The "second state machine" at 0x90-0x167 mirrors the primary one's
  enter/done/nextState/timer/receiver shape almost exactly, just at
  different offsets and (per this range) one level further from the
  object's start - strong evidence `StateActor` really is one shared actor
  object across many 0802xxxx/081Dxxxx ranges, just incompletely merged so
  far.
* `include/scene_actor_collision.h` (from the immediately preceding
  subsystem, 080213EC-08021EA8) already reads bytes 0x9E/0xA2/0xA4 - which
  fall inside this unit's new `actionFlags`/`unkA0` span - through a
  *different* named type, `struct SceneActorRegistryActor`. That struct and
  `StateActor` look like the same object described twice by different
  eras of decomp work; not reconciled here (out of this unit's scope), but
  worth flagging for whoever tackles that range next.

## Check iterations

Rough total: ~140 `tools/check.py` runs across the session (most functions
1-2 iterations once the "isolated field = safe, clustered field = raw
offset" rule was established by trial on the first dozen; the two gap
attempts cost ~6 iterations each before giving up).
