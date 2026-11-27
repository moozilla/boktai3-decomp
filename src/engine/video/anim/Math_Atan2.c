#include "global.h"
/* The first octant includes equal components at Q8 ratio 256. */
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
            {
                s32 r = 0x40 - gUnk_08614C44[Div(x << 8, y)];
                return r;
            }
        } else {
            s32 ny = -y;
            if (x > ny)
                return (0x100 - gUnk_08614C44[Div(-(y << 8), x)]) & 0xFF;
            return gUnk_08614C44[Div(x << 8, ny)] + 0xC0;
        }
    } else {
        if (y > 0) {
            s32 nx = -x;
            if (nx > y) {
                s32 r = 0x80 - gUnk_08614C44[Div(y << 8, nx)];
                return r;
            }
            return gUnk_08614C44[-Div(x << 8, y)] + 0x40;
        } else {
            s32 nx = -x;
            s32 ny = -y;
            if (nx > ny)
                return gUnk_08614C44[-Div(y << 8, nx)] + 0x80;
            {
                s32 r = 0xC0 - gUnk_08614C44[-Div(x << 8, ny)];
                return r;
            }
        }
    }
}
