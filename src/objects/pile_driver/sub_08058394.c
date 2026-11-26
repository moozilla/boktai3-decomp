#include "objects/pile_driver/scene_panel.h"
void PileDriver_TickLongNine(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 5) {
        PileDriver_SetChannelValue(panel, 2, 10);
        PileDriver_SetState(panel, PileDriver_TickLongTen);
    }
}
