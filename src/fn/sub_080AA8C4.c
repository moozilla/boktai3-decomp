#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: segment address lifetimes and register allocation.
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
static inline u32 WordFlag(struct SceneActorRegistryResetView *p, u32 mask)
{
    return mask & p->flags2D0;
}
static inline u8 IsInactive(struct SceneActorRegistryResetView *actor)
{
    if (actor) {
        u32 mask = 0x40;
        if ((actor->flags2D4 & mask) || WordFlag(actor, 0x20000) || actor->health <= 0)
            return 1;
    }
    return 0;
}
static inline s32 Shift(s32 value, s32 bits)
{
    if (value >= 0)
        return value >> bits;
    return -(-value >> bits);
}
static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline s32 LengthSquared(struct ContactPosition *p)
{
    return p->x * p->x + p->y * p->y + p->z * p->z;
}

static inline s32 Height(s32 cell, struct ContactPosition *p)
{
    u32 attr = *SceneActorTerrain_GetCell(cell);
    u32 kind = attr >> 4;
    u32 mask = 15;
    s32 height;
    mask &= attr;
    height = mask << 8;
    switch (kind) {
        case 1:
            height -= ((u8 *)p)[4];
            break;
        case 2:
            height -= ((u8 *)p)[0];
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
static inline s32 NearHeight(s32 x, s32 z, s32 limit)
{
    if (Abs(x) > limit || Abs(z) > limit)
        return 0;
    return 1;
}
void TentacleActor_AlignSegmentsToTerrain(struct SceneActorRegistryResetView *actor)
{
    struct TentacleTrail *trail;
    s32 i;
    u32 k1 = 0x3f2;
    if (IsInactive(actor))
        return;
    do {
        trail = actor->context;
    } while (0);
    if (WordFlag(actor, 0x100000))
        return;
    for (i = 0; i < trail->segmentCount; i++) {
        struct TrailSegment *segment = &trail->segments[i];
        if (TestSegment(segment, 2) && (!TestSegment(segment, 1))) {
            s16 height =
                Height(SceneActorTerrain_GetCellIndex(&segment->position), &segment->position);
            s32 y = *((s16 *)((((u8 *)trail) + (i * 296)) + k1));
            if (y != height) {
                if (y < height) {
                    s32 delta = height - y;
                    if (!NearHeight(delta, delta, 255))
                        *((s16 *)((((u8 *)trail) + (i * 296)) + k1)) = actor->position.y;
                    else
                        *((s16 *)((((u8 *)trail) + (i * 296)) + k1)) = height;
                } else {
                    s32 delta = height - y;
                    if (Abs(delta) > 256)
                        *((s16 *)((((u8 *)trail) + (i * 296)) + k1)) = actor->position.y;
                    else
                        *((s16 *)((((u8 *)trail) + (i * 296)) + k1)) += Div(delta, 10);
                }
            }
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", TentacleActor_AlignSegmentsToTerrain);
#endif
