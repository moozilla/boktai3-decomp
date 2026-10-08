#include "global.h"

extern u16 gUnk_03004CF0, gUnk_03004CFC, gUnk_03004D08, gUnk_03004CF8;

void sub_08216F78(u32 n, u32 a, u32 b, u32 c, u32 d)
{
    if (n == 0) {
        gUnk_03004CF0 = (a << 8) | c;
        *(vu16 *)0x04000040 = gUnk_03004CF0;
        gUnk_03004CFC = (b << 8) | d;
        *(vu16 *)0x04000044 = gUnk_03004CFC;
    } else {
        gUnk_03004D08 = (a << 8) | c;
        *(vu16 *)0x04000042 = gUnk_03004D08;
        gUnk_03004CF8 = (b << 8) | d;
        *(vu16 *)0x04000046 = gUnk_03004CF8;
    }
}
