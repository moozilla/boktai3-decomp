#include "global.h"
extern u8 *gUnk_02000610;
u8 *Script_DecodeOperand(u8 *, u32 *, u32 *);
u32 sub_0821AA88(u8 *p)
{
    u32 a, b;
    if (p) {
        gUnk_02000610 = Script_DecodeOperand(p, &a, &b);
        if (gUnk_02000610) return b;
    }
    return 0;
}
