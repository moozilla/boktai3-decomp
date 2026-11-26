#include "objects/pile_driver/pile_driver_effects.h"
void PileDriver_InitParticles(struct PileDriver *input)
{
    struct PileDriverEffectsView *panel = (void *)input;
    struct PileDriverParticles *particles = (void *)&panel->particleResource;
    s32 i;
    particles->resource = AnimData_FindEntry(0x1C1E);
    particles->active = 0;
    i = 0;
    for (; i < 32; i++) {
        struct PileDriverParticle *particle = &particles->entries[i];
        ParticleSprite_Initialize(particle, particles->resource, 1);
        Effect_SetOrigin(particle, -4, -4);
        Effect_SetPalette(particle, 1);
        particle->active = 0;
    }
}
