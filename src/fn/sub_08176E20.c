#include "global.h"
#include "player.h"
#include "scene_hud_display.h"
#include "engine.h"
#include "ram.h"
u32 SceneHud_CanOpenFromStart(struct SceneHudState *p)
{
    struct Player *player;
    if (gUnk_03005260[1] & 8) {
        player = p->actionOwner;
        if (player && player->kind == 1 && player->actionMode != 6 && !(player->statusFlags & 1) &&
            p->controlMode <= 2) {
            u32 mask = 0x201;
            if ((gEngineFlags | gScenePauseFlags) & mask)
                return 0;
            if (player->historyDirectionMode != 1)
                return 1;
        }
    }
    return 0;
}
