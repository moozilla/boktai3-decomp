#include "spatial_index.h"

void Collider_ScheduleUpdates(void *unused, struct SpatialBucket *bucket, s32 bucketIndex)
{
    struct SpatialLink *link;
    s32 enabled, quota, count, divisor, delay;
    u32 one;
    u32 mask = 0x800;
    if ((gEngineFlags | gScenePauseFlags) & mask)
        return;
    count = bucket->passiveCount;
    if (count <= 0)
        return;
    link = bucket->passive;
    enabled = 0;
    if (count <= 4)
        quota = 1;
    else {
        if (count <= 9) {
            quota = 3;
            count += 2;
            divisor = 3;
        } else {
            quota = 4;
            count += 3;
            divisor = 4;
        }
        count = Div(count, divisor);
    }
    do {
        delay = count - 1;
    } while (0);
    if (link) {
        one = 1;
        do {
            struct SpatialEntry *collider = link->entry;
            if (collider->flags & one) {
                collider->flags |= 2;
                collider->updateDelay = 0;
            } else if (enabled >= quota) {
                collider->flags |= 2;
            } else if (collider->updateDelay > 0) {
                collider->flags |= 2;
                collider->updateDelay--;
            } else {
                collider->updateDelay = delay;
                collider->flags &= ~2;
                enabled++;
            }
            link = link->next;
        } while (link);
    }
}

void Collider_OnContactDefault(void *owner, struct SpatialEntry *other)
{
}

void Collider_SetContactCallback(struct SpatialEntry *entry, SpatialContactCallback callback,
                                 void *owner)
{
    entry->owner = owner;
    entry->callback = callback;
}

void Collider_SetExtents(struct SpatialEntry *entry, s32 x, s32 y, s32 z)
{
    entry->extents.x = x;
    entry->extents.y = y;
    entry->extents.z = z;
}

static inline s8 SpatialIndex_ReadAttachedBucket(struct SpatialEntry *entry, s8 **bucketField)
{
    s8 *field = &entry->bucket;
    s8 current = *field;
    *bucketField = field;
    return current;
}

static inline u8 SpatialIndex_NeedsReinsert(struct SpatialEntry *entry, s32 bucket)
{
    s8 *field;
    s8 previous;
    if (!(entry->registrationFlags & 1))
        return 0;
    if (entry->flags & 8)
        return 0;
    previous = SpatialIndex_ReadAttachedBucket(entry, &field);
    if (previous < 0 || bucket == *field || !SpatialIndex_RemoveEntry(entry))
        return 0;
    return 1;
}

void SpatialIndex_SetPosition(struct SpatialEntry *entry, s32 x, s32 y, s32 z)
{
    struct SpatialIndex *index = gSpatialIndex;
    s32 bucket;
    if (!index)
        return;
    entry->screen.x = entry->position.x - ((struct TilemapPanel *)gSceneCamera)->scroll.x;
    entry->screen.z = entry->position.z - ((struct TilemapPanel *)gSceneCamera)->scroll.y;
    if (!index->bucketWidth)
        bucket = 0;
    else
        bucket = Div(x + entry->offset.x, index->bucketWidth);
    if (bucket > 9)
        bucket = 9;
    if (SpatialIndex_NeedsReinsert(entry, bucket)) {
        entry->position.x = x;
        entry->position.y = y;
        entry->position.z = z;
        entry->bucket = bucket;
        SpatialIndex_InsertEntry(entry);
    } else {
        entry->position.x = x;
        entry->position.y = y;
        entry->position.z = z;
        entry->bucket = bucket;
    }
}

void Collider_SetFixedPositionWithYOffset(struct SpatialEntry *entry, u32 position, u32 yOffset)
{
    s16 coordinates[3];
    s16 *positionValues = coordinates;
    Position_GetHeightAdjustedCoordinates(position, yOffset, positionValues);
    SpatialIndex_SetPosition(entry, coordinates[0], positionValues[1], positionValues[2]);
}

void Collider_SetOffset(struct SpatialEntry *entry, const struct ContactPosition *offset)
{
    entry->offset.x = offset->x;
    entry->offset.y = offset->y;
    entry->offset.z = offset->z;
}

void Collider_SetCategory(struct SpatialEntry *entry, u32 mask)
{
    entry->mask = mask;
}

void Collider_SetContactParameter(struct SpatialEntry *entry, u32 kind)
{
    entry->kind = kind;
}

void SpatialIndex_InitEntry(struct SpatialEntry *entry, struct ContactPosition *position,
                            struct ContactPosition *bounds, struct ContactPosition *offset,
                            u32 flags, u32 mask, u32 category, u32 kind, u32 parameter,
                            SpatialContactCallback callback, void *owner)
{
    entry->bucket = -1;
    if (!callback)
        Collider_SetContactCallback(entry, Collider_OnContactDefault, owner);
    else
        Collider_SetContactCallback(entry, callback, owner);
    SpatialIndex_SetPosition(entry, position->x, position->y, position->z);
    Collider_SetExtents(entry, bounds->x, bounds->y, bounds->z);
    Collider_SetOffset(entry, offset);
    entry->flags |= flags;
    Collider_SetCategory(entry, mask);
    entry->parameter = parameter;
    entry->category = category;
    Collider_SetContactParameter(entry, kind);
}

void SpatialIndex_InsertEntry(struct SpatialEntry *entry)
{
    struct SpatialIndex *index = gSpatialIndex;
    struct SpatialBucket *bucket;
    struct SpatialLink *link;
    s32 column;
    if (!index)
        return;
    column = Div(entry->position.x + entry->offset.x, index->bucketWidth);
    if (column > 9)
        column = 9;
    bucket = &index->buckets[column];
    link = Heap_Alloc(sizeof(struct SpatialLink));
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

u8 SpatialIndex_RemoveEntry(struct SpatialEntry *entry)
{
    struct SpatialIndex *index = gSpatialIndex;
    struct SpatialBucket *matchBucket;
    struct SpatialBucket *bucket;
    struct SpatialLink *link;
    s32 column;
    if (!index)
        return 0;
    column = Div(entry->position.x + entry->offset.x, index->bucketWidth);
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
