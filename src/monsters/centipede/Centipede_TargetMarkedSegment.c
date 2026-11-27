#include "monsters/chase_actor.h"
#include "monsters/centipede/centipede_body.h"
#include "scene/motion/scene_actor_chase_shared.h"
inline static u8 TestMask(struct TentacleAnimationSegment *e, char mask)
{
    if (e->animation.status & mask)
        return 1;
    return 0;
}

void Centipede_TargetMarkedSegment(struct ChaseActor *actor)
{
    struct TentacleBody *g = actor->base.context;
    s32 found = 0xff;
    s32 i;
    for (i = 0; i < g->segmentCount; i++) {
        if (TestMask(&g->segments[i], 4)) {
            found = i;
            break;
        }
    }
    if (found != 0xff) {
        u32 mask = 1;
        if (SceneActorChase_TestField(&g->attackFlags, mask)) {
            g->markedSegmentBody.position = g->segments[found].position;
            g->markedSegmentBody.sprite = &g->segments[found];
        }
    }
}
