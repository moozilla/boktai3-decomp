#include "resource_registry.h"
#include "runtime.h"

struct ResourceRegistryEntry *ResourceRegistry_AllocateEntry(void)
{
    s32 i;
    struct ResourceRegistryEntry *entry;
    do {
        for (entry = gResourceRegistryEntries, i = 0; i < gResourceRegistryCount; i++, entry++) {
            do {
                if (!(entry->flags & 0x80))
                    return entry;
            } while (0);
        }
        gResourceRegistryCount++;
        if (gResourceRegistryCount > 31)
            return 0;
    } while (0);
    return entry;
}

struct ResourceRegistryEntry *ResourceRegistry_FindEntry(u16 id)
{
    struct ResourceRegistryEntry *s = gResourceRegistryEntries;
    s32 i = 0;
    for (; i < gResourceRegistryCount; i++, s++) {
        if ((s->flags & 0x80) != 0 && s->id == id)
            return s;
    }
    return 0;
}

void ResourceRegistry_Initialize(void)
{
    gResourceRegistryCount = 0;
}

void Heap_Free(void *);

void ResourceRegistry_ClearEntries(u32 keep)
{
    s32 retained = 0;
    struct ResourceRegistryEntry *entry = gResourceRegistryEntries;
    s32 i = 0;
    while (i < gResourceRegistryCount) {
        if (entry->flags & 0x80) {
            if ((entry->flags & 1) && !(keep & 1))
                retained = i;
            else {
                if (entry->flags & 2)
                    Heap_Free(entry->data);
                entry->id = 0;
                entry->flags = 0;
            }
        }
        i++;
        entry++;
    }
    gResourceRegistryCount = retained + 1;
}

struct ResourceRegistryEntry *ResourceRegistry_AllocateEntry(void);

void ResourceRegistry_RegisterResource(u32 id, void *data, u32 flags)
{
    void *saved_data = data;
    s32 saved_flags = flags;
    u16 saved_id = id;
    struct ResourceRegistryEntry *entry = ResourceRegistry_AllocateEntry();
    if (entry) {
        entry->id = saved_id;
        entry->data = saved_data;
        entry->flags = saved_flags | (s8)0x80;
    }
}

struct ResourceRegistryEntry *ResourceRegistry_FindEntry(u16);

void ResourceRegistry_UnregisterResource(u16 id)
{
    struct ResourceRegistryEntry *s = ResourceRegistry_FindEntry(id);
    if (s != 0)
        s->flags = 0;
}

void *ResourceRegistry_GetResource(u16 id)
{
    struct ResourceRegistryEntry *s = ResourceRegistry_FindEntry(id);
    if (s == 0)
        return 0;
    return s->data;
}
