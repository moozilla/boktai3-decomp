#include "global.h"
// Tentative: Refresh the selected target; discard targets outside the search
// limits.
struct Vec {
    s16 x, y, z, pad;
};
struct Object {
    u8 pad0[9];
    u8 angle;
    u8 pada[2];
    struct Vec pos;
    u8 pad14[0x124];
    u32 range;
    u8 arc, pad13d[3];
    u32 distance;
    u8 delta;
    s8 target;
    u8 count, slot;
};
void *sub_0802C678(void);
struct Object *sub_0802CA8C(int);
int sub_082215E4(int, int);
void sub_0802CB94(struct Object *p)
{
    struct Object *t;
    if (p->target < 0)
        return;
    t = sub_0802CA8C(p->target);
    if (t) {
        struct Vec *src = &p->pos;
        struct Vec *dst = &t->pos;
        int angle = sub_082215E4(dst->x - src->x, dst->z - src->z);
        int facing = p->angle;
        int signedAngle = (s8)angle;
        u8 diff;
        int dx, dz, magnitude;
        u32 distance;
        diff = facing - signedAngle;
        dx = dst->x - src->x;
        dz = dst->z - src->z;
        distance = dx * dx + dz * dz;
        magnitude = (s8)diff;
        if (magnitude < 0)
            magnitude = -magnitude;
        if (magnitude <= p->arc && distance <= p->range) {
            p->delta = diff;
            p->distance = distance;
        } else
            p->target = -1;
    } else
        p->target = -1;
}
