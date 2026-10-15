#include "scene_panel_orchestration.h"
void ScenePanel_SetTransitionGate(void)
{
    struct ScenePanelInteractionView *panel = SCENE_PANEL_GLOBAL;
    if (panel)
        panel->gateC12 = 1;
}
