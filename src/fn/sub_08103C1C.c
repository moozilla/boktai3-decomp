#include "global.h"

extern u32 gUnk_020001AC;
s32 sub_0821ABA8(s32, s32);
u8 *sub_08101A3C(s32);

void sub_08103C1C(void)
{
    s32 r = sub_0821ABA8(0x6e, 0);
    if (gUnk_020001AC != 0 && r != 0) {
        u8 *p = sub_08101A3C(r);
        *(s32 *)(p + 0xe0) = sub_0821ABA8(0x6d, -1);
    }
}
