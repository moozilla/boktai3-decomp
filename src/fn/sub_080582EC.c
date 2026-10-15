#include "scene_panel.h"
#define ScenePanel_TickLongSix sub_080582EC
void ScenePanel_TickLongSix(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 5) {
        sub_08058130(panel, 2, 7);
        sub_08058150(panel, sub_08058324);
    }
}
