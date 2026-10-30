#ifndef GUARD_SCENE_ACTOR_DELAYED_RESPONSE_H
#define GUARD_SCENE_ACTOR_DELAYED_RESPONSE_H

#include "scene_actor_chase_types.h"

/* A delayed response starts with the saved controller's eight-byte record. */
struct SceneActorDelayedResponse {
    struct SceneActorChaseSavedControl saved;
    u8 state, phase, behavior, kind;
    s32 delay;
    void *control;
};

static inline void SceneActorDelay_Prepare(struct SceneActorDelayedResponse *response,
                                            void *control, u32 behavior, s32 duration)
{
    response->delay = duration;
    response->control = control;
    response->state = 0;
    response->phase = 0;
    response->behavior = behavior;
}

#endif
