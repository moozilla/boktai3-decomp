#include "scene/camera/scene_camera_controller.h"
#include "global.h"
#include "engine/video/render.h"
#include "engine/ram.h"

struct Vec {
    s16 x, y, z;
};

static inline int project(int input)
{
    int v = input * 3;
    return v >= 0 ? v >> 5 : -(-v >> 5);
}

/* Camera command 1 (snap_to): move the camera to pos at once and recompute
   the scroll origin (gCameraOrigin) from its isometric projection. */
void Camera_SnapTo(struct Vec *pos)
{
    struct SceneCameraController *c = ((struct SceneCameraController *)gActiveCamera);
    if (c) {
        c->position.x = pos->x;
        c->position.y = pos->y;
        c->position.z = pos->z;
        gCamera.position.x = c->position.x;
        gCamera.position.y = c->position.y;
        gCamera.position.z = c->position.z;
        {
            struct Vec *out = &gCameraOrigin;
            struct CameraPosition *in = &gCamera.position;
            int xy, height, x = in->x, z = in->z;
            out->x = project(x - z);
            xy = project(x + z);
            height = project(in->y);
            out->y = xy - height;
            out->z = xy + height;
        }
        ((struct SceneCameraController *)gActiveCamera)->moving = 1;
        ((struct SceneCameraController *)gActiveCamera)->positionMode = 1;
    }
}
