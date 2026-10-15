#include "scene_panel.h"
void ScenePanel_BeginLongSequence(struct ScenePanel *panel)
{
    ScenePanel_SetChannelValue(panel, 2, 6);
    ScenePanel_SetState(panel, ScenePanel_TickLongSix);
    panel->transitionComplete = 0;
    panel->stateFrame = 0;
}
