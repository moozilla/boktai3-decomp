#include "engine/math/math_shift.h"
#include "scene/actor/scene_actor_control_helpers.h"
extern const u32 gUnk_08605C3C[8];

u32 SceneActorControl_GetTravelProfile(struct ChaseActor *actor)
{
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    s32 sample;
    s32 index;
    sample = abs(Math_ShiftTowardZero(state->field3E0, 7));
    index = sample;
    if (index > 7)
        index = 7;
    {
        u32 mask = 8;
        u32 flags = state->flags3D4;
        if (SceneActorControl_TestFlags(flags, mask))
            return gUnk_08605C3C[index] + 6;
        return gUnk_08605C3C[index];
    }
}
