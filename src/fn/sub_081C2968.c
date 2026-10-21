#include "global.h"
/* Tentative particle pool: caller clears bit zero and increments the count. */
struct ParticleEntry {
    u32 flags;
    u8 unknown4[0x34];
};
struct ParticlePool {
    u8 unknown0[0x18];
    struct ParticleEntry entries[32];
    u8 unknown718[0x60];
    s32 count;
};
struct ParticleEntry *ParticlePool_FindFreeEntry(struct ParticlePool *p)
{
    s32 i, offset;
    u32 mask;
    struct ParticlePool *cursor;
    if (p->count <= 31)
        goto search;
    goto fail;
found:
    return (void *)((u8 *)p + (offset + 0x18));
search:
    i = 0;
    mask = 1;
    cursor = p;
    offset = 0;
    for (; i <= 31; i++) {
        if (cursor->entries[0].flags & mask)
            goto found;
        cursor = (void *)((u8 *)cursor + 0x38);
        offset += 0x38;
    }
fail:
    return 0;
}
