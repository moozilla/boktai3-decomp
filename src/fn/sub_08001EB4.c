#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "background_controller.h"

s32 Background_FindMapId(struct BackgroundResource *resource, u16 id)
{
    register struct BackgroundMapEntry *entry asm("r2") =
        (struct BackgroundMapEntry *)resource->maps;
    s32 i;
    for (i = 0; i < resource->count; i++) {
        if (entry->id == id)
            return i;
        entry++;
    }
    return 0;
}
