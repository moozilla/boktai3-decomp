#include "objects/pile_driver/pile_driver.h"
void PileDriver_BeginLongSequence(struct PileDriver *panel)
{
    PileDriver_SetChannelValue(panel, 2, 6);
    PileDriver_SetState(panel, PileDriver_TickLongSix);
    panel->transitionComplete = 0;
    panel->stateFrame = 0;
}
