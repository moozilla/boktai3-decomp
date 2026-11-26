#include "objects/pile_driver/scene_panel.h"
void PileDriver_TickLongEleven(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 5) {
        PileDriver_SetChannelValue(panel, 2, 2);
        PileDriver_SetState(panel, 0);
        panel->transitionComplete = 1;
    }
}
