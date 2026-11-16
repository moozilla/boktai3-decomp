#include "scene_actor_control_phase.h"
void SceneActorControl_UpdateShadowReturn(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B2))
        SceneActorAnimation_SelectDirectionFlagsVariant((void *)actor, 17, 0);
    if (actor->base.field2B3) {
        actor->base.field2B0 = 1;
        ActorShadow_Enable((u8 *)actor + 0x320);
    }
}
