#include "global.h"
#include "player/player.h"

void sub_0814B02C(struct Player *p, s32 angle)
{
    u32 heading = ((angle + 0x60) & 0xff) >> 6;

    heading = (heading + 2) & 3;

    if (heading > 1) {
        p->heading = 3 - heading;
        p->side = 1;
    } else {
        p->heading = heading;
        p->side = 0;
    }
}
