# Script entry points, batches 1-9 (2026-10-16)

Tags: #scripts #text #actors #tooling

Targets from [docs/tasks/script_targets.md](../docs/tasks/script_targets.md), top of
the "Unmatched entry points" table first. All names are provisional (`low` in
`symbols/functions.csv`); options are named in the C (picked up by
`tools/assets/gcloptions.py`) or in `gcl/names/option.csv`.

## Matched

| Address | Name | Key | What the scripts do with it |
|---|---|---|---|
| 0817809C | `Hud_SetModeFromScript` | C6AA | `-m` HUD mode for sub_08177DEC |
| 081780B4 | (hidden) | | re-applies the current HUD mode (+0xA5E); was trailing data of 0817809C |
| 08227A10 | `Terrain_GetFloorHeightFromScript` | 0CD4 | floor height (slopes included) at world x z, written to the result operand |
| 080311F0 | `MessageWindow_OpenFromScript` | DD91 | `-s` message, `-r` rect, `-c` palette, `-i`, `-m` spacing, `-w`, `-d` scroll, `-p` values, `-a`; replaced a 70.8% draft |
| 0816B884 | `GameState_SetAreaFlagFromScript` | 0C1B | sets bit `-i` of area `-s` (default: game state +0x5AC) in the words at +0x5C0 |
| 0802E3BC | `Packet_QueueFromScript` | DDD0 | fills a message packet (`-n` id, `-k`, `-f`, `-a` command, `-p` args) in the two-page queue at 0x02000480 and passes it to sub_08020DD0; its bounds check reads `counts[counts[page]]` as the original does |
| 082268C0 | `Camera_SetFollowFromScript` | CAEE | `-w` target, `-f` (1), `-N` (0) for sub_08226558 after stopping the camera move (tentative) |
| 0815CBF8 | `ScriptActor_MoveToFromScript` | 933C | `-p` x y z, `-d`, `-e` for sub_0815B0A8 on the actor picked by `-i` |
| 0815CC78 | (hidden) | | sibling of 0815CBF8 calling sub_0815BF18 |
| 08103CF8 | `NamedActor_SetRouteFromScript` | 5D74 | `-n` actor, `-r` route id and mode (sub_0821E5C4) |
| 081780D4 | `Hud_SetLayoutFromScript` | 2FA0 | `-o` element visibility list (sub_08177C9C) unless the HUD is busy |
| 08033BD4 | `TextWindowManager_Create` | 6273 | creates the text window manager task once |
| 0811DE68 | `GameFlag_TestFromScript` | 1E6E | result = bit `-i` of flag set `-k` (sub_0811DD94) |
| 0815CDAC | `ScriptActor_EnterStateFourFromScript` | BB01 | puts the actor picked by `-i` into state 4 (`-e`, `-f`) |
| 081AF488 | `Vanargand_Create` | 7C56 | boss task creation (`-t`, default 5) |

The two hidden functions were counted as part of the entry point before them
(56 and 256 bytes in the target list); they are now boundaries in
`symbols/build_extra_functions.csv`, `progress_extra_functions.csv` and
`progress_functions.csv`.

## Not matched yet

* 08225624, the `trap` command (`Trap_CreateFromScript`): guarded draft at 93.5%
  in `src/fn/sub_08225624.c`. Everything matches except the two loops that clear
  the four-entry `-w` and `-s` lists. The original runs a pointer from
  `&list[3]` down and compares it with the list base (`cmp r0, r2; bge`), no
  counter, no entry check. Tried: index loops (`for`, `do`, count up/down,
  `s16`/`s8` counters, separate counter variables), pointer loops (give an
  unsigned compare and an entry check), `(s32)` pointer casts (signed, but extra
  registers), byte-offset loops. A 400-second permuter run only reached 790
  from 895 and crashes intermittently on its own type inference
  (`KeyError` in `perm_temp_for_expr`). `trap.flags |= 0;` reproduces the
  redundant flags reload before sub_0821E104.
* 081D0C04 (registry BB34, 804 bytes, scene NPC creator, `SceneNpc_CreateFromScript`):
  draft at about 63%, not committed yet. Open issues: the 0xFFFF0000 mask is reloaded per
  bitfield write in the original (CSE'd into r4 in the draft), and the
  `-i` switch's five `setMode` bodies cross-jump differently.

## Idioms that worked

* A position as `struct { u32 x:16, y:16, z:16, pad:16; }` written
  `x = v; y = 0; z = w;` gives the original's single `str` for x (y folded in)
  and the mask kept in a callee-saved register for z (08227A10). Reading it
  back through a `struct { s16 x, y, z, pad; }` view gives `ldrh; lsl; asr`.
* An optional script byte, `static inline u8 f(void) { u8 v; if (Script_GetPc())
  v = Script_GetValue(); else v = 0; return v; }` (08103CF8); the early-return
  form loses the `lsl/lsr` placement.
* `Script_GetKeywordValue('x', d)` is a real call (sub_0821ABA8); when the asm
  has an inline `Script_SeekToKeyword` + `Script_GetValue`, write that instead.
* A `while (Script_GetPc()) { a[n] = Script_GetValue(); n++; }` loop needs the
  increment as its own statement to keep the load/add/store after the call
  (080311F0).

## Batch 2

Matched (27 functions, 3,448 bytes, +0.143 percentage points):

| Address | Name | Key | Notes |
|---|---|---|---|
| 08225624 | `Trap_CreateFromScript` | command trap | see "trap" below |
| 08103DCC | `NamedActor_SetSpeedFromScript` | 15ED | `-n` actor, `-s` (6) at +0xBE |
| 0815CA88 | `ScriptActor_SetAnimationFromScript` | C12D | `-p` + 366 to sub_0815AEAC |
| 0815CCF8 | `ScriptActor_TurnFromScript` | 9763 | optional `-p` point, `-d`, `-f`, `-e` for sub_0815B15C |
| 08225CB8 | `Terrain_LoadFromScript` | 30AD | `-p` HP resource or `-h`/`-t`/`-r`/`-z`; `-v` map callback set |
| 08179298 | `Hud_Create` | CC28 | picks one of three HUD creators |
| 081F2034 | `SingletonSlot7_Create` | 5856 | slot 7 of the singleton table at 0x020000B8 |
| 0820A460 / 0820A4D0 | `Slot600_ActivateFromScript` / `Slot600_DeactivateFromScript` | 2863 / 81C7 | slot `-n` of the eight-slot manager at 0x02000600 |
| 0822E304 | `GameState_AddListEntryFromScript` | 297E | `-a` into the first free table160 entry |
| 081A3270 | `Hresvelgr_Create` | C077 | |
| 081BA5D0 | `Registry2388_Create` | 2388 | |
| 0822E340, 0822E564, 0822E924, 0822DE64, 0822ECC8 | `GameState_Reset*FromScript`, `SolarFruit_Command` | 01AB, 1887, 1606, 65E2, FE88 | reset a game-state list, then refill it from `-n` count and two value options |
| 0822DB54 | `RecordEntry_AddFromScript` | 3757 | copies a 32-byte record of 08E6092C |
| 081DC618, 081DE520, 081EB724 | `ScreenParams_SetFromScript`, `ScreenParams328_SetFromScript`, `Params3D0_SetFromScript` | 317B, FE3F, 3578 | parameter blocks for three screens |
| 0811D358, 0811D1C0 | `GameState_ClearSlots7A8FromScript`, `GameState_SetNameFromScript` | 7245, 98D8 | |
| 08178564 | `Hud_CloseMessageFromScript` | C8D1 | HUD mode 7 |
| 0815A5E8 | `ScriptActor_SetLevelFromScript` | 5335 | |
| 080F752C | `Sword_Create` | DB8D | |
| 08073BA0 | `SceneActorContacts_Create` | D854 | |

Guarded drafts (not counted): 081D0C04 `SceneNpc_CreateFromScript` (66.8%),
081AF4F4 `Vanargand_ResetFromScript` (97.3%, two zero constants in the other order),
0815A838 (92.0%) and 0815A680 (51%), which both keep a second copy of the actor pointer
that no tested source form reproduces, and 08119EF0 `Glove_StartFromScript` (58.5%).

### trap (08225624)

The clearing loops are the project's usual signed pointer form (see
choice_menu.c, directional_interaction.c): `s32 zero = 0; u16 *out = list + 3;
do { *out-- = zero; } while ((s32)out >= (s32)base);`. The rest came from the
permuter: the names list needs `u16 *base = *&names;` (taking its address stops
`names + 3` being hoisted into a callee-saved register before the `-w` loop),
and the `-s` loop is `for (i = 0, list = names; ...)`.

### Idioms

* "Found" code placed between the keyword test and the search loop comes from
  an early `if (!Script_SeekToKeyword('n')) return ...;` instead of wrapping
  the loop in `if (...) { }` (0822E304, 0820A460).
* Game-state arrays addressed index first (`lsls; adds base; adds #off`) need
  `*(s16 *)((u8 *)gUnk_02000710 + i * 2 + OFF)`; `state->array[i]` adds the
  offset first. Storing -1 that way also gives the `movs; rsbs; adds` copy.
* A loop that only counts (`for (i = 0; i < count; i++) f(Script_GetValue());`)
  is reversed by agbcc into `subs; cmp #0; bne`; writing the count-down
  yourself gives `bgt` instead.
* Callback names: check `symbols/functions.csv` before declaring a `sub_`
  callback. A renamed one (sub_08073B60 is `SceneActorContacts_Update`)
  links to nothing in `tools/check.py`, which then reports a one-bit literal
  mismatch, and `make` fails with an undefined reference.

## Batch 3

Matched (6 functions, 756 bytes, +0.031 percentage points): 081AF4F4
`Vanargand_ResetFromScript` (was a 97.3% draft), 08016764
`PaletteLevels_ConfigureFromScript` (3535), 0815D524 `ScriptActor_SetModeFromScript`
(4CDD), and three more parameter blocks: 081DBF98, 081DCD90, 081DBA54
(`ScreenParams2E8/310/2C0_SetFromScript`).

New guarded drafts: 081803D8 `BossSlot2_AdvanceStepFromScript` (74.7%) and
08103B94 `NamedActor_SetMoveModeFromScript` (47.7%).

081D0C04 `SceneNpc_CreateFromScript` is at 92% in wip (not yet committed):
writing `zero = 0` inside the `sub_0802D760(..., zero = 0)` call fixes the whole
front half (it shifts reload's register rotation, which also stops the mask
constant from being kept in r4), setHandler/setBox inlines and a separate
`speed` local fix the stores; still open are the facing computation (result in
the constant's register) and how the five `-i` cases cross-jump.

Idioms:
* Boss tasks share a state setter: `static inline void setState(p, u8 state)
  { p->fieldAC = 0; p->fieldAD = 0; p->fieldAE = state; p->fieldC4 = 0;
  p->fieldB0 = 1; }` (081AF4F4, 081803D8).
* Two copies of one pointer (`adds r3, r0; mov ip, r3`) came from
  `struct T *task = f(); struct T *p = task;` with each used somewhere (081803D8).
* A bound held in a register (`cmp r2, ip`) is a local count (`s32 count = 3;`)
  declared before the loop (0822DB54).

## Batch 4: 081D0C04

`SceneNpc_CreateFromScript` (registry BB34, 804 bytes) matches, so all of the
original top 15 are done. It took, in order:
* `sub_0802D760(npc, 1, model, 0xC6A0, zero = 0)`: assigning zero inside the
  call fixed the whole front half (reload register rotation; the position
  mask is then reloaded per write as in the original).
* `setHandler(fn, &npc->callbacks[i])` and `setBox(npc, 16, 20)` inlines,
  a block-local `s32 speed = 400` for the motion stores.
* Per-case shapes in the `-i` switch so that only cases 3 and 4 cross-jump:
  callback temps assigned before the state store in cases 0-4, a mode temp
  in cases 2-4, and a block-local `u8 one = 1` in case 0 (found with a small
  brute force over these choices plus three permuter runs).
* The facing computation: register-pinning concession (marked in the file).
  `t` pinned to r2 with an empty asm barrier between `a + 0x70` and `>> 5`;
  without the barrier agbcc turns the arithmetic shift into a logical one.
  `tools/depin.py` reduced it to these two hints.

## Batch 5

Matched (3 functions, 1,108 bytes, +0.046 percentage points):
* 081D0F28 `SceneNpc_CommandFromScript` (8918): selects an NPC (-n, else the
  last created) and puts it in the scripted state; -p, -d angle, -w face the
  player (Math_Atan2), -s. Its `setAngle` inline is the facing code of
  081D0C04, and needed the same register-pinning concession.
* 0811B848 `RegistryE7BC_CreateFromScript` (E7BC): three required points
  -p/-s/-o for sub_0811B278.
* 08018664 `PaletteBlend_AddFromScript` (3A89): adds one of 16 blend entries
  to the palette blend task (0x02000094). The early returns had to be separate
  `if`s (`if (!p) return -1; if (p->entryCount > 15) return -1;`).

Named (already matched): 08030864 `SceneActorManager_Create` (286F) and
081C0BD4 `SceneActorManager_Exists` (FB4A), the scene actor manager at
0x02000480 that also holds the message packet queue.

In progress, not committed: 0810067C `FloorSwitch_Create` (53%, register
allocation of the slot pointer and the hoisted field addresses),
0810858C (97%, a permuter run found `one = e->field60 = 1;` and an `f = fr`
copy) and 08111A48, a palette fade start (8.24 channel deltas; the colour
channels' zero-extension and constant placement differ).

The six enemy spawners named in the request (F426 08204704, D459 08205900,
F639 082109D8, 4CD8 08208FD8, 5153 08202608, 01A2 0820FD10) and 0820361C
(98CB) are already matched C, but each is just Task_Create plus an init
callback, so the C does not say which enemy or object they create. They are
left unnamed until the kind numbers can be confirmed from script data.

Tooling: decomp-permuter crashes ("nodes should only appear once in AST") on
a multi-declarator with a struct type (`struct T *a, *b;`); declare such
pointers separately.

## Batch 6

Matched 19 small entry points (1,880 bytes, +0.078 percentage points):
08103ADC, 08103E60 (`NamedActor_SetFlag20/SetRangeFromScript`), 0810BDB4,
0810BE58 (registries C5BD/1022 on the object at 0x020004B4), 0815CF6C
(`ScriptActor_StartActionFromScript`), 0816B834 (`GameState_ClearAreaFlagsFromScript`),
08178008, 08178588 (`Hud_RefreshSpriteFromScript`, `Hud_ShowPanelFromScript`),
081BA57C, 081D51B0, 081F213C, 0820A540 (`Slot600_ResetFromScript`), 081D105C
(`SceneNpc_StopFromScript`), 08227888 (`PlayerName_SetFromScript`, default name
bytes 80 A2 80 98 80 91 80 9F), 0822E9C8 (`GameState_HasUnlockFromScript`) and
five more screen parameter blocks (081D7510, 081E1A2C, 081E03F4, 081EC0A8).

Idioms: a parameter block written through the global each time (`gUnk_X.f =`)
reloads the address per store, through a local pointer it keeps one register;
an inline that returns through one exit (`goto done; p = 0; done: return p;`)
reproduces a pointer computed before its test (0810BE58).

Still open: FloorSwitch_Create (a permuter run is at score 4130 from 5099; the
`struct Vec16 *d = &delta` pointer for the scene-object scan put the slot in r7
as in the original), 0810858C (97%), 08121268 (97%, one constant placement),
0822BFC8 (89%, an extra pointer copy), 08111A48.

## Batch 7

Matched 3 (616 bytes, +0.025 percentage points): 08100298 `WoodenCrate_Create`
(820B; really a command on the crate named -n), 081BB294
`SolarBank_ApplyInterestFromScript` (61A0: grows the balance at +0x7B0 by the
rate at +0x598 per day, capped at 9999, then sets a new rate) and 081DCE34
`Time_OnLoadSave` (7978: parses the region table and computes sunrise/sunset).

Near misses kept in wip with permuter runs: 0806F5C0 (scene effect schedule,
score 55: reload register choices), 081C4080 (registry F3E4, score 60),
0810858C (7 bytes), 08121268 (14 bytes), 0822BFC8, FloorSwitch_Create.

## Batch 8

Matched 1 (1,312 bytes, counted as register-pinned): 081CE1BC
`SceneCharacter_PlaceFromScript` (registry 8EA3, behind GCL `place_character`).
It creates a scene character named `-n` of kind `-k` (default 9) at `-p`
facing `-d`. It picks the model, palette and extra setup by kind (kind 29 copies
its own palette and reads `-C` tag and sound), installs six callbacks, and
starts the behavior picked by `-i` (0-8). It reuses the SceneNpc layout and
idioms of 081D0C04, with these new ones:

* Plain `0`/`1` literals in the kind-29 behavior blocks (not shared `zero`/`one`
  temporaries) give the original's constant order (`movs r3, #0; movs r2, #0`).
* The update-handler temporaries are a two-element array (`updates[0]` and
  `updates[1]`, found by the permuter). In case 2 one block-local `fn` is
  shared by both branches; two separate scalars don't match.
* The facing math keeps the 081D0C04 pins, plus `angle` pinned to r9.

Guarded drafts (no exact gain): FloorSwitch_Create (0810067C), 0810858C
`AnimatedTile_CreateFromScript`, 08121268 `RegistryA8CB_CreateFromScript`,
0822BFC8 `Registry3C8D_RunFromScript`, 0806F5C0
`SceneEffectSchedule_AddFromScript`, 081C4080 `RegistryF3E4_Create`, 08111A48
`PaletteFade_StartFromScript` and 08103110 `NamedObject_CreateFromScript`
(registry A4FC, 66%; it inlines the packed-command sprite stepper of
`include/actor_attachment_sprite_playback.h`). FloorSwitch_Create stays near
70% after about an hour of permuter time: the "pressed" block is placed
differently and the scratch registers in its hoisted-address region differ.

Enemy spawners (registry kinds 51-63) stay unnamed: all of them load the shared
sprites 7E82/7E7D, so no family in docs/ASSETS.md picks them out.

## Batch 9

Matched 10. Two are entry points:

* 081CE6DC `SceneCharacter_CommandFromScript` (registry F9D7). It finds the
  character named `-n`; for kind 29 it applies `-C` and resets the speed to
  400, then runs the same `-i` behavior switch as place_character, except that
  case 2 falls through into case 4. No register pinning needed.
* 08054B64 `HitSwitch_AddFromScript` returns whatever its (void) callee leaves
  in r0, so the callee is declared with a result in that file.

Eight are routines that entry points call:

* 082151E4 `AnimData_Load` (9 callers), 08177DEC `Hud_SetMode` (4) and
  0822B358 `Sound_StopSong` (3).
* 0822C034 `Region_FindContaining` (7) and 082265F8 `Camera_ReadMoveRate` (8).
* 0822E498 `GameState_AddEquipment`, 0822E668 `GameState_AddUnlock` and
  081975C8 `Nidhoggr_ReadOptions`.

Idioms:

* `for (i = 0, p = base; i < n; p++, i++)`: the order of the increments (and
  of the initializers) decides the instruction order (0822C034, 082151E4).
* After `if (busy) return;`, write the stores of zero as literal `0`: agbcc
  reuses the known-zero register (08177DEC). Storing the variable adds a copy.
* A loop-invariant `mask = ~1` re-assigned before each loop matches where
  `&= ~1` doesn't (08177DEC).
* `x = v = f();` before the zero temporaries are set orders the stores
  (081975C8).

Guarded drafts: Registry1745_Create (0810B914, a singleton task creator),
Pool1B8_FindByName (081081F0, 4 bytes off), Route_Start (0821E5C4),
Anim_SetSequence (08220C8C, one register) and Math_Atan2 (082215E4); the
Glove_StartFromScript draft went from 58.5% to 82.9%. The remaining
unmatched entry points are mostly large and inline the packed-command
sprite stepper.

## Batch 10

Matched 1: 08163E24 `ParticlePool_Create`, the 16-sprite task kept by scene
NPCs and kind-29 characters (68 bytes). Guarded draft: 081D2A38
`TaggedSceneNpc_Create`, the SceneNpc creator behind two tiny registry
routines (tag 0 and 1), at about 96%. agbcc keeps model E00E in sb for both
calls; the original loads it from the pool each time.

### The sprite stepper without pins

All matched copies of the packed-command sprite stepper (the
`*_playback.h` headers, sub_08045F48 and others) rely on register pins. Most of
the remaining script entry points inline it, so a natural form would help.
Tested in ContactSequenceActor_UpdateAppearing (08045F48), this plain inline
gets within 29 bytes:

```c
const vu16 *cmd = &s->commands[s->index];
sprite->frame = *cmd >> 6;
{ u32 flags = s->flags; u32 bit = 1;
  if ((bit & flags) != (((*cmd & 0x30) >> 4) & bit)) sprite->flags |= 4;
  else sprite->flags &= ~4; }
{ u32 flags = s->flags; u32 bit = 2;
  if ((u8)(bit & flags) != (((*cmd & 0x30) >> 4) & bit)) sprite->flags |= 8;
  else sprite->flags &= ~8; }
if (++s->elapsed >= s->duration) {
    s->elapsed = 0;
    if (s->flags & 4) {
        if (s->index == 0) { if (s->flags & 8) s->elapsed = s->duration;
                             else s->index = s->frameCount - 1; ended = 1; }
        else { s->index--; ended = 0; }
    } else if (++s->index >= s->frameCount) {
        if ((u8)(s->flags & 8)) { s->elapsed = s->duration; s->index = s->frameCount - 1; }
        else s->index = 0;
        ended = 1;
    } else ended = 0;
    cmd = &s->commands[s->index];
    s->frameWeight = *cmd & 15;
    s->duration = (s->frameWeight * s->speed) >> 6;
    if (s->duration == 0) s->duration = 1;
} else ended = 0;
```

What it needed: a separate block-scoped `bit` for each flip test, so it gets
a scratch register; explicit `ended` in every branch; and `vu16` commands, so
the weight read stays a halfword load. One difference remains: the original
copies `bit` before the AND (`adds r1, r3, #0; ands r1, r0`), and only the
pinned versions' empty asm barrier reproduces that. 15 permuter minutes found
nothing; none of 18 type/operand combinations did either. Plugged into the
A4FC draft it scores 64.5%, about the same as the pinned header, because
A4FC's differences are elsewhere.

## Batch 11

Four earlier drafts now match (1,016 bytes):

* 0810858C `AnimatedTile_CreateFromScript` (registry 8192, register-pinned).
  It uses the batch 10 stepper with only the two flip tests in the shared
  header's barrier form: `first = bit; asm("" : "+r"(first)); first &= flags;`
  for the first test and the r1-pinned `(u8)` form for the second. `const vu16 *`
  commands keep the halfword weight read. The rest of the stepper is plain C.
* 08121268 `RegistryA8CB_CreateFromScript` and 08119EF0
  `Glove_StartFromScript`: in the active/update/timer block, write
  `{ update = fn; u8 *active = &e->active; u32 zero = 0; u16 one = 1; ... }`.
  `one` must be u16 for the later `draw->flags = one | draw->flags` to copy it.
* 081C4080 `RegistryF3E4_Create`: the state stores past +8 go through a
  sub-struct pointer (`struct StateInner *in = &p->state.inner`); the
  original addresses them from that base.

Guarded draft: 082161B4 `Bg_SetupLayers` (88%), the background-layer setup
for a map resource. Wrapping each DISPCNT OR in an inline `enableBg(bits)`
took it from 59% to 84%, because it puts the constant first. Separate
variables for mode 7's three bits gave 88%.
