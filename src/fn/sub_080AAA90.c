#include "global.h"

#include "scene_actor_reset.h"
struct TentacleImpulseSegment {
    u8 unknown0[0x1C];
    struct ContactPosition position;
    u8 unknown24[0x105 - 0x24];
    u8 flags;
    u8 unknown106[0x11C - 0x106];
    struct ContactPosition velocity;
    u8 unknown124[4];
};
struct TentacleImpulseTrail {
    u8 unknown0[0x3D4];
    struct TentacleImpulseSegment segments[4];
    u8 unknown874[0x8DA - 0x874];
    s16 count;
};
static inline u32 Tentacle_StatusFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D0 & mask;
}
static inline u8 Tentacle_IsInactive(struct SceneActorRegistryResetView *p)
{
    if (p) {
        u32 mask = 0x40;
        if ((p->flags2D4 & mask) || Tentacle_StatusFlags(p, 0x20000) || p->health <= 0)
            return 1;
    }
    return 0;
}
static inline s32 Tentacle_HalfTowardZero(s32 v)
{
    return v >= 0 ? v >> 1 : -((-v) >> 1);
}
#define TentacleActor_UpdateSegmentImpulses sub_080AAA90
static inline u8 Tentacle_TestSegmentFlags(struct TentacleImpulseSegment *p, u8 mask)
{
    if (p->flags & mask)
        return 1;
    return 0;
}

void TentacleActor_UpdateSegmentImpulses(struct SceneActorRegistryResetView *p)
{
    struct TentacleImpulseTrail *g;
    s32 i;
    if (Tentacle_IsInactive(p))
        return;
    g = p->context;
    for (i = 0; i < g->count; i++) {
        if (Tentacle_TestSegmentFlags(&g->segments[i], 2) &&
            !Tentacle_TestSegmentFlags(&g->segments[i], 1)) {
            g->segments[i].position.x += g->segments[i].velocity.x;
            g->segments[i].position.y += g->segments[i].velocity.y;
            g->segments[i].position.z += g->segments[i].velocity.z;
            g->segments[i].velocity.x = Tentacle_HalfTowardZero(g->segments[i].velocity.x);
            g->segments[i].velocity.y = Tentacle_HalfTowardZero(g->segments[i].velocity.y);
            g->segments[i].velocity.z = Tentacle_HalfTowardZero(g->segments[i].velocity.z);
        }
    }
}
