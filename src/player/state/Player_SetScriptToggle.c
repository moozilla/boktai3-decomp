#include "global.h"
#include "player/player.h"

void Player_SetScriptToggle(void)
{
    u8 *g = (*(u8 **)gPlayers);

    if (g != 0) {
        g[0x740] = 1;
    }
}
