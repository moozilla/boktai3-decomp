# Round 11 package D holdouts

Batch 21 leaves 95 of the original 466 package D functions with scored concessions. Since Batch 12, 147 functions covering 58,868 bytes have become concession-free. The build still reproduces the original ROM.

The list below contains only functions still open at this checkpoint, in ROM order. The Batch 12 source archive supplies a starting C draft for each. Its instruction and byte differences describe that archived draft. Subsequent compiler or source findings are recorded separately below the affected entry.

- **0810E9D4 · TimerDisplay_DrawRecord**
  The two returned tile pointers coalesce with r0.
  Write both record delimiters through a tile-row cursor that advances after each store. The retained draft differs in 6 aligned assembly rows.

- **0810FDDC · sub_0810FDDC**
  Indexed widget access improves on keeping separate doubled and base indices.
  Carry a pointer to the row for the final, unconditionally visible digit. The retained draft differs in 22 aligned assembly rows.

- **081169A8 · Glove_InitializeStyleModifiers**
  Removing its sole pin swaps the participant and link pointer registers.
  Index the typed payload array after validating the link first. The retained draft differs in 13 aligned assembly rows.

- **0811AE9C · Terrain_ResolveActorMotion**
  The pin-free query/mask draft differs in 42 scope rows.
  Compute vertical displacement by scaling the reverse height difference using unsigned fixed-point multiplication. The detached difference fell from 40 to 32 aligned assembly rows.

- **0811B164 · SpatialIndex_SetPosition**
  The registration byte can be named.
  Read signed bucket into a value argument for the range/unchanged-bucket test, then call removal separately. The retained draft differs in 6 aligned assembly rows.

- **081301C4 · SceneActorReaction_UpdateRouteRecovery**
  After depinning, the signed height copies from ROM r1 to r0 and the stack-address scratch changes from r0 to r1.
  Assign the route height through a position-to-position helper while preserving route X/Z. The retained draft differs in 3 aligned assembly rows.

- **08135F70 · SceneActorReaction_ResolveDamage**
  Forming the updated hit value at its halfword store improved the reaction draft substantially. The retained ordinary source also removes both local damage macros.
  Return the hit flag address together with the updated halfword, then read the reaction-mode bit via its field. The detached difference fell from 114 to 14 aligned assembly rows.

- **0813727C · sub_0813727C**
  The accepted-motion branch now forms the actor field at the store. The remaining difference is in the reset sequence.
  Read the clear and ResetControl arguments from the actual flag field in the accepted-motion arm. The detached difference fell from 26 to 3 aligned assembly rows.

- **081381CC · LinkSession_UpdatePlayerValueAverage**
  A typed selection-bits accessor restores the session pointer to r1.
  Compute the selection byte index in a halfword before taking the pointer to the two four-participant pages. The detached difference fell from 3 to 2 aligned assembly rows.

- **081384A0 · sub_081384A0**
  The pin-free selection sort changes the inner-loop induction values.
  Use paired score/index cursors that descend together over the unsorted prefix. The retained draft differs in 27 aligned assembly rows.

- **08138778 · PositionEffectDisplay_UpdateSources**
  An indexed loop using PlayerCue_StepSequenceEnded reproduces the playback body.
  Advance the slot pointer once per iteration for assignment while retaining the index for sequence and stop flags. The retained draft differs in 16 aligned assembly rows.

- **0813B4C8 · Player_RecalculateDerivedStats**
  The unpinned contiguous stat loops assign different registers to the quotient, cap and destination.
  Drive each three-stat update by reaching the end of the output array to avoid reversal of a fixed-count loop. The detached difference fell from 26 to 21 aligned assembly rows.

- **0813D834 · sub_0813D834**
  The two pinned values connect the dispatch kind to the scaled status-timer offset across branches.
  Use a pointer into scriptStatus as the switch result and write through it after both dispatch arms. The retained draft differs in 23 aligned assembly rows.

- **0813EDDC · PlayerAction_RefreshFlagState**
  Removing 7 local constraints changes 8 normalized assembly rows across 4 regions.
  Separate the optional override from ordinary slot resolution and assign the validated current item directly. The retained draft differs in 8 aligned assembly rows.

- **08146EAC · Player_UpdateAlternateFormCue**
  Removing 1 local constraint changes 11 normalized assembly rows across 5 regions.
  Use the typed game-state position fields for clamp and the sprite X/Z, letting ordinary reads share their base. The retained draft differs in 11 aligned assembly rows.

- **0814D9C4 · sub_0814D9C4**
  The combo index is scaled into an offset and then converted to a phase-table pointer in two action arms.
  Index the weapon phase table using comboIndex as an array subscript before both animation callbacks. The retained draft differs in 93 aligned assembly rows.

- **0814F328 · Player_UpdateAttachedAttackCharge**
  Using packed three-component vectors and the typed player frame removes the burst and latch constraints.
  Model the held key pointer as a player-relative input view, then test the bit through its typed field. The retained draft differs in 32 aligned assembly rows.

- **08156F54 · Player_TestTetherActionInput**
  The trigger path loads the global tether pointer a second time in the ROM.
  Classify state 1 and state 0x11 by masking their differing bit before reading the tether status fields. The retained draft differs in 5 aligned assembly rows.

- **08159164 · sub_08159164**
  Removing 1 local constraint changes 21 normalized assembly rows across 6 regions.
  Acquire the first player once inside the playable-state guard, reusing that pointer for both bonus categories. The retained draft differs in 21 aligned assembly rows.

- **0815A048 · sub_0815A048**
  Unpinning the previous flags, bit 4 and the halfword zero merges the zero sources for the byte and halfword resets.
  Use a dedicated halfword zero and byte zero for the reset stores rather than two unsigned words. The retained draft differs in 52 aligned assembly rows.

- **0815E034 · sub_0815E034**
  The contact collider, damage, flags and common mask compete across the cost branches.
  Use the masked attack category directly for the damage dispatch instead of keeping the common 63 in a local. The retained draft differs in 62 aligned assembly rows.

- **0815E1F0 · PlayerContact_ApplyResponse**
  Removing 2 local constraints changes 14 normalized assembly rows across 6 regions.
  Carry the source power into the timed-reset call as its incremented duration, with no unused flag read. The retained draft differs in 14 aligned assembly rows.

- **0815F5B4 · TargetRegistry_FindById**
  The requested ID and the scene-actor mask exchange r3/r4.
  Compare by subtracting the requested identifier after selecting the appropriate stored ID width. The retained draft differs in 5 aligned assembly rows.

- **081606C0 · sub_081606C0**
  The two sets of flag-accumulation tests require different mask and first-bit registers.
  Walk the 16 action records by their terminal address and compute the returned slot index from the cursor. The retained draft differs in 18 aligned assembly rows.

- **081608B0 · PlayerAttackEffect_EmitPaletteBaseTrail**
  The trail angle in r9 survives the particle-spawn setup.
  Carry the shared sine-table pointer for the angle across both endpoint builds while calculating each pitch-dependent distance separately. The detached difference fell from 89 to 85 aligned assembly rows.

- **08162BE4 · PlayerBarrier_TryPlace**
  Removing the placement pin changes the live registers in the barrier-placement path.
  Compute both placement coordinates as a grid index and a half-cell center. The retained draft differs in 13 aligned assembly rows.

- **081639E4 · sub_081639E4**
  Both index/address pins feed table access.
  Derive both jitter samples from the starting random cursor in one 1024-entry table. The retained draft differs in 41 aligned assembly rows.

- **08164480 · sub_08164480**
  The pointer read now matches without volatile qualification.
  Load the HUD owner through its named view member at all three timer tests with a plain pointer. The retained draft differs in 22 aligned assembly rows.

- **08164DC0 · Hud_UpdateTransientStatGauge**
  Two fill loops load a mask directly into the register where the ROM first copies it.
  Write the gauge decoration tiles and blank row through a descending halfword cursor with an end pointer. The retained draft differs in 7 aligned assembly rows.

- **08168A0C · SceneHud_RebuildPrimaryOptions**
  The pinned state pointer feeds the initial selected-slot and list accesses.
  Share the selected-slot pointer with the options cursor in the normal branch without caching the game-state base. The retained draft differs in 9 aligned assembly rows.

- **0816BC54 · sub_0816BC54**
  The five register constraints in this multi-branch map update are coupled.
  Load the active page before comparing the first neighbor and carry its saved index through that assignment. The retained draft differs in 377 aligned assembly rows.

- **0816D4E4 · SceneHud_CanApplySelectedAction**
  The inventory-present path loads the signed slot again for Inventory_GetEquipmentEntry.
  Test the equipped halfword sign using its unsigned high bit while passing a signed typed read to the item lookup. The retained draft differs in 2 aligned assembly rows.

- **0816E570 · sub_0816E570**
  The random table base must survive the Mod calls for the velocity samples.
  Represent the random source as its 1024-entry table for both coordinates, speed, and flip. The retained draft differs in 24 aligned assembly rows.

- **0816E6F0 · sub_0816E6F0**
  Removing both counter/phase mask barriers and their pins changes the halfword mask/activation stores.
  Advance the actual halfword phase counter and detect every sixteenth tick from its low nibble. The retained draft differs in 9 aligned assembly rows.

- **08170D78 · sub_08170D78**
  The ROM reloads a signed item slot after testing it, even though no store or call intervenes.
  Check the signed equipment-slot id with the unsigned high bit, then pass its signed form to inventory resolution. The detached difference fell from 26 to 24 aligned assembly rows.

- **0817AD7C · BossSlot2_SpawnBobbingFormation**
  The address of the four-halfword position array moves to just before the loop.
  Use a named position aggregate with contiguous coordinate storage and move per-formation mask initialization relative to its owner. The retained draft differs in 2 aligned assembly rows.

- **0817AEC8 · sub_0817AEC8**
  Removing the table operand changes which address stays live in the six-iteration loop.
  Factor sine projection through an ordinary polar-offset helper, narrow circular heading to a byte, and address the RNG through its global ring. The detached difference fell from 52 to 48 aligned assembly rows.

- **0817B0B4 · BossSlot2_UpdateSequencePalette**
  Removing 4 local constraints changes 25 normalized assembly rows across 10 regions.
  Represent the common palette banks as indexed banks, reverse source-pair initialization and accumulate the palette base across the mode dispatch. The detached difference fell from 25 to 14 aligned assembly rows.

- **0817BCD4 · ActorDecision_UpdatePulsePhase**
  The actor pointer occupies r4 and its copy occupies r6.
  Restructure the transition query into a separate phase predicate or early exit, then inline or retain reset consumption. The retained draft differs in 2 aligned assembly rows.
  The adjacent 0817B8BC owner and actor views reproduce all but the pointer passed to two movement queries. An explicit reset-branch actor reference leaves only the first query using r4 where the ROM uses r6.

- **0818E9B8 · SpriteColliderGroup_ActivateParts**
  Removing 2 local constraints changes 10 normalized assembly rows across 5 regions.
  Reconstruct typed part stride, activation count and slot index through the live part-group fields. The retained draft differs in 10 aligned assembly rows.

- **0818EA3C · SpriteColliderGroup_ResetActiveParts**
  The very first count load and comparison use r6 where the ROM uses r5.
  Test the same group iterator used by activation with an addressable count or direct field and a typed indexed first element. The retained draft differs in 2 aligned assembly rows.

- **0819BB18 · sub_0819BB18**
  The pinned setup copies the bank address through three tied operands.
  Separate field-address and payload lifetimes through typed owner accessors (closest v_x_2). The retained draft differs in 38 aligned assembly rows.

- **0819BCB8 · Hresvelgr_EmitFacingMotion**
  The relative sample helper recovers the real use of the apparent 0x0403B714 dead literal.
  Separate field-address and payload lifetimes through typed owner accessors (closest out_x_1). The retained draft differs in 53 aligned assembly rows.

- **0819F788 · Hresvelgr_UpdateAlternatingVolley**
  The alternating volley computes three wrapped indices across a call.
  Separate field-address and payload lifetimes through typed owner accessors (closest out_x_1). The retained draft differs in 60 aligned assembly rows.

- **0819FC34 · Hresvelgr_UpdateTurningCharge**
  The mask-first direction setters are clean in the landed function.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_flags_0). The retained draft differs in 16 aligned assembly rows.

- **081A09C4 · Hresvelgr_PlayDescent**
  The typed actor pointer merges back into the input without the operand.
  Separate field-address and payload lifetimes through typed owner accessors (closest actor_flags_1). The retained draft differs in 10 aligned assembly rows.

- **081A0BC8 · Hresvelgr_UpdateBoundaryBarrage**
  The phase index is no longer kept in r5 after the pins are removed.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_x_1). The retained draft differs in 29 aligned assembly rows.

- **081A12DC · Hresvelgr_UpdateRisingProjectile**
  The quarter-direction pointer returned by SetHeading loses r5, which changes several allocations across the projectile construction calls.
  Separate field-address and payload lifetimes through typed owner accessors (closest out_x_2). The detached difference fell from 49 to 44 aligned assembly rows.

- **081A14FC · sub_081A14FC**
  With the other constraints retained, deleting just the Y sample operand changes one instruction from asr to lsr.
  Separate field-address and payload lifetimes through typed owner accessors (closest actor_action_1). The detached difference fell from 90 to 58 aligned assembly rows.

- **081A8E08 · sub_081A8E08**
  The capture-setup helper returns its Y coordinate for later use as an animation parameter.
  Separate field-address and payload lifetimes through typed owner accessors (closest out_x_1). The retained draft differs in 12 aligned assembly rows.

- **081A93A0 · sub_081A93A0**
  The attack controller has both a switch-state allocation and helper-level constraints on vector operations.
  Separate field-address and payload lifetimes through typed owner accessors (closest a_y_1). The detached difference fell from 102 to 71 aligned assembly rows.

- **081AA330 · sub_081AA330**
  The sole remaining mismatch is r1 instead of r2 for the 0x56c offset of the selected-halves object.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_flags_1). The retained draft differs in 2 aligned assembly rows.

- **081AA650 · Vanargand_UpdateTimedAnimationAttack**
  Phase dispatch and the flags pointer compete for r2/r6.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_callback_0). The retained draft differs in 18 aligned assembly rows.

- **081ACC24 · Vanargand_ApplyPendingDamage**
  The stage colors are clean through the shared palette helper.
  Separate field-address and payload lifetimes through typed owner accessors (closest self_accumulatedDamage_0). The detached difference fell from 210 to 201 aligned assembly rows.

- **081AFD10 · PairedSceneActor_UpdateHalfPresentation**
  Removing 28 local constraints changes 63 normalized assembly rows across 24 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest source_y_0). The detached difference fell from 63 to 59 aligned assembly rows.

- **081B08B8 · ActorStatus_UpdatePlayerApproach**
  Removing 5 local constraints changes 8 normalized assembly rows across 6 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest base_sequence_1). The retained draft differs in 8 aligned assembly rows.

- **081B66E0 · VanargandEffect_Initialize**
  Removing 1 local constraint changes 2 normalized assembly rows across 1 region.
  Separate field-address and payload lifetimes through typed owner accessors (closest effect_animation_1). The retained draft differs in 2 aligned assembly rows.

- **081B6E90 · ActorEffect_UpdateRadialExpiry**
  The six-iteration trail loop assigns the index, table, RNG cursor and 1023 mask to four different registers.
  Separate field-address and payload lifetimes through typed owner accessors (closest effect_entered_0). The retained draft differs in 16 aligned assembly rows.

- **081BB904 · sub_081BB904**
  Removing 5 local constraints changes 16 normalized assembly rows across 8 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest panel_unk1D8_1). The retained draft differs in 16 aligned assembly rows.

- **081BD38C · DarkLoan_BuildDisplay**
  Removing 1 local constraint changes 77 normalized assembly rows across 21 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_background_1). The retained draft differs in 77 aligned assembly rows.

- **081BDE94 · Storage_BeginSwapTransition**
  Removing 7 local constraints changes 47 normalized assembly rows across 9 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest widget_flags_1). The retained draft differs in 47 aligned assembly rows.

- **081C62B4 · sub_081C62B4**
  Removing 1 local constraint changes 6 normalized assembly rows across 5 regions.
  Walk distinct digit and coordinate arrays with their native strides. The retained draft differs in 6 aligned assembly rows.

- **081D0864 · sub_081D0864**
  Removing 2 local constraints changes 17 normalized assembly rows across 9 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest npc_sub20_1). The retained draft differs in 17 aligned assembly rows.

- **081D11E4 · SceneNpc_SpawnTransitionParticle**
  The ROM pins the random table in r6 across calls to Mod.
  Separate field-address and payload lifetimes through typed owner accessors (closest effect_particles_1). The retained draft differs in 28 aligned assembly rows.

- **081D3620 · sub_081D3620**
  The cloud loop’s table, coordinate scratch, and random value have different allocation without constraints.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_particles_1). The retained draft differs in 27 aligned assembly rows.

- **081D905C · TimedEffect_UpdateHoming**
  Removing 1 local constraint changes 105 normalized assembly rows across 45 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest a_x_0). The retained draft differs in 105 aligned assembly rows.

- **081D9D3C · Projectile_SpawnImpactPair**
  Expanding the three RNG setup assignments removes the private macro but swaps the parameter-buffer and table registers inside the impact loop.
  Separate field-address and payload lifetimes through typed owner accessors (closest b_y_0). The detached difference fell from 51 to 35 aligned assembly rows.

- **081DE0CC · NameEntry_Initialize**
  Removing 2 local constraints changes 88 normalized assembly rows across 29 regions.
  Represent the menu sprite and its relocated resource as an array of complete widget records. The retained draft differs in 88 aligned assembly rows.

- **081DEE38 · Time_InitializeDateMenu**
  Removing 3 local constraints changes 148 normalized assembly rows across 24 regions.
  Represent the menu sprite and its relocated resource as an array of complete widget records. The detached difference fell from 148 to 68 aligned assembly rows.

- **081E09A8 · sub_081E09A8**
  Removing 1 local constraint changes 4 normalized assembly rows across 2 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest s_timer_0). The retained draft differs in 4 aligned assembly rows.

- **081E0DD8 · GameStart_DrawSaveInfo**
  Removing 1 local constraint changes 74 normalized assembly rows across 15 regions.
  Move the saved-region dispatch around its sequence-selection consumer. The retained draft differs in 74 aligned assembly rows.

- **081E2910 · Presentation_UpdateDropShakeBlink**
  Removing 4 local constraints changes 76 normalized assembly rows across 38 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_state_1). The detached difference fell from 74 to 73 aligned assembly rows.

- **081EC388 · ActorEffect_SpawnRandomizedBurst**
  Removing 1 local constraint changes 21 normalized assembly rows across 12 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest actor_height_0). The retained draft differs in 21 aligned assembly rows.

- **081EC4AC · ActorEffect_SpawnBurstAtAnchor**
  Removing 1 local constraint changes 21 normalized assembly rows across 12 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest actor_anchor_0). The retained draft differs in 21 aligned assembly rows.

- **081EC7D8 · ActorDisplay_UpdateForegroundSprite**
  Removing 2 local constraints changes 74 normalized assembly rows across 26 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest owner_sequence_1). The retained draft differs in 74 aligned assembly rows.

- **081EF914 · SingletonSlot7_UpdateEndingScene**
  Removing 7 local constraints changes 122 normalized assembly rows across 86 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest owner_sprite_1). The retained draft differs in 122 aligned assembly rows.

- **081F28C4 · SpriteTrail_CreateFixedEffect**
  Removing 1 local constraint changes 4 normalized assembly rows across 4 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_phase_2). The retained draft differs in 4 aligned assembly rows.

- **081F2A08 · SpriteTrail_CreateMotionEffect**
  Removing 2 local constraints changes 34 normalized assembly rows across 28 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_phase_2). The retained draft differs in 34 aligned assembly rows.

- **081F467C · Ratatosk_UpdateHomingShot**
  The final tick comparison and callback address use one lower reload register than the ROM.
  Separate field-address and payload lifetimes through typed owner accessors (closest a_y_0). The detached difference fell from 5 to 2 aligned assembly rows.

- **081F8988 · Ratatosk_UpdateSunOrbRelocation**
  Using the sequence field instead of the 0x180 pinned address changes reload choices in the sequence-two transition.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_active_0). The retained draft differs in 8 aligned assembly rows.

- **081FDAF4 · RisingActor_AdvanceHeight**
  Removing 2 local constraints changes 4 normalized assembly rows across 3 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest state_fieldF4_2). The retained draft differs in 4 aligned assembly rows.

- **081FDCB0 · sub_081FDCB0**
  Removing 3 local constraints changes 38 normalized assembly rows across 13 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_x_2). The retained draft differs in 38 aligned assembly rows.

- **081FF454 · HomingProjectile_UpdateDefaultState**
  Removing 4 local constraints changes 58 normalized assembly rows across 22 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_lifetime_0). The detached difference fell from 40 to 38 aligned assembly rows.

- **08202730 · ProjectilePool_Update**
  Removing 10 local constraints changes 44 normalized assembly rows across 18 regions.
  Tested the shared player-cue stepper with the sprite and sequence stored in separate projectile fields. The retained draft differs in 44 aligned assembly rows.
  The uninitialized terrain scratch and two empty assembly operands were removed. The command and three operand constraints still affect register allocation.

- **0820EE34 · OrangeSpine_ApplyHitReaction**
  The damage handler’s empty outputs are consumed only by empty operands.
  Tested direct reaction and HUD member stores, a separately evaluated spark guard, and named RNG storage through the Orange Spine damage and spark paths. The retained draft differs in 74 aligned assembly rows.

- **0821025C · ProjectileEmitter_EmitAttackFrame**
  Removing 1 local constraint changes 5 normalized assembly rows across 5 regions.
  Tested signed and narrow frame reads and two alternative shared selector calls for the emitter. The retained draft differs in 5 aligned assembly rows.

- **082124D8 · ForgingBook_InitSprites**
  Removing 1 local constraint changes 14 normalized assembly rows across 10 regions.
  Reconstructed the book effect using a single stack position and direct x/y assignments instead of a retained pointer to the stack record. The retained draft differs in 14 aligned assembly rows.

- **0821537C · sub_0821537C**
  Using the SDK DMA copy and counting from the saved queue length improve the loop, but the lifetime of the DMA mode and queue globals still changes the literal loads and saved registers.
  Compared a counted slot loop against a fixed-width queue pointer with typed DMA3 channel writes. The retained draft differs in 10 aligned assembly rows.

- **08215B18 · Bg_SetAffineTransform**
  Using C division and changing the initial-scale helper’s denominator type leaves agbcc converting the signed halfword before the dividend setup.
  Split sine-table evaluation and the fixed-point reciprocal at the first and remaining matrix coefficients. The retained draft differs in 25 aligned assembly rows.

- **08217430 · sub_08217430**
  Reversing palette address operands and using a typed bank array or transfer helper change the two source registers for CpuFastSet.
  Compared destination bank arrays, source palette-key indexing, and preparing the bank pointers in either order before CpuFastSet. The retained draft differs in 7 aligned assembly rows.

- **0821E5C4 · Route_Start**
  Typed route-header indexing and using the stored point byte improve the route cursor setup.
  Indexed route headers as typed records and points as eight-byte SceneActorRoutePoint entries. The detached difference fell from 19 to 10 aligned assembly rows.

- **08229440 · Clock_CheckRtcStatus**
  The SDK returns u8. A two-word scratch object reproduced the bytes, but no caller or driver data supports the extra word. The archived version uses the SDK prototype and direct status decoding.
  Compared direct status checks, SDK-style packed status and error nibble records, and independent decoding around Clock_EndRtcAccess. The retained draft differs in 5 aligned assembly rows.

- **08233140 · TransientSprite_Spawn**
  Removing 2 local constraints changes 3 normalized assembly rows across 1 region.
  Tested a typed slot header, a setter for the narrow ID, and an activation helper. The detached difference fell from 3 to 2 aligned assembly rows.

- **082343B8 · SpiralBurst_UpdateParticles**
  Removing 3 local constraints changes 81 normalized assembly rows across 40 regions.
  Tested fresh radius and angle reads at each spiral coordinate update and separated the timer comparison from the zero-age case. The detached difference fell from 81 to 20 aligned assembly rows.

- **08234540 · SpiralBurst_InitParticles**
  The spiral emitter uses a separate mask and two table aliases across its loop.
  Compared captured and direct random-table pointers across the spiral loop, with narrow angle state and alternative slot cursor advancement. The detached difference fell from 81 to 59 aligned assembly rows.
