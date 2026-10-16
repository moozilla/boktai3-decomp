#include "global.h"
#ifdef NONMATCHING
// fuzzy: 86.7%
// Open: agbcc cross-jumps the shared "subs r0, r0, r1" tails of the
// quadrant cases; the original keeps one per case.
extern const u8 gUnk_08614C44[];
s32 Div(s32, s32);
s32 Math_Atan2(s32 x, s32 y)
{
    if (x == 0) {
        if (y == 0)
            return 0;
        if (y > 0)
            return 0x40;
        return 0xC0;
    }
    if (y == 0) {
        if (x > 0)
            return 0;
        return 0x80;
    }
    if (x > 0) {
        if (y > 0) {
            if (x > y)
                return gUnk_08614C44[Div(y << 8, x)];
            return 0x40 - gUnk_08614C44[Div(x << 8, y)];
        } else {
            s32 ny = -y;
            if (x > ny)
                return (0x100 - gUnk_08614C44[Div(-(y << 8), x)]) & 0xFF;
            return gUnk_08614C44[Div(x << 8, ny)] + 0xC0;
        }
    } else {
        if (y > 0) {
            s32 nx = -x;
            if (nx > y)
                return 0x80 - gUnk_08614C44[Div(y << 8, nx)];
            return gUnk_08614C44[-Div(x << 8, y)] + 0x40;
        } else {
            s32 nx = -x;
            s32 ny = -y;
            if (nx <= ny)
                return 0xC0 - gUnk_08614C44[-Div(x << 8, ny)];
            return gUnk_08614C44[-Div(y << 8, nx)] + 0x80;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Math_Atan2);
#endif
