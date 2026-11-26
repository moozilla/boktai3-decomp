#include "engine/shared/global.h"
#include "player/player.h"

struct PlayerSelection {
    u8 unknown0[0xA];
    u16 playerMask;
};

struct Player *Player_FindMaskedSlot(const struct PlayerSelection *selection)
{
    s32 i;
    for (i = 0; i < 4; i++) {
        if (selection->playerMask & (1 << i))
            return gPlayers[i];
    }
    return gPlayers[0];
}
