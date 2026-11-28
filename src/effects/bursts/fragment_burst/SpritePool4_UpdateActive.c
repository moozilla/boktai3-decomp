#include "effects/bursts/fragment_burst.h"

// confidence=low: TimedSpritePool_UpdateActive for a pool of four 0x160-byte sprites.
extern void *gUnk_08610790[];

s32 SpritePool4_UpdateActive(struct FragmentBurstPool *pool)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        if (FragmentBurstPool_IsOccupied(pool, i))
            sub_08249240(pool, &pool->bursts[i], gUnk_08610790[pool->bursts[i].state]);
    }
    return 0;
}
