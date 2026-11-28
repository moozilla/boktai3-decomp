#include "scene/actor/scene_actor_control_motion.h"

void SceneActorControl_InitPositionCurveEndpoints(struct ChaseActor *actor,
                                                  const struct ContactPosition *target)
{
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    state->vectors[0] = actor->base.body.position;
    state->vectors[1] = *target;
    state->field3E0 = 0;
}
