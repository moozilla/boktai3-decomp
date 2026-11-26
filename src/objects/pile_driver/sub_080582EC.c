#include "objects/pile_driver/scene_panel.h"
void PileDriver_TickLongSix(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 5) {
        PileDriver_SetChannelValue(panel, 2, 7);
        PileDriver_SetState(panel, PileDriver_TickLongSeven);
    }
}
