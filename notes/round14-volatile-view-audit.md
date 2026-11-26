# Round 14: volatile-view audit and RFU field recovery

Tags: #matching #animation #wireless #tooling

## Scope and result

This pass audited the 21 units in the initial volatile-view measurement, using
`bd0c9bc89` as their source baseline. The worker merged that checkpoint before
editing. The measurement counted **34 qualifier occurrences**; the concession
scanner reports **33 warning lines**, because the declaration and cast in
`08224CA4` occupy the same line.

**27 warning lines are resolved in 16 changed units; six remain in five
units.** The removed lines represent three different source improvements:

* 22 unjustified volatile views of ordinary animation, task, pool, vector,
  random-table, or pause state were removed in 11 units.
* Two wireless polling functions now use the already volatile shared session
  byte directly. Its real interrupt volatility remains intact.
* Three RFU manager functions now use the existing shared volatile fields and
  no longer contain artificial discarded reads. The stock compiler generates
  those reads from ordinary assignment to the volatile byte member itself.

The last two categories must not be described as removing legitimate
volatility. No qualifier-only cast cleanup was credited where the same local
volatile access remained. No new helper, typedef, union, compiler setting, or
ignore was introduced. Existing animation APIs were reused; their headers were
not changed.

| Kind, within these 21 units | Before | After |
| --- | ---: | ---: |
| `volatile-view` warning lines | 33 | 6 |
| `once-loop` | 2 | 2 |
| `const-local` | 13 | 13 |
| `local-union` | 1 | 1 |
| Errors | 0 | 0 |
| Total warnings | 49 | 22 |

The first 13 changed units were scanned together with the compiler-aware
checker: 38 to 14 warnings. The three later RFU units were checked separately:
three to zero. The other five units are unchanged. There are no new warning
kinds or sites. The nine earlier diagnostic corrections and the separate,
source-backed libgcc `umul_ppmm` once-loop ignore at root integration are not
source improvements in this pass and are not credited here.

Independent stock `tools/check.py` checks used reviewed whole-unit extents for
every changed file: **16 units, 45 functions, 11,664 bytes, all exact**. This
worker did not run a full build or regenerate project progress reports.

## Accepted source changes

The worker commit IDs below may change when cherry-picked for integration.
All byte extents include the entire translation unit, including pools and
padding. `src/minigames/result_game/result_tutorial.c` contains 30 functions; each other listed unit
contains one.

| Unit | Source improvement | Warning lines removed | Bytes | Worker commit |
| --- | --- | ---: | ---: | --- |
| `080364C0` | Poll the shared IRQ-volatile session byte directly. | 1 | 120 | `57e9db548` |
| `0803670C` | Use the same direct session-state access in the parent NI poller. | 1 | 108 | `c1e2053ad` |
| `0805D2B0` | Read `sequence.index` directly; remove the volatile index pointers and copy chain. | 2 | 724 | `e83fea2a1` |
| `0806AF74` | Read the actual sequence command table through an ordinary const pointer. | 1 | 848 | `9c658040d` |
| `080838AC` | Remove volatility from the task-owned motion flag pointers. | 2 | 480 | `01b9bb7d6` |
| `08126D88` | Use ordinary commands and the shared sequence-duration operation. | 5 | 1,260 | `ac4380de2` |
| `08128F98` | Use indexed reads of the ordinary RAM random table. | 2 | 828 | `5dea8c717` |
| `081294B4` | Replace the private sequence layout and copied playback body with shared types and playback. | 2 | 540 | `47245a65a` |
| `0818F218` | Read and update the pool's actual occupancy field. | 1 | 52 | `3f3c7f038` |
| `081B0F34` | Store through the actual vector fields and typed target pointer. | 2 | 600 | `0d91f4763` |
| `081B1334` | Write vector `x` directly, like the other components. | 1 | 1,428 | `ac3598f00` |
| `081FFAB8` | Use ordinary commands and the shared sequence-duration operation. | 3 | 704 | `f279db406` |
| `08223580` | Use shared RFU clock/ACK fields and remove the dummy pre-store read. | 1 | 64 | `28cee231e` |
| `08224C84` | Clear the shared clock flag directly; remove the dummy read. | 1 | 32 | `6501b47d9` |
| `08224CA4` | Request completion through the shared clock flag; remove the dummy read. | 1 | 40 | `a7e4599da` |
| `src/minigames/result_game/result_tutorial.c` | Clear the scheduler pause word through an ordinary pointer. | 1 | 3,836 | `9104426f1` |

The existing duplicate flag pointers and once-loop in `080838AC`, the
cursor/sample union in `08128F98`, and the constant locals in the actor and
tutorial units remain visible. These changes do not declare their surrounding
functions free of source debt.

### Animation command width

Four units exposed the same first symptom when their qualifiers were removed:
the final `command & 15` access narrowed from the ROM's halfword load to a byte
load. Merely retaining a cast elsewhere would not recover the source.

`include/engine/video/display_sequence_inline.h` already provides
`DisplaySequence_UpdateWeight`, used by the common sprite playback helpers.
Using that real operation in `SpriteProjectile_UpdateRebounds` (`08126D88`)
and `HomingProjectile_UpdateManager` (`081FFAB8`) matches exactly and removes
their copied duration arithmetic. Their wider copied playback blocks remain;
the full common-step candidates did not match.

`FeatherEffectPool_CreateDirectional` (`081294B4`) matches with the complete
`PlayerCue_StepSequence` helper from `include/player/cutscene/player_cue_sequence.h`, the
shared `DisplaySequenceState`, and the existing graphics API declarations.
Adjacent effect functions including `081299F8` and `08129B04` already use that
helper. This removes the private sequence layout and the entire duplicated
playback block. `Anim_SetSequence` (`08220C8C`) remains open, as described below.

### Ordinary random state

`gRandomTable` is a writable **RAM** sample bank at `0x0203B400`, not a ROM
resource. `include/engine/ram.h` declares 1,024 halfwords;
`Random_InitValueTable` (`082210B8`) initializes them, and normal gameplay
consumers index the table with `gRandomIndex`. The scatter cleanup uses
ordinary indexed reads. No asynchronous writer was established for this
table. It is distinct from the separate effect-random cursor/table path.

## Proven interrupt volatility and removed RFU dummy reads

The wireless conclusion is supported by a concrete callback chain, rather
than by the presence of a busy-wait alone:

1. `STWI_init_all` in `src/lib/rfu/librfu_stwi.c` installs `IntrSIO32`, or its RAM
   copy, in the serial interrupt table. `src/lib/rfu/librfu_intr.c` dispatches
   the registered master/slave callbacks through `Callback_Dummy_M` and
   `Callback_Dummy_S`.
2. `rfu_setMSCCallback` in `src/lib/rfu/librfu_rfu.c` forwards to
   `STWI_set_Callback_S`. The REQ callback is stored in `gRfuFixed` and invoked
   through the RFU request callback path.
3. `RfuManager_Init` (`082235C0`) registers `Rfu_ReqCallback` (`08224524`) and
   `RfuManager_ApplyRequestResult` (`08223E28`). It also stores the game's event
   and receive callbacks in the manager's `+0x40/+0x44` fields.
4. `WirelessController_InitLinkCallbacks` (`08034A2C`) supplies
   `WirelessController_OnLinkEvent` (`080345D8`). Request-result processing
   reaches `RfuManager_NotifyEvent` (`08224980`), which dispatches that event
   callback. The link-event handler can clear `gWirelessSessionState`.

Consequently, the existing `volatile u8 gWirelessSessionState` declaration
in `include/engine/ram.h` is justified. The two accepted poller changes remove their
local aliases without weakening the accesses.

The existing `include/link/wireless/rfu_manager.h` also correctly declares
`childClockSlave_flag` at offset 2 and `parentAck_flag` at offset 3 as `vu8`.
`Rfu_ReqCallback` updates the ACK flag and can call
`RfuManager_CompletePendingMscCycle`; `RfuManager_ApplyRequestResult` writes
the child clock flag and reports the clock-slave event. These are actual
interrupt-managed fields.

The first typed RFU candidates preserved each old explicit discarded read.
Scope showed one extra `ldrb` at each store, before the read already generated
for the volatile member assignment. Deleting the discarded statement then
matched all three units exactly. The final code uses direct shared fields and
plain assignments; no local byte-view type or new qualifier is needed.

This removes real source debt: the three discarded-read statements were
stand-ins for the compiler's treatment of the actual volatile member. It does
not remove the genuine asynchronous accesses. No annotation was added.

## Six remaining warnings in five units

| Function / unit | Warning lines | Classification and measured natural candidate |
| --- | ---: | --- |
| `WirelessController_ResetSession` (`080340AC`) | 1 | The session byte is genuinely IRQ-volatile. Removing the local pointer and storing through the shared declaration gives 204 bytes with eight differing bytes. The qualifier remains. |
| `SceneInput_GetNavigationKeys` (`0805E7D8`) | 1 | Ordinary sampled input, with no IRQ writer established. Ordinary array access, direct global access, and the real `RuntimePadState` fields each produce 64 bytes versus 72. The volatile pointer and existing once-loop remain open. |
| `ActorController_UpdateAnimation` (`08184F04`) | 1 | Ordinary keyframe status. The shared widget `status` field produces 224 bytes versus 228; an actual byte local produces 220. The volatile read remains an unresolved allocation form. |
| `Anim_SetSequence` (`08220C8C`) | 2 | Ordinary sequence command data. Qualifier removal changes one load-width byte; the existing shared command read restores halfword width but swaps two operand registers. The volatile command declaration and cast remain open. |
| `TileAnimation_Upload` (`src/engine/video/tile_animation.c`, `08019788`) | 1 | Real DMA3 register access at `0x040000D4`; volatility must remain. The declaration warning is a hardware-recognition limitation in the scanner. |

The DMA case is established by the actual stores to source, destination and
control registers, followed by a control read. `include/gba/io_reg.h` declares
DMA3 source/destination/control at offsets `0xD4/0xD8/0xDC` with volatile
register access. The local qualifier was not removed or disguised.

For navigation input, `src/engine/runtime_input.c` writes the real
`RuntimePadState` record (`held`, `pressed`, `released`, `reserved`). The typed
candidate does not restore the ROM's separate held-value reload or its extra
mask copy/register lifetime. No private replacement volatile type was added.

For actor animation, `Anim_Step` (`08220F70`) writes the status byte at
animation offset `0x1D`, corresponding to controller offset `0xD1`. The
existing `AnimSprite` view exposes this byte. Ordinary access folds the
mask test into the loaded register and drops the ROM's separate copy. The
tested single regmove prohibition did not recover it; no new constant local
or packed union was substituted.

For `Anim_SetSequence`, using the existing shared state and command-read API
leaves two operand-register differences at 148 bytes. The diagnostic-only
`AGBCC_NOTIE=243` makes that candidate exact, confirming the tie mechanism.
Direct initial weight/duration fields and the shared duration-update helper
were also tested and did not match. No compiler knob is used by production
source. The shared type's `unknownA` field is the sequence identifier read and
written by this function; renaming it throughout the project was outside this
bounded audit.

These are bounded source/type attempts, not claims that cleaner matches are
impossible. Four of the six warning lines are unresolved ordinary-object
volatility; the other two describe a legitimate IRQ alias and hardware access.

## Three file-level errors inspected without modification

The separate volatile pointer declarations in
`TargetRegistry_RegisterActor` (`0815F6A8`),
`TargetRegistry_RegisterSceneActor` (`0815F6F0`), and
`SpriteTrail_UpdatePositions` (`081F2404`) were inspected for interrupt
provenance. No credible IRQ writer was established.

The named stores to `gSceneLinkList` are in `TargetRegistry_Init` (`0815F77C`)
and `TargetRegistry_Destroy` (`0815F770`). The named stores to `gSceneCamera`
are in `TilemapPanel_Initialize` and `SceneTilemap_Destroy` in
`src/ui/save/scene_tilemap.c`. This evidence does not justify centralizing either
volatile declaration. Those root-owned errors were left unchanged; they are
outside the 21-unit counts above.

## Reproduction

`notes/round14-volatile-view-trials.jsonl` preserves 76 baseline/candidate
records. Each includes the source path, hypothesis, source hash, measured
extent and byte difference, and a complete patch against the unit in
`bd0c9bc89`. Compile failures are retained as failures. None of the recorded
matches uses an instrumented compiler override.

`notes/round14-volatile-view-open-candidates.patch` contains four unlanded
natural candidates for the unchanged source units. `git apply --check`
passed. It excludes the DMA case, where removing volatility is incorrect.

Transient independent check results and compiler-aware before/after logs are
under `build/round14-volatile-views/`. Each changed unit was separately checked
with `tools/check.py --target-size` using the extent in the accepted table;
the trial files were not the only verification of the landed source.
