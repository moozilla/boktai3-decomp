#include "objects/pile_driver/pile_driver.h"
void PileDriver_TickLongNine(struct PileDriver *panel)
{
    if (++panel->stateFrame > 5) {
        PileDriver_SetChannelValue(panel, 2, 10);
        PileDriver_SetState(panel, PileDriver_TickLongTen);
    }
}
