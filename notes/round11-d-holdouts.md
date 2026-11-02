# Round 11 package D holdouts

Batch 26 leaves 54 of the original 466 package D functions with scored concessions. Since Batch 12, 188 functions covering 80,884 bytes have become concession-free. The build still reproduces the original ROM.

The list below contains only functions still open at this checkpoint, in ROM order. The Batch 12 source archive supplies a starting C draft for each. Its instruction and byte differences describe that archived draft. Subsequent compiler or source findings are recorded separately below the affected entry.

- **0810E9D4 · TimerDisplay_DrawRecord**
  The two returned tile pointers coalesce with r0.
  Write both record delimiters through a tile-row cursor that advances after each store. The retained draft differs in 6 aligned assembly rows.

- **0810FDDC · sub_0810FDDC**
  Indexed widget access improves on keeping separate doubled and base indices.
  Carry a pointer to the row for the final, unconditionally visible digit. The retained draft differs in 22 aligned assembly rows.

- **0811AE9C · Terrain_ResolveActorMotion**
  The pin-free query/mask draft differs in 42 scope rows.
  Compute vertical displacement by scaling the reverse height difference using unsigned fixed-point multiplication. The detached difference fell from 40 to 32 aligned assembly rows.

- **081381CC · LinkSession_UpdatePlayerValueAverage**
  A typed selection-bits accessor restores the session pointer to r1.
  Compute the selection byte index in a halfword before taking the pointer to the two four-participant pages. The detached difference fell from 3 to 2 aligned assembly rows.

- **081384A0 · sub_081384A0**
  The pin-free selection sort changes the inner-loop induction values.
  Use paired score/index cursors that descend together over the unsorted prefix. The retained draft differs in 27 aligned assembly rows.

- **08138778 · PositionEffectDisplay_UpdateSources**
  An indexed loop using PlayerCue_StepSequenceEnded reproduces the playback body.
  Advance the slot pointer once per iteration for assignment while retaining the index for sequence and stop flags. The retained draft differs in 16 aligned assembly rows.
  The allocator and updater agree on four 0x60-byte records in a 0x19C-byte display task. The complete layout uses the shared display sprite and sequence records.

- **0813D834 · sub_0813D834**
  The two pinned values connect the dispatch kind to the scaled status-timer offset across branches.
  Use a pointer into scriptStatus as the switch result and write through it after both dispatch arms. The retained draft differs in 23 aligned assembly rows.

- **0813EDDC · PlayerAction_RefreshFlagState**
  Removing 7 local constraints changes 8 normalized assembly rows across 4 regions.
  Separate the optional override from ordinary slot resolution and assign the validated current item directly. The retained draft differs in 8 aligned assembly rows.

- **0814D9C4 · sub_0814D9C4**
  The combo index is scaled into an offset and then converted to a phase-table pointer in two action arms.
  Index the weapon phase table using comboIndex as an array subscript before both animation callbacks. The retained draft differs in 93 aligned assembly rows.

- **0814F328 · Player_UpdateAttachedAttackCharge**
  Using packed three-component vectors and the typed player frame removes the burst and latch constraints.
  Model the held key pointer as a player-relative input view, then test the bit through its typed field. The retained draft differs in 32 aligned assembly rows.
  The held-key pointer is Player.inputKeys at +0x348. A signed-byte offset truncates this to +0x48 and reads the wrong field. Use the real input field in both branches. The valid detached draft differs in 32 aligned rows.
  The full 1,244 bytes now use Player and the actual charge state. The buffered restart, charge tier and duration are confirmed by adjoining charge readers and writers. Two original held-input address groups remain.

- **08159164 · sub_08159164**
  Removing 1 local constraint changes 21 normalized assembly rows across 6 regions.
  Acquire the first player once inside the playable-state guard, reusing that pointer for both bonus categories. The retained draft differs in 21 aligned assembly rows.

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

- **08168A0C · SceneHud_RebuildPrimaryOptions**
  The pinned state pointer feeds the initial selected-slot and list accesses.
  Share the selected-slot pointer with the options cursor in the normal branch without caching the game-state base. The retained draft differs in 9 aligned assembly rows.

- **0816BC54 · sub_0816BC54**
  The five register constraints in this multi-branch map update are coupled.
  Load the active page before comparing the first neighbor and carry its saved index through that assignment. The retained draft differs in 377 aligned assembly rows.

- **0816E570 · sub_0816E570**
  The random table base must survive the Mod calls for the velocity samples.
  Represent the random source as its 1024-entry table for both coordinates, speed, and flip. The retained draft differs in 24 aligned assembly rows.

- **0817AEC8 · sub_0817AEC8**
  Removing the table operand changes which address stays live in the six-iteration loop.
  Factor sine projection through an ordinary polar-offset helper, narrow circular heading to a byte, and address the RNG through its global ring. The detached difference fell from 52 to 48 aligned assembly rows.
  Capturing the shared random index and mask across the three draws restores their setup order. A draft retaining just the original sine-table binding differs in the velocity argument at 0817AFB6. The ROM rematerializes the stack address where the draft copies the vector register.
  The random bank barrier is gone. The emitter shares the halfword table source and index-first reads with the transition spawner, and its actor origin and stack velocity now use ContactPosition. Only the original binding of the Q12 sine table remains.

- **0818E9B8 · SpriteColliderGroup_ActivateParts**
  Removing 2 local constraints changes 10 normalized assembly rows across 5 regions.
  Reconstruct typed part stride, activation count and slot index through the live part-group fields. The retained draft differs in 10 aligned assembly rows.
  The ROM saves r8 without reading it in the loop. Comparable stock m4a pitch and pan routines keep a byte-field zero mask through allocation before the final pass removes its use. In the ordinary armor draft, common-subexpression elimination removes the masks earlier.

- **0818EA3C · SpriteColliderGroup_ResetActiveParts**
  The very first count load and comparison use r6 where the ROM uses r5.
  Test the same group iterator used by activation with an addressable count or direct field and a typed indexed first element. The retained draft differs in 2 aligned assembly rows.
  The reset now uses the same typed Armor group and action field as the activation routines. All instructions in the ordinary draft match except the initial part-count byte load and its comparison, which use r6 instead of r5.

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
  The charge now uses the actual actor body and player flags. An archived switch placed the owner initialization before its first case label, so it could use an uninitialized pointer. It is excluded. The initialized ordinary draft differs only in how it addresses the first flag word.

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
  The capture setup and player drain use the same player action mode and the 16-color palettes in banks 5, 6, and 7 of the boss palette block. The local height binding remains. The player-position argument is the embedded animation position.

- **081AA330 · sub_081AA330**
  The sole remaining mismatch is r1 instead of r2 for the 0x56c offset of the selected-halves object.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_flags_1). The retained draft differs in 2 aligned assembly rows.
  All selected-half queries now use the shared Vanargand attack pair at 0x56C. The ordinary draft differs only in the register used for the final field offset. Stock reload chooses r1, and r2 is outside the spill set in that draft.

- **081ACC24 · Vanargand_ApplyPendingDamage**
  The stage colors are clean through the shared palette helper.
  Separate field-address and payload lifetimes through typed owner accessors (closest self_accumulatedDamage_0). The detached difference fell from 210 to 201 aligned assembly rows.
  The damage path no longer needs a barrier on the body total. Its signed halfword is read together with the field pointer subsequently used for threshold writeback. The body, capture, and attack handlers now share the boss palette banks and the damage part.

- **081AFD10 · PairedSceneActor_UpdateHalfPresentation**
  Removing 28 local constraints changes 63 normalized assembly rows across 24 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest source_y_0). The detached difference fell from 63 to 59 aligned assembly rows.

- **081B08B8 · ActorStatus_UpdatePlayerApproach**
  Removing 5 local constraints changes 8 normalized assembly rows across 6 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest base_sequence_1). The retained draft differs in 8 aligned assembly rows.
  The player approach and animation use the paired actor layout. Four bindings are gone. The remaining clobber affects the register used for the signed player-X load. A fresh ordinary draft differs in two rows. An older draft added a volatile local and is excluded.

- **081B66E0 · VanargandEffect_Initialize**
  Removing 1 local constraint changes 2 normalized assembly rows across 1 region.
  Separate field-address and payload lifetimes through typed owner accessors (closest effect_animation_1). The retained draft differs in 2 aligned assembly rows.
  All three callers pass a variant, a delay, and two position-vector pointers to the factory. The factory and update now share the real 0xA0-byte task. The original clobber remains.

- **081BB904 · sub_081BB904**
  Removing 5 local constraints changes 16 normalized assembly rows across 8 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest panel_unk1D8_1). The retained draft differs in 16 aligned assembly rows.
  The account balance, transfer and repeat fields now come from the actual panel and save data. Four byte-sized repeat masks match without a barrier. In the ordinary draft, three scratch registers rotate on the right-input path. The first right-hand address reload accounts for all eight differing instructions when isolated in the compiler.

- **081BDE94 · Storage_BeginSwapTransition**
  Removing 7 local constraints changes 47 normalized assembly rows across 9 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest widget_flags_1). The retained draft differs in 47 aligned assembly rows.
  The widget initializer and swap transition now share the storage screen layout. The consumable ID must be read as a signed halfword from the save array. A narrowed draft makes the empty-slot test always true and is excluded.
  A single signed item local now spans the three switch arms, removing all three r2 bindings. The four remaining sites are in the equipment marker, consumable slot and consumable marker address helpers. The valid ordinary draft differs principally in reload register choices.

- **081D3620 · sub_081D3620**
  The cloud loop’s table, coordinate scratch, and random value have different allocation without constraints.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_particles_1). The retained draft differs in 27 aligned assembly rows.
  The original X and Z offsets each add a masked random sample minus 512 to the previous coordinate. A detached numeric-width candidate discards the previous X coordinate and is excluded.
  The forty-particle cloud record is shared with the initializer. Keeping the X and Z indices separate and using the two-halfword source removed two bindings. The sample value still uses the original r0 binding.

- **081E2910 · Presentation_UpdateDropShakeBlink**
  Removing 4 local constraints changes 76 normalized assembly rows across 38 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_state_1). The detached difference fell from 74 to 73 aligned assembly rows.
  The sequence and its initializer share the five 0x60 widgets. The remaining sites are the destination of the falling and shaking coordinates and the random-table address in the blink.

- **081EC7D8 · ActorDisplay_UpdateForegroundSprite**
  Removing 2 local constraints changes 74 normalized assembly rows across 26 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest owner_sequence_1). The retained draft differs in 74 aligned assembly rows.
  The foreground drawable now uses the verified departure actor layout. A prior trial narrowed the shake-field offsets 0x5E0, 0x5E6 and 0x5E8 to bytes and addressed the wrong members. It has been excluded. The correctly typed ordinary draft differs in 27 assembly rows. The checked-in function has one original horizontal-sum binding.
  The foreground and accent share their 0x48-byte drawable type. The foreground, accent and background test three different halfwords at 0x5E4, 0x5EC and 0x5EE. The background uses a signed load, and the common screen origin at 0x5E0 is preserved at its full offset.

- **081EF914 · SingletonSlot7_UpdateEndingScene**
  Removing 7 local constraints changes 122 normalized assembly rows across 86 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest owner_sprite_1). The retained draft differs in 122 aligned assembly rows.
  The shared slot-7 entry helper removes the entry-result pin. The ending-scene foreground and sequence flags now use the departure display types, with four prior bindings remaining.

- **081F28C4 · SpriteTrail_CreateFixedEffect**
  Removing 1 local constraint changes 4 normalized assembly rows across 4 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_phase_2). The retained draft differs in 4 aligned assembly rows.

- **081F2A08 · SpriteTrail_CreateMotionEffect**
  Removing 2 local constraints changes 34 normalized assembly rows across 28 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_phase_2). The retained draft differs in 34 aligned assembly rows.
  Separating trail motion setup from rendering removed one binding. A fully ordinary 360-byte layout now differs only in an equal live pointer used to copy the first history position.

- **081F467C · Ratatosk_UpdateHomingShot**
  The final tick comparison and callback address use one lower reload register than the ROM.
  Separate field-address and payload lifetimes through typed owner accessors (closest a_y_0). The detached difference fell from 5 to 2 aligned assembly rows.

- **081F8988 · Ratatosk_UpdateSunOrbRelocation**
  Using the sequence field instead of the 0x180 pinned address changes reload choices in the sequence-two transition.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_active_0). The retained draft differs in 8 aligned assembly rows.
  Use the genuine 0x180 state word. Converting its offset to s8 reads from -128. The valid tracking-state draft preserves the selected-slot call and has eight differing operands.
  The four 24-byte targets and the relocation position pointer are shared with their producers and getters. The selected-slot pair overlaps the fourth entry’s trailing word. The verified ordinary draft differs in four rows.

- **081FDAF4 · RisingActor_AdvanceHeight**
  Removing 2 local constraints changes 4 normalized assembly rows across 3 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest state_fieldF4_2). The retained draft differs in 4 aligned assembly rows.
  The Sun Orb manager passes the actual effect and its resource to the update. The shared slot layout is 0x4C bytes.

- **081FDCB0 · sub_081FDCB0**
  Removing 3 local constraints changes 38 normalized assembly rows across 13 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_x_2). The retained draft differs in 38 aligned assembly rows.
  The Sun Orb manager and homing callback now agree on the resource and the 0x38 trail record. Isolating the steering stage reduces the ordinary draft to 22 differing rows.

- **081FF454 · HomingProjectile_UpdateDefaultState**
  Removing 4 local constraints changes 58 normalized assembly rows across 22 regions.
  Separate field-address and payload lifetimes through typed owner accessors (closest p_lifetime_0). The detached difference fell from 40 to 38 aligned assembly rows.

- **08202730 · ProjectilePool_Update**
  Removing 10 local constraints changes 44 normalized assembly rows across 18 regions.
  Tested the shared player-cue stepper with the sprite and sequence stored in separate projectile fields. The retained draft differs in 44 aligned assembly rows.
  The uninitialized terrain scratch and two empty assembly operands were removed. The command and three operand constraints still affect register allocation.
  Both flip axes now use the shared command reader. Two operand constraints remain in the sprite and velocity paths.
  The live projectile slot is 0x128 bytes and the copied configuration is 0x5C. The updater, spawner and allocation routines now use the same slot, signed motion and collider layout.

- **0820EE34 · OrangeSpine_ApplyHitReaction**
  The damage handler’s empty outputs are consumed only by empty operands.
  Tested direct reaction and HUD member stores, a separately evaluated spark guard, and named RNG storage through the Orange Spine damage and spark paths. The retained draft differs in 74 aligned assembly rows.

- **0821537C · sub_0821537C**
  Using the SDK DMA copy and counting from the saved queue length improve the loop, but the lifetime of the DMA mode and queue globals still changes the literal loads and saved registers.
  Compared a counted slot loop against a fixed-width queue pointer with typed DMA3 channel writes. The retained draft differs in 10 aligned assembly rows.

- **08217430 · sub_08217430**
  Reversing palette address operands and using a typed bank array or transfer helper change the two source registers for CpuFastSet.
  Compared destination bank arrays, source palette-key indexing, and preparing the bank pointers in either order before CpuFastSet. The retained draft differs in 7 aligned assembly rows.

- **08229440 · Clock_CheckRtcStatus**
  The SDK returns u8. A two-word scratch object reproduced the bytes, but no caller or driver data supports the extra word. The archived version uses the SDK prototype and direct status decoding.
  Compared direct status checks, SDK-style packed status and error nibble records, and independent decoding around Clock_EndRtcAccess. The retained draft differs in 5 aligned assembly rows.

- **08234540 · SpiralBurst_InitParticles**
  The spiral emitter uses a separate mask and two table aliases across its loop.
  Compared captured and direct random-table pointers across the spiral loop, with narrow angle state and alternative slot cursor advancement. The detached difference fell from 81 to 59 aligned assembly rows.
  Taking the signed cosine sample through its table and reading sine directly removes the last empty assembly barrier. The two preexisting RNG table bindings remain.
