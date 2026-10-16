# Script entry points, batches 1-2 (2026-10-16)

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
