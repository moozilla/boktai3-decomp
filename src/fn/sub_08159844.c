#include "global.h"

struct P08159844 { u8 f0[0x430]; u16 f430; };

void sub_08159844(struct P08159844 *p, s32 d)
{
    if (p->f430 <= d)
        p->f430 = 0;
    else
        p->f430 -= d;
}
