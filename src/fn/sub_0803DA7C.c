#include "global.h"

extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
s32 Mod(s32, s32);

void sub_0803DA7C(u8 *p, s32 a, s32 lo, s32 hi)
{
    s32 n;
    s32 v;
    *(s32 *)(p + 0x174) = a;
    n = hi - lo;
    v = lo;
    if (n != 0) {
        gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
        v += Mod(*(const u16 *)((gUnk_03005308 << 1) + (u32)gUnk_0203B400), n);
    }
    *(s32 *)(p + 0x178) = v;
}
