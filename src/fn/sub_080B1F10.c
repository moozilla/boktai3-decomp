#include "global.h"

extern const u16 gUnk_08606004[];

u16 sub_080B1F10(u8 *p)
{
    u8 *anim = *(u8 **)(p + 4) + 0x20;
    u16 r;
    if (*(u16 *)(anim + 0xc)) {
        r = 0;
    } else {
        u32 m = 0x10000;
        *(u32 *)(p + 0x2d0) |= m;
        r = gUnk_08606004[*(u16 *)(anim + 0x14)];
    }
    return r;
}
