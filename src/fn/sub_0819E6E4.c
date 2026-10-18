#include "global.h"

extern u32 gUnk_08611E10[];
struct A {
    u8 f[0xa1];
    u8 a;
    u8 g[0xa];
    u8 i;
};
void Callback_InvokeActor(void *, u32);

void Hresvelgr_UpdateBehaviorMode(struct A *p)
{
    p->a = 0;
    Callback_InvokeActor(p, gUnk_08611E10[p->i]);
}
