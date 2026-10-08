#include "global.h"
struct VM { u8 p0[4]; u32 f4; u32 *f8; u32 stk[24]; u32 *sp; };
extern struct VM gUnk_02000610;
u8 *Script_GetPc(void);
u32 Script_GetValue(void);
s32 sub_0821B82C(void)
{
    u8 *p = Script_GetPc();
    if (p)
        gUnk_02000610.f4 = Script_GetValue();
    else
        gUnk_02000610.f4 = (u32)p;
    return 1;
}
