#include "global.h"
extern u8 *gUnk_020000B0;
void sub_082151E4(u8 *, u32);
void sub_082144E4(u8 *, u8 *, u32);
u32 sub_08019AA8(u8 *p)
{
    u8 *e;
    u8 *q;
    s32 i;
    gUnk_020000B0 = p;
    e = p + 0x18;
    i = 0xf;
    do {
        q = e + 0x38;
        sub_082151E4(q, 0xda6d);
        sub_082144E4(e + 0xc, q, 0);
        i--;
        e += 0xb0;
    } while (i >= 0);
    return 0;
}
