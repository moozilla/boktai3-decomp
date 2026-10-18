#define Camera_ClampCenteredPosition sub_08225E2C
#include "global.h"
struct Camera {
    u8 unknown[0x8e];
    s16 minX, minZ, maxX, maxZ;
};
void sub_08225E2C(struct Camera *bounds, s16 *position)
{
    s32 half = position[1] >> 1;
    s32 x = position[0] - half;
    s32 z = position[2] - half;
    s32 limit;
    limit = bounds->minX;
    if (x < limit)
        x = limit;
    else {
        limit = bounds->maxX;
        if (x > limit)
            x = limit;
    }
    limit = bounds->minZ;
    if (z < limit)
        z = limit;
    else {
        limit = bounds->maxZ;
        if (z > limit)
            z = limit;
    }
    position[0] = x + half;
    position[2] = z + half;
}
