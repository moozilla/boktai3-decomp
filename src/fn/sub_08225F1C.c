#define Camera_ApplyPositionBounds   sub_08225F1C
#define Camera_ClampCenteredPosition sub_08225E2C
#define Camera_ClampDiagonalPosition sub_08225E84
#include "global.h"
struct Camera {
    u8 unknown[0x8c];
    u16 boundsMode;
    s16 minX, minZ, maxX, maxZ;
};
void sub_08225E2C(struct Camera *, s16 *);
void sub_08225E84(struct Camera *, s16 *);
void sub_08225F1C(struct Camera *camera, s16 *position)
{
    if (camera->boundsMode == 1)
        sub_08225E2C(camera, position);
    else if (camera->boundsMode == 2)
        sub_08225E84(camera, position);
}
