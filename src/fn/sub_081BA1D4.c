#include "global.h"

struct Q {
    u8 f[0x52c];
    s32 sel;
    u8 g[0x28];
    u8 *p558;
};
void SelectionPanel_ProcessConfirm(void *);

void Registry2388_UpdateSelection(struct Q *p)
{
    s32 v;
    if (p->p558 == 0)
        v = -1;
    else
        v = *(s16 *)(p->p558 + 0xa0);
    p->sel = v;
    SelectionPanel_ProcessConfirm(p);
}
