#include "global.h"

struct S {
    u8 f[0x1bc];
    u32 a;
    u8 g[0x1d0 - 0x1c0];
    u32 b;
};
void Task_Kill(struct S *);
void sub_0824923C(struct S *, u32);

s32 sub_08055D74(struct S *p)
{
    if (p->a)
        Task_Kill(p);
    else
        sub_0824923C(p, p->b);
    return 0;
}
