#include "scene_panel.h"
#define ScenePanel_TickForwardThree sub_080581C0
void ScenePanel_TickForwardThree(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 7) {
        sub_08058130(panel, 2, 4);
        sub_08058150(panel, sub_080581F8);
    }
}
