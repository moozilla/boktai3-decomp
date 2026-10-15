#include "scene_panel_interaction.h"
s32 ScenePanel_CanAcquireTether(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    struct TetherActor *tether = gUnk_02000488;
    if (tether) {
        struct ScenePanelInteractionPlayer *player = SCENE_PANEL_PLAYER;
        if (player->kindValue == 1 && player->tetherMode == 6) {
            if (ScenePanel_IsWithinHorizontalRadius(&tether->position, &panel->center, 70))
                return 1;
        }
    }
    return 0;
}
