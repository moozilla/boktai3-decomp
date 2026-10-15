#include "scene_panel_interaction.h"
// MATCHING_CONCESSION: register-pinning

#define ScenePanel_CanAcceptPlayer sub_0805B34C
s32 ScenePanel_CanAcceptPlayer(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    struct ScenePanelInteractionPlayer *player = SCENE_PANEL_PLAYER;
    if (player->kindValue == 1 && (u8)(player->actionMode - 3) > 2) {
        asm("" : : : "r3");
        if (player->tetherMode == 7) {
            if (sub_0805B28C(&player->position, &panel->approachPosition, 100))
                return 1;
        }
    }
    return 0;
}
