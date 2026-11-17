#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"
struct TrailSegment {
    u8 unknown0[0x1c];
    struct ContactPosition position;
    u8 unknown24[0x78];
    u8 collider[0x69];
    u8 flags;
    u8 unknown106[0xe];
    struct ContactPosition target;
    struct ContactPosition velocity;
    u8 unknown124[4];
};
struct TentacleTrail {
    u8 unknown0[0x3d4];
    struct TrailSegment segments[4];
    u8 unknown874[0x66];
    s16 segmentCount;
    u8 unknown8dc[0xe];
    u8 activeCount;
};
static inline u8 TestSegment(struct TrailSegment *segment, u32 mask)
{
    if (segment->flags & mask)
        return 1;
    return 0;
}
static inline u32 WordFlag(struct ChaseActor *p, u32 mask)
{
    return mask & p->base.statusFlags;
}
static inline u8 IsInactive(struct ChaseActor *actor)
{
    if (actor) {
        u32 mask = 0x40;
        if ((actor->base.motionFlags & mask) || WordFlag(actor, 0x20000) || actor->base.health <= 0)
            return 1;
    }
    return 0;
}
static inline s32 Height(s32 cell, struct ContactPosition *position)
{
    struct ContactPosition *p = position;
    u8 attr = *SceneActorTerrain_GetCell(cell);
    u8 kind = attr >> 4;
    u32 mask = 15;
    s32 height;
    mask &= attr;
    height = mask << 8;
    switch (kind) {
        case 1:
            height -= (u8)p->z;
            break;
        case 2:
            height -= (u8)p->x;
            break;
    }
    return height;
}
static inline s32 Abs(s32 n)
{
    if (n < 0)
        return -n;
    return n;
}
void TentacleActor_AlignSegmentsToTerrain(struct ChaseActor *actor)
{
    struct TentacleTrail *trail;
    s32 i;
    u32 positionY = (u32) & ((struct TentacleTrail *)0)->segments[0].position.y;
    if (IsInactive(actor))
        return;
    trail = actor->base.context;
    if (WordFlag(actor, 0x100000))
        return;
    for (i = 0; i < trail->segmentCount; i++) {
        struct TrailSegment *segment = &trail->segments[i];
        struct ContactPosition *position = &segment->position;
        if (TestSegment(segment, 2) && (!TestSegment(segment, 1))) {
            s32 rawHeight = Height(SceneActorTerrain_GetCellIndex(position), position);
            s32 height = rawHeight;
            s16 y;
            y = segment->position.y;
            if (y != height) {
                if (y < height) {
                    s32 delta = height - y;
                    if (Abs(delta) > 255)
                        *((s16 *)((((u8 *)trail) + (i * sizeof(struct TrailSegment))) +
                                  positionY)) = actor->base.body.position.y;
                    else if (Abs(delta) > 255)
                        *((s16 *)((((u8 *)trail) + (i * sizeof(struct TrailSegment))) +
                                  positionY)) = actor->base.body.position.y;
                    else
                        *((s16 *)((((u8 *)trail) + (i * sizeof(struct TrailSegment))) +
                                  positionY)) = height;
                } else {
                    s32 delta;
                    height = rawHeight - y;
                    delta = height;
                    if (Abs(delta) > 256)
                        *((s16 *)((((u8 *)trail) + (i * sizeof(struct TrailSegment))) +
                                  positionY)) = actor->base.body.position.y;
                    else
                        *((s16 *)((((u8 *)trail) + (i * sizeof(struct TrailSegment))) +
                                  positionY)) += Div(delta, 10);
                }
            }
        }
    }
}
