#include "global.h"
struct FlagAC { s8 a; u8 b, c, d; };
s32 sub_0823AAAC(s32);
s32 sub_0823AB4C(u8 *, s32);
void sub_0823AC34(u8 *p)
{
    struct FlagAC *f = (struct FlagAC *)(p + 0x33c);
    s8 value = (*(u8 **)(p + 0x344))[0x3e];
    u32 zero = 0;
    f->a = value;
    if (f->a < 0) {
        f->d = zero;
        f->b = zero;
    } else {
        f->d = sub_0823AAAC(f->a);
        f->b = sub_0823AB4C(p, f->a);
    }
    f->c = zero;
}
