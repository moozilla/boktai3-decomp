#include "global.h"
#include "player.h"

void Player_ClearFlag740(void)
{
    u8 *g = (*(u8 **)gPlayers);

    if (g != 0) {
        g[0x740] = 0;
    }
}
