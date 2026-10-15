#include "scene_panel.h"
#define ScenePanel_TickPulseRest sub_08058464
void ScenePanel_TickPulseRest(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 37) {
        sub_08058130(panel, 2, 12);
        sub_08058150(panel, sub_0805849C);
    }
}
