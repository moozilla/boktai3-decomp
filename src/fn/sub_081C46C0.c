#include "global.h"
struct S {
    u8 f[0x64];
    u32 a;
};
void sub_0824923C(void *, u32);
void Registry6A37_Update(struct S *p)
{
    if (p->a != 0)
        sub_0824923C(p, p->a);
}
