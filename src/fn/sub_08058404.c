#include "scene_panel.h"
#define ScenePanel_TickLongEleven sub_08058404
void ScenePanel_TickLongEleven(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 5) {
        sub_08058130(panel, 2, 2);
        sub_08058150(panel, 0);
        panel->transitionComplete = 1;
    }
}
