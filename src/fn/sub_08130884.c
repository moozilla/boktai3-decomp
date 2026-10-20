#include "scene_actor_control_phase.h"
u32 Trigger_ConsumePending(void *, u32);
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *callback, u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                            u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

void SceneActorControl_UpdateTriggerPhase(struct SceneActorRegistryResetView *actor)
{
    u8 taken = SceneActorControl_TakeFlag(&actor->field2B1);
    if (taken) {
        SetControl(actor, actor->entryTarget224, 1);
    } else if (actor->field2B0) {
        Trigger_ConsumePending((u8 *)actor + 0xc, 1);
        SetControl(actor, actor->savedTarget, taken);
        SetState(actor, 2, 0, 2);
    } else {
        actor->counter2AC++;
    }
}
