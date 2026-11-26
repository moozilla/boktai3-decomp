#include "monsters/centipede/centipede_body.h"
#include "scene/motion/scene_actor_availability.h"

s32 Mod(s32, s32);

inline static u8 TestMask(struct TentacleAnimationSegment *e, char mask)
{
    if (e->animation.status & mask)
        return 1;
    return 0;
}

void Centipede_ShiftSegmentTrail(struct ChaseActor *actor)
{
    struct TentacleBody *g;
    struct ContactPosition carry;
    s32 i;
    if (SceneActor_ActionsBlocked(actor))
        return;
    g = actor->base.context;
    if (Mod(actor->base.frameCounter, g->unknown874))
        return;
    carry = *(struct ContactPosition *)((u8 *)actor->base.drawable + 0x1c);
    for (i = 0; i < g->segmentCount; i++) {
        if (TestMask(&g->segments[i], 2) && !TestMask(&g->segments[i], 1)) {
            struct ContactPosition t = g->segments[i].position;
            g->segments[i].targetPosition = carry;
            carry = t;
        }
    }
}
