# depin-manual-2 worklog (2026-10-21)

Worker branch cloud/depin-manual-2. List: wip/depin_list2.tsv (60 functions).
Method: depin.py --write over all files (only LinkMenu_WaitSelectionMode lost a hint), then a pipeline that strips every hint into wip/depin2/ and runs tools/permute.py on the unpinned copy (150 s, -j 2). A permuter score of 0 is NOT proof of a match (it ignores branch offsets, and one result reassigned a parameter); always port the change and run check.py.

## Fully clean (no pins, no asm)

* SceneActorRecord_Tick (0806FA14): Enabled() written as a u8-returning `if (...) return TRUE; return FALSE;` helper replaces the result pin; the r5 pin was replaced (permuter) by holding the address as an int temp, `location = 0x02000710;`, dereferenced as `*(struct GameState **)location` in the loop.
* SceneNpc_UpdateBlinkPhase (081CF490): permuter; dropped the `phase` local and the volatile read; reads npc->phase everywhere and the compare reads `*((u8 *)npc + 0x18)`.
* LargeEffectPool_AllocateSlot (0812B084): second use of i in the mask shift via `(*(&i))`.
* SceneActorControl_EnterTimedState (0813042C): permuter; masked value split over u32/u16 temporaries, `if ((bit = zero) == 0) Enter(*(&actor), zero)`.
* ActorSoundPhase_UpdateConditionalStop (081A8820): `do { Stop(q+..); q->st = v; } while (0);` around the uses of the parameter copy.
* Camera_SetIsometricOffset (082261B8): `b = (lo >> 16) | hi; *(u32 *)&in = b;` (store through the s32 temp).
* Hresvelgr_UpdateMotionPhase (0819FADC): two temps for the parameter, second read through `*(&actor)`.
* SelectionPanel_CommitSelection (0810E218): `(*(*(&column)))`, constant 2 in an unsigned temp, EWRAM table by literal `(const u16 *)0x0203B400` (the symbol does not match), store through `*(&context->selection)`.
* FloorSwitch_Release (081004F8): the store/script/palette sequence in `do { } while (0)`.
* SceneSteeringActor_DestroyPool (0806CE24): extra u32 temp assigned `(shared = (u32)pool)` inside the Sprite_Remove argument and later `shared = stride`.
* SceneHud_RemoveAnimations (08169738): second loop uses its own pointer temp.
* SceneHud_CanOpenFromStart (08176E20): the `asm("" ::: "r4")` clobber was only keeping the compiler from deriving 0x45f from 0x418; splitting the last condition into `if (flags & mask) return 0; if (mode != 1) return 1;` does it.
* PairSlotPool_Update (08125A94), CallbackPool_UpdateEntries (08104D6C): clobber hints replaced by an early `if (!mask) return 0;` before the loop instead of wrapping it in the if.
* DirectionalActor_TryBeginState (08184FE8): two early returns instead of `goto begin` + volatile cast + clobber.
* ScenePool_SelectRepeatedDirection (0803D78C), ScenePool_SelectInitialDirection (0803D680): Memory_Clear called through `static inline void ClearHistory(int size, u32 *p)` (size first, found by permuter) and a literal `state = 1`; replaces both r0 pins.
* AnimatedBodyPool_Destroy (0812286C): `bodies` declared inside the loop body.

## Fewer hints

* LinkMenu_WaitSelectionMode (0810A540): 3 -> 2 (depin.py auto).


## Not solved (blockers, 60-function list; 19 clean + 1 fewer hints)

Permuter score 0 is not proof: SceneActorRegistry_CountAvailable scored 0 but differs by one branch offset (unpinned code threads `inactive == 1` past the `cmp r0,#0`; the +r barrier on inactive keeps the compare). Permuter "dummy" mutations (`if (a || b) {} else {...}`, reassigning a parameter) are not usable.

* ContactCollider_GetImpactDirection (08238520): the pin only stops cross-jumping of the two identical `+0xe0` tails; no plain-C form keeps both copies.
* SceneObject_BeginLowering / AnimateActive / AnimateInactive (0801F3CC, 0801EFDC, 0801EF60): the memory barriers keep two flag read-modify-writes separate and the `*(s+0x2c)` reloaded; without them the |=2 and |=3 merge. Best permuter score 15, only via a semantically wrong mutation.
* Interp_Step (08222358): same barrier effect (re-read of len/t); best 55.
* SceneActors_RunFirstEventScript / Second (08021AE8/B54): only `add r4, sp, #0x20` is scheduled late (7 bytes); a pointer copy of &call costs an extra register.
* SceneActorRegistry_CountAvailable (0806FE18): see above, 1 halfword.
* SpriteNotice_TickManager (08123044): the 0x920 constant is folded into base+imm unless pinned; best permuter 5 (invalid pointer type mix).
* PaletteTransition_CopyBanks (081D4024), ArmorPalette_CopyKindBanks (08181634): r5/r6 swap of c and the bank pointer; do-while(0) wrappers, copies and swapped-argument wrappers did not move it (permuter 205 / 20).
* PursuingActors_ReleaseAll (0812C020): original keeps i*stride in r4 and reuses it in the inner loop; none of four spellings reproduce.
* EncodedRecord_XorRange (08044D6C): original has index as a distinct copy of start (r4 vs r5); coalescing cannot be undone without the asm clobber. Permuter 260.
* ScenePool_CreateEntryFromScript (0803D61C): needs the re-copy `adds r0, r5, #0` before the call; (*(&x)), copies, and saved temps all failed.
* Player_TestTetherActionInput (08156F54): g needs to be a copy of the global's address in r3 (permuter 30 only by dummy mutation).
* Actor_InitDrawable (0802D760): m -> r1 vs r0 only; permuter 10 via long long type games.
* FloorSwitch_SetOccupant (0810048C), ResultTutorial_AnimateLevel (0801DB5C, 2 bytes of register), SceneActorControl_StartStoredCurve (080849DC, permuter 50), Slime_RollMoveFlag (70), TextWindow_Destroy (75), LevelAdvanceDisplay_UpdateParticle (70), SceneHud_GetActionIconPosition (40): near misses, unsolved in 5 minutes each.
* Untouched beyond the first 150 s permuter pass (scores in parentheses): AreaEffect_ShowEnabledSprites (320), CallbackSlots_DispatchActive (210), MessageWindow_Clear (200), ProximityEmitter_TickRise (205), SceneActorControl_BeginSavedHeadingPhase (250), SceneActorRegistry_TestCategoryFlags (400), SceneInventory_ClassifyItemAction (205) / ClassifyItemMove, SceneParticlePool_Update (230), SlotPool_UpdateActiveSlots (120), TextState_MeasureTextColumns (220), TransientSpriteEffects_Initialize (100), Wireless_StartChildSearch (245), WirelessGameDisplay_UpdateStatusIcons (395), sub_0822216C (1400), sub_08072204 (200 invalid), PlayerAnimation_SelectSecondaryKeyframes.

## New techniques

* Clobber hints (`asm("" ::: "rN")`) and wrapping the body in `if (x) { loop }`: an early `if (!x) return 0;` before the loop (or splitting a compound final condition into two early returns) changes the register/constant choice the same way (PairSlotPool_Update, CallbackPool_UpdateEntries, SceneHud_CanOpenFromStart, DirectionalActor_TryBeginState; the latter also removed an old volatile cast).
* Swapping the argument order through a `static inline` wrapper (`void Wrap(int size, u32 *p) { Fn(p, size); }`) changes the order the argument expressions are evaluated and which temporary gets the low register (ScenePool_Select*Direction, MotionPool_InitializeSlot). Replaces r0 pins on the argument.
* Declaring a pointer temp inside the loop body instead of hoisting it (AnimatedBodyPool_Destroy) removes +r barriers on three loop-carried pointers.
* An int temp holding a global's literal address (`location = 0x02000710; *(T **)location`) acts like a register pin for the address (SceneActorRecord_Tick); same for a literal EWRAM table address in SelectionPanel_CommitSelection.
* Tooling: strip all hints into wip/depin2/ and run tools/permute.py there (works for any path) with `--run 150`; port a score-0 result by hand and always re-run check.py, since the permuter score ignores branch offsets.
