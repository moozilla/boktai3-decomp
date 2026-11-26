#include "objects/pile_driver/pile_driver_effects.h"
void PileDriver_DestroyParticles(struct ScenePanel *input)
{
    struct ScenePanelEffectsView *panel = (void *)input;
    struct ScenePanelParticle *particle = panel->particleEntries;
    s32 i;
    for (i = 31; i >= 0; i--) {
        Effect_Remove(particle);
        particle++;
    }
}
