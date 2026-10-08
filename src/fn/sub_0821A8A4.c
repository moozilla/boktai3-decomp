#include "global.h"
struct VM { u8 p0[4]; u32 f4; u32 *f8; u32 stk[24]; u32 *sp; };
extern struct VM gUnk_02000610;
u32 *sub_0821A8A4(u32 a, u32 b)
{
    if (a == 0)
        return 0;
    else {
        u32 *p = gUnk_02000610.sp;
        u32 *q = p + b;
        *q++ = a;
        gUnk_02000610.sp = q;
        return p;
    }
}
