#include "global.h"

void sub_0811D4AC(u8 *, s32, s32, s32, s32, s32, s32);

u32 sub_0803EBF4(u32 a, u8 *b, u8 *e)
{
    u32 f;
    u32 z;
    if (e[0x64] != 0) {
        z = 0;
        e[0x64] = z;
        *(u32 *)(e + 0x54) = 0x200;
        sub_0811D4AC(b, 0x3000, 0x2000, -0x2000, 0x5000, z, 0x6000);
    }
    f = *(u32 *)(e + 0x44);
    if (f <= 7) {
        *(u32 *)(b + 0x858) |= 0x20;
    } else if (f - 0x18 <= 0xf) {
        *(u32 *)(b + 0x858) |= 0x110;
    } else if (f > 0x27) {
        *(u32 *)(b + 0x858) |= 0x300;
    }
    (*(u32 *)(e + 0x44))++;
    return 0;
}
