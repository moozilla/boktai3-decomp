#include "repeated_effect.h"

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
        RepeatedEffect_SetState(p, 0, RepeatedEffect_TickNeutral);
    }
}
