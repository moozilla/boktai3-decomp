#include "global.h"

#define gSpriteGroupEntryUpdates gUnk_086106D4
struct SpriteGroupEntry {
    u8 pad[10];
    u8 state;
    s8 slot;
};
struct SpriteGroup {
    u8 pad[0x218];
    u32 used;
    struct SpriteGroupEntry entries[8];
};
extern void *gUnk_086106D4[];
s32 sub_08249240(void *, void *, void *);
static inline u8 Occupied(u32 *used, u32 mask)
{
    if (*used & mask)
        return 1;
    return 0;
}
static inline u32 *SlotMask(struct SpriteGroup *pool)
{
    return &pool->used;
}
s32 SpriteGroup_UpdateEntries(struct SpriteGroup *input)
{
    struct SpriteGroup *pool = input;
    struct SpriteGroupEntry *slot;
    s32 i;
    u32 *used;
    void **table;
    if (!pool->used)
        return 0;

    i = 0;
    used = SlotMask(pool);
    slot = pool->entries;
    table = gUnk_086106D4;
    for (; i < 8; slot++, i++) {
        if (Occupied(used, 1u << i))
            sub_08249240(pool, slot, table[slot->state]);
    }
    return 0;
}
