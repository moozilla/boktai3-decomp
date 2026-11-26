#include "global.h"
struct CameraBounds {
    u8 pad0[0x8e];
    s16 minDiff, minSum, maxDiff, maxSum;
};
struct Vec3s {
    s16 x, y, z;
};
void Camera_ClampDiagonalPosition(struct CameraBounds *camera, struct Vec3s *pos)
{
    s32 d = pos->z - pos->x;
    s32 adjust;
    if (d < camera->minDiff) {
        adjust = (camera->minDiff - d) >> 1;
        pos->x -= adjust;
        pos->z += adjust;
    } else if (d > camera->maxDiff) {
        adjust = (d - camera->maxDiff) >> 1;
        pos->x += adjust;
        pos->z -= adjust;
    }
    d = pos->z + pos->x - pos->y;
    if (d < camera->minSum) {
        adjust = (camera->minSum - d) >> 1;
        pos->x += adjust;
        pos->z += adjust;
    } else if (d > camera->maxSum) {
        adjust = (d - camera->maxSum) >> 1;
        pos->x -= adjust;
        pos->z -= adjust;
    }
}
