#include "global.h"
/* Tentative: converts a 2D grid-style (x, y) pair to an isometric camera offset and applies it. */
#ifdef NONMATCHING
// fuzzy: 90.6%
// differs: load order / register choice of the two halfwords of the packed pair
struct PackedXY {
    s16 x, y;
    u32 pad;
};
extern void *gUnk_03005420;
void Camera_SetTargetOffset(s16 *);

#define HALF(v) ((v) >= 0 ? (v) >> 1 : -(-(v) >> 1))

void Camera_SetIsometricOffset(u32 x, u32 y)
{
    s16 out[4];
    struct PackedXY in;
    if (gUnk_03005420 != 0) {
        s32 b, a;
        s32 t;
        s16 *o;
        *(u32 *)&in = (u16)x | (y << 16);
        o = out;
        a = in.x;
        b = in.y;
        t = ((a + b) << 5) / 3;
        o[0] = HALF(t);
        o[1] = 0;
        t = ((b - a) << 5) / 3;
        o[2] = HALF(t);
        Camera_SetTargetOffset(out);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Camera_SetIsometricOffset);
#endif
