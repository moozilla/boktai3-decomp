#include "scene_panel.h"
#define ScenePanel_TickLongEight sub_0805835C
void ScenePanel_TickLongEight(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 5) {
        sub_08058130(panel, 2, 9);
        sub_08058150(panel, sub_08058394);
    }
}
