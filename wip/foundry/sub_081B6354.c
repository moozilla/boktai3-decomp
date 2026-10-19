#include "global.h"

extern void sub_081AFB94(void *a, u32 b, u32 c, u32 d);

void sub_081B6354(u8 *base, u32 x, u16 a2, u16 a3, u32 a4, u32 a5, u32 a6, u16 a7, u16 a8)
{
    s32 i;
    u32 v;

    x &= 0x7f;
    v = (0xa0 + x) & 0xff;
    for (i = 0; i < 2; i++) {
        u8 *e = base + i * 0x21c;
        if (i == 1) {
            *(u16 *)(base + 0x360) = v;
        } else {
            *(u16 *)(e + 0x144) = (0xa0 - x) & 0xff;
        }
        *(u16 *)(e + 0x146) = a2;
        *(u16 *)(e + 0x20A) = a4;
        sub_081AFB94(e + 0x98, a4, a5, a6);
        *(u16 *)(e + 0x220) = a7;
        *(u16 *)(e + 0x20E) = a3;
        *(u16 *)(e + 0x148) = a8;
    }
}
