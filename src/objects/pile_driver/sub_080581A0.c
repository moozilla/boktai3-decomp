#include "objects/pile_driver/scene_panel.h"
void PileDriver_BeginForwardSequence(struct ScenePanel *panel)
{
    PileDriver_SetChannelValue(panel, 2, 3);
    PileDriver_SetState(panel, PileDriver_TickForwardThree);
}
