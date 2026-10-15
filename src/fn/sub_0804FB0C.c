#include "repeated_effect.h"
#define RepeatedEffect_TickUpperRecovery sub_0804FB0C

void RepeatedEffect_TickUpperRecovery(struct RepeatedEffect *p)
{
    u16 *c = &p->stateFrame;
    u32 v = *c + 1;
    *c = v;
    if ((u16)v <= 7) {
        p->sprite.frame = 7;
    } else if ((u16)v <= 0xf) {
        p->sprite.frame = 6;
    } else {
        p->sprite.frame = 2;
        sub_0804F7D0(p, 0, sub_0804FB48);
    }
}
