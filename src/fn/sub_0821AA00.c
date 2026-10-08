#include "global.h"
extern u8 *gUnk_02000610;
u8 *Script_DecodeOperand(u8 *, u32 *, u32 *);
u32 sub_0821AA00(u8 *p)
{
    u32 a, b;
    gUnk_02000610 = Script_DecodeOperand(p, &a, &b);
    return b;
}
