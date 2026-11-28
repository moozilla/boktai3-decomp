#include "effects/bursts/radial_burst.h"

void RadialBurstEffect_RemoveParticles(struct RadialBurst *burst)
{
    s32 i;

    for (i = 0; i < 12; i++)
        Effect_Remove(&burst->particles[i]);
}
