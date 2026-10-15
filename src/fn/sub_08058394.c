#include "scene_panel.h"
#define ScenePanel_TickLongNine sub_08058394
void ScenePanel_TickLongNine(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 5) {
        sub_08058130(panel, 2, 10);
        sub_08058150(panel, sub_080583CC);
    }
}
