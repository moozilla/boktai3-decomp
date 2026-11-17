#include "global.h"
#include "scene_actor_control_phase.h"
u8 sub_0812F3CC(struct ChaseActor *);
// confidence=low: restores saved control, then enters route behavior two after sixty ticks.
#define SceneActorRoute_UpdateDelayedEntry sub_08131A2C
static inline void BeginControlZero(struct ChaseActor *actor, void *control, u32 kind)
{
    u32 offset;
    {
        u8 *field = &actor->base.field2B2;
        *field = 1;
    }
    offset = 0x2B0;
    *((u8 *)actor + offset) = 0u;
    actor->base.controlKind = kind;
    offset -= 0x94;
    *(void **)((u8 *)actor + offset) = control;
    actor->base.field2C8 = 0u;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0u;
}
static inline void State(struct ChaseActor *a, u32 behavior, u32 zero)
{
    a->base.state2A8 = zero;
    a->base.phase2A9 = zero;
    a->base.behavior2AA = behavior;
    a->base.counter2AC = zero;
    a->base.field2B1 = 1;
}
void SceneActorRoute_UpdateDelayedEntry(struct ChaseActor *actor)
{
    u8 result;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1))
        BeginControlZero(actor, actor->base.onStanding, 0);
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    result = sub_0812F3CC(actor);
    if (result)
        return;
    if ((s32)actor->base.counter2AC > 59)
        State(actor, 2, result);
    else
        actor->base.counter2AC++;
}
