#include "global.h"
extern u32 gUnk_020000B0;
void sub_08214514(u8 *);
u32 sub_08019A78(u8 *p)
{
    u8 *e = p + 0x18;
    s32 i = 0xf;
    do {
        if (e[0x10] != 0)
            sub_08214514(e + 0xc);
        i--;
        e += 0xb0;
    } while (i >= 0);
    gUnk_020000B0 = 0;
    return 0;
}
