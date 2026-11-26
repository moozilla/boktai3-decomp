#include "objects/pile_driver/scene_panel.h"
/* ScenePanel_BeginReverseSequence: Sets presentation channel 2 to value 4 and installs the reverse sequence callback */
#define ScenePanel_BeginReverseSequence sub_0805822C
#define ScenePanel_BeginReverseSequence sub_0805822C
void ScenePanel_BeginReverseSequence(struct ScenePanel *panel)
{
    PileDriver_SetChannelValue(panel, 2, 4);
    PileDriver_SetState(panel, PileDriver_TickReverseFour);
}
