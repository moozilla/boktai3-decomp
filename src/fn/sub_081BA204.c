#include "global.h"

struct Q { u8 f[0x52c]; s32 sel; u8 g[0x28]; u8 *p558; u8 g2[4]; s32 cnt; };
void sub_081BA0FC(void *);

void sub_081BA204(struct Q *p)
{
    s32 v;
    if (p->p558 == 0)
        v = -1;
    else
        v = *(s16 *)(p->p558 + 0xa0);
    p->sel = v;
    p->cnt++;
    if (p->cnt > 0x3c)
        sub_081BA0FC(p);
}
