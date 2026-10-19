#include "global.h"

#ifdef NONMATCHING
// fuzzy: 85.0%
// foundry draft (worker best 85.0%); what differs is mostly register allocation and instruction order
#define SpatialIndex_RemoveEntry sub_0811B394
#define gSpatialIndex            gUnk_02000200
struct SpatialEntry {
    u32 flags, mask, category;
    u8 unknownC[3];
    u8 registrationFlags;
    s16 x, y, z;
    u8 unknown16[10];
    s16 offsetX;
};
struct SpatialLink {
    struct SpatialEntry *entry;
    struct SpatialLink *previous, *next;
};
struct SpatialBucket {
    struct SpatialLink *active, *passive;
    s32 activeCount, passiveCount;
};
struct SpatialIndex {
    u8 unknown0[0x18];
    struct SpatialBucket buckets[10];
    u8 unknownB8[4];
    s32 bucketWidth;
};
extern struct SpatialIndex *gSpatialIndex;
s32 Div(s32, s32);
void Heap_Free(void *);
u8 sub_0811B394(struct SpatialEntry *entry)
{
    struct SpatialIndex *index = gSpatialIndex;
    struct SpatialBucket *bucket;
    struct SpatialLink *link;
    s32 column;
    if (!index)
        return 0;
    column = Div(entry->x + entry->offsetX, index->bucketWidth);
    if (column > 9)
        column = 9;
    bucket = &index->buckets[column];
    if (entry->flags & 8)
        link = bucket->active;
    else if (entry->flags & 16)
        link = bucket->passive;
    else
        return 0;
    while (link) {
        if (link->entry == entry) {
            struct SpatialLink *previous;
            entry->registrationFlags &= 0xfe;
            previous = link->previous;
            if (!previous) {
                if (link->next) {
                    link->next->previous = previous;
                    if (entry->flags & 8) {
                        bucket->active = link->next;
                        goto activeRemoved;
                    } else if (entry->flags & 16) {
                        bucket->passive = link->next;
                        goto passiveRemoved;
                    }
                    goto freeLink;
                } else {
                    Heap_Free(link);
                    if (entry->flags & 8) {
                        bucket->active = previous;
                        bucket->activeCount--;
                    } else if (entry->flags & 16) {
                        bucket->passive = previous;
                        bucket->passiveCount--;
                    }
                    return 1;
                }
            } else {
                previous->next = link->next;
                if (link->next)
                    link->next->previous = link->previous;
            }
            if (entry->flags & 8) {
            activeRemoved:
                bucket->activeCount--;
            } else if (entry->flags & 16) {
            passiveRemoved:
                bucket->passiveCount--;
            }
        freeLink:
            Heap_Free(link);
            return 1;
        }
        link = link->next;
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0811B394);
#endif
