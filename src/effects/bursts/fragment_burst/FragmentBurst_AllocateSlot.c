#include "effects/bursts/fragment_burst.h"

struct FragmentBurst *FragmentBurst_AllocateSlot(struct FragmentBurstPool *pool)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        if (!FragmentBurstPool_IsOccupied(pool, i)) {
            struct FragmentBurst *burst = &pool->bursts[i];

            pool->occupied |= 1 << i;
            Memory_Clear(burst, sizeof(*burst));
            burst->slot = i;
            return burst;
        }
    }
    return 0;
}
