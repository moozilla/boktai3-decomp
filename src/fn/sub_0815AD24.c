#include "player.h"
/* Enable the first player's stat gauge. */

s32 sub_0815AD24(void)
{
    gPlayers[0]->statGaugeFull = 1;
    return 0;
}
