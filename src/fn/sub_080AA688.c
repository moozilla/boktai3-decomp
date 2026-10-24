#include "global.h"
#include "scene_actor_reset.h"
// MATCHING_CONCESSION: register-pinning
// Remaining differences: the reverse-loop stride uses r0 instead of r1.
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
static inline u8 *CountAddress(struct ActorTrail *trail)
{
    register u32 offset asm("r0") = 0x8ea;
    register u8 *p asm("r1");
    p = (u8 *)trail + offset;
    asm("" : "+r"(p));
    return p;
}
static inline u8 isRemoved(struct TrailSegment *segment)
{
    u32 mask = 1;
    u32 value = segment->flags;
    if (mask & value)
        return 1;
    return 0;
}
#define TentacleActor_RemoveSegmentColliders sub_080AA688
void TentacleActor_RemoveSegmentColliders(struct SceneActorRegistryResetView *actor)
{
    struct ActorTrail *trail = actor->context;
    struct TrailSegment *segments = trail->segments;
    s32 i;
    if (trail->activeCount) {
        for (i = trail->segmentCount - 1; i >= 0; i--) {
            if (!isRemoved(&segments[i])) {
                struct TrailSegment *segment = &segments[i];
                Collider_Remove(segment->collider);
                segment->flags |= 1;
                --*CountAddress(trail);
            }
        }
    }
}
