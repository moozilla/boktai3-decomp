#include "scene_panel.h"
/* ScenePanel_SetChannelFive: Sets presentation channel 2 to value 5 and clears the state callback */
#define ScenePanel_SetChannelFive sub_08058184
#define ScenePanel_SetChannelFive sub_08058184
void ScenePanel_SetChannelFive(struct ScenePanel *panel)
{
    ScenePanel_SetChannelValue(panel, 2, 5);
    ScenePanel_SetState(panel, 0);
}
