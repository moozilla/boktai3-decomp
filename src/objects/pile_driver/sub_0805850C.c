#include "objects/pile_driver/pile_driver.h"
void PileDriver_TickPulseTwo(struct PileDriver *panel)
{
    if (++panel->stateFrame > 7) {
        PileDriver_SetChannelValue(panel, 2, 2);
        PileDriver_SetState(panel, PileDriver_TickPulseRest);
    }
}
