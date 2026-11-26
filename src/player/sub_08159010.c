#include "engine/shared/global.h"
#include "engine/shared/player.h"

void Player_SetTransitionMode(s32 v)
{
    u8 *g = (*(u8 **)gPlayers);

    if (g != 0) {
        g[0xAC9] = v;
    }
}
