#include "objects/pile_driver/pile_driver_effects.h"
void PileDriver_DestroyParticles(struct PileDriver *input)
{
    struct PileDriverEffectsView *panel = (void *)input;
    struct PileDriverParticle *particle = panel->particleEntries;
    s32 i;
    for (i = 31; i >= 0; i--) {
        Effect_Remove(particle);
        particle++;
    }
}
