#include "global.h"
#include "scene_actor_control_phase.h"
u8 SceneActorReaction_IsRangeReady(struct SceneActorRegistryResetView *);
s32 Callback_InvokeActor(void *, void *);
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor)
{
    u32 behavior = 15;
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
void SceneActorControl_UpdateEntryPhase(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *control = actor->entryTarget224;
        SetControl(actor, control, 2);
    }
    if (SceneActorReaction_IsRangeReady(actor)) {
        if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
            return;
        if (sub_0812F3CC(actor))
            return;
    }
    if (actor->field2B0) {
        SetState(actor);
        SetControl(actor, actor->savedTarget, 0);
    } else
        ++actor->counter2AC;
}
