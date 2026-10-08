#include "global.h"
s32 Mod(s32, s32);
void sub_0811235C(u8 *s)
{
    u32 c = s[0x1208];
    if (c == 1) {
        if (Mod(s[0x1209], 0x14) == 0) {
            u32 *w = (u32 *)(s + 0x2CC);
            u32 v = *w;
            if (v & c)
                v &= ~1;
            else
                v |= c;
            *w = v;
        }
        s[0x1209]++;
    } else {
        *(u32 *)(s + 0x2CC) |= 1;
    }
}
