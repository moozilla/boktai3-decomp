#include "global.h"
#include "scene/actor/scene_actor_reset.h"
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
static inline u8 hasFlag(struct TrailSegment *segment, u8 mask)
{
    u8 value = segment->flags;
    if (mask & value)
        return 1;
    return 0;
}
static inline u8 isRemoved(struct TrailSegment *segment)
{
    return hasFlag(segment, 1);
}
void Centipede_RemoveSegmentColliders(struct ChaseActor *actor)
{
    struct ActorTrail *trail = actor->base.context;
    struct TrailSegment *segments = trail->segments;
    s32 i;
    if (trail->activeCount) {
        for (i = trail->segmentCount - 1; i >= 0; i--) {
            if (!isRemoved(&segments[i])) {
                struct TrailSegment *segment = &segments[i];
                Collider_Remove(segment->collider);
                segment->flags |= 1;
                trail->activeCount--;
            }
        }
    }
}
