#include "scene_panel.h"
void ScenePanel_BeginPulseSequence(struct ScenePanel *panel)
{
    ScenePanel_SetChannelValue(panel, 2, 12);
    ScenePanel_SetState(panel, ScenePanel_TickPulseTwelve);
}
