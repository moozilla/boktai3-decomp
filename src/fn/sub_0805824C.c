#include "scene_panel.h"
#define ScenePanel_TickReverseFour sub_0805824C
void ScenePanel_TickReverseFour(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 7) {
        sub_08058130(panel, 2, 3);
        sub_08058150(panel, sub_08058284);
    }
}
