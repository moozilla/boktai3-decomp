#include "scene/camera/scene_camera_controller.h"
#include "global.h"
#include "engine/video/render.h"
#include "engine/ram.h"

struct Vec {
    s16 x, y, z;
};

void Camera_ApplyPositionBounds(struct SceneCameraController *, s16 *);

static inline int project(int input)
{
    int v = input * 3;
    return v >= 0 ? v >> 5 : -(-v >> 5);
}

static inline void updateScroll(void)
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

/* Move the camera to pos at once (clamped), and when it follows no target
   (f1c == 0) refresh the scroll position from it plus the offset at 0x80. */
void Camera_SetPosition(u16 *pos)
{
    struct SceneCameraController *c = (void *)gActiveCamera;
    if (c) {
        c->target.x = pos[0];
        c->target.y = pos[1];
        c->target.z = pos[2];
        Camera_ApplyPositionBounds(c, &c->target.x);
        c = (void *)gActiveCamera;
        c->secondaryMode = 1;
        if (c->moving == 0) {
            gCamera.position.x = c->target.x + c->field80;
            gCamera.position.y = c->target.y;
            gCamera.position.z = c->target.z + c->field82;
            updateScroll();
        }
    }
}
