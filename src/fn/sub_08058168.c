#include "scene_panel.h"
#define ScenePanel_SetChannelTwo sub_08058168
void ScenePanel_SetChannelTwo(struct ScenePanel *panel)
{
    ScenePanel_SetChannelValue(panel, 2, 2);
    ScenePanel_SetState(panel, 0);
}
