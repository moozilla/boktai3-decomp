#include "scene_actor_control_phase.h"
struct RotatingDrawable {
    u32 flags;
    u8 unknown4[2];
    u8 angle;
};
static inline s32 NormalizeAngle(s32 angle)
{
    s32 result = angle;
    if (angle > 128)
        result -= 256;
    else if (angle < -128)
        result += 256;
    return result;
}
static inline u32 RotationPhase_Mask(u32 *p, u32 mask)
{
    return *p & mask;
}
static inline void RotationPhase_Change(struct SceneActorRegistryResetView *actor, void *callback,
                                        u8 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    actor->flags114 = RotationPhase_Mask(&actor->flags114, ~1);
    actor->word104 = 0;
}
