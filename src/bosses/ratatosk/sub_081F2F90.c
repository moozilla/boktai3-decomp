#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

struct RatatoskPoint {
    s16 x, y, z, pad;
};

s32 sub_081F2F90(struct Ratatosk *self)
{
    s32 selected = -1;
    u32 maximum = 0;
    s32 i;

    for (i = 0; i < 9; i++) {
        if (self->orbPoints[i].distance >= maximum) {
            maximum = self->orbPoints[i].distance;
            selected = i;
        }
    }
    return selected;
}

s32 sub_081F2FBC(struct Ratatosk *self)
{
    s32 selected = -1;
    u32 maximum = 0;
    s32 i;

    for (i = 0; i < 9; i++) {
        if (self->orbPoints[i].anchorDistance >= maximum) {
            maximum = self->orbPoints[i].anchorDistance;
            selected = i;
        }
    }
    return selected;
}

s32 sub_081F2FE8(struct Ratatosk *self)
{
    s32 selected = -1;
    u32 maximum = 0;
    s32 i;

    for (i = 0; i < 9; i++) {
        u32 total = self->orbPoints[i].distance + self->orbPoints[i].anchorDistance;

        if (total >= maximum) {
            maximum = total;
            selected = i;
        }
    }
    return selected;
}

struct RatatoskPoint *sub_081F3018(u8 *p, s16 *pos)
{
    s32 idx = -1;
    u32 best = 0xFFFFFFFF;
    s32 i = 0;
    s32 px = pos[0];
    s32 pz = pos[2];
    struct RatatoskPoint *q = (struct RatatoskPoint *)(p + 0xCCC);
    do {
        s32 dx = q->x - px;
        s32 dz = q->z - pz;
        u32 d = dx * dx + dz * dz;
        if (d <= best) {
            idx = i;
            best = d;
        }
        q++;
        i++;
    } while (i <= 8);
    if (idx == -1)
        return NULL;
    return (struct RatatoskPoint *)(p + 0xCCC) + idx;
}
