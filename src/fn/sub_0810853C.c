#include "global.h"
/* Provisional names and field interpretations; confidence low. */
#define gNamedEntryPool gUnk_020001BC
struct NamedEntry {
    u8 pad0[0x58];
    u32 name;
    u8 pad5C[0xC];
};
struct NamedEntryPool {
    u8 pad0[0x1C];
    struct NamedEntry entries[3];
    u32 used;
};
extern struct NamedEntryPool *gUnk_020001BC;
static inline u8 IsUsed(u32 mask, s32 index, s32 one)
{
    if (mask & (one << index))
        return 1;
    return 0;
}
struct NamedEntry *Pool1BC_FindByName(u32 name)
{
    struct NamedEntryPool *pool = gUnk_020001BC;
    struct NamedEntry *entry;
    s32 i;
    u32 used;
    s32 one;
    if (pool)
        goto search;
    goto fail;
found:
    return entry;
search:
    i = 0;
    used = pool->used;
    one = 1;
    entry = pool->entries;
    do {
        if (IsUsed(used, i, one) && entry->name == name)
            goto found;
        entry++;
        i++;
    } while (i < 3);
fail:
    return 0;
}
