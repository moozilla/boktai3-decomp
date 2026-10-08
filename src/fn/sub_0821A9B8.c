#include "global.h"
extern u8 *gUnk_02000610;
u8 *Script_DecodeOperand(u8 *, u32 *, u32 *);
s32 sub_0821A9B8(void)
{
    u32 a, b;
    u8 *p = gUnk_02000610;
    if (p == 0) return 0;
    if (*p == 0) return 0;
    for (;;) {
        p = Script_DecodeOperand(p, &a, &b);
        if (a == 0) return 0;
        if ((a & 0xF0) != 0x50) continue;
        gUnk_02000610 = (u8 *)b;
        return (s32)a >> 16;
    }
}
