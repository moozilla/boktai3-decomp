#include "global.h"
void sub_0824923C(void *, u32);
void PaletteFade_UpdateTransition(void *);
struct S {
    u8 f[0x18];
    u32 a;
};
u32 PaletteFade_OnTaskUpdate(struct S *p)
{
    if (p->a)
        sub_0824923C(p, p->a);
    PaletteFade_UpdateTransition(p);
    return 0;
}
