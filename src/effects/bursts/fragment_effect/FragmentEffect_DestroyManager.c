#include "effects/bursts/fragment_effect.h"

u32 FragmentEffect_DestroyManager(struct FragmentEffectPool *pool)
{
    s32 i;

    for (i = 0; i < 8; i++) {
        if (FragmentEffectPool_IsOccupied(pool, i))
            Sprite_Remove(&pool->fragments[i]);
    }
    gFragmentEffectManager = 0;
    return 0;
}
