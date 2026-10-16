#include "scene_actor_control_motion.h"
void SceneActorControl_TickSway(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    if (state->sway > 320) {
        state->swayStep = -1;
        state->sway = 320;
    } else if (state->sway < -320) {
        state->swayStep = 1;
        state->sway = -320;
    }
    state->sway += state->swayStep * 12;
}
