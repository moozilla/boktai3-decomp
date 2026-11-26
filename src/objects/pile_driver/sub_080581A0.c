#include "objects/pile_driver/pile_driver.h"
void PileDriver_BeginForwardSequence(struct PileDriver *panel)
{
    PileDriver_SetChannelValue(panel, 2, 3);
    PileDriver_SetState(panel, PileDriver_TickForwardThree);
}
