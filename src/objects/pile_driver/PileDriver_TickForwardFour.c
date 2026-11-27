#include "objects/pile_driver/pile_driver.h"
void PileDriver_TickForwardFour(struct PileDriver *panel)
{
    if (++panel->stateFrame > 7) {
        PileDriver_SetChannelValue(panel, 2, 5);
        PileDriver_SetState(panel, 0);
    }
}
