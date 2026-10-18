#include "global.h"
void Callback_InvokeActor(void *, u32);
void PaletteFade_UpdateTransition(void *);
struct S {
    u8 f[0x18];
    u32 a;
};
u32 PaletteFade_OnTaskUpdate(struct S *p)
{
    if (p->a)
        Callback_InvokeActor(p, p->a);
    PaletteFade_UpdateTransition(p);
    return 0;
}
