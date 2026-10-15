#include "scene_panel_effects.h"
#define ScenePanel_AddParticle sub_08057A44
void ScenePanel_AddParticle(struct ScenePanel *input, union ScenePanelPosition *position,
                            union ScenePanelPosition *velocity, u32 lifetime, u32 renderMode)
{
    struct ScenePanelEffectsView *panel = (void *)input;
    struct ScenePanelParticles *particles = (void *)&panel->particleResource;
    struct ScenePanelParticle *particle = &particles->entries[particles->cursor];
    particle->flags &= ~1;
    particle->position = *position;
    particle->renderMode = renderMode;
    sub_08217EEC(particle, particles->resource, 2);
    particle->velocity = *velocity;
    particle->age = 0;
    particle->lifetime = lifetime;
    particle->active = 1;
    particles->active = 1;
    particles->cursor++;
    if (particles->cursor > 31)
        particles->cursor = 0;
}
