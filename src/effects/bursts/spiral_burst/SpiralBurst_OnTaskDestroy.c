#include "effects/bursts/spiral_burst.h"

u32 SpiralBurst_OnTaskDestroy(struct SpiralBurst *burst)
{
    s32 i;

    for (i = 0; i < 8; i++)
        Effect_Remove(&burst->particles[i]);
    return 0;
}
