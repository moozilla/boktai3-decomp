#include "global.h"

extern u32 gUnk_08613E00[];
struct A {
    u8 f[0xa1];
    u8 a;
    u8 g[0xa];
    u8 i;
};
void Callback_InvokeActor(void *, u32);

void SingletonSlot7_DispatchUpdateCallback(struct A *p)
{
    p->a = 0;
    Callback_InvokeActor(p, gUnk_08613E00[p->i]);
}
