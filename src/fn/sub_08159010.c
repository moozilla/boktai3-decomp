#include "global.h"
#include "player.h"

void Player_SetTransitionMode(s32 v)
{
    u8 *g = (*(u8 **)gPlayers);

    if (g != 0) {
        g[0xAC9] = v;
    }
}
