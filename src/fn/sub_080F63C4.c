#include "global.h"

extern const u32 gUnk_0860643C[];
void sub_0824923C(u8 *, u32);

void sub_080F63C4(u8 *s)
{
    s32 i;
    for (i = 0; i < s[0xd2c]; i++) {
        u8 *e = s + (i * 0x194 + 0x4bc);
        sub_0824923C(e, gUnk_0860643C[e[0x11e]]);
    }
}
