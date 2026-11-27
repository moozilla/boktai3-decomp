#include "scene/camera/scene_camera_controller.h"
#include "global.h"
#include "engine/video/render.h"
#include "engine/ram.h"

struct Rect {
    u16 x, y, w;
};
void Camera_ApplyPositionBounds(struct SceneCameraController *, void *);
/* Camera mode 2 with limits from two ranges {start, ?, end} (+0x8E..+0x94),
   then Camera_ApplyPositionBounds (tentative). */
void Camera_SetBounds(struct Rect *a, struct Rect *b)
{
    struct SceneCameraController *c = (void *)gActiveCamera;
    if (c) {
        c->minBounds[0] = a->w - a->x;
        c->minBounds[1] = a->w + a->x - a->y;
        c->maxBounds[0] = b->w - b->x;
        c->maxBounds[1] = b->w + b->x - b->y;
        c->boundsMode = 2;
        Camera_ApplyPositionBounds(c, &gCamera.position);
    }
}
