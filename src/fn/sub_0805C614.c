#include "scene_panel_orchestration.h"
void ScenePanel_BeginArrivalRamp(void)
{
    struct ScenePanelInteractionView *panel = SCENE_PANEL_GLOBAL;
    if (panel) {
        u8 *enabled = &panel->arrivalRampEnabled;
        u32 zero = 0;
        *enabled = 1;
        SCENE_PANEL_GLOBAL->arrivalRampFrame = zero;
    }
}
