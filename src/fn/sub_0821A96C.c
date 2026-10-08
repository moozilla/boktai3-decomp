#include "global.h"
struct VM { u8 *pc; u32 f4; u32 **f8; };
extern struct VM gUnk_02000610;
u8 *Script_DecodeOperand(u8 *, u32 *, u32 *);
u32 Script_SeekToKeyword(u8 k)
{
    u32 a, b;
    u8 *p = (u8 *)gUnk_02000610.f8[-1];
    for (;;) {
        p = Script_DecodeOperand(p, &a, &b);
        if (a == 0) return 0;
        if ((a & 0xF0) == 0x50 && ((s32)a >> 16) == k) {
            gUnk_02000610.pc = (u8 *)b;
            return b;
        }
    }
}
