#define SpritePool12_UpdateActive sub_0812808C
#define gSpritePool12Callbacks gUnk_08610784
#include "global.h"
struct PoolSprite {
    u8 unknown[0x9A];
    u8 state;
    u8 unknown9B[5];
};
struct SpritePool {
    u8 unknown[0x38];
    u32 activeMask;
    struct PoolSprite sprites[12];
};
extern void *gUnk_08610784[];
s32 sub_08249240(void *, void *, void *);
static inline u8 IsActive(u32 *mask, s32 slot)
{
    if (*mask & (1 << slot))
        return TRUE;
    return FALSE;
}
s32 sub_0812808C(struct SpritePool *pool)
{
    s32 i = 0;
    void **callbacks = gUnk_08610784;
    u8 *state = &pool->sprites[0].state;
    struct PoolSprite *sprite = pool->sprites;
    do {
        if (IsActive(&pool->activeMask, i))
            sub_08249240(pool, sprite, callbacks[*state]);
        state += 0xA0;
        sprite++;
        i++;
    } while (i < 12);
    return 0;
}
