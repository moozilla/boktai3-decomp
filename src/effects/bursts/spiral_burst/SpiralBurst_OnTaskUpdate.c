#include "effects/bursts/spiral_burst.h"

u32 SpiralBurst_OnTaskUpdate(struct SpiralBurst *burst)
{
    Callback_InvokeActor(burst, burst->state);
    return 0;
}
