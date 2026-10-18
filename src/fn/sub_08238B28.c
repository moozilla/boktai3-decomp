#include "global.h"

struct S08238B28 {
    u32 f0;
    u8 f4[0x60];
    u8 f64;
};

void Player_OnDirectionalEffectAnimationEvent(struct S08238B28 *p)
{
    if (p->f64 != 0)
        p->f64 = 0;
    else
        p->f0 |= 1;
}
