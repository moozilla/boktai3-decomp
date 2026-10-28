#include "global.h"
#include "runtime.h"
#include "resource_registry.h"
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
