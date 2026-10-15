#include "repeated_effect.h"
#define RepeatedEffect_InitState sub_0804FEA8

void RepeatedEffect_InitState(struct RepeatedEffect *p)
{
    u32 k = p->state;
    if (k == 0) {
        p->sprite.frame = k;
        sub_0804F7D0(p, 0, sub_0804FB48);
    } else if (k == 2) {
        p->sprite.frame = 8;
        sub_0804F7D0(p, 2, sub_0804F970);
    } else if (k == 1) {
        p->sprite.frame = 5;
        sub_0804F7D0(p, 1, sub_0804F888);
    }
}
