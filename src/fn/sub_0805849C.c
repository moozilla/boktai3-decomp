#include "scene_panel.h"
void ScenePanel_TickPulseTwelve(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 7) {
        ScenePanel_SetChannelValue(panel, 2, 13);
        ScenePanel_SetState(panel, ScenePanel_TickPulseThirteen);
    }
}
