#include "scene_panel.h"
void ScenePanel_BeginForwardSequence(struct ScenePanel *panel)
{
    ScenePanel_SetChannelValue(panel, 2, 3);
    ScenePanel_SetState(panel, ScenePanel_TickForwardThree);
}
