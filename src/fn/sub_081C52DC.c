#include "global.h"
struct S {
    u8 f[0xd4];
    u32 a;
};
void sub_081C5264(void *);
void Callback_InvokeActor(void *, u32);
void Registry1909_Update(struct S *p)
{
    sub_081C5264(p);
    if (p->a != 0)
        Callback_InvokeActor(p, p->a);
}
