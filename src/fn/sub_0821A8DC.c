#include "global.h"
struct VM { u8 p0[4]; u32 f4; u32 *f8; u32 stk[24]; u32 *sp; };
extern struct VM gUnk_02000610;
u32 sub_0821A8DC(s32 n)
{
    if (!n)
        return gUnk_02000610.f4;
    else {
        u32 *e = (u32 *)gUnk_02000610.sp[-1];
        return ((u32 *)e[1])[n - 1];
    }
}
