#include "scene_panel_interaction.h"
void ScenePanel_AdvancePlayerArrival(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    struct ScenePanelInteractionPlayer *player = SCENE_PANEL_PLAYER;
    if (player->tetherMode == 0) {
        struct ScenePanelInteractionPackedPosition position;
        position.x = (u32)(panel->origin.x * 3 + player->position.x) >> 2;
        position.y = (u32)(panel->origin.y * 3 + player->position.y) >> 2;
        position.z = (u32)(panel->origin.z * 3 + player->position.z) >> 2;
        sub_08226380(16, &position);
        if (panel->mode) {
            PlayerCue_Motion7Front(SCENE_PANEL_PLAYER, 1, 0);
            Sound_PlaySong(5);
            ScenePanel_SetState(input, ScenePanel_BeginForwardSequence);
            ScenePanel_SetEffectState(input, 7);
        } else {
            PlayerCue_Motion7Front(SCENE_PANEL_PLAYER, 0, 0);
            Sound_PlaySong(407);
            ScenePanel_SetEffectState(input, 6);
        }
    }
}
