#include "global.h"
struct VM { u8 p0[4]; u32 f4; u32 *f8; u32 stk[24]; u32 *sp; };
extern struct VM gUnk_02000610;
u32 Script_GetValue(void);
u32 sub_08225884(u16);
u32 sub_08249238(void);
s32 sub_08225590(void)
{
    if (sub_08225884(Script_GetValue()) == 0)
        return -1;
    gUnk_02000610.f4 = sub_08249238();
    return 0;
}
