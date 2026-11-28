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

struct ContactPosition *sub_081F3018(struct Ratatosk *self, struct ContactPosition *position)
{
    s32 selected = -1;
    u32 minimum = -1;
    s32 i = 0;
    s32 x = position->x;
    s32 z = position->z;

    for (; i < 9; i++) {
        s32 dx = self->points[i].x - x;
        s32 dz = self->points[i].z - z;
        u32 distance = dx * dx + dz * dz;

        if (distance <= minimum) {
            selected = i;
            minimum = distance;
        }
    }
    if (selected == -1)
        return NULL;
    return &self->points[selected];
}
