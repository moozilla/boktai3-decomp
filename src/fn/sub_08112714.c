#include "global.h"
void sub_0824923C(void *, u32);
struct S {
    u8 f[0x11B8];
    u32 a;
};
u32 ScriptDisplay_Update(struct S *p)
{
    if (p->a)
        sub_0824923C(p, p->a);
    return 1;
}
