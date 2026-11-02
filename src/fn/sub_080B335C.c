#include "global.h"
#include "rotation_phase.h"
#include "scene_actor_reset.h"

static inline u32 TestMotionFlag(struct SceneActorRegistryResetView *p, u32 mask)
{
    return (p->flags2D4 & mask);
}
s32 SceneActor_IsFacingNearbyTarget(struct SceneActorRegistryResetView *p)
{
    struct SceneActorRegistryResetView *q = p->context;
    s32 absolute;
    if (q->field6E8)
        return 0;
    if (TestMotionFlag(p, 0x100))
        return 0;
    if (p->tracking.distance > 0xFFFF)
        return 0;
    absolute = RotationPhase_HeadingDistance(p->tracking.heading, p->body.direction);
    if (absolute > 0x12)
        return 0;
    return 1;
}
