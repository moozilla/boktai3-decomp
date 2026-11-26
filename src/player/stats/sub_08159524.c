#include "global.h"

struct P08159524 {
    u8 f0[0x1c];
    s32 f1c;
    u8 f20[0x428 - 0x20];
    u16 f428;
};

void Player_SubtractEnergy(struct P08159524 *p, s32 d)
{
    if (p->f1c == 1) {
        if (p->f428 < d)
            p->f428 = 0;
        else
            p->f428 -= d;
    }
}
