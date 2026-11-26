#include "engine/shared/engine_pause.h"
#include "engine/shared/global.h"
#include "player/player.h"
#include "menus/scene_hud/scene_hud_display.h"
#include "engine/shared/engine.h"
#include "engine/shared/ram.h"
u32 SceneHud_CanOpenFromStart(struct SceneHudState *p)
{
    struct Player *player;
    if (gPadState[1] & 8) {
        player = p->actionOwner;
        if (player && player->kind == 1 && player->actionMode != 6 && !(player->statusFlags & 1) &&
            p->controlMode <= 2) {
            if (Engine_PauseBits(0x201))
                return 0;
            if (player->historyDirectionMode != 1)
                return 1;
        }
    }
    return 0;
}
