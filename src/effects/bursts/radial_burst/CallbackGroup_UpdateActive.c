#include "effects/bursts/radial_burst.h"

void CallbackGroup_UpdateActive(struct RadialBurst *burst)
{
    struct RadialBurstParticle *particle;
    s32 count, i;

    if (burst->emitter.particlesActive) {
        count = 0;
        for (i = 0; i < 12; i++) {
            particle = &burst->emitter.particles[i];
            if (particle->active) {
                sub_08249240(particle, (void *)burst->emitter.graphics, particle->update);
                count++;
            }
        }
        if (count == 0)
            burst->emitter.particlesActive = count;
    }
}
