#include "global.h"
#include "player/player.h"

void Player_SetFacingFromAngle(struct Player *p, s32 angle)
{
    s32 heading = ((angle + 0x60) & 0xff) >> 6;

    if (heading > 1) {
        p->heading = 3 - heading;
        p->side = 1;
    } else {
        p->heading = heading;
        p->side = 0;
    }
}
