#include "player/effects/spiral_trail.h"

void Effect_Remove(void *);

void SpiralTrail_RemoveParticles(struct SpiralTrailTask *p)
{
    struct SpiralTrailParticle *q = p->emitter.particles;
    s32 i;

    for (i = 7; i >= 0; i--) {
        Effect_Remove(q);
        q++;
    }
}
