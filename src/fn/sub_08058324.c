#include "scene_panel.h"
#define ScenePanel_TickLongSeven sub_08058324
void ScenePanel_TickLongSeven(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 5) {
        sub_08058130(panel, 2, 8);
        sub_08058150(panel, sub_0805835C);
    }
}
