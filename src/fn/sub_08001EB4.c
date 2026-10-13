#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct MapEntry {
    u32 a;
    u16 id;
    u16 b;
    u32 c;
};
struct MapResource {
    u32 magic;
    u16 count;
    u8 pad[6];
    struct MapEntry *entries;
};
s32 Background_FindMapId(struct MapResource *resource, u16 id)
{
    register struct MapEntry *entry asm("r2") = resource->entries;
    s32 i;
    for (i = 0; i < resource->count; i++) {
        if (entry->id == id)
            return i;
        entry++;
    }
    return 0;
}
