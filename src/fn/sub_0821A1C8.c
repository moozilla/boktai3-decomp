#include "global.h"
#include "runtime.h"
#include "resource_registry.h"


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
