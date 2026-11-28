#include "effects/bursts/fragment_effect.h"

extern void (*const gUnk_086107E8[])(void);

u32 FragmentEffect_UpdateManager(struct FragmentEffectPool *pool)
{
    struct Fragment *fragments = pool->fragments;
    s32 i;

    if (pool->occupied) {
        for (i = 0; i < 8; i++) {
            if (FragmentEffectPool_IsOccupied(pool, i))
                sub_08249240(pool, &fragments[i], gUnk_086107E8[fragments[i].state]);
        }
    }
    return 0;
}
