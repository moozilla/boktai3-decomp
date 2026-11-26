#include "engine/shared/scene_actor_control_phase.h"
void SceneActorControl_UpdateShadowReturn(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B2))
        SceneActorAnimation_SelectDirectionFlagsVariant((void *)actor, 17, 0);
    if (actor->base.restart) {
        actor->base.stepDone = 1;
        ActorShadow_Enable(actor->base.shadow);
    }
}
