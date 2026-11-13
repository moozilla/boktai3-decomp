#include "shadow_registry.h"

struct ShadowRegistry *ShadowRegistry_GetTask(void)
{
    struct ShadowRegistry *registry = (void *)gShadowRegistry;
    if (registry == 0)
        return ShadowRegistry_CreateTask();
    return registry;
}

struct ShadowRegistryEntry *ShadowRegistry_FindEntry(struct ShadowRegistry *registry, u32 id)
{
    struct ShadowRegistryEntry *entry = registry->head;
    while (entry) {
        if (entry->id == id)
            return entry;
        entry = entry->next;
    }
    return 0;
}

s32 ShadowRegistry_LinkEntry(struct ShadowRegistry *registry, struct ShadowRegistryEntry *entry)
{
    u32 active = entry->mode;
    struct ShadowRegistryEntry *first;
    s32 result;
    if (active != 0) {
        result = -1;
    } else {
        entry->previous = (void *)active;
        first = registry->head;
        entry->next = first;
        if (first) {
            first->previous = entry;
        }
        registry->head = entry;
        entry->mode = 1;
        result = 0;
    }
    return result;
}

s32 ShadowRegistry_UnlinkEntry(struct ShadowRegistry *registry, struct ShadowRegistryEntry *entry)
{
    struct ShadowRegistryEntry *previous = entry->previous;
    struct ShadowRegistryEntry *next = entry->next;
    s32 result;
    if (entry->mode == 0) {
        return -1;
    }
    if (previous) {
        previous->next = next;
    } else {
        registry->head = next;
    }
    if (next) {
        next->previous = previous;
    }
    result = 0;
    entry->mode = result;
    return result;
}

u32 ShadowRegistry_OnUpdate(void)
{
    return 0;
}

u32 ShadowRegistry_OnDestroy(struct ShadowRegistry *registry)
{
    struct ShadowRegistryEntry *entry = registry->head;
    struct ShadowRegistryEntry *next;
    while (entry != 0) {
        next = entry->next;
        Shadow_Destroy(&entry->shadow);
        ShadowRegistry_UnlinkEntry(registry, entry);
        Heap_Free(entry);
        entry = next;
    }
    {
        u32 zero = 0;
        registry->head = (void *)zero;
        gShadowRegistry = zero;
    }
    return 0;
}

s32 ShadowRegistry_InitTask(struct ShadowRegistry *registry)
{
    gShadowRegistry = (u32)registry;
    registry->head = 0;
    return 0;
}

struct ShadowRegistry *ShadowRegistry_CreateTask(void)
{
    struct ShadowRegistry *registry;
    if (gShadowRegistry != 0)
        return (void *)gShadowRegistry;
    registry = Task_Create(9, sizeof(*registry));
    if (registry != 0) {
        Task_SetCallbacks(registry, (void *)ShadowRegistry_OnUpdate,
                          (void *)ShadowRegistry_OnDestroy);
        if (ShadowRegistry_InitTask(registry) < 0) {
            Task_Kill(registry);
            return 0;
        }
    }
    return registry;
}

s32 ShadowRegistry_Create(s32 id, s32 mode, struct SceneVec *position)
{
    struct ShadowRegistry *registry = ShadowRegistry_GetTask();
    struct ShadowRegistryEntry *entry;
    struct SceneVec *destination;
    if (!registry)
        return -1;
    if (ShadowRegistry_FindEntry(registry, id))
        return -1;
    entry = Heap_Alloc(sizeof(*entry));
    if (!entry)
        return -1;
    Memory_Clear(entry, sizeof(*entry));
    entry->id = id;
    destination = &entry->position;
    entry->position = *position;
    if (Shadow_Initialize(&entry->shadow, destination, mode == 2) < 0)
        goto fail;
    if (ShadowRegistry_LinkEntry(registry, entry) < 0)
        goto fail;
    entry->mode = mode;
    return 0;
fail:
    Heap_Free(entry);
    return -1;
}

s32 ShadowRegistry_CreateFromScript(void)
{
    struct ShadowScriptPosition position;
    s32 result;
    s32 id = Script_GetKeywordValue('n', 0);
    s32 mode = Script_GetKeywordValue('m', 1);
    if (!Script_SeekToKeyword('p')) {
        result = -1;
    } else {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
        result = ShadowRegistry_Create(id, mode, (void *)&position);
    }
    return result;
}

s32 ShadowRegistry_Remove(s32 id)
{
    struct ShadowRegistry *registry = ShadowRegistry_GetTask();
    struct ShadowRegistryEntry *entry;
    if (!registry)
        return -1;
    entry = ShadowRegistry_FindEntry(registry, id);
    if (!entry)
        return -1;
    Shadow_Destroy(&entry->shadow);
    ShadowRegistry_UnlinkEntry(registry, entry);
    Heap_Free(entry);
    return 0;
}

s32 ShadowRegistry_RemoveFromScript(void)
{
    return ShadowRegistry_Remove(Script_GetKeywordValue('n', 0));
}

s32 ShadowRegistry_SetEntryMode(u32 id, u32 mode)
{
    struct ShadowRegistry *registry = ShadowRegistry_GetTask();
    struct ShadowRegistryEntry *entry;
    if (registry == 0 || (entry = ShadowRegistry_FindEntry(registry, id)) == 0)
        return -1;
    entry->mode = mode;
    if ((u8)mode == 1)
        ActorShadow_Enable(&entry->shadow);
    else if ((u8)mode == 2)
        ActorShadow_Disable(&entry->shadow);
    return 0;
}

s32 ShadowRegistry_SetModeFromScript(void)
{
    u32 id = Script_GetKeywordValue('n', 0);
    u32 mode = Script_GetKeywordValue('m', 1);
    return ShadowRegistry_SetEntryMode(id, mode);
}

s32 ShadowRegistry_SetEntryPosition(u32 id, struct SceneVec *position)
{
    struct ShadowRegistry *registry = ShadowRegistry_GetTask();
    struct ShadowRegistryEntry *entry;
    if (registry != 0) {
        entry = ShadowRegistry_FindEntry(registry, id);
        if (entry != 0) {
            entry->position = *position;
            return 0;
        }
    }
    return -1;
}

s32 ShadowRegistry_SetPositionFromScript(void)
{
    struct ShadowScriptPosition position;
    u32 id = Script_GetKeywordValue('n', 0);
    s32 result;
    if (Script_SeekToKeyword('p') == 0) {
        result = -1;
    } else {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
        result = ShadowRegistry_SetEntryPosition(id, (void *)&position);
    }
    return result;
}

void ParticleManager_ClearTaskPtr(void)
{
    gParticleGroupManager = 0;
}
