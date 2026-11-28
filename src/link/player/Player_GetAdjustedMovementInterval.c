#include "global.h"
#include "player/player.h"

s32 Player_GetAdjustedMovementInterval(struct Player *p)
{
    s32 interval = 12;

    if (p->movementModifierKind == 2 && p->movementModifierActive != 0)
        interval = 8;
    if (p->movementSpeedReduction != 0)
        interval -= p->movementSpeedReduction;
    if (interval <= 1)
        return 2;
    return interval;
}
