#include "objects/pile_driver/scene_panel.h"
void PileDriver_TickLongSeven(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 5) {
        PileDriver_SetChannelValue(panel, 2, 8);
        PileDriver_SetState(panel, PileDriver_TickLongEight);
    }
}
