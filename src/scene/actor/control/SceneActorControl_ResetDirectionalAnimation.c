#include "scene/actor/scene_actor_control_phase.h"

void SceneActorControl_ResetDirectionalAnimation(struct ChaseActor *actor)
{
    if (actor->base.field2B2)
        actor->base.field2B2 = 0;
    SceneActorAnimation_SelectDirectionalEntry((struct RoomActor *)actor, 0);
}
