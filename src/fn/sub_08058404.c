#include "scene_panel.h"
void ScenePanel_TickLongEleven(struct ScenePanel *panel)
{
    if (++panel->stateFrame > 5) {
        ScenePanel_SetChannelValue(panel, 2, 2);
        ScenePanel_SetState(panel, 0);
        panel->transitionComplete = 1;
    }
}
