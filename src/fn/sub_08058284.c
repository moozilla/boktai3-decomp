#include "scene_panel.h"
void ScenePanel_TickReverseThree(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 7) {
        ScenePanel_SetChannelValue(panel, 2, 2);
        ScenePanel_SetState(panel, 0);
    }
}
