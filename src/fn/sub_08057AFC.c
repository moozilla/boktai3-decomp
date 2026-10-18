#include "scene_panel_effects.h"
void ScenePanel_InitParticles(struct ScenePanel *input)
{
    struct ScenePanelEffectsView *panel = (void *)input;
    struct ScenePanelParticles *particles = (void *)&panel->particleResource;
    s32 i;
    s32 scale;
    particles->resource = AnimData_FindEntry(0x1C1E);
    particles->active = 0;
    i = 0;
    scale = -4;
    for (; i < 32; i++) {
        struct ScenePanelParticle *particle = &particles->entries[i];
        sub_08217DE0(particle, particles->resource, 1);
        Effect_SetOrigin(particle, scale, scale);
        Effect_SetPalette(particle, 1);
        particle->active = 0;
    }
}
