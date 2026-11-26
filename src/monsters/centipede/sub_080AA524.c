#include "monsters/centipede/centipede_body.h"
#include "global.h"
#include "scene/actor/scene_actor_reset.h"
void Centipede_SpawnSparkBurst(struct ContactPosition *);
static inline u8 isUnavailable(struct TentacleAnimationSegment *segment)
{
    u32 value = segment->animation.status;
    if (5 & value)
        return 1;
    return 0;
}
static inline void setFlag(struct TentacleAnimationSegment *segment, u8 mask)
{
    segment->animation.status |= mask;
}
s32 ActorTrail_RemoveLastSegment(struct ChaseActor *actor)
{
    struct TentacleBody *trail = actor->base.context;
    struct TentacleAnimationSegment *segments = trail->segments;
    s32 i;
    if (trail->spreadStage > 2)
        goto scan;
    goto failure;
removeSegment: {
    struct TentacleAnimationSegment *segment = &segments[i];
    Collider_Remove(&segment->bodyCollider);
    setFlag(segment, 1);
    trail->spreadStage--;
    Centipede_SpawnSparkBurst(&segment->position);
    return 1;
}
scan:
    for (i = trail->segmentCount - 1; i >= 0; i--) {
        if (!isUnavailable(&segments[i]))
            goto removeSegment;
    }
failure:
    return 0;
}
