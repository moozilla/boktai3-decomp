#include "objects/pile_driver/pile_driver.h"
/* PileDriver_SetChannelFive: Sets presentation channel 2 to value 5 and clears the state callback */
void PileDriver_SetChannelFive(struct PileDriver *panel)
{
    PileDriver_SetChannelValue(panel, 2, 5);
    PileDriver_SetState(panel, 0);
}
