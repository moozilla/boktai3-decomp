#include "monsters/centipede/centipede_body.h"
#include "scene/actor/scene_actor_reset.h"
static inline u8 hasFlag(struct TentacleAnimationSegment *segment, u8 mask)
{
    u8 value = segment->animation.status;
    if (mask & value)
        return 1;
    return 0;
}
static inline u8 isRemoved(struct TentacleAnimationSegment *segment)
{
    return hasFlag(segment, 1);
}
void Centipede_RemoveSegmentColliders(struct ChaseActor *actor)
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
            }
        }
    }
}
