#include "scene_panel.h"
void ScenePanel_TickLongNine(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 5) {
        ScenePanel_SetChannelValue(panel, 2, 10);
        ScenePanel_SetState(panel, ScenePanel_TickLongTen);
    }
}
