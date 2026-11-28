#include "global.h"
#include "player/player.h"
/* Player_SetTimedEffect: Stores an effect parameter and starts its thirty-two-tick timer; effect
   interpretation tentative. */

void Player_SetTimedEffect(struct Player *p, u16 parameter)
{
    p->actionEighteenParameter = parameter;
    p->actionEighteenDuration = 32;
}
