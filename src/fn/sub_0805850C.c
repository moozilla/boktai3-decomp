#include "scene_panel.h"
#define ScenePanel_TickPulseTwo sub_0805850C
void ScenePanel_TickPulseTwo(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 7) {
        sub_08058130(panel, 2, 2);
        sub_08058150(panel, sub_08058464);
    }
}
