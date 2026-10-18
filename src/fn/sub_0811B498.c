#include "global.h"

struct BoundsPosition {
    s32 x, y, z;
};
struct BoundsExtent {
    s16 x, y, z;
};

s32 Bounds_TestOverlap(struct BoundsPosition *a, struct BoundsExtent *ae, struct BoundsPosition *b,
                       struct BoundsExtent *be)
{
    s32 extent = ae->x + be->x;
    if (a->x - extent > b->x || b->x - extent > a->x)
        goto separated;
    extent = ae->y + be->y;
    if (a->y - extent > b->y || b->y - extent > a->y)
        goto separated;
    extent = ae->z + be->z;
    if (a->z - extent > b->z || b->z - extent > a->z)
        goto separated;
    return 1;
separated:
    return -1;
}
