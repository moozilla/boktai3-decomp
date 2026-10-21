#include "global.h"
// MATCHING_CONCESSION: register-pinning
/* Tentative: converts a 2D grid-style (x, y) pair to an isometric camera offset and applies it. */
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
        s32 a;
        register s32 b asm("r6");
        s32 t;
        u32 lo, hi;
        s16 *o;
        lo = x << 16;
        hi = y << 16;
        *(u32 *)&in = (lo >> 16) | hi;
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
