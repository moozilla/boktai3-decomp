#include "scene_panel.h"
#define ScenePanel_TickLongTen sub_080583CC
void ScenePanel_TickLongTen(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 5) {
        sub_08058130(panel, 2, 11);
        sub_08058150(panel, sub_08058404);
    }
}
