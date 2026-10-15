#include "scene_panel_interaction.h"
#define ScenePanel_CanAcquireTether sub_0805B2CC
s32 ScenePanel_CanAcquireTether(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    struct TetherActor *tether = gUnk_02000488;
    if (tether) {
        struct ScenePanelInteractionPlayer *player = SCENE_PANEL_PLAYER;
        if (player->kindValue == 1 && player->tetherMode == 6) {
            if (sub_0805B28C(&tether->position, &panel->center, 70))
                return 1;
        }
    }
    return 0;
}
