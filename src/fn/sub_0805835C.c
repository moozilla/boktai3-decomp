#include "scene_panel.h"
void ScenePanel_TickLongEight(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 5) {
        ScenePanel_SetChannelValue(panel, 2, 9);
        ScenePanel_SetState(panel, ScenePanel_TickLongNine);
    }
}
