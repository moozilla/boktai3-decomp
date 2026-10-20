#include "global.h"

#ifdef NONMATCHING
// fuzzy: 81.4%
// Draft: builds a center/half-extent box from two corner points.
// Open: the negative arm of the halving needs its own sign-extension
// before the u16 truncation of the extent temporary.
struct HalfBox {
    u16 x, y, z;
};

struct BoxOut {
    u8 pad0[0x14];
    s16 f14, f16, f18;
    u8 pad1a[2];
    s16 ex, ey, ez;
    s16 cx, cy, cz;
};

#define HALF(dst, expr)                                                                            \
    value = (expr);                                                                                \
    half = value >= 0 ? value >> 1 : -(-value >> 1);                                               \
    dst = half

void sub_082203A8(struct HalfBox *a, struct HalfBox *b, struct BoxOut *o)
{
    s16 value;
    s16 half;
    u16 t;
    HALF(t, a->x - b->x);
    HALF(o->cx, b->x + a->x);
    o->ex = (s16)t < 0 ? -(s16)t : (s16)t;
    HALF(t, a->y - b->y);
    HALF(o->cy, a->y + b->y);
    o->ey = (s16)t < 0 ? -(s16)t : (s16)t;
    HALF(t, a->z - b->z);
    HALF(o->cz, a->z + b->z);
    o->ez = (s16)t < 0 ? -(s16)t : (s16)t;
    o->f14 = 0;
    o->f16 = 0;
    o->f18 = 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_082203A8);
#endif
