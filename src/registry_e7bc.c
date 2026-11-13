#include "registry_e7bc.h"

s32 Bounds_TestOverlap(struct BoundsPosition *position, struct ContactPosition *extents,
                       struct BoundsPosition *otherPosition, struct ContactPosition *otherExtents)
{
    s32 extent = extents->x + otherExtents->x;
    if (position->x - extent > otherPosition->x || otherPosition->x - extent > position->x)
        goto separated;
    extent = extents->y + otherExtents->y;
    if (position->y - extent > otherPosition->y || otherPosition->y - extent > position->y)
        goto separated;
    extent = extents->z + otherExtents->z;
    if (position->z - extent > otherPosition->z || otherPosition->z - extent > position->z)
        goto separated;
    return 1;
separated:
    return -1;
}

static inline u32 SpatialIndex_TestFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}

void Collision_DispatchOverlaps(struct SpatialEntry *body, struct SpatialLink *link)
{
    struct BoundsPosition position, otherPosition;
    struct SpatialEntry *other;
    s32 overlap;

    position.x = body->position.x + body->offset.x;
    position.y = body->position.y + body->offset.y;
    position.z = body->position.z + body->offset.z;
    while (link) {
        other = link->entry;
        if (SpatialIndex_TestFlags(&other->flags, 3) || other == body ||
            other->mask == body->mask || other->hit == 1) {
            link = link->next;
            continue;
        }
        otherPosition.x = other->position.x + other->offset.x;
        otherPosition.y = other->position.y + other->offset.y;
        otherPosition.z = other->position.z + other->offset.z;
        overlap = Bounds_TestOverlap(&position, &body->extents, &otherPosition, &other->extents);
        if (overlap == 1 && (other->flags & body->flags)) {
            if (!(other->flags & 0x40))
                body->callback(body->owner, other);
            if (!(body->flags & 0x40)) {
                other->hit = overlap;
                other->hitTimer = 10;
                other->callback(other->owner, body);
            }
        }
        link = link->next;
    }
}

s32 Collision_ProcessGroups(struct SpatialIndex *manager)
{
    s32 index;
    for (index = 0; index < 10; index++) {
        struct SpatialLink *node;
        Collider_ScheduleUpdates(manager, &manager->buckets[index], index);
        node = manager->buckets[index].active;
        while (node) {
            struct SpatialLink *old;
            if (manager->buckets[index].passiveCount > 0)
                Collision_DispatchOverlaps(node->entry, manager->buckets[index].passive);
            old = node;
            node = node->next;
            Heap_Free(old);
        }
        manager->buckets[index].active = 0;
    }
    return 0;
}

s32 CategoryLists_Destroy(struct SpatialIndex *manager)
{
    s32 index = 0;
    do {
        struct SpatialLink *node = manager->buckets[index].passive;
        while (node) {
            struct SpatialLink *next = node->next;
            Heap_Free(node);
            node = next;
        }
        node = manager->buckets[index].active;
        while (node) {
            struct SpatialLink *next = node->next;
            Heap_Free(node);
            node = next;
        }
        index++;
    } while (index <= 9);
    gSpatialIndex = 0;
    return 0;
}

void Collider_InitManagerBuckets(struct SpatialIndex *manager)
{
    s32 index;
    s32 width;
    manager->unknownB8 = -1;
    index = 0;
    do {
        manager->buckets[index].active = 0;
        manager->buckets[index].passive = 0;
        manager->buckets[index].activeCount = 0;
        manager->buckets[index].passiveCount = 0;
        index++;
    } while (index <= 9);
    width = SceneTilemap_GetLayoutLength() << 5;
    if (width <= 0)
        width = 0x7fffffff;
    manager->bucketWidth = Div(width, 9);
}

s32 Collider_InitManager(struct SpatialIndex *manager)
{
    Collider_InitManagerBuckets(manager);
    gSpatialIndex = manager;
    return 0;
}

struct SpatialIndex *Collider_CreateManager(void)
{
    struct SpatialIndex *manager;
    if (gSpatialIndex != 0)
        return gSpatialIndex;
    manager = Task_Create(5, sizeof(struct SpatialIndex));
    if (manager != 0) {
        Task_SetCallbacks(manager, (void (*)(void *))Collision_ProcessGroups,
                          (void (*)(void *))CategoryLists_Destroy);
        if (Collider_InitManager(manager) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}

void RegistryE7BC_SetFlags(u32 *flags, u32 bits)
{
    *flags |= bits;
}

void RegistryE7BC_ClearFlags(u32 *flags, u32 bits)
{
    *flags &= ~bits;
}

void RegistryE7BC_OnObjectComplete(void *context, struct SpatialEntry *other)
{
    struct RegistryE7BCObject *object = context;
    if (object) {
        Script_Call(object->procedure, 0);
        object->active = 0;
    }
}

s32 RegistryE7BC_QueueActiveObjects(struct RegistryE7BCManager *manager)
{
    struct RegistryE7BCObject *object = manager->objects;
    s32 index;
    for (index = 0; index < 12; index++) {
        if (manager->objects[index].active == 1)
            SpatialIndex_InsertEntry(&object->collider);
        object++;
    }
    return 0;
}

void RegistryE7BC_OnManagerDestroy(void)
{
    gRegistryE7BCManager = 0;
}

s32 RegistryE7BC_InitManager(struct RegistryE7BCManager *manager)
{
    gRegistryE7BCManager = manager;
    return 0;
}

struct RegistryE7BCManager *RegistryE7BC_CreateManager(void)
{
    struct RegistryE7BCManager *manager = Task_Create(0x8, sizeof(struct RegistryE7BCManager));
    if (manager != 0) {
        Task_SetCallbacks(manager, (void (*)(void *))RegistryE7BC_QueueActiveObjects,
                          (void (*)(void *))RegistryE7BC_OnManagerDestroy);
        if (RegistryE7BC_InitManager(manager) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}

struct RegistryE7BCObject *RegistryE7BC_FindFreeObject(struct RegistryE7BCManager *manager)
{
    s32 index = 0;
    struct RegistryE7BCObject *object = manager->objects;
    struct RegistryE7BCObject *result;
    do {
        if (object->active == 1) {
            object++;
            index++;
        } else {
            result = object;
            goto end;
        }
    } while (index <= 11);
    result = 0;
end:
    return result;
}

static inline s32 RegistryE7BC_ReadOptionalValue(void)
{
    s32 value;
    if (Script_GetPc())
        value = Script_GetValue();
    else
        value = 0;
    return value;
}

void RegistryE7BC_CreateFromScript(void)
{
    struct RegistryE7BCManager *manager = gRegistryE7BCManager;
    struct RegistryE7BCObject *object;
    union RegistryE7BCPosition position;
    union RegistryE7BCPosition extents;
    union RegistryE7BCPosition offset;
    if (!manager) {
        manager = RegistryE7BC_CreateManager();
        if (!manager)
            return;
    }
    object = RegistryE7BC_FindFreeObject(manager);
    if (!object)
        return;
    object->procedure = Script_GetKeywordValue('P', 0);
    if (!Script_SeekToKeyword('p'))
        return;
    position.packed.x = RegistryE7BC_ReadOptionalValue();
    position.packed.y = RegistryE7BC_ReadOptionalValue();
    position.packed.z = RegistryE7BC_ReadOptionalValue();
    if (!Script_SeekToKeyword('s'))
        return;
    extents.packed.x = RegistryE7BC_ReadOptionalValue();
    extents.packed.y = RegistryE7BC_ReadOptionalValue();
    extents.packed.z = RegistryE7BC_ReadOptionalValue();
    if (!Script_SeekToKeyword('o'))
        return;
    offset.packed.x = RegistryE7BC_ReadOptionalValue();
    offset.packed.y = RegistryE7BC_ReadOptionalValue();
    offset.packed.z = RegistryE7BC_ReadOptionalValue();
    SpatialIndex_InitEntry(&object->collider, &position.coordinates, &extents.coordinates,
                           &offset.coordinates, 12, 7, 0, 6, 12, RegistryE7BC_OnObjectComplete,
                           object);
    SpatialIndex_InsertEntry(&object->collider);
    object->active = 1;
}

void RegistryE7BC_ResetManager(void)
{
    gRegistryE7BCManager = 0;
}

void GameState_SetResultIndex(s32 value)
{
    s32 clamped;
    (*(s16 *)(u32)&gGameState->unk890) = value;
    clamped = (*(s16 *)(u32)&gGameState->unk890);
    if (clamped > 0xb)
        clamped = 0xb;
    else if (clamped < 0)
        clamped = 0;
    (*(s16 *)(u32)&gGameState->unk890) = clamped;
}

void GameState_SetResultTimer(s32 value)
{
    s32 clamped;
    (*(s32 *)(u32)&gGameState->unk88C) = value;
    clamped = (*(s32 *)(u32)&gGameState->unk88C);
    if (clamped > 0xEA60)
        clamped = 0xEA60;
    else if (clamped < 0)
        clamped = 0;
    (*(s32 *)(u32)&gGameState->unk88C) = clamped;
}

void GameState_SetActionCount892(s32 value)
{
    s32 clamped;
    (*(s16 *)(u32)&gGameState->unk892) = value;
    clamped = (*(s16 *)(u32)&gGameState->unk892);
    if (clamped > 0x3E7)
        clamped = 0x3E7;
    else if (clamped < 0)
        clamped = 0;
    (*(s16 *)(u32)&gGameState->unk892) = clamped;
}

void GameState_SetActionCount894(s32 value)
{
    s32 clamped;
    (*(s16 *)(u32)&gGameState->unk894) = value;
    clamped = (*(s16 *)(u32)&gGameState->unk894);
    if (clamped > 0x3E7)
        clamped = 0x3E7;
    else if (clamped < 0)
        clamped = 0;
    (*(s16 *)(u32)&gGameState->unk894) = clamped;
}

s16 GameState_GetResultIndex(void)
{
    s32 clamped = (*(s16 *)(u32)&gGameState->unk890);
    if (clamped > 0xb)
        clamped = 0xb;
    else if (clamped < 0)
        clamped = 0;
    (*(s16 *)(u32)&gGameState->unk890) = clamped;
    return (*(s16 *)(u32)&gGameState->unk890);
}

s32 GameState_GetResultTimer(void)
{
    s32 clamped = (*(s32 *)(u32)&gGameState->unk88C);
    if (clamped > 0xEA60)
        clamped = 0xEA60;
    else if (clamped < 0)
        clamped = 0;
    (*(s32 *)(u32)&gGameState->unk88C) = clamped;
    return (*(s32 *)(u32)&gGameState->unk88C);
}

s16 GameState_GetActionCount892(void)
{
    s32 clamped = (*(s16 *)(u32)&gGameState->unk892);
    if (clamped > 999)
        clamped = 999;
    else if (clamped < 0)
        clamped = 0;
    (*(s16 *)(u32)&gGameState->unk892) = clamped;
    return (*(s16 *)(u32)&gGameState->unk892);
}

s16 GameState_GetActionCount894(void)
{
    s32 clamped = (*(s16 *)(u32)&gGameState->unk894);
    if (clamped > 0x3E7)
        clamped = 0x3E7;
    else if (clamped < 0)
        clamped = 0;
    (*(s16 *)(u32)&gGameState->unk894) = clamped;
    return (*(s16 *)(u32)&gGameState->unk894);
}

void GameState_AddActionCount892(s32 value)
{
    s32 clamped = (*(s16 *)(u32)&gGameState->unk892) + value;
    if (clamped > 0x3E7)
        clamped = 0x3E7;
    else if (clamped < 0)
        clamped = 0;
    (*(s16 *)(u32)&gGameState->unk892) = clamped;
}

void GameState_AddActionCount894(s32 value)
{
    s32 clamped = (*(s16 *)(u32)&gGameState->unk894) + value;
    if (clamped > 0x3E7)
        clamped = 0x3E7;
    else if (clamped < 0)
        clamped = 0;
    (*(s16 *)(u32)&gGameState->unk894) = clamped;
}

void GameState_ResetActionCounts(void)
{
    GameState_SetActionCount892(0);
    GameState_SetActionCount894(0);
}
