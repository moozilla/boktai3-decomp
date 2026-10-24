// MATCHING_CONCESSION: register-pinning
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
s32 SpriteGroup_UpdateEntries(struct SpriteGroup *input)
{
    struct SpriteGroup *pool = input;
    struct SpriteGroupEntry *slot;
    s32 i;
    register u32 maskOffset asm("r1") = (u32) & ((struct SpriteGroup *)0)->used;
    register u32 *guard asm("r0");
    asm("" : "+r"(maskOffset));
    guard = (void *)((u32)pool + maskOffset);
    if (*guard) {
        u32 *used;
        void **table;
        i = 0;
        {
            u32 offset = (u32) & ((struct SpriteGroup *)0)->used;
            used = (void *)((u32)pool + offset);
        }
        {
            register u32 offset asm("r1") = (u32) & ((struct SpriteGroup *)0)->entries;
            asm("" : "+r"(offset));
            slot = (void *)((u32)pool + offset);
        }
        table = gUnk_086106D4;
        for (; i < 8; slot++, i++) {
            if (Occupied(used, 1u << i))
                sub_08249240(pool, slot, table[slot->state]);
        }
    }
    return 0;
}
