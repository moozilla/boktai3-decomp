#include "objects/pile_driver/scene_panel.h"
void PileDriver_BeginPulseSequence(struct ScenePanel *panel)
{
    PileDriver_SetChannelValue(panel, 2, 12);
    PileDriver_SetState(panel, PileDriver_TickPulseTwelve);
}
