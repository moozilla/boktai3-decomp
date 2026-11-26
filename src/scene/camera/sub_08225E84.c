#include "scene/camera/scene_camera_controller.h"
#include "global.h"
struct Vec3s {
    s16 x, y, z;
};
void Camera_ClampDiagonalPosition(struct SceneCameraController *camera, struct Vec3s *pos)
{
    s32 d = pos->z - pos->x;
    s32 adjust;
    if (d < camera->minBounds[0]) {
        adjust = (camera->minBounds[0] - d) >> 1;
        pos->x -= adjust;
        pos->z += adjust;
    } else if (d > camera->maxBounds[0]) {
        adjust = (d - camera->maxBounds[0]) >> 1;
        pos->x += adjust;
        pos->z -= adjust;
    }
    d = pos->z + pos->x - pos->y;
    if (d < camera->minBounds[1]) {
        adjust = (camera->minBounds[1] - d) >> 1;
        pos->x += adjust;
        pos->z += adjust;
    } else if (d > camera->maxBounds[1]) {
        adjust = (d - camera->maxBounds[1]) >> 1;
        pos->x -= adjust;
        pos->z -= adjust;
    }
}
