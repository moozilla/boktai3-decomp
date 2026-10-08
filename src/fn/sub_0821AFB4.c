#include "global.h"
struct VM { u8 p0[4]; u32 f4; };
extern struct VM gUnk_02000610;
s32 sub_0821AF2C(u32, u32, u32);
u32 sub_0821AFB4(u32 a, u32 b)
{
    if (sub_0821AF2C(a, b, 0) == 1) return gUnk_02000610.f4;
    return gUnk_02000610.f4 = 0;
}
