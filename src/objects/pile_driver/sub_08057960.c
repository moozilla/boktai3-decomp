#include "objects/pile_driver/pile_driver_effects.h"
void PileDriver_SetEffectState(struct ScenePanel *input, u16 state)
{
    struct ScenePanelEffectsView *panel = (void *)input;
    panel->effectState = state;
    panel->effectFrame = 0;
}
