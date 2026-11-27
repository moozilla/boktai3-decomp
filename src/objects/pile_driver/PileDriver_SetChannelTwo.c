#include "objects/pile_driver/pile_driver.h"
/* PileDriver_SetChannelTwo: Sets presentation channel 2 to value 2 and clears the state callback */
void PileDriver_SetChannelTwo(struct PileDriver *panel)
{
    PileDriver_SetChannelValue(panel, 2, 2);
    PileDriver_SetState(panel, 0);
}
