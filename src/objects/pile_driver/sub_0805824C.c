#include "objects/pile_driver/scene_panel.h"
void PileDriver_TickReverseFour(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 7) {
        PileDriver_SetChannelValue(panel, 2, 3);
        PileDriver_SetState(panel, PileDriver_TickReverseThree);
    }
}
