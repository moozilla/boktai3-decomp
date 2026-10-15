#include "scene_actor_control_phase.h"
#define SceneActorControl_UpdateShadowReturn sub_08082CA8
void SceneActorControl_UpdateShadowReturn(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeFlag(&actor->field2B2))
        SceneActorAnimation_SelectDirectionFlagsVariant((void *)actor, 17, 0);
    if (actor->field2B3) {
        actor->field2B0 = 1;
        ActorShadow_Enable((u8 *)actor + 0x320);
    }
}
