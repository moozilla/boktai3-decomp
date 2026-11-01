#include "global.h"
// confidence=low: restarts saved control or advances behavior after callbacks complete.
#include "scene_actor_control_phase.h"
#include "engine.h"
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
        s32 mask = -2;
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
static inline void ClearBit(u16 *flags)
{
    u32 mask = ~1;
    *flags &= mask;
}
void SceneActorControl_RestartOrAdvance(struct SceneActorRegistryResetView *actor)
{
    u8 *control = (u8 *)SceneActorControl_GetState(actor);
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *target = actor->entryTarget224;
        SetControl(actor, target, 2);
    }
    if (!*(u16 *)(control + 0x63C)) {
        if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque)) {
            ClearBit((u16 *)(control + 0x628));
            return;
        }
        if (sub_0812F3CC(actor)) {
            ClearBit((u16 *)(control + 0x628));
            return;
        }
    }
    if (actor->field2B0)
        SetState(actor);
    else
        ++actor->counter2AC;
}
