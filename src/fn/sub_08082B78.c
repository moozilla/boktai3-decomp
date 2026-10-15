#include "scene_actor_control_phase.h"
void SceneActorControl_UpdateFacingAnimation(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorControlState *control = SceneActorControl_GetState(actor);
    u8 changed = SceneActorControl_TakeFlag(&actor->field2B2);
    if (changed) {
        u8 direction;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
        direction = actor->direction;
        control->savedDirection = direction;
    } else if (control->savedDirection != actor->direction) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
        control->savedDirection = actor->direction;
    } else
        actor->field2B3 = changed;
}
