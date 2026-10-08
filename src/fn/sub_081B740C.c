#include "global.h"

struct E { s32 cnt; u32 fl; };
void sub_081B73F8(void *, void *);

void sub_081B740C(void *p, struct E *e, u32 k)
{
    if (--e->cnt == 0)
        sub_081B73F8(p, e);
    if (e->cnt == 4) {
        if (k & 1)
            e->fl |= 0x200;
        else
            e->fl |= 0x400;
    }
}
