#include "global.h"
// confidence=low: SpritePool12_UpdateActive for a pool of four 0x160-byte sprites.
struct PoolSprite4 {
    u8 unknown[0x156];
    u8 state;
    u8 unknown157[0x160 - 0x157];
};
struct SpritePool4 {
    u8 unknown[0x38];
    u32 activeMask;
    struct PoolSprite4 sprites[4];
};
extern void *gUnk_08610790[];
s32 sub_08249240(void *, void *, void *);
static inline u8 IsActive(u32 *mask, s32 slot)
{
    if (*mask & (1 << slot))
        return TRUE;
    return FALSE;
}
#define SpritePool4_UpdateActive sub_08128A00
s32 SpritePool4_UpdateActive(struct SpritePool4 *pool)
{
    s32 i = 0;
    void **callbacks = gUnk_08610790;
    u8 *state = &pool->sprites[0].state;
    struct PoolSprite4 *sprite = pool->sprites;
    do {
        if (IsActive(&pool->activeMask, i))
            sub_08249240(pool, sprite, callbacks[*state]);
        state += sizeof(struct PoolSprite4);
        sprite++;
        i++;
    } while (i < 4);
    return 0;
}
