#include "global.h"

struct ContactBox {
    u8 pad0[0x1C];
    s16 ex, ey, ez; // half extents
    u8 pad22[2];
    s16 cx, cy, cz; // center
    u8 pad2a[0x18];
    u8 dir; // facing angle
};

struct ContactVec {
    s16 x, y, z;
};

// Collider contact helper: the push-out offset along the side `a` faces (+x, +z, -x, -z)
// that separates box `a` from box `b`.
void sub_0822032C(struct ContactBox *a, struct ContactBox *b, struct ContactVec *out)
{
    s32 side = ((a->dir + 0x20) & 0xFF) >> 6;

    if (side == 0) {
        out->x = a->cx + a->ex + b->ex - b->cx;
        out->y = side;
        out->z = side;
    } else if (side == 1) {
        out->x = 0;
        out->y = 0;
        out->z = a->cz + a->ez + b->ez - b->cz;
    } else if (side == 2) {
        out->x = a->cx - a->ex - b->ex - b->cx;
        out->y = 0;
        out->z = 0;
    } else {
        out->x = 0;
        out->y = 0;
        out->z = a->cz - a->ez - b->ez - b->cz;
    }
}
