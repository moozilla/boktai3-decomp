#include "global.h"
#include "player.h"

void sub_0814B02C(struct Player *p, s32 angle)
{
    u32 heading;
    angle += 0x60;
    angle &= 0xff;
    heading = angle >> 6;
    heading += 2;
    heading &= 3;
    if (heading > 1) {
        p->heading = 3 - heading;
        p->side = 1;
    } else {
        p->heading = heading;
        p->side = 0;
    }
}
