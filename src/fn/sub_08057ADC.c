#include "scene_panel_effects.h"
#define ScenePanel_DestroyParticles sub_08057ADC
void ScenePanel_DestroyParticles(struct ScenePanel *input)
{
    struct ScenePanelEffectsView *panel = (void *)input;
    struct ScenePanelParticle *particle = panel->particleEntries;
    s32 i;
    for (i = 31; i >= 0; i--) {
        Effect_Remove(particle);
        particle++;
    }
}
