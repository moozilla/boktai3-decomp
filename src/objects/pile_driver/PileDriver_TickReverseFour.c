#include "objects/pile_driver/pile_driver.h"
void PileDriver_TickReverseFour(struct PileDriver *panel)
{
    if (++panel->stateFrame > 7) {
        PileDriver_SetChannelValue(panel, 2, 3);
        PileDriver_SetState(panel, PileDriver_TickReverseThree);
    }
}
