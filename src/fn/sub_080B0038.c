// MATCHING_CONCESSION: register-pinning
#include "scene_actor_chase.h"
void Vorn_ClearRouteResumeFlag(void *);
void SceneActorMovement_ResolveRoutePosition(void *);
u8 sub_0812F3CC(void *);
static inline u32 *Route_SetCtl(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    u32 *field;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    field = &actor->flags114;
    {
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
    return field;
}
static inline void Route_RestoreMotion(struct SceneActorRegistryResetView *actor, u32 result,
                                       u32 *flags)
{
    /* Restore the saved target as its stored 32-bit control value. */
    actor->word104 = *(u32 *)&actor->entryTarget228;
    actor->word108 = result;
    actor->word10C = result;
    actor->word110 = 1;
    *flags = 1;
}
// confidence=low: restores route control and movement after an action completes.
#define Vorn_RestoreRouteMovement sub_080B0038
void Vorn_RestoreRouteMovement(struct SceneActorRegistryResetView *a)
{
    u8 *context = a->context;
    u8 *group = context + 0x3D4;
    u32 *action;
    u8 result;
    u32 *flags;
    asm("" : "+r"(group));
    if (a->field2B1)
        a->field2B1 = 0;
    if ((u8)Callback_InvokeActor(a, a->contactQuery.opaque))
        return;
    result = sub_0812F3CC(a);
    if (result)
        return;
    if (a->field2B0) {
        struct SceneActorRegistryControl *control;
        u32 actionMask;
        Vorn_ClearRouteResumeFlag(context);
        SceneActorMovement_ResolveRoutePosition(a);
        control = (void *)(group + 0x4C);
        actionMask = 0x200000;
        action = &a->flags2CC;
        if (Chase_Bits32(action, actionMask)) {
            s32 mask;
            u16 *motion;
            u32 *state = &a->flags2D0;
            mask = ~0x1000000;
            *state = mask & *state;
            motion = &a->flags2D4;
            {
                u32 motionMask = ~0x200;
                motionMask &= *motion;
                {
                    register u32 zero asm("r1") = 0;
                    *motion = motionMask;
                    control->mode = zero;
                }
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
        flags = Route_SetCtl(a, *(void **)((u8 *)a + 0x22C), 0);
        Route_RestoreMotion(a, result, flags);
    } else
        a->counter2AC++;
}
