#include "global.h"

// Builds a center/half-extent box from two corner points.
struct HalfBox {
    u16 x, y, z;
};

struct BoxOut {
    u8 pad0[0x14];
    s16 f14, f16, f18;
    u8 pad1a[2];
    s16 ex, ey, ez;
    u8 pad22[2];
    s16 cx, cy, cz;
};

static inline s32 Half(s16 n)
{
    if (n >= 0)
        return n >> 1;
    return (s16) - ((-n) >> 1);
}
static inline s16 Abs16(s16 v)
{
    return v < 0 ? -v : v;
}
void sub_082203A8(struct HalfBox *a, struct HalfBox *b, struct BoxOut *o)
{
    u16 t;
    t = Half(a->x - b->x);
    o->cx = Half(a->x + b->x);
    o->ex = Abs16(t);
    t = Half(a->y - b->y);
    o->cy = Half(a->y + b->y);
    o->ey = Abs16(t);
    t = Half(a->z - b->z);
    o->cz = Half(a->z + b->z);
    o->ez = Abs16(t);
    o->f14 = 0;
    o->f16 = 0;
    o->f18 = 0;
}
