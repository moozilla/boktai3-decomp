#include "global.h"

struct A {
    u8 f[0x18];
    s32 n;
};
void *sub_081B7340(void *);
void VanargandAfterimagePool_ActivateSlot(void *, void *);

s32 VanargandAfterimagePool_Spawn(struct A *p, void *q)
{
    void *r = sub_081B7340(p);
    if (r != 0) {
        VanargandAfterimagePool_ActivateSlot(r, q);
        p->n++;
        return 1;
    }
    return 0;
}
