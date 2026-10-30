// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reset.h"

static inline u32 TestFlag(struct SceneActorRegistryResetView *p, u32 mask)
{
    u32 m = mask;
    asm("" : "+r"(m));
    return p->flags2D4 & m;
}

s32 SceneActor_IsFacingNearbyTarget(struct SceneActorRegistryResetView *p)
{
    struct SceneActorRegistryResetView *q = p->context;
    s32 difference, absolute;
    if (q->field6E8)
        return 0;
    if (TestFlag(p, 0x100))
        return 0;
    if (p->tracking.distance > 0xFFFF)
        return 0;
    difference = p->tracking.heading - p->body.direction;
    if (difference > 0x80)
        difference -= 0x100;
    else if (difference < -0x80)
        difference += 0x100;
    absolute = difference;
    if (absolute < 0)
        absolute = -absolute;
    if (absolute > 0x12)
        return 0;
    return 1;
}
