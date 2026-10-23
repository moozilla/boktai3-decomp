// MATCHING_CONCESSION: register-pinning
#include "scene_actor_chase.h"
#ifdef NONMATCHING
// fuzzy: 84.2%
// Remaining differences: flag-load scheduling and reset-address reuse.
void Vorn_ClearRouteResumeFlag(void *);
void SceneActorMovement_ResolveRoutePosition(void *);
u8 sub_0812F3CC(void *);
// confidence=low: restores route control and movement after an action completes.
#define Vorn_RestoreRouteMovement sub_080B0038
void Vorn_RestoreRouteMovement(struct SceneActorRegistryResetView *a)
{
    u8 *context = a->context;
    u8 *group = context + 0x3D4;
    u32 *action;
    u8 result;
    asm("" : "+r"(group));
    if (a->field2B1)
        a->field2B1 = 0;
    if ((u8)Callback_InvokeActor(a, a->update270))
        return;
    result = sub_0812F3CC(a);
    if (result)
        return;
    if (a->field2B0) {
        struct SceneActorRegistryControl *control;
        Vorn_ClearRouteResumeFlag(context);
        SceneActorMovement_ResolveRoutePosition(a);
        control = (void *)(group + 0x4C);
        action = &a->flags2CC;
        if (Chase_Bits32(action, 0x200000)) {
            s32 mask;
            u16 *motion;
            u32 *state = &a->flags2D0;
            mask = ~0x1000000;
            *state = mask & *state;
            motion = &a->flags2D4;
            Chase_ClearBits(motion, 0x200);
            {
                register u32 zero asm("r1") = 0;
                asm("" : "+r"(zero));
                control->mode = zero;
            }
            control->counter = result;
            control->timer = result;
        }
        {
            s32 mask = ~0x90;
            *action = mask & *action;
        }
        a->field2C2 = 64;
        Memory_Clear(*(void **)(group + 0x200), 8);
        Chase_SetBehavior(a, 2);
        Chase_SetCtl(a, *(void **)((u8 *)a + 0x22C), 0);
        a->word104 = (u32)a->entryTarget228;
        a->word108 = result;
        a->word10C = result;
        a->word110 = 1;
        a->flags114 = 1;
    } else
        a->counter2AC++;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080B0038);
#endif
