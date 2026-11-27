#include "objects/pile_driver/pile_driver.h"
void PileDriver_TickLongSix(struct PileDriver *panel)
{
    if (++panel->stateFrame > 5) {
        PileDriver_SetChannelValue(panel, 2, 7);
        PileDriver_SetState(panel, PileDriver_TickLongSeven);
    }
}
