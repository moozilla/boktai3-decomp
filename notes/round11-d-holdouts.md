# Round 11 package D holdouts

Of the 466 assigned rows, 193 functions became concession-free and 25 improved partially. 272 functions retain concessions. One queued function, `Script_InitRuntime` (0821B048), was already clean. A block comment caused the old report to include nearby functions in its body. The neighboring `GameState_SelectRamBase` still has a pin. Its ordinary draft exchanges the two source operands of one Thumb addition.

Each entry records a source experiment and the remaining stock-compiler difference. Every committed partial cleanup reproduces the original machine code. The separate draft archive holds full translation units. Port the named function and its relevant helpers when resuming, because neighboring functions can have moved on. The index gives the normalized assembly-row distance measured against this delivery. Earlier scope counts in the notes use scope’s ROM-side and draft-side instruction counts.

- **0810E9D4 · TimerDisplay_DrawRecord**
  The two returned tile pointers coalesce with r0. In the ROM the pointer moves to r1 before the constant 31 or 1 is stored. A typed pointer and a shared row-fill helper both lose those copies. 6 ROM / 6 draft.

- **0810F348 · RecordMenu_UpdateSelection**
  The left/right mask, glyph pointer, choice pointer and zero compete across the animation call. Removing the hard-register declarations together gets closer than individual removals, but changes the branch-local choice register and the confirm/cancel tests. 17 ROM / 15 draft.

- **0810FDDC · sub_0810FDDC**
  Indexed widget access improves on keeping separate doubled and base indices. The ROM carries the first doubled index across both glyph calls and keeps the tens index in r0 across the decrement loop. 19 ROM / 19 draft (+3 moved branches).

- **081169A8 · Glove_InitializeStyleModifiers**
  Removing its sole pin swaps the participant and link pointer registers. Scope confirms that increasing the link pointer's priority by 967 restores the exact bytes. Extracting the payload lookup and changing declarations or participant widths did not reproduce that allocation.

- **0811AE9C · Terrain_ResolveActorMotion**
  The pin-free query/mask draft differs in 42 scope rows. An exhaustive removal-subset search found no free group. A partly depinned spelling comes within one negate/shift pair in the fixed-point vertical correction.

- **0811B164 · SpatialIndex_SetPosition**
  The registration byte can be named. Without the clobber, the address of the signed bucket and the zero index for its load exchange r0/r1. An ordinary bucket accessor and early rejection still leave six changed rows.

- **08121D14 · SpriteParticle_AllocateSlot_08121D14**
  Removing DEBUG_NOP shares the shifted mask between the test and reservation. Using the narrowed slot index splits the shifts as the ROM does but inserts a halfword-width or byte-width conversion. The pin-free draft is four bytes longer than the ROM.

- **081294B4 · FeatherEffectPool_CreateDirectional**
  Without the velocity operands the signed-direction switch folds components into the angle setup. Rotating the component order and changing their integer widths do not preserve the branch layout. That experiment differs by 14 scope rows.

- **0812A660 · ActorEffect_InitColliders**
  The two relative collider pointers conflict with the power/value call arguments. Removing each original constraint separately leaves the second argument copy or its saved register different. That experiment differs by 44 scope rows.

- **0812C41C · FragmentEffect_SpawnRandomParticle**
  The three sin/cos samples compete with the loop-carried random table and the outgoing call arguments. Subset removal after the earlier partial still perturbs the sampling and output stores. That experiment differs by 46 scope rows.

- **0812CF9C · SceneSlot_EmitPulse**
  The emitter twin reuses the same sampled angles as 0812C41C. The pin-free spelling changes how the table and component values survive the helper calls. That experiment differs by 46 scope rows.

- **0812D064 · LargeFragmentEffect_SpawnRandomParticle**
  Only constrained subsets reproduce the random-table addressing and velocity setup. Dropping the remaining constraints changes the table source and lifespan temporaries. That experiment differs by 13 scope rows.

- **081301C4 · SceneActorReaction_UpdateRouteRecovery**
  After depinning, the signed height copies from ROM r1 to r0 and the stack-address scratch changes from r0 to r1. Signed and unsigned locals, point members and a typed height getter reproduce the same swap. That experiment differs by 3 scope rows.

- **08131820 · SceneActorControl_UpdateRouteApproach**
  The state-entry setup and timer stores now match without their nine old constraints. Removing the remaining five changes the prior-index comparison and the final counter increment. Direct-field and narrow-index alternatives still differ. 5 ROM / 5 draft.

- **08135F70 · SceneActorReaction_ResolveDamage**
  Expanding both once-used flag macros into ordinary blocks loses the loop-depth weighting on the saved motion flags and mode. The resulting spills propagate through the damage-display path. 113 ROM / 109 draft (+7 moved branches).

- **0813727C · sub_0813727C**
  The unpinned resetFlags pointer is used twice in the accepted-motion arm. The allocator assigns different registers to it and the prior flag pointer across ResetControl. 25 ROM / 25 draft.

- **081376EC · sub_081376EC**
  Without the loop-entry barrier the zero index and zero found/tag share different registers. The eight-stimulus scan and its callback tail shift together. 64 ROM / 64 draft.

- **081381CC · LinkSession_UpdatePlayerValueAverage**
  A typed selection-bits accessor restores the session pointer to r1. The remaining differences are the timing of the +0x54 address and which operand supplies the index add. 2 ROM / 2 draft.

- **081383E0 · Registry1745_UpdateParticipantScores**
  The fill loop loses the copy of the 0xffff mask into r2. Halfword and word mask locals and alternate fill loops still hoist the literal straight into r2. 3 ROM / 1 draft.

- **081384A0 · sub_081384A0**
  The pin-free selection sort changes the inner-loop induction values. Reusing the pass counter avoids its copy but changes the post-loop register assignment. 22 ROM / 28 draft (+1 moved branch).

- **08138778 · PositionEffectDisplay_UpdateSources**
  An indexed loop using PlayerCue_StepSequenceEnded reproduces the playback body. Position assignment uses an extra strength-reduced address, and the manager base trades ip and r8 with the stepping pointer. 13 ROM / 17 draft.

- **0813B18C · ActorDirection_ComputeRelativeCardinal**
  Removing 1 local constraint changes 8 normalized assembly rows across 5 regions. The first change drops or moves the addition and load present at that position in the original.

- **0813B4C8 · Player_RecalculateDerivedStats**
  The unpinned contiguous stat loops assign different registers to the quotient, cap and destination. The first loop also reverses its counter. 26 ROM / 26 draft.

- **0813B7C4 · sub_0813B7C4**
  Removing 1 local constraint changes 16 normalized assembly rows across 8 regions. The operations at the first differing site are unchanged, but their operands or branch targets differ.

- **0813C42C · ConvergingEffects_EmitNearParticle**
  Removing the angle pin changes how the two sine table samples and random radius are kept across the spawn path. 11 ROM / 9 draft.

- **0813CB8C · Player_UpdateVisualPalette**
  The palette mode is live across several calls. Plain parameters change allocation and the placement of the final selection branches. 23 ROM / 24 draft (+2 moved branches).

- **0813CE20 · sub_0813CE20**
  Removing 1 local constraint changes 19 normalized assembly rows across 10 regions. The draft introduces addition work at the first differing site. The optimizer substitutes `mode` for `requestedMode`. Changing this decision in the diagnostic compiler reproduces the original bytes. Matchsearch tested 2 further source spellings.

- **0813D834 · sub_0813D834**
  The two pinned values connect the dispatch kind to the scaled status-timer offset across branches. Using normal locals changes the comparisons and merged exit. 17 ROM / 15 draft (+6 moved branches).

- **0813DB98 · Player_UpdateStatusVisuals**
  Removing 1 local constraint changes 72 normalized assembly rows across 32 regions. The first left shift uses r2 where the original uses r0. The compiler merges equivalent values before register allocation. Matchsearch tested 2 further source spellings.

- **0813EDDC · PlayerAction_RefreshFlagState**
  Removing 7 local constraints changes 8 normalized assembly rows across 4 regions. The first register transfer uses r0 where the original uses r1. The reload pass assigns r0 at this site. Changing this decision in the diagnostic compiler reproduces the original bytes. Matchsearch tested 2 further source spellings. The archived revision also removes the remaining constraints from the local helpers.

- **081411B8 · Player_ApplyDirectionalForce**
  The active/input checks merge differently without the barrier on the player pointer, and the scaled Z sample no longer occupies the original register. 5 ROM / 5 draft (+1 moved branch).

- **0814159C · Player_UpdateCameraOffset**
  Splitting input-plus-one from its mask leaves the history sample, camera angle and mode allocated differently across the clamp calls. 20 ROM / 21 draft (+1 moved branch).

- **081432DC · Player_UpdateHeldInteraction**
  Removing 3 local constraints changes 23 normalized assembly rows across 8 regions. The first changed instruction group contains right shift in the original and right shift, addition in the draft. The allocator assigns `value` to r1 where the original uses r0.

- **08146EAC · Player_UpdateAlternateFormCue**
  Removing 1 local constraint changes 11 normalized assembly rows across 5 regions. The first register transfer uses r5 where the original uses r3. The reload pass assigns r5 at this site.

- **08148FC0 · Player_UpdateWalkFootsteps**
  The position used by the nearby-effect query and emission calls loses its independent pointer allocation. Position-local and direct field spellings shift the playback state registers. 11 ROM / 11 draft.

- **0814D9C4 · sub_0814D9C4**
  The combo index is scaled into an offset and then converted to a phase-table pointer in two action arms. Removing either pair of constraints changes the live table registers across the callbacks. 105 ROM / 99 draft (+1 moved branch).

- **0814DE0C · Player_ConfigureActionSelectionDisplay**
  Removing 1 local constraint changes 2 normalized assembly rows across 2 regions. The first changed instruction group contains load in the original and addition in the draft.

- **0814DF0C · Player_IsStatBBelow**
  Removing 1 local constraint changes 4 normalized assembly rows across 4 regions. The first changed instruction group contains load in the original and addition in the draft.

- **0814DF58 · Player_SubtractStatB**
  The ordinary-C spelling changes 8 normalized assembly rows across 8 regions. The first changed instruction group contains load in the original and addition in the draft.

- **0814F328 · Player_UpdateAttachedAttackCharge**
  Using packed three-component vectors and the typed player frame removes the burst and latch constraints. The remaining two held-key address calculations rotate agbcc’s reload registers through the common firing and facing paths. 45 ROM / 40 draft (+1 moved branch).

- **08156F54 · Player_TestTetherActionInput**
  The trigger path loads the global tether pointer a second time in the ROM. Normal C shares the first load over the two state tests. 5 ROM / 3 draft.

- **08156FE0 · Player_SelectActionSequence**
  The selector has six pins, including shared helper state. Removing these changes many branch arms. Subgroup testing found no exact partial. 168 ROM / 94 draft (+7 moved branches).

- **0815767C · Player_ApplyControlPresentation**
  Removing 2 local constraints changes 34 normalized assembly rows across 9 regions. The first load uses r1 where the original uses r0. The reload pass assigns r1 at this site. Matchsearch tested 2 further source spellings.

- **08158C14 · sub_08158C14**
  Four independently computed counters lose their joint address lifetimes when the operand barrier is removed. The reset writes are reordered. 20 ROM / 20 draft.

- **08159164 · sub_08159164**
  Removing 1 local constraint changes 21 normalized assembly rows across 6 regions. The first instruction uses r4 where the original uses lr.

- **0815A048 · sub_0815A048**
  Unpinning the previous flags, bit 4 and the halfword zero merges the zero sources for the byte and halfword resets. 43 ROM / 44 draft (+1 moved branch).

- **0815A73C · sub_0815A73C**
  The maximum-health and current-health offsets need to survive over separate loads and stores. Removing the two offsets’ constraints changes the temporary addresses. 25 ROM / 25 draft.

- **0815E034 · sub_0815E034**
  The contact collider, damage, flags and common mask compete across the cost branches. The flag-reading helper also retains a masked result that ordinary C elides. 117 ROM / 83 draft (+17 moved branches).

- **0815E1F0 · PlayerContact_ApplyResponse**
  Removing 2 local constraints changes 14 normalized assembly rows across 6 regions. The first changed instruction group contains load, addition, register transfer in the original and load in the draft. The allocator assigns `power` to r1 where the original uses r2.

- **0815ED78 · Player_ApplyLatchedActionMode**
  Removing 1 local constraint changes 9 normalized assembly rows across 3 regions. The first register transfer uses r1 where the original uses r0. The reload pass assigns r1 at this site.

- **0815F5B4 · TargetRegistry_FindById**
  The requested ID and the scene-actor mask exchange r3/r4. Early returns, separate comparisons on the two ID widths and a shared equality test all preserved the mismatch-only retry path but did not resolve the allocation. 5 ROM / 5 draft.

- **08160108 · PlayerBurst_InitializeParticles**
  Writing the random-sample helper as an ordinary halfword read retains its address arithmetic. The caller’s random-index and particle loop allocation still changes in 27 regions.

- **081606C0 · sub_081606C0**
  The two sets of flag-accumulation tests require different mask and first-bit registers. Removing the constraints as a group changes both paths. 16 ROM / 18 draft.

- **081608B0 · PlayerAttackEffect_EmitPaletteBaseTrail**
  The trail angle in r9 survives the particle-spawn setup. Without the pin the palette and position field offsets rotate through different reload registers. 88 ROM / 88 draft.

- **08162BE4 · PlayerBarrier_TryPlace**
  Removing the placement pin changes the live registers in the barrier-placement path. 12 ROM / 12 draft.

- **08163020 · sub_08163020**
  The unpinned temporary changes register allocation across the collision tests. 24 ROM / 24 draft.

- **081639E4 · sub_081639E4**
  Both index/address pins feed table access. The index and its derived pointer coalesce differently when removed together. 41 ROM / 37 draft (+2 moved branches).

- **08164480 · sub_08164480**
  The pointer read now matches without volatile qualification. Its address must still reach r0 before the HUD base add at each inline site. That experiment differs by 38 scope rows.

- **08164DC0 · Hud_UpdateTransientStatGauge**
  Two fill loops load a mask directly into the register where the ROM first copies it. The final 0xa7f flag-store offset also advances the reload order. 8 ROM / 6 draft.

- **08165D10 · SceneHud_TickPaletteBlend**
  The green/blue extraction leaves 31 available. The unpinned red expression reuses it. The ROM materializes 31 a second time and ANDs the color into it. 2 ROM / 2 draft.

- **081680F4 · Hud_LoadConditionalPaletteRows**
  The unpinned index and final zero change how the loop and palette stores allocate temporaries. 11 ROM / 11 draft.

- **08168A0C · SceneHud_RebuildPrimaryOptions**
  The pinned state pointer feeds the initial selected-slot and list accesses. Removing its pin changes the reload chosen for the current option state. 9 ROM / 9 draft.

- **08168EE8 · SceneHud_TickWidgetSlides**
  The three expanded slide loops keep their destination cursor in a shared hard register. A plain cursor changes the saved-register assignment for the three loops. 34 ROM / 34 draft.

- **08169A9C · StatusPanel_DrawDecimalValue**
  The tile attributes 0xffffd000 load directly to r7. The ROM loads r1 and copies it to r7 before the digit loop. 2 ROM / 2 draft.

- **0816B958 · MapMenu_DrawActiveMarkers**
  The draw helper and caller pin the marker index, base and attributes. Removing them changes addressing and the register used for the 0xe000 or 0xe400 tile flags. 26 ROM / 14 draft (+3 moved branches).

- **0816BC54 · sub_0816BC54**
  The five register constraints in this multi-branch map update are coupled. Their joint removal changes register allocation in several dispatch arms. 150 ROM / 138 draft (+10 moved branches).

- **0816D4E4 · SceneHud_CanApplySelectedAction**
  The inventory-present path loads the signed slot again for Inventory_GetEquipmentEntry. Reversing the test or taking the address from the game-state array still lets cse reuse the checked value. That experiment differs by 18 scope rows.

- **0816D634 · sub_0816D634**
  The empty asm precedes the shared clear path for two item-duration cases. Without it agbcc cross-jumps the cases on a different boundary. 2 ROM / 0 draft (+1 moved branch).

- **0816E570 · sub_0816E570**
  The random table base must survive the Mod calls for the velocity samples. Removing the r6 pin changes the ownership of that register. 26 ROM / 22 draft.

- **0816E6F0 · sub_0816E6F0**
  Removing both counter/phase mask barriers and their pins changes the halfword mask/activation stores. 9 ROM / 9 draft.

- **0816ECF0 · PauseMenu_BeginVitalRatioExchange**
  The two fixed registers hold a field offset and address. Normal offset calculation changes local allocation around the gauge setup. 22 ROM / 22 draft.

- **08170D78 · sub_08170D78**
  The ROM reloads a signed item slot after testing it, even though no store or call intervenes. The barrier-free body reuses the first load and changes the later reload rotation. 27 ROM / 23 draft.

- **08171014 · SceneHud_DrawComparisonBars**
  The comparison-bar fill wants its shared tile bits copied into a saved register before the loop. Ordinary mask and typed widget forms change the literal materialization. That experiment differs by 12 scope rows.

- **08171AF8 · SceneHud_UpdateActionSwapSelection**
  The A-key branch emits the same AND with the inputs exchanged. Direct reads from gPadState, typed mask locals and both operand orders keep a copy of the keys instead of the constant. 2 ROM / 2 draft.

- **08171EFC · sub_08171EFC**
  The ROM narrows the incremented discard timer to a halfword twice. Using the field again or splitting the increment and store lets agbcc share the first narrowing. 2 ROM / 0 draft.

- **08173C0C · SceneHud_TickEquipmentToggle**
  The equipment toggle must keep the action-input address available but separately computed around grid navigation. Removing the debug block changes its saved register. That experiment differs by 9 scope rows.

- **08173E68 · SceneHud_TickEquipmentSwap**
  The cancel mask is now ordinary C. Without the remaining confirm mask barrier the A-key AND starts with a copy of the keys rather than the mask held in r7. 2 ROM / 2 draft.

- **08178C80 · sub_08178C80**
  The two initialization sites store 0xff from a negated one. The helper’s unbound value folds into a byte constant and changes later scratch use. That experiment differs by 24 scope rows.

- **08179C38 · ActorImpact_CopyContact**
  Removing 1 local constraint changes 10 normalized assembly rows across 10 regions. The first addition uses r7 where the original uses r6. Scope also identifies an instruction-order change at this site.

- **0817A354 · sub_0817A354**
  Removing 1 local constraint changes 8 normalized assembly rows across 6 regions. The first addition uses r4 where the original uses r5. Scope also identifies an instruction-order change at this site. Changing this decision in the diagnostic compiler reproduces the original bytes. Matchsearch tested 2 further source spellings.

- **0817AD7C · BossSlot2_SpawnBobbingFormation**
  The address of the four-halfword position array moves to just before the loop. The ROM forms it before setting up the angle and origin. Using the same pointer for GetOffset, the coordinate additions and the actor creation call does not keep the earlier address materialization. The stock draft differs by 1 ROM / 1 draft.

- **0817AEC8 · sub_0817AEC8**
  Removing the table operand changes which address stays live in the six-iteration loop. Testing sampled and direct reads separately at all three call sites leaves the loop setup and sine-table allocation displaced. Scope measured 51 ROM / 45 draft (+3 moved branches).

- **0817B0B4 · BossSlot2_UpdateSequencePalette**
  Removing 4 local constraints changes 25 normalized assembly rows across 10 regions. The first load uses r2 where the original uses r3. The reload pass assigns r2 at this site.

- **0817BCD4 · ActorDecision_UpdatePulsePhase**
  The actor pointer occupies r4 and its copy occupies r6. CSE substitutes r4 for r6 at just the reachability and nearby-position calls. Splitting the phase decision into an inline helper and changing which pointer supplies each argument did not preserve both calls. The stock draft differs by 2 ROM / 2 draft.

- **0817D67C · ActorEffect_InitializeOffset**
  The sine sample for Z goes to r3, while the ROM keeps it in r2 through the signed fixed-point scaling. Forcing the Z pseudo into r2 verifies the diagnosis. Separating angle, table-index and scale locals did not recover that choice. The stock draft differs by 5 ROM / 5 draft.

- **0817EBB4 · BossSlot2_UpdateSummonSpinSequence**
  Removing 3 local constraints changes 20 normalized assembly rows across 9 regions. The first addition uses r1 where the original uses r2. Matchsearch tested 2 further source spellings.

- **0817F9C0 · BossSlot2_UpdatePresentation**
  Removing 1 local constraint changes 21 normalized assembly rows across 8 regions. The first register transfer uses r3 where the original uses r1. The reload pass assigns r3 at this site. Changing this decision in the diagnostic compiler reproduces the original bytes. Matchsearch tested 3 further source spellings.

- **0817FDE0 · sub_0817FDE0**
  Removing 1 local constraint changes 76 normalized assembly rows across 35 regions. The first register transfer uses r5 where the original uses r4. Scope also identifies an instruction-order change at this site. Changing this decision in the diagnostic compiler reproduces the original bytes.

- **08180570 · SpriteComponent_Update**
  The ROM materializes -96 in callee-saved r5, copies it into r0 and then subtracts the sort offset. Ordinary signed, halfword and byte sort expressions put the constant straight into r0, eliminating the save and restore of r5. The stock draft differs by 5 ROM / 3 draft.

- **08180730 · ProximityVolume_Update**
  Removing 1 local constraint changes 23 normalized assembly rows across 19 regions. The first addition uses r4 where the original uses r5. Scope also identifies an instruction-order change at this site. Changing this decision in the diagnostic compiler reproduces the original bytes.

- **08181634 · ArmorPalette_CopyKindBanks**
  Removing 4 local constraints changes 11 normalized assembly rows across 9 regions. The first addition uses r5 where the original uses r6. The allocator assigns `s0` to r5 where the original uses r6.

- **0818171C · ActorPalette_LoadScriptBanks**
  Using a byte offset measured from the palette owner matches the last destination address, with the two inputs to the Thumb ADD exchanged. Array views, integer versus pointer addition, and passing the last destination through an inline helper all preserve the computed address. The stock draft differs by 1 ROM / 1 draft.

- **08182454 · ActorController_SetAnimation**
  Removing 9 local constraints changes 28 normalized assembly rows across 11 regions. The first changed instruction group contains register transfer, instruction in the original and register transfer, instruction in the draft. Matchsearch tested 4 further source spellings.

- **08186CC8 · Armor_SelectAlignedAttack**
  Expanding the grid, magnitude and branch macros into ordinary C changes register allocation around the facing tests. The first block-only version differs across 33 assembly regions.

- **08189FF8 · Armor_UpdateModeAttackDecision**
  Expanding the axis-facing and component macros changes how the compiler holds the grid deltas across the attack decision branches. The block-only version differs across 51 assembly regions.

- **0818CA6C · Armor_RetractPartStretch**
  Removing 4 local constraints changes 15 normalized assembly rows across 5 regions. The first change drops or moves the register transfer and mask present at that position in the original. The allocator assigns `one` to r2 where the original uses r4.

- **0818CB30 · Armor_UpdatePartOrbit**
  Removing 2 local constraints changes 25 normalized assembly rows across 10 regions. The first changed instruction group contains register transfer, instruction in the original and register transfer, instruction in the draft.

- **0818D21C · Armor_InitPartLunge**
  Removing 3 local constraints changes 10 normalized assembly rows across 8 regions. The first change drops or moves the register transfer present at that position in the original.

- **0818E9B8 · SpriteColliderGroup_ActivateParts**
  Removing 2 local constraints changes 10 normalized assembly rows across 5 regions. The first changed instruction group contains register transfer, instruction, addition in the original and addition in the draft. Scope also identifies an instruction-order change at this site.

- **0818EA3C · SpriteColliderGroup_ResetActiveParts**
  The very first count load and comparison use r6 where the ROM uses r5. The reload trace reports that r5 is unavailable as a spill register at that instruction. Count accessors, narrowing/widening the local and rewriting the loop entry did not change both registers. The stock draft differs by 2 ROM / 2 draft.

- **0818EA90 · Armor_QueueFallingParts**
  The loop index and the active-count pointer trade r8 and ip. Raising the active-count pseudo priority by 138 reproduces the ROM. Tried changing the index initialization, ordering the four cursor initializations, and storing through typed field pointers. The stock draft differs by 6 ROM / 6 draft.

- **0818ECFC · sub_0818ECFC**
  Removing 1 local constraint changes 65 normalized assembly rows across 28 regions. The first register transfer uses r9 where the original uses r0. The allocator assigns `pool` to sb where the original uses r4.

- **08198B70 · SceneEffect_InitActor**
  Removing 1 local constraint changes 4 normalized assembly rows across 3 regions. The first register transfer uses r1 where the original uses r0. Scope also identifies an instruction-order change at this site. Changing this decision in the diagnostic compiler reproduces the original bytes. Matchsearch tested 1 further source spelling.

- **08198F3C · SceneEffect_InitializeRing**
  Removing 1 local constraint changes 5 normalized assembly rows across 3 regions. The first register transfer uses r1 where the original uses r0. Scope also identifies an instruction-order change at this site. Changing this decision in the diagnostic compiler reproduces the original bytes.

- **0819946C · SceneEffectQueue_PostTypeZeroMessage**
  The three message posts share PostSceneEffect. Expanding the projection macro moves coordinate and camera-origin loads in the projection calculation. The combined unit differs in 24 regions.

- **08199578 · SceneEffectQueue_PostTypeOneMessage**
  This caller uses the same projection helper as the type-zero message. The shared ordinary-C version changes the allocation of the projected coordinates before message construction.

- **08199684 · SceneEffectQueue_PostTypeThreeMessage**
  The projected-side result feeds the type-three message arguments. Expanding the shared projection changes the order of coordinate loads, so the three callers still need a shared source form.

- **0819B488 · sub_0819B488**
  The mutable value pointer takes r6 and the loop count takes r5 instead of the ROM’s opposite assignment. Moving the forward pointer increments on either side of the store or expressing them as post-increment does not fix the order. Scope measured 11 ROM / 11 draft.

- **0819B8B4 · sub_0819B8B4**
  This is the angle-unwrapping sibling of 0819B488. Range-preserving variants of the cursor updates and the sibling coefficient helper retain the same r5/r6 swap. Scope measured 11 ROM / 11 draft.

- **0819BB18 · sub_0819BB18**
  The pinned setup copies the bank address through three tied operands. In this correct ordinary draft the bank starts at 0x0203B000 and randomValue adds 512 halfwords. Using the shared bank helper changes which load is hoisted and the registers for the output vectors. Scope measured 30 ROM / 23 draft.

- **0819BCB8 · Hresvelgr_EmitFacingMotion**
  The relative sample helper recovers the real use of the apparent 0x0403B714 dead literal. The ordinary draft loses the actor/side allocation and the arithmetic-right-shift quotient scratch. The register priority knob tested for the actor improves only part of the difference. Scope measured 54 ROM / 54 draft. The archived revision also removes the remaining constraints from the local helpers.

- **0819CA38 · Hresvelgr_RefreshTarget**
  Removing the final travel-result pin changes the square temporary and lets the halfword store use the return register directly. Several field-pointer, scalar-width, and union views could not recover the otherwise redundant ROM copy. Scope measured 8 ROM / 6 draft.

- **0819F788 · Hresvelgr_UpdateAlternatingVolley**
  The alternating volley computes three wrapped indices across a call. Removing the high register constraints changes the cursor alias and alters the post-call loop decrement. Shared table views do not recover both. Scope measured 57 ROM / 55 draft (+3 moved branches).

- **0819FC34 · Hresvelgr_UpdateTurningCharge**
  The mask-first direction setters are clean in the landed function. Giving SetFlags a direct actor->flags update still changes which register holds the flags address during the charge. Scope measured 16 ROM / 14 draft.

- **081A09C4 · Hresvelgr_PlayDescent**
  The typed actor pointer merges back into the input without the operand. That changes the low register retained across flight and animation calls. The compiler-directed search found only equivalent reload forms. Scope measured 11 ROM / 5 draft.

- **081A0B20 · Hresvelgr_PlayNearestPointTravel**
  The ordinary function merges the second actor pointer with the input, losing the r4-to-r7 copy at entry. The ROM uses the saved pointer for the later UpdateTravel call. Named pointer and independent zero forms did not restore that split. Scope measured 4 ROM / 4 draft (+1 moved branch).

- **081A0BC8 · Hresvelgr_UpdateBoundaryBarrage**
  The phase index is no longer kept in r5 after the pins are removed. Unpinning the action-offset input alone primarily changes reload order at the tail. Scope measured 28 ROM / 28 draft.

- **081A12DC · Hresvelgr_UpdateRisingProjectile**
  The quarter-direction pointer returned by SetHeading loses r5, which changes several allocations across the projectile construction calls. The independent mask-first heading helper resolves the direction math but leaves that pointer allocation. Scope measured 48 ROM / 48 draft.

- **081A14FC · sub_081A14FC**
  With the other constraints retained, deleting just the Y sample operand changes one instruction from asr to lsr. Removing the table and result pins together changes the sampling-loop registers. The fixed-address and shared bank forms were tested independently. Scope measured 90 ROM / 85 draft (+1 moved branch).

- **081A4DF0 · sub_081A4DF0**
  The ROM leaves the last Anim_SetKeyframes result in r0 while clearing the final projectile entry and active field. An explicit result local introduces a register copy. Both counted and explicit-last-iteration return forms retain the difference. Scope measured 12 ROM / 14 draft.

- **081A681C · ScreenPalette_UpdateTransition**
  The final palette bank index loses the r0 temporary for variant times three, merging the copy used to form bank+9. Hoisting the multiplication or narrowing its local does not recreate the copy. Scope measured 4 ROM / 4 draft.

- **081A8888 · Vanargand_UpdateTimedAlternateTransition**
  In the alternate-zero branch, the ROM keeps mode 13 in r1 and the incrementing destination in r0. Expressing the writes through the fields or duplicating the action-stop tail still swaps them. Scope measured 10 ROM / 10 draft.

- **081A8E08 · sub_081A8E08**
  The capture-setup helper returns its Y coordinate for later use as an animation parameter. Without its pin the zero has a different lifetime from the pending flag clear and uses a different low register. Scope measured 13 ROM / 9 draft.

- **081A93A0 · sub_081A93A0**
  The attack controller has both a switch-state allocation and helper-level constraints on vector operations. Removing hint pairs or rewriting phase transitions with ordinary phase arithmetic leaves register and shape differences. Scope measured 57 ROM / 53 draft (+11 moved branches).

- **081A95A8 · Vanargand_UpdatePairedAttack**
  The ROM holds the phase across calls and resets the timer from r4 in case zero. Without the pin GCC propagates zero and cross-jumps that reset with another arm. Explicit returns and phase-relative increments still merge the tails. Scope measured 9 ROM / 7 draft (+1 moved branch).

- **081A96E0 · Vanargand_UpdateSequencedPairAttack**
  Removing 3 local constraints changes 45 normalized assembly rows across 22 regions. The draft introduces call work at the first differing site.

- **081A9910 · Vanargand_UpdateSynchronizedPairAttack**
  Removing 2 local constraints changes 12 normalized assembly rows across 6 regions. The first load uses r0 where the original uses r1. The reload pass assigns r0 at this site.

- **081A9F88 · Vanargand_UpdateChargedPairAttack**
  Removing 3 local constraints changes 16 normalized assembly rows across 5 regions. The first addition uses r4 where the original uses r0. The allocator assigns the live value to r4 where the original uses r0.

- **081AA178 · Vanargand_UpdateRepeatedPairAttack**
  Exactly one immediate-zero load disappears when resetting the sixth repeat. GCC stores the already-live callback result from r7. The nonmatching draft uses that ordinary zero path. Scope measured 2 ROM / 2 draft.

- **081AA330 · sub_081AA330**
  The sole remaining mismatch is r1 instead of r2 for the 0x56c offset of the selected-halves object. Using p->obj directly or distributing the timer increment across the preceding mask cases keeps that reload rotation. Scope measured 2 ROM / 2 draft.

- **081AA650 · Vanargand_UpdateTimedAnimationAttack**
  Phase dispatch and the flags pointer compete for r2/r6. Retaining one pin saves instructions but does not match. Phase-relative increments and duplicating case exits leave the wrong allocations. Scope measured 18 ROM / 18 draft.

- **081ACC24 · Vanargand_ApplyPendingDamage**
  The stage colors are clean through the shared palette helper. The damage accumulator, shifted part-damage amount and mask have residual pin/operand constraints independently of the palette path. Scope measured 51 ROM / 48 draft (+6 moved branches).

- **081AF540 · Vanargand_ProjectEffectOffset**
  The packed-coordinate constructor wants x and y in r5/r6. Unpinned C reverses them. Reordering the sum gets the add instruction right. Both halfword loads and the subtraction are still reversed. Scope measured 3 ROM / 3 draft.

- **081AFD10 · PairedSceneActor_UpdateHalfPresentation**
  Removing 28 local constraints changes 63 normalized assembly rows across 24 regions. The first load uses r4 where the original uses r6. Matchsearch tested 5 further source spellings.

- **081B0018 · PairedSceneActor_InitializeHalf**
  Removing 3 local constraints changes 60 normalized assembly rows across 29 regions. The first changed instruction group contains addition in the original and register transfer in the draft. Scope also identifies an instruction-order change at this site.

- **081B08B8 · ActorStatus_UpdatePlayerApproach**
  Removing 5 local constraints changes 8 normalized assembly rows across 6 regions. The first addition uses r4 where the original uses r6. Matchsearch tested 2 further source spellings.

- **081B5EA4 · sub_081B5EA4**
  At controller initialization end the 0x5b4 state offset goes in r3 instead of r2, shifting the following two-way switch mode copy. Porting the sibling loop’s stride shape did not fix this unrelated post-loop allocation. Scope measured 5 ROM / 5 draft.

- **081B66E0 · VanargandEffect_Initialize**
  Removing 1 local constraint changes 2 normalized assembly rows across 1 region. The first changed instruction group contains addition in the original and subtraction in the draft. Matchsearch tested 2 further source spellings.

- **081B6E90 · ActorEffect_UpdateRadialExpiry**
  The six-iteration trail loop assigns the index, table, RNG cursor and 1023 mask to four different registers. Shared sampled/direct helper combinations and typed cursor loads improve the count without restoring the ROM’s full rotation. Scope measured 16 ROM / 16 draft.

- **081B95E4 · MotionTrack_Update**
  Removing 2 local constraints changes 2 normalized assembly rows across 1 region. The first register transfer uses r3 where the original uses r1. The reload pass assigns r3 at this site. Changing this decision in the diagnostic compiler reproduces the original bytes.

- **081BB904 · sub_081BB904**
  Removing 5 local constraints changes 16 normalized assembly rows across 8 regions. The first mask uses r1 where the original uses r0. The allocator assigns `h` to r1 where the original uses r0. Matchsearch tested 2 further source spellings.

- **081BBE88 · sub_081BBE88**
  Removing 1 local constraint changes 5 normalized assembly rows across 3 regions. The first load uses r2 where the original uses r1. The allocator assigns the live value to r2 where the original uses r1.

- **081BC820 · StatGauge_BeginAdjustment**
  Removing 4 local constraints changes 65 normalized assembly rows across 35 regions. The first addition uses r6 where the original uses r7. The compiler merges equivalent values before register allocation.

- **081BD38C · DarkLoan_BuildDisplay**
  Removing 1 local constraint changes 77 normalized assembly rows across 21 regions. The first changed instruction group contains addition in the original and register transfer, addition in the draft.

- **081BDE94 · Storage_BeginSwapTransition**
  Removing 7 local constraints changes 47 normalized assembly rows across 9 regions. The first changed instruction group contains addition, bitwise complement, right shift in the original and bitwise complement, right shift, load in the draft. The allocator assigns the live value to r1 where the original uses r0. Matchsearch tested 1 further source spelling.

- **081BF67C · Storage_InitWidgets**
  Removing 3 local constraints changes 5 normalized assembly rows across 4 regions. The first change drops or moves the register transfer present at that position in the original. The compiler merges equivalent values before register allocation. Matchsearch tested 2 further source spellings.

- **081C1918 · ControlPanel_UpdateGaugeIndicator**
  Removing 3 local constraints changes 9 normalized assembly rows across 5 regions. The first load uses r1 where the original uses r2. The reload pass assigns r1 at this site.

- **081C1E48 · sub_081C1E48**
  Removing 1 local constraint changes 2 normalized assembly rows across 2 regions. The first subtraction uses r1 where the original uses r7. The allocator assigns `factor` to r1 where the original uses r7.

- **081C2570 · ControlPanel_InitializeFromScript**
  Removing 2 local constraints changes 72 normalized assembly rows across 48 regions. The first addition uses r5 where the original uses r7. The compiler merges equivalent values before register allocation.

- **081C4708 · sub_081C4708**
  Removing 1 local constraint changes 5 normalized assembly rows across 3 regions. The first addition uses r1 where the original uses r2. The allocator assigns `script` to r1 where the original uses r2.

- **081C58BC · sub_081C58BC**
  Removing 1 local constraint changes 2 normalized assembly rows across 1 region. The first changed instruction group contains register transfer, left shift in the original and subtraction in the draft.

- **081C62B4 · sub_081C62B4**
  Removing 1 local constraint changes 6 normalized assembly rows across 5 regions. The first addition uses r5 where the original uses r6. Scope also identifies an instruction-order change at this site. Changing this decision in the diagnostic compiler reproduces the original bytes.

- **081C6954 · StateActor_CreateFromScriptEntry**
  Removing 8 local constraints changes 103 normalized assembly rows across 36 regions. The first changed instruction group contains register transfer, instruction in the original and register transfer, instruction in the draft. Scope also identifies an instruction-order change at this site. Matchsearch tested 1 further source spelling.

- **081C7024 · sub_081C7024**
  Removing 2 local constraints changes 4 normalized assembly rows across 1 region. The first addition uses r0 where the original uses r2. Matchsearch tested 1 further source spelling.

- **081C74F0 · StateActor_UpdateDashAnimation13**
  Removing 1 local constraint changes 36 normalized assembly rows across 31 regions. The first addition uses r5 where the original uses r4. Scope also identifies an instruction-order change at this site. Changing this decision in the diagnostic compiler reproduces the original bytes.

- **081C7934 · StateActor_UpdateAnimation16State**
  Removing 1 local constraint changes 11 normalized assembly rows across 7 regions. The first right shift uses r2 where the original uses r3.

- **081C8EC0 · sub_081C8EC0**
  The radial constructor naturally uses the 0x0403B714 origin for the late relative sample, but its capture displaces the tag pointer, radius, and angle. Reordering capture and real sample reads keeps a different prologue. Scope measured 12 ROM / 11 draft.

- **081C8FB0 · sub_081C8FB0**
  A typed targeting overlay places range at the observed 0x228 offset. The ordinary member access folds the overlay base and changes the distance-test instructions.

- **081CB294 · StateActor_UpdateRiseToTerrain**
  Removing 2 local constraints changes 33 normalized assembly rows across 22 regions. The first addition uses r5 where the original uses r6.

- **081CB5B0 · sub_081CB5B0**
  Removing 1 local constraint changes 2 normalized assembly rows across 1 region. The first changed instruction group contains register transfer, store in the original and store in the draft. The compiler merges equivalent values before register allocation.

- **081CBB1C · sub_081CBB1C**
  Removing 2 local constraints changes 4 normalized assembly rows across 1 region. The first addition uses r0 where the original uses r2. Matchsearch tested 1 further source spelling.

- **081CC1B4 · sub_081CC1B4**
  Removing 10 local constraints changes 32 normalized assembly rows across 18 regions. The first addition uses r3 where the original uses r2. Matchsearch tested 2 further source spellings.

- **081CD484 · SceneNpc_DispatchCutsceneCommands**
  Removing 4 local constraints changes 66 normalized assembly rows across 9 regions. The first changed instruction group contains load, addition, left shift in the original and load, addition, left shift in the draft. The allocator assigns the live value to r2 where the original uses r1.

- **081CEE50 · sub_081CEE50**
  Removing 2 local constraints changes 16 normalized assembly rows across 3 regions. The first load uses r1 where the original uses r2. The allocator assigns `a` to r1 where the original uses r2. Matchsearch tested 1 further source spelling.

- **081CF398 · sub_081CF398**
  An ordinary palette interpolation call with the two bank addresses in locals changes five assembly regions. The old fade wrapper changes the lifetime of the bank and tick values.

- **081D0864 · sub_081D0864**
  Removing 2 local constraints changes 17 normalized assembly rows across 9 regions. The first load uses r2 where the original uses r1. The compiler merges equivalent values before register allocation.

- **081D11E4 · SceneNpc_SpawnTransitionParticle**
  The ROM pins the random table in r6 across calls to Mod. Reading the four samples through a shared indexed helper recovers some of the loop but not the full register allocation. Scope measured 25 ROM / 21 draft (+2 moved branches).

- **081D1404 · SceneNpc_UpdateTransitionEffect**
  Removing 1 local constraint changes 7 normalized assembly rows across 3 regions. The first changed instruction group contains comparison, conditional branch, register transfer in the original and bitwise complement, right shift, branch in the draft.

- **081D1A8C · sub_081D1A8C**
  Removing 2 local constraints changes 4 normalized assembly rows across 1 region. The first addition uses r0 where the original uses r2. Matchsearch tested 1 further source spelling.

- **081D1C38 · sub_081D1C38**
  Removing 2 local constraints changes 4 normalized assembly rows across 1 region. The first addition uses r0 where the original uses r2. Matchsearch tested 1 further source spelling.

- **081D2318 · TaggedSceneNpc_UpdateTargetFacing**
  Removing 1 local constraint changes 20 normalized assembly rows across 10 regions. The first sign shift uses r1 where the original uses r3. Scope also identifies an instruction-order change at this site. Matchsearch tested 1 further source spelling.

- **081D2D78 · sub_081D2D78**
  Removing 2 local constraints changes 18 normalized assembly rows across 14 regions. The first addition uses r7 where the original uses r6. The compiler merges equivalent values before register allocation.

- **081D2EA4 · sub_081D2EA4**
  Passing a sprite and sequence to the common stepper leaves the sprite in a saved low register and removes the ROM's repeated transfers from ip. The ordinary embedded-sprite version also computes the command address through the sequence instead of its owner.

- **081D3094 · sub_081D3094**
  Removing 2 local constraints changes 6 normalized assembly rows across 4 regions. The first register transfer uses r0 where the original uses r9. Scope also identifies an instruction-order change at this site. Changing this decision in the diagnostic compiler reproduces the original bytes.

- **081D31EC · sub_081D31EC**
  Removing 2 local constraints changes 12 normalized assembly rows across 6 regions. The first register transfer uses r3 where the original uses r6. Scope also identifies an instruction-order change at this site.

- **081D3620 · sub_081D3620**
  The cloud loop’s table, coordinate scratch, and random value have different allocation without constraints. The shared sample macro narrows the difference while keeping the same wrapped indices. Scope measured 24 ROM / 24 draft.

- **081D4024 · PaletteTransition_CopyBanks**
  Removing 2 local constraints changes 11 normalized assembly rows across 9 regions. The first addition uses r5 where the original uses r6. The allocator assigns `c` to r5 where the original uses r6.

- **081D61C0 · sub_081D61C0**
  Removing 6 local constraints changes 85 normalized assembly rows across 45 regions. The operations at the first differing site are unchanged, but their operands or branch targets differ. Scope also identifies an instruction-order change at this site. Matchsearch tested 1 further source spelling.

- **081D8934 · Task4E01_UpdateTrailImpact**
  Removing 1 local constraint changes 4 normalized assembly rows across 2 regions. The first load uses r1 where the original uses r3. The compiler merges equivalent values before register allocation. Matchsearch tested 1 further source spelling.

- **081D905C · TimedEffect_UpdateHoming**
  Removing 1 local constraint changes 105 normalized assembly rows across 45 regions. The first addition uses r6 where the original uses r7.

- **081D9D3C · Projectile_SpawnImpactPair**
  Expanding the three RNG setup assignments removes the private macro but swaps the parameter-buffer and table registers inside the impact loop. Mixed shared/direct sample calls at the five reads did not restore the allocation. Scope measured 50 ROM / 44 draft (+9 moved branches).

- **081DB474 · Options_UpdateScreen**
  Removing 1 local constraint changes 2 normalized assembly rows across 2 regions. The first change drops or moves the load present at that position in the original.

- **081DB794 · Options_InitScreen**
  Removing 1 local constraint changes 149 normalized assembly rows across 40 regions. The first changed instruction group contains register transfer, instruction in the original and register transfer, instruction in the draft. The allocator assigns `zero` to r6 where the original uses r0.

- **081DE0CC · NameEntry_Initialize**
  Removing 2 local constraints changes 88 normalized assembly rows across 29 regions. The operations at the first differing site are unchanged, but their operands or branch targets differ.

- **081DEE38 · Time_InitializeDateMenu**
  Removing 3 local constraints changes 148 normalized assembly rows across 24 regions. The operations at the first differing site are unchanged, but their operands or branch targets differ. The allocator assigns `zero` to r5 where the original uses r2.

- **081E09A8 · sub_081E09A8**
  Removing 1 local constraint changes 4 normalized assembly rows across 2 regions. The operations at the first differing site are unchanged, but their operands or branch targets differ. Scope also identifies an instruction-order change at this site. Matchsearch tested 3 further source spellings.

- **081E0DD8 · GameStart_DrawSaveInfo**
  Removing 1 local constraint changes 74 normalized assembly rows across 15 regions. The first load uses r0 where the original uses r5. The allocator assigns the live value to r0 where the original uses r5.

- **081E2910 · Presentation_UpdateDropShakeBlink**
  Removing 4 local constraints changes 76 normalized assembly rows across 38 regions. The first changed instruction group contains addition, register transfer, load in the original and addition, load in the draft. The allocator assigns the live value to r6 where the original uses r0. Matchsearch tested 6 further source spellings.

- **081EC388 · ActorEffect_SpawnRandomizedBurst**
  Removing 1 local constraint changes 21 normalized assembly rows across 12 regions. The first register transfer uses r8 where the original uses r9. The allocator assigns the live value to r8 where the original uses sb.

- **081EC4AC · ActorEffect_SpawnBurstAtAnchor**
  Removing 1 local constraint changes 21 normalized assembly rows across 12 regions. The first register transfer uses r8 where the original uses r9. The allocator assigns the live value to r8 where the original uses sb.

- **081EC628 · sub_081EC628**
  Removing 1 local constraint changes 23 normalized assembly rows across 4 regions. The draft introduces register transfer work at the first differing site. The allocator assigns `mask` to r2 where the original uses r0.

- **081EC670 · ScreenLayers_HidePair**
  Removing 2 local constraints changes 5 normalized assembly rows across 3 regions. The first load uses r2 where the original uses r0. The allocator assigns `control` to r2 where the original uses r0. Matchsearch tested 2 further source spellings.

- **081EC7D8 · ActorDisplay_UpdateForegroundSprite**
  Removing 2 local constraints changes 74 normalized assembly rows across 26 regions. The first addition uses r6 where the original uses r5. The compiler merges equivalent values before register allocation.

- **081EC974 · ActorDisplay_UpdateAccentSprite**
  Removing 1 local constraint changes 12 normalized assembly rows across 3 regions. The first register transfer uses r4 where the original uses r1. The allocator assigns `screen` to r1 where the original uses r0.

- **081ED8A0 · SceneActors_UpdateDelayedMovement**
  Both flip tests now use the packed control byte. The fully shared step reloads duration on the hold path where the ROM preserves an extra copy, changing allocation in both animations. A source local for duration partly closes that gap.

- **081EF914 · SingletonSlot7_UpdateEndingScene**
  Removing 7 local constraints changes 122 normalized assembly rows across 86 regions. The first register transfer uses r8 where the original uses r9. The optimizer substitutes `context` for `p`. Matchsearch tested 2 further source spellings.

- **081F02EC · SingletonSlot7_BeginScaledCooldown**
  Removing 6 local constraints changes 3 normalized assembly rows across 1 region. The first register transfer uses r2 where the original uses r1. The reload pass assigns r2 at this site. Matchsearch tested 1 further source spelling.

- **081F05D0 · SingletonSlot7_OnTargetContact**
  Removing 6 local constraints changes 2 normalized assembly rows across 1 region. The first load uses r4 where the original uses r1. The reload pass assigns r4 at this site. Matchsearch tested 1 further source spelling.

- **081F1B14 · SingletonSlot7_Update**
  Removing 1 local constraint changes 38 normalized assembly rows across 15 regions. The first addition uses r5 where the original uses r6. The compiler merges equivalent values before register allocation.

- **081F28C4 · SpriteTrail_CreateFixedEffect**
  Removing 1 local constraint changes 4 normalized assembly rows across 4 regions. The first changed instruction group contains store in the original and register transfer in the draft.

- **081F2A08 · SpriteTrail_CreateMotionEffect**
  Removing 2 local constraints changes 34 normalized assembly rows across 28 regions. The first changed instruction group contains store in the original and register transfer in the draft.

- **081F467C · Ratatosk_UpdateHomingShot**
  The final tick comparison and callback address use one lower reload register than the ROM. Splitting the finished branch and porting the sibling facing macro do not fix both reload choices. Scope measured 5 ROM / 5 draft.

- **081F6748 · SceneActors_FireTrackingProjectileSpread**
  With only the facing operand removed, the stack-vector address moves ahead of the wrapped high-byte angle calculation. Several equivalent facing computations and assignment orders leave that motion. Scope measured 6 ROM / 6 draft.

- **081F7DAC · Ratatosk_UpdateCloneAttack**
  The two random position draws share a pinned r4 value across their Mod calls. Simple unpinning changes allocation for the position offsets. Removing either barrier alone still changes instructions. Scope measured 23 ROM / 15 draft (+8 moved branches).

- **081F8988 · Ratatosk_UpdateSunOrbRelocation**
  Using the sequence field instead of the 0x180 pinned address changes reload choices in the sequence-two transition. An inline setter reduces the difference, but the following repeat reset still uses the wrong temporaries. Scope measured 20 ROM / 14 draft (+1 moved branch).

- **081FBF2C · sub_081FBF2C**
  Both ROM routines load particle flags again after updating velocity. An ordinary field read makes cse keep the first value through the stores, spills the loop index and changes the frame. Reading through the configuration or sprite subobject recovers the reload but leaves the wrong base register. The 081FBF2C draft includes the typed configuration flags.

- **081FD664 · OrbitEffect_AdjustHeading**
  Removing 2 local constraints changes 7 normalized assembly rows across 7 regions. The first load uses r1 where the original uses r2. Scope also identifies an instruction-order change at this site. Changing this decision in the diagnostic compiler reproduces the original bytes.

- **081FD7EC · OrbitEffect_FireVolley**
  Removing 2 local constraints changes 47 normalized assembly rows across 20 regions. The first addition uses r5 where the original uses r6. Scope also identifies an instruction-order change at this site.

- **081FDAF4 · RisingActor_AdvanceHeight**
  Removing 2 local constraints changes 4 normalized assembly rows across 3 regions. The first instruction uses lr where the original uses r4.

- **081FDCB0 · sub_081FDCB0**
  Removing 3 local constraints changes 38 normalized assembly rows across 13 regions. The first changed instruction group contains addition, register transfer, store in the original and addition, store, left shift in the draft. The compiler merges equivalent values before register allocation. Matchsearch tested 1 further source spelling.

- **081FDDC0 · OrbitEffect_SpawnOrbitTrail**
  Removing 11 local constraints changes 23 normalized assembly rows across 10 regions. The first changed instruction group contains load, addition in the original and load in the draft. The allocator assigns `mask` to r1 where the original uses r2. Matchsearch tested 3 further source spellings.

- **081FEE0C · SpriteParticle_UpdateManager**
  Removing 3 local constraints changes 86 normalized assembly rows across 40 regions. The draft introduces register transfer and addition work at the first differing site. Scope also identifies an instruction-order change at this site. Matchsearch tested 5 further source spellings.

- **081FF454 · HomingProjectile_UpdateDefaultState**
  Removing 4 local constraints changes 58 normalized assembly rows across 22 regions. The first changed instruction group contains register transfer, mask in the original and register transfer, mask in the draft. Matchsearch tested 1 further source spelling. The archived revision also removes the remaining constraints from the local helpers.

- **08201160 · sub_08201160**
  Removing 1 local constraint changes 1 normalized assembly row across 1 region. The first change drops or moves the register transfer present at that position in the original.

- **082012D4 · PlayerClone_BeginActiveReactions**
  Removing 1 local constraint changes 11 normalized assembly rows across 6 regions. The first change drops or moves the register transfer present at that position in the original. Matchsearch tested 3 further source spellings.

- **082015D4 · ScriptCombatActor_ApplyHitReaction**
  Removing 4 local constraints changes 79 normalized assembly rows across 22 regions. The operations at the first differing site are unchanged, but their operands or branch targets differ. Matchsearch tested 2 further source spellings.

- **08202730 · ProjectilePool_Update**
  Removing 10 local constraints changes 44 normalized assembly rows across 18 regions. The operations at the first differing site are unchanged, but their operands or branch targets differ. Matchsearch tested 2 further source spellings.

- **08206C00 · VulcanCannon_UpdateTargeting**
  Target angle parameters, the player pointer and the cached target snapshot interact. Eliminating the snapshot operand alone changes memory ownership across the turn animation calls. Independent pin/pointer changes do not produce an exact partial. Scope measured 126 ROM / 115 draft (+13 moved branches).

- **082074F0 · VulcanCannon_UpdateDamageState**
  Removing 8 local constraints changes 38 normalized assembly rows across 20 regions. The draft introduces register transfer work at the first differing site. The reload pass assigns r4 at this site. Matchsearch tested 2 further source spellings.

- **08208498 · CannonTurret_FireAimedVolley**
  The projectile spread needs a particular captured table address across the jitter and speed sample. The two shared RNG helper forms change the local allocation even when they supply the same values. Scope measured 9 ROM / 9 draft.

- **0820EE34 · OrangeSpine_ApplyHitReaction**
  The damage handler’s empty outputs are consumed only by empty operands. Removing those together is semantically straightforward, but changes the reload choices at two field accesses. Removing the other pins also changes the HUD setup. Scope measured 59 ROM / 59 draft.

- **0821025C · ProjectileEmitter_EmitAttackFrame**
  Removing 1 local constraint changes 5 normalized assembly rows across 5 regions. The first register transfer uses r9 where the original uses ip. The reload pass assigns a different temporary at this site. Changing this decision in the diagnostic compiler reproduces the original bytes.

- **08210398 · PinkSpine_UpdateDamageAnimation**
  Removing 1 local constraint changes 6 normalized assembly rows across 3 regions. The first addition uses r0 where the original uses r2. Matchsearch tested 1 further source spelling.

- **082124D8 · ForgingBook_InitSprites**
  Removing 1 local constraint changes 14 normalized assembly rows across 10 regions. The first addition uses r5 where the original uses r4. Scope also identifies an instruction-order change at this site. Changing this decision in the diagnostic compiler reproduces the original bytes.

- **08213FD8 · sub_08213FD8**
  Removing 3 local constraints changes 23 normalized assembly rows across 7 regions. The first load uses r0 where the original uses r2. The allocator assigns `affine` to r0 where the original uses r2.

- **0821537C · sub_0821537C**
  Using the SDK DMA copy and counting from the saved queue length improve the loop, but the lifetime of the DMA mode and queue globals still changes the literal loads and saved registers. That experiment differs by 11 scope rows.

- **08215B18 · Bg_SetAffineTransform**
  Using C division and changing the initial-scale helper’s denominator type leaves agbcc converting the signed halfword before the dividend setup. The other three reciprocal calls also rotate their reloads. That experiment differs by 26 scope rows.

- **082160B8 · Bg_SetupLayer**
  Switch alternatives and typed display-control stores still allocate the shadow pointer and current control bits differently at the shared store after the four layer cases. That experiment differs by 12 scope rows.

- **08217430 · sub_08217430**
  Reversing palette address operands and using a typed bank array or transfer helper change the two source registers for CpuFastSet. The ROM keeps the computed bank in r3 while the function prepares both call arguments. That experiment differs by 8 scope rows.

- **08218254 · SpriteOam_QueueCameraRelativeSprites**
  The accepted rewrite removes the flags, initial-flags and depth bindings and the mask operand. Removing the two remaining pins changes the affine matrix counter post-increment and the attribute register across the loop. That experiment differs by 62 scope rows.

- **0821BF5C · sub_0821BF5C**
  The cell-index difference crosses the terrain branches. Separate differences inside each shape branch move the register rotation into the cell-override lookup. The clearest pin-free spelling leaves the two calls using the wrong spill register. That experiment differs by 25 scope rows.

- **0821D4F0 · sub_0821D4F0**
  The initial cell and loop index contend for r6. Initializing the cell from the copied and original position, counting down, and narrowing the status lifetime were tried. Giving the cell higher allocation priority reduces the error to four instruction rows. That experiment differs by 18 scope rows.

- **0821E5C4 · Route_Start**
  Typed route-header indexing and using the stored point byte improve the route cursor setup. The held points offset and base still contend for r0/r1, and the compiler changes the byte clamp when the casts are combined. That experiment differs by 25 scope rows.

- **0821E8C8 · SceneActorRoute_IsOutsidePointBounds**
  Plain signed coordinates, direct point access and an absolute-distance helper change the loads and allocation of the two-axis point. The aggregate spelling is closer but still differs at the sign-extension and first-axis subtraction. That experiment differs by 6 scope rows.

- **0821F15C · sub_0821F15C**
  After ScriptProc_Start, the ROM copies the resource out of r0 before loading gTerrainMap. A field store, setter and reordered helper arguments keep the result in r0 and instead fetch the map through r1. That experiment differs by 5 scope rows.

- **0821FB8C · sub_0821FB8C**
  Removing the empty block merges the return tails for the 4-way steering correction. Independent flag setters and an early return separate more of the paths, but the correction angle and its delta still exchange r4/r5. That experiment differs by 14 scope rows.

- **0821FC34 · sub_0821FC34**
  The 8-way steering twin has the same tail-merge issue as 0821FB8C with a different angular mask. Separate flag writes and the early movement return still leave distinct angle and mask allocation. That experiment differs by 15 scope rows.

- **082207DC · sub_082207DC**
  Without the debug block, agbcc reuses the stack-box address when it inlines the three axis checks. Capturing the box pointer before the builder call, switching to an array and splitting the overlap test still rotate the spill registers. That experiment differs by 17 scope rows.

- **082213B8 · Anim_DrawDecimalDigits**
  Removing the digit-loop index pin changes allocation across all eight digit extractions. Retaining that pin and computing suppression in ordinary C remains two instructions apart at the stack load and OR for the suppression bit. That experiment differs by 63 scope rows.

- **08227FEC · sub_08227FEC**
  Naming the guard value and checking it with the reset pointer live gives the correct registers, but agbcc moves the magic literal load ahead of the load from the guard. The ordinary direct test instead interchanges r1 and r2. That experiment differs by 4 scope rows.

- **082285C4 · Clock_CalculateDateDifference**
  The /400 term in each inlined year calculation loads the year before constructing the 400 literal. The ROM reverses the two. Reordered division helpers, a separate divisor and the arithmetic return spellings left both sites different. That experiment differs by 4 scope rows.

- **08229440 · Clock_CheckRtcStatus**
  The ROM retains both the shifted RTC return in r5 and the extracted byte in r4 across Clock_EndRtcAccess. With the correct u8 return prototype and ordinary status temporaries, allocation coalesces the values. That experiment differs by 9 scope rows.

- **0822D604 · Selection_GetDisplayFrame**
  The first two frame ranges each end in their own addition in the ROM. Independent returns, changed range order and local frame bases still allow jump2 to share the addition. That experiment differs by 6 scope rows.

- **08230AC8 · CgbSound**
  In the PWM frequency adjustment, the ROM reads SOUNDBIAS_H into r0 and copies it to r1 for both thresholds. Narrow scalar types, a rate reader and an adjustment helper either coalesce that copy or read directly into r1. That experiment differs by four scope rows.

- **082317A0 · EffectPart_UpdateLoopAnimation**
  Removing 4 local constraints changes 7 normalized assembly rows across 6 regions. The first load uses r1 where the original uses r0. The allocator assigns `input` to r1 where the original uses r0. Matchsearch tested 3 further source spellings.

- **082318E0 · EffectPart_UpdateFinish**
  Removing 3 local constraints changes 5 normalized assembly rows across 4 regions. The first load uses r1 where the original uses r0. The allocator assigns `input` to r1 where the original uses r0. Matchsearch tested 3 further source spellings.

- **08231A0C · EffectPart_UpdateAnimation**
  Removing 4 local constraints changes 7 normalized assembly rows across 6 regions. The first load uses r1 where the original uses r0. The allocator assigns `input` to r1 where the original uses r0. Matchsearch tested 3 further source spellings.

- **08231B3C · EffectPart_UpdateBlink**
  Removing 4 local constraints changes 7 normalized assembly rows across 6 regions. The first load uses r1 where the original uses r0. The allocator assigns `input` to r1 where the original uses r0. Matchsearch tested 3 further source spellings.

- **08231C84 · EffectPart_UpdateAnimation2**
  Removing 4 local constraints changes 7 normalized assembly rows across 6 regions. The first load uses r1 where the original uses r0. The allocator assigns `input` to r1 where the original uses r0. Matchsearch tested 3 further source spellings.

- **08231DB4 · EffectPart_UpdateBlink2**
  Removing 4 local constraints changes 7 normalized assembly rows across 6 regions. The first load uses r1 where the original uses r0. The allocator assigns `input` to r1 where the original uses r0. Matchsearch tested 3 further source spellings.

- **08233140 · TransientSprite_Spawn**
  Removing 2 local constraints changes 3 normalized assembly rows across 1 region. The first register transfer uses r0 where the original uses r1. The allocator assigns the live value to r0 where the original uses r1.

- **08233760 · ChargedBurst_EmitTerminalDebris**
  Deleting only the low-mask operand removes the copy from r9 immediately before the first and. Mask-first expressions, direct table reads and the shared sample macro do not reproduce it. Scope measured 54 ROM / 50 draft (+3 moved branches).

- **082343B8 · SpiralBurst_UpdateParticles**
  Removing 3 local constraints changes 81 normalized assembly rows across 40 regions. The first changed instruction group contains addition in the original and addition in the draft. Scope also identifies an instruction-order change at this site. Matchsearch tested 1 further source spelling.

- **08234540 · SpiralBurst_InitParticles**
  The spiral emitter uses a separate mask and two table aliases across its loop. Capturing a per-iteration RandomTableView or dropping one of the table pins alone still changes the three-way allocation. Scope measured 67 ROM / 63 draft (+2 moved branches).

- **08234DC8 · sub_08234DC8**
  Expanding the timer-capture macro into ordinary assignments changes two assembly regions. The incremented word and its stored halfword no longer have the original lifetimes.

- **082356E4 · sub_082356E4**
  Removing 1 local constraint changes 8 normalized assembly rows across 3 regions. The first addition uses r1 where the original uses r2.

- **08235B34 · sub_08235B34**
  Removing 1 local constraint changes 10 normalized assembly rows across 9 regions. The first addition uses r5 where the original uses r4. Scope also identifies an instruction-order change at this site. Changing this decision in the diagnostic compiler reproduces the original bytes.

- **082368DC · OrbitEffect_Initialize**
  Removing 4 local constraints changes 16 normalized assembly rows across 9 regions. The first register transfer uses r9 where the original uses r8. The compiler merges equivalent values before register allocation.

- **08236F48 · sub_08236F48**
  Removing 2 local constraints changes 4 normalized assembly rows across 1 region. The first changed instruction group contains load, addition, comparison in the original and load, comparison in the draft. The allocator assigns `limit` to r2 where the original uses r0.

- **08236FF4 · AttachmentSprite_Setup**
  Removing 3 local constraints changes 41 normalized assembly rows across 11 regions. The first changed instruction group contains addition in the original and addition in the draft.

- **082377B8 · sub_082377B8**
  Removing 1 local constraint changes 4 normalized assembly rows across 1 region. The first mask uses r0 where the original uses r1. The compiler merges equivalent values before register allocation.

- **08237A28 · sub_08237A28**
  The ROM reloads radius in r2 and keeps the glyph angle in r4 while writing packed positions. The normal field load and direct/sampled RNG variants trade the angle and radius allocations. Scope measured 23 ROM / 23 draft.

- **08238110 · sub_08238110**
  Removing 1 local constraint changes 6 normalized assembly rows across 1 region. The first addition uses r1 where the original uses r0.

- **08238520 · ContactCollider_GetImpactDirection**
  Removing 1 local constraint changes 8 normalized assembly rows across 5 regions. The first change drops or moves the addition and load present at that position in the original.

- **08238580 · sub_08238580**
  Removing 1 local constraint changes 15 normalized assembly rows across 13 regions. The first addition uses r6 where the original uses r5. Scope also identifies an instruction-order change at this site. Changing this decision in the diagnostic compiler reproduces the original bytes. Matchsearch tested 1 further source spelling.

- **0823A3CC · sub_0823A3CC**
  Removing 1 local constraint changes 2 normalized assembly rows across 2 regions. The draft introduces register transfer work at the first differing site. The allocator assigns `zero` to r2 where the original uses r4.

- **0823C280 · sub_0823C280**
  Removing 3 local constraints changes 30 normalized assembly rows across 9 regions. The first changed instruction group contains register transfer, instruction in the original and register transfer, instruction in the draft.

- **0823C930 · Player_UpdateTerrainSidestep**
  The early 08142194 sibling computes a shifted probe position with two scaled sine samples, then tests the terrain. Reconstructing that probe makes the compiler discard part of the work here, where its result does not reach the particle call. 18 ROM / 14 draft.

- **0823DAB4 · Player_UpdateSweepAction**
  The two branch calculations combine differently. The ROM adds 0x100 to the angular product before adding the heading and subtracting 0x20. Common-tail, signed/narrow and mask-first forms tend to fold the bias to 0xe0. 5 ROM / 3 draft (+1 moved branch).

- **0823E704 · sub_0823E704**
  Removing 1 local constraint changes 12 normalized assembly rows across 6 regions. The draft introduces register transfer and instruction work at the first differing site.

- **082424D0 · sub_082424D0**
  Removing 2 local constraints changes 33 normalized assembly rows across 9 regions. The first load uses r1 where the original uses r0. The reload pass assigns r1 at this site. Matchsearch tested 2 further source spellings.

- **08243428 · Player_ApplyTerrainDamage**
  Removing 1 local constraint changes 74 normalized assembly rows across 47 regions. The first changed instruction group contains register transfer in the original and addition, register transfer, call in the draft.
