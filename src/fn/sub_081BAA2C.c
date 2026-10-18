#include "global.h"

struct Q {
    u8 f[0x228];
    s32 sel;
    u8 *p;
    u8 g[0x34];
    s32 cnt;
};
void Registry3F74_StartSelection(void *);

void Registry3F74_UpdateOpeningDelay(struct Q *p)
{
    s32 v;
    if (p->p == 0)
        v = -1;
    else
        v = *(s16 *)(p->p + 0xa0);
    p->sel = v;
    p->cnt++;
    if (p->cnt > 0x3c)
        Registry3F74_StartSelection(p);
}
