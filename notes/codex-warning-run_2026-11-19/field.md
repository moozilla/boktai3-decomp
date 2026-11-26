# Field objects, hazards, and engine core

Stopped at the wind-down marker after Roaming objects, unit 30 of 80. The reviewed queue files went from 146 baseline warnings to 63 current warnings and zero errors. The current count includes two existing sites exposed by linter fixes. One additional warning was removed in a shared header. All touched sources and affected header dependents passed their stock checks. All 68 commit requests were acknowledged. The next unit is Bouncing object pool (tentative).

| Unit | Warnings before → after | Result |
| --- | --- | --- |
| Game entry point | 1 → 0 | 08001590 clears the typed background task table. The compiler reverses an ordinary forward counted loop, producing the ROM's descending stores and eliminating the explicit zero local. Named Background_ClearSlots to describe the table it writes. |
| Terrain overlays | 1 → 0 | 08003C20 uses the registered scene body and its sprite and keyframe display. A typed body flag query preserves the mask evaluation before the field read. Replaced private overlay views with the shared terrain grid, terrain override, and sequence sprite. Stock unit and all four overlay-header dependents match. All 1,435 shared display/body header dependents match. |
| Scene objects | 12 → 2 source warnings, plus one newly detected site | 080045E8 uses typed queries for the two collider flag words across five hit gates. Direct literal tests reversed the mask and field load. 08004B64 copies the native SceneVec and uses the direct mask for forward jitter. 08005264 reuses the existing object flag reader during its fall and landing. The full unit matches. |
| Terrain modifiers | 1 → 0 | 080061CC calls the shared height conversion directly. Updating the height argument in both sign branches keeps the copy at the join used by the script call. The unit also uses native SceneVec and TerrainCellOverride records. 08005E10 is named TerrainModifiers_ClearGlobal. The stock unit and its header dependent match. |
| Items and containers | 5 → 0 | 080070D0 shares the player-status query. 08007ECC and 080084F4 share the pointer-based action flag query. 080083F0 and 08008820 use forward counted loops that compile to the original backward argument clears. The complete unit matches. |
| Pooled interactables | 4 → 0 | 08008A98 assigns the reset directly. 08008ABC shares the reaction-word query with scene objects. The state script uses its sentinel in place. Native widget, resource, and position layouts replace the pool copies, and 08008C18 is named LightFixture_NullCallback. The unit matches, as do all 146 scene-object header dependents and the pooled-interactable header dependent. |
| Directional interaction controllers | 7 → 0 | 080096A0 uses a typed field query. The three proximity/input checks share the named player slot status table with the item unit. 08009B7C uses direct indexed stores and a counted default clear. Using the actual array rather than a local cursor preserves the loop bound calculation and removes both address tricks. Native position, sprite, and graphics types replace the private views. The stock unit, both slot-status header dependents, and all four controller header dependents match. |
| Collision controllers | 6 → 0 | The receive-hit callback shares both collider flag queries. Idle playback uses its flip literals directly. The hit delay, hit callback, and pause path share the same mode transition. Destruction and initialization reuse a manager pointer setter, and initialization shares the player action flag query. Native tile drawables, sequence state, and the sun clock replace private views. The stock unit and header dependents match. |
| Falling objects | 1 → 0, plus one header warning removed | The falling-to-landing path uses the existing state transition, now with a direct timer reset. The ground lookup uses native terrain cells. Named the manager reset at 0800CBA0. The unit and both header dependents match. |
| Group objects | 11 → 6 | 0801010C uses a random table pointer inside the loop. A plain local matches exactly when its first table-address hoist is disabled diagnostically. The five state changes now share a typed transition. Native position, sprite, sequence, resource and body layouts replace the private views. Named five commands and movement functions. The complete unit and its header dependent match. |
| Animated hazards | 4 → 0 | Native collider, sequence sprite, playback state, render resource, and terrain cells replace private layouts. Writing sprite priority directly matches initialization with plain zero arguments. Assigning the sprite y field directly restores the early zero in both creation paths. The state transition clears its timer directly. Named the empty callback. The stock unit and all four header dependents match. |
| Breakable barriers | 2 → 0 | Masked the two spark samples directly before adding their biases. Named the broken-state update. The task uses a native render resource and collider and allocates its actual manager type. The stock unit and all three header dependents match. |
| Weather and storm controller | 8 → 4 | The shared weather task supplies its phase, cooldown, tilemap state and particle slots. Native particle endpoints and drawable priority remove the shared one local. The existing background display API handles BG1 in active storms. All five source files and header dependents match. |
| Clock events | 3 → 3 | Direct zeros, frame update expressions, and the shared clock-event test all miss the zero live through the clock reads. Direct arming writes form the target loop with the value load three instructions later. The stock unit matches. |
| Tile animation | 2 → 1 | Upload programs the shared volatile DMA3 register fields and uses SDK names for VRAM and the transfer mode. The same SDK copy macro moves the DMA base load after the channel count. Direct initialization stores leave the zero and count loads in reverse order. The complete unit matches. |
| Scene message controller | 3 → 1 | The shared script argument bitfield produces the ROM header mask. Advancing the step uses a direct increment and latch clears. The argument copy still depends on the inner one-pass loop. The stock unit and all three header dependents match. |
| Tether actor | 17 plus one newly detected → 10 | Native position records replace both unions. Assignments directly initialize state, clear the saved kind, and reset descent values. Shake masks and wander samples use their direct forms. Recovered the segment drawable and terrain probe layouts and named three accessors. The stock unit and all 24 header dependents match. |
| Rectangular area effects | 12 → 10 | Typed rectangle coordinates produce the target packed writes. The resource accumulator reads the real action-mode field. Coverage and visibility use native game, link, player, and area-manager layouts. Each changed file matches. |
| Stored-resource station | 3 → 2 | The digit origin uses the same packed position as glyph-pair initialization, producing the target partial clear. Transfer and animation read native station and player fields. All three files match. |
| Directional force controller | 3 → 0 | Both configurations reset the delay directly. The tick uses named turn, half-turn and wrap-mask constants for its fixed-point heading. All three files match. |
| Breakable props | 3 → 3 | The callback reads the native collider reward flag. Its direct mask still reverses the reads. The burst position needs packed word updates and the table pointer needs to stay in the particle loop. Both files and all nine header dependents match. |
| Proximity particle emitter | 7 → 2 | The native drawable fields and typed particle array let the particle defaults use direct values. Both main sprites use direct priorities and the ground lookup uses native cells. Deleted an unreachable source return while preserving the ROM fallthrough. All three files match. |
| Stacked actor | 9 → 4 | Both particle initializers use the named RNG table and direct cursor masks and velocity clears. Idle reads the player mode and flags through the native Player. All three files match. |
| Interaction marker | 4 → 3 | The native terrain lookup and direct sprite resource arguments match. Idle button and player status keep their mask order. The default script-argument loop still needs its early zero. |
| Saved-position marker | 5 → 5 | The position initializer uses shared terrain grid and override records. The existing palette branches, script-argument zero, and idle mask order remain. Both files match. |
| Hit switch | 2 → 1 | Native sprite resource, collider, scene body, position, and terrain registration preserve the switch layout. The completion animation compares and increments its frame directly. The full unit and its header dependent match. |
| Script keyframe pool | 2 → 0 | Visibility fields and the resource procedure use direct values. Allocation uses the actual manager prefix and entry sizes. The complete unit matches. |
| Terrain edge markers | 1 → 0 | Clear the packed coordinates individually as in the script camera readers. This reproduces the masked partial-word store. The source file matches. |
| Script utility references | 3 → 3 | Removing one, two, or three assignment loops changes the local allocation of the engine flags address. Plain setter forms match the ROM instruction shape. The stock unit matches. |
| Roaming objects | 4 → 2 | Both animations derive facing rows from the wrapped heading and its sector. The two impulse masks remain open. All 28 functions match. |

## Concession annotations

- 08004B64, local-union. The recovered SceneVec is also read and written as two packed words. PuzzleBlock_SpawnEdgeParticles copies this same two-word position, and the directional dust stores preserve y and the padding halfword during x/z updates. Typed bitfield-only candidates narrowed the stores and missed.

## Open for a second pass

- 08004B64, backward jitter mask. Inlining both masks lets the second loop pass combine four equal invariants and hoist the mask. Blocking that one move reproduces the target. The private random sample-address macro also changes table reference density. Persistent search: `build/cx/work/dust-search`.
- 08004CE4, height nibble mask. Direct expressions postpone the first mask load until after the reference-height byte load and eliminate one register copy. Typed grid access keeps that same difference.

- 0801010C and 08010298, random table wrappers. Plain table locals hoist out of the loop. Blocking only the table move matches each function. Direct global reads, shared draw helpers, type changes, and a real sampler descriptor miss. Persistent stores are under `build/cx/work/group-convergence-search` and `build/cx/work/group-chase-search`.
- 08010514, four single-pass loops. Direct table assignment preserves instruction shape, but its constant-equivalence lifetime puts it below the cursor mask in allocation. Suppressing the table equivalence or raising its priority is exact diagnostically. Explicit sequential draws and using the same table for the speed sample miss.

- 080177AC, endpoint random-table wrapper. Blocking the first table hoist (`AGBCC_NOMOVE=152` on the plain-pointer scratch) reproduces the ROM. Global indexing, explicit address forms, shared random APIs, and typed endpoints with a plain pointer still miss. Candidate sources are under `build/cx/work/weather`.
- 08017BEC and 08017C8C, storm-active bit. Direct typed writes place the 4096 load after the GameState pointer. 08017C8C's pause mask similarly moves across both flag reads. The original warnings remain open without adding flag helpers.

- ClockEvents_Update and ClockEvents_Initialize. The update uses one zero before the frame store for the wrap and noon latch clears. Removing only the one-pass loop loses that value as well. Direct arming stores move the constant to the end of the loop setup. Counted forward and reverse walks leave this load late or change the loop.

- TileAnimation_Initialize. Direct and chained zero stores compile to the same loop with the count load before the zero. Forward array and alternate countdown loops move the zero or change loop instructions; the original shared zero remains.

- SceneMessages_RunScript, inner argument-copy loop. Removing it preserves instruction shape and rotates four register homes. Raising the output cursor priority (`AGBCC_PRI=out:+1440`) matches diagnostically. Typed command argument sources and ordinary pointer and counted loops miss.

- TetherActor_InitializeSegments and TetherActor_BlendPalette. The direct -4 origin arguments reverse the setup order with the loop count. Direct RGB channel masks change which high registers hold the weight and mask; native channel expressions and bitfields move more code.
- TetherActor_BeginAuxiliaryState and TetherActor_TestTerrainTransition. A plain cell scan and a direct state comparison tie the cell or position against the actor for allocation. A diagnostic priority increase of one matches each. Existing terrain sampling APIs and typed field variants miss.
- TetherActor_StartInteractionEffect. Direct zeros stop the two initialization branches sharing the full attachment tail. A common source tail with each cooldown stored in its branch restores sharing. It still places the zeros after the join, where the ROM has one in each branch before the cooldown store.
- TetherActor_OnChargeContact, TetherActor_HandleFlaggedCell, TetherActor_TestStepPosition, and TetherActor_Tick. Direct masks change the order of the flag and immediate loads. The large cell handler also exchanges pointer homes. No new flag parameter helpers.
- 0804CB4C, bounce random-table proved zero. `AGBCC_NOEQUIV=table` restores the late frame pointer and reload choices on the direct-symbol version. Ordinary paired draws from the neighboring recovery state, true cursor records, and typed frame access still miss. Persistent search: `build/cx/work/tether/bounce-search`.

- Rectangular area coverage at 0804E0A8, 0804E10C, 0804EC00, and 0804EC60. The direct flag puts the mask after the GameState read, and bypassing the byte predicate removes its boolean branches. Native retyping matches with the existing flag locals.
- Rectangular area resource mode at 0804E730, 0804E800, 0804E8D8, and 0804ECD0. Direct mask evaluation loads player flags before 32. Equivalent boolean expressions and repeated typed field reads still miss. The hide-loop literal at 0804E87C similarly moves after its loop setup.
- 0804E2AC, entry enable literal. A direct write ties the count-cursor and manager for allocation. A diagnostic priority increase of 50 on the count matches. The native point is landed while the enable local remains.

- Stored-resource station 0804F0B4 and 0804F140. Writing the status literal directly changes the position of its load relative to the player flag read. Both native type variants preserve the exact code with the original locals.

- Breakable props 080515C0 and 080515DC. The callback's direct mask loads later. Native signed and packed positions shorten the burst's word-preserving coordinate updates. A plain RNG pointer matches diagnostically when its first hoist is blocked (`AGBCC_NOMOVE=353`). Shared address and cursor forms also hoist and miss.

- Proximity emitter 08051B70 and 08051EA8. A direct signed comparison changes the live pointer that supplies the out-of-circle return. Explicit returns do not reproduce those registers. Direct palette-blend assignments build a second 1 after the nearby store; reading back the field and sharing a boolean tail also miss.

- Stacked actor. Direct five-bit sample masks move the shared mask and rotate several coordinate registers. The falling form retains the reported mask. Each of the three direct idle bit tests independently changes the relative order of the flag and mask loads.

- Interaction marker. Each direct idle flag test reverses one load. The direct argument clear matches loop shape and changes the load order at setup, while the forward array loop also changes the traversal.

- Saved-position marker. Both direct idle masks move one load apiece. The direct zero moves across the clear-loop setup. Direct palette writes change branch merging and constant placement even with native terrain and palette fields.

- HitSwitch_ReceiveCollision. The response bit and the cooldown both use 32. Writing each value where it is used moves one load, including with native collider and cooldown fields. The shared local remains open.

- ScriptUtility_SetFlag800. With a direct update, the engine flags address receives r3 and the two work registers rotate. `AGBCC_FORCE=23:1` gives an exact diagnostic build. Direct field, cached word, pointer, and fewer-loop forms all miss.

- RoamingObject_ApplyImpulse and RoamingObjectB_ApplyImpulse. Writing the clear mask directly moves the listener flags load ahead of the mask construction in both variants. The animation sector rewrite is exact in both, while a byte heading and default-row branch select miss.

## PARAMETER HELPERS

- `SceneBody_TestFlags` has 1 caller and 1 call site. Needed for the match because the direct test changes the mask and field-read order.
- `SceneCollider_ReadContactFlags` has 2 callers and 4 call sites. Needed for the match because the direct tests change mask evaluation, even though both callers read the same collider word.
- `SceneCollider_ReadReactionFlags` has 3 callers and 5 call sites. Needed for the match because the direct tests change mask evaluation, even though all callers read the same collider word.
- `SceneObject_ReadFlags` has 2 callers and 4 call sites. Needed for the match. This extends the pre-existing `flags16` accessor into the landing and falling path.
- `PlayerSlot_ReadStatus` has 5 callers and 5 call sites. Real shared player-slot status lookup across proximity, input, and script gates, with the slot-table lookup inside the accessor.
- `Player_ReadStatusBits` has 3 callers and 3 call sites. Needed for the match. The same action flag is tested in each caller, but the helper only masks a word through a pointer.
- `Directional_ReadPlayerStatus` has 1 caller and 1 call site. Needed for the match because the direct expression changes the order of the loads.
- `CollisionController_SetMode` has 3 callers and 4 call sites. Real state transition that writes the mode, clears the timer, and sets the entry latch.
- `CollisionController_SetManager` has 2 callers and 2 call sites. Needed for the match because the direct null clear uses a different evaluation order.
- `BoardPiece_SetState` has 5 callers and 5 call sites. Real state transition that advances the piece state, enables the slot, and clears the timer.
- `FallingObject_SetState` has 3 callers and 4 call sites. Real pre-existing state transition now reused for landing and implemented with a direct timer clear.
- `heightLevel` has 2 callers and 2 call sites. Real pre-existing signed terrain-height conversion applied to position input in the create and change paths.

## Tool changes

- The linter reports statement expressions whose macro body only assigns a temporary inside a one-pass loop and returns it. The static scan found one added report in the tree, DUST_SAMPLE_ADDRESS. The full linter test suite passed (37 tests).

- Fixed scope hoist probes that repeated an initializer outside its nested block. Five hoist tests passed, and the convergence probe now compiles in the actual inner block.

- The linter now recognizes ordinary `register` storage on constant declarations. The static comparison found six newly visible existing sites, including the tether interaction-effect zero. All 38 linter tests passed.

- Land now reports when the old source cannot compile against a changed header. That situation in tether previously reported the existing bounce warning as newly added. The actual before/after source each has the same proved-zero site. All 22 landing tests passed.
