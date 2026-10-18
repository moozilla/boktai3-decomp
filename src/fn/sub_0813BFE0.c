#include "global.h"

struct S0813BFE0 {
    u32 f0;
    u8 f4[0x60];
    u8 f64;
};

void Player_OnOrbitingEffectIdle(struct S0813BFE0 *p)
{
    if (p->f64 != 0)
        p->f64 = 0;
    else
        p->f0 |= 1;
}
