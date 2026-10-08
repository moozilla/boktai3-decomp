#include "global.h"

struct Q { u8 f[0x228]; s32 sel; u8 *p; };
void sub_081BA79C(void *);

void sub_081BA9FC(struct Q *p)
{
    s32 v;
    if (p->p == 0)
        v = -1;
    else
        v = *(s16 *)(p->p + 0xa0);
    p->sel = v;
    sub_081BA79C(p);
}
