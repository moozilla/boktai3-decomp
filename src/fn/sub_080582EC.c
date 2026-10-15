#include "scene_panel.h"
void ScenePanel_TickLongSix(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 5) {
        ScenePanel_SetChannelValue(panel, 2, 7);
        ScenePanel_SetState(panel, ScenePanel_TickLongSeven);
    }
}
