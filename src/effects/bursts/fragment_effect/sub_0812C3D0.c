#include "effects/bursts/fragment_effect.h"

struct Fragment *sub_0812C3D0(struct FragmentEffectPool *pool)
{
    s32 i;
    struct Fragment *fragment;

    for (i = 0; i < 8; i++) {
        if (!FragmentEffectPool_IsOccupied(pool, i)) {
            pool->occupied |= 1 << i;
            fragment = &pool->fragments[i];
            fragment->slot = i;
            return fragment;
        }
    }
    return 0;
}
