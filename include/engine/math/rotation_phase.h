#include "scene/actor/scene_actor_control_phase.h"
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
static inline s32 RotationPhase_HeadingDistance(s32 heading, s32 direction)
{
    s32 difference = NormalizeAngle(heading - direction);
    return difference < 0 ? -difference : difference;
}
static inline u32 RotationPhase_Mask(u32 *p, u32 mask)
{
    return *p & mask;
}
static inline void RotationPhase_Change(struct ChaseActor *actor, void *callback, u8 kind)
{
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = callback;
    actor->base.field2C8 = 0;
    actor->base.pendingControlFlags = RotationPhase_Mask(&actor->base.pendingControlFlags, ~1);
    actor->base.pendingControl = 0;
}
