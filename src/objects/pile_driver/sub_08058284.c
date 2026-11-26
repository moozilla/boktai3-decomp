#include "objects/pile_driver/scene_panel.h"
void PileDriver_TickReverseThree(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 7) {
        PileDriver_SetChannelValue(panel, 2, 2);
        PileDriver_SetState(panel, 0);
    }
}
