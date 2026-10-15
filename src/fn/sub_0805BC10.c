#include "scene_panel_interaction.h"
#define ScenePanel_AdvanceTetherReturn sub_0805BC10
void ScenePanel_AdvanceTetherReturn(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    panel->frame++;
    switch (panel->frame) {
        case 33: {
            struct TetherActor *tether = gUnk_02000488;
            s32 sourceX = tether->position.x;
            struct ScenePanelInteractionPlayer *player = SCENE_PANEL_PLAYER;
            s32 x = sourceX - player->position.x;
            s32 z = tether->position.z - player->position.z;
            s32 direction = (((u16)Math_Atan2(x, z) + 112) >> 5) & 7;
            sub_0815AF14(player, direction);
            break;
        }
        case 64:
            Sound_PlaySong(453);
            break;
        case 300:
            sub_08226380(16, &SCENE_PANEL_PLAYER->position);
            break;
        case 316:
            if (panel->transitionSound)
                Sound_PlaySong(panel->transitionSound);
            sub_0815B2E0(SCENE_PANEL_PLAYER, -1, 0);
            break;
        case 360:
            sub_08057978(input, 7);
            break;
    }
}
