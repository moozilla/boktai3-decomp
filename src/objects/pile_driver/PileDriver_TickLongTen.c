#include "objects/pile_driver/pile_driver.h"
void PileDriver_TickLongTen(struct PileDriver *panel)
{
    if (++panel->stateFrame > 5) {
        PileDriver_SetChannelValue(panel, 2, 11);
        PileDriver_SetState(panel, PileDriver_TickLongEleven);
    }
}
