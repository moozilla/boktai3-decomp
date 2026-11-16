#include "scene_actor_chase.h"
#include "debug.h"
void Vorn_ClearRouteResumeFlag(void *);
void SceneActorMovement_ResolveRoutePosition(void *);
u8 sub_0812F3CC(void *);
static inline u32 *Route_SetCtl(struct ChaseActor *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    u32 *field;
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = callback;
    actor->base.field2C8 = 0;
    field = &actor->base.pendingControlFlags;
    {
        u32 mask = ~1;
        *field &= mask;
    }
    actor->base.pendingControl = 0;
    return field;
}
static inline void Route_RestoreMotion(struct ChaseActor *actor, u32 result, u32 *flags)
{
    /* Restore the saved target as its stored 32-bit control value. */
    actor->base.pendingControl = *(u32 *)&actor->base.onAlternateEntry;
    actor->base.pendingControlData = result;
    actor->base.pendingControlRelease = result;
    actor->base.pendingControlKind = 1;
    *flags = 1;
}
// confidence=low: restores route control and movement after an action completes.
#define Vorn_RestoreRouteMovement sub_080B0038
void Vorn_RestoreRouteMovement(struct ChaseActor *a)
{
    u8 *context = a->base.context;
    u8 *group = context + 0x3D4;
    u32 *action;
    u32 *flags;
    if (Chase_TakeArrived(a)) {
        DEBUG_PRINT();
    }
    if ((u8)Callback_InvokeActor(a, a->base.contactQuery.opaque))
        return;
    if (sub_0812F3CC(a))
        return;
    if (a->base.stepDone) {
        struct SceneActorRegistryControl *control;
        u32 actionMask;
        Vorn_ClearRouteResumeFlag(context);
        SceneActorMovement_ResolveRoutePosition(a);
        control = (void *)(group + 0x4C);
        actionMask = 0x200000;
        action = &a->base.behaviorFlags;
        if (Chase_Bits32(action, actionMask)) {
            s32 mask;
            u32 *state = &a->base.statusFlags;
            mask = ~0x1000000;
            *state = mask & *state;
            {
                struct ChaseFlags40 *motion = (void *)&a->base.motionFlags;
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
        a->base.field2C2 = 64;
        Memory_Clear(*(void **)(group + 0x200), 8);
        Chase_SetBehavior(a, 2);
        flags = Route_SetCtl(a, *(void **)((u8 *)a + 0x22C), 0);
        Route_RestoreMotion(a, 0, flags);
    } else
        a->base.counter2AC++;
}
