#include "repeated_effect.h"
#define RepeatedEffect_SetState sub_0804F7D0

void RepeatedEffect_SetState(struct RepeatedEffect *p, u8 a, void (*b)(struct RepeatedEffect *))
{
    p->state = a;
    p->update = b;
    p->balance = 0;
    p->stateFrame = 0;
}
