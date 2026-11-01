#include "scene_actor_control_motion.h"

extern const u32 gUnk_08605E48[8];

u32 Lun_GetRouteProgressRate(void *actor)
{
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    s32 phase = state->field3E0 >> 7;
    s16 index = phase;
    if (phase > 7)
        index = 7;
    return gUnk_08605E48[index];
}
