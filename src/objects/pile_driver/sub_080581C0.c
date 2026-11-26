#include "objects/pile_driver/scene_panel.h"
void PileDriver_TickForwardThree(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 7) {
        PileDriver_SetChannelValue(panel, 2, 4);
        PileDriver_SetState(panel, PileDriver_TickForwardFour);
    }
}
