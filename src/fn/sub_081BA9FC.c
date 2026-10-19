#include "global.h"

struct Q {
    u8 f[0x228];
    s32 sel;
    u8 *p;
};
void Registry3F74_ConfirmSelection(void *);

void Registry3F74_UpdateSelection(struct Q *p)
{
    s32 v;
    if (p->p == 0)
        v = -1;
    else
        v = *(s16 *)(p->p + 0xa0);
    p->sel = v;
    Registry3F74_ConfirmSelection(p);
}
