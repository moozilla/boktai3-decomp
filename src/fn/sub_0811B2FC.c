/* Provisional family and semantic names; inferred from static accesses and callers. */
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
void *Heap_Alloc(u32);
void SpatialIndex_InsertEntry(struct SpatialEntry *entry)
{
    struct SpatialIndex *index = gSpatialIndex;
    struct SpatialBucket *bucket;
    struct SpatialLink *link;
    s32 column;
    if (!index)
        return;
    column = Div(entry->x + entry->offsetX, index->bucketWidth);
    if (column > 9)
        column = 9;
    bucket = &index->buckets[column];
    link = Heap_Alloc(12);
    link->previous = 0;
    link->entry = entry;
    if (entry->flags & 8) {
        struct SpatialLink *head = bucket->active;
        if (!head)
            link->next = 0;
        else {
            link->next = head;
            head->previous = link;
        }
        bucket->active = link;
        entry->registrationFlags |= 1;
        bucket->activeCount++;
    } else if (entry->flags & 16) {
        struct SpatialLink *head = bucket->passive;
        if (!head)
            link->next = 0;
        else {
            link->next = head;
            head->previous = link;
        }
        bucket->passive = link;
        entry->registrationFlags |= 1;
        bucket->passiveCount++;
    }
}
