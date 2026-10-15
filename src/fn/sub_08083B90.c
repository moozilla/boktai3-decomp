// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_helpers.h"
extern const u32 gUnk_08605C3C[8];
static inline s32 Divide128(s32 value)
{
    if (value >= 0)
        return value >> 7;
    return -((-value) >> 7);
}
static inline u32 ReadProfile(u32 index)
{
    register const u32 *table asm("r0") = gUnk_08605C3C;
    return table[index];
}
u32 SceneActorControl_GetTravelProfile(struct SceneActorRegistryResetView *actor)
{
    register struct SceneActorControlMotionState *state asm("r3") =
        SceneActorControl_GetMotionState(actor);
    register u32 offset asm("r4") = 0x3E0;
    s32 sample;
    s32 index;
    u32 result;
    asm("" : "+r"(offset));
    {
        register u16 *field asm("r0") = (void *)((u8 *)state + offset);
        sample = Divide128(*field);
    }
    if (sample < 0)
        sample = -sample;
    index = sample;
    if (index > 7)
        index = 7;
    {
        u32 mask = 8;
        u32 flags;
        register u32 *field asm("r0");
        offset = 0x3D4;
        field = (void *)((u8 *)state + offset);
        flags = *field;
        if (!SceneActorControl_TestFlags(flags, mask))
            result = ReadProfile(index);
        else
            result = ReadProfile(index) + 6;
    }
    return result;
}
