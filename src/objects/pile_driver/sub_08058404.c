#include "objects/pile_driver/pile_driver.h"
void PileDriver_TickLongEleven(struct PileDriver *panel)
{
    if (++panel->stateFrame > 5) {
        PileDriver_SetChannelValue(panel, 2, 2);
        PileDriver_SetState(panel, 0);
        panel->transitionComplete = 1;
    }
}
