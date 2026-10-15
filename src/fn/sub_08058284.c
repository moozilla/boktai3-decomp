#include "scene_panel.h"
#define ScenePanel_TickReverseThree sub_08058284
void ScenePanel_TickReverseThree(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 7) {
        sub_08058130(panel, 2, 2);
        sub_08058150(panel, 0);
    }
}
