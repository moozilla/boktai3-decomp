#include "objects/pile_driver/pile_driver_effects.h"
void PileDriver_SetEffectState(struct PileDriver *input, u16 state)
{
    struct PileDriverEffectsView *panel = (void *)input;
    panel->effectState = state;
    panel->effectFrame = 0;
}
