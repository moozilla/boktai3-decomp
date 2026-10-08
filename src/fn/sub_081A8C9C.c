#include "global.h"

extern u32 gUnk_08612014[];
struct A { u8 f[0xa1]; u8 a; u8 g[0xa]; u8 i; };
void sub_0824923C(void *, u32);

void sub_081A8C9C(struct A *p)
{
    p->a = 0;
    sub_0824923C(p, gUnk_08612014[p->i]);
}
