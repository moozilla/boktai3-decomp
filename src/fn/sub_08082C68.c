#include "scene_actor_control_phase.h"
#define SceneActorControl_UpdatePersistentAnimation sub_08082C68
void SceneActorControl_UpdatePersistentAnimation(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeFlag(&actor->field2B2))
        SceneActorAnimation_SelectDirectionFlagsVariant((void *)actor, 15, 0);
    actor->field2B3 = 0;
}
