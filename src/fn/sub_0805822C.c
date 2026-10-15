#include "scene_panel.h"
#define ScenePanel_BeginReverseSequence sub_0805822C
void ScenePanel_BeginReverseSequence(struct ScenePanel *panel)
{
    ScenePanel_SetChannelValue(panel, 2, 4);
    ScenePanel_SetState(panel, ScenePanel_TickReverseFour);
}
