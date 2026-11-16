#include "scene_actor_control_phase.h"
void SceneActorControl_UpdatePersistentAnimation(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B2))
        SceneActorAnimation_SelectDirectionFlagsVariant((void *)actor, 15, 0);
    actor->base.restart = 0;
}
