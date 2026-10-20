#include "global.h"
#ifdef NONMATCHING
// fuzzy: 97.0% (2 bytes: final accumulate lands in r1 instead of r0 in case 4)

s32 Div(s32, s32);

s32 sub_0823D750(u8 *p)
{
    int r;
    switch (p[0xBF5]) {
        if (1) {
            case 0:
            case 1:
            case 2: {
                u32 base = (*(u16 *)(p + (r = 0x420)) + 0x63) >> 1;
                s32 v = *(u16 *)(p + 0xBF8) + *(s16 *)(p + 0xC16);
                if (v < 0)
                    v = 0;
                return base + v;
            }
        }
        return (s32)p;
        case 3:
            return ((*(u16 *)(p + 0x420) >> 1) + 0x63) >> 1;
        case 4:
            r = (*(u16 *)(p + 0xBF8) + (Div(40, 10) - 8) * 5) << 1;
            return r;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0823D750);
#endif
