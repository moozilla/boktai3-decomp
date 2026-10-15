#include "scene_panel_effects.h"
void ScenePanel_UpdateParticles(struct ScenePanel *input)
{
    struct ScenePanelEffectsView *panel = (void *)input;
    struct ScenePanelParticles *particles = (void *)&panel->particleResource;
    if (particles->active) {
        u32 anyActive = 0;
        s32 i;
        for (i = 0; i < 32; i++) {
            struct ScenePanelParticle *particle = &particles->entries[i];
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
                        sub_08217EEC(particle, particles->resource, 3);
                    particle->age++;
                }
            }
        }
        particles->active = anyActive;
    }
}
