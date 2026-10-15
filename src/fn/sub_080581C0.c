#include "scene_panel.h"
void ScenePanel_TickForwardThree(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 7) {
        ScenePanel_SetChannelValue(panel, 2, 4);
        ScenePanel_SetState(panel, ScenePanel_TickForwardFour);
    }
}
