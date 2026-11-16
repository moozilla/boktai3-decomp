#include "global.h"
#include "vorn.h"
#include "rotation_phase.h"
#include "scene_actor_reset.h"

static inline u32 TestMotionFlag(struct Vorn *p, u32 mask)
{
    return (p->chase.base.flags2D4 & mask);
}
s32 SceneActor_IsFacingNearbyTarget(struct Vorn *p)
{
    struct Vorn *q = p->chase.base.context;
    s32 absolute;
    if (q->field6E8)
        return 0;
    if (TestMotionFlag(p, 0x100))
        return 0;
    if (p->chase.base.tracking.distance > 0xFFFF)
        return 0;
    absolute =
        RotationPhase_HeadingDistance(p->chase.base.tracking.heading, p->chase.base.body.direction);
    if (absolute > 0x12)
        return 0;
    return 1;
}
