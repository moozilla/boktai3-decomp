#include "scene_panel.h"
#define ScenePanel_TickForwardFour sub_080581F8
void ScenePanel_TickForwardFour(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 7) {
        sub_08058130(panel, 2, 5);
        sub_08058150(panel, 0);
    }
}
