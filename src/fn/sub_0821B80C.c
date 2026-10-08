#include "global.h"
struct VM { u8 p0[4]; u32 f4; u32 *f8; u32 stk[24]; u32 *sp; };
extern struct VM gUnk_02000610;
u8 *Script_DecodeOperand(u8 *, u32 *, u32 *);
s32 sub_0821B80C(u8 *p)
{
    u32 a, b;
    Script_DecodeOperand(p, &a, &b);
    gUnk_02000610.f4 = b;
    return 0;
}
