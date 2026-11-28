#include "effects/bursts/fragment_burst.h"

struct FragmentBurst *FragmentBurst_AllocateSlot(struct FragmentBurstPool *);

struct FragmentBurst *FragmentBurst_AllocateSpriteGroup(struct FragmentBurstPool *pool)
{
    struct FragmentBurst *burst = FragmentBurst_AllocateSlot(pool);

    if (!burst)
        return NULL;
    return burst;
}
