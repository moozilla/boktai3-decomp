#include "global.h"

struct S { u32 a; u32 b; u32 c; s32 d; u32 e; u16 cnt; u16 sh; };

void sub_08003734(struct S *p)
{
    p->d = (p->cnt << 6) >> p->sh;
    if (p->cnt > (1 << p->sh)) {
        p->d = 0x40;
        p->a = 0;
    }
    p->cnt++;
}
