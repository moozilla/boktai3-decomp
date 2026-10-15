#include "scene_panel.h"
#define ScenePanel_TickPulseThirteen sub_080584D4
void ScenePanel_TickPulseThirteen(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 37) {
        sub_08058130(panel, 2, 12);
        sub_08058150(panel, sub_0805850C);
    }
}
