#include "repeated_effect.h"

void RepeatedEffect_InitState(struct RepeatedEffect *p)
{
    u32 k = p->state;
    if (k == 0) {
        p->sprite.frame = k;
        RepeatedEffect_SetState(p, 0, RepeatedEffect_TickNeutral);
    } else if (k == 2) {
        p->sprite.frame = 8;
        RepeatedEffect_SetState(p, 2, RepeatedEffect_TickUpperState);
    } else if (k == 1) {
        p->sprite.frame = 5;
        RepeatedEffect_SetState(p, 1, RepeatedEffect_TickLowerState);
    }
}
