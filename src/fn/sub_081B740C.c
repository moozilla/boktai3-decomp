#include "global.h"

struct E {
    s32 cnt;
    u32 fl;
};
void VanargandAfterimagePool_ReleaseSlot(void *, void *);

void VanargandAfterimagePool_UpdateSlot(void *p, struct E *e, u32 k)
{
    if (--e->cnt == 0)
        VanargandAfterimagePool_ReleaseSlot(p, e);
    if (e->cnt == 4) {
        if (k & 1)
            e->fl |= 0x200;
        else
            e->fl |= 0x400;
    }
}
