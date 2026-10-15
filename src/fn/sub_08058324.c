#include "scene_panel.h"
void ScenePanel_TickLongSeven(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 5) {
        ScenePanel_SetChannelValue(panel, 2, 8);
        ScenePanel_SetState(panel, ScenePanel_TickLongEight);
    }
}
