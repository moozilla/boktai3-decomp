#include "global.h"

#ifdef NONMATCHING
// fuzzy: 90.0%
// fix.py best version (reprinted C)
// foundry draft (worker best 81.6%); what differs is mostly register allocation and instruction order
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
s32 SpriteGroup_UpdateEntries(struct SpriteGroup *input)
{
    struct SpriteGroup *pool = input;
    struct SpriteGroupEntry *slot;
    s32 i;
    {
        u32 *d1 = (u32 *)(((u8 *)pool) + 0x214);
        if (d1[1]) {
            u32 *used;
            void **table;
            i = 0;
            used = &pool->used;
            slot = pool->entries;
            table = gUnk_086106D4;
            for (; i < 8; slot++, i++) {
                if (Occupied(used, 1u << i))
                    sub_08249240(pool, slot, table[slot->state]);
            }
        }
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", SpriteGroup_UpdateEntries);
#endif
