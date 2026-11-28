#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/actor/scene_actor_control_phase.h"
u32 Trigger_ConsumePending(void *, u32);

void SceneActorControl_UpdateTriggerPhase(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorTerrain_InstallControl(actor, actor->base.onEntry, 1);
    } else if (actor->base.stepDone) {
        Trigger_ConsumePending(actor->base.receiver, 1);
        SceneActorTerrain_InstallControl(actor, actor->base.onStanding, 0);
        SceneActorTerrain_EnterState(actor, 2, 0, 2);
    } else {
        actor->base.counter2AC++;
    }
}
