#include "global.h"

struct SpriteGroupEntry {
    u8 pad[10];
    u8 state;
    s8 slot;
};
struct SpriteGroup {
    u8 pad[0x1c];
    u8 sprites[0x1fc];
    u32 used;
    struct SpriteGroupEntry entries[8];
};
extern struct SpriteGroup *volatile gSpriteGroup;
void Sprite_Remove(void *);
static inline u8 Occupied(u32 *used, u32 mask)
{
    if (*used & mask)
        return 1;
    return 0;
}
s32 SpriteGroup_RemoveSprites(struct SpriteGroup *pool)
{
    s32 i = 0;
    u32 zero;
    u32 *used = &pool->used;
    struct SpriteGroupEntry *entry = pool->entries;
    for (; i < 8; entry++, i++) {
        if (Occupied(used, 1u << i))
            Sprite_Remove(pool->sprites + entry->slot * 0x2c);
    }
    zero = 0;
    gSpriteGroup = (void *)zero;
    return 0;
}
