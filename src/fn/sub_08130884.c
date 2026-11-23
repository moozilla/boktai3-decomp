#include "scene_actor_control_phase.h"
#include "scene_actor_terrain_reaction.h"
u32 Trigger_ConsumePending(void *, u32);
static inline void SetControl(struct ChaseActor *actor, void *callback, u32 kind)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = callback;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.pendingControl = 0;
}

void SceneActorControl_UpdateTriggerPhase(struct ChaseActor *actor)
{
    u8 taken = SceneActorControl_TakeFlag(&actor->base.field2B1);
    if (taken) {
        SetControl(actor, actor->base.onEntry, 1);
    } else if (actor->base.stepDone) {
        Trigger_ConsumePending((u8 *)actor + 0xc, 1);
        SetControl(actor, actor->base.onStanding, taken);
        SceneActorTerrain_EnterState(actor, 2, 0, 2);
    } else {
        actor->base.counter2AC++;
    }
}
