#include "objects/pile_driver/pile_driver_effects.h"
void PileDriver_UpdateParticles(struct PileDriver *input)
{
    struct PileDriverEffectsView *panel = (void *)input;
    struct PileDriverParticles *particles = (void *)&panel->particleResource;
    if (particles->active) {
        u32 anyActive = 0;
        s32 i;
        for (i = 0; i < 32; i++) {
            struct PileDriverParticle *particle = &particles->entries[i];
            if (particle->active) {
                if (particle->age >= particle->lifetime) {
                    particle->flags |= 1;
                    particle->active = 0;
                } else {
                    anyActive = 1;
                    particle->position.world.x += particle->velocity.world.x;
                    particle->position.world.y += particle->velocity.world.y;
                    particle->position.world.z += particle->velocity.world.z;
                    if (particle->age == 6)
                        SpriteDrawable_SelectTileFrame(particle, particles->resource, 3);
                    particle->age++;
                }
            }
        }
        particles->active = anyActive;
    }
}
