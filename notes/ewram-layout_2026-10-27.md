# EWRAM shared declarations and layouts (2026-10-27)

Tags: #matching #tooling

The worker assignment covered EWRAM rows without a header at the start of the
pass, in address order. The batch introduced 54 scalar/pointer/array
declarations and 14 agreed struct layouts; eight entries remain blocked. All editable consumers of the accepted
objects use the shared declarations. Remaining private declarations are only
in protected files. Every accepted conversion preserves compiler output;
new matches, register hints and instruction bodies were not added. No full ROM
build was run in this worker checkout.

## Validation

Scalar and pointer renames used `tools/ramlayout.py` before/after assembly
comparisons. Struct conversions compared each edited consumer's complete
assembly and then used `tools/check.py`. Existing guarded drafts were checked
in both modes when touched and restored. Protected files remained unchanged.
Compatibility cleanup of headers compared their direct and indirect consumers,
including 241 consumers of `background_controller.h` and 3,105 consumers of
`game_state.h`. Two pre-existing NONMATCHING compilation errors remained in
protected 081D95F0 and 08208740: duplicate helper definitions. Only diagnostic
line numbers shifted when unused declarations were removed; the errors were
not introduced or repaired by this pass. Actual users of the moved game-state
pointers checked exact again after the header cleanup.

The research kit's renderer audit was checked as supporting evidence; its
layout decisions do not establish EWRAM types. The renderer header and the
known raw BG scroll accesses were left untouched.

## Storage corrections

The eight-entry registry at 020000B8 ends at 020000D8, which is an independent
actor-pool manager pointer. The four script section pointers at 02000448 end
at 02000458, an independent command-table list head. The two halfwords at
02000548 end at the notification pointer at 0200054C. Four slot words at
02000560 and four actor pointers at 02000580 end before the separate counters
at 02000570 and 02000590. These five apparent interior labels were corrected
using their loops, declarations, and independently addressed ROM accesses.

`layoutmerge.py` counts some array dimensions in declarations as indexed
accesses. This inflated several extents: a declaration of `[4]` suggested an
access to element 4. Declared bounds, actual loops and ROM accesses were checked
before adjusting sizes. Array dimensions alone were not treated as accesses.

## Integration detail

Keep the worker commits in order. The asynchronous runner captured the next
menu object's header/inventory preparation while requests 004 and 005 were
being committed; their dependent consumer commits are 005 and 006. The final
tree is validated, but those preparation commits should not be cherry-picked
individually without their immediate dependents. Subsequent requests used a
commit-marker and clean-tree gate before any further edits.

## Tool improvements

A protected-consumer preflight should follow declarations through headers,
not only look for source files that would be renamed. It should reject a
rename when a restored protected source relies on the old declaration from a
header. `ramlayout.py` should also reconcile only the requested batch rather
than repeatedly attempting unrelated existing `ram.h` views, try a cast before
retaining editable duplicate declarations, and normalize file-local aliases
when evaluating header inclusion.

`layoutmerge.py` needs a consumer-only mode, reuse of completed reports without
rescanning, and exclusion of declarations from its array-access scanner.
Header-equivalence checks should report existing baseline compilation failures
separately: the temporary checker initially mistook shifted diagnostic line
numbers for changed output.

The final editable reset consumer retains an explicit four-halfword view of
the existing `gTrackedPlayerId` declaration. The two text-menu consumers retain
typed views of the existing incomplete `gScreenTextParameters` declaration.
Those already-header-backed objects were outside this assignment; their
private extern declarations no longer prevent using `ram.h`. All three units
remained exact.

A pending commit request briefly included a transient compiler input found by
an untracked-file scan. It was removed before the runner committed it; new-file
requests now admit only intentional headers and documentation. No compiler
input or generated artifact was committed.

Exact replay commands and per-object outcomes remain in
`build/layout_commands.sh` and `build/LAYOUT_LOG.md`; batch specifications and
validation logs are retained under `build/`.

## Per-object results

| Address | Name | Outcome | Header or blocker |
|---|---|---|---|
| 02000060 | `gParticleGroupManager` | unified | `include/ram.h` |
| 02000080 | `gWeatherCooldownBase` | unified | `include/ram.h` |
| 02000084 | `gWeatherCooldownRange` | unified | `include/ram.h` |
| 02000088 | `gStormActive` | unified | `include/ram.h` |
| 0200008C | `gStormTimer` | unified | `include/ram.h` |
| 020000B8 | `gSceneActorRegistry` | unified | `include/ram.h` |
| 020000D8 | `gActorPoolManager` | unified | `include/ram.h` |
| 020000F0 | `gWirelessLastStatus` | unified | `include/ram.h` |
| 02000124 | `gActorAttachmentManager` | unified | `include/ram.h` |
| 02000134 | `gSceneParticleEmitterPool` | unified | `include/ram.h` |
| 020001AC | `gNamedActorManager` | unified | `include/ram.h` |
| 020001B0 | `gActorGroupPool` | unified | `include/ram.h` |
| 020001C0 | `gLinkSessionStep` | unified | `include/ram.h` |
| 020001CC | `gConfirmPanel` | unified | `include/ram.h` |
| 02000204 | `gRegistryE7BCManager` | unified | `include/ram.h` |
| 0200021C | `gOffsetActorAnimationRepeats` | unified | `include/ram.h` |
| 02000220 | `gHresvelgrAttackCount` | unified | `include/ram.h` |
| 0200022C | `gHresvelgrHazardIndex` | unified | `include/ram.h` |
| 02000230 | `gHresvelgrVolleyState` | unified | `include/ram.h` |
| 02000234 | `gHresvelgrVolleyCount` | unified | `include/ram.h` |
| 02000268 | `gRegistryF3E4Actor` | unified | `include/ram.h` |
| 02000278 | `gSceneNpcBlendPalette` | unified | `include/ram.h` |
| 020002A0 | `gOptionMenuParameters` | struct | `include/menu_parameters.h` |
| 020002C0 | `gChoiceMenuParameters` | struct | `include/menu_parameters.h` |
| 020002E8 | `gMessageMenuParameters` | struct | `include/menu_parameters.h` |
| 02000310 | `gRegionMenuParameters` | struct | `include/menu_parameters.h` |
| 02000340 | `gDateMenuParameters` | struct | `include/menu_parameters.h` |
| 02000358 | `gSelectionMenuParameters` | struct | `include/menu_parameters.h` |
| 02000378 | `gScriptMessageParameters` | struct | `include/menu_parameters.h` |
| 020003A0 | `gListMenuParameters` | struct | `include/menu_parameters.h` |
| 020003C0 | `gSoundTestParameters` | struct | `include/menu_parameters.h` |
| 020003C8 | `gScriptedActorController` | unified | `include/ram.h` |
| 020003D0 | `gTimeDisplayParameters` | struct | `include/menu_parameters.h` |
| 02000448 | `gScriptSectionBases` | unified | `include/ram.h` |
| 02000458 | `gScriptCommandTables` | unified | `include/ram.h` |
| 02000478 | `gWeatherCollisionTimer` | unified | `include/ram.h` |
| 0200047C | `gUnk_0200047C` | skipped | shared gSceneMessageController declaration conflicts in protected src/large_scene_animation_controller.c; ramlayout rolled back all changes |
| 02000480 | `gSceneActorListManager` | unified | `include/ram.h` |
| 02000484 | `gWirelessController` | unified | `include/ram.h` |
| 02000488 | `gUnk_02000488` | skipped | restored protected src/tether_actor.c needs the old header declaration; renamed header leaves old gUnk references undeclared; full batch restored |
| 020004B4 | `gUnk_020004B4` | skipped | protected src/fn/sub_08135F70.c uses the old declaration from include/area_effect_link.h; header rename gives compile error after restoring consumer |
| 020004B8 | `gUnk_020004B8` | skipped | protected src/fn/sub_0803EF4C.c uses the old declaration from include/scene_arc.h; header rename gives compile error after restoring consumer |
| 020004F0 | `gUnk_020004F0` | skipped | protected src/fn/sub_0803DF90.c uses the old declaration from include/scene_pool.h; header rename gives compile error after restoring consumer |
| 02000500 | `gPlayerInteractionMode` | unified | `include/ram.h` |
| 02000504 | `gPlayerSavedMenuOption` | unified | `include/ram.h` |
| 02000514 | `gPlayerLinkAttackBonus` | unified | `include/ram.h` |
| 02000520 | `gPlayerScriptState` | unified | `include/ram.h` |
| 02000528 | `gPlayerActionPending` | unified | `include/ram.h` |
| 0200052C | `gPlayerDirectionLatch` | unified | `include/ram.h` |
| 02000534 | `gPlayerPassageState` | unified | `include/ram.h` |
| 0200053C | `gPlayerBackgroundMode` | unified | `include/ram.h` |
| 02000548 | `gPlayerInitialSlotValues` | unified | `include/ram.h` |
| 0200054C | `gPlayerPendingActor` | unified | `include/ram.h` |
| 02000550 | `gPlayerBackgroundState` | unified | `include/ram.h` |
| 02000554 | `gPlayerAbilityLatched` | unified | `include/ram.h` |
| 02000560 | `gUnk_02000560` | skipped | four-word array declaration conflicts with existing scalar views in shared headers (e.g. area_effect.h); ramlayout rolled back all changes |
| 02000570 | `gPlayerSolarActionPending` | unified | `include/ram.h` |
| 02000580 | `gUnk_02000580` | skipped | protected src/fn/sub_080CCD7C.c uses the old declaration from include/player.h; header rename gives compile error after restoring consumer |
| 02000590 | `gPlayerEffectPendingCount` | unified | `include/ram.h` |
| 0200059C | `gPlayerAttackEffect` | unified | `include/ram.h` |
| 020005A8 | `gHudResetFlag` | unified | `include/ram.h` |
| 020005B4 | `gNameEntrySavedState` | unified | `include/ram.h` |
| 020005DC | `gScreenTextSavedValue` | unified | `include/ram.h` |
| 0200060C | `gSaveStateSignature` | unified | `include/ram.h` |
| 02000610 | `gScriptVm` | struct | `include/script_virtual_machine.h` |
| 02000700 | `gScriptLocalVariables` | unified | `include/ram.h` |
| 02000704 | `gBackupGameVariables` | unified | `include/ram.h` |
| 02000708 | `gGameVariables` | unified | `include/ram.h` |
| 0200070C | `gBackupGameState` | unified | `include/ram.h` |
| 02000714 | `gHeapHead` | struct | `include/heap.h` |
| 0201F400 | `gHeapEndGuard` | unified | `include/ram.h` |
| 02037000 | `gBgMapWorkBuffer` | unified | `include/ram.h` |
| 02039000 | `gObjTileUploadRequests` | struct | `include/obj_tile_upload.h` |
| 0203B000 | `gResourceRegistryEntries` | struct | `include/resource_registry.h` |
| 0203B400 | `gUnk_0203B400` | skipped | protected src/fn/sub_08025FFC.c relies on old random-table declaration in engine.h; header rename leaves old uses undeclared |
| 0203C400 | `gGameStateStorage` | unified | `include/ram.h` |
