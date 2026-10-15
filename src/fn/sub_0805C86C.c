#include "scene_panel_commands.h"
void ScenePanel_SubmitPlayerMidpoint(struct ScenePanelInteractionView *panel)
{
    struct ScenePanelInteractionPackedPosition position;
    struct ScenePanelInteractionPlayer *player = SCENE_PANEL_PLAYER;
    position.x = ((u32)(player->position.x + panel->origin.x) << 15) >> 16;
    position.y = (u16)player->position.y + 150;
    position.z = ((u32)(player->position.z + panel->origin.z) << 15) >> 16;
    sub_08225F88(&position);
}
