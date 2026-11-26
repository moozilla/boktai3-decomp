#include "objects/pile_driver/scene_panel.h"
void PileDriver_TickLongEight(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 5) {
        PileDriver_SetChannelValue(panel, 2, 9);
        PileDriver_SetState(panel, PileDriver_TickLongNine);
    }
}
