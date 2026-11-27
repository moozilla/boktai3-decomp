#include "global.h"
void Callback_InvokeActor(void *, u32);
struct S {
    u8 f[0x11B8];
    u32 a;
};
u32 ScriptDisplay_Update(struct S *p)
{
    if (p->a)
        Callback_InvokeActor(p, p->a);
    return 1;
}
