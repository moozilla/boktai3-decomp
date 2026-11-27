#include "global.h"
#include "scene/collision/contact_position.h"

struct Ratatosk {
    u8 unk0[0xCCC];
    struct ContactPosition movementPoints[9];
};

struct ContactPosition *Ratatosk_FindPointFarthestFromPosition(struct Ratatosk *self,
                                                               struct ContactPosition *position)
{
    u32 maximum = 0;
    s32 selected = -1;
    s32 i = 0;
    struct ContactPosition *result;
    s32 x = position->x;
    s32 z = position->z;
    for (; i < 9; i++) {
        s32 dx = self->movementPoints[i].x - x;
        s32 dz = self->movementPoints[i].z - z;
        u32 distance = dx * dx + dz * dz;
        if (distance >= maximum) {
            selected = i;
            maximum = distance;
        }
    }
    if (selected != -1)
        result = &self->movementPoints[selected];
    else
        result = NULL;
    return result;
}
