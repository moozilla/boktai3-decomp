#include "objects/pile_driver/pile_driver.h"
void PileDriver_TickReverseThree(struct PileDriver *panel)
{
    if (++panel->stateFrame > 7) {
        PileDriver_SetChannelValue(panel, 2, 2);
        PileDriver_SetState(panel, 0);
    }
}
