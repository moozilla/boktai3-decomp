#include "global.h"
struct Camera {
    u8 unknown[0x8c];
    u16 boundsMode;
    s16 minX, minZ, maxX, maxZ;
};
void Camera_ClampCenteredPosition(struct Camera *, s16 *);
void Camera_ClampDiagonalPosition(struct Camera *, s16 *);
void Camera_ApplyPositionBounds(struct Camera *camera, s16 *position)
{
    if (camera->boundsMode == 1)
        Camera_ClampCenteredPosition(camera, position);
    else if (camera->boundsMode == 2)
        Camera_ClampDiagonalPosition(camera, position);
}
