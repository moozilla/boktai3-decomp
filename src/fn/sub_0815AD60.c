#include "player.h"
/* Restrict the first player's action requests. */

s32 Player_RestrictActionRequestsFromScript(void)
{
    gPlayers[0]->actionRequestRestricted = 1;
    return 0;
}
