#include "global.h"
#include "ram.h"

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
s32 Div(s32, s32);
void Heap_Free(void *);
u8 SpatialIndex_RemoveEntry(struct SpatialEntry *entry)
{
    struct SpatialIndex *index = gSpatialIndex;
    struct SpatialBucket *matchBucket;
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
            matchBucket = bucket;
            entry->registrationFlags &= 0xfe;
            previous = link->previous;
            if (!previous) {
                if (link->next) {
                    link->next->previous = previous;
                    if (entry->flags & 8) {
                        matchBucket->active = link->next;
                        goto activeRemoved;
                    } else if (entry->flags & 16) {
                        matchBucket->passive = link->next;
                        goto passiveRemoved;
                    }
                    goto freeLink;
                } else {
                    Heap_Free(link);
                    if (entry->flags & 8) {
                        matchBucket->active = previous;
                        matchBucket->activeCount--;
                    } else if (entry->flags & 16) {
                        matchBucket->passive = previous;
                        matchBucket->passiveCount--;
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
                matchBucket->activeCount--;
            } else if (entry->flags & 16) {
            passiveRemoved:
                matchBucket->passiveCount--;
            }
        freeLink:
            Heap_Free(link);
            return 1;
        }
        link = link->next;
    }
    return 0;
}
