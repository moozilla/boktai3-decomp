#include "objects/pile_driver/scene_panel.h"
/* ScenePanel_SetChannelTwo: Sets presentation channel 2 to value 2 and clears the state callback */
#define ScenePanel_SetChannelTwo sub_08058168
#define ScenePanel_SetChannelTwo sub_08058168
void ScenePanel_SetChannelTwo(struct ScenePanel *panel)
{
    PileDriver_SetChannelValue(panel, 2, 2);
    PileDriver_SetState(panel, 0);
}
