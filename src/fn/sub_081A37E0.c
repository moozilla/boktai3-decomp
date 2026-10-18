#include "global.h"
/* Effect pool family names and field interpretations are tentative. */
#define gEffectPool gUnk_0200023C
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
extern struct EffectPool *gUnk_0200023C;
struct EffectPoolEntry *EffectPool_FindIdleActiveEntry(void)
{
    struct EffectPoolEntry *entry = gEffectPool->entries;
    s32 i;
    if (gEffectPool->count <= 2)
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
