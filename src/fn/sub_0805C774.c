#include "scene_panel_commands.h"
#include "game_state.h"
#define ScenePanel_SelectMode sub_0805C774
void ScenePanel_SelectMode(struct ScenePanelInteractionView *panel)
{
    s32 mode;
    if (panel->skipMainSprite == 0)
        mode = gUnk_02000710->unk876;
    else
        mode = panel->savedMode;
    panel->mode = mode;
}
