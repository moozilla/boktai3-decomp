#include "scene_panel.h"
void ScenePanel_TickReverseFour(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 7) {
        ScenePanel_SetChannelValue(panel, 2, 3);
        ScenePanel_SetState(panel, ScenePanel_TickReverseThree);
    }
}
