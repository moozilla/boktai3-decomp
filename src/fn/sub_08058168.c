#include "scene_panel.h"
/* ScenePanel_SetChannelTwo: Sets presentation channel 2 to value 2 and clears the state callback */
#define ScenePanel_SetChannelTwo sub_08058168
#define ScenePanel_SetChannelTwo sub_08058168
void ScenePanel_SetChannelTwo(struct ScenePanel *panel)
{
    ScenePanel_SetChannelValue(panel, 2, 2);
    ScenePanel_SetState(panel, 0);
}
