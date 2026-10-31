#include "player.h"
/* Restrict the first player's action requests. */

s32 sub_0815AD60(void)
{
    gPlayers[0]->actionRequestRestricted = 1;
    return 0;
}
