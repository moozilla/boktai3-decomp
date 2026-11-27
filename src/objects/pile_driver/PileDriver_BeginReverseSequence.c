#include "objects/pile_driver/pile_driver.h"
/* PileDriver_BeginReverseSequence: Sets presentation channel 2 to value 4 and installs the reverse sequence callback */
void PileDriver_BeginReverseSequence(struct PileDriver *panel)
{
    PileDriver_SetChannelValue(panel, 2, 4);
    PileDriver_SetState(panel, PileDriver_TickReverseFour);
}
