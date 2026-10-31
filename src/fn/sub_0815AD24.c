#include "player.h"
/* Enable the first player's stat gauge. */

s32 Player_EnableStatGaugeFromScript(void)
{
    gPlayers[0]->statGaugeFull = 1;
    return 0;
}
