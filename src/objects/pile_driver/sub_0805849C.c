#include "objects/pile_driver/scene_panel.h"
void PileDriver_TickPulseTwelve(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 7) {
        PileDriver_SetChannelValue(panel, 2, 13);
        PileDriver_SetState(panel, PileDriver_TickPulseThirteen);
    }
}
