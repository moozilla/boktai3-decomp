# Scene actors and NPCs, second half

Queue baseline is 657 warnings across 37 units. The run stopped after 25 reviewed units, at 205 warnings from 417 in those units. That removes 212 reported warnings, or 32.3% of the full queue. The count includes two linter false positives and the earlier parameter-helper matches listed below. All changed units and checked header dependents matched.

## Reviewed units

- Scene actor visibility and stored motion (08084280), 17 → 0 warnings, 0 errors. Shared drawable helpers preserve the sprite and tile paths and early mask load. The room actor fields, terrain control installer, and SDK-backed ZeroFill8 replace private views and entry code. The three movement handlers share typed stored-point approach.
- Phase object (080F2700), 12 → 5 warnings, 0 errors. Seven files share the recovered phase actor layout and all seven stock checks pass. The common mode setter clears five warnings. The indexed Ivy entry loop and the existing movement-curve random-byte helper clear one each.

- Child group (080F33BC), 14 → 0 warnings, 0 errors. The parent mode and entry latch, sword activation, collider arrays, and HUD offset use their shared types and helpers. One C89 index across the three trailing traversals gives the ROM allocation without a one-pass loop. An indexed palette loop clears the palette warning. All eight PhaseObject header dependents match.

- Actor phase (080F8010), 7 → 0 warnings, 0 errors. The shared control installer replaces the transition IDs. Axe_InitEffectPalettes identifies the black and copied palettes, and auxiliary collider and trail offsets establish the Axe layout. The placement setter also matches in the earlier stored-motion unit. All 107 existing header dependents passed.

- Actor recoil (08116280), 2 → 0 warnings, 0 errors. The shared ActorStats, SpatialEntry and scene-pool body replace private views. A mask argument in the damage query and a byte flag setter preserve the original ordering. Both recoil callbacks use the same polar velocity helper. Stock unit MATCH.

- Actor particles (0811BCBC), 2 → 0 warnings, 0 errors. The raw terrain height and the rendering target now have distinct lifetimes. That preserves the stock allocation without either single-pass loop. The routine uses the shared stats, model parts, and camera. Stock unit MATCH.

- Actor ability (0811F474), 1 → 0 warnings, 0 errors. Both palette-start handlers test the cooldown in place and supply the literal initial fade mode. Stock unit MATCH.

- Registry A8CB (0811FAB8), 15 → 0 warnings, 0 errors. The motion and pause helpers follow the sibling callbacks. Ordinary indexed loops match both manager traversals. The command dispatcher uses the shared registry actor, scene receiver, and camera layouts. All six stock unit checks match. Actor recoil also adopts the shared byte flag setter.

- Scene actor 14 (08121B98), 5 → 2 warnings, 0 errors. Playback and destruction share the typed task, sprite, and slot layout. Direct playback and spread-mask literals match. All three stock units and both new header dependents match.

- Actor mode (0812DA2C), 1 → 0 warnings, 0 errors. Response initialization assigns the five fields directly. Stock unit MATCH.

- Scene actor behavior (0812E7EC), 51 → 1 reported warnings, 0 errors. State transitions, controller installs, the receiver, and route handlers now share their types and operations. One original report was a false positive in a file-scope payload union. All 22 stock checks passed again at coordinator review. The parameter helper audit below distinguishes source recovery from warning movement.

- Scene actor reaction (08131D5C), 87 → 61 warnings, 0 errors. The stock checks passed for all 11 source files. The shared state/control installs and existing ZeroFill8 clear the transition and damage-offset warnings. The defeat handlers use the canonical collider and group fields. No new parameter helpers.

- Scene actor placement (081369C8), 18 → 5 warnings, 0 errors. Shared state and control entry clear both reset paths. Show and hide share the real tile widget and room actor collider, and the inactive restore uses the existing placement availability predicate. All six stock checks match.

- Script actor (0815A400), 1 → 0 warnings, 0 errors. Direct writes clear the scene mode, active state, and item field. Stock unit MATCH.

- Actor impact (08179B9C), 6 → 4 warnings, 0 errors. The shared typed coordinate pointer lets both probes clear the vertical component directly. All three collider constructors write native packed coordinates. The stock unit and the header dependent match.

- Actor position (0817A0FC), 9 → 7 reported warnings, 0 errors. The quarter-turn probe uses ContactPosition and clears its vertical offset directly. The bounds search uses its shared type and has a descriptive symbol. Its file-scope union was the other baseline false positive corrected by the scope-aware linter. Both changed stock units and all three header dependents match.

- Actor state (0817B5C0), 4 → 4 warnings, 0 errors. The existing source passes its stock check. The facing loop and depleted-state literals remain open. The later decision pass consolidated entry and restart handling and rechecked this unit.

- Actor decision (0817BB54), 11 → 10 warnings, 0 errors. The hazard flag matches as a literal. The decision and state units share multi-field entry consumption and scene restart. Both source units and both header dependents match.

- Scene event actor (0817E378), 30 → 20 warnings, 0 errors. Seven functions share sequence step entry, which resets the sequence timer with the step. Timed animation and charge reuse direction and palette setup. The stock unit and header dependent match.

- Actor palette (08181634), 3 → 0 warnings, 0 errors. Six paths share palette startup, and the restore-delay callback clears its pointers directly. The stock unit and header dependent match.

- Actor controller (08182F60), 59 → 42 warnings, 0 errors. The defensive and paired collider profiles match the main motion and transition files. The leaf callbacks use the actual actor, collider, motion, and saved-position types. Typed behavior reset, velocity zero, and a direct disable bit match. All 15 stock checks and all four header dependents match.

- Actor status (081AFA28), 44 → 36 warnings, 0 errors. The shared paired half and attack pair replace the private status, capture, approach, rise, and flanking overlays. Six callbacks share phase initialization, and five step or latch values match directly. All 12 stock checks and 27 header dependents match.

- Paired scene actor (081B61E0), 5 → 5 warnings, 0 errors. Both component queries now use the typed half action flags at 1F8 and 414. The two state application callbacks keep the matched action gate. All four stock checks MATCH. Commit requests 0124–0125.

- Scene actor sequence (081C069C), 7 → 2 warnings, 0 errors. Both scroll pairs use shared clamped fixed-point interpolation. The animation callback uses the recovered layout and the initializer uses the SDK zero fill. All 21 functions and the header dependent MATCH. Commit requests 0126–0127.

- Contact impact effect (081C3010), 6 → 1 warnings, 0 errors. The shared clamped fixed-point operation covers both position and scale ramps. Motion entry resets the timer and result with the callback, and both constructors use the SDK position fill. All 25 functions and both header dependents MATCH. Commit request 0128.

## Open source decisions

- 080F2858, target alignment. The turn completion stores a shared stop value into the halfword timer and byte speed. Literal and chained stores sink the zero into the loop exit and change the turn-count allocation. A loop-level helper preserves the shared zero, but changes the speed-address lifetime across the earlier sound call. Replay in build/cx/search/080F2858.
- 080F3230, entry convergence. An indexed flags access and direct opacity/visibility literals leave two differing instruction placements at the loop entrance. The shared mode setter currently merges three addresses that the ROM keeps live separately through that loop. Replay in build/cx/search/080F3230. The existing exact forms still carry all four warnings.
- 080F2C34 still assigns a fixed transition in each iteration. Moving it outside changes the loop invariant order. This steering is outside the initial warning inventory.

- 08121E90, sprite teardown. A direct null assignment aliases the return zero in the same register. The ROM assigns the pointer using a separate register before loading the return zero. Inline publication matches, but nearby reset sources use direct stores. Kept the original exact reset.
- 08122338, shard trajectory. The ROM keeps the random table pointer live across three divides and loads the cursor mask for each draw. Direct and shared draw helpers keep the mask live and rematerialize the table instead. Kept the original cursor setup and cleared the two spread masks.

- 0812F3CC, contact filtering. Direct and shared mask tests let the path reuse its zero when setting up the successful contact, changing the register allocation and omitting the ROM’s separate zero load. The original exact mask local remains.

- 08131D5C–08132904, damage reactions. The ROM keeps the flash timer value and behavior flag load in the order established by the exact locals. Direct stores and a single shared palette-flash operation both change that order. Similar direct clears in the defeat path move the null or timer zero past the other field write.
- 08132D2C, 0813599C and 08135D74, defeat and reaction gates. Direct masks shift address and constant loads, even with the recovered group, collider, and control fields. The original mask and timer locals remain.
- 08135F70 and 081363F4, damage resolution. An ordinary halfword clear changes the held mask and flag lifetimes. Direct visibility and priority values alter the HUD branch and its output-field allocation. The five local unions remain open with the other literal and flag warnings.

- 08136C80–08136EB0, placement visibility. Reading the relevant bit directly changes its lifetime through the drawable, shadow, and collider updates. In the show and inactive-restore paths the ROM carries the successful clear-bit result through the callback to the collider timer. The typed paths still match with the five original locals.

- 08179B9C–0817A0FC, impact contact and terrain probes. The direct light bit test moves the mask after the source load. Literal probe distances simplify the multiply and move another constant. Rewriting the angle as an eight-bit heading changes the mask lifetime. Four exact warnings remain.

- 0817A71C and 0817A7C8, random bursts and reaction start. The nearest plain random draw was shape 4 because it hoisted the address before both burst iterations. The stock loop loads it after the packed size stores. The reaction constants also move before or after the first destination address in the direct expressions.
- Actor quarter-turn search. As in the impact probe, the direct heading mask moves one instruction and the literal distance folds the multiply. Exact source retains both locals.

- 0817B5E4 and 0817B694. Removing the one-pass loop produces the same facing operations but ties each AND to the shifted value’s register, while the ROM ties it to the mask register. A shared cardinal calculation and typed accumulators also differ. The direct state and depleted-flag stores move their constants across the destination loads.

- 0817BB54 onward, decision phase writes. The direct phase values cross the destination-address setup and in two cases change the common branch tail. The exact phase locals remain.

- 0817E378 onward, scene event flags and lunge. The direct mask stores change the order of the immediate and field-address loads. The lunge’s direct radius-squared test also changes its live range across the heading and terrain queries. The twenty exact warnings remain.

- 08182F60–08184F04, controller leaves. Native collider types match through the two-step accesses. A single actor-group field expression merges the effective offsets and changes address setup. The raw leaves also use different inactive-bit scheduling from the shared upper profiles. The remaining 42 warnings are mostly motion/collider masks and phase timer zeros.

- 081AFA28–081B1B90. Open action flag masks, phase latches, and motion vectors still differ when written directly. In the presentation callback, the byte cast selects shifts instead of the direction mask, and the expanded ready-state reset changes the control-flow tail. The elapsed status field still needs confirmation across its owner.

- 081B61E0, Paired scene actor. The typed queries still keep the bit in the register that the direct mask uses for the loaded flags. Expanding the application guard changes address setup and the state-update tail. Five exact locals remain.

- 081C069C, Scene actor sequence. The named waypoint field assignments coalesce the first two halfword stores into words. The direct display expression changes the BG shadow load and mask order. The exact endpoint Y and enabled-layer locals remain.

- 081C3010, Contact impact effect. Using the endpoint frame directly changes the high-end timer comparison in the shrinking callback. The eight-frame duration local remains.

## Tools

- Scope now suggests an annotated moved instruction when one is available and points to the disassembly otherwise. The target-alignment draft had exposed an invalid `why None` suggestion. Three focused tests pass. Commit request 0010.

- The concession linter now restricts local-union reports to function scope. The command payload exposed a false positive. All 37 linter tests and both libgcc-number header dependents passed. Commit request 0034.

## Pending at STOP

- The coordinator stopped the run at 01:15 PST. The State actor kind 086121C4 unit (081C66F8) is untouched with 39 warnings. Its first function, 081C6710, still matches. The next 11 units are pending. No priority files are listed in the Scene 2 queue.


## PARAMETER HELPERS

Counts are distinct direct caller functions, with call-site counts where they differ. These are the added or reshaped inlines through the coordinator review, followed by meaningful shared operations added after it.

- `ActorStats_SetDamageFlags` — 2 callers. **Needed for the match.** Generic byte OR of a supplied mask.
- `PhaseObject_SetFlags` — 2 callers. **Needed for the match.** Mask-first byte OR under the group transition.
- `PhaseObject_EnterMode` — 6 callers, 7 sites. **Real operation.** Publishes the phase group mode, enables it, and requests entry.
- `PhaseObject_TakeEntry` — 7 callers. **Real operation.** Tests and consumes a group entry latch and clears the entry reset.
- `RegistryActor_StartMotion` — 4 callers, 5 sites. **Real operation.** Installs the motion callback and resets the input latch and fall timer.
- `RegistryActor_SetMotionFlags` — 2 callers. **Needed for the match.** Generic OR of the command-motion bit.
- `RegistryActor_MessagePauseBits` — 2 callers. **Needed for the match.** The combined pause check has domain meaning, but the mask parameter was introduced to match.
- `SceneActorBehavior_GlobalPauseBits` — 3 callers. **Needed for the match.** The three dispatchers test the same pause bit, and the mask plumbing preserved the stock output.
- `SceneActorBehavior_OverrideBits` — 3 callers. **Needed for the match.** Generic word-mask query; its pointer parameter also avoided a conflicting global declaration.
- `SceneActorBehavior_MotionBits` — 3 callers. **Needed for the match.** Mask-first copy of an existing chase motion query.
- `SceneActorBehavior_WriteSelector` — 3 callers. **Needed for the match.** Generic byte store of the supplied selector.
- `SceneActorBehavior_SetWordFlags` — 2 callers. **Needed for the match.** Generic word OR covering two unrelated flag fields.
- `SceneActorPlacement_SetFlags` — 4 callers. **Needed for the match.** The mask-first OR preserves the placement-latch update's load order.
- `ActorPhase_GetDrawable` — 3 callers, 23 sites. **Real operation.** Gets the common drawable from the actor phase's typed view.
- `Axe_SetEffectFlags` — 1 caller, 2 sites. **Needed for the match.** Generic OR in the collapse handler.
- `clearCollision` — 1 caller, 2 sites. **Needed for the match.** Single-function collision-mask wrapper.
- `Recoil_TestDamageFlags` — 1 caller. **Needed for the match.** Tests and exports the byte pointer in the exact recoil schedule.
- `ActorRecoil_IsDamageCategory` — 1 caller, 4 sites. **Needed for the match.** The equality wrapper keeps the existing call expansion.
- `ActorRecoil_ScaleVelocity` — 1 caller, 2 sites. **Real operation.** Truncates a signed fixed-point velocity component toward zero.
- `ActorRecoil_SetPolarVelocity` — 2 callers. **Real operation.** Projects a knockback angle and speed into both planar components.
- `ActorRecoil_GetStats` — 1 caller, 11 sites. **Real operation.** Gets the embedded stats with the shared type; no constant parameter.
- `SwordChild_Activate` — 1 caller. **Needed for the match.** The byte setter's value parameter preserves the activation write.
- `ChildGroup_Jitter` — 1 caller, 3 sites. **Real operation.** Applies the same bounded random jitter to three axes.
- `RegistryActor_TestMotionFlags` — 1 caller. **Needed for the match.** Generic command-motion mask test.
- `SetAttachmentFlags` — 1 caller. **Needed for the match.** Generic descriptor flag OR with the descriptor evaluated first.
- `SceneActorControl_SetDrawableFlags` — 1 caller. **Needed for the match.** Selects the typed drawable, but takes a mask to preserve this handler's schedule.
- `RegistryActor_TestDamageFlags` — 1 caller. **Needed for the match.** Generic mask test local to the knockback path.
- `RegistryActor_SetUpdateFlags` — 4 callers. **Needed for the match.** All four mark the same fall update; the mask-taking OR is a compiler accommodation.
- `EnableMotionFlags` — 1 caller. **Needed for the match.** One mask-first word OR in stored motion.
- `ActorParticles_NextRandom` — 3 callers, 13 sites. **Real operation.** Advances the shared sample cursor and reads its table entry; no constant parameter.
- `ShowDrawableAndShadow` — 1 caller, 2 sites. **Real operation.** Restores both render paths together; no constant parameter.

- `SceneEventActor_EnterSequenceStep` — 7 callers, 22 sites. **Real operation.** Selects the event sequence step and resets its elapsed frame counter. The same signature serves recoil, removal, lunge, summon, delayed finish, charge, and song stop.

- `ArmorPalette_BeginEffect` — 6 callers. **Real operation.** Installs an update callback and optional completion callback and starts the effect timer. The same signature serves fast and slow white fades, accent fade, transitions, and both restore delays.

- `ActorController_SelectDefenseColliders` — 8 callers. **Real operation.** Activates colliders 1–3 and leaves the attack collider inactive across the motion and animation transitions.
- `ActorController_SelectPairedColliders` — 3 callers. **Real operation.** Selects colliders 1 and 3 for the heavy, fixed, and layered transitions.

- `PairedSceneHalf_BeginActionPhase` — 7 callers, 9 sites. **Real operation.** Selects the phase, resets its timer, and latches entry for the capture, orbit, approach, and strike state machines.

- `Fixed8_Interpolate` — 6 callers, 11 sites. **Real operation.** Clamps an interpolation interval and uses eight fractional bits with signed truncation. Both scroll callbacks and four contact movement or scale callbacks use the same signature.
- `ContactImpactEffect_EnterUpdate` — 7 callers, 7 sites. **Real operation.** Installs a contact effect callback and clears its timer and result together, including explicit cancellation.

No exemptions.
