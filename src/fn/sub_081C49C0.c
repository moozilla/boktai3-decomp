#include "global.h"
struct S {
    u8 f[0xac];
    u32 a;
};
void sub_081C4950(void *);
void Callback_InvokeActor(void *, u32);
void ShipWheel_Update(struct S *p)
{
    sub_081C4950(p);
    if (p->a != 0)
        Callback_InvokeActor(&p->a, p->a);
}
