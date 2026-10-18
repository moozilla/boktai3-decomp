#include "global.h"
struct S {
    u8 f[0x64];
    u32 a;
};
void Callback_InvokeActor(void *, u32);
void Registry6A37_Update(struct S *p)
{
    if (p->a != 0)
        Callback_InvokeActor(p, p->a);
}
