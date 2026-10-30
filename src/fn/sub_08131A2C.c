#include "global.h"
#include "scene_actor_control_phase.h"
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
// confidence=low: restores saved control, then enters route behavior two after sixty ticks.
#define SceneActorRoute_UpdateDelayedEntry sub_08131A2C
static inline void BeginControlZero(struct SceneActorRegistryResetView *actor, void *control,
                                    u32 kind)
{
    u32 offset;
    u32 zero;
    {
        u8 *field = &actor->field2B2;
        zero = 0;
        *field = 1;
    }
    offset = 0x2B0;
    *((u8 *)actor + offset) = zero;
    actor->field2B5 = kind;
    offset -= 0x94;
    *(void **)((u8 *)actor + offset) = control;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = zero;
}
static inline void State(struct SceneActorRegistryResetView *a, u32 behavior, u32 zero)
{
    a->state2A8 = zero;
    a->phase2A9 = zero;
    a->behavior2AA = behavior;
    a->counter2AC = zero;
    a->field2B1 = 1;
}
void SceneActorRoute_UpdateDelayedEntry(struct SceneActorRegistryResetView *actor)
{
    u8 result;
    if (SceneActorControl_TakeFlag(&actor->field2B1))
        BeginControlZero(actor, actor->savedTarget, 0);
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    result = sub_0812F3CC(actor);
    if (result)
        return;
    if ((s32)actor->counter2AC > 59)
        State(actor, 2, result);
    else
        actor->counter2AC++;
}
