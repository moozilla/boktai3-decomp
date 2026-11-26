#include "objects/pile_driver/pile_driver.h"
void PileDriver_TickPulseThirteen(struct PileDriver *panel)
{
    if (++panel->stateFrame > 37) {
        PileDriver_SetChannelValue(panel, 2, 12);
        PileDriver_SetState(panel, PileDriver_TickPulseTwo);
    }
}
