#include "scene_panel.h"
void ScenePanel_TickForwardFour(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 7) {
        ScenePanel_SetChannelValue(panel, 2, 5);
        ScenePanel_SetState(panel, 0);
    }
}
