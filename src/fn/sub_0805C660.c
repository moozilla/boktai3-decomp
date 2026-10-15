#include "scene_panel_orchestration.h"
#define ScenePanel_ClearTransitionGate sub_0805C660
void ScenePanel_ClearTransitionGate(void)
{
    struct ScenePanelInteractionView *panel = SCENE_PANEL_GLOBAL;
    if (panel)
        panel->gateC12 = 0;
}
