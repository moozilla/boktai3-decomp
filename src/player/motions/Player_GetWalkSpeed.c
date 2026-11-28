#include "global.h"
#include "player/player.h"

s32 Player_GetWalkSpeed(struct Player *p)
{
    s32 speed = 12;
    if (p->movementModifierKind == 2 && p->movementModifierActive != 0)
        speed = 8;
    if (p->movementSpeedReduction != 0)
        speed -= p->movementSpeedReduction;
    if (speed <= 1)
        return 2;
    return speed;
}
