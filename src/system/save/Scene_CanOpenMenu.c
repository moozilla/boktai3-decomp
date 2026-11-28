#include "global.h"
#include "engine/engine.h"
#include "player/player.h"
#include "player/player_slot_status.h"
u8 Scene_CanOpenMenu(void)
{
    struct Player *player = gPlayers[0];

    if (player && !(gEngineFlags & 0x200)) {
        if (PlayerSlot_ReadStatus(0, 1) && (u8)(player->actionMode - 3) > 2 &&
            !Player_ReadStatusBits(&player->statusFlags, 0x100000) &&
            (!player->active || (u16)(player->sceneMode - 5) > 1)) {
            u8 mode = player->tetherMode;
            u8 allowed = 1;

            if (mode > 1 && mode != 7)
                allowed = 0;
            if (allowed)
                return 1;
        }
    }
    return 0;
}
