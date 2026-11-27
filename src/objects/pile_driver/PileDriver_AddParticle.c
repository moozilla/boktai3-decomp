#include "objects/pile_driver/pile_driver_effects.h"
void PileDriver_AddParticle(struct PileDriver *input, union PileDriverPosition *position,
                            union PileDriverPosition *velocity, u32 lifetime, u32 renderMode)
{
    struct PileDriverEffectsView *panel = (void *)input;
    struct PileDriverParticles *particles = (void *)&panel->particleResource;
    struct PileDriverParticle *particle = &particles->entries[particles->cursor];
    particle->flags &= ~1;
    particle->position = *position;
    particle->renderMode = renderMode;
    SpriteDrawable_SelectTileFrame(particle, particles->resource, 2);
    particle->velocity = *velocity;
    particle->age = 0;
    particle->lifetime = lifetime;
    particle->active = 1;
    particles->active = 1;
    particles->cursor++;
    if (particles->cursor > 31)
        particles->cursor = 0;
}
