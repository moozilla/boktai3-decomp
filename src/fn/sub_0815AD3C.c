#include "player.h"
/* Enable the first player's stat gauge and history direction mode. */

s32 sub_0815AD3C(void)
{
    struct Player *p = gPlayers[0];
    p->statGaugeFull = 1;
    p->historyDirectionMode = 1;
    return 0;
}
