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

