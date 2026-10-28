#include "global.h"
#include "ram.h"
/* Effect pool family names and field interpretations are tentative. */
struct EffectPoolEntry {
    u32 flags;
    u8 unknown4[0x15a];
    u16 state;
    u8 unknown160[0x18];
};
struct EffectPool {
    u8 unknown0[0x3c];
    u32 count;
    u8 unknown40[8];
    struct EffectPoolEntry entries[3];
};
struct EffectPoolEntry *EffectPool_FindIdleActiveEntry(void)
{
    struct EffectPoolEntry *entry = ((struct EffectPool *)gEffectPoolManager)->entries;
    s32 i;
    if (((struct EffectPool *)gEffectPoolManager)->count <= 2)
        goto scan;
    goto fail;
found:
    return entry;
scan:
    i = 0;
    do {
        if ((entry->flags & 1) && entry->state == 0)
            goto found;
        entry++;
        i++;
    } while (i <= 2);
fail:
    return NULL;
}
