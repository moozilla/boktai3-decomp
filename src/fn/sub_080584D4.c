#include "scene_panel.h"
void ScenePanel_TickPulseThirteen(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 37) {
        ScenePanel_SetChannelValue(panel, 2, 12);
        ScenePanel_SetState(panel, ScenePanel_TickPulseTwo);
    }
}
