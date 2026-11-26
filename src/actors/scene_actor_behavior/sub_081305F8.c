#include "engine/shared/scene_actor_control_phase.h"
#include "engine/shared/global.h"
#include "actors/scene_actor_reaction/scene_actor_reaction.h"


static inline void SetControl(struct ChaseActor *actor, u32 kind, void *control)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

void SceneActorControl_BeginIdleControl(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeStateEntry(actor)) {
        void *target = SceneActorControl_OnIdle;
        SetControl(actor, 0x10, target);
    }
    actor->base.counter2AC++;
}
