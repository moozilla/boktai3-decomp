#include "scene_panel_orchestration.h"
#define ScenePanel_SetTransitionGate sub_0805C640
void ScenePanel_SetTransitionGate(void)
{
    struct ScenePanelInteractionView *panel = SCENE_PANEL_GLOBAL;
    if (panel)
        panel->gateC12 = 1;
}
