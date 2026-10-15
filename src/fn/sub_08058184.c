#include "scene_panel.h"
#define ScenePanel_SetChannelFive sub_08058184
void ScenePanel_SetChannelFive(struct ScenePanel *panel)
{
    ScenePanel_SetChannelValue(panel, 2, 5);
    ScenePanel_SetState(panel, 0);
}
