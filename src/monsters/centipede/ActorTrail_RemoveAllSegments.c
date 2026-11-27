#include "monsters/centipede/centipede_body.h"
#include "global.h"
#include "scene/actor/scene_actor_reset.h"
void Centipede_SpawnSparkBurst(struct ContactPosition *);
static inline u8 isRemoved(struct TentacleAnimationSegment *segment)
{
    u32 value = segment->animation.status;
    if (1 & value)
        return 1;
    return 0;
}
void ActorTrail_RemoveAllSegments(struct ChaseActor *actor)
{
    struct TentacleBody *trail = actor->base.context;
    struct TentacleAnimationSegment *segments = trail->segments;
    s32 i;
    if (trail->spreadStage) {
        for (i = trail->segmentCount - 1; i >= 0; i--) {
            if (!isRemoved(&segments[i])) {
                struct TentacleAnimationSegment *segment = &segments[i];
                Collider_Remove(&segment->bodyCollider);
                segment->animation.status |= 1;
                trail->spreadStage--;
                Centipede_SpawnSparkBurst(&segment->position);
            }
        }
    }
}
