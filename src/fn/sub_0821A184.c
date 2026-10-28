#include "global.h"
#include "runtime.h"
#include "resource_registry.h"
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
