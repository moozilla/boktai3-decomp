#include "scene_panel_interaction.h"
// MATCHING_CONCESSION: register-pinning

s32 ScenePanel_CanAcceptPlayer(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    struct ScenePanelInteractionPlayer *player = SCENE_PANEL_PLAYER;
    if (player->kindValue == 1 && (u8)(player->actionMode - 3) > 2) {
        asm("" : : : "r3");
        if (player->tetherMode == 7) {
            if (ScenePanel_IsWithinHorizontalRadius(&player->position, &panel->approachPosition,
                                                    100))
                return 1;
        }
    }
    return 0;
}
