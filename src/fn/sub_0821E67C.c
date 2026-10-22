#include "global.h"

extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);

struct PathPt {
    u32 x : 16, : 16, z : 16, : 16;
};
struct PathCtx {
    u8 pad0[8];
    u16 *target;
};

static inline s32 scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

s32 sub_0821E67C(struct PathCtx *ctx, s16 *pos, s16 *out, u8 *dir, u32 rangeX, u32 rangeZ, s32 speed)
{
    struct PathPt t;
    s32 dx, dz;
    u8 angle;
    u32 a;

    t.x = ctx->target[0];
    t.z = ctx->target[1];
    dx = (s16)t.x - pos[0];
    dz = (s16)t.z - pos[2];
    angle = Math_Atan2(dx, dz);
    *dir = angle;
    a = dx;
    if (dx < 0)
        a = -dx;
    if (a <= rangeX) {
        a = dz;
        if (dz < 0)
            a = -dz;
        if (a <= rangeZ) {
            out[0] = dx;
            out[2] = dz;
            return 1;
        }
    }
    out[0] = scale(gUnk_086149C4[(angle + 0x40) & 0xff] * speed);
    out[2] = scale(gUnk_086149C4[angle] * speed);
    return 0;
}
