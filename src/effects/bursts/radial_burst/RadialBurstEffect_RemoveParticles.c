#include "effects/bursts/radial_burst.h"

void RadialBurstEffect_RemoveParticles(struct RadialBurst *burst)
{
    struct RadialBurstEmitter *emitter = &burst->emitter;
    s32 i;

    for (i = 0; i < 12; i++)
        Effect_Remove(&emitter->particles[i]);
}
