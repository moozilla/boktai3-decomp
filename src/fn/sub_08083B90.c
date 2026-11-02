#include "scene_actor_control_helpers.h"
extern const u32 gUnk_08605C3C[8];
static inline s32 Divide128(s32 value)
{
    if (value >= 0)
        return value >> 7;
    return -((-value) >> 7);
}

static inline u32 SceneActorControl_ReadTravelProfile(const u32 *table, u32 index)
{
    return table[index];
}
u32 SceneActorControl_GetTravelProfile(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    s32 sample;
    s32 index;
    sample = Divide128(state->field3E0);
    if (sample < 0)
        sample = -sample;
    index = sample;
    if (index > 7)
        index = 7;
    {
        u32 mask = 8;
        u32 flags = state->flags3D4;
        if (SceneActorControl_TestFlags(flags, mask))
            return SceneActorControl_ReadTravelProfile(gUnk_08605C3C, index) + 6;
        return SceneActorControl_ReadTravelProfile(gUnk_08605C3C, index);
    }
}
