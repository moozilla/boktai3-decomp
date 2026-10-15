#include "scene_panel_orchestration.h"
#define ScenePanel_ReadInitialMode sub_0805C454
void ScenePanel_ReadInitialMode(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    s32 mode = Script_SeekToKeyword('m');
    if (mode)
        mode = Script_GetValue();
    panel->initialMode = mode;
}
