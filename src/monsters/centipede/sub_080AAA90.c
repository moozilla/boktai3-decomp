#include "monsters/centipede/centipede_body.h"

#include "scene/actor/scene_actor_reset.h"
#include "scene/motion/scene_actor_availability.h"
#include "scene/motion/scene_actor_chase_shared.h"
static inline s32 Tentacle_HalfTowardZero(s32 v)
{
    return v >= 0 ? v >> 1 : -((-v) >> 1);
}
#define TentacleActor_UpdateSegmentImpulses sub_080AAA90
static inline u8 Tentacle_TestSegmentFlags(struct TentacleAnimationSegment *p, u8 mask)
{
    if (p->animation.status & mask)
        return 1;
    return 0;
}

void TentacleActor_UpdateSegmentImpulses(struct ChaseActor *p)
{
    struct TentacleBody *g;
    s32 i;
    if (SceneActor_ActionsBlocked(p))
        return;
    g = p->base.context;
    for (i = 0; i < g->segmentCount; i++) {
        if (Tentacle_TestSegmentFlags(&g->segments[i], 2) &&
            !Tentacle_TestSegmentFlags(&g->segments[i], 1)) {
            g->segments[i].position.x += g->segments[i].offset.x;
            g->segments[i].position.y += g->segments[i].offset.y;
            g->segments[i].position.z += g->segments[i].offset.z;
            g->segments[i].offset.x = Tentacle_HalfTowardZero(g->segments[i].offset.x);
            g->segments[i].offset.y = Tentacle_HalfTowardZero(g->segments[i].offset.y);
            g->segments[i].offset.z = Tentacle_HalfTowardZero(g->segments[i].offset.z);
        }
    }
}
