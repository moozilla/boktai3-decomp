#include "scene_panel_interaction.h"
#define ScenePanel_WaitTetherReturn sub_0805BB20
void ScenePanel_WaitTetherReturn(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    if (sub_0805B2CC(input)) {
        sub_0815B15C(SCENE_PANEL_PLAYER, 0, -1, 0, 0);
        panel->variant = 2;
        sub_08057960(input, 1);
    } else if (sub_0805B3A0(input)) {
        struct TetherActor *tether = gUnk_02000488;
        s32 sourceX = tether->position.x;
        struct ScenePanelInteractionPlayer *player = SCENE_PANEL_PLAYER;
        s32 x = sourceX - player->position.x;
        s32 z = tether->position.z - player->position.z;
        s32 direction;
        if (player->actionMode <= 1 || player->actionMode == 6) {
            direction = (((u16)Math_Atan2(x, z) + 112) >> 5) & 7;
            sub_0815AF50(player, direction, 0);
        } else {
            direction = (((u16)Math_Atan2(x, z) + 112) >> 5) & 7;
            sub_0815AF14(player, direction);
            sub_0815C4FC(SCENE_PANEL_PLAYER, 0);
        }
        TetherActor_BeginAuxiliaryState(gUnk_02000488);
        sub_08226380(16, &gUnk_02000488->position);
        sub_08057960(input, 13);
        sub_08057978(input, 6);
    }
}
