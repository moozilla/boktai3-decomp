#ifndef GUARD_RESOURCE_REGISTRY_H
#define GUARD_RESOURCE_REGISTRY_H

#include "global.h"

/* RAM objects; each row of symbols/ram.csv with header include/resource_registry.h. */

struct ResourceRegistryEntry {
    u16 id;
    u8 flags;
    u8 unk3;
    void *data;
};
extern struct ResourceRegistryEntry gResourceRegistryEntries[32];

#define RESOURCE_REGISTRY_ENTRIES_ADDR 0x0203B000 /* gResourceRegistryEntries */

#endif
