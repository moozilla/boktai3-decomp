#include "global.h"
struct S { u8 f[0x64]; u32 a; };
void sub_0824923C(void *, u32);
void sub_081C46C0(struct S *p)
{
    if (p->a != 0) sub_0824923C(p, p->a);
}
