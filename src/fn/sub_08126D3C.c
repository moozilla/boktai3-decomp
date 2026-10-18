#define SpritePool10_AllocateSlot sub_08126D3C
#include "global.h"
struct PoolSprite {
    u8 unknown[0xC1];
    u8 slot;
    u8 unknownC2[0x12];
};
struct SpritePool {
    u8 unknown[0x18];
    struct PoolSprite sprites[10];
    u32 activeMask;
};
static inline u8 IsActive(u32 *mask, s32 slot)
{
    if (*mask & (1 << slot))
        return TRUE;
    return FALSE;
}
struct PoolSprite *sub_08126D3C(struct SpritePool *pool)
{
    s32 i = 0;
    for (; i < 10; i++) {
        if (!IsActive(&pool->activeMask, i)) {
            struct PoolSprite *sprite = &pool->sprites[i];
            pool->activeMask |= 1 << i;
            sprite->slot = i;
            return sprite;
        }
    }
    return NULL;
}
