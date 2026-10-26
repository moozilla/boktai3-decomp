// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
struct MotionPosition {
    u32 x : 16;
    s32 y : 16;
    u32 z : 16;
};
struct TerrainMotion {
    u32 previousCellFlags, previousShape, cellFlags, shape;
    s32 x, y, z, unknown1C;
    s32 deltaX, deltaY, deltaZ, unknown2C;
    u8 unknown30[8];
    s32 verticalOffset, previousOffset;
    u32 collision;
};
s32 Terrain_GetCellClearance(struct MotionPosition *);
s32 Terrain_GetCellHighByte(struct MotionPosition *);
void Memory_Clear(void *, u32);
static inline s32 Abs(s32 x)
{
    if (x < 0)
        x = -x;
    return x;
}
static inline s32 Query(struct MotionPosition *p)
{
    register s32 result asm("r1") = Terrain_GetCellClearance(p);
    asm("" : "+r"(result));
    return result;
}
static inline s32 Offset(s32 z)
{
    s32 offset = z + 96;
    return offset;
}
// confidence=low: resolves glove and actor motion against terrain clearance and depth limits.
void Terrain_ResolveActorMotion(struct TerrainMotion *p)
{
    struct MotionPosition old, next;
    s32 y, floor, difference;
    u32 bits;
    u32 shape, flags;
    p->collision = 0;
    p->previousOffset = p->verticalOffset;
    old.x = (p->x << 4) >> 16;
    old.y = ((p->y + p->verticalOffset) << 4) >> 16;
    old.z = (p->z << 4) >> 16;
    next.x = ((p->x + p->deltaX) << 4) >> 16;
    next.y = ((p->y + p->verticalOffset + p->deltaY) << 4) >> 16;
    next.z = ((p->z + p->deltaZ) << 4) >> 16;
    if ((s16)next.z < 6) {
        next.z = 6;
        p->z = 6 << 12;
        p->deltaZ = 0;
        p->collision = 2;
    } else if ((s16)next.z > 60) {
        next.z = 60;
        p->z = 60 << 12;
        p->deltaZ = 0;
        p->collision = 2;
    }
    Terrain_GetCellClearance(&old);
    floor = Query(&next);
    y = (s16)next.y;
    floor -= Offset((s16)next.z);
    if (floor - y < -8) {
        next.z = p->z >> 12;
        p->deltaZ = 0;
        p->collision |= 2;
        floor = Query(&next);
        y = (s16)next.y;
        floor -= Offset((s16)next.z);
    }
    bits = Terrain_GetCellHighByte(&next);
    {
        u32 source = bits;
        register u32 mask asm("r3");
        asm("" : "+r"(source));
        flags = source;
        mask = 0xF00;
        asm("" : "+r"(mask));
        mask &= flags;
        asm("" : "+r"(mask));
        shape = mask;
        flags &= 0xF000;
    }
    difference = floor - y;
    asm("" : "+r"(difference));
    if (y >= floor) {
        p->y = floor << 12;
        if (Abs(p->verticalOffset >> 8) > 8)
            p->collision |= 4;
        p->collision |= 1;
        if (p->verticalOffset > 0)
            p->verticalOffset = 0;
        if (p->deltaY > 0)
            p->deltaY = 0;
    } else {
        s32 offset = -(difference << 12);
        asm("" : "+r"(offset));
        p->verticalOffset = offset;
        p->y = floor << 12;
        if ((y = p->deltaY >= 0) && (Abs(offset) >> 12) <= 3)
            p->verticalOffset = 0;
    }
    p->previousShape = p->cellFlags;
    p->previousCellFlags = p->shape;
    p->cellFlags = flags;
    p->shape = shape;
    p->x += p->deltaX;
    p->verticalOffset += p->deltaY;
    p->z += p->deltaZ;
    Memory_Clear(&p->deltaX, 16);
}

struct SpatialLink {
    void *entry;
    struct SpatialLink *previous, *next;
};
struct SpatialBucket {
    struct SpatialLink *active, *passive;
    s32 activeCount, passiveCount;
};
struct SpatialIndex {
    u8 unknown0[0x18];
    struct SpatialBucket buckets[10];
};
void SpatialIndex_ScanBuckets(u32 flags)
{
    if (gSpatialIndex) {
        struct SpatialIndex *index = gSpatialIndex;
        s32 i = 0;
        u32 active = flags & 8;
        u32 passive = flags & 16;
        for (; i <= 9; i++) {
            struct SpatialLink *link;
            struct SpatialBucket *bucket = &index->buckets[i];
            if (active)
                link = bucket->active;
            else if (passive)
                link = bucket->passive;
            else
                return;
            while (link)
                link = link->next;
        }
    }
}
