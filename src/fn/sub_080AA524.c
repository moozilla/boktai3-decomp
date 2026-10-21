#include "global.h"
#include "scene_actor_reset.h"
struct TrailSegment {
    u8 unknown0[0x1C];
    struct ContactPosition position;
    u8 unknown24[0x78];
    u8 collider[0x69];
    u8 flags;
    u8 unknown106[0x22];
};
struct ActorTrail {
    u8 unknown0[0x3D4];
    struct TrailSegment segments[4];
    u8 unknown874[0x66];
    s16 segmentCount;
    u8 unknown8DC[0xE];
    u8 activeCount;
};
void TentacleActor_SpawnSparkBurst(struct ContactPosition *);
static inline u8 isUnavailable(struct TrailSegment *segment)
{
    u32 mask = 5;
    u32 value = segment->flags;
    if (mask & value)
        return 1;
    return 0;
}
static inline void setFlag(struct TrailSegment *segment, u8 mask)
{
    segment->flags |= mask;
}
s32 ActorTrail_RemoveLastSegment(struct SceneActorRegistryResetView *actor)
{
    struct ActorTrail *trail = actor->context;
    struct TrailSegment *segments = trail->segments;
    s32 i;
    if (trail->activeCount > 2)
        goto scan;
    goto failure;
removeSegment: {
    struct TrailSegment *segment = &segments[i];
    Collider_Remove(segment->collider);
    setFlag(segment, 1);
    trail->activeCount--;
    TentacleActor_SpawnSparkBurst(&segment->position);
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
