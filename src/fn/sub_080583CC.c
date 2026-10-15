#include "scene_panel.h"
void ScenePanel_TickLongTen(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 5) {
        ScenePanel_SetChannelValue(panel, 2, 11);
        ScenePanel_SetState(panel, ScenePanel_TickLongEleven);
    }
}
