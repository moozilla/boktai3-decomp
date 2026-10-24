#include "global.h"

struct SpriteNotice {
    u8 unknown0[0x43];
    s8 index;
    u8 unknown44[4];
};
struct SpriteNoticePool {
    u8 unknown0[0x20];
    struct SpriteNotice slots[32];
    u32 occupied;
};
static inline u8 Occupied(u32 *mask, s32 index)
{
    if (*mask & (1 << index))
        return 1;
    return 0;
}
struct SpriteNotice *SpriteNotice_AllocateSlot(struct SpriteNoticePool *pool)
{
    s32 i;
    for (i = 0; i < 32; i++) {
        if (!Occupied(&pool->occupied, i)) {
            struct SpriteNotice *slot = &pool->slots[i];
            slot->index = i;
            pool->occupied |= 1 << i;
            return slot;
        }
    }
    return 0;
}
