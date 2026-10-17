#include "scene_panel_interaction.h"
void ScenePanel_WaitTetherReturn(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    if (ScenePanel_CanAcquireTether(input)) {
        PlayerCue_MoveTo(SCENE_PANEL_PLAYER, 0, -1, 0, 0);
        panel->variant = 2;
        ScenePanel_SetEffectState(input, 1);
    } else if (ScenePanel_IsTetherDistant(input)) {
        struct TetherActor *tether = gUnk_02000488;
        s32 sourceX = tether->position.x;
        struct ScenePanelInteractionPlayer *player = SCENE_PANEL_PLAYER;
        s32 x = sourceX - player->position.x;
        s32 z = tether->position.z - player->position.z;
        s32 direction;
        if (player->actionMode <= 1 || player->actionMode == 6) {
            direction = (((u16)Math_Atan2(x, z) + 112) >> 5) & 7;
            PlayerCue_Face(player, direction, 0);
        } else {
            direction = (((u16)Math_Atan2(x, z) + 112) >> 5) & 7;
            PlayerCue_Stand(player, direction);
            PlayerCue_StateMotion(SCENE_PANEL_PLAYER, 0);
        }
        TetherActor_BeginAuxiliaryState(gUnk_02000488);
        sub_08226380(16, &gUnk_02000488->position);
        ScenePanel_SetEffectState(input, 13);
        ScenePanel_CallPendingScript(input, 6);
    }
}
