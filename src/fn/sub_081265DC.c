#include "global.h"
extern u8 *gUnk_02000154;
s32 sub_081265DC(u8 *s)
{
    u8 *p;
    s32 i;
    u32 m;
    u32 t;
    gUnk_02000154 = s;
    m = 0xff;
    p = s + 0x88;
    i = 0x2f;
    do {
        t = *p;
        t = t | m;
        *p = t;
        p += 0x70;
        i--;
    } while (i >= 0);
    return 0;
}
