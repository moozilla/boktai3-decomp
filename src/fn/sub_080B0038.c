#include "scene_actor_chase.h"
#include "debug.h"
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
    u32 *flags;
    if (Chase_TakeArrived(a)) {
        DEBUG_PRINT();
    }
    if ((u8)Callback_InvokeActor(a, a->contactQuery.opaque))
        return;
    if (sub_0812F3CC(a))
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
            u32 *state = &a->flags2D0;
            mask = ~0x1000000;
            *state = mask & *state;
            {
                struct ChaseFlags40 *motion = (void *)&a->flags2D4;
                s32 motionMask = ~0x200;
                motion->bits &= motionMask;
            }
            control->mode = 0;
            control->counter = 0;
            control->timer = 0;
        }
        {
            s32 mask = ~0x90;
            *action = mask & *action;
        }
        a->field2C2 = 64;
        Memory_Clear(*(void **)(group + 0x200), 8);
        Chase_SetBehavior(a, 2);
        flags = Route_SetCtl(a, *(void **)((u8 *)a + 0x22C), 0);
        Route_RestoreMotion(a, 0, flags);
    } else
        a->counter2AC++;
}
