#include "scene_panel_orchestration.h"
void ScenePanel_SuppressMainPresentation(void)
{
    if (gUnk_020004A4) {
        struct ScenePanelInteractionView *panel;
        u8 mode;
        SCENE_PANEL_GLOBAL->skipMainSprite = 1;
        panel = SCENE_PANEL_GLOBAL;
        mode = panel->mode;
        panel->savedMode = mode;
    }
}
