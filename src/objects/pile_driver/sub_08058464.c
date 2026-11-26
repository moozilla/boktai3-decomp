#include "objects/pile_driver/scene_panel.h"
void PileDriver_TickPulseRest(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 37) {
        PileDriver_SetChannelValue(panel, 2, 12);
        PileDriver_SetState(panel, PileDriver_TickPulseTwelve);
    }
}
