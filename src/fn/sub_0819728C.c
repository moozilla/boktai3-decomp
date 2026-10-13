#include "global.h"

static inline s32 Approach(s32 d)
{
    s32 step = d >> 4;
    if (d > 0 && step == 0)
        step = 1;
    return step;
}

void sub_0819728C(u8 *t)
{
    *(s16 *)(t + 0x7290) += Approach(*(s16 *)(t + 0x7298) - *(s16 *)(t + 0x7290));
    *(s16 *)(t + 0x7292) += Approach(*(s16 *)(t + 0x729a) - *(s16 *)(t + 0x7292));
    *(s16 *)(t + 0x7294) += Approach(*(s16 *)(t + 0x729c) - *(s16 *)(t + 0x7294));
}
