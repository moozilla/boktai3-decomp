#include "global.h"
#include "ram.h"

// MATCHING_CONCESSION: register-pinning

struct SpatialEntry {
    u32 flags;
    u8 unknown4[12];
    s16 x, y, z;
    u8 unknown16[10];
    s16 offsetX;
    u8 unknown22[6];
    s16 screenX;
    u8 unknown2A[2];
    s16 screenZ;
    u8 unknown2E[1];
    u8 unknown2F;
    u8 unknown30;
    s8 bucket;
};
struct SpatialIndex {
    u8 unknown0[0xbc];
    s32 bucketWidth;
};
struct SpatialCamera {
    u8 unknown0[0x44];
    s32 x, z;
};
extern struct SpatialCamera *gUnk_020004B8;
s32 Div(s32, s32);
u8 SpatialIndex_RemoveEntry(struct SpatialEntry *);
void SpatialIndex_InsertEntry(struct SpatialEntry *);
static inline u8 NeedsReinsert(struct SpatialEntry *entry, s32 bucket)
{
    s8 *bucketField;
    if (!(*((u8 *)entry + 15) & 1) || (entry->flags & 8))
        return 0;
    bucketField = &entry->bucket;
    asm("" : : : "r0");
    if (*bucketField < 0 || bucket == entry->bucket || !SpatialIndex_RemoveEntry(entry))
        return 0;
    return 1;
}
void SpatialIndex_SetPosition(struct SpatialEntry *entry, s32 x, s32 y, s32 z)
{
    struct SpatialIndex *index = gSpatialIndex;
    s32 bucket;
    if (!index)
        return;
    entry->screenX = entry->x - gUnk_020004B8->x;
    entry->screenZ = entry->z - gUnk_020004B8->z;
    if (!index->bucketWidth)
        bucket = 0;
    else
        bucket = Div(x + entry->offsetX, index->bucketWidth);
    if (bucket > 9)
        bucket = 9;
    if (NeedsReinsert(entry, bucket)) {
        entry->x = x;
        entry->y = y;
        entry->z = z;
        entry->bucket = bucket;
        SpatialIndex_InsertEntry(entry);
    } else {
        entry->x = x;
        entry->y = y;
        entry->z = z;
        entry->bucket = bucket;
    }
}
