#include "effects/bursts/spiral_burst.h"

void SpiralBurst_SetState(struct SpiralBurst *burst, void (*state)(struct SpiralBurst *))
{
    burst->state = state;
    burst->timer = 0;
}
