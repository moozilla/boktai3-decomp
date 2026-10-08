#include "global.h"

extern u32 gUnk_02000560;
struct A { u8 f[0xaf]; u8 a; };
void sub_08021AA8(void *);

void sub_081AD3C0(struct A *p)
{
    u32 m;
    p->a = 0;
    m = 4;
    if ((m & gUnk_02000560) == 0)
        sub_08021AA8(p);
}
