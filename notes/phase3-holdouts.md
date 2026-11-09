# Phase 3 holdouts

One section per worker. Each entry: the best pin-free draft, what blocks it, the knob that
matches (when one does) and the next idea.

## field-effects

### Scene_SpawnDirectionalDust (src/scene_object.c, 08004B64): deferred

Draft: `notes/phase3-drafts/field-effects/Scene_SpawnDirectionalDust.c` (117 ROM / 93 draft
insns differ; it relies on a local `u32 x:16` bitfield struct defined next to it in the unit copy).
The ROM writes the particle's x and z halves as SImode read-modify-write (`ldr; and #0xFFFF0000;
orr; str`), z through the particle pointer in r5, not through the stack. Probes:
* a struct of u16 fields gives `strh`;
* a `u32 x:16` bitfield struct gives the SImode RMW only while the struct is ADDRESSOF (its address
  is never stored in a pointer variable); once `q = &pt` exists the store becomes `strh`;
* `pt.x = pt.x op ...` written directly also narrows to `strh`; `t = ...; pt.x = t;` keeps the RMW.
The pinned version gets z's RMW from a union of words plus FIRST/LAST macros. Next idea: find
the struct the particle really has at that offset (a packed `u32` position word written whole by
its siblings) and recover it in `include/`.

### Scene_TryStepDown (src/scene_object.c, 08004CE4): deferred

Draft: `notes/phase3-drafts/field-effects/Scene_TryStepDown.c` (42 ROM / 40 draft insns differ,
allocation only). Fixed so far: parameters used directly (no `actor = input` copy) and the
sibling-style `neighbor = index + delta; dynamic = TerrainModifiers_FindCellOverride(neighbor, 1);
savedIndex = neighbor;` put actor in r5 and index in r4. Remaining: the ROM gives mask (0xF0) r4,
cell r6, type r7, direction r8; the draft gives mask r8 (pri 1875, 6 refs / 64), cell r4 (3243).
`AGBCC_PRI=mask:+1400` cuts it to 17/19 but does not match: the low-register choice in the
`heightByte`/`lowMask` block (ROM ldrb r1, movs r0 #0xf; draft r0/r1) and a `regmove` copy at
line 773 remain. The draft still has statement expressions and plain `register` locals from the
pinned form; strip them once the shape is right. The ROM's dead `subs r0, r7, #1` tail comes from
`type == 1 || type == 2` with both arms returning 0 (fact 57-style cross-jump).

Tags: #matching #phase3 #holdouts

One section per worker. Each row: the function, its best pin-free draft (a snippet to
`scope.py FILE.c put`), the differing instructions, the mechanism and the knob that matches,
what was tried, and the next idea.

## menus-link (Menus, HUD and dialogue; Link and wireless)

### StatusScreen_DrawClock (0800E5B8, src/status_screen.c)

Draft: `notes/phase3-menus-link/StatusScreen_DrawClock.c` (4 ROM / 4 draft insns).
* The pinned match keeps the digit-array address in a user variable `values` (pinned to ip).
  Without the pin it gets sb: two sets of `values = digits` each get a REG_EQUIV (the frame
  pointer is function-invariant), and update_equiv_regs (local-alloc.c) doubles the live length
  once per set, 48 -> 192, so its priority (625) falls below the hoisted `gBgRecords` constants
  (833), which take sb first; then pass 0 of find_reg picks sb over ip. Needs priority > 833
  (`AGBCC_PRI=values:+600`). The sibling NumericDisplay_DrawPair gets the same refs from an
  inline `do { row = 0; values = digits; } while (0);`, which the concession detector does not
  count (see the note to the coordinator).
* Indexing the digits (`int tile = digits[col] + 0x60;`) instead: loop.c's hoisted giv base
  lands in ip as in the ROM, and only the inner loop's shift and digit pointer swap r3/r6
  (shift 5 refs over 14 insns against the pointer's 5 over 13; `AGBCC_PRI=shift#2:+551`
  fixes the first loop). Tried: shift inline (hoisted by loop.c: right registers, but the
  `out = dst` copy then comes before it), `dst[col]` (giv order flips), `shift = row * 16`,
  the shift in the for init, operand orders of the add.
* Next: a form where shift lives two insns less (or the pointer two more) without moving the
  `out = dst` copy; or what made the original's `values` denser.

### Inline `do { ... } while (0)` fences (for phase 4)

tools/concessions.py counts empty do/while(0) loops and single-use statement macros, but not an
inline `do { ... } while (0)` with a body. Its loop notes still stop regmove's fixup_match_1 scan
(it breaks at NOTE_INSN_LOOP_BEG/END, regmove.c), so it works as a fence the way DEBUG_NOP does.
The coordinator said to leave the scoring alone for now. These are the ones in src/ on
2026-11-09, 161 blocks in 122 files (`function:line`):

* `src/actor_animation_controller.c`: Actor_ConfigureInteractionChildFromScript:4331
* `src/background_controller.c`: Background_FindMapId:385
* `src/fn/sub_080192FC.c`: ClockEvents_Update:37
* `src/fn/sub_0801F4C4.c`: SceneObject_UpdateLowering:51
* `src/fn/sub_0801F5B8.c`: SceneObject_UpdateLowered:9, SceneObject_UpdateLowered:13
* `src/fn/sub_08020A60.c`: SceneMessages_RunScript:24
* `src/fn/sub_0802141C.c`: SceneActors_FindAvailable:21, SceneActors_FindAvailable:22
* `src/fn/sub_0802FCA8.c`: Actor_SelectPathNodeAction:93
* `src/fn/sub_08035958.c`: Wireless_OnParentUniSendComplete:48
* `src/fn/sub_08035F5C.c`: Wireless_TryStartParentNiSend:56
* `src/fn/sub_080362E0.c`: Wireless_ExchangeChildNiPacket:90
* `src/fn/sub_08038838.c`: WirelessGameMenu_UpdateSelectionExchange:42
* `src/fn/sub_08038AD4.c`: WirelessGameMenu_UpdateActiveRound:103
* `src/fn/sub_0803D8A8.c`: ScenePool_ResumeAfterIdle:9
* `src/fn/sub_080427FC.c`: ContactActor_ApplyHealthContact:33
* `src/fn/sub_080478A4.c`: BackgroundFlash_UpdateAppearing:18
* `src/fn/sub_080479B0.c`: BackgroundFlash_UpdateDisappearing:10, BackgroundFlash_UpdateDisappearing:17
* `src/fn/sub_08047F68.c`: ContactBurst_OnContact:16
* `src/fn/sub_080480A4.c`: ContactBurst_Update:70
* `src/fn/sub_0805386C.c`: InteractionMarker_TickIdle:27
* `src/fn/sub_08053DF8.c`: SavedPositionMarker_TickIdle:27
* `src/fn/sub_08054DC4.c`: DisplayTrack_SetSequenceFromScript:16
* `src/fn/sub_08055C04.c`: StageIconDisplay_TickReveal:4
* `src/fn/sub_08055DD4.c`: StageIconDisplay_InitAnimations:26, StageIconDisplay_InitAnimations:31
* `src/fn/sub_08056D24.c`: ScriptUtility_SetFlag800:8, ScriptUtility_SetFlag800:9, ScriptUtility_SetFlag800:10
* `src/fn/sub_0805DDE8.c`: SceneHud_DrawCenteredMeter:18
* `src/fn/sub_0805E270.c`: SceneInput_InitGlyphPair:26
* `src/fn/sub_0805E394.c`: SceneInput_GetGridPosition:17
* `src/fn/sub_0805E7D8.c`: SceneInput_GetNavigationKeys:14
* `src/fn/sub_08060A70.c`: SceneInventory_BeginConsumableSwapPair:39
* `src/fn/sub_0806B8DC.c`: SpriteEffectController_UpdatePool:17, SpriteEffectController_UpdatePool:18
* `src/fn/sub_0806E130.c`: RouteSpritePool_Tick:33
* `src/fn/sub_08070FA8.c`: RegistryCurrentGame:25
* `src/fn/sub_08072E50.c`: SceneActorContacts_SetDistanceGate:27
* `src/fn/sub_080764C8.c`: SceneActorAnimation_SelectDirectionFlagsVariant:29
* `src/fn/sub_080838AC.c`: SceneActorControl_TickGroundBob:82
* `src/fn/sub_08086A2C.c`: Ghoul_UpdateDyingState:57
* `src/fn/sub_08089800.c`: Ghoul_UpdateScriptedAction:32, Ghoul_UpdateScriptedAction:34
* `src/fn/sub_0808CE50.c`: Ghoul_UpdateSpecialActionControl:61, Ghoul_UpdateSpecialActionControl:63
* `src/fn/sub_0808CF54.c`: Ghoul_UpdateAnimationElevenControl:61, Ghoul_UpdateAnimationElevenControl:63
* `src/fn/sub_0808D058.c`: Ghoul_UpdateAttackControl:61, Ghoul_UpdateAttackControl:63
* `src/fn/sub_080A8144.c`: SceneActorPlacement_UpdateFlaggedResponse:27, SceneActorPlacement_UpdateFlaggedResponse:29
* `src/fn/sub_080AA8C4.c`: TentacleActor_AlignSegmentsToTerrain:81
* `src/fn/sub_080B713C.c`: SceneActorTerrain_FindOpenNeighborDirection:11
* `src/fn/sub_080BB4DC.c`: Skeleton_UpdateDownedState:57
* `src/fn/sub_080C61AC.c`: Golem_UpdateDying:57
* `src/fn/sub_080D48F4.c`: Mummy_UpdateScriptedAction:60, Mummy_UpdateScriptedAction:62
* `src/fn/sub_080DA5C8.c`: Mummy_ResetResponseState:30
* `src/fn/sub_080E0390.c`: Dog_UpdateDefeatDelay:57
* `src/fn/sub_080E3790.c`: Dog_UpdateScriptedAction:60, Dog_UpdateScriptedAction:62
* `src/fn/sub_080F34C8.c`: ChildGroup_WaitForInactiveMembers:45
* `src/fn/sub_080F3B60.c`: ChildGroup_UpdateTrailingMembers:95
* `src/fn/sub_080F4FA8.c`: Sword_UpdateHorizontalWaveReturn:20
* `src/fn/sub_080F508C.c`: Sword_UpdateDepthWaveReturn:20
* `src/fn/sub_080F5B68.c`: TargetMotion_AdvanceFollowSegment:41
* `src/fn/sub_080F645C.c`: Sword_UpdateDamage:56, Sword_UpdateDamage:74
* `src/fn/sub_080FC894.c`: Axe_CopyAuxiliaryVector:15
* `src/fn/sub_081004F8.c`: FloorSwitch_Release:43
* `src/fn/sub_081029F8.c`: NamedObject_UpdatePlayerAttachment:73
* `src/fn/sub_08103110.c`: NamedObject_CreateFromScript:209
* `src/fn/sub_08109040.c`: SelectionSlots_ClearValue:23
* `src/fn/sub_0810B3BC.c`: SceneMenu_UpdateRecords:45
* `src/fn/sub_0810D60C.c`: PanelText_DrawDecimal:17
* `src/fn/sub_0811A030.c`: MotionActor_DispatchRequestedState:72
* `src/fn/sub_0811B094.c`: Collider_ScheduleUpdates:49
* `src/fn/sub_0811BD30.c`: Actor_UpdateShadowSpriteParts:84, Actor_UpdateShadowSpriteParts:88
* `src/fn/sub_0811D270.c`: PanelText_DrawDigits:17
* `src/fn/sub_0811E728.c`: StatModifier_ScaleForGlobalMode:12, StatModifier_ScaleForGlobalMode:18
* `src/fn/sub_0811E77C.c`: StatModifier_AddForGlobalMode:11, StatModifier_AddForGlobalMode:17
* `src/fn/sub_0811E7C8.c`: StatModifier_AddForOwnerMode:11, StatModifier_AddForOwnerMode:17
* `src/fn/sub_08121110.c`: ActorAction_RunActiveCallbacks:31
* `src/fn/sub_081296D0.c`: FeatherEffectPool_UpdateTask:36
* `src/fn/sub_0812F3CC.c`: SceneActorReaction_BeginContactResponse:86
* `src/fn/sub_08136F58.c`: sub_08136F58:95
* `src/fn/sub_0813BAD8.c`: sub_0813BAD8:132
* `src/fn/sub_0813C5B4.c`: ConvergingEffects_EmitFarParticle:57
* `src/fn/sub_0813FE30.c`: sub_0813FE30:50
* `src/fn/sub_08140F40.c`: Player_GetForceDirection:93, Player_GetForceDirection:94
* `src/fn/sub_081559A4.c`: Player_UpdateTimedChargeAction:95
* `src/fn/sub_08165488.c`: SceneHud_DrawDecimalDigits:16
* `src/fn/sub_0816F6B4.c`: PauseMenu_BeginTypeThirteenItemUse:33
* `src/fn/sub_0816F8AC.c`: PauseMenu_BeginTypeFourteenItemUse:33
* `src/fn/sub_08173C0C.c`: SceneHud_TickEquipmentToggle:37
* `src/fn/sub_08175008.c`: Presentation_UpdateAllocationSelection:150
* `src/fn/sub_0817A57C.c`: SceneActors_SpawnConvergingParticle:57
* `src/fn/sub_0817B5E4.c`: update_facing:13
* `src/fn/sub_081A55D4.c`: Vanargand_BlendPaletteBanks:16
* `src/fn/sub_081A8820.c`: ActorSoundPhase_UpdateConditionalStop:39
* `src/fn/sub_081B9BC0.c`: Registry2388_BlendPaletteBanks:18
* `src/fn/sub_081BD854.c`: DarkLoan_CreateHiddenDisplay:19
* `src/fn/sub_081C0478.c`: ScenePaletteFade_BlendPaletteBanks:16
* `src/fn/sub_081C6094.c`: sub_081C6094:24
* `src/fn/sub_081D010C.c`: sub_081D010C:76
* `src/fn/sub_081D07D8.c`: SceneNpc_EnterProjectedIdle:59
* `src/fn/sub_081D58B4.c`: MonsterBook_UpdateHighlightPulse:7
* `src/fn/sub_081D71CC.c`: ScreenTransition_FadeBlendOut:26
* `src/fn/sub_081EB8E0.c`: sub_081EB8E0:180
* `src/fn/sub_08210C5C.c`: Collection_UpdateHighlightPulse:7
* `src/fn/sub_0821211C.c`: ForgingBook_UpdateHighlightPulse:7
* `src/fn/sub_08213294.c`: ForgingBook_UpdateCursorPulse:7
* `src/fn/sub_08214B44.c`: Sprite_RenderScaledList:54
* `src/fn/sub_08214D20.c`: sub_08214D20:43, sub_08214D20:44
* `src/fn/sub_08217430.c`: sub_08217430:86
* `src/fn/sub_08219C40.c`: Heap_Alloc:16
* `src/fn/sub_0821A184.c`: ResourceRegistry_AllocateEntry:8, ResourceRegistry_AllocateEntry:10
* `src/fn/sub_082210B8.c`: Random_InitValueTable:10
* `src/fn/sub_08221D40.c`: ScalarApproach_ApplyProfileArray:13
* `src/fn/sub_082237A0.c`: RfuManager_StartChildConnect:46
* `src/fn/sub_08228DB8.c`: SunClock_GetPhase:52, SunClock_GetPhase:62, SunClock_GetPhase:99, SunClock_GetPhase:109
* `src/fn/sub_082392E8.c`: sub_082392E8:70, sub_082392E8:73
* `src/fn/sub_08239480.c`: Player_EmitConvergingParticle:57
* `src/fn/sub_0823A028.c`: ActorEffect_SelectFlashingValue:15
* `src/fn/sub_0824AC74.c`: Libgcc_MultiplyInt64:61
* `src/golem_2.c`: Golem_UpdateScriptedAction:393, Golem_UpdateScriptedAction:395
* `src/group_objects.c`: BoardPiece_Wander:493, BoardPiece_Wander:494, BoardPiece_Wander:495, BoardPiece_Wander:496
* `src/modifier_selection_and_stat_previews.c`: ModifierMenu_DrawSecondSourceStats:697, ModifierMenu_DrawSecondSourceStats:716, ModifierMenu_DrawBackground:863
* `src/palette_controller.c`: Palette_BlendRange:69, Palette_InitializeFromScript:461, Palette_SetBrightnessFromScript:709
* `src/password.c`: Text_CopyWithoutTags:9, Password_DecodeEntry:174
* `src/status_screen.c`: NumericDisplay_DrawPair:22, NumericDisplay_DrawStatusValues:68
* `src/status_screen_fade.c`: StatusScreen_FadeInPalette:98
* `src/tether_actor.c`: TetherActor_BeginAuxiliaryState:1122, slopePush_AdjustSlopeMovement:1183, TetherActor_UpdateWanderingState:3125
* `src/wireless_game_items_and_display.c`: WirelessGameItems_UpdateTemporaryItem:887, WirelessGameItems_StartOutgoingItems:1292
