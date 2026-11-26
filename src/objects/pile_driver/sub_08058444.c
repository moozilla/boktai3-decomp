#include "objects/pile_driver/pile_driver.h"
void PileDriver_BeginPulseSequence(struct PileDriver *panel)
{
    PileDriver_SetChannelValue(panel, 2, 12);
    PileDriver_SetState(panel, PileDriver_TickPulseTwelve);
}
