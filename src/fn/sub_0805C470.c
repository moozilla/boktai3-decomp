#include "scene_panel_orchestration.h"
#define ScenePanel_SuppressMainPresentation sub_0805C470
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
