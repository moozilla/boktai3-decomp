# Round 11 package D holdouts

Batch 15 leaves 185 of the original 466 package D functions with scored concessions. Since Batch 12, 57 functions covering 19,848 bytes have become concession-free. The build still reproduces the original ROM.

The list below contains only functions still open at this checkpoint, in ROM order. The Batch 12 source archive supplies a starting C draft for each. Its instruction and byte differences describe that archived draft. Subsequent compiler or source findings are recorded separately below the affected entry.

- **0810E9D4 · TimerDisplay_DrawRecord**
  The two returned tile pointers coalesce with r0.
  Write both record delimiters through a tile-row cursor that advances after each store. The retained draft differs in 6 aligned assembly rows.

- **0810F348 · RecordMenu_UpdateSelection**
  The left/right mask, glyph pointer, choice pointer and zero compete across the animation call.
  Give left and right selection changes separate sound and cursor animation call sites before the shared exit. The retained draft differs in 16 aligned assembly rows.

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

- **081294B4 · FeatherEffectPool_CreateDirectional**
  Without the velocity operands the signed-direction switch folds components into the angle setup.
  Decode direction into row and flip from the two quadrant bits (rows 0,1,1,0). The retained draft differs in 14 aligned assembly rows.

- **0812A660 · ActorEffect_InitColliders**
  The two relative collider pointers conflict with the power/value call arguments.
  Give the body collider its own base-relative address independently of the attack collider. The retained draft differs in 35 aligned assembly rows.

- **0812C41C · FragmentEffect_SpawnRandomParticle**
  The three sin/cos samples compete with the loop-carried random table and the outgoing call arguments.
  Carry the signed radius through the two horizontal components, then use direct random-table indexing for height. The detached difference fell from 60 to 46 aligned assembly rows.

- **0812CF9C · SceneSlot_EmitPulse**
  The emitter twin reuses the same sampled angles as 0812C41C.
  Carry the signed radius through the two horizontal components, then use direct random-table indexing for height. The detached difference fell from 60 to 46 aligned assembly rows.

- **0812D064 · LargeFragmentEffect_SpawnRandomParticle**
  Only constrained subsets reproduce the random-table addressing and velocity setup.
  Use the same typed random cursor reader for both vertical velocity and particle lifetime. The detached difference fell from 16 to 15 aligned assembly rows.

- **081301C4 · SceneActorReaction_UpdateRouteRecovery**
  After depinning, the signed height copies from ROM r1 to r0 and the stack-address scratch changes from r0 to r1.
  Assign the route height through a position-to-position helper while preserving route X/Z. The retained draft differs in 3 aligned assembly rows.

- **08131820 · SceneActorControl_UpdateRouteApproach**
  The state-entry setup and timer stores now match without their nine old constraints.
  Use the stored previous route index as the repeated-choice operand for modulo. The retained draft differs in 5 aligned assembly rows.

- **08135F70 · SceneActorReaction_ResolveDamage**
  Forming the updated hit value at its halfword store improved the reaction draft substantially. The retained ordinary source also removes both local damage macros.
  Return the hit flag address together with the updated halfword, then read the reaction-mode bit via its field. The detached difference fell from 114 to 14 aligned assembly rows.

- **0813727C · sub_0813727C**
  The accepted-motion branch now forms the actor field at the store. The remaining difference is in the reset sequence.
  Read the clear and ResetControl arguments from the actual flag field in the accepted-motion arm. The detached difference fell from 26 to 3 aligned assembly rows.

- **081381CC · LinkSession_UpdatePlayerValueAverage**
  A typed selection-bits accessor restores the session pointer to r1.
  Compute the selection byte index in a halfword before taking the pointer to the two four-participant pages. The detached difference fell from 3 to 2 aligned assembly rows.

- **081383E0 · Registry1745_UpdateParticipantScores**
  The fill loop loses the copy of the 0xffff mask into r2.
  Copy the absent halfword from the just-written points slot into the corresponding total. The retained draft differs in 2 aligned assembly rows.

- **081384A0 · sub_081384A0**
  The pin-free selection sort changes the inner-loop induction values.
  Use paired score/index cursors that descend together over the unsorted prefix. The retained draft differs in 27 aligned assembly rows.

- **08138778 · PositionEffectDisplay_UpdateSources**
  An indexed loop using PlayerCue_StepSequenceEnded reproduces the playback body.
  Advance the slot pointer once per iteration for assignment while retaining the index for sequence and stop flags. The retained draft differs in 16 aligned assembly rows.

- **0813B4C8 · Player_RecalculateDerivedStats**
  The unpinned contiguous stat loops assign different registers to the quotient, cap and destination.
  Drive each three-stat update by reaching the end of the output array to avoid reversal of a fixed-count loop. The detached difference fell from 26 to 21 aligned assembly rows.

- **0813B7C4 · sub_0813B7C4**
  Removing 1 local constraint changes 16 normalized assembly rows across 8 regions.
  Select the body collision kind using the only distinction that affects the collider, then apply its override. The retained draft differs in 16 aligned assembly rows.

- **0813C42C · ConvergingEffects_EmitNearParticle**
  Removing the angle pin changes how the two sine table samples and random radius are kept across the spawn path.
  Cache the sine-table slot for the sampled angle and use the wrapped displacement for cosine. The retained draft differs in 11 aligned assembly rows.

- **0813D834 · sub_0813D834**
  The two pinned values connect the dispatch kind to the scaled status-timer offset across branches.
  Use a pointer into scriptStatus as the switch result and write through it after both dispatch arms. The retained draft differs in 23 aligned assembly rows.

- **0813EDDC · PlayerAction_RefreshFlagState**
  Removing 7 local constraints changes 8 normalized assembly rows across 4 regions.
  Separate the optional override from ordinary slot resolution and assign the validated current item directly. The retained draft differs in 8 aligned assembly rows.

- **081411B8 · Player_ApplyDirectionalForce**
  The active/input checks merge differently without the barrier on the player pointer, and the scaled Z sample no longer occupies the original register.
  Fold the two force-block checks into a single player validity region and evaluate the vertical sine sample at the point of its product. The retained draft differs in 27 aligned assembly rows.

- **0814159C · Player_UpdateCameraOffset**
  Splitting input-plus-one from its mask leaves the history sample, camera angle and mode allocated differently across the clamp calls.
  Fold the direction-history sample into its phase by wrapping an 8-bit angle, removing the index mask. The retained draft differs in 22 aligned assembly rows.

- **081432DC · Player_UpdateHeldInteraction**
  Removing 3 local constraints changes 23 normalized assembly rows across 8 regions.
  Compute the facing sector from the low angle byte before storing it, instead of shifting a full unsigned halfword. The retained draft differs in 23 aligned assembly rows.

- **08146EAC · Player_UpdateAlternateFormCue**
  Removing 1 local constraint changes 11 normalized assembly rows across 5 regions.
  Use the typed game-state position fields for clamp and the sprite X/Z, letting ordinary reads share their base. The retained draft differs in 11 aligned assembly rows.

- **0814D9C4 · sub_0814D9C4**
  The combo index is scaled into an offset and then converted to a phase-table pointer in two action arms.
  Index the weapon phase table using comboIndex as an array subscript before both animation callbacks. The retained draft differs in 93 aligned assembly rows.

- **0814DE0C · Player_ConfigureActionSelectionDisplay**
  Removing 1 local constraint changes 2 normalized assembly rows across 2 regions.
  Test flags through the enclosing stat region anchored to the four-byte player field. The retained draft differs in 2 aligned assembly rows.

- **0814DF0C · Player_IsStatBBelow**
  Removing 1 local constraint changes 4 normalized assembly rows across 4 regions.
  Resolve override or player stat on separate returning paths, keeping the threshold live across either read. The retained draft differs in 4 aligned assembly rows.

- **0814DF58 · Player_SubtractStatB**
  The formerly detached source still passed a register through the LOADB macro. Its replacement names the override fields at all four calls. The revised concession detector also catches register names passed as macro parameters.
  Use the named override fields separately on the read and write paths, preserving both tests. The retained draft differs in 8 aligned assembly rows.

- **0814F328 · Player_UpdateAttachedAttackCharge**
  Using packed three-component vectors and the typed player frame removes the burst and latch constraints.
  Model the held key pointer as a player-relative input view, then test the bit through its typed field. The retained draft differs in 32 aligned assembly rows.

- **08156F54 · Player_TestTetherActionInput**
  The trigger path loads the global tether pointer a second time in the ROM.
  Classify state 1 and state 0x11 by masking their differing bit before reading the tether status fields. The retained draft differs in 5 aligned assembly rows.

- **08156FE0 · Player_SelectActionSequence**
  The selector has six pins, including shared helper state.
  Read held keys and action mode via the typed input and action-mode fields before the alternate-selector tests. The retained draft differs in 24 aligned assembly rows.

- **08158C14 · sub_08158C14**
  Four independently computed counters lose their joint address lifetimes when the operand barrier is removed.
  Store the four reset counters together through chains of halfword assignments in the same order. The retained draft differs in 20 aligned assembly rows.

- **08159164 · sub_08159164**
  Removing 1 local constraint changes 21 normalized assembly rows across 6 regions.
  Acquire the first player once inside the playable-state guard, reusing that pointer for both bonus categories. The retained draft differs in 21 aligned assembly rows.

- **0815A048 · sub_0815A048**
  Unpinning the previous flags, bit 4 and the halfword zero merges the zero sources for the byte and halfword resets.
  Use a dedicated halfword zero and byte zero for the reset stores rather than two unsigned words. The retained draft differs in 52 aligned assembly rows.

- **0815A73C · sub_0815A73C**
  The maximum-health and current-health offsets need to survive over separate loads and stores.
  Address the user health and maximum through their actual members on either side of the percentage calculation. The retained draft differs in 25 aligned assembly rows.

- **0815E034 · sub_0815E034**
  The contact collider, damage, flags and common mask compete across the cost branches.
  Use the masked attack category directly for the damage dispatch instead of keeping the common 63 in a local. The retained draft differs in 62 aligned assembly rows.

- **0815E1F0 · PlayerContact_ApplyResponse**
  Removing 2 local constraints changes 14 normalized assembly rows across 6 regions.
  Carry the source power into the timed-reset call as its incremented duration, with no unused flag read. The retained draft differs in 14 aligned assembly rows.

- **0815ED78 · Player_ApplyLatchedActionMode**
  Removing 1 local constraint changes 9 normalized assembly rows across 3 regions.
  Dispatch the latched resource through a switch and supply explicit known modes as helper arguments. The retained draft differs in 9 aligned assembly rows.

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

- **08163020 · sub_08163020**
  The unpinned temporary changes register allocation across the collision tests.
  Compute each letter’s signed displacement from its inside-out array index instead of maintaining an offset across both halves. The retained draft differs in 6 aligned assembly rows.

- **081639E4 · sub_081639E4**
  Both index/address pins feed table access.
  Derive both jitter samples from the starting random cursor in one 1024-entry table. The retained draft differs in 41 aligned assembly rows.

- **08164480 · sub_08164480**
  The pointer read now matches without volatile qualification.
  Load the HUD owner through its named view member at all three timer tests with a plain pointer. The retained draft differs in 22 aligned assembly rows.

- **08164DC0 · Hud_UpdateTransientStatGauge**
  Two fill loops load a mask directly into the register where the ROM first copies it.
  Write the gauge decoration tiles and blank row through a descending halfword cursor with an end pointer. The retained draft differs in 7 aligned assembly rows.

- **08165D10 · SceneHud_TickPaletteBlend**
  A byte-exact permutation introduced an empty loop between forming the red-channel mask and applying it. Sixteen ordinary RGB555 unpackers were also tried. The prior two-row draft remains closest.
  Extract red by discarding the high eleven bits of the 16-bit value via a narrowed left shift. The retained draft differs in 2 aligned assembly rows.

- **081680F4 · Hud_LoadConditionalPaletteRows**
  The unpinned index and final zero change how the loop and palette stores allocate temporaries.
  Drive the four palette rows using a row coordinate from 16 through 19 rather than a separate loop counter. The retained draft differs in 6 aligned assembly rows.

- **08168A0C · SceneHud_RebuildPrimaryOptions**
  The pinned state pointer feeds the initial selected-slot and list accesses.
  Share the selected-slot pointer with the options cursor in the normal branch without caching the game-state base. The retained draft differs in 9 aligned assembly rows.

- **08168EE8 · SceneHud_TickWidgetSlides**
  The three expanded slide loops keep their destination cursor in a shared hard register.
  Replace all three slide macros with one inline helper whose parameters are the actual widget fields and halfword destination. The retained draft differs in 34 aligned assembly rows.

- **0816B958 · MapMenu_DrawActiveMarkers**
  The draw helper and caller pin the marker index, base and attributes.
  Derive the horizontal-flip bit directly from the normalized marker flag and combine it with palette bank 14. The retained draft differs in 67 aligned assembly rows.

- **0816BC54 · sub_0816BC54**
  The five register constraints in this multi-branch map update are coupled.
  Load the active page before comparing the first neighbor and carry its saved index through that assignment. The retained draft differs in 377 aligned assembly rows.

- **0816D4E4 · SceneHud_CanApplySelectedAction**
  The inventory-present path loads the signed slot again for Inventory_GetEquipmentEntry.
  Test the equipped halfword sign using its unsigned high bit while passing a signed typed read to the item lookup. The retained draft differs in 2 aligned assembly rows.

- **0816D634 · sub_0816D634**
  The empty asm precedes the shared clear path for two item-duration cases.
  Place a direct clear-and-return completion at the first duration case(s), allowing the late cross-jump pass to choose its join. The retained draft differs in 43 aligned assembly rows.

- **0816E570 · sub_0816E570**
  The random table base must survive the Mod calls for the velocity samples.
  Represent the random source as its 1024-entry table for both coordinates, speed, and flip. The retained draft differs in 24 aligned assembly rows.

- **0816E6F0 · sub_0816E6F0**
  Removing both counter/phase mask barriers and their pins changes the halfword mask/activation stores.
  Advance the actual halfword phase counter and detect every sixteenth tick from its low nibble. The retained draft differs in 9 aligned assembly rows.

- **08170D78 · sub_08170D78**
  The ROM reloads a signed item slot after testing it, even though no store or call intervenes.
  Check the signed equipment-slot id with the unsigned high bit, then pass its signed form to inventory resolution. The detached difference fell from 26 to 24 aligned assembly rows.

- **08179C38 · ActorImpact_CopyContact**
  The contact-field stores from the newly matched 08029870 sibling reduced the detached candidate to four differing normalized rows.
  Reconstruct contact reward fields with a typed word cursor or a two-field copy helper, then read the reaction kind through the owning actor. The detached difference fell from 10 to 4 aligned assembly rows.

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

- **0817D67C · ActorEffect_InitializeOffset**
  The sine sample for Z goes to r3, while the ROM keeps it in r2 through the signed fixed-point scaling.
  Port the matched offset-by-heading idiom from linked effects into paired, mutating or scalar radius projections. The retained draft differs in 5 aligned assembly rows.

- **0817EBB4 · BossSlot2_UpdateSummonSpinSequence**
  Removing 3 local constraints changes 20 normalized assembly rows across 9 regions.
  Apply the newly upstream four-way and eight-way helpers independently to spin setup and both repeated animation branch tails. The retained draft differs in 20 aligned assembly rows.

- **0817F9C0 · BossSlot2_UpdatePresentation**
  Removing 1 local constraint changes 21 normalized assembly rows across 8 regions.
  Move shadow enabling into each grounded branch or a typed owner helper so field formation occurs inside the call expression. The retained draft differs in 21 aligned assembly rows.

- **08180730 · ProximityVolume_Update**
  Removing 1 local constraint changes 23 normalized assembly rows across 19 regions.
  Use the nav-probe position vector consistently for both coordinates and separate the inactive-emitter return from proximity motion. The retained draft differs in 23 aligned assembly rows.

- **08181634 · ArmorPalette_CopyKindBanks**
  Removing 4 local constraints changes 11 normalized assembly rows across 9 regions.
  Keep the three palette-copy destinations as one running bank cursor and compute the independent fourth destination from its owning index. The retained draft differs in 11 aligned assembly rows.

- **0818171C · ActorPalette_LoadScriptBanks**
  Using a byte offset measured from the palette owner matches the last destination address, with the two inputs to the Thumb ADD exchanged.
  Contrast typed palette row addressing with signed, unsigned and pointer accumulation in the last bank destination. The retained draft differs in 1 aligned assembly row.

- **08182454 · ActorController_SetAnimation**
  Removing 9 local constraints changes 28 normalized assembly rows across 11 regions.
  Derive the side from the actual flags, pass typed animation subobjects directly, and remove the grouped argument constraint with alternate remaining bindings. The retained draft differs in 28 aligned assembly rows.

- **08186CC8 · Armor_SelectAlignedAttack**
  Tested the single decision-clear site separately from the widely shared grid operations. Signed coordinate helpers produced a closer ordinary grid calculation.
  Reconstruct the absolute coordinate and grid-cell operations as common signed helpers, then examine the single decision-clear site independently. The detached difference fell from 60 to 36 aligned assembly rows.

- **0818CB30 · Armor_UpdatePartOrbit**
  Removing 2 local constraints changes 25 normalized assembly rows across 10 regions.
  Replace the held zero with an ordinary return path and implement the terminal parent-state dispatch as a range, guards, and explicit cases. The retained draft differs in 25 aligned assembly rows.

- **0818E9B8 · SpriteColliderGroup_ActivateParts**
  Removing 2 local constraints changes 10 normalized assembly rows across 5 regions.
  Reconstruct typed part stride, activation count and slot index through the live part-group fields. The retained draft differs in 10 aligned assembly rows.

- **0818EA3C · SpriteColliderGroup_ResetActiveParts**
  The very first count load and comparison use r6 where the ROM uses r5.
  Test the same group iterator used by activation with an addressable count or direct field and a typed indexed first element. The retained draft differs in 2 aligned assembly rows.

- **0818EA90 · Armor_QueueFallingParts**
  The loop index and the active-count pointer trade r8 and ip.
  Unify the position, state and launch slot iterators through the real part address and test increment placement and count ownership. The retained draft differs in 6 aligned assembly rows.

- **0818ECFC · sub_0818ECFC**
  Giving the two collider calls independent accesses to the owning slot’s position leaves the timing of the position address and its argument register as the remaining differences.
  Test read ownership at both collider calls individually, together with delaying the common position pointer until the parity branch. The detached difference fell from 65 to 5 aligned assembly rows.

- **0819BB18 · sub_0819BB18**
  The pinned setup copies the bank address through three tied operands.
  Separate field-address and payload lifetimes through typed owner accessors (closest v_x_2). The retained draft differs in 38 aligned assembly rows.

- **0819BCB8 · Hresvelgr_EmitFacingMotion**
  The relative sample helper recovers the real use of the apparent 0x0403B714 dead literal.
  Separate field-address and payload lifetimes through typed owner accessors (closest out_x_1). The retained draft differs in 53 aligned assembly rows.

- **0819CA38 · Hresvelgr_RefreshTarget**
  Removing the final travel-result pin changes the square temporary and lets the halfword store use the return register directly.
  Separate field-address and payload lifetimes through typed owner accessors (closest out_x_2). The retained draft differs in 7 aligned assembly rows.

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

- **081A4DF0 · sub_081A4DF0**
  The ROM leaves the last Anim_SetKeyframes result in r0 while clearing the final projectile entry and active field.
  Separate field-address and payload lifetimes through typed owner accessors (closest pool_parameterA_2). The retained draft differs in 14 aligned assembly rows.

- **081A681C · ScreenPalette_UpdateTransition**
  The final palette bank index loses the r0 temporary for variant times three, merging the copy used to form bank+9.
  Separate field-address and payload lifetimes through typed owner accessors (closest s_palettes_1). The retained draft differs in 4 aligned assembly rows.

- **081A8E08 · sub_081A8E08**
  The capture-setup helper returns its Y coordinate for later use as an animation parameter.
  Separate field-address and payload lifetimes through typed owner accessors (closest out_x_1). The retained draft differs in 12 aligned assembly rows.

- **081A93A0 · sub_081A93A0**
  The attack controller has both a switch-state allocation and helper-level constraints on vector operations.
  Separate field-address and payload lifetimes through typed owner accessors (closest a_y_1). The detached difference fell from 102 to 71 aligned assembly rows.

- **081A95A8 · Vanargand_UpdatePairedAttack**
  The ROM holds the phase across calls and resets the timer from r4 in case zero.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_flags_1). The retained draft differs in 26 aligned assembly rows.

- **081AA178 · Vanargand_UpdateRepeatedPairAttack**
  Exactly one immediate-zero load disappears when resetting the sixth repeat.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_mode_0). The retained draft differs in 2 aligned assembly rows.

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

- **081B0018 · PairedSceneActor_InitializeHalf**
  Removing 3 local constraints changes 60 normalized assembly rows across 29 regions.
  Initialize the eight-byte half-offset record using the checked-in ZeroFill8 helper. The detached difference fell from 60 to 58 aligned assembly rows.

- **081B08B8 · ActorStatus_UpdatePlayerApproach**
  Removing 5 local constraints changes 8 normalized assembly rows across 6 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest base_sequence_1). The retained draft differs in 8 aligned assembly rows.

- **081B5EA4 · sub_081B5EA4**
  At controller initialization end the 0x5b4 state offset goes in r3 instead of r2, shifting the following two-way switch mode copy.
  Separate field-address and payload lifetimes through typed owner accessors (closest v_x_2). The retained draft differs in 5 aligned assembly rows.

- **081B66E0 · VanargandEffect_Initialize**
  Removing 1 local constraint changes 2 normalized assembly rows across 1 region.
  Separate field-address and payload lifetimes through typed owner accessors (closest effect_animation_1). The retained draft differs in 2 aligned assembly rows.

- **081B6E90 · ActorEffect_UpdateRadialExpiry**
  The six-iteration trail loop assigns the index, table, RNG cursor and 1023 mask to four different registers.
  Separate field-address and payload lifetimes through typed owner accessors (closest effect_entered_0). The retained draft differs in 16 aligned assembly rows.

- **081B95E4 · MotionTrack_Update**
  Removing 2 local constraints changes 2 normalized assembly rows across 1 region.
  For the vertical steering arm, vary the lifetime and order of the source and destination coordinate loads and factor a typed position-vector step through the existing scalar helper. The retained draft differs in 2 aligned assembly rows.
  The differing offset register is chosen during reload. The sixth movement case inherits a rotation from the last two signed-coordinate loads in case four. ROM and draft have the same size.

- **081BB904 · sub_081BB904**
  Removing 5 local constraints changes 16 normalized assembly rows across 8 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest panel_unk1D8_1). The retained draft differs in 16 aligned assembly rows.

- **081BBE88 · sub_081BBE88**
  Removing 1 local constraint changes 5 normalized assembly rows across 3 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_w58_1). The retained draft differs in 5 aligned assembly rows.

- **081BC820 · StatGauge_BeginAdjustment**
  Removing 4 local constraints changes 65 normalized assembly rows across 35 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest widget_flags_1). The retained draft differs in 65 aligned assembly rows.

- **081BD38C · DarkLoan_BuildDisplay**
  Removing 1 local constraint changes 77 normalized assembly rows across 21 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_background_1). The retained draft differs in 77 aligned assembly rows.

- **081BDE94 · Storage_BeginSwapTransition**
  Removing 7 local constraints changes 47 normalized assembly rows across 9 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest widget_flags_1). The retained draft differs in 47 aligned assembly rows.

- **081BF67C · Storage_InitWidgets**
  Removing 3 local constraints changes 5 normalized assembly rows across 4 regions.
  Represent the menu sprite and its relocated resource as an array of complete widget records. The detached difference fell from 5 to 3 aligned assembly rows.

- **081C1918 · ControlPanel_UpdateGaugeIndicator**
  Removing 3 local constraints changes 9 normalized assembly rows across 5 regions.
  Use the established Player indicatorUpdateRate/displaySegmentCount fields for the actual divisor and sound segment, testing the grouped register lifetimes separately. The retained draft differs in 9 aligned assembly rows.

- **081C1E48 · sub_081C1E48**
  Removing 1 local constraint changes 2 normalized assembly rows across 2 regions.
  Use one lifetime for the segment multiplier and resulting triangular step, including reciprocal operand order and nonnegative half division. The retained draft differs in 2 aligned assembly rows.

- **081C2570 · ControlPanel_InitializeFromScript**
  Removing 2 local constraints changes 72 normalized assembly rows across 48 regions.
  Reconstruct the three 0x4c-spaced sprite/animation pairs through one resource initializer and independently release the shared sprite and animation registers. The retained draft differs in 72 aligned assembly rows.
  The three display entries have a 0x4C-byte stride. Each has a position at +0x1C and graphics at +0x2C. The middle entry’s animation variant is at +0x4A. Replacing the layout alone does not reproduce both cursor registers.

- **081C4708 · sub_081C4708**
  Removing 1 local constraint changes 5 normalized assembly rows across 3 regions.
  Treat the script result as a typed tile resource and fold the first-tile load into its consuming call or into a small resource-load helper. The retained draft differs in 5 aligned assembly rows.

- **081C58BC · sub_081C58BC**
  Removing 1 local constraint changes 2 normalized assembly rows across 1 region.
  Recover the adjacent packed triplet and selection as a two-field object, and compare a field accessor/base pointer around the rank lookup. The retained draft differs in 2 aligned assembly rows.

- **081C62B4 · sub_081C62B4**
  Removing 1 local constraint changes 6 normalized assembly rows across 5 regions.
  Walk distinct digit and coordinate arrays with their native strides. The retained draft differs in 6 aligned assembly rows.

- **081C74F0 · StateActor_UpdateDashAnimation13**
  Removing 1 local constraint changes 36 normalized assembly rows across 31 regions.
  Separate dash-tag access from the actor entry switch and carry the position owner through an accessor or its real member. The detached difference fell from 36 to 19 aligned assembly rows.

- **081C7934 · StateActor_UpdateAnimation16State**
  Removing 1 local constraint changes 11 normalized assembly rows across 7 regions.
  Replace the local terrain decoder with the shared SceneActorTerrain_GetHeight and compare its cell/position call boundary inside the entry transition. The detached difference fell from 11 to 10 aligned assembly rows.

- **081C8EC0 · sub_081C8EC0**
  The radial constructor naturally uses the 0x0403B714 origin for the late relative sample, but its capture displaces the tag pointer, radius, and angle.
  Use the established relative-sample origin, direct/random-bank helpers, and mixed read sequences for the three radial effect RNG draws. The detached difference fell from 16 to 14 aligned assembly rows.

- **081CB294 · StateActor_UpdateRiseToTerrain**
  Removing 2 local constraints changes 33 normalized assembly rows across 22 regions.
  Read the rise position through owner versus position-view fields and release the sine-delta operand and saved-position binding separately. The retained draft differs in 33 aligned assembly rows.
  A typed position and tag reduce the ordinary draft to 11 differing rows. The terrain slope loads and two independently materialized zeros for the phase and tag resets still differ.

- **081CC1B4 · sub_081CC1B4**
  Removing 10 local constraints changes 32 normalized assembly rows across 18 regions.
  Map MenuState and MenuKind to the established Player.sceneMode and Player.tetherMode fields and release the original field constraints independently. The retained draft differs in 32 aligned assembly rows.

- **081CF398 · sub_081CF398**
  An ordinary palette interpolation call with the two bank addresses in locals changes five assembly regions.
  Replace the fade macro by an ordinary palette helper/direct call and test bank addresses and tick bounds at the fade site. The retained draft differs in 7 aligned assembly rows.
  The 081CF264 sibling confirms the fade timing and palette banks. Sharing a fade macro across those two callers changes the sibling’s bytes. The current stock-matching function still uses its local macro.

- **081D0864 · sub_081D0864**
  Removing 2 local constraints changes 17 normalized assembly rows across 9 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest npc_sub20_1). The retained draft differs in 17 aligned assembly rows.

- **081D11E4 · SceneNpc_SpawnTransitionParticle**
  The ROM pins the random table in r6 across calls to Mod.
  Separate field-address and payload lifetimes through typed owner accessors (closest effect_particles_1). The retained draft differs in 28 aligned assembly rows.

- **081D2318 · TaggedSceneNpc_UpdateTargetFacing**
  Removing 1 local constraint changes 20 normalized assembly rows across 10 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_position_1). The detached difference fell from 20 to 2 aligned assembly rows.

- **081D2D78 · sub_081D2D78**
  Removing 2 local constraints changes 18 normalized assembly rows across 14 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest slot_position_1). The retained draft differs in 18 aligned assembly rows.

- **081D3094 · sub_081D3094**
  Removing 2 local constraints changes 6 normalized assembly rows across 4 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_graphics_1). The retained draft differs in 6 aligned assembly rows.

- **081D31EC · sub_081D31EC**
  Removing 2 local constraints changes 12 normalized assembly rows across 6 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest src_position_0). The retained draft differs in 12 aligned assembly rows.

- **081D3620 · sub_081D3620**
  The cloud loop’s table, coordinate scratch, and random value have different allocation without constraints.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_particles_1). The retained draft differs in 27 aligned assembly rows.

- **081D4024 · PaletteTransition_CopyBanks**
  Removing 2 local constraints changes 11 normalized assembly rows across 9 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest c_firstBank_1). The retained draft differs in 11 aligned assembly rows.

- **081D61C0 · sub_081D61C0**
  Removing 6 local constraints changes 85 normalized assembly rows across 45 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_entry_0). The detached difference fell from 85 to 40 aligned assembly rows.

- **081D8934 · Task4E01_UpdateTrailImpact**
  Removing 1 local constraint changes 4 normalized assembly rows across 2 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest sprite_flags_1). The retained draft differs in 4 aligned assembly rows.

- **081D905C · TimedEffect_UpdateHoming**
  Removing 1 local constraint changes 105 normalized assembly rows across 45 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest a_x_0). The retained draft differs in 105 aligned assembly rows.

- **081D9D3C · Projectile_SpawnImpactPair**
  Expanding the three RNG setup assignments removes the private macro but swaps the parameter-buffer and table registers inside the impact loop.
  Separate field-address and payload lifetimes through typed owner accessors (closest b_y_0). The detached difference fell from 51 to 35 aligned assembly rows.

- **081DB794 · Options_InitScreen**
  Removing 1 local constraint changes 149 normalized assembly rows across 40 regions.
  Represent the menu sprite and its relocated resource as an array of complete widget records. The retained draft differs in 149 aligned assembly rows.

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

- **081EC670 · ScreenLayers_HidePair**
  Removing 2 local constraints changes 5 normalized assembly rows across 3 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest screen_firstFlags_1). The retained draft differs in 5 aligned assembly rows.

- **081EC7D8 · ActorDisplay_UpdateForegroundSprite**
  Removing 2 local constraints changes 74 normalized assembly rows across 26 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest owner_sequence_1). The retained draft differs in 74 aligned assembly rows.

- **081EF914 · SingletonSlot7_UpdateEndingScene**
  Removing 7 local constraints changes 122 normalized assembly rows across 86 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest owner_sprite_1). The retained draft differs in 122 aligned assembly rows.

- **081F02EC · SingletonSlot7_BeginScaledCooldown**
  Removing 6 local constraints changes 3 normalized assembly rows across 1 region.
  Separate field-address and payload lifetimes through typed owner accessors (closest action_cooldown_0). The retained draft differs in 3 aligned assembly rows.

- **081F05D0 · SingletonSlot7_OnTargetContact**
  Removing 6 local constraints changes 2 normalized assembly rows across 1 region.
  Separate field-address and payload lifetimes through typed owner accessors (closest target_owner_0). The retained draft differs in 2 aligned assembly rows.

- **081F28C4 · SpriteTrail_CreateFixedEffect**
  Removing 1 local constraint changes 4 normalized assembly rows across 4 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_phase_2). The retained draft differs in 4 aligned assembly rows.

- **081F2A08 · SpriteTrail_CreateMotionEffect**
  Removing 2 local constraints changes 34 normalized assembly rows across 28 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_phase_2). The retained draft differs in 34 aligned assembly rows.

- **081F467C · Ratatosk_UpdateHomingShot**
  The final tick comparison and callback address use one lower reload register than the ROM.
  Separate field-address and payload lifetimes through typed owner accessors (closest a_y_0). The detached difference fell from 5 to 2 aligned assembly rows.

- **081F7DAC · Ratatosk_UpdateCloneAttack**
  The two random position draws share a pinned r4 value across their Mod calls.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_active_0). The retained draft differs in 22 aligned assembly rows.

- **081F8988 · Ratatosk_UpdateSunOrbRelocation**
  Using the sequence field instead of the 0x180 pinned address changes reload choices in the sequence-two transition.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_active_0). The retained draft differs in 8 aligned assembly rows.

- **081FD7EC · OrbitEffect_FireVolley**
  Removing 2 local constraints changes 47 normalized assembly rows across 20 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_wide_0). The retained draft differs in 47 aligned assembly rows.

- **081FDAF4 · RisingActor_AdvanceHeight**
  Removing 2 local constraints changes 4 normalized assembly rows across 3 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest state_fieldF4_2). The retained draft differs in 4 aligned assembly rows.

- **081FDCB0 · sub_081FDCB0**
  Removing 3 local constraints changes 38 normalized assembly rows across 13 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_x_2). The retained draft differs in 38 aligned assembly rows.

- **081FF454 · HomingProjectile_UpdateDefaultState**
  Removing 4 local constraints changes 58 normalized assembly rows across 22 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_lifetime_0). The detached difference fell from 40 to 38 aligned assembly rows.

- **082012D4 · PlayerClone_BeginActiveReactions**
  Removing 1 local constraint changes 11 normalized assembly rows across 6 regions.
  Passing the slot reset value into Anim_LoadKeyframe gives the desired long-lived zero. The detached difference fell from 11 to 1 aligned assembly rows.

- **08202730 · ProjectilePool_Update**
  Removing 10 local constraints changes 44 normalized assembly rows across 18 regions.
  Tested the shared player-cue stepper with the sprite and sequence stored in separate projectile fields. The retained draft differs in 44 aligned assembly rows.
  The uninitialized terrain scratch and two empty assembly operands were removed. The command and three operand constraints still affect register allocation.

- **08206C00 · VulcanCannon_UpdateTargeting**
  Target angle parameters, the player pointer and the cached target snapshot interact.
  Compared structure assignment and Memory_Copy for the effective player position, with both target-angle argument orders. The detached difference fell from 249 to 88 aligned assembly rows.

- **082074F0 · VulcanCannon_UpdateDamageState**
  Removing 8 local constraints changes 38 normalized assembly rows across 20 regions.
  Reconstructed the Vulcan damage animation call as an inline restart using the resource and facing in both parameter orders. The retained draft differs in 38 aligned assembly rows.

- **08208498 · CannonTurret_FireAimedVolley**
  The projectile spread needs a particular captured table address across the jitter and speed sample.
  Separated jitter and speed samples into individual narrow values, and retried the random table as a symbol and a constant-address pointer. The retained draft differs in 13 aligned assembly rows.

- **0820EE34 · OrangeSpine_ApplyHitReaction**
  The damage handler’s empty outputs are consumed only by empty operands.
  Tested direct reaction and HUD member stores, a separately evaluated spark guard, and named RNG storage through the Orange Spine damage and spark paths. The retained draft differs in 74 aligned assembly rows.

- **0821025C · ProjectileEmitter_EmitAttackFrame**
  Removing 1 local constraint changes 5 normalized assembly rows across 5 regions.
  Tested signed and narrow frame reads and two alternative shared selector calls for the emitter. The retained draft differs in 5 aligned assembly rows.

- **08210398 · PinkSpine_UpdateDamageAnimation**
  Removing 1 local constraint changes 6 normalized assembly rows across 3 regions.
  Compared a typed collider-flags pointer, its actual halfword width and alternate value captures at the branch. The retained draft differs in 6 aligned assembly rows.

- **082124D8 · ForgingBook_InitSprites**
  Removing 1 local constraint changes 14 normalized assembly rows across 10 regions.
  Reconstructed the book effect using a single stack position and direct x/y assignments instead of a retained pointer to the stack record. The retained draft differs in 14 aligned assembly rows.

- **08213FD8 · sub_08213FD8**
  Removing 3 local constraints changes 23 normalized assembly rows across 7 regions.
  Wrote the four BG2 matrix coefficients through the SDK registers. The detached difference fell from 23 to 9 aligned assembly rows.

- **0821537C · sub_0821537C**
  Using the SDK DMA copy and counting from the saved queue length improve the loop, but the lifetime of the DMA mode and queue globals still changes the literal loads and saved registers.
  Compared a counted slot loop against a fixed-width queue pointer with typed DMA3 channel writes. The retained draft differs in 10 aligned assembly rows.

- **08215B18 · Bg_SetAffineTransform**
  Using C division and changing the initial-scale helper’s denominator type leaves agbcc converting the signed halfword before the dividend setup.
  Split sine-table evaluation and the fixed-point reciprocal at the first and remaining matrix coefficients. The retained draft differs in 25 aligned assembly rows.

- **082160B8 · Bg_SetupLayer**
  Switch alternatives and typed display-control stores still allocate the shadow pointer and current control bits differently at the shared store after the four layer cases.
  Moved the display-control OR into each layer switch arm and varied fixed versus computed layer bits, testing whether the original had one common shadow store. The retained draft differs in 12 aligned assembly rows.

- **08217430 · sub_08217430**
  Reversing palette address operands and using a typed bank array or transfer helper change the two source registers for CpuFastSet.
  Compared destination bank arrays, source palette-key indexing, and preparing the bank pointers in either order before CpuFastSet. The retained draft differs in 7 aligned assembly rows.

- **0821BF5C · sub_0821BF5C**
  The cell-index difference crosses the terrain branches.
  Divided the directional terrain tests into their three source shape cases and varied which rejection or success tails return directly. The retained draft differs in 26 aligned assembly rows.

- **0821D4F0 · sub_0821D4F0**
  The initial cell and loop index contend for r6.
  Evaluated step height separately in the same-cell and crossed-cell paths, keeping the initial cell index and position snapshot independent. The retained draft differs in 14 aligned assembly rows.

- **0821E5C4 · Route_Start**
  Typed route-header indexing and using the stored point byte improve the route cursor setup.
  Indexed route headers as typed records and points as eight-byte SceneActorRoutePoint entries. The detached difference fell from 19 to 10 aligned assembly rows.

- **082207DC · sub_082207DC**
  Without the debug block, agbcc reuses the stack-box address when it inlines the three axis checks.
  Built and tested the segment box in a separate inline block, including an early axis rejection and a single-element box array to vary stack object lifetimes. The retained draft differs in 17 aligned assembly rows.

- **082213B8 · Anim_DrawDecimalDigits**
  Removing the digit-loop index pin changes allocation across all eight digit extractions.
  Compared arithmetic and boolean leading-zero suppression after digit extraction. The retained draft differs in 54 aligned assembly rows.

- **08227FEC · sub_08227FEC**
  Naming the guard value and checking it with the reset pointer live gives the correct registers, but agbcc moves the magic literal load ahead of the load from the guard.
  Used the validated-guard predicate from the adjacent heap routine with separate read, compare and write branches and both literal evaluation orders. The retained draft differs in 5 aligned assembly rows.

- **08229440 · Clock_CheckRtcStatus**
  The SDK returns u8. A two-word scratch object reproduced the bytes, but no caller or driver data supports the extra word. The archived version uses the SDK prototype and direct status decoding.
  Compared direct status checks, SDK-style packed status and error nibble records, and independent decoding around Clock_EndRtcAccess. The retained draft differs in 5 aligned assembly rows.

- **08230AC8 · CgbSound**
  In the PWM frequency adjustment, the ROM reads SOUNDBIAS_H into r0 and copies it to r1 for both thresholds.
  Factored the PSG frequency quantization around the SOUNDBIAS read and tested thresholds, two-bit PWM classes and argument order for the full pitch helper. The retained draft differs in 3 aligned assembly rows.

- **08233140 · TransientSprite_Spawn**
  Removing 2 local constraints changes 3 normalized assembly rows across 1 region.
  Tested a typed slot header, a setter for the narrow ID, and an activation helper. The detached difference fell from 3 to 2 aligned assembly rows.

- **082343B8 · SpiralBurst_UpdateParticles**
  Removing 3 local constraints changes 81 normalized assembly rows across 40 regions.
  Tested fresh radius and angle reads at each spiral coordinate update and separated the timer comparison from the zero-age case. The detached difference fell from 81 to 20 aligned assembly rows.

- **08234540 · SpiralBurst_InitParticles**
  The spiral emitter uses a separate mask and two table aliases across its loop.
  Compared captured and direct random-table pointers across the spiral loop, with narrow angle state and alternative slot cursor advancement. The detached difference fell from 81 to 59 aligned assembly rows.

- **082356E4 · sub_082356E4**
  Removing 1 local constraint changes 8 normalized assembly rows across 3 regions.
  Tested the incremented timer, stored halfword, and explicit early returns for the two shake strengths and task expiry. The retained draft differs in 8 aligned assembly rows.

- **08236FF4 · AttachmentSprite_Setup**
  Removing 3 local constraints changes 41 normalized assembly rows across 11 regions.
  Reconstructed the attachment animation pointer relative to the sprite or parent and varied the ID selection for the two attachment kinds. The detached difference fell from 41 to 5 aligned assembly rows.

- **08237A28 · sub_08237A28**
  The ROM reloads radius in r2 and keeps the glyph angle in r4 while writing packed positions.
  Compared a captured glyph-slot pointer with separate per-axis lookups while reloading the radius for the second trig product. The retained draft differs in 23 aligned assembly rows.

- **0823C930 · Player_UpdateTerrainSidestep**
  The early 08142194 sibling computes a shifted probe position with two scaled sine samples, then tests the terrain.
  Reconstructed both trigonometric sidestep samples as a short-lived probe and reused the typed terrain helpers. The retained draft differs in 17 aligned assembly rows.
