#include "scene_actor_control_phase.h"
void SceneActorControl_UpdateFacingAnimation(struct ChaseActor *actor)
{
    struct SceneActorControlState *control = SceneActorControl_GetState(actor);
    u8 changed = SceneActorControl_TakeFlag(&actor->base.field2B2);
    if (changed) {
        u8 direction;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
        direction = actor->base.body.direction;
        control->savedDirection = direction;
    } else if (control->savedDirection != actor->base.body.direction) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
        control->savedDirection = actor->base.body.direction;
    } else
        actor->base.restart = changed;
}
